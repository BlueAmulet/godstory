//===========================================================================================================
// 文档说明:摆摊界面操作
// 创建时间:2009-9-21
// 创建人: soar
//=========================================================================================================== 

function OpenOrCloseStallWnd()
{
	if(StallWndGui.isVisible())
	{
		CloseStallWnd();
	}
	else
	{
		OpenStallWnd();
	}
}

function OpenStallWnd()
{
	GameMainWndGui_Layer4.pushtoback(StallWndGui);
	StallWndGui.setvisible(1);	
	CreateShortCut();
	showStallMoney(0,0);
	$IsDirty++;
}

function CloseStallWnd()
{
	clearStallSlots();
	StallWndGui.setvisible(0);
	$IsDirty--;	
	CloseStallInfoWnd();
	CloseLookupWnd();
	//CloseLookupReturnWnd();
	$srcIndex = -1;
	CloseChangePriceWnd();
	CloseSetStallWnd();
	CloseStallHelpWnd();
}

function OpenOrCloseStallSeeWnd()
{
	if(StallSeeWndGui.isVisible())
	{
		CloseStallSeeWnd();
	}
	else
	{
		OpenStallSeeWnd();
	}
}

function OpenStallSeeWnd()
{
	if(!StallSeeWndGui.isVisible())
	{
		GameMainWndGui_Layer4.pushtoback(StallSeeWndGui);
		StallSeeWndGui.setvisible(1);
		$IsDirty++;
	}
}

function CloseStallSeeWnd()
{
	StallSeeWndGui.setvisible(0);
	clearSeeStallSlots();
	if(StallWnd_SetTradeItemWnd.isvisible())
		CloseSetTradeItemWnd();
	$IsDirty--;
}

function OpenOrCloseStallInfoWnd()
{
	if(StallWnd_StallInfoWnd.isVisible())
	{
		CloseStallInfoWnd();
	}
	else
	{
		OpenStallInfoWnd();
	}
}

function OpenStallInfoWnd()
{
	if(!StallWnd_StallInfoWnd.isVisible())
	{
		GameMainWndGui_Layer4.pushtoback(StallWnd_StallInfoWnd);
		StallWnd_StallInfoWnd.setvisible(1);
		clearTradeAndChatMessage();
		showStallInfo();
		$IsDirty++;
	}
}

function CloseStallInfoWnd()
{
	if(StallWnd_StallInfoWnd.isvisible())
	{
		StallWnd_StallInfoWnd.setvisible(0);
		$IsDirty--;
	}
}

function OpenOrCloseLookupWnd()
{
	if(StallWnd_LookupWnd.isVisible())
	{
		CloseLookupWnd();
	}
	else
	{
		OpenLookupWnd();
	}
}

function OpenLookupWnd()
{
	GameMainWndGui_Layer4.pushtoback(StallWnd_LookupWnd);
	StallWnd_LookupWnd.setvisible(1);
	showResultPage(0,0);
	clearResultItemInfo();
	LookupWnd_TextEdit.setText("");
	$IsDirty++;
}

function CloseLookupWnd()
{
	if(StallWnd_LookupWnd.isvisible())
	{
		StallWnd_LookupWnd.setvisible(0);
		clearLookUpList();
		$IsDirty--;
	}
}

//function OpenOrCloseLookupReturnWnd()
//{
//	if(StallWnd_LookupWnd.isVisible())
//	{
//		CloseLookupReturnWnd();
//	}
//	else
//	{
//		OpenLookupReturnWnd();
//	}
//}
//
function OpenLookupReturnWnd()
{
	GameMainWndGui_Layer4.pushtoback(StallWnd_LookupWnd);
	StallWnd_LookupWnd.setvisible(1);
	$IsDirty++;
}

function CloseLookupReturnWnd()
{
	StallWnd_LookupWnd.setvisible(0);
	$IsDirty--;
}

function OpenOrCloseChangePriceWnd()
{
	if(StallWnd_ChangePriceWnd.isVisible())
	{
		CloseChangePriceWnd();
	}
	else
	{
		OpenChangePriceWnd(1);
	}
}

function OpenChangePriceWnd(%type)
{
	%obj = "StallWndGui_ShortCut_" @ $pitchOnIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected ())
		{
			setChangePriceWndPos($pitchOnIndex,1);
			ChangePriceWnd_OnStall.command = "setChangeMoneyOk(1);";
			%isShow = ShowChangePriceWnd($pitchOnIndex, 1);		
		}
	}
	%obj = "StallWndGui_PetShortCut_" @ $StallPetListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		{
			setChangePriceWndPos($StallPetListIndex, 2);
			ChangePriceWnd_OnStall.command = "setPetChangeMoneyOk(1);";
			%isShow = ShowChangePriceWnd($StallPetListIndex, 2);	
		}
	}
	if(%type == 1)
	{
		if(%isShow)
		{
			StallWnd_ChangePriceWnd.setvisible(1);
			setChangePriceMoney();
			ChangePriceWnd_Cancel.command = "CloseChangePriceWnd();";
			$IsDirty++;
		}
	}
	else
	{
		StallWnd_ChangePriceWnd.setvisible(1);
		ChangePriceWnd_Cancel.command = "CloseChangePriceWnd();";
		$IsDirty++;
	}
	GameMainWndGui_Layer4.pushtoback(StallWnd_ChangePriceWnd);
}
//设置改价的钱的显示
function setChangePriceMoney()
{
	%gold = StallWndGui_SingleItemMoney_GoldText.getValue();
	if(%gold > 0)
		ChangePriceWnd_Price_Gold.setText(%gold);
	else
		ChangePriceWnd_Price_Gold.setText(0);
	%siller = StallWndGui_SingleItemMoney_SillerText.getValue();
	if(%siller > 0)
		ChangePriceWnd_Price_Siller.setText(%siller);
	else
		ChangePriceWnd_Price_Siller.setText(0);
	%copper =	StallWndGui_SingleItemMoney_CopperText.getValue();
	if(%copper > 0)
		ChangePriceWnd_Price_Copper.setText(%copper);
	else
		ChangePriceWnd_Price_Copper.setText(0);
}
function CloseChangePriceWnd()
{
	if(StallWnd_ChangePriceWnd.isvisible())
	{
		StallWnd_ChangePriceWnd.setvisible(0);
		clearStallInput();
		$IsDirty--;
	}
}

function OpenOrCloseSetStallWnd()
{
	if(StallWnd_SetStallWnd.isVisible())
	{
		CloseSetStallWnd();
	}
	else
	{
		OpenSetStallWnd();
	}
}

function OpenSetStallWnd()
{
	GameMainWndGui_Layer4.pushtoback(StallWnd_SetStallWnd);
	StallWnd_SetStallWnd.setvisible(1);
	$IsDirty++;
}

function CloseSetStallWnd()
{
	if(StallWnd_SetStallWnd.isvisible())
	{
		StallWnd_SetStallWnd.setvisible(0);
		$IsDirty--;
	}
}

