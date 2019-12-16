//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "processedFFMaterial.h"
#include "gfx/cubemapData.h"
#include "materials/sceneData.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//------------------------------------------------------------------------------
//宏定义
//
//------------------------------------------------------------------------------
//#define STATEBLOCK
//------------------------------------------------------------------------------
//静态成员
//状态块
//------------------------------------------------------------------------------
//alpha blend
GFXStateBlock* ProcessedFFMaterial::mAlphaTrueSB		= NULL;
GFXStateBlock* ProcessedFFMaterial::mAlphaFalseSB	= NULL;
//light
GFXStateBlock* ProcessedFFMaterial::mLitEnableSB	= NULL;
GFXStateBlock* ProcessedFFMaterial::mLitBlendSB	= NULL;
//z write
GFXStateBlock* ProcessedFFMaterial::mZWriteFalseSB	= NULL;
GFXStateBlock* ProcessedFFMaterial::mZWriteTrueSB	= NULL;
//alpha test
//static GFXStateBlock* mAlphaTestTrueSB;
GFXStateBlock* ProcessedFFMaterial::mAlphaRef0SB	= NULL;//ref = 0
GFXStateBlock* ProcessedFFMaterial::mAlphaRef1SB	= NULL;//ref = 1
GFXStateBlock* ProcessedFFMaterial::mAlphaRef20SB	= NULL;//ref = 20
GFXStateBlock* ProcessedFFMaterial::mAlphaTestFalseSB	= NULL;
//cull mode
GFXStateBlock* ProcessedFFMaterial::mCullNoneSB	= NULL;
GFXStateBlock* ProcessedFFMaterial::mCullStoreSB	= NULL;
//cleanup
GFXStateBlock* ProcessedFFMaterial::mCleanupSB	= NULL;

ProcessedFFMaterial::ProcessedFFMaterial()
{

}

ProcessedFFMaterial::ProcessedFFMaterial(Material &mat)
{
   mMaterial = &mat;
   mHasSetStageData = false;
   mCullMode = -1;
   mHasGlow = false;
}

void ProcessedFFMaterial::createPasses(U32 stageNum, SceneGraphData& sgData)
{
   FixedFuncFeatureData featData;
   determineFeatures(stageNum, featData, sgData);
   // Just create a simple pass!
   addPass(0, featData);
}

void ProcessedFFMaterial::determineFeatures(U32 stageNum, FixedFuncFeatureData& featData, SceneGraphData& sgData)
{
   if(mMaterial->stages[stageNum].tex[ProcessedMaterial::BaseTex])
   {
      featData.features[FixedFuncFeatureData::BaseTex] = true;
   }
   if(!mMaterial->emissive[stageNum])
   {
      if( sgData.lightmap)
      {
         featData.features[FixedFuncFeatureData::Lightmap] = true;
      }
   }
}

U32 ProcessedFFMaterial::getNumStages()
{
   // Loops through all stages to determine how many stages we actually use
   U32 numStages = 0;

   U32 i;
   for( i=0; i<Material::MAX_STAGES; i++ )
   {
      // Assume stage is inactive
      bool stageActive = false;

      // Cubemaps only on first stage
      if( i == 0 )
      {
         // If we have a cubemap the stage is active
         if( mMaterial->mCubemapData || mMaterial->dynamicCubemap )
         {
            numStages++;
            continue;
         }
      }

      // Loop through all features
      for( U32 j=0; j<NumFeatures; j++ )
      {
         // If we have a texture for the feature the stage is active
         if( mMaterial->stages[i].tex[j] )
         {
            stageActive = true;
            break;
         }
      }

      // If this stage has specular lighting, it's active
      if( mMaterial->pixelSpecular[i] ||
         mMaterial->vertexSpecular[i] )
      {
         stageActive = true;
      }

      // Increment the number of active stages
      numStages += stageActive;
   }


   return numStages;
}

void ProcessedFFMaterial::cleanup(U32 pass)
{
   if( mPasses.size())
   {
      for( U32 i=0; i<mPasses[pass].numTex; i++ )
      {
         GFX->setTexture(i, NULL);
      }
   }

   // Reset the previous cullmode
   if( mCullMode != -1 )
   {
#ifdef STATEBLOCK
		AssertFatal(mCullStoreSB, "ProcessedFFMaterial::cleanup -- mCullStoreSB cannot be NULL.");
		mCullStoreSB->apply();
#else
		GFX->setCullMode( (GFXCullMode) mCullMode );
#endif
      mCullMode = -1;
   }

#ifdef STATEBLOCK
	AssertFatal(mCleanupSB, "ProcessedFFMaterial::cleanup -- mCleanupSB cannot be NULL.");
	mCleanupSB->apply();
#else
	// Misc. cleanup
	GFX->setAlphaBlendEnable( false );
	GFX->setAlphaTestEnable( false );
	GFX->setZWriteEnable( true );
#endif
}

