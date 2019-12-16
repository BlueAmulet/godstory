//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORMCURSORCONTROLLER_H_
#define _PLATFORMCURSORCONTROLLER_H_

#include "core/tVector.h"

class PlatformWindow;


class PlatformCursorController
{
protected:
   struct PlatformCursor
   {
      StringTableEntry mCursorName;	// CursorÍ¼±êÃû
	  void* mCursorHandle;			// CursorµÄ¾ä±ú
   };

   Vector<PlatformCursor> mCursors;

   PlatformWindow *mOwner;

public:
   PlatformCursorController( PlatformWindow *owner ) { mOwner = owner; };
   virtual ~PlatformCursorController() { mOwner=NULL; };

   enum 
   {
      curArrow = 0,
      curWait,
      curPlus,
      curResizeVert,
      curResizeHorz,
      curResizeAll,
      curIBeam,
      curResizeNESW,
      curResizeNWSE,
   };

public:
   virtual void setCursorPosition(S32 x, S32 y) = 0;
   virtual void getCursorPosition( Point2I &point ) = 0;
   virtual void setCursorVisible(bool visible) = 0;
   virtual bool isCursorVisible() = 0;

   virtual void setCursorShape(PlatformCursor& cursor) = 0;

   virtual void pushCursor(StringTableEntry cursorname); 
   virtual void popCursor();
   virtual void refreshCursor();

   virtual U32 getDoubleClickTime() = 0;
   virtual S32 getDoubleClickWidth() = 0;
   virtual S32 getDoubleClickHeight() = 0;
   Vector<PlatformCursor> getCursorVector() { return mCursors; }
   virtual StringTableEntry getSysCursor(U32 index) = 0;
};


#endif