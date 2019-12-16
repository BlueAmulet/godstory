//===========================================================================================================
// 文档说明:关系界面操作
// 创建时间:2009-9-14
// 创建人: soar
//=========================================================================================================== 

$theListType = 1;

function OpenOrCloseRelationWnd()
{
	if(RelationWndGui.isvisible())
	{
		RelationWndGui.setvisible(0);
		$IsDirty--;
	}
	else
	{
		GameMainWndGui_Layer3.pushToBack(RelationWndGui);
		RelationWndGui.setvisible(1);
		SptSocialRequest();
		$IsDirty++;
	}
}

function OpenRelationWnd()
{
	GameMainWndGui_Layer3.pushToBack(RelationWndGui);
	RelationWndGui.setvisible(1);
	$IsDirty++;
	SptSocialRequest();
}

function CloseRelationWnd()
{
	RelationWndGui.setvisible(0);
	$IsDirty--;
}

function OpenOrCloseSeeInfoWnd()
{
	if(SeeInfoWnd.isvisible())
	{
		SeeInfoWnd.setvisible(0);
		$IsDirty--;
	}
	else
	{
		GameMainWndGui_Layer4.pushToBack(SeeInfoWnd);
		SeeInfoWnd.setvisible(1);
		$IsDirty++;
	}
}

function OpenSeeInfoWnd()
{
	GameMainWndGui_Layer4.pushToBack(SeeInfoWnd);
	SeeInfoWnd.setvisible(1);
	$IsDirty++;
}

function CloseSeeInfoWnd()
{
	SeeInfoWnd.setvisible(0);
	$IsDirty--;
}

function OpenOrCloseFriendCheerWnd()
{
	if(FriendCheerWnd.isvisible())
	{
		FriendCheerWnd.setvisible(0);
		$IsDirty--;
	}
	else
	{
		GameMainWndGui_Layer4.pushToBack(FriendCheerWnd);
		FriendCheerWnd.setvisible(1);
		$IsDirty++;
	}
}

function OpenFriendCheerWnd()
{
	GameMainWndGui_Layer4.pushToBack(FriendCheerWnd);
	FriendCheerWnd.setvisible(1);
	$IsDirty++;
}

function CloseFriendCheerWnd()
{
	FriendCheerWnd.setvisible(0);
	$IsDirty--;
}

function OpenOrCloseFriendHintWnd()
{
	if(FriendHintWnd.isvisible())
	{
		FriendHintWnd.setvisible(0);
		$IsDirty--;
	}
	else
	{
		GameMainWndGui_Layer4.pushToBack(FriendHintWnd);
		FriendHintWnd.setvisible(1);
		$IsDirty++;
	}
}

function OpenFriendHintWnd()
{
	GameMainWndGui_Layer4.pushToBack(FriendHintWnd);
	FriendHintWnd.setvisible(1);
	$IsDirty++;
}

function CloseFriendHintWnd()
{
	FriendHintWnd.setvisible(0);
	$IsDirty--;
}

function OpenOrCloseSettingWnd()
{
	if(SettingWnd.isvisible())
	{
		SettingWnd.setvisible(0);
		$IsDirty--;
	}
	else
	{
		GameMainWndGui_Layer4.pushToBack(SettingWnd);
		SettingWnd.setvisible(1);
		$IsDirty++;
	}
}

function OpenSettingWnd()
{
	GameMainWndGui_Layer4.pushToBack(SettingWnd);
	SettingWnd.setvisible(1);
	$IsDirty++;
}

function CloseSettingWnd()
{
	SettingWnd.setvisible(0);
	$IsDirty--;
}

function OpenRelationWnd_PageList(%index)
{
	$theListType = %index;
	switch(%index)
	{
		case 1:
			RelationWndGui_PageScroll_1.setvisible(1);
			RelationWndGui_PageScroll_2.setvisible(0);
			RelationWndGui_PageScroll_3.setvisible(0);
			RelationWndGui_PageScroll_4.setvisible(0);
			RelationWndGui_PageScroll_5.setvisible(0);
		case 2:
			RelationWndGui_PageScroll_1.setvisible(0);
			RelationWndGui_PageScroll_2.setvisible(1);
			RelationWndGui_PageScroll_3.setvisible(0);
			RelationWndGui_PageScroll_4.setvisible(0);
			RelationWndGui_PageScroll_5.setvisible(0);
		case 3:
			RelationWndGui_PageScroll_1.setvisible(0);
			RelationWndGui_PageScroll_2.setvisible(0);
			RelationWndGui_PageScroll_3.setvisible(1);
			RelationWndGui_PageScroll_4.setvisible(0);
			RelationWndGui_PageScroll_5.setvisible(0);
		case 4:
			RelationWndGui_PageScroll_1.setvisible(0);
			RelationWndGui_PageScroll_2.setvisible(0);
			RelationWndGui_PageScroll_3.setvisible(0);
			RelationWndGui_PageScroll_4.setvisible(1);
			RelationWndGui_PageScroll_5.setvisible(0);
		case 5:
			RelationWndGui_PageScroll_1.setvisible(0);
			RelationWndGui_PageScroll_2.setvisible(0);
			RelationWndGui_PageScroll_3.setvisible(0);
			RelationWndGui_PageScroll_4.setvisible(0);
			RelationWndGui_PageScroll_5.setvisible(1);
	}
}

