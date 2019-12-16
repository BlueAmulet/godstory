//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "processedCustomMaterial.h"
#include "gfx/cubemapData.h"
#include "materials/sceneData.h"
#include "shaderGen/featureMgr.h"
#include SHADER_CONSTANT_INCLUDE_FILE
#include "materials/customMaterial.h"
#include "materials/shaderData.h"
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
GFXStateBlock* ProcessedCustomMaterial::mAlphaTrueSB		= NULL;
GFXStateBlock* ProcessedCustomMaterial::mAlphaFalseSB	= NULL;
//z write
GFXStateBlock* ProcessedCustomMaterial::mZWriteTrueSB	= NULL;
GFXStateBlock* ProcessedCustomMaterial::mZWriteFalseSB	= NULL;
//alpha test
//static GFXStateBlock* mAlphaTestTrueSB;
GFXStateBlock* ProcessedCustomMaterial::mAlphaRef0SB	= NULL;//ref = 0
GFXStateBlock* ProcessedCustomMaterial::mAlphaRef1SB	= NULL;//ref = 1
GFXStateBlock* ProcessedCustomMaterial::mAlphaRef20SB	= NULL;//ref = 20
GFXStateBlock* ProcessedCustomMaterial::mAlphaTestFalseSB	= NULL;
//cull mode
GFXStateBlock* ProcessedCustomMaterial::mCullNoneSB	= NULL;
GFXStateBlock* ProcessedCustomMaterial::mCullStoreSB	= NULL;
//cleanup
GFXStateBlock* ProcessedCustomMaterial::mCleanupSB	= NULL;

//------------------------------------------------------------------------------
//成员函数
//
//------------------------------------------------------------------------------
ProcessedCustomMaterial::ProcessedCustomMaterial(Material &mat)
{
   mMaterial = &mat;
   mHasSetStageData = false;
   mHasGlow = false;
   mMaxStages = 0;
   mCullMode = -1;
   mMaxTex = 0;
}

void ProcessedCustomMaterial::setStageData()
{
   // Only do this once
   if( mHasSetStageData ) return;
   mHasSetStageData = true;

   // Should probably be keeping this as a member.
   CustomMaterial* custMat = static_cast<CustomMaterial*>(mMaterial);

   S32 i;

   // Loop through all the possible textures, set the right flags, and load them if needed
   for( i=0; i<CustomMaterial::MAX_TEX_PER_PASS; i++ )
   {
      mFlags[i] = Material::NoTexture;   // Set none as the default in case none of the cases below catch it.

      if( !custMat->texFilename[i] ) continue;

      if(!dStrcmp(custMat->texFilename[i], "$dynamiclight"))
      {
         mFlags[i] = Material::DynamicLight;
         mMaxTex = i+1;
         continue;
      }

      if(!dStrcmp(custMat->texFilename[i], "$dynamiclightmask"))
      {
         mFlags[i] = Material::DynamicLightMask;
         mMaxTex = i+1;
         continue;
      }

      if( !dStrcmp( custMat->texFilename[i], "$lightmap" ) )
      {
         mFlags[i] = Material::Lightmap;
         mMaxTex = i+1;
         continue;
      }

      if( !dStrcmp( custMat->texFilename[i], "$normmap" ) )
      {
         mFlags[i] = Material::NormLightmap;
         mMaxTex = i+1;
         continue;
      }

      if( !dStrcmp( custMat->texFilename[i], "$fog" ) )
      {
         mFlags[i] = Material::Fog;
         mMaxTex = i+1;
         continue;
      }

      if( !dStrcmp( custMat->texFilename[i], "$cubemap" ) )
      {
         if( custMat->mCubemapData )
         {
            mFlags[i] = Material::Cube;
            mMaxTex = i+1;
         }
         else
         {
            Con::warnf( "Invalid cubemap data for CustomMaterial - %s : %s", custMat->getName(), custMat->cubemapName );
         }
         continue;
      }

      if( !dStrcmp( custMat->texFilename[i], "$dynamicCubemap" ) )
      {
         mFlags[i] = Material::SGCube;
         mMaxTex = i+1;
         continue;
      }

      if( !dStrcmp( custMat->texFilename[i], "$backbuff" ) )
      {
         mFlags[i] = Material::BackBuff;
         mMaxTex = i+1;
         continue;
      }

      if( !dStrcmp( custMat->texFilename[i], "$reflectbuff" ) )
      {
         mFlags[i] = Material::ReflectBuff;
         mMaxTex = i+1;
         continue;
      }

      if( !dStrcmp( custMat->texFilename[i], "$miscbuff" ) )
      {
         mFlags[i] = Material::Misc;
         mMaxTex = i+1;
         continue;
      }

      if( custMat->texFilename[i] && custMat->texFilename[i][0] )
      {
         mStages[0].tex[i] = createTexture( custMat->texFilename[i], &GFXDefaultStaticDiffuseProfile );
         mFlags[i] = Material::Standard;
         mMaxTex = i+1;
      }
   }

   // We only get one cubemap
   if( custMat->mCubemapData )
   {
      custMat->mCubemapData->createMap();
      mStages[0].cubemap = mMaterial->mCubemapData->cubemap;
   }

   // Load our shader, if we have one.
   if( custMat->mShaderData && !custMat->mShaderData->getShader() )
   {
      custMat->mShaderData->initShader();
   }

   // Glow!
   if(custMat->glow[0])
      mHasGlow = true;
}

