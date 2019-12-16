//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _RENDER_ELEM_MGR_H_
#include "renderElemMgr.h"
#endif

//**************************************************************************
// RenderObjectMgr
//**************************************************************************
class RenderShadowMgr : public RenderElemMgr
{
public:
	void addElement( RenderInst *inst );
    void sort();
    static S32 FN_CDECL cmpKeyFunc(const void* p1, const void* p2);
    void render();
};
