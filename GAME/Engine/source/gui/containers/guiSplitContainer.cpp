//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/containers/guiSplitContainer.h"
#include "gui/core/guiCanvas.h"
#include "console/consoleTypes.h"

//-----------------------------------------------------------------------------
// GuiSplitContainer
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiSplitContainer);

GuiSplitContainer::GuiSplitContainer()
{
   setMinExtent(Point2I(64,64) );
   setExtent(200,200);

   setDocking( Docking::dockNone );

   // We only support client docked items in a split container
   mValidDockingMask = Docking::dockClient;
   mFixedPanel       = panelFirst;
   mFixedPanelSize  = 100;
   mSplitterType     = splitterVertical;
   mSplitterSize     = 2;
   mSplitPoint       = Point2I(0,0);
   mSplitRect        = RectI(0,0, mSplitterSize,mSplitterSize);
   mDragging         = false;


}

GuiSplitContainer::~GuiSplitContainer()
{
}

void GuiSplitContainer::initPersistFields()
{
   Con::setIntVariable("$SPLITTER_VERT",   splitterVertical);
   Con::setIntVariable("$SPLITTER_HORZ",   splitterHorizontal);

   addGroup("Splitter", "Options to configure split panels contained by this control");
   addField("SplitterType", TypeS32, Offset( mSplitterType, GuiSplitContainer), "Vertical/Horizontal Splitter" );
   addField("SplitterSize", TypeS32, Offset( mSplitterSize, GuiSplitContainer), "The size of the splitter" );
   addField("FixedPanel", TypeS32, Offset( mFixedPanel, GuiSplitContainer), "(1/2) Which panel will stay fixed on parent resizing");
   addField("FixedSize", TypeS32, Offset( mFixedPanelSize, GuiSplitContainer), "The total width of the fixed panel" );
   endGroup("Splitter");

   Parent::initPersistFields();
}

bool GuiSplitContainer::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   if( mSplitterType == splitterHorizontal )
      mSplitPoint = Point2I( 100, 300 );
   else
      mSplitPoint = Point2I( 300, 100 );

   return true;
}

bool GuiSplitContainer::onWake()
{
   if( !Parent::onWake() )
      return false;

   // Create Panel 1
   if( empty() )
   {
      GuiPanel *newPanel = new GuiPanel();
      AssertFatal( newPanel, "GuiSplitContainer::onAdd - Cannot create subordinate panel #1!" );
      newPanel->registerObject();
      newPanel->setInternalName( "Panel1" );
      newPanel->setDocking( Docking::dockClient );
      addObject( (SimObject*)newPanel );
   }
   else
   {
      GuiContainer *containerCtrl = dynamic_cast<GuiContainer*>( at(0) );
      if( containerCtrl )
      {
         containerCtrl->setInternalName("Panel1");
         containerCtrl->setDocking( Docking::dockClient );
      }
   }

   if( size() == 1 )
   {
      GuiPanel *newPanel = new GuiPanel();

      // Create Panel 2
      newPanel = new GuiPanel();
      AssertFatal( newPanel, "GuiSplitContainer::onAdd - Cannot create subordinate panel #2!" );
      newPanel->registerObject();
      newPanel->setInternalName( "Panel2" );
      newPanel->setDocking( Docking::dockClient );
      addObject( (SimObject*)newPanel );
   }
   else
   {
      GuiContainer *containerCtrl = dynamic_cast<GuiContainer*>( at(1) );
      if( containerCtrl )
      {
         containerCtrl->setInternalName("Panel2");
         containerCtrl->setDocking( Docking::dockClient );
      }
   }

   // Has FixedWidth been specified?
   if( mFixedPanelSize == 0 )
   {
      // Nope, so try to guess as best we can
      GuiContainer *firstPanel = dynamic_cast<GuiContainer*>( at(0) );
      GuiContainer *secondPanel = dynamic_cast<GuiContainer*>( at(1) );
      if( mFixedPanel == panelFirst )
      {
         if ( mSplitterType == splitterHorizontal )
            mFixedPanelSize = firstPanel->getExtent().y;
         else
            mFixedPanelSize = firstPanel->getExtent().x;

         mSplitPoint = Point2I( mFixedPanelSize, mFixedPanelSize );
      }
      else if( mFixedPanel == panelSecond )
      {
         if ( mSplitterType == splitterHorizontal )
            mFixedPanelSize = getExtent().y - secondPanel->getExtent().y;
         else
            mFixedPanelSize = getExtent().x - secondPanel->getExtent().x;

         mSplitPoint = getExtent() - Point2I( mFixedPanelSize, mFixedPanelSize );
      }

   }


   setUpdateLayout();

   return true;
}

