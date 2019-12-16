//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "gfx/gBitmap.h"
#include "core/resManager.h"
#include "platform/event.h"
#include "gui/core/guiArrayCtrl.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gfx/gfxDevice.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
GFXStateBlock* GuiScrollCtrl::mSetSB = NULL;

IMPLEMENT_CONOBJECT(GuiScrollCtrl);

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

GuiScrollCtrl::GuiScrollCtrl()
{
   mChildMargin.set(0,0);
   mBorderThickness = 1;
   mScrollBarThickness = 16;
   mScrollBarArrowBtnLength = 16;
   mScrollBarDragTolerance = 130;
   mStateDepressed = false;
   mHitRegion = None;

   mWillFirstRespond = true;
   mUseConstantHeightThumb = false;
   mIsContainer = true;

   mForceVScrollBar = ScrollBarAlwaysOn;
   mForceHScrollBar = ScrollBarAlwaysOn;
   setExtent(200,200);
}

static const EnumTable::Enums scrollBarEnums[] =
{
   { GuiScrollCtrl::ScrollBarAlwaysOn,     "alwaysOn"     },
   { GuiScrollCtrl::ScrollBarAlwaysOff,    "alwaysOff"    },
   { GuiScrollCtrl::ScrollBarDynamic,      "dynamic"      },
};
static const EnumTable gScrollBarTable(3, &scrollBarEnums[0]);

ConsoleMethod(GuiScrollCtrl, scrollToTop, void, 2, 2, "() - scrolls the scroll control to the top of the child content area.")
{
   argc; argv;
   object->scrollTo( 0, 0 );
}

ConsoleMethod(GuiScrollCtrl, scrollToBottom, void, 2, 2, "() - scrolls the scroll control to the bottom of the child content area.")
{
   argc; argv;
   object->scrollTo( 0, 0x7FFFFFFF );
}

ConsoleMethod(GuiScrollCtrl, setScrollPosition, void, 4, 4, "(x, y) - scrolls the scroll control to the specified position.")
{
   object->scrollTo(dAtoi(argv[2]), dAtoi(argv[3]));
}

ConsoleMethod(GuiScrollCtrl, getScrollPositionX, S32, 2, 2, "() - get the current x scroll position of the scroll control.")
{
   return object->getChildRelPos().x;
}

ConsoleMethod(GuiScrollCtrl, getScrollPositionY, S32, 2, 2, "() - get the current y scroll position of the scroll control.")
{
   return object->getChildRelPos().y;
}

void GuiScrollCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("willFirstRespond",     TypeBool,    Offset(mWillFirstRespond, GuiScrollCtrl));
   addField("hScrollBar",           TypeEnum,    Offset(mForceHScrollBar, GuiScrollCtrl), 1, &gScrollBarTable);
   addField("vScrollBar",           TypeEnum,    Offset(mForceVScrollBar, GuiScrollCtrl), 1, &gScrollBarTable);
   addField("constantThumbHeight",  TypeBool,    Offset(mUseConstantHeightThumb, GuiScrollCtrl));
   addField("childMargin",          TypePoint2I, Offset(mChildMargin, GuiScrollCtrl));

}

bool GuiScrollCtrl::resize(const Point2I &newPos, const Point2I &newExt)
{
   if( !Parent::resize(newPos, newExt) )
      return false;

   computeSizes();
   return true;
}

void GuiScrollCtrl::childResized(GuiControl *child)
{
   Parent::childResized(child);
   computeSizes();
}

bool GuiScrollCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   mTextureObject = mProfile->mTextureObject;
   if (mTextureObject && (mProfile->constructBitmapArray() >= BmpStates * BmpCount))
   {
      mBitmapBounds = mProfile->mBitmapArrayRects.address();

      //init
      mBaseThumbSize = mBitmapBounds[BmpStates * BmpVThumbTopCap].extent.y +
         mBitmapBounds[BmpStates * BmpVThumbBottomCap].extent.y;
      mScrollBarThickness      = mBitmapBounds[BmpStates * BmpVPage].extent.x;
      mScrollBarArrowBtnLength = mBitmapBounds[BmpStates * BmpUp].extent.y;
      computeSizes();
   } 
   else
   {
      Con::warnf("No texture loaded for scroll control named %s with profile %s", getName(), mProfile->getName());
   }
   return true;
}

void GuiScrollCtrl::onSleep()
{
   // Reset the mouse tracking state of this control
   //  when it is put to sleep
   mStateDepressed = false;
   mHitRegion = None;

   Parent::onSleep();
   mTextureObject = NULL;
}

