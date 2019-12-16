//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "renderMeshQueryMgr.h"
#include "gfx/gfxTransformSaver.h"

#include "gfx/D3D9/gfxD3D9Device.h"
#include "gfx/gfxD3D9OcclusionQuery.h"

#include "gui/missionEditor/editor.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"

#include "sceneGraph/renderableSceneObject.h"
//===================================================================================
//---------------------------------------------------------------------------
//¾²Ì¬³ÉÔ±ºÍºê¶¨Òå
//×´Ì¬¿é
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* RenderMeshQueryMgr::mCullCWSB = NULL;
GFXStateBlock* RenderMeshQueryMgr::mCullCCWSB = NULL;
GFXStateBlock* RenderMeshQueryMgr::mClearSB = NULL;

void RenderMeshQueryMgr::clear()
{
	currentPos = 0;
	mElementList.clear();
}

RenderMeshQueryMgr::~RenderMeshQueryMgr()
{
	for(int i=0;i<MAX_QUERY_NUM;i++)
		SAFE_DELETE(queryList[i].m_pQuery);
}

void RenderMeshQueryMgr::sort()
{
	dQsort( mElementList.address(), mElementList.size(), sizeof(MainSortElem), cmpTranslucentKeyFunc);
}

//-----------------------------------------------------------------------------
// QSort callback function
//-----------------------------------------------------------------------------
S32 FN_CDECL RenderMeshQueryMgr::cmpTranslucentKeyFunc(const void* p1, const void* p2)
{
	const MainSortElem* mse1 = (const MainSortElem*) p1;
	const MainSortElem* mse2 = (const MainSortElem*) p2;

	return S32(mse2->key) - S32(mse1->key);  //µ¹Ðò
} 


//-----------------------------------------------------------------------------
// add element
//-----------------------------------------------------------------------------
void RenderMeshQueryMgr::addElement( RenderInst *inst )
{
	mElementList.increment();
	MainSortElem &elem = mElementList.last();
	elem.inst = inst;

	// sort by distance (the multiply is there to prevent us from losing information when converting to a U32)
	F32 camDist = (gRenderInstManager.getCamPos() - inst->sortPoint).len();
	elem.key = *((U32*)&camDist);
}

//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
#ifdef STATEBLOCK
void RenderMeshQueryMgr::render()
{
	// Early out if nothing to draw.
	if(!mElementList.size())
		return;
	PROFILE_SCOPE(RenderMeshQueryMgr);
	// Automagically save & restore our viewport and transforms.
	GFXTransformSaver saver;
	// set render states
	if( gClientSceneGraph->isReflectPass() )
	{
		AssertFatal(mCullCWSB, "RenderInteriorMgr::render -- mCullCWSB cannot be NULL.");
		mCullCWSB->apply();
	}
	else
	{
		AssertFatal(mCullCCWSB, "RenderInteriorMgr::render -- mCullCCWSB cannot be NULL.");
		mCullCCWSB->apply();
	}
	GFX->disableShaders();
	queryStruct *query = NULL;
	U32 binSize = mElementList.size();
	for( U32 j=0; j<binSize; j++)
	{
		RenderInst *ri = mElementList[j].inst;
		AssertFatal(currentPos <MAX_QUERY_NUM, "RenderMeshQueryMgr currentPos> MAX_QUERY_NUM");
		query = &queryList[currentPos];
		if(!query->m_pQuery)
		{
			query->m_pQuery = new GFXD3D9OcclusionQuery(GFX);
			AssertFatal(query->m_pQuery->initialize(), "RenderMeshQueryMgr m_pQuery->initialize error");
		}
		currentPos++;
		query->m_pQuery->begin();
		query->m_inst = ri;
		ri->obj->renderQuery();
		query->m_pQuery->end();
	}
	AssertFatal(mClearSB, "RenderInteriorMgr::render -- mClearSB cannot be NULL.");
	mClearSB->apply();
}
#else
void RenderMeshQueryMgr::render()
{
   // Early out if nothing to draw.
   if(!mElementList.size())
      return;

   PROFILE_SCOPE(RenderMeshQueryMgr);

   // Automagically save & restore our viewport and transforms.
   GFXTransformSaver saver;

   // set render states
   GFXCullMode mode = GFXCullNone;
   if( gClientSceneGraph->isReflectPass() )
   {
      mode = GFXCullCW;
   }
   else
   {
      mode = GFXCullCCW;
   }

   GFX->disableShaders();
   GFX->setZEnable( true );
   GFX->setZWriteEnable( false );
   GFX->setAlphaBlendEnable( false );
   GFX->enableColorWrites(false,false,false,false);
   GFX->setLightingEnable( false );
   GFX->setTexture(0,NULL);
   GFX->setTexture(1,NULL);

   queryStruct *query = NULL;
   U32 binSize = mElementList.size();
   for( U32 j=0; j<binSize; j++)
   {
      RenderInst *ri = mElementList[j].inst;

	  AssertFatal(currentPos <MAX_QUERY_NUM, "RenderMeshQueryMgr currentPos> MAX_QUERY_NUM");

	  query = &queryList[currentPos];
	  if(!query->m_pQuery)
	  {
		  query->m_pQuery = new GFXD3D9OcclusionQuery(GFX);
		  AssertFatal(query->m_pQuery->initialize(), "RenderMeshQueryMgr m_pQuery->initialize error");
	  }
	  currentPos++;

	  query->m_pQuery->begin();
	  query->m_inst = ri;

	  ri->obj->renderQuery();

	  query->m_pQuery->end();
   }

   GFX->enableColorWrites(true,true,true,true);
   GFX->setZWriteEnable( true );
   GFX->setAlphaBlendEnable( true );
}

#endif
void RenderMeshQueryMgr::query()
{
	int querys;
	int pixelNum;

	querys = 0;

	queryStruct *item = NULL;
	for(int i=0;i<currentPos;i++)
	{
		item = &queryList[i];
		pixelNum = item->m_pQuery->query();
		item->m_inst->obj->setQueryResult(pixelNum);
		querys++;
	}

#ifdef DEBUG
	gClientSceneGraph->setQueryTimes(querys);
#endif
}
void RenderMeshQueryMgr::resetStateBlock()
{
	//mCullCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCW);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSColorWriteEnable, 0);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->endStateBlock(mCullCWSB);
	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSColorWriteEnable, 0);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->endStateBlock(mCullCCWSB);
	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSColorWriteEnable, GFXCOLORWRITEENABLE_RED|GFXCOLORWRITEENABLE_GREEN|GFXCOLORWRITEENABLE_BLUE|GFXCOLORWRITEENABLE_ALPHA);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mClearSB);
}
void RenderMeshQueryMgr::releaseStateBlock()
{
	if (mCullCWSB)
	{
		mCullCWSB->release();
	}
	if (mCullCCWSB)
	{
		mCullCCWSB->release();
	}
	if (mClearSB)
	{
		mClearSB->release();
	}
}
void RenderMeshQueryMgr::init()
{
	if (mCullCWSB == NULL)
	{
		mCullCWSB = new GFXD3D9StateBlock;
		mCullCWSB->registerResourceWithDevice(GFX);
		mCullCWSB->mZombify = &releaseStateBlock;
		mCullCWSB->mResurrect = &resetStateBlock;
		mCullCCWSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}
void RenderMeshQueryMgr::shutdown()
{
	SAFE_DELETE(mCullCWSB);
	SAFE_DELETE(mCullCCWSB);
	SAFE_DELETE(mClearSB);
}