function OpenOrCloseSetTradeItemWnd()
{
	if(StallWnd_SetTradeItemWnd.isVisible())
	{
		CloseSetTradeItemWnd();
	}
	else
	{
		OpenSetTradeItemWnd();
	}
}

function OpenSetTradeItemWnd()
{
	GameMainWndGui_Layer4.pushtoback(StallWnd_SetTradeItemWnd);
	StallWnd_SetTradeItemWnd.setvisible(1);
	$IsDirty++;
}

function CloseSetTradeItemWnd()
{
	StallWnd_SetTradeItemWnd.setvisible(0);
	$IsDirty--;
}
//创建shortcut
function CreateShortCut()
{
	%num = 12 + GetBagSize(4) + GetBagSize(5);
	%posX = 0;
	%posY = 0;
	for(%i=0; %i<%num;%i++)
	{ 
		%ShortX = 12 + %posX*38;
		%ShortY = 124 + %posY*38;
		new GuiBitmapCtrl("Stall_Bitmap_" @ %i) {
    	canSaveDynamicFields = "0";
    	Enabled = "1";
    	isContainer = "0";
    	Profile = "GuiDefaultProfile";
    	HorizSizing = "right";
    	VertSizing = "bottom";
    	position = %ShortX SPC %ShortY;
    	Extent = "36 36";
    	MinExtent = "8 2";
    	canSave = "1";
    	Visible = "1";
    	hovertime = "1000";
    	bitmap = "gameres/gui/images/GUIWindow31_1_006.png";
    	wrap = "0";
   	};
		new dGuiShortCut("StallWndGui_ShortCut_" @ %i) {
       canSaveDynamicFields = "0";
       Enabled = "1";
       isContainer = "0";
       HorizSizing = "right";
       VertSizing = "bottom";
       position = (%ShortX+2) @ " " @ (%ShortY+2);
       Extent = "32 32";
       MinExtent = "8 2";
       canSave = "1";
       Visible = "1";
       hovertime = "1000";
       slotType = "7";
       slotCol = %i;
       boudary = "0 0 32 32";
       MouseDownCommand = "setPitchOnIndex(" @%i @");";
    };
    %posX ++;
		if(%posX > 5)
		{
			%posY ++;
			%posX = 0;
		}
    StallWndGui.add("Stall_Bitmap_" @ %i);
    StallWndGui.add("StallWndGui_ShortCut_" @ %i);
	}
	
}
//设置改价界面位置
function setChangePriceWndPos(%index, %type)
{ 
	if(%type == 1)
	{                
		%obj = "StallWndGui_ShortCut_" @ %index;
		if(isObject(%obj))
			%obj.setBeSelected(1);
		StallWnd_ChangePriceWnd.setposition(getword(%obj.getposition(),0) +100, getword(%obj.getposition(),1) + 100);
	}
	else if(%type == 2)
	{
		%obj = "StallWndGui_PetShortCut_" @ %index;
		if(isObject(%obj))
			%obj.setBeSelected(1);
		StallWnd_ChangePriceWnd.setposition(getword(%obj.getposition(),0) +100, getword(%obj.getposition(),1) + 100);
	}
}
//打开改价界面
function OpenPrice(%index)
{
	showStallMoney(0,0);
	OpenChangePriceWnd(0);
	%obj = "StallWndGui_PetShortCut_" @ $StallPetListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		 %obj.setBeSelected(0);
	}
	%obj = "StallWndGui_ShortCut_" @ $pitchOnIndex; 
	if(isObject(%obj))
	{
		if(%obj.getBeSelected ())
	 	%obj.setBeSelected(0);
	}
	
	$pitchOnIndex = %index;
	setChangePriceWndPos(%index, 1);
	ChangePriceWnd_OnStall.command = "setChangeMoneyOk(0);";
	ChangePriceWnd_Cancel.command = "setCancelStallSetPrice();";
}
//打开宠物改价
function oPenPetPrice(%index)
{
	echo(%index);
	showStallMoney(0,0);
	OpenChangePriceWnd(0);
	%obj = "StallWndGui_ShortCut_" @ $pitchOnIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		 %obj.setBeSelected(0);
	}
	%obj = "StallWndGui_PetShortCut_" @ $StallPetListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		 %obj.setBeSelected(0);
	}
	$StallPetListIndex = %index;
	setChangePriceWndPos($StallPetListIndex, 2);
	ChangePriceWnd_OnStall.command = "setPetChangeMoneyOk(0);";
	ChangePriceWnd_Cancel.command = "setCancelStallSetPrice();";	
}
//右键点击物品
$srcIndex = -1;
function oPenSetPrice(%index,%src)
{
	showStallMoney(0,0);
	OpenChangePriceWnd(0);
	clearAllSelectState();
	%obj = "StallWndGui_ShortCut_" @ %index;
	%obj.setBeSelected(1);
	
	$pitchOnIndex = %index;
	$srcIndex = %src;
	setChangePriceWndPos(%index, 1);
	ChangePriceWnd_OnStall.command = "setChangeMoneyOk(1);";
	ChangePriceWnd_Cancel.command = "setCancelStallSetPrice();";
	
}
//右键点击宠物交换到宠物摆摊兰
function openSetPetPrice(%index, %src)
{
	showStallMoney(0,0);
	OpenChangePriceWnd(0);
	clearAllSelectState();
	%obj = "StallWndGui_PetShortCut_" @ %index;
	%obj.setBeSelected(1);
	
	$StallPetListIndex = %index;
	$srcIndex = %src;
	setChangePriceWndPos($StallPetListIndex, 2);
	ChangePriceWnd_OnStall.command = "setPetChangeMoneyOk(1);";
	ChangePriceWnd_Cancel.command = "setCancelStallSetPrice();";
}
//设置改价取消
function setCancelStallSetPrice()
{
	%obj = "StallWndGui_ShortCut_" @ $pitchOnIndex;
	if(isObject(%obj))
		%obj.setBeSelected(0);
	%obj = "StallWndGui_PetShortCut_" @ $StallPetListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		 %obj.setBeSelected(0);
	}
	cancleExchange();
	CloseChangePriceWnd();
}
//删除shortcut
function clearStallSlots()
{
	clearAllSelect();
	%num = 12 + GetBagSize(4) + GetBagSize(5);
	for(%i=0; %i<%num; %i++)
	{
		%obj = "StallWndGui_ShortCut_"@%i;
		if(isObject(%obj))
			%obj.delete();
		%obj = "Stall_Bitmap_" @ %i;
		if(isObject(%obj))
			%obj.delete();
	}
	$pitchOnIndex = -1;
	$StallPetListIndex = -1;
	clearStall(0);
}
//清除所有选中状态
function clearAllSelectState()
{
	%obj = "StallWndGui_PetShortCut_" @ $StallPetListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		 	%obj.setBeSelected(0);
	} 
	%obj = "StallWndGui_LookupShortCut_" @ $selectBuyItemListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
	  	%obj.setBeSelected(0);
	}    
	%obj = "StallWndGui_ShortCut_" @ $pitchOnIndex; 
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
	  	%obj.setBeSelected(0);
	}
}
//设置选中index
function setPitchOnIndex(%index)
{    
	clearAllSelectState();
	$pitchOnIndex = %index;
	%obj = "StallWndGui_ShortCut_" @ $pitchOnIndex;
	if(isObject(%obj))
		%obj.setBeSelected(1);  
	showStallItemMoney($pitchOnIndex, 1);
}

