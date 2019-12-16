//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _MATERIALS_PROCESSEDDYNAMICLIGHTMATERIAL_H_
#define _MATERIALS_PROCESSEDDYNAMICLIGHTMATERIAL_H_

#include "materials/processedShaderMaterial.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"

// CodeReview - I really don't like having a separate ProcessedMaterial subclass just for lighting
// but it's the easiest way to get dynamic lighting up and running without changing any rendering
// or worse, the lighting subsystem. - AlexS 4/05/07

/// This processed material class is used for dynamic lighting.
/// As such it contains only minimal modifications to the base
/// ProcessedShaderMaterial class.  Basically just
/// sets the proper dynamic lighting features in addition to
/// everything else we need.
class ProcessedDynamicLightMaterial : public ProcessedShaderMaterial
{
   typedef ProcessedShaderMaterial Parent;
   // CodeReview - Handling dual lighting/masking here is clearly a hack.  Can we do
   // this in a better way? - AlexS 4/05/07
   friend class MatInstance;
   bool mIsDualLight;
   bool mHasMask;
   sgLightInfo::sgFeatures mDynamicLightingFeatures;

   /// Fills in the GFXShaderFeatureData for the given stage
   void determineFeatures( U32 stageNum, GFXShaderFeatureData &fd, SceneGraphData& sgData );
public:
   void setDualLight(bool dualLight);
   void setMask(bool mask);
   void setDynamicLightingFeatures(sgLightInfo::sgFeatures feat);

   ProcessedDynamicLightMaterial(Material& mat);

   bool isDualLight()
   {
      return mIsDualLight;
   }

   bool isMask()
   {
      return mHasMask;
   }
};

#endif
