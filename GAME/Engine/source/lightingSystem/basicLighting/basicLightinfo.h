//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _BASICLIGHTINFO_H_
#define _BASICLIGHTINFO_H_

#include "sceneGraph/lightInfo.h"

class BasicLightInfo : public LightInfo
{
public:
   // Sets values in Scene Graph Data before rendering
   virtual void setSceneGraphData(SceneGraphData* sgd);

   // Gets the radius affected by this light
   virtual F32 getRadius();
};

#endif
