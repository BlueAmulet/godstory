//===========================================================================================================
// 文档说明:显示人物信息操作
// 创建时间:2009-3-24
// 创建人: soar
//=========================================================================================================== 
$PetIconGui_Status = 2;

function ChangePKState()
{
	%player = getPlayer();
	if(%player.getPKState())
	{
		%player.setPKState(0);
		PKCloseButton.setvisible(1);
		PKOpenButton.setvisible(0);
	}
	else
	{
		%player.setPKState(1);
		PKCloseButton.setvisible(0);
		PKOpenButton.setvisible(1);
	}
}

function MinimizePlayerIcon()
{
	$PetIconGui_Status = 1;
	PlayerIconGui.setvisible(0);
	PetIconGui.setvisible(0);
	PlayerMinimizeButton.setVisible(0);
	PlayerMaxmizeButton.setVisible(1);
}

function MaxmizePlayerIcon()
{
	$PetIconGui_Status = 2;
	PlayerIconGui.setvisible(1);
	PetIconGui.setvisible(1);
	PlayerMinimizeButton.setVisible(1);
	PlayerMaxmizeButton.setVisible(0);
}

function ShowTargetPlayerWnd( %index )
{
	TargetPlayerIconGui.setVisible(%index);
	ObjectBuffWnd.setvisible(%index);
}

function IsPetIconGuiVisible()
{
	return PetIconGui.isVisible();
}

function TeamBuild()
{
	if(getTargetPlayerTeamId() $= 0)
	{
		if(IsMyselfHaveTeam() $= 0)
		{
			SptTeam_build(GetTargetPlayerId());
		}
		else
		{
			if(IsMyselfCaption() !$= 0)
			{
				SptTeam_AddRequest(GetTargetPlayerId());
			}
		}
	}
	else
	{
		if(IsTargetPlayerCaption() !$= 0)
		{
			if(IsMyselfHaveTeam() $= 0)
			{
				SptTeam_JoinRequest(getTargetPlayerTeamId());
			}
		}
	}
}

function HideTeammateBackWnd()
{
	if(TeammateBackWnd.IsVisible())
	{
		TeammateBackWnd.setVisible(0);
		//TeammateHideButton.SetText("显示");
	}
	else
	{
		TeammateBackWnd.setVisible(1);
		//TeammateHideButton.SetText("隐藏");
	}
}

function OpenSetFlagWnd()
{
	if(SetFlagWnd.IsVisible())
	{
		SetFlagWnd.setVisible(0);
	}
	else
	{
		SetFlagWnd.setVisible(1);
	}
}

function OpenPetAttack()
{
	PetDefendButton.setvisible(0);
	PetAttackButton.setvisible(1);
}

function OpenPetDefend()
{
	PetDefendButton.setvisible(1);
	PetAttackButton.setvisible(0);
}

function ShowPetWnd(%index)
{
	PetIconGui.setvisible(%index);
}

function  AddFriendById()
{
	%playerId  = GetTargetPlayerId();
	if( %playerId > 0 )
	{
		//echo( "TargetPlayerId: " @ %playerId );
		SptSocialMakeFirend(%playerId);
	}
}

function SecretChat()
{
	%playerId  = GetTargetPlayerId();
	if( %playerId > 0 )
	{		
		SptCreateChatDialog(%playerId);
	}
}

function ClosePlayerCommand()
{
	ObjectIcon_PlayerCommand.setvisible(0);
}

function OpenPlayerCommand()
{
	ObjectIcon_PlayerCommand.setvisible(1);
}

//临时看看效果用
function ChangePetAttackState()
{
	if(PetCloseButton.isvisible())
	{		
		PKCloseButton.setvisible(0);
		PKOpenButton.setvisible(1);
	}
	else
	{		
		PKCloseButton.setvisible(1);
		PKOpenButton.setvisible(0);
	}
}

function Quick_WeiShi()
{
	%player = getPlayer();
	%nPetSlot = %player.GetSpawnedPetSlot();
	Pet_WeiShi(%nPetSlot, 1);		//喂食
}

function Quick_BuQi()
{
	%player = getPlayer();
	%nPetSlot = %player.GetSpawnedPetSlot();
	Pet_WeiShi(%nPetSlot, 2);   //补气
}

function Quick_Disband()
{
	%player = getPlayer();
	%nPetSlot = %player.GetSpawnedPetSlot();
	Pet_DisbandPet(%nPetSlot);   //收回
}

function PetIconWnd_Refresh()
{
	PetIconGui.setDirty();
}