//收购栏显示价钱
$selectBuyItemListIndex = -1;
function selectBuyItemList(%index)
{
	clearAllSelectState();
	$selectBuyItemListIndex = %index;
	%obj = "StallWndGui_LookupShortCut_" @ $selectBuyItemListIndex;
	if(isObject(%obj))
	  %obj.setBeSelected(1);
	 showStallItemMoney($selectBuyItemListIndex, 3);
}
//改价Ok
function setChangeMoneyOk(%flag)
{
	%gold = ChangePriceWnd_Price_Gold.getValue();
	%siller = ChangePriceWnd_Price_Siller.getvalue();
	%copper = ChangePriceWnd_Price_Copper.getValue();
	%money = calculatePrice(%gold, %siller, %copper);
	if(%money == 0)
		return;
	if(%flag == 0)
		SetStallPrice($pitchOnIndex, %money, 1);
	else
	{
		if($srcIndex > -1)
			setAddEvent($srcIndex, 1);
		setStallMoney($pitchOnIndex, %money, 1);
	}
	$srcIndex = -1;
	CloseChangePriceWnd();
}
//宠物改价
function setPetChangeMoneyOk(%flag)
{
	%gold = ChangePriceWnd_Price_Gold.getValue();
	%siller = ChangePriceWnd_Price_Siller.getvalue();
	%copper = ChangePriceWnd_Price_Copper.getValue();
	%money = calculatePrice(%gold, %siller, %copper);
	if(%money $= 0)
		return;
	if(%flag == 0)
		SetStallPrice($StallPetListIndex, %money, 2);
	else
	{		
		setAddEvent($srcIndex, 2);
		setStallMoney($StallPetListIndex, %money, 2);
	}
	CloseChangePriceWnd();
}
//初始
function initStallWnd(%name, %info, %mapID, %x, %y, %rate, %playerName, %playerLevel)
{
	StallWndGui_StallName.setText(%name);
	%mapName = getViewMapData(%mapID,2);
	%posText = Pos3DTo2D(%mapID,%x, %y);
	%posx = getWord(%posText, 0);
	%posy = getWord(%posText, 1);
	%address =%mapName @ "[" @%posx @" "@%posy @"]";
	StallWndGui_StallAddress.settext(%address);
	StallWndGui_StallRevenue.setText(%rate @"%");
	StallWndGui_StallPlayerName.setText(%playerName);
	StallWndGui_StallPlayerLevel.setText(%playerLevel@"级");
}
//显示价格
function showStallMoney(%money, %num)
{
	if(%money >= 10000)
	{	
		StallWndGui_SingleItemMoney_Gold.setVisible(1);
		StallWndGui_SingleItemMoney_Siller.setVisible(1);
		%gold = mfloor(%money / 10000);
	  %silver	= mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  StallWndGui_SingleItemMoney_GoldText.settext(%gold);
	  StallWndGui_SingleItemMoney_SillerText.settext(%silver);
	  StallWndGui_SingleItemMoney_CopperText.settext(%copper);
	}
	else if(%money < 100)
	{
		%copper = %money;
		StallWndGui_SingleItemMoney_Gold.setVisible(0);
		StallWndGui_SingleItemMoney_Siller.setVisible(0);
		StallWndGui_SingleItemMoney_GoldText.settext("");
	  StallWndGui_SingleItemMoney_SillerText.settext("");
	  StallWndGui_SingleItemMoney_CopperText.settext(%copper);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
	  StallWndGui_SingleItemMoney_Gold.setVisible(0);
		StallWndGui_SingleItemMoney_Siller.setVisible(1);
		StallWndGui_SingleItemMoney_GoldText.settext("");
	  StallWndGui_SingleItemMoney_SillerText.settext(%silver);
	  StallWndGui_SingleItemMoney_CopperText.settext(%copper);
	}
	%allMoney = %num * %money;
	if(%allMoney >= 10000)
	{
		StallWndGui_AllItemMoney_Gold.setVisible(1);
		StallWndGui_AllItemMoney_Siller.setVisible(1);
		%gold = mfloor(%allMoney / 10000);
	  %silver	= mfloor((%allMoney - (%gold * 10000)) / 100);
	  %copper	= (%allMoney - (%gold * 10000 + (%silver * 100)));
	  StallWndGui_AllItemMoney_GoldText.settext(%gold);
	  StallWndGui_AllItemMoney_SillerText.settext(%silver);
	  StallWndGui_AllItemMoney_CopperText.settext(%copper);
	}
	else if(%allMoney < 100)
	{
		%copper = %allMoney;
		StallWndGui_AllItemMoney_Gold.setVisible(0);
		StallWndGui_AllItemMoney_Siller.setVisible(0);
		StallWndGui_AllItemMoney_GoldText.settext("");
	  StallWndGui_AllItemMoney_SillerText.settext("");
	  StallWndGui_AllItemMoney_CopperText.settext(%copper);
	}
	else
	{
		%silver = mfloor(%allMoney / 100);
		%copper = (%allMoney - (%silver * 100));
	  StallWndGui_AllItemMoney_Gold.setVisible(0);
		StallWndGui_AllItemMoney_Siller.setVisible(1);
		StallWndGui_AllItemMoney_GoldText.settext("");
	  StallWndGui_AllItemMoney_SillerText.settext(%silver);
	  StallWndGui_AllItemMoney_CopperText.settext(%copper);
	}
}
//得到重名数据
function changeStallName()
{
	%name = StallWndGui_StallName.getText();
	setStallName(%name);
}
//重命名
function stallRename(%name)
{
	StallWndGui_StallName.setText(%name);
}

//清除输入金钱
function clearStallInput()
{
	ChangePriceWnd_Price_Gold.setValue(0);
	ChangePriceWnd_Price_Siller.setValue(0);
	ChangePriceWnd_Price_Copper.setValue(0);
}

