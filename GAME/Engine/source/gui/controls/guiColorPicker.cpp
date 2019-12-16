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
#include "gui/controls/guiColorPicker.h"
#include "gfx/primBuilder.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
GFXStateBlock* GuiColorPickerCtrl::mSetSB = NULL;

/// @name Common colors we use
/// @{
ColorF colorWhite(1.,1.,1.);
ColorF colorWhiteBlend(1.,1.,1.,.75);
ColorF colorBlack(.0,.0,.0);
ColorF colorAlpha(0.0f, 0.0f, 0.0f, 0.0f);

ColorI GuiColorPickerCtrl::mColorRange[9] = {
	ColorI(255,255,255), // White
	ColorI(255,255,255), // White
	ColorI(255,0,255),   // Pink
	ColorI(0,0,255),     // Blue
	ColorI(0,255,255),   // Light blue
	ColorI(0,255,0),     // Green
	ColorI(255,255,0),   // Yellow
	ColorI(255,0,0),     // Red
	ColorI(0,0,0)        // Black
};
/// @}

IMPLEMENT_CONOBJECT(GuiColorPickerCtrl);

//--------------------------------------------------------------------------
GuiColorPickerCtrl::GuiColorPickerCtrl()
{
	setExtent(140, 30);
	mDisplayMode = pPallet;
	mBaseColor = ColorF(1.,.0,1.);
	mPickColor = ColorF(.0,.0,.0);
	mSelectorPos = Point2I(0,0);
	mMouseDown = mMouseOver = false;
	mActive = true;
	mPositionChanged = false;
	mSelectorGap = 1;
	mActionOnMove = false;
}

//--------------------------------------------------------------------------
static const EnumTable::Enums gColorPickerModeEnums[] =
{
	{ GuiColorPickerCtrl::pPallet,		"Pallete"   },
	{ GuiColorPickerCtrl::pHorizColorRange,	"HorizColor"},
	{ GuiColorPickerCtrl::pVertColorRange,	"VertColor" },
	{ GuiColorPickerCtrl::pHorizColorBrightnessRange,	"HorizBrightnessColor"},
	{ GuiColorPickerCtrl::pVertColorBrightnessRange,	"VertBrightnessColor" },
	{ GuiColorPickerCtrl::pBlendColorRange,	"BlendColor"},
	{ GuiColorPickerCtrl::pHorizAlphaRange,	"HorizAlpha"},
	{ GuiColorPickerCtrl::pVertAlphaRange,	"VertAlpha" },
	{ GuiColorPickerCtrl::pDropperBackground,	"Dropper" },
};

static const EnumTable gColorPickerModeTable( 9, gColorPickerModeEnums );

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addGroup("ColorPicker");
	addField("BaseColor", TypeColorF, Offset(mBaseColor, GuiColorPickerCtrl));
	addField("PickColor", TypeColorF, Offset(mPickColor, GuiColorPickerCtrl));
	addField("SelectorGap", TypeS32,  Offset(mSelectorGap, GuiColorPickerCtrl)); 
	addField("DisplayMode", TypeEnum, Offset(mDisplayMode, GuiColorPickerCtrl), 1, &gColorPickerModeTable );
	addField("ActionOnMove", TypeBool,Offset(mActionOnMove, GuiColorPickerCtrl));
	endGroup("ColorPicker");
}

