//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "gui/core/guiCanvas.h"
#include "gui/containers/guiWindowCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxFontRenderBatcher.h"

IMPLEMENT_CONOBJECT(GuiWindowCtrl);

//-----------------------------------------------------------------------------


GuiWindowCtrl::GuiWindowCtrl() : mResizeEdge(edgeNone),
mResizeWidth(true),
mResizeHeight(true),
mResizeMargin(5.f),
mCanMove(true),
mCanClose(true),
mCanMinimize(true),
mCanMaximize(true),
mCanHelp(false),
mCanDock(false),
mEdgeSnap(true)
{
	mTitleHeight = 20;

	mIsContainer = true;

	mCloseCommand = StringTable->insert("");
	mHelpCommand  = StringTable->insert("");

	mMinimized = false;
	mMaximized = false;
	mMouseMovingWin = false;
	mMouseResizeWidth = false;
	mMouseResizeHeight = false;
	mMouseOver = false;
	setExtent(100, 200);
	mMinSize.set(50, 50);
	mMinimizeIndex = -1;
	mTabIndex = -1;
	mBoundCheck = false;

	RectI closeRect(80, 2, 16, 16);
	mCloseButton = closeRect;
	closeRect.point.x -= 18;
	mMaximizeButton = closeRect;
	closeRect.point.x -= 18;
	mMinimizeButton = closeRect;
	closeRect.point.x -= 18;
	mHelpButton = closeRect;

	//other defaults
	mActive = true;
	mPressClose = false;
	mPressMaximize = false;
	mPressMinimize = false;
	mPressHelp = false;

	mText = StringTable->insert("New Window");
}

void GuiWindowCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("resizeWidth",       TypeBool,         Offset(mResizeWidth, GuiWindowCtrl));
	addField("resizeHeight",      TypeBool,         Offset(mResizeHeight, GuiWindowCtrl));
	addField("canMove",           TypeBool,         Offset(mCanMove, GuiWindowCtrl));
	addField("canClose",          TypeBool,         Offset(mCanClose, GuiWindowCtrl));
	addField("canMinimize",       TypeBool,         Offset(mCanMinimize, GuiWindowCtrl));
	addField("canMaximize",       TypeBool,         Offset(mCanMaximize, GuiWindowCtrl));
	addField("canHelp",           TypeBool,         Offset(mCanHelp,GuiWindowCtrl));
	addField("boundCheck",        TypeBool,         Offset(mBoundCheck,GuiWindowCtrl));
	addField("minSize",           TypePoint2I,      Offset(mMinSize, GuiWindowCtrl));
	addField("closeCommand",      TypeString,       Offset(mCloseCommand, GuiWindowCtrl));
	addField("helpCommand",       TypeString,       Offset(mHelpCommand,GuiWindowCtrl));
	addField("EdgeSnap",          TypeBool,         Offset(mEdgeSnap,GuiWindowCtrl));
	addField("TitleHeight",       TypeS32,          Offset(mTitleHeight,GuiWindowCtrl));
	addField("text", TypeString, Offset(mText, GuiWindowCtrl ));
}

bool GuiWindowCtrl::isMinimized(S32 &index)
{
	index = mMinimizeIndex;
	return mMinimized && mVisible;
}

// helper fn so button positioning shares code...
void GuiWindowCtrl::PositionButtons(void)
{
	if( !mBitmapBounds || !mAwake )
		return;

	S32 buttonWidth = mBitmapBounds[BmpStates * BmpClose].extent.x;
	S32 buttonHeight = mBitmapBounds[BmpStates * BmpClose].extent.y;
	Point2I mainOff = mProfile->mTextOffset;

	// until a pref, if alignment is CENTER, put buttons RIGHT justified.
	// ELSE, put buttons LEFT justified.
	int closeLeft = mainOff.x, closeTop = mainOff.y, closeOff = buttonWidth + 2;
	if ( mProfile->mAlignment == GuiControlProfile::CenterJustify )
	{
		closeOff = -closeOff;
		closeLeft = getWidth() - buttonWidth - mainOff.x;
	}
	RectI closeRect(closeLeft, closeTop, buttonHeight, buttonWidth);
	mCloseButton = closeRect;

	// Always put Minimize on left side of Maximize.
	closeRect.point.x += closeOff;
	//<Edit>:[thinking]:增加帮助按钮,在此处不考虑不存在关毕按钮.
	if (!(mCanMaximize|| mCanMinimize))
	{
		mHelpButton = closeRect;
	}
	else if (closeOff>0)
	{
		mMinimizeButton = closeRect;
		closeRect.point.x += closeOff;
		mMaximizeButton = closeRect;
		closeRect.point.x += closeOff;
		mHelpButton = closeRect;
	}
	else
	{
		mMaximizeButton = closeRect;
		closeRect.point.x += closeOff;
		mMinimizeButton = closeRect;
		closeRect.point.x += closeOff;
		mHelpButton = closeRect;
	}
}

