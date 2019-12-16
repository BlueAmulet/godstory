//===========================================================================================================
// 文档说明:商店操作
// 创建时间:2009-6-1
// 创建人: soar
//=========================================================================================================== 
$AckBuyFlag = -1;
$ShopGoodsIndex = -1;

//设置商品基本信息

function ShowGoodInfor(%name,%type,%money,%stockNum,%num,%index)
{
	("ShopItemName_" @ %index).setVisible(1);
	("ShopItemName_" @ %index).settext(%name);
	if(%type ==1)//金元
	{
		("ShopItemComItemShortCut_" @ %index).setVisible(0);
		("ShopItemComItemShortCutText_" @ %index).setVisible(0);
		("ShopItemReputeBase_" @ %index).setVisible(0);
		("ShopItemPriceBase_" @ %index).setVisible(1);
		("ShopItemPrice_" @%index @ "_GoldBitMap").setBitmap("gameres/gui/images/GUIWindow31_1_024.png");
		("ShopItemPrice_" @%index @ "_SilverBitMap").setBitmap("gameres/gui/images/GUIWindow31_1_025.png");
		("ShopItemPrice_" @%index @ "_CopperBitMap").setBitmap("gameres/gui/images/GUIWindow31_1_026.png");
			if(%money >= 10000)
			{
				("ShopItemPrice_" @%index @ "_GoldBitMap").setVisible(1);
				("ShopItemPrice_" @%index @ "_SilverBitMap").setVisible(1);
				%gold = mfloor(%money / 10000);
			  %silver	= mfloor((%money - (%gold * 10000)) / 100);
			  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
			  ("ShopItemPrice_" @ %index @ "_Gold").settext(%gold);
			  ("ShopItemPrice_" @ %index @ "_silver").settext(%silver);
			  ("ShopItemPrice_" @ %index @ "_copper").settext(%copper);
			}
			else if(%money < 100)
			{
				%copper = %money;
				("ShopItemPrice_" @%index @ "_GoldBitMap").setVisible(0);
				("ShopItemPrice_" @%index @ "_SilverBitMap").setVisible(0);
				("ShopItemPrice_" @ %index @ "_Gold").settext("");
			  ("ShopItemPrice_" @ %index @ "_silver").settext("");
			  ("ShopItemPrice_" @ %index @ "_copper").settext(mfloor(%copper));
			}
			else
			{
				%silver = mfloor(%money / 100);
				%copper = (%money - (%silver * 100));
				("ShopItemPrice_" @%index @ "_GoldBitMap").setVisible(0);
				("ShopItemPrice_" @%index @ "_SilverBitMap").setVisible(1);
				("ShopItemPrice_" @ %index @ "_Gold").settext("");
			  ("ShopItemPrice_" @ %index @ "_silver").settext(%silver);
			  ("ShopItemPrice_" @ %index @ "_copper").settext(%copper);
			 
			}
			
	}
	else if(%type == 2)
	{
		("ShopItemComItemShortCut_" @ %index).setVisible(0);
		("ShopItemComItemShortCutText_" @ %index).setVisible(0);
		("ShopItemReputeBase_" @ %index).setVisible(0);
		("ShopItemPriceBase_" @ %index).setVisible(1);
		("ShopItemPrice_" @%index @ "_GoldBitMap").setBitmap("gameres/gui/images/GUIWindow31_1_020.png");
		("ShopItemPrice_" @%index @ "_SilverBitMap").setBitmap("gameres/gui/images/GUIWindow31_1_021.png");
		("ShopItemPrice_" @%index @ "_CopperBitMap").setBitmap("gameres/gui/images/GUIWindow31_1_022.png");
			if(%money >= 10000)
			{
				("ShopItemPrice_" @%index @ "_GoldBitMap").setVisible(1);
				("ShopItemPrice_" @%index @ "_SilverBitMap").setVisible(1);
				%gold = mfloor(%money / 10000);
			  %silver	= mfloor((%money - (%gold * 10000)) / 100);
			  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
			  ("ShopItemPrice_" @ %index @ "_Gold").settext(%gold);
			  ("ShopItemPrice_" @ %index @ "_silver").settext(%silver);
			  ("ShopItemPrice_" @ %index @ "_copper").settext(%copper);
			}
			else if(%money < 100)
			{
				%copper = %money;
				("ShopItemPrice_" @%index @ "_GoldBitMap").setVisible(0);
				("ShopItemPrice_" @%index @ "_SilverBitMap").setVisible(0);
				("ShopItemPrice_" @ %index @ "_Gold").settext("");
			  ("ShopItemPrice_" @ %index @ "_silver").settext("");
			  ("ShopItemPrice_" @ %index @ "_copper").settext(mfloor(%copper));
			}
			else
			{
				%silver = mfloor(%money / 100);
				%copper = (%money - (%silver * 100));
				("ShopItemPrice_" @%index @ "_GoldBitMap").setVisible(0);
				("ShopItemPrice_" @%index @ "_SilverBitMap").setVisible(1);
				("ShopItemPrice_" @ %index @ "_Gold").settext("");
			  ("ShopItemPrice_" @ %index @ "_silver").settext(%silver);
			  ("ShopItemPrice_" @ %index @ "_copper").settext(%copper);
			 
			}
	}
	else if(%type $= 5)//声望
	{
		ShowShopItem_Currency5(%index);
		("ShopItemPrestige_" @ %index).setText("声望" @ %money);
	}
	else if(%type > 100)//物品
	{
		//%name = getItemName(%type);
		echo(%money);
		("ShopItemComItemShortCutText_" @ %index).setVisible(1);
		("ShopItemComItemShortCutText_" @ %index).setText(%money);
		ShowShopItem_ExchangeItem(%index, 1, %type);
	}
	if(%stockNum == 0)
	{
		("ShopItemCurrentlyNum_" @ %index).setVisible(1);
		("ShopItemCurrentlyNum_" @ %index).settext("库存：" @ "无限");	
	}
	else
	{
		("ShopItemCurrentlyNum_" @ %index).setVisible(1);
		("ShopItemCurrentlyNum_" @ %index).settext("库存：" @ %num);
	}
	setCoolImage(%index, 0);
}

