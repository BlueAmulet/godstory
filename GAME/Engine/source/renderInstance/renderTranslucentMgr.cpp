//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "RenderTranslucentMgr.h"
#include "materials/sceneData.h"
#include "sceneGraph/sceneGraph.h"
#include "materials/matInstance.h"
#include "gfx/primBuilder.h"
#include "gfx/D3D9/gfxD3D9EnumTranslate.h"
#include SHADER_CONSTANT_INCLUDE_FILE

#include "gfx/D3D9/gfxD3D9StateBlock.h"

#include "sceneGraph/renderableSceneObject.h"
#include "Effects/EdgeBlur.h"
#include "T3D/fx/fxFoliageReplicator.h"

#include "ts/tsShapeInstance.h"
#include "gfx/D3D/DXATIUtil.h"

//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
#define HIGH_NUM ((U32(-1)/2) - 1)

GFXStateBlock* RenderTranslucentMgr::mSetSB = NULL;
//cull
GFXStateBlock* RenderTranslucentMgr::mCullCCWSB = NULL;
GFXStateBlock* RenderTranslucentMgr::mCullNoneSB = NULL;
//object translucent
GFXStateBlock* RenderTranslucentMgr::mSetTransSB = NULL;
//particles
GFXStateBlock* RenderTranslucentMgr::mParticleOneSB = NULL;//1,5
GFXStateBlock* RenderTranslucentMgr::mParticleTwoSB = NULL;//2,6
GFXStateBlock* RenderTranslucentMgr::mParticleTriSB = NULL;//3,7
GFXStateBlock* RenderTranslucentMgr::mParticleForSB = NULL;//4,8
//light enable
GFXStateBlock* RenderTranslucentMgr::mLightEnableSB = NULL;
GFXStateBlock* RenderTranslucentMgr::mLightStoreSB = NULL;
//mat set
GFXStateBlock* RenderTranslucentMgr::mTexOpSB = NULL;
GFXStateBlock* RenderTranslucentMgr::mTransflagSB = NULL;
GFXStateBlock* RenderTranslucentMgr::mTransSB = NULL;
//alpha blend
GFXStateBlock* RenderTranslucentMgr::mAlphaBlendSB = NULL;
//z write
GFXStateBlock* RenderTranslucentMgr::mZWriteFalseSB = NULL;
//z enable
GFXStateBlock* RenderTranslucentMgr::mZTrueSB = NULL;
GFXStateBlock* RenderTranslucentMgr::mZFalseSB = NULL;
//clear
GFXStateBlock* RenderTranslucentMgr::mClearSB = NULL;
//**************************************************************************
// RenderTranslucentMgr
//**************************************************************************

void RenderTranslucentMgr::sort()
{
	dQsort( mElementList.address(), mElementList.size(), sizeof(MainSortElem), cmpTranslucentKeyFunc);
}

//-----------------------------------------------------------------------------
// QSort callback function
//-----------------------------------------------------------------------------
S32 FN_CDECL RenderTranslucentMgr::cmpTranslucentKeyFunc(const void* p1, const void* p2)
{
	const MainSortElem* mse1 = (const MainSortElem*) p1;
	const MainSortElem* mse2 = (const MainSortElem*) p2;

	if(mse2->inst->fadeFlag==mse1->inst->fadeFlag)
	{
		if(mse2->key != mse1->key)
			return S32(mse2->key) - S32(mse1->key);  //倒序
		else
			return S32(mse1->key2) - S32(mse2->key2);  //Ray: 顺序，根据SortedIndex，这里的假设是没有两个模型会有完全相同的坐标，否则会有问题
	}
	else
	{
		return (mse1->inst->fadeFlag - mse2->inst->fadeFlag);
	}
} 

//-----------------------------------------------------------------------------
// setup scenegraph data
//-----------------------------------------------------------------------------
void RenderTranslucentMgr::setupSGData( RenderInst *ri, SceneGraphData &data )
{
   Parent::setupSGData(ri, data);

   data.backBuffTex = NULL;
   data.cubemap = NULL;   
   data.lightmap = NULL;
   data.normLightmap = NULL;
}

