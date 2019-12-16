//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(disable: 4996) 
#endif

#include <d3d8.h>
#include <d3dx8core.h>
#include <d3dx8tex.h>
#include "gfx/D3D8/gfxD3D8TextureManager.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxCardProfile.h"
#include "gfx/ddsFile.h"
#include "platform/profiler.h"
#include "gfx/D3D8/gfxD3D8EnumTranslate.h"
#include "core/unicode.h"
#include "util/swizzle.h"
#include "gfx/D3D8/gfxD3D8Device.h"

// Gross hack to let the diag utility know that we only need stubs
#define DUMMYDEF
#include "gfx/D3D/DXDiagNVUtil.h"

//-----------------------------------------------------------------------------
// Utility function, valid only in this file
#ifdef D3D_TEXTURE_SPEW
U32 GFXD3D8TextureObject::mTexCount = 0;
#endif

// FOR NOW JUST DEFINE THIS TO NORMAL ASSERT -patw
#ifdef POWER_DEBUG
#  define D3D8Assert(x, y) AssertFatal( SUCCEEDED(x), y );
#else
#  define D3D8Assert(x, y) x;
#endif

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
GFXD3D8TextureManager::GFXD3D8TextureManager( LPDIRECT3DDEVICE8 d3ddevice ) 
{
   mD3DDevice = d3ddevice;
   dMemset( mCurTexSet, 0, sizeof( mCurTexSet ) );
}

//-----------------------------------------------------------------------------
// innerCreateTexture
//-----------------------------------------------------------------------------
void GFXD3D8TextureManager::innerCreateTexture( GFXD3D8TextureObject *retTex, 
                                               U32 height, 
                                               U32 width, 
                                               U32 depth,
                                               GFXFormat format, 
                                               GFXTextureProfile *profile, 
                                               U32 numMipLevels,
                                               bool forceMips)
{   
   DWORD usage;   // 0, D3DUSAGE_RENDERTARGET, or D3DUSAGE_DYNAMIC
   D3DPOOL pool;  // D3DPOOL_DEFAULT or D3DPOOL_MANAGED

   bool supportsRGB24Textures = GFX->getCardProfiler()->queryProfile("allowRGB24BitTextures", false);

   // 24 bit textures not supported by most (or any?) cards - kick to 32
   if( format == GFXFormatR8G8B8 && !supportsRGB24Textures )
      format = GFXFormatR8G8B8X8;

   retTex->mProfile = profile;
   D3DFORMAT d3dTextureFormat = GFXD3D8TextureFormat[format];

   if( retTex->mProfile->isDynamic() )
   {
      usage = D3DUSAGE_DYNAMIC;
      pool = D3DPOOL_DEFAULT;
   }
   else
   {
      usage = 0;
      pool = D3DPOOL_MANAGED;
   }

   if( retTex->mProfile->isRenderTarget() )
   {
      usage |= D3DUSAGE_RENDERTARGET;
      pool = D3DPOOL_DEFAULT;
   }

   if( retTex->mProfile->isSystemMemory() )
   {
      pool = D3DPOOL_SYSTEMMEM;
   }

   // Set the managed flag...
   retTex->isManaged = (pool == D3DPOOL_MANAGED);
   
   if( depth > 0 )
   {
      D3D8Assert(
         GFXD3D8X.GFXD3D8XCreateVolumeTexture(
            mD3DDevice,
            width,
            height,
            depth,
            numMipLevels,
            usage,
            d3dTextureFormat,
            pool,
            retTex->get3DTexPtr()
         ), "GFXD3D8TextureManager::_createTexture - failed to create volume texture!"
      );

      retTex->mTextureSize.set( width, height, depth );
      retTex->mMipLevels = retTex->get3DTex()->GetLevelCount();
   }
   else
   {
      bool fastCreate = true;

      // Check for power of 2 textures - this is a problem with FX 5xxx cards
      // with current drivers - 3/2/05
      //if( !MathUtils::isPow2(width) || !MathUtils::isPow2(height) )
      //{
      //   fastCreate = false;
      //}

      // Try to create the texture directly - should gain us a bit in high
      // performance cases where we know we're creating good stuff and we
      // don't want to bother with D3DX - slow function.
      HRESULT res = D3DERR_INVALIDCALL;
      if( fastCreate )
      {
         res = mD3DDevice->CreateTexture(width, height, numMipLevels, usage, GFXD3D8TextureFormat[format], pool, retTex->get2DTexPtr() );
      }

      if( !fastCreate || (res != D3D_OK) )
      {
         D3D8Assert(
            GFXD3D8X.GFXD3D8XCreateTexture(
            mD3DDevice,
            width,
            height,
            numMipLevels,
            usage,
            d3dTextureFormat,
            pool,
            retTex->get2DTexPtr()
            ), "GFXD3D8TextureManager::_createTexture - failed to create texture!"
         );
      }

      // Get the actual size of the texture...
      D3DSURFACE_DESC probeDesc;
      D3D8Assert( retTex->get2DTex()->GetLevelDesc( 0, &probeDesc ), "Failed to get surface description");
      retTex->mTextureSize.set(probeDesc.Width, probeDesc.Height, 0);

      // All done!
      retTex->mMipLevels = retTex->get2DTex()->GetLevelCount();
   }

   retTex->mFormat = format;
}

