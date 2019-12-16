//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/event.h"
#include "platformWin32/platformWin32.h"
#include "platformWin32/winConsole.h"
#include "platformWin32/winDirectInput.h"
#include "windowManager/win32/win32Window.h"
#include "console/console.h"
#include "math/mRandom.h"
#include "core/fileStream.h"
#include "T3D/resource.h"
#include <d3d9.h>
#include "gfx/gfxInit.h"
#include "gfx/gfxDevice.h"
#include "core/unicode.h"
#include "gui/core/guiCanvas.h"
#include "util/BackgroundLoadMgr.h"

extern void createFontInit();
extern void createFontShutdown();
extern void installRedBookDevices();
extern void handleRedBookCallback(U32, U32);

static MRandomLCG sgPlatRandom;
static bool sgQueueEvents;

// is keyboard input a standard (non-changing) VK keycode
#define dIsStandardVK(c) (((0x08 <= (c)) && ((c) <= 0x12)) || \
                          ((c) == 0x1b) ||                    \
                          ((0x20 <= (c)) && ((c) <= 0x2e)) || \
                          ((0x30 <= (c)) && ((c) <= 0x39)) || \
                          ((0x41 <= (c)) && ((c) <= 0x5a)) || \
                          ((0x70 <= (c)) && ((c) <= 0x7B)))

extern InputObjectInstances DIK_to_Key( U8 dikCode );

// static helper variables
static HANDLE gMutexHandle = NULL;
static bool sgDoubleByteEnabled = false;

// track window states
Win32PlatState winState;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Microsoft Layer for Unicode
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/mslu/winprog/compiling_your_application_with_the_microsoft_layer_for_unicode.asp
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef UNICODE

HMODULE LoadUnicowsProc(void)
{
    return(LoadLibraryA("unicows.dll"));
}

#ifdef _cplusplus
extern "C" {
#endif
extern FARPROC _PfnLoadUnicows = (FARPROC) &LoadUnicowsProc;
#ifdef _cplusplus
}
#endif

#endif

//--------------------------------------
Win32PlatState::Win32PlatState()
{
   log_fp      = NULL;
   hinstOpenGL = NULL;
   hinstGLU    = NULL;
   hinstOpenAL = NULL;
   appDC       = NULL;
   appInstance = NULL;
   currentTime = 0;
   processId   = 0;
}

//--------------------------------------
bool Platform::excludeOtherInstances(const char *mutexName)
{
#ifdef UNICODE
   UTF16 b[512];
   convertUTF8toUTF16((UTF8 *)mutexName, b, sizeof(b));
   gMutexHandle = CreateMutex(NULL, true, b);
#else
   gMutexHandle = CreateMutex(NULL, true, mutexName);
#endif
   if(!gMutexHandle)
      return false;

   if(GetLastError() == ERROR_ALREADY_EXISTS)
   {
      CloseHandle(gMutexHandle);
      gMutexHandle = NULL;
      return false;
   }

   return true;
}

void Platform::restartInstance()
{
   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

   TCHAR cen_buf[2048];
   GetModuleFileName( NULL, cen_buf, 2047);

   // Start the child process. 
   if( CreateProcess( cen_buf,
      NULL,            // Command line
      NULL,           // Process handle not inheritable
      NULL,           // Thread handle not inheritable
      FALSE,          // Set handle inheritance to FALSE
      0,              // No creation flags
      NULL,           // Use parent's environment block
      NULL,           // Use parent's starting directory 
      &si,            // Pointer to STARTUPINFO structure
      &pi )           // Pointer to PROCESS_INFORMATION structure
      != false )
   {
      WaitForInputIdle( pi.hProcess, 5000 );
      CloseHandle( pi.hProcess );
      CloseHandle( pi.hThread );
   }
}

///just check if the app's global mutex exists, and if so, 
///return true - otherwise, false. Should be called before ExcludeOther 
/// at very start of app execution.
bool Platform::checkOtherInstances(const char *mutexName)
{
#ifdef POWER_MULTITHREAD

	HANDLE pMutex	=	NULL;
#ifdef UNICODE
   UTF16 b[512];
   convertUTF8toUTF16((UTF8 *)mutexName, b, sizeof(b));
   pMutex  = CreateMutex(NULL, true, b);
#else
   pMutex = CreateMutex(NULL, true, mutexName);
#endif
   if(!pMutex)
      return false;

   if(GetLastError() == ERROR_ALREADY_EXISTS)
   {
	   //another mutex of the same name exists
	   //close ours
      CloseHandle(pMutex);
      pMutex = NULL;
      return true;
   }
#endif

     CloseHandle(pMutex);
     pMutex = NULL;

   //we don;t care, always false
   return false;

}

