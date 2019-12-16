//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "gfx/gfxDevice.h"
#include "console/consoleTypes.h"
#include "gui/core/guiCanvas.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "gui/core/guiDefaultControlRender.h"

IMPLEMENT_CONOBJECT(GuiButtonCtrl);

GuiButtonCtrl::GuiButtonCtrl()
{
	setExtent(140, 30);
	mButtonText				= StringTable->insert("");
	mRightMouseDownCommand	= StringTable->insert("");
	mMouseDownCommand       = StringTable->insert("");
	mMouseUpCommand         = StringTable->insert("");
}

bool GuiButtonCtrl::onWake()
{
	if( !Parent::onWake() )
		return false;
	// Button Theme?
	if( mProfile->constructBitmapArray() >= 36 )
		mHasTheme = true;
	else
		mHasTheme = false;
	return true;

}

void GuiButtonCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("RightMouseDownCommand",	TypeString, Offset(mRightMouseDownCommand,	GuiButtonCtrl));
	addField("MouseDownCommand",        TypeString, Offset(mMouseDownCommand,       GuiButtonCtrl));
    addField("MouseUpCommand",          TypeString, Offset(mMouseUpCommand,         GuiButtonCtrl));        
}

void GuiButtonCtrl::onRightMouseDown(const GuiEvent &event)
{
	// �����ؼ����������ű�
	if (mRightMouseDownCommand && mRightMouseDownCommand[0])
	{
		Con::setVariable("$ThisControl",avar("%d",getId()));
		Con::evaluate(mRightMouseDownCommand,false);
	}
	Parent::onRightMouseDown( event );
}

void GuiButtonCtrl::onMouseDown(const GuiEvent &event)
{
	//�����ؼ�����갴�²����ű�
	if (mMouseDownCommand && mMouseDownCommand[0])
	{
		Con::setVariable("$ThisControl",avar("%d",getId()));
		Con::evaluate(mMouseDownCommand,false);
	}
	Parent::onMouseDown(event);
}

void GuiButtonCtrl::onMouseUp(const GuiEvent &event)
{
	//�����ؼ�������ɿ������ű�
	if (mMouseUpCommand && mMouseUpCommand[0])
	{
		Con::setVariable("$ThisControl",avar("%d",getId()));
		Con::evaluate(mMouseUpCommand,false);
	}
	Parent::onMouseUp(event);
}

//--------------------------------------------------------------------------
void GuiButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	bool highlight = mMouseOver;
	bool depressed = mDepressed;

	//<Edit>:thinking:�޸��˰�ť��ͬ״̬�µ�������ɫ��ͼƬ��ʾ˳��
	ColorI fontColor   = mActive ? (depressed ? mProfile->mFontColorSEL : highlight ? mProfile->mFontColorHL : mProfile->mFontColor) : mProfile->mFontColorNA;
	ColorI backColor   = mActive ? mProfile->mFillColor : mProfile->mFillColorNA;
	ColorI borderColor = mActive ? mProfile->mBorderColor : mProfile->mBorderColorNA;

	RectI boundsRect(offset, getExtent());

	if( mProfile->mBorder != 0 && !mHasTheme )
	{
		if (mDepressed || mStateOn)
			renderFilledBorder( boundsRect, mProfile->mBorderColorHL, mProfile->mFillColorHL );
		else
			renderFilledBorder( boundsRect, mProfile->mBorderColor, mProfile->mFillColor );
	}
	else if( mHasTheme )
	{
		S32 indexMultiplier = 1;
		if ( !mActive )
			indexMultiplier = 4;
		else if ( mDepressed || mStateOn )
			indexMultiplier = 2;
		else if ( mMouseOver ) 
			indexMultiplier = 3;

		renderSizableBitmapBordersFilled( boundsRect, indexMultiplier, mProfile );
	}

	Point2I textPos = offset;
	if(depressed)
		textPos += Point2I(1,1);

	GFX->getDrawUtil()->setBitmapModulation( fontColor );
	renderJustifiedText(textPos, getExtent(), mButtonText);

	//render the children
	renderChildControls( offset, updateRect);

	m_DirtyFlag = false;
}

