//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gui/controls/guiListBoxCtrl.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/gfxFontRenderBatcher.h"

IMPLEMENT_CONOBJECT(GuiListBoxCtrl);

GuiListBoxCtrl::GuiListBoxCtrl()
{
   mItems.clear();
   mSelectedItems.clear();
   mMultipleSelections = true;
   mFitParentWidth = true;
   mItemSize = Point2I(10,20);
   mLastClickItem = NULL;
}

GuiListBoxCtrl::~GuiListBoxCtrl()
{
   clearItems();
}

void GuiListBoxCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField( "AllowMultipleSelections", TypeBool, Offset( mMultipleSelections, GuiListBoxCtrl) );
   addField( "FitParentWidth", TypeBool, Offset( mFitParentWidth, GuiListBoxCtrl) );
}

bool GuiListBoxCtrl::onWake()
{
   if( !Parent::onWake() )
      return false;

   updateSize();

   return true;
}

//-----------------------------------------------------------------------------
// Item Accessors
//-----------------------------------------------------------------------------
ConsoleMethod( GuiListBoxCtrl, setMultipleSelection, void, 3, 3, "listBox.setMultipleSelection([true/false])" )
{
   object->setMultipleSelection( dAtob( argv[2] ) );
}


ConsoleMethod( GuiListBoxCtrl, clearItems, void, 2, 2, "clearItems() - Clears all the items in the listbox" )
{
   object->clearItems();
}

void GuiListBoxCtrl::clearItems()
{
   // Free item list allocated memory
   while( mItems.size() )
      deleteItem( 0 );

   // Free our vector lists
   mItems.clear();
   mSelectedItems.clear();
}


ConsoleMethod( GuiListBoxCtrl, clearSelection, void, 2, 2, "clearSelection() - sets all currently selected items to unselected" )
{
   object->clearSelection();
}
void GuiListBoxCtrl::clearSelection()
{
   if( !mSelectedItems.size() )
      return;

   VectorPtr<LBItem*>::iterator i = mSelectedItems.begin();
   for( ; i != mSelectedItems.end(); i++ )
      (*i)->isSelected = false;

   mSelectedItems.clear();
}


ConsoleMethod( GuiListBoxCtrl, setSelected, void, 3, 4, "setSelected(index, [true]/false) - sets the item at the index specified to selected or not")
{
   bool value = true;
   if( argc == 4 )
      value = dAtob( argv[3] );

   if( value == true )
      object->addSelection( dAtoi( argv[2] ) );
   else
      object->removeSelection( dAtoi( argv[2] ) );
}

void GuiListBoxCtrl::removeSelection( S32 index )
{
   // Range Check
   if( index >= mItems.size() || index < 0 )
   {
      Con::warnf("GuiListBoxCtrl::removeSelection - index out of range!" );
      return;
   }

   removeSelection( mItems[index], index );
}
void GuiListBoxCtrl::removeSelection( LBItem *item, S32 index )
{
   if( !mSelectedItems.size() )
      return;

   if( !item )
      return;

   for( S32 i = 0 ; i < mSelectedItems.size(); i++ )
   {
      if( mSelectedItems[i] == item )
      {
         mSelectedItems.erase( &mSelectedItems[i] );
         item->isSelected = false;
         Con::executef(this, "onUnSelect", Con::getIntArg( index ), item->itemText);
         return;
      }
   }
}

void GuiListBoxCtrl::addSelection( S32 index )
{
   // Range Check
   if( index >= mItems.size() || index < 0 )
   {
      Con::warnf("GuiListBoxCtrl::addSelection- index out of range!" );
      return;
   }

   addSelection( mItems[index], index );

}
void GuiListBoxCtrl::addSelection( LBItem *item, S32 index )
{
   if( !mMultipleSelections )
   {
      if( !mSelectedItems.empty() )
      {
         LBItem* selItem = mSelectedItems.front();
         if( selItem != item )
            clearSelection();
         else
            return;
      }
   }
   else
   {
      if( !mSelectedItems.empty() )
      {
         for( S32 i = 0; i < mSelectedItems.size(); i++ )
         {
            if( mSelectedItems[ i ] == item )
               return;
         }
      }
   }

   item->isSelected = true;
   mSelectedItems.push_front( item );

   Con::executef(this, "onSelect", Con::getIntArg( index ), item->itemText);

}

