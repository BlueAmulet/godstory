//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGLIGHTINFO_H_
#define _SGLIGHTINFO_H_

#ifndef _LIGHTINFO_H_
#include "sceneGraph/lightInfo.h"
#endif

#ifndef _MPLANE_H_
#include "math/mPlane.h"
#endif

#ifndef _MMATRIX_H_
#include "math/mMatrix.h"
#endif

class GFXCubemap;

// Synapse Gaming Lighting Kit Class
class sgLightInfo : public LightInfo
{
public:
   enum sgFeatures
   {
      // in order from most features to least...
      sgFull = 0,
      sgNoCube,
      sgNoSpecCube,
      sgFeatureCount
   };
   sgFeatures sgSupportedFeatures;
   bool sgAssignedToTSObject;
   bool sgCastsShadows;
   bool sgDiffuseRestrictZone;
   bool sgAmbientRestrictZone;
   S32 sgZone[2];
   F32 sgLocalAmbientAmount;
   bool sgSmoothSpotLight;
   bool sgDoubleSidedAmbient;
   bool sgAssignedToParticleSystem;
   StringTableEntry sgLightingModelName;
   bool sgUseNormals;
   Point3F sgTempModelInfo;
   MatrixF sgLightingTransform;
   PlaneF sgSpotPlane;
   GFXCubemap *sgLightMask;

   sgLightInfo();
   ~sgLightInfo();

   //
   // LightInfo methods
   //
   // Copies a light 
   virtual void set(LightInfo* light);
   // Sets a fixed function GFXLight with our properties 
   virtual void setGFXLight(GFXLightInfo* light);
   // Sets values in Scene Graph Data before rendering
   virtual void setSceneGraphData(SceneGraphData* sgd);
   // Gets the radius affected by this light
   virtual F32 getRadius();

   //
   // SGLightInfo methods
   //
   bool sgIsInZone(S32 zone);
   bool sgAllowDiffuseZoneLighting(S32 zone);   
   bool sgCanBeSecondary() {return sgSupportedFeatures >= sgNoSpecCube;}
   static bool sgAllowSpecular(sgFeatures features) {return features < sgNoSpecCube;}
   static bool sgAllowCubeMapping(sgFeatures features) {return features < sgNoCube;}
};

class LightInfoDual
{
public:
   sgLightInfo *sgLightPrimary;
   sgLightInfo *sgLightSecondary;

   LightInfoDual()
   {
      sgLightPrimary = NULL;
      sgLightSecondary = NULL;
   }
};

class LightInfoDualList : public Vector<LightInfoDual>
{
public:
};

#define SG_CHECK_LIGHT(x) AssertFatal(dynamic_cast<sgLightInfo*>(x), "Incorrect light type!")
#endif