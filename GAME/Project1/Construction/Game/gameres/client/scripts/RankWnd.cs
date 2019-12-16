//===========================================================================================================
// 文档说明:排行榜
// 创建时间:2009-12-18
// 创建人: Batcel
//===========================================================================================================

$CurrentRankSeries = 0;
$CurrentRankPage = 1;
$MaxRankPage = 10;
function OpenRankWnd()
{
	if(RankWndGui.IsVisible())
	{
		RankWndGui.setVisible(0);
		$IsDirty--;
	}
	else
	{
		GameMainWndGui_Layer3.pushToBack(RankWndGui);
		RankWndGui.setVisible(1);
		$IsDirty++;
	}
}

function CloseRankWnd()
{
	if(RankWndGui.IsVisible())
	{
		RankWndGui.setVisible(0);
		$IsDirty--;
	}
}

function ShowRankTable(%index)
{
	for(%i = 0; %i <= %index; %i++)
	{
		("Rank_SeriesBtn_" @ %i).setPosition(6,63+(%i*18));
		("Rank_SeriesTab_" @ %i).setVisible(0);
		if(%i == %index)
		 ("Rank_SeriesTab_" @ %i).setVisible(1);
	}
	for(%i=6; %i>%index; %i--)
	{
		("Rank_SeriesBtn_" @ %i).setPosition(6,519-((6-%i)*18));
		("Rank_SeriesTab_" @ %i).setVisible(0);
	}
}

function ClickRankSeriesBtn(%series,%typetxt)
{
	$CurrentRankSeries = %series;
	$CurrentRankPage = 1;
	Rank_RichTxt_Sum.setContent("<t c=\'0xf6f0a6ff\' b='1'> "@ %typetxt @ "</t>");
	Rank_SkyListBox_1.clearItems();
  Rank_SkyListBox_2.ClearItems();
  Rank_SkyListBox_3.ClearItems();
	SptRequestRankSeriesInfo(%series,$CurrentRankPage-1);
	Rank_SkyTerraPeople.setVisible(1);
	Rank_Common.setVisible(0);
	RankList_CurrentPage.settext($CurrentRankPage @ "/" @ $MaxRankPage); 
}

function LastRankPage(%IsFirstPage)
{
	if($CurrentRankPage == 1) 
	   return;
	if(%IsFirstPage)
	{
		$CurrentRankPage = 1;
	}
	else
	{	  
	  $CurrentRankPage--;
  }
  Rank_CommonListBox.clearItems();
  Rank_SkyListBox_1.clearItems();
  Rank_SkyListBox_2.ClearItems();
  Rank_SkyListBox_3.ClearItems();
  if($CurrentRankPage == 1)
  {
  	Rank_SkyTerraPeople.setVisible(1);
	  Rank_Common.setVisible(0);
  }
	SptRequestRankSeriesInfo($CurrentRankSeries,$CurrentRankPage-1);
	RankList_CurrentPage.settext($CurrentRankPage @ "/" @ $MaxRankPage); 
}

function NextRankPage(%IsEndPage)
{
	if($CurrentRankPage ==$MaxRankPage )  return;
	if(%IsEndPage)
	{		
		$CurrentRankPage = $MaxRankPage; 
		 Rank_SkyTerraPeople.setVisible(0);
	   Rank_Common.setVisible(1);
	}
	else
	{
	  $CurrentRankPage++;
	}
	Rank_CommonListBox.clearItems();
	if($CurrentRankPage == 2)
	{
	  Rank_SkyTerraPeople.setVisible(0);
	  Rank_Common.setVisible(1);
	}
	SptRequestRankSeriesInfo($CurrentRankSeries,$CurrentRankPage-1);
	RankList_CurrentPage.settext($CurrentRankPage @ "/" @ $MaxRankPage); 
}

function RecvRankSeriesListInfo(%index,%grade,%name,%value,%Isonline,%revereValue,%playerId)
{
	if(%grade>0)
	  %grade1 = "<t c = '0x00ff00ff' h='1'>↑" @ %grade @"</t>";
	else if(%grade == 0)
	   %grade1 = "<t c = '0xffffffff' h='1'>—</t>";
	else
	  %grade1 = "<t c = '0xff0000ff' h='1'>↓" @ (-%grade) @" </t>";
	
	if(%Isonline)
	  %Isonline1 = "<t c='0x00ff00ff' h='1'>在线</t>";
	else
	  %Isonline1 = "<t c='0x9b9b9bff' h='1'>离线</t>";
	
	%valuetxt = "";
	if($CurrentRankSeries ==0 ||$CurrentRankSeries ==1 ||$CurrentRankSeries ==2)
	  %valuetxt = "级";
	else
	  %valuetxt = "";
	  
	%index1 = "<t c='0xffffffff' h='1'>"@ (%index+1)@ "</t>";
	%name1 = "<t c='0xffffffff' h='1'>"@ %name @"</t>";
	%value1 = "<t c='0xffffffff' h='1'>"@ %value @ %valuetxt @"</t>";
	%revereValue1 = "<t c='0xffffffff' h='1'>"@ %revereValue @"</t>";
	if(%index<3)
	  Rank_SkyListBox_1.addItem( %index1 TAB %grade1 TAB %name1 TAB %value1 TAB %Isonline1 TAB %revereValue1,%playerId);
	else if(%index <6)
	  Rank_SkyListBox_2.addItem( %index1 TAB %grade1 TAB %name1 TAB %value1 TAB %Isonline1 TAB %revereValue1,%playerId);
	else if(%index <9)
	  Rank_SkyListBox_3.addItem( %index1 TAB %grade1 TAB %name1 TAB %value1 TAB %Isonline1 TAB %revereValue1,%playerId);
	else
	  Rank_CommonListBox.addItem( %index1 TAB %grade1 TAB %name1 TAB %value1 TAB %Isonline1 TAB %revereValue1,%playerId);	  
}

function RequestReverePlayer()
{
	if($CurrentRankPage == 1)
	{
	}
	else
	{
		%playerid=Rank_CommonListBox.getSelectedId();
		SptRequestRevere(%playerid);
	}
	
}

function RequestSnootyPlayer()
{
	if($CurrentRankPage == 1)
	{
		
	}
	else
	{
		%playerid=Rank_CommonListBox.getSelectedId();
		SptRequestSnooty(%playerid);
	}
}

function GetMyselfSeatInRank(%index)
{
	if(%index == -1)
	   RankList_MyselfSeat.settext("很遗憾,您未能上榜!");
	else
	   RankList_MyselfSeat.settext("您在当前排行榜中的排位:" @(%index+1));
}