//-----------------------------------------------------------------------------
// add element
//-----------------------------------------------------------------------------
void RenderTranslucentMgr::addElement( RenderInst *inst )
{
   mElementList.increment();
   MainSortElem &elem = mElementList.last();
   elem.inst = inst;

   // sort by distance (the multiply is there to prevent us from losing information when converting to a U32)
   F32 camDist = (gRenderInstManager.getCamPos() - inst->sortPoint).len();
   elem.key = *((U32*)&camDist);
   elem.key2 = (U32)inst->SortedIndex;
}

//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
#ifdef STATEBLOCK
//void RenderTranslucentMgr::render()
//{
//	// Early out if nothing to draw.
//	if(!mElementList.size())
//		return;
//
//	PROFILE_SCOPE(RenderTranslucentMgrRender);
//
//	GFX->pushWorldMatrix();
//	AssertFatal(mSetSB, "RenderTranslucentMgr::render -- mSetSB cannot be NULL.");
//	mSetSB->apply();
//	SceneGraphData sgData;
//
//	GFXVertexBuffer * lastVB = NULL;
//	GFXPrimitiveBuffer * lastPB = NULL;
//
//	U32 binSize = mElementList.size();
//
//#ifdef DEBUG
//	gClientSceneGraph->setTrDPtimes(binSize);
//	int SaTimes = 0;
//	int SaKinds = 0;
//#endif
//
//	for( U32 j=0; j<binSize; )
//	{
//		RenderInst *ri = mElementList[j].inst;
//		MatInstance *mat = ri->matInst;
//		U32 matListEnd = j;
//
//		// set culling
//		if( !mat || mat->getMaterial()->doubleSided )
//		{
//			AssertFatal(mCullNoneSB, "RenderTranslucentMgr::render -- mCullNoneSB cannot be NULL.");
//			mCullNoneSB->apply();
//		}
//		else
//		{
//			AssertFatal(mCullCCWSB, "RenderTranslucentMgr::render -- mCullCCWSB cannot be NULL.");
//			mCullCCWSB->apply();
//		}
//
//
//		// render these separately...
//		if(ri->type == RenderInstManager::RIT_ObjectTranslucent)
//		{
//			ri->obj->renderObject(ri->state, ri);
//
//			AssertFatal(mSetTransSB, "RenderTranslucentMgr::render -- mSetTransSB cannot be NULL.");
//			mSetTransSB->apply();
//			lastVB = NULL;
//			lastPB = NULL;
//			j++;
//			continue;
//		}
//
//		// handle particles
//		if( ri->particles )
//		{
//			S32 temp_s32 = (ri->particles - 1) % 4;
//			if(temp_s32 == 0)
//			{
//				AssertFatal(mParticleOneSB, "RenderTranslucentMgr::render -- mParticleOneSB cannot be NULL.");
//				mParticleOneSB->apply();
//			}
//			else if (temp_s32 == 1)
//			{
//					AssertFatal(mParticleTwoSB, "RenderTranslucentMgr::render -- mParticleTwoSB cannot be NULL.");
//					mParticleTwoSB->apply();
//			}
//			else if (temp_s32 == 2)
//			{
//					AssertFatal(mParticleTriSB, "RenderTranslucentMgr::render -- mParticleTriSB cannot be NULL.");
//					mParticleTriSB->apply();
//			}
//			else if (temp_s32 == 3)
//			{
//					AssertFatal(mParticleForSB, "RenderTranslucentMgr::render -- mParticleForSB cannot be NULL.");
//					mParticleForSB->apply();
//			}
//			else
//			{
//
//			}
//
//
//				AssertFatal(mLightStoreSB, "RenderTranslucentMgr::render -- mLightStoreSB cannot be NULL.");
//				mLightStoreSB->capture();
//			//GFX->setSrcBlend( GFXBlendSrcAlpha );
//
//			//   For particles, transFlags now contains both src and dest blend
//			//   settings packed together. SrcBlend is in the upper 4 bits and
//			//   DestBlend is in the lower 4. Also, alpha testing is disabled
//			//   for all but the two common blends that are compatible with it.
//			//   With further analysis, appropriate alpha testing can be configured 
//			//   for additional blends.
//			//
//			// unpack and set blends from transFlags
//			/*GFXBlend src_blend = (GFXBlend)((ri->transFlags >> 4) & 0x0f);
//			GFXBlend dst_blend = (GFXBlend)(ri->transFlags & 0x0f);
//			GFX->setSrcBlend(src_blend);
//			GFX->setDestBlend(dst_blend);
//			if( src_blend != GFXBlendSrcAlpha || (dst_blend != GFXBlendInvSrcAlpha && dst_blend != GFXBlendOne) )
//			{
//			GFX->setAlphaTestEnable( false );
//			}*/
//
//			//GFX->pushWorldMatrix();
//
//			MatrixF mat;
//			mat = (*ri->objXform) * (*ri->worldXform);
//
//			static bool bfind = false;
//			static ShaderData *sgParticleRender = NULL; 
//			if(!bfind)
//			{
//				if ( !Sim::findObject( "ParticleRender", sgParticleRender ) )
//					Con::warnf("ParticleRender - failed to locate ParticleRender shader ParticleRenderBlendData!");
//				bfind = true;
//			}
//			GFX->setShader(sgParticleRender->getShader());
//
//			mat.transpose();
//			GFX->setVertexShaderConstF(0, (float*)&mat, 4);
//			ColorF temp_point4f = ri->maxBlendColor;
//			GFX->setPixelShaderConstF(0, (float*)&temp_point4f, 1);
//			Point4F temp_p4 = Point4F(0.0f, 0.0f ,0.0f, 0.0f);
//			temp_p4.x = ri->visibility;
//			GFX->setPixelShaderConstF(1, (float*)&temp_p4, 1);
//
//			GFX->setTexture( 0, ri->miscTex );
//			GFX->setPrimitiveBuffer( *ri->primBuff );
//			GFX->setVertexBuffer( *ri->vertBuff );
//
//			if(ri->particles < 5)
//				GFX->drawIndexedPrimitive( GFXTriangleList, 0, ri->primBuffIndex * 4, 0, ri->primBuffIndex * 2 );
//			else
//				GFX->drawPrimitive( GFXTriangleStrip, 0, ri->primBuffIndex * 2 - 2);
//
//			GFX->disableShaders();
//
//			//GFX->popWorldMatrix();
//			//AssertFatal(mLightStoreSB, "RenderTranslucentMgr::render -- mLightStoreSB cannot be NULL.");
//			mLightStoreSB->apply();
//
//			lastVB = NULL;    // no longer valid, null it
//			lastPB = NULL;    // no longer valid, null it
//
//			j++;
//			continue;
//		}
//
//		// .ifl?
//		if( !mat && !ri->particles )
//		{
//			AssertFatal(mTexOpSB, "RenderTranslucentMgr::render -- mTexOpSB cannot be NULL.");
//			mTexOpSB->apply();
//			if( ri->translucent )
//			{
//				if( ri->transFlags )
//				{
//					AssertFatal(mTransflagSB, "RenderTranslucentMgr::render -- mTransflagSB cannot be NULL.");
//					mTransflagSB->apply();
//				}
//				else
//				{
//					AssertFatal(mTransSB, "RenderTranslucentMgr::render -- mTransSB cannot be NULL.");
//					mTransSB->apply();
//				}
//			}
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
//		setupSGData( ri, sgData );
//		sgData.matIsInited = true;
//
//		AssertFatal(mZWriteFalseSB, "RenderTranslucentMgr::render -- mZWriteFalseSB cannot be NULL.");
//		mZWriteFalseSB->apply();
//		bool firstmatpass = true;
//#ifdef DEBUG
//		bool debugTest = true;
//#endif
//
//		bool lastenableblend = sgData.enableBlendColor;
//		F32 lastvisibility = sgData.visibility;
//        F32 baseColorExposure = ( mat && mat->getMaterial() ) ? mat->getMaterial()->getColorExposure() : 1.0f;
//        F32 lastOverrideColorExposure = sgData.overrideColorExposure;
//        F32 lastFogFactor = sgData.fogFactor;
//		GFXTextureObject *lastVertTex = (GFXTextureObject *)-1;
//        F32 registerArry[4];
//
//		while( mat->setupPass( sgData ) )
//		{
//			// Z sorting and stuff is still not working in this mgr...
//			U32 a;
//			for( a=j; a<binSize; a++ )
//			{
//				RenderInst *passRI = mElementList[a].inst;
//
//				if (newPassNeeded(mat, passRI))
//					break;
//
//				if(a!=j)
//					setupSGData( passRI, sgData );
//
//				if (lastenableblend!=sgData.enableBlendColor)
//					break;
//
//				// don't break the material multipass rendering...
//				if(firstmatpass)
//				{
//					if(passRI->primitiveFirstPass)
//					{
//						bool &firstpass = *passRI->primitiveFirstPass;                  
//						if(!firstpass)
//						{
//						AssertFatal(mAlphaBlendSB, "RenderTranslucentMgr::render -- mAlphaBlendSB cannot be NULL.");
//						mAlphaBlendSB->apply();
//						}
//						firstpass = false;
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
//					sgData.matIsInited = true;
//					mat->setLightInfo(sgData);                             
//					mat->setEyePosition(*passRI->objXform, gRenderInstManager.getCamPos());
//
//					// Visibility
//					if(lastvisibility != sgData.visibility)
//					{
//						GFX->setPixelShaderConstF( PC_VISIBILITY, &sgData.visibility, 1, 1 );
//						lastvisibility = sgData.visibility;
//					}
//
//                    // Color Exposure
//                    if ( lastOverrideColorExposure != sgData.overrideColorExposure )
//                    {
//                        registerArry[0] = baseColorExposure * sgData.overrideColorExposure;
//                        GFX->setPixelShaderConstF(PC_COLOREXPOSURE, (float*)registerArry, 1, 1 );
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
//						GFX->setVertexShaderConstF( VC_VERTEX_TEX, (float*)registerArry, 1,1 );
//					}
//					else
//					{
//						registerArry[0] = 0.0f;
//						GFX->setVertexShaderConstF( VC_VERTEX_TEX, (float*)registerArry, 1,1 );
//					}
//					lastVertTex = sgData.vertexTex;
//				}
//
//				mat->setWorldXForm(*passRI->worldXform);
//				mat->setBuffers(passRI->vertBuff, passRI->primBuff);
//
//			if(ri->meshDisableZ)
//			{
//				AssertFatal(mZFalseSB, "RenderTranslucentMgr::render -- mZFalseSB cannot be NULL.");
//				mZFalseSB->apply();
//			}
//			else
//			{
//				AssertFatal(mZTrueSB, "RenderTranslucentMgr::render -- mZTrueSB cannot be NULL.");
//				mZTrueSB->apply();
//			}
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
//	AssertFatal(mClearSB, "RenderTranslucentMgr::render -- mClearSB cannot be NULL.");
//	mClearSB->apply();
//	GFX->popWorldMatrix();
//
//#ifdef DEBUG
//	gClientSceneGraph->setSaDPtimes(SaTimes);
//	gClientSceneGraph->setSkDPtimes(SaKinds);
//#endif
//}
#else
void RenderTranslucentMgr::render()
{
	// Early out if nothing to draw.
	if(!mElementList.size())
		return;

	PROFILE_SCOPE(RenderTranslucentMgrRender);

	GFX->pushWorldMatrix();

	SceneGraphData sgData;

	// set up register combiners
	GFX->setTextureStageAlphaOp( 0, GFXTOPModulate );
	GFX->setTextureStageAlphaOp( 1, GFXTOPDisable );
	GFX->setTextureStageAlphaArg1( 0, GFXTATexture );
	GFX->setTextureStageAlphaArg2( 0, GFXTADiffuse );

	GFX->setTextureStageColorOp( 0, GFXTOPModulate );
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );

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


	// turn on alpha test
	GFX->setAlphaTestEnable( true );
	GFX->setAlphaRef( 1 );
	GFX->setAlphaFunc( GFXCmpGreaterEqual );

	GFX->setAlphaBlendEnable( true );

	GFX->setZWriteEnable( false );

	GFXVertexBuffer * lastVB = NULL;
	GFXPrimitiveBuffer * lastPB = NULL;

	U32 binSize = mElementList.size();