//显示出摊或收摊
function showStallState(%flag)
{
	if(%flag $= 1)
	{
		StallWndGui_StartStall.setVisible(0);
		StallWndGui_OverStall.setVisible(1);
	}
	else
	{
		StallWndGui_StartStall.setVisible(1);
		StallWndGui_OverStall.setVisible(0);
	}
}
//-------------------------------------------------------------------------------------
//打开别人摆摊界面
function openSeeStall(%num)
{
	OpenStallSeeWnd();
	CreateSeeStallShortcut(%num);
	showSeeStallMoney(0,0,0);
}
//创建购买摆摊拦
$StallSpaceNum = 12;
function CreateSeeStallShortcut(%num)
{
	$StallSpaceNum = %num;
	%posX = 0;
	%posY = 0;
	for(%i=0; %i<%num;%i++)
	{ 
		%ShortX = 12 + %posX*38;
		%ShortY = 124 + %posY*38;
		new GuiBitmapCtrl("SeeStall_Bitmap_" @ %i) {
    	canSaveDynamicFields = "0";
    	Enabled = "1";
    	isContainer = "0";
    	Profile = "GuiDefaultProfile";
    	HorizSizing = "right";
    	VertSizing = "bottom";
    	position = %ShortX SPC %ShortY;
    	Extent = "36 36";
    	MinExtent = "8 2";
    	canSave = "1";
    	Visible = "1";
    	hovertime = "1000";
    	bitmap = "gameres/gui/images/GUIWindow31_1_006.png";
    	wrap = "0";
   	};
   	
		new dGuiShortCut("SeeStallWndGui_ShortCut_" @ %i) {
       canSaveDynamicFields = "0";
       Enabled = "1";
       isContainer = "0";
       HorizSizing = "right";
       VertSizing = "bottom";
       position = (%ShortX+2) @ " " @ (%ShortY+2);
       Extent = "32 32";
       MinExtent = "8 2";
       canSave = "1";
       Visible = "1";
       hovertime = "1000";
       slotType = "27";
       slotCol = %i;
       boudary = "0 0 32 32";
       disabledrag = true;
       MouseDownCommand = "setSelectStallItem(" @%i @");";
    };
    %posX ++;
		if(%posX > 5)
		{
			%posY ++;
			%posX = 0;
		}
    StallSeeWndGui.add("SeeStall_Bitmap_" @ %i);
    StallSeeWndGui.add("SeeStallWndGui_ShortCut_" @ %i);
	}
}
//清除别人摆摊
function clearSeeStallSlots()
{
	clearAllSeeSelected();
	for(%i=0; %i<$StallSpaceNum; %i++)
	{
		%obj = "SeeStallWndGui_ShortCut_"@%i;
		if(isObject(%obj))
			%obj.delete();
		%obj = "SeeStall_Bitmap_" @ %i;
		if(isObject(%obj))
			%obj.delete();
	}
	$SeeStallPatchOnIndex = -1;
	$LookPetListIndex = -1;
	$SeeSellListIndex = -1;
}

//
function initSeeStallWnd(%name, %info, %mapID, %x, %y, %rate, %playerName, %level)
{
	StallSeeWndGui_StallName.setText(%name);
	//StallSeeWndGui_StallInfo.setText(%info);
	%mapName = getViewMapData(%mapID,2);
	%posText = Pos3DTo2D(%mapID,%x, %y);
	%posx = getWord(%posText, 0);
	%posy = getWord(%posText, 1);
	%address =%mapName @ "[" @%posx @" "@%posy @"]";
	StallSeeWndGui_StallAddress.setText(%address);
	StallSeeWndGui_StallRevenue.setText(%rate @ "%");
	StallSeeWndGui_StallPlayerName.setText(%playerName);
	StallSeeWndGui_StallPlayerLevel.setText(%level @ "级");
}

