//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "windowManager/platformCursorController.h"
#include <windows.h>

void PlatformCursorController::pushCursor(StringTableEntry cursorname)
{
   // Place the new cursor shape onto the stack
   mCursors.increment();
   mCursors.last().mCursorName = cursorname;

   // Now actually change the shape
   setCursorShape(mCursors.last());
}

void PlatformCursorController::popCursor()
{
   // Before poping the stack, make sure we're not trying to remove the last cursor shape
   if(mCursors.size() <= 1)
      return;

   // Pop the stack
   mCursors.pop_back();

   // Now set the cursor shape
   //setCursorShape(mCursors.last()); 
   ::SetCursor((HCURSOR)mCursors.last().mCursorHandle);
}

void PlatformCursorController::refreshCursor()
{
	// Refresh the cursor's shape
	//setCursorShape(mCursors.last());
	::SetCursor((HCURSOR)mCursors.last().mCursorHandle);
}