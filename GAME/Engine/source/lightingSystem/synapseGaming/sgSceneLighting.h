//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGSCENELIGHTING_H_
#define _SGSCENELIGHTING_H_

#include "lightingSystem/common/sceneLighting.h"

class sgSceneLighting : public SceneLighting
{
private:
   typedef SceneLighting Parent;
protected:
   virtual void getMLName(const char* misName, const U32 missionCRC, const U32 buffSize, char* filenameBuffer);
public:
   sgSceneLighting(AvailableSLInterfaces* lightingInterfaces);
   void sgLightingStartEvent();
   void sgLightingCompleteEvent();
   void sgSGPassSetupEvent();
   void sgSGObjectCompleteEvent(S32 object);
};

#endif