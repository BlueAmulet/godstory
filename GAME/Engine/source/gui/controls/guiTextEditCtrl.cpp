//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "gui/core/guiCanvas.h"
#include "gui/controls/guiMLTextCtrl.h"
#include "gui/controls/guiTextEditCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gfx/gfxDevice.h"
#include "core/frameAllocator.h"
#include "sfx/sfxSystem.h"
#include "core/unicode.h"
#include "gfx/gfxFontRenderBatcher.h"

IMPLEMENT_CONOBJECT(GuiTextEditCtrl);

U32 GuiTextEditCtrl::smNumAwake = 0;

GuiTextEditCtrl::GuiTextEditCtrl()
{
   mInsertOn = true;
   mBlockStart = 0;
   mBlockEnd = 0;
   mCursorPos = 0;
   mCursorOn = false;
   mNumFramesElapsed = 0;

   mDragHit = false;
   mTabComplete = false;
   mKeyEnter = true;
   mScrollDir = 0;

   mUndoBlockStart = 0;
   mUndoBlockEnd = 0;
   mUndoCursorPos = 0;
   mPasswordText = false;

   mSinkAllKeyEvents = false;

   mActive = true;

   mTextOffsetReset = true;

   mHistoryDirty = false;
   mHistorySize = 0;
   mHistoryLast = -1;
   mHistoryIndex = 0;
   mHistoryBuf = NULL;

   mbUseMaxNum    = false;
   mbUseMinNum    = false;
   mMaxNum        = S32_MAX;
   mMinNum        = S32_MIN;

   mValidateCommand = StringTable->insert("");
   mEscapeCommand = StringTable->insert( "" );
#if defined(__MACOSX__)
   UTF8	bullet[4] = { 0xE2, 0x80, 0xA2, 0 };
   
   mPasswordMask = StringTable->insert( bullet );
#else
   mPasswordMask = StringTable->insert( "*" );
#endif
   //mDeniedSound = dynamic_cast<SFXProfile*>( Sim::findObject( "InputDeniedSound" ) );
   mSoundID = NULL;

}

GuiTextEditCtrl::~GuiTextEditCtrl()
{
   //delete the history buffer if it exists
   if (mHistoryBuf)
   {
      for (S32 i = 0; i < mHistorySize; i++)
         delete [] mHistoryBuf[i];

      delete [] mHistoryBuf;
   }
}

void GuiTextEditCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("validate",          TypeString,    Offset(mValidateCommand,   GuiTextEditCtrl));
   addField("escapeCommand",     TypeString,    Offset(mEscapeCommand,     GuiTextEditCtrl));
   addField("historySize",       TypeS32,       Offset(mHistorySize,       GuiTextEditCtrl));
   addField("password",          TypeBool,      Offset(mPasswordText,      GuiTextEditCtrl));     
   addField("tabComplete",       TypeBool,      Offset(mTabComplete,       GuiTextEditCtrl)); 
   addField("KeyEnter",          TypeBool,      Offset(mKeyEnter,          GuiTextEditCtrl)); 
   addField("bUseMaxNum",        TypeBool,      Offset(mbUseMaxNum,        GuiTextEditCtrl));
   addField("bUseMinNum",        TypeBool,      Offset(mbUseMinNum,        GuiTextEditCtrl));
   addField("maxNum",            TypeS32,       Offset(mMaxNum,            GuiTextEditCtrl));
   addField("minNum",            TypeS32,       Offset(mMinNum,            GuiTextEditCtrl));
   //addField("deniedSound",       TypeSFXProfilePtr, Offset(mDeniedSound, GuiTextEditCtrl));
   addField("soundID",			 TypeString,	Offset(mSoundID,		   GuiTextEditCtrl));
   addField("sinkAllKeyEvents",  TypeBool,      Offset(mSinkAllKeyEvents,  GuiTextEditCtrl));
   addField("passwordMask",      TypeString,    Offset(mPasswordMask,      GuiTextEditCtrl)); 
}

bool GuiTextEditCtrl::onAdd()
{
   if ( ! Parent::onAdd() )
      return false;

   //create the history buffer
   if ( mHistorySize > 0 )
   {
      mHistoryBuf = new UTF16*[mHistorySize];
      for ( S32 i = 0; i < mHistorySize; i++ )
      {
         mHistoryBuf[i] = new UTF16[GuiTextCtrl::MAX_STRING_LENGTH + 1];
         mHistoryBuf[i][0] = '\0';
      }
   }

   if( mText && mText[0] )
   {
      setText(mText);
   }

   return true;
}

void GuiTextEditCtrl::onStaticModified(const char* slotName, const char* newValue)
{
   if(!dStricmp(slotName, "text"))
      setText(mText);
}

bool GuiTextEditCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   // CodeReview - This static should be on the canvas. BJG 3/25/07
   // If this is the first awake text edit control, enable keyboard translation
   if (smNumAwake == 0)
      getRoot()->enableKeyboardTranslation();
   ++smNumAwake;

   return true;
}

void GuiTextEditCtrl::onSleep()
{
   Parent::onSleep();

   GuiCanvas *root = getRoot();

   // If this is the last awake text edit control, disable keyboard translation
   --smNumAwake;
   if (smNumAwake == 0)
      root->disableKeyboardTranslation();

   // If we're still the first responder then
   // restore the accelerators.
   if ( mFirstResponder == this )
      root->setNativeAcceleratorsEnabled( true );
}

void GuiTextEditCtrl::execConsoleCallback()
{
   // Execute the console command!
   if ( mConsoleCommand[0] )
   {
      char buf[16];
      dSprintf( buf, sizeof( buf ), "%d", getId() );
      Con::setVariable( "$ThisControl", buf );
      Con::evaluate( mConsoleCommand, false );
   }

   // Update the console variable:
   if ( mConsoleVariable[0] )
      Con::setVariable( mConsoleVariable, mTextBuffer.getPtr8() );
}

