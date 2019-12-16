//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGMATINSTANCE_H_
#define _SGMATINSTANCE_H_

#ifndef _SGLIGHTINFO_H_
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#endif

#ifndef _MATINSTANCE_H_
#include "materials/matInstance.h"
#endif

class sgMatInstance;
class sgMatInstanceSingle;
class sgMatInstanceDual;
class sgMatInstanceMask;

class sgMatInstanceHook : public MatInstanceHook
{
private:
   Material* mMaterial;
   sgMatInstanceSingle *mDynamicLightingMaterials_Single[sgLightInfo::sgFeatureCount];
   sgMatInstanceDual *mDynamicLightingMaterials_Dual[sgLightInfo::sgFeatureCount];
   // short a simple - if you want masked lighting sgNoCube is all you get...
   sgMatInstanceMask *mDynamicLightingMaskMaterial;

   void clearDynamicLightingMaterials();
public:
   sgMatInstanceHook(Material* mat, const MatInstance* matInst);
   virtual ~sgMatInstanceHook();

   sgMatInstance* getDynamicLightingMaterial(sgLightInfo *light, bool tryfordual);
};

class sgMatInstance : public MatInstance
{
private:
   typedef MatInstance Parent;
protected:   
   virtual void processMaterial();
public:   
   //sgMatInstance() : Parent() { };
   /// Create a material instance by reference to a Material.
   sgMatInstance( Material &mat, const MatInstance* root );

   enum MatInstanceType
   {
      UnknownMaterial,
      DynamicLightingMaskMaterial,
      DynamicLightingSingleMaterial,
      DynamicLightingDualMaterial
   };
   MatInstanceType mMaterialType;
   sgLightInfo::sgFeatures mDynamicLightingFeatures;   

   bool isDynamicLightingMaterial_Dual() 
   {
      return (mMaterialType == DynamicLightingDualMaterial);
   }   

   void init()
   {
      Parent::init(mSGData, mVertFlags);
   }
};

class sgMatInstanceSingle : public sgMatInstance
{
public:
    sgMatInstanceSingle(Material &mat, const MatInstance* root) : sgMatInstance(mat, root){ mSortWeight = 2; }
};

class sgMatInstanceDual : public sgMatInstance
{
public:
    sgMatInstanceDual(Material &mat, const MatInstance* root) : sgMatInstance(mat, root){ mSortWeight = 3; }
};

class sgMatInstanceMask : public sgMatInstance
{
public:
    sgMatInstanceMask(Material &mat, const MatInstance* root) : sgMatInstance(mat, root){ mSortWeight = 4; }
};

#endif _SGMATINSTANCE_H_
