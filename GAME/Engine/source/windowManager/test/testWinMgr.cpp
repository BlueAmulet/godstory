//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "windowManager/platformWindowMgr.h"
#include "unit/test.h"
#include "core/tVector.h"
#include "gfx/gfxStructs.h"
#include "util/journal/process.h"
#include "gfx/gfxInit.h"

using namespace UnitTesting;

CreateUnitTest(TestWinMgrQueries, "WindowManager/BasicQueries")
{
   void run()
   {
      PlatformWindowManager *pwm = CreatePlatformWindowManager();

      // Check out the primary desktop area...
      RectI primary = pwm->getPrimaryDesktopArea();

      Con::printf("Primary desktop area is (%d,%d) (%d,%d)",
         primary.point.x, primary.point.y, primary.extent.x, primary.extent.y);

      test(primary.isValidRect(), "Got some sort of invalid rect from the window manager!");

      // Now try to get info about all the monitors.
      Vector<RectI> monitorRects;
      pwm->getMonitorRegions(monitorRects);

      test(monitorRects.size() > 0, "Should get at least one monitor rect back from getMonitorRegions!");

      // This test is here just to detect overflow/runaway situations. -- BJG
      test(monitorRects.size() < 64, "Either something's wrong, or you have a lot of monitors...");

      for(S32 i=0; i<monitorRects.size(); i++)
      {
         Con::printf(" Monitor #%d region is (%d,%d) (%d,%d)", i,
            monitorRects[i].point.x, monitorRects[i].point.y, monitorRects[i].extent.x, monitorRects[i].extent.y);

         test(monitorRects[i].isValidRect(), "Got an invalid rect for this monitor - no good.");
      }
   }
};

CreateInteractiveTest(TestWinMgrCreate, "WindowManager/CreateAWindow")
{
   void handleMouseEvent(DeviceId,U32,S32 x,S32 y, bool isRelative)
   {
      Con::printf("Mouse at %d, %d %s", x, y, isRelative ? "(relative)" : "(absolute)");
   }

   void handleAppEvent(DeviceId, U16 event)
   {
      if(event == WindowClose)
         Process::requestShutdown();
   }

   void run()
   {
      PlatformWindowManager *pwm = CreatePlatformWindowManager();

      GFXVideoMode vm;
      vm.resolution.x = 800;
      vm.resolution.y = 600;

      PlatformWindow *pw = pwm->createWindow(NULL, vm);

      test(pw, "Didn't get a window back from the window manager, no good.");
      if(!pw)
         return;

      // Setup our events.
      pw->mouseEvent.notify(this, &TestWinMgrCreate::handleMouseEvent);
      pw->appEvent.notify(this, &TestWinMgrCreate::handleAppEvent);

      // And, go on our way.
      while(Process::processEvents())
         ;

      SAFE_DELETE(pw);
   }
};

CreateInteractiveTest(TestWinMgrGFXInit, "WindowManager/SimpleGFX")
{
   PlatformWindow *mWindow;
   GFXDevice *mDevice;

   void handleDrawEvent(DeviceId id)
   {
      mDevice->beginScene();
      mDevice->setActiveRenderTarget(mWindow->getGFXTarget());
      mDevice->clear( GFXClearZBuffer | GFXClearStencil | GFXClearTarget, ColorI( 255, 255, 0 ), 1.0f, 0 );
      mDevice->endScene();
      mWindow->getGFXTarget()->present();
   }

   void forceDraw()
   {
      handleDrawEvent(0);
   }

   void handleAppEvent(DeviceId, U16 event)
   {
      if(event == WindowClose)
         Process::requestShutdown();
   }

   void run()
   {
      PlatformWindowManager *pwm = CreatePlatformWindowManager();

      // Create a device.
      GFXAdapter a;
      a.mType = Direct3D9;
      a.mIndex = 0;

      mDevice = GFXInit::createDevice(&a);
      test(mDevice, "Unable to create d3d9 device #0.");

      // Initialize the window...
      GFXVideoMode vm;
      vm.resolution.x = 400;
      vm.resolution.y = 400;

      mWindow = pwm->createWindow(mDevice, vm);

      test(mWindow, "Didn't get a window back from the window manager, no good.");
      if(!mWindow)
         return;

      // Setup our events.
      mWindow->displayEvent.notify(this, &TestWinMgrGFXInit::handleDrawEvent);
      mWindow->idleEvent.notify(this, &TestWinMgrGFXInit::forceDraw);
      mWindow->appEvent.notify(this, &TestWinMgrGFXInit::handleAppEvent);

      // And, go on our way.
      while(Process::processEvents())
         ;

      mWindow->displayEvent.remove(this, &TestWinMgrGFXInit::handleDrawEvent);
      mWindow->idleEvent.remove(this, &TestWinMgrGFXInit::forceDraw);
      mWindow->appEvent.remove(this, &TestWinMgrGFXInit::handleAppEvent);

      // Clean up!
      SAFE_DELETE(mDevice);
      SAFE_DELETE(mWindow);
   }
};

