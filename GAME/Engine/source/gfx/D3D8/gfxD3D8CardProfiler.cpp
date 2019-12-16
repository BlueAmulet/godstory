//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <d3d8.h>
#include <d3dx8math.h>
#include "gfx/D3D8/gfxD3D8Device.h"
#include "core/fileStream.h"
#include "console/console.h"
#include "gfx/primBuilder.h"
#include "platform/profiler.h"
#include "gfx/D3D8/gfxD3D8CardProfiler.h"

#define DUMMYDEF
#include "gfx/D3D/DXDiagNVUtil.h"

GFXD3D8CardProfiler::GFXD3D8CardProfiler()
{
   mVersionString = NULL;
   mVendorString  = NULL;
   mCardString    = NULL;
}

GFXD3D8CardProfiler::~GFXD3D8CardProfiler()
{
   if(mVersionString)
      delete[] mVersionString;

   if(mVendorString)
      delete (char*)mVendorString;

   if(mCardString)
      delete (char*)mCardString;
}

void GFXD3D8CardProfiler::init()
{
   mDevice = dynamic_cast<GFXD3D8Device*>(GFX);

   AssertISV(mDevice, "GFXD3D8CardProfiler::init() - No GFXD3D8Device found!");

   mD3DDevice = mDevice->getDevice();


   D3DDEVICE_CREATION_PARAMETERS p;
   mD3DDevice->GetCreationParameters(&p);

   mAdapterOrdinal = p.AdapterOrdinal;

   // get adapter ID
   IDirect3D8 *d3d;
   mD3DDevice->GetDirect3D(&d3d);
   D3DADAPTER_IDENTIFIER8 info;
   d3d->GetAdapterIdentifier(mAdapterOrdinal, 0, &info);
   d3d->Release();

   // Initialize the DxDiag interface...
   NVDXDiagWrapper::DXDiagNVUtil diag;
   diag.InitIDxDiagContainer();

   char foo[512];
   diag.GetDisplayDeviceManufacturer(mAdapterOrdinal, foo);
   mVendorString = dStrdup(foo);
   diag.GetDisplayDeviceChipSet(mAdapterOrdinal, foo);
   mCardString = dStrdup(foo);

/*   if(!dStricmp(mVendorString, "NVIDIA") || !dStricmp(mVendorString, "ATI"))
   { */
      // Get the actual driver revision number then, if we can...
      mVersionString = new char[128];
      F32 version;
      diag.GetDisplayDeviceNVDriverVersion(mAdapterOrdinal, &version );
      dSprintf(mVersionString, 128, "%.2f", version);
/*   }
   else
   {
      // Stick the whole version in there
      diag.GetDisplayDeviceDriverVersionStr(mAdapterOrdinal, foo);
      mVersionString = dStrdup(foo);
   } */


   diag.FreeIDxDiagContainer();

   Parent::init();
}

const char* GFXD3D8CardProfiler::getVersionString()
{
   return mVersionString;
}

const char* GFXD3D8CardProfiler::getCardString()
{
   return mCardString;
}

const char* GFXD3D8CardProfiler::getVendorString()
{
   return mVendorString;
}

const char* GFXD3D8CardProfiler::getRendererString()
{
   return "D3D8";
}

void GFXD3D8CardProfiler::setupCardCapabilities()
{
   // Get the D3D device caps
   D3DCAPS8 caps;
   mD3DDevice->GetDeviceCaps(&caps);

   setCapability( dStrdup( "autoMipMapLevel" ), 0 );

   setCapability( dStrdup( "maxTextureWidth" ), caps.MaxTextureWidth );
   setCapability( dStrdup( "maxTextureHeight" ), caps.MaxTextureHeight );
   setCapability( dStrdup( "maxTextureSize" ), getMin( (U32)caps.MaxTextureWidth, (U32)caps.MaxTextureHeight) );

   bool canDoLERPDetailBlend = ( caps.TextureOpCaps & D3DTEXOPCAPS_LERP ) && ( caps.MaxTextureBlendStages > 1 );

   bool canDoFourStageDetailBlend = ( caps.TextureOpCaps & D3DTEXOPCAPS_SUBTRACT ) &&
      ( caps.PrimitiveMiscCaps & D3DPMISCCAPS_TSSARGTEMP ) &&
      ( caps.MaxTextureBlendStages > 3 );

   setCapability( dStrdup( "lerpDetailBlend" ), canDoLERPDetailBlend );
   setCapability( dStrdup( "fourStageDetailBlend" ), canDoFourStageDetailBlend );
}

bool GFXD3D8CardProfiler::_queryCardCap(const char *query, U32 &foundResult)
{
   return 0;
}
