//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "lightingSystem/basicLighting/basicLightInfo.h"
#include "materials/sceneData.h"

// Sets values in Scene Graph Data before rendering
void BasicLightInfo::setSceneGraphData(SceneGraphData* sgd)
{
   sgd->light = this;
}

// Gets the radius affected by this light
F32 BasicLightInfo::getRadius()
{
   return 1.0f;
}
