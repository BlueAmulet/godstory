//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <d3d8.h>
#include <d3dx8math.h>
#include "gfx/D3D8/gfxD3D8Device.h"
#include "gfx/D3D8/gfxD3D8Target.h"
#include "gfx/D3D8/gfxD3D8TextureObject.h"
#include "gfx/D3D8/gfxD3D8Cubemap.h"

GFXD3D8WindowTarget::GFXD3D8WindowTarget()
{
   mSwapChain = NULL;
   mDepthStencil = NULL;
}

GFXD3D8WindowTarget::~GFXD3D8WindowTarget()
{
   SAFE_RELEASE(mSwapChain);
   SAFE_RELEASE(mDepthStencil);
}

void GFXD3D8WindowTarget::initPresentationParams()
{
   // Clear everything to safe values.
   dMemset( &mPresentationParams, 0, sizeof( mPresentationParams ) );

   // Get some video mode related info.
   GFXVideoMode vm = mWindow->getCurrentMode();

   // Do some validation...

	   // The logic in the Assert below is faulty due to how we currently start up the client
	   // What we seem to want here is an actual identifier of primary or secondary window, but instead
	   // we are relying on if we are creating a second window with a previously created device (see
	   // gfxPCDD9Device.cpp/GFXPCD3D9Device::allocWindowTarget, which is the only place in the code
	   // we currently set a value for mImplicit.
	   //
	   // Until we have a better way to identify the status of a window (primary or secondary), the following
	   // "validation" should be commented out. SRZ 1/25/08
       //
       // NOTE: The following applies to the D39 device as well.
#if 0
   if(vm.fullScreen == true && mImplicit == false)
   {
      AssertISV(false, 
         "GFXD3D8WindowTarget::initPresentationParams - Cannot go fullscreen with secondary window!");
   }
#endif

   // Figure our backbuffer format.
   D3DFORMAT fmt = D3DFMT_X8R8G8B8; 

   if(vm.fullScreen)
   {
      // We can't default to the desktop bitdepth if we're fullscreen - actually
      // set something.

      if( vm.bitDepth == 16 )
         fmt = D3DFMT_R5G6B5; // 16 bit
      else
         fmt = D3DFMT_X8R8G8B8; // 32 bit
   }

   // And fill out our presentation params.
   mPresentationParams.BackBufferWidth  = vm.resolution.x;
   mPresentationParams.BackBufferHeight = vm.resolution.y;
   mPresentationParams.BackBufferFormat = fmt;
   mPresentationParams.BackBufferCount  = 1;
   mPresentationParams.MultiSampleType  = D3DMULTISAMPLE_NONE;
   mPresentationParams.SwapEffect       = D3DSWAPEFFECT_DISCARD;
   mPresentationParams.hDeviceWindow    = mWindow->getHWND();
   mPresentationParams.Windowed         = (vm.fullScreen ? FALSE : TRUE);
   mPresentationParams.EnableAutoDepthStencil = TRUE;
   mPresentationParams.AutoDepthStencilFormat = D3DFMT_D24S8;
   mPresentationParams.Flags            = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
   mPresentationParams.FullScreen_RefreshRateInHz = (vm.refreshRate == 0 || !vm.fullScreen) ? 
                                                     D3DPRESENT_RATE_DEFAULT : vm.refreshRate;
   mPresentationParams.FullScreen_PresentationInterval = ( mPresentationParams.Windowed ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE );
}

PlatformWindow * GFXD3D8WindowTarget::getWindow()
{
   return mWindow;
}

const Point2I GFXD3D8WindowTarget::getSize()
{
   return mSize;
}

bool GFXD3D8WindowTarget::present()
{
   HRESULT res;

   if(mImplicit)
      res = mDevice->getDevice()->Present(NULL, NULL, NULL, NULL);
   else
      res = mSwapChain->Present(NULL, NULL, NULL, NULL);

   return (res == S_OK);
}

void GFXD3D8WindowTarget::resetMode()
{
   mWindow->mSuppressReset = true;

   SAFE_RELEASE(mSwapChain);
   SAFE_RELEASE(mDepthStencil);
   initPresentationParams();

   // So, the video mode has changed - if we're an additional swap chain
   // just kill the swapchain and reallocate to match new vid mode.
   if(mImplicit == false)
   {
      D3D8Assert(mDevice->getDevice()->CreateAdditionalSwapChain(&mPresentationParams, &mSwapChain),
         "GFXD3D8WindowTarget::resetMode - couldn't reallocate additional swap chain!");
      D3D8Assert(mDevice->getDevice()->CreateDepthStencilSurface(mPresentationParams.BackBufferWidth, mPresentationParams.BackBufferHeight, 
         D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &mDepthStencil), 
         "Unable to create z/stencil for additional swap chain!");
   }
   else
   {
      // Otherwise, we have to reset the device, if we're the implicit swapchain.
      mDevice->reset(mPresentationParams);

      // Reacquire our swapchain & DS
      mDevice->getDevice()->GetDepthStencilSurface(&mDepthStencil);
   }

   // Update our size, too.
   mSize = Point2I(mPresentationParams.BackBufferWidth, mPresentationParams.BackBufferHeight);

   mWindow->mSuppressReset = false;
}

