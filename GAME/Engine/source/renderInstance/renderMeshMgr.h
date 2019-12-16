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
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();

};

#endif
