//ע��ϵͳ�߼��ӳ��¼������ͱ��(type)�Ǵ�10001��ʼ��
$SelectedIndividualMsg = 0;		// ��ǰѡ��ĸ����¼�ID
$IndividualMsgCount = 0;			// �����¼��б�������
$SelectedSysMsg = 0;					// ��ǰѡ���ϵͳ�¼�ID
$SysMsgCount = 0;							// ϵͳ�¼��б�������

// ��ϵͳ��Ϣ�б�
function OpenSysMsgBox()
{
	if(!SysMsgBoxGui.isvisible())
	{
		SysMsgBoxGui.setvisible(1);
		CreateMessageBoxItem(1);
	}
}

// �ر�ϵͳ��Ϣ�б�
function CloseSysMsgBox()
{
	if(SysMsgBoxGui.isvisible())
	{
		SysMsgBoxGui.setvisible(0);
		ClearMessageBoxItem(1);
	}
}

// ��ϵͳ��Ϣ��
function OpenSysMsgBoxItem()
{
	PopupInviteDialog($SelectedSysMsg);
}

// �򿪸�����Ϣ�б�
function OpenIndividualMsgBox()
{
	if(!IndividualMsgBoxGui.isvisible())
	{
		IndividualMsgBoxGui.setvisible(1);
		CreateMessageBoxItem(0);
	}
}

// �رո�����Ϣ�б�
function CloseIndividualMsgBox()
{
	if(IndividualMsgBoxGui.isvisible())
	{
		IndividualMsgBoxGui.setvisible(0);
		ClearMessageBoxItem(0);
	}
}

// �򿪸�����Ϣ��
function OpenIndividualMsgBoxItem()
{
	PopupInviteDialog($SelectedIndividualMsg);
}

// ɾ��������Ϣ��
function DelIndividualMsgBoxItem()
{
	GetPlayer().removeLogicEvent(%SelectedIndividualMsg);
	CreateMessageBoxItem(0);		
}

// ��ո�����Ϣ��
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

// ˢ����Ϣ
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

// �����Ϣ��
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
		SysMsgBoxText.setText("��ǰ��Ϣ: 0/" @ $SysMsgCount);
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
		IndividualMsgBoxText.setText("��ǰ��Ϣ: 0/" @ $IndividualMsgCount);
	}
}

// ������Ϣѡ����
function SetSelectedMsgItem(%IsSys, %index, %eid)
{
	%index ++;
	if(%IsSys)
	{
		$SelectedSysMsg = %eid;
		SysMsgBoxText.setText("��ǰ��Ϣ: " @ %index @ "/" @ $SysMsgCount);
	}
	else
	{
		$SelectedIndividualMsg = %eid;
		IndividualMsgBoxText.setText("��ǰ��Ϣ: " @ %index @ "/" @ $IndividualMsgCount);
	}
}

// ������Ϣ�б���
// %IsSys �Ƿ�ϵͳ��Ϣ
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
		SysMsgBoxText.setText("��ǰ��Ϣ: 0/" @ %countItems);
		SysMsgBox_Scroll.setextent(getword(SysMsgBox_Scroll.getextent(),0), %ctrlHeight);
	}
	else
	{
		$IndividualMsgCount = %countItems;
		IndividualMsgBoxText.setText("��ǰ��Ϣ: 0/" @ %countItems);	
  	IndividualBox_Scroll.setextent(getword(IndividualBox_Scroll.getextent(),0), %ctrlHeight);
	}
}