bool GuiWindowCtrl::onWake()
{
	if (! Parent::onWake())
		return false;

	//get the texture for the close, minimize, and maximize buttons
	mTextureObject = mProfile->mTextureObject;
	bool result = mProfile->constructBitmapArray() >= NumBitmaps;
	AssertFatal(result, "Failed to create the bitmap array");
	if(!result)
		return false;

	mBitmapBounds = mProfile->mBitmapArrayRects.address();
	S32 buttonHeight = mBitmapBounds[BmpStates * BmpClose].extent.y;
	//<Edit>:[thinking]:当脚本没有设mTitleHeight值时,调用按钮的高度.脚本定义了mTitleHeight高度
	//是为了防止可点击标题高度太小,因为有可能标题高按钮小的图
	if (mTitleHeight < buttonHeight+4)
		mTitleHeight = buttonHeight + 4;

	//set the button coords
	PositionButtons();

	//set the tab index
	mTabIndex = -1;
	GuiControl *parent = getParent();
	if (parent && mFirstResponder)
	{
		mTabIndex = 0;

		//count the number of windows preceeding this one
		iterator i;
		for (i = parent->begin(); i != parent->end(); i++)
		{
			GuiWindowCtrl *ctrl = dynamic_cast<GuiWindowCtrl *>(*i);
			if (ctrl)
			{
				if (ctrl == this) break;
				else if (ctrl->mFirstResponder) mTabIndex++;
			}
		}
	}

	return true;
}

void GuiWindowCtrl::onSleep()
{
	mTextureObject = NULL;
	Parent::onSleep();
}

GuiControl* GuiWindowCtrl::findHitControl(const Point2I &pt, S32 initialLayer)
{
	if (! mMinimized)
		return Parent::findHitControl(pt, initialLayer);
	else
		return this;
}

bool GuiWindowCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	if( !Parent::resize(newPosition, newExtent) )
		return false;

	//set the button coords
	PositionButtons();

	return true;
}




//-----------------------------------------------------------------------------
// Mouse Methods
//-----------------------------------------------------------------------------
S32 GuiWindowCtrl::findHitEdges( const Point2I &globalPoint )
{
	// No Edges
	S32 edgeMask = edgeNone;

	RectI bounds( getBounds() );

	// Create an EdgeRectI structure that has four edges
	// Left/Right/Top/Bottom
	// Each Edge structure has a hit operation that will take
	// another edge and test for a hit on the edge with a margin
	// specified by the .margin scalar
	EdgeRectI edges = EdgeRectI(bounds, mResizeMargin);

	// Get Cursor Edges
	Edge cursorVertEdge = Edge( globalPoint, Point2F( 1.f, 0.f ) );
	Edge cursorHorzEdge = Edge( globalPoint, Point2F( 0.f, 1.f ) );

	if( edges.left.hit( cursorVertEdge ) )
		edgeMask |= edgeLeft;
	else if( edges.right.hit( cursorVertEdge ) )
		edgeMask |= edgeRight;

	if( edges.top.hit( cursorHorzEdge ) )
		edgeMask |= edgeTop;
	else if( edges.bottom.hit( cursorHorzEdge ) )
		edgeMask |= edgeBottom;

	// Return the resulting mask
	return edgeMask;
}

void GuiWindowCtrl::getSnappableRects( Vector<EdgeRectI> &outVector )
{
	GuiControl *parent = getParent();
	if( !parent )
		return;

	S32 parentSize = parent->size();
	for( S32 i = 0; i < parentSize; i++ )
	{
		GuiWindowCtrl *childWindow = dynamic_cast<GuiWindowCtrl*>(parent->at(i));
		if( !childWindow || !childWindow->isVisible() || childWindow == this )
			continue;

		outVector.push_back(EdgeRectI(childWindow->getGlobalBounds(), mResizeMargin));
	}

}


