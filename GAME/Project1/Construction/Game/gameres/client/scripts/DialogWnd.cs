//===========================================================================================================
// 文档说明:对话框界面操作
// 创建时间:2009-3-16
// 创建人: soar
//=========================================================================================================== 


function Dialog_OK_Cancel::SetType(%this,%type)     //默认类型：确定取消都有，类型1：只有确定没有取消，类型2:确定取消都没有
{
	switch(%type)
	{
		case 1:
			Dialog_OK_Cancel_CANCBTN.setvisible(0);
		case 2:
			Dialog_OK_Cancel_OKBTN.setvisible(0);
			Dialog_OK_Cancel_CANCBTN.setvisible(0);
	}
}

function Dialog_OK_Cancel::SetContent(%this,%text)
{
	Dialog_OK_Cancel_Content.setcontent(%text);
}

function Dialog_OK_Cancel::SetOK(%this,%text, %cmd)
{
	Dialog_OK_Cancel_OKBTN.setvisible(1);
	Dialog_OK_Cancel_OKBTN.settext(%text);
	Dialog_OK_Cancel_OKBTN.Command = "CloseDialog_OK_Cancel();" @ %cmd;
}

function Dialog_OK_Cancel::SetCancel(%this,%text, %cmd)
{
	Dialog_OK_Cancel_CANCBTN.setvisible(1);
	Dialog_OK_Cancel_CANCBTN.settext(%text);
	Dialog_OK_Cancel_CANCBTN.Command = "CloseDialog_OK_Cancel();" @ %cmd;
}

function OpenDialog_OK_Cancel()
{
	Dialog_OK_Cancel.setvisible(1);
	BindOrUnBindSomeHotButton(0);
	$IsDirty++;
}

function CloseDialog_OK_Cancel()
{
	Dialog_OK_Cancel.setvisible(0);
	BindOrUnBindSomeHotButton(1);
	$IsDirty--;
}

function Dialog_Small_OK_Cancel::SetContent(%this,%text)
{
	Dialog_Small_OK_Cancel_Content.setcontent(%text);
}

function Dialog_Small_OK_Cancel::SetOK(%this,%text, %cmd)
{
	Dialog_Small_OK_Cancel_OKBTN.setvisible(1);
	Dialog_Small_OK_Cancel_OKBTN.settext(%text);
	Dialog_Small_OK_Cancel_OKBTN.Command = "CloseDialog_Small_OK_Cancel();" @ %cmd;
}

function Dialog_Small_OK_Cancel::SetCancel(%this,%text, %cmd)
{
	Dialog_Small_OK_Cancel_CANCBTN.setvisible(1);
	Dialog_Small_OK_Cancel_CANCBTN.settext(%text);
	Dialog_Small_OK_Cancel_CANCBTN.Command = "CloseDialog_Small_OK_Cancel();" @ %cmd;
}

function OpenDialog_Small_OK_Cancel()
{
	Dialog_Small_OK_Cancel.setvisible(1);
	BindOrUnBindSomeHotButton(0);
	$IsDirty++;
}

function CloseDialog_Small_OK_Cancel()
{
	Dialog_Small_OK_Cancel.setvisible(0);
	BindOrUnBindSomeHotButton(1);
	$IsDirty--;
}

