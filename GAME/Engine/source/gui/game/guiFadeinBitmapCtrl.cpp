//-----------------------------------------------------------------------------
// PowerEngine
//
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "guiFadeinBitmapCtrl.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gui/controls/guiBitmapCtrl.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
GFXStateBlock* GuiFadeinBitmapCtrl::mSetSB = NULL;

IMPLEMENT_CONOBJECT(GuiFadeinBitmapCtrl);

void GuiFadeinBitmapCtrl::onRender(Point2I offset, const RectI &updateRect)
{
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "GuiFadeinBitmapCtrl::onRender -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setTextureStageMagFilter( 0, GFXTextureFilterLinear );
	GFX->setTextureStageMinFilter( 0, GFXTextureFilterLinear );
#endif


	Parent::onRender(offset, updateRect);
	U32 elapsed = Platform::getRealMilliseconds() - wakeTime;

	U32 alpha;
	if (elapsed < fadeinTime)
	{
		// fade-in
		alpha = (U32)(255.0f * (1.0f - (F32(elapsed) / F32(fadeinTime))));
	}
	else if (elapsed < (fadeinTime+waitTime))
	{
		// wait
		alpha = 0;
	}
	else if (elapsed < (fadeinTime+waitTime+fadeoutTime))
	{
		// fade out
		elapsed -= (fadeinTime+waitTime);
		alpha = (U32)(255.0f * F32(elapsed) / F32(fadeoutTime));
	}
	else
	{
		// done state
		alpha = fadeoutTime ? 255 : 0;
		done = true;
	}
	ColorI color(0,0,0,alpha);
	GFX->getDrawUtil()->drawRectFill( offset, getExtent() + offset, color );
}


void GuiFadeinBitmapCtrl::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->endStateBlock(mSetSB);
}


void GuiFadeinBitmapCtrl::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void GuiFadeinBitmapCtrl::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void GuiFadeinBitmapCtrl::shutdown()
{
	SAFE_DELETE(mSetSB);
}



















