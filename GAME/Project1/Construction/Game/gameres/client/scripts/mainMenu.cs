//===========================================================================================================
// �ĵ�˵��:���������
// ����ʱ��:2009-2-27
// ������: soar
//=========================================================================================================== 
function OpenMiniKeyboard()
{
	if(MiniKeyboardGui.IsVisible())
	{
		MiniKeyboardGui.setvisible(0);
	}
	else
	{
		MiniKeyboardGui.setvisible(1);
	}
}

function AccountInsertChar(%c)
{
    if(PlayerID.isFirstResponder())
    {
        PlayerID.InsertString(%c);
    }
    else
    {
        PlayerPassword.InsertString(%c);
    }
}

function AccountDeleteChar()
{
    if(PlayerID.isFirstResponder())
    {
        PlayerID.DeleteString();
    }
    else
    {
        PlayerPassword.DeleteString();
    }
}

function AccountChangeInteracting()
{
    if(PlayerID.isFirstResponder())
    {
        PlayerPassword.setFirstResponder();
    }
    else
    {
        PlayerID.setFirstResponder();
    } 
}

function fakeLogin()
{
	if(PlayerId.getText() $= "" || PlayerPassword.getText() $= "")
	{
		ShowMessage.setText("�������ʺź����룡");
	}
	else
	{
	  SptAccountServerCheck(PlayerId.getText(),PlayerPassword.getText());
	  ShowMessage.setText("��¼��.........");
		Canvas.pushDialog(DialogWnd_1);
	}
}

//��ĵ�½
function ParseLogin()
{
	if(PlayerId.getText() $= "" || PlayerPassword.getText() $= "")
	{
		ShowMessage.setText("�������ʺź����룡");
	}
	else
	{
		SptConnectAccountServer(PlayerId.getText(),PlayerPassword.getText());
		ShowMessage.setText("��¼��.........");
		Canvas.pushDialog(DialogWnd_1);
		//DialogShowMessage_1.setText("��¼��.........");
	}
}

function ShowRegisterPage()
{
	shellExecute("http://www.firerain.com","","");
}

function ShowHomePage()
{
	shellExecute("http://www.firerain.com","","");
}

function ShowServerSelectWnd()
{
	if (isObject( ServerSelectGui ))
			Canvas.setContent(ServerSelectGui);
	LineSelectList1.clear();
	LineSelectList2.clear();
	ShowServerList();
	ShowPlayerList();
	Canvas.popDialog(DialogWnd_1);
}

function OpenServerTextWnd()
{
	if(ServerTextWnd.IsVisible())
	{
		ServerTextWnd.SetVisible(0);
	}
	else
	{
		ServerTextWnd.SetVisible(1);
	}
}


function OpenServerNotice(%error)
{	
	if(%error ==0)
	{
		InputInfoWnd1.setVisible(0);
		ServerSelectFace.setVisible(0);
		ServerNoticeWnd.setVisible(1);					
	}
	else
	{
		
	}
}

function OpenServerSelect()
{
	  InputInfoWnd1.setVisible(0);
		ServerNoticeWnd.setVisible(0);
		ChangeSeverBtn.setVisible(0);
		ServerSelectFace.setVisible(1);	
		LoginCurrentServerCui.setVisible(0);	
}

function ServerSelectedEnd()
{
	  ShowMessage.setText("");  
	  ServerSelectFace.setVisible(0);
	  ServerNoticeWnd.setVisible(0);
	  ChangeSeverBtn.setVisible(1);		
		InputInfoWnd1.setVisible(1);
		LoginCurrentServerCui.setVisible(1);
}

function IniDefaultScene()
{
	 MainMenuObjectView.setScene(Scene_3);	
}