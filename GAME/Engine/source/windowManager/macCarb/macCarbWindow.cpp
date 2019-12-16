//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "ApplicationServices/ApplicationServices.h"
#include <OpenGL/CGLTypes.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/OpenGL.h>
#include "gfx/GL/ggl/ggl.h"

#define __gl_h_
#include <AGL/agl.h>

#include "windowManager/macCarb/macCarbWindow.h"
#include "gfx/gfxStructs.h"
#include "gfx/gfxTarget.h"
#include "util/safeDelete.h"

#include "gfx/gl/gfxGLDevice.h"

#include "platform/event.h"

//------------------------------------------------------------------------------
// Map osx virtual keys to our own virtual keys

// Last recieved modifiers keys, used to track modifier changes. This
// is a little lame, but osx doesn't seem to deliver individual key events
// for ctrl, shift or option keys.
extern U32 _PrevModifierKeys;

static Rect convertRectIToRect(RectI in)
{
   Rect out;
   out.top    = in.point.y;
   out.left   = in.point.x;
   out.bottom = in.point.y + in.extent.y;
   out.right  = in.point.x + in.extent.x;
   return out;
}

static RectI convertRectToRectI(Rect in)
{
   RectI out;
   out.point.x = in.left;
   out.point.y = in.top;
   out.extent.x = in.right - in.left;
   out.extent.y = in.bottom - in.top;
   
   AssertFatal(out.isValidRect(), "convertRectToRectI - got invalid rect!");
   
   return out;
}

static U32 _getKeyModifiers(EventRef event)
{
   // Convert apple event modifiers into kernel event modifiers
   U32 appleModifiers;
   GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,0,
         sizeof(appleModifiers),0,&appleModifiers);
   U32 modifierKeys = 0;
   if (appleModifiers & shiftKey)
         modifierKeys |= IM_LSHIFT;
   if (appleModifiers & controlKey)
         modifierKeys |= IM_LCTRL;
   if (appleModifiers & optionKey)
         modifierKeys |= IM_LALT;
   if (appleModifiers & rightShiftKey)
         modifierKeys |= IM_RSHIFT;
   if (appleModifiers & rightControlKey)
         modifierKeys |= IM_RCTRL;
   if (appleModifiers & rightOptionKey)
         modifierKeys |= IM_RALT;
   return modifierKeys;
}

MacCarbWindow::MacCarbWindow()
{
   _windowUPP = NewEventHandlerUPP(_windowHandler);
   _mouseUPP = NewEventHandlerUPP(_mouseHandler);
   _keyUPP = NewEventHandlerUPP(_keyHandler);
   _window = NULL;
   
   mTarget = NULL;
   mDevice = NULL;
   
   mExtantId = -1;
   mExtantNext = NULL;
   
   mManager = NULL;
}

MacCarbWindow::~MacCarbWindow()
{
   DisposeEventHandlerUPP(_windowUPP);
   DisposeEventHandlerUPP(_mouseUPP);
   DisposeEventHandlerUPP(_keyUPP);

   if(_window)
      DisposeWindow(_window);
   _window = NULL;

   SAFE_DELETE(mTarget);
   
   // We have to unlink ourselves, don't forget.
   mManager->_unlinkWindow(this);
}

DeviceId MacCarbWindow::getDeviceId()
{
   return mExtantId;
}

GFXDevice *MacCarbWindow::getGFXDevice()
{
   return mDevice;
}

GFXWindowTarget *MacCarbWindow::getGFXTarget()
{
   return mTarget;
}

void MacCarbWindow::setMode(const GFXVideoMode &mode)
{
   if(mode.fullScreen != mCurrentMode.fullScreen)
   {
      // Let the manager deal with fullscreen event handling setup/teardown.
      if(mode.fullScreen)
         mManager->_setupFullscreen(this);
      else
         mManager->_teardownFullscreen();
   }
   
   ((GFXGLWindowTarget*)mTarget.getPointer())->setMode(mode.resolution, mode.fullScreen); 

   mCurrentMode = mode;
}

const GFXVideoMode &MacCarbWindow::getCurrentMode()
{
   return mCurrentMode;
}

bool MacCarbWindow::clearFullscreen()
{
   return false;
}

bool MacCarbWindow::setCaption(const char *cap)
{
   SetWindowTitleWithCFString(_window,CFStringCreateWithCString(kCFAllocatorDefault, cap, kCFStringEncodingUTF8));
   return false;
}

const char *MacCarbWindow::getCaption()
{
   CFStringRef windowTitleRef;
   CopyWindowTitleAsCFString (_window, &windowTitleRef);
   const char *ret = CFStringGetCStringPtr(windowTitleRef, kCFStringEncodingUTF8);
   
   // Memory leak here, call CFRelease on windowTitleRef.
   
   AssertFatal(ret, "MacCarbWindow::getCaption - CFStringGetCStringPtr failed, probably need to call CFStringGetCString");

   return ret;
}
   
