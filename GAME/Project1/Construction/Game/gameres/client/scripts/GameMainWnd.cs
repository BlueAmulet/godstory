//===========================================================================================================
// 文档说明:游戏主界面操作
// 创建时间:2009-7-15
// 创建人: thinking
//===========================================================================================================
function CloseAllGuiWindow()
{
	$hideAllGuiWindow  = true;
	//GameMainWndGui_Layer1.setVisible(false);
	GameMainWndGui_Layer2.setVisible(false);
	GameMainWndGui_Layer3.setVisible(false);
	GameMainWndGui_Layer4.setVisible(false);
	GameMainWndGui_Layer5.setVisible(false);
}
function ShowAllGuiWindow()
{
	$hideAllGuiWindow = false;
	//GameMainWndGui_Layer1.setVisible(true);
	GameMainWndGui_Layer2.setVisible(true);
	GameMainWndGui_Layer3.setVisible(true);
	GameMainWndGui_Layer4.setVisible(true);
	GameMainWndGui_Layer5.setVisible(true);
}