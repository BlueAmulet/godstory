//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

if ( isObject( moveMap ) )
   moveMap.delete();
new ActionMap(moveMap);

//------------------------------------------------------------------------------
// Non-remapable binds
//------------------------------------------------------------------------------

//function escapeFromGame()
//{
//   if ( $Server::ServerType $= "SinglePlayer" )
//      MessageBoxYesNoOld( "Quit Mission", "Exit from this Mission?", "quit();", "");
//   else
//      MessageBoxYesNoOld( "Disconnect", "Disconnect from the server?", "quit();", "");
//}

//moveMap.bindCmd(keyboard, "escape", "", "escapeFromGame();");

//------------------------------------------------------------------------------
// Movement Keys
//------------------------------------------------------------------------------

$movementSpeed = 1; // m/s

function setSpeed(%speed)
{
   if(%speed)
      $movementSpeed = %speed;
}

function moveleft(%val)
{
   $KEY_moveLeft = %val;
   updateTurnLeft();
}

function moveright(%val)
{
   $KEY_moveRight = %val;
   updateTurnRight();
}

function moveforward(%val)
{
   $mvForwardAction = %val;
   $AutoRun = false;
}

function movebackward(%val)
{
   $mvBackwardAction = %val;
   $AutoRun = false;
}

function moveup(%val)
{
   $mvUpAction = %val;
}

function movedown(%val)
{
   $mvDownAction = %val;
}

function turnLeft( %val )
{
   $KEY_turnLeft = %val;
   updateTurnLeft();
}

function turnRight( %val )
{
   $KEY_turnRight = %val;
   updateTurnRight();
}

function updateTurnLeft()
{
   if($CameraDragged == 1 && $mvFreeLook == 0)
   {
      $mvLeftAction = ($KEY_moveLeft || $KEY_turnLeft);
      $mvYawRightSpeed = 0;
   }
   else
   {
      $mvLeftAction = $KEY_moveLeft;
      $mvYawRightSpeed = $KEY_turnLeft ? $Pref::Input::KeyboardTurnSpeed : 0;
   }
}

function updateTurnRight()
{
   if($CameraDragged == 1 && $mvFreeLook == 0)
   {
      $mvRightAction = ($KEY_moveRight || $KEY_turnRight);
      $mvYawLeftSpeed = 0;
   }
   else
   {
      $mvRightAction = $KEY_moveRight;
      $mvYawLeftSpeed = $KEY_turnRight ? $Pref::Input::KeyboardTurnSpeed : 0;
   }
}

function panUp( %val )
{
   $mvPitchDownSpeed = %val ? $Pref::Input::KeyboardTurnSpeed : 0;
}

function panDown( %val )
{
   $mvPitchUpSpeed = %val ? $Pref::Input::KeyboardTurnSpeed : 0;
}

//function getMouseAdjustAmount(%val)
//{
//   // based on a default camera fov of 90'
//   return(%val * ($cameraFov / 90) * 0.005);
//}
//
//function yaw(%val)
//{
//   if($mvFreelook == 0)
//      $mvYaw += getMouseAdjustAmount(%val);
//   else
//      setCameraYaw( getCameraYaw()+getMouseAdjustAmount(%val));
//   $IsDragged = %val;
//}
//
//function pitch(%val)
//{
//   //$mvPitch += getMouseAdjustAmount(%val);
//   setCameraPitch( getCameraPitch()+getMouseAdjustAmount(%val));
//   $IsDragged = %val;
//}

//function jump(%val)
//{
//   $mvTriggerCount2++;
//}

function AutoRun(%val)
{
   if(%val)
      $AutoRun = !$AutoRun;
}

function UpdateOprationMode()
{
   if($Config::MouseOperation)
   {
      moveMap.unbind( keyboard, a, turnleft );
      moveMap.unbind( keyboard, d, turnright );
      moveMap.unbind( keyboard, q, moveleft );
      moveMap.unbind( keyboard, e, moveright );
      moveMap.unbind( keyboard, w, moveforward );
      moveMap.unbind( keyboard, s, movebackward );
      moveMap.unbind( keyboard, numlock, AutoRun );
     // moveMap.unbind( keyboard, space, jump );
   }
   else
   {
      moveMap.bind( keyboard, a, turnleft );
      moveMap.bind( keyboard, d, turnright );
      moveMap.bind( keyboard, q, moveleft );
      moveMap.bind( keyboard, e, moveright );
      moveMap.bind( keyboard, w, moveforward );
      moveMap.bind( keyboard, s, movebackward );
      moveMap.bind( keyboard, numlock, AutoRun );
      //moveMap.bind( keyboard, space, jump );
   }
}