function BindOrUnBindSomeHotButton(%index)
{
	if(%index == 0)
	{
		moveMap.Unbind( keyboard, "1"); 
    moveMap.Unbind( keyboard, "2");
    moveMap.Unbind( keyboard, "3");
    moveMap.Unbind( keyboard, "4");
    moveMap.Unbind( keyboard, "5");
    moveMap.Unbind( keyboard, "6");
    moveMap.Unbind( keyboard, "7");
    moveMap.Unbind( keyboard, "8");
    moveMap.Unbind( keyboard, "9");
    moveMap.Unbind( keyboard, "0" );
    moveMap.Unbind( keyboard, "alt 1"); 
    moveMap.Unbind( keyboard, "alt 2"); 
    moveMap.Unbind( keyboard, "alt 3"); 
    moveMap.Unbind( keyboard, "alt 4"); 
    moveMap.Unbind( keyboard, "alt 5"); 
    moveMap.Unbind( keyboard, "alt 6"); 
    moveMap.Unbind( keyboard, "alt 7"); 
    moveMap.Unbind( keyboard, "alt 8"); 
    moveMap.Unbind( keyboard, "alt 9"); 
    moveMap.Unbind( keyboard, "alt 0" ); 
    moveMap.Unbind( keyboard, a ); 
    moveMap.Unbind( keyboard, "alt a");
    moveMap.Unbind( keyboard, c);     
    moveMap.Unbind( keyboard, "alt c"); 
    moveMap.Unbind( keyboard, f); 
		moveMap.Unbind( keyboard, "alt f");
		moveMap.Unbind( keyboard, q); 
		moveMap.Unbind( keyboard, "alt q"); 
    moveMap.Unbind( keyboard, s);       
    moveMap.Unbind( keyboard, "alt s"); 
    moveMap.Unbind( keyboard, t ); 
    moveMap.Unbind( keyboard, "alt t" ); 
    moveMap.Unbind( keyboard, z );
    moveMap.Unbind( keyboard, "enter" );
    moveMap.Unbind( keyboard, tab );
    moveMap.Unbind( keyboard, pagedown );
    moveMap.Unbind( keyboard, pageup );
    moveMap.Unbind( keyboard, "F1");
    moveMap.Unbind( keyboard, "F2" );
    moveMap.Unbind( keyboard, "F3" );
    moveMap.Unbind( keyboard, "F4" );
    moveMap.Unbind( keyboard, "F5" );
    moveMap.Unbind( keyboard, "F6" );
    moveMap.Unbind( keyboard, "F10" );
    moveMap.Unbind( keyboard, "F11");
	}
	else
	{
		moveMap.bind( keyboard, "1", onShortCutSelect1 ); 
    moveMap.bind( keyboard, "2", onShortCutSelect2 );
    moveMap.bind( keyboard, "3", onShortCutSelect3 );
    moveMap.bind( keyboard, "4", onShortCutSelect4 );
    moveMap.bind( keyboard, "5", onShortCutSelect5 );
    moveMap.bind( keyboard, "6", onShortCutSelect6 );
    moveMap.bind( keyboard, "7", onShortCutSelect7 );
    moveMap.bind( keyboard, "8", onShortCutSelect8 );
    moveMap.bind( keyboard, "9", onShortCutSelect9 );
    moveMap.bind( keyboard, "0", onShortCutSelect10 );
    moveMap.bind( keyboard, "alt 1", onShortCutSelect1A ); 
    moveMap.bind( keyboard, "alt 2", onShortCutSelect2A ); 
    moveMap.bind( keyboard, "alt 3", onShortCutSelect3A ); 
    moveMap.bind( keyboard, "alt 4", onShortCutSelect4A ); 
    moveMap.bind( keyboard, "alt 5", onShortCutSelect5A ); 
    moveMap.bind( keyboard, "alt 6", onShortCutSelect6A ); 
    moveMap.bind( keyboard, "alt 7", onShortCutSelect7A ); 
    moveMap.bind( keyboard, "alt 8", onShortCutSelect8A ); 
    moveMap.bind( keyboard, "alt 9", onShortCutSelect9A ); 
    moveMap.bind( keyboard, "alt 0", onShortCutSelect10A ); 
    moveMap.bind( keyboard, a, togglePack ); 
    moveMap.bind( keyboard, "alt a", togglePack );
    moveMap.bind( keyboard, c, togglePersona );     
    moveMap.bind( keyboard, "alt c", togglePersona ); 
    moveMap.bind( keyboard, f, toggleRelation ); 
		moveMap.bind( keyboard, "alt f", toggleRelation );
		moveMap.bind( keyboard, q, toggleMission ); 
		moveMap.bind( keyboard, "alt q", toggleMission ); 
    moveMap.bind( keyboard, s, toggleSkill );       
    moveMap.bind( keyboard, "alt s", toggleSkill ); 
    moveMap.bind( keyboard, t, toggleTeam ); 
    moveMap.bind( keyboard, "alt t", toggleTeam ); 
    moveMap.bind( keyboard, z, autoPickupItem );
    moveMap.bind( keyboard, "enter", toggleChatInput );
    moveMap.bind( keyboard, tab, toggleBigViewMap );
    moveMap.bind( keyboard, pagedown, toggleCameraZoomIn );
    moveMap.bind( keyboard, pageup, toggleCameraZoomOut );
    moveMap.bind( keyboard, "F1", toggleSelfIcon );
    moveMap.bind( keyboard, "F2", toggleTeamIcon1 );
    moveMap.bind( keyboard, "F3", toggleTeamIcon2 );
    moveMap.bind( keyboard, "F4", toggleTeamIcon3 );
    moveMap.bind( keyboard, "F5", toggleTeamIcon4 );
    moveMap.bind( keyboard, "F6", toggleTeamIcon5 );
    moveMap.bind( keyboard, "F10", toggleHideGUIWndAll );
    moveMap.bind( keyboard, "F11", toggleHideName );
	}
}

function SetLoadingValue(%value)
{
	LoadingProgress.setValue(%value);
	Canvas.repaint();
}