//Currency5:声望
function ShowShopItem_Currency5(%index)
{
	("ShopItemPriceBase_" @ %index).setVisible(0);
	("ShopItemComItemShortCut_" @ %index).setVisible(0);
	("ShopItemReputeBase_" @ %index).setVisible(1);
}

//ExchangeItem:物品
function ShowShopItem_ExchangeItem(%index,%type,%id)
{
	("ShopItemPriceBase_" @ %index).setVisible(0);
	("ShopItemComItemShortCut_" @ %index).setVisible(1);
	("ShopItemReputeBase_" @ %index).setVisible(0);
	("ShopItemComItemShortCut_" @ %index).setobject(%type,%id);
}
//=====================================================================

function ShowShopItemNum(%ItemNum, %TotalNum)
{
	$currentPage = %ItemNum;
	$totalPage = %TotalNum;
	if(%TotalNum == 0)
		ShopCurrentlyPageNum.setText(0 @ "/" @ 0);
	else
		ShopCurrentlyPageNum.setText(%ItemNum @ "/" @ %TotalNum);
}

function ShowNpcShopWnd()
{
	if(!ShopWndGui.IsVisible())
	{
		ShopWndGui.setvisible(1);
		ForceOpenBag();
		$IsDirty++;
		ShopWndGui_PopButton_1.add("全部",1);
		ShopWndGui_PopButton_1.add("可用",2);
		ShopWndGui_PopButton_1.setSelected(1);
	}
} 
//关闭窗口
function CloseNpcShopWnd()
{
	ShopWndGui.setVisible(false);
	ShopHelpWndGui.setVisible(false);
	$IsDirty--;
	if($ShopGoodsIndex != -1)
	  (Button_CoverImage @ $ShopGoodsIndex).setStateOn(false);
	  
	for(%i = 0; %i < 9; %i++)
	{
		clearNPCShopText(%i);
	}
	ShopWndGui_PopButton_1.clear();
	$AckBuyFlag = -1;
	$ShopGoodsIndex = -1;
	CloseShopWnd_BatchSellWnd();
	CloseShopWnd_NotarizeWnd();
}

