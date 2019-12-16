//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "platform/event.h"
#include "gui/containers/guiScrollCtrl.h"
#include "ui/guiArrayCtrlEx.h"

IMPLEMENT_CONOBJECT(GuiArrayCtrlEx);

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

GuiArrayCtrlEx::GuiArrayCtrlEx()
{
   mActive = true;

   mCellUnit.set(80, 10);
   mSize = Point2I(5, 30);
   mSelectedCell.set(-1, -1);
   mMouseOverCell.set(-1, -1);
   mHeaderDim.set(0, 0);
   mIsContainer = true;
}

void GuiArrayCtrlEx::initPersistFields()
{
	Parent::initPersistFields();
	addField("CellUnit",               TypePoint2I, Offset(mCellUnit, GuiArrayCtrlEx));
}
bool GuiArrayCtrlEx::onWake()
{
   if (! Parent::onWake())
      return false;

   //get the font
   mFont = mProfile->mFont;

   return true;
}

void GuiArrayCtrlEx::onSleep()
{
   Parent::onSleep();
   mFont = NULL;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiArrayCtrlEx::setSize(Point2I newSize)
{
   mSize = newSize;
   Point2I newExtent(newSize.x * mCellUnit.x + mHeaderDim.x, newSize.y * mCellUnit.y + mHeaderDim.y);

   setExtent(newExtent);
}

void GuiArrayCtrlEx::getScrollDimensions(S32 &cell_size, S32 &num_cells)
{
   cell_size = mCellUnit.y;
   num_cells = mSize.y;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiArrayCtrlEx::cellSelected(Point2I cell)
{
   if (cell.x < 0 || cell.x >= mSize.x || cell.y < 0 || cell.y >= mSize.y)
   {
      mSelectedCell = Point2I(-1,-1);
      return false;
   }

   mSelectedCell = cell;
   scrollSelectionVisible();
   onCellSelected(cell);
	setUpdate();
   return true;
}

void GuiArrayCtrlEx::onCellSelected(Point2I cell)
{
  	Con::executef(this, "onSelect", Con::getFloatArg(cell.x), Con::getFloatArg(cell.y));

   //call the console function
   execConsoleCallback();
}

// Called when a cell is highlighted
void GuiArrayCtrlEx::onCellHighlighted(Point2I cell)
{
	// Do nothing
}

void GuiArrayCtrlEx::setSelectedCell(Point2I cell)
{
   cellSelected(cell);
}

Point2I GuiArrayCtrlEx::getSelectedCell()
{
   return mSelectedCell;
}

void GuiArrayCtrlEx::scrollSelectionVisible()
{
   scrollCellVisible(mSelectedCell);
}

void GuiArrayCtrlEx::scrollCellVisible(Point2I cell)
{
   //make sure we have a parent
   //make sure we have a valid cell selected
   GuiScrollCtrl *parent = dynamic_cast<GuiScrollCtrl*>(getParent());
   if(!parent || cell.x < 0 || cell.y < 0)
      return;

   RectI cellBounds(cell.x * mCellUnit.x, cell.y * mCellUnit.y, mCellUnit.x, mCellUnit.y);
   parent->scrollRectVisible(cellBounds);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiArrayCtrlEx::onRenderColumnHeaders(Point2I offset, Point2I parentOffset, Point2I headerDim)
{
   if (mProfile->mBorder)
   {
      RectI cellR(offset.x + headerDim.x, parentOffset.y, getWidth() - headerDim.x, headerDim.y);
      GFX->getDrawUtil()->drawRectFill(cellR, mProfile->mBorderColor);
   }
}

void GuiArrayCtrlEx::onRenderRowHeader(Point2I offset, Point2I parentOffset, Point2I headerDim, Point2I cell)
{
   ColorI color;
   RectI cellR;
   if (cell.x % 2)
      color.set(255, 0, 0, 255);
   else
      color.set(0, 255, 0, 255);

   cellR.point.set(parentOffset.x, offset.y);
   cellR.extent.set(headerDim.x, mCellUnit.y);
   GFX->getDrawUtil()->drawRectFill(cellR, color);
}

void GuiArrayCtrlEx::onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver)
{
   ColorI color(255 * (cell.x % 2), 255 * (cell.y % 2), 255 * ((cell.x + cell.y) % 2), 255);
   if (selected)
   {
      color.set(255, 0, 0, 255);
   }
   else if (mouseOver)
   {
      color.set(0, 0, 255, 255);
   }

   //draw the cell
   RectI cellR(offset.x, offset.y, mCellUnit.x, mCellUnit.y);
   GFX->getDrawUtil()->drawRectFill(cellR, color);
}

void GuiArrayCtrlEx::onRender(Point2I offset, const RectI &updateRect)
{

   Parent::onRender( offset, updateRect );

   //make sure we have a parent
   GuiControl *parent = getParent();
   if (! parent)
      return;

   S32 i, j;
   RectI headerClip;
   RectI clipRect(updateRect.point, updateRect.extent);

   Point2I parentOffset = parent->localToGlobalCoord(Point2I(0, 0));

   //if we have column headings
   if (mHeaderDim.y > 0)
   {
      headerClip.point.x =   parentOffset.x + mHeaderDim.x;
      headerClip.point.y =   parentOffset.y;
      headerClip.extent.x =  clipRect.extent.x;// - headerClip.point.x; // This seems to fix some strange problems with some Gui's, bug? -pw
      headerClip.extent.y =  mHeaderDim.y;

      if (headerClip.intersect(clipRect))
      {
         GFX->setClipRect(headerClip);

         //now render the header
         onRenderColumnHeaders(offset, parentOffset, mHeaderDim);

         clipRect.point.y = headerClip.point.y + headerClip.extent.y - 1;
      }
      offset.y += mHeaderDim.y;
   }

   //if we have row headings
   if (mHeaderDim.x > 0)
   {
      clipRect.point.x = getMax(clipRect.point.x, parentOffset.x + mHeaderDim.x);
      offset.x += mHeaderDim.x;
   }

   //save the original for clipping the row headers
   RectI origClipRect = clipRect;

   for (j = 0; j < mSize.y; j++)
   {
      //skip until we get to a visible row
      if (getRowCount(j+1) * mCellUnit.y + offset.y < updateRect.point.y)
         continue;

      //break once we've reached the last visible row
      if(getRowCount(j) * mCellUnit.y + offset.y >= updateRect.point.y + updateRect.extent.y)
         break;

      //render the header
      if (mHeaderDim.x > 0)
      {
         headerClip.point.x = parentOffset.x;
         headerClip.extent.x = mHeaderDim.x;
         headerClip.point.y = offset.y + getRowCount(j) * mCellUnit.y;
         headerClip.extent.y = mCellUnit.y;
         if (headerClip.intersect(origClipRect))
         {
            GFX->setClipRect(headerClip);

            //render the row header
            onRenderRowHeader(Point2I(0, offset.y + getRowCount(j) * mCellUnit.y),
                              Point2I(parentOffset.x, offset.y + getRowCount(j) * mCellUnit.y),
                              mHeaderDim, Point2I(0, j));
         }
      }

      //render the cells for the row
      for (i = 0; i < mSize.x; i++)
      {
         //skip past columns off the left edge
         if ((i + 1) * mCellUnit.x + offset.x < updateRect.point.x)
            continue;

         //break once past the last visible column
         if (i * mCellUnit.x + offset.x >= updateRect.point.x + updateRect.extent.x)
            break;

         S32 cellx = offset.x + i * mCellUnit.x;
         S32 celly = offset.y + getRowCount(j) * mCellUnit.y;

         RectI cellClip(cellx, celly, mCellUnit.x, mCellUnit.y * getRowNumber(j));

         //make sure the cell is within the update region
         if (cellClip.intersect(clipRect))
         {
            //set the clip rect
            GFX->setClipRect(cellClip);

            //render the cell
            onRenderCell(Point2I(cellx, celly), Point2I(i, j),
               i == mSelectedCell.x && j == mSelectedCell.y,
               i == mMouseOverCell.x && j == mMouseOverCell.y);
         }
      }
   }
}

void GuiArrayCtrlEx::onMouseDown(const GuiEvent &event)
{
   if ( !mActive || !mAwake || !mVisible )
      return;

   //let the guiControl method take care of the rest
   Parent::onMouseDown(event);

   Point2I pt = globalToLocalCoord(event.mousePoint);
   pt.x -= mHeaderDim.x; pt.y -= mHeaderDim.y;

   int x = (pt.x < 0) ? -1 : pt.x / mCellUnit.x;
   int y = (pt.y < 0) ? -1 : pt.y / mCellUnit.y;
   Point2I cell(x, getEntryIndex(y));
   if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
   {

      //store the previously selected cell
      Point2I prevSelected = mSelectedCell;

      //select the new cell
      cellSelected(Point2I(cell.x, cell.y));

      //if we double clicked on the *same* cell, evaluate the altConsole Command
      if ( ( event.mouseClickCount > 1 ) && ( prevSelected == mSelectedCell ) && mAltConsoleCommand[0] )
         Con::evaluate( mAltConsoleCommand, false );
   }
}

void GuiArrayCtrlEx::onMouseEnter(const GuiEvent &event)
{
   Point2I pt = globalToLocalCoord(event.mousePoint);
   pt.x -= mHeaderDim.x; pt.y -= mHeaderDim.y;

   //get the cell
   int x = (pt.x < 0) ? -1 : pt.x / mCellUnit.x;
   int y = (pt.y < 0) ? -1 : pt.y / mCellUnit.y;
   Point2I cell(x, getEntryIndex(y));
   if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
   {
      mMouseOverCell = cell;
      setUpdateRegion(Point2I(cell.x * mCellUnit.x + mHeaderDim.x,
                              cell.y * mCellUnit.y + mHeaderDim.y), mCellUnit );
	  onCellHighlighted(mMouseOverCell);
   }
}

void GuiArrayCtrlEx::onMouseLeave(const GuiEvent & /*event*/)
{
	Point2I extent = mCellUnit;
	extent.y *= getRowNumber(mMouseOverCell.y);
   setUpdateRegion(Point2I(mMouseOverCell.x * mCellUnit.x + mHeaderDim.x,
                           mMouseOverCell.y * mCellUnit.y + mHeaderDim.y), extent);
   mMouseOverCell.set(-1,-1);
   onCellHighlighted(mMouseOverCell);
}

void GuiArrayCtrlEx::onMouseDragged(const GuiEvent &event)
{
   // for the array control, the behaviour of onMouseDragged is the same
   // as on mouse moved - basically just recalc the currend mouse over cell
   // and set the update regions if necessary
   GuiArrayCtrlEx::onMouseMove(event);
}

void GuiArrayCtrlEx::onMouseMove(const GuiEvent &event)
{
   Point2I pt = globalToLocalCoord(event.mousePoint);
   pt.x -= mHeaderDim.x; pt.y -= mHeaderDim.y;
   int x = (pt.x < 0) ? -1 : pt.x / mCellUnit.x;
   int y = (pt.y < 0) ? -1 : pt.y / mCellUnit.y;
   Point2I cell(x, getEntryIndex(y));
   if (cell.x != mMouseOverCell.x || cell.y != mMouseOverCell.y)
   {
      if (mMouseOverCell.x != -1)
      {
		  Point2I extent = mCellUnit;
		  extent.y *= getRowNumber(cell.y);
         setUpdateRegion(Point2I(mMouseOverCell.x * mCellUnit.x + mHeaderDim.x,
                           mMouseOverCell.y * mCellUnit.y + mHeaderDim.y), extent);
      }

      if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
      {
		  Point2I extent = mCellUnit;
		  extent.y *= getRowNumber(cell.y);
         setUpdateRegion(Point2I(cell.x * mCellUnit.x + mHeaderDim.x,
                           cell.y * mCellUnit.y + mHeaderDim.y), extent);
         mMouseOverCell = cell;
      }
      else
         mMouseOverCell.set(-1,-1);
   }
   onCellHighlighted(mMouseOverCell);
}

bool GuiArrayCtrlEx::onKeyDown(const GuiEvent &event)
{
   //if this control is a dead end, kill the event
   if ((! mVisible) || (! mActive) || (! mAwake)) return true;

   //get the parent
   S32 pageSize = 1;
   GuiControl *parent = getParent();
   if (parent && mCellUnit.y > 0)
   {
      pageSize = getMax(1, (parent->getHeight() / mCellUnit.y) - 1);
   }

   Point2I delta(0,0);
   switch (event.keyCode)
   {
      case KEY_LEFT:
         delta.set(-1, 0);
         break;
      case KEY_RIGHT:
         delta.set(1, 0);
         break;
      case KEY_UP:
         delta.set(0, -1);
         break;
      case KEY_DOWN:
         delta.set(0, 1);
         break;
      case KEY_PAGE_UP:
         delta.set(0, -pageSize);
         break;
      case KEY_PAGE_DOWN:
         delta.set(0, pageSize);
         break;
      case KEY_HOME:
         cellSelected( Point2I( 0, 0 ) );
         return( true );
      case KEY_END:
         cellSelected( Point2I( 0, mSize.y - 1 ) );
         return( true );
      default:
         return Parent::onKeyDown(event);
   }
   if (mSize.x < 1 || mSize.y < 1)
      return true;

   //select the first cell if no previous cell was selected
   if (mSelectedCell.x == -1 || mSelectedCell.y == -1)
   {
      cellSelected(Point2I(0,0));
      return true;
   }

   //select the cell
   Point2I cell = mSelectedCell;
   cell.x = getMax(0, getMin(mSize.x - 1, cell.x + delta.x));
   cell.y = getMax(0, getMin(mSize.y - 1, cell.y + delta.y));
   cell.y = getEntryIndex(cell.y);
   cellSelected(cell);

   return true;
}

void GuiArrayCtrlEx::onRightMouseDown(const GuiEvent &event)
{
   if ( !mActive || !mAwake || !mVisible )
      return;

   Parent::onRightMouseDown( event );

   Point2I pt = globalToLocalCoord( event.mousePoint );
   pt.x -= mHeaderDim.x; pt.y -= mHeaderDim.y;

   int x = (pt.x < 0) ? -1 : pt.x / mCellUnit.x;
   int y = (pt.y < 0) ? -1 : pt.y / mCellUnit.y;
   Point2I cell(x, getEntryIndex(y));
   if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
   {
      char buf[32];
      dSprintf( buf, sizeof( buf ), "%d %d", event.mousePoint.x, event.mousePoint.y );
      // Pass it to the console:
  	   Con::executef(this, "onRightMouseDown", Con::getIntArg(cell.x), Con::getIntArg(cell.y), buf);
   }
}
