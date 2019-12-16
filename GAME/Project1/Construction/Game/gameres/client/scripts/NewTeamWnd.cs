//===========================================================================================================
// 文档说明:组队操作
// 创建时间:2009-5-26
// 创建人: Batcel
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


//组队跟随
function OnTeamFollow()
{
	EnableTeamFollow(1);
	TeamFollowBtn.setText("取消跟随");
	TeamFollowBtn.Command ="OffTeamFollow();";
}

//取消组队跟随
function OffTeamFollow()
{
	EnableTeamFollow(0);
	TeamFollowBtn.setText("组队跟随");
	TeamFollowBtn.Command ="OnTeamFollow();";
}

//收到/关闭组队跟随邀请
function RecvTeamFollowRequest(%On)
{
	if(%On == 1)
	{
    TeamFollowDialogGui. setVisible(1);
    $IsDirty++;
    TeamFollowBtn.setActive(1);
    TeamFollowBtn.setText("组队跟随");
	  TeamFollowBtn.Command ="OnTeamFollow();";
  }
  else
  {
  	TeamFollowBtn.setActive(0);
  }
}

//同意组队跟随
function AgreeTeamFollow()
{
	OnTeamFollow();
	TeamFollowDialogGui. setVisible(0);
  $IsDirty--;
  
}


//拒绝组队跟随
function DisagreeTeamFollow()
{
	OffTeamFollow();
	TeamFollowDialogGui. setVisible(0);
  $IsDirty--;
}


//检查组队跟随状态
function CheckTeamFollowState()
{
	 if(IsMyselfCaption())
	   return;
	 if(SptTeam_IsFollow())
	 {
	 	TeamFollowBtn.setText("组队跟随");
	  TeamFollowBtn.Command ="OnTeamFollow();";
	 }
}

//创建队伍
function CreateTeam()
{
	if(IsMyselfHaveTeam())
	{		 
	   ShowTeamOperationTxt("您现在不能创建队伍!");
	   PlayOperationErrorSound();
	}
	else
	{	
		SptTeam_BuildAlong();//向服务器发送创建队伍请求		    
	}	
}
//离开队伍请求
function LeaveTeam()
{
	if(IsMyselfHaveTeam())
	   SptTeam_LeaveRequest();	
}

//确定更改队伍名称函数
function ChangeTeamName()
{
	if(TeamNameEdit_1.getText()!$="")
	{
		SptTeam_ChangeName(TeamNameEdit_1.getText());
	}
	else
	 ShowTeamOperationTxt("请输入队伍名称!");
}

//队伍更名成功
function TeamHavaChangeName(%szName)
{
	TeamNameEdit_1.setText(%szName);
}