//显示察看价格
$SeeStallPatchOnIndex = -1;
$SeeStallPatchOnPrice = 0;
//$SeeStallPatchOnNum = 1;
$SelectTypeFlag = 0; // 选中那个栏位 1-普通栏位 2-宠物 3-收购
$SeeSelectIndex = -1;
function showSeeStallMoney(%money, %num, %index, %maxNum)
{
	$SeeSelectIndex = %index;
	$SeeStallPatchOnPrice = %money;
	//$SeeStallPatchOnNum = %num;
	$StallerHaveItemNum = %num;
	$StallerItemMaxNum = %maxNum;
	if(%money >= 10000)
	{	
		StallSeeWndGui_SingleItemMoney_Gold.setVisible(1);
		StallSeeWndGui_SingleItemMoney_Siller.setVisible(1);
		%gold = mfloor(%money / 10000);
	  %silver	= mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  StallSeeWndGui_SingleItemMoney_GoldText.settext(%gold);
	  StallSeeWndGui_SingleItemMoney_SillerText.settext(%silver);
	  StallSeeWndGui_SingleItemMoney_CopperText.settext(%copper);
	}
	else if(%money < 100)
	{
		%copper = %money;
		StallSeeWndGui_SingleItemMoney_Gold.setVisible(0);
		StallSeeWndGui_SingleItemMoney_Siller.setVisible(0);
		StallSeeWndGui_SingleItemMoney_GoldText.settext("");
	  StallSeeWndGui_SingleItemMoney_SillerText.settext("");
	  StallSeeWndGui_SingleItemMoney_CopperText.settext(%copper);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
	  StallSeeWndGui_SingleItemMoney_Gold.setVisible(0);
		StallSeeWndGui_SingleItemMoney_Siller.setVisible(1);
		StallSeeWndGui_SingleItemMoney_GoldText.settext("");
	  StallSeeWndGui_SingleItemMoney_SillerText.settext(%silver);
	  StallSeeWndGui_SingleItemMoney_CopperText.settext(%copper);
	}
	%allMoney = %num * %money;
	if(%allMoney >= 10000)
	{
		StallSeeWndGui_AllItemMoney_Gold.setVisible(1);
		StallSeeWndGui_AllItemMoney_Siller.setVisible(1);
		%gold = mfloor(%allMoney / 10000);
	  %silver	= mfloor((%allMoney - (%gold * 10000)) / 100);
	  %copper	= (%allMoney - (%gold * 10000 + (%silver * 100)));
	  StallSeeWndGui_AllItemMoney_GoldText.settext(%gold);
	  StallSeeWndGui_AllItemMoney_SillerText.settext(%silver);
	  StallSeeWndGui_AllItemMoney_CopperText.settext(%copper);
	}
	else if(%allMoney < 100)
	{
		%copper = %allMoney;
		StallSeeWndGui_AllItemMoney_Gold.setVisible(0);
		StallSeeWndGui_AllItemMoney_Siller.setVisible(0);
		StallSeeWndGui_AllItemMoney_GoldText.settext("");
	  StallSeeWndGui_AllItemMoney_SillerText.settext("");
	  StallSeeWndGui_AllItemMoney_CopperText.settext(%copper);
	}
	else
	{
		%silver = mfloor(%allMoney / 100);
		%copper = (%allMoney - (%silver * 100));
	  StallSeeWndGui_AllItemMoney_Gold.setVisible(0);
		StallSeeWndGui_AllItemMoney_Siller.setVisible(1);
		StallSeeWndGui_AllItemMoney_GoldText.settext("");
	  StallSeeWndGui_AllItemMoney_SillerText.settext(%silver);
	  StallSeeWndGui_AllItemMoney_CopperText.settext(%copper);
	}
	if(StallWnd_SetTradeItemWnd.isvisible())
		showStallBuyItem();
}
//显示购买框
function showStallBuyItem()
{
	if($SeeStallPatchOnPrice >= 10000)
	{	
		SetTradeItemWnd_Single_Gold.setVisible(1);
		SetTradeItemWnd_Single_Siller.setVisible(1);
		%gold = mfloor($SeeStallPatchOnPrice / 10000);
	  %silver	= mfloor(($SeeStallPatchOnPrice - (%gold * 10000)) / 100);
	  %copper	= ($SeeStallPatchOnPrice - (%gold * 10000 + (%silver * 100)));
	  SetTradeItemWnd_SingleText_Gold.settext(%gold);
	  SetTradeItemWnd_SingleText_Siller.settext(%silver);
	  SetTradeItemWnd_SingleText_Copper.settext(%copper);
	}
	else if($SeeStallPatchOnPrice < 100)
	{
		%copper = $SeeStallPatchOnPrice;
		SetTradeItemWnd_Single_Gold.setVisible(0);
		SetTradeItemWnd_Single_Siller.setVisible(0);
		SetTradeItemWnd_SingleText_Gold.settext("");
	  SetTradeItemWnd_SingleText_Siller.settext("");
	  SetTradeItemWnd_SingleText_Copper.settext(%copper);
	}
	else
	{
		%silver = mfloor($SeeStallPatchOnPrice / 100);
		%copper = ($SeeStallPatchOnPrice - (%silver * 100));
	  SetTradeItemWnd_Single_Gold.setVisible(0);
		SetTradeItemWnd_Single_Siller.setVisible(1);
		SetTradeItemWnd_SingleText_Gold.settext("");
	  SetTradeItemWnd_SingleText_Siller.settext(%silver);
	  SetTradeItemWnd_SingleText_Copper.settext(%copper);
	}
	SetTradeItemWnd_ItemNum.setText("1");
	showStallBuyItemAllPrice();
}
//显示总价
function showStallBuyItemAllPrice()
{
	%num = SetTradeItemWnd_ItemNum.getValue();
	%money = $SeeStallPatchOnPrice * %num;
	if(%money >= 10000)
	{	
		SetTradeItemWnd_All_Gold.setVisible(1);
		SetTradeItemWnd_All_Siller.setVisible(1);
		%gold = mfloor(%money / 10000);
	  %silver	= mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  SetTradeItemWnd_AllText_Gold.settext(%gold);
	  SetTradeItemWnd_AllText_Siller.settext(%silver);
	  SetTradeItemWnd_AllText_Copper.settext(%copper);
	}
	else if(%money < 100)
	{
		%copper = %money;
		SetTradeItemWnd_All_Gold.setVisible(0);
		SetTradeItemWnd_All_Siller.setVisible(0);
		SetTradeItemWnd_AllText_Gold.settext("");
	  SetTradeItemWnd_AllText_Siller.settext("");
	  SetTradeItemWnd_AllText_Copper.settext(%copper);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
	  SetTradeItemWnd_All_Gold.setVisible(0);
		SetTradeItemWnd_All_Siller.setVisible(1);
		SetTradeItemWnd_AllText_Gold.settext("");
	  SetTradeItemWnd_AllText_Siller.settext(%silver);
	  SetTradeItemWnd_AllText_Copper.settext(%copper);
	}
}
//加
$StallerHaveItemNum = 1;
$StallerItemMaxNum = 1;
function addBuyStallNum()
{
	%num = SetTradeItemWnd_ItemNum.getValue();
	%num++;
	if(%num > $StallerHaveItemNum)
		%num = $StallerHaveItemNum;
	SetTradeItemWnd_ItemNum.setText(%num);
	showStallBuyItemAllPrice();
}
//减
function reduceBuyStallNum()
{
	%num = SetTradeItemWnd_ItemNum.getValue();
	%num--;
	if(%num < 1)
		%num = 1;
	SetTradeItemWnd_ItemNum.setText(%num);
	showStallBuyItemAllPrice();
}

function setTradeBuyNum()
{
	if($StallerItemMaxNum > $StallerHaveItemNum)
		SetTradeItemWnd_ItemNum.setText($StallerHaveItemNum);
	else
		SetTradeItemWnd_ItemNum.setText($StallerItemMaxNum);
}
//点击购买
function openSetBuyInfo()
{
	 %gold = StallSeeWndGui_SingleItemMoney_GoldText.getValue();
	 %silver = StallSeeWndGui_SingleItemMoney_SillerText.getValue();
	 %copper = StallSeeWndGui_SingleItemMoney_CopperText.getValue();
	 %money = calculatePrice(%gold, %siller, %copper);
 	if(%money == 0)
		return;
	else
	{
		OpenSetTradeItemWnd();
		showStallBuyItem();
	}
}
//确定购买
function setBuyStallItemOk()
{
	%num = SetTradeItemWnd_ItemNum.getValue();
	makeSureToBuyStallItem($SeeSelectIndex, %num, $SelectTypeFlag);
	CloseSetTradeItemWnd();
}   

// 选中控件
function setSelectStallItem(%index)
{
	clearAllSeeSelected();
	$SelectTypeFlag = 1;
	$SeeStallPatchOnIndex = %index;
	%obj = "SeeStallWndGui_ShortCut_" @ $SeeStallPatchOnIndex;
	if(isObject(%obj))
		%obj.setBeSelected(1);  
	CloseSetTradeItemWnd();
	setSeeItemPrice(%index, 1);
}

//摆摊栏宠物兰
$StallPetListIndex = -1;
$LookPetListIndex = -1;
function selectPetList(%index)
{
	clearAllSelectState();
	$StallPetListIndex = %index;
	%obj = "StallWndGui_PetShortCut_" @ $StallPetListIndex;
	if(isObject(%obj))
		%obj.setBeSelected(1);
	showStallItemMoney($StallPetListIndex, 2);
}
//清除所有察看摆摊栏选中状态
$SeeSellListIndex = -1; // 出售栏下标
function clearAllSeeSelected()
{
	%obj = "SeeStallWndGui_ShortCut_" @ $SeeStallPatchOnIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		  %obj.setBeSelected(0);
	}
	%obj = "StallSeeWndGui_PetShortCut_" @ $LookPetListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		  %obj.setBeSelected(0);
	}
	%obj = "StallSeeWndGui_LookupShortCut_" @ $SeeSellListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		  %obj.setBeSelected(0);
	}
}
function selectLookPetList(%index)
{
	clearAllSeeSelected();
	$SelectTypeFlag = 2;
	$LookPetListIndex = %index;
	%obj = "StallSeeWndGui_PetShortCut_" @ $LookPetListIndex;
	if(isObject(%obj))
		%obj.setBeSelected(1);
	setSeeItemPrice(%index, 2);
	CloseSetTradeItemWnd();
}
//显示查找结果
function showResultItemInfo(%index, %name)
{
	("LookupReturnWnd_ItemName_"@%index).setText(%name);
}

