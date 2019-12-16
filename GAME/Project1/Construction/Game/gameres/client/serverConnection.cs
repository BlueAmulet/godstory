//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

// Functions dealing with connecting to a server


//----------------------------------------------------------------------------
// GameConnection client callbacks
//----------------------------------------------------------------------------

function GameConnection::initialControlSet(%this)
{
   echo ("*** Initial Control Object");
	
	 SptDisableLineSwitching();
	 SptDisableGoingCopymap();
   
  if (Canvas.getContent() != GameMainWndGui.getId())
  {
     Canvas.setContent(GameMainWndGui);
  	 Canvas.popDialog(DialogWnd_1);
  }
  

}

function GameConnection::setLagIcon(%this, %state)
{
   if (%this.getAddress() $= "local")
      return;
   LagIcon.setVisible(%state $= "true");
}

function GameConnection::onConnectionAccepted(%this)
{
   // Called on the new connection object after connect() succeeds.
   LagIcon.setVisible(false);
}

function GameConnection::onConnectionTimedOut(%this)
{
   // Called when an established connection times out
   disconnectedCleanup();
   MessageBoxOK( "TIMED OUT", "The server connection has timed out.");
}

function GameConnection::onConnectionDropped(%this, %msg)
{
   // Established connection was dropped by the server
   disconnectedCleanup();
   MessageBoxOK( "DISCONNECT", "The server has dropped the connection: " @ %msg);
}

function GameConnection::onConnectionError(%this, %msg)
{
   // General connection error, usually raised by ghosted objects
   // initialization problems, such as missing files.  We'll display
   // the server's connection error message.
   disconnectedCleanup();
   MessageBoxOK( "DISCONNECT", $ServerConnectionErrorMessage @ " (" @ %msg @ ")" );
}


//----------------------------------------------------------------------------
// Connection Failed Events
//----------------------------------------------------------------------------

function GameConnection::onConnectRequestRejected( %this, %msg )
{
   switch$(%msg)
   {
      case "CR_INVALID_PROTOCOL_VERSION":
         %error = "Incompatible protocol version: Your game version is not compatible with this server.";
      case "CR_INVALID_CONNECT_PACKET":
         %error = "Internal Error: badly formed network packet";
      case "CR_YOUAREBANNED":
         %error = "You are not allowed to play on this server.";
      case "CR_SERVERFULL":
         %error = "This server is full.";
      case "CHR_PASSWORD":
         // XXX Should put up a password-entry dialog.
         if ($Client::Password $= "")
            MessageBoxOK( "REJECTED", "That server requires a password.");
         else {
            $Client::Password = "";
            MessageBoxOK( "REJECTED", "That password is incorrect.");
         }
         return;
      case "CHR_PROTOCOL":
         %error = "Incompatible protocol version: Your game version is not compatible with this server.";
      case "CHR_CLASSCRC":
         %error = "Incompatible game classes: Your game version is not compatible with this server.";
      case "CHR_INVALID_CHALLENGE_PACKET":
         %error = "Internal Error: Invalid server response packet";
      default:
         %error = "Connection error.  Please try another server.  Error code: (" @ %msg @ ")";
   }
   disconnectedCleanup();
   MessageBoxOK( "REJECTED", %error);
}

function GameConnection::onConnectRequestTimedOut(%this)
{
   disconnectedCleanup();
   MessageBoxOK( "TIMED OUT", "Your connection to the server timed out." );
}


//-----------------------------------------------------------------------------
// Disconnect
//-----------------------------------------------------------------------------

function disconnect()
{
   // Delete the connection if it's still there.
   switchToWaitScreen();
   
   if (isObject(ServerConnection))
      ServerConnection.delete();
   disconnectedCleanup();

   // Call destroyServer in case we're hosting
   //destroyServer();
}

function disconnectedCleanup()
{
	if(!SptIsLineSwitching() && !SptIsGoingCopymap())
	{
		echo("cleanup resource");
	   if( isObject(ClientMissionCleanup) )
	   {
	      ClientMissionCleanup.delete();
	   }
	   clearClientPaths();
	
	   // Clear misc script stuff
	   HudMessageVector.clear();
	
	   // Terminate all playing sounds
	   sfxStopAll();
	   PauseSound(0);
	   if (isObject(MusicPlayer))
	      MusicPlayer.stop();
	
	   //
	   LagIcon.setVisible(false);
	   
	   // Clear all print messages
	   clientCmdclearBottomPrint();
	   clientCmdClearCenterPrint();
	
	   // Back to the launch screen
	   //if (isObject( MainMenuGui ))
	   //   Canvas.setContent( MainMenuGui );
	
	   // Dump anything we're not using
	   clearTextureHolds();
	   purgeResources();
  }
	// 无论切线还是断开连接，都需要清除客户端某些信息
	SptOnDisconnectZoneServer();
}

