//===========================================================================================================
// �ĵ�˵��:ȫ�ֹ��÷���
// ����ʱ��:2009-09-17
// ������: daniel
//===========================================================================================================

$schInvite = 0;
// �������밴ť 
function CommonInviteDialog(%eid)
{
	%player = GetPlayer();
	%type = %player.getLogicEventType(%eid);
	if(%type == 1)  		//���뽻��
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Trade.png");
	}
	else if(%type == 2)	//�������
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Team.png");
	}
	else if(%type == 3) //������Ӹ���
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Friend.png");
	}
	else if(%type == 6 )
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Friend.png");
	}
	else if(%type == 22 ) // ���ʼ�֪ͨ
	{
		AnimateButton.setBitmap($GUI::GUISkinDirectory @ "MsgBox_Friend.png");
	}
	
	AnimateButton.setvisible(1);
	AnimateButton.Command = "PopupInviteDialog(" @ %eid @ ");";
	if($schInvite != 0)
		cancel($schInvite);
	//5������ع������밴ť
	$schInvite = schedule(5000, 0, "HideInviteButton", %type);
}

//������������Ի���
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
	
	if(%type == 1)			//���뽻��
	{
		%content = %who @ "����������������";
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOK("����", "agreeTrade(" @ %eid @ ");");
		Dialog_OK_Cancel.SetCancel("�ܾ�", "rejectTrade(" @ %eid @ ");");
		Dialog_OK_Cancel.setvisible(1);
	}
	
	if(%type == 2) 			// �������
	{
		%content = %who @ "���������������" ;
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOk("����", "AgreeTeamInviteRequestById(" @ %sender @ ");" );
		Dialog_OK_Cancel.SetCancel("�ܾ�", "RefuseTeamInviteRequestById(" @ %sender @");" );
		Dialog_OK_Cancel.setVisible(1);
	}
	
		if(%type == 3) 		// �������
	{
		%content = %who @ "���������������" ;
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOk("����", "AgreeTeamApplyRequestById(" @ %sender @ ");" );
		Dialog_OK_Cancel.SetCancel("�ܾ�", "DisagreeTeamApplyRequestById(" @ %sender @");" );
		Dialog_OK_Cancel.setVisible(1);
	}
	
	if( %type == 6 ) 		// ����Ϊ����
	{
			Dialog_OK_Content.setContent( "���["@ %who @"]����Ϊ���ѣ�" );
			Dialog_OnlyOK.setVisible( 1 );
	}
	
	if( %type == 22 )		// ֪ͨ���ʼ�
	{
		refreshMail();
		Dialog_OK_Content.setContent( "����һ�����ʼ�����ע����գ�" );
		Dialog_OnlyOK.setVisible( 1 );
	}
}

// ��ȡ�ӳ��¼���������
function GetLogicEventTypeName(%type)
{
	switch(%type)
	{
		case 1:		return "���뽻��";
		case 2: 	return "�������";
		case 3: 	return "�������";
		case 4: 	return "������Ӹ���";
		case 5:		return "������Ӹ���";
		case 6: 	return "����������";
		case 7: 	return "����������";
		case 8: 	return "���������";
		case 9:		return "���������";
		case 10: 	return "������";
		case 11: 	return "������";
		case 12: 	return "�����д�";
		case 13: 	return "����˫����";
		case 14: 	return "����˫����";
		case 15: 	return "���봫��";
		case 16: 	return "���븱��";
		case 17: 	return "���󸱱�";
		case 18: 	return "��������";
		case 19: 	return "�����ʦ";
		case 20:	return "������ͽ";
		case 21: 	return "�����̳ǹ���";
		case 22: 	return "֪ͨ���ʼ�";
	}
	return "";
}

