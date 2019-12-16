//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "materials/sceneData.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"

sgLightInfo::sgLightInfo() : LightInfo()
{
   // default to this for weapon fire, explosions, ...
   sgSupportedFeatures = sgNoSpecCube;
   sgAssignedToTSObject = false;
   sgCastsShadows = true;
   sgDiffuseRestrictZone = false;
   sgAmbientRestrictZone = false;
   sgZone[0] = -1;
   sgZone[1] = -1;
   sgLocalAmbientAmount = 0.0;
   sgSmoothSpotLight = false;
   sgDoubleSidedAmbient = false;
   sgAssignedToParticleSystem = false;
   sgLightingModelName = NULL;
   sgUseNormals = true;
   sgSpotPlane = PlaneF(0, 0, 0, 0);
   sgLightMask = NULL;
   sgLightingTransform.identity();
}

sgLightInfo::~sgLightInfo()
{
}

bool sgLightInfo::sgIsInZone(S32 zone)
{
   if((zone == sgZone[0]) || (zone ==sgZone[1]))
      return true;
   return false;
}

bool sgLightInfo::sgAllowDiffuseZoneLighting(S32 zone)
{
   if(!sgDiffuseRestrictZone)
      return true;
   if(sgIsInZone(zone))
      return true;
   return false;
}

// Copies the data from a light
void sgLightInfo::set(LightInfo* light)
{
   sgLightInfo* sourceLight = dynamic_cast<sgLightInfo*>(light);
   if (sourceLight)
   {
      mType = sourceLight->mType;
      mPos = sourceLight->mPos;
      mDirection = sourceLight->mDirection;
      mColor = sourceLight->mColor;
	  mShadowColor = sourceLight->mShadowColor;
	  mDynamicShadowedColor = sourceLight->mDynamicShadowedColor;
      mAmbient = sourceLight->mAmbient;
	  mObjAmbient = sourceLight->mObjAmbient;
      mBackColor = sourceLight->mBackColor;
      mRadius = sourceLight->mRadius;
      mScore = sourceLight->mScore;
      mReceiveLMLighting = sourceLight->mReceiveLMLighting;
      sgSupportedFeatures = sourceLight->sgSupportedFeatures;
      sgSpotAngle = sourceLight->sgSpotAngle;
      sgAssignedToTSObject = sourceLight->sgAssignedToTSObject;
      sgCastsShadows = sourceLight->sgCastsShadows;
      sgDiffuseRestrictZone = sourceLight->sgDiffuseRestrictZone;
      sgAmbientRestrictZone = sourceLight->sgAmbientRestrictZone;
      sgZone[0] = sourceLight->sgZone[0];
      sgZone[1] = sourceLight->sgZone[1];
      sgLocalAmbientAmount = sourceLight->sgLocalAmbientAmount;
      sgSmoothSpotLight = sourceLight->sgSmoothSpotLight;
      sgDoubleSidedAmbient = sourceLight->sgDoubleSidedAmbient;
      sgAssignedToParticleSystem = sourceLight->sgAssignedToParticleSystem;
      sgLightingModelName = sourceLight->sgLightingModelName;
      sgUseNormals = sourceLight->sgUseNormals;
      sgSpotPlane = sourceLight->sgSpotPlane;
      sgLightMask = sourceLight->sgLightMask;
      sgLightingTransform = sourceLight->sgLightingTransform;
   }
}

// Sets up a GFX fixed function light
void sgLightInfo::setGFXLight(GFXLightInfo* light)
{
   switch (mType) {
      case LightInfo::SGStaticPoint :
      case LightInfo::Point :
         light->mType = GFXLightInfo::Point;
         break;
      case LightInfo::Spot :
      case LightInfo::SGStaticSpot :
         light->mType = GFXLightInfo::Spot;
         break;
      case LightInfo::Vector:
         light->mType = GFXLightInfo::Vector;
         break;
      case LightInfo::Ambient:
         light->mType = GFXLightInfo::Ambient;
         break;
   }
   light->mPos = mPos;
   light->mDirection = mDirection;
   light->mColor = mColor;
   light->mAmbient = mAmbient;
   light->mRadius = mRadius;
   light->sgSpotAngle = sgSpotAngle;
}

// Sets values in Scene Graph Data before rendering
void sgLightInfo::setSceneGraphData(SceneGraphData* sgd)
{
   // get the model...
   sgLightingModel &lightingmodel = sgLightingModelManager::sgGetLightingModel(sgLightingModelName);
   lightingmodel.sgSetState(this);

   // get the info...
   F32 maxrad = lightingmodel.sgGetMaxRadius(true);
   sgTempModelInfo[0] = 0.5f / maxrad;

   // get the dynamic lighting texture...
   if((mType == LightInfo::Spot) || (mType == LightInfo::SGStaticSpot))
      sgd->dynamicLight = lightingmodel.sgGetDynamicLightingTextureSpot();
   else
      sgd->dynamicLight = lightingmodel.sgGetDynamicLightingTextureOmni();
   // reset the model...
   lightingmodel.sgResetState();

   sgd->light = this;
}

F32 sgLightInfo::getRadius()
{
   // get the model...
   sgLightingModel &lightingmodel = sgLightingModelManager::sgGetLightingModel(this);
   // get the info...
   F32 maxrad = lightingmodel.sgGetMaxRadius(true);
   sgTempModelInfo[0] = 0.5f / maxrad;
   lightingmodel.sgResetState();
   return maxrad;
}