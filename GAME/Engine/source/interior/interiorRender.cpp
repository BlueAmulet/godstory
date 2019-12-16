//-----------------------------------------------------------------------------
// PowerEngine Advanced
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "interior/interior.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/lightManager.h"

#include "gfx/gBitmap.h"
#include "math/mMatrix.h"
#include "math/mRect.h"
#include "core/bitVector.h"
#include "platform/profiler.h"
#include "gfx/gfxDevice.h"
#include "interior/interiorInstance.h"
#include "gfx/gfxTextureHandle.h"
#include "materials/materialList.h"
#include "materials/sceneData.h"
#include "materials/matInstance.h"
#include "T3D/fx/glowBuffer.h"
#include "materials/customMaterial.h"
#include "math/mathUtils.h"
#include "renderInstance/renderInstMgr.h"
#include "core/frameAllocator.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* Interior::mCullCWSB = NULL;
GFXStateBlock* Interior::mCullCCWSB = NULL;
GFXStateBlock* Interior::mSetPreBatSB = NULL;

extern bool sgFogActive;
extern U16* sgActivePolyList;
extern U32  sgActivePolyListSize;
extern U16* sgFogPolyList;
extern U32  sgFogPolyListSize;
extern U16* sgEnvironPolyList;
extern U32  sgEnvironPolyListSize;

Point3F sgOSCamPosition;


U32         sgRenderIndices[2048];
U32         csgNumAllowedPoints = 256;

extern "C" {
   F32   texGen0[8];
   F32   texGen1[8];
   Point2F *fogCoordinatePointer;
}

InteriorLightPlugin* Interior::smLightPlugin = NULL;

//------------------------------------------------------------------------------
// Set up render states for interor rendering
//------------------------------------------------------------------------------
void Interior::setupRenderStates()
{
   GFX->setBaseRenderState();
#ifdef STATEBLOCK

	if( gClientSceneGraph->isReflectPass() )
	{
		AssertFatal(mCullCWSB, "Interior::setupRenderStates -- mCullCWSB cannot be NULL.");
		mCullCWSB->apply();
	}
	else
	{
		AssertFatal(mCullCCWSB, "Interior::setupRenderStates -- mCullCCWSB cannot be NULL.");
		mCullCCWSB->apply();
	}
#else
   for( U32 i=0; i<GFX->getNumSamplers(); i++ )
   {
      GFX->setTextureStageAddressModeU( i, GFXAddressWrap );
      GFX->setTextureStageAddressModeV( i, GFXAddressWrap );
      GFX->setTextureStageMagFilter( i, GFXTextureFilterLinear );
      GFX->setTextureStageMinFilter( i, GFXTextureFilterLinear );
      GFX->setTextureStageMipFilter( i, GFXTextureFilterLinear );
   }

   // set up states for fixed function materials
   GFX->setTextureStageColorOp( 0, GFXTOPModulate );
   GFX->setTextureStageColorOp( 1, GFXTOPModulate );
   GFX->setTextureStageColorOp( 2, GFXTOPDisable );

   // set culling
   if( gClientSceneGraph->isReflectPass() )
   {
      GFX->setCullMode( GFXCullCW );
   }
   else
   {
      GFX->setCullMode( GFXCullCCW );
   }
#endif


}

//------------------------------------------------------------------------------
// Setup zone visibility
//------------------------------------------------------------------------------
ZoneVisDeterminer Interior::setupZoneVis( InteriorInstance *intInst, SceneState *state )
{

   U32 zoneOffset = intInst->getZoneRangeStart() != 0xFFFFFFFF ? intInst->getZoneRangeStart() : 0;

   U32 baseZone = 0xFFFFFFFF;

   if (intInst->getNumCurrZones() == 1)
   {
      baseZone = intInst->getCurrZone(0);
   }
   else
   {
      for (U32 i = 0; i < intInst->getNumCurrZones(); i++)
      {
         if (state->getZoneState(intInst->getCurrZone(i)).render == true)
         {
            if (baseZone == 0xFFFFFFFF) {
               baseZone = intInst->getCurrZone(i);
               break;
            }
         }
      }
      if (baseZone == 0xFFFFFFFF)
      {
         baseZone = intInst->getCurrZone(0);
      }
   }

   ZoneVisDeterminer zoneVis;
   zoneVis.runFromState(state, zoneOffset, baseZone);
   return zoneVis;
}

