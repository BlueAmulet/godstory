//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/consoleTypes.h"
#include "gui/core/guiCanvas.h"
#include "gui/containers/guiTabBookCtrl.h"
#include "gui/editor/guiEditCtrl.h"
#include "gui/controls/guiPopUpCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/core/guiControl.h"
#include "console/simObject.h"
#include "gfx/gfxFontRenderBatcher.h"

// So we can set tab alignment via gui editor
static const EnumTable::Enums tabAlignEnums[] =
{
   { GuiTabBookCtrl::AlignTop,   "Top"    },
   { GuiTabBookCtrl::AlignBottom,"Bottom" }
};
static const EnumTable gTabAlignEnums(2,&tabAlignEnums[0]);


IMPLEMENT_CONOBJECT(GuiTabBookCtrl);

GuiTabBookCtrl::GuiTabBookCtrl()
{
   VECTOR_SET_ASSOCIATION(mPages);
   mTabHeight = 24;
   mTabPosition = GuiTabBookCtrl::AlignTop;
   mActivePage = NULL;
   mHoverTab = NULL;
   mHasTexture = false;
   mBitmapBounds = NULL;
   setExtent( 400, 300 );
   mPageRect = RectI(0,0,0,0);
   mTabRect = RectI(0,0,0,0);

   mPages.reserve(12);
   mTabMargin = 7;
   mMinTabWidth = 64;
   mIsContainer = true;


}

void GuiTabBookCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("TabPosition",		TypeEnum,		Offset(mTabPosition,GuiTabBookCtrl), 1, &gTabAlignEnums );
   addField("TabMargin",   TypeS32,    Offset(mTabMargin,GuiTabBookCtrl));
   addField("MinTabWidth", TypeS32,    Offset(mMinTabWidth,GuiTabBookCtrl));
}


// Empty for now, will implement for handling design time context menu for manipulating pages
ConsoleMethod( GuiTabBookCtrl, addPage, void, 2, 2, "(no arguments expected)")
{
   object->addNewPage();
}

//ConsoleMethod( GuiTabBookCtrl, removePage, void, 2, 2, "()")
//{
//}


bool GuiTabBookCtrl::onAdd()
{
   Parent::onAdd();

   return true;
}


void GuiTabBookCtrl::onRemove()
{
   Parent::onRemove();
}

void GuiTabBookCtrl::onChildRemoved( GuiControl* child )
{
   for (S32 i = 0; i < mPages.size(); i++ )
   {
      GuiTabPageCtrl* tab = mPages[i].Page;
      if( tab == child )
      {
         if( tab == mActivePage )
            mActivePage = NULL;
         mPages.erase( i );
         break;
      }
   }

   if( mPages.empty() )
      mActivePage = NULL;
   else if (mActivePage == NULL )
      mActivePage = static_cast<GuiTabPageCtrl*>(mPages[0].Page);

}

void GuiTabBookCtrl::onChildAdded( GuiControl *child )
{
   GuiTabPageCtrl *page = dynamic_cast<GuiTabPageCtrl*>(child);
   if( !page )
   {
      Con::warnf("GuiTabBookCtrl::onChildAdded - attempting to add NON GuiTabPageCtrl as child page");
      SimObject *simObj = reinterpret_cast<SimObject*>(child);
      removeObject( simObj );
      if( mActivePage )
      {
         mActivePage->addObject( simObj );
      }
      else
      {
         Con::warnf("GuiTabBookCtrl::onChildAdded - unable to find active page to reassign ownership of new child control to, placing on parent");
         GuiControl *rent = getParent();
         if( rent )
            rent->addObject( simObj );
      }
      return;
   }


   TabHeaderInfo newPage;

   newPage.Page      = page;
   newPage.TabRow    = -1;
   newPage.TabColumn = -1;

   mPages.push_back( newPage );

   // Calculate Page Information
   calculatePageTabs();

   //child->resize( mPageRect.point, mPageRect.extent );


   // Select this Page
   selectPage( page );
}


bool GuiTabBookCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   mHasTexture = mProfile->constructBitmapArray() > 0;
   if( mHasTexture )
   {
      mBitmapBounds = mProfile->mBitmapArrayRects.address();
      mTabHeight = mBitmapBounds[TabSelected].extent.y;
   }
   else
      mTabHeight = 24;

   calculatePageTabs();


   return true;
}

void GuiTabBookCtrl::onSleep()
{
   Parent::onSleep();
}


