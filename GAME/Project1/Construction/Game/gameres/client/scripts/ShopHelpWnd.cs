//===========================================================================================================
// �ĵ�˵��:�������ֽ������
// ����ʱ��:2009-9-24
// ������: soar
//=========================================================================================================== 

//��ʾ�Զ�����
function showAutoSellWnd(%icount)
{
	$AutoSellSlots = %icount;
	for(%i = 0; %i < %icount; %i++)
	{
		 %ShortY = %i * 42 + 1;
  	new GuiControl("ShopHelpWnd_Control_" @ %i) {
     canSaveDynamicFields = "0";
     Enabled = "1";
     isContainer = "1";
     Profile = "GuiDefaultProfile";
     HorizSizing = "right";
     VertSizing = "bottom";
     position = "1 " @ %ShortY;
     Extent = "174 42";
     MinExtent = "8 2";
     canSave = "1";
     Visible = "1";
     hovertime = "1000";

     new GuiBitmapCtrl("ShopHelpWnd_BitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "14 0";
        Extent = "152 42";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        bitmap = "gameres/gui/images/GUIWindow29_1_009.png";
        wrap = "0";
     };
     
     new GuiTextCtrl("ShopHelpWnd_ItemName_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiCurrencyShowInfoTextProfile_4";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "54 2";
        Extent = "100 20";
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
        text = "����һ����Ʒ����";
        maxLength = "1024";
     };
     new GuiBitmapCtrl("ShopHelpWnd_GoldBitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "87 21";
        Extent = "16 16";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "0";
        hovertime = "1000";
        useFadeout = "0";
        bitmap = "gameres/gui/images/GUIWindow31_1_024.png";
        wrap = "0";
     };
     new GuiBitmapCtrl("ShopHelpWnd_SillerBitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "115 21";
        Extent = "16 16";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "0";
        hovertime = "1000";
        useFadeout = "0";
        bitmap = "gameres/gui/images/GUIWindow31_1_025.png";
        wrap = "0";
     };
     new GuiBitmapCtrl("ShopHelpWnd_CopperBitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "144 21";
        Extent = "16 16";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        useFadeout = "0";
        bitmap = "gameres/gui/images/GUIWindow31_1_026.png";
        wrap = "0";
     };
     new GuiTextCtrl("ShopHelpWnd_Gold_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiTextProfile5";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "47 21";
        Extent = "40 20";
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
        text = "";
        maxLength = "1024";
     };
     new GuiTextCtrl("ShopHelpWnd_Siller_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiTextProfile5";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "102 21";
        Extent = "14 20";
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
        text = "";
        maxLength = "1024";
     };
     new GuiTextCtrl("ShopHelpWnd_Copper_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiTextProfile5";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "131 21";
        Extent = "14 20";
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
        text = "";
        maxLength = "1024";
     };
      new GuiButtonCtrl("Button_AutoImage_" @ %i) {
      canSaveDynamicFields = "0";
      Enabled = "1";
      isContainer = "0";
      Profile = "GuiCoverImageButtonProfile";
      HorizSizing = "right";
      VertSizing = "bottom";
      position = "14 0";
      Extent = "152 42";
      MinExtent = "8 2";
      canSave = "1";
      Visible = "1";
      Command = "setSelectListID(" @ %i @");";
      hovertime = "1000";
      groupNum = "1";
      buttonType = "RadioButton";
      useMouseEvents = "0";
      IsStateOn = "0";
   	};
     new dGuiShortCut("ShopHelpWnd_ShortCut_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "19 5";
        Extent = "32 32";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        slotType = "11";
        slotCol = %i;
        boudary = "0 0 32 32";
        disabledrag = true;
     };
  	};
  	ShopScroll_test.add("ShopHelpWnd_Control_" @ %i);
  	ShopScroll_test.setextent(195,(%i+1) * 42);
 // 	showGeneralListInfor(1,%i);
	}
}

function ShopHelpClear()
{
	ShopScroll_test.clear();
	for(%i = 0; %i < $AutoSellSlots; %i++)
	{
		ShopHelpRemove(%i);
	}
}

