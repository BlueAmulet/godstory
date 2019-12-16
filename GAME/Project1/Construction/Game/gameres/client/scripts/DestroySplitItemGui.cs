//拆分
$SplitItemNum = 1;
function PopupSplitItem(%index, %num)
{
	$SplitItemNum = %num;
	SplitItemBtn_Sure.command = "onDoSplit(" @ %index @ ");";
	SplitItemGui.setvisible(1);
	SplitItemNum_Edit.setText("1");
	SplitItemNum_Edit.makeFirstResponder(1);
	$IsDirty++;
}

function onDoSplit(%index)
{
	%num = SplitItemNum_Edit.getText();
	SplitNums(%num, %index);
	SplitItemGui.setvisible(0);
	$IsDirty--;
}
function cancelSplit()
{
	SplitItemNum_Edit.setText("");
	SplitItemGui.setvisible(0);
	$IsDirty--;
	cancelAction();
}

function addSplitItemNum()
{
	%num = SplitItemNum_Edit.getValue();
	%num++;
	if(%num >= $SplitItemNum)
		%num = $SplitItemNum-1;
	SplitItemNum_Edit.setValue(%num);
}

function reduceSplitItemNum()
{
	%num = SplitItemNum_Edit.getValue();
	%num--;
	if(%num < 1)
		%num = 1;
	SplitItemNum_Edit.setValue(%num);	
}
//丢弃
function DorpItemPopUp(%itemName, %index)
{
	OpenDialog_OK_Cancel();
	Dialog_OK_Cancel.SetContent(%itemName);
	Dialog_OK_Cancel.SetOK("确 定", "doDestroyItem(" @ %index @ ");");
	Dialog_OK_Cancel.SetCancel("取 消", "cancelDestroyItem();");
}
function doDestroyItem(%index)
{
	dropItem(%index);	
	DestroyItemGui.setVisible(0);
	$IsDirty--;
}

function cancelDestroyItem()
{
	cancelAction();
	DestroyItemGui.setvisible(0);
	$IsDirty--;
}

//鉴定
function RefreshPlayerIdentifyMoney(%gold,%silver,%copper)
{
 	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		ItemIdentifyHavaMoneyBmp_Gold.setVisible(0);
		ItemIdentifyHavaMoneyBmp_Silver.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		ItemIdentifyHavaMoneyBmp_Gold.setVisible(0);
		ItemIdentifyHavaMoneyBmp_Silver.setVisible(1);
	}
	else
	{
	  ItemIdentifyHavaMoneyBmp_Gold.setVisible(1);
		ItemIdentifyHavaMoneyBmp_Silver.setVisible(1);
  }
  ItemIdentifyHavaMoney_Gold.settext(%gold);
	ItemIdentifyHavaMoney_Silver.settext(%silver);
	ItemIdentifyHavaMoney_Copper.settext(%copper);
}

function RefreshPlayerIdentifyBindMoney(%gold,%silver,%copper)
{
 	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		ItemIdentify_BindGoldBmp.setVisible(0);
	  ItemIdentify_BindSilverBmp.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		ItemIdentify_BindGoldBmp.setVisible(0);
	  ItemIdentify_BindSilverBmp.setVisible(1);
	}
	else
	{
		ItemIdentify_BindGoldBmp.setVisible(1);
	  ItemIdentify_BindSilverBmp.setVisible(1);
  }
  ItemIdentify_BindGold.settext(%gold);
  ItemIdentify_BindSilver.settext(%silver);
  ItemIdentify_BindCopper.settext(%copper);
}

function onDoIdentify()
{
	DoIdentify();
}

function onCancelIdentify()
{
	ItemAppraiseWndGui.setVisible(0);
	$IsDirty--;
	closeIdentify();
}

function openIdentifyWnd()
{
	if(!ItemAppraiseWndGui.isvisible())
	{
		ItemAppraiseWndGui.setvisible(1);
		ForceOpenBag();
		$IsDirty++;
		ItemIdentifyBtn.setActive(0);
		
		ItemIdentifyInstrouce_RichText.setcontent("资质鉴定将花费一定的金钱，鉴定之后的装备会根据鉴定的不同品质附加相应的额外属性");
		ItemIdentifyInstrouce_RichText2.setcontent("右键点击包裹内装备，或直接将装备拖进鉴定栏。");
		
		ItemIdentifyNeedMoney_Gold.settext("");
	  ItemIdentifyNeedMoney_Silver.settext("");
	  ItemIdentifyNeedMoney_Copper.settext("0");
	  
	  ItemIdentifyNeedMoneyBmp_Gold.setVisible(0);
	  ItemIdentifyNeedMoneyBmp_Silver.setVisible(0);
	  
	}
}

