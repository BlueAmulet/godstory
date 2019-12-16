#ifndef __GUIDAMAGEHUD_H__
#define __GUIDAMAGEHUD_H__

#pragma once
#include "gui/core/guiControl.h"
#include "gui/shiny/guiTickCtrl.h"
#include "gfx/gfxTextureHandle.h"

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

class GuiDamageHud : public GuiTickCtrl
{
private:
	typedef GuiTickCtrl Parent;
public:
	enum
	{
		FADE_NONE			= 0,		// 无淡入淡出
		FADE_IN				= 1,		// 淡入
		FADE_OUT			= 2,		// 淡出
		FADE_ALL			= 3,		// 淡入淡出

		STATE_NONE			= 0,		// 无状态
		STATE_ENTER			= 1,		// 刚进入
		STATE_STEP			= 2,		// 正在进行中
		STATE_LEAVE			= 3,		// 退出

		SHOWSTATE_GREENNUM	= 0,		// 显示数字绿
		SHOWSTATE_REDNUM	= 1,		// 显示数字红
		SHOWSTATE_YELLOWNUM	= 2,		// 显示数字黄
		SHOWSTATE_BLUENUM	= 3,		// 显示数字蓝
		SHOWSTATE_MISS		= 5,		// 显示MISS
		SHOWSTATE_CRITICAL	= 6,		// 显示暴击
		SHOWSTATE_ABSORB	= 7,		// 显示吸收
		SHOWSTATE_RESIST	= 8,		// 显示抵御
		SHOWSTATE_REBOUND	= 9,		// 显示反弹
		SHOWSTATE_REFLECT	= 10,		// 显示反射
		SHOWSTATE_IMMUNITY	= 11,		// 显示免疫

		NUM_LENGTH			= 32,
		NUM_TYPE			= 4,
		NUM_MAX				= 10,
	};

protected:
	U32		mState;				// 当前状态
	U32		mShowType;			// 显示类型
	U32		mFadeType;			// 淡入淡出类型

	U32		mLifeTime;			// 淡入后至淡出前的持续时间
	U32		mFadeInTime;		// 淡入限制时长
	U32		mFadeOutTime;		// 淡出限制时长

	F32		mMoveSpeed;
	F32		mMoveSpeedNormal;
	F32		mAddSpeedRate;
	F32		mSpeedEnter;

	U32		mLastTime;			// 上次更新时间
	U32		mTextLen;			// 数字文本长度
	S32		mWordPadding;		// 数字字距
	ColorF	mColorAlpha;		// 淡入淡出颜色
	F32		mAlphaStep;			// 淡入淡出alpha增减步进值

	char	mDamageNumName[NUM_LENGTH];

public:
	static GFXTexHandle		mNumTexture[NUM_TYPE];
	static Vector<RectI>	mNumBitmapArrayRects[NUM_TYPE];

	static GFXTexHandle		mMissTexture;
	static GFXTexHandle		mCriticalTexture;
	static GFXTexHandle		mAbsorbTexture;
	static GFXTexHandle		mResistTexture;
	static GFXTexHandle		mReboundTexture;
	static GFXTexHandle		mReflectTexture;
	static GFXTexHandle		mImmunityTexture;

	static RectI getBitmapArrayRect(U32 ShowType, U32 i);
	static S32	constructBitmapArray(GFXTexHandle& mTextureObject, U32 ShowType);

public:
	GuiDamageHud();

	void SetDrawText(U32 ShowType, const char* Number);
	void ClearText();
	inline bool IsWorking() { return mVisible && mState != STATE_NONE;}

public:
	void onRender(Point2I offset, const RectI &updateRect);
	bool onWake();
	void onSleep();

protected:
	void interpolateTick( F32 delta );
	void processTick();
	void advanceTime( F32 timeDelta );

public:
	static void Initilize();
	static void Destroy();
	static void initPersistFields();
	DECLARE_CONOBJECT(GuiDamageHud);
};

#endif//__GUIDAMAGEHUD_H__