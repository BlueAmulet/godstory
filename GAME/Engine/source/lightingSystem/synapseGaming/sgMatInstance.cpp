//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgMatInstance.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/processedDynamicLightMaterial.h"
#include "materials/processedFFMaterial.h"
#include "materials/processedCustomMaterial.h"

sgMatInstance::sgMatInstance(Material &mat, const MatInstance* root) : Parent(mat)
{
   mSGData = root->getSceneGraphData();
   mVertFlags = root->getVertFlags();
   mMaterialType = UnknownMaterial;
   mSortWeight = 1;
}

void sgMatInstance::processMaterial()
{
   if(dynamic_cast<CustomMaterial*>(mMaterial))
   {
      F32 pixVersion = GFX->getPixelShaderVersion();
      CustomMaterial* custMat = static_cast<CustomMaterial*>(mMaterial);
      if(custMat->mVersion > pixVersion)
      {
         if(custMat->fallback)
         {
            mMaterial = custMat->fallback;
            processMaterial();
         }
         else
         {
            AssertWarn(false, avar("Can't load CustomMaterial for %s, using generic FF fallback", custMat->mapTo));
            mProcessedMaterial = new ProcessedFFMaterial(*mMaterial);
            mProcessedMaterial->init(mSGData, mVertFlags);
         }
      }
      else
      {
         mProcessedMaterial = new ProcessedCustomMaterial(*mMaterial);
         mProcessedMaterial->init(mSGData, mVertFlags);
      }
   }
   else if (GFX->getPixelShaderVersion() > 0.001)
   {
      mProcessedMaterial = new ProcessedDynamicLightMaterial(*mMaterial);
      ProcessedDynamicLightMaterial* pmat = static_cast<ProcessedDynamicLightMaterial*>(mProcessedMaterial);
      pmat->setDualLight(isDynamicLightingMaterial_Dual());
      pmat->setDynamicLightingFeatures(mDynamicLightingFeatures);
      pmat->setMask(mMaterialType == DynamicLightingMaskMaterial);
      mProcessedMaterial->init(mSGData, mVertFlags);
   }
   else
   {
      mProcessedMaterial = new ProcessedFFMaterial(*mMaterial);
      mProcessedMaterial->init(mSGData, mVertFlags);
   }      
}

sgMatInstanceHook::sgMatInstanceHook(Material* mat, const MatInstance* matInst)
{   
   dMemset(mDynamicLightingMaterials_Single, 0, sizeof(mDynamicLightingMaterials_Single));
   dMemset(mDynamicLightingMaterials_Dual, 0, sizeof(mDynamicLightingMaterials_Dual));
   mDynamicLightingMaskMaterial = NULL;
   mMaterial = mat;

   // Create our sub-matinstances 
   if (!mat->emissive[0])
   {   
      CustomMaterial *custmat = dynamic_cast<CustomMaterial *>(mat);
      if(custmat)
      {
         if(custmat->dynamicLightingMaterial && !mDynamicLightingMaterials_Single[sgLightInfo::sgFull])
         {
            // custom materials only use the assigned single-full dynamic lighting shader...
            sgMatInstanceSingle* matinst = new sgMatInstanceSingle(*custmat->dynamicLightingMaterial, matInst);
            matinst->mMaterialType = sgMatInstance::DynamicLightingSingleMaterial;
            matinst->mDynamicLightingFeatures = sgLightInfo::sgFull;
            matinst->init();
            mDynamicLightingMaterials_Single[sgLightInfo::sgFull] = matinst;
         }
         if(custmat->dynamicLightingMaskMaterial && !mDynamicLightingMaskMaterial)
         {
            sgMatInstanceMask* matinst = new sgMatInstanceMask(*custmat->dynamicLightingMaskMaterial, matInst);
            matinst->init();
            mDynamicLightingMaskMaterial = matinst;
         }
      }
      else 
      {
         for(U32 i=0; i<sgLightInfo::sgFeatureCount; i++)
         {
            // always build these...
            if(!mDynamicLightingMaterials_Single[i])
            {
               sgMatInstanceSingle* matinst = new sgMatInstanceSingle(*mat, matInst);  
               matinst->mMaterialType = sgMatInstance::DynamicLightingSingleMaterial;
               matinst->mDynamicLightingFeatures = sgLightInfo::sgFeatures(i);
               matinst->init();
               mDynamicLightingMaterials_Single[i] = matinst;
            }

            // cube mapping exceeds 8 tex params, and 2.0 needed...
            if((i > sgLightInfo::sgFull) && (GFX->getPixelShaderVersion() >= 2.0))
            {
               if(!mDynamicLightingMaterials_Dual[i])
               {
                  // build the piggybacked dual light materials...
                  sgMatInstanceDual* matinst = new sgMatInstanceDual(*mat, matInst);                                 
                  matinst->mMaterialType = sgMatInstance::DynamicLightingDualMaterial;
                  matinst->mDynamicLightingFeatures = sgLightInfo::sgFeatures(i);
                  matinst->init();
                  mDynamicLightingMaterials_Dual[i] = matinst;
               }
            }
         }
      }
   }
}

sgMatInstanceHook::~sgMatInstanceHook()
{
   clearDynamicLightingMaterials();
}

void sgMatInstanceHook::clearDynamicLightingMaterials()
{
   for(U32 i=0; i<sgLightInfo::sgFeatureCount; i++)
   {
      SAFE_DELETE(mDynamicLightingMaterials_Single[i]);
      SAFE_DELETE(mDynamicLightingMaterials_Dual[i]);
   }
   SAFE_DELETE(mDynamicLightingMaskMaterial);
}

sgMatInstance* sgMatInstanceHook::getDynamicLightingMaterial(sgLightInfo *sgLight, bool tryfordual)
{   
   // in order from most features to least...
   // we scan in reverse order (from least to most)
   // this provides the opportunity to combine like
   // materials making batching more efficient, by
   // leaving a level NULL as the next level up is
   // the same (not yet done)...
   AssertFatal((sgLight->sgSupportedFeatures < sgLightInfo::sgFeatureCount), "Invalid light features.");
   // Emissive materials don't have dynamic lighting
   if (mMaterial->emissive[0])
      return NULL;
   // masked?   
   if((sgLight->sgLightMask) && (mDynamicLightingMaskMaterial))
      return mDynamicLightingMaskMaterial;
   // try for dual?
   if(tryfordual && sgLightManager::sgAllowDynamicLightingDualOptimization())
   {
      // try dual...
      for(S32 i=sgLight->sgSupportedFeatures; i>=0; i--)
      {
         if(mDynamicLightingMaterials_Dual[i])
            return mDynamicLightingMaterials_Dual[i];
      }
   }
   // look for single...
   for(S32 i=sgLight->sgSupportedFeatures; i>=0; i--)
   {
      if(mDynamicLightingMaterials_Single[i]) 
         return mDynamicLightingMaterials_Single[i];
   }
   return NULL;
}
