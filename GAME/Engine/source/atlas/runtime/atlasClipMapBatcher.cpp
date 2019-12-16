//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "atlas/resource/atlasResourceGeomTOC.h"
#include "atlas/runtime/atlasClipMapBatcher.h"
#include "materials/shaderData.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "math/mathUtils.h"
#include "materials/sceneData.h"
#include "atlas/runtime/atlasInstance2.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK

GFXStateBlock* AtlasClipMapBatcher::mSetClipmapSB = NULL;
GFXStateBlock* AtlasClipMapBatcher::mSetFogSB = NULL;
GFXStateBlock* AtlasClipMapBatcher::mSetDetailSB = NULL;
GFXStateBlock* AtlasClipMapBatcher::mClearSB = NULL;

bool AtlasClipMapBatcher::smRenderDebugTextures = false;
Vector<AtlasRenderPlugin*> AtlasClipMapBatcher::mRenderPlugins;

AtlasClipMapBatcher::AtlasClipMapBatcher()
{
   mClipMap = NULL;
   
   mDetailMapFadeStartDistance = 30.f;
   mDetailMapFadeEndDistance = 40.f;

   // And have the detail map repeat 128 times across the terrain.
   mDetailMapTextureMultiply = 128.f;

   // Set up our render lists.
   mRenderList[0] = NULL;
   for(S32 i=1; i<4; i++)
      mRenderList[i].clear();

   for (U32 i = 0; i < mRenderPlugins.size(); i++)
   {
      mRenderPlugins[i]->init();
   }

   mIsDetailMappingEnabled = true;
}

void AtlasClipMapBatcher::init(ClipMap *acm, SceneState *state, SceneObject *renderer)
{
   PROFILE_START(AtlasClipMapBatcher_init);

   // Note our render state.
   mClipMap = acm;
   mState = state;
   mObject = renderer;
   
   // Empty the render lists...
   for(S32 i=1; i<4; i++)
      mRenderList[i].clear();
   mDetailList.clear();
   mLightList.clear();
   mFogList.clear();

   // And clear the render notes.
   mRenderNoteAlloc.freeBlocks();

   PROFILE_END(AtlasClipMapBatcher_init);
}

void AtlasClipMapBatcher::queue(const Point3F &camPos, AtlasResourceGeomStub *args)
{
   PROFILE_START(AtlasClipMapBatcher_queue);

   AtlasGeomChunk *agc = args->mChunk;

   Point3F nearPos, farPos;
   Point2F nearTC, farTC;
   agc->calculatePoints(camPos, nearPos, farPos, nearTC, farTC );
   const F32 nearDistance = (camPos - nearPos).len();
   const F32 farDistance = (camPos - farPos).len();

   const RectF texBounds(nearTC, farTC - nearTC);

   // Now, calculate and store levels into a new RenderNote.
   S32 startLevel, endLevel;
   mClipMap->calculateClipMapLevels(nearDistance, farDistance, texBounds, startLevel, endLevel);

   // Allocate a render note.
   RenderNote *rn = mRenderNoteAlloc.alloc();

   // Check if this chunk will get fogged - consider furthest point, and if
   // it'll be fogged then draw a fog pass.
   if(mState->getHazeAndFog(farDistance, farPos.z - camPos.z) > (1.0/256.0)
      || mState->getHazeAndFog(farDistance, -(farPos.z - camPos.z)) > (1.0/256.0))
      mFogList.push_back(rn);

   // Is this chunk within the detail map range?
   if( isDetailMappingEnabled() && nearDistance < mDetailMapFadeEndDistance )
		   mDetailList.push_back(rn);

   // Let our plugins have a shot at the render note.
   for (U32 i = 0; i < mRenderPlugins.size(); i++)
   {
      mRenderPlugins[i]->queue(rn, mObject, args);
   }

   rn->levelStart = startLevel;
   rn->levelEnd   = endLevel;
   rn->levelCount = endLevel - startLevel + 1;
   rn->chunk      = agc;
   rn->nearDist   = nearDistance;

   // Stuff into right list based on shader.
   switch(rn->levelCount)
   {
   case 2:
   case 3:
   case 4:
      mRenderList[rn->levelCount-1].push_back(rn);
      break;

   default:
      Con::errorf("AtlasClipMapBatcher::queue - got unexpected level count of %d", rn->levelCount);
      break;
   }

   AssertFatal(rn->levelCount >= 2 && rn->levelCount <= 4, 
      "AtlasClipMapBatcher::queue - bad level count!");

   PROFILE_END(AtlasClipMapBatcher_queue);
}

