//===========================================================================================================
// �ĵ�˵��:��Ӳ���
// ����ʱ��:2009-5-26
// ������: Batcel
//===========================================================================================================

function CloseTeamWnd()
{
	 NewTeamWndGui.setVisible(0);
	 $IsDirty--;
}

function  ShowTeamOperationTxt(%Text)
{
	ScreenMessage.setText(%Text,0xff0000);
}

function SelectedTeammate(%index)
{
	TeamSelectedBackground.setPosition((12+%index*118),70);
	TeamSelectedBackground.setVisible(1);
}

function OpenTeamInfoGui()
{
	TeammateInfoGui.setVisible(1);
	ApplyInfoGui.setVisible(0);
	InviteInfoGui.setVisible(0);
	TeamSelectedBackground.setVisible(0);
	for(%i=0;%i<6;%i++)
	{
		("TeamSelectedBtn_" @ %i).setStateOn(0);
		("TeamSelectedBtn_" @ %i).setVisible(("TeammateInfo_" @ %i).IsVisible());		
	}
}

function OpenApplyListWnd()
{
	TeammateInfoGui.setVisible(0);
	ApplyInfoGui.setVisible(1);
	InviteInfoGui.setVisible(0);
	TeamSelectedBackground.setVisible(0);
	for(%i=0;%i<6;%i++)
	{
		("TeamSelectedBtn_" @ %i).setVisible(("TeamApplyInfo_" @ %i).IsVisible());
		("TeamSelectedBtn_" @ %i).setStateOn(0);
	}
}

function OpenInviteListWnd()
{
	TeammateInfoGui.setVisible(0);
	ApplyInfoGui.setVisible(0);
	InviteInfoGui.setVisible(1);
	TeamSelectedBackground.setVisible(0);
	for(%i=0;%i<6;%i++)
	{
		("TeamSelectedBtn_" @ %i).setVisible(("TeamInviteInfo_" @ %i).IsVisible());
		("TeamSelectedBtn_" @ %i).setStateOn(0);
	}
}

function OpenInviteJoinTeamWnd()
{
	TeamMateNameEdit.setText("");
	if(InviteTeammateWndGui.IsVisible())
	{
		 InviteTeammateWndGui.setvisible(0);
		 $IsDirty--;
  }
	else
	{
		GameMainWndGui_Layer4.pushToBack(InviteTeammateWndGui);
		InviteTeammateWndGui.setvisible(1);
		$IsDirty++;
	}
}

function CloseInviteJoinTeamWnd()
{
	if(InviteTeammateWndGui.IsVisible())
	{
		 InviteTeammateWndGui.setvisible(0);
		 $IsDirty--;
  }
}


//��Ӹ���
function OnTeamFollow()
{
	EnableTeamFollow(1);
	TeamFollowBtn.setText("ȡ������");
	TeamFollowBtn.Command ="OffTeamFollow();";
}

//ȡ����Ӹ���
function OffTeamFollow()
{
	EnableTeamFollow(0);
	TeamFollowBtn.setText("��Ӹ���");
	TeamFollowBtn.Command ="OnTeamFollow();";
}

//�յ�/�ر���Ӹ�������
function RecvTeamFollowRequest(%On)
{
	if(%On == 1)
	{
    TeamFollowDialogGui. setVisible(1);
    $IsDirty++;
    TeamFollowBtn.setActive(1);
    TeamFollowBtn.setText("��Ӹ���");
	  TeamFollowBtn.Command ="OnTeamFollow();";
  }
  else
  {
  	TeamFollowBtn.setActive(0);
  }
}

//ͬ����Ӹ���
function AgreeTeamFollow()
{
	OnTeamFollow();
	TeamFollowDialogGui. setVisible(0);
  $IsDirty--;
  
}


//�ܾ���Ӹ���
function DisagreeTeamFollow()
{
	OffTeamFollow();
	TeamFollowDialogGui. setVisible(0);
  $IsDirty--;
}


//�����Ӹ���״̬
function CheckTeamFollowState()
{
	 if(IsMyselfCaption())
	   return;
	 if(SptTeam_IsFollow())
	 {
	 	TeamFollowBtn.setText("��Ӹ���");
	  TeamFollowBtn.Command ="OnTeamFollow();";
	 }
}

//��������
function CreateTeam()
{
	if(IsMyselfHaveTeam())
	{		 
	   ShowTeamOperationTxt("�����ڲ��ܴ�������!");
	   PlayOperationErrorSound();
	}
	else
	{	
		SptTeam_BuildAlong();//����������ʹ�����������		    
	}	
}
//�뿪��������
function LeaveTeam()
{
	if(IsMyselfHaveTeam())
	   SptTeam_LeaveRequest();	
}

