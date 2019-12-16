//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "sim/netConnection.h"
#include "util/journal/process.h"
#include "gui/missionEditor/worldEditor.h"
#include "materials/material.h"
#include "materials/matInstance.h"
#include "materials/processedMaterial.h"
#include "materials/processedCustomMaterial.h"
#include "shaderGen/featureMgr.h"
#include "T3D/gameConnection.h"
#include "lightingSystem/synapseGaming/sgLighting.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgMissionLightingFilter.h"
#include "sceneGraph/lightingInterfaces.h"
#include "lightingSystem/synapseGaming/sgShadowTextureCache.h"
#include "lightingSystem/synapseGaming/shadowCasterManager.h"
#include "lightingSystem/synapseGaming/sgLightObject.h"
#include "lightingSystem/synapseGaming/sgObjectBasedProjector.h"
#include "lightingSystem/synapseGaming/sgMatInstance.h"
#include "lightingSystem/synapseGaming/sgLightingFeatures.h"
#include "lightingSystem/common/sceneLighting.h"
#include SHADER_CONSTANT_INCLUDE_FILE
#include "lightingSystem/synapseGaming/sgMeshLightPlugin.h"
#include "lightingSystem/synapseGaming/sgSceneObjectLightingPlugin.h"
#include "lightingSystem/synapseGaming/sgSceneLighting.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//-----------------------------------------------------------------------------
//宏定义
//状态块
//-----------------------------------------------------------------------------
//////#define STATEBLOCK

//
sgLightManager* sgLightManager::smsgLightManagerSingleton = NULL;

bool sgLightManager::sgLightingProperties[sgPropertyCount];
bool sgLightManager::sgUseSelfIlluminationColor = false;
ColorF sgLightManager::sgSelfIlluminationColor = ColorF(0.0f, 0.0f, 0.0f);
//bool sgLightManager::sgDynamicDTSVectorLighting = false;
bool sgLightManager::sgFilterZones = false;
S32 sgLightManager::sgZones[2] = {-1, -1};
//S32 sgLightManager::sgShadowDetailSize = 0;
//bool sgLightManager::sgDynamicParticleSystemLighting = true;
U32 sgLightManager::sgLightingProfileQuality = sgLightManager::lpqtProduction;
bool sgLightManager::sgLightingProfileAllowShadows = true;
LightInfo* sgLightManager::sgDefaultLight = NULL;
//bool sgLightManager::sgDetailMaps = true;
//GFXTexHandle sgLightManager::sgDLightMap = NULL;
S32 sgLightManager::sgMaxBestLights = 8;
bool sgLightManager::sgUseDynamicShadows = true;
bool sgLightManager::sgUseDynamicLightingDualOptimization = true;
//bool sgLightManager::sgUseDynamicShadowSelfShadowing = true;
//bool sgLightManager::sgUseDynamicShadowSelfShadowingOnPS_2_0 = true;

bool sgLightManager::sgUseDynamicRangeLighting = true;
bool sgLightManager::sgUseDRLHighDynamicRange = false;
bool sgLightManager::sgAllowDynamicRangeLighting = false;
bool sgLightManager::sgAllowDRLHighDynamicRange = false;
bool sgLightManager::sgAllowDRLBloom = false;
bool sgLightManager::sgAllowDRLToneMapping = false;
bool sgLightManager::sgUseScreenBloom = false;

U32 sgLightManager::sgDynamicShadowDetailSize = 0;
U32 sgLightManager::sgDynamicShadowQuality = 0;
bool sgLightManager::sgMultipleDynamicShadows = true;
bool sgLightManager::sgShowCacheStats = false;
F32 sgLightManager::sgDRLTarget = 0.25f;
F32 sgLightManager::sgDRLMax = 2.0f;
F32 sgLightManager::sgDRLMin = 0.85f;
F32 sgLightManager::sgDRLMultiplier = 1.0f;
F32 sgLightManager::sgBloomCutOff = 0.7f;
F32 sgLightManager::sgBloomAmount = 2.0f;
F32 sgLightManager::sgBloomSeedAmount = 0.32f;
U32 sgLightManager::sgAtlasMaxDynamicLights = 16;
bool sgLightManager::sgInGUIEditor = false;

bool sgRelightFilter::sgFilterRelight = false;
bool sgRelightFilter::sgFilterRelightVisible = true;
bool sgRelightFilter::sgFilterRelightByDistance = true;
//bool sgRelightFilter::sgFilterRelightByVisiblity;
F32 sgRelightFilter::sgFilterRelightByDistanceRadius = 60;
Point3F sgRelightFilter::sgFilterRelightByDistancePosition;

U32 sgStatistics::sgInteriorLexelCount = 0;
U32 sgStatistics::sgInteriorLexelIlluminationCount = 0;
U32 sgStatistics::sgInteriorLexelIlluminationTime = 0;
U32 sgStatistics::sgInteriorLexelDiffuseCount = 0;
U32 sgStatistics::sgInteriorObjectCount = 0;
U32 sgStatistics::sgInteriorObjectIncludedCount = 0;
U32 sgStatistics::sgInteriorObjectIlluminationCount = 0;
U32 sgStatistics::sgInteriorSurfaceIncludedCount = 0;
U32 sgStatistics::sgInteriorSurfaceIlluminationCount = 0;
U32 sgStatistics::sgInteriorSurfaceIlluminatedCount = 0;
U32 sgStatistics::sgInteriorSurfaceSmoothedCount = 0;
U32 sgStatistics::sgInteriorSurfaceSmoothedLexelCount = 0;
U32 sgStatistics::sgInteriorSurfaceSetupTime = 0;
U32 sgStatistics::sgInteriorSurfaceSetupCount = 0;
U32 sgStatistics::sgInteriorSurfaceMergeTime = 0;
U32 sgStatistics::sgInteriorSurfaceMergeCount = 0;
U32 sgStatistics::sgStaticMeshSurfaceOccluderCount = 0;
U32 sgStatistics::sgStaticMeshBVPTPotentialOccluderCount = 0;
U32 sgStatistics::sgStaticMeshCastRayCount = 0;
U32 sgStatistics::sgTerrainLexelCount = 0;
U32 sgStatistics::sgTerrainLexelTime = 0;


//-----------------------------------------------------------------------------
//静态成员
//状态块
//-----------------------------------------------------------------------------
GFXStateBlock* sgLightManager::mBlendSB = NULL;
//GFXStateBlock* mAddrClampSB = NULL;

//-----------------------------------------------
void WorldEditorRenderScene(WorldEditor* we);
void handleDemoPlay();
//-----------------------------------------------

void sgFindObjectsCallback(SceneObject* obj, void *val)
{
	Vector<SceneObject*> * list = (Vector<SceneObject*>*)val;
	list->push_back(obj);
}

void ProcessLightManagerCallback()
{
	ShadowCasterManager::cleanupUnusedShadows();
	sgShadowTextureCache::sgCleanupUnused();
}