moveMap.bind( mouse, xaxis, processYaw );
moveMap.bind( mouse, yaxis, processPitch );
UpdateOprationMode();

//------------------------------------------------------------------------------
// Mouse Trigger
//------------------------------------------------------------------------------

function mouseFire(%val)
{
   $mvTriggerCount0++;
}

function altTrigger(%val)
{
   $mvTriggerCount1++;
}

moveMap.bind( mouse, button0, mouseFire );
moveMap.bind( mouse, button1, altTrigger );


//------------------------------------------------------------------------------
// Zoom and FOV functions
//------------------------------------------------------------------------------

if($Pref::player::CurrentFOV $= "")
   $Pref::player::CurrentFOV = 45;

function setZoomFOV(%val)
{
   if(%val)
      toggleZoomFOV();
}

function toggleZoom( %val )
{
   if ( %val )
   {
      $ZoomOn = true;
      setFov( $Pref::player::CurrentFOV );
   }
   else
   {
      $ZoomOn = false;
      setFov( $Pref::player::DefaultFov );
   }
}

moveMap.bind(keyboard, r, setZoomFOV);
//moveMap.bind(keyboard, e, toggleZoom);


//------------------------------------------------------------------------------
// Camera & View functions
//------------------------------------------------------------------------------

//function toggleFreeLook( %val )
//{
//   if ( %val )
//      $mvFreeLook = true;
//   else
//      $mvFreeLook = false;
//}

//function toggleFirstPerson(%val)
//{
//   if (%val)
//   {
//      $firstPerson = !$firstPerson;
//      ServerConnection.setFirstPerson($firstPerson);
//   }
//}

//function toggleCamera(%val)
//{
//   if (%val)
//      commandToServer('ToggleCamera');
//}
//
//moveMap.bind( keyboard, z, toggleFreeLook );
//moveMap.bind(keyboard, tab, toggleFirstPerson );
//moveMap.bind(keyboard, "alt c", toggleCamera);


//------------------------------------------------------------------------------
// Misc. Player stuff
//------------------------------------------------------------------------------

moveMap.bindCmd(keyboard, "ctrl w", "commandToServer('playCel',\"wave\");", "");
moveMap.bindCmd(keyboard, "ctrl s", "commandToServer('playCel',\"salute\");", "");
moveMap.bindCmd(keyboard, "ctrl k", "commandToServer('suicide');", "");


//------------------------------------------------------------------------------
// Item manipulation
//------------------------------------------------------------------------------

moveMap.bindCmd(keyboard, "h", "commandToServer('use',\"HealthKit\");", "");
//moveMap.bindCmd(keyboard, "1", "commandToServer('use',\"Rifle\");", "");
//moveMap.bindCmd(keyboard, "2", "commandToServer('use',\"Crossbow\");", "");

//------------------------------------------------------------------------------
// Message HUD functions
//------------------------------------------------------------------------------

function pageMessageHudUp( %val )
{
   if ( %val )
      pageUpMessageHud();
}

function pageMessageHudDown( %val )
{
   if ( %val )
      pageDownMessageHud();
}

function resizeMessageHud( %val )
{
   if ( %val )
      cycleMessageHudSize();
}


moveMap.bind(keyboard, u, toggleMessageHud );
//moveMap.bind(keyboard, y, teamMessageHud );
moveMap.bind(keyboard, "pageUp", pageMessageHudUp );
moveMap.bind(keyboard, "pageDown", pageMessageHudDown );
moveMap.bind(keyboard, "p", resizeMessageHud );


//------------------------------------------------------------------------------
// Demo recording functions
//------------------------------------------------------------------------------

function startRecordingDemo( %val )
{
   if ( %val )
      startDemoRecord();
}

function stopRecordingDemo( %val )
{
   if ( %val )
      stopDemoRecord();
}

moveMap.bind( keyboard, F3, startRecordingDemo );
moveMap.bind( keyboard, F4, stopRecordingDemo );


//------------------------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------------------------

function dropCameraAtPlayer(%val)
{
   if (%val)
      commandToServer('dropCameraAtPlayer');
}

function dropPlayerAtCamera(%val)
{
   if (%val)
      commandToServer('DropPlayerAtCamera');
}

moveMap.bind(keyboard, "F8", dropCameraAtPlayer);
moveMap.bind(keyboard, "F7", dropPlayerAtCamera);


function bringUpOptions(%val)
{
   if(%val)
      Canvas.pushDialog(OptionsDlg);
}

GlobalActionMap.bind(keyboard, "ctrl o", bringUpOptions);


