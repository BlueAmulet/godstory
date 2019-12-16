#include <Windows.h>
#include <process.h>

#include "platform/platform.h"
#include "core/bitStream.h"
#include "app/mainLoop.h"
#include "T3D/gameFunctions.h"
#include "gameplay/ServerGameplayState.h"
#include "Console/console.h"


// Entry point for your game.
//
// This is build by default using the "StandardMainLoop" toolkit. Feel free
// to bring code over directly as you need to modify or extend things. You 
// will need to merge against future changes to the SML code if you do this.
S32 CommonMain(S32 argc, const char **argv)
{
   // Some handy debugging code:
   //   if (argc == 1) {
   //      static const char* argvFake[] = { "dtest.exe", "-jload", "test.jrn" };
   //      argc = 3;
   //      argv = argvFake;
   //   }

   //   Memory::enableLogging("testMem.log");
   //   Memory::setBreakAlloc(104717);

   // Initialize the subsystems.
   StandardMainLoop::init();

   // Handle any command line args.
   if(!StandardMainLoop::handleCommandLine(argc, argv))
   {
      Platform::AlertOK("Error", "Failed to initialize game, shutting down.");

      return 1;
   }

   // <Edit> [3/4/2009 joy] 初始化zone
   g_ServerGameplayState->Initialize();
   
   // Main loop
   StandardMainLoop::doMainLoop();

   // Clean everything up.
   StandardMainLoop::shutdown();

   // Do we need to restart?
   if( StandardMainLoop::requiresRestart() )
      Platform::restartInstance();

   // Return.
   return 0;
}



void OnTimeEvent(S32 timeDelta)
{
	g_ServerGameplayState->TimeLoop(timeDelta);

    //判断是否需要退出
    if (WaitForSingleObject(g_ServerGameplayState->m_StopHandle,0) == WAIT_OBJECT_0)
    {
        Con::executef("quit");
    }
}

void OnPrepRenderImage(S32 StateKey)
{
}