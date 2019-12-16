//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _EFFECTDATA_H_
#define _EFFECTDATA_H_

#include "console/simBase.h"

class GFXEffect;

//**************************************************************************
// Effect data
//**************************************************************************
class EffectData : public SimObject
{
   typedef SimObject Parent;

protected:

   //--------------------------------------------------------------
   // Data
   //--------------------------------------------------------------

   GFXEffect*           mEffect;
   StringTableEntry     mDXEffectFile;
   StringTableEntry     mPoolName;
   bool                 mInitialized;

public:
   DECLARE_CONOBJECT( EffectData );

   //--------------------------------------------------------------
   // Procedures
   //--------------------------------------------------------------

   EffectData();

   GFXEffect* getEffect();

   static void initPersistFields();
};

#endif // _EFFECTDATA_H_