void GuiTextEditCtrl::updateHistory( StringBuffer *inTxt, bool moveIndex )
{
   const UTF16* txt = inTxt->getPtr();

   if(!txt)
      return;

   if(!mHistorySize)
      return;

   // see if it's already in
   if(mHistoryLast == -1 || dStrcmp(txt, mHistoryBuf[mHistoryLast]))
   {
      if(mHistoryLast == mHistorySize-1) // we're at the history limit... shuffle the pointers around:
      {
         UTF16 *first = mHistoryBuf[0];
         for(U32 i = 0; i < mHistorySize - 1; i++)
            mHistoryBuf[i] = mHistoryBuf[i+1];
         mHistoryBuf[mHistorySize-1] = first;
         if(mHistoryIndex > 0)
            mHistoryIndex--;
      }
      else
         mHistoryLast++;

      inTxt->getCopy(mHistoryBuf[mHistoryLast], GuiTextCtrl::MAX_STRING_LENGTH);
      mHistoryBuf[mHistoryLast][GuiTextCtrl::MAX_STRING_LENGTH] = '\0';
   }

   if(moveIndex)
      mHistoryIndex = mHistoryLast + 1;
}

void GuiTextEditCtrl::getText( char *dest )
{
   if ( dest )
      mTextBuffer.getCopy8((UTF8*)dest, GuiTextCtrl::MAX_STRING_LENGTH+1);
}  
 
void GuiTextEditCtrl::setText( const UTF8 *txt )
{
   if(txt && txt[0] != 0)
   {
      Parent::setText(txt);
      mTextBuffer.set( txt );
   }
   else
      mTextBuffer.set( "" );

   mCursorPos = mTextBuffer.length();
}

void GuiTextEditCtrl::setText( const UTF16* txt)
{
   if(txt && txt[0] != 0)
   {
      UTF8* txt8 = convertUTF16toUTF8( txt );
      Parent::setText( txt8 );
      delete[] txt8;
      mTextBuffer.set( txt );
   }
   else
   {
      Parent::setText("");
      mTextBuffer.set("");
   }
   
   mCursorPos = mTextBuffer.length();   
}

void GuiTextEditCtrl::selectAllText()
{
   mBlockStart = 0;
   mBlockEnd = dStrlen(mText);
   setUpdate();
}

void GuiTextEditCtrl::forceValidateText()
{
   if ( mValidateCommand[0] )
   {
      char buf[16];
      dSprintf(buf, sizeof(buf), "%d", getId());
      Con::setVariable("$ThisControl", buf);
      Con::evaluate( mValidateCommand, false );
   }
}

void GuiTextEditCtrl::reallySetCursorPos( const S32 newPos )
{
   S32 charCount = mTextBuffer.length();
   S32 realPos = newPos > charCount ? charCount : newPos < 0 ? 0 : newPos;
   if ( realPos != mCursorPos )
   {
      mCursorPos = realPos;
      setUpdate();
   }
}

S32 GuiTextEditCtrl::setCursorPos( const Point2I &offset )
{
   Point2I ctrlOffset = localToGlobalCoord( Point2I( 0, 0 ) );
   S32 charLength = 0;
   S32 curX;

   curX = offset.x - ctrlOffset.x;
   setUpdate();

   //if the cursor is too far to the left
   if ( curX < 0 )
      return -1;

   //if the cursor is too far to the right
   if ( curX >= ctrlOffset.x + getExtent().x )
      return -2;

   curX = offset.x - mTextOffset.x;
   S32 count=0;
   if(mTextBuffer.length() == 0)
      return 0;

   char oo[MAX_STRING_LENGTH];
   ZeroMemory(oo, MAX_STRING_LENGTH);
   getText(oo); 

   mFont->getStrWidthEx( oo,  &count, curX);

   /*for(count=0; count<strlen(oo); count++)
   {
      UTF16 c = mTextBuffer.getChar(count);

      char oo[2];
	  ZeroMemory(oo,2);
	  convertUTF16toUTF8(&c, oo, 2);

	  if(mPasswordText)
		  charLength += (mFont->getStrWidth( const_cast<char*>(&mPasswordMask[0]) ));
	  else
		  charLength += (mFont->getStrWidth( oo ));

	  if ( charLength > curX )
		  break;      
   }*/

   return count;
}

void GuiTextEditCtrl::onMouseDown( const GuiEvent &event )
{
   mDragHit = false;

   // If we have a double click, select all text.  Otherwise
   // act as before by clearing any selection.
   bool doubleClick = (event.mouseClickCount > 1);
   if(doubleClick)
   {
      selectAllText();

   } else
   {
      //undo any block function
      mBlockStart = 0;
      mBlockEnd = 0;
   }

   //find out where the cursor should be
   S32 pos = setCursorPos( event.mousePoint );

   // if the position is to the left
   if ( pos == -1 )
      mCursorPos = 0;
   else if ( pos == -2 ) //else if the position is to the right
      mCursorPos = mTextBuffer.length();
   else //else set the mCursorPos
      mCursorPos = pos;

   //save the mouseDragPos
   mMouseDragStart = mCursorPos;

   // lock the mouse
   mouseLock();

   //set the drag var
   mDragHit = true;

   //let the parent get the event
   setFirstResponder();
}