//--------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
   ShowCursor(true);
#ifdef UNICODE
   UTF16 m[1024], t[512];
   convertUTF8toUTF16((UTF8 *)windowTitle, t, sizeof(t));
   convertUTF8toUTF16((UTF8 *)message, m, sizeof(m));
   MessageBox(NULL, m, t, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OK);
#else
   MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OK);
#endif
}

//--------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
   ShowCursor(true);
#ifdef UNICODE
   UTF16 m[1024], t[512];
   convertUTF8toUTF16((UTF8 *)windowTitle, t, sizeof(t));
   convertUTF8toUTF16((UTF8 *)message, m, sizeof(m));
   return MessageBox(NULL, m, t, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OKCANCEL) == IDOK;
#else
   return MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OKCANCEL) == IDOK;
#endif
}

//--------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
   ShowCursor(true);
#ifdef UNICODE
   UTF16 m[1024], t[512];
   convertUTF8toUTF16((UTF8 *)windowTitle, t, sizeof(t));
   convertUTF8toUTF16((UTF8 *)message, m, sizeof(m));
   return (MessageBox(NULL, m, t, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_RETRYCANCEL) == IDRETRY);
#else
   return (MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_RETRYCANCEL) == IDRETRY);
#endif
}

//--------------------------------------
HIMC gIMEContext;
#ifndef POWER_LIB
#ifdef UNICODE
HWND hWin32WindowHandle;
#endif
#endif

static void InitInput()
{
#ifndef POWER_LIB
#ifdef UNICODE
   hWin32WindowHandle = getWin32WindowHandle();
   gIMEContext = ImmGetContext(hWin32WindowHandle);
   ImmReleaseContext( getWin32WindowHandle(), gIMEContext );
#endif
#endif
}

//--------------------------------------
void Platform::init()
{
   Con::printf("Initializing platform...");

   // Set the platform variable for the scripts
   Con::setVariable( "$platform", "windows" );

   WinConsole::create();

   if ( !WinConsole::isEnabled() )
      Input::init();

   InitInput();

   sgDoubleByteEnabled = GetSystemMetrics( SM_DBCSENABLED );
   sgQueueEvents = true;
   Con::printf("Done");
}

//--------------------------------------
void Platform::shutdown()
{
	sgQueueEvents = false;

   if(gMutexHandle)
      CloseHandle(gMutexHandle);

   PlatformWindowManager::cleanup();
   Input::destroy();
   
   WinConsole::destroy();
}

extern bool LinkConsoleFunctions;

extern S32 CommonMain(S32 argc, const char **argv);

//--------------------------------------
static S32 run(S32 argc, const char **argv)
{
   // Console hack to ensure consolefunctions get linked in
   LinkConsoleFunctions=true;

   createFontInit();

   S32 ret = CommonMain(argc, argv);

   createFontShutdown();

   return ret;
}


#ifndef GM_CLIENT
//--------------------------------------
S32 main(S32 argc, const char **argv)
{
   winState.appInstance = GetModuleHandle(NULL);
   return run(argc, argv);
}

#endif

//--------------------------------------

#include "unit/test.h"
#include "app/mainLoop.h"

#ifdef NTJ_CHECKMEMORY
#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include <string.h>
#include <crtdbg.h>
static _CrtMemState Xh1,Xh2,Xh_Diff;
#endif

#ifndef GM_CLIENT

S32 PASCAL WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR lpszCmdLine, S32)
{
	#ifdef NTJ_CHECKMEMORY
	//_CrtMemCheckpoint(&Xh1); //设置第一个内存检查点	
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG); 
	_CrtDumpMemoryLeaks();
	//_CrtSetBreakAlloc(253812);
	#endif
   g_MainThreadId = GetCurrentThreadId();

   Vector<char *> argv;
   char moduleName[256];
   GetModuleFileNameA(NULL, moduleName, sizeof(moduleName));
   argv.push_back(moduleName);

   for (const char* word,*ptr = lpszCmdLine; *ptr; )
   {
      // 去掉空格
      for (; dIsspace(*ptr) && *ptr; ptr++)
         ;
      
      // 收集下一个命令行参数
      for (word = ptr; !dIsspace(*ptr) && *ptr; ptr++)
         ;
      
      // 将参数添加到参数列表
      if (*word) 
      {
         int len = ptr - word;
         char *arg = (char *) dMalloc(len + 1);
         dStrncpy(arg, len + 1, word, len);
         arg[len] = 0;
         argv.push_back(arg);
      }
   }

   winState.appInstance = hInstance;

   S32 retVal = run(argv.size(), (const char **) argv.address());

   for(U32 j = 1; j < argv.size(); j++)
      dFree(argv[j]);

   //#ifdef NTJ_CHECKMEMORY
   //_CrtMemCheckpoint(&Xh2);					//设置第二个内存检查点
   //_CrtMemDifference(&Xh_Diff, &Xh1, &Xh2);	//检查变化
   //_CrtMemDumpAllObjectsSince(&Xh_Diff);	//Dump变化
   //#endif
   return retVal;
}