function ShopHelpRemove(%i)
{
		%obj = "ShopHelpWnd_BitMap_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "Button_AutoImage_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_ShortCut_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_ItemName_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_Gold_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_SillerBitMap_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_CopperBitMap_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_Gold_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_Siller_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_Copper_" @ %i;
		if(isobject(%obj))
			%obj.delete();
		%obj = "ShopHelpWnd_Control_" @ %i;
		if(isobject(%obj))
			%obj.delete();
}

function OpenOrCloseShopHelpWnd()
{
	if(ShopHelpWndGui.IsVisible())
	{
		ShopHelpWndGui.setVisible(0);	
		Shop_BuyHelpButton.setStateOn(0);
		ShopHelpClear();
		$IsDirty--;
	}
	else
	{
		ShopHelpWndGui.setVisible(1);
		ShopHelpButton_Common.setStateOn(1);
		openAutoSell();
		ShopHelpWndGui_SellButton.setVisible(1);
		ShopHelpWndGui_SetSelfSellButton.setVisible(1);
		ShopHelpWndGui_ClearButton.setVisible(0);
		ShopHelpWndGui_CancelSelfSellButton.setVisible(0);
		$IsDirty++;
	}
}

function OpenShopHelpWnd()
{
	ShopHelpWndGui.setVisible(1);
	openAutoSell();
	ShopHelpWndGui_SellButton.setVisible(1);
	ShopHelpWndGui_SetSelfSellButton.setVisible(1);
	ShopHelpWndGui_ClearButton.setVisible(0);
	ShopHelpWndGui_CancelSelfSellButton.setVisible(0);
	$IsDirty++;
}

function CloseShopHelpWnd()
{
	ShopHelpWndGui.setVisible(0);	
	Shop_BuyHelpButton.setStateOn(0);
	ShopHelpClear();
	$IsDirty--;
}


function OpenShopCommonScroll()
{
	if(!ShopHelpWndGui_SellButton.isVisible())
		ShopHelpWndGui_SellButton.setVisible(1);
	if(!ShopHelpWndGui_SetSelfSellButton.isVisible())
		ShopHelpWndGui_SetSelfSellButton.setVisible(1);
	if(ShopHelpWndGui_ClearButton.isVisible())
		ShopHelpWndGui_ClearButton.setVisible(0);
	if(ShopHelpWndGui_CancelSelfSellButton.isVisible())
		ShopHelpWndGui_CancelSelfSellButton.setVisible(0);
	refreshAutoSellWnd(1);
	for(%i=0; %i<$AutoSellSlots; %i++)
	{
//		showGeneralListInfor(1,%i);
		("Button_AutoImage_" @ %i).setStateOn(false);
		$AutoListSelectedIndex = -1;
	}
}

function OpenShopExcellenceScroll()
{
	if(!ShopHelpWndGui_SellButton.isVisible())
		ShopHelpWndGui_SellButton.setVisible(1);
	if(!ShopHelpWndGui_SetSelfSellButton.isVisible())
		ShopHelpWndGui_SetSelfSellButton.setVisible(1);
	if(ShopHelpWndGui_ClearButton.isVisible())
		ShopHelpWndGui_ClearButton.setVisible(0);
	if(ShopHelpWndGui_CancelSelfSellButton.isVisible())
		ShopHelpWndGui_CancelSelfSellButton.setVisible(0);
	refreshAutoSellWnd(2);
	for(%i=0; %i<$AutoSellSlots; %i++)
	{
//		showGeneralListInfor(2,%i);
		("Button_AutoImage_" @ %i).setStateOn(false);
		$AutoListSelectedIndex = -1;
	}
}


