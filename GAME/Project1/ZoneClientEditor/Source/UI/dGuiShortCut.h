//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _DGUISHORTCUT_H_
#define _DGUISHORTCUT_H_
#include "gui/core/guiControl.h"
#include "gfx/gfxTextureHandle.h"

#include <string>

class Res;
class GuiRichTextCtrl;
class SkillData;
class Player;
class ShortcutObject;
struct Stats;
class GFXStateBlock;
class CommonFontEX;

class dGuiShortCut : public GuiControl
{
	typedef GuiControl Parent;
public:
	enum ShortStat
	{
		SLOTSTAT_COMMON			= 0,		//��ͨ
		SLOTSTAT_MOUSEOVER		= 1,		//��꾭��
		SLOTSTAT_CHOOSE			= 2,		//ѡ��
		SLOTSTAT_FORBIDDEN		= 3,		//��ֹ����
		SLOTSTAT_COOLDOWN		= 4,		//��ȴ
	};

	dGuiShortCut();
	~dGuiShortCut();

	DECLARE_CONOBJECT(dGuiShortCut);
	static void initPersistFields();

	bool resize(const Point2I &newPosition, const Point2I &newExtent);
	bool onWake();
	void onSleep();

#ifdef NTJ_CLIENT
	void onMouseUp(const GuiEvent &event);
	void onMouseDragged(const GuiEvent &event);
	void onMouseDown(const GuiEvent &event);
	void onMouseMove(const GuiEvent &event);
	void onMouseEnter(const GuiEvent &event);
	void onMouseLeave(const GuiEvent &event);
	void onRightMouseUp(const GuiEvent &event);
	void onRightMouseDragged(const GuiEvent &event);
	void onRightMouseDown(const GuiEvent &event);
	enWarnMessage checkShortcutMove	(S32 old_type, S32 old_col);

	inline void setSlotType(S32 type)	{ mSlotType = type;}
	inline void setSlotCol(S32 row)		{ mSlotIndex = row;}
	inline bool getBeSelected()			{ return mIsEnableSelect;}
	void setBeSelected(bool IsSelected);
	void setEnableEffect(bool IsBorderEffect, U32 looptimes = 0);
	bool updateSlot();
	
	void drawMoveIn(Point2I offset, const RectI &updateRect);
	void drawBeSelected(Point2I offset, const RectI &updateRect);
	void drawBorderEffect(Point2I offset, const RectI &updateRect);
	void drawIconNum(Point2I offset, const RectI &updateRect);
	void drawCoolDown(Point2I offset, const RectI &updateRect);
	void drawForbided(Point2I offset, const RectI &updateRect);
	void drawFrame(Point2I offset, const RectI &updateRect);
	void drawCoolFlash(Point2I offset, const RectI &updateRect);

	void onShortCut();
	S32  getSkillID();
	void showHotInfo();
	
#endif
	void onPreRender();
	void onRender(Point2I offset, const RectI &updateRect);
	void resizeBoundary(const RectI& bound, const Point2I& oldsize, const Point2I& newsize);

protected:
	S32					mSlotType;								//����
	S32					mSlotIndex;								//��λ����
	RectI				mBoundary;								//��Χ	
	Point2I				mMousePos;

	StringTableEntry    mMouseOverCommand;						//��Ӧ����ƶ�ʱ�����Ľű�
	StringTableEntry    mMouseLeaveCommand;						//��Ӧ����뿪ʱ�����Ľű�
	StringTableEntry	mMouseDownCommand;						//��Ӧ��갴��ʱ�����Ľű�
	StringTableEntry	mRightMouseDownCommand;					//��Ӧ����Ҽ����´����Ľű�
	
	bool				mIsEnableSelect;						//�Ƿ�ʹ�ÿ�ѡ��
	bool				mIsEnableEffect;						//�Ƿ�ʹ�ñ߿���Ч
	bool				mIsDisableDrag;							//�Ƿ��ֹSlot�϶�
	bool			    mIsShowNum;								//�Ƿ���ʾ��Ʒ����
	bool				mIsMoveIn;								//�Ƿ��ƶ���Slot��
	bool				mIsForbided;							//�Ƿ��ֹ
	bool				mIsCool;								//�Ƿ���ȴ��ʽ
	bool				mIsRenderFrame;							//�Ƿ���ȾSlot�߿�
	bool				mIsShowCoolFlash;						//�Ƿ���Ⱦ��ȴ��������
	bool				mLastFrame;								//�Ƿ���Ⱦ���һ֡

	StringTableEntry	mIconTextureName;						//ԭICONͼƬ��
	GFXTexHandle		mTexture;								//ԭICON��ͼ	

	U32					mEffectPosX;							//�߿���Ч������ȾX����
	U32					mEffectPosY;							//�߿���Ч������ȾY����
	U32					mLastTime_Effect;						//�߿���Ч������һ����Ⱦʱ
	U32					mLoopEffects;							//��ʾ�߿���Ч����
	U32					mLoopTimes;								//������ʾ��Ч����

	U32					mCoolPosX;								//��ȴ����������ȾX����
	U32					mCoolPosY;								//��ȴ����������ȾY����
	U32					mLastTime_CoolFlash;					//��ȴ����������һ����Ⱦʱ
	
	S32					mNum[3];								//ICON����ʾ����(��λ��ʮλ�͸�λ��
	U32					mFrames;								//��ǰ��ȾSLOT�߿�����ֵ
	GuiRichTextCtrl*	mTextCtrl;								//Slot���ȸпؼ�
	GuiRichTextCtrl*	mCompareTextCtrl;						//Slot���ȸпؼ��Ա�
	ShortcutObject*		mOldSlot;								//��һ����Ⱦ��Slot����
	U32					mOldNum;								//��һ����Ⱦ��Slot�������Ʒ����	
private:
	static GFXTexHandle	  mNumTexture;							//ICON������ͼ
	static GFXTexHandle	  mSelectTexture;						//��������ͼ
	static GFXTexHandle	  mEffectTexture;						//Ч����ͼ
	static GFXTexHandle	  mCoolFlashTexture;					//��ȴ����������ͼƬ
	static CommonFontEX*  mCoolDownFont[3];						//��ȴ����

	static GFXStateBlock* mSetCoolSB;
	static GFXStateBlock* mClearCoolSB;	
public:
	static void init();
	static void shutdown();

	static void Initialize();
	static void Destory();

	static void releaseStateBlock();
	static void resetStateBlock();
};

extern std::string g_itemLink;
extern std::string g_itemLinkName;

#endif