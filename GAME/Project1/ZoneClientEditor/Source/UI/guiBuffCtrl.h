#ifndef _GUIBUFFCTRL_H_
#define _GUIBUFFCTRL_H_

#ifndef _GUIBUTTONCTRL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GAMEOBJECT_H_
#include "Gameplay/GameObjects/GameObject.h"
#endif

class GuiBuffCtrl : public GuiControl
{
	typedef GuiControl Parent;
public:
	GuiBuffCtrl();
	DECLARE_CONOBJECT(GuiBuffCtrl);

	static void initPersistFields();

	RectI mBuffBoundary;
	RectI mRealBuffBoundary;
	Point2I mRemainTimeExtext;
	Point2I mAddendExtext;
	StringTableEntry mBuffFilePath;
	StringTableEntry mFrameName;
	char mBuffAddendNum[16][32];
	char mDeBuffAddendNum[16][32];	
	GFXTexHandle mBuffTexture[16];
	GFXTexHandle mDeBuffTexture[16];
	GFXTexHandle mFrameTexture;
	char cBuffRemainTime[16][32];
	char cDeBuffRemainTime[16][32];
	char cBuffRemainTimeBak[16][32];
	char cDeBuffRemainTimeBak[16][32];
	char mBuffFileName[16][256];
	char mDeBuffFileName[16][256];

	bool bHasTime;
	bool bIsNewline;
	U16 mObjType;
	U16 mTeammateY;
	GameObject* pObjectBuff;
	S32 mBuffNum;
	S32 mDeBuffNum;
	S32 mSpaceNum;
	S32 mTempChangeY;

	bool onWake();
	void onSleep();
	void onRender(Point2I offset, const RectI &updateRect);
	void onPreRender();

	bool isUpDate();
	void setChickOut(Point2I localPoint);
	
	void onMouseDown(const GuiEvent &event);
	void onMouseMove(const GuiEvent &event);
	void onMouseLeave(const GuiEvent &event);
	void onRightMouseDragged(const GuiEvent &event);
	void onRightMouseUp(const GuiEvent &event);
};

#endif