//===========================================================================================================
// 文档说明:全局共用方法
// 创建时间:2009-09-17
// 创建人: daniel
//===========================================================================================================

$schInvite = 0;
// 公共邀请按钮 
function CommonInviteDialog(%eid)
{
	%player = GetPlayer();
	%type = %player.getLogicEventType(%eid);
	if(%type == 1)  		//邀请交易
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Trade.png");
	}
	else if(%type == 2)	//邀请组队
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Team.png");
	}
	else if(%type == 3) //邀请组队跟随
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Friend.png");
	}
	else if(%type == 6 )
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Friend.png");
	}
	else if(%type == 22 ) // 新邮件通知
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Friend.png");
	}
	
	AnimateButton.setvisible(1);
	AnimateButton.Command = "PopupInviteDialog(" @ %eid @ ");";
	if($schInvite != 0)
		cancel($schInvite);
	//5秒后隐藏公共邀请按钮
	$schInvite = schedule(5000, 0, "HideInviteButton", %type);
}

//弹出公共邀请对话框
function PopupInviteDialog(%eid)
{
	%player = GetPlayer();
	%type 	= %player.getLogicEventType(%eid);
	%sender = %player.getLogicEventSenderId(%eid);
	%who 		= %player.getLogicEventSenderName(%eid);
	%relId 	= %player.getLogicEventRID(%eid);
	%content= "";
	
	if($schInvite != 0)
	{
		cancel($schInvite);
		HideInviteButton(0);
	}
	
	if(%type == 1)			//邀请交易
	{
		%content = %who @ "向您发出交易请求";
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOK("接受", "agreeTrade(" @ %eid @ ");");
		Dialog_OK_Cancel.SetCancel("拒绝", "rejectTrade(" @ %eid @ ");");
		Dialog_OK_Cancel.setvisible(1);
	}
	
	if(%type == 2) 			// 邀请组队
	{
		%content = %who @ "向您发出组队邀请" ;
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOk("接受", "AgreeTeamInviteRequestById(" @ %sender @ ");" );
		Dialog_OK_Cancel.SetCancel("拒绝", "RefuseTeamInviteRequestById(" @ %sender @");" );
		Dialog_OK_Cancel.setVisible(1);
	}
	
		if(%type == 3) 		// 申请组队
	{
		%content = %who @ "向您发出组队申请" ;
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOk("接受", "AgreeTeamApplyRequestById(" @ %sender @ ");" );
		Dialog_OK_Cancel.SetCancel("拒绝", "DisagreeTeamApplyRequestById(" @ %sender @");" );
		Dialog_OK_Cancel.setVisible(1);
	}
	
	if( %type == 6 ) 		// 被加为好友
	{
			Dialog_OK_Content.setContent( "玩家["@ %who @"]加您为好友！" );
			Dialog_OnlyOK.setVisible( 1 );
	}
	
	if( %type == 22 )		// 通知新邮件
	{
		refreshMail();
		Dialog_OK_Content.setContent( "您有一封新邮件，请注意查收！" );
		Dialog_OnlyOK.setVisible( 1 );
	}
}

// 获取延迟事件的类型名
function GetLogicEventTypeName(%type)
{
	switch(%type)
	{
		case 1:		return "邀请交易";
		case 2: 	return "邀请组队";
		case 3: 	return "请求组队";
		case 4: 	return "邀请组队跟随";
		case 5:		return "请求组队跟随";
		case 6: 	return "邀请加入好友";
		case 7: 	return "请求加入好友";
		case 8: 	return "邀请加入帮会";
		case 9:		return "请求加入帮会";
		case 10: 	return "邀请结婚";
		case 11: 	return "邀请结拜";
		case 12: 	return "邀请切磋";
		case 13: 	return "邀请双人骑";
		case 14: 	return "请求双人骑";
		case 15: 	return "邀请传送";
		case 16: 	return "邀请副本";
		case 17: 	return "请求副本";
		case 18: 	return "邀请组团";
		case 19: 	return "邀请拜师";
		case 20:	return "邀请收徒";
		case 21: 	return "邀请商城购买";
		case 22: 	return "通知新邮件";
	}
	return "";
}

