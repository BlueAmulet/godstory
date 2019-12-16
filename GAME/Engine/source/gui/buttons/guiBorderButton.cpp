//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxDevice.h"
#include "gui/core/guiCanvas.h"
#include "gui/buttons/guiButtonBaseCtrl.h"


class GuiBorderButtonCtrl : public GuiButtonBaseCtrl
{
   typedef GuiButtonBaseCtrl Parent;

protected:
public:
   DECLARE_CONOBJECT(GuiBorderButtonCtrl);

   void onRender(Point2I offset, const RectI &updateRect);
};

IMPLEMENT_CONOBJECT(GuiBorderButtonCtrl);

void GuiBorderButtonCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	RectI bounds(offset, getExtent());
	if(mActive && mMouseOver)
	{
		bounds.inset(2,2);
		GFX->getDrawUtil()->drawRect(bounds, mProfile->mFontColorHL);
		bounds.inset(-2,-2);
	}
	if(mActive && (mStateOn || mDepressed))
	{
		GFX->getDrawUtil()->drawRect(bounds, mProfile->mFontColorHL);
		bounds.inset(1,1);
		GFX->getDrawUtil()->drawRect(bounds, mProfile->mFontColorHL);
	}

    renderChildControls(offset, updateRect);

	m_DirtyFlag = false;
}