void GuiWindowCtrl::onMouseDown(const GuiEvent &event)
{
	setUpdate();

	mOrigBounds = getBounds();

	mMouseDownPosition = event.mousePoint;
	Point2I localPoint = globalToLocalCoord(event.mousePoint);

	//select this window - move it to the front, and set the first responder
	selectWindow();

	mMouseMovingWin = false;

	S32 hitEdges = findHitEdges( event.mousePoint );

	mResizeEdge = edgeNone;

	// Set flag by default so we only clear it
	// if we don't match either edge
	mMouseResizeHeight = true;

	// Check Bottom/Top edges (Mutually Exclusive)
	if( mResizeHeight && hitEdges & edgeBottom )
		mResizeEdge |= edgeBottom;
	else if( mResizeHeight && hitEdges & edgeTop )
		mResizeEdge |= edgeTop;
	else
		mMouseResizeHeight = false;

	// Set flag by default so we only clear it
	// if we don't match either edge
	mMouseResizeWidth = true;

	// Check Left/Right edges (Mutually Exclusive)
	if( mResizeWidth && hitEdges & edgeLeft )
		mResizeEdge |= edgeLeft;
	else if( mResizeWidth && hitEdges & edgeRight )
		mResizeEdge |= edgeRight;
	else
		mMouseResizeWidth = false;


	//if we clicked within the title bar
	if ( !(mResizeEdge & edgeTop) && localPoint.y < mTitleHeight)
	{
		//if we clicked on the close button
		if (mCanClose && mCloseButton.pointInRect(localPoint))
		{
			mPressClose = mCanClose;
		}
		else if (mCanMaximize && mMaximizeButton.pointInRect(localPoint))
		{
			mPressMaximize = mCanMaximize;
		}
		else if (mCanMinimize && mMinimizeButton.pointInRect(localPoint))
		{
			mPressMinimize = mCanMinimize;
		}
		//<Edit>:[thinking]:增加帮助
		else if(mCanHelp && mHelpButton.pointInRect(localPoint))
		{
			mPressHelp = mCanHelp;
		}
		//else we clicked within the title
		else 
		{
			S32 docking = getDocking();
			if( docking == Docking::dockInvalid || docking == Docking::dockNone )
				mMouseMovingWin = mCanMove;

			mMouseResizeWidth = false;
			mMouseResizeHeight = false;
		}
	}


	if (mMouseMovingWin || mResizeEdge != edgeNone ||
		mPressClose || mPressMaximize || mPressMinimize || mPressHelp)
	{
		mouseLock();
	}
	else
	{

		GuiControl *ctrl = findHitControl(localPoint);
		if (ctrl && ctrl != this)
			ctrl->onMouseDown(event);

	}
}