CreateInteractiveTest(TestWinMgrGFXInitMultiWindow, "WindowManager/GFXMultiWindow")
{
   enum {
      NumWindows = 4,
   };

   PlatformWindowManager *mWindowManager;
   PlatformWindow *mWindows[NumWindows];
   GFXDevice *mDevice;

   void handleDrawEvent(DeviceId id)
   {
      // Which window are we getting this event on?
      PlatformWindow *w = mWindowManager->getWindowById(id);

      mDevice->beginScene();
      mDevice->setActiveRenderTarget(w->getGFXTarget());

      // Vary clear color by window to discern which window is which.
      mDevice->clear( GFXClearTarget, 
         ColorI( 255 - (id * 50), 255, id  * 100 ), 1.0f, 0 );
      mDevice->endScene();

      // Call swap on the window's render target.
      ((GFXWindowTarget*)w->getGFXTarget())->present();
   }

   void handleAppEvent(DeviceId, U16 event)
   {
      if(event == WindowClose)
         Process::requestShutdown();
   }
   
   void handleIdleEvent()
   {
      for(S32 i=0; i<NumWindows; i++)
         handleDrawEvent(mWindows[i]->getDeviceId());
   }

   void run()
   {
      mWindowManager = CreatePlatformWindowManager();

      // Create a device.
      GFXAdapter a;
      a.mType = Direct3D9;
      a.mIndex = 0;

      mDevice = GFXInit::createDevice(&a);
      test(mDevice, "Unable to create d3d9 device #0.");

      // Initialize the windows...
      GFXVideoMode vm;
      vm.resolution.x = 400;
      vm.resolution.y = 400;

      for(S32 i=0; i<NumWindows; i++)
      {
         mWindows[i] = mWindowManager->createWindow(mDevice, vm);

         test(mWindows[i], "Didn't get a window back from the window manager, no good.");
         if(!mWindows[i])
            continue;

         // Setup our events.
         mWindows[i]->displayEvent.notify(this, &TestWinMgrGFXInitMultiWindow::handleDrawEvent);
         mWindows[i]->appEvent.notify(this, &TestWinMgrGFXInitMultiWindow::handleAppEvent);
         mWindows[i]->idleEvent.notify(this, &TestWinMgrGFXInitMultiWindow::handleIdleEvent);
      }

      // And, go on our way.
      while(Process::processEvents())
         ;

      SAFE_DELETE(mWindowManager);
      SAFE_DELETE(mDevice);
   }
};

