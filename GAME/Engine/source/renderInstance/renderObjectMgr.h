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
class RenderObjectMgr : public RenderElemMgr
{
   void sort(){}  // don't sort them
   void render();

};
