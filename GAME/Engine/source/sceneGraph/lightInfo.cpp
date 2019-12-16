//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "math/mMath.h"
#include "core/color.h"
#include "gfx/gfxCubemap.h"
#include "sceneGraph/lightInfo.h"

LightInfo::LightInfo()
{
   mType = Vector;
   mPos = Point3F(0, 0, 0);
   mDirection = Point3F(0, 0, 1);
   mColor = ColorF(0, 0, 0);
   mShadowColor = ColorF(0.0f, 0.0f, 0.0f);
	mDynamicShadowedColor = ColorF(0.0f, 0.0f, 0.0f);
   mAmbient = ColorF(0, 0, 0);
   mBackColor = ColorF(0.0f, 0.0f, 0.0f);
   mObjAmbient = ColorF(0, 0, 0);
   mSelfillum = ColorF(0, 0, 0);
   mIsSelfillum = false;
   mRadius = 1;
   mScore = 0;
   sgSpotAngle = 90.0;
   mReceiveLMLighting = true;
}

LightInfo::~LightInfo()
{
}

// Copies data passed in from light
void LightInfo::set(LightInfo* light)
{
   mType = light->mType;
   mPos = light->mPos;
   mDirection = light->mDirection;
   mColor = light->mColor;
   mShadowColor = light->mShadowColor;
	mDynamicShadowedColor = light->mDynamicShadowedColor;
   mBackColor = light->mBackColor;
   mObjAmbient= light->mObjAmbient;
   mAmbient = light->mAmbient;
   mSelfillum = light->mSelfillum;
   mIsSelfillum = light->mIsSelfillum;
   mRadius = light->mRadius;
   mScore = light->mScore;
   sgSpotAngle = light->sgSpotAngle;
   mReceiveLMLighting = light->mReceiveLMLighting;
}

void LightInfo::setGFXLight(GFXLightInfo* light)
{
   switch (mType) {
      case LightInfo::Point :
         light->mType = GFXLightInfo::Point;
         break;
      case LightInfo::Spot :
         light->mType = GFXLightInfo::Spot;
         break;
      case LightInfo::Vector:
         light->mType = GFXLightInfo::Vector;
         break;
      case LightInfo::Ambient:
         light->mType = GFXLightInfo::Ambient;
         break;
   }
   light->mPos = mPos;
   light->mDirection = mDirection;
   light->mColor = mColor;
   light->mAmbient = mAmbient;
   light->mRadius = mRadius;
   light->sgSpotAngle = sgSpotAngle;
}

void LightInfoList::sgRegisterLight(LightInfo *light)
{
   if(!light)
      return;
   // just add the light, we'll try to scan for dupes later...
   push_back(light);
}

void LightInfoList::sgUnregisterLight(LightInfo *light)
{
   // remove all of them...
   LightInfoList &list = *this;
   for(U32 i=0; i<list.size(); i++)
   {
      if(list[i] != light)
         continue;
      // this moves last to i, which allows
      // the search to continue forward...
      list.erase_fast(i);
      // want to check this location again...
      i--;
   }
}