bool GuiScrollCtrl::calcChildExtents()
{
   // right now the scroll control really only deals well with a single
   // child control for its content
   if (!size())
      return false;

   GuiControl *ctrl = (GuiControl *) front();
   mChildExt = ctrl->getExtent();
   mChildPos = ctrl->getPosition();
   return true;
}

RectI lastVisRect;

void GuiScrollCtrl::scrollRectVisible(RectI rect)
{
   // rect is passed in virtual client space
   if(rect.extent.x > mContentExt.x)
      rect.extent.x = mContentExt.x;
   if(rect.extent.y > mContentExt.y)
      rect.extent.y = mContentExt.y;

   // Determine the points bounding the requested rectangle
	Point2I rectUpperLeft  = rect.point;
	Point2I rectLowerRight = rect.point + rect.extent;

   lastVisRect = rect;

   // Determine the points bounding the actual visible area...
	Point2I visUpperLeft = mChildRelPos;
	Point2I visLowerRight = mChildRelPos + mContentExt;
	Point2I delta(0,0);

   // We basically try to make sure that first the top left of the given
   // rect is visible, and if it is, then that the bottom right is visible.

   // Make sure the rectangle is visible along the X axis...
	if(rectUpperLeft.x < visUpperLeft.x)
		delta.x = rectUpperLeft.x - visUpperLeft.x;
	else if(rectLowerRight.x > visLowerRight.x)
		delta.x = rectLowerRight.x - visLowerRight.x;

   // Make sure the rectangle is visible along the Y axis...
	if(rectUpperLeft.y < visUpperLeft.y)
		delta.y = rectUpperLeft.y - visUpperLeft.y;
	else if(rectLowerRight.y > visLowerRight.y)
		delta.y = rectLowerRight.y - visLowerRight.y;

   // If we had any changes, scroll, otherwise don't.
   if(delta.x || delta.y)
		scrollDelta(delta.x, delta.y);
}


void GuiScrollCtrl::addObject(SimObject *object)
{
   Parent::addObject(object);
   computeSizes();
}

GuiControl* GuiScrollCtrl::findHitControl(const Point2I &pt, S32 initialLayer)
{
   if(pt.x < mProfile->mBorderThickness || pt.y < mProfile->mBorderThickness)
      return this;
   if(pt.x >= getWidth() - mProfile->mBorderThickness - (mHasVScrollBar ? mScrollBarThickness : 0) ||
      pt.y >= getHeight() - mProfile->mBorderThickness - (mHasHScrollBar ? mScrollBarThickness : 0))
      return this;
   return Parent::findHitControl(pt, initialLayer);
}

void GuiScrollCtrl::computeSizes()
{
   S32 thickness = (mProfile ? mProfile->mBorderThickness : 1);
   Point2I borderExtent(thickness, thickness);
   mContentPos = borderExtent + mChildMargin;
   mContentExt = getExtent() - (mChildMargin * 2)
                                - (borderExtent * 2);

   Point2I childLowerRight;

   mHBarEnabled = false;
   mVBarEnabled = false;
   mHasVScrollBar = (mForceVScrollBar == ScrollBarAlwaysOn);
   mHasHScrollBar = (mForceHScrollBar == ScrollBarAlwaysOn);

   setUpdate();

   if (calcChildExtents())
   {
      childLowerRight = mChildPos + mChildExt;

      if (mHasVScrollBar)
         mContentExt.x -= mScrollBarThickness;
      if (mHasHScrollBar)
         mContentExt.y -= mScrollBarThickness;
      if (mChildExt.x > mContentExt.x && (mForceHScrollBar == ScrollBarDynamic))
      {
         mHasHScrollBar = true;
         mContentExt.y -= mScrollBarThickness;
      }
      if (mChildExt.y > mContentExt.y && (mForceVScrollBar == ScrollBarDynamic))
      {
         mHasVScrollBar = true;
         mContentExt.x -= mScrollBarThickness;

         // If Extent X Changed, check Horiz Scrollbar.
         if (mChildExt.x > mContentExt.x && !mHasHScrollBar && (mForceHScrollBar == ScrollBarDynamic))
         {
            mHasHScrollBar = true;
            mContentExt.y -= mScrollBarThickness;
         }
      }
      Point2I contentLowerRight = mContentPos + mContentExt;

      // see if the child controls need to be repositioned (null space in control)
      Point2I delta(0,0);

      if (mChildPos.x > mContentPos.x)
         delta.x = mContentPos.x - mChildPos.x;
      else if (contentLowerRight.x > childLowerRight.x)
      {
         S32 diff = contentLowerRight.x - childLowerRight.x;
         delta.x = getMin(mContentPos.x - mChildPos.x, diff);
      }

      //reposition the children if the child extent > the scroll content extent
      if (mChildPos.y > mContentPos.y)
         delta.y = mContentPos.y - mChildPos.y;
      else if (contentLowerRight.y > childLowerRight.y)
      {
         S32 diff = contentLowerRight.y - childLowerRight.y;
         delta.y = getMin(mContentPos.y - mChildPos.y, diff);
      }

      // apply the deltas to the children...
      if (delta.x || delta.y)
      {
         SimGroup::iterator i;
         for(i = begin(); i != end();i++)
         {
            GuiControl *ctrl = (GuiControl *) (*i);
            ctrl->setPosition( ctrl->getPosition() + delta );
         }
         mChildPos += delta;
         childLowerRight += delta;
      }
      // enable needed scroll bars
      if (mChildExt.x > mContentExt.x)
         mHBarEnabled = true;
      if (mChildExt.y > mContentExt.y)
         mVBarEnabled = true;
      mChildRelPos = mContentPos - mChildPos;
   }
   // build all the rectangles and such...
   calcScrollRects();
   calcThumbs();
}

