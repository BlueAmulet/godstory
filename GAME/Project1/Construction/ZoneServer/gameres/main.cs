//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Package overrides to initialize the mod.
package MMORPG {

function onStart()
{
   exec("./server/server.cs");

   echo("\n--------- Initializing MOD: MMORPG ---------");
   
   // Defaults console values
   exec("./server/defaults.cs");

   // Preferences (overide defaults)
   if (isFile("./server/prefs.cs"))
      exec("./server/prefs.cs");


   // Server gets loaded for all sessions, since clients
   // can host in-game servers.
   initServer();
	
	 exec("./data/repository/function.cs");
	 // 加载地图对话任务脚本
	 exec("./Data/Quest/ServerMission.cs");
}

function onExit()
{
   // Ensure that we are disconnected and/or the server is destroyed.
   // This prevents crashes due to the SceneGraph being deleted before
   // the objects it contains.
   disconnect();
   
   //echo("Exporting server prefs");
   //export("$Pref::Server::*", "./server/prefs.cs", False);

   Parent::onExit();
}

}; // Client package
activatePackage(MMORPG);