void GuiSplitContainer::onRender(Point2I offset, const RectI &updateRect)
{
   Parent::onRender( offset, updateRect );

   // Only render if we're dragging the splitter
   if( mDragging && mSplitRect.isValidRect() )
   {
      // Splitter Rectangle (will adjust positioning only)
      RectI splitterRect = mSplitRect;

      // Currently being dragged to Rect 
      Point2I splitterPoint = localToGlobalCoord( mSplitRect.point );
      splitterRect.point = localToGlobalCoord( mSplitPoint );

      RectI clientRect = getClientRect();
      clientRect.point = localToGlobalCoord( clientRect.point );

      if( mSplitterType == splitterHorizontal ) 
      {
         splitterRect.point.y -= mSplitterSize;
         splitterRect.point.x = splitterPoint.x;
      }
      else
      {
         splitterRect.point.x -= mSplitterSize;
         splitterRect.point.y = splitterPoint.y;
      }

      RectI oldClip = GFX->getClipRect();
      GFX->setClipRect( clientRect );
      GFX->getDrawUtil()->drawRectFill( splitterRect, ColorI( 100,100,100,150 ));
      GFX->setClipRect( oldClip );

   }
   else
      GFX->getDrawUtil()->drawRectFill( mSplitRect, ColorI( 40, 40, 40, 200));
}

Point2I GuiSplitContainer::getMinExtent()
{
   GuiContainer *panelOne = dynamic_cast<GuiContainer*>( at(0) );
   GuiContainer *panelTwo = dynamic_cast<GuiContainer*>( at(1) );

   if( !panelOne || !panelTwo )
      return Parent::getMinExtent();

   Point2I minExtent = Point2I(0,0);
   Point2I panelOneMinExtent = panelOne->getMinExtent();
   Point2I panelTwoMinExtent = panelTwo->getMinExtent();

   if( mSplitterType == splitterHorizontal )
   {
      minExtent.y = mSplitterSize + panelOneMinExtent.y + panelTwoMinExtent.y;
      minExtent.x = getMax( panelOneMinExtent.x, panelTwoMinExtent.x );
   }
   else
   {
      minExtent.x = mSplitterSize + panelOneMinExtent.x + panelTwoMinExtent.x;
      minExtent.y = getMax( panelOneMinExtent.y, panelTwoMinExtent.y );
   }

   return minExtent;
}

// GuiSplitContainer overrides parentResized to make sure that the proper fixed frame's width/height
// is not compromised in the call
void GuiSplitContainer::parentResized(const RectI &oldParentRect, const RectI &newParentRect)
{
   if( size() < 2 )
      return;
   GuiContainer *panelOne = dynamic_cast<GuiContainer*>( at(0) );
   GuiContainer *panelTwo = dynamic_cast<GuiContainer*>( at(1) );

   AssertFatal( panelOne && panelTwo, "GuiSplitContainer::parentResized - Missing/Invalid Subordinate Controls! Split contained controls must derive from GuiContainer!" );

   Point2I newDragPos;
   if( mFixedPanel == panelFirst )
      newDragPos = panelOne->getExtent();
   else
      newDragPos = getExtent() - panelTwo->getExtent();
  
   RectI clientRect = getClientRect();
   solvePanelConstraints( newDragPos, panelOne, panelTwo, clientRect );

   setUpdateLayout();

   Parent::parentResized( oldParentRect, newParentRect );
}

bool GuiSplitContainer::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   if( !Parent::resize(newPosition, newExtent) || size() < 2 )
      return false;

   GuiContainer *panelOne = dynamic_cast<GuiContainer*>( at(0) );
   GuiContainer *panelTwo = dynamic_cast<GuiContainer*>( at(1) );
	
   // 
   AssertFatal( panelOne && panelTwo, "GuiSplitContainer::resize - Missing/Invalid Subordinate Controls! Split contained controls must derive from GuiContainer!" );

   if( !panelOne || !panelTwo )
	   return true;

   // We only need to update the split point if our second panel is fixed.  
   // If the first is fixed, then we can leave the split point alone because
   // the remainder of the size will be added to or taken from the second panel
   Point2I newDragPos;
   if( mFixedPanel == panelSecond )
   {
      RectI bound = getBounds();
      RectI boundsNew = RectI( newPosition, newExtent );
      
      S32 deltaX = newExtent.x - getExtent().x;
      S32 deltaY = newExtent.y - getExtent().y;

      if( mSplitterType == splitterHorizontal )
         mSplitPoint.y += deltaY;
      else
         mSplitPoint.x += deltaX;
   }

   // If we got here, parent returned true
   return true;
}


