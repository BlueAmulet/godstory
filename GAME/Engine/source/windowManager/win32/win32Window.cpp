//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <winuser.h>
#include "math/mMath.h"
#include "gfx/gfxStructs.h"

#include "windowManager/win32/win32Window.h"
#include "windowManager/win32/win32WindowMgr.h"
#include "windowManager/win32/win32CursorController.h"
#include "windowManager/win32/winDispatch.h"

#include "platform/menus/popupMenu.h"
#include "platform/platformInput.h"

// for winState structure
#include "platformWin32/platformWin32.h"

#include <d3d9types.h>
#include "gfx/gfxDevice.h"
#include "gfx/D3D9/pc/gfxPCD3D9Target.h"
//#include "gfx/D3D8/gfxD3D8Target.h"
#include "gfx/gfxInit.h"

#include <zmouse.h>
#include "Gameplay/Data/GraphicsProfile.h"

#include "T3D/gameFunctions.h"

#ifdef NTJ_CLIENT
#include "UI/dGuiMouseGamePlay.h"
#include "../VisualStudio2008/game/resource.h"
#endif
#ifdef NTJ_EDITOR
#include "UI/dWorldEditor.h"
extern bool gPreviewMission;
#endif

const UTF16* _MainWindowClassName = L"PowerEngineJuggernaughtWindow";
const UTF16* _CurtainWindowClassName = L"PowerEngineJuggernaughtCurtainWindow";
const Point2I PlatformWindow::msMinimum(800,600);

#define SCREENSAVER_QUERY_DENY 0 // Disable screensaver

#ifndef IDI_ICON1 
#define IDI_ICON1 107
#endif

static bool isScreenSaverRunning()
{
#ifndef SPI_GETSCREENSAVERRUNNING
#define SPI_GETSCREENSAVERRUNNING 114
#endif
   // Windows 2K, and higher. It might be better to hook into
   // the broadcast WM_SETTINGCHANGE message instead of polling for
   // the screen saver status.
   BOOL sreensaver = false;
   SystemParametersInfo(SPI_GETSCREENSAVERRUNNING,0,&sreensaver,0);
   return sreensaver;
}

Win32Window::Win32Window(): mMouseLockPosition(0,0),
                            mShouldLockMouse(false),
                            mMouseLocked(false),
                            mOwningManager(NULL),
                            mNextWindow(NULL),
                            mWindowHandle(NULL),
                            mTarget(NULL),
                            mDevice(NULL),
                            mAccelHandle(NULL),
                            mSuppressReset(false),
                            mMenuHandle(NULL),
                            mWindowedWindowStyle(0),
                            mPosition(0,0)
{
   mCursorController = new Win32CursorController( this );

   mVideoMode.bitDepth = 32;
   mVideoMode.fullScreen = false;
   mVideoMode.refreshRate = 60;
   mVideoMode.resolution.set(800,600);

   _registerWindowClass();
}

Win32Window::~Win32Window()
{
   if(mAccelHandle)
   {
      DestroyAcceleratorTable(mAccelHandle);
      mAccelHandle = NULL;
   }

   // delete our win handle..
   DestroyWindow(mWindowHandle);

   // unlink ourselves from the window list...
   AssertFatal(mOwningManager, "Win32Window::~Win32Window - orphan window, cannot unlink!");
   mOwningManager->unlinkWindow(this);
}

GFXDevice * Win32Window::getGFXDevice()
{
   return mDevice;
}

GFXWindowTarget * Win32Window::getGFXTarget()
{
   return mTarget;
}

const GFXVideoMode & Win32Window::getCurrentMode()
{
   return mVideoMode;
}

