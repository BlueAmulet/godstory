$MessageID = 0;

// ��Ļ������ʾϵͳ��Ϣ
function SetScreenMessage(%text, %color)
{
	GameMainWndGui_Layer5.PushToBack(ScreenMessage);
	ScreenMessage.SetText(%text, %color);
}

// �������ʾϵͳ��Ϣ
function SetChatMessage(%text, %color)
{
	%r = %color >> 16;
	%g = %color >> 8 & 0xFF;
	%b = %color & 0xFF;		
	%color = "0x" @ getHex(%r) @ getHex(%g) @ getHex(%b) @ "ff";
	%content = "<t f='����' m='1' n='12' c='" @ %color @ "' o='0x010101ff'>" @ %text @ "</t>";
	AddChatMessage("1", %content, "0", "0");
}

function SetRollMessage(%text, %limit, %time, %font, %color, %size)
{
		%r = %color >> 16;
		%g = %color >> 8 & 0xFF;
		%b = %color & 0xFF;		
		%color = "0x" @ getHex(%r) @ getHex(%g) @ getHex(%b) @ "ff";
		
		if(%font == 1)
			%font="'����' m='0'";
		else if(%font == 2)
			%font="'����_GB2312' m='0'";
		else if(%font == 3)
			%font="'����' m='0'";
		else if(%font == 4)
			%font="'΢���ź�' m='0'";
		else
		{
			if(%size >= 10 && %size <= 16)
				%font="'����' m='1'";
			else
				%font="'����' m='0'";
		}
		
		%text = "<t f=" @ %font @ " n='" @ %size @ "' c='" @ %color @ "' o='0x010101ff'>" @ %text @ "</t>";
		RollMessage.SetText(%text,%limit,%time);
}

function getHex(%c)
{
	 if(%c > 15)
	 {
	 		%a = %c >> 4;
	 		%b = %c & 0xF;
	 		return getHexOne(%a) @ getHexOne(%b);
	 }
	 return "0" @ getHexOne(%c);
}

function getHexOne(%c)
{
		if(%c == 10) return "a";
	 	else if(%c == 11) return "b";
	 	else if(%c == 12) return "c";
	 	else if(%c == 13) return "d";
	 	else if(%c == 14) return "e";
	 	else if(%c == 15) return "f";
	 	else	return %c;
}

//�����Ϣ����
// %mode  ��ʾ��ʽ(0 Ϊ������ʾ 1 Ϊ������ʾ)
// %text  ��Ϣ���ݣ�%mode Ϊ 0ʱ ��Ҫ�Լ���ʽ������)
// %x,%y  ���λ��
// %limit ��ʱ���޴α�־(0 Ϊ��ʱ 1Ϊ�޴�)
// %time  ��ʱ�������޴δ���
// %font  ��������
// %color ������ɫ
// %size  �����С
function OutputScrMsg(%text, %x, %y, %mode, %limit, %time, %font, %color, %size)
{
	$MessageID++;
	%MessageControlID = "TempMessage" @ $MessageID;
		
	if(%mode == 0) //������ʾģʽ
	{
		new GuiScreenMessage(%MessageControlID)
		{
	    	Profile = "GuiCurrencyShowInfoTextProfile_1";
        HorizSizing = "center";
        VertSizing = "bottom";
        position =  %x @ " " @ %y;
        Extent = "300 50";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "0";
        hovertime = "1000";
		};
	
		Canvas.getContent().Add(%MessageControlID);
		%MessageControlID.setvisible(1);
		%MessageControlID.settext(%text, %color);
		%time =6000; //6������
	}
	else					// ������ʾģʽ(��ȱ)
	{
		%r = %color >> 16;
		%g = %color >> 8 & 0xFF;
		%b = %color & 0xFF;		
		%color = "0x" @ getHex(%r) @ getHex(%g) @ getHex(%b) @ "ff";
		
		if(%font == 1)
			%font="'����' m='0'";
		else if(%font == 2)
			%font="'����_GB2312' m='0'";
		else if(%font == 3)
			%font="'����' m='0'";
		else if(%font == 4)
			%font="'΢���ź�' m='0'";
		else
		{
			if(%size >= 10 && %size <= 16)
				%font="'����' m='1'";
			else
				%font="'����' m='0'";
		}
		
		%text = "<t f=" @ %font @ " n='" @ %size @ "' c='" @ %color @ "' o='0x010101ff'>" @ %text @ "</t>";
		
		new GuiBitmapCtrl(%MessageControlID) {
        	canSaveDynamicFields = "0";
        	Enabled = "1";
        	isContainer = "0";
        	Profile = "GuiDefaultProfile";
        	HorizSizing = "right";
        	VertSizing = "bottom";
        	position = "300 80";
        	Extent = "475 44";
        	MinExtent = "8 2";
        	canSave = "1";
        	Visible = "1";
        	hovertime = "1000";
        	bitmap = "gameres/gui/images/rollmsg.png";
        	wrap = "0";
					
					new GuiRollMessage(%MessageControlID @ "_sub")
					{
	    			Profile = "GuiCurrencyShowInfoTextProfile_1";
        		HorizSizing = "center";
        		VertSizing = "bottom";
        		position =  %x @ " " @ %y;
        		Extent = "500 50";
        		MinExtent = "8 2";
        		canSave = "1";
        		Visible = "0";
        		hovertime = "1000";
					};
				};
		Canvas.getContent().Add(%MessageControlID);
		(%MessageControlID @ "_sub").settext(%text, %limit, %time);
		if(%limit == 1)
	  	%time =60000; 				//1���Ӻ����
		else
	  	%time = %time * 1001; //%time��Чʱ������
	}
	// �ǵ����ٶ���
	schedule(%time, 0, "ClearMessageControl", Canvas.getContent(), %MessageControlID);
}

function ClearMessageControl(%parent, %obj)
{
	if(isobject(%parent))
		 %parent.remove(%obj);
	if(isobject(%obj))
		%obj.delete();
}

//���ع������밴ť
function HideInviteButton(%type)
{
	AnimateButton.setvisible(0);
	$schInvite = 0;
	if(%type > 10000)
		SysMessageBox.setvisible(1);
	else if(%type != 0)
		PlayerMessageBox.setvisible(1);
}