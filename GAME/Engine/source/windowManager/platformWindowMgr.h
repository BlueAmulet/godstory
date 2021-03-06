//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORM_PLATFORMWINDOWMGR_H_
#define _PLATFORM_PLATFORMWINDOWMGR_H_

#include "math/mRect.h"
#include "util/journal/journaledSignal.h"
#include "windowManager/platformWindow.h"


// Global macro
#define WindowManager PlatformWindowManager::get()

/// Abstract representation of a manager for native OS windows.
///
/// The PlatformWindowManager interface provides a variety of methods for querying 
/// the current desktop configuration, as well as allocating and retrieving
/// existing windows. It may also manage application-level event handling.
class PlatformWindowManager
{
public:

   /// Get Global Singleton
   static PlatformWindowManager *get();
   static void cleanup();

   virtual ~PlatformWindowManager() 
   {
   }

   static void processCmdLineArgs(const S32 argc, const char **argv);

   /// Return the extents in window coordinates of the primary desktop
   /// area. On a single monitor system this is just the display extents.
   /// On a multimon system this is the primary monitor (which PowerEngine should
   /// launch on).
   virtual RectI getPrimaryDesktopArea() = 0;

   /// Retrieve the currently set desktop bit depth
   /// @return The current desktop bit depth, or -1 if an error occurred
   virtual S32 getDesktopBitDepth() = 0;

   /// Retrieve the currently set desktop resolution
   /// @return The current desktop bit depth, or Point2I(-1,-1) if an error occurred
   virtual Point2I getDesktopResolution() = 0;

   /// Populate a vector with all monitors and their extents in window space.
   virtual void getMonitorRegions(Vector<RectI> &regions) = 0;

   /// Create a new window, appropriate for the specified device and mode.
   ///
   /// @return Pointer to the new window.
   virtual PlatformWindow *createWindow(GFXDevice *device, const GFXVideoMode &mode) = 0;

   /// Populate a list with references to all the windows created from this manager.
   virtual void getWindows(Vector<PlatformWindow*> &windows) = 0;

   /// Get the window that currently has the input focus or NULL.
   virtual PlatformWindow* getFocusedWindow() = 0;

   /// Get a window from a device ID.
   ///
   /// @return The window associated with the specified ID, or NULL if no
   ///         match was found.
   virtual PlatformWindow *getWindowById(DeviceId id)=0;

   /// Get the first window in the window list
   ///
   /// @return The first window in the list, or NULL if no windows found
   virtual PlatformWindow *getFirstWindow()=0;

   /// During full-screen toggles we want to suppress ugly transition states,
   /// which we do (on Win32) by showing and hiding a full-monitor black window.
   ///
   /// This method cues the appearance of that window ("lowering the curtain").
   virtual void lowerCurtain()=0;

   /// @see lowerCurtain
   ///
   /// This method removes the curtain window.
   virtual void raiseCurtain()=0;

private:
   /// Process command line arguments from StandardMainLoop. This is done to
   /// allow web plugin functionality, where we are passed platform-specific
   /// information allowing us to set ourselves up in the web browser,
   /// to occur in a platform-neutral way.
   virtual void _processCmdLineArgs(const S32 argc, const char **argv)=0;
};

/// Global function to allocate a new platform window manager.
///
/// This returns an instance of the appropriate window manager for the current OS.
///
/// Depending on situation (for instance, if we are a web plugin) we may
/// need to get the window manager from somewhere else.
PlatformWindowManager *CreatePlatformWindowManager();

#endif