void GFXD3D8WindowTarget::zombify()
{
   // Release our resources
   SAFE_RELEASE(mSwapChain);
   SAFE_RELEASE(mDepthStencil);
}

void GFXD3D8WindowTarget::resurrect()
{
   // This is handled by resetMode
}

// ----------------------------------------------------------------------------

GFXD3D8TextureTarget::GFXD3D8TextureTarget()
{
   mChangeToken = mLastAppliedChange = 0;

   for(S32 i=0; i<MaxRenderSlotId; i++)
      mTargets[i] = NULL;
}

GFXD3D8TextureTarget::~GFXD3D8TextureTarget()
{
   // Release anything we might be holding.
   for(S32 i=0; i<MaxRenderSlotId; i++)
   {
      SAFE_RELEASE(mTargets[i]);
   }
}

const Point2I GFXD3D8TextureTarget::getSize()
{
   if(!mTargets[Color0])
      return Point2I(0,0);

   // Query the first surface.
   D3DSURFACE_DESC sd;
   mTargets[Color0]->GetDesc(&sd);
   return Point2I(sd.Width, sd.Height);
}

void GFXD3D8TextureTarget::attachTexture( RenderSlot slot, GFXTextureObject *tex, U32 mipLevel/*=0*/, U32 zOffset /*= 0*/ )
{
   AssertFatal(slot < MaxRenderSlotId, "GFXD3D8TextureTarget::attachTexture - out of range slot.");

   // Mark state as dirty so device can know to update.
   invalidateState();

   // Release what we had, it's definitely going to change.
   SAFE_RELEASE(mTargets[slot]);

   // Are we clearing?
   if(!tex)
   {
      // Yup - just exit, it'll stay NULL.      
      return;
   }

   // Take care of default targets
   if( tex == GFXTextureTarget::sDefaultDepthStencil )
   {
      static_cast<GFXD3D8Device *>(GFX)->getDevice()->GetDepthStencilSurface(&mTargets[slot]);
   }
   // else if .. etc
   else
   {
      // Cast the texture object to D3D...
      AssertFatal(dynamic_cast<GFXD3D8TextureObject*>(tex), 
         "GFXD3D8TextureTarget::attachTexture - invalid texture object.");

      GFXD3D8TextureObject *d3dto = static_cast<GFXD3D8TextureObject*>(tex);

      // Grab the surface level.
      D3D8Assert(d3dto->get2DTex()->GetSurfaceLevel(mipLevel, &mTargets[slot]), 
         "GFXD3D8TextureTarget::attachTexture - could not get surface level for the passed texture!");
   }
}

void GFXD3D8TextureTarget::attachTexture( RenderSlot slot, GFXCubemap *tex, U32 face, U32 mipLevel/*=0*/ )
{
   AssertFatal(slot < MaxRenderSlotId, "GFXD3D8TextureTarget::attachTexture - out of range slot.");

   // Mark state as dirty so device can know to update.
   invalidateState();

   // Release what we had, it's definitely going to change.
   SAFE_RELEASE(mTargets[slot]);

   // Are we clearing?
   if(!tex)
   {
      // Yup - just exit, it'll stay NULL.      
      return;
   }

   // Cast the texture object to D3D...
   AssertFatal(dynamic_cast<GFXD3D8Cubemap*>(tex), 
      "GFXD3DTextureTarget::attachTexture - invalid cubemap object.");

   GFXD3D8Cubemap *cube = static_cast<GFXD3D8Cubemap*>(tex);

   D3D8Assert(cube->mCubeTex->GetCubeMapSurface( (D3DCUBEMAP_FACES)face, mipLevel, &mTargets[slot] ),
      "GFXD3DTextureTarget::attachTexture - could not get surface level for the passed texture!");

   mTargets[slot]->AddRef();   
}

void GFXD3D8TextureTarget::clearAttachments()
{
   for(S32 i=0; i<MaxRenderSlotId; i++)
      attachTexture((RenderSlot)i, NULL);
}

void GFXD3D8TextureTarget::zombify()
{
   // Dump our attachments and damn the consequences.
   clearAttachments();
}

void GFXD3D8TextureTarget::resurrect()
{
   // Nothing to do here
}