//ȷ�����Ķ������ƺ���
function ChangeTeamName()
{
	if(TeamNameEdit_1.getText()!$="")
	{
		SptTeam_ChangeName(TeamNameEdit_1.getText());
	}
	else
	 ShowTeamOperationTxt("�������������!");
}

//��������ɹ�
function TeamHavaChangeName(%szName)
{
	TeamNameEdit_1.setText(%szName);
}

//��ɢ����
function BreakTeam()
{
	 if(IsMyselfHaveTeam()&&IsMyselfCaption())
	  {
	  	//���ͽ�ɢ��������
	  	SptTeam_Disband();	  	
	  }
}
//�ƽ��ӳ�
function DisplaceTeamHeader()
{
	if(IsMyselfCaption())
	{		
		%SelectedId =whoIsSelected();
		if( %SelectedId == -1)
		{
			ShowTeamOperationTxt("��ѡ��һ�����ѽ��в���!");			
			PlayOperationErrorSound();
		}
		else if(%SelectedId == 0)
		{
			ShowTeamOperationTxt("ֻ�ܶԶ��ѽ��в���!");
			PlayOperationErrorSound();
		}
		else
		{
		   //�����ƽ��ӳ�������
		   SptTeam_ChangeLeader(GetTeammate(%SelectedId,1));
		   TeamSelectedBackground.setVisible(0);
		}
	}
	else
	{
		ShowTeamOperationTxt("�����ܽ��д˲���!");
		PlayOperationErrorSound();
	}	
}

function whoIsSelected()
{
	for(%i = 0; %i < 6; %i++)
	{
		if(("TeamSelectedBtn_" @ %i).IsStateOn())
		   return %i;
  }
  return -1;
}

//��������ӳ�
function TeamHavaChangeHeader(%HeaderId)
{	  
	
	if(getSelectedPlayerId()==%HeaderId)
	{
		SetTeamHeaderBtnState();
		//OpenSetFlagButton.setVisible(1);
	}
	else
	{
		SetTeamMemberBtnState();
		//OpenSetFlagButton.setVisible(0);
	}
	//ˢ�¶����б�
	GetTeammateListInfo();
}

//�߳�����
function KickOutTeam()
{
	if(IsMyselfCaption())
	{
		%SelectedId =whoIsSelected();
	  if(%SelectedId == -1 )  
	  {
	  	ShowTeamOperationTxt("��ѡ��һ�����ѽ��в���!");	
	    PlayOperationErrorSound();
	  }
	  else if(%SelectedId == 0)
	  {
	  		ShowTeamOperationTxt("ֻ�ܶԶ��ѽ��в���!");
	      PlayOperationErrorSound();
	      
	  }
	  else
	  {
		    SptTeam_DropRequest(GetTeammate(%SelectedId,1));
		    TeamSelectedBackground.setVisible(0);
		 }		
	}
	else
	{		
	  PlayOperationErrorSound();
	}
}

//�ӳ�ͨ�������������
function InviteJoinTeamByName()
{ 
	if(TeamMateNameEdit.getText()!$= "")
	 { 
	 	$SocialFindPlayerAction=3;
	 	SptSocialFindPlayerByName(TeamMateNameEdit.getText());
	} 
	else
	 { 
	 	ShowTeamOperationTxt("�������������");
    PlayOperationErrorSound();
	 } 
}

//function InviteJoinTeamByWriteName()
//{
	//TeamMateNameEdit.setText("");
//}
function ReadyToInvationTeam(%PlayerId)
{
	  if(%PlayerId==0)
	 	{
	 		ShowTeamOperationTxt("�������Ҳ����ڻ�����!");
	    PlayKindOfFailedSound(1);
	 	}
	 	else
	 	{
	 		SptTeam_AddRequest(%PlayerId);
	 	  InviteTeammateWndGui.setvisible(0);
	 	} 
} 