CreateInteractiveTest(TestJournaledMultiWindowGFX, "WindowManager/GFXJournaledMultiWindow")
{
   enum {
      NumWindows = 2,
   };

   PlatformWindowManager *mWindowManager;
   PlatformWindow *mWindows[NumWindows];
   GFXDevice *mDevice;

   S32 mNumDraws;
   S32 mNumResize;

   void drawToWindow(PlatformWindow *win)
   {
      // Do some simple checks to make sure we draw the same number of times
      // on both runs.
      if(Journal::IsPlaying())
         mNumDraws--;
      else
         mNumDraws++;

      // Render!
      mDevice->beginScene();
      mDevice->setActiveRenderTarget(win->getGFXTarget());

      // Vary clear color by window to discern which window is which.
      static S32 timeVariance = 0;

      mDevice->clear( GFXClearTarget, 
         ColorI( 0xFF - (++timeVariance * 5), 0xFF, win->getDeviceId() * 0x0F ), 1.0f, 0 );

      mDevice->endScene();

      // Call swap on the window's render target.
      win->getGFXTarget()->present();

   }

   void handleDrawEvent(DeviceId id)
   {
      // Which window are we getting this event on?
      PlatformWindow *w = mWindowManager->getWindowById(id);
      
      drawToWindow(w);
   }

   void handleAppEvent(DeviceId, U16 event)
   {
      if(event == WindowClose)
         Process::requestShutdown();
   }

   void handleIdleEvent()
   {
      for(S32 i=0; i<NumWindows; i++)
         drawToWindow(mWindows[i]);
   }

   void handleResizeEvent(DeviceId id, S32 width, S32 height)
   {
      // Do some simple checks to make sure we resize the same number of times
      // on both runs.

      if(Journal::IsPlaying())
      {
         // If we're playing back, APPLY the resize event...
         mWindowManager->getWindowById(id)->setSize(Point2I(width, height));

         mNumResize--;
      }
      else
      {
         // If we're not playing back, do nothing except note it.
         mNumResize++;
      }

      // Which window are we getting this event on?
      PlatformWindow *w = mWindowManager->getWindowById(id);

      drawToWindow(w);
   }

   /// The mainloop of our app - we'll run this twice, once to create
   /// a journal and again to play it back.
   void mainLoop()
   {
      mWindowManager = CreatePlatformWindowManager();

      // Create a device.
      GFXAdapter a;
      a.mType = Direct3D9;
      a.mIndex = 0;

      mDevice = GFXInit::createDevice(&a);
      test(mDevice, "Unable to create ogl device #0.");

      // Initialize the windows...
      GFXVideoMode vm;
      vm.resolution.x = 400;
      vm.resolution.y = 400;

      for(S32 i=0; i<NumWindows; i++)
      {
         mWindows[i] = mWindowManager->createWindow(mDevice, vm);

         test(mWindows[i], "Didn't get a window back from the window manager, no good.");
         if(!mWindows[i])
            continue;

         // Setup our events.
         mWindows[i]->displayEvent.notify(this, &TestJournaledMultiWindowGFX::handleDrawEvent);
         mWindows[i]->appEvent.notify(this, &TestJournaledMultiWindowGFX::handleAppEvent);
         mWindows[i]->resizeEvent.notify(this, &TestJournaledMultiWindowGFX::handleResizeEvent);

         // Only subscribe to the first idle event.
         if(i==0)
            mWindows[i]->idleEvent.notify(this, &TestJournaledMultiWindowGFX::handleIdleEvent);
      }

      // And, go on our way.
      while(Process::processEvents())
         ;

      // Finally, clean up.
      for(S32 i=0; i<NumWindows; i++)
         SAFE_DELETE(mWindows[i]);

      SAFE_DELETE(mDevice);
      SAFE_DELETE(mWindowManager);
   }


   void run()
   {
      return;

// CodeReview: this should be deleted or enabled.
#if 0
      mNumDraws = 0;
      mNumResize = 0;

      // Record a run of the main loop.
      Journal::Record("multiwindow.jrn");
      mainLoop();
      Journal::Stop();

      test(mNumDraws > 0, "No draws occurred!");
      test(mNumResize > 0, "No resizes occurred!");

      // And play it back.
      Journal::Play("multiwindow.jrn");
      mainLoop();
      Journal::Stop();

      test(mNumDraws == 0, "Failed to play journal back with same number of draws.");
      test(mNumResize == 0, "Failed to play journal back with same number of resizes.");
#endif
   }
};

