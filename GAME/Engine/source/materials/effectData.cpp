//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "materials/effectData.h"
#include "console/consoleTypes.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxEffect.h"
#include "gfx/gfxEffectMgr.h"
#include "console/simFieldDictionary.h"

IMPLEMENT_CONOBJECT( EffectData );

EffectData::EffectData()
{
   mDXEffectFile = NULL;
   mEffect = NULL;
   mPoolName = NULL;
   mInitialized = false;
}

void EffectData::initPersistFields()
{
   Parent::initPersistFields();

   addField( "DXEffectFile", TypeString, Offset( mDXEffectFile, EffectData ) );
   addField( "Pool", TypeString, Offset( mPoolName, EffectData ) );
}

GFXEffect* EffectData::getEffect()
{
   if( !mInitialized )
   {
      GFXEffectMgr* manager = GFX->getEffectManager();
      if( manager )
      {
         const char* poolName = NULL;
         if( mPoolName && mPoolName[ 0 ] != '\0' )
            poolName = mPoolName;

         GFXEffect* effect = manager->createEffect( mDXEffectFile, poolName );
         if( effect )
         {
            //TODO: Set the effect's parameters from our dynamic fields.

            // All done.
            mEffect = effect;
         }
      }

      mInitialized = true;
   }

   return mEffect;
}