//��Ա״̬�°�ť
function SetTeamMemberBtnState()
{
	NoTeamStateGui.setVisible(0);
	HavaTeamStateGui.setVisible(1);
	TeamFollowBtn.setVisible(1);
	ChangeToGroupBtn.setVisible(1);
	if(SptTeam_IsFollowOn())
  {
  	if(SptTeam_IsFollow())
  	 {
	     TeamFollowBtn.setActive(1);	
	     TeamFollowBtn.setText("ȡ������");
	     TeamFollowBtn.Command ="OffTeamFollow();";
	   }
	   else
	   {
	   	 TeamFollowBtn.setActive(1);	
	     TeamFollowBtn.setText("��Ӹ���");
	     TeamFollowBtn.Command ="OnTeamFollow();";
	   }
  }
  else
  {
    TeamFollowBtn.setActive(0);	
	  TeamFollowBtn.setText("��Ӹ���");
	}
	ChangeToGroupBtn.setActive(0);
	ChangeTeamNameBtn.setactive(0);
	ApplyListBtn.setactive(0);
	InviteListBtn.setactive(0);
	ChangeToGroupBtn.setactive(0);
	KickOutTeamBtn.setactive(0);
	BreakTeamBtn.setactive(0);
	DisplaceTeamHeaderBtn.setactive(0);
	TeamInviteBtn.setactive(0);
	//AddFriendBtn.setactive(1);
	TeamNameEdit_1.setActive(0);
	TeamHeaderMark.setVisible(1);	
}

//�ӳ�״̬�°�ť
function SetTeamHeaderBtnState()
{	
	NoTeamStateGui.setVisible(0);
	HavaTeamStateGui.setVisible(1);
	TeamFollowBtn.setVisible(1);
	TeamFollowBtn.setText("��Ӹ���");
	TeamFollowBtn.Command ="OnTeamFollow();";
	ChangeToGroupBtn.setVisible(1);
	TeamFollowBtn.setActive(1);
	ChangeToGroupBtn.setActive(1);
	ChangeTeamNameBtn.setactive(1);	
	ApplyListBtn.setactive(1);
	InviteListBtn.setactive(0);
	ChangeToGroupBtn.setactive(1);
	KickOutTeamBtn.setactive(1);
	BreakTeamBtn.setactive(1);
	DisplaceTeamHeaderBtn.setactive(1);
	TeamInviteBtn.setactive(1);
	//AddFriendBtn.setactive(1);
	TeamNameEdit_1.setActive(1);
	TeamHeaderMark.setVisible(1);
}
//�޶���״̬�°�ť
function SetNoTeamBtnState()
{
	NoTeamStateGui.setVisible(1);
	HavaTeamStateGui.setVisible(0);
	TeamFollowBtn.setVisible(0);
	ChangeToGroupBtn.setVisible(0);
	ApplyListBtn.setactive(0);
	InviteListBtn.setactive(1);	
	TeamNameEdit.setText("");
	TeamHeaderMark.setVisible(0);
}

function CloseNewTeamWnd()
{
	NewTeamWndGui.setvisible(0);
}

//�Լ����뵽������
function ShowTeamBuildOK( %TeamName )
{
	SptChangeChatChannelType(7);
	PlayJoinToTeamSound();
	TeammateIconGui.setVisible(1);	
	if(IsMyselfCaption())
	{
		if(TeamNameEdit.getText()!$="")
		   SptTeam_ChangeName(TeamNameEdit.getText());
		else
		   TeamNameEdit_1.setText(%TeamName);
		SetTeamHeaderBtnState();
		//OpenSetFlagButton.setVisible(1);		
	}
	else
	{
		TeamNameEdit_1.setText(%TeamName);
	  SetTeamMemberBtnState();	  
	}
	 GetTeammateListInfo();
}

//�뿪����
function ShowLeaveTeamNotify( %TeamName )
{
	//TeamMsgText.setText("���Ѿ��뿪����"@ %TeamName);
	//TeamMsgWnd.setvisible(1);
	TeamSelectedBackground.setVisible(0);
	for(%i=0;%i<6;%i++)
	{
		("TeammateInfo_" @ %i).setVisible(0);
		("TeamApplyInfo_" @ %i).setVisible(0);
		("TeamSelectedBtn_" @ %i).setVisible(0);
	}

	//TeamInviteTypeList.clear();
	SetNoTeamBtnState();
	TeammateIconGui.setVisible(0);
	//OpenSetFlagButton.setVisible(0);	      
}


//����Ƕӳ��뿪��ı�ӳ�ͼ��
function ChangeCaptain()
{
//	if(IsMyselfCaption() !$= 0)
//	{ 
//		OpenSetFlagButton.setVisible(1);
//		for(%i = 0; %i < 5; %i++)
//		{                                       
//			 ("KickTeammate_" @ %i).setVisible(1);
//		}
//	} 	
}
//�����뿪����
function TeammateLeaveTeam(%PlayerId)
{
	if(GetTeammate(0,1)==getSelectedPlayerId())
	{
		    SetTeamHeaderBtnState();
		    //OpenSetFlagButton.setVisible(1);
	}
	//ˢ�¶����б�
	GetTeammateListInfo();
	  if(TeammateInfoGui.IsVisible())
	  {
	  	for(%i=0;%i<6;%i++)
	   {
		  ("TeamSelectedBtn_" @ %i).setVisible(("TeammateInfo_" @ %i).IsVisible());		
	   }
	  }
}