//解散队伍
function BreakTeam()
{
	 if(IsMyselfHaveTeam()&&IsMyselfCaption())
	  {
	  	//发送解散队伍请求
	  	SptTeam_Disband();	  	
	  }
}
//移交队长
function DisplaceTeamHeader()
{
	if(IsMyselfCaption())
	{		
		%SelectedId =whoIsSelected();
		if( %SelectedId == -1)
		{
			ShowTeamOperationTxt("请选择一个队友进行操作!");			
			PlayOperationErrorSound();
		}
		else if(%SelectedId == 0)
		{
			ShowTeamOperationTxt("只能对队友进行操作!");
			PlayOperationErrorSound();
		}
		else
		{
		   //发送移交队长的请求
		   SptTeam_ChangeLeader(GetTeammate(%SelectedId,1));
		   TeamSelectedBackground.setVisible(0);
		}
	}
	else
	{
		ShowTeamOperationTxt("您不能进行此操作!");
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

//队伍更换队长
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
	//刷新队友列表
	GetTeammateListInfo();
}

//踢出队伍
function KickOutTeam()
{
	if(IsMyselfCaption())
	{
		%SelectedId =whoIsSelected();
	  if(%SelectedId == -1 )  
	  {
	  	ShowTeamOperationTxt("请选择一个队友进行操作!");	
	    PlayOperationErrorSound();
	  }
	  else if(%SelectedId == 0)
	  {
	  		ShowTeamOperationTxt("只能对队友进行操作!");
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

//队长通过名字邀请组队
function InviteJoinTeamByName()
{ 
	if(TeamMateNameEdit.getText()!$= "")
	 { 
	 	$SocialFindPlayerAction=3;
	 	SptSocialFindPlayerByName(TeamMateNameEdit.getText());
	} 
	else
	 { 
	 	ShowTeamOperationTxt("请输入玩家名称");
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
	 		ShowTeamOperationTxt("邀请的玩家不存在或离线!");
	    PlayKindOfFailedSound(1);
	 	}
	 	else
	 	{
	 		SptTeam_AddRequest(%PlayerId);
	 	  InviteTeammateWndGui.setvisible(0);
	 	} 
} 

//队员状态下按钮
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
	     TeamFollowBtn.setText("取消跟随");
	     TeamFollowBtn.Command ="OffTeamFollow();";
	   }
	   else
	   {
	   	 TeamFollowBtn.setActive(1);	
	     TeamFollowBtn.setText("组队跟随");
	     TeamFollowBtn.Command ="OnTeamFollow();";
	   }
  }
  else
  {
    TeamFollowBtn.setActive(0);	
	  TeamFollowBtn.setText("组队跟随");
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

//队长状态下按钮
function SetTeamHeaderBtnState()
{	
	NoTeamStateGui.setVisible(0);
	HavaTeamStateGui.setVisible(1);
	TeamFollowBtn.setVisible(1);
	TeamFollowBtn.setText("组队跟随");
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
//无队伍状态下按钮
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

//自己加入到队伍了
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

//离开队伍
function ShowLeaveTeamNotify( %TeamName )
{
	//TeamMsgText.setText("您已经离开队伍"@ %TeamName);
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


//如果是队长离开则改变队长图像
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
//队友离开队伍
function TeammateLeaveTeam(%PlayerId)
{
	if(GetTeammate(0,1)==getSelectedPlayerId())
	{
		    SetTeamHeaderBtnState();
		    //OpenSetFlagButton.setVisible(1);
	}
	//刷新队友列表
	GetTeammateListInfo();
	  if(TeammateInfoGui.IsVisible())
	  {
	  	for(%i=0;%i<6;%i++)
	   {
		  ("TeamSelectedBtn_" @ %i).setVisible(("TeammateInfo_" @ %i).IsVisible());		
	   }
	  }
}

//获取队友列表信息
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
		  	("TeammateLevel_" @ %i).settext(%Level @ "级");
		  	("TeammateInfo_" @ %i).setVisible(1);
		  	if(TeammateInfoGui.IsVisible())
		  	  ("TeamSelectedBtn_" @ %i).setVisible(1);		  			  	
		  }
		  else
		    break;
	}
}

//门宗转换成中文
function GetFamilyText(%FamilyID)
{
	%Text="无";
	switch(%FamilyID)
	{
	case 0:%Text="无门宗";
	case 1:%Text="昆仑宗";
	case 2:%Text="金禅寺";
	case 3:%Text="蓬莱派";
	case 4:%Text="飞花谷";
	case 5:%Text="九幽教";
	case 6:%Text="山海宗";
	case 7:%Text="幻灵宫";
	case 8:%Text="天魔门";
	}
	return %Text;
}
//玩家加入到队伍
function PlayerJoinTeam(%PlayerId)
{	
	//TeamMsgText.setText(GetLocalPlayerName(%PlayerId) @"加入队伍");
	//TeamMsgWnd.setvisible(1);
	PlayJoinToTeamSound(); 
	//将队友添加到队友列表中
   %Name = Cache_getPlayerName(%PlayerId);
	 %Level = Cache_getPlayerLevel(%PlayerId);
	 %Family = GetFamilyText(Cache_getPlayerFamily(%PlayerId));
	 %Sex = Cache_getPlayerSex(%PlayerId);
	
	
	//TeamMateList.addRow(%PlayerId,GetLocalPlayerName(%PlayerId) TAB %Family TAB %Level @"级",TeamMateList.rowCount());
	 for(%i=0;%i<6;%i++)
	  {
	  	if(!("TeammateInfo_" @ %i).IsVisible())
	  	{
	  		("TeammateIconBitmap_" @ %i).setBitmap("gameres/gui/images/GUIWindow26_1_00"@(6+%Sex) @ ".png");
	  		("TeammateName_" @ %i).settext(%Name);
	  	  ("TeammateClass_" @ %i).settext(%Family);
	  	  ("TeammateLevel_" @ %i).settext(%Level @ "级");
	  	 	("TeammateInfo_" @ %i).setVisible(1);
	  	 	if(TeammateInfoGui.IsVisible())
	  	 	  ("TeamSelectedBtn_" @ %i).setVisible(1);
	  	 	break;
	  	}
	  }
}


//请求组队
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
	  	  ("TeamApplyLevel_" @ %i).settext(%Level @ "级");
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
	  	  ("TeamInviteLevel_" @ %i).settext(%Level @ "级");
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
// 文档说明:队伍邀请列表
// 创建时间:2009-5-27
// 创建人: Batcel
//=========================================================================================================== 
$InviteListIndex[0] =0;
$InviteListIndex[1] =1;
$InviteListIndex[2] =2;
$InviteListIndex[3] =3;
$InviteListIndex[4] =4;
$InviteListIndex[5] =5;


//清空邀请列表
function ClearTeamInviteList()
{
	  for(%i=0; %i<6; %i++)
	  {
	  	if(("TeamInviteInfo_" @ $InviteListIndex[%i]).IsVisible() )
	  	{
	  		SptTeam_RefuseInvationPlayerByIndex(%i,0);
	  		echo("是这里发的拒绝" @ %i);
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

//同意邀请
function AgreeTeamInviteRequest()
{
	 %SelectedId = whoIsSelected();
	 if(%SelectedId == -1)
	  {
	  	ShowTeamOperationTxt("请您选择要加入的队伍!");
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

//通过PlayerId同意邀请
function AgreeTeamInviteRequestById(%playerId)
{
	echo( "playerid:----------------------------------" @ %playerId );
	 %SelectedId = SptTeam_GetInvationIndex(%playerId,0);
	 if(%SelectedId == -1)
	  {
	  	ShowTeamOperationTxt("无效的操作目标!");
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

//通过playerId拒绝邀请
function RefuseTeamInviteRequestById(%playerId)
{
	%SelectedId = SptTeam_GetInvationIndex(%playerId,1);
	
	if(%SelectedId == -1)
	{
		ShowTeamOperationTxt("无效的操作目标!");
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


//拒绝邀请
function RefuseTeamInviteRequest()
{
	%SelectedId = whoIsSelected();
	if(%SelectedId == -1)
	{
		ShowTeamOperationTxt("请选择操作对象!");
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

//更新邀请列表
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
// 文档说明:队伍申请列表
// 创建时间:2009-5-27
// 创建人: Batcel
//=========================================================================================================== 
$ApplyListIndex[0] =0;
$ApplyListIndex[1] =1;
$ApplyListIndex[2] =2;
$ApplyListIndex[3] =3;
$ApplyListIndex[4] =4;
$ApplyListIndex[5] =5;

//通过PlayerId同意申请加入队伍
function AgreeTeamApplyRequestById( %PlayerId )
{
	 %SelectedId = SptTeam_GetInvationIndex( %PlayerId,1 );
	if(%SelectedId == -1)
	 {
	 	ShowTeamOperationTxt("无效的操作目标!");
	  PlayOperationErrorSound();
	 }
	 else
	 {
	 	 if(TeammateInfo_5.IsVisible())
	    {
		   ShowTeamOperationTxt("队伍人数已满!");
	     PlayKindOfFailedSound(1);
	    }
	    else
	    {
	    	 //SptTeam_RemoveRefuse(TeamApplyList.getSelectedId());
	 	     //SptTeam_AcceptJoin(TeamApplyList.getSelectedId());	 	    
	 	     //TeamApplyList.removeRow(TeamApplyList.getSelectedRow());
	 	     
	 	     SptTeam_AcceptInvationPlayerByIndex(%SelectedId,1);
	 	     TeamSelectedBackground.setVisible(0);
	 	     //这里还需更新列表.
	 	     	("TeamApplyInfo_"@ $ApplyListIndex[%SelectedId]).setVisible(0);
	 	     	("TeamSelectedBtn_" @ %SelectedId).setStateOn(0);
	 	      UpdataApplyList(%SelectedId);
	 	  }
	 }
}


//同意申请加入队伍
function AgreeTeamApplyRequest()
{
	 %SelectedId = whoIsSelected();
	if(%SelectedId == -1)
	 {
	 	ShowTeamOperationTxt("请您选择一个玩家进行操作!");
	  PlayOperationErrorSound();
	 }
	 else
	 {
	 	 if(TeammateInfo_5.IsVisible())
	    {
		   ShowTeamOperationTxt("队伍人数已满!");
	     PlayKindOfFailedSound(1);
	    }
	    else
	    {
	    	 //SptTeam_RemoveRefuse(TeamApplyList.getSelectedId());
	 	     //SptTeam_AcceptJoin(TeamApplyList.getSelectedId());	 	    
	 	     //TeamApplyList.removeRow(TeamApplyList.getSelectedRow());
	 	     
	 	     SptTeam_AcceptInvationPlayerByIndex(%SelectedId,1);
	 	     TeamSelectedBackground.setVisible(0);
	 	     //这里还需更新列表.
	 	     	("TeamApplyInfo_"@ $ApplyListIndex[%SelectedId]).setVisible(0);
	 	     	("TeamSelectedBtn_" @ %SelectedId).setStateOn(0);
	 	      UpdataApplyList(%SelectedId);
	 	  }
	 }
}

//通过PlayerId拒绝申请加入队伍
function DisagreeTeamApplyRequestById( %playerId )
{
	%SelectedId = SptTeam_GetInvationIndex( %PlayerId,1 );
	if(%SelectedId == -1)
	 {
	 	ShowTeamOperationTxt("请您选择一个玩家进行操作!");
	  PlayOperationErrorSound();
	 }
	 else
	 {
	 	//SptTeam_RemoveRefuse(TeamApplyList.getSelectedId());	 	
	 	//SptTeam_SendRefused(TeamApplyList.getSelectedId(),1);	 	
	 	//TeamApplyList.removeRow(TeamApplyList.getSelectedRow());
	 	SptTeam_RefuseInvationPlayerByIndex(%SelectedId,1);
	 	TeamSelectedBackground.setVisible(0);
	 	//这里还需更新列表....
	 	("TeamApplyInfo_"@ $ApplyListIndex[%SelectedId]).setVisible(0);
	 	UpdataApplyList(%SelectedId);
	 }
}

//拒绝申请加入队伍
function DisagreeTeamApplyRequest()
{
	%SelectedId = whoIsSelected();
	if(%SelectedId == -1)
	 {
	 	ShowTeamOperationTxt("请您选择一个玩家进行操作!");
	  PlayOperationErrorSound();
	 }
	 else
	 {
	 	//SptTeam_RemoveRefuse(TeamApplyList.getSelectedId());	 	
	 	//SptTeam_SendRefused(TeamApplyList.getSelectedId(),1);	 	
	 	//TeamApplyList.removeRow(TeamApplyList.getSelectedRow());
	 	SptTeam_RefuseInvationPlayerByIndex(%SelectedId,1);
	 	TeamSelectedBackground.setVisible(0);
	 	//这里还需更新列表....
	 	("TeamApplyInfo_"@ $ApplyListIndex[%SelectedId]).setVisible(0);
	 	UpdataApplyList(%SelectedId);
	 }
}

//清空申请列表
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

//别人拒绝了我的请求
function RefusedApplyRequest(%Id,%Type)
{
	
	%Name = Cache_getPlayerName(%Id);
	if(%Type==1)
	   ShowTeamOperationTxt(%Name @ "拒绝了您的申请请求!");
	else
	   ShowTeamOperationTxt(%Name @ "拒绝了您的邀请请求!");	   
	  PlayKindOfFailedSound(1);
}

//系统自动回复的拒绝请求
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


//更新申请列表
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
		ShowTeamOperationTxt("请选择一个队友进行操作!");
	}
}

//===========================================================================================================
// 文档说明:队伍信息列表
// 创建时间:2009-6-18
// 创建人: Batcel
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
	    ShowTeamOperationTxt("请选择要查看的队伍!");
	}
}


function CloseTeamInfoWnd()
{
	TeamInfoWndGui.setvisible(0);
	$IsDirty--;
}

//接收队伍信息
function RecvTeamInfo(%Name,%FamilyID,%Level,%Index)
{
	%Family = GetFamilyText(%FamilyID);
	TeamInfoList.addRow(%Index,%Name TAB %Family TAB %Level @"级",%index);
}

//接收队伍名称
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