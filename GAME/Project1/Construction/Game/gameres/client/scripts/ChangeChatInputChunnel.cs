//===========================================================================================================
// �ĵ�˵��:����Ի�Ƶ���ı����
// ����ʱ��:2009-4-10
// ������: soar
//=========================================================================================================== 

function SelectChatInputChunnel(%index)
{
	switch(%index)
	{
		case 0:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>����</t>");
		case 1:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>����</t>");
		case 2:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>���</t>");
		case 3:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>����</t>");
		case 4:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>ͬ��</t>");
		case 5:
			//OpenShoutWnd();
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>ȫ��</t>");
		case 6:
			//OpenShoutWnd();
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>���</t>");
		case 7:
			//OpenShoutWnd();
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>����</t>");
	}
	ChatInputWndGui.setDirty();
	ChangeChatInputChunnelGui.setVisible(0);
}