bool MacCarbWindow::setBounds(const RectI &newBounds)
{
   Rect bounds = convertRectIToRect(newBounds);
   SetWindowBounds(_window, kWindowContentRgn, &bounds);
   return true;
}

bool MacCarbWindow::setSize(const Point2I &newSize)
{
   RectI b = getBounds();
   b.extent = newSize;
   setBounds(b);
   
   return true;
}

bool MacCarbWindow::setPosition(const Point2I newPos)
{
   RectI b = getBounds();
   b.point = newPos;
   setBounds(b);

   return true;
}

RectI MacCarbWindow::getBounds()
{
   Rect bounds;
   GetWindowBounds(_window, kWindowContentRgn, &bounds);
   return convertRectToRectI(bounds);
}

bool MacCarbWindow::isOpen()
{
   // Maybe check if _window != NULL ?
   return true;
}

bool MacCarbWindow::isVisible()
{
   return IsWindowVisible(_window);
}

bool MacCarbWindow::isFocused()
{
   return IsWindowHilited(_window);
}

//-----------------------------------------------------------------------------

// Deal with deferred events...
struct DefEvent
{
   U32 typeID;
   EventRef e;
   MacCarbWindow *w;
};

Vector<DefEvent> gDeferredQueue;

void deferEvent(U32 typeID, EventRef e, MacCarbWindow *w)
{
   DefEvent de;
   de.typeID = typeID;
   de.e = e;
   de.w = w;
   gDeferredQueue.push_back(de);
}

void processDeferredEvents()
{
   // Go from first queued event to last queued event...
   for(S32 i=0; i<gDeferredQueue.size(); i++)
   {
      DefEvent &de = gDeferredQueue[i];
      
      switch(de.typeID)
      {
         case 0:
            MacCarbWindow::_realWindowHandler(de.e, de.w);
            break;
            
         // The other callbacks, if deferred, would go here.
         case 1:
            break;
         case 2:
            break;
      }
      
      // Dispose of the event so we don't spew memory.
      ReleaseEvent(de.e);
   }
   
   // Wipe the queue so we don't process old events.
   gDeferredQueue.clear();
}

pascal OSStatus MacCarbWindow::_windowHandler(EventHandlerCallRef nextHandler,
   EventRef event,void* userData)
{
   deferEvent(0, CopyEvent(event), (MacCarbWindow*)userData);
   return eventNotHandledErr;
}

void MacCarbWindow::_realWindowHandler(EventRef event, MacCarbWindow *window)
{   
   //U32 eventClass = GetEventClass(event);
   U32 eventType = GetEventKind(event);
   //MacCarbWindow* window = (MacCarbWindow*)userData;

   switch (eventType) {
      case kEventWindowDeactivated:
      case kEventWindowActivated:
      case kEventWindowCollapsing:
      case kEventWindowShown:
         break;
      
      case kEventWindowClose:
         window->appEvent.trigger(window->getDeviceId(),WindowClose);
         break;
      
      case kEventWindowDrawContent:
         if (window->isOpen())
            window->displayEvent.trigger(window->getDeviceId());
         break;
         
      case kEventWindowBoundsChanged: 
      {
         Rect bounds;
         GetEventParameter(event,kEventParamCurrentBounds,typeQDRectangle,
            0,sizeof(bounds),0,&bounds);
            
         RectI boundsI = convertRectToRectI(bounds);

         //  How is this happening PC side..?
         if(window->mDevice && window->mTarget)
         {
            AssertFatal(dynamic_cast<GFXGLDevice*>(window->mDevice), "MacCarbWindow::_windowHandler - got a non GL GFXDevice, did Mac start supporting D3D?");
            ((GFXGLDevice*)window->mDevice)->updateBounds(boundsI, window->mTarget);
         }

         window->resizeEvent.trigger(window->getDeviceId(), boundsI.extent.x, boundsI.extent.y);
         
         break;
      }
   }
   return eventNotHandledErr;
}