//��ȡ�����б���Ϣ
function  GetTeammateListInfo()
{
	TeamSelectedBackground.setVisible(0);
	for(%i=0;%i<6;%i++)
	   ("TeammateInfo_" @ %i).setVisible(0);
	for(%i=0;%i<6;%i++)
	{
		  if(GetTeammate(%i,1)!$=0)
		  {
		  	%Id = GetTeammate(%i,1);
		  	%Name = Cache_getPlayerName(%Id);
		  	%Level = Cache_getPlayerLevel(%Id);
		  	%Family = GetFamilyText(Cache_getPlayerFamily(%Id));
		  	%Sex = Cache_getPlayerSex(%Id)@"";
        
		  	("TeammateIconBitmap_" @ %i ).setBitmap("gameres/gui/images/GUIWindow26_1_00"@(6+%Sex) @ ".png");
		  	("TeammateName_" @ %i).settext(%Name);
		  	("TeammateClass_" @ %i).settext(%Family);
		  	("TeammateLevel_" @ %i).settext(%Level @ "��");
		  	("TeammateInfo_" @ %i).setVisible(1);
		  	if(TeammateInfoGui.IsVisible())
		  	  ("TeamSelectedBtn_" @ %i).setVisible(1);		  			  	
		  }
		  else
		    break;
	}
}

//����ת��������
function GetFamilyText(%FamilyID)
{
	%Text="��";
	switch(%FamilyID)
	{
	case 0:%Text="������";
	case 1:%Text="������";
	case 2:%Text="������";
	case 3:%Text="������";
	case 4:%Text="�ɻ���";
	case 5:%Text="���Ľ�";
	case 6:%Text="ɽ����";
	case 7:%Text="���鹬";
	case 8:%Text="��ħ��";
	}
	return %Text;
}
//��Ҽ��뵽����
function PlayerJoinTeam(%PlayerId)
{	
	//TeamMsgText.setText(GetLocalPlayerName(%PlayerId) @"�������");
	//TeamMsgWnd.setvisible(1);
	PlayJoinToTeamSound(); 
	//��������ӵ������б���
   %Name = Cache_getPlayerName(%PlayerId);
	 %Level = Cache_getPlayerLevel(%PlayerId);
	 %Family = GetFamilyText(Cache_getPlayerFamily(%PlayerId));
	 %Sex = Cache_getPlayerSex(%PlayerId);
	
	
	//TeamMateList.addRow(%PlayerId,GetLocalPlayerName(%PlayerId) TAB %Family TAB %Level @"��",TeamMateList.rowCount());
	 for(%i=0;%i<6;%i++)
	  {
	  	if(!("TeammateInfo_" @ %i).IsVisible())
	  	{
	  		("TeammateIconBitmap_" @ %i).setBitmap("gameres/gui/images/GUIWindow26_1_00"@(6+%Sex) @ ".png");
	  		("TeammateName_" @ %i).settext(%Name);
	  	  ("TeammateClass_" @ %i).settext(%Family);
	  	  ("TeammateLevel_" @ %i).settext(%Level @ "��");
	  	 	("TeammateInfo_" @ %i).setVisible(1);
	  	 	if(TeammateInfoGui.IsVisible())
	  	 	  ("TeamSelectedBtn_" @ %i).setVisible(1);
	  	 	break;
	  	}
	  }
}


