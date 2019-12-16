//===========================================================================================================
// 文档说明:服务器选择界面操作
// 创建时间:2009-3-3
// 创建人: soar
//===========================================================================================================

ShowServerMassage.setText("线路"@ $Pref::Net::Line);
ShowServerMassage2.setText("线路"@ $Pref::Net::Line);

$SelectPlayerIndex = 0;
function OpenServerSelectWnd()
{
	if(ServerSelectWnd.IsVisible())
	{
		ServerSelectWnd.SetVisible(0);		
		ServerSelectLeftWnd_Texture2.setvisible(0);
	}
	else
	{
		ServerSelectWnd.SetVisible(1);	
		ServerSelectLeftWnd_Texture2.setvisible(1);
	}
}

function ShowServerList()
{
	%index = ServerListNum();
	
	LineSelectList1.ClearItems();
	LineSelectList2.ClearItems();
  for(%i = 0; %i < %index; %i++)
	{
		%ServerName = GetLineId(%i);
		%imageGood = "<i s='gameres/gui/images/GUIlogin03_1_005.png'/>";
		%imageCrowd = "<i s='gameres/gui/images/GUIlogin03_1_006.png'/>";
		%imageVeryCrowd = "<i s='gameres/gui/images/GUIlogin03_1_007.png'/>";
		
		%Server = "<t h='1' c='0xffffffff'>" @ %ServerName @ "</t>";	
		%text = %Server TAB %imageGood;	
		LineSelectList1.addItem(%text,%i); 
		LineSelectList2.addItem(%text,%i);
		LineSelectList1.setCommand("LineSelectList1.SelectIt();",%i);
		LineSelectList2.setCommand("LineSelectList2.SelectIt();",%i);          
	}
}

function LineSelectList1::SelectIt(%this)
{
	%selectId = LineSelectList1.getselectedid();
	%ServerName = GetLineId(%selectId);
	SelectLine(%ServerName);
	ChangeShowServerMassage(%ServerName);
	OpenServerSelectWnd();
}

function LineSelectList2::SelectIt(%this)
{
	%selectId = LineSelectList2.getselectedid();
	%ServerName = GetLineId(%selectId);
	SelectLine(%ServerName);
	ChangeShowServerMassage(%ServerName);
	//OpenPlayerInfoWnd();
}

function ChangeShowServerMassage(%ServerName)
{
	ShowServerMassage.setText("线路"@ %ServerName);
	ShowServerMassage2.setText("线路"@ %ServerName);
}

function ShowPlayerList()
{
	PlayerButton3.setVisible(0);
	PlayerButton4.setVisible(0);
	SelectObjectView.clearPlayer();
	for(%i = 0; %i < 5; %i++)
	{	
		("PlayerLevel" @ %i).setVisible(0);
		("PlayerName" @ %i).setVisible(0);
		("PlayerFamilySign" @ %i).setVisible(0);
		("PlayerFamilyName" @ %i).setVisible(0);
		("PlayerSoulName" @ %i).setVisible(0);
		("PlayerSoulLevel" @ %i).setVisible(0);
		//("Name" @ %i).setText("");
		("PlayerHint" @ %i).setText("[ 创建角色 ]");
	}
	%index = PlayerListNum();
  for(%i = 0; %i < %index; %i++)
	{
		%PlayerName = GetPlayerName(%i);
		%PlayerLevel = SptGetPlayerLevel(%i);
		%PlayerFamilyName = SptGetPlayerFamily(%i);
		("PlayerButton" @ %i).setVisible(1);
		("PlayerLevel" @ %i).setVisible(1);
		("PlayerName" @ %i).setVisible(1);
		("PlayerFamilyName" @ %i).setVisible(1);
		("PlayerSoulName" @ %i).setVisible(1);
		("PlayerSoulLevel" @ %i).setVisible(1);
		("PlayerName" @ %i).setText(%PlayerName);
		("PlayerLevel" @ %i).setText(%PlayerLevel @ "级");
		("PlayerHint" @ %i).setText("");
		switch(%PlayerFamilyName)
		{
			case 0:
				%text = "新手";
				("PlayerSoulName" @ %i).setVisible(0);
				("PlayerSoulLevel" @ %i).setVisible(0);
			case 1:
				%text = "昆仑宗";
				%address = "gameres/gui/images/GUIlogin03_1_014.png";
			case 2:
				%text = "金禅寺";
				%address = "gameres/gui/images/GUIlogin03_1_015.png";
			case 3:
				%text = "蓬莱派";
				%address = "gameres/gui/images/GUIlogin03_1_016.png";
			case 4:
				%text = "飞花谷";
				%address = "gameres/gui/images/GUIlogin03_1_017.png";
			case 5:
				%text = "九幽教";
				%address = "gameres/gui/images/GUIlogin03_1_018.png";
			case 6:
				%text = "山海宗";
				%address = "gameres/gui/images/GUIlogin03_1_019.png";
			case 7:
				%text = "幻灵宫";
				%address = "gameres/gui/images/GUIlogin03_1_020.png";
			case 8:
				%text = "天魔门";
				%address = "gameres/gui/images/GUIlogin03_1_021.png";
		}	
		("PlayerFamilyName" @ %i).setText(%text);
		if(%PlayerFamilyName > 0)
		{
			("PlayerListBckImg" @ %i).setbitmap(%address);
		}
		else
		{
			("PlayerListBckImg" @ %i).setbitmap("gameres/gui/images/GUIlogin03_1_003.png");
		}
	}
	if(PlayerListNum() > 0)
	{
		 if($SelectPlayerIndex > PlayerListNum()-1)
		 		$SelectPlayerIndex = PlayerListNum()-1;
		 
		("PlayerButton"@ $SelectPlayerIndex).setStateOn(true);
		SelectPlayer( $SelectPlayerIndex );
	}
	else
	{
		SelectObjectView.setScene(Scene_3);
	}
	
	SetPlayerButtonState(%index);
}