void GuiScrollCtrl::calcScrollRects(void)
{
   S32 thickness = ( mProfile ? mProfile->mBorderThickness : 1 );
   if (mHasHScrollBar)
   {
      mLeftArrowRect.set(thickness,
                        getHeight() - thickness - mScrollBarThickness - 1,
                        mScrollBarArrowBtnLength,
                        mScrollBarThickness);

      mRightArrowRect.set(getWidth() - thickness - (mHasVScrollBar ? mScrollBarThickness : 0) - mScrollBarArrowBtnLength,
                        getHeight() - thickness - mScrollBarThickness - 1,
                        mScrollBarArrowBtnLength,
                        mScrollBarThickness);
      mHTrackRect.set(mLeftArrowRect.point.x + mLeftArrowRect.extent.x,
                        mLeftArrowRect.point.y,
                        mRightArrowRect.point.x - (mLeftArrowRect.point.x + mLeftArrowRect.extent.x),
                        mScrollBarThickness);
   }
   if (mHasVScrollBar)
   {
      mUpArrowRect.set(getWidth() - thickness - mScrollBarThickness,
                        thickness,
                        mScrollBarThickness,
                        mScrollBarArrowBtnLength);
      mDownArrowRect.set(getWidth() - thickness - mScrollBarThickness,
                        getHeight() - thickness - mScrollBarArrowBtnLength - (mHasHScrollBar ? ( mScrollBarThickness + 1 ) : 0),
                        mScrollBarThickness,
                        mScrollBarArrowBtnLength);
      mVTrackRect.set(mUpArrowRect.point.x,
                        mUpArrowRect.point.y + mUpArrowRect.extent.y,
                        mScrollBarThickness,
                        mDownArrowRect.point.y - (mUpArrowRect.point.y + mUpArrowRect.extent.y) );
   }
}

void GuiScrollCtrl::calcThumbs()
{
   if (mHBarEnabled && mChildExt.x > 0)
   {
      U32 trackSize = mHTrackRect.len_x();

      if (mUseConstantHeightThumb)
         mHThumbSize = mBaseThumbSize;
      else
         mHThumbSize = getMax(mBaseThumbSize, S32((mContentExt.x * trackSize) / mChildExt.x));

      mHThumbPos = mHTrackRect.point.x + (mChildRelPos.x * (trackSize - mHThumbSize)) / (mChildExt.x - mContentExt.x);
   }
   if (mVBarEnabled && mChildExt.y > 0)
   {
      U32 trackSize = mVTrackRect.len_y();

      if (mUseConstantHeightThumb)
         mVThumbSize = mBaseThumbSize;
      else
         mVThumbSize = getMax(mBaseThumbSize, S32((mContentExt.y * trackSize) / mChildExt.y));

      mVThumbPos = mVTrackRect.point.y + (mChildRelPos.y * (trackSize - mVThumbSize)) / (mChildExt.y - mContentExt.y);
   }
}


