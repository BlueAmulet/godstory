//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _LIGHTINFO_H_
#define _LIGHTINFO_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _GFXSTRUCTS_H_
#include "gfx/gfxStructs.h"
#endif

struct SceneGraphData;

// This is the base LightInfo* class that will be tracked by the engine.  Should basically contain a bounding volume and methods to interact
// with the rest of the system (for example, setting GFX fixed function lights).
class LightInfo 
{
public:
   enum Type {
      Point    = 0,
      Spot     = 1,
      Vector   = 2,
      Ambient  = 3,
      SGStaticPoint,
      SGStaticSpot
   };
   Type        mType;

   Point3F     mPos;
   VectorF     mDirection;
   ColorF      mColor;
   ColorF      mShadowColor;//光照计算中，仅阴影面使用的阳光颜色（地形）
	ColorF		mDynamicShadowedColor;//物体的动态影子使用的颜色（物体）
   ColorF      mAmbient;
   ColorF      mObjAmbient;
   ColorF      mBackColor; // 打上背光
   bool		   mIsSelfillum;
   ColorF      mSelfillum;
   F32         mRadius;
   F32         sgSpotAngle;
   //private:
   S32         mScore;

   bool        mReceiveLMLighting;
public:
   // Methods
   LightInfo();
   virtual ~LightInfo();

   // Copies data passed in from light
   virtual void set(LightInfo* light);

   // Sets a fixed function GFXLight with our properties 
   virtual void setGFXLight(GFXLightInfo* light);

   // Sets values in Scene Graph Data before rendering
   virtual void setSceneGraphData(SceneGraphData* sgd) = 0;

   // Gets the radius affected by this light
   virtual F32 getRadius() = 0;
};

class LightInfoList : public Vector<LightInfo*>
{
public:
   void sgRegisterLight(LightInfo *light);
   void sgUnregisterLight(LightInfo *light);
};

#endif