//------------------------------------------------------------------------------
// Dubuging Functions
//------------------------------------------------------------------------------

$MFDebugRenderMode = 0;
function cycleDebugRenderMode(%val)
{
   if (!%val)
      return;

   if (getBuildString() $= "Debug")
   {
      if($MFDebugRenderMode == 0)
      {
         // Outline mode, including fonts so no stats
         $MFDebugRenderMode = 1;
         setInteriorRenderMode(1);
      }
      else if ($MFDebugRenderMode == 1)
      {
         // Interior debug mode
         $MFDebugRenderMode = 2;
         setInteriorRenderMode(7);
      }
      else if ($MFDebugRenderMode == 2)
      {
         // Back to normal
         $MFDebugRenderMode = 0;
         setInteriorRenderMode(0);
      }
   }
   else
   {
      echo("Debug render modes only available when running a Debug build.");
   }
}

GlobalActionMap.bind(keyboard, "F9", cycleDebugRenderMode);


//------------------------------------------------------------------------------
// Misc.
//------------------------------------------------------------------------------

GlobalActionMap.bind(keyboard, "tilde", toggleConsole);
GlobalActionMap.bindCmd(keyboard, "alt enter", "", "toggleFullScreen();");
//GlobalActionMap.bindCmd(keyboard, "F1", "", "contextHelp();");

//------------------------------------------------------------------------------
// 游戏快捷键操作
//------------------------------------------------------------------------------
moveMap.bind( keyboard, "1", onShortCutSelect1 ); 
moveMap.bind( keyboard, "2", onShortCutSelect2 );
moveMap.bind( keyboard, "3", onShortCutSelect3 );
moveMap.bind( keyboard, "4", onShortCutSelect4 );
moveMap.bind( keyboard, "5", onShortCutSelect5 );
moveMap.bind( keyboard, "6", onShortCutSelect6 );
moveMap.bind( keyboard, "7", onShortCutSelect7 );
moveMap.bind( keyboard, "8", onShortCutSelect8 );
moveMap.bind( keyboard, "9", onShortCutSelect9 );
moveMap.bind( keyboard, "0", onShortCutSelect10 );
moveMap.bind( keyboard, "alt 1", onShortCutSelect1A ); 
moveMap.bind( keyboard, "alt 2", onShortCutSelect2A ); 
moveMap.bind( keyboard, "alt 3", onShortCutSelect3A ); 
moveMap.bind( keyboard, "alt 4", onShortCutSelect4A ); 
moveMap.bind( keyboard, "alt 5", onShortCutSelect5A ); 
moveMap.bind( keyboard, "alt 6", onShortCutSelect6A ); 
moveMap.bind( keyboard, "alt 7", onShortCutSelect7A ); 
moveMap.bind( keyboard, "alt 8", onShortCutSelect8A ); 
moveMap.bind( keyboard, "alt 9", onShortCutSelect9A ); 
moveMap.bind( keyboard, "alt 0", onShortCutSelect10A ); 
moveMap.bind( keyboard, a, togglePack ); 
moveMap.bind( keyboard, "alt a", togglePack );
moveMap.bind( keyboard, c, togglePersona );     
moveMap.bind( keyboard, "alt c", togglePersona );
moveMap.bind( keyboard, f, toggleRelation ); 
moveMap.bind( keyboard, "alt f", toggleRelation );
moveMap.bind( keyboard, q, toggleMission ); 
moveMap.bind( keyboard, "alt q", toggleMission ); 
moveMap.bind( keyboard, s, toggleSkill );       
moveMap.bind( keyboard, "alt s", toggleSkill ); 
moveMap.bind( keyboard, t, toggleTeam ); 
moveMap.bind( keyboard, "alt t", toggleTeam );
moveMap.bind( keyboard, o, toggleSysMenu ); 
moveMap.bind( keyboard, "alt o", toggleSysMenu );
moveMap.bind( keyboard, x, togglePet ); 
moveMap.bind( keyboard, "alt x", togglePet );
moveMap.bind( keyboard, z, autoPickupItem );
moveMap.bind( keyboard, "enter", toggleChatInput );
moveMap.bind( keyboard, tab, toggleBigViewMap );
moveMap.bind( keyboard, pagedown, toggleCameraZoomIn );
moveMap.bind( keyboard, pageup, toggleCameraZoomOut );

