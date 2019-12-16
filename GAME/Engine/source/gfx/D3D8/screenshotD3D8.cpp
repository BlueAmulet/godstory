//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3d8.h>
#include <d3dx8core.h>
#include <d3dx8tex.h>

#include "gfx/D3D8/gfxD3D8Device.h"

#include "gfx/D3D8/screenshotD3D8.h"
#include "core/fileStream.h"
#include "gui/core/guiCanvas.h"

//-----------------------------------------------------------------------------
// Capture standard screenshot - read it from the back buffer
//    This function jumps through some hoops copying surfaces around so that
//    the screenshot will work when using a multisample back buffer.
//-----------------------------------------------------------------------------
void ScreenShotD3D8::captureStandard()
{
   // TODO: Fix this -patw

   /*
   LPDIRECT3DDEVICE8 D3DDevice = dynamic_cast<GFXD3D8Device *>(GFX)->getDevice();

   Point2I size = GFX->getVideoMode().resolution;

   // set up the 2 copy surfaces
   GFXTexHandle tex[2];
   IDirect3DSurface8 *surface[2];
   
   tex[0].set( size.x, size.y, GFXFormatR8G8B8X8, &GFXDefaultRenderTargetProfile );
   tex[1].set( size.x, size.y, GFXFormatR8G8B8X8, &GFXSystemMemProfile );
   
   // grab the back buffer
   IDirect3DSurface8 * backBuffer;
   D3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer );

   // grab the top level surface of tex 0
   GFXD3D8TextureObject *to = (GFXD3D8TextureObject *) &(*tex[0]);
   D3D9Assert( to->get2DTex()->GetSurfaceLevel( 0, &surface[0] ), NULL );

   // use StretchRect because it allows a copy from a multisample surface
   // to a normal rendertarget surface
   D3DDevice->StretchRect( backBuffer, NULL, surface[0], NULL, D3DTEXF_NONE );

   // grab the top level surface of tex 1
   to = (GFXD3D8TextureObject *) &(*tex[1]);
   D3D9Assert( to->get2DTex()->GetSurfaceLevel( 0, &surface[1] ), NULL );

   // copy the data from the render target to the system memory texture
   D3DDevice->GetRenderTargetData( surface[0], surface[1] );
   
   // save it off
   D3DXIMAGE_FILEFORMAT format;
   
   if( dStrstr( (const char*)mFilename, ".jpg" ) )
   {
      format = D3DXIFF_JPG;
   }
   else
   {
      format = D3DXIFF_PNG;
   }
   
   D3DXSaveSurfaceToFile( mFilename, format, surface[1], NULL, NULL );

   // release the COM pointers
   surface[0]->Release();
   surface[1]->Release();
   backBuffer->Release();

   mPending = false;
   */
}