S32 GuiListBoxCtrl::getItemIndex( LBItem *item )
{
   if( mItems.empty() )
      return -1;

   // Lookup the index of an item in our list, by the pointer to the item
   for( S32 i = 0; i < mItems.size(); i++ )
      if( mItems[i] == item )
         return i;

   return -1;
}

ConsoleMethod( GuiListBoxCtrl, getItemCount, S32, 2, 2, "getItemCount() - returns the number of items in the list")
{
   return object->getItemCount();
}

S32 GuiListBoxCtrl::getItemCount()
{
   return mItems.size();
}

ConsoleMethod( GuiListBoxCtrl, getSelCount, S32, 2, 2, "getSelCount() - returns the number of items currently selected")
{
   return object->getSelCount();
}
S32 GuiListBoxCtrl::getSelCount()
{
   return mSelectedItems.size();
}

ConsoleMethod( GuiListBoxCtrl, getSelectedItem, S32, 2, 2, "getSelectedItem() - returns the selected items index or -1 if none. "
              "If multiple selections exist it returns the first selected item" )
{
   return object->getSelectedItem();
}
S32 GuiListBoxCtrl::getSelectedItem()
{
   if( mSelectedItems.empty() || mItems.empty() )
      return -1;

   for( S32 i = 0 ; i < mItems.size(); i++ )
      if( mItems[i]->isSelected )
         return i;

   return -1;
}

ConsoleMethod( GuiListBoxCtrl, getSelectedItems, const char*, 2, 2, "getSelectedItems() - returns a space delimited list "
              "of the selected items indexes in the list")
{
   S32 selCount = object->getSelCount();
   if( selCount == -1 || selCount == 0 )
      return StringTable->lookup("-1");
   else if( selCount == 1 )
      return Con::getIntArg(object->getSelectedItem());

   Vector<S32> selItems;
   object->getSelectedItems( selItems );

   if( selItems.empty() )
      return StringTable->lookup("-1");

   S32 length = selItems.size() * 4;
   UTF8 *retBuffer = Con::getReturnBuffer( length );
   dMemset( retBuffer, 0, length );
   Vector<S32>::iterator i = selItems.begin();
   for( ; i != selItems.end(); i++ )
   {
      UTF8 retFormat[12];
      dSprintf( retFormat, 12, "%d ", (*i) );
      dStrcat( retBuffer, length, retFormat );
   }

   return retBuffer;
}
void GuiListBoxCtrl::getSelectedItems( Vector<S32> &Items )
{
   // Clear our return vector
   Items.clear();
   
   // If there are no selected items, return an empty vector
   if( mSelectedItems.empty() )
      return;
   
   for( S32 i = 0; i < mItems.size(); i++ )
      if( mItems[i]->isSelected )
         Items.push_back( i );
}

ConsoleMethod(GuiListBoxCtrl, findItemText, S32, 3, 4, "listBox.findItemText( myItemText, [?caseSensitive - false] ) - Returns index of item with matching text or -1 if none")
{
   bool bCaseSensitive = false;

   if( argc == 4 )
      bCaseSensitive = dAtob( argv[3] );

   return object->findItemText( argv[2], bCaseSensitive );
}

