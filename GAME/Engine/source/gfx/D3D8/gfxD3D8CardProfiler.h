//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXD3D8CARDPROFILER_H_
#define _GFXD3D8CARDPROFILER_H_

#include "gfx/gfxCardProfile.h"

class GFXD3D8CardProfiler : public GFXCardProfiler
{
private:
   typedef GFXCardProfiler Parent;

   GFXD3D8Device *mDevice;

   IDirect3DDevice8 *mD3DDevice;
   UINT mAdapterOrdinal;

   char * mVersionString;
   const char * mVendorString;
   char * mCardString;

public:
   GFXD3D8CardProfiler();
   ~GFXD3D8CardProfiler();
   void init();

protected:
   const char* getVersionString();
   const char* getCardString();
   const char* getVendorString();
   const char* getRendererString();
   void setupCardCapabilities();
   bool _queryCardCap(const char *query, U32 &foundResult);
};

#endif
