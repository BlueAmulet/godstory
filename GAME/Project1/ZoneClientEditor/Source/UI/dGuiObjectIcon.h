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

	S32					mObjectType;							//ICON��������
	S32					mObjectID;								//ICON����ID
	RectI				mBoundary;								//��Χ
	GFXTexHandle		mTexture;								//ICON��ͼ

	//S32				mNum[3];								//ICON����ʾ����(��λ��ʮλ�͸�λ��
	//GFXTexHandle		mNumTexture;							//ICON������ͼ
	//bool				mIsShowNum;								//�Ƿ���ʾ����

	GuiRichTextCtrl*	mTextCtrl;								//Slot���ȸпؼ�
private:
	static GFXStateBlock* mSetCoolSB;
	static GFXStateBlock* mClearCoolSB;
public:
	static void init();
	static void shutdown();
	//
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();
};
#endif//_DGUIOBJECTICON_H_