void GuiTextEditCtrl::onMouseDragged( const GuiEvent &event )
{
   S32 pos = setCursorPos( event.mousePoint );

   // if the position is to the left
   if ( pos == -1 )
      mScrollDir = -1;
   else if ( pos == -2 ) // the position is to the right
      mScrollDir = 1;
   else // set the new cursor position
   {
      mScrollDir = 0;
      mCursorPos = pos;
   }

   // update the block:
   mBlockStart = getMin( mCursorPos, mMouseDragStart );
   mBlockEnd = getMax( mCursorPos, mMouseDragStart );
   if ( mBlockStart < 0 )
      mBlockStart = 0;

   if ( mBlockStart == mBlockEnd )
      mBlockStart = mBlockEnd = 0;

   //let the parent get the event
   Parent::onMouseDragged(event);
}

void GuiTextEditCtrl::onMouseUp(const GuiEvent &event)
{
   event;
   mDragHit = false;
   mScrollDir = 0;
   mouseUnlock();
}

void GuiTextEditCtrl::saveUndoState()
{
   //save the current state
   mUndoText.set(&mTextBuffer);
   mUndoBlockStart = mBlockStart;
   mUndoBlockEnd   = mBlockEnd;
   mUndoCursorPos  = mCursorPos;
}

void GuiTextEditCtrl::onCopy(bool andCut)
{
   // Don't copy/cut password field!
   if(mPasswordText)
      return;

   if (mBlockEnd > 0)
   {
      //save the current state
      saveUndoState();

      //copy the text to the clipboard
      UTF8* clipBuff = mTextBuffer.createSubstring8(mBlockStart, mBlockEnd - mBlockStart);
      Platform::setClipboard(clipBuff);
      delete[] clipBuff;

      //if we pressed the cut shortcut, we need to cut the selected text from the control...
      if (andCut)
      {
         mTextBuffer.cut(mBlockStart, mBlockEnd - mBlockStart);
         mCursorPos = mBlockStart;
      }

      mBlockStart = 0;
      mBlockEnd = 0;
   }

}

void GuiTextEditCtrl::onPaste()
{           
   //first, make sure there's something in the clipboard to copy...
   const UTF8 *clipboard = Platform::getClipboard();
   if(dStrlen(clipboard) <= 0)
      return;

   //save the current state
   saveUndoState();

   //delete anything hilited
   if (mBlockEnd > 0)
   {
      mTextBuffer.cut(mBlockStart, mBlockEnd - mBlockStart);
      mCursorPos = mBlockStart;
      mBlockStart = 0;
      mBlockEnd = 0;
   }

   // We'll be converting to UTF16, and maybe trimming the string,
   // so let's use a StringBuffer, for convinience.
   StringBuffer pasteText(clipboard);

   // Space left after we remove the highlighted text
   S32 stringLen = mTextBuffer.length();

   // Trim down to fit in a buffer of size mMaxStrLen
   S32 pasteLen = pasteText.length();

   if(stringLen + pasteLen > mMaxStrLen)
   {
      pasteLen = mMaxStrLen - stringLen;

      pasteText.cut(pasteLen, pasteText.length() - pasteLen);
   }

   if (mCursorPos == stringLen)
   {
      mTextBuffer.append(pasteText);
   }
   else
   {
      mTextBuffer.insert(mCursorPos, pasteText);
   }

   mCursorPos += pasteLen;
}

void GuiTextEditCtrl::onUndo()
{
    StringBuffer tempBuffer;
    S32 tempBlockStart;
    S32 tempBlockEnd;
    S32 tempCursorPos;

    //save the current
    tempBuffer.set(&mTextBuffer);
    tempBlockStart = mBlockStart;
    tempBlockEnd   = mBlockEnd;
    tempCursorPos  = mCursorPos;

    //restore the prev
    mTextBuffer.set(&mUndoText);
    mBlockStart = mUndoBlockStart;
    mBlockEnd   = mUndoBlockEnd;
    mCursorPos  = mUndoCursorPos;

    //update the undo
    mUndoText.set(&tempBuffer);
    mUndoBlockStart = tempBlockStart;
    mUndoBlockEnd   = tempBlockEnd;
    mUndoCursorPos  = tempCursorPos;
}

void GuiTextEditCtrl::setMaxNum(S32 maxNum)
{
	mMaxNum = maxNum;
}

void GuiTextEditCtrl::setMinNum(S32 minNum)
{
	mMinNum = minNum;
}

void GuiTextEditCtrl::ClampNum(bool bMax,bool bMin,S32 iMax,S32 iMin)
{
	mbUseMaxNum = bMax;
	mbUseMinNum = bMin;
	mMaxNum     = iMax;
	mMinNum     = iMin;
}