// 关于包裹栏物品的音效播放和在聊天栏的信息显示
function HandleItemSomething(%name, %flag, %oldnums, %newnums)
{
	if(%name !$= "")
		%name = "<t c='0x00ff00ff'>" @ %name @ "</t>";
	
	if(%flag == 2) 			// 购买
	{
		%text = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>您购买了【</t>" @ %name @"<t>】 × " @ (%newnums - %oldnums) @"</t></t>";
		AddChatMessage("13", %text, "0", "0");
		PlayBuyItemSound();
	}
	else if(%flag == 3)	// 出售
	{
		%text = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>您出售了【</t>" @ %name @"<t>】 × " @ (%oldnums - %newnums) @"</t></t>";
		AddChatMessage("13", %text, "0", "0");
		PlayBuyItemSound();
	}
	else if(%flag == 19)
	{
		%text = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>您鉴定【</t>" @ %name @"<t>】 成功 " @ "</t></t>";
		AddChatMessage("13", %text, "0", "0");
		//PlayBuyItemSound();
	}
	else if(%flag == 15)
	{
		%text = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>您修理了【</t>" @ %name @"<t>】 " @ "</t></t>";
		AddChatMessage("13", %text, "0", "0");
	}
	
	else if(%flag > 2)
	{
		if(%oldnums > %newnums)
			%text = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>您失去了【</t>" @ %name @"<t>】 × " @ (%oldnums - %newnums) @"</t></t>";
		else
			%text = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>您获得了【</t>" @ %name @"<t>】 × " @ (%newnums - %oldnums) @"</t></t>";
		AddChatMessage("13", %text, "0", "0");
		PlayUsedItemSound();
	}
}

function ShowCompleteMission(%missionid)
{
	%MissionData = "MissionData_" @ %missionid;
	%Mission_Name = "恭喜您完成【" @ %MissionData.Name @ "】任务";
	SetScreenMessage(%Mission_Name, 0xFFFF00);
}

// 在聊天框里显示金钱的个人消息
function ShowMoneyMessage(%type, %curmoney, %newmoney)
{
	%typeStr 		= "金元";
	%GoldImg 		= "gameres/gui/images/GUIWindow31_1_024.png";
	%SilverImg 	= "gameres/gui/images/GUIWindow31_1_025.png";
	%CopperImg	= "gameres/gui/images/GUIWindow31_1_026.png";
	if(%type == 1)
	{
		%typeStr 		= "灵元";
		%GoldImg 		= "gameres/gui/images/GUIWindow31_1_020.png";
		%SilverImg 	= "gameres/gui/images/GUIWindow31_1_021.png";
		%CopperImg	= "gameres/gui/images/GUIWindow31_1_022.png";
	}
	else if(%type == 2)
		%typeStr		= "仓库金元";
	
	%text = "";
	%dis = %curmoney - %newmoney;
	if(%dis > 0)
	{
		%text = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>您失去" @ %typeStr @ ": </t>";
	}
	else if(%dis < 0)
	{
		%text = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>您获得" @ %typeStr @ ": </t>";
		%dis = %newmoney - %curmoney;
	}
	
	if(%dis != 0)
	{
		%gold = mfloor(%dis / 10000);
		if(%gold != 0)
			%text = %text @ "<t>" @ %gold @ "</t><i s='" @ %GoldImg @ "' w='16' h='16'/>";
			
		%silver = mfloor((%dis - %gold * 10000) / 100);
		if(%silver != 0)
			%text = %text @ "<t>" @ %silver @ "</t><i s='" @ %SilverImg @ "' w='16' h='16'/>";
			
		%copper = %dis % 100;
		if(%copper != 0)
			%text = %text @ "<t>" @ %copper @ "</t><i s='" @ %CopperImg @ "' w='16' h='16'/>";
		%text = %text @ "</t>";
		AddChatMessage("13", %text, "0", "0");
	}
}

function uiRelationPrentice(%player)
{
	  %srcName = %player.getPlayerName();
	  %content = %srcName @ "向您发出交收徒请求";
	  %playerId = %player.getPlayerID();
	  
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOK("接受", "agreePrentice("@%playerId@");");
		Dialog_OK_Cancel.SetCancel("拒绝", "rejectRelation("@%playerId@");");
		Dialog_OK_Cancel.setvisible(1);
}

function uiRelationMaster(%player)
{
	  %srcName = %player.getPlayerName();
	  %content = %srcName @ "向您发出拜师请求";
	  %playerId = %player.getPlayerID();
	  
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOK("接受", "agreeMaster("@%playerId@");");
		Dialog_OK_Cancel.SetCancel("拒绝", "rejectRelation("@%playerId@");");
		Dialog_OK_Cancel.setvisible(1);
}