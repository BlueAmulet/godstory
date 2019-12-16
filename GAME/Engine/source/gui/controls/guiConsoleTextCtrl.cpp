//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "core/color.h"
#include "gui/controls/guiConsoleTextCtrl.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/gfxFontRenderBatcher.h"

IMPLEMENT_CONOBJECT(GuiConsoleTextCtrl);

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

GuiConsoleTextCtrl::GuiConsoleTextCtrl()
{
	//default fonts
	mConsoleExpression = NULL;
	mResult = NULL;
}

void GuiConsoleTextCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("GuiConsoleTextCtrl");		
	addField("expression",  TypeCaseString,  Offset(mConsoleExpression, GuiConsoleTextCtrl));
	endGroup("GuiConsoleTextCtrl");		
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiConsoleTextCtrl::onWake()
{
	if (! Parent::onWake())
		return false;

    mFont = mProfile->mFont;
	return true;
}

void GuiConsoleTextCtrl::onSleep()
{
	Parent::onSleep();
	mFont = NULL;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiConsoleTextCtrl::setText(const char *txt)
{
	//make sure we don't call this before onAdd();
	AssertFatal(mProfile, "Can't call setText() until setProfile() has been called.");

	if (txt)
		mConsoleExpression = StringTable->insert(txt);
	else
		mConsoleExpression = NULL;

	//Make sure we have a font
	mProfile->incRefCount();
    mFont = mProfile->mFont;
	setUpdate();

	//decrement the profile referrence
	mProfile->decRefCount();
}

void GuiConsoleTextCtrl::calcResize()
{
	if (!mResult)
		return;

	//resize
	if (mProfile->mAutoSizeWidth)
	{
		if (mProfile->mAutoSizeHeight)
			setExtent(Point2I(mFont->getStrWidth(( UTF8 *)mResult) + 4, mFont->getHeight() + 4));
		else
			setExtent(Point2I(mFont->getStrWidth(( UTF8 *)mResult) + 4, getHeight()));
	}
	else if (mProfile->mAutoSizeHeight)
	{
		setExtent(Point2I(getWidth(), mFont->getHeight() + 4));
	}
}


void GuiConsoleTextCtrl::onPreRender()
{
	if (mConsoleExpression)
		mResult = Con::evaluatef("$temp = %s;", mConsoleExpression);
	calcResize();
}


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiConsoleTextCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;
	
	// draw the background rectangle
	RectI r(offset, getExtent());

	GFXDrawUtil *drawer = GFX->getDrawUtil();
	drawer->drawRectFill(r, ColorI(255,255,255));

	// draw the border
	r.extent += r.point;
	drawer->drawRect( r.point, r.extent - Point2I( 1, 1 ), ColorI( 0, 0, 0 ) );

	if (mResult)
	{
		S32 txt_w = mFont->getStrWidth(( UTF8 *)mResult);
		Point2I localStart;
		switch (mProfile->mAlignment)
		{
		case GuiControlProfile::RightJustify:
			localStart.set(getWidth() - txt_w-2, 0);
			break;
		case GuiControlProfile::CenterJustify:
			localStart.set((getWidth() - txt_w) / 2, 0);
			break;
		default:
			// GuiControlProfile::LeftJustify
			localStart.set(2,0);
			break;
		}

		Point2I globalStart = localToGlobalCoord(localStart);

		//draw the text
		drawer->setBitmapModulation(mProfile->mFontColor);
		drawer->drawText(mFont, globalStart, mResult, mProfile->mFontColors);
	}

	//render the child controls
	renderChildControls(offset, updateRect);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

const char *GuiConsoleTextCtrl::getScriptValue()
{
	return getText();
}

void GuiConsoleTextCtrl::setScriptValue(const char *val)
{
	setText(val);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// EOF //