#endif

//--------------------------------------

F32 Platform::getRandom()
{
   return sgPlatRandom.randF();
}

S32 Platform::getRandomI(S32 i,S32 n)
{
	return sgPlatRandom.randI(i,n);
}

////--------------------------------------
/// Spawn the default Operating System web browser with a URL
/// @param webAddress URL to pass to browser
/// @return true if browser successfully spawned
bool Platform::openWebBrowser( const char* webAddress )
{
   static bool sHaveKey = false;
   static wchar_t sWebKey[512];
   char utf8WebKey[512];

   {
      HKEY regKey;
      DWORD size = sizeof( sWebKey );

      if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, dT("\\http\\shell\\open\\command"), 0, KEY_QUERY_VALUE, &regKey ) != ERROR_SUCCESS )
      {
         Con::errorf( ConsoleLogEntry::General, "Platform::openWebBrowser - Failed to open the HKCR\\http registry key!!!");
         return( false );
      }

      if ( RegQueryValueEx( regKey, dT(""), NULL, NULL, (unsigned char *)sWebKey, &size ) != ERROR_SUCCESS ) 
      {
         Con::errorf( ConsoleLogEntry::General, "Platform::openWebBrowser - Failed to query the open command registry key!!!" );
         return( false );
      }

      RegCloseKey( regKey );
      sHaveKey = true;

      convertUTF16toUTF8(sWebKey,utf8WebKey,512);

#ifdef UNICODE
      char *p = dStrstr((const char *)utf8WebKey, "%1"); 
#else
      char *p = strstr( (const char *) sWebKey  , "%1"); 
#endif
      if (p) *p = 0; 

   }

   STARTUPINFO si;
   dMemset( &si, 0, sizeof( si ) );
   si.cb = sizeof( si );

   char buf[1024];
#ifdef UNICODE
   dSprintf( buf, sizeof( buf ), "%s %s", utf8WebKey, webAddress );   
   UTF16 b[1024];
   convertUTF8toUTF16((UTF8 *)buf, b, sizeof(b));
#else
   dSprintf( buf, sizeof( buf ), "%s %s", sWebKey, webAddress );   
#endif

   //Con::errorf( ConsoleLogEntry::General, "** Web browser command = %s **", buf );

   PROCESS_INFORMATION pi;
   dMemset( &pi, 0, sizeof( pi ) );
   CreateProcess( NULL,
#ifdef UNICODE
      b,
#else
      buf, 
#endif
      NULL,
      NULL,
      false,
      CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
      NULL,
      NULL,
      &si,
      &pi );

   return( true );
}

//--------------------------------------
// 登记游戏项目安装路径到注册表
//--------------------------------------
#ifdef UNICODE
static const UTF16* PowerEngineRegKey = dT("SOFTWARE\\FairRain\\神仙传\\Setup");
#else
static const char* PowerEngineRegKey = "SOFTWARE\\FairRain\\神仙传\\Setup";
#endif

// 注册游戏运行目录路径
bool Platform::RegGamePath(const char* szPathName)
{
	HKEY regKey; 
	// 查询软件注册键是否存在
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, PowerEngineRegKey, 0, KEY_WRITE, &regKey))
	{
		// 创建软件注册键
		if (RegCreateKey(HKEY_LOCAL_MACHINE, PowerEngineRegKey, &regKey))
			return false;	
	}

	if (RegSetValueEx(regKey, dT("InstallPath"), 0, REG_SZ, (const U8*) szPathName, dStrlen(szPathName) + 1))
	{
		RegCloseKey(regKey);
		Con::warnf("错误! 客户端运行目录的注册表键值创建失败.");
		return false;
	}
	else
	{
		RegCloseKey(regKey);
		return true;
	}
}