S32 AtlasClipMapBatcher::cmpRenderNote(const void *a, const void *b)
{
   RenderNote *fa = *((RenderNote **)a);
   RenderNote *fb = *((RenderNote **)b);

   // Sort by distance.
   if(fa->nearDist > fb->nearDist)
      return 1;
   else if(fa->nearDist < fb->nearDist)
      return -1;

   // So this is what equality is like...
   return 0;
}

void AtlasClipMapBatcher::sort()
{
   PROFILE_START(AtlasClipMapBatcher_sort);

   // Sort our elements. The other lists are already good to go, and there's
   // no benefit to drawing them in a specific order as this pass (the first
   // pass) will already set up the Z buffer.
   for(S32 i=1; i<4; i++)
      dQsort(mRenderList[i].address(),mRenderList[i].size(),sizeof(RenderNote*),cmpRenderNote);

   PROFILE_END(AtlasClipMapBatcher_sort);
}

void AtlasClipMapBatcher::renderClipMap()
{
   // Change tracking state variables.
   U32 numShaderChanges = 0;
   U32 numTextureChanges = 0;
   U32 numRendered = 0;
   GFXShader *lastShader = NULL;
   S32 lastMax = -1;

#ifdef STATEBLOCK
	AssertFatal(mSetClipmapSB, "AtlasClipMapBatcher::renderClipMap -- mSetClipmapSB cannot be NULL.");
	mSetClipmapSB->apply();
#else
	for(S32 i=0; i<4; i++)
	{
		GFX->setTextureStageAddressModeU(i, GFXAddressWrap);
		GFX->setTextureStageAddressModeV(i, GFXAddressWrap);
		GFX->setTextureStageMagFilter(   i, GFXTextureFilterLinear);
		GFX->setTextureStageMipFilter(   i, GFXTextureFilterLinear);
		GFX->setTextureStageMinFilter(   i, GFXTextureFilterLinear);
		GFX->setTextureStageColorOp(     i, GFXTOPModulate);
	}
#endif


   for(S32 curBin=1; curBin<4; curBin++)
   {
      // If bin is empty, skip.
      if(mRenderList[curBin].size() == 0)
         continue;

      // Set up shader.
      numShaderChanges++;
      lastShader = mClipMap->getShader(curBin);
      AssertFatal(lastShader, 
         avar("AtlasClipMapBatcher::render - no shader present for %d level rendering!", curBin));
      GFX->setShader(lastShader);

      for(S32 i=0; i<mRenderList[curBin].size(); i++)
      {
         // Grab the render note.
         const RenderNote *rn = mRenderList[curBin][i];
         numRendered++;

         // Set up clipmap levels.
         if(rn->levelEnd != lastMax)
         {
            numTextureChanges += rn->levelCount;

            // And iterate...
            Point4F shaderConsts[4];

            for(S32 j=rn->levelStart; j <= rn->levelEnd; j++)
            {
               const S32 shaderIdx = rn->levelEnd - j;

               AssertFatal(shaderIdx >= 0 && shaderIdx < mClipMap->mClipStackDepth, 
                  "AtlasClipMapBatcher::render - out of range shaderIdx!");

               Point4F &pt = shaderConsts[shaderIdx];

               // Note the offset and center for this level as well.
               ClipMap::ClipStackEntry &cse = mClipMap->mLevels[j];
               pt.x = cse.mClipCenter.x * cse.mScale;
               pt.y = cse.mClipCenter.y * cse.mScale;
               pt.z = cse.mScale;
               pt.w = 0.f;

               if(smRenderDebugTextures)
                  GFX->setTexture(shaderIdx, cse.mDebugTex);
               else
                  GFX->setTexture(shaderIdx, cse.mTex);
            }

            // Set all the constants in one go.
            GFX->setVertexShaderConstF(50, &shaderConsts[0].x, rn->levelCount);
         }

         // Alright, we're all ready to go - so issue the draw.
         rn->chunk->render();
      }
   }

   // Don't forget to clean up.
   for(S32 i=0; i<4; i++)
      GFX->setTexture(0, NULL);


   /*Con::printf("AtlasClipMapBatcher::render - %d shader changes, %d texture changes, %d chunks drawn.",
      numShaderChanges, numTextureChanges, numRendered);*/
}