function AddFriendOk()
{                      
	if($SocialFindPlayerAction == 0)
	{
		$SocialFindPlayerAction = 1;
	}
	else
		return;
		
	if(FriendHintWnd_CheckBox_1.isStateOn())
	{
		SptSocialFindPlayerByName(FriendHintWnd_TextEdit.gettext());
	}
	else
	{
		SptSocialFindPlayerById(FriendHintWnd_TextEdit.gettext());
	}
}

function AddFriendList(%listType,%playerID,%name,%level,%family,%type,%status)
{
	
	switch(%type)
	{
		case 0:
			%type = "临时";
		case 1:
			%type = "好友";
		case 2:
			%type = "配偶";
		case 3:
			%type = "师傅";
		case 4:
			%type = "徒弟";
		case 5:
			%type = "兄弟";
		case 6:
			%type = "仇人";
		case 7:
			%type = "屏蔽";
	}
	switch(%status)
	{
		case 0:
			%status = "离线";
		case 1:
			%status = "空闲";
		case 2:
			%status = "副本";
		case 3:
			%status = "组队";
		case 4:
			%status = "战斗";
		case 5:
			%status = "摆摊";
		case 6:
			%status = "暂离";
	}
	
	
	%family = GetFamilyText(%family);

	%index = ("RelationWndGui_PageTextList_" @ %listType).getRowNumById( %playerId );
	if( %index != -1	)
	{		
		("RelationWndGui_PageTextList_" @ %listType).removeRow( %index );
	}
	
	("RelationWndGui_PageTextList_" @ %listType).addRow(%playerID,%name TAB %level @"级" TAB %family TAB %type TAB %status);
	
	//%listType: 1(仙友录),2(临时录),3(仇人录),4(绝交录),5(帮会);
}

function ClearFriendList()
{
	RelationWndGui_PageTextList_1.clear();
}

function RemoveFriendList( %playerId )
{
	RelationWndGui_PageTextList_1.removeRowById( %playerId );
}

function SocialToBuild()
{
	%playerId = RelationWndGui_PageTextList_1.getSelectedId();

	if(IsMyselfHaveTeam() $= 0)
	{
		SptTeam_build(%playerId);
		SptTeam_JoinRequest(Cache_getPlayerTeamId(%playerId));
	}
	else
	{
		if(IsMyselfCaption() !$= 0)
		{
			SptTeam_AddRequest(%playerId);
		}
	}
}

function SeePlayerInfo()
{
	if($SocialFindPlayerAction == 0)
	{
		$SocialFindPlayerAction = 2;
	}
	else
		return;
		
	SptSocialFindPlayerById(RelationWndGui_PageTextList_1.getselectedid());
}

function ShowPlayerInfoInRelationWnd(%id,%name,%level,%soullevel,%friendvalue,%type,%mapname,%family,%groupName)
{	
	switch(%type)
	{
		case 0:
			%type = "临时";
		case 1:
			%type = "好友";
		case 2:
			%type = "配偶";
		case 3:
			%type = "师傅";
		case 4:
			%type = "徒弟";
		case 5:
			%type = "兄弟";
		case 6:
			%type = "仇人";
		case 7:
			%type = "屏蔽";
	}
	switch(%family)
	{
		case 0:
			%family = "无";
		case 1:
			%family = "圣";
		case 2:
			%family = "佛";
		case 3:
			%family = "仙";
		case 4:
			%family = "精";
		case 5:
			%family = "鬼";
		case 6:
			%family = "怪";	
		case 7:
			%family = "妖";
		case 8:
			%family = "魔";
	}
	SeeInfoWnd_ID.settext(%id);
	SeeInfoWnd_PlayerName.settext(%name);
	SeeInfoWnd_PlayerLevel.settext(%level);
	SeeInfoWnd_SoulLevel.settext(%soullevel);
	SeeInfoWnd_AmityNum.settext(%friendvalue);
	SeeInfoWnd_Rapport.settext(%type);
	SeeInfoWnd_Address.settext(%mapname);
	SeeInfoWnd_Family.settext(%family);
	SeeInfoWnd_FactionName.settext(%groupName);
	OpenSeeInfoWnd();
}

function DisengageFriend()
{
	%playerId = RelationWndGui_PageTextList_1.getselectedid();
	if( %playerId != -1 )
		SptSocialDestoryFriend(%playerId);
}