sgLightManager::sgLightManager()
{
   dMemset(&sgSpecialLights, 0, sizeof(sgSpecialLights));
   if (!sgDefaultLight)
      sgDefaultLight = createSGLightInfo();
   sgTranslatedSun = createSGLightInfo();
   mLightingInterfaces = new AvailableSLInterfaces();
}

sgLightManager::~sgLightManager()
{   
	smsgLightManagerSingleton = NULL;
	SAFE_DELETE(sgDefaultLight);
	SAFE_DELETE(sgTranslatedSun);
	SAFE_DELETE(mLightingInterfaces);
}

void sgLightManager::activate()
{
   Process::notify(ProcessLightManagerCallback, PROCESS_FIRST_ORDER);

   // Register our shader features with the feature manager
   gFeatureMgr.registerFeature(GFXShaderFeatureData::DynamicLight, new DynamicLightingFeature);
   gFeatureMgr.registerFeature(GFXShaderFeatureData::DynamicLightDual, new DynamicLightingDummyFeature);
   gFeatureMgr.registerFeature(GFXShaderFeatureData::DynamicLightMask, new DynamicLightingDummyFeature);
   gFeatureMgr.registerFeature(GFXShaderFeatureData::DynamicLightAttenuateBackFace, new DynamicLightingDummyFeature);
   gFeatureMgr.registerFeature(GFXShaderFeatureData::ExposureX2, new ExposureFeatureX2);
   gFeatureMgr.registerFeature(GFXShaderFeatureData::ExposureX4, new ExposureFeatureX4);
   gFeatureMgr.registerFeature(GFXShaderFeatureData::SelfIllumination, new SelfIlluminationFeature); 

#ifdef NTJ_EDITOR
   WorldEditor::smRenderSceneSignal.notify(WorldEditorRenderScene);
   GameConnection::smPlayingDemo.notify(handleDemoPlay);   
#endif

   TSMesh::registerLightPlugin(&g_sgMeshLightPlugin);
   mSgActivate.trigger(true);
}

void sgLightManager::deactivate()
{
   Process::remove(ProcessLightManagerCallback);	
#ifdef NTJ_EDITOR
   WorldEditor::smRenderSceneSignal.remove(WorldEditorRenderScene);
   GameConnection::smPlayingDemo.remove(handleDemoPlay);   
#endif

   ShadowCasterManager::clearAllShadows();
   sgShadowSharedZBuffer::sgClear();
   sgLightingModelManager::cleanup();

   // Unregister our shader features.
   gFeatureMgr.unregisterFeature(GFXShaderFeatureData::DynamicLight);
   gFeatureMgr.unregisterFeature(GFXShaderFeatureData::DynamicLightDual);
   gFeatureMgr.unregisterFeature(GFXShaderFeatureData::DynamicLightMask);
   gFeatureMgr.unregisterFeature(GFXShaderFeatureData::DynamicLightAttenuateBackFace);
   gFeatureMgr.unregisterFeature(GFXShaderFeatureData::ExposureX2);
   gFeatureMgr.unregisterFeature(GFXShaderFeatureData::ExposureX4);
   gFeatureMgr.unregisterFeature(GFXShaderFeatureData::SelfIllumination);

   TSMesh::unregisterLightPlugin(&g_sgMeshLightPlugin);
   mSgActivate.trigger(false);

   cleanup();
}

const char* sgLightManager::getId() const
{
   return "SG";
}

// Returns the active scene lighting interface for this light manager.  
AvailableSLInterfaces* sgLightManager::getSceneLightingInterface()
{
   return mLightingInterfaces;
}

AvailableSLInterfaces* sgLightManager::getLightingInterfaces()
{
   return mLightingInterfaces;
}

// Used outside of the lighting system
LightInfo* sgLightManager::createLightInfo()
{
   return createSGLightInfo();
}

// Used inside of the lighting system
sgLightInfo* sgLightManager::createSGLightInfo()
{
   return new sgLightInfo();
}

LightInfo* sgLightManager::getDefaultLight()
{
   return sgDefaultLight;
}

//-----------------------------------------------

void sgLightManager::sgBuildDualLightLists(const LightInfoList &list, LightInfoDualList &listdual)
{
	LightInfoList pri, sec;

	// clean up...
	listdual.clear();

	// sort...
	for(U32 i=0; i<list.size(); i++)
	{
      SG_CHECK_LIGHT(list[i]);		
      sgLightInfo *light = static_cast<sgLightInfo*>(list[i]);
      
		if((light->mType == LightInfo::Vector) ||
		   (light->mType == LightInfo::Ambient))
		{
			// non-group-able...
			listdual.increment();
			listdual.last().sgLightPrimary = light;
			listdual.last().sgLightSecondary = NULL;
			continue;
		}

		// sort...
		if(light->sgCanBeSecondary())
			sec.push_back(light);
		else
			pri.push_back(light);
	}

	// build using primary lights...
	// these can only go in slot one...
	while(pri.size())
	{
		listdual.increment();
		listdual.last().sgLightPrimary = static_cast<sgLightInfo*>(pri[0]);
		listdual.last().sgLightSecondary = NULL;
		pri.erase_fast(U32(0));
		
		if(sec.size() <= 0)
			continue;

		// piggyback any available secondary lights...
		listdual.last().sgLightSecondary = static_cast<sgLightInfo*>(sec[0]);
		sec.erase_fast(U32(0));
	}

	// use up any extra secondary lights...
	// treat these as primary lights...
	while(sec.size())
	{
		listdual.increment();
		listdual.last().sgLightPrimary = static_cast<sgLightInfo*>(sec[0]);
		listdual.last().sgLightSecondary = NULL;
		sec.erase_fast(U32(0));
		
		if(sec.size() <= 0)
			continue;

		// piggyback any available secondary lights...
		listdual.last().sgLightSecondary = static_cast<sgLightInfo*>(sec[0]);
		sec.erase_fast(U32(0));
	}
}

LightInfo *sgLightManager::getSpecialLight(SpecialLightTypesEnum type)
{
	if(sgSpecialLights[type])
   {
		return sgSpecialLights[type];
   }
	// return a default light...
	return sgDefaultLight;
}

void sgLightManager::registerGlobalLight(LightInfo *light, SimObject *obj, bool zonealreadyset)
{
	if(!zonealreadyset)
	{
      SG_CHECK_LIGHT(light);      
		SceneObject *sceneobj = dynamic_cast<SceneObject *>(obj);
		if(sceneobj)
		{
         sgLightInfo* sglight = static_cast<sgLightInfo*>(light);
			U32 count = getMin(sceneobj->getNumCurrZones(), U32(2));
			for(U32 i=0; i<count; i++)
				sglight->sgZone[i] = sceneobj->getCurrZone(i);
		}
	}

	sgRegisteredGlobalLights.sgRegisterLight(light);

	// not here!!!
	/*if(light->mType == LightInfo::Vector)
	{
	if(getSpecialLight(slSunLightType) == sgDefaultLight)
	setSpecialLight(slSunLightType, light);
	}*/
}