bool GuiTextEditCtrl::onKeyDown(const GuiEvent &event)
{
   if(! isActive())
      return false;

   S32 stringLen = mTextBuffer.length();
   setUpdate();

   // Ugly, but now I'm cool like MarkF.
   if(event.keyCode == KEY_BACKSPACE)
      goto dealWithBackspace;

   if (event.modifier & SI_SHIFT)
    {
        switch (event.keyCode)
        {
            case KEY_TAB:
               if ( mTabComplete )
               {
                  Con::executef( this, "onTabComplete", "1" );
                  return( true );
               }
			   break; // We don't want to fall through if we don't handle the TAB here.

            case KEY_HOME:
               mBlockStart = 0;
               mBlockEnd = mCursorPos;
               mCursorPos = 0;
               return true;

            case KEY_END:
                mBlockStart = mCursorPos;
                mBlockEnd = stringLen;
                mCursorPos = stringLen;
                return true;

            case KEY_LEFT:
                if ((mCursorPos > 0) & (stringLen > 0))
                {
                    //if we already have a selected block
                    if (mCursorPos == mBlockEnd)
                    {
                        mCursorPos--;
                        mBlockEnd--;
                        if (mBlockEnd == mBlockStart)
                        {
                            mBlockStart = 0;
                            mBlockEnd = 0;
                        }
                    }
                    else {
                        mCursorPos--;
                        mBlockStart = mCursorPos;

                        if (mBlockEnd == 0)
                        {
                            mBlockEnd = mCursorPos + 1;
                        }
                    }
                }
                return true;

            case KEY_RIGHT:
                if (mCursorPos < stringLen)
                {
                    if ((mCursorPos == mBlockStart) && (mBlockEnd > 0))
                    {
                        mCursorPos++;
                        mBlockStart++;
                        if (mBlockStart == mBlockEnd)
                        {
                            mBlockStart = 0;
                            mBlockEnd = 0;
                        }
                    }
                    else
                    {
                        if (mBlockEnd == 0)
                        {
                            mBlockStart = mCursorPos;
                            mBlockEnd = mCursorPos;
                        }
                        mCursorPos++;
                        mBlockEnd++;
                    }
                }
                return true;
        }
    }
   else if (event.modifier & SI_CTRL)
   {
      switch(event.keyCode)
      {
         // Added UNIX emacs key bindings - just a little hack here...

         // BJGTODO: Add vi bindings.

         // Ctrl-B - move one character back
         case KEY_B:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_LEFT;
            return(onKeyDown(new_event));
         }

         // Ctrl-F - move one character forward
         case KEY_F:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_RIGHT;
            return(onKeyDown(new_event));
         }

         // Ctrl-A - move to the beginning of the line
         case KEY_A:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_HOME;
            return(onKeyDown(new_event));
         }

         // Ctrl-E - move to the end of the line
         case KEY_E:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_END;
            return(onKeyDown(new_event));
         }

         // Ctrl-P - move backward in history
         case KEY_P:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_UP;
            return(onKeyDown(new_event));
         }

         // Ctrl-N - move forward in history
         case KEY_N:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_DOWN;
            return(onKeyDown(new_event));
         }

         // Ctrl-D - delete under cursor
         case KEY_D:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_DELETE;
            return(onKeyDown(new_event));
         }

         case KEY_U:
         { 
            GuiEvent new_event;
            new_event.modifier = SI_CTRL;
            new_event.keyCode = KEY_DELETE;
            return(onKeyDown(new_event));
         }

         // End added UNIX emacs key bindings
         
#if !defined(POWER_OS_MAC)
         // windows style cut / copy / paste / undo keybinds
         case KEY_C:
         case KEY_X:
         {
            // copy, and cut the text if we hit ctrl-x
            onCopy( event.keyCode==KEY_X );
            return true;
         }
         case KEY_V:
         {
            onPaste();

            // Execute the console command!
            execConsoleCallback();
            return true;
         }

         case KEY_Z:
            if (! mDragHit)
            {
               onUndo();
               return true;
            }
#endif

         case KEY_DELETE:
         case KEY_BACKSPACE:
            //save the current state
            saveUndoState();

            //delete everything in the field
            mTextBuffer.set("");
            mCursorPos  = 0;
            mBlockStart = 0;
            mBlockEnd   = 0;

            execConsoleCallback();

            return true;
      }
   }
   else if (event.modifier & SI_ALT)
   {
	   //[editor]thinkig:处理快捷键
	   return false;
   }
#if defined(POWER_OS_MAC)
   // mac style cut / copy / paste / undo keybinds
   else if (event.modifier & SI_ALT)
   {
      // Added Mac cut/copy/paste/undo keys
      // Mac command key maps to alt in PowerEngine.
      switch(event.keyCode)
      {
         case KEY_C:
         case KEY_X:
         {
            // copy, and cut the text if we hit cmd-x
            onCopy( event.keyCode==KEY_X );
            return true;
         }
         case KEY_V:
         {
            onPaste();

            // Execute the console command!
            execConsoleCallback();

            return true;
         }
            
         case KEY_Z:
            if (! mDragHit)
            {
               onUndo();
               return true;
            }
      }
   }
