//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "processedShaderMaterial.h"
#include "gfx/cubemapData.h"
#include "materials/sceneData.h"
#include "shaderGen/shaderFeature.h"
#include "materials/shaderData.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include SHADER_CONSTANT_INCLUDE_FILE

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
GFXStateBlock* ProcessedShaderMaterial::mAlphaTrueSB		= NULL;
GFXStateBlock* ProcessedShaderMaterial::mAlphaFalseSB	= NULL;
//force translate
GFXStateBlock* ProcessedShaderMaterial::mForceTranslateSB	= NULL;
//z write
GFXStateBlock* ProcessedShaderMaterial::mZWriteTrueSB	= NULL;
GFXStateBlock* ProcessedShaderMaterial::mZWriteFalseSB	= NULL;
//alpha test
//static GFXStateBlock* mAlphaTestTrueSB;
GFXStateBlock* ProcessedShaderMaterial::mAlphaRef0SB	= NULL;//ref = 0
GFXStateBlock* ProcessedShaderMaterial::mAlphaRef1SB	= NULL;//ref = 1
GFXStateBlock* ProcessedShaderMaterial::mAlphaRef20SB	= NULL;//ref = 20
GFXStateBlock* ProcessedShaderMaterial::mAlphaTestFalseSB	= NULL;
//z enable
GFXStateBlock* ProcessedShaderMaterial::mZEnableSB	= NULL;
GFXStateBlock* ProcessedShaderMaterial::mZEnableFalseSB	= NULL;
//disable color
GFXStateBlock* ProcessedShaderMaterial::mDisableColorSB	= NULL;
//enable blend color
GFXStateBlock* ProcessedShaderMaterial::mEnableBlendColorSB	= NULL;
//cull mode
GFXStateBlock* ProcessedShaderMaterial::mCullNoneSB	= NULL;
GFXStateBlock* ProcessedShaderMaterial::mCullStoreSB	= NULL;
//cleanup
GFXStateBlock* ProcessedShaderMaterial::mCleanupSB	= NULL;

//------------------------------------------------------------------------------
//成员函数
//
//------------------------------------------------------------------------------
ProcessedShaderMaterial::ProcessedShaderMaterial()
{

}

ProcessedShaderMaterial::ProcessedShaderMaterial(Material &mat)
{
   mMaterial = &mat;
   mHasSetStageData = false;
   mHasGlow = false;
   mMaxStages = 0;
   mCullMode = -1;
}

