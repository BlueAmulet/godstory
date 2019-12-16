//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgSceneLighting.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLightMap.h"

sgSceneLighting::sgSceneLighting(AvailableSLInterfaces* lightingInterfaces) : SceneLighting(lightingInterfaces)
{

}

void sgSceneLighting::sgLightingStartEvent()
{   
   Con::printf("");
   Con::printf("//-----------------------------------------------");
   Con::printf("Synapse Gaming Lighting Pack");

   // clear static mesh shadow data...
   sgShadowObjects::sgClearStaticMeshBVPTData();

   Parent::sgLightingStartEvent();
}

void sgSceneLighting::sgLightingCompleteEvent()
{
   // clear static mesh shadow data...
   sgShadowObjects::sgClearStaticMeshBVPTData();

   Parent::sgLightingCompleteEvent();
}

void sgSceneLighting::getMLName(const char* misName, const U32 missionCRC, const U32 buffSize, char* filenameBuffer)
{
   if(sgLightManager::sgAllowFullLightMaps())
      dSprintf(mFileName, sizeof(mFileName), "%s_%xsg.ml", misName, missionCRC);
   else
      dSprintf(mFileName, sizeof(mFileName), "%s_%x-sgraw.ml", misName, missionCRC);
}

void sgSceneLighting::sgSGPassSetupEvent()
{
   Con::printf("  Starting Synapse Gaming Lighting Pack scene lighting..."); 
   Parent::sgSGPassSetupEvent();
   // stats...
   sgStatistics::sgClear();
   sgStatistics::sgInteriorObjectCount += mLitObjects.size();
}

void sgSceneLighting::sgSGObjectCompleteEvent( S32 object )
{
   if(object >= mLitObjects.size())
   {
      Con::printf("  Synapse Gaming Lighting Pack scene lighting complete (%3.3f seconds)", (Platform::getRealMilliseconds()-sgTimeTemp2)/1000.f);
      // stats...
      sgStatistics::sgPrint();
   }
   Parent::sgSGObjectCompleteEvent(object);
}