void GuiScrollCtrl::scrollDelta(S32 deltaX, S32 deltaY)
{
   scrollTo(mChildRelPos.x + deltaX, mChildRelPos.y + deltaY);
}

void GuiScrollCtrl::scrollTo(S32 x, S32 y)
{
   if(!size())
      return;

   setUpdate();
   if (x > mChildExt.x - mContentExt.x)
      x = mChildExt.x - mContentExt.x;
   if (x < 0)
      x = 0;

   if (y > mChildExt.y - mContentExt.y)
      y = mChildExt.y - mContentExt.y;
   if (y < 0)
      y = 0;

   Point2I delta(x - mChildRelPos.x, y - mChildRelPos.y);
   mChildRelPos += delta;
   mChildPos -= delta;

   for(SimSet::iterator i = begin(); i != end();i++)
   {
      GuiControl *ctrl = (GuiControl *) (*i);
      ctrl->setPosition( ctrl->getPosition() - delta );
   }
   calcThumbs();
}

GuiScrollCtrl::Region GuiScrollCtrl::findHitRegion(const Point2I &pt)
{
   if (mVBarEnabled && mHasVScrollBar)
   {
      if (mUpArrowRect.pointInRect(pt))
         return UpArrow;
      else if (mDownArrowRect.pointInRect(pt))
         return DownArrow;
      else if (mVTrackRect.pointInRect(pt))
      {
         if (pt.y < mVThumbPos)
            return UpPage;
         else if (pt.y < mVThumbPos + mVThumbSize)
            return VertThumb;
         else
            return DownPage;
      }
   }
   if (mHBarEnabled && mHasHScrollBar)
   {
      if (mLeftArrowRect.pointInRect(pt))
         return LeftArrow;
      else if (mRightArrowRect.pointInRect(pt))
         return RightArrow;
      else if (mHTrackRect.pointInRect(pt))
      {
         if (pt.x < mHThumbPos)
            return LeftPage;
         else if (pt.x < mHThumbPos + mHThumbSize)
            return HorizThumb;
         else
            return RightPage;
      }
   }
   return None;
}

bool GuiScrollCtrl::wantsTabListMembership()
{
   return true;
}

bool GuiScrollCtrl::loseFirstResponder()
{
   setUpdate();
   return true;
}

bool GuiScrollCtrl::becomeFirstResponder()
{
   setUpdate();
   return mWillFirstRespond;
}

bool GuiScrollCtrl::onKeyDown(const GuiEvent &event)
{
   if (mWillFirstRespond)
   {
      switch (event.keyCode)
      {
         case KEY_RIGHT:
            scrollByRegion(RightArrow);
            return true;

         case KEY_LEFT:
            scrollByRegion(LeftArrow);
            return true;

         case KEY_DOWN:
            scrollByRegion(DownArrow);
            return true;

         case KEY_UP:
            scrollByRegion(UpArrow);
            return true;

         case KEY_PAGE_UP:
            scrollByRegion(UpPage);
            return true;

         case KEY_PAGE_DOWN:
            scrollByRegion(DownPage);
            return true;
      }
   }
   return Parent::onKeyDown(event);
}

void GuiScrollCtrl::onMouseDown(const GuiEvent &event)
{
	setUpdate();

	Point2I curMousePos = globalToLocalCoord(event.mousePoint);
	mHitRegion = findHitRegion(curMousePos);
	mStateDepressed = true;

	// Set a 0.5 second delay before we start scrolling
	mLastUpdated = Platform::getVirtualMilliseconds() + 500;

	scrollByRegion(mHitRegion);

	if (mHitRegion == VertThumb)
	{
		mChildRelPosAnchor = mChildRelPos;
		mThumbMouseDelta = curMousePos.y - mVThumbPos;
	}
	else if (mHitRegion == HorizThumb)
	{
		mChildRelPosAnchor = mChildRelPos;
		mThumbMouseDelta = curMousePos.x - mHThumbPos;
	}

	mClickOut = false;

    if (mHitRegion == None)
	{
		mClickOut = true;
		return ;
	}

	mouseLock();
}
GuiControl* GuiScrollCtrl::onHitControlMouseDown(const GuiEvent &event, const Point2I &pt, Point2I offset, RectI clipRect, S32 initialLayer)
{
	iterator i = end(); // find in z order (last to first)
	while (i != begin())
	{
		i--;
		GuiControl *ctrl = static_cast<GuiControl *>(*i);

		if( !ctrl )
			continue;

		if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
		{
			continue;
		}
		else if (ctrl->mVisible/* && ctrl->pointInControl(pt)*/)
		{
			Point2I childPosition = offset + ctrl->getPosition();
			RectI childRect( childPosition, ctrl->getExtent() + Point2I(1,1) );

			childRect.intersect( clipRect );

			if( mVBarEnabled && mHasVScrollBar )
				childRect.extent.x -= mVTrackRect.extent.x;

			if( mHBarEnabled && mHasHScrollBar )
				childRect.extent.y -=  mHTrackRect.extent.y;

			if( childRect.pointInRect( pt ) )
			{
				//Point2I ptemp = pt- ctrl->getPosition();
				GuiControl *hitCtrl = ctrl->onHitControlMouseDown( event, pt, childPosition, childRect );

				if(hitCtrl->getControlProfile()->mModal  && !hitCtrl->mClickOut )
					return hitCtrl;
			}
			//hitCtrl->mClickOut = false;
		}
	}

	onMouseDown( event );
	return this;
}

