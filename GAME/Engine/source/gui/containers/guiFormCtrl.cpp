//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/containers/guiFormCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gfx/gfxFontRenderBatcher.h"

IMPLEMENT_CONOBJECT(GuiFormCtrl);

ConsoleMethod(GuiFormCtrl, setCaption, void, 3, 3, "setCaption(caption) - Sets the title of the Form")
{
	object->setCaption(argv[2]);
}

GuiFormCtrl::GuiFormCtrl()
{
	setMinExtent(Point2I(200,100));
	mActive        = true;
	mMouseOver     = false;
	mDepressed     = false;
	mCanMove       = false;
	mCaption = StringTable->insert("[none]");
	mUseSmallCaption = false;
	mSmallCaption = StringTable->insert("");

	mContentLibrary = StringTable->insert("");
	mContent = StringTable->insert("");

	mCanSaveFieldDictionary = true;


	mIsContainer = true;
}

GuiFormCtrl::~GuiFormCtrl()
{
}

void GuiFormCtrl::initPersistFields()
{

	addField("Caption",       TypeString,  Offset(mCaption,     GuiFormCtrl));
	addField("ContentLibrary",TypeString,  Offset(mContentLibrary, GuiFormCtrl));

	Parent::initPersistFields();


}

void GuiFormCtrl::setCaption(const char* caption)
{
	if(caption)
	{
		mCaption = StringTable->insert(caption, true);
	}
}

bool GuiFormCtrl::onWake()
{
	if ( !Parent::onWake() )
		return false;

	mFont = mProfile->mFont;
	AssertFatal(mFont, "GuiFormCtrl::onWake: invalid font in profile" );

	mProfile->constructBitmapArray();

	if(mProfile->mUseBitmapArray && mProfile->mBitmapArrayRects.size())
	{
		mThumbSize.set(   mProfile->mBitmapArrayRects[0].extent.x, mProfile->mBitmapArrayRects[0].extent.y );
		mThumbSize.setMax( mProfile->mBitmapArrayRects[1].extent );

		if(mFont->getHeight() > mThumbSize.y)
			mThumbSize.y = mFont->getHeight();
	}
	else
	{
		mThumbSize.set(20, 20);
	}

	return true;
}


void GuiFormCtrl::addObject(SimObject *newObj )
{
	GuiControl *newCtrl = dynamic_cast<GuiControl*>( newObj );
	GuiFormCtrl*formCtrl = dynamic_cast<GuiFormCtrl*>( newObj );
	if( newCtrl && formCtrl )
		newCtrl->setCanSave( true );
	else if ( newCtrl )
		newCtrl->setCanSave( false );

	Parent::addObject( newObj );
}

void GuiFormCtrl::onSleep()
{
	Parent::onSleep();
    mFont = NULL;
}

bool GuiFormCtrl::onAdd()
{
	if(!Parent::onAdd())
		return false;

	return true;
}

bool GuiFormCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	if( !Parent::resize(newPosition, newExtent) ) 
		return false;

	if( !mAwake || !mProfile->mBitmapArrayRects.size() )
		return false;

	// Should the caption be modified because the title bar is too small?
	S32 textWidth = mProfile->mFont->getStrWidth(const_cast<char*>(mCaption));
	S32 newTextArea = getWidth() - mThumbSize.x - mProfile->mBitmapArrayRects[4].extent.x;
	if(newTextArea < textWidth)
	{
		static char buf[256];

		mUseSmallCaption = true;
		mSmallCaption = StringTable->insert("");

		S32 strlen = dStrlen((const char*)mCaption);
		for(S32 i=strlen; i>=0; --i)
		{
			dStrcpy(buf, sizeof(buf), "");
			dStrncat(buf, sizeof(buf), (const char*)mCaption, i);
			dStrcat(buf, sizeof(buf), "...");

			textWidth = mProfile->mFont->getStrWidth(buf);

			if(textWidth < newTextArea)
			{
				mSmallCaption = StringTable->insert(buf, true);
				break;
			}
		}

	} else
	{
		mUseSmallCaption = false;
	}

	Con::executef(this, "onResize");

	return true;
}

void GuiFormCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;

	// Fill in the control's child area
	RectI boundsRect(offset, getExtent());
	boundsRect.point.y += mThumbSize.y;
	boundsRect.extent.y -= mThumbSize.y;

	// draw the border of the form if specified
	if (mProfile->mOpaque)
		GFX->getDrawUtil()->drawRectFill(boundsRect, mProfile->mFillColor);

	if (mProfile->mBorder)
		renderBorder(boundsRect, mProfile);

	// If we don't have a child, put some text in the child area
	if( empty() )
	{
		GFX->getDrawUtil()->setBitmapModulation(ColorI(0,0,0));
		renderJustifiedText(boundsRect.point, boundsRect.extent, "[none]");
	}

	S32 textWidth = 0;

	// Draw our little bar, too
	if(mProfile->mBitmapArrayRects.size() >= 5)
	{
		GFX->getDrawUtil()->clearBitmapModulation();

		S32 barStart = ( 1 + mProfile->mBorderThickness) + offset.x + textWidth;
		S32 barTop   = mThumbSize.y/2 + offset.y - mProfile->mBitmapArrayRects[3].extent.y /2;

		Point2I barOffset(barStart, barTop);

		// Draw the start of the bar...
		GFX->getDrawUtil()->drawBitmapStretchSR(mProfile->mTextureObject ,RectI(barOffset, mProfile->mBitmapArrayRects[2].extent), mProfile->mBitmapArrayRects[2] );

		// Now draw the middle...
		barOffset.x += mProfile->mBitmapArrayRects[2].extent.x;

		S32 barMiddleSize = (getExtent().x - (barOffset.x - offset.x)) - mProfile->mBitmapArrayRects[4].extent.x;

		if(barMiddleSize>0)
		{
			// We have to do this inset to prevent nasty stretching artifacts
			RectI foo = mProfile->mBitmapArrayRects[3];
			foo.inset(1,0);

			GFX->getDrawUtil()->drawBitmapStretchSR(
				mProfile->mTextureObject,
				RectI(barOffset, Point2I(barMiddleSize, mProfile->mBitmapArrayRects[3].extent.y)),
				foo
				);
		}

		// And the end
		barOffset.x += barMiddleSize;

		GFX->getDrawUtil()->drawBitmapStretchSR( mProfile->mTextureObject, RectI(barOffset, mProfile->mBitmapArrayRects[4].extent),
			mProfile->mBitmapArrayRects[4]);

		GFX->getDrawUtil()->setBitmapModulation((mMouseOver ? mProfile->mFontColorHL : mProfile->mFontColor));
		renderJustifiedText(Point2I(mThumbSize.x, 0) + offset, Point2I(getWidth() - mThumbSize.x - mProfile->mBitmapArrayRects[4].extent.x, mThumbSize.y), (mUseSmallCaption ? mSmallCaption : mCaption) );

	}

	// Render the children
	renderChildControls(offset, updateRect);
}

void GuiFormCtrl::onMouseMove(const GuiEvent &event)
{
	Point2I localMove = globalToLocalCoord(event.mousePoint);

	// If we're clicking in the header then resize
	mMouseOver = (localMove.y < mThumbSize.y);
	if(isMouseLocked())
		mDepressed = mMouseOver;

}

void GuiFormCtrl::onMouseEnter(const GuiEvent &event)
{
	setUpdate();
	if(isMouseLocked())
	{
		mDepressed = true;
		mMouseOver = true;
	}
	else
	{
		mMouseOver = true;
	}

}

void GuiFormCtrl::onMouseLeave(const GuiEvent &event)
{
	setUpdate();
	if(isMouseLocked())
		mDepressed = false;
	mMouseOver = false;
}

void GuiFormCtrl::onMouseDown(const GuiEvent &event)
{
	Point2I localClick = globalToLocalCoord(event.mousePoint);

	// If we're clicking in the header then resize
	if(localClick.y < mThumbSize.y)
	{
		mouseLock();
		mDepressed = true;
		mMouseMovingWin = mCanMove;

		//update
		setUpdate();
	}

	mOrigBounds = getBounds();

	mMouseDownPosition = event.mousePoint;

	if (mMouseMovingWin )
	{
		mouseLock();
	}
	else
	{
		GuiControl *ctrl = findHitControl(localClick);
		if (ctrl && ctrl != this)
			ctrl->onMouseDown(event);
	}
}

void GuiFormCtrl::onMouseUp(const GuiEvent &event)
{
	// Make sure we only get events we ought to be getting...
	if (! mActive)
		return; 

	mouseUnlock();
	setUpdate();

	Point2I localClick = globalToLocalCoord(event.mousePoint);

	// If we're clicking in the header then resize
	//if(localClick.y < mThumbSize.y && mDepressed)
	//   setCollapsed(!mCollapsed);
}