bool GuiSplitContainer::layoutControls( RectI &clientRect )
{
   if( size() < 2 )
      return false;

   GuiContainer *panelOne = dynamic_cast<GuiContainer*>( at(0) );
   GuiContainer *panelTwo = dynamic_cast<GuiContainer*>( at(1) );

   // 
   AssertFatal( panelOne && panelTwo, "GuiSplitContainer::layoutControl - Missing/Invalid Subordinate Controls! Split contained controls must derive from GuiContainer!" );

   RectI panelOneRect = RectI( clientRect.point, Point2I( 0, 0 ) );
   RectI panelTwoRect;
   RectI splitRect;

   solvePanelConstraints( getSplitPoint(), panelOne, panelTwo, clientRect );

   switch( mSplitterType )
   {
   case splitterHorizontal:
      panelOneRect.extent = Point2I( clientRect.extent.x, getSplitPoint().y );
      panelTwoRect = panelOneRect;
      panelTwoRect.intersect( clientRect );
      panelTwoRect.point.y = panelOneRect.extent.y;
      panelTwoRect.extent.y = clientRect.extent.y - panelOneRect.extent.y;

      // Generate new Splitter Rectangle
      splitRect = panelTwoRect;
      splitRect.extent.y = 0;
      splitRect.inset( 0, -mSplitterSize );

      panelOneRect.extent.y -= mSplitterSize;
      panelTwoRect.point.y += mSplitterSize;
      panelTwoRect.extent.y -= mSplitterSize;

      break;
   case splitterVertical:
      panelOneRect.extent = Point2I( getSplitPoint().x, clientRect.extent.y );
      panelTwoRect = panelOneRect;
      panelTwoRect.intersect( clientRect );
      panelTwoRect.point.x = panelOneRect.extent.x;
      panelTwoRect.extent.x = clientRect.extent.x - panelOneRect.extent.x;

      // Generate new Splitter Rectangle
      splitRect = panelTwoRect;
      splitRect.extent.x = 0;
      splitRect.inset( -mSplitterSize, 0 );

      panelOneRect.extent.x -= mSplitterSize;
      panelTwoRect.point.x += mSplitterSize;
      panelTwoRect.extent.x -= mSplitterSize;


      break;
   }

   // Update Split Rect
   mSplitRect = splitRect;

   // Dock Appropriately
   if( !( mFixedPanel == panelFirst && !panelOne->isVisible()) )
      dockControl( panelOne, panelOne->getDocking(), panelOneRect );
   if( !( mFixedPanel == panelSecond && !panelTwo->isVisible()) )
      dockControl( panelTwo, panelOne->getDocking(), panelTwoRect );   

   // 
   return false;
}

void GuiSplitContainer::solvePanelConstraints( Point2I newDragPos, GuiContainer * firstPanel, GuiContainer * secondPanel, RectI clientRect )
{
   if( !firstPanel || !secondPanel )
      return;

   if ( mSplitterType == splitterHorizontal )
   {
      // Constrain based on Y axis (Horizontal Splitter)

      // This accounts for the splitter width 
      S32 splitterSize = (S32)(mSplitRect.extent.y * 0.5);

      // Collapsed fixed panel
      if( mFixedPanel == panelSecond && !secondPanel->isVisible() )
      {
         newDragPos = Point2I(mSplitPoint.x, getExtent().y - splitterSize );
      }
      else if( mFixedPanel == panelFirst && !firstPanel->isVisible() )
      {
         newDragPos = Point2I(mSplitPoint.x, splitterSize );
      }
      else // Normal constraints
      {

         S32 newPosition = mClamp( newDragPos.y, firstPanel->getMinExtent().y + splitterSize,
            getExtent().y - secondPanel->getMinExtent().y - splitterSize );
         newDragPos = Point2I( mSplitPoint.x, newPosition );
      }
   }
   else
   {
      // Constrain based on X axis (Vertical Splitter)

      // This accounts for the splitter width 
      S32 splitterSize = (S32)(mSplitRect.extent.x * 0.5);
      // Collapsed fixed panel
      if( mFixedPanel == panelSecond && !secondPanel->isVisible() )
      {
         newDragPos = Point2I(getExtent().x - splitterSize, mSplitPoint.y  );

      }
      else if( mFixedPanel == panelFirst && !firstPanel->isVisible() )
      {
         newDragPos = Point2I( splitterSize, mSplitPoint.x );

      }
      else // Normal constraints
      {

         S32 newPosition = mClamp( newDragPos.x, firstPanel->getMinExtent().x + splitterSize,
            getExtent().x - secondPanel->getMinExtent().x - splitterSize );
         newDragPos = Point2I( newPosition, mSplitPoint.y );
      }
   }

   // Just incase, clamp to bounds of controls
   newDragPos.x = mClamp( newDragPos.x, clientRect.point.x, clientRect.point.x + clientRect.extent.x );
   newDragPos.y = mClamp( newDragPos.y, clientRect.point.y, clientRect.point.y + clientRect.extent.y );

   mSplitPoint = newDragPos;
}


