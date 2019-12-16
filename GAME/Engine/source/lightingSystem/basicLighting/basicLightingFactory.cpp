//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "lightingSystem/basicLighting/basicLightManager.h"

class BasicLightingFactory : public LightManagerFactory
{
public:
   ~BasicLightingFactory();
   virtual LightManager* createLightManager();
   virtual void cleanupLightManager();
   virtual const char* getName() const;
};

BasicLightingFactory::~BasicLightingFactory()
{
   BasicLightManager::cleanup();
}

BasicLightingFactory p_BasicLightingFactory;

LightManager* BasicLightingFactory::createLightManager()
{
   return BasicLightManager::get();
}

void BasicLightingFactory::cleanupLightManager()
{
}

const char* BasicLightingFactory::getName() const
{
   return "Basic Lighting";
}