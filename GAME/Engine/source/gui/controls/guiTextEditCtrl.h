//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUITEXTEDITCTRL_H_
#define _GUITEXTEDITCTRL_H_

#ifndef _GUITYPES_H_
#include "gui/core/guiTypes.h"
#endif
#ifndef _GUITEXTCTRL_H_
#include "gui/controls/guiTextCtrl.h"
#endif
#ifndef _STRINGBUFFER_H_
#include "core/stringBuffer.h"
#endif

class GuiTextEditCtrl : public GuiTextCtrl
{
private:
   typedef GuiTextCtrl Parent;

   static U32 smNumAwake;

protected:

   StringBuffer mTextBuffer;

   StringTableEntry mValidateCommand;
   StringTableEntry mEscapeCommand;
  // SFXProfile*  mDeniedSound;
   StringTableEntry mSoundID;

   // for animating the cursor
   S32      mNumFramesElapsed;
   U32      mTimeLastCursorFlipped;
   ColorI   mCursorColor;
   bool     mCursorOn;

   bool     mInsertOn;
   S32      mMouseDragStart;
   Point2I  mTextOffset;
   bool     mTextOffsetReset;
   bool     mDragHit;
   bool     mTabComplete;
   bool     mKeyEnter; //是否对Enter按键消息处理
   S32      mScrollDir;

   //undo members
   StringBuffer mUndoText;
   S32      mUndoBlockStart;
   S32      mUndoBlockEnd;
   S32      mUndoCursorPos;
   void saveUndoState();

   S32      mBlockStart;
   S32      mBlockEnd;
   S32      mCursorPos;
   S32 setCursorPos(const Point2I &offset);

   bool                 mHistoryDirty;
   S32                  mHistoryLast;
   S32                  mHistoryIndex;
   S32                  mHistorySize;
   bool                 mPasswordText;
   StringTableEntry     mPasswordMask;

   bool mbUseMaxNum;
   bool mbUseMinNum;
   S32  mMaxNum;
   S32  mMinNum;

   /// If set, any non-ESC key is handled here or not at all
   bool    mSinkAllKeyEvents;   
   UTF16   **mHistoryBuf;
   void updateHistory(StringBuffer *txt, bool moveIndex);

   void playDeniedSound();
   void execConsoleCallback();

public:
   GuiTextEditCtrl();
   ~GuiTextEditCtrl();
   DECLARE_CONOBJECT(GuiTextEditCtrl);
   static void initPersistFields();

   bool onAdd();
   bool onWake();
   void onSleep();

   /// Get the contents of the control.
   ///
   /// dest should be of size GuiTextCtrl::MAX_STRING_LENGTH+1.
   void getText(char *dest);

   void setText(S32 tag);
   virtual void setText(const UTF8* txt);
   virtual void setText(const UTF16* txt);
   S32   getCursorPos()   { return( mCursorPos ); }
   void  reallySetCursorPos( const S32 newPos );
   
   void selectAllText();
   void forceValidateText();
   const char *getScriptValue();
   void setScriptValue(const char *value);

   bool onKeyDown(const GuiEvent &event);
   void onMouseDown(const GuiEvent &event);
   void onMouseDragged(const GuiEvent &event);
   void onMouseUp(const GuiEvent &event);
   
   void onCopy(bool andCut);
   void onPaste();
   void onUndo();

   void setMaxNum(S32 maxNum);
   void setMinNum(S32 minNum);
   void ClampNum(bool bMax,bool bMin,S32 iMax,S32 iMin);

   virtual void setFirstResponder();
   virtual void onLoseFirstResponder();

   void parentResized(const RectI &oldParentRect, const RectI &newParentRect);
   bool hasText();

   void onStaticModified(const char* slotName, const char* newValue = NULL);

   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);
   virtual void drawText( const RectI &drawRect, bool isFocused );

   void InsertString(StringTableEntry txt);//小键盘输入字符函数 [02/03/2009 soar]
   void DeleteString();//小键盘删除字符函数 [02/03/2009 soar]
};

#endif //_GUI_TEXTEDIT_CTRL_H
