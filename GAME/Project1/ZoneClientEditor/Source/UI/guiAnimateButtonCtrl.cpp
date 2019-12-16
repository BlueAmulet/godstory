//-----------------------------------------------------------------------------
// PowerEngine
//
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiDefaultControlRender.h"
#include "ui/guiAnimateButtonCtrl.h"

IMPLEMENT_CONOBJECT(GuiAnimateButtonCtrl);

//-------------------------------------
GuiAnimateButtonCtrl::GuiAnimateButtonCtrl()
{
	mBitmapName = StringTable->insert("");
	mFrameRate = 0;
	mFrame = 0;
	mLastTime = 0;
	setExtent(140, 30);
}

GuiAnimateButtonCtrl::~GuiAnimateButtonCtrl()
{
	mTexture = NULL;
	mTextureRects.clear();
}

//-------------------------------------
void GuiAnimateButtonCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("bitmap", TypeFilename, Offset(mBitmapName, GuiAnimateButtonCtrl));
	addField("framerate", TypeS32, Offset(mFrameRate, GuiAnimateButtonCtrl));
}

//-------------------------------------
bool GuiAnimateButtonCtrl::onWake()
{
	if (! Parent::onWake())
		return false;
	setActive(true);
	setBitmap(mBitmapName);
	setFrameRate(mFrameRate);
	return true;
}

//-------------------------------------
void GuiAnimateButtonCtrl::onSleep()
{
	mTexture = NULL;
	mTextureRects.clear();
	Parent::onSleep();
}

//-------------------------------------
void GuiAnimateButtonCtrl::inspectPostApply()
{
	Parent::inspectPostApply();
	if ((getWidth() == 0) && (getHeight() == 0) && mTexture)
	{
		setExtent( mTexture->getWidth(), mTexture->getHeight());
	}
}

//-------------------------------------
void GuiAnimateButtonCtrl::setBitmap(StringTableEntry name)
{
	if(!name || *name == 0)
		return;

	mBitmapName = StringTable->insert(name);
	if(!isAwake())
		return;

	mTexture = GFXTexHandle(mBitmapName, &GFXDefaultPersistentProfile, true);
	if (mTexture.isNull())
		Con::warnf("GFXTextureManager::createTexture - Unable to load Texture: %s", mBitmapName);
	constructBitmapArray();
	mFrame = mLastTime = 0;
	setUpdate();
}


//-------------------------------------
void GuiAnimateButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	if(!isVisible())
		return;
	if(mTextureRects.empty())
	{
		Parent::onRender(offset, updateRect);
		return;
	}

	U32 currentTime = Platform::getVirtualMilliseconds();
	if(currentTime - mLastTime >= mFrameRate)
	{
		if(mFrame < mTextureRects.size() - 1)
			mFrame++;
		else
			mFrame = 0;
		mLastTime = currentTime;
		m_DirtyFlag = true;
	}

	RectI rect(offset, getExtent());
	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->drawBitmapStretchSR(mTexture, rect, mTextureRects[mFrame]);
	renderChildControls( offset, updateRect);
	m_DirtyFlag = false;
}

// ----------------------------------------------------------------------------
// 获取动画每帧图片尺寸大小
RectI GuiAnimateButtonCtrl::getBitmapArrayRect(U32 index)
{
	if(index >= mTextureRects.size())
		return RectI(0,0,0,0);

	return mTextureRects[index];
}

// ----------------------------------------------------------------------------
// 获取动画文件各帧图片尺寸位置
S32 GuiAnimateButtonCtrl::constructBitmapArray()
{
	if( mTexture.isNull() )
		return 0;

	if(mTextureRects.size())
		return mTextureRects.size();	

	GBitmap *bmp = mTexture->getBitmap();
	ColorI sepColor;
	if ( !bmp || !bmp->getColor( 0, 0, sepColor ) )
	{
		return 0;
	}

	S32 curY = 0;
	ColorI color;
	mTextureRects.clear();
	while(curY < bmp->getHeight())
	{
		// skip any sep colors
		bmp->getColor( 0, curY, color);
		if(color == sepColor)
		{
			curY++;
			continue;
		}
		// ok, process left to right, grabbing bitmaps as we go...
		S32 curX = 0;
		while(curX < bmp->getWidth())
		{
			bmp->getColor(curX, curY, color);
			if(color == sepColor)
			{
				curX++;
				continue;
			}
			S32 startX = curX;
			while(curX < bmp->getWidth())
			{
				bmp->getColor(curX, curY, color);
				if(color == sepColor)
					break;
				curX++;
			}
			S32 stepY = curY;
			while(stepY < bmp->getHeight())
			{
				bmp->getColor(startX, stepY, color);
				if(color == sepColor)
					break;
				stepY++;
			}
			mTextureRects.push_back(RectI(startX, curY, curX - startX, stepY - curY));
		}
		// ok, now skip to the next separation color on column 0
		while(curY < bmp->getHeight())
		{
			bmp->getColor(0, curY, color);
			if(color == sepColor)
				break;
			curY++;
		}
	}
	return mTextureRects.size();
}

ConsoleMethod( GuiAnimateButtonCtrl, setBitmap, void, 3, 3, "setBitmap(%bitmapname)")
{
	object->setBitmap(argv[2]);
}