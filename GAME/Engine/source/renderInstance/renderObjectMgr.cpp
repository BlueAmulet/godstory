//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderObjectMgr.h"
#include "sceneGraph/sceneObject.h"
#include "sceneGraph/renderableSceneObject.h"

//-----------------------------------------------------------------------------
// render objects
//-----------------------------------------------------------------------------
void RenderObjectMgr::render()
{
   // Early out if nothing to draw.
   if(!mElementList.size())
      return;

   PROFILE_START(RenderObjectMgrRender);

   for( U32 i=0; i<mElementList.size(); i++ )
   {
      RenderInst *ri = mElementList[i].inst;
      ri->obj->renderObject( ri->state, ri );
   }

   PROFILE_END(RenderObjectMgrRender);

}