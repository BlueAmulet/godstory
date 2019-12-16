//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _RENDER_FIRST_MGR_H_
#define _RENDER_FIRST_MGR_H_

#include "renderElemMgr.h"

class GFXStateBlock;
//**************************************************************************
// RenderMeshMgr
//**************************************************************************
class RenderFirstMgr : public RenderElemMgr
{
public:
   virtual void render();
};

#endif
