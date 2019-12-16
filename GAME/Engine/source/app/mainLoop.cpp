//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "app/mainLoop.h"
#include "app/badWordFilter.h"
#include "app/game.h"

#include "platform/platformTimer.h"
#include "platform/platformRedBook.h"
#include "platform/nativeDialogs/fileDialog.h"

#include "core/threadStatic.h"
#include "core/resManager.h"

#include "windowManager/platformWindowMgr.h"

#include "util/journal/process.h"
#include "util/fpsTracker.h"

#include "console/telnetConsole.h"
#include "console/telnetDebugger.h"
#include "console/debugOutputConsumer.h"

#include "gfx/gBitmap.h"
#include "gfx/gFont.h"
#include "gfx/debugDraw.h"

#include "sim/netStringTable.h"
#include "sim/actionMap.h"
#include "sim/netInterface.h"

#include "sfx/sfxSystem.h"
#include "sfx/sfxWavResource.h"
#include "sfx/sfxMP3Resource.h"
//#ifndef POWER_NO_OGGVORBIS
//#include "sfx/vorbis/sfxOggResource.h"
//#endif
#include "T3D/fx/particleEmitter.h"
#include "Gameplay/Data/SoundResource.h"

#include "util/sampler.h"
#include "T3D/trigger.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "T3D/fx/fxFoliageReplicator.h"

#include "util/BackgroundLoadMgr.h"
#include "T3D/gameFunctions.h"

#ifdef POWER_ENABLE_VFS
#include "platform/platformVFS.h"
#endif

#include "sceneGraph/lightManager.h"
#include "T3D/fx/cameraFXMgr.h"

#ifdef NTJ_EDITOR
#include "Gameplay/EditorGameplayState.h"
#endif
#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Util/LocalString.h"
#endif
#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/item/NpcShopData.h"
#endif
#include "shaderGen/manager/shaderGenManager.h"

DITTS( F32, gTimeScale, 1.0 );
DITTS( U32, gTimeAdvance, 0 );
DITTS( U32, gFrameSkip, 0 );
DITTS( U32, gFrameCount, 0 );

extern S32 sgBackgroundProcessSleepTime;
extern S32 sgTimeManagerProcessInterval;

extern void netInit();
extern FPSTracker gFPS;

static bool gRequiresRestart = false;

void StandardMainLoop::init()
{
   // 断言功能创建
   PlatformAssert::create();

   // 初始化网络
   if(!Net::init())
   {
      AssertISV(false, "初始化网络失败!(StandardMainLoop::init())");
   }

   FrameAllocator::init(POWER_FRAME_SIZE);
   _StringTable::create();

   ResManager::create();
   ResourceManager->registerExtension(".png", constructBitmapPNG);
   ResourceManager->registerExtension(".jng", constructBitmapMNG);
   //ResourceManager->registerExtension(".fnt", constructNewFont);
   ResourceManager->registerExtension(".jpg", constructBitmapJPEG);
   ResourceManager->registerExtension(".bmp", constructBitmapBMP);
   ResourceManager->registerExtension(".dds", constructBitmapDDS);
   ResourceManager->registerExtension(".wav", SFXWavResource::create);
   ResourceManager->registerExtension(".mp3", SFXMP3Resource::create);

   Con::init();
   Platform::initConsole();
   NetStringTable::create();

   TelnetConsole::create();
   TelnetDebugger::create();  

   Processor::init();
   Math::init();
   Platform::init();
   Platform::initConsole();

#ifndef GM_CLIENT
   SFXSystem::init();
   GFXDevice::create();
   GFXTextureManager::init();
   SoundManager::creat();
   triggerDataMannager::creatTriggerDataManager();
#endif 

   Con::addVariable("timeScale", TypeF32, &ATTS(gTimeScale));
   Con::addVariable("timeAdvance", TypeS32, &ATTS(gTimeAdvance));
   Con::addVariable("frameSkip", TypeS32, &ATTS(gFrameSkip));

   Con::setVariable( "defaultGame", StringTable->insert("gameres") );

   netInit();
   Sim::init();

   ActionMap* globalMap = new ActionMap;
   globalMap->registerObject("GlobalActionMap");
   Sim::getActiveActionMapSet()->pushObject(globalMap);

   BadWordFilter::create();
   
#ifndef GM_CLIENT
   Process::init();
   Sampler::init();
#endif

#ifdef NTJ_CLIENT
   // 加载本地化字符串数据
   g_LocalString->LoadFile(GAME_SYSSTRING_INI);
   // 加载物品描述字符串数据
   g_LocalString->LoadFile(GAME_ITEMDESC_INI);
   InitScriptVariable();
#endif
}