void AtlasClipMapBatcher::renderFog()
{
   // Grab the shader for this pass - replaceme w/ real code.
   ShaderData *sd;
   if(!Sim::findObject("AtlasShaderFog", sd) || !sd->getShader())
   {
      Con::errorf("AtlasClipMapBatcher::renderFog - no shader 'AtlasShaderFog' present!");
      return;
   }

   // Set up the fog shader and texture.
   GFX->setShader(sd->getShader());

   Point4F fogConst(
         gClientSceneGraph->getFogHeightOffset(), 
         gClientSceneGraph->getFogInvHeightRange(), 
         gClientSceneGraph->getVisibleDistanceMod(), 
         0);
   GFX->setVertexShaderConstF(22, &fogConst[0], 1);

   GFX->setTexture(0, gClientSceneGraph->getFogTexture());

#ifdef STATEBLOCK
	AssertFatal(mSetFogSB, "AtlasClipMapBatcher::renderFog -- mSetFogSB cannot be NULL.");
	mSetFogSB->apply();
#else
   GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
   GFX->setTextureStageAddressModeV(0, GFXAddressClamp);

   // We need the eye pos but AtlasInstance2 deals with setting this up.

   // Set blend mode and alpha test as well.
   GFX->setAlphaBlendEnable(true);
   GFX->setSrcBlend(GFXBlendSrcAlpha);
   GFX->setDestBlend(GFXBlendInvSrcAlpha);

   GFX->setAlphaTestEnable(true);
   GFX->setAlphaFunc(GFXCmpGreaterEqual);
   GFX->setAlphaRef(2);

#endif


   // And render all the fogged chunks - all for now.
   for(S32 i=0; i<mFogList.size(); i++)
   {
      // Grab the render note.
      const RenderNote *rn = mFogList[i];

      // And draw it.
      rn->chunk->render();
   }

#ifdef STATEBLOCK
	AssertFatal(mClearSB, "AtlasClipMapBatcher::renderFog -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
	GFX->setAlphaBlendEnable(false);
	GFX->setAlphaTestEnable(false);
#endif


   // Don't forget to clean up.
   for(S32 i=0; i<4; i++)
      GFX->setTexture(0, NULL);
}

void AtlasClipMapBatcher::renderDetail()
{
   // Grab the shader for this pass - replaceme w/ real code.
   ShaderData *sd;
   if(!Sim::findObject("AtlasShaderDetail", sd))
   {
      Con::errorf("AtlasClipMapBatcher::renderDetail - no shader 'AtlasShaderDetail' present!");
      return;
   }

   // Set up the fog shader and texture.
   GFX->setShader(sd->getShader());

   Point4F detailConst(
      mDetailMapFadeStartDistance,
      1.f / (mDetailMapFadeEndDistance - mDetailMapFadeStartDistance), 
      mDetailMapTextureMultiply,
      0);
   GFX->setVertexShaderConstF(50, &detailConst[0], 1);

   GFX->setTexture(0, ((AtlasInstance*)mObject)->mDetailTex);
#ifdef STATEBLOCK
	AssertFatal(mSetDetailSB, "AtlasClipMapBatcher::renderDetail -- mSetDetailSB cannot be NULL.");
	mSetDetailSB->apply();
   // Set the brightness scale.
   Point4F brightnessScale(1.f, 0, 0, 0);
   GFX->setPixelShaderConstF(0, &brightnessScale.x, 1);
#else
   GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
   GFX->setTextureStageAddressModeV(0, GFXAddressWrap);

   // We need the eye pos but AtlasInstance2 deals with setting this up.

   // Set the brightness scale.
   Point4F brightnessScale(1.f, 0, 0, 0);
   GFX->setPixelShaderConstF(0, &brightnessScale.x, 1);

   // This blend works, so don't question it. (It should be a multiply blend.)
   GFX->setAlphaBlendEnable(true);
   GFX->setSrcBlend(GFXBlendDestColor);
   GFX->setDestBlend(GFXBlendSrcColor);
#endif


   // And render all the detailed chunks - all for now.
   for(S32 i=0; i<mDetailList.size(); i++)
   {
      // Grab the render note.
      const RenderNote *rn = mDetailList[i];

      // And draw it.
      rn->chunk->render();
   }

#ifdef STATEBLOCK
	AssertFatal(mClearSB, "AtlasClipMapBatcher::renderDetail -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
	GFX->setAlphaBlendEnable(false);
	GFX->setAlphaTestEnable(false);
#endif
   // Don't forget to clean up.
   for(S32 i=0; i<4; i++)
      GFX->setTexture(0, NULL);

   GFX->setShader(NULL);
}

void AtlasClipMapBatcher::render()
{
   renderClipMap();

   if( isDetailMappingEnabled() )
	   renderDetail();

   // Let our plugins have a shot at rendering
   // We have to build a SceneGraphData here for now to make the material happy...
   SceneGraphData sgData;
   sgData.setDefaultLights();
   sgData.camPos = mState->getCameraPosition();
   sgData.objTrans = mObject->getRenderTransform();
   for (U32 i = 0; i < mRenderPlugins.size(); i++)
   {
      mRenderPlugins[i]->render(sgData, mObject); 
   }

   renderFog();
}

void AtlasClipMapBatcher::registerRenderPlugin( AtlasRenderPlugin* arp )
{
   mRenderPlugins.push_back(arp);
}

void AtlasClipMapBatcher::unregisterRenderPlugin( AtlasRenderPlugin* arp )
{   
   for (S32 i = mRenderPlugins.size() - 1; i >= 0; i--)
   {
      if (mRenderPlugins[i] == arp)
         mRenderPlugins.erase(i);
   }
}



void AtlasClipMapBatcher::resetStateBlock()
{
	//mSetClipmapSB
	GFX->beginStateBlock();
	for(S32 i=0; i<4; i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
		GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPModulate);
	}
	GFX->endStateBlock(mSetClipmapSB);

	//mSetFogSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 2);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual);
	GFX->endStateBlock(mSetFogSB);

	//mSetDetailSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendSrcColor);
	GFX->endStateBlock(mSetDetailSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->endStateBlock(mClearSB);

}


void AtlasClipMapBatcher::releaseStateBlock()
{
	if (mSetClipmapSB)
	{
		mSetClipmapSB->release();
	}

	if (mSetFogSB)
	{
		mSetFogSB->release();
	}

	if (mSetDetailSB)
	{
		mSetDetailSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void AtlasClipMapBatcher::initsb()
{
	if (mSetClipmapSB == NULL)
	{
		mSetClipmapSB = new GFXD3D9StateBlock;
		mSetClipmapSB->registerResourceWithDevice(GFX);
		mSetClipmapSB->mZombify = &releaseStateBlock;
		mSetClipmapSB->mResurrect = &resetStateBlock;

		mSetFogSB = new GFXD3D9StateBlock;
		mSetDetailSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void AtlasClipMapBatcher::shutdown()
{
	SAFE_DELETE(mSetClipmapSB);
	SAFE_DELETE(mSetFogSB);
	SAFE_DELETE(mSetDetailSB);
	SAFE_DELETE(mClearSB);
}

