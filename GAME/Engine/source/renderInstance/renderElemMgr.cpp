//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderElemMgr.h"
#include "materials/matInstance.h"
#include "sceneGraph/sceneGraph.h"

//-----------------------------------------------------------------------------
// RenderElemMgr
//-----------------------------------------------------------------------------
RenderElemMgr::RenderElemMgr()
{
   mElementList.reserve( 2048 );
}

//-----------------------------------------------------------------------------
// addElement
//-----------------------------------------------------------------------------
void RenderElemMgr::addElement( RenderInst *inst )
{
   AssertFatal(inst->light != NULL, "Light is NULL!");
   AssertFatal(inst->lightSecondary != NULL, "Secondary light is NULL!");

   mElementList.increment();
   MainSortElem &elem = mElementList.last();
   elem.inst = inst;
   elem.key = elem.key2 = 0;

   // sort by material
   if( inst->matInst )
   {
      elem.key = (U32) inst->matInst->getMaterial();
   }

   // sort by vertex buffer
   if( inst->vertBuff )
   {
      elem.key2 = (U32) inst->vertBuff->getPointer();
   }

}

//-----------------------------------------------------------------------------
// clear
//-----------------------------------------------------------------------------
void RenderElemMgr::clear()
{
   mElementList.clear();
}

//-----------------------------------------------------------------------------
// sort
//-----------------------------------------------------------------------------
void RenderElemMgr::sort()
{
   dQsort( mElementList.address(), mElementList.size(), sizeof(MainSortElem), cmpKeyFunc);
}

//-----------------------------------------------------------------------------
// QSort callback function
//-----------------------------------------------------------------------------
S32 FN_CDECL RenderElemMgr::cmpKeyFunc(const void* p1, const void* p2)
{
   const MainSortElem* mse1 = (const MainSortElem*) p1;
   const MainSortElem* mse2 = (const MainSortElem*) p2;

   // dynamic lights *MUST* be rendered after the base pass!
   if(mse1->inst && mse1->inst->matInst &&
      mse2->inst && mse2->inst->matInst)
   {
      S32 testA = mse1->inst->matInst->compare(mse2->inst->matInst);
      if(testA != 0)
         return testA;
   }

   // FIXED CODE
   S32 test1 = S32(mse2->key) - S32(mse1->key);

   return ( test1 == 0 ) ? S32(mse2->key2) - S32(mse1->key2) : test1;
   /* ORIGINAL CODE
   S32 test1 = S32(mse1->key) - S32(mse2->key);

   return ( test1 == 0 ) ? S32(mse1->key2) - S32(mse2->key2) : test1;
   */
} 

void RenderElemMgr::setupSGData( RenderInst *ri, SceneGraphData &data )
{
   data.reset();
   data.setFogParams();

   data.light = ri->light;   
   data.lightSecondary = ri->lightSecondary;
   
   data.dynamicLight = ri->dynamicLight;
   data.dynamicLightSecondary = ri->dynamicLightSecondary;

   data.fogFactor = ri->fogFactor;

   data.camPos = gRenderInstManager.getCamPos();
   data.objTrans = *ri->objXform;
   data.backBuffTex = ri->backBuffTex;
   data.cubemap = ri->cubemap;
   data.miscTex = ri->miscTex;
   data.vertexTex = ri->vertexTex;
   data.boneNum = ri->boneNum;
   
   data.lightmap     = ri->lightmap;
   data.normLightmap = ri->normLightmap;
   data.visibility = ri->visibility;

   data.GameRenderStatus	= ri->GameRenderStatus;
   data.maxBlendColor = ri->maxBlendColor;
   data.enableBlendColor = ri->enableBlendColor;

   data.overrideColorExposure = ri->overrideColorExposure;
}