void sgLightManager::setSpecialLight(SpecialLightTypesEnum type, LightInfo *light) 
{
   SG_CHECK_LIGHT(light);
   sgLightInfo* l = static_cast<sgLightInfo*>(light);
   sgSpecialLights[type] = l;
   if (type == slSunLightType)
   {
      // The sunlight must have certain parameters on
      l->sgZone[0] = 0;
      l->sgDoubleSidedAmbient = true;
      l->sgUseNormals = true;
      // Update our translated light
      sgSpecialLights[slTranslatedSunlightType] = sgTranslatedSun;
      sgTranslatedSun->mDirection.set(light->mDirection);
      sgTranslatedSun->mPos.set(sgSpecialLights[type]->mDirection * -10000.0f); 
      sgTranslatedSun->mAmbient = light->mAmbient;
   }
}
void sgLightManager::registerGlobalLights(bool staticlighting)
{
	// make sure we're clean...
	unregisterAllLights();

	// ask all light objects to register themselves...
	PROFILE_START(sphereInFrustum);
	SimSet *lightset = Sim::getLightSet();
	for(SimObject **itr=lightset->begin(); itr!=lightset->end(); itr++)
		(*itr)->registerLights(this, staticlighting);
	PROFILE_END(sphereInFrustum);

	sgCameraBaseLights.clear();
	sgCameraBaseLights.merge(sgRegisteredGlobalLights);

	// find dupes...
	dQsort(sgCameraBaseLights.address(), sgCameraBaseLights.size(), sizeof(LightInfo*), sgSortLightsByAddress);
	LightInfo *last = NULL;
	for(U32 i=0; i<sgCameraBaseLights.size(); i++)
	{
		if(sgCameraBaseLights[i] == last)
		{
			sgCameraBaseLights.erase(i);
			i--;
			continue;
		}
		last = sgCameraBaseLights[i];
	}

//  Ray:只用视景体内部的灯光是有严重问题的
	if (gClientSceneGraph && gClientSceneGraph->getCurSceneState())
	{
		SceneState *state = gClientSceneGraph->getCurSceneState();
		const Frustum& frustum = state->getFrustum();
		LightInfo *light = NULL;
		for (U32 k=0; k<sgCameraBaseLights.size(); k++)
		{
			light = static_cast<sgLightInfo*>(sgCameraBaseLights[k]);
			if (!light || !frustum.sphereInFrustum(light->mPos, light->getRadius()))
			{
				sgCameraBaseLights.erase(k);
				k--;
			}
		}
	}
}

void sgLightManager::unregisterAllLights()
{
	sgRegisteredGlobalLights.clear();
	sgRegisteredLocalLights.clear();

	dMemset(&sgSpecialLights, 0, sizeof(sgSpecialLights));
}

void sgLightManager::getAllUnsortedLights(LightInfoList &list)
{
	list.merge(sgCameraBaseLights);
}
//--------------------------------------------------------------------------
// Lighting update: not used directly by sceneobject...
// - if an interior, which contains this object, moves then this value will be incorrect
bool sgLightManager::getLightingAmbientColor(SceneObject* obj, ColorF * col, sgLightInfo** light)
{
   AssertFatal(col!=NULL, "SceneObject::getLightingAmbientColor: invalid color ptr");
   AssertFatal(light, "Light must be a valid pointer");

   const F32 cRayLength = 100.f;             // down/up
   //const F32 cTerrainRayLength = 10.f;       // height above terrain for no ambient
   const F32 cColorStep = 0.2f;              // amount to add per 100ms
   const F32 ambientMoveTol = 0.01f;

   sgSceneObjectLightingPlugin* lightPlugin = dynamic_cast<sgSceneObjectLightingPlugin*>(obj->getLightingPlugin());
   AssertFatal(lightPlugin, "No lighting plug-in available!");
   if (!lightPlugin)
      return false;

   *light = lightPlugin->mLightingInfo.mAmbientLight;
   
   PROFILE_START(GetLightingAmbientColor);  
   
   // Get the top of our object box, that determines our ambient lighting.
   Point3F pos;
   const Box3F& b = obj->getRenderWorldBox();
   b.getCenter(&pos);
   pos.z = b.max.z;

   // Check to see if we've moved or not.
   VectorF dist = pos - lightPlugin->mLightingInfo.mLastPosition;

   // If we have (or if we haven't computed the color yet, calculate it)
   if ((!lightPlugin->mLightingInfo.mInit) || (dist.len() > ambientMoveTol))
   {
      lightPlugin->mLightingInfo.mInit = true;
      lightPlugin->mLightingInfo.mLastPosition = pos;

      // check if shadowed:
      obj->disableCollision();
      lightPlugin->mLightingInfo.mUseInfo = false;
      // Ambient light is determined by the surface we are standing on.
      RayInfo collision;      
      if(gClientContainer.castRay(pos, Point3F(pos.x, pos.y, pos.z - cRayLength),
         mLightingInterfaces->mAvailableObjectTypes, &collision))
      {
         bool found = false;
         ColorF col;
              
         collision.point.convolveInverse(collision.object->getScale());

         // Seem if any system can give us the light/color information we want.
         for(SceneLightingInterface** sitr = mLightingInterfaces->mAvailableSystemInterfaces.begin(); sitr != mLightingInterfaces->mAvailableSystemInterfaces.end() && !found; sitr++)
         {
            SceneLightingInterface* si = (*sitr);
            found = si->getColorFromRayInfo(collision, col);
            if (found)
            {
				if(obj->getType() & GameObjectType)
				{
					col *= 1.5;
				}

               lightPlugin->mLightingInfo.mDefaultColor = col;
               lightPlugin->mLightingInfo.mUseInfo = true;
            }
         }
      }
      obj->enableCollision();
   }
   PROFILE_END(GetLightingAmbientColor);

   // has a value?
   if(lightPlugin->mLightingInfo.mUseInfo)
   {
      // currently in an interior which has an alarm state?
      ColorF color;
      color = lightPlugin->mLightingInfo.mDefaultColor;

      S32 time = Platform::getVirtualMilliseconds();

      // has a previous color?
      if(lightPlugin->mLightingInfo.mHasLastColor)
      {
         // do each component
         F32 * pColor = const_cast<F32*>((const F32 *)color);
         F32 * pLastColor = const_cast<F32*>((const F32 *)lightPlugin->mLightingInfo.mLastColor);

         // cColorStep is amount added per 100ms
         F32 step = (F32(time - lightPlugin->mLightingInfo.mLastTime) / 100.f) * cColorStep;

         for(U32 i = 0; i < 3; i++)
         {
            if(pColor[i] > pLastColor[i])
               pColor[i] = mClampF(pLastColor[i] + step, 0.f, pColor[i]);
            else if(pColor[i] < pLastColor[i])
               pColor[i] = mClampF(pLastColor[i] - step, pColor[i], 1.f);
         }
      }

      lightPlugin->mLightingInfo.mHasLastColor = true;
      lightPlugin->mLightingInfo.mLastColor = color;
      lightPlugin->mLightingInfo.mLastTime = time;
      *col = color;
   }
   else
   {
      lightPlugin->mLightingInfo.mHasLastColor = true;
	  LightInfo *pSun = getSpecialLight(slSunLightType);
	  if(!pSun)
		lightPlugin->mLightingInfo.mDefaultColor = ColorF(1.0,0.0,0.0,1.0);  //Ray: 这里应该调用太阳的颜色啊！
	  else
		lightPlugin->mLightingInfo.mDefaultColor = pSun->mAmbient;

	  lightPlugin->mLightingInfo.mDefaultColor*=2;
      lightPlugin->mLightingInfo.mLastColor = lightPlugin->mLightingInfo.mDefaultColor;
      *col = lightPlugin->mLightingInfo.mDefaultColor;
   }
   return true;
}

