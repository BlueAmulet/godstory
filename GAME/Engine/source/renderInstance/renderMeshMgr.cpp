//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderMeshMgr.h"
#include "materials/sceneData.h"
#include "sceneGraph/sceneGraph.h"
#include "materials/matInstance.h"
#include "gfx/gfxTransformSaver.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "ts/tsShapeInstance.h"
#include "gfx/D3D/DXATIUtil.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* RenderMeshMgr::mCullStoreSB = NULL;
GFXStateBlock* RenderMeshMgr::mSetSB = NULL;
GFXStateBlock* RenderMeshMgr::mFirstSB = NULL;
GFXStateBlock* RenderMeshMgr::mSecondSB = NULL;
GFXStateBlock* RenderMeshMgr::mDefaultSB = NULL;

//**************************************************************************
// RenderMeshMgr
//**************************************************************************

//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
#ifdef STATEBLOCK
//void RenderMeshMgr::render()
//{
//	// Early out if nothing to draw.
//	if(!mElementList.size())
//		return;
//
//	PROFILE_SCOPE(RenderMeshMgrRender);
//
//	// Automagically save & restore our viewport and transforms.
//	GFXTransformSaver saver;
//
//	// set render states
//	AssertFatal(mSetSB, "RenderMeshMgr::renderObject -- mSetSB cannot be NULL.");
//	mSetSB->apply();
//	AssertFatal(mCullStoreSB, "RenderMeshMgr::renderObject -- mCullStoreSB cannot be NULL.");
//	mCullStoreSB->capture();
//	// init loop data
//	GFXVertexBuffer * lastVB = NULL;
//	GFXPrimitiveBuffer * lastPB = NULL;
//	SceneGraphData sgData;
//	U32 binSize = mElementList.size();
//#ifdef DEBUG
//	gClientSceneGraph->setSdDPtimes(binSize);
//	int SaTimes = 0;
//	int SaKinds = 0;
//#endif
//	for( U32 j=0; j<binSize; )
//	{
//		RenderInst *ri = mElementList[j].inst;
//
//		setupSGData( ri, sgData );
//		sgData.matIsInited = true;
//
//		MatInstance *mat = ri->matInst;
//
//		// .ifl?
//		if( !mat && !ri->particles )
//		{
//			AssertFatal(mDefaultSB, "RenderMeshMgr::renderObject -- mDefaultSB cannot be NULL.");
//			mDefaultSB->apply();
//
//			GFX->pushWorldMatrix();
//			GFX->setWorldMatrix( *ri->worldXform );
//
//			GFX->setTexture( 0, ri->miscTex );
//			GFX->setPrimitiveBuffer( *ri->primBuff );
//			GFX->setVertexBuffer( *ri->vertBuff );
//			GFX->disableShaders();
//			GFX->setupGenericShaders( GFXDevice::GSModColorTexture );
//			GFX->drawPrimitive( ri->primBuffIndex );
//
//			GFX->popWorldMatrix();
//
//			lastVB = NULL;    // no longer valid, null it
//			lastPB = NULL;    // no longer valid, null it
//
//			j++;
//			continue;
//		}
//
//		if( !mat )
//		{
//			mat = gRenderInstManager.getWarningMat();
//		}
//		U32 matListEnd = j;
//
//		AssertFatal(mCullStoreSB, "RenderMeshMgr::renderObject -- mCullStoreSB cannot be NULL.");
//		mCullStoreSB->apply();
//#ifdef DEBUG
//		bool debugTest = true;
//#endif
//		bool firstmatpass = true;
//        F32 baseColorExposure = ( mat && mat->getMaterial() ) ? mat->getMaterial()->getColorExposure() : 1.0f;
//        F32 lastOverrideColorExposure = sgData.overrideColorExposure;
//        F32 lastFogFactor = sgData.fogFactor;
//		GFXTextureObject *lastVertTex = (GFXTextureObject *)-1;
//        F32 registerArry[4];
//		while( mat && mat->setupPass( sgData ) )
//		{
//			U32 a;
//			for( a=j; a<binSize; a++ )
//			{
//				RenderInst *passRI = mElementList[a].inst;
//
//				if (newPassNeeded(mat, passRI))
//					break;
//
//				// no dynamics if glowing...
//				RenderPassData *passdata = mat->getPass(mat->getCurPass());
//				if(passdata && passdata->glow && passRI->dynamicLight)
//					continue;
//
//				// don't break the material multipass rendering...
//				if(firstmatpass)
//				{
//					if(passRI->primitiveFirstPass)
//					{
//						bool &firstpass = *passRI->primitiveFirstPass;
//						if(firstpass)
//						{
//							AssertFatal(mFirstSB, "RenderMeshMgr::renderObject -- mFirstSB cannot be NULL.");
//							mFirstSB->apply();
//							firstpass = false;
//						}
//						else
//						{
//							AssertFatal((passRI->light->mType != LightInfo::Vector), "Not good");
//							AssertFatal((passRI->light->mType != LightInfo::Ambient), "Not good");
//							//mat->setLightingBlendFunc();
//							AssertFatal(mSecondSB, "RenderMeshMgr::renderObject -- mSecondSB cannot be NULL.");
//							mSecondSB->apply();
//						}
//					}
//					else
//					{
//						AssertFatal(mFirstSB, "RenderMeshMgr::renderObject -- mFirstSB cannot be NULL.");
//						mFirstSB->apply();
//					}
//				}
//
//				if(a!=j)
//				{
//#ifdef DEBUG
//					SaTimes++;
//					if(debugTest)
//						SaKinds++;
//					debugTest = false;
//#endif
//					mat->setObjectXForm(*passRI->objXform);
//					setupSGData( passRI, sgData );
//					sgData.matIsInited = true;
//					mat->setLightInfo(sgData);                             
//					mat->setEyePosition(*passRI->objXform, gRenderInstManager.getCamPos());
//
//                    // Color Exposure
//                    if ( lastOverrideColorExposure != sgData.overrideColorExposure )
//                    {
//                        registerArry[0] = baseColorExposure * sgData.overrideColorExposure;
//                        GFX->setPixelShaderConstF(PC_COLOREXPOSURE, (float*)registerArry, 1, 1);
//                        lastOverrideColorExposure = sgData.overrideColorExposure;
//                    }
//
//                    // Fog Factor
//                    if ( lastFogFactor != sgData.fogFactor )
//                    {
//                        registerArry[0] = sgData.fogHeightOffset;
//                        registerArry[1] = sgData.fogInvHeightRange;
//                        registerArry[2] = sgData.visDist;
//                        registerArry[3] = sgData.fogFactor;
//                        GFX->setVertexShaderConstF( VC_FOGDATA, (float*)registerArry, 1 );
//                        lastFogFactor = sgData.fogFactor;
//                    }
//				}
//
//				if( lastVertTex!=sgData.vertexTex )
//				{
//					GFX->setTexture(VERTEX_TEXTURE_STAGE0 ,sgData.vertexTex);
//					if(sgData.vertexTex)
//					{
//						registerArry[0] = passRI->boneNum;
//						GFX->setVertexShaderConstF( VC_VERTEX_TEX, (float*)registerArry, 1 ,1);
//					}
//					else
//					{
//						registerArry[0] = 0.0f;
//						GFX->setVertexShaderConstF( VC_VERTEX_TEX, (float*)registerArry, 1 ,1);
//					}
//					lastVertTex = sgData.vertexTex;
//				}
//
//				mat->setWorldXForm(*passRI->worldXform);
//				mat->setBuffers(passRI->vertBuff, passRI->primBuff);
//
//				// draw it
//				GFX->drawPrimitive( passRI->primBuffIndex );
//			}
//
//			matListEnd = a;
//			firstmatpass = false;
//		}
//
//		// force increment if none happened, otherwise go to end of batch
//		j = ( j == matListEnd ) ? j+1 : matListEnd;
//
//	}
//
//#ifdef DEBUG
//	gClientSceneGraph->setSaDPtimes(SaTimes);
//	gClientSceneGraph->setSkDPtimes(SaKinds);
//#endif
//}

