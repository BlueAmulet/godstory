//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Mission Loading
// Server download handshaking.  This produces a number of onPhaseX
// calls so the game scripts can update the game's GUI.
//
// Loading Phases:
// Phase 1: Download Datablocks
// Phase 2: Download Ghost Objects
// Phase 3: Scene Lighting
//----------------------------------------------------------------------------
function clientCmdMissionCrcError()
{
	echo("CRC ERROR!	");
}

//----------------------------------------------------------------------------
// Phase 1 
//----------------------------------------------------------------------------

function clientCmdMissionStartPhase1(%seq, %missionName, %musicTrack)
{
   // These need to come after the cls.
   echo ("*** New Mission: " @ %missionName);
   echo ("*** Phase 1: Download Datablocks & Targets");
   onMissionDownloadPhase1(%missionName, %musicTrack);
   commandToServer('MissionStartPhase1Ack', %seq);
}

function onDataBlockObjectReceived(%index, %total)
{
   onPhase1Progress(%index / %total);
}

//----------------------------------------------------------------------------
// Phase 2
//----------------------------------------------------------------------------

function clientCmdMissionStartPhase2(%seq,%missionName)
{
   echo ("*** Phase 2: Download Ghost Objects");
   
   if(!SptIsLineSwitching())
   		purgeResources();
   		
   echo( %missionName );
   onMissionDownloadPhase2(%missionName);
   %crc = getMissionCrc();
   echo( "Client Mission Crc: " @ %crc );
   commandToServer('MissionStartPhase2Ack', %seq, $pref::Player:PlayerDB, %crc );
}

function onGhostAlwaysStarted(%ghostCount)
{
   if(!SptIsLineSwitching())
   {
   	  
   		$ghostCount = %ghostCount;
   		$ghostsRecvd = 0;
   		
   		// 设置非等待渲染模式
   		%mode = $pref::Video::disableVerticalSync;
   		$pref::Video::disableVerticalSync = 1;
   		canvas.resetMode();
   		
   		// 加载BIN资源
   		LoadClientMisOpt();
   		
   		// 还原设置
   		$pref::Video::disableVerticalSync = %mode;
   		canvas.resetMode();
   }
}

function onGhostAlwaysObjectReceived()
{
 	 if(!SptIsLineSwitching())
   {	
   		$ghostsRecvd++;
   		onPhase2Progress($ghostsRecvd / $ghostCount);
   }
}

//----------------------------------------------------------------------------
// Phase 3
//----------------------------------------------------------------------------

function clientCmdMissionStartPhase3(%seq,%missionName)
{
   StartClientReplication();
   StartFoliageReplication();
   echo ("*** Phase 3: Mission Lighting");
   $MSeq = %seq;
   $Client::MissionFile = %missionName;

   // Need to light the mission before we are ready.
   // The sceneLightingComplete function will complete the handshake 
   // once the scene lighting is done.
   if (lightScene("sceneLightingComplete", ""))
   {
      echo("Lighting mission....");
	 		if(!SptIsLineSwitching())
   		{      
      		schedule(1, 0, "updateLightingProgress");
      		onMissionDownloadPhase3(%missionName);
      }
      $lightingMission = true;
   }
}

function updateLightingProgress()
{
		onPhase3Progress($SceneLighting::lightingProgress);

 		if ($lightingMission)
    		$lightingProgressThread = schedule(1, 0, "updateLightingProgress");
}

function sceneLightingComplete()
{
   echo("Mission lighting done");
   $lightingMission = false;
   
   if(!SptIsLineSwitching())
   {  
   		onPhase3Complete();
   }
   
   // The is also the end of the mission load cycle.
   onMissionDownloadComplete();
   
   %UID = GetSelectedPlayerUID();
   %PlayerId = GetSelectedPlayerId();
   commandToServer('MissionStartPhase3Ack', $MSeq,%UID,%PlayerId);
   PauseSound(1); 
}

//----------------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------------

