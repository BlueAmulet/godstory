//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "gfx/gfxDevice.h"
#include "console/consoleTypes.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/buttons/guiToolboxButtonCtrl.h"

IMPLEMENT_CONOBJECT(GuiToolboxButtonCtrl);

//-------------------------------------
GuiToolboxButtonCtrl::GuiToolboxButtonCtrl()
{
	mNormalBitmapName = StringTable->insert("");
	mLoweredBitmapName = StringTable->insert("sceneeditor/client/images/buttondown");
	mHoverBitmapName = StringTable->insert("sceneeditor/client/images/buttonup");
	setMinExtent(Point2I(16,16));
	setExtent(48, 48);
	mButtonType = ButtonTypeRadio;
	mTipHoverTime = 100;

}


//-------------------------------------
void GuiToolboxButtonCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("normalBitmap", TypeFilename, Offset(mNormalBitmapName, GuiToolboxButtonCtrl));
	addField("loweredBitmap", TypeFilename, Offset(mLoweredBitmapName, GuiToolboxButtonCtrl));
	addField("hoverBitmap", TypeFilename, Offset(mHoverBitmapName, GuiToolboxButtonCtrl));
}


//-------------------------------------
bool GuiToolboxButtonCtrl::onWake()
{
	if (! Parent::onWake())
		return false;

	setActive( true );

	setNormalBitmap( mNormalBitmapName );
	setLoweredBitmap( mLoweredBitmapName );
	setHoverBitmap( mHoverBitmapName );

	return true;
}


//-------------------------------------
void GuiToolboxButtonCtrl::onSleep()
{
	mTextureNormal = NULL;
	mTextureLowered = NULL;
	mTextureHover = NULL;
	Parent::onSleep();
}


//-------------------------------------

ConsoleMethod( GuiToolboxButtonCtrl, setNormalBitmap, void, 3, 3, "( filepath name ) sets the bitmap that shows when the button is active")
{
	object->setNormalBitmap(argv[2]);
}

ConsoleMethod( GuiToolboxButtonCtrl, setLoweredBitmap, void, 3, 3, "( filepath name ) sets the bitmap that shows when the button is disabled")
{
	object->setLoweredBitmap(argv[2]);
}

ConsoleMethod( GuiToolboxButtonCtrl, setHoverBitmap, void, 3, 3, "( filepath name ) sets the bitmap that shows when the button is disabled")
{
	object->setHoverBitmap(argv[2]);
}

//-------------------------------------
void GuiToolboxButtonCtrl::inspectPostApply()
{
	// if the extent is set to (0,0) in the gui editor and appy hit, this control will
	// set it's extent to be exactly the size of the normal bitmap (if present)
	Parent::inspectPostApply();

	if ((getWidth() == 0) && (getHeight() == 0) && mTextureNormal)
	{
		setExtent( mTextureNormal->getWidth(), mTextureNormal->getHeight());
	}
}


//-------------------------------------
void GuiToolboxButtonCtrl::setNormalBitmap( StringTableEntry bitmapName )
{
	mNormalBitmapName = StringTable->insert( bitmapName );

	if(!isAwake())
		return;

	if ( *mNormalBitmapName )
		mTextureNormal = GFXTexHandle( mNormalBitmapName, &GFXDefaultStaticDiffuseProfile );
	else
		mTextureNormal = NULL;

	setUpdate();
}   

void GuiToolboxButtonCtrl::setLoweredBitmap( StringTableEntry bitmapName )
{
	mLoweredBitmapName = StringTable->insert( bitmapName );

	if(!isAwake())
		return;

	if ( *mLoweredBitmapName )
		mTextureLowered = GFXTexHandle( mLoweredBitmapName, &GFXDefaultStaticDiffuseProfile );
	else
		mTextureLowered = NULL;

	setUpdate();
}   

void GuiToolboxButtonCtrl::setHoverBitmap( StringTableEntry bitmapName )
{
	mHoverBitmapName = StringTable->insert( bitmapName );

	if(!isAwake())
		return;

	if ( *mHoverBitmapName )
		mTextureHover = GFXTexHandle( mHoverBitmapName, &GFXDefaultStaticDiffuseProfile );
	else
		mTextureHover = NULL;

	setUpdate();
}   



//-------------------------------------
void GuiToolboxButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	m_DirtyFlag = false;
	
	// Only render the state rect (hover/down) if we're active
	if (mActive)
	{
		RectI r(offset, getExtent());
		if ( mDepressed  || mStateOn )
			renderStateRect( mTextureLowered , r );
		else if (mMouseOver) 
			renderStateRect( mTextureHover , r );    
	}

	// Now render the image
	if( mTextureNormal )
	{
		renderButton( mTextureNormal, offset, updateRect );
		return;
	}

	Point2I textPos = offset;
	if( mDepressed )
		textPos += Point2I(1,1);

	// Make sure we take the profile's textOffset into account.
	textPos += mProfile->mTextOffset;

	GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
	renderJustifiedText(textPos, getExtent(), mButtonText);
}

void GuiToolboxButtonCtrl::renderStateRect( GFXTexHandle &texture, const RectI& rect )
{
	if (texture)
	{
		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapStretch( texture, rect );
	}
}

//------------------------------------------------------------------------------

void GuiToolboxButtonCtrl::renderButton(GFXTexHandle &texture, Point2I &offset, const RectI& updateRect)
{
	if (texture)
	{
		Point2I finalOffset = offset;

		finalOffset.x += ( ( getWidth() / 2 ) - ( texture.getWidth() / 2 ) );
		finalOffset.y += ( ( getHeight() / 2 ) - ( texture.getHeight() / 2 ) );

		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmap(texture, finalOffset);

		renderChildControls( offset, updateRect);
	}
}