#else
void RenderMeshMgr::render()
{
   // Early out if nothing to draw.
   if(!mElementList.size())
      return;

   PROFILE_SCOPE(RenderMeshMgrRender);

   // Automagically save & restore our viewport and transforms.
   GFXTransformSaver saver;

   // set render states
   GFXCullMode mode = GFXCullNone;
   if( gClientSceneGraph->isReflectPass() )
   {
      mode = GFXCullCW;
   }
   else
   {
      mode = GFXCullCCW;
   }

   for( U32 i=0; i<GFX->getNumSamplers(); i++ )
   {
      GFX->setTextureStageAddressModeU( i, GFXAddressWrap );
      GFX->setTextureStageAddressModeV( i, GFXAddressWrap );

      GFX->setTextureStageMagFilter( i, GFXTextureFilterLinear );
      GFX->setTextureStageMinFilter( i, GFXTextureFilterLinear );
      GFX->setTextureStageMipFilter( i, GFXTextureFilterLinear );
   }

   GFX->setTextureStageAddressModeU( VERTEX_TEXTURE_STAGE0, GFXAddressClamp );
   GFX->setTextureStageAddressModeV( VERTEX_TEXTURE_STAGE0, GFXAddressClamp );
   GFX->setTextureStageMagFilter( VERTEX_TEXTURE_STAGE0, GFXTextureFilterNone );
   GFX->setTextureStageMinFilter( VERTEX_TEXTURE_STAGE0, GFXTextureFilterNone );
   GFX->setTextureStageMipFilter( VERTEX_TEXTURE_STAGE0, GFXTextureFilterNone );

   // init loop data
   GFXVertexBuffer * lastVB = NULL;
   GFXPrimitiveBuffer * lastPB = NULL;
   SceneGraphData sgData;
   U32 binSize = mElementList.size();
#ifdef DEBUG
   gClientSceneGraph->setSdDPtimes(binSize);
   int SaTimes = 0;
   int SaKinds = 0;
#endif

   for( U32 j=0; j<binSize; )
   {
      RenderInst *ri = mElementList[j].inst;

      setupSGData( ri, sgData );
	  sgData.matIsInited = true;

      MatInstance *mat = ri->matInst;

      // .ifl?
      if( !mat && !ri->particles )
      {
         GFX->setTextureStageColorOp( 0, GFXTOPModulate );
         GFX->setTextureStageColorOp( 1, GFXTOPDisable );

         GFX->pushWorldMatrix();
         GFX->setWorldMatrix( *ri->worldXform );

         GFX->setTexture( 0, ri->miscTex );
         GFX->setPrimitiveBuffer( *ri->primBuff );
         GFX->setVertexBuffer( *ri->vertBuff );
         GFX->disableShaders();
         GFX->setupGenericShaders( GFXDevice::GSModColorTexture );
         GFX->drawPrimitive( ri->primBuffIndex );

         GFX->popWorldMatrix();

         lastVB = NULL;    // no longer valid, null it
         lastPB = NULL;    // no longer valid, null it

         j++;
         continue;
      }

      if( !mat )
      {
         mat = gRenderInstManager.getWarningMat();
      }
      U32 matListEnd = j;

	  GFX->setCullMode(mode);
#ifdef DEBUG
	  bool debugTest = true;
#endif
	  bool enableFluidLight = 0;
	  F32 fluidLightX = 0;
	  F32 fluidLightY = 0;

      bool firstmatpass = true;
      F32 baseColorExposure = ( mat && mat->getMaterial() ) ? mat->getMaterial()->getColorExposure() : 1.0f;
      F32 lastOverrideColorExposure = sgData.overrideColorExposure;
      F32 lastFogFactor = sgData.fogFactor;
	  GFXTextureObject *lastVertTex = (GFXTextureObject *)-1;
	  RenderInst *passRI;

      F32 registerArry[4];
      while( mat && mat->setupPass( sgData ) )
      {
		  U32 a;
		  for( a=j; a<binSize; a++ )
		  {
			  passRI = mElementList[a].inst;

			  if (newPassNeeded(mat, passRI))
				  break;

			  // no dynamics if glowing...
			  RenderPassData *passdata = mat->getPass(mat->getCurPass());
			  if(passdata && passdata->glow && passRI->dynamicLight)
				  continue;

			  // don't break the material multipass rendering...
			  if(firstmatpass)
			  {
				  if(passRI->primitiveFirstPass)
				  {
					  bool &firstpass = *passRI->primitiveFirstPass;
					  if(firstpass)
					  {
						  GFX->setAlphaBlendEnable(false);
						  GFX->setSrcBlend(GFXBlendOne);
						  GFX->setDestBlend(GFXBlendZero);
						  firstpass = false;
					  }
					  else
					  {
						   AssertFatal((passRI->light->mType != LightInfo::Vector), "Not good");
						   AssertFatal((passRI->light->mType != LightInfo::Ambient), "Not good");
						   //mat->setLightingBlendFunc();
						   GFX->setAlphaBlendEnable(true);
						   GFX->setSrcBlend(GFXBlendSrcAlpha);
						   GFX->setDestBlend(GFXBlendOne);  
					  }
				  }
				  else
				  {
					  GFX->setAlphaBlendEnable(false);
					  GFX->setSrcBlend(GFXBlendOne);
					  GFX->setDestBlend(GFXBlendZero);
				  }
			  }

			  if(a!=j)
			  {
#ifdef DEBUG
				  SaTimes++;
				  if(debugTest)
					  SaKinds++;
				  debugTest = false;
#endif
				  setupSGData( passRI, sgData );
				  sgData.matIsInited = true;

				  mat->setObjectXForm(sgData.objTrans);
				  mat->setLightInfo(sgData);                             
				  mat->setEyePosition(sgData.objTrans, gRenderInstManager.getCamPos());

                  // Color Exposure
                  if ( lastOverrideColorExposure != sgData.overrideColorExposure )
                  {
                      registerArry[0] = baseColorExposure * sgData.overrideColorExposure;
                      GFX->setPixelShaderConstF(PC_COLOREXPOSURE, (float*)registerArry, 1, 1);
                      lastOverrideColorExposure = sgData.overrideColorExposure;
                  }

                  // Fog Factor
                  if ( lastFogFactor != sgData.fogFactor )
                  {
                      registerArry[0] = sgData.fogHeightOffset;
                      registerArry[1] = sgData.fogInvHeightRange;
                      registerArry[2] = sgData.visDist;
                      registerArry[3] = sgData.fogFactor;
                      GFX->setVertexShaderConstF( VC_FOGDATA, (float*)registerArry, 1 );
                      lastFogFactor = sgData.fogFactor;
                  }
			  }

			  //流光
			  if( enableFluidLight != passRI->enableFluidLight)
			  {
				  registerArry[0] = passRI->enableFluidLight?1:0;
				  GFX->setPixelShaderConstF(PC_FLUIDLIGHT, (float*)registerArry, 1, 1);
				  enableFluidLight = passRI->enableFluidLight;

				  if(enableFluidLight)
				  {
					  if(fluidLightX != passRI->FluidLightX || fluidLightY != passRI->FluidLightY)
					  {
						  registerArry[0] = passRI->FluidLightX;
						  registerArry[1] = passRI->FluidLightY;
						  GFX->setPixelShaderConstF(PC_FLUIDLIGHTSPEED, (float*)registerArry, 1, 2);
						  fluidLightX = passRI->FluidLightX;
						  fluidLightY = passRI->FluidLightY;
					  }
				  }
			  }

			  if( lastVertTex!=sgData.vertexTex )
			  {
				  if(TSShapeInstance::supportsVertTex)
				  {
					  GFX->setTexture(VERTEX_TEXTURE_STAGE0 ,sgData.vertexTex);
					  if(sgData.vertexTex)
					  {
						  registerArry[0] = passRI->boneNum;
						  GFX->setVertexShaderConstF( VC_VERTEX_TEX, (float*)registerArry, 1,1 );
					  }
					  else
					  {
						  registerArry[0] = 0.0f;
						  GFX->setVertexShaderConstF( VC_VERTEX_TEX, (float*)registerArry, 1,1 );
					  }

					  GFX->setRenderState(GFXRSAlphaBlendEnable,GFX->getRenderState(GFXRSAlphaBlendEnable));
				  }
				  else if(TSShapeInstance::supportsR2VB)
				  {
					  if(sgData.vertexTex)
					  {
						  if(!GFX->isDmapSet())
						  {
							  GFX->setPointSize(r2vbGlbEnable_Set(TRUE));
							  TSShapeInstance::smDummyVB->prepare();  //setup streamSource(1) = Dummy
							  GFX->updateStates();
							  // Tell the driver that stream 1 is to be fetched from the DMAP texture
							  GFX->setPointSize(r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));	
							  GFX->setDmap(true);
						  }

						  GFX->setTexture(VERTEX_TEXTURE_BUFF, sgData.vertexTex);
					  }
					  else
					  {
						  if(GFX->isDmapSet())
						  {
							  GFX->setPointSize(r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
							  // Unbind the DMAP texture
							  GFX->setTexture(VERTEX_TEXTURE_BUFF, NULL);
							  GFX->updateStates();
							  GFX->setPointSize(r2vbGlbEnable_Set(FALSE));
							  TSShapeInstance::smNullVB->prepare();  //setup streamSource(1) = NULL
							  GFX->setDmap(false);
						  }
					  }

					  GFX->setRenderState(GFXRSAlphaBlendEnable,GFX->getRenderState(GFXRSAlphaBlendEnable));
				  }

				  lastVertTex = sgData.vertexTex;
			  }

			  mat->setWorldXForm(*passRI->worldXform);
			  mat->setBuffers(passRI->vertBuff, passRI->primBuff);

			  // draw it
			  GFX->drawPrimitive( passRI->primBuffIndex );
		  }

		  matListEnd = a;
		  firstmatpass = false;
	  }

      // force increment if none happened, otherwise go to end of batch
      j = ( j == matListEnd ) ? j+1 : matListEnd;

   }

   if(GFX->isDmapSet())
   {
	   GFX->setPointSize(r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
	   // Unbind the DMAP texture
	   GFX->setTexture(VERTEX_TEXTURE_BUFF, NULL);
	   GFX->updateStates();
	   GFX->setPointSize(r2vbGlbEnable_Set(FALSE));
	   TSShapeInstance::smNullVB->prepare();  //setup streamSource(1) = NULL
	   GFX->updateStates();
	   GFX->setDmap(false);
   }

#ifdef DEBUG
   gClientSceneGraph->setSaDPtimes(SaTimes);
   gClientSceneGraph->setSkDPtimes(SaKinds);
#endif
}

#endif

void RenderMeshMgr::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	for( U32 i=0; i<GFX->getNumSamplers(); i++ )
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
	}
	GFX->endStateBlock(mSetSB);

	//mCullStoreSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->endStateBlock(mCullStoreSB);


	//mFirstSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->endStateBlock(mFirstSB);

	//mSecondSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mSecondSB);

	//mDefaultSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mDefaultSB);
}


void RenderMeshMgr::releaseStateBlock()
{
	if (mFirstSB)
	{
		mFirstSB->release();
	}

	if (mSecondSB)
	{
		mSecondSB->release();
	}

	if (mCullStoreSB)
	{
		mCullStoreSB->release();
	}

	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mDefaultSB)
	{
		mDefaultSB->release();
	}
}

void RenderMeshMgr::init()
{
	if (mFirstSB == NULL)
	{
		mFirstSB = new GFXD3D9StateBlock;
		mFirstSB->registerResourceWithDevice(GFX);
		mFirstSB->mZombify = &releaseStateBlock;
		mFirstSB->mResurrect = &resetStateBlock;

		mSecondSB = new GFXD3D9StateBlock;
		mSetSB = new GFXD3D9StateBlock;
		mCullStoreSB = new GFXD3D9StateBlock;
		mDefaultSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void RenderMeshMgr::shutdown()
{
	SAFE_DELETE(mFirstSB);
	SAFE_DELETE(mSecondSB);
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mCullStoreSB);
	SAFE_DELETE(mDefaultSB);
}