S32 GuiListBoxCtrl::findItemText( StringTableEntry text, bool caseSensitive )
{
   // Check Proper Arguments
   if( !text || !text[0] || text == StringTable->lookup("") )
   {
      Con::warnf("GuiListBoxCtrl::findItemText - No Text Specified!");
      return -1;
   }

   // Check Items Exist.
   if( mItems.empty() )
      return -1;

   // Lookup the index of an item in our list, by the pointer to the item
   for( S32 i = 0; i < mItems.size(); i++ )
   {
      // Case Sensitive Compare?
      if( caseSensitive && ( dStrcmp( mItems[i]->itemText, text ) == 0 ) )
         return i;
      else if (!caseSensitive && ( dStricmp( mItems[i]->itemText, text ) == 0 ))
         return i;
   }

   // Not Found!
   return -1;
}

ConsoleMethod(GuiListBoxCtrl, setCurSel, void, 3, 3, "setCurSel(index) - sets the currently selected item at the specified index")
{
   object->setCurSel( dAtoi( argv[2] ) );
}
void GuiListBoxCtrl::setCurSel( S32 index )
{
   // Range Check
   if( index >= mItems.size() )
   {
      Con::warnf("GuiListBoxCtrl::setCurSel - index out of range!" );
      return;
   }

   // If index -1 is specified, we clear the selection
   if( index == -1 )
   {
      mSelectedItems.clear();
      return;
   }

   // Add the selection
   addSelection( mItems[ index ], index );

}

ConsoleMethod( GuiListBoxCtrl, setCurSelRange, void, 3, 4, "setCurSelRange(start,[stop]) - sets the current selection range from"
              " index start to stop.  if no stop is specified it sets from start index to the end of the list")
{
   if( argc == 4 )
      object->setCurSelRange( dAtoi(argv[2]) , dAtoi( argv[3] ) );
   else
      object->setCurSelRange( dAtoi(argv[2]), 999999 );
}
void GuiListBoxCtrl::setCurSelRange( S32 start, S32 stop )
{
   // Verify Selection Range
   if( start < 0 )
      start = 0;
   else if( start > mItems.size() )
      start = mItems.size();

   if( stop < 0 )
      stop = 0;
   else if( stop > mItems.size() )
      stop = mItems.size();

   S32 iterStart = ( start < stop ) ? start : stop;
   S32 iterStop  = ( start < stop ) ? stop : start;

   for( ; iterStart <= iterStop; iterStart++ )
      addSelection( mItems[iterStart], iterStart );
}


ConsoleMethod( GuiListBoxCtrl, addItem, void, 3, 4, "addItem(text, color) - adds an item to the end of the list with an optional color" )
{
   if(argc == 3)
   {
      object->addItem( argv[2] );
   } else if(argc == 4)
   {
      U32 elementCount = GuiListBoxCtrl::getStringElementCount(argv[3]);

	  if(elementCount == 3)
	  {
         F32 red, green, blue;

		 red = dAtof(GuiListBoxCtrl::getStringElement( argv[3], 0 ));
		 green = dAtof(GuiListBoxCtrl::getStringElement( argv[3], 1 ));
		 blue = dAtof(GuiListBoxCtrl::getStringElement( argv[3], 2 ));

		 object->addItemWithColor( argv[2], ColorF(red, green, blue) );
	  } 
     else
	  {
         Con::warnf("GuiListBoxCtrl::addItem() - Invalid number of parameters for the color!");
	  }

   } else
   {
      Con::warnf("GuiListBoxCtrl::addItem() - Invalid number of parameters!");
   }  
}
S32 GuiListBoxCtrl::addItem( StringTableEntry text, void *itemData )
{
   // This just calls insert item at the end of the list
   return insertItem( mItems.size(), text, itemData );
}

S32 GuiListBoxCtrl::addItemWithColor( StringTableEntry text, ColorF color, void *itemData )
{
   // This just calls insert item at the end of the list
   return insertItemWithColor( mItems.size(), text, color, itemData );
}

