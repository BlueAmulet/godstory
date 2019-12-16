//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderGlowMgr.h"
#include "materials/sceneData.h"
#include "sceneGraph/sceneGraph.h"
#include "materials/matInstance.h"
#include "T3D/fx/glowBuffer.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* RenderGlowMgr::mSetSB = NULL;
GFXStateBlock* RenderGlowMgr::mClearSB = NULL;

//**************************************************************************
// RenderGlowMgr
//**************************************************************************

//-----------------------------------------------------------------------------
// setup scenegraph data
//-----------------------------------------------------------------------------
void RenderGlowMgr::setupSGData( RenderInst *ri, SceneGraphData &data )
{
   Parent::setupSGData(ri, data);
   data.glowPass = true;
}


//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
void RenderGlowMgr::render()
{
#ifdef _XBOX
   return; // HACK -patw
#endif

   // Early out if nothing to draw.
   if(!mElementList.size())
      return;

   // CodeReview - This is pretty hackish. - AlexS 4/19/07
   if(GFX->getPixelShaderVersion() < 0.001f)
      return;

   GlowBuffer *glowBuffer = gClientSceneGraph->getGlowBuff();
   if( !glowBuffer || glowBuffer->isDisabled() | !mElementList.size() ) return;

   PROFILE_SCOPE(RenderGlowMgrRender);

   RectI vp = GFX->getViewport();

   GFX->pushActiveRenderTarget();
   glowBuffer->setAsRenderTarget();

   GFX->pushWorldMatrix();
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "RenderGlowMgr::render -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
   // set render states
   GFX->setCullMode( GFXCullCCW );
   GFX->setZWriteEnable( false );

   for( U32 i=0; i<GFX->getNumSamplers(); i++ )
   {
      GFX->setTextureStageAddressModeU( i, GFXAddressWrap );
      GFX->setTextureStageAddressModeV( i, GFXAddressWrap );

      GFX->setTextureStageMagFilter( i, GFXTextureFilterLinear );
      GFX->setTextureStageMinFilter( i, GFXTextureFilterLinear );
      GFX->setTextureStageMipFilter( i, GFXTextureFilterLinear );
   }

#endif
   // init loop data
   SceneGraphData sgData;
   U32 binSize = mElementList.size();

   for( U32 j=0; j<binSize; )
   {
      RenderInst *ri = mElementList[j].inst;

      // temp fix - these shouldn't submit glow ri's...
      if(ri->dynamicLight)
      {
         j++;
         continue;
      }

      setupSGData( ri, sgData );
      MatInstance *mat = ri->matInst;
      if( !mat )
      {
         mat = gRenderInstManager.getWarningMat();
      }
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

            // set buffers if changed
			   if(passRI->primBuff)
			   {
               // draw it
               GFX->drawPrimitive( passRI->primBuffIndex );
			   }
         }
         matListEnd = a;

      }

      // force increment if none happened, otherwise go to end of batch
      j = ( j == matListEnd ) ? j+1 : matListEnd;

   }

   // restore render states, copy to screen
#ifdef STATEBLOCK
	AssertFatal(mClearSB, "RenderGlowMgr::render -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
   GFX->setZWriteEnable( true );
#endif
   GFX->popActiveRenderTarget();
   glowBuffer->copyToScreen( vp ); // Hacking this out, it's not working -patw

   GFX->popWorldMatrix();
}

void RenderGlowMgr::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	for( U32 i=0; i<GFX->getNumSamplers(); i++ )
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
	}
	GFX->endStateBlock(mSetSB);
	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->endStateBlock(mClearSB);
}
void RenderGlowMgr::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
	if (mClearSB)
	{
		mClearSB->release();
	}
}
void RenderGlowMgr::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;
		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}
void RenderGlowMgr::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mClearSB);
}