void sgLightManager::setupLights(SceneObject *obj)
{
	resetLights();

	bool outside = false;
	for(U32 i=0; i<obj->getNumCurrZones(); i++)
	{
		if(obj->getCurrZone(i) == 0)
		{
			outside = true;
			break;
		}
	}

	sgSetProperty(sgReceiveSunLightProp, obj->receiveSunLight);
	sgSetProperty(sgAdaptiveSelfIlluminationProp, obj->useAdaptiveSelfIllumination);
	sgSetProperty(sgCustomAmbientLightingProp, obj->useCustomAmbientLighting);
	sgSetProperty(sgCustomAmbientForSelfIlluminationProp, obj->customAmbientForSelfIllumination);

	ColorF ambientColor;
	ColorF selfillum = obj->customAmbientLighting;
   
	sgLightInfo *sun = sgSpecialLights[slSunLightType];
    sgLightInfo* light = NULL;
	if(getLightingAmbientColor(obj, &ambientColor, &light) && sun)
	{
        // Reset the ambient light		
		light->mType = LightInfo::Ambient;
		light->mDirection = VectorF(0.0, 0.0, -1.0);
		light->sgCastsShadows = sun->sgCastsShadows;

		bool npcObj = false;
		if(obj->getType() & GameObjectType)
		{
			npcObj = true;
		}

		// players, vehicles, ...
		if(obj->overrideOptions)
		{
			if(outside)
			{
				light->mType = LightInfo::Vector;
				light->mDirection = sun->mDirection;
			}
            
            if ( sun->mReceiveLMLighting )
            {
			    light->mColor = ambientColor * 0.5f;
			    light->mAmbient = ambientColor * 0.5f;
            }
            else
            {
                light->mColor = sun->mColor;
				light->mAmbient = npcObj ? sun->mObjAmbient : sun->mAmbient;
            }
		}// beyond here are the static dts options...
		else if(sgAllowDiffuseCustomAmbient())
		{
            light->mColor = obj->customAmbientLighting * 0.25f;
            light->mAmbient = obj->customAmbientLighting * 0.75f;
		}
		else if(sgAllowReceiveSunLight() && sun)
		{
			light->mType = LightInfo::Vector;
			if(outside)
				light->mDirection = sun->mDirection;
			if(obj->receiveLMLighting && sun->mReceiveLMLighting )
			{
                light->mColor = ambientColor* 0.5f;
                light->mAmbient = ambientColor * 0.5f;
			}
			else
			{
                light->mColor = sun->mColor;
				light->mAmbient = npcObj ? sun->mObjAmbient : sun->mAmbient;
			}
		}
		else if(obj->receiveLMLighting)
		{
            light->mColor = ambientColor* 0.5f;
            light->mAmbient = ambientColor * 0.5f;
		}
        light->mBackColor = sun->mBackColor;

		if(sgAllowCollectSelfIlluminationColor())
		{
			selfillum = light->mAmbient + light->mColor;
			selfillum.clamp();

			//Ray: 设置指定自发光色
			if(obj->useAdaptiveSelfIllumination)
			{
				light->mIsSelfillum = true;
				light->mSelfillum = obj->customAmbientLighting;
			}
		}        

		light->mPos = light->mDirection * -10000.0;
		//registerLocalLight(light);
	}

	// install assigned baked lights from simgroup...
	// step one get the objects...
	NetConnection *connection = NetConnection::getConnectionToServer();

   sgSceneObjectLightingPlugin* lightPlugin = dynamic_cast<sgSceneObjectLightingPlugin*>(obj->getLightingPlugin());
   AssertFatal(lightPlugin, "No lighting plug-in available!");
   if (lightPlugin)
      lightPlugin->registerLocalLights(connection);

	// step three install dynamic lights...
	sgUseSelfIlluminationColor = sgGetProperty(sgAdaptiveSelfIlluminationProp);
	if(sgUseSelfIlluminationColor)
		sgSelfIlluminationColor = selfillum;

	sgSetupZoneLighting(true, obj);

	sgBestLights.clear();

	//Ray: 属于自己的localLight直接需要，别人的不管
	if(light)
	{
		sgBestLights.push_back(light);
	}

	bool caseStatic = !(obj->getType() & GameBaseObjectType);
	sgFindBestLights(obj->getRenderWorldBox(), sgMaxBestLights, Point3F(0, 0, 0), false,caseStatic);
}

void sgLightManager::setupLights(SceneObject *obj, const Point3F &camerapos,
								 const Point3F &cameradir, F32 viewdist, S32 maxlights)
{
	resetLights();

	sgSetupZoneLighting(true, obj);

	Box3F box;
	box.max = camerapos + Point3F(viewdist, viewdist, viewdist);
	box.min = camerapos - Point3F(viewdist, viewdist, viewdist);

	bool caseStatic = !(obj->getType() & GameBaseObjectType);
	sgFindBestLights(box, maxlights, cameradir, true,caseStatic);
}

void sgLightManager::setupLights(SceneObject *obj, const Box3F &box, S32 maxlights)
{
	resetLights();
	sgSetupZoneLighting(true, obj);

	bool caseStatic = !(obj->getType() & GameBaseObjectType);
	sgFindBestLights(box, maxlights, Point3F(0, 0, 0), true,caseStatic);
}

void sgLightManager::resetLights()
{
	sgSetupZoneLighting(false, NULL);
	sgRegisteredLocalLights.clear();
	sgBestLights.clear();
}

