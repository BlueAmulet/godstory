//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "ApplicationServices/ApplicationServices.h"

#include "gfx/gfxDevice.h"
#include "util/journal/process.h"
#include "windowManager/platformWindowMgr.h"
#include "windowManager/macCarb/macCarbWindowMgr.h"
#include "windowManager/macCarb/macCarbWindow.h"

// Last recieved modifiers keys, used to track modifier changes. This
// is a little lame, but osx doesn't seem to deliver individual key events
// for ctrl, shift or option keys.
U32 _PrevModifierKeys = 0;

static RectI convertCGRectToRectI(CGRect dispRect)
{
   RectI outRect;
   outRect.point.x = dispRect.origin.x;
   outRect.point.y = dispRect.origin.y;
   outRect.extent.x = dispRect.size.width;
   outRect.extent.y = dispRect.size.height;
   return outRect;
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

//------------------------------------------------------------------------------

MacCarbWindowManager::MacCarbWindowManager()
{
   _blocking = false;
   mExtantHead = NULL;
   mExtantIdSource = 0;

   // Default menu
   MenuRef menu;
   CreateStandardWindowMenu(0,&menu);
   InsertMenu(menu, 0);

   // Install general event handler
   EventTypeSpec eventList[] = {
      { kEventClassAppleEvent,   kAEQuitApplication },
      { kEventClassApplication,  kEventAppActivated },
      { kEventClassApplication,  kEventAppDeactivated },
      { kEventClassCommand,      kEventProcessCommand },
   };
   _handlerUPP = NewEventHandlerUPP(_eventHandler);
   InstallApplicationEventHandler(_handlerUPP,GetEventTypeCount(eventList),eventList,this,0);

   // Since we don't use the default event loop, we need to
   // install our own menu handler.
   EventTypeSpec menuList[] = {
      { kEventClassMouse,        kEventMouseDown },
   };
   _menuUPP = NewEventHandlerUPP(_menuHandler);
   InstallApplicationEventHandler(_menuUPP,GetEventTypeCount(menuList),menuList,this,0);

#if defined(TORQUE_ENABLE_UNBUNDLED_EXE)
   // Register with the window manager, and move our windows to the front.
   // This happens automatically for bundled applications; command line tools
   // are allowed to do this manually if needed.
   // This function is available on 10.3 and later, for earlier versions of
   // OSX, refer to the previous versions of this file.
   ProcessSerialNumber psn = { 0, kCurrentProcess };
   TransformProcessType(&psn, kProcessTransformToForegroundApplication);
   SetFrontProcess(&psn);
#endif

   // Hook into kernel processing.
   Process::notify(this,&MacCarbWindowManager::_processEvents);
   
   // Clear out full-screen related event handlers.
   mCurrentFullscreen = NULL;
   _fullscreenMouseHandler = NULL;
   _fullscreenKeyboardHandler = NULL;
}

MacCarbWindowManager::~MacCarbWindowManager()
{
   // Clean up all our event handlers...
   if(_menuUPP)
      DisposeEventHandlerUPP(_menuUPP);
   if(_handlerUPP)
      DisposeEventHandlerUPP(_handlerUPP);
   
   // Kill all our windows, too. They unlink themselves when they are deleted,
   // so this loop works.
   while(mExtantHead)
      delete mExtantHead;
   
   // Remove ourselves from kernel process list.
   Process::remove(this,&MacCarbWindowManager::_processEvents);
}

RectI MacCarbWindowManager::getPrimaryDesktopArea()
{
   // Get the primary desktop handle from Quartz.
   CGDirectDisplayID mainDisplay = CGMainDisplayID();

   // And get the rect.
   CGRect dispRect = CGDisplayBounds(mainDisplay);
   return convertCGRectToRectI(dispRect);
}

void MacCarbWindowManager::getMonitorRegions(Vector<RectI> &vec)
{
   // Even the matrix doesn't have 1024 monitors.
   CGDirectDisplayID displayList[1024];
   int displayCount=0;
   
   // Query the OS for this info.
   CGGetActiveDisplayList(1024, displayList, &displayCount);
   
   // And convert into the vector.
   for(S32 i=0; i<displayCount; i++)
      vec.push_back(convertCGRectToRectI(CGDisplayBounds(displayList[i])));
      
   // All done!
}

void MacCarbWindowManager::_setupFullscreen(MacCarbWindow *fsWindow)
{
   // If we have an existing FS window, we need to teardown first.
   if(mCurrentFullscreen)
      // Pass false so we don't bother bringing all the windows back - we're
      // just going to go fullscreen again!
      _teardownFullscreen(false);
      
   // Ok, note the window as FS. It'll actually acquire fullscreen after it
   // calls us; we just have to do some book-keeping and prepare the way.
   mCurrentFullscreen = fsWindow;
   
   // We may need to kill all the other windows. If we do, we'd want to do it
   // here.
   
   // Ok, now register our global event handlers.
   EventTypeSpec mouseList[] = {
      { kEventClassMouse,     kEventMouseUp },
      { kEventClassMouse,     kEventMouseDown },
      { kEventClassMouse,     kEventMouseMoved },
      { kEventClassMouse,     kEventMouseDragged },
      { kEventClassMouse,     kEventMouseWheelMoved }
   };
   EventTypeSpec keyList[] = {
      { kEventClassKeyboard,  kEventRawKeyDown },
      { kEventClassKeyboard,  kEventRawKeyUp },
      { kEventClassKeyboard,  kEventRawKeyModifiersChanged },
   };
   
   InstallApplicationEventHandler( fsWindow->_mouseUPP, GetEventTypeCount(mouseList), mouseList, fsWindow, &_fullscreenMouseHandler );
   InstallApplicationEventHandler( fsWindow->_keyUPP, GetEventTypeCount(keyList), keyList, fsWindow, &_fullscreenKeyboardHandler );
}

void MacCarbWindowManager::_teardownFullscreen(bool restoreNonFSWindows)
{
   // Uninstall our event handlers...
   RemoveEventHandler(_fullscreenMouseHandler);
   _fullscreenMouseHandler = NULL;

   RemoveEventHandler(_fullscreenKeyboardHandler);
   _fullscreenKeyboardHandler = NULL;
   
   // Clear the fullscreen pointer.
   mCurrentFullscreen = NULL;
   
   // If we had to kill the OS backing for any windows, now is when we'd want to
   // restore it... if restoreNonFSWindows is true.
   
   // Just in case, let's do this too.
   //if(restoreNonFSWindows)
   //   CGReleaseAllDisplays();
}

void MacCarbWindowManager::_unlinkWindow(MacCarbWindow *w)
{
   // Walk the extant list, and remove the window if we find it.
   MacCarbWindow **walk = &mExtantHead;
   
   while(*walk)
   {
      // Match?
      if(*walk == w)
      {
         // Unlink it.
         *walk = w->mExtantNext;
         return;
      }
      
      // Otherwise, advance to next pointer.
      walk = &((*walk)->mExtantNext);
   }
   
   // We should really never be here.
   AssertWarn(false, "MacCabWindowManager::_unlinkWindow - didn't find match!");
   return;
}

PlatformWindow *MacCarbWindowManager::createWindow(GFXDevice *device, const GFXVideoMode &mode)
{
   MacCarbWindow *window = new MacCarbWindow();
   window->mManager = this;
   
   Rect rect;
   rect.top = rect.left = 0;
   rect.bottom = mode.resolution.y;
   rect.right  = mode.resolution.x;
   
   CreateNewWindow(kDocumentWindowClass,
         kWindowStandardHandlerAttribute |
         kWindowStandardFloatingAttributes |
         kWindowInWindowMenuAttribute |
         kWindowCloseBoxAttribute |
         kWindowCollapseBoxAttribute |
         kWindowFullZoomAttribute |
         kWindowResizableAttribute |
         kWindowLiveResizeAttribute,
         &rect,&window->_window);
   
   if (!window->_window)
      return NULL;
      
   // Note ourselves on the window...
   SetWRefCon(window->_window,(long)this);

   // Event handlers
   EventTypeSpec windowList[] = {
      { kEventClassWindow,    kEventWindowCollapsing },
      { kEventClassWindow,    kEventWindowShown },
      { kEventClassWindow,    kEventWindowActivated },
      { kEventClassWindow,    kEventWindowDeactivated },
      { kEventClassWindow,    kEventWindowClose },
      { kEventClassWindow,    kEventWindowDrawContent },
      { kEventClassWindow,    kEventWindowFocusAcquired },
      { kEventClassWindow,    kEventWindowFocusRelinquish },
      { kEventClassWindow,    kEventWindowBoundsChanged },
   };
   EventTypeSpec mouseList[] = {
      { kEventClassMouse,     kEventMouseUp },
      { kEventClassMouse,     kEventMouseDown },
      { kEventClassMouse,     kEventMouseMoved },
      { kEventClassMouse,     kEventMouseDragged },
      { kEventClassMouse,     kEventMouseWheelMoved }
   };
   EventTypeSpec keyList[] = {
      { kEventClassKeyboard,  kEventRawKeyDown },
      { kEventClassKeyboard,  kEventRawKeyUp },
      { kEventClassKeyboard,  kEventRawKeyModifiersChanged },
      { kEventClassTextInput,  kEventTextInputUnicodeForKeyEvent },
   };
   InstallWindowEventHandler(window->_window,window->_windowUPP, GetEventTypeCount(windowList),windowList,window,0);
   InstallWindowEventHandler(window->_window,window->_mouseUPP,  GetEventTypeCount(mouseList),mouseList,window,0);
   InstallWindowEventHandler(window->_window,window->_keyUPP,   GetEventTypeCount(keyList),keyList,window,0);

   // Do some window setup so it looks nice.
   RGBColor black;
   dMemset(&black, 0, sizeof(RGBColor));
   SetWindowContentColor( window->_window, &black);

   // Do a default caption.
   SetWindowTitleWithCFString(window->_window,CFSTR("Grendel"));
   
   // Center the window...
   RepositionWindow(window->_window,0,kWindowCenterOnMainScreen);

   // And show it, potentially doing a fade.
   bool doFade = true;
   
   if(doFade)
   {
      TransitionWindowOptions t;
      dMemset(&t, 0, sizeof(t));
      TransitionWindowWithOptions( window->_window, kWindowFadeTransitionEffect, 
                     kWindowShowTransitionAction, NULL, true, &t);
   }
   else
   {
      ShowWindow(window->_window);
   }

   // Stuff an initial video mode in here so we don't break the device init...
   window->mCurrentMode = mode;

   // Ok, now bind to the device.
   window->mDevice = device;
   if(device)
      window->mTarget = device->allocWindowTarget(window);
   
   // Actually set the video mode now we're bound.
   window->setMode(mode);
   
   // Stick it on our extant list as well.
   window->mExtantNext = mExtantHead;
   mExtantHead = window;
   
   // Assign an ID.
   window->mExtantId = mExtantIdSource++;
   
   // And return.
   return window;
}

void MacCarbWindowManager::getExtantWindows(Vector<PlatformWindow*> &windows)
{
   // Walk the extant list and stuff them all into the supplied vector.
   MacCarbWindow *walk = mExtantHead;
   while(walk)
   {
      windows.push_back(walk);
      walk = walk->mExtantNext;
   }
}

PlatformWindow *MacCarbWindowManager::getWindowById(DeviceId id)
{
   // Walk the list and return the first window with a matching id (hopefully
   // this is also the only window with a matching id).
   MacCarbWindow *mcw = mExtantHead;
   
   while(mcw)
   {
      if(mcw->mExtantId == id)
         return mcw;
      
      mcw = mcw->mExtantNext;
   }
   
   return NULL;
}

extern void processDeferredEvents();

void MacCarbWindowManager::_processEvents()
{
   EventRef event;
   EventTargetRef target = GetEventDispatcherTarget();

   if (_blocking && !Journal::IsPlaying()) 
   {
      // Wait for and process a single event
      if (ReceiveNextEvent(0,0,kEventDurationForever,true,&event) == noErr) 
      {
         SendEventToEventTarget(event,target);
         ReleaseEvent(event);
      }
   }
   else
   {
      // Process all events in the queue, if any, without waiting.
      while (ReceiveNextEvent(0,0,0,true,&event) == noErr) 
      {
         SendEventToEventTarget(event,target);
         ReleaseEvent(event);
      }
   }

   // Handle any deferred OS events.
   processDeferredEvents();
   
   // Fire off idle events for all the windows...
   if (!Journal::IsPlaying())
   {
      MacCarbWindow *mcw = mExtantHead;
      while(mcw)
      {
         mcw->idleEvent.trigger();
         mcw = mcw->mExtantNext;
      }
   }
}

pascal OSStatus MacCarbWindowManager::_menuHandler(EventHandlerCallRef nextHandler,
      EventRef event,void* userData)
{
   // This handler is only called on mouse down events.
   ::WindowRef window;
   Point loc;
   GetEventParameter(event,kEventParamMouseLocation,
       typeQDPoint,0,sizeof(Point),0,&loc);
   if (FindWindow(loc,&window) == inMenuBar) {
      MenuSelect(loc);
      return noErr;
   }
   return eventNotHandledErr;
}

pascal OSStatus MacCarbWindowManager::_eventHandler(EventHandlerCallRef nextHandler,
      EventRef event,void* userData)
{
   U32 eventClass = GetEventClass(event);
   U32 eventType = GetEventKind(event);
   
   bool propagateEvent = false;
   
   switch (eventClass) 
   {
      case kEventClassAppleEvent: 
      {
         AEEventID type;
         GetEventParameter(event,kEventParamAEEventID,typeType,0,
            sizeof(OSType),0,&type);
            
         if (type == kAEQuitApplication)
         {
            ProcessQuit();
            propagateEvent = true;
         }
         break;
      }
      
      case kEventClassCommand: 
      {
         if (eventType == kEventProcessCommand) 
         {
            HICommand command;
            GetEventParameter(event,kEventParamDirectObject,kEventParamHICommand,
               0,sizeof(command),0,&command);
         
            if (command.commandID == kHICommandQuit)
               ProcessQuit();
         }
         break;
      }
      
      case kEventClassApplication: 
      {
         switch(eventType) 
         {
            case kEventAppActivated: 
            {
               _PrevModifierKeys = 0;
               break;
            }
            case kEventAppDeactivated:
               break;
         }
         break;
      }
   }

   // Propagate up if appropriate.
   if( nextHandler && propagateEvent )
         return CallNextEventHandler (nextHandler, event);

   return eventNotHandledErr;
}

//------------------------------------------------------------------------------

PlatformWindowManager *CreatePlatformWindowManager()
{
   return new MacCarbWindowManager();
}