void ProcessedCustomMaterial::init(SceneGraphData& sgData, GFXVertexFlags vertFlags)
{
   // Just have to load the stage data, also loads the shader
   setStageData();
   return;
}

bool ProcessedCustomMaterial::hasCubemap(U32 pass)
{
   // If the material doesn't have a cubemap, we don't
   if( mMaterial->mCubemapData ) return true;
   else return false;
}

bool ProcessedCustomMaterial::setNextRefractPass(bool refractOn, U32 pass)
{
   // Should probably be keeping this as a member.
   CustomMaterial* custMat = static_cast<CustomMaterial*>(mMaterial);
   return ((pass == 0) && (refractOn == custMat->refract));
}

bool ProcessedCustomMaterial::setupPassInternal(SceneGraphData& sgData, U32 pass)
{
   // If we're rendering refractive objects, and we aren't refracting, then we're done rendering.
   if( !setNextRefractPass( sgData.refractPass, pass ) )
   {
      cleanup(pass); // Isn't this handled by MatInstance?
      return false;
   }

   // Store cullmode
   if( mMaterial->doubleSided )
   {
#ifdef STATEBLOCK
		AssertFatal(mCullNoneSB, "ProcessedCustomMaterial::setupPassInternal -- mCullNoneSB cannot be NULL.");
		mCullNoneSB->apply();
		AssertFatal(mCullStoreSB, "ProcessedCustomMaterial::setupPassInternal -- mCullStoreSB cannot be NULL.");
		mCullStoreSB->capture();
		mCullMode = 1;
#else
      GFX->setCullMode( GFXCullNone );
      mCullMode = GFX->getCullMode();
#endif

   }

   // Deal with translucency
   if( mMaterial->translucent )
   {
#ifdef STATEBLOCK
		if (mMaterial->translucentBlendOp != Material::None )
		{
			AssertFatal(mAlphaTrueSB, "ProcessedCustomMaterial::setupPassInternal -- mAlphaTrueSB cannot be NULL.");
			mAlphaTrueSB->apply();
		} 
		else
		{
			AssertFatal(mAlphaFalseSB, "ProcessedCustomMaterial::setupPassInternal -- mAlphaFalseSB cannot be NULL.");
			mAlphaFalseSB->apply();
		}

		setBlendState( mMaterial->translucentBlendOp ); 

		if (mMaterial->translucentZWrite)
		{
			AssertFatal(mZWriteTrueSB, "ProcessedCustomMaterial::setupPassInternal -- mZWriteTrueSB cannot be NULL.");
			mZWriteTrueSB->apply();			 
		} 
		else
		{
			AssertFatal(mZWriteFalseSB, "ProcessedCustomMaterial::setupPassInternal -- mZWriteFalseSB cannot be NULL.");
			mZWriteFalseSB->apply();
		}

		if (mMaterial->alphaTest)
		{
			switch (mMaterial->alphaRef)
			{
			case 0:
				AssertFatal(mAlphaRef0SB, "ProcessedShaderMaterial::setupPassInternal -- mAlphaRef0SB cannot be NULL.");
				mAlphaRef0SB->apply();
				break;
			case 1:
				AssertFatal(mAlphaRef1SB, "ProcessedShaderMaterial::setupPassInternal -- mAlphaRef1SB cannot be NULL.");
				mAlphaRef1SB->apply();
				break;
			case 20:
				AssertFatal(mAlphaRef20SB, "ProcessedShaderMaterial::setupPassInternal -- mAlphaRef20SB cannot be NULL.");
				mAlphaRef20SB->apply();
				break;
			default:
				Con::errorf("ProcessedShaderMaterial::setupPassInternal -- 没有参考值(%d)相对应的状态块。", mMaterial->alphaRef);
			}
		} 
		else
		{
			AssertFatal(mAlphaTestFalseSB, "ProcessedShaderMaterial::setupPassInternal -- mAlphaTestFalseSB cannot be NULL.");
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
   else
   {
#ifdef STATEBLOCK
		AssertFatal(mAlphaFalseSB, "ProcessedCustomMaterial::setupPassInternal -- mAlphaFalseSB cannot be NULL.");
		mAlphaFalseSB->apply();
#else
      GFX->setAlphaBlendEnable( false );
#endif

   }

   // Do more stuff
   setupSubPass( sgData, pass );

   return true;
}

void ProcessedCustomMaterial::setupSubPass(SceneGraphData& sgData, U32 pass)
{
   CustomMaterial* custMat = static_cast<CustomMaterial*>(mMaterial);

   // Set the right blend state
   setBlendState( custMat->blendOp );

   // activate shader
   if( custMat->mShaderData && custMat->mShaderData->getShader() )
   {
      custMat->mShaderData->getShader()->process();
   }
   else
   {
      GFX->disableShaders();
#ifdef STATEBLOCK
		GFX->setTextureStageState(mMaxTex, GFXTSSColorOp, GFXTOPDisable);
#else
		GFX->setTextureStageColorOp( mMaxTex, GFXTOPDisable );
#endif
   }

   // Set our textures
   setTextureStages( sgData, pass );
}

bool ProcessedCustomMaterial::setupPass(SceneGraphData& sgData, U32 pass)
{
   // This setTexTrans nonsense is set to make sure that setTextureTransforms()
   // is called only once per material draw, not per pass
   static bool setTexTrans = false;
   if( !setTexTrans )
   {
      setTextureTransforms();
      setTexTrans = true;
   }

   if( setupPassInternal( sgData, pass ) )
   {
      setShaderConstants(sgData, pass);
      GFX->setPixelShaderConstF( PC_ACCUM_TIME, &CustomMaterial::mAccumTime, 1, 1 );
      return true;
   }
   else
   {
      setTexTrans = false;
      return false;
   }
}

void ProcessedCustomMaterial::setTextureStages(SceneGraphData &sgData, U32 pass )
{

   for( U32 i=0; i<mMaxTex; i++ )
   {
#ifdef STATEBLOCK
		GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPModulate);
#else
		GFX->setTextureStageColorOp( i, GFXTOPModulate );
#endif
      U32 currTexFlag = mFlags[i];
      if (!gClientSceneGraph->getLightManager()->setTextureStage(sgData, currTexFlag, i))
      {
         switch( currTexFlag )
         {
         case 0:
         default:
            break;

         case Material::Mask:
         case Material::Standard:
         case Material::Bump:
         case Material::Detail:
            {
#ifdef STATEBLOCK
					GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
					GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
#else
					GFX->setTextureStageAddressModeU( i, GFXAddressWrap );
					GFX->setTextureStageAddressModeV( i, GFXAddressWrap );
#endif
               GFX->setTexture( i, mStages[0].tex[i] );
               break;
            }
         case Material::Lightmap:
            {
               GFX->setTexture( i, sgData.lightmap );
               break;
            }
         case Material::NormLightmap:
            {
               GFX->setTexture( i, sgData.normLightmap );
               break;
            }
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
         case Material::Cube:
            {
               GFX->setCubeTexture( i, mStages[0].cubemap );
               break;
            }
         case Material::SGCube:
            {
               GFX->setCubeTexture( i, sgData.cubemap );
               break;
            }
         case Material::BackBuff:
            {
               GFX->setTexture( i, sgData.backBuffTex );
               break;
            }
         case Material::ReflectBuff:
            {
               GFX->setTexture( i, sgData.reflectTex );
               break;
            }
         case Material::Misc:
            {
               GFX->setTexture( i, sgData.miscTex );
               break;
            }

         }
      }
   }
}

void ProcessedCustomMaterial::cleanup(U32 pass)
{
   // This needs to be modified to actually clean up the specified pass.
   // It's enough if we're just using shaders, but mix shader and FF 
   // and you're in a whole world of trouble.
   for( U32 i=0; i<mMaxTex; i++ )
   {
      // set up textures
      switch( mFlags[i] )
      {
      case 0:
      default:
         break;

      case Material::BackBuff:
         {
            // have to unbind render targets or D3D complains
            GFX->setTexture( i, NULL );
            break;
         }
      case Material::ReflectBuff:
         {
            // have to unbind render targets or D3D complains
            GFX->setTexture( i, NULL );
            break;
         }

      }
   }

   // Reset cull mode
   if( mCullMode != -1 )
   {
#ifdef STATEBLOCK
		AssertFatal(mCullStoreSB, "ProcessedShaderMaterial::cleanup -- mCullStoreSB cannot be NULL.");
		mCullStoreSB->apply();
#else
		GFX->setCullMode( (GFXCullMode) mCullMode );
#endif
      mCullMode = -1;
   }
#ifdef STATEBLOCK
	AssertFatal(mCleanupSB, "ProcessedCustomMaterial::cleanup -- mCleanupSB cannot be NULL.");
	mCleanupSB->apply();
#else
	// Misc. cleanup
	GFX->setAlphaBlendEnable( false );
	GFX->setAlphaTestEnable( false );
	GFX->setZWriteEnable( true );
#endif

}

void ProcessedCustomMaterial::initsb()
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
		mCullNoneSB = new GFXD3D9StateBlock;
		mCullStoreSB = new GFXD3D9StateBlock;
		mCleanupSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void ProcessedCustomMaterial::shutdown()
{
	SAFE_DELETE(mAlphaTrueSB);
	SAFE_DELETE(mAlphaFalseSB);
	SAFE_DELETE(mZWriteTrueSB);
	SAFE_DELETE(mZWriteFalseSB);
	SAFE_DELETE(mAlphaRef0SB);
	SAFE_DELETE(mAlphaRef1SB);
	SAFE_DELETE(mAlphaRef20SB);
	SAFE_DELETE(mAlphaTestFalseSB);
	SAFE_DELETE(mCullNoneSB);
	SAFE_DELETE(mCullStoreSB);
	SAFE_DELETE(mCleanupSB);
}

void ProcessedCustomMaterial::resetStateBlock()
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

void ProcessedCustomMaterial::releaseStateBlock()
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

