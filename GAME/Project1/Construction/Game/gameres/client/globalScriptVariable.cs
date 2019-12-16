//===========================================================================================================
// 文档说明:全局变量定义
// 创建时间:2009-07-08
// 创建人: thinking
//=========================================================================================================== 
//界面脚本
$GUI::GUIDirectory = "gameres/gui/";
//界面图片
$GUI::GUISkinDirectory = $GUI::GUIDirectory @ "images/";
//字体目录
$GUI::FontCacheDirectory = "fonts/";
//隐藏所有窗口
$hideAllGuiWindow = false;

$createPlayer_Sex = 0;
$createPlayer_Data = 0;
//$createPlayer_Face = 0;
//$createPlayer_Hair = 0;
$createPlayer_Apparel = 0;
//人物脸型_男
$PlayerFaceShape_Boy[0] = 21091;
$PlayerFaceShape_Boy[1] = 21092; 
$PlayerFaceShape_Boy[2] = 21093;
$PlayerFaceShape_Boy[3] = 21094;
$PlayerFaceShape_Boy[4] = 21095;
$PlayerFaceShape_Boy[5] = 21096;

//人物脸型名称_男
$PlayerFaceName_Boy[0] = "五官1";
$PlayerFaceName_Boy[1] = "五官2";
$PlayerFaceName_Boy[2] = "五官3";
$PlayerFaceName_Boy[3] = "五官4";
$PlayerFaceName_Boy[4] = "五官5";
$PlayerFaceName_Boy[5] = "五官6";
			
//人物脸型_女
$PlayerFaceShape_Girl[0] = 21591;
$PlayerFaceShape_Girl[1] = 21592;
$PlayerFaceShape_Girl[2] = 21593;
$PlayerFaceShape_Girl[3] = 21594;
$PlayerFaceShape_Girl[4] = 21595;
$PlayerFaceShape_Girl[5] = 21596;

//人物脸型名称_女
$PlayerFaceName_Girl[0] = "五官1";
$PlayerFaceName_Girl[1] = "五官2";
$PlayerFaceName_Girl[2] = "五官3";
$PlayerFaceName_Girl[3] = "五官4";
$PlayerFaceName_Girl[4] = "五官5";
$PlayerFaceName_Girl[5] = "五官6";
	
//人物发型名称_男
$PlayerHairName_Boy[0] = "发型1";
$PlayerHairName_Boy[1] = "发型2";
$PlayerHairName_Boy[2] = "发型3";
$PlayerHairName_Boy[3] = "发型4";
$PlayerHairName_Boy[4] = "发型5";
$PlayerHairName_Boy[5] = "发型6";
		
//人物发型名称_女
$PlayerHairName_Girl[0] = "发型1";
$PlayerHairName_Girl[1] = "发型2";
$PlayerHairName_Girl[2] = "发型3";
$PlayerHairName_Girl[3] = "发型4";
$PlayerHairName_Girl[4] = "发型5";
$PlayerHairName_Girl[5] = "发型6";	

//人物发型_男
$PlayerHairShape_Boy[0] = 31091;
$PlayerHairShape_Boy[1] = 31092;
$PlayerHairShape_Boy[2] = 31093;
$PlayerHairShape_Boy[3] = 31094;
$PlayerHairShape_Boy[4] = 31095;
$PlayerHairShape_Boy[5] = 31096;

//人物发型_女
$PlayerHairShape_Girl[0] = 31591;
$PlayerHairShape_Girl[1] = 31592;
$PlayerHairShape_Girl[2] = 31593;
$PlayerHairShape_Girl[3] = 31594;
$PlayerHairShape_Girl[4] = 31595;
$PlayerHairShape_Girl[5] = 31596;

//人物发色
$PlayerHairColorShape[0] = "a";
$PlayerHairColorShape[1] = "b";
$PlayerHairColorShape[2] = "c";
$PlayerHairColorShape[3] = "d";
$PlayerHairColorShape[4] = "e";
$PlayerHairColorShape[5] = "f";

//人物发色名称
$PlayerHairColorName[0] = "发色1";
$PlayerHairColorName[1] = "发色2";
$PlayerHairColorName[2] = "发色3";
$PlayerHairColorName[3] = "发色4";
$PlayerHairColorName[4] = "发色5";
$PlayerHairColorName[5] = "发色6";

