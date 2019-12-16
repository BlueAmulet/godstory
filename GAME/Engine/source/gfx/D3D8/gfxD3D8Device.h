//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GFXD3D8Device_H_
#define _GFXD3D8Device_H_

#include "platform/platform.h"

//-----------------------------------------------------------------------------

// NOTE: Commented this section out, because of include/library conflicts

//#ifdef POWER_DEBUG
//   #include "dxerr8.h"
//#endif

inline void D3D8Assert( HRESULT hr, const char *info ) 
{
   AssertFatal( SUCCEEDED( hr ), info );
//#if defined( POWER_DEBUG )
//   if( FAILED( hr ) ) 
//   {
//      char buf[256];
//      dSprintf( buf, 256, "%s\n%s\n%s", DXGetErrorString8( hr ), DXGetErrorDescription8( hr ), info );
//      AssertFatal( false, buf ); 
////      DXTrace( __FILE__, __LINE__, hr, info, true );
//   }
//#endif
}

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) if( (a) != NULL ) (a)->Release(); (a) = NULL;
#endif

// Forward declarations, so that D3D8 and D3D9 can live together in harmony
struct IDirect3DIndexBuffer8;
struct IDirect3DVertexBuffer8;
struct IDirect3D8;
struct IDirect3DDevice8;
struct _D3DVIEWPORT8;

#include "gfx/gfxDevice.h"
#include "platformWin32/platformWin32.h"
#include "gfx/D3D8/gfxD3D8TextureManager.h"
#include "gfx/D3D8/gfxD3D8ShaderMgr.h"
#include "gfx/D3D8/gfxD3D8Cubemap.h"
#include "gfx/D3D8/gfxD3D8PrimitiveBuffer.h"
#include "platform/platformDlibrary.h"
#include "gfx/gfxInit.h"
#include "gfx/gfxFence.h"

