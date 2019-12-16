//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFX_D3D8_Texture_Manager_H_
#define _GFX_D3D8_Texture_Manager_H_

#include "console/console.h"

#include "gfx/gfxTextureManager.h"
#include "gfx/D3D8/gfxD3D8TextureObject.h"

// #define D3D_TEXTURE_SPEW

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) if( (a) != NULL ) (a)->Release(); (a) = NULL;
#endif


//*****************************************************************************
// GFX D3D Texture Manager
//*****************************************************************************
struct IDirect3DDevice8;

class GFXD3D8TextureManager : public GFXTextureManager 
{
   friend class GFXD3D8TextureObject;

   U32 mCurTexSet[TEXTURE_STAGE_COUNT];
   
   private:
      IDirect3DDevice8 *mD3DDevice;
      void innerCreateTexture(GFXD3D8TextureObject *obj, U32 height, U32 width, U32 depth, GFXFormat format, GFXTextureProfile *profile, U32 numMipLevels, bool forceMips = false);
 
   protected:
      GFXTextureObject *_createTexture(U32 height, U32 width, U32 depth, GFXFormat format, GFXTextureProfile *profile, U32 numMipLevels, bool forceMips = false);
      bool _loadTexture(GFXTextureObject *texture, DDSFile *dds);
      bool _loadTexture(GFXTextureObject *texture, GBitmap *bmp);
      bool _loadTexture(GFXTextureObject *texture, void *raw);
      bool _refreshTexture(GFXTextureObject *texture);
      bool _freeTexture(GFXTextureObject *texture, bool zombify = false);
      U32 _getTotalVideoMemory();

   public:
      GFXD3D8TextureManager( IDirect3DDevice8 *d3ddevice );

};

#endif