void Win32Window::setMode( const GFXVideoMode &mode )
{
   bool needCurtain = (mVideoMode.fullScreen != mode.fullScreen);
   
   if(needCurtain)
      Con::errorf("Win32Window::setMode - invoking curtain");

   mVideoMode = mode;
   // <Edit> [4/20/2009 joy] 窗口模式下，一些设置要和桌面一致
   if(!mVideoMode.fullScreen)
   {
	  mVideoMode.bitDepth = WindowManager->getDesktopBitDepth();
      // 窗口且最大化，分辨率必须使用桌面分辨率
      if(mVideoMode.fullWindow)
         mVideoMode.resolution = WindowManager->getDesktopResolution();
   }
   g_GraphicsProfile.SetCurrentMode(mVideoMode);

   // Set a minimum on the window size so people can't break us by resizing tiny.
   setMinimumClientSize(g_GraphicsProfile.GetCurrentMode().resolution);
   Point2I size = getMinimumClientSize();
   AdjustRect(size);
   setMinimumWindowSize(size);

   // 窗口非最大化时 保证窗口不超过工作区
   if(!mVideoMode.fullScreen && !mVideoMode.fullWindow)
   {
      // 边框（包括标题）大小
      size -= getMinimumClientSize();
      // <Edit> [4/15/2009 joy] 先改为windowSize，进行Clamp后再转为clientSize，保证整个窗口不超过工作区
      mVideoMode.resolution += size;
      ClampRect(mVideoMode.resolution, size);
      mVideoMode.resolution -= size;
   }

   mSuppressReset = true;

   // Set our window to have the right style based on the mode
   if(mode.fullScreen)
   {
      if(needCurtain)
         mOwningManager->lowerCurtain();

      SetWindowLong( getHWND(), GWL_STYLE, WS_POPUP|WS_SYSMENU );
      SetWindowPos( getHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
   }
   else
   {
      SetWindowLong( getHWND(), GWL_STYLE, mWindowedWindowStyle);
      SetWindowPos( getHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

      if(needCurtain)
         mOwningManager->lowerCurtain();

      setSize(mVideoMode.resolution);

      // We have to force Win32 to update the window frame and make the window
      // visible and no longer topmost - this code might be possible to simplify.
      SetWindowPos( getHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
      //ShowWindow( getHWND(), SW_SHOWNORMAL);
   }

   mSuppressReset = false;

   if(mTarget.isValid())
   {
		onWindowZombify();
		mTarget->resetMode();
   }

   if(needCurtain)
      mOwningManager->raiseCurtain();

   SetForegroundWindow(getHWND());
}

bool Win32Window::clearFullscreen()
{
   return true;
}

bool Win32Window::setCaption( const char *cap )
{
   return SetWindowTextA(mWindowHandle, cap);
}

const char * Win32Window::getCaption()
{
   char buff[512];
   S32 strLen = GetWindowTextA(mWindowHandle, buff, 512);

   if(strLen==0)
      return NULL;

   return StringTable->insert(buff);
}

bool Win32Window::setBounds( const RectI &newBounds )
{
   SetWindowPos( mWindowHandle, HWND_NOTOPMOST, newBounds.point.x, newBounds.point.y,
      newBounds.extent.x, newBounds.extent.y, NULL );

   return true;
}

// in: ClientRect   out: WindowRect
void Win32Window::AdjustRect(Point2I &size)
{
	RECT newRect;
	newRect.left = 0;
	newRect.top  = 0;
	newRect.bottom = newRect.top + size.y;
	newRect.right  = newRect.left + size.x;

	AdjustWindowRect( &newRect, mWindowedWindowStyle, (bool)(getMenuHandle() != NULL) );

	size.x = newRect.right - newRect.left;
	size.y = newRect.bottom - newRect.top;
}

void Win32Window::ClampRect(Point2I &size, Point2I &border)
{
	RectI workArea = WindowManager->getPrimaryDesktopArea();
	size -= border;
	workArea.extent -= border;
	workArea.extent.setMax(mMinimumClient);
	size.setMin(workArea.extent);
	size.y = getMin(mRound(workArea.extent.x / g_GraphicsProfile.GetCurrentMode().proportion), size.y);
	size.x = getMin(mRound(workArea.extent.y * g_GraphicsProfile.GetCurrentMode().proportion), size.x);
	size += border;
}

void Win32Window::onSizing(WPARAM wParam, RECT& newRect)
{
	RECT oldWindowRect,oldClientRect;
	GetWindowRect(mWindowHandle, &oldWindowRect);
	GetClientRect(mWindowHandle, &oldClientRect);

	Point2I oldWindowExtent(oldWindowRect.right - oldWindowRect.left, oldWindowRect.bottom - oldWindowRect.top);
	Point2I newWindowExtent(newRect.right - newRect.left, newRect.bottom - newRect.top);
	Point2I offset = newWindowExtent - oldWindowExtent;

	// 窗口没有改变
	if(offset.x == 0 && offset.y == 0)
		return;

	Point2I oldClientExtent(oldClientRect.right - oldClientRect.left, oldClientRect.bottom - oldClientRect.top);
	Point2I border = oldWindowExtent - oldClientExtent;
	ClampRect(newWindowExtent, border);
	offset = newWindowExtent - oldWindowExtent;

	Point2I newClientExtent = oldClientExtent + offset;

	switch (wParam)
	{
	case WMSZ_LEFT:
		{
			newClientExtent.y = mCeil(newClientExtent.x / g_GraphicsProfile.GetCurrentMode().proportion);
			newRect.top = newRect.bottom - newClientExtent.y - border.y;
			newRect.left = newRect.right - newClientExtent.x - border.x;
		}
		break;
	case WMSZ_TOP:
		{
			newClientExtent.x = mFloor(newClientExtent.y * g_GraphicsProfile.GetCurrentMode().proportion);
			newRect.top = newRect.bottom - newClientExtent.y - border.y;
			newRect.left = newRect.right - newClientExtent.x - border.x;
		}
		break;
	case WMSZ_RIGHT:
		{
			newClientExtent.y = mCeil(newClientExtent.x / g_GraphicsProfile.GetCurrentMode().proportion);
			newRect.bottom = newRect.top + newClientExtent.y + border.y;
			newRect.right = newRect.left + newClientExtent.x + border.x;
		}
		break;
	case WMSZ_BOTTOM:
		{
			newClientExtent.x = mFloor(newClientExtent.y * g_GraphicsProfile.GetCurrentMode().proportion);
			newRect.bottom = newRect.top + newClientExtent.y + border.y;
			newRect.right = newRect.left + newClientExtent.x + border.x;
		}
		break;
	case WMSZ_TOPLEFT:
		{
			S32 tmpX = mFloor(newClientExtent.y * g_GraphicsProfile.GetCurrentMode().proportion);
			newClientExtent.x = getMax(tmpX, newClientExtent.x);
			newClientExtent.y = mCeil(newClientExtent.x / g_GraphicsProfile.GetCurrentMode().proportion);
			newRect.top = newRect.bottom - newClientExtent.y - border.y;
			newRect.left = newRect.right - newClientExtent.x - border.x;
		}
		break;
	case WMSZ_TOPRIGHT:
		{
			S32 tmpX = mFloor(newClientExtent.y * g_GraphicsProfile.GetCurrentMode().proportion);
			newClientExtent.x = getMax(tmpX, newClientExtent.x);
			newClientExtent.y = mCeil(newClientExtent.x / g_GraphicsProfile.GetCurrentMode().proportion);
			newRect.top = newRect.bottom - newClientExtent.y - border.y;
			newRect.right = newRect.left + newClientExtent.x + border.x;
		}
		break;
	case WMSZ_BOTTOMRIGHT:
		{
			S32 tmpX = mFloor(newClientExtent.y * g_GraphicsProfile.GetCurrentMode().proportion);
			newClientExtent.x = getMax(tmpX, newClientExtent.x);
			newClientExtent.y = mCeil(newClientExtent.x / g_GraphicsProfile.GetCurrentMode().proportion);
			newRect.bottom = newRect.top + newClientExtent.y + border.y;
			newRect.right = newRect.left + newClientExtent.x + border.x;
		}
		break;
	case WMSZ_BOTTOMLEFT:
		{
			S32 tmpX = mFloor(newClientExtent.y * g_GraphicsProfile.GetCurrentMode().proportion);
			newClientExtent.x = getMax(tmpX, newClientExtent.x);
			newClientExtent.y = mCeil(newClientExtent.x / g_GraphicsProfile.GetCurrentMode().proportion);
			newRect.bottom = newRect.top + newClientExtent.y + border.y;
			newRect.left = newRect.right - newClientExtent.x - border.x;
		}
		break;
	}
	// 标题栏高度 - 需要更精确的方法
	S32 caption = getMax(0, border.y - border.x);
	if(newRect.top <= -caption)
	{
		newRect.bottom -= (newRect.top + caption - 1);
		newRect.top = -caption + 1;
	}
}

bool Win32Window::setSize( const Point2I &newSize )
{
   // <Edit> [4/20/2009 joy] 窗口最大化模式 游戏分辨率必须和桌面分辨率一样
   if(mVideoMode.fullWindow)
      AssertFatal(newSize == mVideoMode.resolution, "invalid resolution!");
   // Create the window rect (screen centered if not owned by a parent)
   RECT newRect;
   newRect.left = 0;
   newRect.top  = 0;
   newRect.bottom = newRect.top + newSize.y;
   newRect.right  = newRect.left + newSize.x;

   // Adjust the window rect to ensure the client rectangle is the desired resolution
   AdjustWindowRect( &newRect, mWindowedWindowStyle, (bool)(getMenuHandle() != NULL) );

   // Center the window on the screen if we're not a child
   if( !mOwningManager->mParentWindow )
   {
      //HMONITOR hMon = MonitorFromWindow(mWindowHandle, MONITOR_DEFAULTTOPRIMARY);

      //// Get the monitor's extents.
      //MONITORINFO monInfo;
      //dMemset(&monInfo, 0, sizeof MONITORINFO);
      //monInfo.cbSize = sizeof MONITORINFO;
      //GetMonitorInfo(hMon, &monInfo);

      //S32 deltaX = ((monInfo.rcMonitor.right - monInfo.rcMonitor.left) / 2) - ((newRect.right - newRect.left) / 2);
      //S32 deltaY = ((monInfo.rcMonitor.bottom - monInfo.rcMonitor.top) / 2) - ((newRect.bottom - newRect.top)/ 2);

      // <Edit> [4/15/2009 joy] 设置为工作区的中心，可能对双显示器有影响
      S32 deltaX = 0;
      S32 deltaY = 0;
	  if(!mVideoMode.fullWindow)
      {
         RectI workArea = WindowManager->getPrimaryDesktopArea();
         deltaX = ((workArea.extent.x + workArea.point.x + workArea.point.x) / 2) - ((newRect.right + newRect.left) / 2);
         deltaY = ((workArea.extent.y + workArea.point.y + workArea.point.y) / 2) - ((newRect.bottom + newRect.top) / 2);
      }

      MoveWindow( mWindowHandle, newRect.left + deltaX, newRect.top + deltaY, newRect.right - newRect.left, newRect.bottom - newRect.top, true );

   }
   else // Just position it according to the mPosition plus new extent
      MoveWindow(mWindowHandle, newRect.left, newRect.top, newRect.right - newRect.left, newRect.bottom - newRect.top, true);

   InvalidateRect( NULL, NULL, true );

   return true;
}

bool Win32Window::setPosition( const Point2I pos)
{
   mPosition = pos;
   setBounds(RectI(pos, mVideoMode.resolution));
   return true;
}

RectI Win32Window::getBounds()
{
   return RectI(mPosition, mVideoMode.resolution);
}

bool Win32Window::isOpen()
{
   return true;
}

bool Win32Window::isVisible()
{
   // Is the window open and visible, ie. not minimized?
   
   if(!mWindowHandle)
      return false;
   
   return IsWindowVisible(mWindowHandle) 
         && !IsIconic(mWindowHandle)
         && !isScreenSaverRunning();
}

bool Win32Window::isFocused()
{
   // CodeReview This is enough to make the plugin and normal/editor scenarios
   // coexist but it seems brittle. I think we need a better way to detect
   // if we're the foreground window, maybe taking into account if any of our
   // window's parents are foreground? [bjg 4/30/07]
   if(mOwningManager->mParentWindow)
      return (GetFocus() == mWindowHandle);
   else
      return (GetFocus() == mWindowHandle && GetForegroundWindow() == mWindowHandle);
}

DeviceId Win32Window::getDeviceId()
{
   return mDeviceId;
}

void Win32Window::minimize()
{
   ShowWindow( mWindowHandle, SW_MINIMIZE );
}

void Win32Window::restore()
{
   ShowWindow( mWindowHandle, SW_RESTORE );
}

void Win32Window::hide()
{
   ShowWindow( mWindowHandle, SW_HIDE );
}

void Win32Window::show()
{
   ShowWindow( mWindowHandle, SW_SHOWNORMAL );
}

void Win32Window::close()
{
   delete this;
}

void Win32Window::_registerWindowClass()
{
   // Check to see if it exists already.
   WNDCLASSEX classInfo;
   if (GetClassInfoEx(GetModuleHandle(NULL),_MainWindowClassName,&classInfo))
      return;

   HMODULE appInstance = GetModuleHandle(NULL);
   HICON   appIcon = LoadIcon(appInstance, MAKEINTRESOURCE(IDI_ICON1));

   // Window class shared by all MainWindow objects
   classInfo.lpszClassName = _MainWindowClassName;
   classInfo.cbSize        = sizeof(WNDCLASSEX);
   classInfo.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
   classInfo.lpfnWndProc   = (WNDPROC)WindowProc;
   classInfo.hInstance     = appInstance;       // Owner of this class
   classInfo.hIcon         = appIcon;           // Icon name
   classInfo.hIconSm       = appIcon;           // Icon name
   classInfo.hCursor       = LoadCursor(NULL, IDC_ARROW); // Cursor
   classInfo.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);    // Default color
   classInfo.lpszMenuName  = NULL;
   classInfo.cbClsExtra    = 0;
   classInfo.cbWndExtra    = 0;
   if (!RegisterClassEx(&classInfo))
      AssertISV(false,"Window class initialization failed");

   classInfo.lpfnWndProc = DefWindowProc;
   classInfo.hCursor = NULL;
   classInfo.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
   classInfo.lpszClassName = _CurtainWindowClassName;
   if (!RegisterClassEx(&classInfo))
      AssertISV(false,"Curtain window class initialization failed");
}

LRESULT PASCAL Win32Window::WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
   // CodeReview [tom, 4/30/2007] The two casts here seem somewhat silly and redundant ?
   Win32Window* window = (Win32Window*)((PlatformWindow*)GetWindowLong(hWnd, GWL_USERDATA));
   const DeviceId devId = window ? window->getDeviceId() : 0;

   switch (message)
   { 

   case WM_DISPLAYCHANGE:
      if(window && window->isVisible() && !window->mSuppressReset && window->getCurrentMode().bitDepth != wParam)
      {
         Con::warnf("Win32Window::WindowProc - resetting device due to display mode BPP change.");
		 onWindowZombify();
         window->getGFXTarget()->resetMode();
      }
	  if(window && !window->mVideoMode.fullScreen)
	  {
         GFXVideoMode vm = GFXInit::getInitialVideoMode();
         window->setMode(vm);
	  }
      break;

   case WM_MOUSEACTIVATE:
      SetFocus(hWnd);
      return MA_ACTIVATE;

      // Associate the window pointer with this window
   case WM_CREATE:
      // CodeReview [tom, 4/30/2007] Why don't we just cast this to a LONG 
      //            instead of having a ton of essentially pointless casts ?
      SetWindowLong(hWnd, GWL_USERDATA,
         (LONG)((PlatformWindow*)((CREATESTRUCT*)lParam)->lpCreateParams));
      break;

   case WM_SETFOCUS:
      // NOTE: if wParam is NOT equal to our window handle then we are GAINING focus
      Dispatch(DelayedDispatch, hWnd, message, wParam, lParam);
      return 0;

   case WM_KILLFOCUS:
      // NOTE: if wParam is NOT equal to our window handle then we are LOSING focus
      Dispatch(DelayedDispatch, hWnd, message, wParam, lParam);
      return 0;

      // The window is being dragged
   case WM_MOVE:
      if(!window)
         break;

      window->mPosition.x = (int)LOWORD(lParam);
      window->mPosition.y = (int)HIWORD(lParam);
      return 0;

      // Update viewport when the window moves
   case WM_SIZE:
      if(window && window->mSuppressReset)
         break;

      // This is dispatched immediately to prevent a race condition with journaling and window minimizing
      if (wParam != SIZE_MINIMIZED && !Journal::IsPlaying()) 
         Dispatch( ImmediateDispatch, hWnd,message,wParam,lParam );

      if(wParam != SIZE_MINIMIZED && window != NULL )
      {
         if(!window->mVideoMode.fullScreen)
         {
            window->mVideoMode.resolution.set(LOWORD(lParam),HIWORD(lParam));
            // <Edit> [4/15/2009 joy] 设置FOV
#ifdef NTJ_EDITOR
            dWorldEditor::SetCameraFov(window->mVideoMode.resolution);
#endif
#ifdef NTJ_CLIENT
            dGuiMouseGamePlay::SetCameraFov(window->mVideoMode.resolution);
#endif
         }

         if(window->getGFXTarget())
         {
            Con::warnf("Win32Window::WindowProc - resetting device due to window size change.");
			onWindowZombify();
            window->getGFXTarget()->resetMode();
         }
      }
      return 0;

   case WM_SIZING:
      {
         if(window && !window->mVideoMode.fullScreen)
         {
			 window->onSizing(wParam, *((RECT*)lParam));
         }
      }
      return true;
   case WM_WINDOWPOSCHANGING:
	   {
		   // <Edit> [4/15/2009 joy] 应对工作区改变的情况，或许还有更好的方法
		   if(window && window->mMinimumSize.lenSquared() > 0)
		   {
			   WINDOWPOS* pWindowPos = (WINDOWPOS*)lParam;
			   if( pWindowPos && ((SWP_NOZORDER | SWP_NOACTIVATE) == pWindowPos->flags))
			   {
				   if(window->mVideoMode.fullWindow)
				   {
					   RECT newRect;
					   newRect.left = 0;
					   newRect.top = 0;
					   newRect.bottom = WindowManager->getDesktopResolution().y + newRect.top;
					   newRect.right = WindowManager->getDesktopResolution().x + newRect.left;
					   AdjustWindowRect( &newRect, window->mWindowedWindowStyle, (bool)(window->getMenuHandle() != NULL) );
					   pWindowPos->x = newRect.left;
					   pWindowPos->y = newRect.top;
					   pWindowPos->cx = newRect.right - newRect.left;
					   pWindowPos->cy = newRect.bottom - newRect.top;
					   break;
				   }
				   RECT oldWindowRect,oldClientRect;
				   GetWindowRect(window->mWindowHandle, &oldWindowRect);
				   GetClientRect(window->mWindowHandle, &oldClientRect);

				   Point2I oldWindowExtent(oldWindowRect.right - oldWindowRect.left, oldWindowRect.bottom - oldWindowRect.top);
				   Point2I oldClientExtent(oldClientRect.right - oldClientRect.left, oldClientRect.bottom - oldClientRect.top);
				   Point2I newWindow(pWindowPos->cx, pWindowPos->cy);

				   Point2I border = oldWindowExtent - oldClientExtent;
				   window->ClampRect(newWindow, border);
				   pWindowPos->cx = newWindow.x;
				   pWindowPos->cy = newWindow.y;
			   }
		   }
	   }
	   break;
      // Limit resize to a safe minimum
   case WM_GETMINMAXINFO:
      MINMAXINFO *winfo;
      winfo = (MINMAXINFO*)(lParam);
      if(window && window->mMinimumSize.lenSquared() > 0)
      {
         winfo->ptMinTrackSize.x = window->mMinimumSize.x;
         winfo->ptMinTrackSize.y = window->mMinimumSize.y;
		 // <Edit> [4/20/2009 joy] 设置最大ClientSize为桌面大小
		 if(!window->mVideoMode.fullScreen && window->mVideoMode.fullWindow)
		 {
			 RECT newRect;
			 newRect.left = 0;
			 newRect.top = 0;
			 newRect.bottom = WindowManager->getDesktopResolution().y + newRect.top;
			 newRect.right = WindowManager->getDesktopResolution().x + newRect.left;
			 AdjustWindowRect( &newRect, window->mWindowedWindowStyle, (bool)(window->getMenuHandle() != NULL) );
			 winfo->ptMaxTrackSize.x = newRect.right - newRect.left;
			 winfo->ptMaxTrackSize.y = newRect.bottom - newRect.top;
		 }
      }
      break;

      // Override background erase so window doesn't get cleared
   case WM_ERASEBKGND:
	   if(Journal::IsDispatching())
		   break;

	   if( window)
			window->syncDisplayEvent.trigger(devId);
      return 1;

   case WM_MENUSELECT:
      winState.renderThreadBlocked = true;
      break;

      // Refresh the window
   case WM_PAINT:
      // Use validate instead of begin/end paint, which seem to installs
      // some Dx clipping state that isn't getting restored properly
      ValidateRect(hWnd,0);

      // Skip it if we're dispatching.
      if(Journal::IsDispatching())
         break;

      if( window == NULL )
         break;

      //// Default render if..
      //// 1. We have no device
      //// 2. We have a device but it's not allowing rendering
      if( !window->getGFXDevice() || !window->getGFXDevice()->allowRender() )
         window->defaultRender();
      if( winState.renderThreadBlocked )
         window->displayEvent.trigger(devId);
      break;

      // Power shutdown query
   case WM_POWERBROADCAST: {
      if (wParam == PBT_APMQUERYSUSPEND)
         if (GetForegroundWindow() == hWnd)
            return BROADCAST_QUERY_DENY;
      break;
                           }

   // Screensaver activation and monitor power requests
   case WM_SYSCOMMAND:
      switch (wParam) {
      case SC_SCREENSAVE:
      case SC_MONITORPOWER:
         if (GetForegroundWindow() == hWnd)
            return SCREENSAVER_QUERY_DENY;
         break;
         }
      break;

   // Menus
   case WM_COMMAND:
      {
         winState.renderThreadBlocked = false;

         if( window == NULL )
            break;
        
         // [tom, 8/21/2006] Pass off to the relevant PopupMenu if it's a menu
         // or accelerator command. PopupMenu will in turn hand off to script.
         //
         // Note: PopupMenu::handleSelect() will not do anything if the menu
         // item is disabled, so we don't need to deal with that here.

         S32 numItems = GetMenuItemCount(window->getMenuHandle());
         for(S32 i = 0;i < numItems;i++)
         {
            MENUITEMINFOA mi;
            mi.cbSize = sizeof(mi);
            mi.fMask = MIIM_DATA;
            if(GetMenuItemInfoA(window->getMenuHandle(), i, TRUE, &mi))
            {
               if(mi.fMask & MIIM_DATA && mi.dwItemData != 0)
               {
                  PopupMenu *mnu = (PopupMenu *)mi.dwItemData;
                  if(mnu->canHandleID(LOWORD(wParam)))
                  {
                     if ( mnu->handleSelect(LOWORD(wParam)) )
                        return 0;
                  }
               }
            }
         }
      }
      break;

   case WM_INITMENUPOPUP:
      {
         HMENU menu = (HMENU)wParam;
         MENUINFO mi;
         mi.cbSize = sizeof(mi);
         mi.fMask = MIM_MENUDATA;
         if(GetMenuInfo(menu, &mi) && mi.dwMenuData != 0)
         {
            PopupMenu *pm = (PopupMenu *)mi.dwMenuData;
            if(pm != NULL)
               pm->onMenuSelect();
         }
      }
      break;
      // Some events need to be consumed as well as queued up
      // for later dispatch.
   case WM_CLOSE:
#ifdef NTJ_EDITOR
       if ( !gPreviewMission )
#endif
           ((GFXPCD3D9WindowTarget*)(window->getGFXTarget()))->mWindow = NULL;
   case WM_MOUSEWHEEL:

      // CodeReview This fixes some issues with inappropriate event handling
      //            around device resets and in full-screen mode but feels 
      //            heavy-handed. Is it clobbering something important?
      //            [bjg 6/13/07]

      #pragma message(ENGINE(增加中文字符的输入))
	  // 增加中文字符的输入 [9/10/2008 joy]
   case WM_CHAR:
   case WM_KEYUP:
   case WM_KEYDOWN:
   case WM_SYSKEYUP:
   case WM_SYSKEYDOWN:
      Dispatch(DelayedDispatch,hWnd,message,wParam,lParam);
      return 0;
   }

   // Queue up for later and invoke the Windows default handler.
   Dispatch(DelayedDispatch,hWnd,message,wParam,lParam);
   return DefWindowProc(hWnd, message, wParam, lParam);
}


void Win32Window::defaultRender()
{
   // Get Window Device Context
   HDC logoDC = GetDC(mWindowHandle);

   // Get Window Rectangle
   RECT lRect;
   GetClientRect(mWindowHandle,&lRect);

   // Fill with AppWorkspace color
   FillRect( logoDC, &lRect, (HBRUSH)GetSysColorBrush(COLOR_APPWORKSPACE) );

   // Release Device Context
   ReleaseDC(mWindowHandle,logoDC);
}

//-----------------------------------------------------------------------------
// Accelerators
//-----------------------------------------------------------------------------

void Win32Window::addAccelerator(Accelerator &accel)
{
   ACCEL winAccel;
   winAccel.fVirt = FVIRTKEY;
   winAccel.cmd = accel.mID;

   if(accel.mDescriptor.flags & SI_SHIFT)
      winAccel.fVirt |= FSHIFT;
   if(accel.mDescriptor.flags & SI_CTRL)
      winAccel.fVirt |= FCONTROL;
   if(accel.mDescriptor.flags & SI_ALT)
      winAccel.fVirt |= FALT;

   winAccel.key = TranslateKeyCodeToOS(accel.mDescriptor.eventCode);

   for(WinAccelList::iterator i = mWinAccelList.begin();i != mWinAccelList.end();++i)
   {
      if(i->cmd == winAccel.cmd)
      {
         // Already in list, just update it
         i->fVirt = winAccel.fVirt;
         i->key = winAccel.key;
         return;
      }

      if(i->fVirt == winAccel.fVirt && i->key == winAccel.key)
      {
         // Existing accelerator in list, don't add this one
         return;
      }
   }

   mWinAccelList.push_back(winAccel);
}

void Win32Window::removeAccelerator(Accelerator &accel)
{
   for(WinAccelList::iterator i = mWinAccelList.begin();i != mWinAccelList.end();++i)
   {
      if(i->cmd == accel.mID)
      {
         mWinAccelList.erase(i);
         return;
      }
   }
}

//-----------------------------------------------------------------------------

static bool isMenuItemIDEnabled(HMENU menu, U32 id)
{
   S32 numItems = GetMenuItemCount(menu);
   for(S32 i = 0;i < numItems;i++)
   {
      MENUITEMINFOA mi;
      mi.cbSize = sizeof(mi);
      mi.fMask = MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_DATA;
      if(GetMenuItemInfoA(menu, i, TRUE, &mi))
      {
         if(mi.fMask & MIIM_ID && mi.wID == id)
         {
            // This is an item on this menu
            return (mi.fMask & MIIM_STATE) && ! (mi.fState & MFS_DISABLED);
         }

         if((mi.fMask & MIIM_SUBMENU) && mi.hSubMenu != 0 && (mi.fMask & MIIM_DATA) && mi.dwItemData != 0)
         {
            // This is a submenu, if it can handle this ID then recurse to find correct state
            PopupMenu *mnu = (PopupMenu *)mi.dwItemData;
            if(mnu->canHandleID(id))
               return isMenuItemIDEnabled(mi.hSubMenu, id);
         }
      }
   }

   return false;
}

bool Win32Window::isAccelerator(const InputEventInfo &info)
{
   U32 virt;
   virt = FVIRTKEY;
   if(info.modifier & SI_SHIFT)
      virt |= FSHIFT;
   if(info.modifier & SI_CTRL)
      virt |= FCONTROL;
   if(info.modifier & SI_ALT)
      virt |= FALT;

   U8 keyCode = TranslateKeyCodeToOS(info.objInst);

   for(S32 i = 0;i < mWinAccelList.size();++i)
   {
      const ACCEL &accel = mWinAccelList[i];
      if(accel.key == keyCode && accel.fVirt == virt && isMenuItemIDEnabled(getMenuHandle(), accel.cmd))
         return true;
   }
   return false;
}

//-----------------------------------------------------------------------------

void Win32Window::addAccelerators(AcceleratorList &list)
{
   if(mAccelHandle)
   {
      DestroyAcceleratorTable(mAccelHandle);
      mAccelHandle = NULL;
   }

   for(AcceleratorList::iterator i = list.begin();i != list.end();++i)
   {
      addAccelerator(*i);
   }

   if(mWinAccelList.size() > 0)
      mAccelHandle = CreateAcceleratorTable(&mWinAccelList[0], mWinAccelList.size());
}

void Win32Window::removeAccelerators(AcceleratorList &list)
{
   if(mAccelHandle)
   {
      DestroyAcceleratorTable(mAccelHandle);
      mAccelHandle = NULL;
   }

   for(AcceleratorList::iterator i = list.begin();i != list.end();++i)
   {
      removeAccelerator(*i);
   }
   
   if(mWinAccelList.size() > 0)
      mAccelHandle = CreateAcceleratorTable(mWinAccelList.address(), mWinAccelList.size());
}

bool Win32Window::translateMessage(MSG &msg)
{
   if(mAccelHandle == NULL || mWindowHandle == NULL || !mEnableAccelerators)
      return false;

   int ret = TranslateAccelerator(mWindowHandle, mAccelHandle, &msg);
   return ret != 0;
}

//-----------------------------------------------------------------------------
// Mouse Locking
//-----------------------------------------------------------------------------

void Win32Window::setMouseLocked( bool enable )
{
   // Maintain a good state without unnecessary 
   //  cursor hides/modifications
   if( enable && mMouseLocked )
      return;
   else if(!enable && !mMouseLocked )
      return;

   // Need to be focused to enable mouse lock
   // but we can disable it no problem if we're 
   // not focused
   if( !isFocused() && enable )
   {
      mShouldLockMouse = enable;
      return;
   }

   // Set Flag
   mMouseLocked = enable;

   if( enable )
   {
      getCursorPosition( mMouseLockPosition );

      RECT r;
      GetWindowRect(getHWND(), &r);

      // Hide the cursor before it's moved
      setCursorVisible( false );

      // We have to nudge the cursor clip rect in a bit so we don't go out
      // side the bounds of the window... We'll just do it by 32 in all
      // directions, which will break for very small windows (< 200x200 or so)
      // but otherwise won't matter.
      RECT rCopy = r;
      rCopy.top  += 32; rCopy.bottom -= 64;
      rCopy.left += 32; rCopy.right  -= 64;
      ClipCursor(&rCopy);

      S32 centerX = (r.right + r.left) >> 1;
      S32 centerY = ((r.bottom + r.top) >> 1);


      // Consume all existing mouse events and those posted to our own dispatch queue
      MSG msg;
      PeekMessage( &msg, 0,WM_MOUSEFIRST,WM_MOUSELAST , PM_QS_POSTMESSAGE | PM_NOYIELD | PM_REMOVE );
      RemoveMessages( NULL, WM_MOUSEMOVE, WM_MOUSEMOVE );

      // Set the CursorPos
      SetCursorPos(centerX, centerY);

      // reset should lock flag
      mShouldLockMouse = true;
   }
   else
   {
      // This belongs before the unlock code
      mShouldLockMouse = false;

      ClipCursor(NULL);
      setCursorPosition( mMouseLockPosition.x,mMouseLockPosition.y );

      // Consume all existing mouse events and those posted to our own dispatch queue
      MSG msg;
      PeekMessage( &msg, NULL,WM_MOUSEFIRST,WM_MOUSELAST , PM_QS_POSTMESSAGE | PM_NOYIELD | PM_REMOVE );
      RemoveMessages( NULL, WM_MOUSEMOVE, WM_MOUSEMOVE );

      // Show the Cursor
      setCursorVisible( true );

   }
}

const UTF16 *Win32Window::getWindowClassName()
{
   return _MainWindowClassName;
}

const UTF16 *Win32Window::getCurtainWindowClassName()
{
   return _CurtainWindowClassName;
}