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
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();
	//
	//�豸����ʱ����
	//
	static void resetStateBlock();	
};




#endif