//�������
function RequestToTeam(%ID,%Name,%Type,%Count)
{
	PlayKindOfInviteSound(1); 
	%Name = Cache_getPlayerName(%ID);
	%Level = Cache_getPlayerLevel(%ID);
	%Family = GetFamilyText(Cache_getPlayerFamily(%ID));
	%Sex = Cache_getPlayerSex(%ID);
	if(%Type $= 1)
	{
	  for(%n=0;%n<6;%n++)
	  {
	  	%i = $InviteListIndex [%n];
	  	
	  	if(!("TeamApplyInfo_" @ %i ).IsVisible())
	  	{
	  		("TeamApplyIconBitmap_" @ %i).setBitmap("gameres/gui/images/GUIWindow26_1_00"@(6+%Sex) @ ".png");
	  		("TeamApplyName_" @ %i).settext(%Name);
	  	  ("TeamApplyClass_" @ %i).settext(%Family);
	  	  ("TeamApplyLevel_" @ %i).settext(%Level @ "��");
	  	 	("TeamApplyInfo_" @ %i).setVisible(1);
	  	 	if(ApplyInfoGui.IsVisible())
	  	 	  ("TeamSelectedBtn_" @ %n).setVisible(1);
	  	 	break;
	  	}
	  }
		  
	}
	else
	{
		 for(%n=0;%n<6;%n++)
	  {
	  	%i = $InviteListIndex [%n];
	  	if(!("TeamInviteInfo_" @ %i).IsVisible())
	  	{
	  		("TeamInviteIconBitmap_" @ %i).setBitmap("gameres/gui/images/GUIWindow26_1_00"@(6+%Sex) @ ".png");
	  		("TeamInviteName_" @ %i).settext(%Name);
	  	  ("TeamInviteClass_" @ %i).settext(%Family);
	  	  ("TeamInviteLevel_" @ %i).settext(%Level @ "��");
	  	 	("TeamInviteInfo_" @ %i).setVisible(1);
	  	 	if(InviteInfoGui.IsVisible())
	  	 	  ("TeamSelectedBtn_" @ %n).setVisible(1);
	  	 	break;
	  	}
	  }
	}
}

function LookTeamRequest()
{
 	if($RType $=1)
 	  TeamApplyListGui.setVisible(1);
 	else
 	  TeamInviteListGui.setVisible(1);
 	 TeamRequestWnd.setvisible(0); 
}

function closeTeamRequestWnd()
{
	TeamRequestWnd.setVisible(0);
}


//===========================================================================================================
// �ĵ�˵��:���������б�
// ����ʱ��:2009-5-27
// ������: Batcel
//=========================================================================================================== 
$InviteListIndex[0] =0;
$InviteListIndex[1] =1;
$InviteListIndex[2] =2;
$InviteListIndex[3] =3;
$InviteListIndex[4] =4;
$InviteListIndex[5] =5;


//��������б�
function ClearTeamInviteList()
{
	  for(%i=0; %i<6; %i++)
	  {
	  	if(("TeamInviteInfo_" @ $InviteListIndex[%i]).IsVisible() )
	  	{
	  		SptTeam_RefuseInvationPlayerByIndex(%i,0);
	  		echo("�����﷢�ľܾ�" @ %i);
	  		("TeamInviteInfo_" @ $InviteListIndex[%i]).setVisible(0);
	  		("TeamSelectedBtn_" @ %i).setVisible(0);
	  	}
	  	//SptTeam_RemoveRefuse(TeamInviteList.getSelectedId());
	  	//if(TeamInviteTypeList.getRowId(%i)==0)
	  	//   SptTeam_SendRefused(TeamInviteList.getRowId(%i),0);
	  	//else
	  	//   SptTeam_SendRefused(TeamInviteList.getRowId(%i),2);
	  		   
	  }
	   //TeamInviteList.clear();
}

//ͬ������
function AgreeTeamInviteRequest()
{
	 %SelectedId = whoIsSelected();
	 if(%SelectedId == -1)
	  {
	  	ShowTeamOperationTxt("����ѡ��Ҫ����Ķ���!");
	    PlayOperationErrorSound();
	  }
	  else
	  {	  	
	  	 SptTeam_AcceptInvationPlayerByIndex(%SelectedId,0);
	  	 TeamSelectedBackground.setVisible(0);
	  	 ("TeamInviteInfo_" @ $InviteListIndex[%SelectedId]).setVisible(0);	  	 
	  	//SptTeam_RemoveRefuse(TeamInviteList.getSelectedId());
	  	//if(TeamInviteTypeList.getRowId(TeamInviteList.getSelectedRow())==2)
	  	  // SptTeam_AcceptAdd(TeamInviteList.getSelectedId());
	  	//else
	  	  // SptTeam_AcceptBuild(TeamInviteList.getSelectedId());	  	
	  	//TeamInviteTypeList.removeRow(TeamInviteList.getSelectedRow());
	  	//TeamInviteList.removeRow(TeamInviteList.getSelectedRow());
	  	ClearTeamInviteList();	  	
	  }
}

//ͨ��PlayerIdͬ������
function AgreeTeamInviteRequestById(%playerId)
{
	echo( "playerid:----------------------------------" @ %playerId );
	 %SelectedId = SptTeam_GetInvationIndex(%playerId,0);
	 if(%SelectedId == -1)
	  {
	  	ShowTeamOperationTxt("��Ч�Ĳ���Ŀ��!");
	    PlayOperationErrorSound();
	  }
	  else
	  {	  	
	  	 SptTeam_AcceptInvationPlayerByIndex(%SelectedId,0);
	  	 TeamSelectedBackground.setVisible(0);
	  	 ("TeamInviteInfo_" @ $InviteListIndex[%SelectedId]).setVisible(0);	  	 
	  	ClearTeamInviteList();	  	
	  }
}

