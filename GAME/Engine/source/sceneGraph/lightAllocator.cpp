//-----------------------------------------------------------------------------
// PowerEngine Shader Engine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "sceneGraph/lightAllocator.h"
#include "sceneGraph/lightManager.h"
#include "sceneGraph/lightInfo.h"


LightAllocator::LightAllocator()
{
}

LightAllocator::~LightAllocator()
{
   clear();
}

void LightAllocator::clear()
{
   free();

   for ( S32 i = 0; i < mFreeLights.size(); i++ )
      delete mFreeLights[i];

   mFreeLights.clear();
}

void LightAllocator::free()
{
   mFreeLights.merge( mAllocLights );
   mAllocLights.clear();
}

void LightAllocator::reserve( LightManager* lm, S32 count )
{
   const S32 reserved = mFreeLights.size();
   if ( reserved >= count )
      return;

   mFreeLights.setSize( count );
   for ( S32 i = reserved; i < count; i++ )
      mFreeLights[i] = lm->createLightInfo();
}

LightInfo* LightAllocator::alloc( LightManager* lm )
{
   LightInfo* light;
   
   if ( !mFreeLights.empty() )
   {
      light = mFreeLights.last();
      mFreeLights.pop_back();
   }
   else
      light = lm->createLightInfo();

   mAllocLights.push_back( light );     
   return light;
}