// ���ڰ�������Ʒ����Ч���ź�������������Ϣ��ʾ
function HandleItemSomething(%name, %flag, %oldnums, %newnums)
{
	if(%name !$= "")
		%name = "<t c='0x00ff00ff'>" @ %name @ "</t>";
	
	if(%flag == 2) 			// ����
	{
		%text = "<t f='����' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>�������ˡ�</t>" @ %name @"<t>�� �� " @ (%newnums - %oldnums) @"</t></t>";
		AddChatMessage("13", %text, "0", "0");
		PlayBuyItemSound();
	}
	else if(%flag == 3)	// ����
	{
		%text = "<t f='����' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>�������ˡ�</t>" @ %name @"<t>�� �� " @ (%oldnums - %newnums) @"</t></t>";
		AddChatMessage("13", %text, "0", "0");
		PlayBuyItemSound();
	}
	else if(%flag == 19)
	{
		%text = "<t f='����' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>��������</t>" @ %name @"<t>�� �ɹ� " @ "</t></t>";
		AddChatMessage("13", %text, "0", "0");
		//PlayBuyItemSound();
	}
	else if(%flag == 15)
	{
		%text = "<t f='����' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>�������ˡ�</t>" @ %name @"<t>�� " @ "</t></t>";
		AddChatMessage("13", %text, "0", "0");
	}
	
	else if(%flag > 2)
	{
		if(%oldnums > %newnums)
			%text = "<t f='����' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>��ʧȥ�ˡ�</t>" @ %name @"<t>�� �� " @ (%oldnums - %newnums) @"</t></t>";
		else
			%text = "<t f='����' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>������ˡ�</t>" @ %name @"<t>�� �� " @ (%newnums - %oldnums) @"</t></t>";
		AddChatMessage("13", %text, "0", "0");
		PlayUsedItemSound();
	}
}

function ShowCompleteMission(%missionid)
{
	%MissionData = "MissionData_" @ %missionid;
	%Mission_Name = "��ϲ����ɡ�" @ %MissionData.Name @ "������";
	SetScreenMessage(%Mission_Name, 0xFFFF00);
}

// �����������ʾ��Ǯ�ĸ�����Ϣ
function ShowMoneyMessage(%type, %curmoney, %newmoney)
{
	%typeStr 		= "��Ԫ";
	%GoldImg 		= "gameres/gui/images/GUIWindow31_1_024.png";
	%SilverImg 	= "gameres/gui/images/GUIWindow31_1_025.png";
	%CopperImg	= "gameres/gui/images/GUIWindow31_1_026.png";
	if(%type == 1)
	{
		%typeStr 		= "��Ԫ";
		%GoldImg 		= "gameres/gui/images/GUIWindow31_1_020.png";
		%SilverImg 	= "gameres/gui/images/GUIWindow31_1_021.png";
		%CopperImg	= "gameres/gui/images/GUIWindow31_1_022.png";
	}
	else if(%type == 2)
		%typeStr		= "�ֿ��Ԫ";
	
	%text = "";
	%dis = %curmoney - %newmoney;
	if(%dis > 0)
	{
		%text = "<t f='����' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>��ʧȥ" @ %typeStr @ ": </t>";
	}
	else if(%dis < 0)
	{
		%text = "<t f='����' m='1' n='12' c='0xffffffff' o='0x010101ff'><t>�����" @ %typeStr @ ": </t>";
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
	  %content = %srcName @ "������������ͽ����";
	  %playerId = %player.getPlayerID();
	  
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOK("����", "agreePrentice("@%playerId@");");
		Dialog_OK_Cancel.SetCancel("�ܾ�", "rejectRelation("@%playerId@");");
		Dialog_OK_Cancel.setvisible(1);
}

function uiRelationMaster(%player)
{
	  %srcName = %player.getPlayerName();
	  %content = %srcName @ "����������ʦ����";
	  %playerId = %player.getPlayerID();
	  
		Dialog_OK_Cancel.SetContent(%content);
		Dialog_OK_Cancel.SetOK("����", "agreeMaster("@%playerId@");");
		Dialog_OK_Cancel.SetCancel("�ܾ�", "rejectRelation("@%playerId@");");
		Dialog_OK_Cancel.setvisible(1);
}