#endif
   else
   {
	  Con::executef( this, "OnKeyDown" );
      switch(event.keyCode)
      {
         case KEY_ESCAPE:
            if ( mEscapeCommand[0] )
            {
               Con::setVariable("$ThisControl", avar("%d", getId()));
               Con::evaluate( mEscapeCommand );
               return( true );
            }
            return( Parent::onKeyDown( event ) );

         case KEY_RETURN:
			 {
				 Con::executef( this, "OnKeyReturn" );
				 if(!mKeyEnter)  return false;
			 }
         case KEY_NUMPADENTER:
            //first validate			
            if (mProfile->mReturnTab)
            {
               onLoseFirstResponder();
            }

            updateHistory(&mTextBuffer, true);
            mHistoryDirty = false;

            //next exec the alt console command
            execAltConsoleCallback();

            // Notify of Return
            if ( isMethod("onReturn") )
               Con::executef( this, "onReturn" );

            if (mProfile->mReturnTab)
            {
               GuiCanvas *root = getRoot();
               if (root)
               {
                  root->tabNext();
                  return true;
               }
            }                     
			return true;

         case KEY_UP:
         {
            if(mHistoryDirty)
            {
               updateHistory(&mTextBuffer, false);
               mHistoryDirty = false;
            }

            mHistoryIndex--;
            
            if(mHistoryIndex >= 0 && mHistoryIndex <= mHistoryLast)
               setText(mHistoryBuf[mHistoryIndex]);
            else if(mHistoryIndex < 0)
               mHistoryIndex = 0;
            
            return true;
         }

         case KEY_DOWN:
            if(mHistoryDirty)
            {
               updateHistory(&mTextBuffer, false);
               mHistoryDirty = false;
            }
            mHistoryIndex++;
            if(mHistoryIndex > mHistoryLast)
            {
               mHistoryIndex = mHistoryLast + 1;
               setText("");
            }
            else
               setText(mHistoryBuf[mHistoryIndex]);
            return true;

         case KEY_LEFT:
            mBlockStart = 0;
            mBlockEnd = 0;
            if (mCursorPos > 0)
            {
                mCursorPos--;
            }
            return true;

         case KEY_RIGHT:
            mBlockStart = 0;
            mBlockEnd = 0;
            if (mCursorPos < stringLen)
            {
                mCursorPos++;
            }
            return true;

         case KEY_BACKSPACE:
dealWithBackspace:
            //save the current state
            saveUndoState();

            if (mBlockEnd > 0)
            {
               mTextBuffer.cut(mBlockStart, mBlockEnd-mBlockStart);
               mCursorPos  = mBlockStart;
               mBlockStart = 0;
               mBlockEnd   = 0;
               mHistoryDirty = true;

               // Execute the console command!
               execConsoleCallback();

            }
            else if (mCursorPos > 0)
            {
				// <Edit>:[thinking]:处理非法起始点问题
				if(mTextBuffer.length() == 0)
					return true;

				if(mTextBuffer.length() < mCursorPos )
					mCursorPos = mTextBuffer.length();

				mTextBuffer.cut(mCursorPos-1, 1);
				mCursorPos--;        
				mHistoryDirty = true;

               // Execute the console command!
               execConsoleCallback();
            }
            return true;

         case KEY_DELETE:
            //save the current state
            saveUndoState();

            if (mBlockEnd > 0)
            {
               mHistoryDirty = true;
               mTextBuffer.cut(mBlockStart, mBlockEnd-mBlockStart);

               mCursorPos = mBlockStart;
               mBlockStart = 0;
               mBlockEnd = 0;

               // Execute the console command!
               execConsoleCallback();
            }
            else if (mCursorPos < stringLen)
            {
               mHistoryDirty = true;
               mTextBuffer.cut(mCursorPos, 1);

               // Execute the console command!
               execConsoleCallback();
            }
            return true;

         case KEY_INSERT:
            mInsertOn = !mInsertOn;
            return true;

         case KEY_HOME:
            mBlockStart = 0;
            mBlockEnd   = 0;
            mCursorPos  = 0;
            return true;

         case KEY_END:
            mBlockStart = 0;
            mBlockEnd   = 0;
            mCursorPos  = stringLen;
            return true;

         }
   }

   switch ( event.keyCode )
   {
      case KEY_TAB:
         if ( mTabComplete )
         {
            Con::executef( this, "onTabComplete", "0" );
            return( true );
         }
      case KEY_UP:
      case KEY_DOWN:
      case KEY_ESCAPE:
         return Parent::onKeyDown( event );
   }

   // If it's a typable key event, eat it.
   if(event.keyCode && Input::getAscii(event.keyCode, STATE_UPPER))
      return true;

   if ( oneUTF16toUTF32(&event.ascii) )
   {
      // Get the character ready to add to a UTF8 string.
      UTF16 convertedChar[2] = { event.ascii, 0 };

      //see if it's a number field
      if ( mProfile->mNumbersOnly )
      {
		  // <Edit>:[thinking]:过滤连续零或零以后还可以输入数字的情况
		  if( event.ascii == '0' && ((mCursorPos == 1 && mTextBuffer.getChar(0) == '0') 
			  || (mCursorPos == 0 && mTextBuffer.getChar(0) != '\0') ))
		  {
			  playDeniedSound();
			  return true;
		  }

		  if (mCursorPos > 0 && mTextBuffer.getChar(0) == '0')
		  {
			  playDeniedSound();
			  return true;
		  }

         if (event.ascii == '-')
         {
            //a minus sign only exists at the beginning, and only a single minus sign
            if ( mCursorPos != 0 )
            {
               playDeniedSound();
               return true;
            }

            if ( mInsertOn && ( mTextBuffer.getChar(0) == '-' ) ) 
            {
               playDeniedSound();
               return true;
            }
         }
         // BJTODO: This is probably not unicode safe.
         else if ( event.ascii < '0' || event.ascii > '9' )
         {
            playDeniedSound();
            return true;
         }
      }

      //save the current state
      saveUndoState();

      //delete anything highlighted
      if ( mBlockEnd > 0 )
      {
         mTextBuffer.cut(mBlockStart, mBlockEnd-mBlockStart);
         mCursorPos  = mBlockStart;
         mBlockStart = 0;
         mBlockEnd   = 0;
      }

      if ( ( mInsertOn && ( stringLen < mMaxStrLen ) ) ||
          ( !mInsertOn && ( mCursorPos < mMaxStrLen ) ) )
      {
         if ( mCursorPos == stringLen )
         {
            mTextBuffer.append(convertedChar);
            mCursorPos++;
         }
         else
         {
            if ( mInsertOn )
            {
               mTextBuffer.insert(mCursorPos, convertedChar);
               mCursorPos++;
            }
            else
            {
               mTextBuffer.cut(mCursorPos, 1);
               mTextBuffer.insert(mCursorPos, convertedChar);
               mCursorPos++;
            }
         }
      }
      else
         playDeniedSound();

	  // <Edit>:[thinking]:增加最大最小值处理
	  if ( mProfile->mNumbersOnly )
	  {
		  S32 numVual = 0;
		  char strBuff[MAX_STRING_LENGTH] = {0,};
		  getText(strBuff);
		 
		  if(mTextBuffer.getChar(0) == '-')
			  numVual = -1 * atoi(strBuff+1);
		  else
			  numVual = atoi(strBuff);

		  if(mbUseMaxNum && numVual> mMaxNum)
		  {
			  dSprintf(strBuff,sizeof(strBuff),"%d",mMaxNum);
			  mTextBuffer.set(strBuff);
		  }
			  
		  if(mbUseMinNum && numVual < mMinNum)
		  {
			  dSprintf(strBuff,sizeof(strBuff),"%d",mMinNum);
			  mTextBuffer.set(strBuff);
		  }
	  }

      //reset the history index
      mHistoryDirty = true;

      //execute the console command if it exists
      execConsoleCallback();

      return true;
   }

   //not handled - pass the event to it's parent

   // Or eat it if that's appropriate.
   if (mSinkAllKeyEvents)
      return true;

   return Parent::onKeyDown( event );
}

