//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------


//-------------------------------------
//
// Icon Button Control
// Draws the bitmap within a special button control.  Only a single bitmap is used and the
// button will be drawn in a highlighted mode when the mouse hovers over it or when it
// has been clicked.
//
// Use mTextLocation to choose where within the button the text will be drawn, if at all.
// Use mTextMargin to set the text away from the button sides or from the bitmap.
// Use mButtonMargin to set everything away from the button sides.
// Use mErrorBitmapName to set the name of a bitmap to draw if the main bitmap cannot be found.
// Use mFitBitmapToButton to force the bitmap to fill the entire button extent.  Usually used
// with no button text defined.
//
// if the extent is set to (0,0) in the gui editor and appy hit, this control will
// set it's extent to be exactly the size of the normal bitmap (if present)
//


#include "console/console.h"
#include "gfx/gfxDevice.h"
#include "console/consoleTypes.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/buttons/guiIconButtonCtrl.h"
#include "gfx/gfxFontRenderBatcher.h"

static const ColorI colorWhite(255,255,255);
static const ColorI colorBlack(0,0,0);

IMPLEMENT_CONOBJECT(GuiIconButtonCtrl);

//-------------------------------------
GuiIconButtonCtrl::GuiIconButtonCtrl()
{
	mBitmapName = StringTable->insert("");
	mTextLocation = TextLocLeft;
	mIconLocation = IconLocLeft;
	mTextMargin = 4;
	mButtonMargin.set(4,4);

	mFitBitmapToButton = false;

	mErrorBitmapName = StringTable->insert("");
	mErrorTextureHandle = NULL;

	setExtent(140, 30);
}

static const EnumTable::Enums textLocEnums[] = 
{
	{ GuiIconButtonCtrl::TextLocNone, "None" },
	{ GuiIconButtonCtrl::TextLocBottom, "Bottom" },
	{ GuiIconButtonCtrl::TextLocRight, "Right" },
	{ GuiIconButtonCtrl::TextLocTop, "Top" },
	{ GuiIconButtonCtrl::TextLocLeft, "Left" },
	{ GuiIconButtonCtrl::TextLocCenter, "Center" },
};

static const EnumTable gTextLocTable(6, &textLocEnums[0]); 


static const EnumTable::Enums iconLocEnums[] = 
{
	{ GuiIconButtonCtrl::IconLocLeft, "Left" },
	{ GuiIconButtonCtrl::IconLocRight, "Right" },
	{ GuiIconButtonCtrl::IconLocNone, "None" },
};
static const EnumTable gIconLocTable(3, &iconLocEnums[0]); 


//-------------------------------------
void GuiIconButtonCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("buttonMargin", TypePoint2I,    Offset(mButtonMargin, GuiIconButtonCtrl));
	addField("iconBitmap", TypeFilename,   Offset(mBitmapName, GuiIconButtonCtrl));
	addField("iconLocation", TypeEnum, Offset(mIconLocation, GuiIconButtonCtrl), 1, &gIconLocTable);
	addField("sizeIconToButton", TypeBool,   Offset(mFitBitmapToButton, GuiIconButtonCtrl));
	addField("textLocation", TypeEnum, Offset(mTextLocation, GuiIconButtonCtrl), 1, &gTextLocTable);
	addField("textMargin", TypeS32,    Offset(mTextMargin, GuiIconButtonCtrl));   

}


//-------------------------------------
bool GuiIconButtonCtrl::onWake()
{
	if (! Parent::onWake())
		return false;
	setActive(true);

	setBitmap(mBitmapName);

	if( mProfile )
		mProfile->constructBitmapArray();

	return true;
}


//-------------------------------------
void GuiIconButtonCtrl::onSleep()
{
	mTextureNormal = NULL;
	Parent::onSleep();
}


//-------------------------------------

ConsoleMethod( GuiIconButtonCtrl, setBitmap, void, 3, 3, "(filepath name)")
{
	char* argBuffer = Con::getArgBuffer( 512 );
	Platform::makeFullPathName( argv[2], argBuffer, 512 );
	object->setBitmap( argBuffer );
}

//-------------------------------------
void GuiIconButtonCtrl::inspectPostApply()
{
	// if the extent is set to (0,0) in the gui editor and appy hit, this control will
	// set it's extent to be exactly the size of the normal bitmap (if present)
	Parent::inspectPostApply();

	if ((getWidth() == 0) && (getHeight() == 0) && mTextureNormal)
	{
		setExtent( mTextureNormal->getWidth() + 4, mTextureNormal->getHeight() + 4);
	}
}


//-------------------------------------
void GuiIconButtonCtrl::setBitmap(const char *name)
{
	mBitmapName = StringTable->insert(name);
	if(!isAwake())
		return;

	if (*mBitmapName)
	{
		mTextureNormal = GFXTexHandle( name, &GFXDefaultStaticDiffuseProfile );
	}
	else
	{
		mTextureNormal = NULL;
	}
	setUpdate();
}   

//-------------------------------------
void GuiIconButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	renderButton( offset, updateRect);
	m_DirtyFlag = false;
}

//------------------------------------------------------------------------------