//ͨ��playerId�ܾ�����
function RefuseTeamInviteRequestById(%playerId)
{
	%SelectedId = SptTeam_GetInvationIndex(%playerId,1);
	
	if(%SelectedId == -1)
	{
		ShowTeamOperationTxt("��Ч�Ĳ���Ŀ��!");
	  PlayOperationErrorSound();
	}
	else
	{
		SptTeam_RefuseInvationPlayerByIndex(%SelectedId,1);
		TeamSelectedBackground.setVisible(0);
		
		//here need  update Invite list
		("TeamInviteInfo_" @ $InviteListIndex[%SelectedId]).setVisible(0);
		("TeamSelectedBtn_" @ %SelectedId).setStateOn(0);
		UpdataInviteList(%SelectedId);
	}
}


//�ܾ�����
function RefuseTeamInviteRequest()
{
	%SelectedId = whoIsSelected();
	if(%SelectedId == -1)
	{
		ShowTeamOperationTxt("��ѡ���������!");
	  PlayOperationErrorSound();
	}
	else
	{
		SptTeam_RefuseInvationPlayerByIndex(%SelectedId,1);
		TeamSelectedBackground.setVisible(0);
		
		//here need  update Invite list
		("TeamInviteInfo_" @ $InviteListIndex[%SelectedId]).setVisible(0);
		("TeamSelectedBtn_" @ %SelectedId).setStateOn(0);
		UpdataInviteList(%SelectedId);
	}
}

//���������б�
function UpdataInviteList(%Id)
{
	for(%i=%Id;%i<6;%i++)
	{
		if(!("TeamInviteInfo_"@ $InviteListIndex[%i+1]).IsVisible())
		{
			("TeamInviteInfo_" @ $InviteListIndex[%i]).setVisible(0);
			("TeamSelectedBtn_" @ %i).setVisible(0);
			break;
		}
		else
		{
			 ("TeamInviteInfo_"@ $InviteListIndex[%i+1]).setPosition((14+118*%i),14);
			 ("TeamInviteInfo_"@ $InviteListIndex[%i]).setPosition((14+118*(%i+1)),14);
			  %temp = $InviteListIndex[%i];
	      $InviteListIndex[%i]=$InviteListIndex[%i+1];
	      $InviteListIndex[%i+1]=%temp;
		}
	}
}


function SetAutoRefuseInvite()
{
	if(InviteAutoCheckBtn.IsStateOn())
		$IsTeamAutoRefuseInvation = true;	
	else
	  $IsTeamAutoRefuseInvation = false;
} 


//===========================================================================================================
// �ĵ�˵��:���������б�
// ����ʱ��:2009-5-27
// ������: Batcel
//=========================================================================================================== 
$ApplyListIndex[0] =0;
$ApplyListIndex[1] =1;
$ApplyListIndex[2] =2;
$ApplyListIndex[3] =3;
$ApplyListIndex[4] =4;
$ApplyListIndex[5] =5;

//ͨ��PlayerIdͬ������������
function AgreeTeamApplyRequestById( %PlayerId )
{
	 %SelectedId = SptTeam_GetInvationIndex( %PlayerId,1 );
	if(%SelectedId == -1)
	 {
	 	ShowTeamOperationTxt("��Ч�Ĳ���Ŀ��!");
	  PlayOperationErrorSound();
	 }
	 else
	 {
	 	 if(TeammateInfo_5.IsVisible())
	    {
		   ShowTeamOperationTxt("������������!");
	     PlayKindOfFailedSound(1);
	    }
	    else
	    {
	    	 //SptTeam_RemoveRefuse(TeamApplyList.getSelectedId());
	 	     //SptTeam_AcceptJoin(TeamApplyList.getSelectedId());	 	    
	 	     //TeamApplyList.removeRow(TeamApplyList.getSelectedRow());
	 	     
	 	     SptTeam_AcceptInvationPlayerByIndex(%SelectedId,1);
	 	     TeamSelectedBackground.setVisible(0);
	 	     //���ﻹ������б�.
	 	     	("TeamApplyInfo_"@ $ApplyListIndex[%SelectedId]).setVisible(0);
	 	     	("TeamSelectedBtn_" @ %SelectedId).setStateOn(0);
	 	      UpdataApplyList(%SelectedId);
	 	  }
	 }
}


