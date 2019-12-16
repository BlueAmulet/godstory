//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/processedDynamicLightMaterial.h"
#include "gfx/cubemapData.h"

ProcessedDynamicLightMaterial::ProcessedDynamicLightMaterial(Material& mat) : ProcessedShaderMaterial(mat)
{
   mIsDualLight = false;
}

void ProcessedDynamicLightMaterial::setMask(bool mask)
{
   mHasMask = mask;
}

void ProcessedDynamicLightMaterial::setDualLight(bool dualLight)
{
   mIsDualLight = dualLight;
}

void ProcessedDynamicLightMaterial::setDynamicLightingFeatures(sgLightInfo::sgFeatures feat)
{
   mDynamicLightingFeatures = feat;
}

void ProcessedDynamicLightMaterial::determineFeatures(U32 stageNum, GFXShaderFeatureData &fd, SceneGraphData& sgData )
{
   Parent::determineFeatures(stageNum, fd, sgData);

   // Override some of the ProcessedShaderMaterial decisions
   fd.features[GFXShaderFeatureData::RTLighting] = false;
   fd.features[GFXShaderFeatureData::LightMap] = false;
   fd.features[GFXShaderFeatureData::LightNormMap] = false;

   // Dynamic light feature (last stage only)
   if (!mMaterial->emissive[stageNum] && stageNum == (mMaxStages-1))
   {
      fd.features[GFXShaderFeatureData::DynamicLight] = true;   

      if (GFX->getPixelShaderVersion() >= 2.0)
      {
         // dual dynamic lighting...
         fd.features[GFXShaderFeatureData::DynamicLightDual] = mIsDualLight;

         // dynamic lighting mask...
         fd.features[GFXShaderFeatureData::DynamicLightMask] = mHasMask;
      }

      // dynamic lighting back face attn...
      fd.features[GFXShaderFeatureData::DynamicLightAttenuateBackFace] = mMaterial->attenuateBackFace; 
   }

   // pixel specular
   if( GFX->getPixelShaderVersion() >= 2.0 )
   {
      if(fd.features[GFXShaderFeatureData::PixSpecular])
      {
         fd.features[GFXShaderFeatureData::PixSpecular] = sgLightInfo::sgAllowSpecular(mDynamicLightingFeatures);           
      }
   }

   // cubemap
   if(fd.features[GFXShaderFeatureData::CubeMap])
   {
      fd.features[GFXShaderFeatureData::CubeMap] = sgLightInfo::sgAllowCubeMapping(mDynamicLightingFeatures);
   }
}
