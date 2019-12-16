//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#include "sgAtlasDynamicLighting.h"
//#include "atlas/runtime/atlasClipMapBatcher.h"
#include "atlas/resource/atlasResourceGeomTOC.h"
#include "materials/matInstance.h"
#include "math/mathUtils.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK

GFXStateBlock* AtlasLightingPlugin::mSetSB = NULL;
//class AtlasLightingPlugin : public AtlasRenderPlugin
//{
//private:
//   MatInstance* mLightMatInst;
//   MatInstance* mLightMaskMatInst;
//   bool mMatInit;
//   bool mMatOk;
//
//   Vector<AtlasClipMapBatcher::RenderNote*> mRenderNotes;
//public:
//   AtlasLightingPlugin();
//   ~AtlasLightingPlugin();
//
//   virtual void init();
//   virtual void queue(AtlasClipMapBatcher::RenderNote* rn, SceneObject* mObject, AtlasResourceGeomStub *args);  
//   virtual void render(SceneGraphData& sgData, SceneObject* obj);
//};

AtlasLightingPlugin p_AtlasLightingPlugin;

AtlasLightingPlugin::AtlasLightingPlugin()
{
   AtlasClipMapBatcher::registerRenderPlugin(this);
   mMatInit = false;
   mMatOk = true;
}

AtlasLightingPlugin::~AtlasLightingPlugin()
{
   AtlasClipMapBatcher::unregisterRenderPlugin(this);
   SAFE_DELETE(mLightMatInst);
   SAFE_DELETE(mLightMaskMatInst);
}

void AtlasLightingPlugin::init()
{
   if (mMatInit)
      return;

   mMatInit = false;
   mMatOk = true;

   // Get the lighting material.
   mLightMatInst = new MatInstance("AtlasDynamicLightingMaterial");
   if (!mLightMatInst->getMaterial())
   {
      mMatOk = false;
      Con::errorf("AtlasClipMapBatcher::renderLight - no material 'AtlasDynamicLightingMaterial' found, skipping light rendering and spamming the console.");
   }

   // Get the lighting mask material.
   mLightMaskMatInst = new MatInstance("AtlasDynamicLightingMaskMaterial");
   if (!mLightMaskMatInst->getMaterial())
   {
      mMatOk = false;
      Con::errorf("AtlasClipMapBatcher::renderLight - no material 'AtlasDynamicLightingMaskMaterial' found, skipping light rendering and spamming the console.");
   }
}

void AtlasLightingPlugin::queue(AtlasClipMapBatcher::RenderNote* rn, SceneObject* mObject, AtlasResourceGeomStub *args )
{
   // convert bounds to world space... (can we cache this?)
   Box3F box;
   MathUtils::transformBoundingBox(args->mBounds, mObject->getRenderTransform(), Point3F(1.0f, 1.0f, 1.0f), box);

   // Ok, now let's see if we have any lighting to do...   
   static LightInfoList lights;
   lights.clear();

   SGLM->setupLights(mObject, box, sgLightManager::sgAtlasMaxDynamicLights);
   SGLM->getBestLights(lights);

   // We do, add it. This is a little icky cuz we have to recalc the lights when
   // we render.
   if(lights.size())
      mRenderNotes.push_back(rn);

   SGLM->resetLights();   
}

void AtlasLightingPlugin::render(SceneGraphData& sgData, SceneObject* obj)
{   
   if (!mMatInit)
   {
      mLightMatInst->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXAtlasVert2*)NULL));      
      mLightMaskMatInst->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXAtlasVert2*)NULL));   
      mMatInit = true;
   }

   // Move frequently used things outside the loop.
   Box3F box;
   static LightInfoList lights;
   lights.clear();

   // And draw the light list!
   for(S32 i=0; i<mRenderNotes.size(); i++)
   {
      AtlasClipMapBatcher::RenderNote *&rn = mRenderNotes[i];
      lights.clear();

      // convert bounds to world space...
      MathUtils::transformBoundingBox(rn->chunk->mBounds, sgData.objTrans, Point3F(1.0f, 1.0f, 1.0f), box);

      SGLM->setupLights(obj, box, sgLightManager::sgAtlasMaxDynamicLights);
      SGLM->getBestLights(lights);

      GFX->setTexture(0, NULL);

      for(U32 l=0; l<lights.size(); l++)
      {
         SG_CHECK_LIGHT(lights[l]);         
         sgLightInfo* light = static_cast<sgLightInfo*>(lights[l]);

         // Filter light types...
         if((light->mType != LightInfo::Point) && (light->mType != LightInfo::Spot))
            continue;

         light->setSceneGraphData(&sgData);
                  
         MatInstance *mat = NULL;
         if(light->sgLightMask)
            mat = mLightMaskMatInst;
         else
            mat = mLightMatInst;

         while(mat->setupPass(sgData))
         {

#ifdef STATEBLOCK
				AssertFatal(mSetSB, "AtlasLightingPlugin::render -- mSetSB cannot be NULL.");
				mSetSB->apply();  
#else
				GFX->setAlphaBlendEnable(true);
				GFX->setSrcBlend(GFXBlendSrcAlpha);
				GFX->setDestBlend(GFXBlendOne);

				GFX->setAlphaTestEnable(true);
				GFX->setAlphaFunc(GFXCmpGreater);
				GFX->setAlphaRef(0);
#endif
            rn->chunk->render();
         }

         sgData.dynamicLight = NULL;
      }

      SGLM->resetLights();
   }
   mRenderNotes.clear();
}


void AtlasLightingPlugin::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreater);
	GFX->endStateBlock(mSetSB);


}


void AtlasLightingPlugin::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void AtlasLightingPlugin::initsb()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void AtlasLightingPlugin::shutdown()
{
	SAFE_DELETE(mSetSB);
}
