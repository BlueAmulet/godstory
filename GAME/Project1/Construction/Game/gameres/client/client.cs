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

	 exec("~/gui/GameMainWndGui.gui");                  //游戏主界面

   // Load up the Game GUIs
   exec("~/gui/PlayGui.gui");


   // Load up the shell GUIs
   exec("~/gui/mainMenuGui.gui");
   exec("~/gui/loadingGui.gui");
   exec("~/gui/flashPlayer.gui");											//flash播放器	
   exec("~/gui/MiniKeyboardGui.gui");                 //小键盘界面
   exec("~/gui/ServerSelectGui.gui");                 //服务器选择界面
   exec("~/gui/CreatePlayerGui.gui");                 //创建人物界面
   exec("~/gui/DialogWndGui.gui");                    //信息提示框界面
   exec("~/gui/BottomControlWndGui.gui");             //底部控制栏
   exec("~/gui/BirdViewMapGui.gui");                  //小地图显示
   exec("~/gui/PlayerIconGui.gui");                   //显示主界面当前角色、目标、队友的ICON信息（头像血条蓝条等）
   exec("~/gui/SystemSettingGui.gui");                //系统设置
   exec("~/gui/ChatWndGui.gui");                      //显示聊天内容窗口
   exec("~/gui/ChatInputWndGui.gui");                 //聊天输入框
   exec("~/gui/ChangeChatInputChunnelGui.gui");       //聊天输入频道选择框
   exec("~/gui/ShoutWndGui.gui");                     //喊话窗口
   exec("~/gui/CloseChunnelGui.gui");                 //频道屏蔽
   exec("~/gui/BigViewMap.gui");                      //世界地图 
   //exec("~/gui/TeamWndGui.gui");                      //组队窗口
	 //exec("~/gui/Invertoty.gui");												//物品栏
	 exec("~/gui/TradeWndGui.gui");                     //交易界面
	 exec("~/gui/ShopWndGui.gui");                      //商店界面
	 exec("~/gui/BagWndGui.gui");                       //包裹
   exec("~/gui/NewTeamWndGui.gui");                   //队伍
   exec("~/gui/PickUpItemWndGui.gui");                //物品拾取
   exec("~/gui/MissionWndGui.gui");                   //任务界面
   exec("~/gui/MissionTraceGui.gui");                 //任务追踪界面
   exec("~/gui/MissionDialogWndGui.gui");             //任务对话窗口
   exec("~/gui/SkillWndGui.gui");                     //技能
   exec("~/gui/PlayerInfoGui.gui");                   //显示当前角色属性信息
   exec("~/gui/CurrencyMessageBoxGui.gui");           //通用消息对话框
   exec("~/gui/DestroySplitItemGui.gui");             //销毁拆分物品界面 
   exec("~/gui/MessageWnd.gui");             		      //屏幕消息框
   exec("~/gui/AreaTriggerGui.gui");                  //区域触发显示
   exec("~/gui/PetInfoGui.gui");                      //宠物信息
   exec("~/gui/EquipStrengthenWndGui.gui");           //装备强化界面
   exec("~/gui/EquipEmbedableListWnd.gui");           //可镶嵌列表界面
   exec("~/gui/EquipEmbedGemWnd.gui");                //装备镶嵌界面
   exec("~/gui/EquipPunchHole.gui");                	//装备打孔界面
   exec("~/gui/ShowBuffGui.gui");                     //显示BUFF
   exec("~/gui/LivingSkillWndGui.gui");               //生活技能
   exec("~/gui/BankWndGui.gui");                      //仓库界面
   exec("~/gui/RelationWndGui.gui");                  //关系界面
   exec("~/gui/StallWndGui.gui");                     //摆摊界面
   exec("~/gui/MessageBoxGui.gui");                   //信息盒子
   exec("~/gui/ShopHelpWndGui.gui");                  //售物助手
   exec("~/gui/FloatingShortCutGui.gui");             //浮动快捷栏
   exec("~/gui/guiVocalProgressBar.gui");             //吟唱进度条
   exec("~/gui/WalcomeWndGui.gui");                   //欢迎界面
   exec("~/gui/TimerTriggerCtrli.gui");               //时间触发器界面
   exec("~/gui/PopUpMenuWndGui.gui");                 //右键弹出菜单界面
   exec("~/gui/ResurgenceWndGui.gui");                //复活
   exec("~/gui/PetCheckupWndGui.gui");                //宠物鉴定
   exec("~/gui/PetStrengthenWndGui.gui");             //宠物强化
   exec("~/gui/PetDecompoundWndGui.gui");             //宠物炼化
   exec("~/gui/PetBackToYoungWndGui.gui");            //宠物还童
   exec("~/gui/PetHelpWndGui.gui");                   //宠物列表
   exec("~/gui/MailWndGui.gui");                      //邮件
   exec("~/gui/XianZhuanWndGui.gui");                 //仙篆录
   exec("~/gui/RankWndGui.gui");                      //排行榜
   exec("~/gui/FriendChatWndGui.gui");                //仙友聊天框
   exec("~/gui/BusinessCityWndGui.gui");              //商城
   exec("~/gui/SoulGeniusWndGui.gui");                //元神
   exec("~/gui/itemSplit.gui");                       //物品分解
   exec("~/gui/itemCompose.gui");                     //物品合成
   exec("~/gui/MPManagerList.gui");                   //师徒界面相关
   exec("~/gui/CheckCodeGui.gui");                    //验证界面
   exec("~/gui/HelpDirectWndGui.gui");                //帮助指引
   
   
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
   exec("./scripts/itemSplit.cs");                       //物品分解
   exec("./scripts/itemCompose.cs");                     //物品合成
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
   
   // 载入Datablock
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