ConsoleMethod(GuiListBoxCtrl, setItemColor, void, 4, 4, "(index, color)")
{
   U32 elementCount = GuiListBoxCtrl::getStringElementCount(argv[3]);

   if(elementCount == 3)
   {
      F32 red = dAtof(GuiListBoxCtrl::getStringElement( argv[3], 0 ));
      F32 green = dAtof(GuiListBoxCtrl::getStringElement( argv[3], 1 ));
      F32 blue = dAtof(GuiListBoxCtrl::getStringElement( argv[3], 2 ));

      object->setItemColor( dAtoi(argv[2]), ColorF(red, green, blue) );
   }
   else
      Con::warnf("GuiListBoxCtrl::addItem() - Invalid number of parameters for the color!");
}

void GuiListBoxCtrl::setItemColor( S32 index, ColorF color )
{
   if ((index >= mItems.size()) || index < 0)
   {
      Con::warnf("GuiListBoxCtrl::setItemColor - invalid index");
      return;
   }

   LBItem* item = mItems[index];
   item->hasColor = true;
   item->color = color;
}

ConsoleMethod(GuiListBoxCtrl, clearItemColor, void, 3, 3, "(index)")
{
   object->clearItemColor(dAtoi(argv[2]));
}

void GuiListBoxCtrl::clearItemColor( S32 index )
{
   if ((index >= mItems.size()) || index < 0)
   {
      Con::warnf("GuiListBoxCtrl::setItemColor - invalid index");
      return;
   }

   LBItem* item = mItems[index];
   item->hasColor = false;
}

ConsoleMethod( GuiListBoxCtrl, insertItem, void, 4, 4, "insertItem( text, index ) - inserts an item into the list at the specified index and returns the index assigned or -1 on error")
{
   object->insertItem( dAtoi( argv[3] ), argv[2] );
}

S32 GuiListBoxCtrl::insertItem( S32 index, StringTableEntry text, void *itemData )
{
   // If the index is greater than our list size, insert it at the end
   if( index >= mItems.size() )
      index = mItems.size();

   // Sanity checking
   if( !text )
   {
      Con::warnf("GuiListBoxCtrl::insertItem - cannot add NULL string" );
      return -1;
   }

   LBItem *newItem = new LBItem;
   if( !newItem )
   {
      Con::warnf("GuiListBoxCtrl::insertItem - error allocating item memory!" );
      return -1;
   }

   // Assign item data
   newItem->itemText    = StringTable->insert(text);
   newItem->itemData    = itemData;
   newItem->isSelected  = false;
   newItem->hasColor    = false;

   // Add to list
   mItems.insert(index);
   mItems[index] = newItem;

   // Resize our list to fit our items
   updateSize();

   // Return our index in list (last)
   return index;

}

S32 GuiListBoxCtrl::insertItemWithColor( S32 index, StringTableEntry text, ColorF color, void *itemData )
{
   // If the index is greater than our list size, insert it at the end
   if( index >= mItems.size() )
      index = mItems.size();

   // Sanity checking
   if( !text )
   {
      Con::warnf("GuiListBoxCtrl::insertItem - cannot add NULL string" );
      return -1;
   }

   if( color == ColorF(-1, -1, -1) )
   {
      Con::warnf("GuiListBoxCtrl::insertItem - cannot add NULL color" );
      return -1;
   }

   LBItem *newItem = new LBItem;
   if( !newItem )
   {
      Con::warnf("GuiListBoxCtrl::insertItem - error allocating item memory!" );
      return -1;
   }

   // Assign item data
   newItem->itemText    = StringTable->insert(text);
   newItem->itemData    = itemData;
   newItem->isSelected  = false;
   newItem->hasColor    = true;
   newItem->color       = color;

   // Add to list
   mItems.insert(index);
   mItems[index] = newItem;

   // Resize our list to fit our items
   updateSize();

   // Return our index in list (last)
   return index;

}

ConsoleMethod ( GuiListBoxCtrl, deleteItem, void, 3, 3, "deleteItem(itemIndex)" )
{
   object->deleteItem( dAtoi( argv[2] ) );
}

