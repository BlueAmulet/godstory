//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _RENDER_REFRACT_MGR_H_
#define _RENDER_REFRACT_MGR_H_

#include "renderElemMgr.h"
class GFXStateBlock;
//**************************************************************************
// RenderRefractionMgr
//**************************************************************************
class RenderRefractMgr : public RenderElemMgr
{
public:
   virtual void render();
private:
	static GFXStateBlock* mCullCWSB;
	static GFXStateBlock* mCullCCWSB;
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
