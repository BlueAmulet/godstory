//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderInteriorMgr.h"
#include "materials/sceneData.h"
#include "sceneGraph/sceneGraph.h"
#include "materials/matInstance.h"
#include "gfx/gfxTransformSaver.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* RenderInteriorMgr::mCullCWSB = NULL;
GFXStateBlock* RenderInteriorMgr::mCullCCWSB = NULL;
GFXStateBlock* RenderInteriorMgr::mClearSB = NULL;

//**************************************************************************
// RenderInteriorMgr
//**************************************************************************

void RenderInteriorMgr::setupSGData( RenderInst *ri, SceneGraphData &data )
{
   Parent::setupSGData(ri, data);
   ri->miscTex = NULL;
}

//-----------------------------------------------------------------------------
// addElement
//-----------------------------------------------------------------------------
void RenderInteriorMgr::addElement( RenderInst *inst )
{
   mElementList.increment();
   MainSortElem &elem = mElementList.last();
   elem.inst = inst;
   elem.key = elem.key2 = 0;

   // sort by material and matInst
   if( inst->matInst )
   {
      elem.key = (U32) inst->matInst->getMaterial();
   }

   // sort by vertex buffer
   if( inst->vertBuff )
   {
      elem.key2 = (U32) inst->vertBuff->getPointer();
   }

}

