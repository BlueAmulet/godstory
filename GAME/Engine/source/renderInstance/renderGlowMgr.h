//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _RENDER_GLOW_MGR_H_
#define _RENDER_GLOW_MGR_H_

#include "renderElemMgr.h"

class GFXStateBlock;
//**************************************************************************
// RenderGlowMgr
//**************************************************************************
class RenderGlowMgr : public RenderElemMgr
{
protected:
   virtual void setupSGData( RenderInst *ri, SceneGraphData &data );
public:
   typedef RenderElemMgr Parent;
   virtual void render();
private:
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mClearSB;
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
