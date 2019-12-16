//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef __GUICLOCKCTRL_H__
#define __GUICLOCKCTRL_H__

#ifndef _GFONT_H_
#include "gfx/gFont.h"
#endif

#ifndef _GUICONTROL_H_
#include "gui/shiny/guiTickCtrl.h"
#endif

class CommonFontEX;

class GuiClockCtrl : public GuiTickCtrl
{
	typedef GuiTickCtrl Parent;
public:
	DECLARE_CONOBJECT(GuiClockCtrl);
	GuiClockCtrl();
	~GuiClockCtrl();

	static void initPersistFields();
	virtual bool onWake();
	virtual void onSleep();
	void onRender(Point2I offset, const RectI &updateRect);
	void SetColor(ColorI& col);

protected:
	virtual void interpolateTick( F32 delta ) {};
	virtual void processTick();
	virtual void advanceTime( F32 timeDelta ) {};

protected:
	CommonFontEX*   mFont;
	UTF8			mText[32];
	ColorI			mSetColorRGB;
	bool			mUseSetColor;
	S32				mLifeTime;
	S32				mLastUpdateTime;
};

#endif//__GUICLOCKCTRL_H__