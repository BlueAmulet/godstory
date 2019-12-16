#include "Gameplay/GameObjects/AnimationObject.h"
#include "gfx/gBitmap.h"
#include "gfx/gfxDrawUtil.h"
#include "console/consoleTypes.h"
#include "gui/core/guiTypes.h"

GFX_ImplementTextureProfile(GFXAniObjTextureProfile,
							GFXTextureProfile::DiffuseMap, 
							GFXTextureProfile::PreserveSize |
							GFXTextureProfile::Static |
							GFXTextureProfile::KeepBitmap |
							GFXTextureProfile::NoMipmap, 
							GFXTextureProfile::None);

IMPLEMENT_CONOBJECT(AnimationObject);

AnimationObject::AnimationObject()
{
	mTexture	= NULL;
	mFrame		= 0;
	mFrameRate	= 0;
	mLastTime	= 0;
	mLoop		= true;
}

AnimationObject::~AnimationObject()
{
	mTexture = NULL;
	mTextureRects.clear();
}

bool AnimationObject::onAdd()
{
	if(!Parent::onAdd())
		return false;
	return true;
}

// ----------------------------------------------------------------------------
// 设置动画纹理对象
void AnimationObject::setTexture(StringTableEntry textureName)
{
	mTexture = GFXTexHandle(textureName, &GFXAniObjTextureProfile);	
	constructBitmapArray();
	mLastTime = Platform::getVirtualMilliseconds();
}

// ----------------------------------------------------------------------------
// 渲染方法
void AnimationObject::onRender(const RectI& offsetRect)
{
	if(mTextureRects.empty())
		return;

	GFX->getDrawUtil()->drawBitmapStretchSR(mTexture, offsetRect, mTextureRects[mFrame]);
}

// ----------------------------------------------------------------------------
// Tick处理事件
void AnimationObject::processTick()
{
	if(mTextureRects.empty())
		return;
	U32 currentTime = Platform::getVirtualMilliseconds();
	if(currentTime - mLastTime >= mFrameRate)
	{
		if(mFrame < mTextureRects.size() - 1)
			mFrame++;
		else
		{
			if(mLoop) mFrame = 0;
		}
		mLastTime = currentTime;
	}
}

// ----------------------------------------------------------------------------
// 获取动画每帧图片尺寸大小
RectI AnimationObject::getBitmapArrayRect()
{
	if(mTextureRects.empty())
		return RectI(0,0,0,0);

	return mTextureRects[mFrame];
}

// ----------------------------------------------------------------------------
// 获取动画文件各帧图片尺寸位置
S32 AnimationObject::constructBitmapArray()
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

void AnimationObject::initPersistFields()
{
	Parent::initPersistFields();	
	addField("FrameRate",	TypeS32,		Offset(mFrameRate,	AnimationObject));
	addField("Loop",		TypeBool,		Offset(mLoop,		AnimationObject));
}

// ----------------------------------------------------------------------------
// 脚本方法：设置动画帧率(毫秒)
ConsoleMethod(AnimationObject, setFrameRate, void, 3,3,"setFrameRate(%rate)")
{
	object->setFrameRate(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 脚本方法:设置动画纹理文件名
ConsoleMethod(AnimationObject, setTexture, void, 3,3,"setTexture(%rate)")
{
	object->setTexture(argv[2]);
}