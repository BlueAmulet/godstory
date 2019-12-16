//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderInstMgr.h"
#include "materials/sceneData.h"
#include "materials/matInstance.h"
#include SHADER_CONSTANT_INCLUDE_FILE
#include "materials/customMaterial.h"
#include "sceneGraph/sceneGraph.h"
#include "gfx/primbuilder.h"
#include "platform/profiler.h"
#include "terrain/sky.h"
#include "renderElemMgr.h"
#include "renderObjectMgr.h"
#include "renderInteriorMgr.h"
#include "renderMeshMgr.h"
#include "renderMeshQueryMgr.h"
#include "renderShadowMeshQueryMgr.h"
#include "renderRefractMgr.h"
#include "RenderFirstMgr.h"
#include "renderTranslucentMgr.h"
#include "renderGlowMgr.h"
#include "renderShadowMgr.h"
#include "util/safeDelete.h"

#define POOL_GROW_SIZE 2048
#define HIGH_NUM ((U32(-1)/2) - 1)

RenderInstManager gRenderInstManager;

//*****************************************************************************
// RenderInstance
//*****************************************************************************

//-----------------------------------------------------------------------------
// calcSortPoint
//-----------------------------------------------------------------------------
void RenderInst::calcSortPoint( SceneObject *obj, const Point3F &camPosition )
{
   if( !obj ) return;

   // This this sort point calculation changes much, visit TSMesh::render, as it 
   // calculates a sort point per mesh instead of using the object 
   const Box3F& rBox = obj->getObjBox();
   Point3F objSpaceCamPosition = camPosition;
   obj->getRenderWorldTransform().mulP( objSpaceCamPosition );
   objSpaceCamPosition.convolveInverse( obj->getScale() );
   if(obj->IsUseClosestPoint())
		sortPoint = rBox.getClosestPoint( objSpaceCamPosition );
   else
		rBox.getCenter(&sortPoint);	
   sortPoint.convolve( obj->getScale() );
   obj->getRenderTransform().mulP( sortPoint );
}

// methods
void RenderInst::clear()
{
   dMemset( this, 0, sizeof(RenderInst) );
   light = gClientSceneGraph->getLightManager()->getDefaultLight();
   lightSecondary = light;
   fogFactor = 1.0f;
   visibility = 1.0f;
   enableBlendColor = false;
   maxBlendColor = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
   meshDisableZ = 0;
   overrideColorExposure = 1.0f;
   fadeFlag = 0;
   vertexTex = NULL;
   boneNum = 0;

   pInstNext = NULL;
   needRender = true;
}