function closeIdentifyWnd()
{
	if(ItemAppraiseWndGui.isvisible())
	{
		ItemAppraiseWndGui.setvisible(0);
		$IsDirty--;
	}
}

function activeOnIdentify(%state, %money)
{	
	ItemIdentifyBtn.setActive(%state);
	
	if(%money >= 10000)
	{
		%gold = mfloor(%money / 10000);
	  %silver	=mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  ItemIdentifyNeedMoney_Gold.settext(%gold);
	  ItemIdentifyNeedMoney_Silver.settext(%silver);
	  ItemIdentifyNeedMoney_Copper.settext(%copper);
	  
	  ItemIdentifyNeedMoneyBmp_Gold.setVisible(1);
	  ItemIdentifyNeedMoneyBmp_Silver.setVisible(1);
	}
	else if(%money < 100)
	{
		%copper = %money;
		ItemIdentifyNeedMoney_Gold.settext("");
	  ItemIdentifyNeedMoney_Silver.settext("");
	  ItemIdentifyNeedMoney_Copper.settext(%copper);
	  
	  ItemIdentifyNeedMoneyBmp_Gold.setVisible(0);
	  ItemIdentifyNeedMoneyBmp_Silver.setVisible(0);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
		ItemIdentifyNeedMoney_Gold.settext("");
	  ItemIdentifyNeedMoney_Silver.settext(%silver);
	  ItemIdentifyNeedMoney_Copper.settext(%copper);
	  
	  ItemIdentifyNeedMoneyBmp_Gold.setVisible(0);
	  ItemIdentifyNeedMoneyBmp_Silver.setVisible(1);
	}
}

//修复界面
function openRepair(%type)
{
	if(%type == 1)//经久修理
		openDurabilityWnd();
	else
		openItemRepairWnd();
}

function closeRepairWnd()
{
	if(DurabilityStrengthenWndGui.isVisible())
		closeDurabilityWnd();
	else
		closeItemRepairWnd();
}
function openDurabilityWnd()
{
	GameMainWndGui_Layer3.PushToBack(DurabilityStrengthenWndGui);
	if(!DurabilityStrengthenWndGui.isvisible())
	{
		DurabilityStrengthenWndGui.setVisible(1);
		ForceOpenBag();
		DurabilityStrengthen_RichText.setContent("经久强化后，装备普通修理将100%成功，装备的耐久上限不下降.");
		DurabilityStrengthen_RichText2.setContent("右键点击包裹内装备，或直接将装备拖进修复栏.");
		$IsDirty++;
	}
}

function closeDurabilityWnd()
{
	if(DurabilityStrengthenWndGui.isvisible())
	{
		DurabilityStrengthenWndGui.setVisible(0);
		$IsDirty--;
	}
}

function openItemRepairWnd()
{
	GameMainWndGui_Layer3.PushToBack(ItemRepairWndGui);
	if(!ItemRepairWndGui.isVisible())
	{
		ItemRepairWndGui.setvisible(1);
		ForceOpenBag();
		ItemRepair_RichText.setContent("修复后，装备的当前耐久将恢复到最大值，但进行装备普通修理时仍然有可能会失败.");
		ItemRepair_RichText2.setContent("右键点击包裹内装备，或直接将装备拖进修复栏.");
		$IsDirty++;
	}
}

function closeItemRepairWnd()
{
	if(ItemRepairWndGui.isVisible())
	{
		ItemRepairWndGui.setvisible(0);
		$IsDirty--;
	}
}

function RefreshPlayerDurabilityMoney(%gold,%silver,%copper)
{
	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		DurabilityStrengthenHavaMoneyBmp_Gold.setVisible(0);
		DurabilityStrengthenHavaMoneyBmp_Silver.setVisible(0);
  }
  else if(%gold == 0 && %silver != 0)
  {
  	%gold = "";
  	DurabilityStrengthenHavaMoneyBmp_Gold.setVisible(0);
	  DurabilityStrengthenHavaMoneyBmp_Silver.setVisible(1);
  }
  else
  {
  	DurabilityStrengthenHavaMoneyBmp_Gold.setVisible(1);
	  DurabilityStrengthenHavaMoneyBmp_Silver.setVisible(1);
  }
	DurabilityStrengthenHavaMoney_Gold.settext(%gold);
	DurabilityStrengthenHavaMoney_Silver.settext(%silver);
	DurabilityStrengthenHavaMoney_Copper.settext(%copper);
}