//ͬ������������
function AgreeTeamApplyRequest()
{
	 %SelectedId = whoIsSelected();
	if(%SelectedId == -1)
	 {
	 	ShowTeamOperationTxt("����ѡ��һ����ҽ��в���!");
	  PlayOperationErrorSound();
	 }
	 else
	 {
	 	 if(TeammateInfo_5.IsVisible())
	    {
		   ShowTeamOperationTxt("������������!");
	     PlayKindOfFailedSound(1);
	    }
	    else
	    {
	    	 //SptTeam_RemoveRefuse(TeamApplyList.getSelectedId());
	 	     //SptTeam_AcceptJoin(TeamApplyList.getSelectedId());	 	    
	 	     //TeamApplyList.removeRow(TeamApplyList.getSelectedRow());
	 	     
	 	     SptTeam_AcceptInvationPlayerByIndex(%SelectedId,1);
	 	     TeamSelectedBackground.setVisible(0);
	 	     //���ﻹ������б�.
	 	     	("TeamApplyInfo_"@ $ApplyListIndex[%SelectedId]).setVisible(0);
	 	     	("TeamSelectedBtn_" @ %SelectedId).setStateOn(0);
	 	      UpdataApplyList(%SelectedId);
	 	  }
	 }
}

//ͨ��PlayerId�ܾ�����������
function DisagreeTeamApplyRequestById( %playerId )
{
	%SelectedId = SptTeam_GetInvationIndex( %PlayerId,1 );
	if(%SelectedId == -1)
	 {
	 	ShowTeamOperationTxt("����ѡ��һ����ҽ��в���!");
	  PlayOperationErrorSound();
	 }
	 else
	 {
	 	//SptTeam_RemoveRefuse(TeamApplyList.getSelectedId());	 	
	 	//SptTeam_SendRefused(TeamApplyList.getSelectedId(),1);	 	
	 	//TeamApplyList.removeRow(TeamApplyList.getSelectedRow());
	 	SptTeam_RefuseInvationPlayerByIndex(%SelectedId,1);
	 	TeamSelectedBackground.setVisible(0);
	 	//���ﻹ������б�....
	 	("TeamApplyInfo_"@ $ApplyListIndex[%SelectedId]).setVisible(0);
	 	UpdataApplyList(%SelectedId);
	 }
}

//�ܾ�����������
function DisagreeTeamApplyRequest()
{
	%SelectedId = whoIsSelected();
	if(%SelectedId == -1)
	 {
	 	ShowTeamOperationTxt("����ѡ��һ����ҽ��в���!");
	  PlayOperationErrorSound();
	 }
	 else
	 {
	 	//SptTeam_RemoveRefuse(TeamApplyList.getSelectedId());	 	
	 	//SptTeam_SendRefused(TeamApplyList.getSelectedId(),1);	 	
	 	//TeamApplyList.removeRow(TeamApplyList.getSelectedRow());
	 	SptTeam_RefuseInvationPlayerByIndex(%SelectedId,1);
	 	TeamSelectedBackground.setVisible(0);
	 	//���ﻹ������б�....
	 	("TeamApplyInfo_"@ $ApplyListIndex[%SelectedId]).setVisible(0);
	 	UpdataApplyList(%SelectedId);
	 }
}

//��������б�
function ClearTeamApplyList()
{
	for(%i=0;%i<6;%i++)
	{
		if(("TeamApplyInfo_" @ $ApplyListIndex[%i]).IsVisible())
		{
		  //SptTeam_RemoveRefuse(TeamApplyList.getRowId(%i));
	    //SptTeam_SendRefused(TeamApplyList.getRowId(%i),1);
	    //......................
	    
	    SptTeam_RefuseInvationPlayerByIndex(%i,1);
	    ("TeamApplyInfo_" @ $ApplyListIndex[%i]).setVisible(0);
	    if(ApplyInfoGui.IsVisible())
	    {
	       ("TeamSelectedBtn_" @ %i).setVisible(0);
	       ("TeamSelectedBtn_" @ %i).setStateOn(0);	     
	    }
	  }	    
	}
	 //TeamApplyList.clear();
}

//���˾ܾ����ҵ�����
function RefusedApplyRequest(%Id,%Type)
{
	
	%Name = Cache_getPlayerName(%Id);
	if(%Type==1)
	   ShowTeamOperationTxt(%Name @ "�ܾ���������������!");
	else
	   ShowTeamOperationTxt(%Name @ "�ܾ���������������!");	   
	  PlayKindOfFailedSound(1);
}