void GuiTextEditCtrl::setFirstResponder()
{
   Parent::setFirstResponder();

   GuiCanvas *root = getRoot();
   if (root != NULL)
   {
		root->enableKeyboardTranslation();
  

	   // If the native OS accelerator keys are not disabled
		// then some key events like Delete, ctrl+V, etc may
		// not make it down to us.
		root->setNativeAcceleratorsEnabled( false );
   }
}

void GuiTextEditCtrl::onLoseFirstResponder()
{
   GuiCanvas *root = getRoot();
   root->setNativeAcceleratorsEnabled( true );
   root->disableKeyboardTranslation();

   //first, update the history
   updateHistory( &mTextBuffer, true );

   //execute the validate command
   if ( mValidateCommand[0] )
   {
      char buf[16];
      dSprintf(buf, sizeof(buf), "%d", getId());
      Con::setVariable("$ThisControl", buf);
      Con::evaluate( mValidateCommand, false );
   }

   if( isMethod( "onValidate" ) )
      Con::executef( this, "onValidate" );

   // Redraw the control:
   setUpdate();
}


void GuiTextEditCtrl::parentResized(const RectI &oldParentRect, const RectI &newParentRect)
{
   Parent::parentResized( oldParentRect, newParentRect );
   mTextOffsetReset = true;
}

void GuiTextEditCtrl::onRender(Point2I offset, const RectI & /*updateRect*/)
{
   RectI ctrlRect( offset, getExtent() );

   //if opaque, fill the update rect with the fill color
   if ( mProfile->mOpaque )
   {
      if(isFirstResponder())
         GFX->getDrawUtil()->drawRectFill( ctrlRect, mProfile->mFillColorHL );
      else
         GFX->getDrawUtil()->drawRectFill( ctrlRect, mProfile->mFillColor );
   }

   //if there's a border, draw the border
   if ( mProfile->mBorder )
      renderBorder( ctrlRect, mProfile );

   drawText( ctrlRect, isFirstResponder() );
   
}

void GuiTextEditCtrl::onPreRender()
{
   if ( isFirstResponder() )
   {
      U32 timeElapsed = Platform::getVirtualMilliseconds() - mTimeLastCursorFlipped;
      mNumFramesElapsed++;
      if ( ( timeElapsed > 500 ) && ( mNumFramesElapsed > 3 ) )
      {
         mCursorOn = !mCursorOn;
         mTimeLastCursorFlipped = Platform::getVirtualMilliseconds();
         mNumFramesElapsed = 0;
         setUpdate();
      }

      //update the cursor if the text is scrolling
      if ( mDragHit )
      {
         if ( ( mScrollDir < 0 ) && ( mCursorPos > 0 ) )
            mCursorPos--;
         else if ( ( mScrollDir > 0 ) && ( mCursorPos < (S32) mTextBuffer.length() ) )
            mCursorPos++;
      }
   } 
}