void StandardMainLoop::shutdown()
{
   preShutdown();

   //getUserDataBlockGroup中的对象没有注册过
   clearUserDataBlockGroup();

   // <Edit> [3/11/2009 joy] shutdown需要放在preShutdown之后
  
   BadWordFilter::destroy();

   // Note: tho the SceneGraphs are created after the Manager, delete them after, rather
   //  than before to make sure that all the objects are removed from the graph.
   Sim::shutdown();

   // Shut down SFX before SIM so that it clears out any audio handles
   SFXSystem::destroy();

   Process::shutdown();

   // Purge any resources on the timeout list...
   if (ResourceManager)
      ResourceManager->purge();

   //RedBook::destroy();

#ifndef NTJ_SERVER
   ParticleEmitter::shutdown();
   RibbonEmitter::shutdown();
   CommonFontEX::shutdown();
   CommonFontManager::DeleteAll();
   VolumeFog::shutdown();
   CameraShakeEX::shutdown();
#endif

#ifdef NTJ_EDITOR
   g_EditorGameplayState->Shutdown();
#endif
#ifdef NTJ_CLIENT
   g_ClientGameplayState->Shutdown();
#endif
#ifdef NTJ_SERVER
   g_ServerGameplayState->Shutdown();
#endif

   Platform::shutdown();

   TelnetDebugger::destroy();
   TelnetConsole::destroy();

   NetStringTable::destroy();
   Con::shutdown();

   SoundManager::destroy();
   triggerDataMannager::destroyTriggerDataManager();

   ResManager::destroy();
   GFXDevice::destroy();

   AvailableLightManagers::destroyManagers();

   _StringTable::destroy();
   FrameAllocator::destroy();
   Net::shutdown();
   Sampler::destroy();

   // asserts should be destroyed LAST
   PlatformAssert::destroy();
   g_TestHulingyun.Print();
}

void StandardMainLoop::preShutdown()
{
#ifdef POWER_TOOLS
   // Tools are given a chance to do pre-quit processing
   // - This is because for tools we like to do things such
   //   as prompting to save changes before shutting down
   //   and onExit is packaged which means we can't be sure
   //   where in the shutdown namespace chain we are when using
   //   onExit since some components of the tools may already be
   //   destroyed that may be vital to saving changes to avoid
   //   loss of work [1/5/2007 justind]
   if( Con::isFunction("onPreExit") )
      Con::executef( "onPreExit");
#endif

   //exec the script onExit() function
   Con::executef("onExit");
#ifdef NTJ_SERVER
   g_ServerGameplayState->preShutdown();
#endif
#ifdef NTJ_CLIENT
   g_ClientGameplayState->preShutdown();
#endif
#ifdef NTJ_EDITOR
	g_EditorGameplayState->preShutdown();
#endif

	{
#pragma message(ENGINE(释放全局变量的内存))
		for (U32 i=0; i<ShaderGenManager::_mTrackedShaders.size(); i++)
		{
			ShaderGenManager::_mTrackedShaders[i].~TrackedAutoGenShader();
		}
//#pragma message(ENGINE(程序退出时删除程序生成的shader文件))
//		const char filename[] = "*.*";
//		char dir[MAX_PATH];
//		dMemset(dir, 0, sizeof(dir));
//		const char *shaderPath = Con::getVariable("$pref::Video::shaderPath");
//		if (dStrlen(shaderPath) == 0)
//		{
//			shaderPath = "temp";
//		}
//		dSprintf(dir, sizeof(dir), "%s/%s", Platform::getCurrentDirectory(), shaderPath);
//		Platform::deleteDirFiles(dir, filename);
	}
}