function clearResultItemInfo()
{
	for(%i=1; %i<4; %i++)
	{
	 ("LookupReturnWnd_ItemName_"@%i).setText("");
	}
	
}
$TotalLookUpPage = 0;
$CurrentLookUpPage = 1;
$selectLookUpIndex = -1;
function showResultPage(%num,%Total)
{
	%text = %num @"/"@%Total;
	$TotalLookUpPage = %Total;
//	$CurrentLookUpPage = %num;
	LookupReturnWnd_PageNum.setText(%text);
}

function LookUpItemOk()
{
	%name = LookupWnd_TextEdit.getValue();
	clearResultItemInfo();
	if(LookupWnd_CheckBox_1.isStateOn())
		LookUpItem(%name, 1);
	else
		LookUpItem(%name, 2);
}
//查找提示信息
function HintInfor(%fail)
{
	if(%fail)
	{
		LookupWnd_Hint.setText("没有你所要查找的物品，请确认后再查找");
		showResultPage(0,0);
	}
	else
		LookupWnd_Hint.setText("成功查找到物品");
}

function AddLookUpPage()
{
	$CurrentLookUpPage++;
	if($CurrentLookUpPage > $TotalLookUpPage-1)
		$CurrentLookUpPage = $TotalLookUpPage;
	showLookUpItem($CurrentLookUpPage);	
}

function ReduceLookUpPage()
{
	$CurrentLookUpPage--;
	if($CurrentLookUpPage <= 1)
		$CurrentLookUpPage = 1;
	showLookUpItem($CurrentLookUpPage);	
}

function AddStallBuyNum()
{
	%buyNum = LookupReturnWnd_Edit_BuyItemNum.getValue();
	%buyNum++;
	if(%buyNum > 999)
		%buyNum = 999;
	LookupReturnWnd_Edit_BuyItemNum.setValue(%buyNum);
}

function ReduceStallBuyNum()
{
	%buyNum = LookupReturnWnd_Edit_BuyItemNum.getValue();
	%buyNum--;
	if(%buyNum < 1)
	 %buyNum = 1;
	LookupReturnWnd_Edit_BuyItemNum.setValue(%buyNum);
}

function addItemToLookUpList()
{
	%gold = LookupReturnWnd_Edit_Gold.getValue();
	%siller = LookupReturnWnd_Edit_Sliver.getValue();
	%copper = LookupReturnWnd_Edit_Copper.getValue();
	%money = calculatePrice(%gold, %siller, %copper);
	%num = LookupReturnWnd_Edit_BuyItemNum.getValue();
	AddNeedBuyItem($selectLookUpIndex, %money, %num);
	//CloseLookupReturnWnd();
}

function selectLookUpList(%index)
{
	$selectLookUpIndex = %index;
}
//下架
function removeFromStall()
{
	%obj = "StallWndGui_ShortCut_" @ $pitchOnIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected ())
		{
			removeStall($pitchOnIndex, 1);
		}
	}
	%obj = "StallWndGui_PetShortCut_" @ $StallPetListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		{
			removeStall($StallPetListIndex, 2);
		}
	}
	%obj = "StallWndGui_LookupShortCut_" @ $selectBuyItemListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
	  	removeStall($selectBuyItemListIndex, 3);
	} 
	else
		return;   
}

//清除选中状态
function clearSelectState(%index,%type)
{
	if(%type == 1)
	{
		%obj = "StallWndGui_ShortCut_" @ %index;
		if(isObject(%obj))
			%obj.setBeSelected(0);
	}
	else if(%type == 2)
	{
		%obj = "StallWndGui_PetShortCut_" @ %index;
		if(isObject(%obj))
			%obj.setBeSelected(0);
	}
	else if(%type == 3)
	{
		%obj = "StallWndGui_LookupShortCut_" @ %index;
		if(isObject(%obj))
			%obj.setBeSelected(0);
	}
	showStallMoney(0,0);
}
//全部下架
function clearAllSelect()
{
	%obj = "StallWndGui_ShortCut_" @ $pitchOnIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected ())
		{
			%obj.setBeSelected(0);
		}
	}
	%obj = "StallWndGui_PetShortCut_" @ $StallPetListIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		{
			%obj.setBeSelected(0);
		}
	}
	showStallMoney(0,0);
}

//打开或关闭出售栏
function openBuyStallItemInfo()
{
	GameMainWndGui_Layer4.pushtoback(StallWnd_SellItemWnd);
	StallWnd_SellItemWnd.setvisible(1);
	$IsDirty++;
}

function closeSellStallItemInfo()
{
	StallWnd_SellItemWnd.setvisible(0);
	$BuyStallItemIndex = -1;
	$SellStallItemNum = 1;
	$BuyStallItemMoney = 0;
	SellItemWnd_ItemNum.setValue(1);
	$IsDirty--;
}

$SellStallItemNum = 1; //出售总量
$SellStallItemMaxOver = 1; //出售物品的叠加数
$BuyStallItemIndex = -1;
$BuyStallItemMoney = 0;
function showSellStallItemInfo(%money, %num, %index, %maxNum)
{
	openBuyStallItemInfo();
	$SellStallItemNum = %num;
	$BuyStallItemIndex = %index;
	$BuyStallItemMoney = %money;
	$SellStallItemMaxOver = %maxNum;
	if(%money >= 10000)
	{	
		SellItemWnd_Single_Gold.setVisible(1);
		SellItemWnd_Single_Siller.setVisible(1);
		%gold = mfloor(%money / 10000);
	  %silver	= mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  SellItemWnd_SingleText_Gold.settext(%gold);
	  SellItemWnd_SingleText_Siller.settext(%silver);
	  SellItemWnd_SingleText_Copper.settext(%copper);
	}
	else if(%money < 100)
	{
		%copper = %money;
		SellItemWnd_Single_Gold.setVisible(0);
		SellItemWnd_Single_Siller.setVisible(0);
		SellItemWnd_SingleText_Gold.settext("");
	  SellItemWnd_SingleText_Siller.settext("");
	  SellItemWnd_SingleText_Copper.settext(%copper);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
	  SellItemWnd_Single_Gold.setVisible(0);
		SellItemWnd_Single_Siller.setVisible(1);
		SellItemWnd_SingleText_Gold.settext("");
	  SellItemWnd_SingleText_Siller.settext(%silver);
	  SellItemWnd_SingleText_Copper.settext(%copper);
	}
	showAllNeedMoney(1);
}

function setSellItemNum()
{
	if($SellStallItemNum < $SellStallItemMaxOver)
		SellItemWnd_ItemNum.setValue($SellStallItemNum);
	else
		SellItemWnd_ItemNum.setValue($SellStallItemMaxOver);
}

