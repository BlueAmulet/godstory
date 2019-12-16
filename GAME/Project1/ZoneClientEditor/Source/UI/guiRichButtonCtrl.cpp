//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "guiRichButtonCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "console/consoleTypes.h"

IMPLEMENT_CONOBJECT(GuiRichButtonCtrl);

void GuiRichButtonCtrl::onRender( Point2I offset, const RectI &updateRect )
{
	m_DirtyFlag = false;
	bool highlight = mMouseOver;
	bool depressed = mDepressed;

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

	CRichTextDrawer::Style style;

	dStrcpy( style.fontName, sizeof(style.fontName), mProfile->mFontType );
	style.fontSize = mProfile->mFontSize;
	style.fontColor = fontColor;
	style.fontIsBitmap = mProfile->mBitmapFont;
	style.outline = mProfile->mFontOutline;
	style.valign = 1;

	mDrawer.setDefaultStyle( style );
	mDrawer.setDefaultLineHeight(getExtent().y);
	mDrawer.setWidth( getExtent().x );
	mDrawer.setParentControl( this );
	mDrawer.setContent( mButtonText );


	if (bIsUseTextEx == true)
	{
		setWidth( mDrawer.getRealLineWidth() );
	}
	

	//render the children
	//renderChildControls( offset, updateRect);

	textPos.y = offset.y + ( getExtent().y - mDrawer.getHeight() ) / 2 ;

	switch( getControlProfile()->mAlignment )
	{
	case GuiControlProfile::RightJustify:
		textPos.x = offset.x + ( getExtent().x - mDrawer.getRealLineWidth() );
		break;
	case GuiControlProfile::CenterJustify:
		textPos.x = offset.x + ( getExtent().x - mDrawer.getRealLineWidth() ) / 2 ;
		break;
	default:
		// GuiControlProfile::LeftJustify
		textPos.x = offset.x + 5;
		break;
	}
 
	if(depressed)
		textPos += Point2I(1,1);

	
	mDrawer.render( textPos, updateRect  );
}

GuiRichButtonCtrl::GuiRichButtonCtrl()
{
	bIsUseTextEx = false;
}

void GuiRichButtonCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("IsUseTextEx", TypeBool, Offset(bIsUseTextEx, GuiRichButtonCtrl));
}

bool GuiRichButtonCtrl::onWake()
{
	if ( !Parent::onWake() )
		return false;

	CRichTextDrawer::Style style;

	dStrcpy( style.fontName, sizeof(style.fontName), mProfile->mFontType );
	style.fontSize = mProfile->mFontSize;

	mDrawer.setDefaultStyle( style );
	mDrawer.setDefaultLineHeight(15);
	mDrawer.setWidth( getExtent().x );
	mDrawer.setParentControl( this );
	mDrawer.setContent( mButtonText );


	if (bIsUseTextEx == true)
	{
		setWidth( mDrawer.getRealLineWidth() + 10 );
	}

	return true;
}