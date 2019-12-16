//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "platformMacCarb/macCarbOGLVideo.h"
#include "platformMacCarb/macCarbConsole.h"
#include "platform/platformInput.h"
#include "platform/gameInterface.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "platformMacCarb/macCarbEvents.h"
#include "platformMacCarb/macCarbUtil.h"
#include "platform/platformThread.h"
#include "gfx/gfxInit.h"



//------------------------------------------------------------------------------
#pragma mark ---- PlatState ----
MacCarbPlatState platState;

MacCarbPlatState::MacCarbPlatState()
{
   hDisplay       = NULL;
   appWindow      = NULL;
   
   captureDisplay = true;
   fadeWindows    = true;
   isFullScreen   = false;
   
   quit           = false;

   ctx            = NULL;

   // start with something reasonable.
   desktopBitsPixel  = 16;
   desktopWidth      = 1024;
   desktopHeight     = 768;

   osVersion = 0;
   
   dStrcpy(appWindowTitle, "Mac Torque Game Engine");
   
   // Semaphore for alerts. We put the app in a modal state by blocking the main 
   // Torque thread until the RAEL thread  allows it to continue. 
   alertSemaphore = Semaphore::createSemaphore(0);
   alertDlg = NULL;
}

#pragma mark ---- Window stuff ----
//------------------------------------------------------------------------------
const U32 kTFullscreenWindowAttrs =  kWindowNoShadowAttribute | kWindowStandardHandlerAttribute;
const U32 kTDefaultWindowAttrs = kWindowStandardDocumentAttributes | kWindowStandardHandlerAttribute;
//------------------------------------------------------------------------------
static Rect _MacCarbGetCenteredRect( GDHandle hDevice, U32 width, U32 height)
{
   Rect rect;
   Rect dRect;

   // device bounds, eg: [top,left , bottom,right] = [0,0 , 768,1024]
   dRect = (**hDevice).gdRect;
   // center the window's rect in the display's rect.
   rect.top = dRect.top + (dRect.bottom - dRect.top - height) / 2;
   rect.left = dRect.left + (dRect.right - dRect.left - width) / 2;
   rect.right = rect.left + width;
   rect.bottom = rect.top + height;

   return rect;
}

//------------------------------------------------------------------------------
WindowPtr MacCarbCreateOpenGLWindow( GDHandle hDevice, U32 width, U32 height, bool fullScreen )
{
   WindowPtr w = NULL;

   Rect rect = _MacCarbGetCenteredRect(hDevice, width, height);
   
   OSStatus err;
   WindowAttributes windAttr = 0L;
   WindowClass windClass = kDocumentWindowClass;

   if (fullScreen)
   {
      windAttr = kTFullscreenWindowAttrs;
      windClass = kAltPlainWindowClass;
//    Overlay windows can be used to make transperant windows,
//    which are good for performing all kinds of cool tricks.
//      windClass = kOverlayWindowClass;
//      windAttr |= kWindowOpaqueForEventsAttribute;
   }
   else
   {
      windAttr = kTDefaultWindowAttrs;
   }
   
   err = CreateNewWindow(windClass, windAttr, &rect, &w);
   AssertISV( err == noErr && w != NULL, "Failed to create a new window.");
   
   // in windowed-fullscreen mode, we set the window's level to be
   // in front of the blanking window
   if (fullScreen)
   { 
     // create a new group if ours doesn't already exist
      if (platState.torqueWindowGroup==NULL)
         CreateWindowGroup(NULL, &platState.torqueWindowGroup);

      // place window in group
      SetWindowGroup(w, platState.torqueWindowGroup);
      
      // set window group level to one higher than blanking window.
      SetWindowGroupLevel(platState.torqueWindowGroup, kTFullscreenWindowLevel);
   }
   
   RGBColor black;
   dMemset(&black, 0, sizeof(RGBColor));
   SetWindowContentColor( w, &black);
      
   return(w);
}