void GuiIconButtonCtrl::renderButton( Point2I &offset, const RectI& updateRect )
{
	bool highlight = mMouseOver;
	bool depressed = mDepressed;

	ColorI fontColor   = mActive ? (highlight ? mProfile->mFontColor : mProfile->mFontColor) : mProfile->mFontColorNA;
	ColorI backColor   = mActive ? mProfile->mFillColor : mProfile->mFillColorNA; 
	ColorI borderColor = mActive ? mProfile->mBorderColor : mProfile->mBorderColorNA;

	RectI boundsRect(offset, getExtent());

	if (mDepressed || mStateOn)
	{
		// If there is a bitmap array then render using it.  Otherwise use a standard
		// fill.
		if(mProfile->mUseBitmapArray && mProfile->mBitmapArrayRects.size())
			renderBitmapArray(boundsRect, statePressed);
		else
			renderSlightlyLoweredBox(boundsRect, mProfile);
	}
	else if(mMouseOver && mActive)
	{
		// If there is a bitmap array then render using it.  Otherwise use a standard
		// fill.
		if(mProfile->mUseBitmapArray && mProfile->mBitmapArrayRects.size())
			renderBitmapArray(boundsRect, stateMouseOver);
		else
			renderSlightlyRaisedBox(boundsRect, mProfile);
	}
	else
	{
		// If there is a bitmap array then render using it.  Otherwise use a standard
		// fill.
		if(mProfile->mUseBitmapArray && mProfile->mBitmapArrayRects.size())
		{
			if(mActive)
				renderBitmapArray(boundsRect, stateNormal);
			else
				renderBitmapArray(boundsRect, stateDisabled);
		}
		else
		{
			GFX->getDrawUtil()->drawRectFill(boundsRect, mProfile->mFillColorNA);
			GFX->getDrawUtil()->drawRect(boundsRect, mProfile->mBorderColorNA);
		}
	}

	Point2I textPos = offset;
	if(depressed)
		textPos += Point2I(1,1);

	// Render the icon
	if ( mTextureNormal && mIconLocation != GuiIconButtonCtrl::IconLocNone )
	{
		// Render the normal bitmap
		GFX->getDrawUtil()->clearBitmapModulation();

		// Maintain the bitmap size or fill the button?
		if(!mFitBitmapToButton)
		{
			RectI iconRect(offset + mButtonMargin, Point2I(mTextureNormal->getWidth(), mTextureNormal->getHeight()));
			Point2I textureSize( mTextureNormal->getWidth(), mTextureNormal->getHeight() );

			if( mIconLocation == IconLocRight )         
				iconRect.set( offset + getExtent() - ( mButtonMargin + textureSize ), textureSize  );
			else if( mIconLocation == IconLocLeft )
				iconRect.set(offset + mButtonMargin, textureSize );

			GFX->getDrawUtil()->drawBitmapStretch(mTextureNormal, iconRect);

		} 
		else
		{
			RectI rect(offset + mButtonMargin, getExtent() - (mButtonMargin * 2) );
			GFX->getDrawUtil()->drawBitmapStretch(mTextureNormal, rect);
		}

	}

	// Render text
	if(mTextLocation != TextLocNone)
	{
		GFX->getDrawUtil()->setBitmapModulation( fontColor );
		S32 textWidth = mProfile->mFont->getStrWidth(const_cast<char*>(mButtonText));

		if(mTextLocation == TextLocRight)
		{

			Point2I start( mTextMargin, (getHeight()-mProfile->mFont->getHeight())/2 );
			if( mTextureNormal && mIconLocation != GuiIconButtonCtrl::IconLocNone )
			{
				start.x = mTextureNormal->getWidth() + mButtonMargin.x + mTextMargin;
			}

			GFX->getDrawUtil()->drawText( mProfile->mFont, start + offset, mButtonText, mProfile->mFontColors );

		}

		if(mTextLocation == TextLocCenter)
		{
			Point2I start;
			if( mTextureNormal && mIconLocation != GuiIconButtonCtrl::IconLocNone )
			{
				start.set( ( (getWidth() - textWidth - mTextureNormal->getWidth() )/2) + mTextureNormal->getWidth(), (getHeight()-mProfile->mFont->getHeight())/2 );
			}
			else
				start.set( (getWidth() - textWidth)/2, (getHeight()-mProfile->mFont->getHeight())/2 );
			GFX->getDrawUtil()->setBitmapModulation( fontColor );
			GFX->getDrawUtil()->drawText( mProfile->mFont, start + offset, mButtonText, mProfile->mFontColors );
		}
	}


	renderChildControls( offset, updateRect);
}

// Draw the bitmap array's borders according to the button's state.
void GuiIconButtonCtrl::renderBitmapArray(RectI &bounds, S32 state)
{
	switch(state)
	{
	case stateNormal:
		if(mProfile->mBorder == -2)
			renderSizableBitmapBordersFilled(bounds, 1, mProfile);
		else
			renderFixedBitmapBordersFilled(bounds, 1, mProfile);
		break;

	case stateMouseOver:
		if(mProfile->mBorder == -2)
			renderSizableBitmapBordersFilled(bounds, 3, mProfile);
		else
			renderFixedBitmapBordersFilled(bounds, 3, mProfile);
		break;

	case statePressed:
		if(mProfile->mBorder == -2)
			renderSizableBitmapBordersFilled(bounds, 2, mProfile);
		else
			renderFixedBitmapBordersFilled(bounds, 2, mProfile);
		break;

	case stateDisabled:
		if(mProfile->mBorder == -2)
			renderSizableBitmapBordersFilled(bounds, 4, mProfile);
		else
			renderFixedBitmapBordersFilled(bounds, 4, mProfile);
		break;
	}
}