void  GuiListBoxCtrl::deleteItem( S32 index )
{
   // Range Check
   if( index >= mItems.size() || index < 0 )
   {
      Con::warnf("GuiListBoxCtrl::deleteItem - index out of range!" );
      return;
   }

   // Grab our item
   LBItem* item = mItems[ index ];
   if( !item )
   {
      Con::warnf("GuiListBoxCtrl::deleteItem - Bad Item Data!" );
      return;
   }

   // Remove it from the selected list.
   if( item->isSelected )
   {
      for( VectorPtr<LBItem*>::iterator i = mSelectedItems.begin(); i != mSelectedItems.end(); i++ )
      {
         if( item == *i )
         {
            mSelectedItems.erase_fast( i );
            break;
         }
      }
   }

   // Remove it from the list
   mItems.erase( &mItems[ index ] );

   // Free the memory associated with it
   delete item;
}


ConsoleMethod( GuiListBoxCtrl, getItemText, const char*, 3, 3, "getItemText(index) - returns the text of the item at the specified index")
{
   return object->getItemText( dAtoi( argv[2] ) );
}

StringTableEntry GuiListBoxCtrl::getItemText( S32 index )
{
   // Range Checking
   if( index > mItems.size() || index < 0 )
   {
      Con::warnf( "GuiListBoxCtrl::getItemText - index out of range!" );
      return StringTable->lookup("");
   }
   
   return mItems[ index ]->itemText;   
}


ConsoleMethod( GuiListBoxCtrl, setItemText, void, 4, 4, "setItemText(index, newtext) - sets the items text at the specified index" )
{
   object->setItemText( dAtoi( argv[2] ), argv[3] );
}
void GuiListBoxCtrl::setItemText( S32 index, StringTableEntry text )
{
   // Sanity Checking
   if( !text )
   {
      Con::warnf("GuiListBoxCtrl::setItemText - Invalid Text Specified!" );
      return;
   }
   // Range Checking
   if( index > mItems.size() || index < 0 )
   {
      Con::warnf( "GuiListBoxCtrl::getItemText - index out of range!" );
      return;
   }

   mItems[ index ]->itemText = StringTable->insert( text );
}
//-----------------------------------------------------------------------------
// Sizing Functions
//-----------------------------------------------------------------------------
void GuiListBoxCtrl::updateSize()
{
   if( !mProfile || !mProfile->mFont )
      return;

   CommonFontEX *font = mProfile->mFont;
   GuiScrollCtrl* parent = dynamic_cast<GuiScrollCtrl *>(getParent());

   if ( mFitParentWidth && parent )
      mItemSize.x = parent->getContentExtent().x;
   else
   {
      // Find the maximum width cell:
      S32 maxWidth = 1;
      for ( U32 i = 0; i < mItems.size(); i++ )
      {
         S32 width = font->getStrWidth( const_cast<char*>(mItems[i]->itemText ));
         if( width > maxWidth )
            maxWidth = width;
      }
      mItemSize.x = maxWidth + 6;
   }

   mItemSize.y = font->getHeight() + 2;

   Point2I newExtent( mItemSize.x, mItemSize.y * mItems.size() );
   setExtent( newExtent );

}

void GuiListBoxCtrl::parentResized(const RectI &oldParentRect, const RectI &newParentRect)
{
   Parent::parentResized( oldParentRect, newParentRect );

   updateSize();
}

//-----------------------------------------------------------------------------
// Overrides
//-----------------------------------------------------------------------------
void GuiListBoxCtrl::onRender( Point2I offset, const RectI &updateRect )
{
   RectI clipRect(updateRect.point, updateRect.extent);

   if( !mProfile )
      return;

   // Save our original clip rect
   RectI oldClipRect = clipRect;

   for ( S32 i = 0; i < mItems.size(); i++)
   {
      S32 colorBoxSize = 0;
      ColorI boxColor = ColorI(0, 0, 0);
      // Only render visible items
      if ((i + 1) * mItemSize.y + offset.y < updateRect.point.y)
         continue;

      // Break our once we're no longer in visible item range
      if( i * mItemSize.y + offset.y >= updateRect.point.y + updateRect.extent.y)
         break;

      // Render color box if needed
      if(mItems[i]->hasColor)
      {
         // Set the size of the color box to be drawn next to the item text
         colorBoxSize = 3;
         boxColor = ColorI(mItems[i]->color);
         // Draw the box first
         ColorI black = ColorI(0, 0, 0);
         drawBox(  Point2I(offset.x + mProfile->mTextOffset.x + colorBoxSize, offset.y + ( i * mItemSize.y ) + 8), colorBoxSize, black, boxColor );
      }

      RectI itemRect = RectI( offset.x + mProfile->mTextOffset.x + (colorBoxSize * 2), offset.y + ( i * mItemSize.y ), mItemSize.x, mItemSize.y );

      // Render our item
      onRenderItem( itemRect, mItems[i] );
   }

   GFX->setClipRect( oldClipRect );
}

