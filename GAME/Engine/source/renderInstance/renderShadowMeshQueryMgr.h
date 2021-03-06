//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _RENDER_SHADOW_MESH_QUERY_MGR_H_
#define _RENDER_SHADOW_MESH_QUERY_MGR_H_

#include "RenderTranslucentMgr.h"

//**************************************************************************
// RenderMeshMgr
//**************************************************************************
class GFXD3D9OcclusionQuery ;
struct RenderInst;
class GFXStateBlock;

#define MAX_QUERY_NUM 800

struct queryShadowStruct
{
	GFXD3D9OcclusionQuery *			m_pQuery;
	RenderInst *					m_inst;
};

class RenderShadowMeshQueryMgr : public RenderTranslucentMgr
{
	int currentPos;
	queryShadowStruct queryList[MAX_QUERY_NUM];

protected:
	void sort();
	void addElement( RenderInst *inst );

	static S32 FN_CDECL cmpTranslucentKeyFunc(const void* p1, const void* p2);
public:
	RenderShadowMeshQueryMgr()
	{
		currentPos = 0;
		memset(queryList,NULL,sizeof(queryList));
	}
	~RenderShadowMeshQueryMgr();


	virtual void clear();
	virtual void render();

	void query();
private:
	static GFXStateBlock* mCullCWSB;
	static GFXStateBlock* mCullCCWSB;
	static GFXStateBlock* mClearSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};




#endif