//ϵͳ�Զ��ظ��ľܾ�����
function TeamAutoRefused(%index,%Type)
{
	TeamSelectedBackground.setVisible(0);
	if(%Type==1)
	{
		("TeamApplyInfo_"@ $ApplyListIndex[%index]).setVisible(0);		
	 	UpdataApplyList(%index);
	}
	else
	{
	  ("TeamInviteInfo_" @ $InviteListIndex[%index]).setVisible(0);
	 	UpdataInviteList(%index);
	}
}


//���������б�
function UpdataApplyList(%Id)
{
	for(%i=%Id;%i<6;%i++)
	{
		if(!("TeamApplyInfo_"@ $ApplyListIndex[%i+1]).IsVisible())
		{
			("TeamApplyInfo_"@ $ApplyListIndex[%i]).setVisible(0);
			("TeamSelectedBtn_" @ %i).setVisible(0);
			break;
		}
		else
		{
			 ("TeamApplyInfo_"@ $ApplyListIndex[%i+1]).setPosition((14+118*%i),14);
			 ("TeamApplyInfo_"@ $ApplyListIndex[%i]).setPosition((14+118*(%i+1)),14);
			  %temp = $ApplyListIndex[%i];
	      $ApplyListIndex[%i]=$ApplyListIndex[%i+1];
	      $ApplyListIndex[%i+1]=%temp;
		}
	}
}


function SetAutoRefuseApply()
{
	if(ApplyAutoCheckBtn.IsStateOn())
	   $IsTeamAutoRefuseApply = true;
	else
	   $IsTeamAutoRefuseApply = false;
}


function TeamAddFriend(%Type)
{
	%SelectedId = WhoIsSelected();
	if(%SelectedId !=-1)
	{
		if(%Type==2)
		{
			%playerId = GetTeammate(%SelectedId,1);
			SptSocialMakeFirend(%playerId);
		}
		else
		{
	    %playerId=SptTeam_GetInvationPlayerByIndex(%SelectedId,%Type);
	    SptSocialMakeFirend(%playerId);
	  }
	}
	else
	{
		ShowTeamOperationTxt("��ѡ��һ�����ѽ��в���!");
	}
}

//===========================================================================================================
// �ĵ�˵��:������Ϣ�б�
// ����ʱ��:2009-6-18
// ������: Batcel
//=========================================================================================================== 

function OpenTeamInfoWnd()
{
	%SelectedId = WhoIsSelected();
	if(TeamInfoWndGui.IsVisible())
	   TeamInfoWndGui.setvisible(0);
	else
	{
		 if(%SelectedId !=-1)
		 {
		 	  %TeamId=Cache_getPlayerTeamId(SptTeam_GetInvationPlayerByIndex(%SelectedId,0));
		 	  if(%TeamId!=0)
		 	  {
		 	   TeamInfoList.clear();
		 	   SptTeam_TeamInfoRequest(%TeamId);
		 	   GameMainWndGui_Layer4.pushToBack(TeamInfoWndGui);
	       TeamInfoWndGui.setvisible(1);
	       $IsDirty++;
	      }
	   }
	   else
	    ShowTeamOperationTxt("��ѡ��Ҫ�鿴�Ķ���!");
	}
}


function CloseTeamInfoWnd()
{
	TeamInfoWndGui.setvisible(0);
	$IsDirty--;
}

//���ն�����Ϣ
function RecvTeamInfo(%Name,%FamilyID,%Level,%Index)
{
	%Family = GetFamilyText(%FamilyID);
	TeamInfoList.addRow(%Index,%Name TAB %Family TAB %Level @"��",%index);
}

//���ն�������
function RecvTeamInfoName(%Name)
{
	TeamInfoName.setText(%Name);
}

function AgreeJoinToTeam()
{
	   %SelectedId = whoIsSelected();
	   if(%SelectedId !=-1)
	   {	   	
	   	 SptTeam_AcceptInvationPlayerByIndex(%SelectedId,0);
	    // SptTeam_RemoveRefuse(TeamInviteList.getSelectedId());
	    //if(TeamInviteTypeList.getRowId(TeamInviteList.getSelectedRow())==2)
	  	//   SptTeam_AcceptAdd(TeamInviteList.getSelectedId());
	  	//else
	  	//   SptTeam_AcceptBuild(TeamInviteList.getSelectedId());	  	
	  	//TeamInviteTypeList.removeRow(TeamInviteList.getSelectedRow());
	  	//TeamInviteList.removeRow(TeamInviteList.getSelectedRow());
	  	("TeamInviteInfo_" @ $InviteListIndex[%SelectedId]).setVisible(0);
	  	TeamInfoWndGui.setvisible(0);
	  	ClearTeamInviteList();
	  }
}