void GuiScrollCtrl::onMouseUp(const GuiEvent &)
{
   mouseUnlock();

   setUpdate();

   mHitRegion = None;
   mStateDepressed = false;
}

void GuiScrollCtrl::onMouseDragged(const GuiEvent &event)
{
   Point2I curMousePos = globalToLocalCoord(event.mousePoint);
   setUpdate();

   if ( (mHitRegion != VertThumb) && (mHitRegion != HorizThumb) )
   {
      Region hit = findHitRegion(curMousePos);
      if (hit != mHitRegion)
         mStateDepressed = false;
      else
         mStateDepressed = true;
      return;
   }

   // ok... if the mouse is 'near' the scroll bar, scroll with it
   // otherwise, snap back to the previous position.

   if (mHitRegion == VertThumb)
   {
      if (curMousePos.x >= mVTrackRect.point.x - mScrollBarDragTolerance &&
         curMousePos.x <= mVTrackRect.point.x + mVTrackRect.extent.x - 1 + mScrollBarDragTolerance &&
         curMousePos.y >= mVTrackRect.point.y - mScrollBarDragTolerance &&
         curMousePos.y <= mVTrackRect.point.y + mVTrackRect.extent.y - 1 + mScrollBarDragTolerance)
      {
         S32 newVThumbPos = curMousePos.y - mThumbMouseDelta;
         if(newVThumbPos != mVThumbPos)
         {
            S32 newVPos = (newVThumbPos - mVTrackRect.point.y) *
                          (mChildExt.y - mContentExt.y) /
                          (mVTrackRect.extent.y - mVThumbSize);

            scrollTo(mChildRelPosAnchor.x, newVPos);
         }
      }
      else
         scrollTo(mChildRelPosAnchor.x, mChildRelPosAnchor.y);
   }
   else if (mHitRegion == HorizThumb)
   {
      if (curMousePos.x >= mHTrackRect.point.x - mScrollBarDragTolerance &&
         curMousePos.x <= mHTrackRect.point.x + mHTrackRect.extent.x - 1 + mScrollBarDragTolerance &&
         curMousePos.y >= mHTrackRect.point.y - mScrollBarDragTolerance &&
         curMousePos.y <= mHTrackRect.point.y + mHTrackRect.extent.y - 1 + mScrollBarDragTolerance)
      {
         S32 newHThumbPos = curMousePos.x - mThumbMouseDelta;
         if(newHThumbPos != mHThumbPos)
         {
            S32 newHPos = (newHThumbPos - mHTrackRect.point.x) *
                          (mChildExt.x - mContentExt.x) /
                          (mHTrackRect.extent.x - mHThumbSize);

            scrollTo(newHPos, mChildRelPosAnchor.y);
         }
      }
      else
         scrollTo(mChildRelPosAnchor.x, mChildRelPosAnchor.y);
   }
}

bool GuiScrollCtrl::onMouseWheelUp(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

   Point2I previousPos = mChildPos;
   scrollByRegion((event.modifier & SI_CTRL) ? UpPage : UpArrow);

   // Tell the kids that the mouse moved (relatively):
   iterator itr;
   for ( itr = begin(); itr != end(); itr++ )
   {
      GuiControl* grandKid = static_cast<GuiControl*>( *itr );
      grandKid->onMouseMove( event );
   }

   // If no scrolling happened (already at the top), pass it on to the parent.
   GuiControl* parent = getParent();
   if (parent && (previousPos == mChildPos))
      return parent->onMouseWheelUp(event);

   return true;
}