void GuiTextEditCtrl::drawText( const RectI &drawRect, bool isFocused )
{
   StringBuffer textBuffer;
   Point2I drawPoint = drawRect.point;
   Point2I paddingLeftTop, paddingRightBottom;

   // Just a little sanity.
   if(mCursorPos > mTextBuffer.length()) 
      mCursorPos = mTextBuffer.length();

   // Apply password masking (make the masking char optional perhaps?)
   if(mPasswordText)
   {
      for(U32 i = 0; i<mTextBuffer.length(); i++)
         textBuffer.append(mPasswordMask);
   }
   else
   {
      // Or else just copy it over.
      textBuffer.set(&mTextBuffer);
   }

   paddingLeftTop.set(( mProfile->mTextOffset.x != 0 ? mProfile->mTextOffset.x : 3 ), mProfile->mTextOffset.y);
   paddingRightBottom = paddingLeftTop;

   // Center vertically:
   drawPoint.y += ( ( drawRect.extent.y - paddingLeftTop.y - paddingRightBottom.y - mFont->getHeight() ) / 2 ) + paddingLeftTop.y;

   // Align horizontally:
   char oo[MAX_STRING_LENGTH];
   ZeroMemory(oo,MAX_STRING_LENGTH);
   convertUTF16toUTF8(textBuffer.getPtr(), oo, MAX_STRING_LENGTH);
   S32 textWidth = mFont->getStrWidth(oo);

   if ( drawRect.extent.x - paddingLeftTop.x > textWidth )
   {
      switch( mProfile->mAlignment )
      {
      case GuiControlProfile::RightJustify:
         drawPoint.x += ( drawRect.extent.x - textWidth - paddingRightBottom.x );
         break;
      case GuiControlProfile::CenterJustify:
         drawPoint.x += ( ( drawRect.extent.x - textWidth ) / 2 );
         break;
      default:
      case GuiControlProfile::LeftJustify :
         drawPoint.x += paddingLeftTop.x;
         break;
      }
   }
   else
      drawPoint.x += paddingLeftTop.x;

   ColorI fontColor = mActive ? mProfile->mFontColor : mProfile->mFontColorNA;

   // now draw the text
   Point2I cursorStart, cursorEnd;
   mTextOffset.y = drawPoint.y;
   if ( mTextOffsetReset )
   {
      mTextOffset.x = drawPoint.x;
      mTextOffsetReset = false;
   }

   if ( drawRect.extent.x - paddingLeftTop.x > textWidth )
      mTextOffset.x = drawPoint.x;
   else
   {
      // Alignment affects large text
      if ( mProfile->mAlignment == GuiControlProfile::RightJustify
         || mProfile->mAlignment == GuiControlProfile::CenterJustify )
      {
         if ( mTextOffset.x + textWidth < (drawRect.point.x + drawRect.extent.x) - paddingRightBottom.x)
            mTextOffset.x = (drawRect.point.x + drawRect.extent.x) - paddingRightBottom.x - textWidth;
      }
   }

   // calculate the cursor
   if ( isFocused )
   {
      // Where in the string are we?
      S32 cursorOffset=0, charWidth=0;
      UTF16 tempChar = mTextBuffer.getChar(mCursorPos);

	  // Alright, we want to terminate things momentarily.
	  if(mCursorPos > 0)
	  {
		  wchar_t oo[512];
		  ZeroMemory(oo,MAX_STRING_LENGTH);
		  wcsncpy_s(oo, textBuffer.getPtr(), mCursorPos);
		  char bb[MAX_STRING_LENGTH];
		  ZeroMemory(bb,MAX_STRING_LENGTH);
		  convertUTF16toUTF8(oo, bb, MAX_STRING_LENGTH);

		  cursorOffset = mFont->getStrWidth(bb);
	  }
	  else
         cursorOffset = 0;

      if ( tempChar )
	  {
	     char oo[2];
         ZeroMemory(oo,2);
         convertUTF16toUTF8(&tempChar, oo, 2);
         charWidth = mFont->getStrWidth( oo );
	  }
      else
         charWidth = paddingRightBottom.x;

      if( mTextOffset.x + cursorOffset + charWidth >= (drawRect.point.x + drawRect.extent.x) - paddingLeftTop.x )
      {
         // Cursor somewhere beyond the textcontrol,
         // skip forward roughly 25% of the total width (if possible)
         S32 skipForward = drawRect.extent.x / 4;

         if ( cursorOffset + skipForward > textWidth )
            mTextOffset.x = (drawRect.point.x + drawRect.extent.x) - paddingRightBottom.x - textWidth;
         else
            mTextOffset.x -= skipForward;
      }
      else if( mTextOffset.x + cursorOffset < drawRect.point.x + paddingLeftTop.x )
      {
         // Cursor somewhere before the textcontrol
         // skip backward roughly 25% of the total width (if possible)
         S32 skipBackward = drawRect.extent.x / 4;

         if ( cursorOffset - skipBackward < 0 )
            mTextOffset.x = drawRect.point.x + paddingLeftTop.x;
         else
            mTextOffset.x += skipBackward;
      }
      cursorStart.x = mTextOffset.x + cursorOffset;
      cursorEnd.x = cursorStart.x;

      S32 cursorHeight = mFont->getHeight();
      if ( cursorHeight < drawRect.extent.y )
      {
         cursorStart.y = drawPoint.y;
         cursorEnd.y = cursorStart.y + cursorHeight;
      }
      else
      {
         cursorStart.y = drawRect.point.y;
         cursorEnd.y = cursorStart.y + drawRect.extent.y;
      }
   }

   //draw the text
   if ( !isFocused )
      mBlockStart = mBlockEnd = 0;

   //also verify the block start/end
   if ((mBlockStart > textBuffer.length() || (mBlockEnd > textBuffer.length()) || (mBlockStart > mBlockEnd)))
      mBlockStart = mBlockEnd = 0;

   Point2I tempOffset = mTextOffset;

   //draw the portion before the highlight
   if ( mBlockStart > 0 )
   {
      GFX->getDrawUtil()->setBitmapModulation( fontColor );
      const UTF16* preString2 = textBuffer.getPtr();
	  wchar_t oo[512];
	  ZeroMemory(oo,MAX_STRING_LENGTH);
	  wcsncpy_s(oo, preString2, wcslen(preString2));
	  char bb[MAX_STRING_LENGTH];
	  ZeroMemory(bb,MAX_STRING_LENGTH);
	  convertUTF16toUTF8(oo, bb, MAX_STRING_LENGTH);
      GFX->getDrawUtil()->drawText( mFont, tempOffset, bb, mProfile->mFontColors );
	  ZeroMemory(oo,MAX_STRING_LENGTH);
	  wcsncpy_s(oo, preString2, mBlockStart);
      ZeroMemory(bb,MAX_STRING_LENGTH);
	  convertUTF16toUTF8(oo, bb, MAX_STRING_LENGTH);
      tempOffset.x += mFont->getStrWidth(bb);
   }

   //draw the hilighted portion
   if ( mBlockEnd > 0 )
   {
      const UTF16* highlightBuff = textBuffer.getPtr() + mBlockStart;
      U32 highlightBuffLen = mBlockEnd-mBlockStart;

	  wchar_t oo[512];
	  ZeroMemory(oo,MAX_STRING_LENGTH);
	  wcsncpy_s(oo, highlightBuff, highlightBuffLen);
	  char bb[MAX_STRING_LENGTH];
	  ZeroMemory(bb,MAX_STRING_LENGTH);
	  convertUTF16toUTF8(oo, bb, MAX_STRING_LENGTH);
      S32 highlightWidth = mFont->getStrWidth(bb);

      GFX->getDrawUtil()->drawRectFill( Point2I( tempOffset.x, drawRect.point.y ),
         Point2I( tempOffset.x + highlightWidth, drawRect.point.y + drawRect.extent.y - 1),
         mProfile->mFontColorSEL );

      GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColorHL );
      GFX->getDrawUtil()->drawTextN( mFont, tempOffset, highlightBuff, highlightBuffLen, mProfile->mFontColors );
      tempOffset.x += highlightWidth;
   }

   //draw the portion after the highlite
   if(mBlockEnd < mTextBuffer.length())
   {
      const UTF16* finalBuff = textBuffer.getPtr() + mBlockEnd;
      U32 finalBuffLen = mTextBuffer.length() - mBlockEnd;

      GFX->getDrawUtil()->setBitmapModulation( fontColor );
      GFX->getDrawUtil()->drawTextN( mFont, tempOffset, finalBuff, finalBuffLen, mProfile->mFontColors );
   }

   //draw the cursor
   if ( isFocused && mCursorOn ) 
   {
	   GFX->getDrawUtil()->drawLine( cursorStart, cursorEnd, mProfile->mCursorColor );
	   // <Edit>:[thinking]:加粗光标
	   GFX->getDrawUtil()->drawLine( cursorStart.x+1, cursorStart.y,cursorEnd.x+1,cursorEnd.y,mProfile->mCursorColor );
   }
      
}

