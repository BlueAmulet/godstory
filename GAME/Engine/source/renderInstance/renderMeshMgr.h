//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _RENDER_MESH_MGR_H_
#define _RENDER_MESH_MGR_H_

#include "renderElemMgr.h"

class GFXStateBlock;
//**************************************************************************
// RenderMeshMgr
//**************************************************************************
class RenderMeshMgr : public RenderElemMgr
{
public:
   virtual void render();
private:
	static GFXStateBlock* mCullStoreSB;
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mFirstSB;
	static GFXStateBlock* mSecondSB;
	static GFXStateBlock* mDefaultSB;
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
