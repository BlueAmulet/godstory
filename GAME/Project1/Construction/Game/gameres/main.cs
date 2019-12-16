//-----------------------------------------------------------------------------
// PowerEngine                                                                 
// Copyright (C) FireRain.com, Inc.                                            
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Package overrides to initialize the mod.
package MMORPG {

function onStart()
{
   // Here is where we will do the video device stuff, so it overwrites the defaults
   // First set the PCI device variables (yes AGP/PCI-E works too)
   initDisplayDeviceInfo();
	 
   // Uncomment useNVPerfHud to allow you to start up correctly
   // when you drop your executable onto NVPerfHud
   //$Video::useNVPerfHud = true;
   
   // Uncomment these to allow you to force your app into using
   // a specific pixel shader version (0 is for fixed function)
   //$pref::Video::forcePixVersion = true;
   //$pref::Video::forcedPixVersion = 0;
   
    //加载客户端全局变量
   exec("./client/globalScriptVariable.cs");
   exec("./client/globalScriptFunction.cs");
   
   // Defaults console values
   exec("./client/defaults.cs");

   // Preferences (overide defaults)
   if (isFile("./client/prefs.cs"))
      exec("./client/prefs.cs");
   
   // Initialise stuff.
   exec("./client/scripts/common.cs");
   initializeCommon();

   echo(" % - Initialized Common");
   
   
   echo("\n--------- Initializing MOD: MMORPG ---------");

   // Load the scripts that start it all...
   exec("./client/client.cs");
   
   // Start up the audio system.
   sfxStartup();

   // Start up in either client, or dedicated server mode
   //configureCanvas();
   initClient();
   exec("./data/repository/function.cs");
	 // 加载地图对话任务脚本
	 exec("./Data/Quest/ClientMission.cs");
}

function onExit()
{
   echo("Exporting client prefs");
   
   //Ray:客户端不再导到prefs.cs了，全部放置到config.ini
   //export("$pref::*", "./client/prefs.cs", False);
   
   //保存自动售卖列表
   saveAutoSellList();
   // Ensure that we are disconnected and/or the server is destroyed.
   // This prevents crashes due to the SceneGraph being deleted before
   // the objects it contains.
   disconnect();
   
   // Shutdown stuff.
   shutdownCommon();
}

}; // Client package
activatePackage(MMORPG);