pascal OSStatus MacCarbWindow::_mouseHandler(EventHandlerCallRef nextHandler,
   EventRef event,void* userData)
{
   U32 eventType = GetEventKind(event);
   MacCarbWindow* window = (MacCarbWindow*)userData;

   // Pass mouse events to the default handler first, so that it
   // can deal with the standard controls, resizing, etc.
   if (CallNextEventHandler(nextHandler,event) == noErr)
      return noErr;
   U32 modifierKeys = _getKeyModifiers(event);

   // Button events
   if (eventType == kEventMouseDown || eventType == kEventMouseUp) 
   {
      EventMouseButton mouseButton;
      GetEventParameter(event,kEventParamMouseButton,typeMouseButton,0,
            sizeof(mouseButton),0,&mouseButton);
   
      //U32 clickCount;
      //GetEventParameter(event,kEventParamClickCount,typeUInt32,0,
      //      sizeof(UInt32),0,&clickCount);
      
      window->buttonEvent.trigger(
         window->getDeviceId(),
         modifierKeys,
         (eventType == kEventMouseDown) ? IA_MAKE : IA_BREAK,
         mouseButton - 1);
   }
   
   // Mouse wheel
   else if(eventType == kEventMouseWheelMoved)
   {
      SInt32 wheelDelta;
      GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,0,
            sizeof(wheelDelta),0,&wheelDelta);
      
      window->wheelEvent.trigger(
            window->getDeviceId(),
            modifierKeys,
            wheelDelta);
   }

   // Movement events
   else if (eventType == kEventMouseMoved || eventType == kEventMouseDragged) 
   {
      Point loc;
      Rect bounds;
      
      GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,0,
         sizeof(loc),0,&loc);
      
      GetWindowBounds(window->_window,kWindowContentRgn,&bounds);
      
      window->mouseEvent.trigger(
         window->getDeviceId(),
         modifierKeys,
         loc.h - bounds.left,
         loc.v - bounds.top);
         
         S32 x = loc.h - bounds.left;
         S32 y = loc.v - bounds.top;
         
         Point2I extent = window->getBounds().extent;
         
         bool shouldHide = false;
         if(x >= 0 && y >= 0 && x <= extent.x && y <= extent.y)
            shouldHide = true;
         
         bool cursorVisible = CGCursorIsVisible();
         
         if(cursorVisible == shouldHide)
         {
            if(shouldHide)
               CGDisplayHideCursor(kCGDirectMainDisplay);
            else
               CGDisplayShowCursor(kCGDirectMainDisplay);
         }
   }
   
   return eventNotHandledErr;
}

// Table to convert modifier masks into event key codes
static struct ModifierMap {
   U32 mask,keyCode;
} _ModifierMap[] = {
   { IM_LSHIFT, KEY_LSHIFT   },
   { IM_RSHIFT, KEY_RSHIFT   },
   { IM_LALT,   KEY_LALT     },
   { IM_RALT,   KEY_RALT     },
   { IM_LCTRL,  KEY_LCONTROL },
   { IM_RCTRL,  KEY_RCONTROL },
   { IM_LOPT,   KEY_MAC_LOPT },
   { IM_ROPT,   KEY_MAC_ROPT },
};
static int _ModifierMapCount = sizeof(_ModifierMap) / sizeof(ModifierMap);

extern U8 TranslateOSKeyCode(U8 in);

pascal OSStatus MacCarbWindow::_keyHandler(EventHandlerCallRef nextHandler,
   EventRef event,void* userData)
{
   U32 eventType = GetEventKind(event);
   MacCarbWindow* window = (MacCarbWindow*)userData;
   U32 modifierKeys = _getKeyModifiers(event);

   // Changes to modifier keys are reported on a special event.
   // Modifier keys don't come through as raw key events.
   if (eventType == kEventRawKeyModifiersChanged) 
   {
      U32 changed = modifierKeys ^ _PrevModifierKeys;
      
      for (int i = 0; i < _ModifierMapCount; i++)
         if (_ModifierMap[i].mask & changed) 
         {
            window->keyEvent.trigger(
               window->getDeviceId(),
               modifierKeys,
               (_PrevModifierKeys & _ModifierMap[i].mask)?
                  IA_BREAK: IA_MAKE,
               _ModifierMap[i].keyCode);
         }
         
      _PrevModifierKeys = modifierKeys;
   }
   else if(eventType == kEventTextInputUnicodeForKeyEvent)
   {
      UniChar  *text = NULL;
      UInt32   textLen;
      U32      charCount = 0;

      // get the input string from the text input event
      // first get the number of bytes required
      GetEventParameter( event, kEventParamTextInputSendText, typeUnicodeText, NULL, 0, &textLen, NULL);
      charCount = textLen / sizeof(UniChar);
      text = new UniChar[charCount];

      // now that we've allocated space, get the buffer of text from the input method or keyboard.
      GetEventParameter( event, kEventParamTextInputSendText, typeUnicodeText, NULL, textLen, NULL, text);
      
      // Propagate each character.
      for(S32 i=0; i<charCount; i++)
         window->charEvent.trigger(window->getDeviceId(), modifierKeys, text[i]);
      
      // Don't forget to clean up memory!
      delete[] text;
   }
   else
   {
      
      U32 keyCode;
      GetEventParameter(event,kEventParamKeyCode,typeUInt32,0,
         sizeof(keyCode),0,&keyCode);
      keyCode &= 0xff;

      // Map osx keys to virtual keys..
      U32 vkey = TranslateOSKeyCode(keyCode); //_VirtualKeyRemap[keyCode];

      // This seems like a bad idea -- BJG
      if (!vkey)
         vkey = KEY_BUTTON0 + keyCode; //KeyUnmapped + keyCode;
         
      // And dispatch the event...
      window->keyEvent.trigger(
         window->getDeviceId(),
         modifierKeys,
         (eventType == kEventRawKeyDown)? IA_MAKE: IA_BREAK,
         vkey);
   }
   
   return eventNotHandledErr;
}
