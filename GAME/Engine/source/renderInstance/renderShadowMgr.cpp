//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderShadowMgr.h"
#include "sceneGraph/sceneObject.h"
#include "lightingSystem/common/shadowBase.h"

//-----------------------------------------------------------------------------
// addElement
//-----------------------------------------------------------------------------
void RenderShadowMgr::addElement( RenderInst *inst )
{
	mElementList.increment();
	MainSortElem &elem = mElementList.last();
	elem.inst = inst;
	elem.key = elem.key2 = 0;

	elem.key = (U32) inst->SortedIndex;

	// sort by material
	if( inst->matInst )
	{
		elem.key2 = (U32) inst->matInst->getMaterial();
	}
}

//-----------------------------------------------------------------------------
// sort
//-----------------------------------------------------------------------------
void RenderShadowMgr::sort()
{
	dQsort( mElementList.address(), mElementList.size(), sizeof(MainSortElem), cmpKeyFunc);
}

//-----------------------------------------------------------------------------
// QSort callback function
//-----------------------------------------------------------------------------
S32 FN_CDECL RenderShadowMgr::cmpKeyFunc(const void* p1, const void* p2)
{
	const MainSortElem* mse1 = (const MainSortElem*) p1;
	const MainSortElem* mse2 = (const MainSortElem*) p2;

	S32 test1 = S32(mse1->key) - S32(mse2->key);
	return ( test1 == 0 ) ? S32(mse1->key2) - S32(mse2->key2) : test1;
} 

//-----------------------------------------------------------------------------
// render objects
//-----------------------------------------------------------------------------
void RenderShadowMgr::render()
{
   // Early out if nothing to draw.
   if(!mElementList.size())
      return;

   PROFILE_START(RenderShadowMgrRender);

   ShadowBase *pShadow=NULL;
   ShadowBase *pLast = NULL;
   S32 index = -1,lastIndex = -1;;
   for( U32 i=0; i<mElementList.size(); i++ )
   {
      RenderInst *ri = mElementList[i].inst;
	  pShadow = (ShadowBase*)ri->obj;

	  if(ri->SortedIndex!=index)
	  {
		  index = ri->SortedIndex;
		  //if(pLast)
			 // pLast->clearRenderStatus(lastIndex);

		  pShadow->setRenderStatus(index);
		  pLast = pShadow;
		  lastIndex = index;
	  }
	  
	  pShadow->render(index);
   }

   //if(pShadow)
	  // pShadow->clearRenderStatus(lastIndex);

   PROFILE_END(RenderShadowMgrRender);

}