void GuiWindowCtrl::onMouseDragged(const GuiEvent &event)
{
	GuiControl *parent = getParent();
	GuiCanvas *root = getRoot();
	if ( !root ) return;

	Point2I deltaMousePosition = event.mousePoint - mMouseDownPosition;

	Point2I newPosition = getPosition();
	Point2I newExtent = getExtent();

	if (mMouseMovingWin && parent)
	{
		if( parent != root && !mBoundCheck) 
		{
			newPosition.x = mOrigBounds.point.x + deltaMousePosition.x;
			newPosition.y = getMax(0, mOrigBounds.point.y + deltaMousePosition.y );
		}
		else
		{
			newPosition.x = getMax(0, getMin(parent->getWidth() - getWidth(), mOrigBounds.point.x + deltaMousePosition.x));
			newPosition.y = getMax(0, getMin(parent->getHeight() - getHeight(), mOrigBounds.point.y + deltaMousePosition.y));
		}

		// Check snapping to other windows
		if( mEdgeSnap )
		{
			RectI bounds = getGlobalBounds();
			bounds.point = mOrigBounds.point + deltaMousePosition;
			EdgeRectI edges = EdgeRectI( bounds, mResizeMargin );

			Vector<EdgeRectI> snapList;
			getSnappableRects( snapList );
			for( S32 i =0; i < snapList.size(); i++ )
			{
				// Compare Edges for hit
				EdgeRectI &snapRect = snapList[i];
				if( edges.left.hit( snapRect.right ) )
					newPosition.x = snapRect.right.position.x;
				else if( edges.right.hit( snapRect.left ) )
					newPosition.x = snapRect.left.position.x - bounds.extent.x;
				if( edges.bottom.hit( snapRect.top ) )
					newPosition.y = snapRect.top.position.y - bounds.extent.y;
				else if( edges.top.hit( snapRect.bottom ) )
					newPosition.y = snapRect.bottom.position.y;
			}
		}
	}
	else if(mPressClose || mPressMaximize || mPressMinimize || mPressHelp)
	{
		setUpdate();
		return;
	}
	else
	{
		if( ( !mMouseResizeHeight && !mMouseResizeWidth ) || !parent )
			return;

		if( mResizeEdge & edgeBottom)
			newExtent.y = getMin(parent->getHeight(), mOrigBounds.extent.y + deltaMousePosition.y);
		else if ( mResizeEdge & edgeTop )
		{
			newPosition.y = mOrigBounds.point.y + deltaMousePosition.y;
			newExtent.y = getMin(parent->getHeight(), mOrigBounds.extent.y - deltaMousePosition.y);
		}

		if( mResizeEdge & edgeRight )
			newExtent.x = getMin(parent->getWidth(), mOrigBounds.extent.x + deltaMousePosition.x);
		else if( mResizeEdge & edgeLeft )
		{
			newPosition.x = mOrigBounds.point.x + deltaMousePosition.x;
			newExtent.x = getMin(parent->getWidth(), mOrigBounds.extent.x - deltaMousePosition.x);
		}

		//if (mMouseResizeWidth && parent)
		//{
		//   newExtent.x = getMax(0, getMax(minExtent.x, getMin(parent->getWidth(), mOrigBounds.extent.x + deltaMousePosition.x)));
		//   update = true;
		//}

	}

	Point2I pos = parent->localToGlobalCoord(getPosition());
	root->addUpdateRegion(pos, getExtent());
	resize(newPosition, newExtent);
}

void GuiWindowCtrl::onMouseUp(const GuiEvent &event)
{
	bool closing = mPressClose;
	bool maximizing = mPressMaximize;
	bool minimizing = mPressMinimize;
	bool helping    = mPressHelp;
	mPressClose = false;
	mPressMaximize = false;
	mPressMinimize = false;
	mPressHelp  = false;

	event;
	mouseUnlock();

	mMouseMovingWin = false;
	mMouseResizeWidth = false;
	mMouseResizeHeight = false;

	GuiControl *parent = getParent();
	if (! parent)
		return;

	//see if we take an action
	Point2I localPoint = globalToLocalCoord(event.mousePoint);
	if (closing && mCloseButton.pointInRect(localPoint))
	{
		Con::evaluate(mCloseCommand);
	}
	else if (maximizing && mMaximizeButton.pointInRect(localPoint))
	{
		if (mMaximized)
		{
			//resize to the previous position and extent, bounded by the parent
			resize(Point2I(getMax(0, getMin(parent->getWidth() - mStandardBounds.extent.x, mStandardBounds.point.x)),
				getMax(0, getMin(parent->getHeight() - mStandardBounds.extent.y, mStandardBounds.point.y))),
				mStandardBounds.extent);
			//set the flag
			mMaximized = false;
		}
		else
		{
			//only save the position if we're not minimized
			if (! mMinimized)
			{
				mStandardBounds = getBounds();
			}
			else
			{
				mMinimized = false;
			}

			//resize to fit the parent
			resize(Point2I(0, 0), parent->getExtent());

			//set the flag
			mMaximized = true;
		}
	}
	else if (minimizing && mMinimizeButton.pointInRect(localPoint))
	{
		if (mMinimized)
		{
			//resize to the previous position and extent, bounded by the parent
			resize(Point2I(getMax(0, getMin(parent->getWidth() - mStandardBounds.extent.x, mStandardBounds.point.x)),
				getMax(0, getMin(parent->getHeight() - mStandardBounds.extent.y, mStandardBounds.point.y))),
				mStandardBounds.extent);
			//set the flag
			mMinimized = false;
		}
		else
		{
			if (parent->getWidth() < 100 || parent->getHeight() < mTitleHeight + 3)
				return;

			//only save the position if we're not maximized
			if (! mMaximized)
			{
				mStandardBounds = getBounds();
			}
			else
			{
				mMaximized = false;
			}

			//first find the lowest unused minimized index up to 32 minimized windows
			U32 indexMask = 0;
			iterator i;
			S32 count = 0;
			for (i = parent->begin(); i != parent->end() && count < 32; i++)
			{
				count++;
				S32 index;
				GuiWindowCtrl *ctrl = dynamic_cast<GuiWindowCtrl *>(*i);
				if (ctrl && ctrl->isMinimized(index))
				{
					indexMask |= (1 << index);
				}
			}

			//now find the first unused bit
			for (count = 0; count < 32; count++)
			{
				if (! (indexMask & (1 << count))) break;
			}

			//if we have more than 32 minimized windows, use the first position
			count = getMax(0, count);

			//this algorithm assumes all window have the same title height, and will minimize to 98 pix
			Point2I newExtent(98, mTitleHeight);

			//first, how many can fit across
			S32 numAcross = getMax(1, (parent->getWidth() / newExtent.x + 2));

			//find the new "mini position"
			Point2I newPosition;
			newPosition.x = (count % numAcross) * (newExtent.x + 2) + 2;
			newPosition.y = parent->getHeight() - (((count / numAcross) + 1) * (newExtent.y + 2)) - 2;

			//find the minimized position and extent
			resize(newPosition, newExtent);

			//set the index so other windows will not try to minimize to the same location
			mMinimizeIndex = count;

			//set the flag
			mMinimized = true;
		}
	}

	else if (helping && mHelpButton.pointInRect(localPoint))
	{
		Con::evaluate(mHelpCommand);
	}
}

