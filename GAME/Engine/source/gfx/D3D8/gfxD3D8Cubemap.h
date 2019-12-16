//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXD3D8CUBEMAP_H_
#define _GFXD3D8CUBEMAP_H_

#include "gfx/gfxCubemap.h"
#include "gfx/gfxResource.h"

//**************************************************************************
// Cube map
//**************************************************************************

// Forward Declarations
struct IDirect3DCubeTexture8;
struct IDirect3DSurface8;
struct IDirect3DDevice8;
class GFXD3D8TextureTarget;

class GFXD3D8Cubemap : public GFXCubemap
{
   friend class GFXD3D8TextureTarget;

   IDirect3DCubeTexture8  *mCubeTex;
   IDirect3DSurface8      *mDepthBuff;

   static _D3DCUBEMAP_FACES faceList[6];
   bool mInit;
   bool mDynamic;
   U32  mTexSize;
   S32  mCallbackHandle;
   
   void fillCubeTextures( GFXTexHandle *faces, IDirect3DDevice8 *D3DDevice );
   void releaseSurfaces();
   static void texManagerCallback( GFXTexCallbackCode code, void *userData );

public:
   virtual void initStatic( GFXTexHandle *faces );
   virtual void initDynamic( U32 texSize );
   virtual void setToTexUnit( U32 tuNum );
   virtual void updateDynamic( const Point3F &pos );

   GFXD3D8Cubemap();
   virtual ~GFXD3D8Cubemap();

   // GFXResource interface
   virtual void zombify();
   virtual void resurrect();
};

#endif // GFXD3D8CUBEMAP
