//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/shadowCaster.h"
#include "gfx/gfxDevice.h"
#include "sceneGraph/sceneGraph.h"
#include "ts/tsShapeInstance.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/common/blobShadow.h"
#include "lightingSystem/synapseGaming/sgObjectBasedProjector.h"
#include "lightingSystem/synapseGaming/shadowCasterManager.h"
#include "Gameplay/Data/GraphicsProfile.h"

const U32 SHADOW_UNUSED_TIMEOUT = 3000;


ShadowCaster::ShadowCaster()
{
   mIsRegistered = false;
   mLastRenderTime = 0;
   mIsStaticObject = false;

   mSimpleShadow = NULL;
}

ShadowCaster::~ShadowCaster()
{
	if(mSimpleShadow)
		delete mSimpleShadow;

	// Delete our shadows
   clearShadowMultimap();
}

void ShadowCaster::render( SceneObject* parentObject, TSShapeInstance* shapeInstance, F32 camDist, bool forceUpdate /*= false*/ )
{
	bool simpleFlag = (GFX->getPixelShaderVersion() < 0.001);
	if(!simpleFlag && (parentObject->getType() & GameBaseObjectType) )
	{
		if(g_GraphicsProfile.mShadowLevel==1)
			simpleFlag = true;
		else if(g_GraphicsProfile.mShadowLevel==2 && !parentObject->isSelfPlayer())
		{
			simpleFlag = true;
		}
	}

	// If we're rendering, we probably have shadows.  If we have shadows, we need to be
   // registered.
   if(!mIsRegistered)
   {
      ShadowCasterManager::registerShadowCaster(this);
      mIsRegistered = true;
   }

   U32 time = Platform::getRealMilliseconds();
   RectI viewport = GFX->getViewport();
   MatrixF proj = GFX->getProjectionMatrix();

   // For getting a list of lights
   SGLM->setupLights(parentObject);

   // The lights which affect our parent the most
   LightInfoList lights;
   SGLM->getBestLights(lights);

   if(simpleFlag)		//Ray: 修改简单阴影
   {
	   if(lights.size())
	   {
			if(!mSimpleShadow)
			{
				sgLightInfo *light = static_cast<sgLightInfo*>(lights[0]);
				mSimpleShadow = new BlobShadow(parentObject, light, shapeInstance);
			}

			// Make sure the shadow wants to be rendered
			if(mSimpleShadow->shouldRender(camDist))
			{
				// Render! (and note the time)
				mLastRenderTime = time;
				mSimpleShadow->preRender(camDist);
			}
	   }
   }
   else
   {
	   if(lights.size())   //Ray: 目前只受太阳光的阴影
	   {
		   SG_CHECK_LIGHT(lights[0]);
		   sgLightInfo *light = static_cast<sgLightInfo*>(lights[0]);
		   if(light->sgCastsShadows)
		   {
			   // If the light casts a shadow, render a shadow
			   ShadowBase *shadow = lookupShadow(parentObject, light, shapeInstance);
			   AssertFatal((shadow), "Shadow not found?");

			   // Make sure the shadow wants to be rendered
			   if(shadow->shouldRender(camDist))
			   {
				   // Render! (and note the time)
				   mLastRenderTime = time;
				   shadow->preRender(camDist);
			   }
		   }
	   }
   }

   // Remove the dirty flag
   shapeInstance->shadowDirty = false;

   // Reset the lights
   SGLM->resetLights();
   GFX->setProjectionMatrix(proj);
   GFX->setViewport(viewport);
}

ShadowBase* ShadowCaster::createNewShadow(SceneObject* parentObject, LightInfo* light, TSShapeInstance* shapeInstance)
{
   // Create a shadow
   ShadowBase* shadow = NULL;

   if(GFX->getPixelShaderVersion() < 0.001)
   {
      // No shaders, use a blob shadow
      shadow = new BlobShadow(parentObject, light, shapeInstance);
   }
   else
   {
      // Shaders, use a real shadow
      shadow = new sgShadowProjector(parentObject, light, shapeInstance);
   }
   return shadow;
}