function showAllNeedMoney(%num)
{
	%allMoney = $BuyStallItemMoney * %num;
	if(%allMoney >= 10000)
	{
		SellItemWnd_All_Gold.setVisible(1);
		SellItemWnd_All_Siller.setVisible(1);
		%gold = mfloor(%allMoney / 10000);
	  %silver	= mfloor((%allMoney - (%gold * 10000)) / 100);
	  %copper	= (%allMoney - (%gold * 10000 + (%silver * 100)));
	  SellItemWnd_AllText_Gold.settext(%gold);
	  SellItemWnd_AllText_Siller.settext(%silver);
	  SellItemWnd_AllText_Copper.settext(%copper);
	}
	else if(%allMoney < 100)
	{
		%copper = %allMoney;
		SellItemWnd_All_Gold.setVisible(0);
		SellItemWnd_All_Siller.setVisible(0);
		SellItemWnd_AllText_Gold.settext("");
	  SellItemWnd_AllText_Siller.settext("");
	  SellItemWnd_AllText_Copper.settext(%copper);
	}
	else
	{
		%silver = mfloor(%allMoney / 100);
		%copper = (%allMoney - (%silver * 100));
	  SellItemWnd_All_Gold.setVisible(0);
		SellItemWnd_All_Siller.setVisible(1);
		SellItemWnd_AllText_Gold.settext("");
	  SellItemWnd_AllText_Siller.settext(%silver);
	  SellItemWnd_AllText_Copper.settext(%copper);
	}
}
//添加出售数量
function addSellStallBulistItem()
{
	%num = SellItemWnd_ItemNum.getValue();
	%num ++;
	if(%num > $SellStallItemNum)
		%num = $SellStallItemNum;
	SellItemWnd_ItemNum.setValue(%num);
	showAllNeedMoney(%num);
}
//减少出售数量
function reduceSellStallBuyListItem()
{
	%num = SellItemWnd_ItemNum.getValue();
	%num --;
	if(%num < 1)
		%num = 1;
	SellItemWnd_ItemNum.setValue(%num);
	showAllNeedMoney(%num);
}

//出售OK
function setSellStallItemOk()
{
	%num = SellItemWnd_ItemNum.getValue();
	makeSureToBuyStallItem($BuyStallItemIndex, %num, 3);
	%obj = "StallSeeWndGui_LookupShortCut_" @ $BuyStallItemIndex;
	if(isObject(%obj))
	{
		if(%obj.getBeSelected())
		  %obj.setBeSelected(0);
	}
	closeSellStallItemInfo();
}
//打开购买确认
function openBuyStallItemWnd(%index, %flag, %money, %num, %maxNum)
{
	$SeeSelectIndex = %index;
	$SelectTypeFlag = %flag;
	$SeeStallPatchOnPrice = %money;
	OpenSetTradeItemWnd();
	showStallBuyItem();
	$StallerHaveItemNum = %num;
	$StallerItemMaxNum = %maxNum;
}

//显示摆摊信息
function stallReInfo(%info, %flag)
{
	StallInfoWnd_MLTextEdit.setText(%info);
	StallInfoWnd_Note_Edit.setActive(0);
	StallInfoWnd_Send.setActive(0);
	if(%flag)
	{
		StallInfoWnd_Btn_Buy.setVisible(0);
		StallInfoWnd_ClearInfo.setVisible(0);
		StallInfoWnd_ChangeInfo.setVisible(0);
		StallInfoWnd_MLTextEdit.setActive(0);
		StallInfoWnd_Btn_Sell.setActive(0);
		StallInfoWnd_Note_Edit.setActive(1);
		StallInfoWnd_Send.setActive(1);
	}
}
//修改摆摊信息
function doModifactionOK()
{
	%info = StallInfoWnd_MLTextEdit.getText();
	setStallInfo(%info);
}

function sendChatMSG()
{
	%content = StallInfoWnd_Note_Edit.getText();
	addStallChatMsg(%content);
	echo("Content:"@%content);
	StallInfoWnd_Note_Edit.setText("");
}

function addStallChatMessage(%chatMessage)
{
	//StallInfoWnd_Scroll_LeaveWord.setvisible(1);
	StallInfoWnd_List_LeaveWord.addMsg(14, %chatMessage, 0, "");
}

function addStallTradeMessage(%msg)
{
	StallInfoWnd_List_Trade.addMsg(14, %msg, 0, "");
}

//清除交易信息
function clearTradeAndChatMessage()
{
	StallInfoWnd_List_Trade.clear();
	StallInfoWnd_List_LeaveWord.clear();
}

function ShowChatList()
{
	StallInfoWnd_Scroll_Trade.setVisible(0);
	StallInfoWnd_Scroll_LeaveWord.setVisible(1);
}

function ShowTradeList()
{
	StallInfoWnd_Scroll_LeaveWord.setVisible(0);
	StallInfoWnd_Scroll_Trade.setVisible(1);
}

function showStallHelp()
{
	if(StallHelpWndGui.isvisible())
		CloseStallHelpWnd();
	else
	{
		OpenStallHelpWnd();
		refreshTradeHelp(0);
	}
}
//打开或关闭摆摊助手
function OpenOrCloseStallHelpWnd()
{
	if(StallHelpWndGui.IsVisible())
	{
		CloseStallHelpWnd();
	}
	else
	{
		OpenStallHelpWnd();
	}
}

function OpenStallHelpWnd()
{
	if(StallHelpWndGui.isvisible())
		return;
	clearStallHelpItem();
	StallWndGui.setextent(getword(StallWndGui.getextent(),0) + getword(StallHelpWndGui.getextent(),0),getword(StallWndGui.getextent(),1));
	StallHelpWndGui.setVisible(1);
	$IsDirty++;
}

function CloseStallHelpWnd()
{
	if(!StallHelpWndGui.isvisible())
			return;
	StallWndGui.setextent(getword(StallWndGui.getextent(),0) - getword(StallHelpWndGui.getextent(),0),getword(StallWndGui.getextent(),1));
	StallHelpWndGui.setVisible(0);
	$IsDirty--;
}

$StallHelpItemCount = 0;
function RefreshTradeHelpShow(%icount)
{
	$StallHelpItemCount = %icount;
	for(%i=0; %i<%icount; %i++)
	{
		%ShortX = 1;
		%ShortY = 3 + %i*44;
      new GuiBitmapCtrl(StallHelp_Bitmap_@ %i) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "0";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = %ShortX SPC %ShortY;
         Extent = "174 42";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         mustPrerender = "0";
         hovertime = "1000";
         bitmap = "gameres/gui/images/GUIWindow29_1_009.png";
         wrap = "0";
      };
      new GuiTextCtrl(StallHelp_ItemName_@ %i) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "0";
         Profile = "GuiCurrencyShowInfoTextProfile_4";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = (%ShortX+45) SPC (%ShortY+15);
         Extent = "111 19";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         mustPrerender = "0";
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
      new GuiButtonCtrl(StallHelp_Button_@ %i) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "0";
         Profile = "GuiCoverImageButtonProfile";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = %ShortX SPC (%ShortY+1);
         Extent = "174 42";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         mustPrerender = "0";
         hovertime = "1000";
         groupNum = "-1";
         buttonType = "RadioButton";
         useMouseEvents = "0";
         IsStateOn = "0";
         command = "selectedHelpIndex(" @%i @");";
      };
      new dGuiShortCut(StallHelp_ShortCut_@ %i) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "0";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = (%ShortX+9) SPC (%ShortY+5);
         Extent = "32 32";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         mustPrerender = "0";
         hovertime = "1000";
         slotType = "37";
         slotCol = %i;
         boudary = "0 0 32 32";
         disabledrag = "1";
      };
      StallHelpGuiControl.add("StallHelp_Bitmap_"@%i);
      StallHelpGuiControl.add("StallHelp_ItemName_"@%i);
      StallHelpGuiControl.add("StallHelp_Button_"@%i);
      StallHelpGuiControl.add("StallHelp_ShortCut_"@%i);
      StallHelpGuiControl.setextent(182, 5+(%i+1)*44);
   }
}