bool ProcessedFFMaterial::setupPass(SceneGraphData& sgData, U32 pass)
{
   // Make sure we have a pass
   if(pass >= mPasses.size())
      return false;
   // blend op for multipassing  
   if( pass > 0 )
   {
#ifdef STATEBLOCK
		AssertFatal(mAlphaTrueSB, "ProcessedFFMaterial::setupPass -- mAlphaTrueSB cannot be NULL.");
		mAlphaTrueSB->apply();
#else
		GFX->setAlphaBlendEnable( true );
#endif
      setBlendState( mPasses[pass].blendOp );
   }
   else
   {
#ifdef STATEBLOCK
		AssertFatal(mAlphaFalseSB, "ProcessedFFMaterial::setupPass -- mAlphaFalseSB cannot be NULL.");
		mAlphaFalseSB->apply();
#else
		GFX->setAlphaBlendEnable( false );
#endif
   }

   // Deal with translucency
   if( mMaterial->translucent )
   {
#ifdef STATEBLOCK
		if (mMaterial->translucentBlendOp != Material::None )
		{
			AssertFatal(mAlphaTrueSB, "ProcessedFFMaterial::setupPass -- mAlphaTrueSB cannot be NULL.");
			mAlphaTrueSB->apply();
		} 
		else
		{
			AssertFatal(mAlphaFalseSB, "ProcessedFFMaterial::setupPass -- mAlphaFalseSB cannot be NULL.");
			mAlphaFalseSB->apply();
		}

		setBlendState( mMaterial->translucentBlendOp ); 

		if (mMaterial->translucentZWrite)
		{
			AssertFatal(mZWriteTrueSB, "ProcessedFFMaterial::setupPass -- mZWriteTrueSB cannot be NULL.");
			mZWriteTrueSB->apply();			 
		} 
		else
		{
			AssertFatal(mZWriteFalseSB, "ProcessedFFMaterial::setupPass -- mZWriteFalseSB cannot be NULL.");
			mZWriteFalseSB->apply();
		}

		if (mMaterial->alphaTest)
		{
			switch (mMaterial->alphaRef)
			{
			case 0:
				AssertFatal(mAlphaRef0SB, "ProcessedFFMaterial::setupPass -- mAlphaRef0SB cannot be NULL.");
				mAlphaRef0SB->apply();
				break;
			case 1:
				AssertFatal(mAlphaRef1SB, "ProcessedFFMaterial::setupPass -- mAlphaRef1SB cannot be NULL.");
				mAlphaRef1SB->apply();
				break;
			case 20:
				AssertFatal(mAlphaRef20SB, "ProcessedFFMaterial::setupPass -- mAlphaRef20SB cannot be NULL.");
				mAlphaRef20SB->apply();
				break;
			default:
				Con::errorf("ProcessedFFMaterial::setupPass -- 没有参考值(%d)相对应的状态块。", mMaterial->alphaRef);
			}
		} 
		else
		{
			AssertFatal(mAlphaTestFalseSB, "ProcessedFFMaterial::setupPass -- mAlphaTestFalseSB cannot be NULL.");
			mAlphaTestFalseSB->apply();
		}
#else
		GFX->setAlphaBlendEnable( mMaterial->translucentBlendOp != Material::None );
		setBlendState( mMaterial->translucentBlendOp );
		GFX->setZWriteEnable( mMaterial->translucentZWrite );
		GFX->setAlphaTestEnable( mMaterial->alphaTest );
		GFX->setAlphaRef( mMaterial->alphaRef );
		GFX->setAlphaFunc( GFXCmpGreaterEqual );

		//set up register combiners
		GFX->setTextureStageAlphaOp( 0, GFXTOPModulate );
		GFX->setTextureStageAlphaOp( 1, GFXTOPDisable );
		GFX->setTextureStageAlphaArg1( 0, GFXTATexture );
		GFX->setTextureStageAlphaArg2( 0, GFXTADiffuse );
#endif
   }

   // Store the current cullmode so we can reset it when we're done
   if( mMaterial->doubleSided )
   {
#ifdef STATEBLOCK
		AssertFatal(mCullNoneSB, "ProcessedFFMaterial::setupPass -- mCullNoneSB cannot be NULL.");
		mCullNoneSB->apply();
		AssertFatal(mCullStoreSB, "ProcessedFFMaterial::setupPass -- mCullStoreSB cannot be NULL.");
		mCullStoreSB->capture();
		mCullMode = 1;//使该值不为-1
#else
		mCullMode = GFX->getCullMode();
		GFX->setCullMode( GFXCullNone );
#endif
   }

   // Bind our textures
   setTextureStages(sgData, pass);
   return true;
}

