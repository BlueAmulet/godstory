//-----------------------------------------------------------------------------

// Variables used by client scripts & code.  The ones marked with (c)
// are accessed from code.  Variables preceeded by Pref:: are client
// preferences and stored automatically in the ~/client/prefs.cs file
// in between sessions.
//
//    (c) Client::MissionFile             Mission file name
//    ( ) Client::Password                Password for server join

//    (?) Pref::Player::CurrentFOV
//    (?) Pref::Player::DefaultFov
//    ( ) Pref::Input::KeyboardTurnSpeed

//    (c) pref::Master[n]                 List of master servers
//    (c) pref::Net::RegionMask     
//    (c) pref::Client::ServerFavoriteCount
//    (c) pref::Client::ServerFavorite[FavoriteCount]
//    .. Many more prefs... need to finish this off

// Moves, not finished with this either...
//    (c) firstPerson
//    $mv*Action...

//-----------------------------------------------------------------------------
// These are variables used to control the shell scripts and
// can be overriden by mods:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// loadMaterials - load all materials.cs files
//-----------------------------------------------------------------------------
function loadMaterials()
{
   echo("\n--------- loadMaterials  ---------");
	
   for( %file = findFirstFile( "*/materials.cs" ); %file !$= ""; %file = findNextFile( "*/materials.cs" ))
   {
   	  echo("\n---------begin loadMaterials "@ %file @" ---------");
      exec( %file );
   }
   
   for( %file = findFirstFile( "*/materials.cmp" ); %file !$= ""; %file = findNextFile( "*/materials.cmp" ))
   {
   	  echo("\n---------begin loadMaterials "@ %file @" ---------");
      exec( %file );
   }   
}

function reloadMaterials()
{
   reloadTextures();
   loadMaterials();
   reInitMaterials();
}