//人物身体_男								   
$PlayerBodyShape_Boy[0] = "ply01_001_3";
$PlayerBodyShape_Boy[1] = "ply01_002_3";
$PlayerBodyShape_Boy[2] = "ply01_003_3";	
//人物服装_男								   
$PlayerApparelShape_Boy[0] = "102029001";
$PlayerApparelShape_Boy[1] = "102029002";
$PlayerApparelShape_Boy[2] = "102029003";	
//人物服装名称_男	                                       
$PlayerApparelName_Boy[0] = "服装1";
$PlayerApparelName_Boy[1] = "服装2";
$PlayerApparelName_Boy[2] = "服装3";
//人物身体_女
$PlayerBodyShape_Girl[0] = "ply02_001_3";
$PlayerBodyShape_Girl[1] = "ply02_002_3";
$PlayerBodyShape_Girl[2] = "ply02_003_3";	
//人物服装_女								   
$PlayerApparelShape_Girl[0] = "102029004";
$PlayerApparelShape_Girl[1] = "102029005";
$PlayerApparelShape_Girl[2] = "102029006";	                                     
//人物服装名称_女                                       
$PlayerApparelName_Girl[0] = "服装1";
$PlayerApparelName_Girl[1] = "服装2";
$PlayerApparelName_Girl[2] = "服装3";		

$FirstFaceNum = 0;			
$FirstHairNum = 0;
$FirstHColorNum = 0;
$FirstApparelNum = 0;			 

$FaceMaxNum = 5;
$HairMaxNum = 5;
$HColorMaxNum = 5;
$ApparelMaxNum = 2;
  
//使用的模型类型
$UseShapeType = 0;
//外形图片页数
$ShapeImageNum = 0;
$ShapeImageMaxNum = 0;
$LivingCurrentSelectSeries = 0;                  //选中类生活技能
$SelectPrescriptionID = 0;                       //选中项配方ID
$DoingTreeItemId =0;                             //正在制造配方的树ItemId

$SocialFindPlayerAction = 0;
//自动售卖
$AutoSellSlots	= 0;
$AutoListSelectedIndex = -1;

