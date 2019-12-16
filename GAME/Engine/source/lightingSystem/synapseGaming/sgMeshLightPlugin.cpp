//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgMeshLightPlugin.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgMatInstance.h"

sgMeshLightPlugin g_sgMeshLightPlugin;

//
U32 sgMeshLightPlugin::prepareLight()
{
	sgLightManager *lm = SGLM;
	baselights.clear();
	lm->getBestLights(baselights);

	if(baselights.size() < 1)
		baselights.push_back(lm->getSpecialLight(LightManager::slSunLightType));

	duallights.clear();
	lm->sgBuildDualLightLists(baselights, duallights);

	return duallights.size();
}

void sgMeshLightPlugin::processRI( TSMesh* mesh, Vector<RenderInst*>& list)
{
	sgLightManager *lm = SGLM;

   for(S32 l=0; l<duallights.size(); l++)
   {
      // Set up our light information
      bool sunlight = false;
      LightInfoDual *duallight = &duallights[l];
      GFXTextureObject* dynamicLight = NULL;
      GFXTextureObject* dynamicLightSecondary = NULL;

      if(!duallight->sgLightPrimary)
         continue;
      
      sgLightInfo* light = duallight->sgLightPrimary;
      sgLightInfo* secondary = duallight->sgLightSecondary; 

      if((light->mType == LightInfo::Vector) || (light->mType == LightInfo::Ambient))
      {
         sunlight = true;         
      }
      else
      {
         // get the model...
         sgLightingModel &lightingmodel = sgLightingModelManager::sgGetLightingModel(
            light->sgLightingModelName);
         lightingmodel.sgSetState(light);
         // get the info...  
         F32 maxrad = lightingmodel.sgGetMaxRadius(true);
         light->sgTempModelInfo[0] = 0.5f / maxrad;         
         // get the dynamic lighting texture...
         if((light->mType == LightInfo::Spot) || (light->mType == LightInfo::SGStaticSpot))
            dynamicLight = lightingmodel.sgGetDynamicLightingTextureSpot();
         else
            dynamicLight = lightingmodel.sgGetDynamicLightingTextureOmni();
         // reset the model...
         lightingmodel.sgResetState();

         if(secondary)
         {
            // get the model...
            sgLightingModel &lightingmodel = sgLightingModelManager::sgGetLightingModel(
               secondary->sgLightingModelName);
            lightingmodel.sgSetState(secondary);
            // get the info...
            F32 maxrad = lightingmodel.sgGetMaxRadius(true);
            secondary->sgTempModelInfo[0] = 0.5f / maxrad;
            // get the dynamic lighting texture...
            if((secondary->mType == LightInfo::Spot) || (secondary->mType == LightInfo::SGStaticSpot))
               dynamicLightSecondary = lightingmodel.sgGetDynamicLightingTextureSpot();
            else
               dynamicLightSecondary = lightingmodel.sgGetDynamicLightingTextureOmni();
            // reset the model...
            lightingmodel.sgResetState();
         }     
      }

      // Finally, let's process some geometry      
      for (U32 i = 0; i < list.size(); i++)
      {
		  //Ray: 太阳光使用第一遍pass,点光源混合状态将不同因此使用后续pass
         RenderInst* ri;
		 if(sunlight)
		 {
			 ri = list[i];
		 }
		 else
		 {
			 ri = gRenderInstManager.allocInst(false);
			 *ri = *(list[i]);
		 }

         ri->light = light;

         ri->dynamicLight = dynamicLight;
         ri->dynamicLightSecondary = dynamicLightSecondary;
         MatInstance* matInst = ri->matInst;

         bool isDualLight = false;
         //----------------------------------------------
         if(!sunlight)
         {               
            sgMatInstance *dmat = SGLM->getDynamicLightingMaterial(matInst, light, (secondary != NULL));
            if(matInst->getMaterial()->translucent || !dmat || ri->translucent || (ri->visibility < 1.0f) || ri->GameRenderStatus&Material::SunLightOnly)
               continue;
            isDualLight = dmat->isDynamicLightingMaterial_Dual();
            matInst = dmat;
         } else { 
            sgMatInstance* dmat = dynamic_cast<sgMatInstance*>(matInst);
            if (dmat)
            {
               isDualLight = dmat->isDynamicLightingMaterial_Dual();
            }
         }
         ri->matInst = matInst;
         if(isDualLight)
         {
            // piggyback onto the primary light...
            ri->lightSecondary = secondary;
         }
         else if(secondary)
         {
            // not dual material, but two lights?
            // finalize light one...
            gRenderInstManager.addInst(ri);

            // create light two...
            RenderInst* ri2 = gRenderInstManager.allocInst(false);
            *ri2 = *ri;

            // make the secondary look like the primary...
            ri2->dynamicLight = ri->dynamicLightSecondary;
            ri2->light = secondary;
            ri2->lightSecondary = lm->getDefaultLight();
            ri = ri2;
         }

         // finalize whichever is left open...
         if (ri->lightSecondary == NULL)
            ri->lightSecondary = lm->getDefaultLight();
         gRenderInstManager.addInst(ri);
      }
   }
}
