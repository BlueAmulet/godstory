//===========================================================================================================
// 文档说明:聊天窗口操作
// 创建时间:2009-4-1
// 创建人: soar
//=========================================================================================================== 

$ChatWndX = getWord(ChatWndGui.extent,0);
$ChatWndY = getWord(ChatWndGui.extent,1);

if($ChatWndY == 478)
{
	DownDwindleButton.setActive(false);
}

if($ChatWndY == 678)
{
	UpMaqnifyButton.setActive(false);
}

if($ChatWndX == 330)
{
	RightDwindleButton.setActive(false);
}

if($ChatWndX == 520)
{
	RightMaqnifyButton.setActive(false);
}

function ChatWndLock()
{
	ChatWndUnLockButton.setVisible(0);
	ChatWndLockButton.setVisible(1);
}

function ChatWndUnLock()
{
	ChatWndUnLockButton.setVisible(1);
	ChatWndLockButton.setVisible(0);
}

function UpMaqnifyChatWnd(%ZoomMode)
{
	%ChatWndExtentX = getWord(ChatWndGui.extent,0);
  %ChatWndExtentY = getWord(ChatWndGui.extent,1);
  
  %ChatWndExtentX_Wnd1 = getWord(ChatWndGui_Wnd1.extent,0);
  %ChatWndExtentY_Wnd1 = getWord(ChatWndGui_Wnd1.extent,1);
  
  %ChatWndExtentX_Wnd2 = getWord(ChatWndGui_Wnd2.extent,0);
  %ChatWndExtentY_Wnd2 = getWord(ChatWndGui_Wnd2.extent,1);
	
	%SetPosY = getWord(ChatWndGui.getPosition(),1);
	%SetPosY_Wnd1 = getWord(ChatWndGui_Wnd1.getPosition(),1);
	%SetPosY_Wnd2 = getWord(ChatWndGui_Wnd2.getPosition(),1);
	
	%LinePosY = getWord(CleftLineBitmap.getPosition(),1);
	%LineExtentX = getWord(CleftLineBitmap.extent,0);
	
	if(%ZoomMode $= 0)
	{
		if(%ChatWndExtentY < 678)
		{
			%SetPosY -= 20;
			%ChatWndExtentY += 20;
			//%SetPosY_Wnd1 -= 20;
			%ChatWndExtentY_Wnd1 += 8;
			%SetPosY_Wnd2 += 8;
			%ChatWndExtentY_Wnd2 += 12;
		}
	}
	else if(%ZoomMode $= 1)
	{
		if(%ChatWndExtentY > 478)
		{
			%SetPosY += 20;
			%ChatWndExtentY -= 20;
			//%SetPosY_Wnd1 += 20;
			%ChatWndExtentY_Wnd1 -= 8;
			%SetPosY_Wnd2 -= 8;
			%ChatWndExtentY_Wnd2 -= 12;
		}
	}
	else if(%ZoomMode $= 2)
	{
		if(%ChatWndExtentX < 520)
		{
			%ChatWndExtentX +=19;
			%ChatWndExtentX_Wnd1 += 19;
		}
	}
	else if(%ZoomMode $= 3)
	{
		if(%ChatWndExtentX > 330)
		{
			%ChatWndExtentX -=19;
			%ChatWndExtentX_Wnd1 -= 19;
		}
	}
	
	if(%ChatWndExtentY == 478)
	{
		DownDwindleButton.setActive(false);
	}
	else
	{
		DownDwindleButton.setActive(true);
	}
	
	if(%ChatWndExtentY == 678)
  {
  	UpMaqnifyButton.setActive(false);
  }
  else
  {
  	UpMaqnifyButton.setActive(true);
  }
  
  if(%ChatWndExtentX == 330)
  {
  	RightDwindleButton.setActive(false);
  }
  else
  {
  	RightDwindleButton.setActive(true);
  }
  
  if(%ChatWndExtentX == 520)
  {
  	RightMaqnifyButton.setActive(false);
  }
  else
  {
  	RightMaqnifyButton.setActive(true);
  }
	
	ChatWndGui.SetPosition(0,%SetPosY);
  ChatWndGui.SetExtent(%ChatWndExtentX,%ChatWndExtentY); 
  
  ChatWndGui_Wnd1.SetPosition(0,%SetPosY_Wnd1);
  ChatWndGui_Wnd1.SetExtent(%ChatWndExtentX_Wnd1,%ChatWndExtentY_Wnd1);  
    
  ChatWndGui_Wnd2.SetPosition(0,%SetPosY_Wnd2);
  ChatWndGui_Wnd2.SetExtent(%ChatWndExtentX_Wnd1,%ChatWndExtentY_Wnd2);
	                
	%LinePosY = %SetPosY_Wnd2;
	CleftLineBitmap.SetPosition(0,%LinePosY-1);
	%LineExtentX = %ChatWndExtentX_Wnd1 - 20;
	CleftLineBitmap.SetExtent(%LineExtentX,2);
	
	//DownLineBitmap.SetExtent(%ChatWndExtentX,1);
	
	ChatWnd_ShowMessage2.ScrollDown();
	
	ChatWnd_ShowMessage1.Refresh();
	ChatWnd_ShowMessage2.Refresh();
	ChatWndGui.setDirty();
}

