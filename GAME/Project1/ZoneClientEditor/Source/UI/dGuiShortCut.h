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
		SLOTSTAT_COMMON			= 0,		//普通
		SLOTSTAT_MOUSEOVER		= 1,		//鼠标经过
		SLOTSTAT_CHOOSE			= 2,		//选中
		SLOTSTAT_FORBIDDEN		= 3,		//禁止操作
		SLOTSTAT_COOLDOWN		= 4,		//冷却
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
	S32					mSlotType;								//类型
	S32					mSlotIndex;								//槽位索引
	RectI				mBoundary;								//范围	
	Point2I				mMousePos;

	StringTableEntry    mMouseOverCommand;						//响应鼠标移动时触发的脚本
	StringTableEntry    mMouseLeaveCommand;						//响应鼠标离开时触发的脚本
	StringTableEntry	mMouseDownCommand;						//响应鼠标按下时触发的脚本
	StringTableEntry	mRightMouseDownCommand;					//响应鼠标右键接下触发的脚本
	
	bool				mIsEnableSelect;						//是否使用可选中
	bool				mIsEnableEffect;						//是否使用边框特效
	bool				mIsDisableDrag;							//是否禁止Slot拖动
	bool			    mIsShowNum;								//是否显示物品数量
	bool				mIsMoveIn;								//是否移动到Slot上
	bool				mIsForbided;							//是否禁止
	bool				mIsCool;								//是否画冷却样式
	bool				mIsRenderFrame;							//是否渲染Slot边框
	bool				mIsShowCoolFlash;						//是否渲染冷却闪动动画
	bool				mLastFrame;								//是否渲染最后一帧

	StringTableEntry	mIconTextureName;						//原ICON图片名
	GFXTexHandle		mTexture;								//原ICON贴图	

	U32					mEffectPosX;							//边框特效动画渲染X坐标
	U32					mEffectPosY;							//边框特效动画渲染Y坐标
	U32					mLastTime_Effect;						//边框特效动画上一次渲染时
	U32					mLoopEffects;							//显示边框特效次数
	U32					mLoopTimes;								//限制显示特效次数

	U32					mCoolPosX;								//冷却闪动动画渲染X坐标
	U32					mCoolPosY;								//冷却闪动动画渲染Y坐标
	U32					mLastTime_CoolFlash;					//冷却闪动动画上一次渲染时
	
	S32					mNum[3];								//ICON上显示数量(百位、十位和个位）
	U32					mFrames;								//当前渲染SLOT边框索引值
	GuiRichTextCtrl*	mTextCtrl;								//Slot上热感控件
	GuiRichTextCtrl*	mCompareTextCtrl;						//Slot上热感控件对比
	ShortcutObject*		mOldSlot;								//上一次渲染的Slot对象
	U32					mOldNum;								//上一次渲染的Slot对象的物品数量	
private:
	static GFXTexHandle	  mNumTexture;							//ICON数字贴图
	static GFXTexHandle	  mSelectTexture;						//鼠标操作贴图
	static GFXTexHandle	  mEffectTexture;						//效果贴图
	static GFXTexHandle	  mCoolFlashTexture;					//冷却完闪动动画图片
	static CommonFontEX*  mCoolDownFont[3];						//冷却字体

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