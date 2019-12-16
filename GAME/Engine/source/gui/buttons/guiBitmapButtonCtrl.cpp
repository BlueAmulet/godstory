//-----------------------------------------------------------------------------
// PowerEngine
//
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------


//-------------------------------------
//
// Bitmap Button Contrl
// Set 'bitmap' comsole field to base name of bitmaps to use.  This control will
// append '_n' for normal
// append '_h' for hilighted
// append '_d' for depressed
//
// if bitmap cannot be found it will use the default bitmap to render.
//
// if the extent is set to (0,0) in the gui editor and appy hit, this control will
// set it's extent to be exactly the size of the normal bitmap (if present)
//


#include "console/console.h"
#include "console/consoleTypes.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/buttons/guiBitmapButtonCtrl.h"

IMPLEMENT_CONOBJECT(GuiBitmapButtonCtrl);

//-------------------------------------
GuiBitmapButtonCtrl::GuiBitmapButtonCtrl()
{
	mBitmapName = StringTable->insert("");
	setExtent(140, 30);
}


//-------------------------------------
void GuiBitmapButtonCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("bitmap", TypeFilename, Offset(mBitmapName, GuiBitmapButtonCtrl));
}


//-------------------------------------
bool GuiBitmapButtonCtrl::onWake()
{
	if (! Parent::onWake())
		return false;
	setActive(true);
	setBitmap(mBitmapName);
	return true;
}


//-------------------------------------
void GuiBitmapButtonCtrl::onSleep()
{
	mTextureNormal = NULL;
	mTextureHilight = NULL;
	mTextureDepressed = NULL;
	mTextureInactive = NULL;
	Parent::onSleep();
}


//-------------------------------------

ConsoleMethod( GuiBitmapButtonCtrl, setBitmap, void, 3, 3, "(filepath name)")
{
	object->setBitmap(argv[2]);
}

//-------------------------------------
void GuiBitmapButtonCtrl::inspectPostApply()
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
void GuiBitmapButtonCtrl::setBitmap(const char *name)
{
	mBitmapName = StringTable->insert(name);
	if(!isAwake())
		return;

	if (*mBitmapName)
	{
		char buffer[1024];
		char *p;
		dStrcpy(buffer, sizeof(buffer), name);
		p = buffer + dStrlen(buffer);

		mTextureNormal = GFXTexHandle(buffer, &GFXDefaultStaticDiffuseProfile, true);
		if (!mTextureNormal)
		{
			dStrcpy(p, sizeof(buffer) - (p - buffer), "_n");
			mTextureNormal = GFXTexHandle(buffer, &GFXDefaultStaticDiffuseProfile, true);
		}
		dStrcpy(p, sizeof(buffer) - (p - buffer), "_h");
		mTextureHilight = GFXTexHandle(buffer, &GFXDefaultStaticDiffuseProfile, true);
		if (!mTextureHilight)
			mTextureHilight = mTextureNormal;
		dStrcpy(p, sizeof(buffer) - (p - buffer), "_d");
		mTextureDepressed = GFXTexHandle(buffer, &GFXDefaultStaticDiffuseProfile, true);
		if (!mTextureDepressed)
			mTextureDepressed = mTextureHilight;
		dStrcpy(p, sizeof(buffer) - (p - buffer), "_i");
		mTextureInactive = GFXTexHandle(buffer, &GFXDefaultStaticDiffuseProfile, true);
		if (!mTextureInactive)
			mTextureInactive = mTextureNormal;

		if (mTextureNormal.isNull() && mTextureHilight.isNull() && mTextureDepressed.isNull() && mTextureInactive.isNull())
			Con::warnf("GFXTextureManager::createTexture - Unable to load Texture: %s", buffer);
	}
	else
	{
		mTextureNormal = NULL;
		mTextureHilight = NULL;
		mTextureDepressed = NULL;
		mTextureInactive = NULL;
	}
	setUpdate();
}


//-------------------------------------
void GuiBitmapButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	enum {
		NORMAL,
		HILIGHT,
		DEPRESSED,
		INACTIVE
	} state = NORMAL;

	if (mActive)
	{
		if (mMouseOver) state = HILIGHT;
		if (mDepressed || mStateOn) state = DEPRESSED;
	}
	else
		state = INACTIVE;

	switch (state)
	{
	case NORMAL:      renderButton(mTextureNormal, offset, updateRect); break;
	case HILIGHT:     renderButton(mTextureHilight ? mTextureHilight : mTextureNormal, offset, updateRect); break;
	case DEPRESSED:   renderButton(mTextureDepressed, offset, updateRect); break;
	case INACTIVE:    renderButton(mTextureInactive ? mTextureInactive : mTextureNormal, offset, updateRect); break;
	}

	m_DirtyFlag = false;
}

//------------------------------------------------------------------------------

void GuiBitmapButtonCtrl::renderButton(GFXTexHandle &texture, Point2I &offset, const RectI& updateRect)
{
	if (texture)
	{
		RectI rect(offset, getExtent());
		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapStretch(texture, rect);

		renderChildControls( offset, updateRect);
	}
	else
		Parent::onRender(offset, updateRect);
}

//------------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiBitmapButtonTextCtrl);

void GuiBitmapButtonTextCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	enum {
		NORMAL,
		HILIGHT,
		DEPRESSED,
		INACTIVE
	} state = NORMAL;

	if (mActive)
	{
		if (mMouseOver) state = HILIGHT;
		if (mDepressed || mStateOn) state = DEPRESSED;
	}
	else
		state = INACTIVE;

	GFXTexHandle texture;

	switch (state)
	{
	case NORMAL:
		texture = mTextureNormal;
		break;
	case HILIGHT:
		texture = mTextureHilight;
		break;
	case DEPRESSED:
		texture = mTextureDepressed;
		break;
	case INACTIVE:
		texture = mTextureInactive;
		if(!texture)
			texture = mTextureNormal;
		break;
	}
	if (texture)
	{
		RectI rect(offset, getExtent());
		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapStretch(texture, rect);

		Point2I textPos = offset;
		if(mDepressed)
			textPos += Point2I(1,1);

		// Make sure we take the profile's textOffset into account.
		textPos += mProfile->mTextOffset;

		GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
		renderJustifiedText(textPos, getExtent(), mButtonText);

		renderChildControls( offset, updateRect);
	}
	else
		Parent::onRender(offset, updateRect);

	m_DirtyFlag = false;
}