ShadowBase* ShadowCaster::lookupShadow( SceneObject* parentObject,  LightInfo* light, TSShapeInstance* shapeInstance )
{
	//Ray:目前只有一个光有影子效果，而且light指针地址都是临时变化的，导致lightToHash不断创建新entry。因此去掉
   ShadowMultimap* entry = mShadowMultimap.find(0/*lightToHash(light)*/);

   if(entry->info)
      return entry->info;

   // Ack, no shadow! Create one
   ShadowBase* shadow = createNewShadow(parentObject, light, shapeInstance);
   AssertFatal(shadow, "We didn't create a shadow?");

   entry->info = shadow;
   return entry->info;
}

ShadowMultimap* ShadowCaster::getFirstShadowEntry()
{
   return mShadowMultimap.find(0);
}

void ShadowCaster::clearShadowMultimap()
{
   // Loop through the entire multimap
   ShadowMultimap *entry = getFirstShadowEntry();
   while(entry)
   {
      // We have a shadow, delete it
      if(entry->info)
      {
         delete entry->info;
         entry->info = NULL;
      }

      entry = entry->linkHigh;
   }

   // all shadows are deleted, so nothing left to monitor...
   if(mIsRegistered)
   {
      ShadowCasterManager::unregisterShadowCaster(this);
      mIsRegistered = false;
   }
}

void ShadowCaster::cleanupUnusedShadows(U32 time)
{
   if(time<mLastRenderTime)
   {
      return;
   }

   // try to ditch the whole thing first...
   if((time - mLastRenderTime) > SHADOW_UNUSED_TIMEOUT)
   {
      clearShadowMultimap();
      return;
   }

   // no? alright lets try to get rid of some old shadows...
   ShadowMultimap *entry = getFirstShadowEntry();
   while(entry)
   {
      if(entry->info)
      {
         if((time - entry->info->getLastRenderTime()) > SHADOW_UNUSED_TIMEOUT)
         {
            delete entry->info;
            entry->info = NULL;
         }
      }
      entry = entry->linkHigh;
   }
}

void ShadowCaster::clearAllShadows()
{
   clearShadowMultimap();
}

void ShadowCaster::resetShapeInst(TSShapeInstance* shapeInst)
{
	ShadowMultimap *entry = getFirstShadowEntry();
	while(entry)
	{
		if(entry->info)
		{
			entry->info->resetShapeInst(shapeInst);
		}
		entry = entry->linkHigh;
	}
}

void ShadowCaster::saveDynamicShadowData(SceneObject* parentObject, TSShapeInstance* shapeInstance, Stream *stream)
{
	if(!mIsRegistered)
	{
		ShadowCasterManager::registerShadowCaster(this);
		mIsRegistered = true;
	}

	//Ray: 目前只保存受太阳光影响的阴影
	SGLM->setupLights(parentObject);
	LightInfoList lights;
	SGLM->getBestLights(lights);
	LightInfo *pDefaultLightInfo = lights[0];
	ShadowBase *shadow = lookupShadow(parentObject, pDefaultLightInfo, shapeInstance);
	shadow->saveDynamicShadowData(stream);
	SGLM->resetLights();
}

void ShadowCaster::setDynamicShadowData(SceneObject* parentObject, TSShapeInstance* shapeInstance, void *p1,void *p2)
{
	if(!mIsRegistered)
	{
		ShadowCasterManager::registerShadowCaster(this);
		mIsRegistered = true;
	}

	mIsStaticObject = true;

	//Ray: 目前只保存默认光影响的阴影
	SGLM->setupLights(parentObject);
	LightInfoList lights;
	SGLM->getBestLights(lights);
	LightInfo *pDefaultLightInfo = lights[0];
	ShadowBase *shadow = lookupShadow(parentObject, pDefaultLightInfo, shapeInstance);
	shadow->setDynamicShadowData(p1,p2);
	SGLM->resetLights();
}

void ShadowCaster::freeResource()
{
	// no? alright lets try to get rid of some old shadows...
	ShadowMultimap *entry = getFirstShadowEntry();
	while(entry)
	{
		if(entry->info)
		{
			entry->info->freeResource();
		}
		entry = entry->linkHigh;
	}
}