void ProcessedFFMaterial::setTextureStages(SceneGraphData& sgData, U32 pass)
{
   // We may need to do some trickery in here for fixed function, this is just copy/paste from MatInstance
#ifdef POWER_DEBUG
   AssertFatal( pass<mPasses.size(), "Pass out of bounds" );
#endif

   for( U32 i=0; i<mPasses[pass].numTex; i++ )
   {
#ifdef STATEBLOCK
		GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPModulate);
#else
		GFX->setTextureStageColorOp( i, GFXTOPModulate );
#endif
      U32 currTexFlag = mPasses[pass].texFlags[i];
      if (!gClientSceneGraph->getLightManager()->setTextureStage(sgData, currTexFlag, i))
      {
         switch( currTexFlag )
         {
         case 0:
#ifdef STATEBLOCK
				GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
				GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
#else
				GFX->setTextureStageAddressModeU( i, GFXAddressWrap );
				GFX->setTextureStageAddressModeV( i, GFXAddressWrap );
#endif

            if( mMaterial->isIFL() && sgData.miscTex )
            {
               GFX->setTexture( i, sgData.miscTex );
            }
            else
            {
               GFX->setTexture( i, mPasses[pass].tex[i] );
            }
#ifdef STATEBLOCK
				GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPModulate);			
#else
				GFX->setTextureStageColorOp(i, GFXTOPModulate);
#endif
            break;

         case Material::NormalizeCube:
            GFX->setCubeTexture(i, Material::getNormalizeCube());
            break;

         case Material::Lightmap:
            GFX->setTexture( i, sgData.lightmap );
#ifdef STATEBLOCK
				GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPModulate);			
#else
				GFX->setTextureStageColorOp(i, GFXTOPModulate);
#endif
            break;

         case Material::NormLightmap:
            GFX->setTexture( i, sgData.normLightmap );
            break;

         case Material::Cube:
            GFX->setTexture( i, mPasses[pass].tex[GFXShaderFeatureData::BaseTex] );
            break;

         case Material::SGCube:
            // No cubemap support just yet
            //GFX->setCubeTexture( i, sgData.cubemap );
            GFX->setTexture( i, mPasses[pass].tex[GFXShaderFeatureData::BaseTex] );
            break;

         case Material::Fog:
            {
#ifdef STATEBLOCK	
					GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressClamp);
					GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressClamp);
#else
					GFX->setTextureStageAddressModeU( i, GFXAddressClamp );
					GFX->setTextureStageAddressModeV( i, GFXAddressClamp );
#endif
               GFX->setTexture( i, sgData.fogTex );
               break;
            }

         case Material::BackBuff:
            GFX->setTexture( i, sgData.backBuffTex );
            break;
         }
      }
   }
}

void ProcessedFFMaterial::setShaderConstants(const SceneGraphData &sgData, U32 pass)
{

}

void ProcessedFFMaterial::setObjectXForm(MatrixF xform, U32 pass)
{

}

void ProcessedFFMaterial::setWorldXForm(MatrixF xform)
{
   // The matrix we're given is Model * View * Projection, so set two matrices to identity and the third to this.
   MatrixF ident(1);
   xform.transpose();
   GFX->setWorldMatrix(ident);
   GFX->setViewMatrix(ident);
   GFX->setProjectionMatrix(xform);
}

void ProcessedFFMaterial::setLightInfo(const SceneGraphData& sgData, U32 pass)
{
   setPrimaryLightInfo(sgData.objTrans, sgData.light, pass);
   setSecondaryLightInfo(sgData.objTrans, sgData.lightSecondary);   
}

