//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "console/console.h"
#include "lightingSystem/synapseGaming/shadowCasterManager.h"
#include "lightingSystem/synapseGaming/shadowCaster.h"
#include "lightingSystem/synapseGaming/sgShadowTextureCache.h"
#include "lightingSystem/common/blobShadow.h"

const U32 SHADOWCASTER_UNUSED_TIMEOUT = 2000;

U32 ShadowCasterManager::smLastCleanup = 0;
Vector<ShadowCaster* > ShadowCasterManager::smShadowCasters;

void ShadowCasterManager::registerShadowCaster(ShadowCaster* shadowCaster)
{
   // If we've already registered, ignore
   for(U32 i = 0; i < smShadowCasters.size(); i++)
   {
      if(smShadowCasters[i] == shadowCaster)
      {
         return;
      }
   }

   smShadowCasters.push_back(shadowCaster);
}

void ShadowCasterManager::unregisterShadowCaster(ShadowCaster* shadowCaster)
{
   // Loop through the vector, find the shadow caster we don't want, remove it.
   for(S32 i = smShadowCasters.size()-1; i >=0 ; i--)
   {
      if(smShadowCasters[i] == shadowCaster)
      {
         smShadowCasters.erase_fast(i);
      }
   }
}

void ShadowCasterManager::cleanupUnusedShadows()
{
   // Current time
   U32 time = Platform::getRealMilliseconds();

   if(time <= smLastCleanup)
      return;

   // We don't want to do this continuously
   if((time - smLastCleanup) < SHADOWCASTER_UNUSED_TIMEOUT)
      return;

   // Loop through all registered shadow casters, tell them to clean up.
   for(U32 i=0; i<smShadowCasters.size(); i++)
      smShadowCasters[i]->cleanupUnusedShadows(time);

   smLastCleanup = time;
}

void ShadowCasterManager::clearAllShadows()
{
   // Loop through all shadow casters, tell them to nuke existing shadows
   for(U32 i = 0; i < smShadowCasters.size(); i++)
   {
      smShadowCasters[i]->clearAllShadows();
   }
   sgShadowTextureCache::sgClear();
   BlobShadow::deleteGenericShadowBitmap();
}

ConsoleFunction(clearAllShadows, void, 1, 1, "Deletes all shadows")
{
   ShadowCasterManager::clearAllShadows();
}