//统一关闭界面方法
$IsDirty = 0;
$CloseNum = 0;
$CloseData[$CloseNum]   = "ClosePlayerInfo();";                        //人物信息界面
$CloseData[$CloseNum++] = "CloseAllBag();";                            //背包界面_合包
$CloseData[$CloseNum++] = "CloseSingleBag();";                         //背包界面_分包
$CloseData[$CloseNum++] = "CloseMissionWnd();";                        //任务界面
$CloseData[$CloseNum++] = "ClosePetInfoWnd();";                        //宠物界面
$CloseData[$CloseNum++] = "CloseRelationWnd();";                       //关系界面
$CloseData[$CloseNum++] = "CloseSeeInfoWnd();";                        //关系界面_查看好友信息
$CloseData[$CloseNum++] = "CloseFriendCheerWnd();";                    //关系界面_加油
$CloseData[$CloseNum++] = "CloseSettingWnd();";                        //关系界面_设置
$CloseData[$CloseNum++] = "CloseFriendHintWnd();";                     //关系界面_添加好友
$CloseData[$CloseNum++] = "CloseStallWnd();";                          //摆摊界面
$CloseData[$CloseNum++] = "CloseStallInfoWnd();";                      //摆摊界面_摊位信息
$CloseData[$CloseNum++] = "CloseLookupWnd();";                         //摆摊界面_查找物品
$CloseData[$CloseNum++] = "CloseLookupReturnWnd();";                   //摆摊界面_查找物品返回信息
$CloseData[$CloseNum++] = "CloseChangePriceWnd();";                    //摆摊界面_设置价格
$CloseData[$CloseNum++] = "CloseSetStallWnd();";                       //摆摊界面_设置摊位
$CloseData[$CloseNum++] = "CloseSetTradeItemWnd();";                   //摆摊界面_设置购买or收购价格
$CloseData[$CloseNum++] = "CloseStallSeeWnd();";                       //摆摊界面_查看目标摊位
$CloseData[$CloseNum++] = "CloseBankWnd();";                           //仓库界面
$CloseData[$CloseNum++] = "CloseBankWnd_InputWnd();";                  //仓库界面_存取界面
$CloseData[$CloseNum++] = "CloseTeamWnd();";                           //队伍界面
$CloseData[$CloseNum++] = "CloseTeamInfoWnd();";                       //队伍信息界面
$CloseData[$CloseNum++] = "CloseInviteJoinTeamWnd();";                 //队伍界面_邀请组队
$CloseData[$CloseNum++] = "CloseSkillWnd();";                          //技能界面
$CloseData[$CloseNum++] = "CloseSkillSelect();";                       //技能界面_技能系选择
$CloseData[$CloseNum++] = "CloseSkillStudy();";                        //技能界面_技能学习
$CloseData[$CloseNum++] = "onCancelIdentify();";                       //物品鉴定
$CloseData[$CloseNum++] = "cancelSplit();";                            //物品拆分
$CloseData[$CloseNum++] = "cancelDestroyItem();";                      //物品销毁
$CloseData[$CloseNum++] = "CloseBigViewMap();";                        //当前地图
//$CloseData[$CloseNum++] = "StopFindPath();";                           //寻径提示框
$CloseData[$CloseNum++] = "CloseSysSettingWnd();";                     //系统设置界面
$CloseData[$CloseNum++] = "CloseNpcShopWnd();";                        //商店界面
$CloseData[$CloseNum++] = "CloseShopHelpWnd();";                       //售物助手界面
$CloseData[$CloseNum++] = "CloseShopWnd_BatchSellWnd();";              //批量购买界面
$CloseData[$CloseNum++] = "CloseShopWnd_NotarizeWnd();";               //确定购买界面
$CloseData[$CloseNum++] = "CloseTradeWnd();";                          //交易界面
$CloseData[$CloseNum++] = "CloseTradeHelpWnd();";                      //交易帮助界面
$CloseData[$CloseNum++] = "CloseTradeNoteWnd();";                      //交易记录界面
$CloseData[$CloseNum++] = "CloseTradeInputMoneyWnd();";                //交易界面_输入对话框
$CloseData[$CloseNum++] = "CloseNpcDialog();";                         //NPC对话框
$CloseData[$CloseNum++] = "EquipEmbedGemWnd_CloseWnd();";              //装备镶嵌界面
$CloseData[$CloseNum++] = "EquipPunchHoleWnd_Close();";                //装备打孔界面
$CloseData[$CloseNum++] = "EquipStrengthenWnd_Close();";        			 //装备强化界面
$CloseData[$CloseNum++] = "CloseDialog_OK_Cancel();";        			     //弹出对话框
$CloseData[$CloseNum++] = "CloseMisssionHover();";        			       //任务追踪界面热感
$CloseData[$CloseNum++] = "CloseStudyLivingSkillWnd();";               //生活技能_学习
$CloseData[$CloseNum++] = "ClosePrescription();";                      //生活技能_制造
$CloseData[$CloseNum++] = "ClosePetInfoGui_StudyWnd();";               //宠物修行界面
$CloseData[$CloseNum++] = "ClsePetCheckupWnd();";                      //宠物鉴定界面
$CloseData[$CloseNum++] = "ClsePetStrengthenWnd();";                   //宠物强化界面
$CloseData[$CloseNum++] = "ClsePetDecompoundWnd();";                   //宠物炼化界面
$CloseData[$CloseNum++] = "ClsePetHelpWnd();";                         //宠物列表界面
$CloseData[$CloseNum++] = "ClosePetBackToYoungWnd();";                 //灵兽还童
$CloseData[$CloseNum++] = "CloseMailWnd();";                           //邮件界面
$CloseData[$CloseNum++] = "ClosePlayerUpgradeHintWnd();";              //人物升级提示
$CloseData[$CloseNum++] = "CloseXianZhuanWnd();";                      //仙篆录
$CloseData[$CloseNum++] = "CloseRankWnd();";                           //排行榜
$CloseData[$CloseNum++] = "CloseSystemMenu();";                        //系统菜单
$CloseData[$CloseNum++] = "CloseBussnessCity();";                      //商城界面
$CloseData[$CloseNum++] = "CloseSoulGeniusWnd();";                     //元神信息界面

$ScheduleEventId = 0;      //调度事件ID
$pitchOnIndex = -1;				 //摆摊栏选中index

$RightDownType = 0;        //右键弹出菜单类型