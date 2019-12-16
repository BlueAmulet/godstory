//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "matInstance.h"
#include "materials/customMaterial.h"
#include "gfx/gfxDevice.h"
#include SHADER_CONSTANT_INCLUDE_FILE
#include "shaderGen/featureMgr.h"
#include "gfx/cubemapData.h"
#include "gfx/gfxCubemap.h"
#include "materials/processedMaterial.h"
#include "materials/processedFFMaterial.h"
#include "materials/processedShaderMaterial.h"
#include "materials/processedCustomMaterial.h"

Vector< MatInstance * > MatInstance::mMatInstList;
//----------------------------------------------------------------------------
// reInitInstances - static function
//----------------------------------------------------------------------------
void MatInstance::reInitInstances()
{
   GFX->flushProceduralShaders();
   for( U32 i=0; i<mMatInstList.size(); i++ )
   {
      mMatInstList[i]->reInit();
   }
}

//****************************************************************************
// Material Instance
//****************************************************************************
MatInstance::MatInstance( Material &mat )
{
   mMaterial = &mat;
   mCullMode = -1;

   if(mat.emissive[0])
	   mSelfIlluminating = true;
   else
	   mSelfIlluminating = false;

   construct();
}

MatInstance::MatInstance( const char * matName)
{
   // Get the material
   Material * foundMat;

   if(!Sim::findObject(matName, foundMat))
   {
      Con::errorf("MatInstance::MatInstance() - Unable to find material '%s'", matName);
      foundMat = NULL;
   }

   mMaterial = foundMat;
   if(foundMat->emissive[0])
	   mSelfIlluminating = true;
   else
	   mSelfIlluminating = false;

   construct();
}

//----------------------------------------------------------------------------
// Construct
//----------------------------------------------------------------------------
void MatInstance::construct()
{
   mCurPass = -1;
   mHasGlow = false;
   mProcessedMaterial = false;
   mVertFlags = (GFXVertexFlags) NULL;
   mMaxStages = 1;

#ifdef POWER_DEBUG
   dMemcpy( mMatName, "Unknown", sizeof("Unknown") );
#endif

   mMatInstList.push_back( this );
   mLightingHook = NULL;
   mSortWeight = 0;
   mSelfIlluminating = false;
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
MatInstance::~MatInstance()
{
   SAFE_DELETE(mLightingHook);   

   // remove from global MatInstance list
   for( U32 i=0; i<mMatInstList.size(); i++ )
   {
      if( mMatInstList[i] == this )
      {
         mMatInstList[i] = mMatInstList.last();
         mMatInstList.decrement();
      }
   }

   delete mProcessedMaterial;
   mProcessedMaterial = NULL;
}

//----------------------------------------------------------------------------
// Init
//----------------------------------------------------------------------------
void MatInstance::init( SceneGraphData &dat, GFXVertexFlags vertFlags )
{
   mSGData = dat;
   mVertFlags = vertFlags;
   
   if( !mProcessedMaterial )
   {
      processMaterial();
   }
}


//----------------------------------------------------------------------------
// reInitialize
//----------------------------------------------------------------------------
void MatInstance::reInit( )
{
   if (!mProcessedMaterial)
      return;
   delete mProcessedMaterial;
   mProcessedMaterial = NULL;
   processMaterial();
}

//----------------------------------------------------------------------------
// Process stages
//----------------------------------------------------------------------------
void MatInstance::processMaterial()
{
   AssertFatal(mMaterial, "Material is not valid!");
   if (!mMaterial)
      return;
#ifdef POWER_DEBUG
   const char *name = mMaterial->getName();
   if( name )
   {
      dStrcpy( mMatName, sizeof(mMatName), name );
   }
   else
   {
      dStrcpy( mMatName, sizeof(mMatName), "Unknown" );
   }
#endif
   if( dynamic_cast<CustomMaterial*>(mMaterial) )
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
   else if(GFX->getPixelShaderVersion() > 0.001)
   {
      mProcessedMaterial = new ProcessedShaderMaterial(*mMaterial);
      mProcessedMaterial->init(mSGData, mVertFlags);
   }
   else
   {
      mProcessedMaterial = new ProcessedFFMaterial(*mMaterial);
      mProcessedMaterial->init(mSGData, mVertFlags);
   }
}

//----------------------------------------------------------------------------
// Filter glow passes - this function filters out passes that do not glow.
//    Returns true if there are no more glow passes, false otherwise.
//----------------------------------------------------------------------------
bool MatInstance::filterGlowPasses( SceneGraphData &sgData )
{
   // if rendering to glow buffer, render only glow passes
   if( sgData.glowPass )
   {
      RenderPassData* data = mProcessedMaterial->getPass(mCurPass);
      while( data && !data->glow )
      {
         mCurPass++;
         if( mCurPass >= mProcessedMaterial->getNumPasses() )
         {
            return true;
         }
         data = mProcessedMaterial->getPass(mCurPass);
      }
   }
   return false;
}

//----------------------------------------------------------------------------
// Setup pass - needs scenegraph data because the lightmap will change across
//    several materials.
//----------------------------------------------------------------------------
bool MatInstance::setupPass( SceneGraphData &sgData )
{
   ++mCurPass;
   if(filterGlowPasses(sgData) || !mProcessedMaterial->setupPass(sgData, mCurPass))
   {
      mProcessedMaterial->cleanup(mCurPass - 1);
      mCurPass = -1;
      return false;
   }
   return true;
}

// Currently, mSortWeight will be set by derived class to get the ordering right if needed
S32 MatInstance::compare( MatInstance* mat )
{
   return mSortWeight - mat->mSortWeight;
}


#ifdef POWER_DEBUG
void MatInstance::dumpMatInsts()
{
   if( !mMatInstList.size() ) return;

   Con::printf("---------------------MatInstances-----------------------------");

   for( U32 i=0; i<mMatInstList.size(); i++ )
   {
      if( mMatInstList[i] )
      {
         Con::printf("Mat: %s", mMatInstList[i]->mMatName);
      }
   }

   Con::printf("-------------------dump complete ------------------------------");

}
#endif

//------------------------------------------------------------------------------
// Console functions
//------------------------------------------------------------------------------
ConsoleFunction( reInitMaterials, void, 1, 1, "" )
{
   MatInstance::reInitInstances();
}


