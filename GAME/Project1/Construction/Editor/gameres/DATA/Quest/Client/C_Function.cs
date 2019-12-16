//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端函数脚本集，只在客户端使用，但函数名建议不与服务端函数使用相同名称
//==================================================================================


//■■■■■■■■■■■获取角色称呼■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■设置防骗防盗信息■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■设置玩家默认已经接受的任务数量■■■■■■■■■■■■■
//■■■■■■■■■■■NPC进入玩家视野触发，决定是否要顶？！特效■■■■■■■
//■■■■■■■■■■■客户端玩家升级调用■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■升级帮助界面内文字调用■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■客户端帮助界面显示■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■客户端收讯息脚本■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■客户端处理寻径按钮计数■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■获取角色称呼■■■■■■■■■■■■■■■■■■■■■■
function GetSexName(%Player)
{
	if(%Player.GetSex() == 1)
	   return "帅哥";
	else
	   return "靓女";
}

//获取屏幕倒计时器内的文字描述
function GetTimerInfo(%index, %type)
{
	switch(%type)
	{
		case 1:
			switch(%index)
      {
				case 1: return "您将获得第一份惊喜礼品";
				case 2: return "您将获得第二份惊喜礼品";
				case 3: return "您将获得第三份惊喜礼品";
				case 4: return "您将获得第四份惊喜礼品";
      }
			return "您将获得一份惊喜礼品";

		case 2:
			switch(%index)
			{
				case 1: return $Get_Dialog_GeShi[31210] @ "<t>恭喜您获得【</t>" @ $Get_Dialog_GeShi[31203] @ GetItemData(105100104,1) @ "</t><t>】，使用快捷键【Alt+E】查收邮件。</t>" @ $Get_Dialog_GeShi[31206] @ "15分钟</t><t>后您若仍然在线，将获得第二份礼物。</t>";
				case 2: return $Get_Dialog_GeShi[31210] @ "<t>恭喜您获得【</t>" @ $Get_Dialog_GeShi[31203] @ GetItemData(105100105,1) @ "</t><t>】，使用快捷键【Alt+E】查收邮件。</t>" @ $Get_Dialog_GeShi[31206] @ "30分钟</t><t>后您若仍然在线，将获得第三份礼物。</t>";
				case 3: return $Get_Dialog_GeShi[31210] @ "<t>恭喜您获得【</t>" @ $Get_Dialog_GeShi[31203] @ GetItemData(105100106,1) @ "</t><t>】，使用快捷键【Alt+E】查收邮件。</t>" @ $Get_Dialog_GeShi[31206] @ "60分钟</t><t>后您若仍然在线，将获得第四份礼物。</t>";
				case 4: return $Get_Dialog_GeShi[31210] @ "<t>恭喜您获得【</t>" @ $Get_Dialog_GeShi[31203] @ GetItemData(105100107,1) @ "</t><t>】，使用快捷键【Alt+E】查收邮件。</t>";
			}
			return "恭喜您获得了一份惊喜礼品";
	}
}
//■■■■■■■■■■■获取角色称呼■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■设置防骗防盗信息■■■■■■■■■■■■■■■■■■■■

//设置防骗防盗信息
$Fang_Pian_Fang_Dao = 0;
function Fang_Pian_Fang_Dao()
{
	$Fang_Pian_Fang_Dao = 1;

	%a = GetRandom(1,8);

	if(%a == 1){%Txt = "官方不会以私聊或者邮件方式告诉玩家中奖或者领奖信息，不会以任何理由向玩家索要帐号和密码。";}
	if(%a == 2){%Txt = "我们的系统公告全部都采用红色和橙色提示，信息前提都会加入[系统]频道的标志。";}
	if(%a == 3){%Txt = "请对采用普通会话频道的各种信息注意鉴别，当发现发布盗号网站者后，将其加入黑名单，将不会再受其信息骚扰！";}
	if(%a == 4){%Txt = "当获得 [资质未鉴定]类装备时，可以前往鉴定师处或采用[鉴定符]鉴定其属性。";}
	if(%a == 5){%Txt = "各种灵兽可以在[灵兽谷]处进行捕捉，是您游戏过程的最好帮助。";}
	if(%a == 6){%Txt = "积极的完成[门宗任务]可以让您在门宗中地位提高，可以购买一些特殊的门宗道具。";}
	if(%a == 7){%Txt = "当您游戏疲劳的时候，可以激活法宝的 [灵战] 状态，即可自动战斗。";}
	if(%a == 8){%Txt = "10级后升级需要手动升级，请关注自己的经验槽状态。";}

//	SetScreenMessage(%Txt, $Color[2]);
	SetChatMessage(%Txt, $Color[2]);

	Schedule(60000, 0, "Fang_Pian_Fang_Dao");
}