//--------------------------------------------------------------------------
// Function to draw a box which can have 4 different colors in each corner blended together
void GuiColorPickerCtrl::drawBlendBox(RectI &bounds, ColorF &c1, ColorF &c2, ColorF &c3, ColorF &c4)
{
	S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
	S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

#ifdef STATEBLOCK
	AssertFatal(mSetSB, "GuiColorPickerCtrl::drawBlendBox -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif

	GFX->setTexture( 0, NULL );

	PrimBuild::begin( GFXTriangleFan, 4 );
	PrimBuild::color( c1 );
	PrimBuild::vertex2i( l, t );

	PrimBuild::color( c2 );
	PrimBuild::vertex2i( r, t );

	PrimBuild::color( c3 );
	PrimBuild::vertex2i( r, b );

	PrimBuild::color( c4 );
	PrimBuild::vertex2i( l, b );
	PrimBuild::end();
}

//--------------------------------------------------------------------------
/// Function to draw a set of boxes blending throughout an array of colors
void GuiColorPickerCtrl::drawBlendRangeBox(RectI &bounds, bool vertical, U8 numColors, ColorI *colors)
{
	S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
	S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

	// Calculate increment value
	S32 x_inc = int(mFloor((r - l) / F32(numColors-1)));
	S32 y_inc = int(mFloor((b - t) / F32(numColors-1)));

#ifdef STATEBLOCK
	AssertFatal(mSetSB, "GuiColorPickerCtrl::drawBlendRangeBox -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif

	GFX->setTexture( 0, NULL );

	for( U16 i = 0;i < numColors - 1; i++ ) 
	{
		// This is not efficent, but then again it doesn't really need to be. -pw
		PrimBuild::begin( GFXTriangleFan, 4 );

		// If we are at the end, x_inc and y_inc need to go to the end (otherwise there is a rendering bug)
		if (i == numColors-2) 
		{
			x_inc += r-l-1;
			y_inc += b-t-1;
		}

		if (!vertical)  // Horizontal (+x)
		{
			// First color
			PrimBuild::color( colors[i] );
			PrimBuild::vertex2i( l, t );
			PrimBuild::vertex2i( l, b );

			// Second color
			PrimBuild::color( colors[i+1] );
			PrimBuild::vertex2i( l + x_inc, b );
			PrimBuild::vertex2i( l + x_inc, t );
			l += x_inc;
		}
		else  // Vertical (+y)
		{
			// First color
			PrimBuild::color( colors[i] );
			PrimBuild::vertex2i( l, t );
			PrimBuild::vertex2i( r, t );

			// Second color
			PrimBuild::color( colors[i+1] );
			PrimBuild::vertex2i( r, t + y_inc );
			PrimBuild::vertex2i( l, t + y_inc );
			t += y_inc;
		}
		PrimBuild::end();
	}
}

void GuiColorPickerCtrl::drawSelector(RectI &bounds, Point2I &selectorPos, SelectorMode mode)
{
	U16 sMax = mSelectorGap*2;
	switch (mode)
	{
	case sVertical:
		// Now draw the vertical selector
		// Up -> Pos
		if (selectorPos.y != bounds.point.y+1)
			GFX->getDrawUtil()->drawLine(selectorPos.x, bounds.point.y, selectorPos.x, selectorPos.y-sMax-1, colorWhiteBlend);
		// Down -> Pos
		if (selectorPos.y != bounds.point.y+bounds.extent.y) 
			GFX->getDrawUtil()->drawLine(selectorPos.x,	selectorPos.y + sMax, selectorPos.x, bounds.point.y + bounds.extent.y, colorWhiteBlend);
		break;
	case sHorizontal:
		// Now draw the horizontal selector
		// Left -> Pos
		if (selectorPos.x != bounds.point.x) 
			GFX->getDrawUtil()->drawLine(bounds.point.x, selectorPos.y-1, selectorPos.x-sMax, selectorPos.y-1, colorWhiteBlend);
		// Right -> Pos
		if (selectorPos.x != bounds.point.x) 
			GFX->getDrawUtil()->drawLine(bounds.point.x+mSelectorPos.x+sMax, selectorPos.y-1, bounds.point.x + bounds.extent.x, selectorPos.y-1, colorWhiteBlend);
		break;
	}
}

//--------------------------------------------------------------------------
/// Function to invoke calls to draw the picker box and selector
void GuiColorPickerCtrl::renderColorBox(RectI &bounds)
{
	RectI pickerBounds;
	pickerBounds.point.x = bounds.point.x+1;
	pickerBounds.point.y = bounds.point.y+1;
	pickerBounds.extent.x = bounds.extent.x-1;
	pickerBounds.extent.y = bounds.extent.y-1;

	if (mProfile->mBorder)
		GFX->getDrawUtil()->drawRect(bounds, mProfile->mBorderColor);

	Point2I selectorPos = Point2I(bounds.point.x+mSelectorPos.x+1, bounds.point.y+mSelectorPos.y+1);

	// Draw color box differently depending on mode
	RectI blendRect;
	switch (mDisplayMode)
	{
	case pHorizColorRange:
		drawBlendRangeBox( pickerBounds, false, 7, mColorRange + 2);
		drawSelector( pickerBounds, selectorPos, sVertical );
		break;
	case pVertColorRange:
		drawBlendRangeBox( pickerBounds, true, 7, mColorRange + 2);
		drawSelector( pickerBounds, selectorPos, sHorizontal );
		break;
	case pHorizColorBrightnessRange:
		blendRect = pickerBounds;
		blendRect.point.y++;
		blendRect.extent.y -= 2;
		drawBlendRangeBox( pickerBounds, false, 9, mColorRange);
		// This is being drawn slightly offset from the larger rect so as to insure 255 and 0
		// can both be selected for every color.
		drawBlendBox( blendRect, colorAlpha, colorAlpha, colorBlack, colorBlack );
		blendRect.point.y += blendRect.extent.y - 1;
		blendRect.extent.y = 2;
		GFX->getDrawUtil()->drawRect( blendRect, colorBlack);
		drawSelector( pickerBounds, selectorPos, sHorizontal );
		drawSelector( pickerBounds, selectorPos, sVertical );
		break;
	case pVertColorBrightnessRange:
		drawBlendRangeBox( pickerBounds, true, 9, mColorRange);
		drawBlendBox( pickerBounds, colorAlpha, colorBlack, colorBlack, colorAlpha );
		drawSelector( pickerBounds, selectorPos, sHorizontal );
		drawSelector( pickerBounds, selectorPos, sVertical );
		break;
	case pHorizAlphaRange:
		drawBlendBox( pickerBounds, colorBlack, colorWhite, colorWhite, colorBlack );
		drawSelector( pickerBounds, selectorPos, sVertical );
		break;
	case pVertAlphaRange:
		drawBlendBox( pickerBounds, colorBlack, colorBlack, colorWhite, colorWhite );
		drawSelector( pickerBounds, selectorPos, sHorizontal ); 
		break;
	case pBlendColorRange:
		drawBlendBox( pickerBounds, colorWhite, mBaseColor, colorBlack, colorBlack );
		drawSelector( pickerBounds, selectorPos, sHorizontal );      
		drawSelector( pickerBounds, selectorPos, sVertical );
		break;
	case pDropperBackground:
		break;
	case pPallet:
	default:
		GFX->getDrawUtil()->drawRectFill( pickerBounds, mBaseColor );
		break;
	}
}

void GuiColorPickerCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	m_DirtyFlag  = false;
	
	RectI boundsRect(offset, getExtent()); 
	renderColorBox(boundsRect);

	if (mPositionChanged) 
	{
		mPositionChanged = false;
		Point2I extent = getRoot()->getExtent();
		// If we are anything but a pallete, change the pick color
		if (mDisplayMode != pPallet)
		{
			Point2I resolution = getRoot()->getExtent();

			U32 buf_x = offset.x + mSelectorPos.x + 1;
			U32 buf_y = resolution.y - ( extent.y - ( offset.y + mSelectorPos.y + 1 ) );

			GFXTexHandle bb( resolution.x, 
				resolution.y, 
				GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile );
			Point2I tmpPt( buf_x, buf_y );
			bb.readBackBuffer( tmpPt );
			GBitmap bmp( bb.getWidth(), bb.getHeight() );

			bb.copyToBmp( &bmp );

			//bmp.writePNGDebug( "foo.png" );

			ColorI tmp;
			bmp.getColor( buf_x, buf_y, tmp );

			mPickColor = (ColorF)tmp;

			// Now do onAction() if we are allowed
			if (mActionOnMove) 
				onAction();
		}

	}

	//render the children
	renderChildControls( offset, updateRect);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::setSelectorPos(const Point2I &pos)
{
	Point2I extent = getExtent();
	RectI rect;
	if (mDisplayMode != pDropperBackground) 
	{
		extent.x -= 3;
		extent.y -= 2;
		rect = RectI(Point2I(1,1), extent);
	}
	else
	{
		rect = RectI(Point2I(0,0), extent);
	}

	if (rect.pointInRect(pos)) 
	{
		mSelectorPos = pos;
		mPositionChanged = true;
		// We now need to update
		setUpdate();
	}

	else
	{
		if ((pos.x > rect.point.x) && (pos.x < (rect.point.x + rect.extent.x)))
			mSelectorPos.x = pos.x;
		else if (pos.x <= rect.point.x)
			mSelectorPos.x = rect.point.x;
		else if (pos.x >= (rect.point.x + rect.extent.x))
			mSelectorPos.x = rect.point.x + rect.extent.x - 1;

		if ((pos.y > rect.point.y) && (pos.y < (rect.point.y + rect.extent.y)))
			mSelectorPos.y = pos.y;
		else if (pos.y <= rect.point.y)
			mSelectorPos.y = rect.point.y;
		else if (pos.y >= (rect.point.y + rect.extent.y))
			mSelectorPos.y = rect.point.y + rect.extent.y - 1;

		mPositionChanged = true;
		setUpdate();
	}
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseDown(const GuiEvent &event)
{
	if (!mActive)
		return;

	if (mDisplayMode == pDropperBackground)
		return;

	mouseLock(this);

	if (mProfile->mCanKeyFocus)
		setFirstResponder();

	// Update the picker cross position
	if (mDisplayMode != pPallet)
		setSelectorPos(globalToLocalCoord(event.mousePoint)); 

	mMouseDown = true;
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseDragged(const GuiEvent &event)
{
	if ((mActive && mMouseDown) || (mActive && (mDisplayMode == pDropperBackground)))
	{
		// Update the picker cross position
		if (mDisplayMode != pPallet)
			setSelectorPos(globalToLocalCoord(event.mousePoint));
	}

	if (!mActionOnMove && mAltConsoleCommand[0] )
		Con::evaluate( mAltConsoleCommand, false );

}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseMove(const GuiEvent &event)
{
	// Only for dropper mode
	if (mActive && (mDisplayMode == pDropperBackground))
		setSelectorPos(globalToLocalCoord(event.mousePoint));
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseEnter(const GuiEvent &event)
{
	mMouseOver = true;
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseLeave(const GuiEvent &)
{
	// Reset state
	mMouseOver = false;
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseUp(const GuiEvent &)
{
	//if we released the mouse within this control, perform the action
	if (mActive && mMouseDown && (mDisplayMode != pDropperBackground)) 
	{
		onAction();
		mMouseDown = false;
	}
	else if (mActive && (mDisplayMode == pDropperBackground)) 
	{
		// In a dropper, the alt command executes the mouse up action (to signal stopping)
		if ( mAltConsoleCommand[0] )
			Con::evaluate( mAltConsoleCommand, false );
	}

	mouseUnlock();
}

//--------------------------------------------------------------------------
const char *GuiColorPickerCtrl::getScriptValue()
{
	static char temp[256];
	ColorF color = getValue();
	dSprintf(temp,256,"%f %f %f %f",color.red, color.green, color.blue, color.alpha);
	return temp; 
}

//--------------------------------------------------------------------------    
void GuiColorPickerCtrl::setScriptValue(const char *value)
{
	ColorF newValue;
	dSscanf(value, "%f %f %f %f", &newValue.red, &newValue.green, &newValue.blue, &newValue.alpha);
	setValue(newValue);
}

ConsoleMethod(GuiColorPickerCtrl, getSelectorPos, const char*, 2, 2, "Gets the current position of the selector")
{
	char *temp = Con::getReturnBuffer(256);
	Point2I pos;
	pos = object->getSelectorPos();
	dSprintf(temp,256,"%d %d",pos.x, pos.y); 
	return temp;
}

ConsoleMethod(GuiColorPickerCtrl, setSelectorPos, void, 3, 3, "Sets the current position of the selector")
{
	Point2I newPos;
	dSscanf(argv[2], "%d %d", &newPos.x, &newPos.y);
	object->setSelectorPos(newPos);
}

ConsoleMethod(GuiColorPickerCtrl, updateColor, void, 2, 2, "Forces update of pick color")
{
	object->updateColor();
}

void GuiColorPickerCtrl::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mSetSB);
}


void GuiColorPickerCtrl::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void GuiColorPickerCtrl::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void GuiColorPickerCtrl::shutdown()
{
	SAFE_DELETE(mSetSB);
}