void ProcessedFFMaterial::setPrimaryLightInfo(MatrixF objTrans, LightInfo* light, U32 pass)
{
   // Just in case
   GFX->setGlobalAmbientColor(ColorF(0.0f, 0.0f, 0.0f, 1.0f));
   if(light->mType == LightInfo::Ambient)
   {
      // Ambient light
      GFX->setGlobalAmbientColor(light->mAmbient);
      return;
   }

   GFX->setLight(0, NULL);
   GFX->setLight(1, NULL);
   // This is a quick hack that lets us use FF lights
   GFXLightMaterial lightMat;
   lightMat.ambient = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
   lightMat.diffuse = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
   lightMat.emissive = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
   lightMat.specular = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
   lightMat.shininess = 128.0f;
   GFX->setLightMaterial(lightMat);

#ifdef STATEBLOCK	
	AssertFatal(mLitEnableSB, "ProcessedFFMaterial::setPrimaryLightInfo -- mLitEnableSB cannot be NULL.");
	mLitEnableSB->apply();
#else
   GFX->setLightingEnable(true);
#endif
   // set object transform
   objTrans.inverse();

   // fill in primary light
   //-------------------------
   GFXLightInfo xlatedLight;
   light->setGFXLight(&xlatedLight);
   Point3F lightPos = light->mPos;
   Point3F lightDir = light->mDirection;
   objTrans.mulP(lightPos);
   objTrans.mulV(lightDir);

   xlatedLight.mPos = lightPos;
   xlatedLight.mDirection = lightDir;

   GFX->setLight(0, &xlatedLight);
}

void ProcessedFFMaterial::setSecondaryLightInfo(MatrixF objTrans, LightInfo* light)
{
   // set object transform
   objTrans.inverse();

   // fill in secondary light
   //-------------------------
   GFXLightInfo xlatedLight;
   light->setGFXLight(&xlatedLight);

   Point3F lightPos = light->mPos;
   Point3F lightDir = light->mDirection;
   objTrans.mulP(lightPos);
   objTrans.mulV(lightDir);

   xlatedLight.mPos = lightPos;
   xlatedLight.mDirection = lightDir;

   GFX->setLight(1, &xlatedLight);
}

void ProcessedFFMaterial::setEyePosition(MatrixF objTrans, Point3F position, U32 pass)
{

}

void ProcessedFFMaterial::init(SceneGraphData& sgData, GFXVertexFlags vertFlags)
{
   setStageData();
   // Just create a simple pass
   createPasses(0, sgData);
}

void ProcessedFFMaterial::setStageData()
{
   // Make sure we don't do this more than once
   if( mHasSetStageData ) return;
   mHasSetStageData = true;

   // Load the textures for every possible stage
   for( U32 i=0; i<Material::MAX_STAGES; i++ )
   {
      if( mMaterial->baseTexFilename[i] && mMaterial->baseTexFilename[i][0] )
      {
         mStages[i].tex[BaseTex] = createTexture( mMaterial->baseTexFilename[i], &GFXDefaultStaticDiffuseProfile );
      }

      if( mMaterial->detailFilename[i] && mMaterial->detailFilename[i][0] )
      {
         mStages[i].tex[DetailMap] = createTexture( mMaterial->detailFilename[i], &GFXDefaultStaticDiffuseProfile );
      }

      if( mMaterial->bumpFilename[i] && mMaterial->bumpFilename[i][0] )
      {
         mStages[i].tex[BumpMap] = createTexture( mMaterial->bumpFilename[i], &GFXDefaultStaticDiffuseProfile );
      }

      if( mMaterial->envFilename[i] && mMaterial->envFilename[i][0] )
      {
         mStages[i].tex[EnvMap] = createTexture( mMaterial->envFilename[i], &GFXDefaultStaticDiffuseProfile );
      }
   }

   // If we have a cubemap, load it for stage 0 (cubemaps are not supported on other stages)
   if( mMaterial->mCubemapData )
   {
      mMaterial->mCubemapData->createMap();
      mStages[0].cubemap = mMaterial->mCubemapData->cubemap;
   }
}

void ProcessedFFMaterial::setTextureTransforms()
{

}

void ProcessedFFMaterial::addPass(U32 stageNum, FixedFuncFeatureData& featData)
{
   U32 numTex = 0;
   // Just creates a simple pass, but it can still glow!
   RenderPassData rpd;
   // Base texture, texunit 0
   if(featData.features[FixedFuncFeatureData::BaseTex])
   {
      rpd.tex[0] = mStages[stageNum].tex[BaseTex];
      rpd.texFlags[0] = 0;
      numTex++;
   }
   // lightmap, texunit 1
   if(featData.features[FixedFuncFeatureData::Lightmap])
   {
      rpd.texFlags[1] = Material::Lightmap;
      numTex++;
   }
   rpd.numTex = numTex;
   rpd.stageNum = stageNum;
   rpd.glow = false;

   mPasses.push_back( rpd );
}

void ProcessedFFMaterial::setPassBlendOp()
{

}

void ProcessedFFMaterial::setLightingBlendFunc()
{
#ifdef STATEBLOCK	
	AssertFatal(mLitBlendSB, "ProcessedFFMaterial::setLightingBlendFunc -- mLitBlendSB cannot be NULL.");
	mLitBlendSB->apply();
#else
	// don't break the material multipass rendering...
	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendOne);
	GFX->setDestBlend(GFXBlendOne);
