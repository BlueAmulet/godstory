//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _FEATUREMGR_H_
#define _FEATUREMGR_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif 

class ShaderFeature;

//**************************************************************************
// Feature Manager
//**************************************************************************
class FeatureMgr
{

   Vector <ShaderFeature *> mFeatures;
   Vector <ShaderFeature *> mAuxFeatures;  // auxiliary features used internally

   void  init();
   void  shutdown();


public:
   enum Constants
   {
      NumAuxFeatures = 1,
   };
   
   FeatureMgr();
   ~FeatureMgr();

   ShaderFeature * get( U32 index );
   ShaderFeature * getAux( U32 index );

   // Allows other systems to add features.  index is the enum in GFXShaderFeatureData
   void registerFeature(const U32 index, ShaderFeature* newFeature);

   // Unregister a feature.
   void unregisterFeature(const U32 index);
};

extern FeatureMgr gFeatureMgr;


#endif // FEATUREMGR