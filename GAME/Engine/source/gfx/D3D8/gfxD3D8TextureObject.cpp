//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3d8.h>
#include <d3dx8.h>
#include "gfx/D3D8/GFXD3D8TextureObject.h"
#include "gfx/D3D8/gfxD3D8Device.h"

#include "platform/profiler.h"

U32 GFXD3D8TextureObject::mTexCount = 0;

//*****************************************************************************
// GFX D3D Texture Object
//*****************************************************************************
GFXD3D8TextureObject::GFXD3D8TextureObject( GFXDevice * d, GFXTextureProfile *profile)
                                        : GFXTextureObject( d, profile )
{
#ifdef D3D_TEXTURE_SPEW
   mTexCount++;
   Con::printf("+ texMake %d %x", mTexCount, this);
#endif

   isManaged = false;
   mD3DTexture = NULL;
   mLocked = false;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GFXD3D8TextureObject::~GFXD3D8TextureObject()
{
   kill();
#ifdef D3D_TEXTURE_SPEW
   mTexCount--;
   Con::printf("+ texkill %d %x", mTexCount, this);
#endif
}

//-----------------------------------------------------------------------------
// lock
//-----------------------------------------------------------------------------
GFXLockedRect *GFXD3D8TextureObject::lock(U32 mipLevel /*= 0*/, RectI *inRect /*= NULL*/)
{
   if( mProfile->isRenderTarget() )
   {
      if( !mLockTex || 
          mLockTex->getWidth() != getWidth() ||
          mLockTex->getHeight() != getHeight() )
      {
         mLockTex.set( getWidth(), getHeight(), mFormat, &GFXSystemMemProfile );
      }

      PROFILE_START(GFXD3D8TextureObject_lock);

      IDirect3DSurface8 *source;
      D3D8Assert( get2DTex()->GetSurfaceLevel( 0, &source ), NULL );
      
      IDirect3DSurface8 *dest;
      GFXD3D8TextureObject *to = (GFXD3D8TextureObject *) &(*mLockTex);
      D3D8Assert( to->get2DTex()->GetSurfaceLevel( 0, &dest ), NULL );

      LPDIRECT3DDEVICE8 D3DDevice = dynamic_cast<GFXD3D8Device *>(GFX)->getDevice();
      D3D8Assert( D3DDevice->CopyRects( source, NULL, 0, dest, NULL ), NULL );

      source->Release();

      D3D8Assert( dest->LockRect( &mLockRect, NULL, D3DLOCK_READONLY ), NULL );
      dest->Release();
      mLocked = true;

      PROFILE_END(GFXD3D8TextureObject_lock);
   }
   else
   {
      RECT r;

      if(inRect)
      {
         r.top  = inRect->point.y;
         r.left = inRect->point.x;
         r.bottom = inRect->point.y + inRect->extent.y;
         r.right  = inRect->point.x + inRect->extent.x;
      }

      D3D8Assert( get2DTex()->LockRect(mipLevel, &mLockRect, inRect ? &r : NULL, 0), 
         "GFXD3D8TextureObject::lock - could not lock non-RT texture!" );
      mLocked = true;

   }

   // GFXLockedRect is set up to correspond to D3DLOCKED_RECT, so this is ok.
   return (GFXLockedRect*)&mLockRect; 
}
   
//-----------------------------------------------------------------------------
// unLock
//-----------------------------------------------------------------------------
void GFXD3D8TextureObject::unlock(U32 mipLevel)
{
   AssertFatal( mLocked, "Attempting to unlock a surface that has not been locked" );

   if( mProfile->isRenderTarget() )
   {
      IDirect3DSurface8 *dest;
      GFXD3D8TextureObject *to = (GFXD3D8TextureObject *) &(*mLockTex);
      D3D8Assert( to->get2DTex()->GetSurfaceLevel( 0, &dest ), NULL );

      dest->UnlockRect();
      dest->Release();

      mLocked = false;
   }
   else
   {
      D3D8Assert( get2DTex()->UnlockRect(mipLevel), 
         "GFXD3DTextureObject::unlock - could not unlock non-RT texture." );

      mLocked = false;
   }

}

//-----------------------------------------------------------------------------
// release
//-----------------------------------------------------------------------------
void GFXD3D8TextureObject::release()
{
   if( mD3DTexture != NULL )
   {
      mD3DTexture->Release();
      mD3DTexture = NULL;
   }
}

//-----------------------------------------------------------------------------
// copyToBmp
//-----------------------------------------------------------------------------
bool GFXD3D8TextureObject::copyToBmp(GBitmap* bmp)
{
   // profiler?
   AssertFatal(bmp, "copyToBmp: null bitmap specified");
   if (!bmp)
      return false;

   AssertFatal(mProfile->isRenderTarget(), "copyToBmp: this texture is not a render target");
   if (!mProfile->isRenderTarget())
      return false;

   // check format limitations
   // at the moment we only support RGBA for the source (other 4 byte formats should
   // be easy to add though)
   AssertFatal(mFormat == GFXFormatR8G8B8A8, "copyToBmp: invalid format");
   if (mFormat != GFXFormatR8G8B8A8)
      return false;

   PROFILE_START(GFXD3D8TextureObject_copyToBmp);

   AssertFatal(bmp->getWidth() == getWidth(), "doh");
   AssertFatal(bmp->getHeight() == getHeight(), "doh");
   U32 width = getWidth();
   U32 height = getHeight();

   // set some constants
   const U32 sourceBytesPerPixel = 4;
   U32 destBytesPerPixel = 0;
   if (bmp->getFormat() == GFXFormatR8G8B8A8)
      destBytesPerPixel = 4;
   else if (bmp->getFormat() == GFXFormatR8G8B8)
      destBytesPerPixel = 3;
   else
      // unsupported
      AssertFatal(false, "unsupported bitmap format");

   // lock the texture
   D3DLOCKED_RECT* lockRect = (D3DLOCKED_RECT*) lock();

   // set pointers
   U8* srcPtr = (U8*)lockRect->pBits;
   U8* destPtr = bmp->getWritableBits();

   // we will want to skip over any D3D cache data in the source texture
   const S32 sourceCacheSize = lockRect->Pitch - width * sourceBytesPerPixel;
   AssertFatal(sourceCacheSize >= 0, "copyToBmp: cache size is less than zero?");

   PROFILE_START(GFXD3D8TextureObject_copyToBmp_pixCopy);
   // copy data into bitmap
   for (int row = 0; row < height; ++row)
   {
      for (int col = 0; col < width; ++col)
      {
         destPtr[0] = srcPtr[2]; // red
         destPtr[1] = srcPtr[1]; // green
         destPtr[2] = srcPtr[0]; // blue 
         if (destBytesPerPixel == 4)
            destPtr[3] = srcPtr[3]; // alpha

         // go to next pixel in src
         srcPtr += sourceBytesPerPixel;

         // go to next pixel in dest
         destPtr += destBytesPerPixel;
      }
      // skip past the cache data for this row (if any)
      srcPtr += sourceCacheSize;
   }
   PROFILE_END(GFXD3D8TextureObject_copyToBmp_pixCopy);

   // assert if we stomped or underran memory
   AssertFatal(U32(destPtr - bmp->getWritableBits()) == width * height * destBytesPerPixel, "copyToBmp: doh, memory error");
   AssertFatal(U32(srcPtr - (U8*)lockRect->pBits) == height * lockRect->Pitch, "copyToBmp: doh, memory error");

   // unlock
   unlock();

   PROFILE_END(GFXD3D8TextureObject_copyToBmp);

   return true;
}

//------------------------------------------------------------------------------

bool GFXD3D8TextureObject::readBackBuffer( Point2I &upperLeft )
{
   AssertFatal( mProfile->isRenderTarget(), "readBackBuffer: this texture is not a render target" );
   if ( !mProfile->isRenderTarget() )
      return false;

   GFXD3D8Device *device = static_cast<GFXD3D8Device *>( mDevice );

   IDirect3DSurface8 *backBuffer;
   device->mD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer );

   IDirect3DSurface8 *surf;

   // TODO: Find out why this doesn't work correctly -pw
   RECT rect;
   rect.top = upperLeft.y;
   rect.left = upperLeft.x;
   rect.bottom = rect.top + getHeight();
   rect.right = rect.left + getWidth();

   get2DTex()->GetSurfaceLevel( 0, &surf );
   device->getDevice()->CopyRects( backBuffer, NULL, 0, surf, NULL );

   surf->Release();
   backBuffer->Release();

   return true;
}

void GFXD3D8TextureObject::zombify()
{
   AssertFatal(!mLocked, "GFXD3D8TextureObject::zombify - Cannot zombify a locked texture!");
   if(isManaged)
      return;

   release();
}

void GFXD3D8TextureObject::resurrect()
{
   if(isManaged)
      return;

   static_cast<GFXD3D8TextureManager*>(GFX->getTextureManager())->refreshTexture(this);
}