function AddChatMessage(%chunnelType, %text, %reserveID, %name)
{
	//根据综合频道设置不同, 来屏蔽需要增加到综合频道不同类的信息.
	//%bAddCommon = 0 ;
	switch(%chunnelType)
	{
    case 0://TYPE_SYS
    	ChatWnd_ShowMessage1.AddMsg(%chunnelType,%text,%reserveID,%name);
    	PlayChatChannelSound(1);
    case 1:
    	ChatWnd_ShowMessage2.AddMsg(%chunnelType,%text,%reserveID, %name);
    	PlayChatChannelSound(1);
    //case 2://TYPE_CHANNEL
    //case 3:
    //case 4:
    //case 5:
    //case 6:
    case 7:
    	ChatWnd_ShowMessage2.AddMsg(%chunnelType,%text,%reserveID,%name);
    //case 8:
    case 9:
    	ChatWnd_ShowMessage2.AddMsg(%chunnelType,%text,%reserveID,%name);
    case 10:
    	ChatWnd_ShowMessage2.AddMsg(%chunnelType,%text,%reserveID,%name);
    	PlayChatChatSound(2);
    case 11:
    	ChatWnd_ShowMessage2.AddMsg(%chunnelType,%text,%reserveID,%name);
    	PlayChatChatSound(2);
    case 13:
     	ChatWnd_ShowMessage2.AddMsg(%chunnelType,%text,%reserveID,%name);
    	PlayChatChatSound(1); 	
  }
}

function ChatPageUp2()
{
	ChatWnd_ShowMessage2.ScrollUpArrow();
}

function ChatPageDown2()
{
	ChatWnd_ShowMessage2.ScrollDownArrow();
}

function ChatPageAllDown2()
{
	ChatWnd_ShowMessage2.ScrollDown();
}

function ChatPageUp1()
{
	ChatWnd_ShowMessage1.ScrollUpArrow();
}

function ChatPageDown1()
{
	ChatWnd_ShowMessage1.ScrollDownArrow();
}

function ChatPageAllDown1()
{
	ChatWnd_ShowMessage1.ScrollDown();
}

function SelectChannelButton1::onRightClick(%this)
{
	Canvas.setTempWnd(CloseChunnel_Synthesize);
	OpenCloseChunnelWnd1();
}

function SelectChannelButton2::onRightClick(%this)
{
	Canvas.setTempWnd(CloseChunnel_SamenessCity);
	OpenCloseChunnelWnd2();
}

function SelectChannelButton3::onRightClick(%this)
{
	Canvas.setTempWnd(CloseChunnel_Team);
	OpenCloseChunnelWnd3();
}

function SelectChannelButton4::onRightClick(%this)
{
	Canvas.setTempWnd(CloseChunnel_School);
	OpenCloseChunnelWnd4();
}

function SelectChannelButton5::onRightClick(%this)
{
	Canvas.setTempWnd(CloseChunnel_Faction);
	OpenCloseChunnelWnd5();
}

function SelectChannelButton6::onRightClick(%this)
{
	Canvas.setTempWnd(CloseChunnel_UserDefined);
	OpenCloseChunnelWnd6();
}