//■■■■■■■■■■■设置防骗防盗信息■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■设置玩家默认已经接受的任务数量■■■■■■■■■■■■■
//设置玩家默认已经接受的任务数量
//$Mid_Accepted_Num = 0;
//■■■■■■■■■■■设置玩家默认已经接受的任务数量■■■■■■■■■■■■■

//■■■■■■■■■■■NPC进入玩家视野触发，决定是否要顶？！特效■■■■■■■
//NPC进入玩家视野触发，决定是否要顶？！特效
function Player::inNpcScope(%Player, %Npc)
{
//	%NpcID = %Npc.GetDataID();
	%Effect = 0;

	%NpcA_Mid = $MissionData_NpcA[%Npc.GetDataID()];
	%NpcZ_Mid = $MissionData_NpcZ[%Npc.GetDataID()];

	//优先显示可交任务特效
	for(%z = 0; %z < %NpcZ_Mid; %z++)
	{
		%Mid = $MissionData_NpcZ[%Npc.GetDataID(), %z + 1];
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
			{
				%Effect = 4;
				break;
			}
	}

	//再显示接任务特效
	if(%Effect > 0)
		for(%a = 0; %a < %NpcA_Mid; %a++)
		{
			%Mid = $MissionData_NpcA[%Npc.GetDataID(), %a + 1];
			if(CanDoThisMission(%Player, %Mid, 1, 0, 0) $= "")
				{
					%Effect = 3;
					break;
				}
		}

	if(%Effect > 0)
		AddEffect(2, %Npc, $SP_Effect[ %Effect ]);	//发送接受任务特效
}
//■■■■■■■■■■■NPC进入玩家视野触发，决定是否要顶？！特效■■■■■■■

//■■■■■■■■■■■客户端玩家升级调用■■■■■■■■■■■■■■■■■■■
function ClientEventLevelUp(%Player)
{

}
//■■■■■■■■■■■客户端玩家升级调用■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■升级帮助界面内文字调用■■■■■■■■■■■■■■■■■
function OpenPlayerUpgradeHintWnd(%level,%text)
{

}
//■■■■■■■■■■■升级帮助界面内文字调用■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■客户端帮助界面显示■■■■■■■■■■■■■■■■■■■
//function OpenHelpDirectWnd(%aimCtrl,%type,%Horiz,%Vert,%txt,%time)
//{
//
//}
//■■■■■■■■■■■客户端帮助界面显示■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■客户端收讯息脚本■■■■■■■■■■■■■■■■■■■■
function ShowHelpDirectByIndex(%index)
{
	//指引标识相关
//OpenHelpDirectWnd(%aimCtrl,%type,%Horiz,%Vert,%txt,%time)
//%aimCtrl(要指向的控件名)
//%type(1 箭头左上  2 箭头右上 3 箭头右下 4 箭头左下)
//%Horiz (1 左偏移 2中  3右偏移)
//%Vert(1 上偏移 2中  3下偏移)
//%txt(要显示的内容,支持带解析)
//%time(关闭界面的时间,0为不关闭)
//例:OpenHelpDirectWnd("PlayerIconGui",1,3,3,"这里是显示个人信息的地方",8000);
//人物图像 -> ""PlayerIconGui""
//队友图像 -> ""TeammateIconGui""
//宠物图像 -> ""PetIconGui""
//目标图像 ->  ""TargetPlayerIconGui""
//底部控制栏-> ""BottomControlWndGui""
//小地图  -> ""BirdViewMapGui""
//聊天窗口 ->""ChatWndGuiAll""
//任务追踪 -> ""MissionTraceWnd""
//快捷按钮条 -> ""BottomControlWndGui1""

	switch(%index)
	{
		case 10100://追踪界面指引
			OpenHelpDirectWnd("MissionTraceWnd",3,1,1,"当任务的要求全部完成时，该任务内容全都会变成绿色！点击即可寻径至相应NPC处！打开任务界面（alt+q）你还可以查看任务信息！",8000);
	}
}
//■■■■■■■■■■■客户端收讯息脚本■■■■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■客户端处理寻径按钮计数■■■■■■■■■■■■■■■■■
function Get_TraceButton()
{
	$TraceButton = $TraceButton + 1;
	%Back = "TraceButton" @ $TraceButton;
	return %Back;
}
//■■■■■■■■■■■客户端处理寻径按钮计数■■■■■■■■■■■■■■■■■