void GuiWindowCtrl::onMouseEnter(const GuiEvent &event)
{
	setUpdate();
	mMouseOver = true;
}

void GuiWindowCtrl::onMouseLeave(const GuiEvent &event)
{
	setUpdate();
	mMouseOver = false;
}

GuiControl *GuiWindowCtrl::findNextTabable(GuiControl *curResponder, bool firstCall)
{
	//set the global if this is the first call (directly from the canvas)
	if (firstCall)
	{
		GuiControl::smCurResponder = NULL;
	}

	//if the window does not already contain the first responder, return false
	//ie.  Can't tab into or out of a window
	if (! ControlIsChild(curResponder))
	{
		return NULL;
	}

	//loop through, checking each child to see if it is the one that follows the firstResponder
	GuiControl *tabCtrl = NULL;
	iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		tabCtrl = ctrl->findNextTabable(curResponder, false);
		if (tabCtrl) break;
	}

	//to ensure the tab cycles within the current window...
	if (! tabCtrl)
	{
		tabCtrl = findFirstTabable();
	}

	mFirstResponder = tabCtrl;
	return tabCtrl;
}

GuiControl *GuiWindowCtrl::findPrevTabable(GuiControl *curResponder, bool firstCall)
{
	if (firstCall)
	{
		GuiControl::smPrevResponder = NULL;
	}

	//if the window does not already contain the first responder, return false
	//ie.  Can't tab into or out of a window
	if (! ControlIsChild(curResponder))
	{
		return NULL;
	}

	//loop through, checking each child to see if it is the one that follows the firstResponder
	GuiControl *tabCtrl = NULL;
	iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		tabCtrl = ctrl->findPrevTabable(curResponder, false);
		if (tabCtrl) break;
	}

	//to ensure the tab cycles within the current window...
	if (! tabCtrl)
	{
		tabCtrl = findLastTabable();
	}

	mFirstResponder = tabCtrl;
	return tabCtrl;
}

