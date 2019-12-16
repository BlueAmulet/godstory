#include "platform/platform.h"
#include "app/mainLoop.h"
#include "T3D/gameFunctions.h"
#include "gameplay/ClientGameplayState.h"
#include "unit/memoryTester.h"
#include "Gameplay/Social/ClientSocial.h"
#include <windows.h>
#include <ShellAPI.h>
#include "SplashScreenEx.h"

#include "encrypt/AuthCrypt.h"

unsigned long __SplashWorkThread(void* pThis)
{
    CSplashScreenEx::Instance()->Create(NULL,30000,CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW);
    CSplashScreenEx::Instance()->SetBitmap(TEXT("Launch/Skin/splash.bmp"));
    CSplashScreenEx::Instance()->Show();

    HWND hWnd = CSplashScreenEx::Instance()->m_hWnd;

    MSG msg;
    ZeroMemory(&msg,sizeof(msg));

    while (!CSplashScreenEx::Instance()->m_isEnd)
    {
        if (GetMessage((LPMSG) &msg, hWnd, 0, 0))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
    }

    return 0;
}

// 客户端游戏程序的入口点
S32 CommonMain(S32 argc, const char **argv)
{
#ifdef NTJ_CLIENT
    //先执行splash文件
    HANDLE hThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)__SplashWorkThread,0,0,0);
    CloseHandle(hThread);
#endif
	// 登记客户端游戏实例到注册表
	char szCurrentDir[1024];
	::GetCurrentDirectoryA(1024,szCurrentDir);
	Platform::RegGamePath(szCurrentDir);

	// 读取游戏配置文件client.ini
	g_ClientGameplayState->LoadClientIni();

	g_clientSocial.init();

   // Initialize the subsystems.
   StandardMainLoop::init();

#ifdef ENABLE_ENCRPY_PACKET
   //初始化数据包的发送key
   char keyarray[] = {"sxz12345"};
   //BigNumber key;
   //key.SetBinary((uint8*)keyarray,sizeof(keyarray));
   AuthCrypt::Instance()->SetKey(0);
#endif

   // 处理命令行参数
   if(!StandardMainLoop::handleCommandLine(argc, argv))
   {
      Platform::AlertOK("Error", "Failed to initialize game, shutting down.");

      return 1;
   }

   // <Edit> [3/4/2009 joy] 初始化客户端
   g_ClientGameplayState->Initialize();

#ifdef NTJ_CLIENT
   //关闭splash窗口
   CSplashScreenEx::Instance()->Hide();
   //显示主窗口
   Con::executef("uiShowMainWindow");
#endif

   // 开始主循环
   StandardMainLoop::doMainLoop();

   // Clean everything up.
   StandardMainLoop::shutdown();

   // 需要重启游戏吗？
   if( StandardMainLoop::requiresRestart() )
      Platform::restartInstance();

   //m.check();

   return 0;
}

void OnTimeEvent(S32 timeDelta)
{
	g_ClientGameplayState->TimeLoop(timeDelta);
}

void OnPrepRenderImage(S32 StateKey)
{
	g_ClientGameplayState->prepRenderImage(StateKey);
}