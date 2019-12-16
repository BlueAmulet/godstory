//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFX_D3D9_OCCLUSIONQUERY_H_
#define _GFX_D3D9_OCCLUSIONQUERY_H_

#include "gfx/gfxResource.h"
class RectI;
class GFXD3D9OcclusionQuery : public GFXResource
{
private:
   GFXDevice *pGFXDevice;
   LPDIRECT3DDEVICE9 d3dDevice;
   LPDIRECT3DQUERY9 d3dQuery;
   LPDIRECT3DTEXTURE9 occlusionTexture;
   IDirect3DSurface9 *backBuff;
   RectI *rect;
   bool canQuery;
public:
   GFXD3D9OcclusionQuery( GFXDevice *device )
   {
	   pGFXDevice = device;
	   d3dDevice = dynamic_cast<GFXD3D9Device *>(device)->getDevice();
	   d3dQuery = NULL;
	   backBuff = NULL;
	   occlusionTexture = NULL;
	   rect = NULL;
	   canQuery = false;
	   registerResourceWithDevice(device);
   };

   virtual ~GFXD3D9OcclusionQuery();

   bool initialize();
   bool begin();
   bool end();
   int query();

   void zombify();
   void resurrect();
   void describeSelf( char* buffer, U32 sizeOfBuffer )
   {
	   buffer[0] = NULL;
   }
};

#endif