bool StandardMainLoop::handleCommandLine( S32 argc, const char **argv )
{
   // Allow the window manager to process command line inputs; this is
   // done to let web plugin functionality happen in a fairly transparent way.
   PlatformWindowManager::get()->processCmdLineArgs(argc, argv);

   Process::handleCommandLine( argc, argv );

   // Set up the command line args for the console scripts...
   Con::setIntVariable("Game::argc", argc);
   U32 i;
   for (i = 0; i < argc; i++)
      Con::setVariable(avar("Game::argv%d", i), argv[i]);

   // Set our working directory.
   Platform::setCurrentDirectory( Platform::getMainDotCsDir() );

#ifdef POWER_PLAYER
   if(argc > 2 && dStricmp(argv[1], "-project") == 0)
   {
      char playerPath[1024];
      Platform::makeFullPathName(argv[2], playerPath, sizeof(playerPath));
      Platform::setCurrentDirectory(playerPath);

      argv += 2;
      argc -= 2;
   }

#endif

   ResourceManager->setWriteablePath(Platform::getCurrentDirectory());
   ResourceManager->addPath( Platform::getCurrentDirectory() );

   Stream *mainCsStream = NULL;

   // The working filestream.
   FileStream str; 

#ifdef NTJ_EDITOR
   const char *defaultScriptName = "main.cs";
#else
   const char *defaultScriptName = "gameres/init.cs";
#endif

   bool useDefaultScript = true;

   // Check if any command-line parameters were passed (the first is just the app name).
   if (argc > 1)
   {
      // If so, check if the first parameter is a file to open.
      if ( (str.open(argv[1], FileStream::Read)) && (argv[1][0] != 0) )
      {
         // If it opens, we assume it is the script to run.
         useDefaultScript = false;
         mainCsStream = &str;
      }
   }

#ifndef NTJ_EDITOR
   Platform::setMainDotCsDir(Platform::getCurrentDirectory());
   Platform::setCurrentDirectory(Platform::getCurrentDirectory());
   Con::executef("exec", defaultScriptName);
#else	//NTJ_EDITOR
   if (useDefaultScript)
   {
      bool success = false;
      success = str.open(defaultScriptName, FileStream::Read);

#if defined( POWER_DEBUG )
      if (!success)
      {
         OpenFileDialog ofd;
         FileDialogData &fdd = ofd.getData();
         fdd.mFilters = StringTable->insert("Main Entry Script (main.cs)|main.cs|");
         fdd.mTitle   = StringTable->insert("Locate Game Entry Script");

         // Get the user's selection
         if( !ofd.Execute() )
            return false;

         // Process and update CWD so we can run the selected main.cs
         S32 pathLen = dStrlen( fdd.mFile );
         FrameTemp<char> szPathCopy( pathLen + 1);

         dStrcpy( szPathCopy, pathLen + 1, fdd.mFile );
         //forwardslash( szPathCopy );

         const char *path = dStrrchr(szPathCopy, '/');
         if(path)
         {
            U32 len = path - szPathCopy;
            szPathCopy[len+1] = 0;

            Platform::setCurrentDirectory(szPathCopy);

            ResourceManager->setWriteablePath(Platform::getCurrentDirectory());
            ResourceManager->addPath( Platform::getCurrentDirectory() );

            success = str.open(fdd.mFile, FileStream::Read);
            if(success)
               defaultScriptName = fdd.mFile;
         }
      }
#endif
      if( !success )
      {
         char msg[1024];
         dSprintf(msg, sizeof(msg), "Failed to open \"%s\".", defaultScriptName);
         Platform::AlertOK("Error", msg);
         return false;
      }
       mainCsStream = &str;
   }

   U32 size = mainCsStream->getStreamSize();
   char *script = new char[size + 1];
   mainCsStream->read(size, script);

   str.close();

   script[size] = 0;

   char buffer[1024], *ptr;
   Platform::makeFullPathName(useDefaultScript ? defaultScriptName : argv[1], buffer, sizeof(buffer), Platform::getCurrentDirectory());
   ptr = dStrrchr(buffer, '/');
   if(ptr != NULL)
      *ptr = 0;
   Platform::setMainDotCsDir(buffer);
   Platform::setCurrentDirectory(buffer);

   Con::evaluate(script, false, useDefaultScript ? defaultScriptName : argv[1]); 
   delete[] script;

#endif		//NTJ_EDITOR

#ifndef NTJ_SERVER
	//生成Shader临时生成文件
   {
		const char filename[] = "*.*";
		char dir[MAX_PATH];
		dMemset(dir, 0, sizeof(dir));
		const char *shaderPath = Con::getVariable("$pref::Video::shaderPath");
		if (dStrlen(shaderPath) == 0)
			shaderPath = "temp";
		dSprintf(dir, sizeof(dir), "%s/%s", Platform::getCurrentDirectory(), shaderPath);
		Platform::deleteDirFiles(dir, filename);
		dSprintf(dir, sizeof(dir), "%s/", dir);
		Platform::createPath(dir);
	}
#endif
   return true;
}