void ProcessedShaderMaterial::determineFeatures( U32 stageNum, GFXShaderFeatureData &fd, SceneGraphData& sgData )
{
   // Sanity check
   if( GFX->getPixelShaderVersion() == 0.0 )
   {
      AssertFatal(false, "Cannot create a shader material if we don't support shaders");
   }

   // CodeReview: [btr, 7/23/07] Is there a reason that this is a loop?  Why not just do fd.features[MyFeature] = blah?  (Except for the texture block at the bottom)  
   for( U32 i=0; i<GFXShaderFeatureData::NumFeatures; i++ )
   {
	   //Ray: VertexColor
	  if (i == GFXShaderFeatureData::VertexColor)
	  {
		  if(mVertFlags & GFXVertexFlagDiffuse)
			  fd.features[i] = true;
	  }

	  if (i == GFXShaderFeatureData::Translucent)
      {
         fd.features[i] = mMaterial->translucent;
      }
      // if normal/bump mapping disabled, continue
      if( Con::getBoolVariable( "$pref::Video::disableNormalmapping", false ) &&
         (i == GFXShaderFeatureData::BumpMap || i == GFXShaderFeatureData::LightNormMap) )
      {
         continue;
      }

      if((i == GFXShaderFeatureData::SelfIllumination) && mMaterial->emissive[stageNum])
         fd.features[i] = true;

      if((i == GFXShaderFeatureData::ExposureX2) &&
         (mMaterial->exposure[stageNum] == 2))
         fd.features[i] = true;

      if((i == GFXShaderFeatureData::ExposureX4) &&
         (mMaterial->exposure[stageNum] == 4))
         fd.features[i] = true;

      // texture coordinate animation
      if( i == GFXShaderFeatureData::TexAnim )
      {
         if( mMaterial->animFlags[stageNum] )
         {
            fd.features[i] = true;
         }
      }

      // vertex shading
      if( i == GFXShaderFeatureData::RTLighting )
      {
         if( sgData.useLightDir && 
            !mMaterial->emissive[stageNum])
         {
            fd.features[i] = true;
         }
      }

      // pixel specular
      if( GFX->getPixelShaderVersion() >= 2.0 )
      {
         if((i == GFXShaderFeatureData::PixSpecular) &&
            !Con::getBoolVariable( "$pref::Video::disablePixSpecular", false ) )
         {
            if( mMaterial->pixelSpecular[stageNum] )
            {
               fd.features[i] = true;
            }
         }
      }

      // vertex specular
      if( i == GFXShaderFeatureData::VertSpecular )
      {
         if( mMaterial->vertexSpecular[stageNum] )
         {
            fd.features[i] = true;
         }
      } 


      // lightmap
      if( i == GFXShaderFeatureData::LightMap && 
         !mMaterial->emissive[stageNum] &&
         !mMaterial->glow[stageNum] &&
         stageNum == (mMaxStages-1) )
      {
         if( sgData.lightmap)
         {
            fd.features[i] = true;
         }
      }

      // lightNormMap
      if( i == GFXShaderFeatureData::LightNormMap && 
         !mMaterial->emissive[stageNum] &&
         !mMaterial->glow[stageNum] &&
         stageNum == (mMaxStages-1) )
      {
         if( sgData.normLightmap )
         {
            fd.features[i] = true;
         }
      } 

      // cubemap
      if( i == GFXShaderFeatureData::CubeMap &&
         stageNum < 1 &&      // cubemaps only available on stage 0 for now - bramage
         ( ( mMaterial->mCubemapData && mMaterial->mCubemapData->cubemap ) || mMaterial->dynamicCubemap ) &&
         !Con::getBoolVariable( "$pref::Video::disableCubemapping", false ) )
      {
         fd.features[i] = true;
      }

      // Visibility
      if ( i == GFXShaderFeatureData::Visibility)
      {
         fd.features[i] = true;
      }     

	  if ( i == GFXShaderFeatureData::EffectLight && mMaterial->effectLight)
	  {
		 fd.features[i] = true;
	  }

      // Color multiply
      if (i == GFXShaderFeatureData::ColorMultiply)
      {
         if (mMaterial->colorMultiply[stageNum].alpha != 0)
            fd.features[i] = true;
      }

      // ColorExposure
      if ( i == GFXShaderFeatureData::ColorExposure)
              fd.features[i] = true;

      // These features only happen in the last state
      if (stageNum == (mMaxStages-1))
      {       
         // fog - last stage only
         if( i == GFXShaderFeatureData::Fog && sgData.useFog && !mMaterial->noFog)
         {
            fd.features[i] = true;
         } 
      }

      // textures
      if( mStages[stageNum].tex[i] )
      {
         fd.features[i] = true;
      }
   }
}

void ProcessedShaderMaterial::createPasses( GFXShaderFeatureData &stageFeatures, U32 stageNum, SceneGraphData& sgData )
{
   // Creates passes for the given stage
   RenderPassData passData;
   U32 texIndex = 0;

   for( U32 i=0; i<GFXShaderFeatureData::NumFeatures; i++ )
   {
      if( !stageFeatures.features[i] ) 
         continue;

      ShaderFeature *sf = gFeatureMgr.get( i );
      if (!sf)
         continue;

      U32 numTexReg = sf->getResources( passData.fData ).numTexReg;

      // adds pass if blend op changes for feature
      setPassBlendOp( sf, passData, texIndex, stageFeatures, stageNum, sgData );

      // Add pass if num tex reg is going to be too high
      if( passData.numTexReg + numTexReg > GFX->getNumSamplers() )
      {
         addPass( passData, texIndex, stageFeatures, stageNum, sgData );
         setPassBlendOp( sf, passData, texIndex, stageFeatures, stageNum, sgData );
      }

      passData.numTexReg += numTexReg;
      passData.fData.features[i] = true;
      sf->setTexData( mStages[stageNum], stageFeatures, passData, texIndex );

      // Add pass if tex units are maxed out
      if( texIndex > GFX->getNumSamplers() )
      {
         addPass( passData, texIndex, stageFeatures, stageNum, sgData );
         setPassBlendOp( sf, passData, texIndex, stageFeatures, stageNum, sgData );
      }
   }

   if( passData.fData.codify() )
   {
      addPass( passData, texIndex, stageFeatures, stageNum, sgData );
   }
} 

