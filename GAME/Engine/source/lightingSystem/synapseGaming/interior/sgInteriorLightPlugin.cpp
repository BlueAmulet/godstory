//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/interior/sgInteriorLightPlugin.h"
#include "interior/interiorInstance.h"
#include "renderInstance/renderInteriorMgr.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgMatInstance.h"

// Private to this file.
sgInteriorLightPlugin p_sgInteriorLightPlugin;

sgInteriorLightPlugin::sgInteriorLightPlugin()
{
   SGLM->mSgActivate.notify(this, &sgInteriorLightPlugin::registerPlugin);   
}

void sgInteriorLightPlugin::registerPlugin(bool active)
{
   if (active)
      Interior::registerLightPlugin(this);
   else
      Interior::unregisterLightPlugin(this);
}

bool sgInteriorLightPlugin::interiorInstInit(InteriorInstance* intInst)
{
   lights.clear();
   lightdatacache.clear();

   lm = SGLM;
   lm->getAllUnsortedLights(lights);

   // filter down to needed lights...
   for(U32 i=0; i<lights.size(); i++)
   {
      SG_CHECK_LIGHT(lights[i]);
      sgLightInfo *light = static_cast<sgLightInfo*>(lights[i]);
      if((light->mType != ::LightInfo::Point) && (light->mType != ::LightInfo::Spot))
         continue;

      // get info...
      sgLightingModel &lightingmodel = sgLightingModelManager::sgGetLightingModel(light);		

      F32 maxrad = lightingmodel.sgGetMaxRadius(true);
      light->sgTempModelInfo[0] = 0.5f / maxrad;

      GFXTextureObject* tex;
      if(light->mType == LightInfo::Spot)
         tex = lightingmodel.sgGetDynamicLightingTextureSpot();
      else
         tex = lightingmodel.sgGetDynamicLightingTextureOmni();

      lightingmodel.sgResetState();

      Point3F offset = Point3F(maxrad, maxrad, maxrad);
      Point3F lightpos = light->mPos;
      intInst->getRenderWorldTransform().mulP(lightpos);
      lightpos.convolveInverse(intInst->getScale());
      Box3F box;
      box.min = lightpos;
      box.max = lightpos;
      box.min -= offset;
      box.max += offset;

      // test visible...
      // TODO!!! - nm, using zone vis instead...

      // test illuminate interior...
      if(intInst->getObjBox().isOverlapped(box) == false)
         continue;

      // add to the list...
      lightdatacache.increment();
      lightdatacache.last().sgLight = light;
      lightdatacache.last().sgPosition = lightpos;
      lightdatacache.last().sgBounds = box;
      lightdatacache.last().sgMaxRadius = maxrad;
      lightdatacache.last().sgTexture = tex;
   }
   return lights.size() > 0;
}

bool sgInteriorLightPlugin::zoneInit( S32 zoneid )
{
   // compare the lights and build list...
   lights.clear();
   for(U32 i=0; i<lightdatacache.size(); i++)
   {
      sgLightInfo* sgLight = lightdatacache[i].sgLight;
      if(!sgLight->sgAllowDiffuseZoneLighting(zoneid))
         continue;

      // am I in the zone?
      // need to verify - this is the only thing stopping
      // the entire interior from being rerendered!!!
      if(!sgLight->sgIsInZone(zoneid))
         continue;

      lights.push_back(lightdatacache[i].sgLight);
   }

   if(lights.size() <= 0)
      return false;

   // get the dual sorted list...
   duallist.clear();
   lm->sgBuildDualLightLists(lights, duallist);

   return true;
}

void sgInteriorLightPlugin::processRI( Vector<RenderInst*>& list )
{
   for(U32 d=0; d<duallist.size(); d++)
   {
      sgDynamicLightCacheData *lightpri = lightdatacache.sgFind(duallist[d].sgLightPrimary);
      sgDynamicLightCacheData *lightsec = lightdatacache.sgFind(duallist[d].sgLightSecondary);

      if(!lightpri)
         continue;

      // avoid dual on single groups...
      bool allowdual = (lightsec != NULL);

      for (U32 i=0; i < list.size(); i++)
      {
         // Here, we need to create new render instances for the extra passes, but use the
         // ones passed into us for the last pass.
         RenderInst* ri;         
         if (d < duallist.size()-1)
         {
            ri = gRenderInstManager.allocInst(false);
            *ri = *(list[i]);
         } else {
            ri = list[i];
         }

         // find the primary material (secondary lights all render the same)...
         sgMatInstance* dmat = SGLM->getDynamicLightingMaterial(ri->matInst, lightpri->sgLight, allowdual);
         if (!dmat)
            continue;

         ri->matInst = dmat;
         ri->dynamicLight = lightpri->sgTexture;
         ri->light = lightpri->sgLight;

         if(dmat->isDynamicLightingMaterial_Dual())
         {
            ri->dynamicLightSecondary = lightsec->sgTexture;
            ri->lightSecondary = lightsec->sgLight;
            gRenderInstManager.addInst(ri);
            continue;
         }

         gRenderInstManager.addInst(ri);

         // could be the material is only single...
         if(lightsec)
         {
            // own pass needs own material...
            dmat = SGLM->getDynamicLightingMaterial(ri->matInst, lightsec->sgLight, false);
            if(!dmat)
               continue;

            RenderInst* secRI = gRenderInstManager.allocInst(false);
            *secRI = *ri;
            ri->matInst = dmat;
            ri->dynamicLight = lightsec->sgTexture;
            ri->light = lightsec->sgLight;
            gRenderInstManager.addInst(ri);
         }
      }
   }
}
