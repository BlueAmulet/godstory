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
   for( %file = findFirstFile( "*/materials.cs" ); %file !$= ""; %file = findNextFile( "*/materials.cs" ))
   {
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

   exec("./ui/customProfiles.cs"); // override the base profiles if necessary
   exec("gameres/gui/Profiles.cs");
   // The common module provides basic client functionality
   initBaseClient();

   // Use our prefs to configure our Canvas/Window
   //configureCanvas();

   // Set the window title
   if (isObject(Canvas))
      Canvas.setWindowTitle(getEngineName() @ " (" @ getVersionString() @ ") - " @ $appName);
      
   /// Load client-side Audio Profiles/Descriptions
   exec("./scripts/audioProfiles.cs");

   // Load up the Game GUIs
   exec("./ui/defaultGameProfiles.cs");
   exec("./ui/PlayGui.gui");
   exec("./ui/ChatHud.gui");
   exec("./ui/playerList.gui");

   // Load up the shell GUIs
   exec("./ui/mainMenuGui.gui");
   exec("./ui/aboutDlg.gui");
   exec("./ui/startMissionGui.gui");
   exec("./ui/joinServerGui.gui");
   exec("./ui/endGameGui.gui");
   exec("./ui/loadingGui.gui");
   exec("./ui/optionsDlg.gui");
   exec("./ui/remapDlg.gui");
	 exec("./ui/guiObjectViewer.ed.gui");

   // Client scripts
   exec("./scripts/client.cs");
   exec("./scripts/game.cs");
   exec("./scripts/missionDownload.cs");
   exec("./scripts/serverConnection.cs");
   exec("./scripts/playerList.cs");
   exec("./scripts/loadingGui.cs");
   exec("./scripts/optionsDlg.cs");
   exec("./scripts/chatHud.cs");
   exec("./scripts/messageHud.cs");
   exec("./scripts/playGui.cs");
   exec("./scripts/centerPrint.cs");
   exec("./scripts/guiObjectViewer.ed.cs");

   // Load useful Materials
   exec("./scripts/glowBuffer.cs");
   exec("./scripts/shaders.cs");
   exec("./scripts/commonMaterialData.cs" );
   
   // Default player key bindings
   exec("./scripts/default.bind.cs");

   if (isFile("./config.cs"))
      exec("./config.cs");

   loadMaterials();
   
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
   //Canvas.setContent(startMissionGui);
   //OpenMissionFile();

   //loadDefaultMission();

   // Connect to server if requested.
   if ($JoinGameAddress !$= "") {
      connect($JoinGameAddress, "", $Pref::Player::Name);
   }
}


//-----------------------------------------------------------------------------
function loadDefaultMission()
{
   Canvas.setCursor("DefaultCursor");
   createServer( "SinglePlayer", expandFileName("~/data/missions/newMission.mis") );
   %conn = new GameConnection(ServerConnection);
   RootGroup.add(ServerConnection);
   %conn.setConnectArgs($pref::Player::Name);
   %conn.setJoinPassword($Client::Password);
   %conn.connectLocal();
}

function loadMainMenu()
{
   // Startup the client with the Main menu...
   if (isObject( MainMenuGui ))
      Canvas.setContent( MainMenuGui );
   else if (isObject( UnifiedMainMenuGui ))
      Canvas.setContent( UnifiedMainMenuGui );
   Canvas.setCursor("DefaultCursor");
}

//function OpenMissionFile()
//{
//	 echo("========================================");
//   %defaultFileName = $Server::MissionFile;
////   if( %defaultFileName $= "" )
////      %defaultFileName = expandFilename("~/data/missions/untitled.mis");
//
//   %dlg = new OpenFileDialog()
//   {
//      Filters        = $Pref::MissionEditor::FileSpec;
//      DefaultPath    = $Pref::MissionEditor::LastPath;
//      DefaultFile    = %defaultFileName;
//      ChangePath     = false;
//      MustExist      = true;
//   };
//         
//   %ret = %dlg.Execute();
//   if(%ret)
//   {
//      $Pref::MissionEditor::LastPath = filePath( %dlg.FileName );
//      %filename = %dlg.FileName;
//   }
//   
//   %dlg.delete();
//
//   if(! %ret)
//      return;
//
//   Canvas.setCursor("DefaultCursor");
//   createServer( "SinglePlayer", expandFileName(%filename) );
//   %conn = new GameConnection(ServerConnection);
//   RootGroup.add(ServerConnection);
//   %conn.setConnectArgs($pref::Player::Name);
//   %conn.setJoinPassword($Client::Password);
//   %conn.connectLocal();
//}