function OpenShopSelfSellScroll()
{
	if(ShopHelpWndGui_SellButton.isVisible())
		ShopHelpWndGui_SellButton.setVisible(0);
	if(ShopHelpWndGui_SetSelfSellButton.isVisible())
		ShopHelpWndGui_SetSelfSellButton.setVisible(0);
	if(!ShopHelpWndGui_ClearButton.isVisible())
		ShopHelpWndGui_ClearButton.setVisible(1);
	if(!ShopHelpWndGui_CancelSelfSellButton.isVisible())
		ShopHelpWndGui_CancelSelfSellButton.setVisible(1);
	refreshAutoSellList();
	for(%i=0; %i<$AutoSellSlots; %i++)
	{
//		showGeneralListInfor(3,%i);
		("Button_AutoImage_" @ %i).setStateOn(false);
		$AutoListSelectedIndex = -1;
	}
}
function RefreshAutoSellSlots(%icount)
{
	%fa = $AutoSellSlots - %icount;
	if(%fa == 0)
		return;
	
	if(%fa > 0)
	{
		for(%i = %icount; %i < $AutoSellSlots; %i++)
		{
			ShopHelpRemove(%i);
			//ShopScroll_test.remove("ShopHelpWnd_Control_" @ %i);
		}
	}
	else
	{
		for(%i = $AutoSellSlots; %i < %icount; %i++)
		{
			%ShortY = %i*42 +1;
			new GuiControl("ShopHelpWnd_Control_" @ %i) {
     canSaveDynamicFields = "0";
     Enabled = "1";
     isContainer = "1";
     Profile = "GuiDefaultProfile";
     HorizSizing = "right";
     VertSizing = "bottom";
     position = "1 " @ %ShortY;
     Extent = "175 42";
     MinExtent = "8 2";
     canSave = "1";
     Visible = "1";
     hovertime = "1000";

     new GuiBitmapCtrl("ShopHelpWnd_BitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "14 0";
        Extent = "152 42";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        bitmap = "gameres/gui/images/GUIWindow29_1_009.png";
        wrap = "0";
     };
     new GuiTextCtrl("ShopHelpWnd_ItemName_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiCurrencyShowInfoTextProfile_4";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "54 2";
        Extent = "100 20";
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
        text = "����һ����Ʒ����";
        maxLength = "1024";
     };
     new GuiBitmapCtrl("ShopHelpWnd_GoldBitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "87 21";
        Extent = "16 16";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "0";
        hovertime = "1000";
        useFadeout = "0";
        bitmap = "gameres/gui/images/GUIWindow31_1_024.png";
        wrap = "0";
     };
     new GuiBitmapCtrl("ShopHelpWnd_SillerBitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "115 21";
        Extent = "16 16";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "0";
        hovertime = "1000";
        useFadeout = "0";
        bitmap = "gameres/gui/images/GUIWindow31_1_025.png";
        wrap = "0";
     };
     new GuiBitmapCtrl("ShopHelpWnd_CopperBitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "144 21";
        Extent = "16 16";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        useFadeout = "0";
        bitmap = "gameres/gui/images/GUIWindow31_1_026.png";
        wrap = "0";
     };
     new GuiTextCtrl("ShopHelpWnd_Gold_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiTextProfile5";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "47 21";
        Extent = "40 20";
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
        text = "";
        maxLength = "1024";
     };
     new GuiTextCtrl("ShopHelpWnd_Siller_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiTextProfile5";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "102 21";
        Extent = "14 20";
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
        text = "";
        maxLength = "1024";
     };
     new GuiTextCtrl("ShopHelpWnd_Copper_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiTextProfile5";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "131 21";
        Extent = "14 20";
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
        text = "";
        maxLength = "1024";
     };
      new GuiButtonCtrl("Button_AutoImage_" @ %i) {
      canSaveDynamicFields = "0";
      Enabled = "1";
      isContainer = "0";
      Profile = "GuiCoverImageButtonProfile";
      HorizSizing = "right";
      VertSizing = "bottom";
      position = "13 0";
      Extent = "152 42";
      MinExtent = "8 2";
      canSave = "1";
      Visible = "1";
      Command = "setSelectListID(" @ %i @");";
      hovertime = "1000";
      groupNum = "1";
      buttonType = "RadioButton";
      useMouseEvents = "0";
      IsStateOn = "0";
   	};
     new dGuiShortCut("ShopHelpWnd_ShortCut_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "19 5";
        Extent = "32 32";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        slotType = "11";
        slotCol = %i;
        boudary = "0 0 32 32";
        disabledrag = true;
     };
  	};
  	ShopScroll_test.add("ShopHelpWnd_Control_" @ %i);
  	ShopScroll_test.setextent(195,(%i+1) * 42);
		}
	}		
	$AutoSellSlots = %icount;
	for(%i=0; %i<$AutoSellSlots; %i++)
	{
		("Button_AutoImage_" @ %i).setStateOn(false);
	}
	$AutoListSelectedIndex = -1;
}