void sgLightManager::sgFindBestLights(const Box3F &box, S32 maxlights, const Point3F &viewdi, bool camerabased,bool caseStatic)
{
	PROFILE_START(getAllUnsortedLights);
	// gets them all and removes any dupes...
	getAllUnsortedLights(sgBestLights);
	PROFILE_END(getAllUnsortedLights);

	SphereF sphere;
	box.getCenter(&sphere.center);
	sphere.radius = Point3F(box.max - sphere.center).len();

	sgLightInfo*pInfo;
	for(U32 i=0; i<sgBestLights.size(); i++)
    {
		if(!caseStatic)
		{
			pInfo = static_cast<sgLightInfo*>(sgBestLights[i]);
			if(pInfo->mType==LightInfo::SGStaticPoint || pInfo->mType==LightInfo::SGStaticSpot)
			{
				sgBestLights[i]->mScore = 0;
				continue;
			}
		}

		sgScoreLight(static_cast<sgLightInfo*>(sgBestLights[i]), box, sphere, camerabased);
    }

	dQsort(sgBestLights.address(), sgBestLights.size(), sizeof(LightInfo*), sgSortLightsByScore);

	for(U32 i=0; i<sgBestLights.size(); i++)
	{
		if((sgBestLights[i]->mScore > 0) && (i < maxlights))
			continue;

		sgBestLights.setSize(i);
		break;
	}
}

void sgLightManager::sgScoreLight(sgLightInfo *light, const Box3F &box, const SphereF &sphere, bool camerabased)
{
	if(sgFilterZones && light->sgDiffuseRestrictZone)
	{
		bool allowdiffuse = false;
		if(sgZones[0] > -1)
		{
			if(light->sgAllowDiffuseZoneLighting(sgZones[0]))
				allowdiffuse = true;
			else if(sgZones[1] > -1)
			{
				if(light->sgAllowDiffuseZoneLighting(sgZones[1]))
					allowdiffuse = true;
			}
		}

		if(!allowdiffuse)
		{
			light->mScore = 0;
			return;
		}
	}


	F32 distintensity = 1;
	F32 colorintensity = 1;
	F32 weight = SG_LIGHTMANAGER_DYNAMIC_PRIORITY;

	if(camerabased)
	{
		sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(light->sgLightingModelName);
		model.sgSetState(light);
		F32 maxrad = model.sgGetMaxRadius(true);
		model.sgResetState();

		Point3F vect = sphere.center - light->mPos;
		F32 dist = vect.len();
		F32 distlightview = sphere.radius + maxrad;

		if(distlightview <= 0)
			distintensity = 0.0f;
		else
		{
			distintensity = 1.0f - (dist / distlightview);
			distintensity = mClampF(distintensity, 0.0f, 1.0f);
		}
	}
	else
	{
		// side test...
		if((light->mType == LightInfo::Spot) || (light->mType == LightInfo::SGStaticSpot))
		{
			bool anyfront = false;
			F32 x, y, z;

			for(U32 i=0; i<8; i++)
			{
				if(i & 0x1)
					x = box.max.x;
				else
					x = box.min.x;

				if(i & 0x2)
					y = box.max.y;
				else
					y = box.min.y;

				if(i & 0x4)
					z = box.max.z;
				else
					z = box.min.z;

				if(light->sgSpotPlane.whichSide(Point3F(x, y, z)) == PlaneF::Back)
					continue;

				anyfront = true;
				break;
			}

			if(!anyfront)
			{
				light->mScore = 0;
				return;
			}
		}

		if((light->mType == LightInfo::Vector) || (light->mType == LightInfo::Ambient))
		{
			// in TSE the sun/env is the base pass and is required!
			colorintensity = 100000;
			//colorintensity =
			//	(light->mColor.red   + light->mAmbient.red) * 0.346f +
			//	(light->mColor.green + light->mAmbient.green) * 0.588f + 
			//	(light->mColor.blue  + light->mAmbient.blue) * 0.070f;
			distintensity = 1;
			weight = SG_LIGHTMANAGER_SUN_PRIORITY;
		}
		else
		{
			if(light->sgAssignedToParticleSystem)
			{
				colorintensity = SG_PARTICLESYSTEMLIGHT_FIXED_INTENSITY;
			}
			else
			{
				colorintensity =
					(light->mColor.red * 0.3333f) +
					(light->mColor.green * 0.3333f) +
					(light->mColor.blue * 0.3333f);
			}

			sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(light->sgLightingModelName);
			model.sgSetState(light);
			distintensity = model.sgScoreLight(light, sphere);
			model.sgResetState();

			if(light->sgAssignedToTSObject)
				weight = SG_LIGHTMANAGER_ASSIGNED_PRIORITY;
			else if((light->mType == LightInfo::SGStaticPoint) || (light->mType == LightInfo::SGStaticSpot))
				weight = SG_LIGHTMANAGER_STATIC_PRIORITY;
		}
	}

	F32 intensity = colorintensity * distintensity;
	if(intensity < SG_MIN_LEXEL_INTENSITY)
		intensity = 0;
	light->mScore = getMax(S32(intensity * weight * 1024.0f),0);
}

static ProcessRegisterInit doSgInit(&sgLightManager::sgInit);

