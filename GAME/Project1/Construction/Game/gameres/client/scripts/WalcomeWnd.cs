//===========================================================================================================
// �ĵ�˵��:��ӭ�������
// ����ʱ��:2009-10-6
// ������: soar
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