bool GuiScrollCtrl::onMouseWheelDown(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

   Point2I previousPos = mChildPos;
   scrollByRegion((event.modifier & SI_CTRL) ? DownPage : DownArrow);

   // Tell the kids that the mouse moved (relatively):
   iterator itr;
   for ( itr = begin(); itr != end(); itr++ )
   {
      GuiControl* grandKid = static_cast<GuiControl *>( *itr );
      grandKid->onMouseMove( event );
   }

   // If no scrolling happened (already at the bottom), pass it on to the parent.
   GuiControl* parent = getParent();
   if (parent && (previousPos == mChildPos))
      return parent->onMouseWheelDown(event);

   return true;
}

void GuiScrollCtrl::onPreRender()
{
   Parent::onPreRender();

   // Short circuit if not depressed to save cycles
   if( mStateDepressed != true )
      return;

   //default to one second, though it shouldn't be necessary
   U32 timeThreshold = 1000;

   // We don't want to scroll by pages at an interval the same as when we're scrolling
   // using the arrow buttons, so adjust accordingly.
   switch( mHitRegion )
   {
   case UpPage:
   case DownPage:
   case LeftPage:
   case RightPage:
      timeThreshold = 200;
      break;
   case UpArrow:
   case DownArrow:
   case LeftArrow:
   case RightArrow:
      timeThreshold = 20;
      break;
   default:
      // Neither a button or a page, don't scroll (shouldn't get here)
      return;
      break;
   };

   S32 timeElapsed = Platform::getVirtualMilliseconds() - mLastUpdated;

   if ( ( timeElapsed > 0 ) && ( timeElapsed > timeThreshold ) )
   {

      mLastUpdated = Platform::getVirtualMilliseconds();
      scrollByRegion(mHitRegion);
   }

}

void GuiScrollCtrl::scrollByRegion(Region reg)
{
   setUpdate();
   if(!size())
      return;
   GuiControl *content = (GuiControl *) front();
   U32 rowHeight, columnWidth;
   U32 pageHeight, pageWidth;

   content->getScrollLineSizes(&rowHeight, &columnWidth);

   if(rowHeight >= mContentExt.y)
      pageHeight = 1;
   else
      pageHeight = mContentExt.y - rowHeight;

   if(columnWidth >= mContentExt.x)
      pageWidth = 1;
   else
      pageWidth = mContentExt.x - columnWidth;

   if (mVBarEnabled)
   {
      switch(reg)
      {
         case UpPage:
            scrollDelta(0, -(S32)pageHeight);
            break;
         case DownPage:
            scrollDelta(0, pageHeight);
            break;
         case UpArrow:
            scrollDelta(0, -(S32)rowHeight);
            break;
         case DownArrow:
            scrollDelta(0, rowHeight);
            break;
      }
   }

   if (mHBarEnabled)
   {
      switch(reg)
      {
         case LeftPage:
            scrollDelta(-(S32)pageWidth, 0);
            break;
         case RightPage:
            scrollDelta(pageWidth, 0);
            break;
         case LeftArrow:
            scrollDelta(-(S32)columnWidth, 0);
            break;
         case RightArrow:
            scrollDelta(columnWidth, 0);
            break;
      }
   }
}


void GuiScrollCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   RectI r(offset.x, offset.y, getWidth(), getHeight());

#ifdef STATEBLOCK
	AssertFatal(mSetSB, "GuiScrollCtrl::onRender -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
   GFX->setTextureStageMagFilter(0, GFXTextureFilterPoint);
#endif
   // draw content controls
   // create a rect to intersect with the updateRect
   RectI contentRect(mContentPos.x + offset.x, mContentPos.y + offset.y, mContentExt.x, mContentExt.y);
   contentRect.intersect(updateRect);
   
   // Always call parent
   Parent::onRender(offset, contentRect);

   // Reset the ClipRect as the parent call can modify it when rendering
   // the child controls
   GFX->setClipRect( r );

   // draw scroll bars
   if (mHasVScrollBar)
      drawVScrollBar(offset);

   if (mHasHScrollBar)
      drawHScrollBar(offset);

   //draw the scroll corner
   if (mHasVScrollBar && mHasHScrollBar)
      drawScrollCorner(offset);

}

void GuiScrollCtrl::drawBorder( const Point2I &offset, bool /*isFirstResponder*/ )
{
}