void GuiTabBookCtrl::addNewPage()
{
   char textbuf[1024];

   GuiTabPageCtrl * page = new GuiTabPageCtrl();

   page->setField("profile", "GuiTabPageProfile");

   dSprintf(textbuf, sizeof(textbuf), "TabBookPage%d_%d", getId(), page->getId());
   page->registerObject(textbuf);

   this->addObject( page );
}

bool GuiTabBookCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   calculatePageTabs();

   return Parent::resize( newPosition, newExtent );
}

void GuiTabBookCtrl::childResized(GuiControl *child)
{
   Parent::childResized( child );

   //child->resize( mPageRect.point, mPageRect.extent );
}

void GuiTabBookCtrl::onMouseDown(const GuiEvent &event)
{
   Point2I localMouse = globalToLocalCoord( event.mousePoint );
   if( mTabRect.pointInRect( localMouse ) )
   {
      GuiTabPageCtrl *tab = findHitTab( localMouse );
      if( tab != NULL )
	  {
		  selectPage( tab );
		  //执行子控件类的脚本命令   2009-2-10 by thinking
		  if ( tab->getConsoleCommand())
		  {
			  Con::setVariable("$ThisControl",avar("%d",getId()));
			  Con::evaluate(tab->getConsoleCommand(), false);
		  }
	  }
         
   }
}

void GuiTabBookCtrl::onMouseMove(const GuiEvent &event)
{

   Point2I localMouse = globalToLocalCoord( event.mousePoint );
   if( mTabRect.pointInRect( localMouse ) )
   {

      GuiTabPageCtrl *tab = findHitTab( localMouse );
      if( tab != NULL && mHoverTab != tab )
         mHoverTab = tab;
      else if ( !tab )
         mHoverTab = NULL;
   }
   Parent::onMouseMove( event );
}

void GuiTabBookCtrl::onMouseLeave( const GuiEvent &event )
{
   mHoverTab = NULL;
}

bool GuiTabBookCtrl::onMouseDownEditor(const GuiEvent &event, Point2I offset)
{
   bool handled = false;
   Point2I localMouse = globalToLocalCoord( event.mousePoint );

   if( mTabRect.pointInRect( localMouse ) )
   {
      GuiTabPageCtrl *tab = findHitTab( localMouse );
      if( tab != NULL )
      {
         selectPage( tab );
         handled = true;
      }
   }

   // This shouldn't be called if it's not design time, but check just incase
   if ( GuiControl::smDesignTime )
   {
      // If we clicked in the editor and our addset is the tab book
      // ctrl, select the child ctrl so we can edit it's properties
      GuiEditCtrl* edit = GuiControl::smEditorHandle;
      if( edit  && ( edit->getAddSet() == this ) && mActivePage != NULL )
         edit->select( mActivePage );
   }

   // Return whether we handled this or not.
   return handled;

}

void GuiTabBookCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   RectI tabRect = mTabRect;
   tabRect.point += offset;
   RectI pageRect = mPageRect;
   pageRect.point += offset;

   // We're so nice we'll store the old modulation before we clear it for our rendering! :)
   ColorI oldModulation;
   GFX->getDrawUtil()->getBitmapModulation( &oldModulation );

   // Wipe it out
   GFX->getDrawUtil()->clearBitmapModulation();

   Parent::onRender(offset, updateRect);

   // Clip to tab area
   GFX->setClipRect( tabRect );

   // Render our tabs
   renderTabs( offset, tabRect );

   // Restore old modulation
   GFX->getDrawUtil()->setBitmapModulation( oldModulation );
}

void GuiTabBookCtrl::renderTabs( const Point2I &offset, const RectI &tabRect )
{
   // If the tab size is zero, don't render tabs,
   //  assuming it's a tab-less book
   if( mPages.empty() || mTabHeight <= 0 )
      return;

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      const TabHeaderInfo &currentTabInfo = mPages[i];
      RectI tabBounds = mPages[i].TabRect;
      tabBounds.point += offset;
      GuiTabPageCtrl *tab = mPages[i].Page;
      if( tab != NULL )
         renderTab( tabBounds, tab );

      // If we're on the last tab, draw the nice end piece
      if( i + 1 == mPages.size() )
      {
         Point2I tabEndPoint = Point2I(currentTabInfo.TabRect.point.x + currentTabInfo.TabRect.extent.x + offset.x, currentTabInfo.TabRect.point.y + offset.y);
         Point2I tabEndExtent = Point2I((tabRect.point.x + tabRect.extent.x) - tabEndPoint.x, currentTabInfo.TabRect.extent.y);
         RectI tabEndRect = RectI(tabEndPoint,tabEndExtent);
         GFX->setClipRect( tabEndRect );
         renderFixedBitmapBordersFilled( tabEndRect, TabEnds + 1, mProfile );
      }
   }
}