//*****************************************************************************
// Render Instance Manager
//*****************************************************************************
RenderInstManager::RenderInstManager()
{   
   mWarningMat = NULL;
   mInitialized = false;
   GFXDevice::getDeviceEventSignal().notify(this, &RenderInstManager::handleGFXEvent);
}

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
RenderInstManager::~RenderInstManager()
{
   uninit();
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
void RenderInstManager::handleGFXEvent(GFXDevice::GFXDeviceEventType event)
{
   switch (event)
   {
   case GFXDevice::deInit :
      init();
      break;
   case GFXDevice::deDestroy :
      uninit();
      break;
   }   
}

void RenderInstManager::init()
{
   if (!mInitialized)
   {
      initBins();      
      mInitialized = true;
   }
}

void RenderInstManager::uninit()
{
   if (mInitialized)
   {
      uninitBins();
      if (mWarningMat)
      {
         SAFE_DELETE(mWarningMat);
      }      
      mInitialized = false;
   }
}

//-----------------------------------------------------------------------------
// initBins
//-----------------------------------------------------------------------------
void RenderInstManager::initBins()
{
   mRenderBins.setSize( GFXBin_NumRenderBins );
   dMemset( mRenderBins.address(), 0, mRenderBins.size() * sizeof(RenderElemMgr*) );

   mRenderBins[GFXBin_First]         = new RenderFirstMgr;
   mRenderBins[GFXBin_Sky]           = new RenderObjectMgr;
   mRenderBins[GFXBin_Begin]         = new RenderObjectMgr;
   mRenderBins[GFXBin_MeshQuery]	 = new RenderMeshQueryMgr;
   mRenderBins[GFXBin_Interior]      = new RenderInteriorMgr;
   mRenderBins[GFXBin_InteriorDynamicLighting] = new RenderInteriorMgr;
   mRenderBins[GFXBin_MeshField]     = new RenderMeshMgr;
   mRenderBins[GFXBin_OnFieldTranslucent]	   = new RenderTranslucentMgr;
   mRenderBins[GFXBin_Mesh]          = new RenderMeshMgr;
   mRenderBins[GFXBin_Shadow]        = new RenderShadowMgr;
   mRenderBins[GFXBin_MiscObject]    = new RenderObjectMgr;
   mRenderBins[GFXBin_Decal]         = new RenderObjectMgr;
   mRenderBins[GFXBin_Refraction]    = new RenderRefractMgr;
   mRenderBins[GFXBin_Water]         = new RenderObjectMgr;
   mRenderBins[GFXBin_Foliage]       = new RenderObjectMgr;
   mRenderBins[GFXBin_Translucent]   = new RenderTranslucentMgr;
   mRenderBins[GFXBin_Glow]          = new RenderGlowMgr;
   mRenderBins[GFXBin_MeshShadowQuery]	 = new RenderShadowMeshQueryMgr;
}

//-----------------------------------------------------------------------------
// uninitBins
//-----------------------------------------------------------------------------
void RenderInstManager::uninitBins()
{
   for( U32 i=0; i<mRenderBins.size(); i++ )
   {
      if( mRenderBins[i] )
      {
         delete mRenderBins[i];
         mRenderBins[i] = NULL;
      }
   }
}

void RenderInstManager::doQuery()
{
	PROFILE_START(RenderInstManager_doQuery);
	((RenderMeshQueryMgr *)mRenderBins[GFXBin_MeshQuery])->query();
	((RenderShadowMeshQueryMgr *)mRenderBins[GFXBin_MeshShadowQuery])->query();
	PROFILE_END(RenderInstManager_doQuery);
}

void RenderInstManager::addQueryInst( RenderInst *inst )
{
	mRenderBins[GFXBin_MeshQuery]->addElement( inst );
}

void RenderInstManager::addShadowQueryInst( RenderInst *inst )
{
	mRenderBins[GFXBin_MeshShadowQuery]->addElement( inst );
}


//-----------------------------------------------------------------------------
// add instance
//-----------------------------------------------------------------------------
void RenderInstManager::addInst( RenderInst *inst )
{
   AssertISV( mInitialized, "RenderInstManager not initialized - call console function 'initRenderInstManager()'" );
   AssertFatal(inst != NULL, "doh, null instance");

  if(inst->type == RIT_First)
  {
	  mRenderBins[GFXBin_First]->addElement( inst );
	  return;
  }

   //PROFILE_START(RenderInstManager_addInst);

   //PROFILE_START(RenderInstManager_selectBin0);

   Material* instMat = NULL;
   if (inst->matInst != NULL)
      instMat = inst->matInst->getMaterial();
   
   if( inst->translucent || (instMat && instMat->isTranslucent() ) || (inst->visibility < 1.0f) || inst->GameRenderStatus&Material::DisableColor)
   {
      //PROFILE_END(RenderInstManager_selectBin0);

	  if(inst->meshDisableZ==2 || inst->type==RIT_MeshField)
		  mRenderBins[GFXBin_OnFieldTranslucent]->addElement( inst );
	  else
		  mRenderBins[GFXBin_Translucent]->addElement( inst );
   }
   else
   {
      //PROFILE_END(RenderInstManager_selectBin0);

      PROFILE_START(RenderInstManager_selectBin1);
      // Deal with all the non-translucent cases.
      switch( inst->type )
      {
      case RIT_Interior:
         mRenderBins[GFXBin_Interior]->addElement( inst );
         break;

      case RIT_InteriorDynamicLighting:
         mRenderBins[GFXBin_InteriorDynamicLighting]->addElement(inst);
         break;

      case RIT_Shadow:
         mRenderBins[GFXBin_Shadow]->addElement(inst);
         break;

      case RIT_Decal:
         mRenderBins[GFXBin_Decal]->addElement(inst);
         break;

      case RIT_Sky:
         mRenderBins[GFXBin_Sky]->addElement( inst );
         break;

      case RIT_Water:
         mRenderBins[GFXBin_Water]->addElement( inst );
         break;

      case RIT_Mesh:
	     mRenderBins[GFXBin_Mesh]->addElement( inst );
         break;

	  case RIT_MeshField:
		 mRenderBins[GFXBin_MeshField]->addElement( inst );
		 break;

      case RIT_Foliage:
         mRenderBins[GFXBin_Foliage]->addElement( inst );
         break;

      case RIT_Begin:
         mRenderBins[GFXBin_Begin]->addElement( inst );
         break;

      default:
         mRenderBins[GFXBin_MiscObject]->addElement( inst );
         break;
      }

      PROFILE_END(RenderInstManager_selectBin1);
   }

   //PROFILE_START(RenderInstMgr_specialBin);

   // handle extra insertions
   if( inst->matInst )
   {
      PROFILE_START(RenderInstMgr_custDynCast); // wonder if this will actually be non zero on the profiler...
      CustomMaterial* custMat = dynamic_cast<CustomMaterial*>( inst->matInst->getMaterial() );
      PROFILE_END(RenderInstMgr_custDynCast);
      if( custMat && custMat->refract )
      {
         mRenderBins[GFXBin_Refraction]->addElement( inst );
      }

      if( inst->matInst->hasGlow() && 
         !gClientSceneGraph->isReflectPass() &&
         !inst->obj )
      {
         mRenderBins[GFXBin_Glow]->addElement( inst );
      }
   }
   //PROFILE_END(RenderInstMgr_specialBin);

   //PROFILE_END(RenderInstManager_addInst);
}


//-----------------------------------------------------------------------------
// QSort callback function
//-----------------------------------------------------------------------------
S32 FN_CDECL cmpKeyFunc(const void* p1, const void* p2)
{
   const RenderElemMgr::MainSortElem* mse1 = (const RenderElemMgr::MainSortElem*) p1;
   const RenderElemMgr::MainSortElem* mse2 = (const RenderElemMgr::MainSortElem*) p2;

   S32 test1 = S32(mse1->key) - S32(mse2->key);

   return ( test1 == 0 ) ? S32(mse1->key2) - S32(mse2->key2) : test1;
}

//-----------------------------------------------------------------------------
// sort
//-----------------------------------------------------------------------------
void RenderInstManager::sort()
{
   PROFILE_START(RIM_sort);

   if( mRenderBins.size() )
   {
      for( U32 i=0; i<GFXBin_NumRenderBins; i++ )
      {
         if( mRenderBins[i] )
         {
            mRenderBins[i]->sort();
         }
      }
   }

   PROFILE_END(RIM_sort);
}

//-----------------------------------------------------------------------------
// clear
//-----------------------------------------------------------------------------
void RenderInstManager::clear()
{
   mRIAllocator.clear();
   mXformAllocator.clear();
   mPrimitiveFirstPassAllocator.clear();
   
   if( mRenderBins.size() )
   {
      for( U32 i=0; i<GFXBin_NumRenderBins; i++ )
      {
         if( mRenderBins[i] )
         {
            mRenderBins[i]->clear();
         }
      }
   }
}

//-----------------------------------------------------------------------------
// initialize warning material instance
//-----------------------------------------------------------------------------
void RenderInstManager::initWarnMat()
{
   if (mWarningMat)
      return;

   Material *warnMat = static_cast<Material*>(Sim::findObject( "WarningMaterial" ) );
   if( !warnMat )
   {
      Con::errorf( "Can't find WarningMaterial" );
   }
   else
   {
      SceneGraphData sgData;
      GFXVertexPNTTB *vertDef = NULL; // the variable itself is the parameter to the template function
      mWarningMat = new MatInstance( *warnMat );
      mWarningMat->init( sgData, (GFXVertexFlags)getGFXVertFlags( vertDef ) );
   }
}

//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
void RenderInstManager::render()
{
   GFX->pushWorldMatrix();
   MatrixF proj = GFX->getProjectionMatrix();

   if( mRenderBins.size() )
   {
      for( U32 i=0; i<GFXBin_NumRenderBins; i++ )
      {
         if( mRenderBins[i] )
         {
            GFX->beginBinNotify( (GFXRenderBinTypes)i );
            mRenderBins[i]->render();
            GFX->endBinNotify();
         }
      }
   }

   GFX->popWorldMatrix();
   GFX->setProjectionMatrix( proj );
}
