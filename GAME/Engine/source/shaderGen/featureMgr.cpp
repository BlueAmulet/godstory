//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "featureMgr.h"
#include "bump.h"
#include "pixSpecular.h"
#include "langElement.h"

FeatureMgr gFeatureMgr;


//****************************************************************************
// Shader Manager
//****************************************************************************
FeatureMgr::FeatureMgr()
{
   init();
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
FeatureMgr::~FeatureMgr()
{
   shutdown();
}

//----------------------------------------------------------------------------
// Shutdown
//----------------------------------------------------------------------------
void FeatureMgr::shutdown()
{
   for( U32 i=0; i<mFeatures.size(); i++ )
   {
      if( mFeatures[i] )
      {
         delete mFeatures[i];
         mFeatures[i] = NULL;
      }
   }
	
   for( U32 i=0; i<mAuxFeatures.size(); i++ )
   {
	   if( mAuxFeatures[i] )
	   {
		   delete mAuxFeatures[i];
		   mAuxFeatures[i] = NULL;
	   }
   }

   LangElement::deleteElements();
}

//----------------------------------------------------------------------------
// Init
//----------------------------------------------------------------------------
void FeatureMgr::init()
{
   mFeatures.setSize( GFXShaderFeatureData::NumFeatures );

   mFeatures[ GFXShaderFeatureData::RTLighting ]					= new VertLightColor;
   mFeatures[ GFXShaderFeatureData::VertexColor]					= new VertexColor;		//Ray: vertx color
   mFeatures[ GFXShaderFeatureData::TexAnim ]						= new TexAnim;
   mFeatures[ GFXShaderFeatureData::BaseTex ]						= new BaseTexFeat;
   mFeatures[ GFXShaderFeatureData::ColorMultiply ]					= new ColorMultiplyFeat;
   mFeatures[ GFXShaderFeatureData::DynamicLight ]					= NULL; 
   mFeatures[ GFXShaderFeatureData::DynamicLightDual ]				= NULL; 
   mFeatures[ GFXShaderFeatureData::DynamicLightMask ]				= NULL; 
   mFeatures[ GFXShaderFeatureData::DynamicLightAttenuateBackFace ] = NULL; 
   mFeatures[ GFXShaderFeatureData::SelfIllumination ]				= NULL; 
   mFeatures[ GFXShaderFeatureData::SpecularMap ]					= new SpecularMapFeat;	//Ray: specular map
   mFeatures[ GFXShaderFeatureData::LightMap ]						= new LightmapFeat;
   mFeatures[ GFXShaderFeatureData::LightNormMap ]					= new LightNormMapFeat;
   mFeatures[ GFXShaderFeatureData::BumpMap ]						= new BumpFeat;
   mFeatures[ GFXShaderFeatureData::DetailMap ]						= new DetailFeat;
   mFeatures[ GFXShaderFeatureData::ExposureX2 ]					= NULL; 
   mFeatures[ GFXShaderFeatureData::ExposureX4 ]					= NULL; 
   mFeatures[ GFXShaderFeatureData::EnvMap ]						= NULL;
   mFeatures[ GFXShaderFeatureData::CubeMap ]						= new ReflectCubeFeat;
   mFeatures[ GFXShaderFeatureData::PixSpecular ]					= new PixelSpecular;
   mFeatures[ GFXShaderFeatureData::VertSpecular ]					= NULL;
   mFeatures[ GFXShaderFeatureData::Translucent ]					= new ShaderFeature;
   mFeatures[ GFXShaderFeatureData::Visibility ]					= new VisibilityFeat;
   mFeatures[ GFXShaderFeatureData::EffectLight ]					= NULL;
   mFeatures[ GFXShaderFeatureData::ColorExposure ]				    = new ColorExposureFeat;
   mFeatures[ GFXShaderFeatureData::Fog ]							= new FogFeat;
   
   // auxiliary features
   mAuxFeatures.push_back( new VertPosition );
}

//----------------------------------------------------------------------------
// Get feature
//----------------------------------------------------------------------------
ShaderFeature * FeatureMgr::get( U32 index )
{
   if( index >= mFeatures.size() )
   {
      AssertFatal( false, "Feature request out of range." );
      return NULL;
   }
   return mFeatures[index];
}

//----------------------------------------------------------------------------
// getAux - get auxiliary feature
//----------------------------------------------------------------------------
ShaderFeature * FeatureMgr::getAux( U32 index )
{
   if( index >= mAuxFeatures.size() )
   {
      return NULL;
   }
   return mAuxFeatures[index];
}

// Allows other systems to add features.  index is the enum in GFXShaderFeatureData
void FeatureMgr::registerFeature( const U32 index, ShaderFeature* newFeature )
{
   if( index >= mFeatures.size() )
   {
      AssertFatal( false, "FeatureMgr::registerFeature, index out of range." );
      return;
   }
   SAFE_DELETE(mFeatures[index]);
   mFeatures[index] = newFeature;
}

// Unregister a feature.
void FeatureMgr::unregisterFeature(const U32 index)
{
   registerFeature(index, NULL);
}





