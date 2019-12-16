//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(disable: 4996) // turn off "deprecation" warnings
#endif

#include <d3d8.h>
#include <d3dx8math.h>
#include "gfx/D3D8/gfxD3D8Device.h"
#include "core/fileStream.h"
#include "console/console.h"
#include "gfx/primBuilder.h"
#include "platform/profiler.h"
#include "gfx/D3D8/gfxD3D8CardProfiler.h"
#include "gfx/D3D8/gfxD3D8VertexBuffer.h"
#include "gfx/D3D8/screenShotD3D8.h"
#include "gfx/D3D8/gfxD3D8EnumTranslate.h"
#include "gfx/D3D8/gfxD3D8Target.h"
#include "core/unicode.h"

#include "windowManager/platformWindowMgr.h"
#include "windowManager/win32/win32Window.h"

#include "platform/EngineConfig.h"
#ifdef SHADER_CONSTANT_INCLUDE_FILE
#include SHADER_CONSTANT_INCLUDE_FILE
#endif
// Gross hack to let the diag utility know that we only need stubs
#define DUMMYDEF
#include "gfx/D3D/DXDiagNVUtil.h"

D3D8XFNTable GFXD3D8Device::smD3DX;

void enumerateD3D8Adapters(Vector<GFXAdapter*>& adapterList)
{
   GFXD3D8Device::enumerateAdapters(adapterList);
}

void createD3D8Instance(GFXDevice** device, U32 adapterIndex)
{
   *device = GFXD3D8Device::createInstance(adapterIndex);
}

bool d3dx8BindFunction( DLibrary *dll, void *&fnAddress, const char *name )
{
   fnAddress = dll->bind( name );

   if (!fnAddress)
      Con::warnf( "D3DX Loader: DLL bind failed for %s", name );

   return fnAddress != 0;
}

void GFXD3D8Device::initD3DXFnTable()
{
   if( !smD3DX.isLoaded )
   {
#ifndef POWER_DEBUG
      const char* d3dxdllString = "d3dx8dll.dll";
#else
      const char* d3dxdllString = "d3dx8dll_DEBUG.dll";
#endif
      smD3DX.dllRef = OsLoadLibrary( d3dxdllString );
      AssertISV(smD3DX.dllRef, avar("Failed to find %s", d3dxdllString));

      smD3DX.isLoaded = true;

#define D3DX_FUNCTION(fn_name, fn_return, fn_args) \
   smD3DX.isLoaded &= d3dx8BindFunction(smD3DX.dllRef, *(void**)&smD3DX.fn_name, #fn_name);
#     include "gfx/D3D8/d3dx8Functions.h"
#undef D3DX_FUNCTION

      AssertISV( smD3DX.isLoaded, "D3DX Failed to load all functions." );
      Con::printf( "GFXD3D8Device - using '%s' for dynamic linking.", d3dxdllString );
   }
}

GFXDevice *GFXD3D8Device::createInstance( U32 adapterIndex )
{
   return new GFXD3D8Device( Direct3DCreate8( D3D_SDK_VERSION ), adapterIndex );
}

GFXD3D8Device::GFXD3D8Device( LPDIRECT3D8 d3d, U32 index ) 
{
   mDeviceSwizzle32 = &Swizzles::bgra;
   GFXVertexColor::setSwizzle( mDeviceSwizzle32 );

   mDeviceSwizzle24 = &Swizzles::bgr;

   mBackbufferFormat = GFXFormatR8G8B8;

   mD3D = d3d;
   mAdapterIndex = index;
   mD3DDevice = NULL;
   mCurrentOpenAllocVB = NULL;
   mCurrentVB = NULL;

   mCurrentOpenAllocPB = NULL;
   mCurrentPB = NULL;
   mDynamicPB = NULL;

   mCanCurrentlyRender = false;
   mTextureManager = NULL;

#ifdef POWER_DEBUG
   mVBListHead = NULL;
   mNumAllocatedVertexBuffers = 0;
#endif

   mCurrentOpenAllocVertexData = NULL;
   mPixVersion = 0.0;
   mNumSamplers = 0;

   mViewport = new D3DVIEWPORT8;
   mDepthStencil = NULL;
}

//-----------------------------------------------------------------------------

GFXD3D8Device::~GFXD3D8Device() 
{
   SAFE_DELETE( mViewport );
   SAFE_RELEASE( mDepthStencil );

   mShaderMgr.shutdown();
   
   releaseDefaultPoolResources();

   // Check up on things
   Con::printf("Cur. D3D8Device ref count=%d", mD3DDevice->AddRef() - 1);
   mD3DDevice->Release();

   // Forcibly clean up the pools
   mVolatileVBList.setSize(0);
   mDynamicPB = NULL;

   // And release our D3D resources.
   SAFE_RELEASE( mD3D );
   SAFE_RELEASE( mD3DDevice );
   
#ifdef POWER_DEBUG
   logVertexBuffers();
#endif

   if( mCardProfiler )
   {
      delete mCardProfiler;
      mCardProfiler = NULL;
   }

   if( gScreenShot )
   {
      delete gScreenShot;
      gScreenShot = NULL;
   }
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::enumerateVideoModes() 
{
   // CodeReview: Something was, apparently, not working in this function and
   // I am not sure what. [5/10/2007 Pat]
   for( U32 j = 0; j < mD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT ); j++ ) 
   {
      D3DDISPLAYMODE mode;
      mD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, j, &mode );

      GFXVideoMode toAdd;

      toAdd.bitDepth = 32; // Ugh, hardcoded is bad.
      toAdd.fullScreen = false;
      toAdd.refreshRate = mode.RefreshRate;
      toAdd.resolution = Point2I( mode.Width, mode.Height );

      mVideoModes.push_back( toAdd );
   }
}

//-----------------------------------------------------------------------------

GFXFormat GFXD3D8Device::selectSupportedFormat(GFXTextureProfile *profile,
		const Vector<GFXFormat> &formats, bool texture, bool mustblend)
{
	DWORD usage = 0;

	if(profile->isDynamic())
		usage |= D3DUSAGE_DYNAMIC;

	if(profile->isRenderTarget())
		usage |= D3DUSAGE_RENDERTARGET;

	if(profile->isZTarget())
		usage |= D3DUSAGE_DEPTHSTENCIL;

	AssertISV( !mustblend, "Can't do that postpixelshader_blending thingy in D3D8" );

	D3DDISPLAYMODE mode;
	D3D8Assert(mD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode), "Unable to get adapter mode.");

	D3DRESOURCETYPE type;
	if(texture)
		type = D3DRTYPE_TEXTURE;
	else
		type = D3DRTYPE_SURFACE;

	for(U32 i=0; i<formats.size(); i++)
	{
		if(mD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mode.Format,
			usage, type, GFXD3D8TextureFormat[formats[i]]) == D3D_OK)
			return formats[i];
	}

	return GFXFormatR8G8B8A8;
}