//------------------------------------------------------------------------------
// Setup scenegraph data structure for materials
//------------------------------------------------------------------------------
SceneGraphData Interior::setupSceneGraphInfo( InteriorInstance *intInst,
                                              SceneState *state )
{
   SceneGraphData sgData;

   LightManager* lm = gClientSceneGraph->getLightManager();
   sgData.light = lm->getSpecialLight(LightManager::slTranslatedSunlightType);
   sgData.lightSecondary = lm->getDefaultLight();
   
   // fill in camera position relative to interior
   sgData.camPos = state->getCameraPosition();

   // fill in interior's transform
   sgData.objTrans = intInst->getTransform();

   // fog
   sgData.setFogParams();

   // misc
   sgData.backBuffTex = GFX->getSfxBackBuffer();

   return sgData;
}

//------------------------------------------------------------------------------
// Render zone RenderNode
//------------------------------------------------------------------------------
void Interior::renderZoneNode( RenderNode &node,
                               InteriorInstance *intInst,
                               SceneGraphData &sgData,
                               RenderInst *coreRi )
{
   //static U16 curBaseTexIndex = 0;

   RenderInst *ri = gRenderInstManager.allocInst(false);
   *ri = *coreRi;


   // setup lightmap
   if( node.lightMapIndex != U8(-1) )
   {
      ri->lightmap = gInteriorLMManager.getHandle(mLMHandle, intInst->getLMHandle(), node.lightMapIndex );

      /*if( node.exterior )
      {
         sgData.normLightmap = NULL;
         sgData.useLightDir = true;
      }
      else
      {*/
         ri->normLightmap = gInteriorLMManager.getNormalHandle(mLMHandle, intInst->getLMHandle(), node.lightMapIndex);
         //ri->useLightDir = false;
      //}
   }

   // setup base map
   //if( node.baseTexIndex )
   //{
   //   curBaseTexIndex = node.baseTexIndex;
   //}

   MatInstance *mat = node.matInst;
   if( mat )
   {

      ri->matInst = mat;
      ri->primBuffIndex = node.primInfoIndex;
      gRenderInstManager.addInst( ri );

   }
}

//------------------------------------------------------------------------------
// Render zone RenderNode
//------------------------------------------------------------------------------
void Interior::renderReflectNode( ReflectRenderNode &node,
                                  InteriorInstance *intInst,
                                  SceneGraphData &sgData,
                                  RenderInst *coreRi )
{
   RenderInst *ri = gRenderInstManager.allocInst(false);
   *ri = *coreRi;

   ri->vertBuff = &mReflectVertBuff;
   ri->primBuff = &mReflectPrimBuff;

   static U16 curBaseTexIndex = 0;

   // use sgData.backBuffer to transfer the reflect texture to the materials
   ReflectPlane *rp = &intInst->mReflectPlanes[ node.reflectPlaneIndex ];
   ri->backBuffTex = rp->getTex();
   ri->reflective = true;

   // setup lightmap
   if( node.lightMapIndex != U8(-1) )
   {
      ri->lightmap = gInteriorLMManager.getHandle(mLMHandle, intInst->getLMHandle(), node.lightMapIndex );
      ri->normLightmap = gInteriorLMManager.getNormalHandle(mLMHandle, intInst->getLMHandle(), node.lightMapIndex);
   }

   MatInstance *mat = node.matInst;
   if( mat )
   {
      ri->matInst = mat;
      ri->primBuffIndex = node.primInfoIndex;
      gRenderInstManager.addInst( ri );

   }
}