//-----------------------------------------------------------------------------
// createTexture
//-----------------------------------------------------------------------------
GFXTextureObject *GFXD3D8TextureManager::_createTexture( U32 height, 
                                                        U32 width,
                                                        U32 depth,
                                                        GFXFormat format, 
                                                        GFXTextureProfile *profile, 
                                                        U32 numMipLevels,
                                                        bool forceMips)
{
   GFXD3D8TextureObject *retTex = new GFXD3D8TextureObject( GFX, profile );
   retTex->registerResourceWithDevice(GFX);

   innerCreateTexture(retTex, height, width, depth, format, profile, numMipLevels, forceMips);

   return retTex;
}

//-----------------------------------------------------------------------------
// loadTexture - GBitmap
//-----------------------------------------------------------------------------
bool GFXD3D8TextureManager::_loadTexture(GFXTextureObject *aTexture, GBitmap *pDLp)
{
   GFXD3D8TextureObject *texture = static_cast<GFXD3D8TextureObject*>(aTexture);

   // Check with profiler to see if we can do automatic mipmap generation.
   bool supportsAutoMips = GFX->getCardProfiler()->queryProfile("autoMipMapLevel", true);

   // Just in case we need to generate mips...
   GBitmap *pDL = pDLp;

   // We probably gotta gen mips on DX8...but don't do it if they don't want us to.
   // If it's a non-power-of-two texture, don't gen mips, because validate tex params 
   // told D3D that it only had 1 mip level.
   if( !supportsAutoMips && !aTexture->mProfile->noMip() && isPow2( aTexture->getHeight() ) && isPow2( aTexture->getWidth() ) )
   {
      // Gotta gen mipmaps ourselves, so pad this bitmap just in case it's non-POT
      pDL = pDLp->createPaddedBitmap();

      if( !pDL )
         pDL = pDLp;
      else
      {
         // Kill the old texture, and give them back the pointer for the new texture
         delete pDLp;
         pDLp = pDL;
      }
      pDL->extrudeMipLevels(false);
   }

   // Settings for mipmap generation
   U32 maxDownloadMip = pDL->getNumMipLevels();

   // Make sure we have a texture - we might be getting called in a resurrect

   // Fill the texture...
   U32 i = 0;
   while( i < maxDownloadMip )
   {
      U32 width = pDL->getWidth(i), height = pDL->getHeight(i);

      RECT rect;
      rect.left    = 0;
      rect.right   = width;
      rect.top     = 0;
      rect.bottom  = height;

      LPDIRECT3DSURFACE8 surf = NULL;
      D3D8Assert( texture->get2DTex()->GetSurfaceLevel( i, &surf ), "Failed to get surface");

      D3DLOCKED_RECT dstRect;

      if( SUCCEEDED( surf->LockRect( &dstRect, NULL, 0 ) ) )
      {
         POINT dstPoint;
         dstPoint.x = 0;
         dstPoint.y = 0;

         if( pDL->getFormat() == GFXFormatR8G8B8X8 || pDL->getFormat() == GFXFormatR8G8B8A8 )
            Swizzles::bgra.ToBuffer( dstRect.pBits,  pDL->getBits( i ), width * height * pDL->bytesPerPixel );
         else if( pDL->getFormat() == GFXFormatR8G8B8 && texture->mFormat != GFXFormatR8G8B8X8 ) // 24 bit textures allowed, we're good
            Swizzles::bgr.ToBuffer( dstRect.pBits, pDL->getBits( i ), width * height * pDL->bytesPerPixel );
         else if( pDL->getFormat() == GFXFormatR8G8B8 && texture->mFormat == GFXFormatR8G8B8X8 )
         {
            U8 *destBits = static_cast<U8 *>( dstRect.pBits );
            const U8 *srcBits = static_cast<const U8 *>( pDL->getBits( i ) );

            // 24-bit textures are _not_ allowed
            for( int px = 0; px < width * height; px++ )
            {
               Swizzles::bgr.ToBuffer( destBits, srcBits, 3 ); // 3bpp in a 24 bit rgb image
               destBits += 4; // 32-bit destination buffer
               srcBits += 3; // 24-bit source buffer
            }
         }
         else
            dMemcpy( dstRect.pBits, pDL->getBits( i ), width * height * pDL->bytesPerPixel );

         surf->UnlockRect();
      }

      surf->Release();
      ++i;
   }

   return true;          
}