CreateInteractiveTest(GFXTestFullscreenToggle, "GFX/TestFullscreenToggle")
{
   enum Constants
   {
      NumWindows = 1,
   };

   PlatformWindowManager *mWindowManager;
   PlatformWindow *mWindows[NumWindows];
   GFXDevice *mDevice;

   void drawToWindow(PlatformWindow *win)
   {
      // Render!
      mDevice->beginScene();
      mDevice->setActiveRenderTarget(win->getGFXTarget());

      // Vary clear color by window to discern which window is which.
      static S32 timeVariance = 0;

      mDevice->clear( GFXClearZBuffer | GFXClearStencil | GFXClearTarget, 
         ColorI( 0xFF - (++timeVariance * 5), 0xFF, win->getDeviceId() * 0x40 ), 1.0f, 0 );

      mDevice->endScene();

      // Call swap on the window's render target.
      win->getGFXTarget()->present();
   }

   void handleDrawEvent(DeviceId id)
   {
      // Which window are we getting this event on?
      PlatformWindow *w = mWindowManager->getWindowById(id);

      drawToWindow(w);
   }

   void handleAppEvent(DeviceId, U16 event)
   {
      if(event == WindowClose)
         Process::requestShutdown();
   }

   void handleIdleEvent()
   {
      // Redraw everything.
      for(S32 i=0; i<NumWindows; i++)
         drawToWindow(mWindows[i]);

      // Don't monopolize the CPU.
      Platform::sleep(10);
   }

   void handleButtonEvent(DeviceId did,U32 modifier,U32 action,U16 button)
   {
      // Only respond to button down
      if(action != IA_MAKE)
         return;

      // Get the window...
      PlatformWindow *win = mWindowManager->getWindowById(did);
      GFXVideoMode winVm = win->getCurrentMode();

      // If the window is not full screen, make it full screen 800x600x32
      if(winVm.fullScreen == false)
      {
         winVm.fullScreen = true;
         winVm.resolution.set(800,600);
      } 
      else
      {
         // If the window is full screen, then bump it to 400x400x32
         winVm.fullScreen = false;
         winVm.resolution.set(400,400);
      }

      win->setMode(winVm);
   }

   void run()
   {
      mWindowManager = CreatePlatformWindowManager();

      // Create a device.
      GFXAdapter a;
      a.mType = Direct3D9;
      a.mIndex = 0;

      mDevice = GFXInit::createDevice(&a);
      test(mDevice, "Unable to create d3d9 device #0.");

      // Initialize the windows...
      GFXVideoMode vm;
      vm.resolution.x = 400;
      vm.resolution.y = 400;

      for(S32 i=0; i<NumWindows; i++)
      {
         mWindows[i] = mWindowManager->createWindow(mDevice, vm);

         test(mWindows[i], "Didn't get a window back from the window manager, no good.");
         if(!mWindows[i])
            continue;

         // Setup our events.
         mWindows[i]->appEvent.notify(this, &GFXTestFullscreenToggle::handleAppEvent);
         mWindows[i]->buttonEvent.notify(this, &GFXTestFullscreenToggle::handleButtonEvent);
         mWindows[i]->displayEvent.notify(this, &GFXTestFullscreenToggle::handleDrawEvent);

         // Only subscribe to the first idle event.
         if(i==0)
            mWindows[i]->idleEvent.notify(this, &GFXTestFullscreenToggle::handleIdleEvent);
      }

      // And, go on our way.
      while(Process::processEvents())
         ;

      // Finally, clean up.
      for(S32 i=0; i<NumWindows; i++)
         SAFE_DELETE(mWindows[i]);

      mDevice->preDestroy();
      SAFE_DELETE(mDevice);
      SAFE_DELETE(mWindowManager);
   }
};