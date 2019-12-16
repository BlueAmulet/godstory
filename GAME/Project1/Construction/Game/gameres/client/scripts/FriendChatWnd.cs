//===========================================================================================================
// 文档说明:仙友聊天界面操作
// 创建时间:2009-12-31
// 创建人: Batcel
//=========================================================================================================== 
function OpenOrCloseChatFriendWnd()
{
	if(FriendChatWndGui.isVisible())
	{
		CloseChatFriendWnd();
	}
	else
	{
		OpenChatFriendWnd();
	}
}

function MaxChatDialogUI(%playerId)
{
	("FriendChatGui_"@%playerId).setvisible(1);
	//("FriendChatIcon_" @ %playerId).setVisible(0);
	//$IsDirty++;
}


function MinChatDialogUI(%playerId)
{	
	("FriendChatGui_"@%playerId).setvisible(0);
	//("FriendChatIcon_" @ %playerId).setVisible(1);
	//$IsDirty--;
}

function CloseChatDialogUI(%playerId)
{
	("FriendChatGui_"@ %playerId).setvisible(0);
	%Extent = FriendChatBoxGui.getExtent();
	FriendChatBoxGui.setExtent(140,(getWord(%Extent,1)-20));
	FriendChat_NameList.removeRowById(%playerId);
	("FriendChat_NoteList"@ %playerId).Clear();
	if(FriendChat_NameList.rowCount()<=0)
	{
	  	FriendChatBoxGui.setVisible(0);
	  	FriendChatIcon.setVisible(0);
	}
	 schedule(0,0,"deleteChatDialogUI",%playerId);	 		
	//$IsDirty--;
}

function deleteChatDialogUI(%playerId)
{
	("FriendChatGui_"@%playerId).delete();
}

function AddFriendChatList(%playerId,%playername)
{	
	%Extent = FriendChatBoxGui.getExtent();
	FriendChatBoxGui.setExtent(140,(getWord(%Extent,1)+20));
	FriendChat_NameList.addRow(%playerId,%playername);
}

function SelectedFriendChatName()
{
	%Row =FriendChat_NameList.getSelectedRow();
	if(%Row != -1)
	{
		%playerId = FriendChat_NameList.getRowId(%Row);
		FriendChatBoxGui.setVisible(0);
		SptOpenChatDialog(%playerId);		
	}
}


function OpenFriendChatNameList()
{
	FriendChatBoxGui.setVisible(1);
}

function SendMessageToFriend(%playerId)
{
	%player = getPlayer();
	if(%player == 0)
	   return;
	   
	%text = ("FriendChatTextEdit_"@ %playerId).getText();
	if( %text !$= "")
	{
		SptSendChatMessageTo(12,%text,%playerId);
		("FriendChat_NoteList"@ %playerId).AddMsg(12,%text,%playerId,%player.getPlayerName());
		("FriendChatTextEdit_"@ %playerId).setText("");
	}
	else
	{
		
	}
}

function AddChatDialogMsg(%playerId,%text,%pName)
{
	("FriendChat_NoteList"@ %playerId).AddMsg(12,%text,%playerId,%pName);
}