bool sgLightManager::sgInit()
{
	for(U32 i=0; i<sgPropertyCount; i++)
		sgLightingProperties[i] = false;

	//Con::addVariable("$pref::OpenGL::sgDynamicDTSVectorLighting", TypeBool, &sgDynamicDTSVectorLighting);
	//Con::addVariable("$pref::TS::sgShadowDetailSize", TypeS32, &sgShadowDetailSize);
	//Con::addVariable("$pref::OpenGL::sgDynamicParticleSystemLighting", TypeBool, &sgDynamicParticleSystemLighting);
	//Con::addVariable("$pref::sgLightManager::MaxBestLights", TypeS32, &sgMaxBestLights);
	Con::addVariable("$pref::sgLightManager::LightingProfileQuality", TypeS32, &sgLightingProfileQuality);
	Con::addVariable("$pref::sgLightManager::LightingProfileAllowShadows", TypeBool, &sgLightingProfileAllowShadows);

	Con::addVariable("$pref::sgLightManager::UseDynamicShadows", TypeBool, &sgUseDynamicShadows);
	//Con::addVariable("$pref::sgLightManager::UseDynamicShadowSelfShadowing", TypeBool, &sgUseDynamicShadowSelfShadowing);
	//Con::addVariable("$pref::sgLightManager::UseDynamicShadowSelfShadowingOnPS_2_0", TypeBool, &sgUseDynamicShadowSelfShadowingOnPS_2_0);
	//Con::addVariable("$pref::sgLightManager::UseBloom", TypeBool, &sgUseBloom);
	//Con::addVariable("$pref::sgLightManager::UseToneMapping", TypeBool, &sgUseToneMapping);
	Con::addVariable("$pref::sgLightManager::UseDynamicRangeLighting", TypeBool, &sgUseDynamicRangeLighting);
	Con::addVariable("$pref::sgLightManager::UseDRLHighDynamicRange", TypeBool, &sgUseDRLHighDynamicRange);
	Con::addVariable("$pref::sgLightManager::UseDynamicLightingDualOptimization", TypeBool, &sgUseDynamicLightingDualOptimization);
    //Con::addVariable("$pref::sgLightManager::sgUseScreenBloom", TypeBool, &sgUseScreenBloom);

	Con::addVariable("$pref::sgLightManager::DynamicShadowDetailSize", TypeS32, &sgDynamicShadowDetailSize);
	Con::addVariable("$pref::sgLightManager::DynamicShadowQuality", TypeS32, &sgDynamicShadowQuality);
	Con::addVariable("$pref::sgLightManager::MultipleDynamicShadows", TypeBool, &sgMultipleDynamicShadows);
	Con::addVariable("$pref::sgLightManager::ShowCacheStats", TypeBool, &sgShowCacheStats);

	//Con::addVariable("$pref::sgLightManager::DRLTarget", TypeF32, &sgDRLTarget);
	//Con::addVariable("$pref::sgLightManager::DRLMax", TypeF32, &sgDRLMax);
	//Con::addVariable("$pref::sgLightManager::DRLMin", TypeF32, &sgDRLMin);
	//Con::addVariable("$pref::sgLightManager::DRLMultiplier", TypeF32, &sgDRLMultiplier);

	//Con::addVariable("$pref::sgLightManager::BloomCutOff", TypeF32, &sgBloomCutOff);
	//Con::addVariable("$pref::sgLightManager::BloomAmount", TypeF32, &sgBloomAmount);
	//Con::addVariable("$pref::sgLightManager::BloomSeedAmount", TypeF32, &sgBloomSeedAmount);

	Con::addVariable("$pref::sgLightManager::AtlasMaxDynamicLights", TypeS32, &sgAtlasMaxDynamicLights);

	Con::addVariable("$sgLightManager::InGUIEditor", TypeBool, &sgInGUIEditor);

	sgRelightFilter::sgInit();
	return true;
}
/*
bool sgLightManager::sgAllowDetailMaps()
{
return true;
}
*/
void sgLightManager::sgGetFilteredLightColor(ColorF &color, ColorF &ambient, S32 objectzone)
{
	sgMissionLightingFilter *filterbasezone = NULL;
	sgMissionLightingFilter *filtercurrentzone = NULL;
	SimSet *filters = Sim::getsgMissionLightingFilterSet();

	for(SimObject ** itr = filters->begin(); itr != filters->end(); itr++)
	{
		sgMissionLightingFilter *filter = dynamic_cast<sgMissionLightingFilter*>(*itr);
		if(!filter)
			continue;

		S32 zone = filter->getCurrZone(0);
		if(zone == 0)
			filterbasezone = filter;
		if(zone == objectzone)
		{
			filtercurrentzone = filter;
			break;
		}
	}

	if(filtercurrentzone)
		filterbasezone = filtercurrentzone;

	if(!filterbasezone)
		return;

	sgMissionLightingFilterData *datablock = (sgMissionLightingFilterData *)filterbasezone->getDataBlock();

	if(!datablock)
		return;

	ColorF composite = datablock->sgLightingFilter * datablock->sgLightingIntensity;

	color *= composite;
	color.clamp();

	ambient *= composite;
	ambient.clamp();

	if(!datablock->sgCinematicFilter)
		return;

	// must use the lighting filter intensity
	// to lock the reference value relative
	// to the lighting intensity
	composite = datablock->sgCinematicFilterReferenceColor *
		datablock->sgCinematicFilterReferenceIntensity *
		datablock->sgLightingIntensity;

	F32 intensity = color.red + color.green + color.blue + ambient.red + ambient.green + ambient.blue;
	F32 intensityref = composite.red + composite.green + composite.blue;

	intensity -= intensityref;

	// blue is too intense...
	if(intensity > 0.0f)
		intensity *= 0.25f;

	F32 redoffset = 1.0f - ((intensity) * 0.1f * datablock->sgCinematicFilterAmount);
	F32 blueoffset = 1.0f + ((intensity) * 0.1f * datablock->sgCinematicFilterAmount);
	F32 greenoffset = 1.0f - ((1.0f - getMin(redoffset, blueoffset)) * 0.5f);

	ColorF multiplier = ColorF(redoffset, greenoffset, blueoffset);

	color *= multiplier;
	color.clamp();
	ambient *= multiplier;
	ambient.clamp();
}

/// Sets shader constants / textures for light infos
void sgLightManager::setLightInfo(ProcessedMaterial* pmat, const Material* mat, const SceneGraphData& sgData, U32 pass)
{
   const sgLightInfo* light = dynamic_cast<const sgLightInfo*>(sgData.light);
   U32 stageNum = pmat->getStageFromPass(pass);
   
   if(!mat->emissive[stageNum])
	   GFX->setPixelShaderConstF(PC_DIFF_COLOR, (float*)&(light->mColor), 1);
   else
   {
	   ColorF selfillum = light->mIsSelfillum ? light->mSelfillum : mat->diffuse[stageNum];
	   selfillum.alpha *= sgData.visibility;
	   GFX->setPixelShaderConstF(PC_DIFF_COLOR, (float*)&selfillum, 1);
   }

   // Light number 1   
   MatrixF objTrans = sgData.objTrans;
   objTrans.inverse();

   Point3F lightPos = light->mPos;
   Point3F lightDir = light->mDirection;
   objTrans.mulP(lightPos);
   objTrans.mulV(lightDir);
   lightDir.normalizeSafe();
   
   Point4F lightPosModel(lightPos.x, lightPos.y, lightPos.z, light->sgTempModelInfo[0]);
   GFX->setVertexShaderConstF(VC_LIGHT_POS1, (float*)&lightPosModel, 1);
   GFX->setVertexShaderConstF(VC_LIGHT_DIR1, (float*)&lightDir, 1, 3);
   GFX->setVertexShaderConstF(VC_LIGHT_DIFFUSE1, (float*)&(light->mColor), 1);
   GFX->setVertexShaderConstF(VC_BACKCOLOR, (float*)&(light->mBackColor), 1);
   GFX->setPixelShaderConstF(PC_AMBIENT_COLOR, (float*)&(light->mAmbient), 1);

   const MatrixF lightingmat = light->sgLightingTransform;
   GFX->setVertexShaderConstF(VC_LIGHT_TRANS, (float*)&lightingmat, 4);
    
   // Light number 2
   const sgLightInfo* light2 = dynamic_cast<const sgLightInfo*>(sgData.lightSecondary);
   AssertFatal(light2 != NULL, "ProcessedSGLightedMaterial::setSecondaryLightInfo: null light");
   lightPos = light2->mPos;
   objTrans.mulP(lightPos);

   Point4F lightPosModel2(lightPos.x, lightPos.y, lightPos.z, light2->sgTempModelInfo[0]);
   GFX->setVertexShaderConstF(VC_LIGHT_POS2, (float*)&lightPosModel2, 1);
   GFX->setPixelShaderConstF(PC_DIFF_COLOR2, (float*)&light2->mColor, 1);

   const MatrixF lightingmat2 = light2->sgLightingTransform;
   GFX->setVertexShaderConstF(VC_LIGHT_TRANS2, (float*)&lightingmat2, 4);

   ProcessedCustomMaterial* pcm = dynamic_cast<ProcessedCustomMaterial*>(pmat);
   if (!pcm)
   {
	   RenderPassData *pPassData = pmat->getPass(pass);
	   if(!pPassData)
		   return;

	   // Set the dynamic light textures
	  for(U32 i=0; i<pPassData->numTex; i++)
	  {
		  setTextureStage(sgData, pPassData->texFlags[i], i);
	  }
   } else {
      // Processed custom materials store their texflags in a different way, so
      // just tell it to update its textures.  
      SceneGraphData& temp = const_cast<SceneGraphData&>(sgData);
      pcm->setTextureStages(temp, pass);
   }
}

