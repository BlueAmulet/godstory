//===========================================================================================================
// �ĵ�˵��:��������������
// ����ʱ��:2009-4-1
// ������: soar
//=========================================================================================================== 
$Chunnel = 0;

function ChangeChunnel( %index )
{
	$Chunnel = %index;
}

function sendInfo()
{
	%itemText = SptGetItemLink();

	%info = InputWnd.getText();
	
	%info = "<t>" @ %info @ "</t>";
		
	if( %itemText !$= "" )
	{	
		%name = SptGetItemLinkName();
		%newName = "</t>" @ %itemText @ "<t>";
		%info = strreplace(%info, %name, %newName );
	}

	if(%info !$= "")
	{
		SptSendChatMessage($Chunnel,%info);
		InputWnd.setText("");
	}
	
	SptClearItemLink();
}

function InputInfo()
{
	sendInfo();
	
  //��λ���������
	if($pref::Input::ChatMode == 1)
		InputWnd.makeFirstResponder(1); 
}

function InputWnd::OnKeyDown( %this )
{

}

function InputWnd::OnKeyReturn(%this)
{
	sendInfo();
	
	//������ģʽ
	if($pref::Input::ChatMode == 0)
		InputWnd.makeFirstResponder(0);
}

function OpenChangeInputChunnelWnd()
{
	if(ChangeChatInputChunnelGui.IsVisible())
	{
		ChangeChatInputChunnelGui.setVisible(0);
	}
	else
	{
		ChangeChatInputChunnelGui.setVisible(1);
	}
}
//�رմ��ڽ�����Ӧ���������
function activeChatWnd()
{
	if(ChatInputWndGui.isVisible())
	{
		if($pref::Input::ChatMode == 1)
		   InputWnd.makeFirstResponder(1);
		else
		  {
		  	InputWnd.makeFirstResponder(1);//ǿ��ȡ����Ӧ(������ģʽ)
		  	InputWnd.makeFirstResponder(0);
		  } 
  }
}