moveMap.bind( keyboard, "F1", toggleSelfIcon );
moveMap.bind( keyboard, "F2", toggleTeamIcon1 );
moveMap.bind( keyboard, "F3", toggleTeamIcon2 );
moveMap.bind( keyboard, "F4", toggleTeamIcon3 );
moveMap.bind( keyboard, "F5", toggleTeamIcon4 );
moveMap.bind( keyboard, "F6", toggleTeamIcon5 );
moveMap.bind( keyboard, "F10", toggleHideGUIWndAll );
moveMap.bind( keyboard, "F11", toggleHideName );
// 快捷栏
function onShortCutSelect1(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(1);
}
function onShortCutSelect2(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(2);
}
function onShortCutSelect3(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(3);
}
function onShortCutSelect4(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(4);
}
function onShortCutSelect5(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(5);
}
function onShortCutSelect6(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(6);
}
function onShortCutSelect7(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(7);
}
function onShortCutSelect8(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(8);
}
function onShortCutSelect9(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(9);
}
function onShortCutSelect10(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(10);
}
function onShortCutSelect1A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(1);
}
function onShortCutSelect2A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(2);
}
function onShortCutSelect3A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(3);
}
function onShortCutSelect4A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(4);
}
function onShortCutSelect5A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(5);
}
function onShortCutSelect6A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(6);
}
function onShortCutSelect7A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(7);
}
function onShortCutSelect8A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(8);
}
function onShortCutSelect9A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(9);
}
function onShortCutSelect10A(%val)
{
	 if(%val == 0)   
	   return;
	 onShortCutPanel(10);
}

//包裹
function togglePack(%val)
{
	if(%val == 0)   
	   return;
	OpenBag();
}

//人物属性（基本信息）
function togglePersona(%val)
{
	if(%val == 0)
	  return;
	OpenOrClosePlayerInfo();
}
//社会关系
function toggleRelation(%val)
{
	if(%val == 0)
	  return;
	OpenOrCloseRelationWnd();
}
//任务
function toggleMission(%val)
{
	if(%val == 0)
	  return;
	OpenOrCloseMissionWnd();
}
//技能栏
function toggleSkill(%val)
{
	if(%val == 0)
	  return;
	OpenSkillWnd();
}

//队伍
function toggleTeam(%val)
{
	if(%val == 0)   
	   return;
	OpenTeamWnd();
}

//灵兽
function togglePet(%val)
{
	if(%val == 0)   
	   return;
	OpenOrClosePetInfoWnd();
}

//一键拾取   
function autoPickupItem(%val)
{
	if(%val == 0)   
	   return;
	SetAIState_Pickup();
}
//聊天输入/输出
function toggleChatInput(%val)
{
	if(%val == 0)   
	   return;
	   
	if(InputWnd.isFirstResponder())
	   InputWnd.OnKeyReturn();
	else
		InputWnd.makeFirstResponder(1);
}

//世界地图(当前地图)
function toggleBigViewMap(%val)
{
	if(%val == 0)
	 return;
	OpenBigViewMap();
}

function toggleCameraZoomIn(%val)
{
	if(%val == 0)
	 return;
	CameraZoomIn();
}

function toggleCameraZoomOut(%val)
{
	if(%val == 0)
	 return;
	CameraZoomOut();
}
//------------------------------------------------
//关闭子界面/打开系统界面
//------------------------------------------------

function toggleSysMenu(%val)
{
	if(%val==0)
	 return;
	 
	 OpenSystemMenuWnd();
}

function toggleSystemSetting()
{ 
	//clearAIPath();
	if(!ClearTempAction())
		return;
	if(!CloseAllGui())
		return;
}
//目标选中自己
function toggleSelfIcon(%val)
{
	 if(%val == 0)   
	   return;
	 selectSelfIcon();
}
//目标选中队友
function toggleTeamIcon1(%val)
{
	if(%val == 0)   
	   return;
	selectTeamIcon(0);
}
function toggleTeamIcon2(%val)
{
	if(%val == 0)   
	   return;
	selectTeamIcon(2);
}
function toggleTeamIcon3(%val)
{
	if(%val == 0)   
	   return;
	selectTeamIcon(2);
}
function toggleTeamIcon4(%val)
{
	if(%val == 0)   
	   return;
	selectTeamIcon(3);
}
function toggleTeamIcon5(%val)
{
	if(%val == 0)   
	   return;
	selectTeamIcon(4);
}
//隐藏所有界面窗口
function toggleHideGUIWndAll(%val)
{
	if(%val == 0)   
	   return;
 
	if($hideAllGuiWindow == false)
	   CloseAllGuiWindow();
	else 
	   ShowAllGuiWindow();
}
//隐藏名字
function toggleHideName(%val)
{
	if(%val == 0)   
	   return;
	   
	if(getDrawNameState())
	   setDrawNameState(false);
	else
	   setDrawNameState(true);
}