void sgLightManager::setLightingBlendFunc()
{

#ifdef STATEBLOCK
	AssertFatal(mBlendSB, "sgLightManager::setLightingBlendFunc -- mBlendSB cannot be NULL.");
	mBlendSB->apply();
#else
   // don't break the material multipass rendering...
   GFX->setAlphaBlendEnable(true);
   GFX->setSrcBlend(GFXBlendSrcAlpha);
   GFX->setDestBlend(GFXBlendOne);  
#endif 
}

bool sgLightManager::setTextureStage(const SceneGraphData& sgData, const U32 currTexFlag, const U32 textureSlot)
{
   switch (currTexFlag)
   {
      case Material::DynamicLight:
#ifdef STATEBLOCK
			//AssertFatal(mAddrClampSB, "sgLightManager::setTextureStage -- mAddrClampSB cannot be NULL.");
			//mAddrClampSB->apply();
			GFX->setSamplerState(textureSlot, GFXSAMPAddressU, GFXAddressClamp);
			GFX->setSamplerState(textureSlot, GFXSAMPAddressV, GFXAddressClamp);
			GFX->setSamplerState(textureSlot, GFXSAMPAddressW, GFXAddressClamp);
#else
         //GFX->setTextureBorderColor(textureSlot, ColorI(0, 0, 0, 0));
         GFX->setTextureStageAddressModeU(textureSlot, GFXAddressClamp);
         GFX->setTextureStageAddressModeV(textureSlot, GFXAddressClamp);
         GFX->setTextureStageAddressModeW(textureSlot, GFXAddressClamp);
#endif

         GFX->setTexture(textureSlot, sgData.dynamicLight);
         return true;
         break;

      case Material::DynamicLightSecondary:
#ifdef STATEBLOCK
			//AssertFatal(mAddrClampSB, "sgLightManager::setTextureStage -- mAddrClampSB cannot be NULL.");
			//mAddrClampSB->apply();
			GFX->setSamplerState(textureSlot, GFXSAMPAddressU, GFXAddressClamp);
			GFX->setSamplerState(textureSlot, GFXSAMPAddressV, GFXAddressClamp);
			GFX->setSamplerState(textureSlot, GFXSAMPAddressW, GFXAddressClamp);
#else
         //GFX->setTextureBorderColor(textureSlot, ColorI(0, 0, 0, 0));
         GFX->setTextureStageAddressModeU(textureSlot, GFXAddressClamp);
         GFX->setTextureStageAddressModeV(textureSlot, GFXAddressClamp);
         GFX->setTextureStageAddressModeW(textureSlot, GFXAddressClamp);
#endif

         GFX->setTexture(textureSlot, sgData.dynamicLightSecondary);
         return true;
         break;

      case Material::DynamicLightMask:
         SG_CHECK_LIGHT(sgData.light);
         GFX->setCubeTexture(textureSlot, static_cast<sgLightInfo*>(sgData.light)->sgLightMask);
         return true;
         break;
   }
   return false;
}

void sgLightManager::sgSetupZoneLighting(bool enable, SimObject *sobj)
{
	sgFilterZones = false;
	// these must be -2...
	sgZones[0] = -2;
	sgZones[1] = -2;
	if(!enable)
		return;
   
   SceneObject* obj = dynamic_cast<SceneObject*>(sobj);
   if (!obj)
      return;
   
   mLightingInterfaces->mAvailableObjectTypes;
   if ((obj->getTypeMask() & (mLightingInterfaces->mZoneLightSkipMask)) != 0)
      return;

	sgFilterZones = true;
	U32 count = getMin(obj->getNumCurrZones(), U32(2));
	for(U32 i=0; i<count; i++)
	{
		sgZones[i] = obj->getCurrZone(i);
	}
}

S32 QSORT_CALLBACK sgLightManager::sgSortLightsByAddress(const void* a, const void* b)
{
	return ((*(LightInfo**)b) - (*(LightInfo**)a));
}

S32 QSORT_CALLBACK sgLightManager::sgSortLightsByScore(const void* a, const void* b)
{
	return((*(LightInfo**)b)->mScore - (*(LightInfo**)a)->mScore);
}

sgMatInstance* sgLightManager::getDynamicLightingMaterial(MatInstance *root, sgLightInfo *light, bool tryfordual )
{
   sgMatInstanceHook* hook;
   if (!root->mLightingHook)
   {
      hook = new sgMatInstanceHook(root->getMaterial(), root);
      root->mLightingHook = hook;
   } else {
      hook = dynamic_cast<sgMatInstanceHook*>(root->mLightingHook);
   }
   if(hook)
	   return hook->getDynamicLightingMaterial(light, tryfordual);   

   return NULL;
}

bool sgLightManager::lightScene(const char* callback, const char* param)
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
   mSceneLighting = new sgSceneLighting(mLightingInterfaces);
   return mSceneLighting->lightScene(callback, flags);   
}


//---------------------------------------------------------------------------------
//状态块
//
//---------------------------------------------------------------------------------

void sgLightManager::resetStateBlock()
{ 
	//mBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mBlendSB);

	////mAddrClampSB
	//GFX->beginStateBlock();
	//GFX->endStateBlock(mAddrClampSB);
}


void sgLightManager::releaseStateBlock()
{
	if (mBlendSB)
	{
		mBlendSB->release();
	}

	//if (mAddrClampSB)
	//{
	//	mAddrClampSB->release();
	//}
}