//------------------------------------------------------------------------------
// Setup the rendering
//------------------------------------------------------------------------------
void Interior::setupRender( InteriorInstance *intInst,
                            SceneState *state,
                            RenderInst *coreRi )
{
   // setup world matrix - for fixed function
   MatrixF world = GFX->getWorldMatrix();
   world.mul( intInst->getRenderTransform() );
   world.scale( intInst->getScale() );
   GFX->setWorldMatrix( world );

   // setup world matrix - for shaders
   MatrixF proj = GFX->getProjectionMatrix();
   proj.mul(world);
   proj.transpose();
   GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );

   // set buffers
   GFX->setVertexBuffer( mVertBuff );
   GFX->setPrimitiveBuffer( mPrimBuff );


   // setup renderInst
   coreRi->worldXform = gRenderInstManager.allocXform();
   *coreRi->worldXform = proj;

   coreRi->vertBuff = &mVertBuff;
   coreRi->primBuff = &mPrimBuff;

   coreRi->objXform = gRenderInstManager.allocXform();
   *coreRi->objXform = intInst->getRenderTransform();

   // grab the sun data from the light manager
   /*Vector<LightInfo*> lights;
   gClientSceneGraph->getLightManager()->getLights(lights);

   if( lights.size() > 0 )
   {
      coreRi->ambient = lights[0]->mAmbient;
      coreRi->diffuse = lights[0]->mColor;
      coreRi->lightDir = lights[0]->mDirection;
   }*/

   coreRi->type = RenderInstManager::RIT_Interior;
   coreRi->backBuffTex = GFX->getSfxBackBuffer();
}


//------------------------------------------------------------------------------
// Render
//------------------------------------------------------------------------------
void Interior::prepBatchRender( InteriorInstance *intInst, SceneState *state )
{
   // coreRi - used as basis for subsequent interior ri allocations
   RenderInst *coreRi = gRenderInstManager.allocInst();
   SceneGraphData sgData;
   setupRender( intInst, state, coreRi );
   ZoneVisDeterminer zoneVis = setupZoneVis( intInst, state );

#ifndef POWER_SHIPPING
   if( smRenderMode != 0 )
   {
      RenderInst *ri = gRenderInstManager.allocInst();
      ri->obj = intInst;
      ri->state = state;
      ri->type = RenderInstManager::RIT_Object;
      gRenderInstManager.addInst( ri );
      return;
   }
#endif

   // render zones
   for( U32 i=0; i<mZones.size(); i++ )
   {
      // No need to try to render zones without any surfaces
      if (mZones[i].surfaceCount == 0)
         continue;

      if( zoneVis.isZoneVisible(i) == false && !gClientSceneGraph->isReflectPass() )
      {
         continue;
      }

      for( U32 j=0; j<mZoneRNList[i].renderNodeList.size(); j++ )
      {
         RenderNode &node = mZoneRNList[i].renderNodeList[j];
         renderZoneNode( node, intInst, sgData, coreRi );
      }
   }
   
   // render static meshes...
   for(U32 i=0; i<mStaticMeshes.size(); i++)
      mStaticMeshes[i]->render(*coreRi, getLMHandle(), intInst->getLMHandle(), NULL, NULL, NULL, NULL);

   // render reflective surfaces
   if( !gClientSceneGraph->isReflectPass() )
   {
      renderLights(intInst, sgData, coreRi, zoneVis);

      GFX->setVertexBuffer( mReflectVertBuff );
      GFX->setPrimitiveBuffer( mReflectPrimBuff );
#ifdef STATEBLOCK
		AssertFatal(mSetPreBatSB, "Interior::prepBatchRender -- mSetPreBatSB cannot be NULL.");
		mSetPreBatSB->apply();
#else
      // hack - need to address wrapping/clamping at some point
      GFX->setTextureStageAddressModeU( 2, GFXAddressWrap );
      GFX->setTextureStageAddressModeV( 2, GFXAddressWrap );
#endif


      for( U32 i=0; i<mZones.size(); i++ )
      {
         if( zoneVis.isZoneVisible(i) == false ) continue;

         for( U32 j=0; j<mZoneReflectRNList[i].reflectList.size(); j++ )
         {
            ReflectRenderNode &node = mZoneReflectRNList[i].reflectList[j];
            renderReflectNode( node, intInst, sgData, coreRi );
         }
      }
   }


   GFX->setBaseRenderState();
}

