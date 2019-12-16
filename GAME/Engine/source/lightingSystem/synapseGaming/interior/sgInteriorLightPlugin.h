//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGINTERIORLIGHTPLUGIN_H_
#define _SGINTERIORLIGHTPLUGIN_H_

#ifndef _INTERIOR_H_
#include "interior/interior.h"
#endif

#ifndef _SGLIGHTINFO_H_
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#endif

class sgLightManager;

struct sgDynamicLightCacheData
{
   Point3F sgPosition;
   Box3F sgBounds;
   sgLightInfo *sgLight;

   // light/model info...
   F32 sgMaxRadius;
   GFXTextureObject* sgTexture;
};

class sgDynamicLightCache : public Vector<sgDynamicLightCacheData>
{
public:
   sgDynamicLightCacheData *sgFind(LightInfo *light)
   {
      sgDynamicLightCache &list = *this;
      for(U32 i=0; i<list.size(); i++)
      {
         if(list[i].sgLight == light)
            return &list[i];
      }
      return NULL;
   }
};

class sgInteriorLightPlugin : public InteriorLightPlugin
{
private:
   sgDynamicLightCache lightdatacache;
   LightInfoList lights;   
   LightInfoDualList duallist;
   sgLightManager *lm;
public:
   sgInteriorLightPlugin();   

   void registerPlugin(bool active);
   virtual bool interiorInstInit(InteriorInstance* intInst);   
   virtual bool zoneInit(S32 zoneid);
   virtual void processRI(Vector<RenderInst*>& list);
};

extern sgInteriorLightPlugin p_sgInteriorLightPlugin;
#endif