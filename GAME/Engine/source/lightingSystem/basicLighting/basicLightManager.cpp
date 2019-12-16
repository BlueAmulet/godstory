//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "lightingSystem/basicLighting/basicLightManager.h"
#include "lightingSystem/basicLighting/basicLightInfo.h"
#include "gfx/gfxDevice.h"
#include "materials/processedMaterial.h"
#include "materials/sceneData.h"
#include "shaderGen/shaderFeature.h"
#include SHADER_CONSTANT_INCLUDE_FILE
#include "lightingSystem/common/sceneLighting.h"
#include "sceneGraph/lightingInterfaces.h"

BasicLightManager* BasicLightManager::smBasicLightManagerSingleton = NULL;

BasicLightManager::BasicLightManager()
{
   mDefaultLight = createBasicLightInfo();
   mTranslatedSun = createBasicLightInfo();
   mLastLightSentToShader = NULL;
   dMemset(&mSpecialLights, 0, sizeof(mSpecialLights));
   mLightingInterfaces = new AvailableSLInterfaces();
}

BasicLightManager::~BasicLightManager()
{
   SAFE_DELETE(mDefaultLight);
   SAFE_DELETE(mTranslatedSun);
   SAFE_DELETE(mLightingInterfaces);
   smBasicLightManagerSingleton = NULL;
}

void BasicLightManager::cleanup()
{
   if (smBasicLightManagerSingleton)
   {
      SAFE_DELETE(smBasicLightManagerSingleton);
   }
}

bool BasicLightManager::canActivate()
{
#ifdef BASIC_LIGHTS_ONLY   
   return Parent::canActivate();
#else
   return true;
#endif
}

void BasicLightManager::activate()
{
   mActivate.trigger(true);
}

void BasicLightManager::deactivate()
{
   mActivate.trigger(false);
}

BasicLightInfo* BasicLightManager::createBasicLightInfo()
{
   return new BasicLightInfo();
}

// Returns a LightInfo
LightInfo* BasicLightManager::createLightInfo()
{
   return createBasicLightInfo();
}

// Returns a "default" light info that callers should not free.  Used for instances where we don't actually care about 
// the light (for example, setting default data for SceneGraphData)
LightInfo* BasicLightManager::getDefaultLight()
{
   return getSpecialLight(LightManager::slSunLightType);
}

// "Special lights" currently this is the sun and the "translated" sun (used by terrain and interior).
LightInfo* BasicLightManager::getSpecialLight(LightManager::SpecialLightTypesEnum type)
{
   if(mSpecialLights[type])
   {
      return mSpecialLights[type];
   }
   // return a default light...
   return mDefaultLight;
}

void BasicLightManager::setSpecialLight(LightManager::SpecialLightTypesEnum type, LightInfo *light)
{
   BASIC_CHECK_LIGHTINFO(light);
   mSpecialLights[type] = static_cast<BasicLightInfo*>(light);
   if (type == LightManager::slSunLightType)
   {
      // Update our translated light
      mSpecialLights[LightManager::slTranslatedSunlightType] = mTranslatedSun;
      mTranslatedSun->mDirection.set(light->mDirection);
      mTranslatedSun->mPos.set(mSpecialLights[type]->mDirection * -10000.0f); 
      mTranslatedSun->mAmbient = light->mAmbient;
   }
}

/// Returns all unsorted and un-scored lights (both global and local).
void BasicLightManager::getAllUnsortedLights(LightInfoList &list)
{
   list.push_back(getSpecialLight(LightManager::slSunLightType));
}

void BasicLightManager::getBestLights(LightInfoList &list)
{
   getAllUnsortedLights(list);
}

/// Sets shader constants / textures for light infos
void BasicLightManager::setLightInfo(ProcessedMaterial* pmat, const Material* mat, const SceneGraphData& sgData, U32 pass) 
{
   // Light number 1   
   MatrixF objTrans = sgData.objTrans;
   objTrans.inverse();

   LightInfo* light = sgData.light;
   Point3F lightPos = light->mPos;
   Point3F lightDir = light->mDirection;
   objTrans.mulP(lightPos);
   objTrans.mulV(lightDir);
   lightDir.normalizeSafe();

   GFX->setVertexShaderConstF(VC_LIGHT_POS1, (float*)&lightPos, 1, 3);
   GFX->setVertexShaderConstF(VC_LIGHT_DIR1, (float*)&lightDir, 1, 3);
   // Try to minimize some of the constants we send down the pipe
   if ((!sgData.matIsInited) || (mLastLightSentToShader != light))
   {
      GFX->setVertexShaderConstF(VC_LIGHT_DIFFUSE1, (float*)&(light->mColor), 1);
      GFX->setPixelShaderConstF(PC_AMBIENT_COLOR, (float*)&(light->mAmbient), 1);
      mLastLightSentToShader = light;
   }
}

void BasicLightManager::registerGlobalLights(bool staticlighting)
{
   // ask all light objects to register themselves...
   SimSet *lightset = Sim::getLightSet();
   for(SimObject **itr=lightset->begin(); itr!=lightset->end(); itr++)
      (*itr)->registerLights(this, staticlighting);
}

const char* BasicLightManager::getId() const
{
   return "BL";
}

// Returns the active scene lighting interface for this light manager.  
AvailableSLInterfaces* BasicLightManager::getSceneLightingInterface()
{   
   return mLightingInterfaces;   
}

bool BasicLightManager::lightScene(const char* callback, const char* param)
{
   BitSet32 flags = 0;

   if(param)
   {
      if(!dStricmp(param, "forceAlways"))
         flags.set(SceneLighting::ForceAlways);
      else if(!dStricmp(param, "forceWritable"))
         flags.set(SceneLighting::ForceWritable);
   }
   // SceneLighting object is freed by Sim:: or itself, so we don't really
   // own it.  Kind of "funny" but we'll leave it like that for now.
   SceneLighting* sl = new SceneLighting(mLightingInterfaces);
   return sl->lightScene(callback, flags);   
}
