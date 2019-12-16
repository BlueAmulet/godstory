//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _WINDOWMANAGER_PLATFORMWINDOW_H_
#define _WINDOWMANAGER_PLATFORMWINDOW_H_

#include "math/mRect.h"
#include "util/journal/journaledSignal.h"
#include "util/safeDelete.h"
#include "windowManager/platformCursorController.h"
#include "windowManager/windowInputGenerator.h"

//forward decl's
class PlatformWindowManager;
class GFXDevice;
struct GFXVideoMode;
class GFXWindowTarget;
class IProcessInput;

/// Abstract representation of a native OS window.
///
/// Every windowing system has its own representations and conventions as
/// regards the windows on-screen. In order to provide PowerEngine with means for
/// interfacing with multiple windows, tracking their state, etc. we provide
/// this interface.
///
/// This interface also allows the app to access the render target for the 
/// window it represents, as well as control mode switches, get mode info,
/// and so on.
///
/// @see PlatformWindowManager
class PlatformWindow
{
   friend class PlatformWindowManager;
protected:

   /// Are we enabling IME or other keyboard input translation services,
   /// or concerned about raw input?
   bool mEnableKeyboardTranslation;

   /// When PowerEngine GuiText input controls have focus they need to
   /// disable native OS keyboard accelerator translation.
   bool mEnableAccelerators;

   /// Minimum allowed size for this window. When possible, we will communicate
   /// this to the OS.
   Point2I mMinimumSize;
   // <Edit> [4/15/2009 joy] 详细设置窗口最小值
   Point2I mMinimumClient;				// 当前窗口长宽比下的窗口最小值
   const static Point2I msMinimum;		// 标准屏幕下的窗口最小值，现在为 800 * 600

   /// Is Idle?
   bool mIsBackground;

   /// Cursor Controller for this Window
   PlatformCursorController *mCursorController;

   /// Window Mouse/Key input Controller for this Window
   WindowInputGenerator *mWindowInputGenerator;

   /// Protected constructor so that the win
   PlatformWindow()
   {
      mIsBackground = false; // This could be toggled to true to prefer performance.
      mMinimumSize.set(0,0);
	  mMinimumClient.set(0,0);
      mEnableKeyboardTranslation = false;
      mEnableAccelerators = true;
      mCursorController = NULL;
      // This controller maps window input (Mouse/Keyboard) to a generic input consumer
      mWindowInputGenerator = new WindowInputGenerator( this );
   }

public:

   /// To get rid of a window, just delete it. Make sure the GFXDevice is
   /// done with it first!
   virtual ~PlatformWindow() 
   {
      SAFE_DELETE( mCursorController );
      SAFE_DELETE( mWindowInputGenerator );
   }

   /// Get the WindowController associated with this window
   virtual void setInputController( IProcessInput *controller ) { if( mWindowInputGenerator ) mWindowInputGenerator->setInputController( controller ); };

   /// Get the ID that uniquely identifies this window in the context of its
   /// window manager.
   virtual DeviceId getDeviceId() { return 0; };

   /// @name GFX State Management
   ///
   /// @{

   /// Return a pointer to the GFX device this window is bound to. A GFX
   /// device may use many windows, but a window can only be used by a
   /// single GFX device.
   virtual GFXDevice *getGFXDevice()=0;

   /// Return a pointer to this window's render target.
   ///
   /// By setting window targets from different windows, we can effect
   /// rendering to multiple windows from a single device.
   virtual GFXWindowTarget *getGFXTarget()=0;

   /// Set the video mode for this window.
   virtual void setMode(const GFXVideoMode &mode)=0;

   /// Get our current video mode - if the window has been resized, it will
   /// reflect this.
   virtual const GFXVideoMode &getCurrentMode()=0;

   /// If we're fullscreen, this function returns us to desktop mode.
   ///
   /// This will be either the last mode that we had that was not
   /// fullscreen, or the equivalent mode, windowed.
   virtual bool clearFullscreen()=0;

   /// Set Idle State (Background)
   /// 
   /// This is called to put a window into idle state, which causes it's 
   /// rendering priority to be toned down to prefer performance
   virtual void setBackground( bool val ) { mIsBackground = val; };

   /// Get Idle State (Background)
   ///
   /// This is called to poll the window as to it's idle state.  
   virtual bool getBackground() { return mIsBackground; };
   /// @}

   /// @name Caption
   ///
   /// @{

   /// Set the window's caption.
   virtual bool setCaption(const char *cap)=0;

   /// Get the window's caption.
   virtual const char *getCaption()=0;

   /// @}

   /// @name Visibility
   ///
   /// Control how the window is displayed
   /// @{

   /// Minimize the window on screen
   virtual void minimize()=0;

   /// Hide the window on screen
   virtual void hide()=0;

   /// Show the window on screen
   virtual void show()=0;

   /// Destroy the window on screen
   virtual void close()=0;

   /// Restore the window from a Maximized or Minimized state
   virtual void restore()=0;

   /// @}

   /// @name Windowed state
   ///
   /// This is only really meaningful if the window is not fullscreen.
   ///
   /// @{

   /// Resize the window to have some new bounds.
   virtual bool setBounds(const RectI &newBounds)=0;

   /// Resize the window to have a new size (but be in the same position).
   virtual bool setSize(const Point2I &newSize)=0;