//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
#ifdef STATEBLOCK
void RenderInteriorMgr::render()
{
	// Early out if nothing to draw.
	if(!mElementList.size())
		return;
	PROFILE_SCOPE(RenderInteriorMgrRender);
	// Automagically save & restore our viewport and transforms.
	GFXTransformSaver saver;
	SceneGraphData sgData;
	if( gClientSceneGraph->isReflectPass() )
	{
		AssertFatal(mCullCWSB, "RenderInteriorMgr::render -- mCullCWSB cannot be NULL.");
		mCullCWSB->apply();
	}
	else
	{
		AssertFatal(mCullCCWSB, "RenderInteriorMgr::render -- mCullCCWSB cannot be NULL.");
		mCullCCWSB->apply();
	}
	U32 binSize = mElementList.size();
	GFXVertexBuffer * lastVB = NULL;
	GFXPrimitiveBuffer * lastPB = NULL;
	GFXTextureObject *lastLM = NULL;
	GFXTextureObject *lastLNM = NULL;
	U32 changeCount = 0;
	for( U32 j=0; j<mElementList.size(); )
	{
		RenderInst *ri = mElementList[j].inst;
		setupSGData( ri, sgData );
		MatInstance *mat = ri->matInst;
		U32 matListEnd = j;
		while( mat->setupPass( sgData ) )
		{
			changeCount++;
			U32 a;
			for( a=j; a<binSize; a++ )
			{
				RenderInst *passRI = mElementList[a].inst;
				// no dynamics if glowing...
				RenderPassData *passdata = mat->getPass(mat->getCurPass());
				if(passdata && passdata->glow && passRI->dynamicLight)
					continue;
				// if reflective, we want to reset textures in case this piece of geometry uses different reflect texture
				if ((newPassNeeded(mat, passRI)) || (a != j && passRI->reflective))  
				{
					lastLM = NULL;  // pointer no longer valid after setupPass() call
					lastLNM = NULL;
					break;
				}
				if(passRI->type == RenderInstManager::RIT_InteriorDynamicLighting)
				{
					mat->setLightingBlendFunc();
					setupSGData( passRI, sgData );
					sgData.matIsInited = true;
					mat->setLightInfo(sgData);                             
				}
				mat->setWorldXForm(*passRI->worldXform);
				mat->setObjectXForm(*passRI->objXform);
				mat->setEyePosition(*passRI->objXform, gRenderInstManager.getCamPos());
				mat->setBuffers(passRI->vertBuff, passRI->primBuff);
				// This section of code is dangerous, it overwrites the
				// lightmap values in sgData.  This could be a problem when multiple
				// render instances use the same multi-pass material.  When
				// the first pass is done, setupPass() is called again on
				// the material, but the lightmap data has been changed in
				// sgData to the lightmaps in the last renderInstance rendered.
				// This section sets the lightmap data for the current batch.
				// For the first iteration, it sets the same lightmap data,
				// however the redundancy will be caught by GFXDevice and not
				// actually sent to the card.  This is done for simplicity given
				// the possible condition mentioned above.  Better to set always
				// than to get bogged down into special case detection.
				//-------------------------------------
				bool dirty = false;
				// set the lightmaps if different
				if( passRI->lightmap && passRI->lightmap != lastLM )
				{
					sgData.lightmap = passRI->lightmap;
					lastLM = passRI->lightmap;
					dirty = true;
				}
				if( passRI->normLightmap && passRI->normLightmap != lastLNM )
				{
					sgData.normLightmap = passRI->normLightmap;
					lastLNM = passRI->normLightmap;
					dirty = true;
				}
				if(dirty && (passRI->type != RenderInstManager::RIT_InteriorDynamicLighting))
				{
					mat->setTextureStages( sgData );
				}
				//-------------------------------------
				// draw it
				if(passRI->prim)
				{
					GFXPrimitive *prim = passRI->prim;
					GFX->drawIndexedPrimitive(prim->type, prim->minIndex, prim->numVertices,
						prim->startIndex, prim->numPrimitives);
					lastVB = NULL;
					lastPB = NULL;
				}
				else
				{
					GFX->drawPrimitive( passRI->primBuffIndex );
				}
			}
			matListEnd = a;
			}
		// force increment if none happened, otherwise go to end of batch
		j = ( j == matListEnd ) ? j+1 : matListEnd;
			}
	AssertFatal(mClearSB, "RenderInteriorMgr::render -- mClearSB cannot be NULL.");
	mClearSB->apply();
}
#else
void RenderInteriorMgr::render()
{
   // Early out if nothing to draw.
   if(!mElementList.size())
      return;

   PROFILE_SCOPE(RenderInteriorMgrRender);

   // Automagically save & restore our viewport and transforms.
   GFXTransformSaver saver;

   SceneGraphData sgData;

   if( gClientSceneGraph->isReflectPass() )
   {
      GFX->setCullMode( GFXCullCW );
   }
   else
   {
      GFX->setCullMode( GFXCullCCW );
   }

   for( U32 i=0; i<GFX->getNumSamplers(); i++ )
   {
      GFX->setTextureStageAddressModeU( i, GFXAddressWrap );
      GFX->setTextureStageAddressModeV( i, GFXAddressWrap );

      GFX->setTextureStageMagFilter( i, GFXTextureFilterLinear );
      GFX->setTextureStageMinFilter( i, GFXTextureFilterLinear );
      GFX->setTextureStageMipFilter( i, GFXTextureFilterLinear );
   }

   // turn on anisotropic only on base tex stage
   //   GFX->setTextureStageMaxAnisotropy( 0, 2 );
   //   GFX->setTextureStageMagFilter( 0, GFXTextureFilterAnisotropic );
   //   GFX->setTextureStageMinFilter( 0, GFXTextureFilterAnisotropic );


   GFX->setZWriteEnable( true );
   GFX->setZEnable( true );


   U32 binSize = mElementList.size();


   GFXVertexBuffer * lastVB = NULL;
   GFXPrimitiveBuffer * lastPB = NULL;

   GFXTextureObject *lastLM = NULL;
   GFXTextureObject *lastLNM = NULL;

   U32 changeCount = 0;

   for( U32 j=0; j<mElementList.size(); )
   {
      RenderInst *ri = mElementList[j].inst;

      setupSGData( ri, sgData );
      MatInstance *mat = ri->matInst;

      U32 matListEnd = j;


      while( mat->setupPass( sgData ) )
      {
         changeCount++;
         U32 a;
         for( a=j; a<binSize; a++ )
         {
            RenderInst *passRI = mElementList[a].inst;

            // no dynamics if glowing...
            RenderPassData *passdata = mat->getPass(mat->getCurPass());
            if(passdata && passdata->glow && passRI->dynamicLight)
               continue;

            // if reflective, we want to reset textures in case this piece of geometry uses different reflect texture
            if ((newPassNeeded(mat, passRI)) || (a != j && passRI->reflective))  
            {
               lastLM = NULL;  // pointer no longer valid after setupPass() call
               lastLNM = NULL;
               break;
            }

			   if(passRI->type == RenderInstManager::RIT_InteriorDynamicLighting)
			   {
               mat->setLightingBlendFunc();

               setupSGData( passRI, sgData );
               sgData.matIsInited = true;
               mat->setLightInfo(sgData);                             
			   }
            mat->setWorldXForm(*passRI->worldXform);
            mat->setObjectXForm(*passRI->objXform);
            mat->setEyePosition(*passRI->objXform, gRenderInstManager.getCamPos());
            mat->setBuffers(passRI->vertBuff, passRI->primBuff);

            // This section of code is dangerous, it overwrites the
            // lightmap values in sgData.  This could be a problem when multiple
            // render instances use the same multi-pass material.  When
            // the first pass is done, setupPass() is called again on
            // the material, but the lightmap data has been changed in
            // sgData to the lightmaps in the last renderInstance rendered.

            // This section sets the lightmap data for the current batch.
            // For the first iteration, it sets the same lightmap data,
            // however the redundancy will be caught by GFXDevice and not
            // actually sent to the card.  This is done for simplicity given
            // the possible condition mentioned above.  Better to set always
            // than to get bogged down into special case detection.
            //-------------------------------------
            bool dirty = false;

            // set the lightmaps if different
            if( passRI->lightmap && passRI->lightmap != lastLM )
            {
               sgData.lightmap = passRI->lightmap;
               lastLM = passRI->lightmap;
               dirty = true;
            }
            if( passRI->normLightmap && passRI->normLightmap != lastLNM )
            {
               sgData.normLightmap = passRI->normLightmap;
               lastLNM = passRI->normLightmap;
               dirty = true;
            }

            if(dirty && (passRI->type != RenderInstManager::RIT_InteriorDynamicLighting))
            {
               mat->setTextureStages( sgData );
            }
            //-------------------------------------
      

               // draw it
			   if(passRI->prim)
			   {
				   GFXPrimitive *prim = passRI->prim;
				   GFX->drawIndexedPrimitive(prim->type, prim->minIndex, prim->numVertices,
					   prim->startIndex, prim->numPrimitives);
   					
				   lastVB = NULL;
				   lastPB = NULL;
			   }
			   else
            {
               GFX->drawPrimitive( passRI->primBuffIndex );
            }

         }

         matListEnd = a;
      }

      // force increment if none happened, otherwise go to end of batch
      j = ( j == matListEnd ) ? j+1 : matListEnd;

   }

   GFX->setLightingEnable(false);
}
#endif
void RenderInteriorMgr::resetStateBlock()
{
	//mCullCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCW);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	for( U32 i=0; i<GFX->getNumSamplers(); i++ )
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
	}
	GFX->endStateBlock(mCullCWSB);
	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	for( U32 i=0; i<GFX->getNumSamplers(); i++ )
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
	}
	GFX->endStateBlock(mCullCCWSB);
	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSLighting, false);
	GFX->endStateBlock(mClearSB);
}
void RenderInteriorMgr::releaseStateBlock()
{
	if (mCullCWSB)
	{
		mCullCWSB->release();
	}
	if (mCullCCWSB)
	{
		mCullCCWSB->release();
	}
	if (mClearSB)
	{
		mClearSB->release();
	}
}
void RenderInteriorMgr::init()
{
	if (mCullCWSB == NULL)
	{
		mCullCWSB = new GFXD3D9StateBlock;
		mCullCWSB->registerResourceWithDevice(GFX);
		mCullCWSB->mZombify = &releaseStateBlock;
		mCullCWSB->mResurrect = &resetStateBlock;
		mCullCCWSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}
void RenderInteriorMgr::shutdown()
{
	SAFE_DELETE(mCullCWSB);
	SAFE_DELETE(mCullCCWSB);
	SAFE_DELETE(mClearSB);
}