#endif

}


void ProcessedFFMaterial::initsb()
{
	if (mAlphaTrueSB == NULL)
	{
		mAlphaTrueSB = new GFXD3D9StateBlock;
		mAlphaTrueSB->registerResourceWithDevice(GFX);
		mAlphaTrueSB->mResurrect = &resetStateBlock;
		mAlphaTrueSB->mZombify = &releaseStateBlock;

		mAlphaFalseSB = new GFXD3D9StateBlock;
		mZWriteTrueSB = new GFXD3D9StateBlock;
		mZWriteFalseSB = new GFXD3D9StateBlock;
		mAlphaRef0SB = new GFXD3D9StateBlock;
		mAlphaRef1SB = new GFXD3D9StateBlock;
		mAlphaRef20SB = new GFXD3D9StateBlock;
		mAlphaTestFalseSB = new GFXD3D9StateBlock;
		mLitEnableSB = new GFXD3D9StateBlock;
		mLitBlendSB = new GFXD3D9StateBlock;
		mCullNoneSB = new GFXD3D9StateBlock;
		mCullStoreSB = new GFXD3D9StateBlock;
		mCleanupSB = new GFXD3D9StateBlock;
	}
}

void ProcessedFFMaterial::shutdown()
{
	SAFE_DELETE(mAlphaTrueSB);
	SAFE_DELETE(mAlphaFalseSB);
	SAFE_DELETE(mZWriteTrueSB);
	SAFE_DELETE(mZWriteFalseSB);
	SAFE_DELETE(mAlphaRef0SB);
	SAFE_DELETE(mAlphaRef1SB);
	SAFE_DELETE(mAlphaRef20SB);
	SAFE_DELETE(mAlphaTestFalseSB);
	SAFE_DELETE(mLitEnableSB);
	SAFE_DELETE(mLitBlendSB);
	SAFE_DELETE(mCullNoneSB);
	SAFE_DELETE(mCullStoreSB);
	SAFE_DELETE(mCleanupSB);
}

void ProcessedFFMaterial::resetStateBlock()
{
	//mAlphaTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mAlphaTrueSB);

	//mAlphaFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mAlphaFalseSB);

	//mZWriteTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->endStateBlock(mZWriteTrueSB);

	//mZWriteFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->endStateBlock(mZWriteFalseSB);

	//mAlphaRef0SB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual );

	// set up register combiners
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->setTextureStageState(0, GFXTSSAlphaArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSAlphaArg2, GFXTADiffuse);
	GFX->endStateBlock(mAlphaRef0SB);

	//mAlphaRef1SB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 1);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual );

	// set up register combiners
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->setTextureStageState(0, GFXTSSAlphaArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSAlphaArg2, GFXTADiffuse);
	GFX->endStateBlock(mAlphaRef1SB);

	//mAlphaRef20SB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 20);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual );

	// set up register combiners
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->setTextureStageState(0, GFXTSSAlphaArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSAlphaArg2, GFXTADiffuse);
	GFX->endStateBlock(mAlphaRef20SB);

	//mAlphaTestFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->endStateBlock(mAlphaTestFalseSB);

	//mLitEnableSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSLighting, GFXCullNone);
	GFX->endStateBlock(mLitEnableSB);

	//mLitBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mLitBlendSB);

	//mCullNoneSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mCullNoneSB);

	//mCullStoreSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCW);
	GFX->endStateBlock(mCullStoreSB);

	//mCleanupSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->endStateBlock(mCleanupSB);

}

void ProcessedFFMaterial::releaseStateBlock()
{
	if (mAlphaTrueSB)
	{
		mAlphaTrueSB->release();
	}

	if (mAlphaFalseSB)
	{
		mAlphaFalseSB->release();
	}

	if (mZWriteTrueSB)
	{
		mZWriteTrueSB->release();
	}

	if (mZWriteFalseSB)
	{
		mZWriteFalseSB->release();
	}

	if (mAlphaRef0SB)
	{
		mAlphaRef0SB->release();
	}

	if (mAlphaRef1SB)
	{
		mAlphaRef1SB->release();
	}

	if (mAlphaRef20SB)
	{
		mAlphaRef20SB->release();
	}

	if (mAlphaTestFalseSB)
	{
		mAlphaTestFalseSB->release();
	}

	if (mCullNoneSB)
	{
		mCullNoneSB->release();
	}

	if (mCullStoreSB)
	{
		mCullStoreSB->release();
	}

	if (mCleanupSB)
	{
		mCleanupSB->release();
	}
}

