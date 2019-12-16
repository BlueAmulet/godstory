//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "renderShadowMeshQueryMgr.h"
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
//#define STATEBLOCK
GFXStateBlock* RenderShadowMeshQueryMgr::mCullCWSB = NULL;
GFXStateBlock* RenderShadowMeshQueryMgr::mCullCCWSB = NULL;
GFXStateBlock* RenderShadowMeshQueryMgr::mClearSB = NULL;

void RenderShadowMeshQueryMgr::clear()
{
	currentPos = 0;
	mElementList.clear();
}

RenderShadowMeshQueryMgr::~RenderShadowMeshQueryMgr()
{
	for(int i=0;i<MAX_QUERY_NUM;i++)
		SAFE_DELETE(queryList[i].m_pQuery);
}

void RenderShadowMeshQueryMgr::sort()
{
	dQsort( mElementList.address(), mElementList.size(), sizeof(MainSortElem), cmpTranslucentKeyFunc);
}

//-----------------------------------------------------------------------------
// QSort callback function
//-----------------------------------------------------------------------------
S32 FN_CDECL RenderShadowMeshQueryMgr::cmpTranslucentKeyFunc(const void* p1, const void* p2)
{
	const MainSortElem* mse1 = (const MainSortElem*) p1;
	const MainSortElem* mse2 = (const MainSortElem*) p2;

	return S32(mse2->key) - S32(mse1->key);  //µ¹Ðò
} 


//-----------------------------------------------------------------------------
// add element
//-----------------------------------------------------------------------------
void RenderShadowMeshQueryMgr::addElement( RenderInst *inst )
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
void RenderShadowMeshQueryMgr::render()
{
	// Early out if nothing to draw.
	if(!mElementList.size())
		return;

	PROFILE_SCOPE(RenderShadowMeshQueryMgr);

	// Automagically save & restore our viewport and transforms.
	GFXTransformSaver saver;

	// set render states
	if( gClientSceneGraph->isReflectPass() )
	{
		AssertFatal(mCullCWSB, "RenderShadowMeshQueryMgr::render -- mCullCWSB cannot be NULL.");
		mCullCWSB->apply();
	}
	else
	{
		AssertFatal(mCullCCWSB, "RenderShadowMeshQueryMgr::render -- mCullCCWSB cannot be NULL.");
		mCullCCWSB->apply();
	}

	GFX->disableShaders();
	GFX->setTexture(0,NULL);
	GFX->setTexture(1,NULL);

	queryShadowStruct *query = NULL;
	U32 binSize = mElementList.size();
	for( U32 j=0; j<binSize; j++)
	{
		RenderInst *ri = mElementList[j].inst;

		AssertFatal(currentPos <MAX_QUERY_NUM, "RenderShadowMeshQueryMgr currentPos> MAX_QUERY_NUM");

		query = &queryList[currentPos];
		if(!query->m_pQuery)
		{
			query->m_pQuery = new GFXD3D9OcclusionQuery(GFX);
			AssertFatal(query->m_pQuery->initialize(), "RenderShadowMeshQueryMgr m_pQuery->initialize error");
		}
		currentPos++;

		query->m_pQuery->begin();
		query->m_inst = ri;

		ri->obj->renderShadowQuery();

		query->m_pQuery->end();
	}
	AssertFatal(mClearSB, "RenderInteriorMgr::render -- mClearSB cannot be NULL.");
	mClearSB->apply();
}

#else
void RenderShadowMeshQueryMgr::render()
{
	// Early out if nothing to draw.
	if(!mElementList.size())
		return;

	PROFILE_SCOPE(RenderShadowMeshQueryMgr);

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

	F32 depthbias = -0.00005f;
	GFX->setZBias(*((U32 *)&depthbias));

	queryShadowStruct *query = NULL;
	U32 binSize = mElementList.size();
	for( U32 j=0; j<binSize; j++)
	{
		RenderInst *ri = mElementList[j].inst;

		AssertFatal(currentPos <MAX_QUERY_NUM, "RenderShadowMeshQueryMgr currentPos> MAX_QUERY_NUM");

		query = &queryList[currentPos];
		if(!query->m_pQuery)
		{
			query->m_pQuery = new GFXD3D9OcclusionQuery(GFX);
			AssertFatal(query->m_pQuery->initialize(), "RenderShadowMeshQueryMgr m_pQuery->initialize error");
		}
		currentPos++;

		query->m_pQuery->begin();
		query->m_inst = ri;

		ri->obj->renderShadowQuery();

		query->m_pQuery->end();
	}

	GFX->enableColorWrites(true,true,true,true);
	GFX->setZWriteEnable( true );
	GFX->setAlphaBlendEnable( true );
	depthbias = 0;
	GFX->setZBias(*((U32 *)&depthbias));
}

#endif

void RenderShadowMeshQueryMgr::query()
{
	int querys;
	int pixelNum;

	querys = 0;

	queryShadowStruct *item = NULL;
	for(int i=0;i<currentPos;i++)
	{
		item = &queryList[i];
		pixelNum = item->m_pQuery->query();
		item->m_inst->obj->setShadowQueryResult(pixelNum);
		querys++;
	}

#ifdef DEBUG
	gClientSceneGraph->setQueryTimes(querys);
#endif
}

void RenderShadowMeshQueryMgr::resetStateBlock()
{
	//mCullCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCW);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSColorWriteEnable, 0);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSDepthBias, -0.00005f);
	GFX->endStateBlock(mCullCWSB);
	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSColorWriteEnable, 0);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSDepthBias, -0.00005f);
	GFX->endStateBlock(mCullCCWSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSColorWriteEnable, GFXCOLORWRITEENABLE_RED|GFXCOLORWRITEENABLE_GREEN|GFXCOLORWRITEENABLE_BLUE|GFXCOLORWRITEENABLE_ALPHA);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSDepthBias, 0);
	GFX->endStateBlock(mClearSB);
}
void RenderShadowMeshQueryMgr::releaseStateBlock()
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
void RenderShadowMeshQueryMgr::init()
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
void RenderShadowMeshQueryMgr::shutdown()
{
	SAFE_DELETE(mCullCWSB);
	SAFE_DELETE(mCullCCWSB);
	SAFE_DELETE(mClearSB);
}
