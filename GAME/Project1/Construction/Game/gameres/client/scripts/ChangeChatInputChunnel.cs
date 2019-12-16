//===========================================================================================================
// 文档说明:输入对话频道改变操作
// 创建时间:2009-4-10
// 创建人: soar
//=========================================================================================================== 

function SelectChatInputChunnel(%index)
{
	switch(%index)
	{
		case 0:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>附近</t>");
		case 1:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>队伍</t>");
		case 2:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>帮会</t>");
		case 3:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>门宗</t>");
		case 4:
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>同城</t>");
		case 5:
			//OpenShoutWnd();
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>全服</t>");
		case 6:
			//OpenShoutWnd();
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>跨服</t>");
		case 7:
			//OpenShoutWnd();
			InputChunnelButton.settext("<t c=\'0xffffffff\' b=\'1\'>跑马</t>");
	}
	ChatInputWndGui.setDirty();
	ChangeChatInputChunnelGui.setVisible(0);
}