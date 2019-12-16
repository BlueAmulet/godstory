//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderRefractMgr.h"
#include "materials/sceneData.h"
#include "sceneGraph/sceneGraph.h"
#include "materials/matInstance.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* RenderRefractMgr::mCullCWSB = NULL;
GFXStateBlock* RenderRefractMgr::mCullCCWSB = NULL;
//**************************************************************************
// RenderRefractMgr
//**************************************************************************

//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
void RenderRefractMgr::render()
{
   // Early out if nothing to draw.
   if(!mElementList.size())
      return;

   PROFILE_SCOPE(RenderRefractMgrRender);

   GFX->pushWorldMatrix();
#ifdef STATEBLOCK

   if( gClientSceneGraph->isReflectPass() )
   {
		AssertFatal(mCullCWSB, "RenderRefractMgr::render -- mCullCWSB cannot be NULL.");
		mCullCWSB->apply();
	}
	else
	{
		AssertFatal(mCullCCWSB, "RenderRefractMgr::render -- mCullCCWSB cannot be NULL.");
		mCullCCWSB->apply();
	}
#else
	if( gClientSceneGraph->isReflectPass() )
	{
      GFX->setCullMode( GFXCullCW );
   }
   else
   {
      GFX->setCullMode( GFXCullCCW );
   }
#endif


   //-------------------------------------
   // copy sfx backbuffer
   //-------------------------------------
   // set ortho projection matrix
   MatrixF proj = GFX->getProjectionMatrix();
   MatrixF newMat(true);
   GFX->setProjectionMatrix( newMat );
   GFX->pushWorldMatrix();
   GFX->setWorldMatrix( newMat );

   GFX->copyBBToSfxBuff();

   // restore projection matrix
   GFX->setProjectionMatrix( proj );
   GFX->popWorldMatrix();
   //-------------------------------------

   // init loop data
   SceneGraphData sgData;
   U32 binSize = mElementList.size();

   for( U32 j=0; j<binSize; )
   {
      RenderInst *ri = mElementList[j].inst;

      setupSGData( ri, sgData );
      sgData.refractPass = true;
      MatInstance *mat = ri->matInst;

      U32 matListEnd = j;


      while( mat->setupPass( sgData ) )
      {
         U32 a;
         for( a=j; a<binSize; a++ )
         {
            RenderInst *passRI = mElementList[a].inst;

            if (newPassNeeded(mat, passRI))
               break;

            mat->setWorldXForm(*passRI->worldXform);
            mat->setObjectXForm(*passRI->objXform);
            mat->setEyePosition(*passRI->objXform, gRenderInstManager.getCamPos());
            mat->setBuffers(passRI->vertBuff, passRI->primBuff);

            // draw it
            GFX->drawPrimitive( passRI->primBuffIndex );
         }
         matListEnd = a;

      }

      // force increment if none happened, otherwise go to end of batch
      j = ( j == matListEnd ) ? j+1 : matListEnd;

   }


   GFX->popWorldMatrix();
}

void RenderRefractMgr::resetStateBlock()
{
	//mCullCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCW);
	GFX->endStateBlock(mCullCWSB);
	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->endStateBlock(mCullCCWSB);
}

void RenderRefractMgr::releaseStateBlock()
{
	if (mCullCWSB)
	{
		mCullCWSB->release();
	}
	if (mCullCCWSB)
	{
		mCullCCWSB->release();
	}
}
void RenderRefractMgr::init()
{
	if (mCullCWSB == NULL)
	{
		mCullCWSB = new GFXD3D9StateBlock;
		mCullCWSB->registerResourceWithDevice(GFX);
		mCullCWSB->mZombify = &releaseStateBlock;
		mCullCWSB->mResurrect = &resetStateBlock;
		mCullCCWSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}
void RenderRefractMgr::shutdown()
{
	SAFE_DELETE(mCullCWSB);
	SAFE_DELETE(mCullCCWSB);
}
