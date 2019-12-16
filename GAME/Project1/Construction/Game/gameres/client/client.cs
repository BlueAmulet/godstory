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

	 exec("~/gui/GameMainWndGui.gui");                  //��Ϸ������

   // Load up the Game GUIs
   exec("~/gui/PlayGui.gui");


   // Load up the shell GUIs
   exec("~/gui/mainMenuGui.gui");
   exec("~/gui/loadingGui.gui");
   exec("~/gui/flashPlayer.gui");											//flash������	
   exec("~/gui/MiniKeyboardGui.gui");                 //С���̽���
   exec("~/gui/ServerSelectGui.gui");                 //������ѡ�����
   exec("~/gui/CreatePlayerGui.gui");                 //�����������
   exec("~/gui/DialogWndGui.gui");                    //��Ϣ��ʾ�����
   exec("~/gui/BottomControlWndGui.gui");             //�ײ�������
   exec("~/gui/BirdViewMapGui.gui");                  //С��ͼ��ʾ
   exec("~/gui/PlayerIconGui.gui");                   //��ʾ�����浱ǰ��ɫ��Ŀ�ꡢ���ѵ�ICON��Ϣ��ͷ��Ѫ�������ȣ�
   exec("~/gui/SystemSettingGui.gui");                //ϵͳ����
   exec("~/gui/ChatWndGui.gui");                      //��ʾ�������ݴ���
   exec("~/gui/ChatInputWndGui.gui");                 //���������
   exec("~/gui/ChangeChatInputChunnelGui.gui");       //��������Ƶ��ѡ���
   exec("~/gui/ShoutWndGui.gui");                     //��������
   exec("~/gui/CloseChunnelGui.gui");                 //Ƶ������
   exec("~/gui/BigViewMap.gui");                      //�����ͼ 
   //exec("~/gui/TeamWndGui.gui");                      //��Ӵ���
	 //exec("~/gui/Invertoty.gui");												//��Ʒ��
	 exec("~/gui/TradeWndGui.gui");                     //���׽���
	 exec("~/gui/ShopWndGui.gui");                      //�̵����
	 exec("~/gui/BagWndGui.gui");                       //����
   exec("~/gui/NewTeamWndGui.gui");                   //����
   exec("~/gui/PickUpItemWndGui.gui");                //��Ʒʰȡ
   exec("~/gui/MissionWndGui.gui");                   //�������
   exec("~/gui/MissionTraceGui.gui");                 //����׷�ٽ���
   exec("~/gui/MissionDialogWndGui.gui");             //����Ի�����
   exec("~/gui/SkillWndGui.gui");                     //����
   exec("~/gui/PlayerInfoGui.gui");                   //��ʾ��ǰ��ɫ������Ϣ
   exec("~/gui/CurrencyMessageBoxGui.gui");           //ͨ����Ϣ�Ի���
   exec("~/gui/DestroySplitItemGui.gui");             //���ٲ����Ʒ���� 
   exec("~/gui/MessageWnd.gui");             		      //��Ļ��Ϣ��
   exec("~/gui/AreaTriggerGui.gui");                  //���򴥷���ʾ
   exec("~/gui/PetInfoGui.gui");                      //������Ϣ
   exec("~/gui/EquipStrengthenWndGui.gui");           //װ��ǿ������
   exec("~/gui/EquipEmbedableListWnd.gui");           //����Ƕ�б����
   exec("~/gui/EquipEmbedGemWnd.gui");                //װ����Ƕ����
   exec("~/gui/EquipPunchHole.gui");                	//װ����׽���
   exec("~/gui/ShowBuffGui.gui");                     //��ʾBUFF
   exec("~/gui/LivingSkillWndGui.gui");               //�����
   exec("~/gui/BankWndGui.gui");                      //�ֿ����
   exec("~/gui/RelationWndGui.gui");                  //��ϵ����
   exec("~/gui/StallWndGui.gui");                     //��̯����
   exec("~/gui/MessageBoxGui.gui");                   //��Ϣ����
   exec("~/gui/ShopHelpWndGui.gui");                  //��������
   exec("~/gui/FloatingShortCutGui.gui");             //���������
   exec("~/gui/guiVocalProgressBar.gui");             //����������
   exec("~/gui/WalcomeWndGui.gui");                   //��ӭ����
   exec("~/gui/TimerTriggerCtrli.gui");               //ʱ�䴥��������
   exec("~/gui/PopUpMenuWndGui.gui");                 //�Ҽ������˵�����
   exec("~/gui/ResurgenceWndGui.gui");                //����
   exec("~/gui/PetCheckupWndGui.gui");                //�������
   exec("~/gui/PetStrengthenWndGui.gui");             //����ǿ��
   exec("~/gui/PetDecompoundWndGui.gui");             //��������
   exec("~/gui/PetBackToYoungWndGui.gui");            //���ﻹͯ
   exec("~/gui/PetHelpWndGui.gui");                   //�����б�
   exec("~/gui/MailWndGui.gui");                      //�ʼ�
   exec("~/gui/XianZhuanWndGui.gui");                 //��׭¼
   exec("~/gui/RankWndGui.gui");                      //���а�
   exec("~/gui/FriendChatWndGui.gui");                //���������
   exec("~/gui/BusinessCityWndGui.gui");              //�̳�
   exec("~/gui/SoulGeniusWndGui.gui");                //Ԫ��
   exec("~/gui/itemSplit.gui");                       //��Ʒ�ֽ�
   exec("~/gui/itemCompose.gui");                     //��Ʒ�ϳ�
   exec("~/gui/MPManagerList.gui");                   //ʦͽ�������
   exec("~/gui/CheckCodeGui.gui");                    //��֤����
   exec("~/gui/HelpDirectWndGui.gui");                //����ָ��
   
   
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
   exec("./scripts/itemSplit.cs");                       //��Ʒ�ֽ�
   exec("./scripts/itemCompose.cs");                     //��Ʒ�ϳ�
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
   
   // ����Datablock
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