function clearNPCShopText(%index)
{
	("ShopItemName_" @ %index).settext("");
	("ShopItemName_" @ %index).setVisible(0);
	("ShopItemPrice_" @ %index @ "_Gold").settext("");
	("ShopItemPrice_" @ %index @ "_silver").settext("");
	("ShopItemPrice_" @ %index @ "_copper").settext("");
	("ShopItemPriceBase_" @ %index).setVisible(0);
	("ShopItemCurrentlyNum_" @ %index).settext("");
	("ShopItemCurrentlyNum_" @ %index).setVisible(0);	
	("ShopItemComItemShortCut_" @ %index).setVisible(0);
	("ShopItemComItemShortCutText_" @ %index).setVisible(0);
}
//单个买
function buyGoodFromShop(%Index, %name, %type, %money)
{
	//$ShopGoodsIndex = %Index;
	echo("%Index----------"@%Index);
	if(%Index != -1)
	{
		if(ShopWndGui_CheckBoxButton_1.IsStateOn)
		{
			echo("YYYYYYYYYYYYYYYYYY");
			OpenShopWnd_NotarizeWnd();
			NotarizeWnd_OKButton.command = "BuyFromShop("@%Index@", 1);CloseShopWnd_NotarizeWnd();";
			NotarizeWnd_ItemName.setcontent(%name);
			
			setBuyItemInfor(1,%type, %money);
		}
		else
	 		BuyFromShop(%Index,1);
	}		
}

function OpenOrCloseShopWnd_NotarizeWnd()
{
	if(ShopWnd_NotarizeWnd.isvisible())
	{
		ShopWnd_NotarizeWnd.setVisible(0);
		$IsDirty--;
	}
	else
	{
		ShopWnd_NotarizeWnd.setVisible(1);
		$IsDirty++;
	}
}

function OpenShopWnd_NotarizeWnd()
{
	ShopWnd_NotarizeWnd.setVisible(1);
	$IsDirty++;
}

function CloseShopWnd_NotarizeWnd()
{
	if(ShopWnd_NotarizeWnd.isvisible())
	{
		ShopWnd_NotarizeWnd.setVisible(0);
		$IsDirty--;
		//$ShopGoodsIndex = -1;
	}
}

function OpenOrCloseShopWnd_BatchSellWnd()
{
	if(ShopWnd_BatchSellWnd.isvisible())
	{
		ShopWnd_BatchSellWnd.setVisible(0);
		$IsDirty--;
	}
	else
	{
		ShopWnd_BatchSellWnd.setVisible(1);
		$IsDirty++;
	}
}
//
function OpenShopWnd_BatchSellWnd()
{
	ShopWnd_BatchSellWnd.setVisible(1);
	BatchSellWnd_TextEdit.makeFirstResponder(1);
	BatchSellWnd_TextEdit.setText("");
	$IsDirty++;
}
//
function CloseShopWnd_BatchSellWnd()
{
	if(ShopWnd_BatchSellWnd.isvisible())
	{
		ShopWnd_BatchSellWnd.setVisible(0);
	  $IsDirty--;
	}
}
//批量买
function buyGoodsFromShop(%index)
{
	$ShopGoodsIndex = %index;
	OpenShopWnd_BatchSellWnd();
}
//确定购买
function onDoBuy()
{
	BatchBuyGoodsFromShop($ShopGoodsIndex);
}
//向右
function setRightPage()
{
	if($totalPage == 0)
		return;
	$currentPage++;
	if($currentpage > $totalPage)
		$currentpage = $totalPage;
	ShowNPCShop($currentPage-1);
}
//向左
function setLeftPage()
{
	if($totalPage == 0)
		return;
	$currentPage--;
	if($currentPage < 1)
	  $currentPage = 1;
	ShowNPCShop($currentPage-1); 
}
//选中哪一项商品
function  selectItemIndex(%index)
{
	$ShopGoodsIndex = %index;
	if($AckBuyFlag $= 0)
	{
		if(("ShopItemName_" @ %index).isvisible())
			//buyGoodFromShop($ShopGoodsIndex);
			BuyShopItem($ShopGoodsIndex);
		else
			return;
	}
	else if($AckBuyFlag $= 1)
	{
		if(("ShopItemName_" @ %index).isvisible())
			buyGoodsFromShop($ShopGoodsIndex);
		else
			return;
	}
}
//设置批量购买状态
function setBatchBuyFlag()
{
	if($AckBuyFlag $= 1)
		$AckBuyFlag = -1;
	else
		$AckBuyFlag = 1;
}
//设置单个购买状态
function setBuyOneFlag()
{
	if($AckBuyFlag $= 0)
		$AckBuyFlag = -1;
	else
		$AckBuyFlag = 0;
}