//------------------------------------------------------------------------------
// Fade a window in, asynchronously.
void MacCarbFadeInWindow( WindowPtr window )
{
   if(!IsValidWindowPtr(window))
      return;
   
   // bump this to the main thread if we're not on the main thread.
   if(Thread::getCurrentThreadId() != platState.firstThreadId)
   {
      MacCarbSendTorqueEventToMain( kEventTorqueFadeInWindow, window );
      return;
   }
   
   // set state on menubar & mouse cursor. 
   if(platState.isFullScreen)
   {
      HideMenuBar();
      MacCarbSetHideCursor(true);
   }
   else
   {
      ShowMenuBar();
   }
   
   SelectWindow(window);

   if(platState.fadeWindows)
   {
      TransitionWindowOptions t;
      dMemset(&t, 0, sizeof(t));
      TransitionWindowWithOptions( window, kWindowFadeTransitionEffect, 
                     kWindowShowTransitionAction, NULL, true, &t);
   }
   else
   {
      ShowWindow(window);
   }
}

//------------------------------------------------------------------------------
// Fade a window out, asynchronously. It will be released when the transition finishes.
void MacCarbFadeAndReleaseWindow( WindowPtr window )
{
   if(!IsValidWindowPtr(window))
      return;

   if(Thread::getCurrentThreadId() != platState.firstThreadId && !platState.quit)
   {
      MacCarbSendTorqueEventToMain( kEventTorqueFadeOutWindow, window );
      return;
   }
   
   if(platState.fadeWindows)
   {
      TransitionWindowOptions t;
      dMemset(&t, 0, sizeof(t));
      TransitionWindowWithOptions( window, kWindowFadeTransitionEffect, 
                     kWindowHideTransitionAction, NULL, false, &t);
   }
   else
   {
      MacCarbSendTorqueEventToMain(kEventTorqueReleaseWindow, window);
   }
}

#pragma mark ---- Init funcs  ----
//------------------------------------------------------------------------------
void Platform::init()
{
   // Set the platform variable for the scripts
   Con::setVariable( "$platform", "macos" );

   MacConsole::create();
   //if ( !MacConsole::isEnabled() )
   Input::init();

   // allow users to specify whether to capture the display or not when going fullscreen
   Con::addVariable("pref::mac::captureDisplay", TypeBool, &platState.captureDisplay);
   Con::addVariable("pref::mac::fadeWindows", TypeBool, &platState.fadeWindows);
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
   setWindowLocked( false );
   Input::destroy();
   MacConsole::destroy();
}

//------------------------------------------------------------------------------
// Get the video settings from the prefs.
static void _MacCarbGetInitialRes(GFXVideoMode &vm, const Point2I &initialSize)
{
   Point2I windowSize = initialSize;
   if( ! windowSize.x > 0 ) windowSize.x = 640;
   if( ! windowSize.y > 0 ) windowSize.y = 480;
   
   const char* resString;
   char *tempBuf, *s;
   
   // cache the desktop size of the main screen
   GFXVideoMode desktopVm = GFXInit::getDesktopResolution();
   
   // load pref variables, properly choose windowed / fullscreen  
   vm.fullScreen = Con::getBoolVariable( "$pref::Video::fullScreen" );
   if (vm.fullScreen)
      resString = Con::getVariable( "$pref::Video::resolution" );
   else
      resString = Con::getVariable( "$pref::Video::windowedRes" );

   // dStrtok is destructive, work on a copy...
   tempBuf = new char[dStrlen( resString ) + 1];
   dStrcpy( tempBuf, resString );

   // set window size
   //DAW: Added min size checks for windowSize
   vm.resolution.x = dAtoi( dStrtok( tempBuf, " x\0" ) );
   if( vm.resolution.x < 300 ) 
      vm.resolution.x = windowSize.x;

   vm.resolution.y = dAtoi( dStrtok( NULL, " x\0") );
   if( vm.resolution.y < 400 ) 
      vm.resolution.y = windowSize.y;

   // bit depth
   if (vm.fullScreen)
   {
      s = dAtoi( dStrtok( NULL, "\0" ) );
      if( ! vm.bitDepth > 0 ) vm.bitDepth = 32;
   }
   else
      vm.bitDepth = desktopVm.bitDepth;

   delete [] tempBuf;
}


#pragma mark ---- Platform utility funcs ----
//--------------------------------------
// Web browser function:
//--------------------------------------
bool Platform::openWebBrowser( const char* webAddress )
{
   OSStatus err;
   CFURLRef url = CFURLCreateWithBytes(NULL,(UInt8*)webAddress,dStrlen(webAddress),kCFStringEncodingASCII,NULL);
   err = LSOpenCFURLRef(url,NULL);
   CFRelease(url);

   // kick out of fullscreen mode, so we can *see* the webpage!
   if(platState.isFullScreen)
      GFXDevice::toggleFullScreen();

   return(err==noErr);
}