#define D3DX_FUNCTION(fn_name, fn_return, fn_args) \
   typedef fn_return (_cdecl *D3DXFNPTR##fn_name##)##fn_args##;
#include "gfx/D3D8/d3dx8Functions.h"
#undef D3DX_FUNCTION

// Function table
struct D3D8XFNTable
{
   D3D8XFNTable() : isLoaded( false ){};
   bool isLoaded;
   DLibraryRef dllRef;
#define D3DX_FUNCTION(fn_name, fn_return, fn_args) \
   D3DXFNPTR##fn_name fn_name;
#include "gfx/D3D8/d3dx8Functions.h"
#undef D3DX_FUNCTION
};

#define GFXD3D8X static_cast<GFXD3D8Device *>(GFX)->smD3DX 

class GFXD3D8Device : public GFXDevice
{
   friend class GFXD3D8PrimitiveBuffer;
   friend class GFXD3D8VertexBuffer;
   friend class GFXD3D8TextureObject;
   typedef GFXDevice Parent;

   private:
      static void initD3DXFnTable();
      MatrixF mTempMatrix;    ///< Temporary matrix, no assurances on value at all
      _D3DVIEWPORT8 *mViewport; ///< Because setViewport gets called a lot, don't want to allocate/unallocate a lot
      RectI mViewportRect;
      RectI mClipRect;
      IDirect3DSurface8 *mDepthStencil;
      typedef RefPtr<GFXD3D8VertexBuffer> RPGDVB;
      Vector<RPGDVB> mVolatileVBList;

      GFXD3D8VertexBuffer *mCurrentOpenAllocVB;
      GFXD3D8VertexBuffer *mCurrentVB;
      void *mCurrentOpenAllocVertexData;
      //-----------------------------------------------------------------------
      RefPtr<GFXD3D8PrimitiveBuffer> mDynamicPB;                       ///< Dynamic index buffer
      GFXD3D8PrimitiveBuffer *mCurrentOpenAllocPB;
      GFXD3D8PrimitiveBuffer *mCurrentPB;

      IDirect3D8       *mD3D;        ///< D3D Handle
      IDirect3DDevice8 *mD3DDevice;  ///< Handle for D3DDevice

      GFXFormat mBackbufferFormat; ///< Our backbuffer format

      U32  mAdapterIndex;            ///< Adapter index because D3D supports multiple adapters

      GFXD3D8ShaderMgr mShaderMgr;    ///< D3D Shader Manager
      F32 mPixVersion;
      U32 mNumSamplers;
      
      /// Special effects back buffer - for refraction and other effects
      virtual void copyBBToSfxBuff();
	  virtual void copyBBToBuff(GFXTexHandle &);

      /// To manage creating and re-creating of these when device is aquired
      void reacquireDefaultPoolResources();

      /// To release all resources we control from D3DPOOL_DEFAULT
      void releaseDefaultPoolResources();

      /// This you will probably never, ever use, but it is used to generate the code for
      /// the initStates() function
      void regenStates();

      GFXD3D8VertexBuffer * findVBPool( U32 vertFlags );
      GFXD3D8VertexBuffer * createVBPool( U32 vertFlags, U32 vertSize );

#ifdef POWER_DEBUG
      /// @name Debug Vertex Buffer information/management
      /// @{
      U32 mNumAllocatedVertexBuffers; ///< To keep track of how many are allocated and freed
      GFXD3D8VertexBuffer *mVBListHead;
      void addVertexBuffer( GFXD3D8VertexBuffer *buffer );
      void removeVertexBuffer( GFXD3D8VertexBuffer *buffer );
      void logVertexBuffers();
      /// @}
#endif

   protected:

      // State overrides
      // {
      void setRenderState( U32 state, U32 value);
      void setTextureStageState( U32 stage, U32 state, U32 value );
      void setSamplerState( U32 stage, U32 type, U32 value );

      void setTextureInternal(U32 textureUnit, const GFXTextureObject* texture);

      virtual void setLightInternal(U32 lightStage, const GFXLightInfo light, bool lightEnable);
      virtual void setLightMaterialInternal(const GFXLightMaterial mat);
      virtual void setGlobalAmbientInternal(ColorF color);

      void setMatrix( GFXMatrixType mtype, const MatrixF &mat );

      void initStates();
      // }

      // Buffer allocation
      // {  
      GFXVertexBuffer *allocVB( U32 numVerts, void **vertexPtr, U32 vertFlags, U32 vertSize );
      GFXVertexBuffer *allocPooledVB( U32 numVerts, void **vertexPtr, U32 vertFlags, U32 vertSize );

      GFXPrimitiveBuffer *allocPB( U32 numIndices, void **indexPtr );
      GFXPrimitiveBuffer *allocPooledPB( U32 numIndices, void **indexPtr );
      // } 

      // Index buffer management
      // {
      void setPrimitiveBuffer( GFXPrimitiveBuffer *buffer );
      void drawIndexedPrimitive( GFXPrimitiveType primType, U32 minIndex, U32 numVerts, U32 startIndex, U32 primitiveCount );
      // }

      virtual GFXShader * createShader( const char *vertFile, const char *pixFile, F32 pixVersion);
                                        
      virtual GFXShader * createShader( GFXShaderFeatureData &featureData, GFXVertexFlags vertFlags );

      // This is called by MatInstance::reinitInstances to cause the shaders to be regenerated.
      virtual void flushProceduralShaders();      

   public:
      static D3D8XFNTable smD3DX;

      static GFXDevice *createInstance( U32 adapterIndex );

      GFXTextureObject* createRenderSurface( U32 width, U32 height, GFXFormat format, U32 mipLevel );
      
      /// Constructor
      /// @param   d3d   Direct3D object to instantiate this device with
      /// @param   index   Adapter index since D3D can use multiple graphics adapters
      GFXD3D8Device( IDirect3D8 *d3d, U32 index );
      ~GFXD3D8Device();

         /// Get a string indicating the installed DirectX version, revision and letter number
      static char * getDXVersion();

      // Activate/deactivate
      // {
      
      ///
      virtual void init( const GFXVideoMode &mode, PlatformWindow *window = NULL );

      virtual void preDestroy() 
      {
         mTextureManager->kill(); 
      }

      GFXAdapterType getAdapterType()
      { 
         return Direct3D8; 
      }
      
      virtual GFXCubemap * createCubemap();
      
      virtual F32  getPixelShaderVersion() const
      {
         return mPixVersion; 
      }
      
      virtual void setPixelShaderVersion( F32 version )
      {
         mPixVersion = version; 
      }

      virtual void disableShaders();
      virtual void setShader( GFXShader *shader );
      virtual void setVertexShaderConstF( U32 reg, const float *data, U32 size, U32 realCntFloat = 4 );
      virtual void setPixelShaderConstF( U32 reg, const float *data, U32 size, U32 realCntFloat = 4 );
      virtual U32  getNumSamplers() const { return mNumSamplers; }

      virtual void enterDebugEvent(ColorI color, const char *name);
      virtual void leaveDebugEvent();
      virtual void setDebugMarker(ColorI color, const char *name);
      
      void enumerateVideoModes();
      static void enumerateAdapters( Vector<GFXAdapter*> &adapterList );
      void setVideoMode( const GFXVideoMode &mode );

	  virtual GFXFormat selectSupportedFormat(GFXTextureProfile *profile,
		  const Vector<GFXFormat> &formats, bool texture, bool mustblend);
      // }

      // Misc rendering control
      // {
      void clear( U32 flags, ColorI color, F32 z, U32 stencil );
      void beginSceneInternal();
      void endSceneInternal();

      void setViewport( const RectI &rect );
      const RectI &getViewport() const;

      void setClipRect( const RectI &rect );
      void setClipRectOrtho( const RectI &rect, const RectI &orthoRect );
      const RectI &getClipRect() const;
      // }

      // Render Targets
      // {

      virtual GFXTextureTarget *allocRenderToTextureTarget();
      virtual GFXTextureTarget *allocRenderToTextureTarget(Point2I size, GFXFormat format);
      virtual GFXWindowTarget *allocWindowTarget(PlatformWindow *window);
      virtual void pushActiveRenderTarget();
      virtual void popActiveRenderTarget();
      virtual void setActiveRenderTarget( GFXTarget *target );
      virtual GFXTarget *getActiveRenderTarget();

      // }

      // Vertex/Index buffer management
      // {
      ///
      void setVB( GFXVertexBuffer *buffer );

      GFXVertexBuffer    *allocVertexBuffer   ( U32 numVerts, U32 vertFlags, U32 vertSize, GFXBufferType bufferType,U8 streamNum=0 );
      GFXPrimitiveBuffer *allocPrimitiveBuffer( U32 numIndices, U32 numPrimitives, GFXBufferType bufferType );
      void  deallocVertexBuffer( GFXD3D8VertexBuffer *vertBuff );
      // }

     GFXTextureObject* allocTexture( GBitmap *bitmap, GFXTextureType type = GFXTextureType_Normal, bool extrudeMips = false );
     GFXTextureObject* allocTexture( StringTableEntry fileName, GFXTextureType type = GFXTextureType_Normal, bool extrudeMips = false, bool preserveSize = false );

      void zombifyTextureManager();
      void resurrectTextureManager();

      U32 getMaxDynamicVerts() { return MAX_DYNAMIC_VERTS; }
      U32 getMaxDynamicIndices() { return MAX_DYNAMIC_INDICES; }

      // Rendering
      // {
      void drawPrimitive( GFXPrimitiveType primType, U32 vertexStart, U32 primitiveCount );
      // }

      IDirect3DDevice8 *getDevice(){ return mD3DDevice; }

      /// Device helper function
      D3DPRESENT_PARAMETERS setupPresentParams( const GFXVideoMode &mode, const HWND &hwnd );

      /// Reset
      void reset( D3DPRESENT_PARAMETERS &d3dpp );

      inline F32 getFillConventionOffset() const { return 0.5f; }

      GFXFence *createFence();
};


inline const RectI &GFXD3D8Device::getClipRect() const
{
   return mClipRect;
}

inline const RectI &GFXD3D8Device::getViewport() const
{
   return mViewportRect;
}

#endif