void GuiSplitContainer::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
   GuiCanvas *rootCtrl = getRoot();
   if( !rootCtrl )
      return;

   S32 desiredCursor = 0;
   RectI splitRect = getSplitRect();

   // Figure out which cursor we want if we need one
   if( mSplitterType == splitterHorizontal )
      desiredCursor = PlatformCursorController::curResizeHorz;
   else if( mSplitterType == splitterVertical )
      desiredCursor = PlatformCursorController::curResizeVert;

   PlatformWindow *platformWindow = static_cast<GuiCanvas*>(getRoot())->getPlatformWindow();
   AssertFatal(platformWindow != NULL,"GuiControl without owning platform window!  This should not be possible.");
   PlatformCursorController *cusrorController = platformWindow->getCursorController();
   AssertFatal(cusrorController != NULL,"PlatformWindow without an owned CursorController!");

   // Check to see if we need one or just the default
   Point2I localPoint = Point2I( globalToLocalCoord( lastGuiEvent.mousePoint ) );
   if( splitRect.pointInRect( localPoint ) || mDragging  )
   {
      // Do we need to change it or is it already set?
      if( rootCtrl->mCursorChanged != desiredCursor )
      {
         // We've already changed the cursor, so set it back
         if( rootCtrl->mCursorChanged != -1 )
            cusrorController->popCursor();

		 cusrorController->pushCursor(cusrorController->getSysCursor(desiredCursor));
         rootCtrl->mCursorChanged = desiredCursor;
      }
   }
   else if(rootCtrl->mCursorChanged != -1)
   {
      // Just the default
      cusrorController->popCursor();
      rootCtrl->mCursorChanged = -1;
   }

}

//-----------------------------------------------------------------------------
// Mouse Events
//-----------------------------------------------------------------------------
void GuiSplitContainer::onMouseDown(const GuiEvent &event)
{

   GuiContainer *firstPanel = dynamic_cast<GuiContainer*>(at(0));
   GuiContainer *secondPanel = dynamic_cast<GuiContainer*>(at(1));

   // This function will constrain the panels to their minExtents and update the mSplitPoint
   if( firstPanel && secondPanel )
   {
      mouseLock();
      mDragging = true;

      RectI clientRect = getClientRect();
      Point2I newDragPos = globalToLocalCoord( event.mousePoint );

      solvePanelConstraints(newDragPos, firstPanel, secondPanel, clientRect);
   }
}

//-----------------------------------------------------------------------------
void GuiSplitContainer::onMouseUp(const GuiEvent &event)
{
   // If we've been dragging, we need to update the fixed panel extent.  
   // NOTE : This should ONLY ever happen in this function.  the Fixed panel
   // is to REMAIN FIXED unless the user changes it.
   if( mDragging )
   {
      Point2I newSplitPoint = getSplitPoint();

      // Update Fixed Panel Extent
      if( mFixedPanel == panelFirst )
         mFixedPanelSize = ( mSplitterType == splitterHorizontal ) ? newSplitPoint.y : newSplitPoint.x;
      else
         mFixedPanelSize = ( mSplitterType == splitterHorizontal ) ? getExtent().y - newSplitPoint.y : getExtent().x - newSplitPoint.x;

      setUpdateLayout();
   }

   mDragging = false;
   mouseUnlock();

}

//-----------------------------------------------------------------------------
void GuiSplitContainer::onMouseDragged(const GuiEvent &event)
{
   GuiContainer *firstPanel = dynamic_cast<GuiContainer*>(at(0));
   GuiContainer *secondPanel = dynamic_cast<GuiContainer*>(at(1));

   // This function will constrain the panels to their minExtents and update the mSplitPoint
   if( mDragging && firstPanel && secondPanel )
   {
      RectI clientRect = getClientRect();
      Point2I newDragPos = globalToLocalCoord( event.mousePoint );

      solvePanelConstraints(newDragPos, firstPanel, secondPanel, clientRect);
   }
}
