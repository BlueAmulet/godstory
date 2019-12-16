//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _DGUIOBJECTICON_H_
#define _DGUIOBJECTICON_H_
#include "gui/core/guiControl.h"
#include "gfx/gfxTextureHandle.h"

class GuiRichTextCtrl;
class GFXStateBlock;

class dGuiObjectIcon : public GuiControl
{
	typedef GuiControl Parent;
public:
	enum
	{
		OBJECTICON_ITEM = 1,
		OBJECTICON_SKILL = 2,
		OBJECTICON_STATUS = 3,
		OBJECTICON_LIVINGSKILL = 4,
	};

	dGuiObjectIcon();
	~dGuiObjectIcon();

	DECLARE_CONOBJECT(dGuiObjectIcon);
	static void initPersistFields();

	bool resize(const Point2I &newPosition, const Point2I &newExtent);
	bool onWake();
	void onSleep();

#ifdef NTJ_CLIENT
	void onMouseEnter(const GuiEvent &event);
	void onMouseLeave(const GuiEvent &event);

	inline void setSlotType(S32 type)	{ mObjectType = type;}
	inline void setSlotCol(S32 row)		{ mObjectID = row;}
	void updateObject();
	void drawIconNum(Point2I offset, const RectI &updateRect);
	void setObject(S32 type, S32 id) { mObjectType = type; mObjectID = id; m_DirtyFlag = true;}

#endif
	void onPreRender();
	void onRender(Point2I offset, const RectI &updateRect);
	void resizeBoundary(const RectI& bound, const Point2I& oldsize, const Point2I& newsize);

protected:

	S32					mObjectType;							//ICON对象类型
	S32					mObjectID;								//ICON对象ID
	RectI				mBoundary;								//范围
	GFXTexHandle		mTexture;								//ICON贴图

	//S32				mNum[3];								//ICON上显示数量(百位、十位和个位）
	//GFXTexHandle		mNumTexture;							//ICON数字贴图
	//bool				mIsShowNum;								//是否显示数字

	GuiRichTextCtrl*	mTextCtrl;								//Slot上热感控件
private:
	static GFXStateBlock* mSetCoolSB;
	static GFXStateBlock* mClearCoolSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};
#endif//_DGUIOBJECTICON_H_