//-----------------------------------------------------------------------------
function initClient()
{
   echo("\n--------- Initializing " @ $appName @ ": Client Scripts ---------");

   // Make sure this variable reflects the correct state.
   $Server::Dedicated = false;

   // Game information used to query the master server
   $Client::GameTypeQuery = $appName;
   $Client::MissionTypeQuery = "Any";

   exec("./mission.cs");
   exec("./missionDownload.cs");
   exec("./serverConnection.cs");
	 


   // Use our prefs to configure our Canvas/Window
   // Set the window title
   if (isObject(Canvas))
      Canvas.setWindowTitle($appName SPC GetCustomGameVersion());
      
   /// Load client-side Audio Profiles/Descriptions
   exec("./scripts/audioProfiles.cs");

	 exec("~/gui/GameMainWndGui.gui");                  //??????????

   // Load up the Game GUIs
   exec("~/gui/PlayGui.gui");


   // Load up the shell GUIs
   exec("~/gui/mainMenuGui.gui");
   exec("~/gui/loadingGui.gui");
   exec("~/gui/flashPlayer.gui");											//flash??????	
   exec("~/gui/MiniKeyboardGui.gui");                 //??????????
   exec("~/gui/ServerSelectGui.gui");                 //??????????????
   exec("~/gui/CreatePlayerGui.gui");                 //????????????
   exec("~/gui/DialogWndGui.gui");                    //??????????????
   exec("~/gui/BottomControlWndGui.gui");             //??????????
   exec("~/gui/BirdViewMapGui.gui");                  //??????????
   exec("~/gui/PlayerIconGui.gui");                   //????????????????????????????????ICON??????????????????????
   exec("~/gui/SystemSettingGui.gui");                //????????
   exec("~/gui/ChatWndGui.gui");                      //????????????????
   exec("~/gui/ChatInputWndGui.gui");                 //??????????
   exec("~/gui/ChangeChatInputChunnelGui.gui");       //??????????????????
   exec("~/gui/ShoutWndGui.gui");                     //????????
   exec("~/gui/CloseChunnelGui.gui");                 //????????
   exec("~/gui/BigViewMap.gui");                      //???????? 
   //exec("~/gui/TeamWndGui.gui");                      //????????
	 //exec("~/gui/Invertoty.gui");												//??????
	 exec("~/gui/TradeWndGui.gui");                     //????????
	 exec("~/gui/ShopWndGui.gui");                      //????????
	 exec("~/gui/BagWndGui.gui");                       //????
   exec("~/gui/NewTeamWndGui.gui");                   //????
   exec("~/gui/PickUpItemWndGui.gui");                //????????
   exec("~/gui/MissionWndGui.gui");                   //????????
   exec("~/gui/MissionTraceGui.gui");                 //????????????
   exec("~/gui/MissionDialogWndGui.gui");             //????????????
   exec("~/gui/SkillWndGui.gui");                     //????
   exec("~/gui/PlayerInfoGui.gui");                   //????????????????????
   exec("~/gui/CurrencyMessageBoxGui.gui");           //??????????????
   exec("~/gui/DestroySplitItemGui.gui");             //???????????????? 
   exec("~/gui/MessageWnd.gui");             		      //??????????
   exec("~/gui/AreaTriggerGui.gui");                  //????????????
   exec("~/gui/PetInfoGui.gui");                      //????????
   exec("~/gui/EquipStrengthenWndGui.gui");           //????????????
   exec("~/gui/EquipEmbedableListWnd.gui");           //??????????????
   exec("~/gui/EquipEmbedGemWnd.gui");                //????????????
   exec("~/gui/EquipPunchHole.gui");                	//????????????
   exec("~/gui/ShowBuffGui.gui");                     //????BUFF
   exec("~/gui/LivingSkillWndGui.gui");               //????????
   exec("~/gui/BankWndGui.gui");                      //????????
   exec("~/gui/RelationWndGui.gui");                  //????????
   exec("~/gui/StallWndGui.gui");                     //????????
   exec("~/gui/MessageBoxGui.gui");                   //????????
   exec("~/gui/ShopHelpWndGui.gui");                  //????????
   exec("~/gui/FloatingShortCutGui.gui");             //??????????
   exec("~/gui/guiVocalProgressBar.gui");             //??????????
   exec("~/gui/WalcomeWndGui.gui");                   //????????
   exec("~/gui/TimerTriggerCtrli.gui");               //??????????????
   exec("~/gui/PopUpMenuWndGui.gui");                 //????????????????
   exec("~/gui/ResurgenceWndGui.gui");                //????
   exec("~/gui/PetCheckupWndGui.gui");                //????????
   exec("~/gui/PetStrengthenWndGui.gui");             //????????
   exec("~/gui/PetDecompoundWndGui.gui");             //????????
   exec("~/gui/PetBackToYoungWndGui.gui");            //????????
   exec("~/gui/PetHelpWndGui.gui");                   //????????
   exec("~/gui/MailWndGui.gui");                      //????
   exec("~/gui/XianZhuanWndGui.gui");                 //??????
   exec("~/gui/RankWndGui.gui");                      //??????
   exec("~/gui/FriendChatWndGui.gui");                //??????????
   exec("~/gui/BusinessCityWndGui.gui");              //????
   exec("~/gui/SoulGeniusWndGui.gui");                //????
   exec("~/gui/itemSplit.gui");                       //????????
   exec("~/gui/itemCompose.gui");                     //????????
   exec("~/gui/MPManagerList.gui");                   //????????????
   exec("~/gui/CheckCodeGui.gui");                    //????????
   exec("~/gui/HelpDirectWndGui.gui");                //????????
   
   
   // Client scripts
   exec("./scripts/GameMainWnd.cs");
   exec("./scripts/actionMap.cs");
   exec("./scripts/game.cs");
   exec("./scripts/loadingGui.cs");
   exec("./scripts/playGui.cs");
   exec("./scripts/mainMenu.cs");
   exec("./scripts/MiniKeyboard.cs");
   exec("./scripts/ServerSelect.cs");
   exec("./scripts/CreatePlayer.cs");
   exec("./scripts/DialogWnd.cs");
   exec("./scripts/DestroySplitItemGui.cs");
   exec("./scripts/BottomControlWnd.cs");
   exec("./scripts/BirdViewMap.cs");
   exec("./scripts/PlayerIcon.cs");
   exec("./scripts/SystemSetting.cs");
   exec("./scripts/ChatWnd.cs");
   exec("./scripts/ChatInputWnd.cs");
   exec("./scripts/ChangeChatInputChunnel.cs");
   exec("./scripts/ShoutWnd.cs");
   exec("./scripts/CloseChunnel.cs");
   //exec("./scripts/TeamWnd.cs");
   exec("./scripts/TradeWnd.cs");
   exec("./scripts/ShopWnd.cs");
   exec("./scripts/BagWnd.cs");
   exec("./scripts/NewTeamWnd.cs"); 
   exec("./scripts/MissionWnd.cs");
   exec("./scripts/MissionTrace.cs");
   exec("./scripts/SkillWnd.cs");
   exec("./scripts/PlayerInfo.cs");
   exec("./scripts/BigViewMap.cs");
   exec("./scripts/MessageWnd.cs");
   exec("./scripts/MessageBoxGui.cs");
   exec("./scripts/PlayAllGuiSound.cs");
   exec("./scripts/PetInfo.cs");
   exec("./scripts/ShowBuff.cs");
   exec("./scripts/BankWnd.cs");
   exec("./scripts/RelationWnd.cs");
   exec("./scripts/StallWnd.cs");
   exec("./scripts/ShopHelpWnd.cs");
   exec("./scripts/LivingSkill.cs");
   exec("./scripts/VocalEvent.cs");
   exec("./scripts/FloatingShortCut.cs");
   exec("./scripts/WalcomeWnd.cs");
   exec("./scripts/PopUpMenuWnd.cs");
   exec("./scripts/ResurgenceWnd.cs");
   exec("./scripts/PetCheckupWnd.cs");
   exec("./scripts/PetStrengthenWnd.cs");
   exec("./scripts/PetDecompoundWnd.cs");
   exec("./scripts/PetBackToYoungWnd.cs");
   exec("./scripts/PetHelpWnd.cs");
   exec("./scripts/MailWnd.cs");
   exec("./scripts/XianZhuanWnd.cs");
   exec("./scripts/RankWnd.cs");
   exec("./scripts/FriendChatWnd.cs");
   exec("./scripts/BusinessCityWnd.cs"); 
   exec("./scripts/SoulGeniusWnd.cs");
   exec("./scripts/itemSplit.cs");                       //????????
   exec("./scripts/itemCompose.cs");                     //????????
   exec("./scripts/MPManagerList.cs");
   exec("./scripts/HelpDirectWnd.cs");
   
   // Load useful Materials
   exec("./scripts/glowBuffer.cs");
   //exec("./scripts/shaders.cs");
   exec("./scripts/commonMaterialData.cs" );
   
   // Default player key bindings
   exec("./scripts/default.bind.cs");

   if (isFile("./config.cs"))
      exec("./config.cs");

   loadMaterials();
   
   // ????Datablock
   clientCmdLoadDataBlock();
   
   // Really shouldn't be starting the networking unless we are
   // going to connect to a remote server, or host a multi-player
   // game.
   setNetPort(0);

   // Copy saved script prefs into C++ code.
   setDefaultFov( $pref::Player::defaultFov );
   setZoomSpeed( $pref::Player::zoomSpeed );

   // Start up the main menu... this is separated out into a 
   // method for easier mod override.
   loadMainMenu();
   // Connect to server if requested.
   //if ($JoinGameAddress !$= "") {
   //   connect($JoinGameAddress, "", $Pref::Player::Name);
   //}

   //if ($JoinGameAddress !$= "") {
   //   ConnectToAccountServer($JoinGameAddress);
   //}
}

function clientCmdLoadDataBlock()
{
   exec("./scripts/datablock.cs");
}

//-----------------------------------------------------------------------------
function onTick()
{
	BackToMainMenu();
}

//-----------------------------------------------------------------------------
function loadMainMenu()
{
   // Startup the client with the Main menu...
   if (isObject( MainMenuGui ))
   {
   	Canvas.setContent( MainMenuGui );   	
   }  
   Canvas.setCursor("DefaultCursor");
   PlayerID.setText($pref::Player::Name);
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