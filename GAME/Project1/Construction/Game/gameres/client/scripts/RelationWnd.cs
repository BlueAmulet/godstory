//===========================================================================================================
// �ĵ�˵��:��ϵ�������
// ����ʱ��:2009-9-14
// ������: soar
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
			%type = "��ʱ";
		case 1:
			%type = "����";
		case 2:
			%type = "��ż";
		case 3:
			%type = "ʦ��";
		case 4:
			%type = "ͽ��";
		case 5:
			%type = "�ֵ�";
		case 6:
			%type = "����";
		case 7:
			%type = "����";
	}
	switch(%status)
	{
		case 0:
			%status = "����";
		case 1:
			%status = "����";
		case 2:
			%status = "����";
		case 3:
			%status = "���";
		case 4:
			%status = "ս��";
		case 5:
			%status = "��̯";
		case 6:
			%status = "����";
	}
	
	
	%family = GetFamilyText(%family);

	%index = ("RelationWndGui_PageTextList_" @ %listType).getRowNumById( %playerId );
	if( %index != -1	)
	{		
		("RelationWndGui_PageTextList_" @ %listType).removeRow( %index );
	}
	
	("RelationWndGui_PageTextList_" @ %listType).addRow(%playerID,%name TAB %level @"��" TAB %family TAB %type TAB %status);
	
	//%listType: 1(����¼),2(��ʱ¼),3(����¼),4(����¼),5(���);
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
			%type = "��ʱ";
		case 1:
			%type = "����";
		case 2:
			%type = "��ż";
		case 3:
			%type = "ʦ��";
		case 4:
			%type = "ͽ��";
		case 5:
			%type = "�ֵ�";
		case 6:
			%type = "����";
		case 7:
			%type = "����";
	}
	switch(%family)
	{
		case 0:
			%family = "��";
		case 1:
			%family = "ʥ";
		case 2:
			%family = "��";
		case 3:
			%family = "��";
		case 4:
			%family = "��";
		case 5:
			%family = "��";
		case 6:
			%family = "��";	
		case 7:
			%family = "��";
		case 8:
			%family = "ħ";
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