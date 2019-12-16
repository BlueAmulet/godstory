//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _RENDER_INTERIOR_MGR_H_
#define _RENDER_INTERIOR_MGR_H_

#include "renderElemMgr.h"

class GFXStateBlock;
//**************************************************************************
// RenderInteriorMgr
//**************************************************************************
class RenderInteriorMgr : public RenderElemMgr
{
protected:
   void setupSGData( RenderInst *ri, SceneGraphData &data );
public:
   typedef RenderElemMgr Parent;
   virtual void render();
   virtual void addElement( RenderInst *inst );
private:
	static GFXStateBlock* mCullCWSB;
	static GFXStateBlock* mCullCCWSB;
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
