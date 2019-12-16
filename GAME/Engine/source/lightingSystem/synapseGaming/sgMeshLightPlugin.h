//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGMESHLIGHTPLUGIN_H_
#define _SGMESHLIGHTPLUGIN_H_

#ifndef _TSMESH_H_
#include "ts/tsMesh.h"
#endif

#ifndef _SGLIGHTINFO_H_
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#endif

class sgMeshLightPlugin : public TSMeshLightPlugin
{
private:
   LightInfoList baselights;
   LightInfoDualList duallights;
public:   
   virtual void processRI(TSMesh* mesh, Vector<RenderInst*>& list);
   virtual U32 prepareLight();
};

extern sgMeshLightPlugin g_sgMeshLightPlugin;

#endif