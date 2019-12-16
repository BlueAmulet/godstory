//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIRICHBUTTONCTRL_H_
#define _GUIRICHBUTTONCTRL_H_

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif

#include "util/RichTextDrawer.h"

class GuiRichButtonCtrl : public GuiButtonCtrl
{
	typedef GuiButtonCtrl Parent;
protected:
	
public:

	GuiRichButtonCtrl();
	DECLARE_CONOBJECT(GuiRichButtonCtrl);
	
	static void initPersistFields();
	
	CRichTextDrawer         mDrawer;

	bool bIsUseTextEx;
	
	void onRender(Point2I offset, const RectI &updateRect);
	bool onWake();
};

#endif 