extern void OnTimeEvent(S32 timeDelta);

// Process a time event and update all sub-processes
void processTimeEvent(S32 elapsedTime)
{
   PROFILE_START(ProcessTimeEvent);

   // cap the elapsed time to one second
   // if it's more than that we're probably in a bad catch-up situation
   if(elapsedTime > 1024)
      elapsedTime = 1024;

   U32 timeDelta;
   if(ATTS(gTimeAdvance))
      timeDelta = ATTS(gTimeAdvance);
   else
      timeDelta = (U32) (elapsedTime * ATTS(gTimeScale));

   Platform::advanceTime(elapsedTime);

   bool tickPass;

#if (defined(NTJ_SERVER) || defined(NTJ_EDITOR))   
   PROFILE_START(ServerProcess);
   tickPass = serverProcess(timeDelta);
   PROFILE_END(ServerProcess);

   PROFILE_START(ServerNetProcess);
   // only send packets if a tick happened
   if(tickPass)
      GNet->processServer();
   PROFILE_END(ServerNetProcess);
#endif

   PROFILE_START(SimAdvanceTime);
   Sim::advanceTime(timeDelta);
   PROFILE_END(SimAdvanceTime);

#if (defined(NTJ_CLIENT) || defined(NTJ_EDITOR))
   PROFILE_START(ClientProcess);
   tickPass = clientProcess(timeDelta);
   PROFILE_END(ClientProcess);

   PROFILE_START(ClientNetProcess);
   if(tickPass)
      GNet->processClient();
   PROFILE_END(ClientNetProcess);
#endif

   GNet->checkTimeouts();

   gFPS.update();

   PROFILE_END(ProcessTimeEvent);

   OnTimeEvent(timeDelta);

   // Update the console time
   Con::setFloatVariable("Sim::Time",F64(Platform::getVirtualMilliseconds()) / 1000);
}

void StandardMainLoop::initMainLoop()
{
    TimeManager tm;
    tm.timeEvent.notify(&::processTimeEvent);
    tm.setBackgroundThreshold(mClamp(sgBackgroundProcessSleepTime, 1, 200));
    tm.setForegroundThreshold(mClamp(sgTimeManagerProcessInterval, 1, 200));

    // Hook in for UDP notification
    Net::smPacketReceive.notify(GNet, &NetInterface::processPacketReceiveEvent);
    Net::smPacketReceiveEx.notify(GNet, &NetInterface::processPacketReceiveEventEx);
}

void StandardMainLoop::doMainLoopOnce()
{
    PROFILE_START(MainLoop);
    Sampler::beginFrame();

    Process::processEvents();

    Sampler::endFrame();
    PROFILE_END(MainLoop);
}

void StandardMainLoop::doMainLoop()
{
   // Register event with this guy.
   TimeManager tm;
   tm.timeEvent.notify(&::processTimeEvent);

   // Hook in for UDP notification
   Net::smPacketReceive.notify(GNet, &NetInterface::processPacketReceiveEvent);
   Net::smPacketReceiveEx.notify(GNet, &NetInterface::processPacketReceiveEventEx);

   bool keepRunning = true;
   while(keepRunning)
   {
      // Update our timer state info - we could do this a little less but
      // this is simple and cheap.
      tm.setBackgroundThreshold(mClamp(sgBackgroundProcessSleepTime, 1, 200));
      tm.setForegroundThreshold(mClamp(sgTimeManagerProcessInterval, 1, 200));
      if(WindowManager->getFirstWindow())
      {
         static bool lastFocus = false;
         bool newFocus = WindowManager->getFirstWindow()->isFocused();
         if(lastFocus != newFocus)
            lastFocus = newFocus;

         tm.setBackground(!newFocus);
      }
      else
         tm.setBackground(false);

      PROFILE_START(MainLoop);
	  Sampler::beginFrame();

      if(!Process::processEvents())
         keepRunning = false;

      CBackgroundLoadMgr::Instance()->Update();
      
	  Sampler::endFrame();
      PROFILE_END(MainLoop);
   }
}

void StandardMainLoop::setRestart(bool restart )
{
   gRequiresRestart = restart;
}

bool StandardMainLoop::requiresRestart()
{
   return gRequiresRestart;
}
