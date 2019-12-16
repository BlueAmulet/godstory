//注：系统逻辑延迟事件的类型编号(type)是从10001开始的
$SelectedIndividualMsg = 0;		// 当前选择的个人事件ID
$IndividualMsgCount = 0;			// 个人事件列表项总数
$SelectedSysMsg = 0;					// 当前选择的系统事件ID
$SysMsgCount = 0;							// 系统事件列表项总数

// 打开系统消息列表
function OpenSysMsgBox()
{
	if(!SysMsgBoxGui.isvisible())
	{
		SysMsgBoxGui.setvisible(1);
		CreateMessageBoxItem(1);
	}
}

// 关闭系统消息列表
function CloseSysMsgBox()
{
	if(SysMsgBoxGui.isvisible())
	{
		SysMsgBoxGui.setvisible(0);
		ClearMessageBoxItem(1);
	}
}

// 打开系统消息项
function OpenSysMsgBoxItem()
{
	PopupInviteDialog($SelectedSysMsg);
}

// 打开个人消息列表
function OpenIndividualMsgBox()
{
	if(!IndividualMsgBoxGui.isvisible())
	{
		IndividualMsgBoxGui.setvisible(1);
		CreateMessageBoxItem(0);
	}
}

// 关闭个人消息列表
function CloseIndividualMsgBox()
{
	if(IndividualMsgBoxGui.isvisible())
	{
		IndividualMsgBoxGui.setvisible(0);
		ClearMessageBoxItem(0);
	}
}

// 打开个人消息项
function OpenIndividualMsgBoxItem()
{
	PopupInviteDialog($SelectedIndividualMsg);
}

// 删除个人消息项
function DelIndividualMsgBoxItem()
{
	GetPlayer().removeLogicEvent(%SelectedIndividualMsg);
	CreateMessageBoxItem(0);		
}

// 清空个人消息项
function ClearIndividualMsgBoxItem()
{
	%player = GetPlayer();
	ClearMessageBoxItem(0);
	for(%i = 0; %i < %player.getLogicEventCount(); %i++)
	{
		%type 	= %player.getLogicEventTypeByIndex(%i);
		if(%type <= 10000)
			continue;
		%eid  	= %player.getLogicEventIDByIndex(%i);
		%player.removeLogicEvent(%eid);
	}
}

// 刷新消息
function RefreshLogicEvent(%IsSys)
{
	if(%IsSys && SysMsgBoxGui.isvisible())
	{
		CreateMessageBoxItem(1);
		return;
	}
	if(!%IsSys && IndividualMsgBoxGui.isvisible())
	{
		CreateMessageBoxItem(0);
		return;
	}
}

// 清除消息项
function ClearMessageBoxItem(%IsSys)
{
	if(%IsSys)
	{
		for(%i = 0; %i < $SysMsgCount; %i++)
		{
			%ctrlID = "SysMsgBoxItem" @ %i;
			if(isObject(%ctrlID))
				%ctrlID.delete();
		}
		SysMsgBox_Scroll.clear();
		$SelectedSysMsg = 0;
		$SysMsgCount = 0;
		SysMsgBoxText.setText("当前信息: 0/" @ $SysMsgCount);
	}
	else
	{
		for(%i = 0; %i < $IndividualMsgCount; %i++)
		{
			%ctrlID = "IndividualMsgBoxItem" @ %i;
			if(isObject(%ctrlID))
				%ctrlID.delete();
		}
		IndividualBox_Scroll.clear();
		$SelectedIndividualMsg = 0;
		$IndividualMsgCount = 0;
		IndividualMsgBoxText.setText("当前信息: 0/" @ $IndividualMsgCount);
	}
}

// 设置消息选中项
function SetSelectedMsgItem(%IsSys, %index, %eid)
{
	%index ++;
	if(%IsSys)
	{
		$SelectedSysMsg = %eid;
		SysMsgBoxText.setText("当前消息: " @ %index @ "/" @ $SysMsgCount);
	}
	else
	{
		$SelectedIndividualMsg = %eid;
		IndividualMsgBoxText.setText("当前消息: " @ %index @ "/" @ $IndividualMsgCount);
	}
}

// 创建消息列表项
// %IsSys 是否系统消息
function CreateMessageBoxItem(%IsSys)
{
	%Player = GetPlayer();
	ClearMessageBoxItem(%IsSys);
	%count = %Player.getLogicEventCount();
	%countItems = 0;
	%ctrlID = 0;	
	%ctrlHeight = 1;
	for(%i = 0; %i < %count; %i++)
	{
		%type 	= %Player.getLogicEventTypeByIndex(%i);			
		if(%IsSys && %type <= 10000)
			continue;
		if(!%IsSys && %type > 10000)
			continue;
		
		%eid  	= %Player.getLogicEventIDByIndex(%i);
		%Pos  	= %countItems * 42 + 1;
		%ctrlHeight += 42;
		
		if(%IsSys)
			%ctrlID = "SysMsgBoxItem" @ %countItems;
		else
			%ctrlID = "IndividualMsgBoxItem" @ %countItems;

		new GuiControl(%ctrlID) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "1";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = "1 " @ %Pos;
         Extent = "156 42";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         hovertime = "1000";

         new GuiBitmapCtrl() {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "1 1";
            Extent = "141 42";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            hovertime = "1000";
            bitmap = "gameres/gui/images/GUIWindow16_1_003.png";
            wrap = "0";
         };
         new GuiBitmapCtrl() {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "4 4";
            Extent = "36 36";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            hovertime = "1000";
            bitmap = "gameres/gui/images/GUIWindow16_1_002.png";
            wrap = "0";
         };
         new GuiTextCtrl() {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiCurrencyShowInfoTextProfile_4";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "50 13";
            Extent = "89 18";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            hovertime = "1000";
            Margin = "0 0 0 0";
            Padding = "0 0 0 0";
            AnchorTop = "1";
            AnchorBottom = "0";
            AnchorLeft = "1";
            AnchorRight = "0";
            text = GetLogicEventTypeName(%type);
            maxLength = "8";
         };
         new GuiButtonCtrl() {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiCoverImageButtonProfile";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "1 1";
            Extent = "141 42";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            groupNum = "0";
            buttonType = "RadioButton";
            useMouseEvents = "0";
            IsStateOn = "0";
            Command = "SetSelectedMsgItem(" @ %IsSys @ "," @ %countItems @ "," @ %eid @");";
         };
      };
      if(%IsSys)
  			SysMsgBox_Scroll.add(%ctrlID);
  		else
  			IndividualBox_Scroll.add(%ctrlID);
  		%countItems ++;
	}

	if(%IsSys)
	{
		$SysMsgCount = %countItems;		
		SysMsgBoxText.setText("当前信息: 0/" @ %countItems);
		SysMsgBox_Scroll.setextent(getword(SysMsgBox_Scroll.getextent(),0), %ctrlHeight);
	}
	else
	{
		$IndividualMsgCount = %countItems;
		IndividualMsgBoxText.setText("当前信息: 0/" @ %countItems);	
  	IndividualBox_Scroll.setextent(getword(IndividualBox_Scroll.getextent(),0), %ctrlHeight);
	}
}