bool GuiWindowCtrl::onKeyDown(const GuiEvent &event)
{
	//if this control is a dead end, kill the event
	if ((! mVisible) || (! mActive) || (! mAwake)) return true;

	if ((event.keyCode == KEY_TAB) && (event.modifier & SI_CTRL))
	{
		//find the next sibling window, and select it
		GuiControl *parent = getParent();
		if (parent)
		{
			GuiWindowCtrl *firstWindow = NULL;
			iterator i;
			for (i = parent->begin(); i != parent->end(); i++)
			{
				GuiWindowCtrl *ctrl = dynamic_cast<GuiWindowCtrl *>(*i);
				if (ctrl && ctrl->getTabIndex() == mTabIndex + 1)
				{
					ctrl->selectWindow();
					return true;
				}
				else if (ctrl && ctrl->getTabIndex() == 0)
				{
					firstWindow = ctrl;
				}
			}
			//recycle from the beginning
			if (firstWindow != this)
			{
				firstWindow->selectWindow();
				return true;
			}
		}
	}

	return Parent::onKeyDown(event);
}

const RectI GuiWindowCtrl::getClientRect()
{

	if( !mProfile || mProfile->mBitmapArrayRects.size() < NumBitmaps )
		return Parent::getClientRect();

	RectI winRect;
	winRect.point.x = mBitmapBounds[BorderLeft].extent.x;
	winRect.point.y = mBitmapBounds[BorderTopKey].extent.y;

	winRect.extent.y = getHeight() - ( winRect.point.y  + mBitmapBounds[BorderBottom].extent.y );
	winRect.extent.x = getWidth() - ( winRect.point.x  + mBitmapBounds[BorderRight].extent.x );

	// Finally, inset it by padding
	// Inset by padding.  margin is specified for all t/b/l/r but 
	// uses only pointx pointy uniformly on both ends. This should be fixed. - JDD
	//winRect.inset( mSizingOptions.mPadding.point.x, mSizingOptions.mPadding.point.y );

	return winRect;
}
void GuiWindowCtrl::selectWindow(void)
{
	//first make sure this window is the front most of its siblings
	GuiControl *parent = getParent();
	if (parent && *parent->end() != this )
	{
		parent->pushObjectToBack(this);
	}

	//also set the first responder to be the one within this window
	setFirstResponder(mFirstResponder);
}

void GuiWindowCtrl::drawWinRect(const RectI &myRect)
{
	Point2I bl = myRect.point;
	Point2I tr;
	tr.x = myRect.point.x + myRect.extent.x - 1;
	tr.y = myRect.point.y + myRect.extent.y - 1;
	GFX->getDrawUtil()->drawRectFill(myRect, mProfile->mFillColor);
	GFX->getDrawUtil()->drawLine(Point2I(bl.x + 1, tr.y), Point2I(bl.x + 1, bl.y), ColorI(255, 255, 255));
	GFX->getDrawUtil()->drawLine(Point2I(bl.x, tr.y + 1), Point2I(tr.x, tr.y + 1), ColorI(255, 255, 255));
	//GFX->drawRect(myRect, ColorI(0, 0, 0)); // Taken out, this is controled via mProfile->mBorder
}

void GuiWindowCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;

	if( !mProfile || !mProfile->mFont || mProfile->mBitmapArrayRects.size() < NumBitmaps )
		return Parent::onRender( offset, updateRect );

	//draw the outline
	RectI winRect;
	winRect.point = offset;
	winRect.extent = getExtent();
	GuiCanvas *root = getRoot();
	GuiControl *firstResponder = root ? root->getFirstResponder() : NULL;

	bool isKey = (!firstResponder || ControlIsChild(firstResponder));

	U32 topBase = isKey ? BorderTopLeftKey : BorderTopLeftNoKey;
	winRect.point.x += mBitmapBounds[BorderLeft].extent.x;
	winRect.point.y += mBitmapBounds[topBase + 2].extent.y;

	// <Edit>:[soar]:绘制时是从起点坐标加一开始绘制，长、宽就会多出一个像素和边框重叠，长、宽减一就可以解决了。
	winRect.extent.x -= mBitmapBounds[BorderLeft].extent.x + mBitmapBounds[BorderRight].extent.x + 1;
	winRect.extent.y -= mBitmapBounds[topBase + 2].extent.y + mBitmapBounds[BorderBottom].extent.y + 1;

	GFX->getDrawUtil()->drawRectFill(winRect, mProfile->mFillColor);

	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->drawBitmapSR(mTextureObject, offset, mBitmapBounds[topBase]);
	GFX->getDrawUtil()->drawBitmapSR(mTextureObject, Point2I(offset.x + getWidth() - mBitmapBounds[topBase+1].extent.x, offset.y),
		mBitmapBounds[topBase + 1]);

	RectI destRect;
	destRect.point.x = offset.x + mBitmapBounds[topBase].extent.x;
	destRect.point.y = offset.y;
	destRect.extent.x = getWidth() - mBitmapBounds[topBase].extent.x - mBitmapBounds[topBase + 1].extent.x;
	destRect.extent.y = mBitmapBounds[topBase + 2].extent.y;
	RectI stretchRect = mBitmapBounds[topBase + 2];
	stretchRect.inset(1,0);
	GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, destRect, stretchRect);

	destRect.point.x = offset.x;
	destRect.point.y = offset.y + mBitmapBounds[topBase].extent.y;
	destRect.extent.x = mBitmapBounds[BorderLeft].extent.x;
	destRect.extent.y = getHeight() - mBitmapBounds[topBase].extent.y - mBitmapBounds[BorderBottomLeft].extent.y;
	stretchRect = mBitmapBounds[BorderLeft];
	stretchRect.inset(0,1);
	GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, destRect, stretchRect);

	destRect.point.x = offset.x + getWidth() - mBitmapBounds[BorderRight].extent.x;
	destRect.extent.x = mBitmapBounds[BorderRight].extent.x;
	destRect.point.y = offset.y + mBitmapBounds[topBase + 1].extent.y;
	destRect.extent.y = getHeight() - mBitmapBounds[topBase + 1].extent.y - mBitmapBounds[BorderBottomRight].extent.y;

	stretchRect = mBitmapBounds[BorderRight];
	stretchRect.inset(0,1);
	GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, destRect, stretchRect);

	GFX->getDrawUtil()->drawBitmapSR(mTextureObject, offset + Point2I(0, getHeight() - mBitmapBounds[BorderBottomLeft].extent.y), mBitmapBounds[BorderBottomLeft]);
	GFX->getDrawUtil()->drawBitmapSR(mTextureObject, offset + getExtent() - mBitmapBounds[BorderBottomRight].extent, mBitmapBounds[BorderBottomRight]);

	destRect.point.x = offset.x + mBitmapBounds[BorderBottomLeft].extent.x;
	destRect.extent.x = getWidth() - mBitmapBounds[BorderBottomLeft].extent.x - mBitmapBounds[BorderBottomRight].extent.x;

	destRect.point.y = offset.y + getHeight() - mBitmapBounds[BorderBottom].extent.y;
	destRect.extent.y = mBitmapBounds[BorderBottom].extent.y;
	stretchRect = mBitmapBounds[BorderBottom];
	stretchRect.inset(1,0);

	GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, destRect, stretchRect);

	//draw the title
	// dhc addition: copied/modded from renderJustifiedText, since we enforce a
	// different color usage here. NOTE: it currently CAN overdraw the controls
	// if mis-positioned or 'scrunched' in a small width.
	GFX->getDrawUtil()->setBitmapModulation(mProfile->mFontColor);
	S32 textWidth = mProfile->mFont->getStrWidth(( UTF8 *)mText);
	Point2I start(0,0);
	// align the horizontal
	if ( mProfile->mAlignment == GuiControlProfile::RightJustify )
		start.set( winRect.extent.x - textWidth, 0 );
	else if ( mProfile->mAlignment == GuiControlProfile::CenterJustify )
		start.set( ( winRect.extent.x - textWidth) / 2, 0 );
	else // GuiControlProfile::LeftJustify or garbage... ;)
		start.set( 0, 0 );
	// If the text is longer then the box size, (it'll get clipped) so force Left Justify
	if( textWidth > winRect.extent.x ) start.set( 0, 0 );
	// center the vertical
	//   start.y = ( winRect.extent.y - ( font->getHeight() - 2 ) ) / 2;
	GFX->getDrawUtil()->drawText( mProfile->mFont, start + offset + mProfile->mTextOffset, mText );

	// deal with rendering the titlebar controls
	AssertFatal(root, "Unable to get the root GuiCanvas.");
	Point2I localPoint = globalToLocalCoord(root->getCursorPos());

	//draw the close button
	Point2I tempUL;
	Point2I tempLR;
	S32 bmp = BmpStates * BmpClose;

	if( mCanClose ) {
		if( mCloseButton.pointInRect( localPoint ) && mPressClose )
			bmp += BmpHilite;
		else if (mCloseButton.pointInRect( localPoint ) && mMouseOver)
			bmp += BmpOver;

		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapSR(mTextureObject, offset + mCloseButton.point, mBitmapBounds[bmp]);
	}

	//draw the maximize button
	if( mMaximized )
		bmp = BmpStates * BmpNormal;
	else
		bmp = BmpStates * BmpMaximize;

	if( mCanMaximize ) {
		if( mMaximizeButton.pointInRect( localPoint ) && mPressMaximize )
			bmp += BmpHilite;
		else if (mMaximizeButton.pointInRect( localPoint ) && mMouseOver)
			bmp += BmpOver;

		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapSR( mTextureObject, offset + mMaximizeButton.point, mBitmapBounds[bmp] );
	}

	//draw the minimize button
	if( mMinimized )
		bmp = BmpStates * BmpNormal;
	else
		bmp = BmpStates * BmpMinimize;

	if( mCanMinimize ) {
		if( mMinimizeButton.pointInRect( localPoint ) && mPressMinimize )
			bmp += BmpHilite;
		else if (mMinimizeButton.pointInRect( localPoint ) && mMouseOver)
			bmp += BmpOver;

		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapSR( mTextureObject, offset + mMinimizeButton.point, mBitmapBounds[bmp] );
	}
	//<Edit>:[thinking]:增加帮助按钮
	bmp = BmpStates * BmpHelp;
	if(mCanHelp){
		if (mHelpButton.pointInRect(localPoint) && mPressHelp)
			bmp += BmpHilite;
		else if (mHelpButton.pointInRect(localPoint) && mMouseOver)
			bmp += BmpOver;

		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapSR( mTextureObject, offset + mHelpButton.point, mBitmapBounds[bmp] );
	}

	if( !mMinimized )
	{
		//render the children
		renderChildControls( offset, updateRect );
	}
}

