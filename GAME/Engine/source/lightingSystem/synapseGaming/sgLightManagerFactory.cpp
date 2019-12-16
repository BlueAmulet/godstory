//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgLightManager.h"

class sgLightManagerFactory : public LightManagerFactory
{
public:
   virtual ~sgLightManagerFactory();
   virtual LightManager* createLightManager();
   virtual void cleanupLightManager();
   virtual const char* getName() const;   
};

sgLightManagerFactory p_sgLightManagerFactory;

LightManager* sgLightManagerFactory::createLightManager()
{
   return sgLightManager::get();
}

sgLightManagerFactory::~sgLightManagerFactory()
{
   sgLightManager::cleanup();
}

const char* sgLightManagerFactory::getName() const
{
   return "SynapseGaming Lighting Kit";
}

void sgLightManagerFactory::cleanupLightManager()
{

}