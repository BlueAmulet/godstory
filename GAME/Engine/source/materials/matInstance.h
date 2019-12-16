//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _MATINSTANCE_H_
#define _MATINSTANCE_H_

#ifndef _MATERIAL_H_
#include "materials/material.h"
#endif

#include "miscShdrDat.h"
#include "sceneData.h"
#include "renderInstance/renderInstMgr.h"
#include "processedMaterial.h"

class GFXShader;
class GFXCubemap;
class ShaderFeature;
class MatInstanceHook;

//**************************************************************************
// Material Instance
//**************************************************************************
class MatInstance
{
public:
   static void reInitInstances();
private:
   static Vector<MatInstance*> mMatInstList;  

   void construct();  
protected:
   Material *        mMaterial;
   ProcessedMaterial* mProcessedMaterial;
   SceneGraphData    mSGData;
   S32               mCurPass;
   GFXVertexFlags    mVertFlags;
   U32               mMaxStages;
   S32               mCullMode;
   bool              mHasGlow;
   U32               mSortWeight;
   bool				 mSelfIlluminating;

   bool filterGlowPasses( SceneGraphData &sgData );   
   virtual void processMaterial();
public:
   // Used to attach information to a MatInstance.
   MatInstanceHook* mLightingHook;

#ifdef POWER_DEBUG
   char mMatName[64];
   static void dumpMatInsts();
#endif

   // Compares this MatInstance to mat
   virtual S32 compare(MatInstance* mat);

   /// Create a material instance by reference to a Material.
   MatInstance( Material &mat );
   /// Create a material instance by name.
   MatInstance( const char * matName );

   virtual ~MatInstance();

   bool setupPass( SceneGraphData &sgData );

   inline void setObjectXForm(MatrixF &xform) 
   {
      mProcessedMaterial->setObjectXForm(xform, getCurPass());
   }

   inline void setWorldXForm(MatrixF &xform) 
   {
      mProcessedMaterial->setWorldXForm(xform);
   }

   inline void setLightInfo(const SceneGraphData& sgData)
   {
      mProcessedMaterial->setLightInfo(sgData, getCurPass());
   }

   inline void setEyePosition(MatrixF &objTrans, Point3F position)
   {
      mProcessedMaterial->setEyePosition(objTrans, position, getCurPass());
   }
   inline void setBuffers(GFXVertexBufferHandleBase* vertBuffer, GFXPrimitiveBufferHandle* primBuffer)
   {
      mProcessedMaterial->setBuffers(vertBuffer, primBuffer);
   }

   inline void setTextureStages( SceneGraphData &sgData )
   {
      mProcessedMaterial->setTextureStages(sgData, getCurPass());
   }

   inline void setLightingBlendFunc()
   {
      mProcessedMaterial->setLightingBlendFunc();
   }

   void init( SceneGraphData &dat, GFXVertexFlags vertFlags );
   void reInit();
   inline Material *getMaterial(){ return mMaterial; }
   inline ProcessedMaterial *getProcessedMaterial()
   {
      return mProcessedMaterial;
   }
   inline bool hasGlow()
   { 
      return mProcessedMaterial->hasGlow(); 
   }
   inline U32 getCurPass()
   {
      return getMax( mCurPass, 0 );
   }
      
   inline U32 getCurStageNum()
   {
      return mProcessedMaterial->getStageFromPass(getCurPass());
   }
   inline RenderPassData *getPass(U32 pass)
   {
      return mProcessedMaterial->getPass(pass);
   }

   inline const SceneGraphData& getSceneGraphData() const
   {
      return mSGData;
   }

   inline const GFXVertexFlags getVertFlags() const
   {
      return mVertFlags;
   }

   inline bool isSelfIlluminating()
   {
	   return mSelfIlluminating;
   }
};

// This class is used by the lighting system to add additional data to a MatInstance.
// Currently, it just defines a virtual destructor and enforces a bit of type checking.
class MatInstanceHook
{
public:
   virtual ~MatInstanceHook() { };
};

#endif _MATINSTANCE_H_
