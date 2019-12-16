//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _WINDOWMANAGER_MACCARB_MACCARBWINDOW_MGR_H_
#define _WINDOWMANAGER_MACCARB_MACCARBWINDOW_MGR_H_

#include <Carbon/Carbon.h>
#include "windowManager/platformWindowMgr.h"

class MacCarbWindow;

class MacCarbWindowManager : public PlatformWindowManager
{
friend class MacCarbWindow;

   // Will this manager block on events?
   bool _blocking;

   // Extant list management.
   MacCarbWindow *mExtantHead;
   S32 mExtantIdSource;

   // Fullscreen event handling
   MacCarbWindow *mCurrentFullscreen;
   EventHandlerRef _fullscreenMouseHandler;
   EventHandlerRef _fullscreenKeyboardHandler;

   // Global event handling
   EventHandlerUPP _handlerUPP;
   EventHandlerUPP _menuUPP;

   static pascal OSStatus _menuHandler(EventHandlerCallRef,EventRef,void*);
   static pascal OSStatus _eventHandler(EventHandlerCallRef,EventRef,void*);
   void _processEvents();

   /// Call and pass the window that's going full screen. This deals with
   /// setting up the appropriate event handlers, and making things work
   /// properly.
   void _setupFullscreen(MacCarbWindow *fsWindow);
   
   /// When the FS window is done being FS, this cleans everything up so we
   /// can go back to the usual window/multiwindow operational pattern.
   void _teardownFullscreen(bool restoreNonFSWindows = true);
   
   void _unlinkWindow(MacCarbWindow *w);
   
public:
	MacCarbWindowManager();
	~MacCarbWindowManager();

   virtual RectI getPrimaryDesktopArea();
   virtual void getMonitorRegions(Vector<RectI> &regions);
   virtual PlatformWindow *createWindow(GFXDevice *device, const GFXVideoMode &mode);
   virtual void getExtantWindows(Vector<PlatformWindow*> &windows);
   virtual PlatformWindow *getWindowById(DeviceId id);
};

#endif