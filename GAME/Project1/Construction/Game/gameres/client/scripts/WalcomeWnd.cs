//===========================================================================================================
// 文档说明:欢迎界面操作
// 创建时间:2009-10-6
// 创建人: soar
//=========================================================================================================== 

function CloseWalcomeWnd()
{
	WalcomeWnd.setvisible(0);
}

function ShowNewPlayerWalcomeWnd()
{
	WalcomeWnd.setvisible(1);
	WalcomeWnd_RichText.setcontent(GetDialogText(0,0,399999,0));
}
