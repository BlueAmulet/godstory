//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef  _WIN32_CURSORCONTROLLER_H_
#define  _WIN32_CURSORCONTROLLER_H_

#include <windows.h>
#include "windowManager/platformCursorController.h"


class Win32CursorController : public PlatformCursorController
{
public:
   Win32CursorController( PlatformWindow *owner ) : PlatformCursorController(owner)
   {
	  StringTableEntry defaultCursorName = StringTable->insert("Default");
      pushCursor(defaultCursorName);
   };

   virtual void setCursorPosition(S32 x, S32 y);
   virtual void getCursorPosition( Point2I &point );
   virtual void setCursorVisible(bool visible);
   virtual bool isCursorVisible();

   void setCursorShape(PlatformCursor& cursor);

   void pushCursor(StringTableEntry cursorname); 
   void popCursor();
   void refreshCursor();

   U32 getDoubleClickTime();
   S32 getDoubleClickWidth();
   S32 getDoubleClickHeight();
   StringTableEntry getSysCursor(U32 index);
};


#endif