void GuiTabBookCtrl::renderTab( RectI tabRect, GuiTabPageCtrl *tab )
{
   StringTableEntry text = tab->getText();
   ColorI oldColor;

   GFX->getDrawUtil()->getBitmapModulation( &oldColor );

   // Is this a skinned control?
   if( mHasTexture && mProfile->mBitmapArrayRects.size() >= 9 )
   {
      S32 indexMultiplier = 1;
      switch( mTabPosition )
      {
      case AlignTop:
      case AlignBottom:
         
         if ( mActivePage == tab )
            indexMultiplier += TabSelected;
         else if( mHoverTab == tab )
            indexMultiplier += TabHover;
         else
            indexMultiplier += TabNormal;
         break;
      } 

      renderFixedBitmapBordersFilled( tabRect, indexMultiplier, mProfile );
   }
   else
   {
      // If this isn't a skinned control or the bitmap is simply missing, handle it WELL
      if ( mActivePage == tab )
         GFX->getDrawUtil()->drawRectFill(tabRect, mProfile->mFillColor);
      else if( mHoverTab == tab )
         GFX->getDrawUtil()->drawRectFill(tabRect, mProfile->mFillColorHL);
      else
         GFX->getDrawUtil()->drawRectFill(tabRect, mProfile->mFillColorNA);

   }


   GFX->getDrawUtil()->setBitmapModulation(mProfile->mFontColor);

   switch( mTabPosition )
   {
   case AlignTop:
   case AlignBottom:
      renderJustifiedText( tabRect.point, tabRect.extent, text);
   break;
   }

   GFX->getDrawUtil()->setBitmapModulation( oldColor);

}

void GuiTabBookCtrl::setUpdate()
{
   Parent::setUpdate();

   setUpdateRegion(Point2I(0,0), getExtent());

   calculatePageTabs();
}

S32 GuiTabBookCtrl::calculatePageTabWidth( GuiTabPageCtrl *page )
{
   if( !page )
      return mTabWidth;

   const char* text = page->getText();

   if( !text || dStrlen(text) == 0 || mProfile->mFont == '\0' )
      return mTabWidth;

   CommonFontEX *font = mProfile->mFont;

   return font->getStrWidth( const_cast<char*>(text));

}

const RectI GuiTabBookCtrl::getClientRect()
{

   if( !mProfile || mProfile->mBitmapArrayRects.size() < NumBitmaps )
      return Parent::getClientRect();

   return mPageRect;
}


void GuiTabBookCtrl::calculatePageTabs()
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  assuming it's a tab-less book
   if( mPages.empty() || mTabHeight <= 0 )
      return;

   S32 currRow    = 0;
   S32 currColumn = 0;
   S32 currX      = 0;
   S32 maxWidth   = 0;

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      // Fetch Tab Width
      S32 tabWidth = calculatePageTabWidth( mPages[i].Page ) + ( mTabMargin * 2 );
      tabWidth = getMax( tabWidth, mMinTabWidth );
      TabHeaderInfo &info = mPages[i];
      switch( mTabPosition )
      {
      case AlignTop:
      case AlignBottom:
         // If we're going to go outside our bounds
         // with this tab move it down a row
         if( currX + tabWidth > getWidth() )
         {
            // Calculate and Advance State.
            maxWidth = getMax( tabWidth, maxWidth );
            balanceRow( currRow, currX );
            info.TabRow = ++currRow;
            // Reset Necessaries
            info.TabColumn = currColumn = maxWidth = currX = 0;
         }
         else
         {
            info.TabRow = currRow;
            info.TabColumn = currColumn++;
         }

         // Calculate Tabs Bounding Rect
         info.TabRect.point.x  = currX;
         info.TabRect.extent.x = tabWidth;
         info.TabRect.extent.y = mTabHeight;

         // Adjust Y Point based on alignment
         if( mTabPosition == AlignTop )
            info.TabRect.point.y  = ( info.TabRow * mTabHeight );
         else 
            info.TabRect.point.y  = getHeight() - ( ( 1 + info.TabRow ) * mTabHeight );

         currX += tabWidth;
         break;
      };
   }

   currRow++;
   currColumn++;

   Point2I localPoint = getExtent();

   // Calculate 
   switch( mTabPosition )
   {
   case AlignTop:

      localPoint.y -= getTop();

      mTabRect.point.x = 0;
      mTabRect.extent.x = localPoint.x;
      mTabRect.point.y = 0;
      mTabRect.extent.y = currRow * mTabHeight;

      mPageRect.point.x = 0;
      mPageRect.point.y = mTabRect.extent.y;
      mPageRect.extent.x = mTabRect.extent.x;
      mPageRect.extent.y = getHeight() - mTabRect.extent.y;

      break;
   case AlignBottom:
      mTabRect.point.x = 0;
      mTabRect.extent.x = localPoint.x;
      mTabRect.extent.y = currRow * mTabHeight;
      mTabRect.point.y = getHeight() - mTabRect.extent.y;

      mPageRect.point.x = 0;
      mPageRect.point.y = 0;
      mPageRect.extent.x = mTabRect.extent.x;
      mPageRect.extent.y = localPoint.y - mTabRect.extent.y;

      break;
   };


}

