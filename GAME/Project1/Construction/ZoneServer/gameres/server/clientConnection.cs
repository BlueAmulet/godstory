//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// This script function is called before a client connection
// is accepted.  Returning "" will accept the connection,
// anything else will be sent back as an error to the client.
// All the connect args are passed also to onConnectRequest
//
function GameConnection::onConnectRequest( %client, %netAddress, %name )
{
   echo("Connect request from: " @ %netAddress);
   if($Server::PlayerCount >= $pref::Server::MaxPlayers)
      return "CR_SERVERFULL";
   return "";
}

//-----------------------------------------------------------------------------
// This script function is the first called on a client accept
//
function GameConnection::onConnect( %client, %name, %layer )
{
   // Simulated client lag for testing...
   // %client.setSimulatedNetParams(0.1, 30);

   %client.guid = 0;  //ÓÃ»§Î¨Ò»ID
  
   $instantGroup = ServerGroup;
   $instantGroup = MissionCleanup;

   echo("CADD: " @ %client @ " " @ %client.getAddress() @ "LayerId:" @ %layer );
   
   // give the connection the right layer id
		%client.setLayerId( %layer );
		
   // If the mission is running, go ahead download it to the client
   if ($missionRunning)
      %client.loadMission();
   $Server::PlayerCount++;
}

//-----------------------------------------------------------------------------
// This function is called when a client drops for any reason
//
function GameConnection::onDrop(%client, %reason)
{
   %client.onClientLeaveGame();
   
   removeTaggedString(%client.name);
   echo("CDROP: " @ %client @ " " @ %client.getAddress());
   $Server::PlayerCount--;
   
   // Reset the server if everyone has left the game
   //if( $Server::PlayerCount == 0)
   //   schedule(0, 0, "resetServerDefaults");
}


//-----------------------------------------------------------------------------

function GameConnection::startMission(%this)
{
   // Inform the client the mission starting
   commandToClient(%this, 'MissionStart', $missionSequence);
}


function GameConnection::endMission(%this)
{
   // Inform the client the mission is done
   commandToClient(%this, 'MissionEnd', $missionSequence);
}


//--------------------------------------------------------------------------
// Sync the clock on the client.

function GameConnection::syncClock(%client, %time)
{
   commandToClient(%client, 'syncClock', %time);
}