function BackToMainMenu()
{
	SptClearAccountServerCheck();
	ShowMessage.setText("");
  InputInfoWnd1.setVisible(1);
	ServerNoticeWnd.setVisible(0);
	ServerSelectFace.setVisible(0);
	LoginCurrentServerCui.setVisible(1);
	ChangeSeverBtn.setVisible(1);
	
	if (isObject( MainMenuGui ))
	{
	  Canvas.setContent( MainMenuGui );   
	}
      
 
}

function ShowDialog()
{
	Canvas.pushDialog(DialogWnd_2);
}

function AddShowPlayerDistance()
{
	PlayerAddButton.setvisible(0);
	PlayerDesButton.setvisible(1);
}

function DesShowPlayerDistance()
{
	PlayerAddButton.setvisible(1);
	PlayerDesButton.setvisible(0);
}

function SetPlayerButtonState(%Playerindex)
{
	%i = 0;
	for(  ; %i < %Playerindex ; %i++)
	{		
		("NoPlayerButton" @ %i).setvisible(0);
		("PlayerButton" @ %i).setvisible(1);
	}
	for( ; %i < 5 ; %i++)
	{
		("NoPlayerButton" @ %i).setvisible(1);
		("PlayerButton" @ %i).setvisible(0);
	}
	
	
	
	/*if(%Playerindex == 0)
	{
		NoPlayerButton0.setvisible(1);
		PlayerButton0.setvisible(0);
		NoPlayerButton1.setvisible(1);
		PlayerButton1.setvisible(0);
		NoPlayerButton2.setvisible(1);
		PlayerButton2.setvisible(0);
	}
	else if(%Playerindex == 1)
	{
		NoPlayerButton0.setvisible(0);
		PlayerButton0.setvisible(1);
		NoPlayerButton1.setvisible(1);
		PlayerButton1.setvisible(0);
		NoPlayerButton2.setvisible(1);
		PlayerButton2.setvisible(0);
	}
	else if(%Playerindex == 2)
	{
		NoPlayerButton0.setvisible(0);
		PlayerButton0.setvisible(1);
		NoPlayerButton1.setvisible(0);
		PlayerButton1.setvisible(1);
		NoPlayerButton2.setvisible(1);
		PlayerButton2.setvisible(0);
	}
	else
	{
		NoPlayerButton0.setvisible(0);
		PlayerButton0.setvisible(1);
		NoPlayerButton1.setvisible(0);
		PlayerButton1.setvisible(1);
		NoPlayerButton2.setvisible(0);
		PlayerButton2.setvisible(1);
	}*/
}

function SptSelectPlayer(%index)
{
  if($SelectPlayerIndex != %index) 
  {    
  	$SelectPlayerIndex = %index;
    SelectPlayer(%index);
  }
}