//-----------------------------------------------------------------------------
// loadTexture - raw
//-----------------------------------------------------------------------------
bool GFXD3D8TextureManager::_loadTexture( GFXTextureObject *inTex, void *raw )
{
   GFXD3D8TextureObject *texture = (GFXD3D8TextureObject *) inTex;

   // currently only for volume textures...
   if( texture->getDepth() < 1 ) return false;

   
   U32 bytesPerPix = 0;

   switch( texture->mFormat )
   {
      case GFXFormatR8G8B8:
         bytesPerPix = 3;
         break;
      case GFXFormatR8G8B8A8:
      case GFXFormatR8G8B8X8:
         bytesPerPix = 4;
         break;
   }

   U32 rowPitch = texture->getWidth() * bytesPerPix;
   U32 slicePitch = texture->getWidth() * texture->getHeight() * bytesPerPix;

   D3DBOX box;
   box.Left    = 0;
   box.Right   = texture->getWidth();
   box.Front   = 0;
   box.Back    = texture->getDepth();
   box.Top     = 0;
   box.Bottom  = texture->getHeight();


   LPDIRECT3DVOLUME8 volume = NULL;
   D3D8Assert( texture->get3DTex()->GetVolumeLevel( 0, &volume ), "Failed to load volume" );


   D3D8Assert(
      GFXD3D8X.GFXD3D8XLoadVolumeFromMemory(
         volume,
         NULL,
         NULL,
         raw,
         GFXD3D8TextureFormat[texture->mFormat],
         rowPitch,
         slicePitch,
         NULL,
         &box,
         D3DX_FILTER_NONE,
         0
      ),
      "Failed to load volume texture" 
   );

   volume->Release();


   return true;
}

//-----------------------------------------------------------------------------
// refreshTexture
//-----------------------------------------------------------------------------
bool GFXD3D8TextureManager::_refreshTexture(GFXTextureObject *texture)
{
   U32 usedStrategies = 0;
   GFXD3D8TextureObject *realTex = static_cast<GFXD3D8TextureObject *>( texture );

   if(texture->mProfile->doStoreBitmap())
   {
//      SAFE_RELEASE(realTex->mD3DTexture);
//      innerCreateTexture(realTex, texture->mTextureSize.x, texture->mTextureSize.y, texture->mFormat, texture->mProfile, texture->mMipLevels);

      if(texture->mBitmap)
         _loadTexture(texture, texture->mBitmap);

      if(texture->mDDS)
         _loadTexture(texture, texture->mDDS);

      usedStrategies++;
   }

   if(texture->mProfile->isRenderTarget() || texture->mProfile->isDynamic())
   {
      realTex->release();
      innerCreateTexture(realTex, texture->getHeight(), texture->getWidth(), texture->getDepth(), texture->mFormat, texture->mProfile, texture->mMipLevels);
      usedStrategies++;
   }

   AssertFatal(usedStrategies < 2, "GFXD3D8TextureManager::_refreshTexture - Inconsistent profile flags!");

   return true;
}


