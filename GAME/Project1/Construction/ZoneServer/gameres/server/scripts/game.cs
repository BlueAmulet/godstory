//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Functions that implement game-play
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function onServerCreated()
{
   // Server::GameType is sent to the master server.
   // This variable should uniquely identify your game and/or mod.
   $Server::GameType = "Test App";

   // Server::MissionType sent to the master server.  Clients can
   // filter servers based on mission type.
   $Server::MissionType = "Deathmatch";

   // GameStartTime is the sim time the game started. Used to calculated
   // game elapsed time.
   $Game::StartTime = 0;

   // Load up all datablocks, objects etc.  This function is called when
   // a server is constructed.
   exec("./camera.cs");
   exec("./markers.cs"); 
   exec("./datablock.cs");
  
   exec("./Player.cs");
   exec("./CreatePlayer.cs");

   // Keep track of when the game started
   $Game::StartTime = $Sim::Time;
}

function onServerDestroyed()
{
   // This function is called as part of a server shutdown.
}


//-----------------------------------------------------------------------------

function onMissionLoaded()
{
	 CollectionResInfo();
	 
   // Called by loadMission() once the mission is finished loading.
   // Nothing special for now, just start up the game play.
   startGame();
}

function onMissionEnded()
{
   // Called by endMission(), right before the mission is destroyed

   // Normally the game should be ended first before the next
   // mission is loaded, this is here in case loadMission has been
   // called directly.  The mission will be ended if the server
   // is destroyed, so we only need to cleanup here.
   $Game::Running = false;
   $Game::Cycling = false;
}


//-----------------------------------------------------------------------------

function startGame()
{
   if ($Game::Running) {
      error("startGame: End the game first!");
      return;
   }

   // Inform the client we're starting up
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      %cl = ClientGroup.getObject( %clientIndex );
      commandToClient(%cl, 'GameStart');
   }

   $Game::Running = true;
}

function endGame()
{
   if (!$Game::Running)  {
      error("endGame: No game running!");
      return;
   }

   // Inform the client the game is over
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      %cl = ClientGroup.getObject( %clientIndex );
      commandToClient(%cl, 'GameEnd');
   }

   // Delete all the temporary mission objects
   resetMission();
   $Game::Running = false;
}

//-----------------------------------------------------------------------------
// GameConnection Methods
// These methods are extensions to the GameConnection class. Extending
// GameConnection make is easier to deal with some of this functionality,
// but these could also be implemented as stand-alone functions.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function GameConnection::onClientEnterGame(%this,%UID,%PlayerId)
{
   commandToClient(%this, 'SyncClock', $Sim::Time - $Game::StartTime);

   // Create a new camera object.
   %this.camera = new Camera() {
      dataBlock = Observer;
   };
   MissionCleanup.add( %this.camera );
   %this.camera.scopeToClient(%this);
 	 
	 //设置相机坐标  
   %spawnPoint = pickSpawnPoint();
   %this.camera.setTransform( %spawnPoint );
 	 %this.setControlObject(%this.camera);
   
   // Create a player object.
   %this.spawnPlayer(%UID,%PlayerId);
}

function GameConnection::onClientLeaveGame(%this)
{
   if (isObject(%this.camera))
      %this.camera.delete();
   if (isObject(%this.player))
      %this.player.delete();
}


//-----------------------------------------------------------------------------

function GameConnection::onLeaveMissionArea(%this)
{
   // The control objects invoked this method when they
   // move out of the mission area.
}

function GameConnection::onEnterMissionArea(%this)
{
   // The control objects invoked this method when they
   // move back into the mission area.
}


//-----------------------------------------------------------------------------

function GameConnection::spawnPlayer(%this,%UID,%PlayerId)
{
   // Combination create player and drop him somewhere
   if(%this.createPlayer(%UID,%PlayerId))
   {
 			// Update the camera to start with the player
   		%this.camera.setTransform(%this.Player.getEyeTransform());
      MissionCleanup.add(%this.Player);
   }
}   


//-----------------------------------------------------------------------------

//function GameConnection::createPlayer(%this, %spawnPoint)
//{
//   if (%this.player > 0)  {
//      // The client should not have a player currently
//      // assigned.  Assigning a new one could result in 
//      // a player ghost.
//      error( "Attempting to create an angus ghost!" );
//   }
//
//   // Create the player object
//   %player = new Player() {
//      dataBlockName = P_1;
//      client = %this;
//   };
//   MissionCleanup.add(%player);
//
//   // Player setup...
//   %player.setTransform(%spawnPoint);
//   %player.setEnergyLevel(%player.getDataBlock().maxEnergy);
//   %player.setShapeName(%this.name);
//   
//   // Update the camera to start with the player
//   %this.camera.setTransform(%player.getEyeTransform());
//
//   // Give the client control of the player
//   %this.player = %player;
//   %this.setControlObject(%player);
//}


//-----------------------------------------------------------------------------
// Support functions
//-----------------------------------------------------------------------------

function pickSpawnPoint() 
{
   %groupName = "MissionGroup/PlayerDropPoints";
   %group = nameToID(%groupName);

   if (%group != -1) {
      %count = %group.getCount();
      if (%count != 0) {
         %index = getRandom(%count-1);
         %spawn = %group.getObject(%index);
         return %spawn.getTransform();
      }
      else
         error("No spawn points found in " @ %groupName);
   }
   else
      error("Missing spawn points group " @ %groupName);

   // Could be no spawn points, in which case we'll stick the
   // player at the center of the world.
   return "0 0 300 1 0 0 0";
}

//load datablocks
function LoadDataBlocks(%basepath)
{
   //load precompiled...
   %path = %basepath @ "*.cmp";
   echo("");
   echo("//-----------------------------------------------");
   echo("Loading light datablocks from: " @ %basepath);
   %file = findFirstFile(%path);
  
  while(%file !$= "")
  {
     %file = filePath(%file) @ "/" @ fileBase(%file)  @ ".cmp";
     exec(%file);
     %file = findNextFile(%path);
  }

   //load uncompiled...
   %path = %basepath @ "*.cs";
   %file = findFirstFile(%path);

   while(%file !$= "")
   {
     %file = filePath(%file) @ "/" @ fileBase(%file) @ ".cs";
     exec(%file);
     %file = findNextFile(%path);
   }
}