function setSelectListID(%index)
{
	if($AutoListSelectedIndex != -1)
		("Button_AutoImage_" @ $AutoListSelectedIndex).setStateOn(false);
	$AutoListSelectedIndex = %index;
}

function setEveryItemInfo(%i,%name,%money)
{
	("ShopHelpWnd_ItemName_" @ %i).setText(%name);
	ShowEveryItemMoney(%i,%money);
}

function ShowEveryItemMoney(%index,%money)
{
	if(%money >= 10000)
	{
		("ShopHelpWnd_GoldBitMap_" @%index).setVisible(1);
		("ShopHelpWnd_SillerBitMap_" @%index).setVisible(1);
		%gold = mfloor(%money / 10000);
	  %silver	= mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  ("ShopHelpWnd_Gold_" @ %index).settext(%gold);
	  ("ShopHelpWnd_Siller_" @ %index).settext(%silver);
	  ("ShopHelpWnd_Copper_" @ %index).settext(%copper);
	}
	else if(%money < 100)
	{
		%copper = %money;
		("ShopHelpWnd_GoldBitMap_" @%index).setVisible(0);
		("ShopHelpWnd_SillerBitMap_" @%index).setVisible(0);
		("ShopHelpWnd_Gold_" @ %index).settext("");
	  ("ShopHelpWnd_Siller_" @ %index).settext("");
	  ("ShopHelpWnd_Copper_" @ %index).settext(%copper);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
	  ("ShopHelpWnd_GoldBitMap_" @%index).setVisible(0);
		("ShopHelpWnd_SillerBitMap_" @%index).setVisible(1);
		("ShopHelpWnd_Gold_" @ %index).settext("");
	  ("ShopHelpWnd_Siller_" @ %index).settext(%silver);
	  ("ShopHelpWnd_Copper_" @ %index).settext(%copper);
	 
	}
	
}

function addCommonToAutoList($AutoListSelectedIndex)
{
	if($AutoListSelectedIndex != -1)
	{
		OpenDialog_OK_Cancel();
		%name = ("ShopHelpWnd_ItemName_" @ $AutoListSelectedIndex).getValue();
		%text = "��ȷ��Ҫ��[" @ %name @ "]���ó��Զ�������";
		Dialog_OK_Cancel.SetContent(%text);
		Dialog_OK_Cancel.setOk("ȷ��", "addToAutoSellList($AutoListSelectedIndex);");
		Dialog_OK_Cancel.SetCancel("ȡ ��", "");
		
	}
	else
		return;	
}

function isClearAll()
{
		OpenDialog_OK_Cancel();
		%text = "��ȷ��Ҫ����Զ������б���";
		Dialog_OK_Cancel.SetContent(%text);
		Dialog_OK_Cancel.setOk("ȷ��", "autoListClearAll();");
		Dialog_OK_Cancel.SetCancel("ȡ ��", "");
}

function deleteItemFromAutoList()
{
	if($AutoListSelectedIndex != -1)
	{
		OpenDialog_OK_Cancel();
		%name = ("ShopHelpWnd_ItemName_" @ $AutoListSelectedIndex).getValue();
		%text = "��ȷ��Ҫȡ��[" @ %name @ "]�Զ�����������";
		Dialog_OK_Cancel.SetContent(%text);
		Dialog_OK_Cancel.setOk("ȷ��", "delFromAutoSellList($AutoListSelectedIndex);");
		Dialog_OK_Cancel.SetCancel("ȡ ��", "");
		
	}
	else
		return;	
}

function setAutoSellItem(%index)
{
		echo("-----------------------------"@%index);
		echo("$AutoListSelectedIndex-------"@$AutoListSelectedIndex);
		OpenDialog_OK_Cancel();
		%name = ("ShopHelpWnd_ItemName_" @ %index).getValue();
		%text = "��ȷ��Ҫ����[" @ %name @ "]��";
		Dialog_OK_Cancel.SetContent(%text);
		Dialog_OK_Cancel.setOk("ȷ��", "AutoSellByIndex("@%index@");");
		Dialog_OK_Cancel.SetCancel("ȡ ��", "");
		
}

function setAutoSellOK($AutoListSelectedIndex)
{
	AutoSellByIndex($AutoListSelectedIndex);
}