//===========================================================================================================
// 文档说明:组队操作
// 创建时间:2009-4-28
// 创建人: soar
//=========================================================================================================== 

$PlayerId = 0;

function RequestToTeam(%ID,%Name)
{
	$PlayerId = %ID;
	//%playerid = getlocalplayer(%ID);
	TeamRequestMessage.settext(%Name);
	TeamWndGui.setVisible(1);
}

function TeamBuildOK()
{
	if(IsMyselfHaveTeam() $= 0)
	{
		SptTeam_AcceptBuild($PlayerId);
	}
	else
	{
		SptTeam_AcceptJoin($PlayerId);
	}
	TeamWndGui.setVisible(0);
}

function ShowTeamBuildOK( %TeamName )
{
	TeamShowMessage.setText("您已经加入队伍：" @ %TeamName);
	TeamMassageWnd.setVisible(1);
	if(SynthesizeClose_3.IsStateOn())
	{
		SptChangeChatChannelType(7);
	}
	TeammateIconGui.setVisible(1);
	if(IsMyselfCaption() !$= 0)
	{
		OpenSetFlagButton.setVisible(1);
		for(%i = 0; %i < 5; %i++)
		{
			("KickTeammate_" @ %i).setVisible(1);
		}
		
	}
}

function ShowLeaveTeamNotify( %TeamName )
{
	TeamShowMessage.setText("您已经离开队伍：" @ %TeamName);
	TeamMassageWnd.setVisible(1);
	TeammateIconGui.setVisible(0);
	OpenSetFlagButton.setVisible(0);
	for(%i = 0; %i < 5; %i++)
	{
		("KickTeammate_" @ %i).setVisible(0);
	}
}

function ChangeCaptain()
{
	if(IsMyselfCaption() !$= 0)
	{
		OpenSetFlagButton.setVisible(1);
		for(%i = 0; %i < 5; %i++)
		{
			("KickTeammate_" @ %i).setVisible(1);
		}
	}
}

function CloseTeamMassageWnd()
{
	TeamMassageWnd.setVisible(0);
}

function CloseTeamWndGui()
{
	TeamWndGui.setVisilbe(0);
}