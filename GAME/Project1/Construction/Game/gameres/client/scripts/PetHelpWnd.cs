//===========================================================================================================
// 文档说明:宠物列表界面操作
// 创建时间:2009-12-1
// 创建人: soar
//=========================================================================================================== 
$PetHelpList_SelectedIndex = -1;
function OpenPetHelpWnd(%nOptType)
{
	$PetHelpList_SelectedIndex = -1;
	PetHelpList_Clear();
	PetHelpList_Refresh(%nOptType);
	PetHelpWnd.setVisible(1);
	$IsDirty++;
}

function ClosePetHelpWnd()
{
	PetHelpWnd.setVisible(0);
	$PetHelpList_SelectedIndex = -1;
	PetHelpList_Clear();
	$IsDirty--;
}

//显示可选宠物列表
function RefreshPetHelpWnd(%icount)
{
	for(%i = 0; %i < %icount; %i++)
	{
		 %ShortY = %i * 42 + 1;
  	 new GuiControl("PetHelpWnd_Control_" @ %i) {
	     canSaveDynamicFields = "0";
	     Enabled = "1";
	     isContainer = "1";
	     Profile = "GuiDefaultProfile";
	     HorizSizing = "right";
	     VertSizing = "bottom";
	     position = "1 " @ %ShortY;
	     Extent = "157 42";
	     MinExtent = "8 2";
	     canSave = "1";
	     Visible = "1";
	     hovertime = "1000";

     new GuiBitmapCtrl("PetHelpWnd_BitMap_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "0 0";
        Extent = "157 42";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        bitmap = "gameres/gui/images/GUIWindow29_1_009.png";
        wrap = "0";
     };
     
     new GuiTextCtrl("PetHelpWnd_ItemName_" @ %i) {
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
        text = "";
        maxLength = "1024";
     };
      new GuiButtonCtrl("PetHelpButton_" @ %i) {
	      canSaveDynamicFields = "0";
	      Enabled = "1";
	      isContainer = "0";
	      Profile = "GuiCoverImageButtonProfile";
	      HorizSizing = "right";
	      VertSizing = "bottom";
	      position = "0 0";
	      Extent = "157 42";
	      MinExtent = "8 2";
	      canSave = "1";
	      Visible = "1";
	      Command = "PetHelpWnd_SelectListID(" @ %i @");";
	      hovertime = "1000";
	      groupNum = "1";
	      useMouseEvents = "0";
	      IsStateOn = "0";
	      buttonType = "RadioButton";
	   	};
	   	 new dGuiShortCut("PetHelpWnd_ShortCut_" @ %i) {
        canSaveDynamicFields = "0";
        Enabled = "1";
        isContainer = "0";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = "6 5";
        Extent = "32 32";
        MinExtent = "8 2";
        canSave = "1";
        Visible = "1";
        hovertime = "1000";
        slotType = "35";
        slotCol = %i;
        boudary = "0 0 32 32";
        disabledrag = true;
        RightMouseDownCommand = "PetHelpWnd_SelectAndSendMsg(" @ %i @ ");";
     };
	   	
  	};
  	
  	//取得对应的宠物名字
  	%sPetName = PetHelpList_GetPetName(%i);
  	("PetHelpWnd_ItemName_" @ %i).setText(%sPetName);
  	
 		PetScroll_Floor.add("PetHelpWnd_Control_" @ %i);
  	PetScroll_Floor.setExtent(175, (%i+1) * 42);
	}
}

function PetHelpWnd_SelectAndSendMsg(%index)  //右击选择调用
{
	PetHelpWnd_SelectListID(%index);
	PetHelpList_SelectListID($PetHelpList_SelectedIndex); //程序中定义的脚本，用于向服务端发送宠物槽的拖动请求
}

function PetHelpWnd_SelectListID(%index)	//dGuiShortcut对应的按钮选择调用
{
	if ($PetHelpList_SelectedIndex != -1)
	{
		("PetHelpButton_" @ $PetHelpList_SelectedIndex).setStateOn(0);
	}
	$PetHelpList_SelectedIndex = %index;
}

function PetHelpWnd_SendSelectMsg()		//"选择"按钮点击调用
{
	if ($PetHelpList_SelectedIndex == -1)
	{
		SetScreenMessage("请选中灵兽", 0xFF0000);
		return;
	}
	PetHelpList_SelectListID($PetHelpList_SelectedIndex); //程序中定义的脚本，用于向服务端发送宠物槽的拖动请求
}

function PetHelpWnd_ShowProperty()		//"显示属性"按钮点击调用
{
	if ($PetHelpList_SelectedIndex == -1)
	{
		SetScreenMessage("请选中灵兽", 0xFF0000);
		return;
	}
	
	%nPetSlotIndex = PetHelpWnd_GetSlotIndex();
	SeePetInfo(%nPetSlotIndex);
}