function RefreshPlayerDurabilityBindMoney(%gold,%silver,%copper)
{
	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
	  DurabilityStrengthen_BindGoldBmp.setVisible(0);
	  DurabilityStrengthen_BindSilverBmp.setVisible(0);
  }
  else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		DurabilityStrengthen_BindGoldBmp.setVisible(0);
	  DurabilityStrengthen_BindSilverBmp.setVisible(1);
  }
  else
  {
  	DurabilityStrengthen_BindGoldBmp.setVisible(1);
	  DurabilityStrengthen_BindSilverBmp.setVisible(1);
  }
 	DurabilityStrengthen_BindGold.settext(%gold);
	DurabilityStrengthen_BindSilver.settext(%silver);
	DurabilityStrengthen_Bindcopper.settext(%copper);
}

function RefreshPlayerItemRepairMoney(%gold,%silver,%copper)
{
	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		ItemRepairHavaMoneyBmp_Gold.setVisible(0);
		ItemRepairHavaMoneyBmp_Silver.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		ItemRepairHavaMoneyBmp_Gold.setVisible(0);
		ItemRepairHavaMoneyBmp_Silver.setVisible(1);
	}
	else
	{
		ItemRepairHavaMoneyBmp_Gold.setVisible(1);
		ItemRepairHavaMoneyBmp_Silver.setVisible(1);
	}
	ItemRepairHavaMoney_Gold.settext(%gold);
	ItemRepairHavaMoney_Silver.settext(%silver);
	ItemRepairHavaMoney_Copper.settext(%copper);
}

function RefreshPlayerItemRepairBindMoney(%gold,%silver,%copper)
{
	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		ItemRepair_BindGoldBmp.setVisible(0);
		ItemRepair_BindSilverBmp.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		ItemRepair_BindGoldBmp.setVisible(0);
		ItemRepair_BindSilverBmp.setVisible(1);
	}
	else
	{
		ItemRepair_BindGoldBmp.setVisible(1);
		ItemRepair_BindSilverBmp.setVisible(1);
	}
	ItemRepair_BindGold.settext(%gold);
	ItemRepair_BindSilver.settext(%silver);
	ItemRepair_BindCopper.settext(%copper);
}

function showRepairNum(%text)
{
	echo(%text);
	showRepairNumText.setcontent(%text);
}

function showPlayerDurabilityNeedMoney(%gold,%silver,%copper)
{
	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		DurabilityStrengthenNeedMoneyBmp_Gold.setVisible(0);
		DurabilityStrengthenNeedMoneyBmp_Silver.setVisible(0);
  }
  else if(%gold == 0 && %silver != 0)
  {
  	%gold = "";
  	DurabilityStrengthenNeedMoneyBmp_Gold.setVisible(0);
	  DurabilityStrengthenNeedMoneyBmp_Silver.setVisible(1);
  }
  else
  {
  	DurabilityStrengthenNeedMoneyBmp_Gold.setVisible(1);
	  DurabilityStrengthenNeedMoneyBmp_Silver.setVisible(1);
  }
	DurabilityStrengthenNeedMoney_Gold.settext(%gold);
	DurabilityStrengthenNeedMoney_Silver.settext(%silver);
	DurabilityStrengthenNeedMoney_Copper.settext(%copper);
}

function showPlayerItemRepairNeedMoney(%gold,%silver,%copper)
{
	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		ItemRepairNeedMoneyBmp_Gold.setVisible(0);
		ItemRepairNeedMoneyBmp_Silver.setVisible(0);
  }
  else if(%gold == 0 && %silver != 0)
  {
  	%gold = "";
  	ItemRepairNeedMoneyBmp_Gold.setVisible(0);
	  ItemRepairNeedMoneyBmp_Silver.setVisible(1);
  }
  else
  {
  	ItemRepairNeedMoneyBmp_Gold.setVisible(1);
	  ItemRepairNeedMoneyBmp_Silver.setVisible(1);
  }
	ItemRepairNeedMoney_Gold.settext(%gold);
	ItemRepairNeedMoney_Silver.settext(%silver);
	ItemRepairNeedMoney_Copper.settext(%copper);
}

function showRepairNeedMoney(%money)
{
	 %gold = mfloor(%money / 10000);
	 %silver	=mfloor((%money - (%gold * 10000)) / 100);
	 %copper	= %money % 100;
	 if(ItemRepairWndGui.isVisible())
	 	 showPlayerItemRepairNeedMoney(%gold, %silver, %copper);
	 else
	 	 showPlayerDurabilityNeedMoney(%gold, %silver, %copper);
}