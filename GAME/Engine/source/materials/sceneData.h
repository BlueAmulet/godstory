//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _SCENEDATA_H_
#define _SCENEDATA_H_

#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/lightManager.h"
#include "sceneGraph/lightInfo.h"

struct VertexData;
class GFXTexHandle;
class GFXCubemap;

//**************************************************************************
// Scene graph data - temp - simulates data scenegraph will provide
//
// CodeReview [btr, 7/31/2007] I'm not sure how temporary this struct is.  But 
// it keeps the material system separate from the SceneGraph and RenderInst 
// systems.  Which is kind of nice.  I think eventually the RenderInst should 
// get rid of the duplicate variables and just contain a SceneGraphData.
//**************************************************************************
struct SceneGraphData
{
   // textures
   GFXTextureObject * dynamicLight;
   GFXTextureObject * dynamicLightSecondary;
   GFXTextureObject * lightmap;
   GFXTextureObject * normLightmap;
   GFXTextureObject * fogTex;
   GFXTextureObject * blackfogTex;
   GFXTextureObject * backBuffTex;
   GFXTextureObject * reflectTex;
   GFXTextureObject * miscTex;
   GFXTextureObject * vertexTex;
   F32 boneNum;
   
   // lighting
	LightInfoList lights;
	Vector<GFXTextureObject*> lightTexs;

   LightInfo*  light;
   LightInfo*  lightSecondary;
   bool        useLightDir;   // use light dir instead of position - used for sunlight outdoors
   bool        matIsInited;   // Set to true in the RenderInstanceMgr's after the MatInstance->setupPass call.

   // fog   
   F32         fogHeightOffset;
   F32         fogInvHeightRange;
   F32         visDist;
   bool        useFog;

   F32         fogFactor;

   // misc
   Point3F        camPos;
   MatrixF        objTrans;
   VertexData *   vertData;
   GFXCubemap *   cubemap;
   bool           glowPass;
   bool           refractPass;
   F32            visibility;
   bool           enableBlendColor;
   ColorF         maxBlendColor;
   F32            overrideColorExposure;

   U32			  GameRenderStatus;
   //-----------------------------------------------------------------------
   // Constructor
   //-----------------------------------------------------------------------
   SceneGraphData()
      : lightmap(), normLightmap(), fogTex()
   {
      reset();
	  maxBlendColor = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	  enableBlendColor = false;
   }

   inline void reset()
   {
      dMemset( this, 0, sizeof( SceneGraphData ) );
      fogFactor = 1.0f;
      visibility = 1.0f;
      overrideColorExposure = 1.0f;
   }

   inline void setDefaultLights()
   {
      light = gClientSceneGraph->getLightManager()->getDefaultLight();
      lightSecondary = light;
   }

   inline void setFogParams()
   {
      // Fogging...
      useFog            = true;
      fogTex            = gClientSceneGraph->getFogTexture();
      blackfogTex       = gClientSceneGraph->getBlackFogTexture();
      fogHeightOffset   = gClientSceneGraph->getFogHeightOffset();
      visDist           = gClientSceneGraph->getVisibleDistanceMod();
      fogInvHeightRange = gClientSceneGraph->getFogInvHeightRange();
   }
};

#endif _SCENEDATA_H_