void GuiListBoxCtrl::onRenderItem( RectI itemRect, LBItem *item )
{
   if( item->isSelected )
      GFX->getDrawUtil()->drawRectFill( itemRect, mProfile->mFillColor );

   GFX->getDrawUtil()->setBitmapModulation(mProfile->mFontColor);
   renderJustifiedText(itemRect.point + Point2I( 2, 0 ), itemRect.extent, item->itemText);
}

void GuiListBoxCtrl::drawBox(const Point2I &box, S32 size, ColorI &outlineColor, ColorI &boxColor)
{
   RectI r(box.x - size, box.y - size, 2 * size + 1, 2 * size + 1);
   r.inset(1, 1);
   GFX->getDrawUtil()->drawRectFill(r, boxColor);
   r.inset(-1, -1);
   GFX->getDrawUtil()->drawRect(r, outlineColor);
}

//-----------------------------------------------------------------------------
// Mouse Events
//-----------------------------------------------------------------------------

void GuiListBoxCtrl::onMouseDragged(const GuiEvent &event)
{
   Parent::onMouseDragged(event);

   if(isMethod("onMouseDragged"))
      Con::executef(this, "onMouseDragged");
}

void GuiListBoxCtrl::onMouseDown( const GuiEvent &event )
{
   Point2I localPoint = globalToLocalCoord(event.mousePoint);
   
   S32 itemHit = ( localPoint.y < 0 ) ? -1 : (S32)mFloor( (F32)localPoint.y / (F32)mItemSize.y );

   if ( itemHit >= mItems.size() || itemHit == -1 )
      return;

   LBItem *hitItem = mItems[ itemHit ];
   if ( hitItem == NULL )
      return;

   // If we're not a multiple selection listbox, we simply select/unselect an item
   if( !mMultipleSelections )
   {
      // No current selection?  Just select the cell and move on
      S32 selItem = getSelectedItem();

      if ( selItem != itemHit && selItem != -1 )
         clearSelection();

      // Set the current selection
      setCurSel( itemHit );

      if( itemHit == selItem && event.mouseClickCount == 2 && isMethod("onDoubleClick") )
         Con::executef( this, "onDoubleClick" );

      // Store the clicked item
      mLastClickItem = hitItem;

      // Evaluate the console command if we clicked the same item twice
      if( selItem == itemHit && event.mouseClickCount > 1 && mAltConsoleCommand[0] )
         Con::evaluate( mAltConsoleCommand, false );

      return;

   }
   
   // Deal with multiple selections
   if( event.modifier & SI_CTRL)
   {
      // Ctrl-Click toggles selection
      if( hitItem->isSelected )
      {
         removeSelection( hitItem, itemHit );

         // We return here when we deselect an item because we don't store last clicked when we deselect
         return;
      }
      else
         addSelection( hitItem, itemHit );
   }
   else if( event.modifier & SI_SHIFT )
   {
      if( !mLastClickItem )
         addSelection( hitItem, itemHit );
      else
         setCurSelRange( getItemIndex( mLastClickItem ), itemHit );
   }
   else
   {
      if( getSelCount() != 0 )
      {
         S32 selItem = getSelectedItem();
         if( selItem != -1 && mItems[selItem] != hitItem )
            clearSelection();
      }
      addSelection( hitItem, itemHit );
   }

   if( hitItem == mLastClickItem && event.mouseClickCount == 2 && isMethod("onDoubleClick") )
      Con::executef( this, "onDoubleClick" );

   mLastClickItem = hitItem;


}