void Interior::registerLightPlugin(InteriorLightPlugin* ilp)
{
   AssertFatal(ilp, "Null lighting plugin not allowed!");
   smLightPlugin = ilp;
}

void Interior::unregisterLightPlugin(InteriorLightPlugin* ilp)
{
   smLightPlugin = NULL;
}


void Interior::renderLights(InteriorInstance *intInst, SceneGraphData &sgData,
		RenderInst *coreRi, const ZoneVisDeterminer &zonevis)
{
   if (!smLightPlugin)
      return;

   if (!smLightPlugin->interiorInstInit(intInst))
      return;

	// build the render instances...
	for(U32 z=0; z<mZones.size(); z++)
	{
		if(!zonevis.isZoneVisible(z))
			continue;

		Zone &zone = mZones[z];
		S32 zoneid = zone.zoneId - 1;
		if(zoneid > -1)
			zoneid += intInst->getZoneRangeStart();// only zone managers...
		else
			zoneid = intInst->getCurrZone(0);// if not what zone is it in...

      if (!smLightPlugin->zoneInit(zoneid))
         continue;

      static Vector<RenderInst*> sRenderList;
      sRenderList.clear();

		for(U32 j=0; j<mZoneRNList[z].renderNodeList.size(); j++)
		{
			RenderNode &node = mZoneRNList[z].renderNodeList[j];

			if (!node.matInst)
				continue;

			RenderInst *ri = gRenderInstManager.allocInst(false);
			*ri = *coreRi;
			ri->type = RenderInstManager::RIT_InteriorDynamicLighting;
			ri->matInst = node.matInst;
			ri->primBuffIndex = node.primInfoIndex;

         sRenderList.push_back(ri);
		}		
      smLightPlugin->processRI(sRenderList);
	}
}


void Interior::resetStateBlock()
{
	//mSetPreBatSB
	GFX->beginStateBlock();
	GFX->setSamplerState(2, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(2, GFXSAMPAddressV, GFXAddressWrap);
	GFX->endStateBlock(mSetPreBatSB);

	//mCullCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCW);
	for(S32 i=0; i<GFX->getNumSamplers(); i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressClamp);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressClamp);
	}
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(2, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mCullCWSB);

	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	for(S32 i=0; i<GFX->getNumSamplers(); i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressClamp);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressClamp);
	}
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(2, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mCullCCWSB);
}


void Interior::releaseStateBlock()
{
	if (mSetPreBatSB)
	{
		mSetPreBatSB->release();
	}

	if (mCullCWSB)
	{
		mCullCWSB->release();
	}

	if (mCullCCWSB)
	{
		mCullCCWSB->release();
	}
}

void Interior::init()
{
	if (mSetPreBatSB == NULL)
	{
		mSetPreBatSB = new GFXD3D9StateBlock;
		mSetPreBatSB->registerResourceWithDevice(GFX);
		mSetPreBatSB->mZombify = &releaseStateBlock;
		mSetPreBatSB->mResurrect = &resetStateBlock;

		mCullCWSB = new GFXD3D9StateBlock;
		mCullCCWSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void Interior::shutdown()
{
	SAFE_DELETE(mCullCWSB);
	SAFE_DELETE(mCullCCWSB);
	SAFE_DELETE(mSetPreBatSB);
}
