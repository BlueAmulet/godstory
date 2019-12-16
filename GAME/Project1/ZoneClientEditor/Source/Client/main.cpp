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

// �ͻ�����Ϸ�������ڵ�
S32 CommonMain(S32 argc, const char **argv)
{
#ifdef NTJ_CLIENT
    //��ִ��splash�ļ�
    HANDLE hThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)__SplashWorkThread,0,0,0);
    CloseHandle(hThread);
#endif
	// �Ǽǿͻ�����Ϸʵ����ע���
	char szCurrentDir[1024];
	::GetCurrentDirectoryA(1024,szCurrentDir);
	Platform::RegGamePath(szCurrentDir);

	// ��ȡ��Ϸ�����ļ�client.ini
	g_ClientGameplayState->LoadClientIni();

	g_clientSocial.init();

   // Initialize the subsystems.
   StandardMainLoop::init();

#ifdef ENABLE_ENCRPY_PACKET
   //��ʼ�����ݰ��ķ���key
   char keyarray[] = {"sxz12345"};
   //BigNumber key;
   //key.SetBinary((uint8*)keyarray,sizeof(keyarray));
   AuthCrypt::Instance()->SetKey(0);
#endif

   // ���������в���
   if(!StandardMainLoop::handleCommandLine(argc, argv))
   {
      Platform::AlertOK("Error", "Failed to initialize game, shutting down.");

      return 1;
   }

   // <Edit> [3/4/2009 joy] ��ʼ���ͻ���
   g_ClientGameplayState->Initialize();

#ifdef NTJ_CLIENT
   //�ر�splash����
   CSplashScreenEx::Instance()->Hide();
   //��ʾ������
   Con::executef("uiShowMainWindow");
#endif

   // ��ʼ��ѭ��
   StandardMainLoop::doMainLoop();

   // Clean everything up.
   StandardMainLoop::shutdown();

   // ��Ҫ������Ϸ��
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