U32 GuiListBoxCtrl::getStringElementCount( const char* inString )
{
    // Non-whitespace chars.
    static const char* set = " \t\n";

    // End of string.
    if ( *inString == 0 )
        return 0;

    U32 wordCount = 0;
    U8 search = 0;

    // Search String.
    while( *inString )
    {
        // Get string element.
        search = *inString;

        // End of string?
        if ( search == 0 )
            break;

        // Move to next element.
        inString++;

        // Search for seperators.
        for( U32 i = 0; set[i]; i++ )
        {
            // Found one?
            if( search == set[i] )
            {
                // Yes...
                search = 0;
                break;
            }   
        }

        // Found a seperator?
        if ( search == 0 )
            continue;

        // We've found a non-seperator.
        wordCount++;

        // Search for end of non-seperator.
        while( 1 )
        {
            // Get string element.
            search = *inString;

            // End of string?
            if ( search == 0 )
                break;

            // Move to next element.
            inString++;

            // Search for seperators.
            for( U32 i = 0; set[i]; i++ )
            {
                // Found one?
                if( search == set[i] )
                {
                    // Yes...
                    search = 0;
                    break;
                }   
            }

            // Found Seperator?
            if ( search == 0 )
                break;
        }

        // End of string?
        if ( *inString == 0 )
        {
            // Bah!
            break;
        }
    }

    // We've finished.
    return wordCount;
}

//------------------------------------------------------------------------------
// Get String Element.
//------------------------------------------------------------------------------
const char* GuiListBoxCtrl::getStringElement( const char* inString, const U32 index )
{
    // Non-whitespace chars.
    static const char* set = " \t\n";

    U32 wordCount = 0;
    U8 search = 0;
    const char* pWordStart = NULL;

    // End of string?
    if ( *inString != 0 )
    {
        // No, so search string.
        while( *inString )
        {
            // Get string element.
            search = *inString;

            // End of string?
            if ( search == 0 )
                break;

            // Move to next element.
            inString++;

            // Search for seperators.
            for( U32 i = 0; set[i]; i++ )
            {
                // Found one?
                if( search == set[i] )
                {
                    // Yes...
                    search = 0;
                    break;
                }   
            }

            // Found a seperator?
            if ( search == 0 )
                continue;

            // Found are word?
            if ( wordCount == index )
            {
                // Yes, so mark it.
                pWordStart = inString-1;
            }

            // We've found a non-seperator.
            wordCount++;

            // Search for end of non-seperator.
            while( 1 )
            {
                // Get string element.
                search = *inString;

                // End of string?
                if ( search == 0 )
                    break;

                // Move to next element.
                inString++;

                // Search for seperators.
                for( U32 i = 0; set[i]; i++ )
                {
                    // Found one?
                    if( search == set[i] )
                    {
                        // Yes...
                        search = 0;
                        break;
                    }   
                }

                // Found Seperator?
                if ( search == 0 )
                    break;
            }

            // Have we found our word?
            if ( pWordStart )
            {
                // Yes, so we've got our word...

                // Result Buffer.
                static char buffer[4096];

                // Calculate word length.
                const U32 length = inString - pWordStart - ((*inString)?1:0);

                // Copy Word.
                dStrncpy( buffer, sizeof(buffer), pWordStart, length);
                buffer[length] = '\0';

                // Return Word.
                return buffer;
            }

            // End of string?
            if ( *inString == 0 )
            {
                // Bah!
                break;
            }
        }
    }

    // Sanity!
    AssertFatal( false, "t2dSceneObject::getStringElement() - Couldn't find specified string element!" );
    // Didn't find it
    return " ";
}