void GuiTextEditCtrl::InsertString(StringTableEntry txt)
{
	//  <Edit>:[thinking]:修改计算长度错误问题
	UTF16* temp =  convertUTF8toUTF16(txt);
	U16 strLen = wcslen(temp);
	if (mTextBuffer.length() + strLen <= mMaxStrLen)
	{
		if( mCursorPos > 0 && mTextBuffer.getChar(0) == '0' || ((mCursorPos == 1 && mTextBuffer.getChar(0) == '0') 
			|| (mCursorPos == 0 && mTextBuffer.getChar(0) != '\0') ))
		{
			return;
		}
		if (mTextBuffer.length() == mCursorPos)
			mTextBuffer.append(temp);
		else
		mTextBuffer.insert(mCursorPos,temp);
		mCursorPos += strLen;
	}

	delete []temp;
}


void GuiTextEditCtrl::DeleteString()
{
	if (mTextBuffer.length() > 0)
	{
		if (mBlockEnd > 0)
		{
			mTextBuffer.cut(mBlockStart, mBlockEnd-mBlockStart);
			mCursorPos  = mBlockStart;
			mBlockStart = 0;
			mBlockEnd   = 0;
			mHistoryDirty = true;
			execConsoleCallback();

		}
		else if (mCursorPos > 0)
		{
			mTextBuffer.cut(mCursorPos - 1, 1);
			mCursorPos--;
			mHistoryDirty = true;
			execConsoleCallback();
		}
	}
}

bool GuiTextEditCtrl::hasText()
{
   return (mTextBuffer.length());
}

void GuiTextEditCtrl::playDeniedSound()
{
   if ( mSoundID != NULL )
      SFX->playOnce( mSoundID );
}

const char *GuiTextEditCtrl::getScriptValue()
{
   return StringTable->insert(mTextBuffer.getPtr8());
}

void GuiTextEditCtrl::setScriptValue(const char *value)
{
   mTextBuffer.set(value);
   mCursorPos = mTextBuffer.length() - 1;
}

ConsoleMethod( GuiTextEditCtrl, getText, const char*, 2, 2, "textEditCtrl.getText()" )
{
   argc; argv;
   if( !object->hasText() )
      return StringTable->insert("");

   char *retBuffer = Con::getReturnBuffer( GuiTextEditCtrl::MAX_STRING_LENGTH );
   object->getText( retBuffer );

   return retBuffer;
}

ConsoleMethod( GuiTextEditCtrl, setText, void, 3, 3, "textEditCtrl.setText( %text )" )
{
   argc; argv;
   object->setText( argv[2] );
}


ConsoleMethod( GuiTextEditCtrl, getCursorPos, S32, 2, 2, "textEditCtrl.getCursorPos()" )
{
   argc; argv;
   return( object->getCursorPos() );
}

ConsoleMethod( GuiTextEditCtrl, setCursorPos, void, 3, 3, "textEditCtrl.setCursorPos( newPos )" )
{
   argc;
   object->reallySetCursorPos( dAtoi( argv[2] ) );
}

ConsoleMethod( GuiTextEditCtrl, selectAllText, void, 2, 2, "textEditCtrl.selectAllText()" )
{
   argc; argv;
   object->selectAllText();
}

ConsoleMethod( GuiTextEditCtrl, forceValidateText, void, 2, 2, "textEditCtrl.forceValidateText()" )
{
   argc; argv;
   object->forceValidateText();
}

ConsoleMethod( GuiTextEditCtrl, InsertString, void, 3, 3, "textEditCtrl.InsertString( %text )")
{
	argc;argv;
	object->InsertString( argv[2]);
}

ConsoleMethod( GuiTextEditCtrl, DeleteString, void, 2, 2, "textEditCtrl.DeleteString()")
{
	argc;argv;
	object->DeleteString();
}

ConsoleMethod( GuiTextEditCtrl,SetMaxNum,void, 3, 3, "textEditCtrl.setMaxNum(%Max)")
{
	argc;argv;
	object->setMaxNum( atoi (argv [2]) );
}

ConsoleMethod( GuiTextEditCtrl,SetMinNum,void, 3, 3, "textEditCtrl.setMinNum(%Min)")
{
	argc;argv;
	object->setMinNum( atoi (argv [2]) );
}