function clearBuyFlag()
{
	$AckBuyFlag = -1;
}
//设置购买信息
function setBuyItemInfor(%num,%type, %money)
{
	NotarizeWnd_ItemNum.setText(%num);
	if(%type == 1)
	{
		showItemPriceTpe(1);
		if(%money >= 10000)
		{
			NotarizeWnd_SinglePrice_GoldBitMap.setVisible(1);
			NotarizeWnd_SinglePrice_SillerBitMap.setVisible(1);
			%gold = mfloor(%money / 10000);
		  %silver	= mfloor((%money - (%gold * 10000)) / 100);
		  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
		  NotarizeWnd_SinglePrice_Gold.settext(%gold);
		  NotarizeWnd_SinglePrice_Siller.settext(%silver);
		  NotarizeWnd_SinglePrice_Copper.settext(%copper);
		}
		else if(%money < 100)
		{
			%copper = %money;
			NotarizeWnd_SinglePrice_GoldBitMap.setVisible(0);
			NotarizeWnd_SinglePrice_SillerBitMap.setVisible(0);
			NotarizeWnd_SinglePrice_Gold.settext("");
		  NotarizeWnd_SinglePrice_Siller.settext("");
		  NotarizeWnd_SinglePrice_Copper.settext(%copper);
		}
		else
		{
			%silver = mfloor(%money / 100);
			%copper = (%money - (%silver * 100));
		  NotarizeWnd_SinglePrice_GoldBitMap.setVisible(0);
			NotarizeWnd_SinglePrice_SillerBitMap.setVisible(1);
			NotarizeWnd_SinglePrice_Gold.settext("");
		  NotarizeWnd_SinglePrice_Siller.settext(%silver);
		  NotarizeWnd_SinglePrice_Copper.settext(%copper);
		}
		%allMoney = %num * %money;
		if(%allMoney >= 10000)
		{
			NotarizeWnd_AllPrice_GoldBitMap.setVisible(1);
			NotarizeWnd_AllPrice_SillerBitMap.setVisible(1);
			%gold = mfloor(%allMoney / 10000);
		  %silver	= mfloor((%allMoney - (%gold * 10000)) / 100);
		  %copper	= (%allMoney - (%gold * 10000 + (%silver * 100)));
		  NotarizeWnd_AllPrice_Gold.settext(%gold);
		  NotarizeWnd_AllPrice_Siller.settext(%silver);
		  NotarizeWnd_AllPrice_Copper.settext(%copper);
		}
		else if(%allMoney < 100)
		{
			%copper = %allMoney;
			NotarizeWnd_AllPrice_GoldBitMap.setVisible(0);
			NotarizeWnd_AllPrice_SillerBitMap.setVisible(0);
			NotarizeWnd_AllPrice_Gold.settext("");
		  NotarizeWnd_AllPrice_Siller.settext("");
		  NotarizeWnd_AllPrice_Copper.settext(%copper);
		}
		else
		{
			%silver = mfloor(%allMoney / 100);
			%copper = (%allMoney - (%silver * 100));
		  NotarizeWnd_AllPrice_GoldBitMap.setVisible(0);
			NotarizeWnd_AllPrice_SillerBitMap.setVisible(1);
			NotarizeWnd_AllPrice_Gold.settext("");
		  NotarizeWnd_AllPrice_Siller.settext(%silver);
		  NotarizeWnd_AllPrice_Copper.settext(%copper);
		}
	}
	else if(%type == 5)
	{
		showItemPriceTpe(0);
		NoItem_Single_PriceText.setText("声望 "@%money);
		NoItem_All_PriceText.setText("声望 "@%money*%num);
	}
	else if(%type > 100)
	{
		showItemPriceTpe(0);
		%name = getItemName(%type);
		NoItem_Single_PriceText.setText(%name@"×"@%money);
		NoItem_All_PriceText.setText(%name@"×"@%money*%num);
	}
}
//批量购买
function buyManyGoodsFromShop(%Index, %name, %type, %money)
{
	%num = BatchSellWnd_TextEdit.getText();
	if(ShopWndGui_CheckBoxButton_1.IsStateOn)
	{
		if(%num $= "")
			return;
		ShopWnd_NotarizeWnd.setVisible(1);
		NotarizeWnd_OKButton.command = "BuyFromShop("@%Index@", "@%num @");CloseShopWnd_NotarizeWnd();";
		NotarizeWnd_ItemName.setContent(%name);
		setBuyItemInfor(%num,%type,%money);
	}
	else
	 	BuyFromShop($ShopGoodsIndex,%num);
}
//增加
function addBuyItemNum()
{
	%num = BatchSellWnd_TextEdit.getText();
	%num++;
	if(%num > 255)
		%num = 255;
	BatchSellWnd_TextEdit.setText(%num);
}
//减少
function reduceBuyNum()
{
	%num = BatchSellWnd_TextEdit.getText();
	%num--;
	if(%num < 1)
		%num = 1;
	BatchSellWnd_TextEdit.setText(%num);
}
//设置一组
function setOneGroupOk()
{
	%num = setOneGroup($ShopGoodsIndex);
	BatchSellWnd_TextEdit.setText(%num);
}
//筛选
function refreshNPCShop()
{
	%id = ShopWndGui_PopButton_1.getSelected();
	
	FilterNPCShopItem(%id);
}

