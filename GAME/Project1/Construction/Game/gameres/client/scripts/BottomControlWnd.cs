//===========================================================================================================
// 文档说明:底部控制栏操作
// 创建时间:2009-3-19
// 创建人: soar
//=========================================================================================================== 

$BagState = 0;

function OpenSystemMenuWnd()
{
	PlayOpenOrCloseGuiSound();
	if(SystemMenuGui.IsVisible())
	{
		SystemMenuGui.setvisible(0);
		$IsDirty--;
	}
	else
	{
		SystemMenuGui.setvisible(1);
		GameMainWndGui_Layer3.PushToBack(SystemMenuGui);
		$IsDirty++;
	}
}

function CloseSystemMenu()
{
	if(SystemMenuGui.IsVisible())
	{
		SystemMenuGui.setvisible(0);
		$IsDirty--;
	}	
}

function OpenBag()
{
	PlayOpenOrCloseGuiSound();
	if($BagState $= 0)
	{
		if(SingleBagWndGui.IsVisible())
			CloseSingleBag();
		else
			OpenSingleBag();
	}
	else
	{
		if(AllBagWndGui.IsVisible())
			CloseAllBag();
		else
			OpenAllBag();
	}
}

function OpenTeamWnd()
{
	PlayOpenOrCloseGuiSound();
	if(NewTeamWndGui.IsVisible())
	{
	   NewTeamWndGui.setVisible(0);
	   $IsDirty--;
	}	   
	else
	{
		if(IsMyselfHaveTeam())
    {
  	  if(IsMyselfCaption())
  	     SetTeamHeaderBtnState();
  	  else
  	     SetTeamMemberBtnState();
    }
    else
    {
  	   SetNoTeamBtnState();
    }
    GameMainWndGui_Layer3.pushToBack(NewTeamWndGui);
	  NewTeamWndGui.setVisible(1);
	  $IsDirty++;
	 }
}


function onShortCutPanel(%index)
{
	 switch(%index)
	 {
	 	  case 1:PanelShortCut_1.onShortCut();
	 	  case 2:PanelShortCut_2.onShortCut();
	 	  case 3:PanelShortCut_3.onShortCut();
	 	  case 4:PanelShortCut_4.onShortCut();
	 	  case 5:PanelShortCut_5.onShortCut();
	 	  case 6:PanelShortCut_6.onShortCut();
	 	  case 7:PanelShortCut_7.onShortCut();
	 	  case 8:PanelShortCut_8.onShortCut();
	 	  case 9:PanelShortCut_9.onShortCut();
	 	  case 10:PanelShortCut_10.onShortCut();
	 	  default:return;
	 } 
}

function ChangeTheWnd_In()
{
	BottomControlWnd_Bitmap1.setvisible(0);
	BottomControlWnd_Bitmap2.setvisible(1);
	ItemBagButton.setvisible(0);
	OpenPlayerInfoButtom.setvisible(0);
	OpenPetInfoButtom.setvisible(0);
	OpenTroopInfoButtom.setvisible(0);
	OpenSkillInfoButtom.setvisible(0);
	OpenMissionInfoButtom.setvisible(0);
	OpenRelationInfoButtom.setvisible(0);
	OpenHonorInfoButtom.setvisible(0);
	FuDiInfoButtom.setvisible(0);
	OpenSystemInfoButtom.setvisible(0);
	LeftInButton.setvisible(0);
	RightInButton.setvisible(0);
	PlayerSoulButtom.setvisible(0);
	MagicCimeliaButtom.setvisible(0);
	LeftOutButton.setvisible(1);
	RightOutButton.setvisible(1);
}

function ChangeTheWnd_Out()
{
	BottomControlWnd_Bitmap1.setvisible(1);
	BottomControlWnd_Bitmap2.setvisible(0);
	ItemBagButton.setvisible(1);
	OpenPlayerInfoButtom.setvisible(1);
	OpenPetInfoButtom.setvisible(1);
	OpenTroopInfoButtom.setvisible(1);
	OpenSkillInfoButtom.setvisible(1);
	OpenMissionInfoButtom.setvisible(1);
	OpenRelationInfoButtom.setvisible(1);
	OpenHonorInfoButtom.setvisible(1);
	FuDiInfoButtom.setvisible(1);
	OpenSystemInfoButtom.setvisible(1);
	LeftInButton.setvisible(1);
	RightInButton.setvisible(1);
	PlayerSoulButtom.setvisible(1);
	MagicCimeliaButtom.setvisible(1);
	LeftOutButton.setvisible(0);
	RightOutButton.setvisible(0);
}
$frist = 1;
function OpenRelationWnd()
{
	if(RelationWndGui.isvisible())
	{
		RelationWndGui.setvisible(0);
		$IsDirty--;
	}
	else
	{
		GameMainWndGui_Layer3.PushToBack(RelationWndGui);
		RelationWndGui.setvisible(1);
		$IsDirty++;
		if($frist == 1)
		{
			SptSocialRequest();
			//$frist++;
		}
	}
}

function HideBottomCtrlButton()
{
 %pos= MissionTraceWnd.getPosition();
 %pos1= UpgradeHintButton.getPosition();
 %pos2= PetUpgradeHintButton.getPosition();
 %pos3= FriendChatIcon.getPosition();
 %pos4= PlayerMessageBox.getPosition();
 %pos5= SysMessageBox.getPosition();
 if(BottomControlButtonGui.IsVisible())
 {
 	BottomControlButtonGui.setVisible(0); 	
 	MissionTraceWnd.setPosition((getWord(%pos,0)+48),200);
 	UpgradeHintButton.setPosition((getWord(%pos1,0)+48),getWord(%pos1,1));
 	PetUpgradeHintButton.setPosition((getWord(%pos2,0)+48),getWord(%pos2,1));
 	FriendChatIcon.setPosition((getWord(%pos3,0)+48),getWord(%pos3,1));
 	PlayerMessageBox.setPosition((getWord(%pos4,0)+48),getWord(%pos4,1));
 	SysMessageBox.setPosition((getWord(%pos5,0)+48),getWord(%pos5,1));
 }
 else
 {
 	BottomControlButtonGui.setVisible(1); 	
 	MissionTraceWnd.setPosition((getWord(%pos,0)-48),200);
 	UpgradeHintButton.setPosition((getWord(%pos1,0)-48),getWord(%pos1,1));
 	PetUpgradeHintButton.setPosition((getWord(%pos2,0)-48),getWord(%pos2,1));
 	FriendChatIcon.setPosition((getWord(%pos3,0)-48),getWord(%pos3,1));
 	PlayerMessageBox.setPosition((getWord(%pos4,0)-48),getWord(%pos4,1));
 	SysMessageBox.setPosition((getWord(%pos5,0)-48),getWord(%pos5,1));
 }
}