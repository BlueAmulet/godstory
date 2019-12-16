//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "guiChunkedBitmapCtrl.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gfx/gBitmap.h"
//#include "gui/core/guiControl.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxTextureHandle.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
GFXStateBlock* GuiChunkedBitmapCtrl::mStoreSB = NULL;
GFXStateBlock* GuiChunkedBitmapCtrl::mSetSB = NULL;

//class GuiChunkedBitmapCtrl : public GuiControl
//{
//private:
//	typedef GuiControl Parent;
//	void renderRegion(const Point2I &offset, const Point2I &extent);
//
//protected:
//	StringTableEntry mBitmapName;
//	GFXTexHandle mTexHandle;
//	bool  mUseVariable;
//	bool  mTile;
//
//public:
//	//creation methods
//	DECLARE_CONOBJECT(GuiChunkedBitmapCtrl);
//	GuiChunkedBitmapCtrl();
//	static void initPersistFields();
//
//	//Parental methods
//	bool onWake();
//	void onSleep();
//
//	void setBitmap(const char *name);
//
//	void onRender(Point2I offset, const RectI &updateRect);
//};

IMPLEMENT_CONOBJECT(GuiChunkedBitmapCtrl);

void GuiChunkedBitmapCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("GuiChunkedBitmapCtrl");		
	addField( "bitmap",        TypeFilename,  Offset( mBitmapName, GuiChunkedBitmapCtrl ) );
	addField( "useVariable",   TypeBool,      Offset( mUseVariable, GuiChunkedBitmapCtrl ) );
	addField( "tile",          TypeBool,      Offset( mTile, GuiChunkedBitmapCtrl ) );
	endGroup("GuiChunkedBitmapCtrl");		
}

ConsoleMethod( GuiChunkedBitmapCtrl, setBitmap, void, 3, 3, "(string filename)"
			  "Set the bitmap contained in this control.")
{
	object->setBitmap( argv[2] );
}

GuiChunkedBitmapCtrl::GuiChunkedBitmapCtrl()
{
	mBitmapName = StringTable->insert("");
	mUseVariable = false;
	mTile = false;
}

void GuiChunkedBitmapCtrl::setBitmap(const char *name)
{
	bool awake = mAwake;
	if(awake)
		onSleep();

	mBitmapName = StringTable->insert(name);
	if(awake)
		onWake();
	setUpdate();
}

bool GuiChunkedBitmapCtrl::onWake()
{
	if(!Parent::onWake())
		return false;

	if( !mTexHandle )
	{
		if ( mUseVariable )
			mTexHandle.set( Con::getVariable( mConsoleVariable ), &GFXDefaultGUIProfile );
		else
			mTexHandle.set( mBitmapName, &GFXDefaultGUIProfile );
	}


	return true;
}

void GuiChunkedBitmapCtrl::onSleep()
{
	mTexHandle = NULL;
	Parent::onSleep();
}

void GuiChunkedBitmapCtrl::renderRegion(const Point2I &offset, const Point2I &extent)
{
	/*
	U32 widthCount = mTexHandle.getTextureCountWidth();
	U32 heightCount = mTexHandle.getTextureCountHeight();
	if(!widthCount || !heightCount)
	return;

	F32 widthScale = F32(extent.x) / F32(mTexHandle.getWidth());
	F32 heightScale = F32(extent.y) / F32(mTexHandle.getHeight());
	GFX->setBitmapModulation(ColorF(1,1,1));
	for(U32 i = 0; i < widthCount; i++)
	{
	for(U32 j = 0; j < heightCount; j++)
	{
	GFXTexHandle t = mTexHandle.getSubTexture(i, j);
	RectI stretchRegion;
	stretchRegion.point.x = (S32)(i * 256 * widthScale  + offset.x);
	stretchRegion.point.y = (S32)(j * 256 * heightScale + offset.y);
	if(i == widthCount - 1)
	stretchRegion.extent.x = extent.x + offset.x - stretchRegion.point.x;
	else
	stretchRegion.extent.x = (S32)((i * 256 + t.getWidth() ) * widthScale  + offset.x - stretchRegion.point.x);
	if(j == heightCount - 1)
	stretchRegion.extent.y = extent.y + offset.y - stretchRegion.point.y;
	else
	stretchRegion.extent.y = (S32)((j * 256 + t.getHeight()) * heightScale + offset.y - stretchRegion.point.y);
	GFX->drawBitmapStretch(t, stretchRegion);
	}
	}
	*/
}


void GuiChunkedBitmapCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;

	if( mTexHandle )
	{
#ifdef STATEBLOCK
		AssertFatal(mStoreSB, "GuiChunkedBitmapCtrl::onRender -- mStoreSB cannot be NULL.");
		mStoreSB->capture();
		AssertFatal(mSetSB, "GuiChunkedBitmapCtrl::onRender -- mSetSB cannot be NULL.");
		mSetSB->apply();
		RectI boundsRect( offset, getExtent());
		GFX->getDrawUtil()->drawBitmapStretch( mTexHandle, boundsRect );
		//AssertFatal(mStoreSB, "GuiChunkedBitmapCtrl::onRender -- mStoreSB cannot be NULL.");
		mStoreSB->apply();
#else
		const U32 magFilt = GFX->getSamplerState(0, GFXSAMPMagFilter);
		const U32 minFilt = GFX->getSamplerState(0, GFXSAMPMinFilter);

		GFX->setTextureStageMagFilter( 0, GFXTextureFilterLinear );
		GFX->setTextureStageMinFilter( 0, GFXTextureFilterLinear );

		RectI boundsRect( offset, getExtent());
		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapStretch( mTexHandle, boundsRect );

		GFX->setTextureStageMagFilter( 0, (GFXTextureFilterType)magFilt );
		GFX->setTextureStageMinFilter( 0, (GFXTextureFilterType)minFilt );
#endif


		/*
		if(mTexHandle)
		{
		if (mTile)
		{
		int stepy = 0;
		for(int y = 0; offset.y + stepy < getHeight(); stepy += mTexHandle.getHeight())
		{
		int stepx = 0;
		for(int x = 0; offset.x + stepx < getWidth(); stepx += mTexHandle.getWidth())
		renderRegion(Point2I(offset.x+stepx, offset.y+stepy), Point2I(mTexHandle.getWidth(), mTexHandle.getHeight()) );
		}
		}
		else
		renderRegion(offset, getExtent());
		*/
	}

	GuiControl *ctrl = static_cast<GuiChunkedBitmapCtrl*>(this);
	if(ctrl && ctrl->mVisible)//<Edit>:thinking
		renderChildControls(offset, updateRect);

	/*
	}
	else
	Parent::onRender(offset, updateRect);
	*/
}


void GuiChunkedBitmapCtrl::resetStateBlock()
{
	//mStoreSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->endStateBlock(mStoreSB);

	//mSetSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->endStateBlock(mSetSB);
}


void GuiChunkedBitmapCtrl::releaseStateBlock()
{
	if (mStoreSB)
	{
		mStoreSB->release();
	}

	if (mSetSB)
	{
		mSetSB->release();
	}
}

void GuiChunkedBitmapCtrl::init()
{
	if (mStoreSB == NULL)
	{
		mStoreSB = new GFXD3D9StateBlock;
		mStoreSB->registerResourceWithDevice(GFX);
		mStoreSB->mZombify = &releaseStateBlock;
		mStoreSB->mResurrect = &resetStateBlock;

		mSetSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void GuiChunkedBitmapCtrl::shutdown()
{
	SAFE_DELETE(mStoreSB);
	SAFE_DELETE(mSetSB);
}