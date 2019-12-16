//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CUBEMAPDATA_H_
#define _CUBEMAPDATA_H_

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif

#include "gfx/gfxTextureHandle.h"

#ifndef _GFXCUBEMAP_H_
#include "gfx/gfxCubemap.h"
#endif

//**************************************************************************
// Cubemap data
//**************************************************************************
class CubemapData : public SimObject
{
   typedef SimObject Parent;

   //--------------------------------------------------------------
   // Data
   //--------------------------------------------------------------
public:

   GFXCubemapHandle  cubemap;
   StringTableEntry  cubeFaceFile[6];
   GFXTexHandle      cubeFace[6];
   bool              dynamic;

   //--------------------------------------------------------------
   // Procedures
   //--------------------------------------------------------------
public:
   CubemapData();
   ~CubemapData();

   bool onAdd();
   static void initPersistFields();

   void createMap();

   DECLARE_CONOBJECT(CubemapData);
};



#endif // CUBEMAPDATA