void GuiTabBookCtrl::balanceRow( S32 row, S32 totalTabWidth )
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty() || mTabHeight <= 0 )
      return;

   Vector<TabHeaderInfo*> rowTemp;
   rowTemp.clear();

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      TabHeaderInfo &info = mPages[i];

      if(info.TabRow == row )
         rowTemp.push_back( &mPages[i] );
   }

   if( rowTemp.empty() )
      return;

   // Balance the tabs across the remaining space
   S32 spaceToDivide = getWidth() - totalTabWidth;
   S32 pointDelta    = 0;
   for( S32 i = 0; i < rowTemp.size(); i++ )
   {
      TabHeaderInfo &info = *rowTemp[i];
      S32 extraSpace = (S32)spaceToDivide / ( rowTemp.size() );
      info.TabRect.extent.x += extraSpace;
      info.TabRect.point.x += pointDelta;
      pointDelta += extraSpace;
   }
}


GuiTabPageCtrl *GuiTabBookCtrl::findHitTab( const GuiEvent &event )
{
   return findHitTab( event.mousePoint );
}

GuiTabPageCtrl *GuiTabBookCtrl::findHitTab( Point2I hitPoint )
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty() || mTabHeight <= 0 )
      return NULL;

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      if( mPages[i].TabRect.pointInRect( hitPoint ) )
         return mPages[i].Page;
   }
   return NULL;
}

void GuiTabBookCtrl::selectPage( S32 index )
{
   if( mPages.size() < index )
      return;

   // Select the page
   selectPage( mPages[ index ].Page );
}


void GuiTabBookCtrl::selectPage( GuiTabPageCtrl *page )
{
   Vector<TabHeaderInfo>::iterator i = mPages.begin();
   for( S32 index = 0; i != mPages.end() ; i++, index++ )
   {
      GuiTabPageCtrl *tab = (*i).Page;
      if( page == tab )
      {
         mActivePage = tab;
         tab->setVisible( true );

         // Notify User
         Con::executef( this, "onTabSelected",  tab->getText(), Con::getIntArg(index) );

      }
      else
         tab->setVisible( false );
   }
   setUpdateLayout( updateSelf );
}

bool GuiTabBookCtrl::onKeyDown(const GuiEvent &event)
{
   // Tab      = Next Page
   // Ctrl-Tab = Previous Page
   if( 0 && event.keyCode == KEY_TAB )
   {
      if( event.modifier & SI_CTRL )
         selectPrevPage();
      else 
         selectNextPage();

      return true;
   }

   return Parent::onKeyDown( event );
}

void GuiTabBookCtrl::selectNextPage()
{
   if( mPages.empty() )
      return;

   if( mActivePage == NULL )
      mActivePage = mPages[0].Page;

   S32 nI = 0;
   for( ; nI < mPages.size(); nI++ )
   {
      GuiTabPageCtrl *tab = mPages[ nI ].Page;
      if( tab == mActivePage )
      {
         if( nI == ( mPages.size() - 1 ) )
            selectPage( 0 );
         else if ( nI + 1 <= ( mPages.size() - 1 ) ) 
            selectPage( nI + 1 );
         else
            selectPage( 0 );
         return;
      }
   }
}

void GuiTabBookCtrl::selectPrevPage()
{
   if( mPages.empty() )
      return;

   if( mActivePage == NULL )
      mActivePage = mPages[0].Page;

   S32 nI = 0;
   for( ; nI < mPages.size(); nI++ )
   {
      GuiTabPageCtrl *tab = mPages[ nI ].Page;
      if( tab == mActivePage )
      {
         if( nI == 0 )
            selectPage( mPages.size() - 1 );
         else
            selectPage( nI - 1 );
         return;
      }
   }

}
ConsoleMethod( GuiTabBookCtrl, selectPage, void, 3, 3, "(int pageIndex)")
{
   S32 pageIndex = dAtoi(argv[2]);

   object->selectPage(pageIndex);
}

