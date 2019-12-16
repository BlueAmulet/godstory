//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "windowManager/platformWindow.h"

#ifndef _WINDOW_INPUTGENERATOR_H_
#define _WINDOW_INPUTGENERATOR_H_

class IProcessInput;

class WindowInputGenerator
{
   bool mNotifyPosition;
protected:
   PlatformWindow *mWindow;
   IProcessInput  *mInputController;
   Point2I         mLastCursorPos;
   bool            mClampToWindow;
   bool            mFocused; ///< We store this off to avoid polling the OS constantly

   ///  This is the scale factor which relates  mouse movement in pixels
   /// (one unit of mouse movement is a mickey) to units in the GUI.
   F32             mPixelsPerMickey;

   // Event Handlers
   void handleMouseButton(DeviceId did, U32 modifier,  U32 action, U16 button);
   void handleMouseWheel (DeviceId did, U32 modifier,  S32 wheelDelta);
   void handleMouseMove  (DeviceId did, U32 modifier,  S32 x,      S32 y, bool isRelative);
   void handleKeyboard   (DeviceId did, U32 modifier,  U32 action, U16 key);
   void handleCharInput  (DeviceId did, U32 modifier,  U16 key);
   void handleAppEvent   (DeviceId did, S32 event);
   void handleInputEvent (U32 deviceInst,F32 fValue, U16 deviceType, U16 objType, U16 ascii, U16 objInst, U8 action, U8 modifier);

   void generateInputEvent( InputEventInfo &inputEvent );
public:
   WindowInputGenerator( PlatformWindow *window );
   virtual ~WindowInputGenerator();

   void setInputController( IProcessInput *inputController ) { mInputController = inputController; };
};

#endif // _WINDOW_INPUTGENERATOR_H_