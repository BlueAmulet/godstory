//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GFXD3D8TextureObject_H_
#define _GFXD3D8TextureObject_H_

#include "gfx/gfxTextureManager.h"
#include "gfx/gfxTextureHandle.h"

//*****************************************************************************
// GFX D3D Texture Object
//*****************************************************************************
struct IDirect3DBaseTexture8;
struct IDirect3DTexture8;
struct IDirect3DVolumeTexture8;

class GFXD3D8TextureObject : public GFXTextureObject
{
private:
   static U32 mTexCount;
   GFXTexHandle mLockTex;     // used to copy render target data to
   D3DLOCKED_RECT mLockRect;
   bool         mLocked;

   IDirect3DBaseTexture8 * mD3DTexture;  // could be 2D or volume tex (possibly cube tex in future)

public:

   GFXD3D8TextureObject( GFXDevice * d, GFXTextureProfile *profile);
   ~GFXD3D8TextureObject();

   IDirect3DBaseTexture8 *    getTex(){ return mD3DTexture; }
   IDirect3DTexture8 *        get2DTex(){ return (IDirect3DTexture8*) mD3DTexture; }
   IDirect3DTexture8 **       get2DTexPtr(){ return (IDirect3DTexture8**) &mD3DTexture; }
   IDirect3DVolumeTexture8 *  get3DTex(){ return (IDirect3DVolumeTexture8*) mD3DTexture; }
   IDirect3DVolumeTexture8 ** get3DTexPtr(){ return (IDirect3DVolumeTexture8**) &mD3DTexture; }

   void release();

   bool isManaged;

   virtual GFXLockedRect * lock( U32 mipLevel = 0, RectI *inRect = NULL );
   virtual void unlock( U32 mipLevel = 0);

   virtual bool copyToBmp(GBitmap* bmp);
   virtual bool readBackBuffer( Point2I &upperLeft );

#ifdef POWER_DEBUG
   virtual void pureVirtualCrash() {};
#endif

   // GFXResource interface
   virtual void zombify();
   virtual void resurrect();
};


#endif