void sgLightManager::init()
{
	if (mBlendSB == NULL)
	{
		mBlendSB = new GFXD3D9StateBlock;
		mBlendSB->registerResourceWithDevice(GFX);
		mBlendSB->mZombify = &releaseStateBlock;
		mBlendSB->mResurrect = &resetStateBlock;
		//mAddrClampSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void sgLightManager::shutdown()
{
	SAFE_DELETE(mBlendSB);
	//SAFE_DELETE(mAddrClampSB);
}

//////////////////////////////////////////////////////////////////////////
// sgStatistics
//////////////////////////////////////////////////////////////////////////
void sgStatistics::sgClear()
{
	sgInteriorLexelCount = 0;
	sgInteriorLexelIlluminationCount = 0;
	sgInteriorLexelIlluminationTime = 0;
    sgInteriorLexelDiffuseCount = 0;
	sgInteriorObjectCount = 0;
	sgInteriorObjectIncludedCount = 0;
	sgInteriorObjectIlluminationCount = 0;
	sgInteriorSurfaceIncludedCount = 0;
	sgInteriorSurfaceIlluminationCount = 0;
	sgInteriorSurfaceIlluminatedCount = 0;
    sgInteriorSurfaceSmoothedCount = 0;
    sgInteriorSurfaceSmoothedLexelCount = 0;
    sgInteriorSurfaceSetupTime = 0;
    sgInteriorSurfaceSetupCount = 0;
    sgInteriorSurfaceMergeTime = 0;
    sgInteriorSurfaceMergeCount = 0;
    sgStaticMeshSurfaceOccluderCount = 0;
   sgStaticMeshBVPTPotentialOccluderCount = 0;
   sgStaticMeshCastRayCount = 0;
	sgTerrainLexelCount = 0;
	sgTerrainLexelTime = 0;
}

void sgStatistics::sgPrint()
{
	Con::printf("");
	Con::printf("  Lighting Pack lighting system stats:");
	Con::printf("    Interior Lexel Count:                %d", sgInteriorLexelCount);
   Con::printf("    Interior Lexel Illumination Count:           %d", sgInteriorLexelIlluminationCount);
   Con::printf("    Interior Lexel Illumination Time (ms):       %f", F32(sgInteriorLexelIlluminationTime) / getMax(1.0f, F32(sgInteriorLexelIlluminationCount)));
   Con::printf("    Interior Lexel Illumination Time Total (ms): %d", sgInteriorLexelIlluminationTime);
   Con::printf("    Interior Lexel Diffuse Count:                %d", sgInteriorLexelDiffuseCount);
	Con::printf("    Interior Object Count:               %d", sgInteriorObjectCount);
	Con::printf("    Interior Object Included Count:      %d", sgInteriorObjectIncludedCount);
	Con::printf("    Interior Object Illumination Count:  %d", sgInteriorObjectIlluminationCount);
	Con::printf("    Interior Surface Included Count:     %d", sgInteriorSurfaceIncludedCount);
	Con::printf("    Interior Surface Illumination Count: %d", sgInteriorSurfaceIlluminationCount);
	Con::printf("    Interior Surface Illuminated Count:  %d", sgInteriorSurfaceIlluminatedCount);
   Con::printf("    Interior Surface Smoothed Count:             %d", sgInteriorSurfaceSmoothedCount);
   Con::printf("    Interior Surface Smoothed Lexel Count:       %d", sgInteriorSurfaceSmoothedLexelCount);
   Con::printf("    Interior Surface Setup Count:                %d", sgInteriorSurfaceSetupCount);
   Con::printf("    Interior Surface Setup Time Total (ms):      %d", sgInteriorSurfaceSetupTime);
   Con::printf("    Interior Surface Merge Count:                %d", sgInteriorSurfaceMergeCount);
   Con::printf("    Interior Surface Merge Time Total (ms):      %d", sgInteriorSurfaceMergeTime);
   Con::printf("    Static Mesh BVPT Potential Occluder Count:   %d", sgStaticMeshBVPTPotentialOccluderCount);
   Con::printf("    Static Mesh Cast Ray Mesh Count:             %d", sgStaticMeshCastRayCount);
   Con::printf("    Static Mesh Surface Occluder Count:          %d", sgStaticMeshSurfaceOccluderCount);
	Con::printf("    Terrain Lexel Count:                 %d", sgTerrainLexelCount);
   Con::printf("    Terrain Lexel Time (ms):                     %f", F32(sgTerrainLexelTime) / getMax(1.0f, F32(sgTerrainLexelCount)));
   Con::printf("    Terrain Lexel Time Total (ms):               %d", sgTerrainLexelTime);
}

///////////////////////////////////////////////////////////////////////////
// sgRelightFilter
///////////////////////////////////////////////////////////////////////////

// The WorldEditor triggers this before it renders the scene.
void WorldEditorRenderScene(WorldEditor* we)
{
   sgRelightFilter::sgRenderAllowedObjects(we);	
}

void handleDemoPlay()
{
   SGLM->lightScene("", "");
}

void sgRelightFilter::sgInit()
{   
	Con::addVariable("SceneLighting::FilterRelight", TypeBool, &sgRelightFilter::sgFilterRelight);
	Con::addVariable("SceneLighting::FilterRelightVisible", TypeBool, &sgRelightFilter::sgFilterRelightVisible);
	Con::addVariable("SceneLighting::FilterRelightByDistance", TypeBool, &sgRelightFilter::sgFilterRelightByDistance);
	//Con::addVariable("SceneLighting::FilterRelightByVisiblity", TypeBool, &sgRelightFilter::sgFilterRelightByVisiblity);
	Con::addVariable("SceneLighting::FilterRelightByDistanceRadius", TypeF32, &sgRelightFilter::sgFilterRelightByDistanceRadius);
	Con::addVariable("SceneLighting::FilterRelightByDistancePosition", TypePoint3F, &sgRelightFilter::sgFilterRelightByDistancePosition);
}

bool sgRelightFilter::sgAllowLighting(const Box3F &box, bool forcefilter)
{
#ifndef POWER_PLAYER
	if((sgRelightFilter::sgFilterRelight && sgRelightFilter::sgFilterRelightByDistance) || forcefilter)
	{
		if(!sgRelightFilter::sgFilterRelightVisible)
			return false;

		Point3F min, max;

		min = EditTSCtrl::smCamPos;
		min.x = min.x - sgRelightFilter::sgFilterRelightByDistanceRadius;
		min.y = min.y - sgRelightFilter::sgFilterRelightByDistanceRadius;
		min.z = min.z - sgRelightFilter::sgFilterRelightByDistanceRadius;

		max = EditTSCtrl::smCamPos;
		max.x = max.x + sgRelightFilter::sgFilterRelightByDistanceRadius;
		max.y = max.y + sgRelightFilter::sgFilterRelightByDistanceRadius;
		max.z = max.z + sgRelightFilter::sgFilterRelightByDistanceRadius;

		Box3F lightbox = Box3F(min, max);

		if(!box.isOverlapped(lightbox))
			return false;
	}
#endif
	return true;
}

void sgRelightFilter::sgRenderAllowedObjects(void *editor)
{
#ifndef POWER_PLAYER
	U32 i;
	WorldEditor *worldeditor = (WorldEditor *)editor;
	Vector<SceneObject *> objects;
	gServerContainer.findObjects(InteriorObjectType, sgFindObjectsCallback, &objects);
	for(i = 0; i < objects.size(); i++)
	{
		SceneObject * obj = objects[i];
		if(worldeditor->objClassIgnored(obj))
			continue;

		if(!sgRelightFilter::sgAllowLighting(obj->getWorldBox(), true))
			continue;

		ColorI color = ColorI(255, 0, 255);
		worldeditor->renderObjectBox(obj, color);
	}
#endif
}

