//===========================================================================================================
// 文档说明:小地图操作
// 创建时间:2009-3-23
// 创建人: soar
//=========================================================================================================== 

function ShowLineList()
{
	%index = ServerListNum();
	
 	for(%i = 0; %i < %index; %i++)
	{
		%LineName = GetLineId(%i);
		BirdLine_TextList.addRow(%i, "线路" @ %LineName);
	}
}
BirdShowLine.setText("线路" @ $Pref::Net::Line);

function SetBirdViewMapMessage(%index)
{
	BirdShowLine.setText("线路" @ %index);
}

function MinimizeBirdViewMap()
{

	if((BirdViewMap.Scale-0.1)>BirdViewMap.MinScale)
	    BirdViewMap.Scale-=0.1;
}

function MaxmizeBirdViewMap()
{
	
	if((BirdViewMap.Scale+0.1)<BirdViewMap.MaxScale)
	     BirdViewMap.Scale+=0.1;
}

function OpenLineSelectWnd()
{
	if(BirdViewMap_LineSelectWnd.IsVisible())
	{
		BirdViewMap_LineSelectWnd.SetVisible(0);
		BirdLine_TextList.Clear();
	}
	else
	{
		BirdViewMap_LineSelectWnd.SetVisible(1);
		ShowLineList();
	}
}                       

function BirdLine_TextList::SelectIt()
{
	%name = BirdLine_TextList.getRowTextById(BirdLine_TextList.getSelectedId());
	%id = BirdLine_TextList.getSelectedId();
	OpenLineSelectWnd();
	BirdShowLine.setText(%name);
	echo( "LineId" @ %id );
	SptSelectLineInGame(%id + 1);
}

function ShowMapName(%MapName)
{	
	BirdMapName.settext(%MapName);
}

function ShowTransportTexture( %ZoneId )
{
	TransportTriggerShow.IsStartChangeAlpha = 0;
	%mapName = getViewMapData(%ZoneId,2);
	if(%mapName !$= "")
	{
		TransportTriggerShow.showtransporttexture("GUIWindow99_1_010");
	  TransportTriggerGui.setVisible(1);
	  TransportTriggerText.setText( "前往 " @ %mapName );
	  schedule(2000,0,"ChangeTransportTextureAlpha");	
	}
}

function ChangeTransportTextureAlpha()
{	
	TransportTriggerShow.IsStartChangeAlpha=1;
	schedule(3000,0,"CloseTranSportScreenShow");
}

function CloseTranSportScreenShow()
{
	TransportTriggerShow.IsStartChangeAlpha=0;
	TransportTriggerGui.setVisible(0);
}

function ShowAreaTriggerTexture(%Id)
{
	TransportTriggerGui.setVisible(0);
	AreaTriggerGui.setVisible(1);	
	schedule(5000,0,"ChangeTextureAlpha");
}

function ChangeTextureAlpha()
{	
	AreaTriggerShow.IsStartChangeAlpha=1;
	schedule(3000,0,"CloseScreenShow");
}
function CloseScreenShow()
{
	AreaTriggerShow.IsStartChangeAlpha=0;
	AreaTriggerGui.setVisible(0);
}

function HideBirdMap()
{
	if(BirdViewHideGui.IsVisible())
	{
		BirdViewHideGui.setVisible(0);
	  BirdMapGui.setVisible(0);
	}
	else
	{
	 BirdViewHideGui.setVisible(1);
	 BirdMapGui.setVisible(1);
	}
}

function UpdateMapTime(%Time)
{
	BirdShowTime.setText(%Time);
}
