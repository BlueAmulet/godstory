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
		FADE_NONE			= 0,		// �޵��뵭��
		FADE_IN				= 1,		// ����
		FADE_OUT			= 2,		// ����
		FADE_ALL			= 3,		// ���뵭��

		STATE_NONE			= 0,		// ��״̬
		STATE_ENTER			= 1,		// �ս���
		STATE_STEP			= 2,		// ���ڽ�����
		STATE_LEAVE			= 3,		// �˳�

		SHOWSTATE_GREENNUM	= 0,		// ��ʾ������
		SHOWSTATE_REDNUM	= 1,		// ��ʾ���ֺ�
		SHOWSTATE_YELLOWNUM	= 2,		// ��ʾ���ֻ�
		SHOWSTATE_BLUENUM	= 3,		// ��ʾ������
		SHOWSTATE_MISS		= 5,		// ��ʾMISS
		SHOWSTATE_CRITICAL	= 6,		// ��ʾ����
		SHOWSTATE_ABSORB	= 7,		// ��ʾ����
		SHOWSTATE_RESIST	= 8,		// ��ʾ����
		SHOWSTATE_REBOUND	= 9,		// ��ʾ����
		SHOWSTATE_REFLECT	= 10,		// ��ʾ����
		SHOWSTATE_IMMUNITY	= 11,		// ��ʾ����

		NUM_LENGTH			= 32,
		NUM_TYPE			= 4,
		NUM_MAX				= 10,
	};

protected:
	U32		mState;				// ��ǰ״̬
	U32		mShowType;			// ��ʾ����
	U32		mFadeType;			// ���뵭������

	U32		mLifeTime;			// �����������ǰ�ĳ���ʱ��
	U32		mFadeInTime;		// ��������ʱ��
	U32		mFadeOutTime;		// ��������ʱ��

	F32		mMoveSpeed;
	F32		mMoveSpeedNormal;
	F32		mAddSpeedRate;
	F32		mSpeedEnter;

	U32		mLastTime;			// �ϴθ���ʱ��
	U32		mTextLen;			// �����ı�����
	S32		mWordPadding;		// �����־�
	ColorF	mColorAlpha;		// ���뵭����ɫ
	F32		mAlphaStep;			// ���뵭��alpha��������ֵ

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