//-----------------------------------------------------------------------------
void GFXD3D8Device::initStates() 
{
      //-------------------------------------
   // Auto-generated default states, see regenStates() for details
   //

   // Render states
   initRenderState( 0, 1 );
   initRenderState( 1, 3 );
   initRenderState( 2, 2 );
   initRenderState( 3, 1 );
   initRenderState( 4, 0 );
   initRenderState( 5, 1 );
   initRenderState( 6, 1 );
   initRenderState( 7, 0 );
   initRenderState( 8, 2 );
   initRenderState( 9, 3 );
   initRenderState( 10, 0 );
   initRenderState( 11, 7 );
   initRenderState( 12, 0 );
   initRenderState( 13, 0 );
   initRenderState( 14, 0 );
   initRenderState( 15, 0 );
   initRenderState( 16, 0 );
   initRenderState( 17, 0 );
   initRenderState( 18, 0 );
   initRenderState( 19, 1065353216 );
   initRenderState( 20, 1065353216 );
   initRenderState( 21, 0 );
   initRenderState( 22, 0 );
   initRenderState( 23, 0 );
   initRenderState( 24, 0 );
   initRenderState( 25, 0 );
   initRenderState( 26, 7 );
   initRenderState( 27, 0 );
   initRenderState( 28, -1 );
   initRenderState( 29, -1 );
   initRenderState( 30, -1 );
   initRenderState( 31, 0 );
   initRenderState( 32, 0 );
   initRenderState( 33, 0 );
   initRenderState( 34, 0 );
   initRenderState( 35, 0 );
   initRenderState( 36, 0 );
   initRenderState( 37, 0 );
   initRenderState( 38, 0 );
   initRenderState( 39, 1 );
   initRenderState( 40, 1 );
   initRenderState( 41, 0 );
   initRenderState( 42, 0 );
   initRenderState( 43, 1 );
   initRenderState( 44, 1 );
   initRenderState( 45, 0 );
   initRenderState( 46, 1 );
   initRenderState( 47, 2 );
   initRenderState( 48, 0 );
   initRenderState( 49, 0 );
   initRenderState( 50, 0 );
   initRenderState( 51, 0 );
   initRenderState( 52, 1065353216 );
   initRenderState( 53, 0 );
   initRenderState( 54, 0 );
   initRenderState( 55, 0 );
   initRenderState( 56, 1065353216 );
   initRenderState( 57, 0 );
   initRenderState( 58, 0 );
   initRenderState( 59, 1 );
   initRenderState( 60, -1 );
   initRenderState( 61, 0 );
   initRenderState( 62, -1163015426 );
   initRenderState( 63, 1132462080 );
   initRenderState( 64, 0 );
   initRenderState( 65, 15 );
   initRenderState( 66, 0 );
   initRenderState( 67, 0 );

   // Texture Stage states
   initTextureState( 0, 0, 3 );
   initTextureState( 0, 1, 2 );
   initTextureState( 0, 2, 1 );
   initTextureState( 0, 3, 1 );
   initTextureState( 0, 4, 2 );
   initTextureState( 0, 5, 1 );
   initTextureState( 0, 6, 0 );
   initTextureState( 0, 7, 0 );
   initTextureState( 0, 8, 0 );
   initTextureState( 0, 9, 0 );
   initTextureState( 0, 10, 0 );
   initTextureState( 0, 11, 0 );
   initTextureState( 0, 12, 0 );
   initTextureState( 0, 13, 0 );
   initTextureState( 0, 14, 1 );
   initTextureState( 0, 15, 1 );
   initTextureState( 0, 16, 1 );
   initTextureState( 1, 0, 0 );
   initTextureState( 1, 1, 2 );
   initTextureState( 1, 2, 1 );
   initTextureState( 1, 3, 0 );
   initTextureState( 1, 4, 2 );
   initTextureState( 1, 5, 1 );
   initTextureState( 1, 6, 0 );
   initTextureState( 1, 7, 0 );
   initTextureState( 1, 8, 0 );
   initTextureState( 1, 9, 0 );
   initTextureState( 1, 10, 1 );
   initTextureState( 1, 11, 0 );
   initTextureState( 1, 12, 0 );
   initTextureState( 1, 13, 0 );
   initTextureState( 1, 14, 1 );
   initTextureState( 1, 15, 1 );
   initTextureState( 1, 16, 1 );
   initTextureState( 2, 0, 0 );
   initTextureState( 2, 1, 2 );
   initTextureState( 2, 2, 1 );
   initTextureState( 2, 3, 0 );
   initTextureState( 2, 4, 2 );
   initTextureState( 2, 5, 1 );
   initTextureState( 2, 6, 0 );
   initTextureState( 2, 7, 0 );
   initTextureState( 2, 8, 0 );
   initTextureState( 2, 9, 0 );
   initTextureState( 2, 10, 2 );
   initTextureState( 2, 11, 0 );
   initTextureState( 2, 12, 0 );
   initTextureState( 2, 13, 0 );
   initTextureState( 2, 14, 1 );
   initTextureState( 2, 15, 1 );
   initTextureState( 2, 16, 1 );
   initTextureState( 3, 0, 0 );
   initTextureState( 3, 1, 2 );
   initTextureState( 3, 2, 1 );
   initTextureState( 3, 3, 0 );
   initTextureState( 3, 4, 2 );
   initTextureState( 3, 5, 1 );
   initTextureState( 3, 6, 0 );
   initTextureState( 3, 7, 0 );
   initTextureState( 3, 8, 0 );
   initTextureState( 3, 9, 0 );
   initTextureState( 3, 10, 3 );
   initTextureState( 3, 11, 0 );
   initTextureState( 3, 12, 0 );
   initTextureState( 3, 13, 0 );
   initTextureState( 3, 14, 1 );
   initTextureState( 3, 15, 1 );
   initTextureState( 3, 16, 1 );
   initTextureState( 4, 0, 0 );
   initTextureState( 4, 1, 2 );
   initTextureState( 4, 2, 1 );
   initTextureState( 4, 3, 0 );
   initTextureState( 4, 4, 2 );
   initTextureState( 4, 5, 1 );
   initTextureState( 4, 6, 0 );
   initTextureState( 4, 7, 0 );
   initTextureState( 4, 8, 0 );
   initTextureState( 4, 9, 0 );
   initTextureState( 4, 10, 4 );
   initTextureState( 4, 11, 0 );
   initTextureState( 4, 12, 0 );
   initTextureState( 4, 13, 0 );
   initTextureState( 4, 14, 1 );
   initTextureState( 4, 15, 1 );
   initTextureState( 4, 16, 1 );
   initTextureState( 5, 0, 0 );
   initTextureState( 5, 1, 2 );
   initTextureState( 5, 2, 1 );
   initTextureState( 5, 3, 0 );
   initTextureState( 5, 4, 2 );
   initTextureState( 5, 5, 1 );
   initTextureState( 5, 6, 0 );
   initTextureState( 5, 7, 0 );
   initTextureState( 5, 8, 0 );
   initTextureState( 5, 9, 0 );
   initTextureState( 5, 10, 5 );
   initTextureState( 5, 11, 0 );
   initTextureState( 5, 12, 0 );
   initTextureState( 5, 13, 0 );
   initTextureState( 5, 14, 1 );
   initTextureState( 5, 15, 1 );
   initTextureState( 5, 16, 1 );
   initTextureState( 6, 0, 0 );
   initTextureState( 6, 1, 2 );
   initTextureState( 6, 2, 1 );
   initTextureState( 6, 3, 0 );
   initTextureState( 6, 4, 2 );
   initTextureState( 6, 5, 1 );
   initTextureState( 6, 6, 0 );
   initTextureState( 6, 7, 0 );
   initTextureState( 6, 8, 0 );
   initTextureState( 6, 9, 0 );
   initTextureState( 6, 10, 6 );
   initTextureState( 6, 11, 0 );
   initTextureState( 6, 12, 0 );
   initTextureState( 6, 13, 0 );
   initTextureState( 6, 14, 1 );
   initTextureState( 6, 15, 1 );
   initTextureState( 6, 16, 1 );
   initTextureState( 7, 0, 0 );
   initTextureState( 7, 1, 2 );
   initTextureState( 7, 2, 1 );
   initTextureState( 7, 3, 0 );
   initTextureState( 7, 4, 2 );
   initTextureState( 7, 5, 1 );
   initTextureState( 7, 6, 0 );
   initTextureState( 7, 7, 0 );
   initTextureState( 7, 8, 0 );
   initTextureState( 7, 9, 0 );
   initTextureState( 7, 10, 7 );
   initTextureState( 7, 11, 0 );
   initTextureState( 7, 12, 0 );
   initTextureState( 7, 13, 0 );
   initTextureState( 7, 14, 1 );
   initTextureState( 7, 15, 1 );
   initTextureState( 7, 16, 1 );

   // Sampler states
   initSamplerState( 0, 0, 0 );
   initSamplerState( 0, 1, 0 );
   initSamplerState( 0, 2, 0 );
   initSamplerState( 0, 3, 0 );
   initSamplerState( 0, 4, 1 );
   initSamplerState( 0, 5, 1 );
   initSamplerState( 0, 6, 0 );
   initSamplerState( 0, 7, 0 );
   initSamplerState( 0, 8, 0 );
   initSamplerState( 0, 9, 1 );
   initSamplerState( 1, 0, 0 );
   initSamplerState( 1, 1, 0 );
   initSamplerState( 1, 2, 0 );
   initSamplerState( 1, 3, 0 );
   initSamplerState( 1, 4, 1 );
   initSamplerState( 1, 5, 1 );
   initSamplerState( 1, 6, 0 );
   initSamplerState( 1, 7, 0 );
   initSamplerState( 1, 8, 0 );
   initSamplerState( 1, 9, 1 );
   initSamplerState( 2, 0, 0 );
   initSamplerState( 2, 1, 0 );
   initSamplerState( 2, 2, 0 );
   initSamplerState( 2, 3, 0 );
   initSamplerState( 2, 4, 1 );
   initSamplerState( 2, 5, 1 );
   initSamplerState( 2, 6, 0 );
   initSamplerState( 2, 7, 0 );
   initSamplerState( 2, 8, 0 );
   initSamplerState( 2, 9, 1 );
   initSamplerState( 3, 0, 0 );
   initSamplerState( 3, 1, 0 );
   initSamplerState( 3, 2, 0 );
   initSamplerState( 3, 3, 0 );
   initSamplerState( 3, 4, 1 );
   initSamplerState( 3, 5, 1 );
   initSamplerState( 3, 6, 0 );
   initSamplerState( 3, 7, 0 );
   initSamplerState( 3, 8, 0 );
   initSamplerState( 3, 9, 1 );
   initSamplerState( 4, 0, 0 );
   initSamplerState( 4, 1, 0 );
   initSamplerState( 4, 2, 0 );
   initSamplerState( 4, 3, 0 );
   initSamplerState( 4, 4, 1 );
   initSamplerState( 4, 5, 1 );
   initSamplerState( 4, 6, 0 );
   initSamplerState( 4, 7, 0 );
   initSamplerState( 4, 8, 0 );
   initSamplerState( 4, 9, 1 );
   initSamplerState( 5, 0, 0 );
   initSamplerState( 5, 1, 0 );
   initSamplerState( 5, 2, 0 );
   initSamplerState( 5, 3, 0 );
   initSamplerState( 5, 4, 1 );
   initSamplerState( 5, 5, 1 );
   initSamplerState( 5, 6, 0 );
   initSamplerState( 5, 7, 0 );
   initSamplerState( 5, 8, 0 );
   initSamplerState( 5, 9, 1 );
   initSamplerState( 6, 0, 0 );
   initSamplerState( 6, 1, 0 );
   initSamplerState( 6, 2, 0 );
   initSamplerState( 6, 3, 0 );
   initSamplerState( 6, 4, 1 );
   initSamplerState( 6, 5, 1 );
   initSamplerState( 6, 6, 0 );
   initSamplerState( 6, 7, 0 );
   initSamplerState( 6, 8, 0 );
   initSamplerState( 6, 9, 1 );
   initSamplerState( 7, 0, 0 );
   initSamplerState( 7, 1, 0 );
   initSamplerState( 7, 2, 0 );
   initSamplerState( 7, 3, 0 );
   initSamplerState( 7, 4, 1 );
   initSamplerState( 7, 5, 1 );
   initSamplerState( 7, 6, 0 );
   initSamplerState( 7, 7, 0 );
   initSamplerState( 7, 8, 0 );
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setMatrix( GFXMatrixType mtype, const MatrixF &mat ) 
{
   mat.transposeTo( mTempMatrix );

   mD3DDevice->SetTransform( (_D3DTRANSFORMSTATETYPE)mtype, (D3DMATRIX *)&mTempMatrix );
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setSamplerState( U32 stage, U32 type, U32 value ) 
{
   // D3D8 only supports 8 textures if a shader isn't bound
   if(stage > 7)
      return;

   switch( type )
   {
      case GFXSAMPMagFilter:
      case GFXSAMPMinFilter:
      case GFXSAMPMipFilter:
 
         mD3DDevice->SetTextureStageState( stage, GFXD3D8SamplerState[type], GFXD3D8TextureFilter[value] );
         break;

      case GFXSAMPAddressU:
      case GFXSAMPAddressV:
      case GFXSAMPAddressW:
         mD3DDevice->SetTextureStageState( stage, GFXD3D8SamplerState[type], GFXD3D8TextureAddress[value] );
         break;

      default:
         mD3DDevice->SetTextureStageState( stage, GFXD3D8SamplerState[type], value );
         break;
   }
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setTextureStageState( U32 stage, U32 state, U32 value ) 
{
   switch( state )
   {
      case GFXTSSColorOp:
      case GFXTSSAlphaOp:
         mD3DDevice->SetTextureStageState( stage, GFXD3D8TextureStageState[state], GFXD3D8TextureOp[value] );
         break;

      default:
         mD3DDevice->SetTextureStageState( stage, GFXD3D8TextureStageState[state], value );
         break;
   }
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setRenderState( U32 state, U32 value ) 
{
   switch( state )
   {
      case GFXRSSrcBlend:
      case GFXRSDestBlend:
      case GFXRSSrcBlendAlpha:
      case GFXRSDestBlendAlpha:
         mD3DDevice->SetRenderState( GFXD3D8RenderState[state], GFXD3D8Blend[value] );
         break;

      case GFXRSBlendOp:
      case GFXRSBlendOpAlpha:
         mD3DDevice->SetRenderState( GFXD3D8RenderState[state], GFXD3D8BlendOp[value] );
         break;

      case GFXRSStencilFail:
      case GFXRSStencilZFail:
      case GFXRSStencilPass:
      case GFXRSCCWStencilFail:
      case GFXRSCCWStencilZFail:
      case GFXRSCCWStencilPass:
         mD3DDevice->SetRenderState( GFXD3D8RenderState[state], GFXD3D8StencilOp[value] );
         break;

      case GFXRSZFunc:
      case GFXRSAlphaFunc:
      case GFXRSStencilFunc:
      case GFXRSCCWStencilFunc:
         mD3DDevice->SetRenderState( GFXD3D8RenderState[state], GFXD3D8CmpFunc[value] );
         break;

      case GFXRSCullMode:
         mD3DDevice->SetRenderState( GFXD3D8RenderState[state], GFXD3D8CullMode[value] );
         break;

      default:
         mD3DDevice->SetRenderState( GFXD3D8RenderState[state], value );
         break;
   }
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::clear( U32 flags, ColorI color, F32 z, U32 stencil ) 
{
   // Kind of a bummer we have to do this, there should be a better way made
   DWORD realflags = 0;

   if( flags & GFXClearTarget )
      realflags |= D3DCLEAR_TARGET;

   if( flags & GFXClearZBuffer )
      realflags |= D3DCLEAR_ZBUFFER;

   if( flags & GFXClearStencil )
      realflags |= D3DCLEAR_STENCIL;

   mD3DDevice->Clear( 0, NULL, realflags, D3DCOLOR_ARGB( color.alpha, color.red, color.green, color.blue ), z, stencil );
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::beginSceneInternal() 
{
   mD3DDevice->BeginScene();
   mCanCurrentlyRender = true;
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::endSceneInternal() 
{
   mD3DDevice->EndScene();
   mCanCurrentlyRender = false;
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setViewport( const RectI &rect ) 
{
   // Validate the rect against maximum possible size
#ifdef POWER_DEBUG
   AssertFatal(mCurrentRT.isValid(), "GFXGLDevice::setClipRect - must have a render target set to do any rendering operations!");
	Point2I size = mCurrentRT->getSize();
   AssertFatal(rect.point.x + rect.extent.x <= size.x, "GFXD3D8Device::setViewport - can't be outside the render target's bounds! (x)");
   AssertFatal(rect.point.y + rect.extent.y <= size.y, "GFXD3D8Device::setViewport - can't be outside the render target's bounds! (y)");
#endif

   mViewportRect = rect;

   mViewport->X       = mViewportRect.point.x;
   mViewport->Y       = mViewportRect.point.y;
   mViewport->Width   = mViewportRect.extent.x;
   mViewport->Height  = mViewportRect.extent.y;
   mViewport->MinZ    = 0.0;
   mViewport->MaxZ    = 1.0;

   D3D8Assert( mD3DDevice->SetViewport( mViewport ), "Error setting viewport" );
}

//-----------------------------------------------------------------------------
#ifdef POWER_DEBUG

void GFXD3D8Device::logVertexBuffers() 
{

   // NOTE: This function should be called on the destructor of this class and ONLY then
   // otherwise it'll produce the wrong output
   if( mNumAllocatedVertexBuffers == 0 )
      return;

   FileStream fs;

   fs.open( "vertexbuffer.log", FileStream::Write );

   char buff[256];

   fs.writeLine( (U8 *)avar("-- Vertex buffer memory leak report -- time = %d", Platform::getRealMilliseconds()) );
   dSprintf( (char *)&buff, sizeof( buff ), "%d un-freed vertex buffers", mNumAllocatedVertexBuffers );
   fs.writeLine( (U8 *)buff );

   GFXD3D8VertexBuffer *walk = mVBListHead;

   while( walk != NULL ) 
   {
      dSprintf( (char *)&buff, sizeof( buff ), "[Name: %s] Size: %d", walk->name, walk->mNumVerts );
      fs.writeLine( (U8 *)buff );

      walk = walk->next;
   }

   fs.writeLine( (U8 *)"-- End report --" );

   fs.close();
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::addVertexBuffer( GFXD3D8VertexBuffer *buffer ) 
{
   mNumAllocatedVertexBuffers++;
   
   if( mVBListHead == NULL ) 
   {
      mVBListHead = buffer;
   }
   else 
   {
      GFXD3D8VertexBuffer *walk = mVBListHead;

      while( walk->next != NULL ) 
      {
         walk = walk->next;
      }

      walk->next = buffer;
   }

   buffer->next = NULL;
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::removeVertexBuffer( GFXD3D8VertexBuffer *buffer ) 
{
   mNumAllocatedVertexBuffers--;

   // Quick check to see if this is head of list
   if( mVBListHead == buffer ) 
   {
      mVBListHead = mVBListHead->next;
      return;
   }

   GFXD3D8VertexBuffer *walk = mVBListHead;

   while( walk->next != NULL ) 
   {
      if( walk->next == buffer ) 
      {
         walk->next = walk->next->next;
         return;
      }

      walk = walk->next;
   }

   AssertFatal( false, "Vertex buffer not found in list." );
}

#endif

//-----------------------------------------------------------------------------

void GFXD3D8Device::releaseDefaultPoolResources() 
{
   // Release all the dynamic vertex buffer arrays
   // Forcibly clean up the pools
   for( U32 i=0; i<mVolatileVBList.size(); i++ )
   {
      // Con::printf("Trying to release vb with COM refcount of %d and internal refcount of %d", mVolatileVBList[i]->vb->AddRef() - 1, mVolatileVBList[i]->mRefCount);  
      // mVolatileVBList[i]->vb->Release();

      mVolatileVBList[i]->vb->Release();
      mVolatileVBList[i]->vb = NULL;
      mVolatileVBList[i] = NULL;
   }
   mVolatileVBList.setSize(0);

   // Set current VB to NULL and set state dirty
   mCurrentVertexBuffer = NULL;
   mVertexBufferDirty = true;

   // Release dynamic index buffer
   if( mDynamicPB != NULL )
   {
      SAFE_RELEASE( mDynamicPB->ib );
   }

   // Set current PB/IB to NULL and set state dirty
   mCurrentPrimitiveBuffer = NULL;
   mCurrentPB = NULL;
   mPrimitiveBufferDirty   = true;

   // Zombify texture manager (for D3D this only modifies default pool textures)
   if( mTextureManager ) 
      mTextureManager->zombify();

   // Kill off other potentially dangling references...
   SAFE_RELEASE( mDepthStencil );
   mD3DDevice->SetRenderTarget(NULL, NULL);

   GFXResource* walk = mResourceListHead;
   while(walk)
   {
      walk->zombify();
      walk = walk->getNextResource();
   }

   // Set global dirty state so the IB/PB and VB get reset
   mStateDirty = true;
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::reacquireDefaultPoolResources() 
{

   // Now do the dynamic index buffers
   if( mDynamicPB == NULL ) 
   {
      mDynamicPB = new GFXD3D8PrimitiveBuffer(this, 0, 0, GFXBufferTypeDynamic);
   }

   D3D8Assert( mD3DDevice->CreateIndexBuffer( sizeof( U16 ) * MAX_DYNAMIC_INDICES, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
      GFXD3D8IndexFormat[GFXIndexFormat16], D3DPOOL_DEFAULT, &mDynamicPB->ib ), "Failed to allocate dynamic IB" );

   // Grab the depth-stencil...
   SAFE_RELEASE(mDepthStencil);
   D3D8Assert(mD3DDevice->GetDepthStencilSurface(&mDepthStencil), 
      "GFXD3D8Device::reacquireDefaultPoolResources - couldn't grab reference to device's depth-stencil surface.");  
   // Ressurect texture manager (for D3D this only modifies default pool textures)
   mTextureManager->resurrect();

   GFXResource* walk = mResourceListHead;
   while(walk)
   {
      walk->resurrect();
      walk = walk->getNextResource();
   }
}

//-----------------------------------------------------------------------------
GFXD3D8VertexBuffer * GFXD3D8Device::findVBPool( U32 vertFlags )
{
   for( U32 i=0; i<mVolatileVBList.size(); i++ )
   {
      if( mVolatileVBList[i]->mVertexType == vertFlags )
      {
         return mVolatileVBList[i];
      }
   }

   return NULL;
}

//-----------------------------------------------------------------------------
GFXD3D8VertexBuffer * GFXD3D8Device::createVBPool( U32 vertFlags, U32 vertSize )
{
   // this is a bit funky, but it will avoid problems with (lack of) copy constructors
   //    with a push_back() situation
   mVolatileVBList.increment();
   RefPtr<GFXD3D8VertexBuffer> newBuff;
   mVolatileVBList.last() = new GFXD3D8VertexBuffer();
   newBuff = mVolatileVBList.last();

   newBuff->mNumVerts   = 0;
   newBuff->mBufferType = GFXBufferTypeVolatile;
   newBuff->mVertexType = vertFlags;
   newBuff->mVertexSize = vertSize;
   newBuff->mDevice = this;

//   Con::printf("Created buff with type %x", vertFlags);

   D3D8Assert( mD3DDevice->CreateVertexBuffer( vertSize * MAX_DYNAMIC_VERTS, 
                                              D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                              vertFlags, 
                                              D3DPOOL_DEFAULT, 
                                              &newBuff->vb ), 
                                              "Failed to allocate dynamic VB" );
   return newBuff;
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setClipRectOrtho( const RectI &inRect, const RectI &orthoRect )
{
   AssertFatal(mCurrentRT.isValid(), "GFXGLDevice::setClipRect - must have a render target set to do any rendering operations!");

   // Clip the rect against the renderable size.
   Point2I size = mCurrentRT->getSize();
   RectI maxRect(Point2I(0,0), size);
   RectI rect = inRect;
   rect.intersect(maxRect);

   mClipRect = rect;

   F32 left   = F32( orthoRect.point.x );
   F32 right  = F32( orthoRect.point.x + orthoRect.extent.x );
   F32 bottom = F32( orthoRect.point.y + orthoRect.extent.y );
   F32 top    = F32( orthoRect.point.y );

   // Set up projection matrix
   GFXD3D8X.GFXD3D8XMatrixOrthoOffCenterLH( (D3DXMATRIX *)&mTempMatrix, left, right, bottom, top, 0.f, 1.f );
   mTempMatrix.transpose();

   setProjectionMatrix( mTempMatrix );

   // Set up world/view matrix
   mTempMatrix.identity();
   setViewMatrix( mTempMatrix );
   setWorldMatrix( mTempMatrix );

   setViewport( mClipRect );
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setClipRect( const RectI &inRect )
{
   AssertFatal(mCurrentRT.isValid(), "GFXGLDevice::setClipRect - must have a render target set to do any rendering operations!");

   // Clip the rect against the renderable size.
   Point2I size = mCurrentRT->getSize();
   RectI maxRect(Point2I(0,0), size);
   RectI rect = inRect;
   rect.intersect(maxRect);

   setClipRectOrtho(rect, rect);
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setVB( GFXVertexBuffer *buffer ) 
{
   AssertFatal( mCurrentOpenAllocVB == NULL, "Calling setVertexBuffer() when a vertex buffer is still open for editing" );

   mCurrentVB = static_cast<GFXD3D8VertexBuffer *>( buffer );

   D3D8Assert( mD3DDevice->SetStreamSource( 0, mCurrentVB->vb, mCurrentVB->mVertexSize ), "Failed to set stream source" );
   D3D8Assert( mD3DDevice->SetVertexShader( mCurrentVB->mVertexType ), "Failed to set FVF" );
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::setPrimitiveBuffer( GFXPrimitiveBuffer *buffer ) 
{
   AssertFatal( mCurrentOpenAllocPB == NULL, "Calling setIndexBuffer() when a index buffer is still open for editing" );

   mCurrentPB = static_cast<GFXD3D8PrimitiveBuffer *>( buffer );

   D3D8Assert( mD3DDevice->SetIndices( mCurrentPB->ib, mCurrentVB->mVolatileStart ), "Failed to set indices" );
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::drawPrimitive( GFXPrimitiveType primType, U32 vertexStart, U32 primitiveCount ) 
{
   // This is done to avoid the function call overhead if possible
   if( mStateDirty )
      updateStates();

   AssertFatal( mCurrentOpenAllocVB == NULL, "Calling drawPrimitive() when a vertex buffer is still open for editing" );
   AssertFatal( mCurrentVB != NULL, "Trying to call draw primitive with no current vertex buffer, call setVertexBuffer()" );

   D3D8Assert( mD3DDevice->DrawPrimitive( GFXD3D8PrimType[primType], mCurrentVB->mVolatileStart + vertexStart, primitiveCount ), "Failed to draw primitives" );  
}

//-----------------------------------------------------------------------------

void GFXD3D8Device::drawIndexedPrimitive( GFXPrimitiveType primType, U32 minIndex, U32 numVerts, U32 startIndex, U32 primitiveCount ) 
{
   // This is done to avoid the function call overhead if possible
   if( mStateDirty )
      updateStates();

   AssertFatal( mCurrentOpenAllocVB == NULL, "Calling drawIndexedPrimitive() when a vertex buffer is still open for editing" );
   AssertFatal( mCurrentVB != NULL, "Trying to call drawIndexedPrimitive with no current vertex buffer, call setVertexBuffer()" );

   AssertFatal( mCurrentOpenAllocPB == NULL, "Calling drawIndexedPrimitive() when a index buffer is still open for editing" );
   AssertFatal( mCurrentPB != NULL, "Trying to call drawIndexedPrimitive with no current index buffer, call setIndexBuffer()" );

   // This seems stupid, however I assure you it isn't. The reason is that, if 
   // something changes the dynamic Primitive Buffer, than the state manager
   // will not register this as a change. Because DirectX 8 requires the vertex
   // buffer offset as a parameter of the call to SetIndices, it is an unfortunate
   // side effect that we need to call SetIndices every DIP call just to be safe.
   // I am not sure if putting in logic to test for a change in "mVolatileStart"
   // into updateStates() is a good idea or not. -patw
   if( mCurrentVB->mBufferType == GFXBufferTypeVolatile )
      D3D8Assert( mD3DDevice->SetIndices( mCurrentPB->ib, mCurrentVB->mVolatileStart ), "Failed to set indices" );

   D3D8Assert( mD3DDevice->DrawIndexedPrimitive( GFXD3D8PrimType[primType], minIndex, numVerts, mCurrentPB->mVolatileStart + startIndex, primitiveCount ), "Failed to draw indexed primitive" );
}

//-----------------------------------------------------------------------------
// Create shader - for D3D.  Returns NULL if cannot create.
//-----------------------------------------------------------------------------
GFXShader * GFXD3D8Device::createShader( const char *vertFile, const char *pixFile, F32 pixVersion)
{
   return (GFXShader*) mShaderMgr.createShader( vertFile, pixFile, pixVersion );
}

//-----------------------------------------------------------------------------
GFXShader * GFXD3D8Device::createShader( GFXShaderFeatureData &featureData,
                                        GFXVertexFlags vertFlags )
{
   return mShaderMgr.getShader( featureData, vertFlags );
}

//-----------------------------------------------------------------------------
void GFXD3D8Device::flushProceduralShaders()
{
   mShaderMgr.flushProceduralShaders();
}

//-----------------------------------------------------------------------------
// Disable shaders
//-----------------------------------------------------------------------------
void GFXD3D8Device::disableShaders()
{
//   mD3DDevice->SetVertexShader( NULL );
//   mD3DDevice->SetPixelShader( NULL );
   setShader( NULL );
}

//-----------------------------------------------------------------------------
// Set shader - this function exists to make sure this is done in one place,
//              and to make sure redundant shader states are not being
//              sent to the card.
//-----------------------------------------------------------------------------
void GFXD3D8Device::setShader( GFXShader *shader )
{
   return;
   //GFXD3DShader *d3dShader = static_cast<GFXD3DShader*>( shader );

   //IDirect3DPixelShader9 *pixShader = d3dShader ? d3dShader->pixShader : NULL;
   //IDirect3DVertexShader9 *vertShader = d3dShader ? d3dShader->vertShader : NULL;

   //if( pixShader != lastPixShader )
   //{
   //   mD3DDevice->SetPixelShader( pixShader );
   //   lastPixShader = pixShader;
   //}

   //if( vertShader != lastVertShader )
   //{
   //   mD3DDevice->SetVertexShader( vertShader );
   //   lastVertShader = vertShader;
   //}

}

//-----------------------------------------------------------------------------
// Set vertex shader constant
//-----------------------------------------------------------------------------
void GFXD3D8Device::setVertexShaderConstF( U32 reg, const float *data, U32 size, U32 realCntFloat )
{
   mD3DDevice->SetVertexShaderConstant( reg, data, size );
}

//-----------------------------------------------------------------------------
// Set pixel shader constant
//-----------------------------------------------------------------------------
void GFXD3D8Device::setPixelShaderConstF( U32 reg, const float *data, U32 size, U32 realCntFloat )
{
   mD3DDevice->SetPixelShaderConstant( reg, data, size );
}

//-----------------------------------------------------------------------------
// allocPrimitiveBuffer
//-----------------------------------------------------------------------------
GFXPrimitiveBuffer * GFXD3D8Device::allocPrimitiveBuffer(U32 numIndices, U32 numPrimitives, GFXBufferType bufferType)
{
   // Allocate a buffer to return
   GFXD3D8PrimitiveBuffer * res = new GFXD3D8PrimitiveBuffer(this, numIndices, numPrimitives, bufferType);

   // Determine usage flags
   U32 usage = 0;
   D3DPOOL pool = D3DPOOL_DEFAULT;

   // Assumptions:
   //    - static buffers are write once, use many
   //    - dynamic buffers are write many, use many
   //    - volatile buffers are write once, use once
   // You may never read from a buffer.
   switch(bufferType)
   {
   case GFXBufferTypeStatic:
      res->registerResourceWithDevice(this);
      pool = D3DPOOL_MANAGED;
      break;

   case GFXBufferTypeDynamic:
      AssertISV(false, "D3D does not support dynamic primitive buffers. -- BJG");
      res->registerResourceWithDevice(this);
      usage |= D3DUSAGE_DYNAMIC;
      break;

   case GFXBufferTypeVolatile:
      res->registerResourceWithDevice(this);
      pool = D3DPOOL_DEFAULT;
      break;
   }

   // We never allow reading from a vert buffer.
   usage |= D3DUSAGE_WRITEONLY;

   // Create d3d index buffer
   if(bufferType == GFXBufferTypeVolatile)
   {
      // Get it from the pool if it's a volatile...
      AssertFatal( numIndices < MAX_DYNAMIC_INDICES, "Cannot allocate that many indices in a volatile buffer, increase MAX_DYNAMIC_INDICES." );

      res->ib              = mDynamicPB->ib;
      // mDynamicPB->ib->AddRef();
      res->mVolatileBuffer = mDynamicPB;
   }
   else
   {
      // Otherwise, get it as a seperate buffer...
      D3D8Assert(mD3DDevice->CreateIndexBuffer( sizeof(U16) * numIndices , usage, GFXD3D8IndexFormat[GFXIndexFormat16], pool, &res->ib ),
         "Failed to allocate an index buffer.");
   }

   // Return buffer
   return res;
}

//-----------------------------------------------------------------------------
// allocVertexBuffer
//-----------------------------------------------------------------------------
GFXVertexBuffer * GFXD3D8Device::allocVertexBuffer( U32 numVerts, U32 vertFlags, U32 vertSize, GFXBufferType bufferType,U8 streamNum )
{
   GFXD3D8VertexBuffer * res = new GFXD3D8VertexBuffer(this, numVerts, vertFlags, vertSize, bufferType);

   // Determine usage flags
   U32 usage = 0;
   D3DPOOL pool = D3DPOOL_DEFAULT;

   res->mNumVerts = 0;

   // Assumptions:
   //    - static buffers are write once, use many
   //    - dynamic buffers are write many, use many
   //    - volatile buffers are write once, use once
   // You may never read from a buffer.
   switch(bufferType)
   {
      case GFXBufferTypeStatic:
         res->registerResourceWithDevice(this);
         pool = D3DPOOL_MANAGED;
         break;

      case GFXBufferTypeDynamic:
      case GFXBufferTypeVolatile:
         pool = D3DPOOL_DEFAULT;
         res->registerResourceWithDevice(this);
         usage |= D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
         break;
   }

   // Create vertex buffer
   if(bufferType == GFXBufferTypeVolatile)
   {
      // Get volatile stuff from a pool...
      AssertFatal( numVerts < MAX_DYNAMIC_VERTS, "Cannot allocate that many verts in a volatile vertex buffer, increase MAX_DYNAMIC_VERTS! -- BJG" );

      // This is all we need here, everything else lives in the lock method on the 
      // buffer...

   }
   else
   {
      // Get a new buffer...
      D3D8Assert( mD3DDevice->CreateVertexBuffer( vertSize * numVerts, usage, vertFlags, pool, &res->vb ), 
               "Failed to allocate VB" );

   }
  

   res->mNumVerts = numVerts;
   res->mStreamNum = streamNum;
   return res;
}

//-----------------------------------------------------------------------------
// deallocate vertex buffer
//-----------------------------------------------------------------------------
void GFXD3D8Device::deallocVertexBuffer( GFXD3D8VertexBuffer *vertBuff )
{
   SAFE_RELEASE(vertBuff->vb);
}

void GFXD3D8Device::resurrectTextureManager()
{
   mTextureManager->resurrect();
}

void GFXD3D8Device::zombifyTextureManager()
{
   mTextureManager->zombify();
}

void GFXD3D8Device::setLightInternal(U32 lightStage, const GFXLightInfo light, bool lightEnable)
{
   if(!lightEnable)
   {
      mD3DDevice->LightEnable(lightStage, false);
      return;
   }
   D3DLIGHT8 d3dLight;
   switch (light.mType)
   {
      case GFXLightInfo::Ambient:
         AssertFatal(false, "Instead of setting an ambient light you should set the global ambient color.");
         return;
      case GFXLightInfo::Vector:
         d3dLight.Type = D3DLIGHT_DIRECTIONAL;
         break;

      case GFXLightInfo::Point:
         d3dLight.Type = D3DLIGHT_POINT;
         break;

      case GFXLightInfo::Spot:
         d3dLight.Type = D3DLIGHT_SPOT;
         break;

      default :
         AssertFatal(false, "Unknown light type!");
   };

   dMemcpy(&d3dLight.Diffuse, &light.mColor, sizeof(light.mColor));
   dMemcpy(&d3dLight.Ambient, &light.mAmbient, sizeof(light.mAmbient));
   dMemcpy(&d3dLight.Specular, &light.mColor, sizeof(light.mColor));
   dMemcpy(&d3dLight.Position, &light.mPos, sizeof(light.mPos));
   dMemcpy(&d3dLight.Direction, &light.mDirection, sizeof(light.mDirection));

   d3dLight.Range = light.mRadius;

   d3dLight.Falloff = 1.0;

   d3dLight.Attenuation0 = 1.0f;
   d3dLight.Attenuation1 = 0.1f;
   d3dLight.Attenuation2 = 0.0f;

   d3dLight.Theta = light.sgSpotAngle;
   d3dLight.Phi = light.sgSpotAngle;

   mD3DDevice->SetLight(lightStage, &d3dLight);
   mD3DDevice->LightEnable(lightStage, true);
}

void GFXD3D8Device::setLightMaterialInternal(const GFXLightMaterial mat)
{
   D3DMATERIAL8 d3dmat;
   dMemset(&d3dmat, 0, sizeof(D3DMATERIAL8));
   D3DCOLORVALUE col;

   col.r = mat.ambient.red;
   col.g = mat.ambient.green;
   col.b = mat.ambient.blue;
   col.a = mat.ambient.alpha;
   d3dmat.Ambient = col;

   col.r = mat.diffuse.red;
   col.g = mat.diffuse.green;
   col.b = mat.diffuse.blue;
   col.a = mat.diffuse.alpha;
   d3dmat.Diffuse = col;

   col.r = mat.specular.red;
   col.g = mat.specular.green;
   col.b = mat.specular.blue;
   col.a = mat.specular.alpha;
   d3dmat.Specular = col;

   col.r = mat.emissive.red;
   col.g = mat.emissive.green;
   col.b = mat.emissive.blue;
   col.a = mat.emissive.alpha;
   d3dmat.Emissive = col;

   d3dmat.Power = mat.shininess;
   mD3DDevice->SetMaterial(&d3dmat);
}

void GFXD3D8Device::setGlobalAmbientInternal(ColorF color)
{
   mD3DDevice->SetRenderState(D3DRS_AMBIENT,
      D3DCOLOR_COLORVALUE(color.red, color.green, color.blue, color.alpha));
}

//-----------------------------------------------------------------------------
// This function should ONLY be called from GFXDevice::updateStates() !!!
//-----------------------------------------------------------------------------
void GFXD3D8Device::setTextureInternal( U32 textureUnit, const GFXTextureObject *texture)
{
   if( texture == NULL   )
   {
      mD3DDevice->SetTexture( textureUnit, NULL );
      return;
   }

   GFXD3D8TextureObject *tex = (GFXD3D8TextureObject *) texture;
   mD3DDevice->SetTexture( textureUnit, tex->getTex() );
}


//-----------------------------------------------------------------------------
// Copy back buffer to Sfx Back Buffer
//-----------------------------------------------------------------------------
void GFXD3D8Device::copyBBToSfxBuff()
{
   if( !mSfxBackBuffer )
   {
      mSfxBackBuffer.set( smSfxBackBufferSize, smSfxBackBufferSize, mBackbufferFormat, &GFXDefaultRenderTargetProfile );
   }

   IDirect3DSurface8 * backBuffer;
   mD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer );

   IDirect3DSurface8 *surf;
   GFXD3D8TextureObject *texObj = (GFXD3D8TextureObject*)(GFXTextureObject*)mSfxBackBuffer;
   texObj->get2DTex()->GetSurfaceLevel( 0, &surf );

   // CodeReview: This may not work like it does in D3D9. If there is an issue
   // check the CopyRects function. [5/10/2007 Pat]
   mD3DDevice->CopyRects( backBuffer, NULL, 0, surf, NULL );

   surf->Release();
   backBuffer->Release();
}

void GFXD3D8Device::copyBBToBuff(GFXTexHandle &TexBuff)
{
	if( !TexBuff )
	{
		return;
	}

	IDirect3DSurface8 * backBuffer;
	mD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer );

	IDirect3DSurface8 *surf;
	GFXD3D8TextureObject *texObj = (GFXD3D8TextureObject*)(GFXTextureObject*)TexBuff;
	texObj->get2DTex()->GetSurfaceLevel( 0, &surf );

	// CodeReview: This may not work like it does in D3D9. If there is an issue
	// check the CopyRects function. [5/10/2007 Pat]
	mD3DDevice->CopyRects( backBuffer, NULL, 0, surf, NULL );

	surf->Release();
	backBuffer->Release();
}
//-----------------------------------------------------------------------------
// Initialize - create window, device, etc
//-----------------------------------------------------------------------------
void GFXD3D8Device::init( const GFXVideoMode &mode, PlatformWindow *window )
{
   deviceOnInit();
   initD3DXFnTable();
   // Set up the Enum translation tables
   GFXD3D8EnumTranslate::init();

   // Grab the HWND.
   Win32Window *win = dynamic_cast<Win32Window*>(window);
   AssertISV(win, "GFXD3D8Device::init - got a non Win32Window window passed in! Did DX go crossplatform?");

   HWND winHwnd = win->getHWND();

   // Create D3D Presentation params
   D3DPRESENT_PARAMETERS d3dpp = setupPresentParams( mode, winHwnd );

   HRESULT hres = mD3D->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, TRUE );

   hres = mD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, winHwnd,
                                      D3DCREATE_MIXED_VERTEXPROCESSING,
                                      &d3dpp, &mD3DDevice );

   //regenStates();

   if(!mD3DDevice)
   {
      // D3D8 requires that the backbuffer format must be compatible with the desktop format if we're windowed.
      // Direct3D8: (ERROR) :Windowed BackBuffer Format must be compatible with Desktop Format. CreateDevice/Reset fails.
      // Chances are we're requesting a 32 bit backbuffer but only have a 16 bit desktop, so let's try a 
      // 16 bit backbuffer.
      if(d3dpp.BackBufferFormat == D3DFMT_X8R8G8B8)
      {
         // Bump it down to 16 bits
         Con::printf("Failed to create D3D8 device with 32 bit format, trying 16 bit format...");

         d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
         hres = mD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, winHwnd,
            D3DCREATE_MIXED_VERTEXPROCESSING,
            &d3dpp, &mD3DDevice );
      }
      else if(d3dpp.BackBufferFormat == D3DFMT_R5G6B5)
      {
         // Or maybe we're requesting a 16 bit backbuffer on a 32 bit desktop.  Bump it up to 32 bits
         Con::printf("Failed to create D3D8 device with 16 bit format, trying 32 bit format...");
         d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
         hres = mD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, winHwnd,
            D3DCREATE_MIXED_VERTEXPROCESSING,
            &d3dpp, &mD3DDevice );
      }

      // Well, that didn't work, something else must be going wrong.
      // Let the user know that we failed and quit.

      // Abort if we can't get going...
      // CodeReview Maybe make this non-catastrophic failure? [bjg,5/17/07]
      if(!mD3DDevice)
      {
         Platform::AlertOK("DirectX Error!", 
            "Failed to initialize Direct3D under DirectX 8! "
            "Make sure you have DirectX 8.1c+ installed and that your monitor is set to at least 16 bit color.");
         Platform::forceShutdown(1);
      }
   }

   // Keep our backbuffer format for the SFX texture
   mBackbufferFormat = d3dpp.BackBufferFormat == D3DFMT_X8R8G8B8 ? GFXFormatR8G8B8 : GFXFormatR5G6B5;

   // Check up on things
   Con::printf("   Cur. D3DDevice ref count=%d", mD3DDevice->AddRef() - 1);
   mD3DDevice->Release();
   
   mTextureManager = new GFXD3D8TextureManager( mD3DDevice );

   // Now re aquire all the resources we trashed earlier
   reacquireDefaultPoolResources();
   
   // Setup default states
   initStates();

   
   //-------- Output init info ---------   
   D3DCAPS8 caps;
   mD3DDevice->GetDeviceCaps( &caps );
   
   U8 *pxPtr = (U8*) &caps.PixelShaderVersion;
   mPixVersion = pxPtr[1] + pxPtr[0] * 0.1;
   Con::printf( "   Pix version detected: %f", mPixVersion );

   bool forcePixVersion = Con::getBoolVariable( "$pref::Video::forcePixVersion", false );
   if( forcePixVersion )
   {
      float forcedPixVersion = Con::getFloatVariable( "$pref::Video::forcedPixVersion", mPixVersion );
      if( forcedPixVersion < mPixVersion )
      {
         mPixVersion = forcedPixVersion;
         Con::errorf( "   Forced pix version: %f", mPixVersion );
      }
   }

   mPixVersion = 0.0f;

   U8 *vertPtr = (U8*) &caps.VertexShaderVersion;
   F32 vertVersion = vertPtr[1] + vertPtr[0] * 0.1;
   Con::printf( "   Vert version detected: %f", vertVersion );

   // Profile number of samplers, store and clamp to TEXTURE_STAGE_COUNT
   mNumSamplers = caps.MaxSimultaneousTextures;

   if( mNumSamplers > TEXTURE_STAGE_COUNT )
      mNumSamplers = TEXTURE_STAGE_COUNT;
   Con::printf( "   Maximum number of simultaneous samplers: %d", mNumSamplers );

   mCardProfiler = new GFXD3D8CardProfiler();
   mCardProfiler->init();

   gScreenShot = new ScreenShotD3D8;

   mInitialized = true;

   deviceInited();

   SAFE_RELEASE(mDepthStencil);
   D3D8Assert( mD3DDevice->GetDepthStencilSurface(&mDepthStencil), 
      "GFXD3D8Device::init - failed to get depth-stencil!");
}

//-----------------------------------------------------------------------------
// Setup D3D present parameters - init helper function
//-----------------------------------------------------------------------------
D3DPRESENT_PARAMETERS GFXD3D8Device::setupPresentParams( const GFXVideoMode &mode, const HWND &hwnd )
{
   // Create D3D Presentation params
   D3DPRESENT_PARAMETERS d3dpp; 
   dMemset( &d3dpp, 0, sizeof( d3dpp ) );

   D3DFORMAT fmt = D3DFMT_X8R8G8B8; // 32 bit

   if( mode.bitDepth == 16 )
   {
      fmt = D3DFMT_R5G6B5;
   }

   D3DDISPLAYMODE displayMode;
   mD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &displayMode );   

   d3dpp.BackBufferWidth  = mode.resolution.x;
   d3dpp.BackBufferHeight = mode.resolution.y;
   d3dpp.BackBufferFormat = fmt;
   d3dpp.BackBufferCount  = 1;
   d3dpp.MultiSampleType  = D3DMULTISAMPLE_NONE;
   d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
   d3dpp.hDeviceWindow    = hwnd;
   d3dpp.Windowed         = !mode.fullScreen;
   d3dpp.EnableAutoDepthStencil = TRUE;
   d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
   d3dpp.Flags            = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
   d3dpp.FullScreen_RefreshRateInHz = (mode.refreshRate == 0 || !mode.fullScreen) ? 
                                       D3DPRESENT_RATE_DEFAULT : mode.refreshRate;
   d3dpp.FullScreen_PresentationInterval = ( d3dpp.Windowed ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE );

   return d3dpp;
}

//-----------------------------------------------------------------------------
// Reset D3D device
//-----------------------------------------------------------------------------
void GFXD3D8Device::reset( D3DPRESENT_PARAMETERS &d3dpp )
{
   mInitialized = false;

   // Debug the ref count on depthstencil...
   if(mDepthStencil)
   {
      Con::printf("GFXPCD3D8Device::reset - depthstencil has %d ref's", mDepthStencil->AddRef() - 1);
      mDepthStencil->Release();
   }

   // First release all the stuff we allocated from D3DPOOL_DEFAULT
   releaseDefaultPoolResources();

   // reset device   
   Con::printf( "--- Resetting D3D Device ---" );
   HRESULT hres = S_OK;
   hres = mD3DDevice->Reset( &d3dpp );

   if( FAILED( hres ) )
   {
      while( mD3DDevice->TestCooperativeLevel() == D3DERR_DEVICELOST )
      {
         Sleep( 100 );
      }

      hres = mD3DDevice->Reset( &d3dpp );
   }

   D3D8Assert( hres, "Failed to create D3D Device" );

   // Setup default states
   initStates();

   // Now re aquire all the resources we trashed earlier
   reacquireDefaultPoolResources();

   mInitialized = true;
}

//-----------------------------------------------------------------------------
// Get a string indicating the installed DirectX version, revision and letter number
//-----------------------------------------------------------------------------
char * GFXD3D8Device::getDXVersion()
{
   DWORD dwVersion = 0;
   DWORD dwRevision = 0;
   TCHAR dxVersionLetter = ' ';

   NVDXDiagWrapper::DXDiagNVUtil * dxDiag = new NVDXDiagWrapper::DXDiagNVUtil();
   dxDiag->InitIDxDiagContainer();
   dxDiag->GetDirectXVersion( &dwVersion, &dwRevision, &dxVersionLetter );
   dxDiag->FreeIDxDiagContainer();
   delete dxDiag;

   if( dwVersion == 0 && dwRevision == 0 )
   {
      Con::errorf("Could not determine DirectX version.");
      return "0";
   }

   static char s_pcVersionString[256];

   dSprintf(s_pcVersionString, 256, "%d.%d.%d %c", LOWORD(dwVersion), HIWORD(dwRevision),
      LOWORD(dwRevision), dxVersionLetter); 

   return s_pcVersionString;
}

//-----------------------------------------------------------------------------
// Enumerate D3D adapters
//-----------------------------------------------------------------------------
void GFXD3D8Device::enumerateAdapters( Vector<GFXAdapter*> &adapterList )
{
   // Grab a D3D8 handle here to first get the D3D8 devices
   LPDIRECT3D8 d3d8 = Direct3DCreate8( D3D_SDK_VERSION );
   
   AssertISV( d3d8, "Could not create D3D object, make sure you have the latest version of DirectX installed" );

   // Print DX version ASAP - this is not at top of function because it has not
   // been tested with DX 8, so it may crash and the user would not get a dialog box...
   Con::printf( "DirectX 8 version - %s", getDXVersion() );

   for( U32 i = 0; i < d3d8->GetAdapterCount(); i++ ) 
   {
      GFXAdapter *toAdd = new GFXAdapter();
      toAdd->mType = Direct3D8;
      toAdd->mIndex = i;

      D3DADAPTER_IDENTIFIER8 temp;
      d3d8->GetAdapterIdentifier( i, NULL, &temp ); // The NULL is the flags which deal with WHQL

      // Change this when we get string library
      dStrcpy( toAdd->mName, sizeof(toAdd->mName), temp.Description );
      dStrncat(toAdd->mName, sizeof(toAdd->mName), " DirectX 8\0", GFXAdapter::MaxAdapterNameLen);

      // Enumerate the video modes.
      for( U32 j = 0; j < d3d8->GetAdapterModeCount( D3DADAPTER_DEFAULT ); j++ ) 
      {
         D3DDISPLAYMODE mode;
         d3d8->EnumAdapterModes( D3DADAPTER_DEFAULT, j, &mode );

         GFXVideoMode vmAdd;

         // Calc the bit-depth or skip it if we can't tell.
         if(mode.Format == D3DFMT_X8R8G8B8 || mode.Format == D3DFMT_R8G8B8)
            vmAdd.bitDepth = 32;
         else if(mode.Format == D3DFMT_R5G6B5 || mode.Format == D3DFMT_A1R5G5B5)
            vmAdd.bitDepth = 16;
         else
            continue;

         vmAdd.fullScreen  = true;
         vmAdd.refreshRate = mode.RefreshRate;
         vmAdd.resolution  = Point2I( mode.Width, mode.Height );

         toAdd->mAvailableModes.push_back( vmAdd );
      }

      adapterList.push_back( toAdd );
   }

   d3d8->Release();
}
//------------------------------------------------------------------------------
void GFXD3D8Device::enterDebugEvent(ColorI color, const char *name)
{
   // No support for this in D3D8 -patw
   AssertFatal( false, "Debug events not supported in D3D8" );
}

//------------------------------------------------------------------------------
void GFXD3D8Device::leaveDebugEvent()
{
   // No support for this in D3D8 -patw
   AssertFatal( false, "Debug events not supported in D3D8" );
}

//------------------------------------------------------------------------------
void GFXD3D8Device::setDebugMarker(ColorI color, const char *name)
{
   // No support for this in D3D8 -patw
   AssertFatal( false, "Debug events not supported in D3D8" );
}

//------------------------------------------------------------------------------

GFXWindowTarget * GFXD3D8Device::allocWindowTarget( PlatformWindow *window )
{
   AssertFatal(window,"GFXD3DDevice::allocWindowTarget - no window provided!");
   AssertFatal(dynamic_cast<Win32Window*>(window), 
      "GFXD3DDevice::allocWindowTarget - only works with Win32Windows!");

   Win32Window *w32w = static_cast<Win32Window*>(window);

   // Set up a new window target...
   GFXD3D8WindowTarget *gdwt = new GFXD3D8WindowTarget();
   gdwt->mSize = window->getBounds().extent;
   gdwt->mDevice = this;
   gdwt->mWindow = w32w;
   gdwt->initPresentationParams();

   // Now, we have to init & bind our device... we have basically two scenarios
   // of which the first is:
   if(mD3DDevice == NULL)
   {
      // Initialize the device.
      AssertISV(mInitialized == false, "GFXD3DDevice::allocWindowTarget - trying to init an already init'ed device!");
      init(window->getCurrentMode(), window);

      // Cool, we have created the device, grab back the depthstencil buffer as well
      // as the swap chain.
      HRESULT hres = mD3DDevice->GetDepthStencilSurface(&gdwt->mDepthStencil);
      D3D8Assert(hres, "GFXD3DDevice::allocWindowTarget - couldn't get depth stencil!");

      gdwt->mImplicit = true;
   }
   else
   {
      // And the second case:
      // Initialized device, create an additional swap chain.      
      gdwt->mImplicit = false;

      HRESULT hres = mD3DDevice->CreateAdditionalSwapChain(&gdwt->mPresentationParams, &gdwt->mSwapChain);
      if(hres != S_OK)
      {
         Con::errorf("GFXD3DDevice::allocWindowTarget - failed to create additional swap chain!");
         SAFE_DELETE(gdwt);
         return NULL;
      }

      hres = mD3DDevice->CreateDepthStencilSurface(gdwt->mPresentationParams.BackBufferWidth, gdwt->mPresentationParams.BackBufferHeight, 
         D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &gdwt->mDepthStencil);

      if(hres != S_OK)
      {
         Con::errorf("GFXD3D8Device::allocWindowTarget - failed to create additional swap chain zbuffer!");
         SAFE_DELETE(gdwt);
         return NULL;
      }
   }
   gdwt->registerResourceWithDevice(this);
   return gdwt;
}

GFXTextureTarget * GFXD3D8Device::allocRenderToTextureTarget()
{
   GFXD3D8TextureTarget* targ = new GFXD3D8TextureTarget();
   targ->registerResourceWithDevice(this);
   return targ;
}

GFXTextureTarget * GFXD3D8Device::allocRenderToTextureTarget( Point2I size, GFXFormat format )
{
   AssertISV(false, "GFXD3DDevice::allocRenderToTextureTarget - alloc with stencil-depth not currently supported.");
   return 0;
}

void GFXD3D8Device::pushActiveRenderTarget()
{
   // Duplicate last item on the stack.
   mRTStack.push_back(mCurrentRT);
}

void GFXD3D8Device::popActiveRenderTarget()
{
   // Pop the last item on the stack, set next item down.
   AssertFatal(mRTStack.size() > 0, "GFXD3DDevice::popActiveRenderTarget - stack is empty!");
   setActiveRenderTarget(mRTStack.last());
   mRTStack.pop_back();
}

GFXTarget *GFXD3D8Device::getActiveRenderTarget()
{
   return mCurrentRT;
}

void GFXD3D8Device::setActiveRenderTarget(GFXTarget *target )
{
   // If we're lacking a window manager, then setting a NULL target will be
   // interpreted as wanting to go back to the frame buffer.
   if(target == NULL)
   {
      // Early out to avoid mutiple re-sets.
      if(mCurrentRT == NULL)
         return;

      mCurrentRT = NULL;

      IDirect3DSurface8 *surf;

      D3D8Assert(mD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surf), "a");
      D3D8Assert(mD3DDevice->SetRenderTarget( surf, mDepthStencil ), "b");

      // reset viewport data
      D3DSURFACE_DESC desc;
      D3D8Assert(surf->GetDesc( &desc ), "c");
      D3D8Assert(surf->Release(), "d");
      RectI vp( 0,0, desc.Width, desc.Height );
      setViewport( vp );

      return;
   }

   AssertFatal(target, 
      "GFXD3D8Device::setActiveRenderTarget - must specify a render target!");

   // Let's do a little validation here...
   bool isValid = dynamic_cast<GFXD3D8WindowTarget*>(target) || dynamic_cast<GFXD3D8TextureTarget*>(target);
   AssertFatal( isValid, 
      "GFXD3D8Device::setActiveRenderTarget - invalid target subclass passed!");

   // Update our current RT.
   mCurrentRT = target;

   // Deal with window case.
   if(GFXD3D8WindowTarget *gdwt = dynamic_cast<GFXD3D8WindowTarget*>(target))
   {
      // Grab the backbuffer and set it.
      IDirect3DSurface8 *bb;
      if(gdwt->mImplicit)
         mD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &bb);
      else
         gdwt->mSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &bb);
      
      mD3DDevice->SetRenderTarget(bb, gdwt->mDepthStencil);
      bb->Release();

      // Exit!
      return;
   }

   // Deal with texture target case.
   if(GFXD3D8TextureTarget *gdtt = dynamic_cast<GFXD3D8TextureTarget*>(target))
   {
      // Clear the state indicator.
      gdtt->stateApplied();

      // Set all the surfaces into the appropriate slots.
      D3D8Assert(mD3DDevice->SetRenderTarget(gdtt->mTargets[GFXTextureTarget::Color0], gdtt->mTargets[GFXTextureTarget::DepthStencil]), 
         "GFXD3D8Device::setActiveRenderTarget - failed to set render target!" );

      // Reset the viewport.
      D3DSURFACE_DESC desc;
      gdtt->mTargets[GFXTextureTarget::Color0]->GetDesc( &desc );
      RectI vp( 0,0, desc.Width, desc.Height );
      setViewport( vp );

      // Exit!
      return;
   }
}

GFXCubemap * GFXD3D8Device::createCubemap()
{
   GFXD3D8Cubemap* cube = new GFXD3D8Cubemap();
   cube->registerResourceWithDevice(this);
   return cube;
}

GFXFence * GFXD3D8Device::createFence()
{
   GFXGeneralFence* fence = new GFXGeneralFence( this );
   fence->registerResourceWithDevice(this);
   return fence;
}