//
function setInventoryStats(%value)
{
	if(%value $= 0)
	{
		if(SingleBagWndGui.isVisible())
		{
			CloseSingleBag();
		}
		if(AllBagWndGui.isVisible())
			CloseAllBag();
	}
	else if(%value $= 1)
	{
		if(!SingleBagWndGui.isVisible())
		{
			OpenSingleBag();
		}
		if(AllBagWndGui.isVisible())
			CloseAllBag();
	}
}
//显示冻结
function setCoolImage(%index, %flag)
{
	if(%flag $= 0)
		("ShopItemCool_BitMap"@%index).setVisible(0);
	if(%flag $= 1)
		("ShopItemCool_BitMap"@%index).setVisible(1);
}

function Button_CoverImage0::onRightMouseUp(%this)
{
	BuyShopItem(0);
}
function Button_CoverImage1::onRightMouseUp(%this)
{
	BuyShopItem(1);
}
function Button_CoverImage2::onRightMouseUp(%this)
{
	BuyShopItem(2);
}
function Button_CoverImage3::onRightMouseUp(%this)
{
	BuyShopItem(3);
}
function Button_CoverImage4::onRightMouseUp(%this)
{
	BuyShopItem(4);
}
function Button_CoverImage5::onRightMouseUp(%this)
{
	BuyShopItem(5);
}
function Button_CoverImage6::onRightMouseUp(%this)
{
	BuyShopItem(6);
}
function Button_CoverImage7::onRightMouseUp(%this)
{
	BuyShopItem(7);
}
function Button_CoverImage8::onRightMouseUp(%this)
{
	BuyShopItem(8);
}
function Button_CoverImage9::onRightMouseUp(%this)
{
	BuyShopItem(9);
}
//显示钱还是物品或声望
function showItemPriceTpe(%type)
{
	if(%type == 1)
	{
		NotarizeWnd_SinglePrice_GoldBitMap.setvisible(1);
		NotarizeWnd_SinglePrice_SillerBitMap.setvisible(1);
		NotarizeWnd_SinglePrice_CopperBitMap.setvisible(1);
		NotarizeWnd_AllPrice_GoldBitMap.setvisible(1);
		NotarizeWnd_AllPrice_SillerBitMap.setvisible(1);
		NotarizeWnd_AllPrice_CopperBitMap.setvisible(1);
		NotarizeWnd_SinglePrice_Gold.setvisible(1);
		NotarizeWnd_SinglePrice_Siller.setvisible(1);
		NotarizeWnd_SinglePrice_Copper.setvisible(1);
		NotarizeWnd_AllPrice_Gold.setvisible(1);
		NotarizeWnd_AllPrice_Siller.setvisible(1);
		NotarizeWnd_AllPrice_Copper.setvisible(1);
		NoItem_Single_PriceText.setvisible(0);
		NoItem_All_PriceText.setvisible(0);
	}
	else
	{
		NotarizeWnd_SinglePrice_GoldBitMap.setvisible(0);
		NotarizeWnd_SinglePrice_SillerBitMap.setvisible(0);
		NotarizeWnd_SinglePrice_CopperBitMap.setvisible(0);
		NotarizeWnd_AllPrice_GoldBitMap.setvisible(0);
		NotarizeWnd_AllPrice_SillerBitMap.setvisible(0);
		NotarizeWnd_AllPrice_CopperBitMap.setvisible(0);
		NotarizeWnd_SinglePrice_Gold.setvisible(0);
		NotarizeWnd_SinglePrice_Siller.setvisible(0);
		NotarizeWnd_SinglePrice_Copper.setvisible(0);
		NotarizeWnd_AllPrice_Gold.setvisible(0);
		NotarizeWnd_AllPrice_Siller.setvisible(0);
		NotarizeWnd_AllPrice_Copper.setvisible(0);
		NoItem_Single_PriceText.setvisible(1);
		NoItem_All_PriceText.setvisible(1);
	}
}