function showStallHelpItem(%flag)
{
	clearStallHelpItem();
	if(%flag < 3)
		ShowPetInfoButton.setVisible(0);
	else
		ShowPetInfoButton.setVisible(1);
	refreshTradeHelp(%flag);
}

function clearStallHelpItem()
{
	for(%i=0; %i<$StallHelpItemCount; %i++)
	{
		%obj = "StallHelp_ShortCut_"@%i;
		if(isObject(%obj))
			%obj.delete();
		%obj = "StallHelp_Button_"@%i;
		if(isObject(%obj))
			%obj.delete();
		%obj = "StallHelp_ItemName_"@%i;	
		if(isObject(%obj))
			%obj.delete();
		%obj = "StallHelp_Bitmap_"@%i;	
		if(isObject(%obj))
			%obj.delete();			
	}
	$StallHelpItemCount = 0;
	$SlectedHelpIndex = -1;
}

function showTradeHelpItemName(%index, %name)
{
	 ("StallHelp_ItemName_"@%index).setText(%name);
}
//
$SlectedHelpIndex = -1;
function selectedHelpIndex(%index)
{
	$SlectedHelpIndex = %index;
}

function setSelectedOk()
{
	AddHelpItemOrPet($SlectedHelpIndex);
}
//---------------------------------------------------------------
//欲收购确定界面
//---------------------------------------------------------------
$maxOverNum = 0;
function OpenWillBuyItemPrice(%index, %maxNum)
{
	$maxOverNum = %maxNum;
	OpenWillBuyItemWnd();
	%obj = "StallWndGui_LookupShortCut_" @ $selectBuyItemListIndex;
	if(isObject(%obj))
		%obj.setBeSelected(0);
	$selectBuyItemListIndex = %index;
	%obj = "StallWndGui_LookupShortCut_" @ $selectBuyItemListIndex;
	if(isObject(%obj))
		%obj.setBeSelected(1);
}

function OpenWillBuyItemWnd()
{
	if(!StallWnd_BuyItemWnd.isVisible())
	{
		GameMainWndGui_Layer4.pushtoback(StallWnd_BuyItemWnd);
		StallWnd_BuyItemWnd.setvisible(1);
		StallWnd_BuyItemWnd.setPosition(getword(StallWndGui.getposition(),0)+376,getword(StallWndGui.getposition(),1)+200);
		$IsDirty++;
	}
}

function CloseWillBuyItemWnd()
{
	if(StallWnd_BuyItemWnd.isVisible())
	{
		StallWnd_BuyItemWnd.setvisible(0);
		$IsDirty--;
		BuyStallItemWndClear();
	}
}

function showWillBuyItemMoney(%money, %num)
{
	%allMoney = %money * %num;
	if(%allMoney >= 10000)
	{
		BuyItemWnd_All_Gold.setVisible(1);
		BuyItemWnd_All_Siller.setVisible(1);
		%gold = mfloor(%allMoney / 10000);
	  %silver	= mfloor((%allMoney - (%gold * 10000)) / 100);
	  %copper	= (%allMoney - (%gold * 10000 + (%silver * 100)));
	  BuyItemWnd_AllText_Gold.settext(%gold);
	  BuyItemWnd_AllText_Siller.settext(%silver);
	  BuyItemWnd_AllText_Copper.settext(%copper);
	}
	else if(%allMoney < 100)
	{
		%copper = %allMoney;
		BuyItemWnd_All_Gold.setVisible(0);
		BuyItemWnd_All_Siller.setVisible(0);
		BuyItemWnd_AllText_Gold.settext("");
	  BuyItemWnd_AllText_Siller.settext("");
	  BuyItemWnd_AllText_Copper.settext(%copper);
	}
	else
	{
		%silver = mfloor(%allMoney / 100);
		%copper = (%allMoney - (%silver * 100));
	  BuyItemWnd_All_Gold.setVisible(0);
		BuyItemWnd_All_Siller.setVisible(1);
		BuyItemWnd_AllText_Gold.settext("");
	  BuyItemWnd_AllText_Siller.settext(%silver);
	  BuyItemWnd_AllText_Copper.settext(%copper);
	}
}

function addBuyStallBulistItem()
{
	%num = BuyItemWnd_ItemNum.getValue();
	%num++;
	if(%num > 999)
		%num = 999;
	BuyItemWnd_ItemNum.setValue(%num);
	caculatePrice(%num);
}

function reduceBuyStallBuyListItem()
{
	%num = BuyItemWnd_ItemNum.getValue();
	%num--;
	if(%num < 1)
		%num = 1;
	BuyItemWnd_ItemNum.setValue(%num);
	caculatePrice(%num);
}

//计算价格
function caculatePrice(%num)
{
	%gold = BuyItemWnd_SingleText_Gold.getValue();
	%siller = BuyItemWnd_SingleText_Siller.getValue();
	%copper = BuyItemWnd_SingleText_Copper.getValue();
	%money = calculatePrice(%gold, %siller, %copper);
	showWillBuyItemMoney(%money, %num);
}

function setBuyItemNum()
{
	BuyItemWnd_ItemNum.setValue($maxOverNum);
	caculatePrice($maxOverNum);
}

function setBuyStallItemOk()
{
	%gold = BuyItemWnd_SingleText_Gold.getValue();
	%siller = BuyItemWnd_SingleText_Siller.getValue();
	%copper = BuyItemWnd_SingleText_Copper.getValue();
	%money = calculatePrice(%gold, %siller, %copper);
	%num = BuyItemWnd_ItemNum.getValue();
	makeSureAddToBuyList($selectBuyItemListIndex,%num, %money);
}

function setBuyStallItemCancel()
{
	%obj = "StallWndGui_LookupShortCut_" @ $selectBuyItemListIndex;
		if(isObject(%obj))
			%obj.setBeSelected(0);
	cancleExchange();
	CloseWillBuyItemWnd();
}

function BuyStallItemWndClear()
{
	BuyItemWnd_SingleText_Gold.setText("");
	BuyItemWnd_SingleText_Siller.setText("");
	BuyItemWnd_SingleText_Copper.setText("");
	BuyItemWnd_ItemNum.setText("1");
	$maxOverNum = 0;
	$selectBuyItemListIndex = -1;
}