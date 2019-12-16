//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgMatInstance.h"
#include "terrain/terrRender.h"

class sgTerrainLightingPlugin : public TerrainLightingPlugin
{
public:
   sgTerrainLightingPlugin();
   void setupLightStage(LightInfo* light, SceneGraphData& sgData, MatInstance* basemat, MatInstance** dmat);
   void registerPlugin(bool active);
};

sgTerrainLightingPlugin p_sgTerrainLightingPlugin;

//
// sgTerrainLightingPlugin
//
sgTerrainLightingPlugin::sgTerrainLightingPlugin()
{
   SGLM->mSgActivate.notify(this, &sgTerrainLightingPlugin::registerPlugin);
}

void sgTerrainLightingPlugin::registerPlugin(bool active)
{
   if (active)
      TerrainRender::mTerrainLighting = this;
   else
      TerrainRender::mTerrainLighting = NULL;
}

void sgTerrainLightingPlugin::setupLightStage(LightInfo* light, SceneGraphData& sgData, MatInstance* basemat, MatInstance** dmat)
{
   SG_CHECK_LIGHT(light);
   sgLightInfo* sgLight = static_cast<sgLightInfo*>(light);
   // Get the best dynamic lighting material for this light.
   *dmat = SGLM->getDynamicLightingMaterial(basemat, sgLight, false);
   if(!dmat)
      return;

   // Get our lighting model.
   sgLightingModel &lightingmodel = sgLightingModelManager::sgGetLightingModel(light);   

   // The sgTempModelInfo has been calced in buildLightArray already

   // Get the dynamic lighting texture.
   if((light->mType == LightInfo::Spot) || (light->mType == LightInfo::SGStaticSpot))
      sgData.dynamicLight = lightingmodel.sgGetDynamicLightingTextureSpot();
   else
      sgData.dynamicLight = lightingmodel.sgGetDynamicLightingTextureOmni();

   // Reset the lighting model.
   lightingmodel.sgResetState();

   // Make the light available via the scenegraphdata.
   sgData.light = light;   
}