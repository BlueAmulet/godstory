//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// Variables used by server scripts & code.  The ones marked with (c)
// are accessed from code.  Variables preceeded by Pref:: are server
// preferences and stored automatically in the ServerPrefs.cs file
// in between server sessions.
//
//    (c) Server::ServerType              {SinglePlayer, MultiPlayer}
//    (c) Server::GameType                Unique game name
//    (c) Server::Dedicated               Bool
//    ( ) Server::MissionFile             Mission .mis file name
//    (c) Server::MissionName             DisplayName from .mis file
//    (c) Server::MissionType             Not used
//    (c) Server::PlayerCount             Current player count
//    (c) Server::GuidList                Player GUID (record list?)
//    (c) Server::Status                  Current server status
//
//    (c) Pref::Server::Name              Server Name
//    (c) Pref::Server::Password          Password for client connections
//    ( ) Pref::Server::AdminPassword     Password for client admins
//    (c) Pref::Server::Info              Server description
//    (c) Pref::Server::MaxPlayers        Max allowed players
//    (c) Pref::Server::RegionMask        Registers this mask with master server
//    ( ) Pref::Server::BanTime           Duration of a player ban
//    ( ) Pref::Server::KickBanTime       Duration of a player kick & ban
//    ( ) Pref::Server::MaxChatLen        Max chat message len
//    ( ) Pref::Server::FloodProtectionEnabled Bool
      $Player::RecoverVigorVelocity = 1;   //活力恢复速度

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

function initServer()
{
   echo("\n--------- Initializing " @ $appName @ ": Server Scripts ---------");

   // Server::Status is returned in the Game Info Query and represents the
   // current status of the server. This string sould be very short.
   $Server::Status = "Unknown";

   // Turn on testing/debug script functions
   $Server::TestCheats = true;

   // Specify where the mission files are.
   $Server::MissionFileSpec = "*/missions/*.mis";

   // The common module provides the basic server functionality
   initBaseServer();

   // Load up game server support scripts
   exec("./scripts/commands.cs");
   exec("./scripts/game.cs");
}


//-----------------------------------------------------------------------------

function initDedicated()
{
   enableWinConsole(true);
   echo("\n--------- Starting Dedicated Server ---------");

   // Make sure this variable reflects the correct state.
   $Server::Dedicated = true;

   // The server isn't started unless a mission has been specified.
   if ($missionArg !$= "") {
      createServer("MultiPlayer", $missionArg);
   }
   else
      echo("No mission specified (use -mission filename)");
}

function initBaseServer()
{
   // Base server functionality
   exec("./message.cs");
   exec("./missionLoad.cs");
   exec("./missionDownload.cs");
   exec("./clientConnection.cs");
   exec("./game.cs");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

function portInit(%port)
{
   %failCount = 0;
  
   while(%failCount < 10 && !setNetPort(%port)) {
      echo("Port init failed on port " @ %port @ " trying next port.");
      %port++; %failCount++;
   }
}
 
function createServer(%serverType, %mission)
{
   if (%mission $= "") {
      error("createServer: mission name unspecified");
      return;
   }
   
   // Make sure our mission name is relative so that it can send
   // across the network correctly
   %mission = makeRelativePath(%mission, getWorkingDirectory());

   destroyServer();

   //
   $missionSequence = 0;
   $Server::PlayerCount = 0;
   $Server::ServerType = %serverType;

   // Setup for multi-player, the network must have been
   // initialized before now.
   if (%serverType $= "MultiPlayer") {
      echo("Starting multiplayer mode");

      // Make sure the network port is set to the correct pref.
      portInit($Pref::Server::Port);
      allowConnections(true);
   }

   // Load the mission
   $ServerGroup = new SimGroup(ServerGroup);
   onServerCreated();
   loadMission(%mission, true);
}


//-----------------------------------------------------------------------------

function destroyServer()
{
   $Server::ServerType = "";
   allowConnections(false);
   stopHeartbeat();
   $missionRunning = false;
   
   // End any running mission
   endMission();
   onServerDestroyed();

   // Delete all the server objects
   if (isObject(MissionGroup))
      MissionGroup.delete();
   if (isObject(MissionCleanup))
      MissionCleanup.delete();
   if (isObject($ServerGroup))
      $ServerGroup.delete();

   // Delete all the connections:
   while (ClientGroup.getCount())
   {
      %client = ClientGroup.getObject(0);
      %client.delete();
   }

   // Delete all the data blocks...
   deleteDataBlocks();
   
   // Save any server settings
   echo( "Exporting server prefs..." );
   export( "$Pref::Server::*", "~/Server/prefs.cs", false );

   // Dump anything we're not using
   purgeResources();
}


//--------------------------------------------------------------------------

function resetServerDefaults()
{
   echo( "Resetting server defaults..." );
   
   // Override server defaults with prefs:   
   exec( "~/Server/defaults.cs" );
   exec( "~/Server/prefs.cs" );

   loadMission( $Server::MissionFile );
}