void GuiWindowCtrl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
#ifdef NTJ_EDITOR
	GuiCanvas *pRoot = getRoot();
	if( !pRoot )
		return;

	PlatformWindow *pWindow = static_cast<GuiCanvas*>(getRoot())->getPlatformWindow();
	AssertFatal(pWindow != NULL,"GuiControl without owning platform window!  This should not be possible.");
	PlatformCursorController *pController = pWindow->getCursorController();
	AssertFatal(pController != NULL,"PlatformWindow without an owned CursorController!");

	S32 desiredCursor = PlatformCursorController::curArrow;
	S32 hitEdges = findHitEdges( lastGuiEvent.mousePoint );

	if( hitEdges & edgeBottom && hitEdges & edgeLeft && mResizeHeight )
		desiredCursor = PlatformCursorController::curResizeNESW;
	else if( hitEdges & edgeBottom && hitEdges & edgeRight && mResizeHeight  )
		desiredCursor = PlatformCursorController::curResizeNWSE;
	else if( hitEdges & edgeBottom && mResizeHeight )
		desiredCursor = PlatformCursorController::curResizeHorz;
	else if( hitEdges & edgeTop && hitEdges & edgeLeft && mResizeHeight )
		desiredCursor = PlatformCursorController::curResizeNWSE;
	else if( hitEdges & edgeTop && hitEdges & edgeRight && mResizeHeight )
		desiredCursor = PlatformCursorController::curResizeNESW;
	else if( hitEdges & edgeTop && mResizeHeight )
		desiredCursor = PlatformCursorController::curResizeHorz;
	else if ( hitEdges && edgeLeft && mResizeWidth )
		desiredCursor = PlatformCursorController::curResizeVert;
	else if( hitEdges & edgeRight && mResizeWidth )
		desiredCursor = PlatformCursorController::curResizeHorz;
	else
		desiredCursor = PlatformCursorController::curArrow;

	// Bail if we're already at the desired cursor
	if(pRoot->mCursorChanged == desiredCursor )
		return;

	// Now change the cursor shape
	pController->popCursor();
	pController->pushCursor(pController->getSysCursor(desiredCursor));
	pRoot->mCursorChanged = desiredCursor;
#endif
}


