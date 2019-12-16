//===========================================================================================================
// 文档说明:频道屏蔽操作
// 创建时间:2009-4-13
// 创建人: soar
//=========================================================================================================== 

function OpenCloseChunnelWnd1()
{
	if(!CloseChunnel_Synthesize.IsVisible())
	{
		CloseChunnel_Synthesize.setVisible(1);
		CloseChunnel_SamenessCity.setVisible(0);
		CloseChunnel_Team.setVisible(0);
		CloseChunnel_School.setVisible(0);
		CloseChunnel_Faction.setVisible(0);
		CloseChunnel_UserDefined.setVisible(0);
	}
}

function OpenCloseChunnelWnd2()
{
	if(!CloseChunnel_SamenessCity.IsVisible())
	{
		CloseChunnel_Synthesize.setVisible(0);
		CloseChunnel_SamenessCity.setVisible(1);
		CloseChunnel_Team.setVisible(0);
		CloseChunnel_School.setVisible(0);
		CloseChunnel_Faction.setVisible(0);
		CloseChunnel_UserDefined.setVisible(0);
	}
}

function OpenCloseChunnelWnd3()
{
	if(!CloseChunnel_Team.IsVisible())
	{
		CloseChunnel_Synthesize.setVisible(0);
		CloseChunnel_SamenessCity.setVisible(0);
		CloseChunnel_Team.setVisible(1);
		CloseChunnel_School.setVisible(0);
		CloseChunnel_Faction.setVisible(0);
		CloseChunnel_UserDefined.setVisible(0);
	}
}

function OpenCloseChunnelWnd4()
{
	if(!CloseChunnel_School.IsVisible())
	{
		CloseChunnel_Synthesize.setVisible(0);
		CloseChunnel_SamenessCity.setVisible(0);
		CloseChunnel_Team.setVisible(0);
		CloseChunnel_School.setVisible(1);
		CloseChunnel_Faction.setVisible(0);
		CloseChunnel_UserDefined.setVisible(0);
	}
}

function OpenCloseChunnelWnd5()
{
	if(!CloseChunnel_Faction.IsVisible())
	{
		CloseChunnel_Synthesize.setVisible(0);
		CloseChunnel_SamenessCity.setVisible(0);
		CloseChunnel_Team.setVisible(0);
		CloseChunnel_School.setVisible(0);
		CloseChunnel_Faction.setVisible(1);
		CloseChunnel_UserDefined.setVisible(0);
	}
}

function OpenCloseChunnelWnd6()
{
	if(!CloseChunnel_UserDefined.IsVisible())
	{
		CloseChunnel_Synthesize.setVisible(0);
		CloseChunnel_SamenessCity.setVisible(0);
		CloseChunnel_Team.setVisible(0);
		CloseChunnel_School.setVisible(0);
		CloseChunnel_Faction.setVisible(0);
		CloseChunnel_UserDefined.setVisible(1);
	}
}

function CloseSynthesize()
{
	if(CloseChunnel_Synthesize.IsVisible())
	{
		CloseChunnel_Synthesize.setVisible(0);
	}
}

function CloseSamenessCity()
{
	if(CloseChunnel_SamenessCity.IsVisible())
	{
		CloseChunnel_SamenessCity.setVisible(0);
	}
}

function CloseTeam()
{
	if(CloseChunnel_Team.IsVisible())
	{
		CloseChunnel_Team.setVisible(0);
	}
}

function CloseSchool()
{
	if(CloseChunnel_School.IsVisible())
	{
		CloseChunnel_School.setVisible(0);
	}
}

function CloseFaction()
{
	if(CloseChunnel_Faction.IsVisible())
	{
		CloseChunnel_Faction.setVisible(0);
	}
}

function CloseUserDefined()
{
	if(CloseChunnel_UserDefined.IsVisible())
	{
		CloseChunnel_UserDefined.setVisible(0);
	}
}