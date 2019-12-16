//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "sceneGraph/lightingInterfaces.h"

void AvailableSLInterfaces::registerSystem(SceneLightingInterface* si)
{   
   mAvailableSystemInterfaces.push_back(si);
}

void AvailableSLInterfaces::initInterfaces()
{
   mAvailableObjectTypes = mClippingMask = mZoneLightSkipMask = 0;
   for(SceneLightingInterface** sitr = mAvailableSystemInterfaces.begin(); sitr != mAvailableSystemInterfaces.end(); sitr++)
   {
      SceneLightingInterface* si = (*sitr);
      si->init();
      mAvailableObjectTypes |= si->addObjectType();
      mClippingMask |= si->addToClippingMask();
      mZoneLightSkipMask |= si->addToZoneLightSkipMask();
   }  
}