//-----------------------------------------------------------------------------
// freeTexture
//-----------------------------------------------------------------------------
bool GFXD3D8TextureManager::_freeTexture(GFXTextureObject *texture, bool zombify)
{
   AssertFatal(dynamic_cast<GFXD3D8TextureObject *>(texture),"Not an actual d3d texture object!");
   GFXD3D8TextureObject *tex = static_cast<GFXD3D8TextureObject *>( texture );

   // If it's a managed texture and we're zombifying, don't blast it, D3D allows
   // us to keep it.
   if(zombify && tex->isManaged)
      return true;

   tex->release();

   return true;
}

//-----------------------------------------------------------------------------
// getTotalVideoMemory
//-----------------------------------------------------------------------------
U32 GFXD3D8TextureManager::_getTotalVideoMemory()
{
   int mem;
   NVDXDiagWrapper::DWORD numDevices;

   NVDXDiagWrapper::DXDiagNVUtil * dxDiagUtil = new NVDXDiagWrapper::DXDiagNVUtil();

   dxDiagUtil->InitIDxDiagContainer();
   dxDiagUtil->GetNumDisplayDevices(&numDevices);
   dxDiagUtil->GetDisplayDeviceMemoryInMB(0, &mem);
   dxDiagUtil->FreeIDxDiagContainer();

   delete dxDiagUtil;

   // It returns megs so scale up...
   return mem * 1024 * 1024;
}

/// Load a texture from a proper DDSFile instance.
bool GFXD3D8TextureManager::_loadTexture(GFXTextureObject *aTexture, DDSFile *dds)
{
   PROFILE_START(GFXD3DTexMan_loadTexture);

   GFXD3D8TextureObject *texture = static_cast<GFXD3D8TextureObject*>(aTexture);

   // Check with profiler to see if we can do automatic mipmap generation.
   bool supportsAutoMips = GFX->getCardProfiler()->queryProfile("autoMipMapLevel", true);

   // Settings for mipmap generation
   U32 maxDownloadMip = dds->mMipMapCount;
   U32 nbMipMapLevel  = dds->mMipMapCount;

   if(supportsAutoMips && false) // Off it goes!
   {
      maxDownloadMip = 1;
      nbMipMapLevel  = aTexture->mMipLevels;
   }

   // Fill the texture...
   U32 i = 0;
   while( i < maxDownloadMip)
   {
      PROFILE_START(GFXD3DTexMan_loadSurface);

      U32 width =  dds->getWidth(i);
      U32 height = dds->getHeight(i);

      RECT rect;
      rect.left    = 0;
      rect.right   = width;
      rect.top     = 0;
      rect.bottom  = height;

      LPDIRECT3DSURFACE8 surf = NULL;
      D3D8Assert(texture->get2DTex()->GetSurfaceLevel( i, &surf ), "Failed to get surface");

      D3DSURFACE_DESC desc;
      surf->GetDesc(&desc);

      D3D8Assert( GFXD3D8X.GFXD3D8XLoadSurfaceFromMemory( surf, NULL, NULL,
         (void*)((U8*)(dds->mSurfaces[0]->mMips[i])), 
         GFXD3D8TextureFormat[dds->mFormat], dds->getPitch(i),
         NULL, &rect, D3DX_FILTER_NONE, 0 ), "Failed to load surface" );

      surf->Release();
      ++i;

      PROFILE_END(GFXD3DTexMan_loadSurface);
   }

   PROFILE_END(GFXD3DTexMan_loadTexture);

   return true;
}