function switchToWaitScreen()
{
	%bitmap = getRandom(1,9);
	switch(%bitmap)
	{
		case 1:
			%bitmap = "~/gui/images/GUIlogin10_1_005";
		case 2:
			%bitmap = "~/gui/images/GUIlogin10_1_006";
		case 3:
			%bitmap = "~/gui/images/GUIlogin10_1_007";
		case 4:
			%bitmap = "~/gui/images/GUIlogin10_1_008";
		case 5:
			%bitmap = "~/gui/images/GUIlogin10_1_009";
		case 6:
			%bitmap = "~/gui/images/GUIlogin10_1_010";
		case 7:
			%bitmap = "~/gui/images/GUIlogin10_1_011";
		case 8:
			%bitmap = "~/gui/images/GUIlogin10_1_012";
		case 9:
			%bitmap = "~/gui/images/GUIlogin10_1_013";
	}
	LoadingGui_CenterBitmap.setbitmap(%bitmap);
	SetLoadingValue( 0 );
	Canvas.setContent("LoadingGui");
	Canvas.repaint();
}

function connect(%server, %layer)
{
	// switchToWaitScreen();
	if( LoadingGui_CenterBitmap.bitmap $= "" )
		LoadingGui_CenterBitmap.setbitmap("~/gui/images/GUIlogin10_1_005");
		
	SetLoadingValue( 0 );
	Canvas.setContent("LoadingGui");
	Canvas.repaint();
	 %conn = new GameConnection(ServerConnection);
   RootGroup.add(ServerConnection);
   %conn.setConnectArgs($pref::Player::Name, %layer );
   %conn.setJoinPassword($Client::Password);
   %conn.connect(%server);
}

function connectFoo(%server,%name, %port)
{
	 switchToWaitScreen();
	 %socket = SetNetPortEx(%port);
   %conn = new GameConnection(%name);
   %conn.SetPort(%socket);
   RootGroup.add(%name);
   %conn.setConnectArgs($pref::Player::Name);
   %conn.setJoinPassword($Client::Password);
   %conn.connect(%server);
}


function clientCmdSyncClock(%time)
{
   // Store the base time in the hud control it will automatically increment.
//   HudClock.setTime(%time);
}

//----------------------------------------------------------------------------
// Loading Phases:
// Phase 1: Download Datablocks
// Phase 2: Download Ghost Objects
// Phase 3: Scene Lighting

//----------------------------------------------------------------------------
// Phase 1
//----------------------------------------------------------------------------

function onMissionDownloadPhase1(%missionName, %musicTrack)
{
   // Reset the loading progress controls:
   LoadingProgress.setValue(0);
   LoadingProgressTxt.setValue("LOADING DATABLOCKS");
}

function onPhase1Progress(%progress)
{
   LoadingProgress.setValue(%progress);
   Canvas.repaint();
}

//----------------------------------------------------------------------------
// Phase 2
//----------------------------------------------------------------------------

function onMissionDownloadPhase2( %MissionFile )
{
	 setMissionFile( %MissionFile );
   // Reset the loading progress controls:
   LoadingProgress.setValue(0);
   LoadingProgressTxt.setValue("LOADING OBJECTS");
   Canvas.repaint();
}

function onPhase2Progress(%progress)
{
   LoadingProgress.setValue(%progress);
   Canvas.repaint();
}

//----------------------------------------------------------------------------
// Phase 3
//----------------------------------------------------------------------------

function onMissionDownloadPhase3()
{
   LoadingProgress.setValue(0);
   LoadingProgressTxt.setValue("LIGHTING MISSION");
   Canvas.repaint();
}

function onPhase3Progress(%progress)
{
   LoadingProgress.setValue(%progress);
}

function onPhase3Complete()
{
   LoadingProgress.setValue( 0.98 );
   
   SptSocialRequest();
}

//----------------------------------------------------------------------------
// Mission loading done!
//----------------------------------------------------------------------------

function onMissionDownloadComplete()
{
   // Client will shortly be dropped into the game, so this is
   // good place for any last minute gui cleanup.
   
}