U32 ProcessedShaderMaterial::getNumStages()
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
	  for( U32 j=0; j<ProcessedMaterial::NumFeatures; j++ )
      {
         // If we have a texture for the feature the stage is active
         if( mStages[i].tex[j] )
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

void ProcessedShaderMaterial::cleanup(U32 pass)
{
   // This needs to be modified to actually clean up the specified pass.
   // It's enough if we're just using shaders, but mix shader and FF 
   // and you're in a whole world of trouble.
   if( mPasses.size())
   {
      // DX warning if dynamic cubemap not explicitly unbound
      for( U32 i=0; i<mPasses[pass].numTex; i++ )
      {
         if( mPasses[pass].texFlags[i] == Material::BackBuff )
         {
            GFX->setTexture( i, NULL );
            continue;
         }
         if( mPasses[pass].texFlags[i] == Material::SGCube )
         {
            GFX->setTexture( i, NULL );
            continue;
         }
      }
   }

   // Reset cull mode
   if( mCullMode != -1 )
   {
#ifdef STATEBLOCK
		AssertFatal(mCullStoreSB, "ProcessedShaderMaterial::cleanup -- mCullCWSB cannot be NULL.");
		mCullStoreSB->apply();
#else
      GFX->setCullMode( (GFXCullMode) mCullMode );
#endif
      mCullMode = -1;
   }
#ifdef STATEBLOCK
	AssertFatal(mCleanupSB, "ProcessedShaderMaterial::cleanup -- mCleanupSB cannot be NULL.");
	mCleanupSB->apply();
#else
   // Misc. cleanup
   F32 depthbias = 0.0f;
   GFX->setZBias(depthbias);

   GFX->enableColorWrites(true,true,true,true);
   GFX->setAlphaBlendEnable( false );
   GFX->setAlphaTestEnable( false );
   GFX->setZWriteEnable( true );
#endif

}

bool ProcessedShaderMaterial::setupPass(SceneGraphData& sgData, U32 pass)
{
   // Make sure we have the pass
   if(pass >= mPasses.size())
      return false;

   PROFILE_SCOPE(ProcessedShaderMaterial_setupPass);

#ifdef STATEBLOCK
   // Deal with mulitpass blending
   if( pass > 0 )
   {
		AssertFatal(mAlphaTrueSB, "ProcessedShaderMaterial::setupPass -- mAlphaTrueSB cannot be NULL.");
		mAlphaTrueSB->apply();
      setBlendState( mPasses[pass].blendOp );
   }
   else
   {
		AssertFatal(mAlphaFalseSB, "ProcessedShaderMaterial::setupPass -- mAlphaFalseSB cannot be NULL.");
		mAlphaFalseSB->apply();
   }
#else
   // Deal with mulitpass blending
   if( pass > 0 )
   {
      GFX->setAlphaBlendEnable( true );
      setBlendState( mPasses[pass].blendOp );
   }
   else
   {
      GFX->setAlphaBlendEnable( false );
   }
#endif


   if(! (sgData.GameRenderStatus&Material::DisableColor))
   {
	   // Deal with translucency
		if( mMaterial->translucent || sgData.visibility < 1.0f || (sgData.GameRenderStatus&Material::ForceTranslate) )
		{
			if(sgData.GameRenderStatus&Material::ForceTranslate )	//Ray: 阻挡透明情况下，强制处理alpha
			{
#ifdef STATEBLOCK
				AssertFatal(mForceTranslateSB, "ProcessedShaderMaterial::setupPass -- mForceTranslateSB cannot be NULL.");
				mForceTranslateSB->apply();
				setBlendState( Material::LerpAlpha ); 
#else
				F32 depthbias = -0.00005f;
				GFX->setZBias(depthbias);

				GFX->setAlphaBlendEnable( true );
				setBlendState( Material::LerpAlpha );   
				GFX->setZWriteEnable( true );
#endif

			}
			else
			{

#ifdef STATEBLOCK
				if (mMaterial->translucentBlendOp != Material::None )
				{
					AssertFatal(mAlphaTrueSB, "ProcessedShaderMaterial::setupPass -- mAlphaTrueSB cannot be NULL.");
					mAlphaTrueSB->apply();
				} 
				else
				{
					AssertFatal(mAlphaFalseSB, "ProcessedShaderMaterial::setupPass -- mAlphaFalseSB cannot be NULL.");
					mAlphaFalseSB->apply();
				}

				setBlendState( mMaterial->translucentBlendOp ); 

				if (mMaterial->translucentZWrite)
				{
					AssertFatal(mZWriteTrueSB, "ProcessedShaderMaterial::setupPass -- mZWriteTrueSB cannot be NULL.");
					mZWriteTrueSB->apply();			 
				} 
				else
				{
					AssertFatal(mZWriteFalseSB, "ProcessedShaderMaterial::setupPass -- mZWriteFalseSB cannot be NULL.");
					mZWriteFalseSB->apply();
				}
#else
				GFX->setAlphaBlendEnable( mMaterial->translucentBlendOp != Material::None );
				setBlendState( mMaterial->translucentBlendOp );  
				GFX->setZWriteEnable( mMaterial->translucentZWrite );
#endif

			}

#ifdef STATEBLOCK
			if (mMaterial->alphaTest)
			{
				switch (mMaterial->alphaRef)
				{
				case 0:
					AssertFatal(mAlphaRef0SB, "ProcessedShaderMaterial::setupPass -- mAlphaRef0SB cannot be NULL.");
					mAlphaRef0SB->apply();
					break;
				case 1:
					AssertFatal(mAlphaRef1SB, "ProcessedShaderMaterial::setupPass -- mAlphaRef1SB cannot be NULL.");
					mAlphaRef1SB->apply();
					break;
				case 20:
					AssertFatal(mAlphaRef20SB, "ProcessedShaderMaterial::setupPass -- mAlphaRef20SB cannot be NULL.");
					mAlphaRef20SB->apply();
					break;
				default:
					Con::errorf("ProcessedShaderMaterial::setupPass -- 没有参考值(%d)相对应的状态块。", mMaterial->alphaRef);
				}
			} 
			else
			{
				AssertFatal(mAlphaTestFalseSB, "ProcessedShaderMaterial::setupPass -- mAlphaTestFalseSB cannot be NULL.");
				mAlphaTestFalseSB->apply();
			}

			if (!mMaterial->translucentZEnable)
			{
				AssertFatal(mZEnableFalseSB, "ProcessedShaderMaterial::setupPass -- mZEnableFalseSB cannot be NULL.");
				mZEnableFalseSB->apply();
			} 
			else
			{
				AssertFatal(mZEnableSB, "ProcessedShaderMaterial::setupPass -- mZEnableSB cannot be NULL.");
				mZEnableSB->apply();
			}
#else
			GFX->setAlphaTestEnable( mMaterial->alphaTest );
			GFX->setAlphaRef( mMaterial->alphaRef );
			GFX->setAlphaFunc( GFXCmpGreaterEqual );

			// set up register combiners
			GFX->setTextureStageAlphaOp( 0, GFXTOPModulate );
			GFX->setTextureStageAlphaOp( 1, GFXTOPDisable );
			GFX->setTextureStageAlphaArg1( 0, GFXTATexture );
			GFX->setTextureStageAlphaArg2( 0, GFXTADiffuse );
			if (!mMaterial->translucentZEnable)
				GFX->setZEnable(false);
			else
				GFX->setZEnable(true);
#endif

		}
		else
		{
			GFX->setZEnable(true);
		}
   }
   else
   {

#ifdef STATEBLOCK
		AssertFatal(mDisableColorSB, "ProcessedShaderMaterial::setupPass -- mDisableColorSB cannot be NULL.");
		mDisableColorSB->apply();
#else
		GFX->setZWriteEnable( true );
		GFX->setZEnable(true);
		GFX->enableColorWrites(false,false,false,false);
		GFX->setAlphaBlendEnable( false );
		setBlendState( Material::None );
		//GFX->disableShaders();
		//GFX->setTextureStageColorOp( mPasses[pass].numTex, GFXTOPDisable );
#endif

   }

   static ShaderData *mMBShader = NULL;
   if(!mMBShader)
   {
	   if ( !Sim::findObject( "maxBlend", mMBShader ) )
		   Con::warnf("maxBlend - failed to locate maxBlend shader maxBlendData!");
   }

   //set shaders
   if(sgData.enableBlendColor && mMBShader)
   {
#ifdef STATEBLOCK
		AssertFatal(mEnableBlendColorSB, "ProcessedShaderMaterial::setupPass -- mEnableBlendColorSB cannot be NULL.");
		mEnableBlendColorSB->apply();
		GFX->setShader(mMBShader->getShader());
		setShaderConstants( sgData, pass); 
		GFX->setPixelShaderConstF( PC_USERDEF1,(float*)&sgData.maxBlendColor, 1 );
#else
		GFX->setZWriteEnable( false );
		GFX->setShader(mMBShader->getShader());
		setShaderConstants( sgData, pass); 
		GFX->setPixelShaderConstF( PC_USERDEF1,(float*)&sgData.maxBlendColor, 1 );
		GFX->setSrcBlend( GFXBlendDestColor );
		GFX->setDestBlend( GFXBlendZero );
#endif

   }
   else if( mPasses[pass].shader)
   {
	   mPasses[pass].shader->process();
	   setShaderConstants( sgData, pass); 
   }
   else
   {
	   GFX->disableShaders();

#ifdef STATEBLOCK
		//此处由于mPasses[pass].numTex是个变量，直接调用setTextureStageState，不采用stateblock
		GFX->setTextureStageState(mPasses[pass].numTex, GFXTSSColorOp, GFXTOPDisable);
#else
	   GFX->setTextureStageColorOp( mPasses[pass].numTex, GFXTOPDisable );
#endif
   } 

   // Set our textures
   if(! (sgData.GameRenderStatus&Material::DisableColor))
      setTextureStages( sgData, pass );

   if( pass == 0 )
   {
      // Only do this for the first pass
      setTextureTransforms();

      if( mMaterial->doubleSided )
      {
#ifdef STATEBLOCK
			AssertFatal(mCullNoneSB, "ProcessedShaderMaterial::setupPass -- mCullNoneSB cannot be NULL.");
			mCullNoneSB->apply();
			AssertFatal(mCullStoreSB, "ProcessedShaderMaterial::setupPass -- mCullStoreSB cannot be NULL.");
			mCullStoreSB->capture();
			mCullMode = 1;//使该值不为-1
#else
			mCullMode = GFX->getCullMode();
         GFX->setCullMode( GFXCullNone );
#endif
      }
   }

   return true;
}

void ProcessedShaderMaterial::setTextureStages( SceneGraphData &sgData, U32 pass )
{
   // Set all of the textures we need to render the give pass.
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
                 break;

              case Material::NormLightmap:
                 GFX->setTexture( i, sgData.normLightmap );
                 break;

              case Material::Cube:
                 GFX->setCubeTexture( i, mPasses[pass].cubeMap );
                 break;

              case Material::SGCube:
                 GFX->setCubeTexture( i, sgData.cubemap );
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

void ProcessedShaderMaterial::setTextureTransforms()
{
   U32 i=0;

   MatrixF texMat( true );

   if ( mMaterial->animFlags[i] )
   {
       mMaterial->updateTimeBasedParams();
       F32 waveOffset = getWaveOffset( i ); // offset is between 0.0 and 1.0

       // handle scroll anim type
       if(  mMaterial->animFlags[i] & Material::Scroll )
       {
          if( mMaterial->animFlags[i] & Material::Wave )
          {
             Point3F scrollOffset;
             scrollOffset.x = mMaterial->scrollDir[i].x * waveOffset;
             scrollOffset.y = mMaterial->scrollDir[i].y * waveOffset;
             scrollOffset.z = 1.0;

             texMat.setColumn( 3, scrollOffset );
          }
          else
          {
             Point3F offset( mMaterial->scrollOffset[i].x, 
                mMaterial->scrollOffset[i].y, 
                1.0 );

             texMat.setColumn( 3, offset );
          }

       }

       // handle rotation
       if( mMaterial->animFlags[i] & Material::Rotate )
       {
          if( mMaterial->animFlags[i] & Material::Wave )
          {
             F32 rotPos = waveOffset * M_2PI;
             texMat.set( EulerF( 0.0, 0.0, rotPos ) );
             texMat.setColumn( 3, Point3F( 0.5, 0.5, 0.0 ) );

             MatrixF test( true );
             test.setColumn( 3, Point3F( mMaterial->rotPivotOffset[i].x, 
                mMaterial->rotPivotOffset[i].y,
                0.0 ) );
             texMat.mul( test );
          }
          else
          {
             texMat.set( EulerF( 0.0, 0.0, mMaterial->rotPos[i] ) );

             texMat.setColumn( 3, Point3F( 0.5, 0.5, 0.0 ) );

             MatrixF test( true );
             test.setColumn( 3, Point3F( mMaterial->rotPivotOffset[i].x, 
                mMaterial->rotPivotOffset[i].y,
                0.0 ) );
             texMat.mul( test );
          }
       }

       // Handle scale + wave offset
       if(  mMaterial->animFlags[i] & Material::Scale &&
          mMaterial->animFlags[i] & Material::Wave )
       {
          F32 wOffset = fabs( waveOffset );

          texMat.setColumn( 3, Point3F( 0.5, 0.5, 0.0 ) );

          MatrixF temp( true );
          temp.setRow( 0, Point3F( wOffset,  0.0,  0.0 ) );
          temp.setRow( 1, Point3F( 0.0,  wOffset,  0.0 ) );
          temp.setRow( 2, Point3F( 0.0,  0.0,  wOffset ) );
          temp.setColumn( 3, Point3F( -wOffset * 0.5, -wOffset * 0.5, 0.0 ) );
          texMat.mul( temp );
       }

       // handle sequence
       if( mMaterial->animFlags[i] & Material::Sequence )
       {
          U32 frameNum = (U32)(mMaterial->mAccumTime * mMaterial->seqFramePerSec[i]);
          F32 offset = frameNum * mMaterial->seqSegSize[i];

          Point3F texOffset = texMat.getPosition();
          texOffset.x += offset;
          texMat.setPosition( texOffset );
       }
   }

   texMat.transpose();
   GFX->setVertexShaderConstF( VC_TEX_TRANS1, (float*) &texMat, 4 );

}

//--------------------------------------------------------------------------
// Get wave offset for texture animations using a wave transform
//--------------------------------------------------------------------------
F32 ProcessedShaderMaterial::getWaveOffset( U32 stage )
{
   switch( mMaterial->waveType[stage] )
   {
   case Material::Sin:
      {
         return mMaterial->waveAmp[stage] * mSin( M_2PI * mMaterial->wavePos[stage] );
         break;
      }

   case Material::Triangle:
      {
         F32 frac = mMaterial->wavePos[stage] - mFloor( mMaterial->wavePos[stage] );
         if( frac > 0.0 && frac <= 0.25 )
         {
            return mMaterial->waveAmp[stage] * frac * 4.0;
         }

         if( frac > 0.25 && frac <= 0.5 )
         {
            return mMaterial->waveAmp[stage] * ( 1.0 - ((frac-0.25)*4.0) );
         }

         if( frac > 0.5 && frac <= 0.75 )
         {
            return mMaterial->waveAmp[stage] * (frac-0.5) * -4.0;
         }

         if( frac > 0.75 && frac <= 1.0 )
         {
            return -mMaterial->waveAmp[stage] * ( 1.0 - ((frac-0.75)*4.0) );
         }

         break;
      }

   case Material::Square:
      {
         F32 frac = mMaterial->wavePos[stage] - mFloor( mMaterial->wavePos[stage] );
         if( frac > 0.0 && frac <= 0.5 )
         {
            return 0.0;
         }
         else
         {
            return mMaterial->waveAmp[stage];
         }
         break;
      }

   }

   return 0.0;
}

void ProcessedShaderMaterial::setShaderConstants(const SceneGraphData &sgData, U32 pass)
{
	// fog
	//-------------------------
	Point4F fogData;
	fogData.x = sgData.fogHeightOffset;
	fogData.y = sgData.fogInvHeightRange;
	fogData.z = sgData.visDist;
    fogData.w = sgData.fogFactor;
	GFX->setVertexShaderConstF( VC_FOGDATA, (float*)&fogData, 1 );

	// Visibility
	F32 visibility = sgData.visibility;
	GFX->setPixelShaderConstF( PC_VISIBILITY, &visibility, 1, 1 );

	// set eye positions
	//-------------------------
	Point3F eyePos   = sgData.camPos;
	MatrixF objTrans = sgData.objTrans;

	setObjectXForm(objTrans, pass);
	setEyePosition(objTrans, eyePos, pass);
	setLightInfo(sgData, pass);
	
	U32 stageNum = getStageFromPass(pass);

	// this is OK for now, will need to change later to support different
    // specular values per pass in custom materials
	GFX->setVertexShaderConstF( VC_MAT_SPECPOWER, (float*)&mMaterial->specularPower[stageNum], 1, 1 );

	//Ray: 阴影中减少高光效应
	ColorF spcol = mMaterial->specular[stageNum] * sgData.light->mColor * sgData.light->mColor;
	//-------------------------
	GFX->setPixelShaderConstF( PC_MAT_SPECCOLOR, (float*)&spcol, 1 );
	GFX->setPixelShaderConstF( PC_MAT_SPECPOWER, (float*)&mMaterial->specularPower[stageNum], 1, 1 );

    // Color multiply
    if (mMaterial->colorMultiply[stageNum].alpha > 0.0f)
    {
       GFX->setPixelShaderConstF(PC_COLORMULTIPLY, (float*)&mMaterial->colorMultiply[stageNum], 1 );
    }

    // Color exposure
    F32 colorExposure = mMaterial->getColorExposure() * sgData.overrideColorExposure;
    float reg[4];
    reg[0] = colorExposure;
    GFX->setPixelShaderConstF(PC_COLOREXPOSURE, (float*)reg, 1, 1 );
}

bool ProcessedShaderMaterial::hasCubemap(U32 pass)
{
   // Only support cubemap on the first stage
   if( mPasses[pass].stageNum > 0 )
   {
      return false;
   }

   if( mPasses[pass].cubeMap )
   {
      return true;
   }
   return false;
}

void ProcessedShaderMaterial::setObjectXForm(MatrixF xform, U32 pass)
{
   // Set cubemap stuff here (it's convenient!)
   if( hasCubemap(pass) || mMaterial->dynamicCubemap)
   {
      MatrixF cubeTrans = xform;
      cubeTrans.setPosition( Point3F( 0.0, 0.0, 0.0 ) );
      cubeTrans.transpose();
      GFX->setVertexShaderConstF( VC_CUBE_TRANS, (float*)&cubeTrans, 3 );
   }
   xform.transpose();
   GFX->setVertexShaderConstF( VC_OBJ_TRANS, (float*)&xform, 4 );
}

void ProcessedShaderMaterial::setWorldXForm(MatrixF xform)
{
   GFX->setVertexShaderConstF( 0, (float*)&xform, 4 );
}

void ProcessedShaderMaterial::setLightingBlendFunc()
{
   gClientSceneGraph->getLightManager()->setLightingBlendFunc();
}

void ProcessedShaderMaterial::setLightInfo(const SceneGraphData& sgData, U32 pass)
{
   gClientSceneGraph->getLightManager()->setLightInfo(this, mMaterial, sgData, pass);
}

void ProcessedShaderMaterial::setEyePosition(MatrixF objTrans, Point3F position, U32 pass)
{
   // Set cubemap stuff here (it's convenient!)
   if(hasCubemap(pass) || mMaterial->dynamicCubemap)
   {
      Point3F cubeEyePos = position - objTrans.getPosition();
      GFX->setVertexShaderConstF( VC_CUBE_EYE_POS, (float*)&cubeEyePos, 1, 3 );
   }
   objTrans.inverse();
   objTrans.mulP( position );
   position.convolveInverse(objTrans.getScale());
   GFX->setVertexShaderConstF( VC_EYE_POS, (float*)&position, 1, 3 );
}

void ProcessedShaderMaterial::init(SceneGraphData& sgData, GFXVertexFlags vertFlags)
{
   // Load our textures
   setStageData();

   // Determine how many stages we use
   mMaxStages = getNumStages(); 
   mVertFlags = vertFlags;
   for( U32 i=0; i<mMaxStages; i++ )
   {
      GFXShaderFeatureData fd;
      // Determine the features of this stage
      determineFeatures( i, fd, sgData );

      if( fd.codify() )
      {
         // Create the passes for this stage
         createPasses( fd, i, sgData );
      }
   }
}

void ProcessedShaderMaterial::setStageData()
{
   // Only do this once
   if( mHasSetStageData ) return;
   mHasSetStageData = true;

   U32 i;

   // Load up all the textures for every possible stage
   for( i=0; i<Material::MAX_STAGES; i++ )
   {
      if( mMaterial->baseTexFilename[i] && mMaterial->baseTexFilename[i][0] )
      {
         mStages[i].tex[ProcessedMaterial::BaseTex] = createTexture( mMaterial->baseTexFilename[i], &GFXDefaultStaticDiffuseProfile );
      }

      if( mMaterial->detailFilename[i] && mMaterial->detailFilename[i][0] )
      {
         mStages[i].tex[ProcessedMaterial::DetailMap] = createTexture( mMaterial->detailFilename[i], &GFXDefaultStaticDiffuseProfile );
      }

	  if( mMaterial->SpecularFilename[i] && mMaterial->SpecularFilename[i][0] )
	  {
		  mStages[i].tex[ProcessedMaterial::SpecularMap] = createTexture( mMaterial->SpecularFilename[i], &GFXDefaultStaticDiffuseProfile );
	  }

      if( mMaterial->bumpFilename[i] && mMaterial->bumpFilename[i][0] )
      {
         mStages[i].tex[ProcessedMaterial::BumpMap] = createTexture( mMaterial->bumpFilename[i], &GFXDefaultStaticDiffuseProfile );
      }

      if( mMaterial->envFilename[i] && mMaterial->envFilename[i][0] )
      {
         mStages[i].tex[ProcessedMaterial::EnvMap] = createTexture( mMaterial->envFilename[i], &GFXDefaultStaticDiffuseProfile );
      }
   }

   // If we have a cubemap put it on stage 0 (cubemaps only supported on stage 0)
   if( mMaterial->mCubemapData )
   {
      mMaterial->mCubemapData->createMap();
      mStages[0].cubemap = mMaterial->mCubemapData->cubemap;
   }
}

void ProcessedShaderMaterial::addPass( RenderPassData &rpd, 
                          U32 &texIndex, 
                          GFXShaderFeatureData &fd,
                          U32 stageNum,
                          SceneGraphData& sgData )
{
   // Set number of textures, stage, glow, etc.
   rpd.numTex = texIndex;
   rpd.fData.useLightDir = sgData.useLightDir;
   rpd.stageNum = stageNum;
   rpd.glow |= mMaterial->glow[stageNum];

   // Copy over features
   dMemcpy( rpd.fData.materialFeatures, fd.features, sizeof( fd.features ) );

   // Generate shader
   rpd.shader = GFX->createShader( rpd.fData, mVertFlags );

   // If a pass glows, we glow
   if( rpd.glow )
   {
      mHasGlow = true;
   }

   mPasses.push_back( rpd );

   rpd.reset();
   texIndex = 0;
}

void ProcessedShaderMaterial::setPassBlendOp( ShaderFeature *sf,
                                 RenderPassData &passData,
                                 U32 &texIndex,
                                 GFXShaderFeatureData &stageFeatures,
                                 U32 stageNum,
                                 SceneGraphData& sgData )
{
   if( sf->getBlendOp() == Material::None )
   {
      return;
   }

   // set up the current blend operation for multi-pass materials
   if( mPasses.size() > 0 )
   {
      // If passData.numTexReg is 0, this is a brand new pass, so set the
      // blend operation to the first feature.
      if( passData.numTexReg == 0 )
      {
         passData.blendOp = sf->getBlendOp();
      }
      else
      {
         // numTegReg is more than zero, if this feature
         // doesn't have the same blend operation, then
         // we need to create yet another pass
         if( sf->getBlendOp() != passData.blendOp )
         {
            addPass( passData, texIndex, stageFeatures, stageNum, sgData );
            passData.blendOp = sf->getBlendOp();
         }
      }
   }
} 

void ProcessedShaderMaterial::initsb()
{
	if (mAlphaTrueSB == NULL)
	{
		mAlphaTrueSB = new GFXD3D9StateBlock;
		mAlphaTrueSB->registerResourceWithDevice(GFX);
		mAlphaTrueSB->mResurrect = &resetStateBlock;
		mAlphaTrueSB->mZombify = &releaseStateBlock;

		mAlphaFalseSB = new GFXD3D9StateBlock;
		mForceTranslateSB = new GFXD3D9StateBlock;
		mZWriteTrueSB = new GFXD3D9StateBlock;
		mZWriteFalseSB = new GFXD3D9StateBlock;
		mAlphaRef0SB = new GFXD3D9StateBlock;
		mAlphaRef1SB = new GFXD3D9StateBlock;
		mAlphaRef20SB = new GFXD3D9StateBlock;
		mAlphaTestFalseSB = new GFXD3D9StateBlock;
		mZEnableSB = new GFXD3D9StateBlock;
		mZEnableFalseSB = new GFXD3D9StateBlock;
		mDisableColorSB = new GFXD3D9StateBlock;
		mEnableBlendColorSB = new GFXD3D9StateBlock;
		mCullNoneSB = new GFXD3D9StateBlock;
		mCullStoreSB = new GFXD3D9StateBlock;
		mCleanupSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void ProcessedShaderMaterial::shutdown()
{
	SAFE_DELETE(mAlphaTrueSB);
	SAFE_DELETE(mAlphaFalseSB);
	SAFE_DELETE(mForceTranslateSB);
	SAFE_DELETE(mZWriteTrueSB);
	SAFE_DELETE(mZWriteFalseSB);
	SAFE_DELETE(mAlphaRef0SB);
	SAFE_DELETE(mAlphaRef1SB);
	SAFE_DELETE(mAlphaRef20SB);
	SAFE_DELETE(mAlphaTestFalseSB);
	SAFE_DELETE(mZEnableFalseSB);
	SAFE_DELETE(mZEnableSB);
	SAFE_DELETE(mDisableColorSB);
	SAFE_DELETE(mEnableBlendColorSB);
	SAFE_DELETE(mCullNoneSB);
	SAFE_DELETE(mCullStoreSB);
	SAFE_DELETE(mCleanupSB);
}

void ProcessedShaderMaterial::resetStateBlock()
{
	//mAlphaTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mAlphaTrueSB);

	//mAlphaFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mAlphaFalseSB);

	//mForceTranslateSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSDepthBias, -0.00005f);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->endStateBlock(mForceTranslateSB);

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

	//mZEnableSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mZEnableSB);

	//mZEnableFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->endStateBlock(mZEnableFalseSB);

	//mDisableColorSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSColorWriteEnable, 0);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mDisableColorSB);

	//mEnableBlendColorSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->endStateBlock(mEnableBlendColorSB);

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
	GFX->setRenderState(GFXRSDepthBias, 0);
	GFX->setRenderState(GFXRSColorWriteEnable, GFXCOLORWRITEENABLE_RED|GFXCOLORWRITEENABLE_GREEN|GFXCOLORWRITEENABLE_BLUE|GFXCOLORWRITEENABLE_ALPHA);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->endStateBlock(mCleanupSB);

}

void ProcessedShaderMaterial::releaseStateBlock()
{
	if (mAlphaTrueSB)
	{
		mAlphaTrueSB->release();
	}

	if (mAlphaFalseSB)
	{
		mAlphaFalseSB->release();
	}

	if (mForceTranslateSB)
	{
		mForceTranslateSB->release();
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

	if (mZEnableSB)
	{
		mZEnableSB->release();
	}

	if (mZEnableFalseSB)
	{
		mZEnableFalseSB->release();
	}

	if (mDisableColorSB)
	{
		mDisableColorSB->release();
	}

	if (mEnableBlendColorSB)
	{
		mEnableBlendColorSB->release();
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



