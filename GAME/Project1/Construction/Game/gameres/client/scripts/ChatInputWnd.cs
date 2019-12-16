//===========================================================================================================
// 文档说明:聊天输入栏操作
// 创建时间:2009-4-1
// 创建人: soar
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
	
  //定位聊天输入框
	if($pref::Input::ChatMode == 1)
		InputWnd.makeFirstResponder(1); 
}

function InputWnd::OnKeyDown( %this )
{

}

function InputWnd::OnKeyReturn(%this)
{
	sendInfo();
	
	//非聊天模式
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
//关闭窗口界面响应聊天输入框
function activeChatWnd()
{
	if(ChatInputWndGui.isVisible())
	{
		if($pref::Input::ChatMode == 1)
		   InputWnd.makeFirstResponder(1);
		else
		  {
		  	InputWnd.makeFirstResponder(1);//强制取回响应(非聊天模式)
		  	InputWnd.makeFirstResponder(0);
		  } 
  }
}