void GuiScrollCtrl::drawVScrollBar(const Point2I &offset)
{
   if(mTextureObject.isNull())
      return;

   Point2I pos = offset + mUpArrowRect.point;
   S32 bitmap = (mVBarEnabled ? ((mHitRegion == UpArrow && mStateDepressed) ?
         BmpStates * BmpUp + BmpHilite : BmpStates * BmpUp) : BmpStates * BmpUp + BmpDisabled);

   GFX->getDrawUtil()->clearBitmapModulation();
   GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[bitmap]);

   pos.y += mScrollBarArrowBtnLength;
   S32 end;
   if (mVBarEnabled)
      end = mVThumbPos + offset.y;
   else
      end = mDownArrowRect.point.y + offset.y;

   bitmap = (mVBarEnabled ? ((mHitRegion == DownPage && mStateDepressed) ?
         BmpStates * BmpVPage + BmpHilite : BmpStates * BmpVPage) : BmpStates * BmpVPage + BmpDisabled);

   if (end > pos.y)
   {
      GFX->getDrawUtil()->clearBitmapModulation();
      GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, RectI(pos, Point2I(mBitmapBounds[bitmap].extent.x, end - pos.y)), mBitmapBounds[bitmap]);
   }

   pos.y = end;
   if (mVBarEnabled)
   {
      bool thumbSelected = (mHitRegion == VertThumb && mStateDepressed);
      S32 ttop = (thumbSelected ? BmpStates * BmpVThumbTopCap + BmpHilite : BmpStates * BmpVThumbTopCap);
      S32 tmid = (thumbSelected ? BmpStates * BmpVThumb + BmpHilite : BmpStates * BmpVThumb);
      S32 tbot = (thumbSelected ? BmpStates * BmpVThumbBottomCap + BmpHilite : BmpStates * BmpVThumbBottomCap);

      // draw the thumb
      GFX->getDrawUtil()->clearBitmapModulation();
      GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[ttop]);
      pos.y += mBitmapBounds[ttop].extent.y;
      end = mVThumbPos + mVThumbSize - mBitmapBounds[tbot].extent.y + offset.y;

      if (end > pos.y)
      {
         GFX->getDrawUtil()->clearBitmapModulation();
         GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, RectI(pos, Point2I(mBitmapBounds[tmid].extent.x, end - pos.y)), mBitmapBounds[tmid]);
      }

      pos.y = end;
      GFX->getDrawUtil()->clearBitmapModulation();
      GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[tbot]);
      pos.y += mBitmapBounds[tbot].extent.y;
      end = mVTrackRect.point.y + mVTrackRect.extent.y - 1 + offset.y;

      bitmap = (mHitRegion == DownPage && mStateDepressed) ? BmpStates * BmpVPage + BmpHilite : BmpStates * BmpVPage;
      if (end > pos.y)
      {
         GFX->getDrawUtil()->clearBitmapModulation();
         GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, RectI(pos, Point2I(mBitmapBounds[bitmap].extent.x, end - pos.y)), mBitmapBounds[bitmap]);
      }

      pos.y = end;
   }

   bitmap = (mVBarEnabled ? ((mHitRegion == DownArrow && mStateDepressed ) ?
         BmpStates * BmpDown + BmpHilite : BmpStates * BmpDown) : BmpStates * BmpDown + BmpDisabled);

   GFX->getDrawUtil()->clearBitmapModulation();
   GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[bitmap]);
}