   /// Move the window to a new position on screen.
   virtual bool setPosition(const Point2I)=0;

   /// Get the position and size (fullscreen windows are always at (0,0)).
   virtual RectI getBounds()=0;

   /// Returns true if the window is instantiated in the OS.
   virtual bool isOpen() = 0;

   /// Returns true if the window is visible.
   virtual bool isVisible() = 0;

   /// Returns true if the window has input focus
   virtual bool isFocused() = 0;

   /// Set if we want to process key events into appropriate character events
   /// as well.
   virtual void setKeyboardTranslation(const bool enabled)
   {
      mEnableKeyboardTranslation = enabled;
   }

   /// Returns true if keyboard translation is enabled.
   virtual bool getKeyboardTranslation() const
   {
      return mEnableKeyboardTranslation;
   }

   /// Used to disable native OS keyboard accelerators.
   virtual void setAcceleratorsEnabled(const bool enabled)
   {
      mEnableAccelerators = enabled;
   }

   /// Returns true if native OS keyboard accelerators are enabled.
   virtual bool getAcceleratorsEnabled() const
   {
      return mEnableAccelerators;
   }

   /// Sets a minimum window size. We'll work with the OS to prevent user
   /// from sizing the window to less than this. Setting to (0,0) means
   /// user has complete freedom of resize.
   virtual void setMinimumWindowSize(Point2I minSize)
   {
      mMinimumSize = minSize;
   }

   /// Returns the current minimum window size for this window.
   virtual Point2I getMinimumWindowSize()
   {
      return mMinimumSize;
   }
   /// @}
   Point2I getMinimumClientSize()
   {
      return mMinimumClient;
   }
   void setMinimumClientSize(Point2I& newClient)
   {
      F32 proportion = (F32)newClient.x / (F32)newClient.y;
      mMinimumClient.y = getMax(mRound(msMinimum.x / proportion), msMinimum.y);
      mMinimumClient.x = getMax(mRound(msMinimum.y * proportion), msMinimum.x);
   }
   static Point2I getMinimum()
   {
      return msMinimum;
   }


   /// @name Window Cursor
   ///
   /// Accessors to control a windows cursor shape and visibility
   ///
   /// @{
   /// Get the CursorController that this window owns.
   virtual PlatformCursorController *getCursorController() { return mCursorController; };

   /// Set the cursor position based on logical coordinates from the upper-right corner
   ///
   /// @param x The X position of the cursor
   /// @param y The Y position of the cursor
   virtual void setCursorPosition(S32 x, S32 y)
   {
      if( mCursorController != NULL )
         mCursorController->setCursorPosition(x,y);
   };

   /// Get the cursor position based on logical coordinates from the upper-right corner
   ///
   /// @param point A reference to a Point2I to store the coordinates
   virtual void getCursorPosition( Point2I &point )
   {
      if( mCursorController != NULL )
         mCursorController->getCursorPosition(point);
   };

   /// Set the cursor visibility on this window
   /// 
   /// @param visible Whether the cursor should be visible or not
   virtual void setCursorVisible(bool visible)
   {
      if( mCursorController != NULL )
         mCursorController->setCursorVisible(visible);
   };

   /// Get the cursor visibility on this window
   /// 
   /// @return true if the cursor is visible or false if it's hidden
   virtual bool isCursorVisible()
   {
      if( mCursorController != NULL )
         return mCursorController->isCursorVisible();
      return false;
   };

   /// Lock the mouse to this window.
   ///
   /// When this is set, the mouse will always be returned to the center
   /// of the client area after every mouse event. The mouse will also be
   /// hidden while it is locked.
   ///
   /// The mouse cannot be moved out of the bounds of the window, but the
   /// window may lose focus (for instance by an alt-tab or other event).
   /// While the window lacks focus, no mouse events will be reported.
   virtual void setMouseLocked( bool enable )=0;

   /// Is the mouse locked ?
   virtual bool isMouseLocked() const = 0;

   /// Should the mouse be locked at the next opportunity ?
   ///
   /// This flag is set to the current state of the mouse lock
   /// on a window, to specify the preferred lock status of the
   /// mouse in a platform window.
   /// 
   /// This is important for situations where a call is made 
   /// to setMouseLocked, and the window is not in a state that
   /// it can be cleanly locked. Take for example if it was called
   /// while the window is in the background, then it is not appropriate
   /// to lock the window, but rather the window should query this
   /// state at it's next opportunity and lock the mouse if requested.
   virtual bool shouldLockMouse() const = 0;

   /// @}

   virtual PlatformWindow * getNextWindow() const = 0;

   /// @name Event Handlers
   ///
   /// Various events that this window receives. These are all subclasses of
   /// JournaledSignal, so you can subscribe to them and receive notifications
   /// per the documentation for that class.
   ///
   /// @{

   ///
   AppEvent          appEvent;
   MouseEvent        mouseEvent;
   MouseWheelEvent   wheelEvent;
   ButtonEvent       buttonEvent;
   LinearEvent       linearEvent;
   KeyEvent          keyEvent;
   CharEvent         charEvent;
   DisplayEvent      displayEvent;
   SyncDisplayEvent  syncDisplayEvent;
   ResizeEvent       resizeEvent;
   IdleEvent         idleEvent;

   /// @}
};

#endif