#ifdef DEBUG
	gClientSceneGraph->setTrDPtimes(binSize);
	int SaTimes = 0;
	int SaKinds = 0;
#endif

	for( U32 j=0; j<binSize; )
	{
		RenderInst *ri = mElementList[j].inst;
		if(!ri->needRender)
			continue;

		MatInstance *mat = ri->matInst;
		U32 matListEnd = j;

		// set culling
		if( !mat || mat->getMaterial()->doubleSided )
		{
			GFX->setCullMode( GFXCullNone );
		}
		else
		{
			GFX->setCullMode( GFXCullCCW );
		}


		// render these separately...
		if(ri->type == RenderInstManager::RIT_ObjectTranslucent)
		{
			ri->obj->renderObject(ri->state, ri);

			GFX->setCullMode( GFXCullNone );
			GFX->setTextureStageAlphaOp( 0, GFXTOPModulate );
			GFX->setTextureStageAlphaOp( 1, GFXTOPDisable );
			GFX->setTextureStageAlphaArg1( 0, GFXTATexture );
			GFX->setTextureStageAlphaArg2( 0, GFXTADiffuse );
			GFX->setTextureStageColorOp( 0, GFXTOPModulate );
			GFX->setTextureStageColorOp( 1, GFXTOPDisable );
			GFX->setAlphaTestEnable( true );
			GFX->setAlphaRef( 1 );
			GFX->setAlphaFunc( GFXCmpGreaterEqual );
			GFX->setZWriteEnable( false );
			lastVB = NULL;
			lastPB = NULL;
			j++;
			continue;
		}

		// handle particles
		if ((ri->particles >= RENDERTRANSLUCENT_TYPE_EDGEBLUR) && (ri->particles <= RENDERTRANSLUCENT_TYPE_RIBBON))
		{
			if(ri->particles == RENDERTRANSLUCENT_TYPE_EDGEBLUR)
			{
				CEdgeBlur* ed = (CEdgeBlur*)ri->obj;
				ed->renderObject(NULL, ri);
			}
			else
				ri->obj->renderObject(NULL, ri);
			j++;
			continue;
		}
		else
		{

		}

		// .ifl?
		if( !mat && !ri->particles )
		{
			GFX->setTextureStageColorOp( 0, GFXTOPModulate );
			GFX->setTextureStageColorOp( 1, GFXTOPDisable );
			GFX->setZWriteEnable( false );

			if( ri->translucent )
			{
				GFX->setAlphaBlendEnable( true );
				GFX->setSrcBlend( GFXBlendSrcAlpha );

				if( ri->transFlags )
					GFX->setDestBlend( GFXBlendInvSrcAlpha);
				else
					GFX->setDestBlend( GFXBlendOne );
			}

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

		setupSGData( ri, sgData );
		sgData.matIsInited = true;

		GFX->setZWriteEnable(false);
		bool firstmatpass = true;
#ifdef DEBUG
		bool debugTest = true;
#endif
		bool enableFluidLight = 0;
		F32 fluidLightX = 0;
		F32 fluidLightY = 0;

		bool lastenableblend = sgData.enableBlendColor;
		F32 lastvisibility = sgData.visibility;
        F32 baseColorExposure = ( mat && mat->getMaterial() ) ? mat->getMaterial()->getColorExposure() : 1.0f;
        F32 lastOverrideColorExposure = sgData.overrideColorExposure;
        F32 lastFogFactor = sgData.fogFactor;
		GFXTextureObject *lastVertTex = (GFXTextureObject *)-1;
        F32 registerArry[4];
		RenderInst *passRI = NULL;

		//if(ri->enableBlendColor && ri->obj && ri->obj->m_pChain)
		//{
		//	passRI = ri->obj->m_pChain;
		//	while(passRI)
		//	{
		//		while( mat->setupPass( sgData ) )
		//		{
		//			while(passRI)
		//			{

		//				passRI = passRI->pInstNext;
		//			}
		//		}

		//		passRI = passRI->pInstNext;
		//	}

		//	continue;
		//}

		while( mat->setupPass( sgData ) )
		{
			// Z sorting and stuff is still not working in this mgr...
			U32 a;
			for( a=j; a<binSize; a++ )
			{
				passRI = mElementList[a].inst;

				if(a!=j)
				{
					if (newPassNeeded(mat, passRI))
						break;

						setupSGData( passRI, sgData );

					if (lastenableblend!=sgData.enableBlendColor)
						break;

#ifdef DEBUG
					SaTimes++;
					if(debugTest)
						SaKinds++;
					debugTest = false;
#endif
					mat->setObjectXForm(sgData.objTrans);
					sgData.matIsInited = true;
					mat->setLightInfo(sgData);                             
					mat->setEyePosition(sgData.objTrans, gRenderInstManager.getCamPos());

					// Visibility
					if(lastvisibility != sgData.visibility)
					{
						GFX->setPixelShaderConstF( PC_VISIBILITY, &sgData.visibility, 1, 1 );
						lastvisibility = sgData.visibility;
					}

                    // Color Exposure
                    if ( lastOverrideColorExposure != sgData.overrideColorExposure )
                    {
                        registerArry[0] = baseColorExposure * sgData.overrideColorExposure;
                        GFX->setPixelShaderConstF(PC_COLOREXPOSURE, (float*)registerArry, 1, 1 );
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
				
				// don't break the material multipass rendering...
				if(firstmatpass)
				{
					if(passRI->primitiveFirstPass)
					{
						bool &firstpass = *passRI->primitiveFirstPass;                  
						if(!firstpass)
						{
							GFX->setAlphaBlendEnable(true);
							GFX->setSrcBlend(GFXBlendOne);
							GFX->setDestBlend(GFXBlendOne);
						}
						firstpass = false;
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
								GFX->setPointSize(r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));	
								GFX->setDmap(true);
							}
							// Tell the driver that stream 1 is to be fetched from the DMAP texture
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

				if(passRI->meshDisableZ)
					GFX->setZEnable( false );
				else
					GFX->setZEnable( true );

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

	GFX->setZEnable( true );
	GFX->setZWriteEnable( true );
	GFX->setAlphaTestEnable( false );
	GFX->setAlphaBlendEnable( false );

	GFX->popWorldMatrix();

#ifdef DEBUG
	gClientSceneGraph->setSaDPtimes(SaTimes);
	gClientSceneGraph->setSkDPtimes(SaKinds);
#endif
}
#endif

void RenderTranslucentMgr::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->setTextureStageState(0, GFXTSSAlphaArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSAlphaArg2, GFXTADiffuse);

	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	for(S32 i=0; i<GFX->getNumSamplers(); i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
	}
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 1);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual );
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mSetSB);

	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->endStateBlock(mCullCCWSB);

	//mCullNoneSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mCullNoneSB);

	//mSetTransSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->setTextureStageState(0, GFXTSSAlphaArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSAlphaArg2, GFXTADiffuse);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 1);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual );
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->endStateBlock(mSetTransSB);

	//mParticleOneSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mParticleOneSB);

	//mParticleTwoSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendZero);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcColor);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mParticleTwoSB);

	//mParticleTriSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mParticleTriSB);

	//mParticleForSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mParticleForSB);

	//mLightEnableSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSLighting, false);
	GFX->endStateBlock(mLightEnableSB);

	//mLightStoreSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSLighting, false);
	GFX->endStateBlock(mLightStoreSB);

	//mTexOpSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->endStateBlock(mTexOpSB);

	//mTransflagSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mTransflagSB);

	//mTransSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mTransSB);

	//mAlphaBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mAlphaBlendSB);

	//mZWriteFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->endStateBlock(mZWriteFalseSB);

	//mZTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mZTrueSB);

	//mZFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->endStateBlock(mZFalseSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mClearSB);
}


void RenderTranslucentMgr::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mCullCCWSB)
	{
		mCullCCWSB->release();
	}

	if (mCullNoneSB)
	{
		mCullNoneSB->release();
	}

	if (mSetTransSB)
	{
		mSetTransSB->release();
	}

	if (mParticleOneSB)
	{
		mParticleOneSB->release();
	}

	if (mParticleTwoSB)
	{
		mParticleTwoSB->release();
	}

	if (mParticleTriSB)
	{
		mParticleTriSB->release();
	}

	if (mParticleForSB)
	{
		mParticleForSB->release();
	}

	if (mLightEnableSB)
	{
		mLightEnableSB->release();
	}

	if (mLightStoreSB)
	{
		mLightStoreSB->release();
	}

	if (mTexOpSB)
	{
		mTexOpSB->release();
	}

	if (mTransflagSB)
	{
		mTransflagSB->release();
	}

	if (mTransSB)
	{
		mTransSB->release();
	}

	if (mAlphaBlendSB)
	{
		mAlphaBlendSB->release();
	}

	if (mZWriteFalseSB)
	{
		mZWriteFalseSB->release();
	}

	if (mZTrueSB)
	{
		mZTrueSB->release();
	}

	if (mZFalseSB)
	{
		mZFalseSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void RenderTranslucentMgr::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mCullCCWSB = new GFXD3D9StateBlock;
		mCullNoneSB = new GFXD3D9StateBlock;
		mSetTransSB = new GFXD3D9StateBlock;
		mParticleOneSB = new GFXD3D9StateBlock;
		mParticleTwoSB = new GFXD3D9StateBlock;
		mParticleTriSB = new GFXD3D9StateBlock;
		mParticleForSB = new GFXD3D9StateBlock;
		mLightEnableSB = new GFXD3D9StateBlock;
		mLightStoreSB = new GFXD3D9StateBlock;
		mTexOpSB = new GFXD3D9StateBlock;
		mTransflagSB = new GFXD3D9StateBlock;
		mTransSB = new GFXD3D9StateBlock;
		mAlphaBlendSB = new GFXD3D9StateBlock;
		mZWriteFalseSB = new GFXD3D9StateBlock;
		mZTrueSB = new GFXD3D9StateBlock;
		mZFalseSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void RenderTranslucentMgr::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mCullCCWSB);
	SAFE_DELETE(mCullNoneSB);
	SAFE_DELETE(mSetTransSB);
	SAFE_DELETE(mParticleOneSB);
	SAFE_DELETE(mParticleTwoSB);
	SAFE_DELETE(mParticleTriSB);
	SAFE_DELETE(mParticleForSB);
	SAFE_DELETE(mLightEnableSB);
	SAFE_DELETE(mLightStoreSB);
	SAFE_DELETE(mTexOpSB);
	SAFE_DELETE(mTransflagSB);
	SAFE_DELETE(mTransSB);
	SAFE_DELETE(mAlphaBlendSB);
	SAFE_DELETE(mZWriteFalseSB);
	SAFE_DELETE(mZTrueSB);
	SAFE_DELETE(mZFalseSB);
	SAFE_DELETE(mClearSB);
}