void GuiScrollCtrl::drawHScrollBar(const Point2I &offset)
{
   S32 bitmap;

   //draw the left arrow
   bitmap = (mHBarEnabled ? ((mHitRegion == LeftArrow && mStateDepressed) ?
            BmpStates * BmpLeft + BmpHilite : BmpStates * BmpLeft) : BmpStates * BmpLeft + BmpDisabled);

   Point2I pos = offset;
   pos += mLeftArrowRect.point;

   GFX->getDrawUtil()->clearBitmapModulation();
   GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[bitmap]);

   pos.x += mLeftArrowRect.extent.x;

   //draw the left page
   S32 end;
   if (mHBarEnabled)
      end = mHThumbPos + offset.x;
   else
      end = mRightArrowRect.point.x + offset.x;

   bitmap = (mHBarEnabled ? ((mHitRegion == LeftPage && mStateDepressed) ?
            BmpStates * BmpHPage + BmpHilite : BmpStates * BmpHPage) : BmpStates * BmpHPage + BmpDisabled);

   if (end > pos.x)
   {
      GFX->getDrawUtil()->clearBitmapModulation();
      GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, RectI(pos, Point2I(end - pos.x, mBitmapBounds[bitmap].extent.y)), mBitmapBounds[bitmap]);
   }
   pos.x = end;

   //draw the thumb and the rightPage
   if (mHBarEnabled)
   {
      bool thumbSelected = (mHitRegion == HorizThumb && mStateDepressed);
      S32 ttop = (thumbSelected ? BmpStates * BmpHThumbLeftCap + BmpHilite : BmpStates * BmpHThumbLeftCap );
      S32 tmid = (thumbSelected ? BmpStates * BmpHThumb + BmpHilite : BmpStates * BmpHThumb);
      S32 tbot = (thumbSelected ? BmpStates * BmpHThumbRightCap + BmpHilite : BmpStates * BmpHThumbRightCap);

      // draw the thumb
      GFX->getDrawUtil()->clearBitmapModulation();
      GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[ttop]);
      pos.x += mBitmapBounds[ttop].extent.x;
      end = mHThumbPos + mHThumbSize - mBitmapBounds[tbot].extent.x + offset.x;
      if (end > pos.x)
      {
         GFX->getDrawUtil()->clearBitmapModulation();
         GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, RectI(pos, Point2I(end - pos.x, mBitmapBounds[tmid].extent.y)), mBitmapBounds[tmid]);
      }

      pos.x = end;
      GFX->getDrawUtil()->clearBitmapModulation();
      GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[tbot]);
      pos.x += mBitmapBounds[tbot].extent.x;
      end = mHTrackRect.point.x + mHTrackRect.extent.x - 1 + offset.x;

      bitmap = ((mHitRegion == RightPage && mStateDepressed) ? BmpStates * BmpHPage + BmpHilite : BmpStates * BmpHPage);

      if (end > pos.x)
      {
         GFX->getDrawUtil()->clearBitmapModulation();
         GFX->getDrawUtil()->drawBitmapStretchSR(mTextureObject, RectI(pos, Point2I(end - pos.x, mBitmapBounds[bitmap].extent.y)), mBitmapBounds[bitmap]);
      }

      pos.x = end;
   }
   bitmap = (mHBarEnabled ? ((mHitRegion == RightArrow && mStateDepressed) ?
            BmpStates * BmpRight + BmpHilite : BmpStates * BmpRight) : BmpStates * BmpRight + BmpDisabled);

   GFX->getDrawUtil()->clearBitmapModulation();
   GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[bitmap]);
}

void GuiScrollCtrl::drawScrollCorner(const Point2I &offset)
{
   Point2I pos = offset;
   pos.x += mRightArrowRect.point.x + mRightArrowRect.extent.x;
   pos.y += mRightArrowRect.point.y;
   GFX->getDrawUtil()->clearBitmapModulation();
   GFX->getDrawUtil()->drawBitmapSR(mTextureObject, pos, mBitmapBounds[BmpStates * BmpResize]);
}

void GuiScrollCtrl::autoScroll(Region reg)
{
   scrollByRegion(reg);
}

void GuiScrollCtrl::setScrollUpArrow()
{
	scrollByRegion(UpArrow);
}

void GuiScrollCtrl::setScrollUp()
{
	scrollByRegion(UpPage);
}

void GuiScrollCtrl::setScrollDownArrow()
{ 
	scrollByRegion(DownArrow);
}

void GuiScrollCtrl::setScrollDown()
{
	scrollByRegion(DownPage);
}

void GuiScrollCtrl::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterPoint);
	GFX->endStateBlock(mSetSB);
}


void GuiScrollCtrl::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void GuiScrollCtrl::init()
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

void GuiScrollCtrl::shutdown()
{
	SAFE_DELETE(mSetSB);
}

ConsoleMethod(GuiScrollCtrl, setScrollUpArrow, void, 2, 2, "obj.setScrollUpArrow")
{
	object->setScrollUpArrow();
}

ConsoleMethod(GuiScrollCtrl, setScrollUp, void ,2 ,2 ,"obj.setScrollUp")
{
	object->setScrollUp();
}

ConsoleMethod(GuiScrollCtrl, setScrollDownArrow, void, 2, 2, "obj.setScrollDownArrow")
{
	object->setScrollDownArrow();
}

ConsoleMethod(GuiScrollCtrl, setScrollDown, void, 2, 2, "obj.setScrollDown")
{
	object->setScrollDown();
}