//===========================================================================================================
// 文档说明:包裹界面操作
// 创建时间:2009-6-2
// 创建人: soar
// 修改人： hanfei
// 修改日期：2009-12-23
//===========================================================================================================

//=============================================================================
//包裹栏的合包操作
// 

//打开或关闭合包包裹栏
function OpenOrCloseAllBag()
{
	if(AllBagWndGui.IsVisible())
		CloseAllBag();
	else
		OpenAllBag();
}

//打开合包包裹栏
function OpenAllBag()
{
	GameMainWndGui_Layer3.PushToBack(AllBagWndGui);
	AllBagWndGui.setVisible(1);
	initIventory();
	FilterItem(-1, 0);
	$IsDirty++;
}

//关闭合包包裹栏
function CloseAllBag()
{
	AllBagWndGui.setVisible(0);
	$IsDirty--;
}

//关闭分包包裹栏显示，打开合包包裹栏显示
function ShowAllBag()
{	
	CloseSingleBag();
	OpenAllBag();
	if(AllBag_HotItemWnd.isvisible() && SingleBag_HotItemWnd.isvisible() != 1)
	{
		AllBagWndGui.setposition(getword(SingleBagWndGui.getposition(),0) - 70,getword(SingleBagWndGui.getposition(),1));
	}
	else if(AllBag_HotItemWnd.isvisible() != 1 && SingleBag_HotItemWnd.isvisible())
	{
		AllBagWndGui.setposition(getword(SingleBagWndGui.getposition(),0) + 70,getword(SingleBagWndGui.getposition(),1));
	}
	else
	{
		AllBagWndGui.setposition(getword(SingleBagWndGui.getposition(),0),getword(SingleBagWndGui.getposition(),1));
	}
	$BagState = 1;
	$ChoiceBagIndex = -1;
	FilterItem(-1,0);
}

//创建合包包裹栏的背景UI对象
function CreateAllBagSlotBack()
{
	 new GuiBitmapCtrl(AllBag_CommonPageImage) {
                  canSaveDynamicFields = "0";
                  Enabled = "1";
                  isContainer = "0";
                  Profile = "GuiDefaultProfile";
                  HorizSizing = "right";
                  VertSizing = "bottom";
                  position = "0 0";
                  Extent = "297 556";
                  MinExtent = "8 2";
                  canSave = "1";
                  Visible = "1";
                  mustPrerender = "0";
                  hovertime = "1000";
                  bitmap = "gameres/gui/images/GUIWindow31_1_002.png";
                  wrap = "0";
   };
}

//合包包裹栏加锁
function AllBag_LockCom()
{
	//AllBag_LockButton.setVisible(0);
	//AllBag_UnLockButton.setVisible(1);
}

//合包包裹栏解锁
function AllBag_UnLockCom()
{
	//AllBag_LockButton.setVisible(1);
	//AllBag_UnLockButton.setVisible(0);
}

//打开或关闭合包包裹栏热销物品窗口
function OpenAllBag_HotWnd()
{
	if(AllBag_HotItemWnd.IsVisible())
	{
		AllBag_HotItemWnd.setVisible(0);		
		AllBagWndGui.setposition(getword(AllBagWndGui.getposition(),0) +70 ,getword(AllBagWndGui.getposition(),1));
		AllBagWndGui.setextent(getword(AllBagWndGui.getextent(),0) - 70,getword(AllBagWndGui.getextent(),1));
		//AllBagWndGui_MainWnd.setposition(getword(AllBagWndGui_MainWnd.getposition(),0) -63,getword(AllBagWndGui_MainWnd.getposition(),1));
		AllBagWndGui_MainWnd.setposition(0,0);
	}
	else
	{
		AllBag_HotItemWnd.setVisible(1);		
		AllBagWndGui.setposition(getword(AllBagWndGui.getposition(),0) - 70,getword(AllBagWndGui.getposition(),1));
		AllBagWndGui.setextent(getword(AllBagWndGui.getextent(),0) + 70,getword(AllBagWndGui.getextent(),1));
		//AllBagWndGui_MainWnd.setposition(getword(AllBagWndGui_MainWnd.getposition(),0) + 63,getword(AllBagWndGui_MainWnd.getposition(),1));
		AllBagWndGui_MainWnd.setposition(70,0);
	}
}

//显示热卖物品窗口
function OpenAllBagHotItem()
{
	HotItemWnd_ShowPage.setPosition(1,28);
	HotItemWnd_DiscountItemButton.setPosition(1,358);
}

//显示折扣物口窗口
function OpenAllBagDiscountItem()
{
	HotItemWnd_ShowPage.setPosition(1,52);
	HotItemWnd_DiscountItemButton.setPosition(1,22);
}

//
function OpenAllBagHomeItem()
{
}

//=============================================================================
//包裹栏的分包操作
//

//打开或关闭分包包裹栏
function OpenOrCloseSingleBag()
{
	if(SingleBagWndGui.IsVisible())
		CloseSingleBag();
	else
		OpenSingleBag();
}

//打开分包包裹栏
function OpenSingleBag()
{
	GameMainWndGui_Layer3.PushToBack(SingleBagWndGui);
	SingleBagWndGui.setVisible(1);
	ChooseTabItem(0);
	initIventory();
	$IsDirty++;
}

//关闭分包包裹栏
function CloseSingleBag()
{
	SingleBagWndGui.setVisible(0);
	$IsDirty--;
}

$ChoiceBagIndex = 0;	//当前选择的包裹栏（目前有0,1,2,-1四种)
//切换包裹过滤类型栏位
function ChooseTabItem(%tabindex)
{
	FilterItem($ChoiceBagIndex, %tabindex);
	echo("ChoiceBagIndex = " @ $ChoiceBagIndex @ "tabindex = " @%tabindex);
	if(%tabindex > 0)
	{
		if(SingleBagWndGui.IsVisible())
		{
			SingleBag_NeatenButton.setActive(0);
			SingleBag_SplitItemButton.setActive(0);
		}
		if(AllBagWndGui.IsVisible())
		{
			AllBag_NeatenButton.setActive(0);
			AllBag_SplitItemButton.setActive(0);
		}
	}
	else
	{
		if(SingleBagWndGui.IsVisible())
		{
			SingleBag_NeatenButton.setActive(1);
			SingleBag_SplitItemButton.setActive(1);
		}
		if(AllBagWndGui.IsVisible())
		{
			AllBag_NeatenButton.setActive(1);
			AllBag_SplitItemButton.setActive(1);
		}
	}
}

//关闭合包，打开分包包裹栏
function ShowSingleBag()
{
	CloseAllBag();
	OpenSingleBag();
	if(AllBag_HotItemWnd.isvisible() && !SingleBag_HotItemWnd.isvisible())
	{
		SingleBagWndGui.setposition(getword(AllBagWndGui.getposition(),0) + 63,getword(AllBagWndGui.getposition(),1));
	}
	else if(AllBag_HotItemWnd.isvisible() != 1 && SingleBag_HotItemWnd.isvisible())
	{
		SingleBagWndGui.setposition(getword(AllBagWndGui.getposition(),0) - 63,getword(AllBagWndGui.getposition(),1));
	}
	else
	{
		SingleBagWndGui.setposition(getword(AllBagWndGui.getposition(),0),getword(AllBagWndGui.getposition(),1));
	}
	$ChoiceBagIndex = 0;
	FilterItem(0,0);
	$BagState = 0;
}

//分包包裹栏加锁
function SingleBag_LockCom()
{
	//SingleBag_LockButton.setVisible(0);
	//SingleBag_UnLockButton.setVisible(1);
}

//分包包裹栏解锁
function SingleBag_UnLockCom()
{
	//SingleBag_LockButton.setVisible(1);
	//SingleBag_UnLockButton.setVisible(0);
}

//打开分包包裹栏的普通包裹
function OpenSingleBag_CommonPage()
{
	SingleBag_CommonPage.setVisible(1);
	SingleBag_AddBagPage_1.setVisible(0);
	SingleBag_AddBagPage_2.setVisible(0);
	SingleBag_AddBagButton_1.setposition(5,258);
	SingleBag_AddBagButton_2.setposition(5,279);
	SingleBag_AllItemButton.setposition(143,45);
	SingleBag_PropButton.setposition(182,45);
	SingleBag_MissionButton.setposition(221,45);
	SingleBag_MaterialButton.setposition(260,45);
	$ChoiceBagIndex = 0;
	ChooseTabItem(0);
	SingleBag_AllItemButton.setStateOn(1);
}

//打开分包包裹栏的第一个扩展包裹
function OpenSingleBag_AddBagPage_1()
{
	SingleBag_CommonPage.setVisible(0);
	SingleBag_AddBagPage_1.setVisible(1);
	SingleBag_AddBagPage_2.setVisible(0);
	SingleBag_AddBagButton_1.setposition(5,66);
	SingleBag_AddBagButton_2.setposition(5,279);
	SingleBag_AllItemButton.setposition(143,66);
	SingleBag_PropButton.setposition(182,66);
	SingleBag_MissionButton.setposition(221,66);
	SingleBag_MaterialButton.setposition(260,66);
	$ChoiceBagIndex = 1;
	ChooseTabItem(0);
	SingleBag_AllItemButton.setStateOn(1);
}

//打开分包包裹栏的第二个扩展包裹
function OpenSingleBag_AddBagPage_2()
{
	SingleBag_CommonPage.setVisible(0);
	SingleBag_AddBagPage_1.setVisible(0);
	SingleBag_AddBagPage_2.setVisible(1);
	SingleBag_AddBagButton_1.setposition(5,66);
	SingleBag_AddBagButton_2.setposition(5,88);
	SingleBag_AllItemButton.setposition(143,88);
	SingleBag_PropButton.setposition(182,88);
	SingleBag_MissionButton.setposition(221,88);
	SingleBag_MaterialButton.setposition(260,88);
	$ChoiceBagIndex = 2;
	ChooseTabItem(0);
	SingleBag_AllItemButton.setStateOn(1);
}

//打开或关闭分包包裹栏的热销物品窗口
function OpenSingleBag_HotItemWnd()
{
	if(SingleBag_HotItemWnd.IsVisible())
	{
		SingleBag_HotItemWnd.setVisible(0);
		SingleBagWndGui.setposition(getword(SingleBagWndGui.getposition(),0) + 63,getword(SingleBagWndGui.getposition(),1));
		SingleBagWndGui.setextent(getword(SingleBagWndGui.getextent(),0) - 63,getword(SingleBagWndGui.getextent(),1));
		SingleBagWndGui_MainWnd.setposition(getword(SingleBagWndGui_MainWnd.getposition(),0) - 63,getword(SingleBagWndGui_MainWnd.getposition(),1));
	}
	else
	{
		SingleBag_HotItemWnd.setVisible(1);
		SingleBagWndGui.setposition(getword(SingleBagWndGui.getposition(),0) - 63,getword(SingleBagWndGui.getposition(),1));
		SingleBagWndGui.setextent(getword(SingleBagWndGui.getextent(),0) + 63,getword(SingleBagWndGui.getextent(),1));
		SingleBagWndGui_MainWnd.setposition(getword(SingleBagWndGui_MainWnd.getposition(),0) + 63,getword(SingleBagWndGui_MainWnd.getposition(),1));
	}
}

//显示分包包裹栏的热销物品窗口
function OpenSingleBagHotItem()
{
	HotItemWnd_Single_ShowPage.setPosition(2,26);
	HotItemWnd_Single_DiscountItemButton.setPosition(1,365);
}

//显示分包包裹栏的折扣物品窗口
function OpenSingleBagDiscountItem()
{
	HotItemWnd_Single_ShowPage.setPosition(2,50);
	HotItemWnd_Single_DiscountItemButton.setPosition(1,22);
}

function OpenSingleBagHomeItem()
{	
}

// %BagIndex  背包索引编号
// %type 0-全部 1-道具 2-任务 3-材料
function FilterItem(%BagIndex, %type)
{
	%ShortX = %ShortY = 0;
	%start = 0;
	%end = GetBagSize(0);	
	%posX = %posY = 0;
	%ChangeNumX = 4;
	%ChangeNumY = 5;
	%AllNum = 0;
	//清除包裹栏所有UI对象
	clearBagSlot();	

	if(%BagIndex == 1)			 // 包裹1
	{
		%start = GetBagSize(0);
		%end = GetBagSize(%BagIndex) + %start;
		CreateSlotBackGround();	
		SingleBag_AddBagPage_1.Add("Bag_SlotBackground");	
	}
	else if(%BagIndex == 2)	 // 包裹2
	{
		%start = GetBagSize(0) + GetBagSize(1);
		%end = GetBagSize(2) + %start;
		CreateSlotBackGround();	
		SingleBag_AddBagPage_2.Add("Bag_SlotBackground");	
	}
	else if(%BagIndex == -1) // 合包后所有
	{
		%end = GetBagSize(0) + GetBagSize(1) + GetBagSize(2); 
		CreateAllBagSlotBack();  
		AllBag_CommonPage.add(AllBag_CommonPageImage);
		%ChangeNumX = 0;
		%ChangeNumY = 0;
	}
	else
	{
		CreateSlotBackGround();	
		SingleBag_CommonPage.Add("Bag_SlotBackground");	
	}
	
	for(%i = %start; %i < %end; %i++)
	{		   		
		if(isBagType(%i, %type))
			{
				%ShortX = %posX * 37 + %ChangeNumX;
    		%ShortY = %posY * 37 + %ChangeNumY; 
				new GuiBitmapCtrl("Bag_PageBitmap_" @ %i) {
      		Enabled = "1";
      		Profile = "GuiDefaultProfile";
      		position = %ShortX SPC %ShortY;
      		Extent = "36 36";
      		MinExtent = "8 2";
      		hovertime = "1000";
      		bitmap = "gameres/gui/images/GUIWindow31_1_006.png";
      		wrap = "0";
   			};
				new dGuiShortCut("BagShortCut_Common_" @ %i) {
       			Enabled = "1";
      			position = (%ShortX +2) SPC (%ShortY+ 2);
      			Extent = "36 36";
      			MinExtent = "8 2";
      			hovertime = "1000";
      			slotType = "2";
      			slotCol = %i;
      			boudary = "0 0 32 32";
    			};
    		if(%BagIndex == 0)
    		{
    			SingleBag_CommonPage.Add("Bag_PageBitmap_" @ %i);
    			SingleBag_CommonPage.Add("BagShortCut_Common_" @ %i);
    		}
    		else if(%BagIndex == 1)
    		{
    			SingleBag_AddBagPage_1.Add("Bag_PageBitmap_" @ %i);
    			SingleBag_AddBagPage_1.Add("BagShortCut_Common_" @ %i);
    		}
    		else if(%BagIndex == 2)
    		{
    			SingleBag_AddBagPage_2.Add("Bag_PageBitmap_" @ %i);
    			SingleBag_AddBagPage_2.Add("BagShortCut_Common_" @ %i);
    		}
    		else if(%BagIndex == -1)
    		{
    			AllBag_CommonPage.Add("Bag_PageBitmap_" @ %i);
    			AllBag_CommonPage.Add("BagShortCut_Common_" @ %i);    			
    		}
    		
    		%posX ++;
    		%AllNum ++;
				if(%posX > 7)
				{
					%posY ++;
					%posX = 0;
				}
			}							
	}
	
	if(%BagIndex == -1)
	{
		if(%AllNum == 0)
			AllBag_CommonPage.setvisible(0);
		else
		{
			AllBag_CommonPage.setvisible(1);
			AllBag_CommonPage.setextent(getword(AllBag_CommonPage.getextent(),0),(%posY + 1) * 37);
		}
	}
}

//创建分包包裹栏的背景UI对象
function CreateSlotBackGround()
{
	new GuiBitmapCtrl(Bag_SlotBackground) {
    canSaveDynamicFields = "0";
    Enabled = "1";
    isContainer = "0";
    Profile = "GuiDefaultProfile";
    HorizSizing = "right";
    VertSizing = "bottom";
    position = "0 0";
    Extent = "303 192";
    MinExtent = "8 2";
    canSave = "1";
    Visible = "1";
    hovertime = "1000";
    bitmap = "gameres/gui/images/GUIWindow31_1_012.png";
    wrap = "0";
   };
}

//清空包裹栏所有UI对象
function clearBagSlot()
{
	for(%i = 0; %i < 120; %i++)
	{
		%obj = "Bag_PageBitmap_" @ %i;
		if(isObject(%obj))
			%obj.delete();
		%obj = "BagShortCut_Common_" @ %i;
		if(isObject(%obj))
			%obj.delete();
	}
	
	%obj = "Bag_SlotBackground";
	if(isObject(%obj))
		%obj.delete();
		
	%obj = "AllBag_CommonPageImage";
	if(isObject(%obj))
		%obj.delete();		
	
	AllBag_CommonPage.clear();
	SingleBag_CommonPage.clear();
	SingleBag_AddBagPage_1.clear();
	SingleBag_AddBagPage_2.clear();
}

//初始相关参数
function initIventory()
{
	%bag0Num = GetBagSize(0);
	%bag1Num = GetBagSize(1);
	%bag2Num = GetBagSize(2);
	SingleBag_CommonButton.setText("     乾坤袋(40格)");
	if(%bag1Num > 0)
	{
		SingleBag_AddBagButton_1.setActive(1);
		SingleBag_AddBagButton_1.setText("     乾坤袋("@ %bag1Num @"格)");
	}
	else
	{
		SingleBag_AddBagButton_1.setActive(0);
		SingleBag_AddBagButton_1.setText("     乾坤袋");
	}
	if(%bag2Num > 0)
	{
		SingleBag_AddBagButton_2.setActive(1);
		SingleBag_AddBagButton_2.setText("     乾坤袋("@ %bag2Num @"格)");
	}
	else
	{
		SingleBag_AddBagButton_2.setActive(0);
		SingleBag_AddBagButton_2.setText("     乾坤袋");
	}
	
	%player = getPlayer();
	if(%player == 0)
	  return;
	%level = %player.getLevel();
	//等级限制，启用或禁止相关按钮
	if(%level < 10)
		SingleBag_BusinessButton.setActive(0);
	else
		SingleBag_BusinessButton.setActive(1);	
	
	%allNum = %bag0Num + %bag1Num + %bag2Num;	
	if(%allNum > 40)
	{
		if(%level > 10)
			SingleBag_SplitBagButton.setActive(1);
		AllBag_CurrentlyItemNum.setText("乾坤袋 ("@ %allNum @"格)");
	}
	else
	{
		SingleBag_SplitBagButton.setActive(0);
		AllBag_CurrentlyItemNum.setText("乾坤袋 ");
	}
	
	if($BagState == 0)
	{
		//分包状态
		FilterItem(0,0);
		OpenSingleBag_CommonPage();
	}
	else
	{
		//合包状态
		FilterItem(-1,0);
	}
}

//刷新包裹栏的金元显示
function RefreshPlayerBagMoney(%gold,%silver,%copper)
{
 	if(%gold == "0" && %silver == "0")
	{
		%gold = "";
		%silver = "";
		AllBag_GoldImage.setvisible(0);
		SingleBag_GoldImage.setvisible(0);
		AllBag_SilverImage.setvisible(0);
		SingleBag_SilverImage.setvisible(0);
	}
	else if(%gold == "0" && %silver != "0")
	{
		%gold = "";
		AllBag_GoldImage.setvisible(0);
		SingleBag_GoldImage.setvisible(0);
		AllBag_SilverImage.setvisible(1);
		SingleBag_SilverImage.setvisible(1);
	}
	else
	{
	  AllBag_GoldImage.setvisible(1);
		SingleBag_GoldImage.setvisible(1);
		AllBag_SilverImage.setvisible(1);
		SingleBag_SilverImage.setvisible(1);
  }
  
  AllBag_Gold.settext(%gold);
	AllBag_Silver.settext(%silver);
	AllBag_Copper.settext(%copper);  
	
	SingleBag_Gold.settext(%gold);
	SingleBag_Silver.settext(%silver);
	SingleBag_Copper.settext(%copper);
}

//刷新包裹栏的灵元显示
function RefreshPlayerBagBindMoney(%gold,%silver,%copper)
{
 	if(%gold == "0" && %silver == "0")
	{
		%gold = "";
		%silver = "";		
		AllBag_GoldRockImage.setVisible(0);
		AllBag_SilverRockImage.setVisible(0);
		SingleBag_GoldRockImage.setVisible(0);
		SingleBag_SilverRockImage.setVisible(0);		
	}
	else if(%gold == "0" && %silver != "0")
	{
		%gold = "";
		AllBag_GoldRockImage.setVisible(0);
		AllBag_SilverRockImage.setVisible(1);
		SingleBag_GoldRockImage.setVisible(0);
		SingleBag_SilverRockImage.setVisible(1);
	}
	else
	{
	  AllBag_GoldRockImage.setVisible(1);
		AllBag_SilverRockImage.setVisible(1);
		SingleBag_GoldRockImage.setVisible(1);
		SingleBag_SilverRockImage.setVisible(1);
  }
  AllBag_GoldRock.settext(%gold);
  AllBag_SilverRock.settext(%silver);
  AllBag_CopperRock.settext(%copper);
  SingleBag_GoldRock.settext(%gold);
  SingleBag_SilverRock.settext(%silver);
  SingleBag_CopperRock.settext(%copper);
}

//=============================================================================
// 所有类型物品栏内物品的热感提示窗口
//显示热感物品提示窗口
function ShowItemHot(%x, %y, %height, %icon)
{
	GameMainWndGui_Layer5.pushtoback(ItemHot_Show);
	ItemHot_Show.setposition(%x, %y);
	ItemHot_Show.setExtent(220, %height+5);
	HotItem_Image.setBitmap(%icon);
	ItemHot_Show.setvisible(1);
}

//隐藏热感物品提示窗口
function HideItemHot()
{
	ItemHot_Show.setvisible(0);
}

//显示用于对比的热感物品提示窗口
function ShowCompareItemHot(%x, %y, %height, %icon)
{
		GameMainWndGui_Layer5.pushtoback(CompareItemHot_Show);
		CompareItemHot_Show.setposition(%x, %y);
		CompareItemHot_Show.setExtent(220, %height+24);
		CompareHotItem_Image.setBitmap(%icon);
		CompareItemHot_Show.setvisible(1);		
}

//隐藏用于对比的热感物品提示窗口
function HideCompareItemHot()
{
	CompareItemHot_Show.setvisible(0);
}
function updateItemHot(%index)
{
	if(ItemHot_Show.isVisible())
	{
		%obj = "BagShortCut_Common_" @%index;
		if(isObject(%obj))
			%obj.showHot();
	}
}

//function HideItemInfo()
//{
//		ItemHot_Show.setVisible(0);
//		CompareItemHot_Show.setVisible(0);
//}
//=============================================================================
// 聊天窗口内物品链接的热感提示窗口

// 隐藏所有聊天物品链接热感提示窗口
function closeChatLink()
{
	HideChatLink_Item();
	HideCompareChatLink_Item();
	
	ChatItem_Close.setVisible( 0 );
}

//显示聊天物品链接热感提示窗口
function ShowChatLink_Item(%x, %y, %height, %icon)
{
	GameMainWndGui_Layer5.pushtoback(ChatItem_Show);
	ChatItem_Show.setposition(%x, %y);
	ChatItem_Show.setExtent(220, %height+5);
	ChatItem_Close.setPosition( %x + 218, %y + 2 );
	ChatLink_Item_Image.setBitmap(%icon);
	ChatItem_Show.setVisible( 1 );	
	ChatItem_Close.setVisible( 1 );
}

//隐藏聊天物品链接热感提示窗口
function HideChatLink_Item()
{
	ChatItem_Show.setvisible(0);	
}

//显示聊天物品链接用于对比的热感提示窗口
function ShowCompareChatLink_Item(%x, %y, %height, %icon)
{
	GameMainWndGui_Layer5.pushtoback(CompareChatItem_Show);
	CompareChatItem_Show.setposition(%x + 220, %y);
	ChatItem_Close.setPosition( %x + 438, %y + 2 );
	CompareChatItem_Show.setExtent(220, %height+5);
	CompareChatItem_Image.setBitmap(%icon);
	CompareChatItem_Show.setvisible(1);
}

//隐藏聊天物品链接用于对比的热感提示窗口
function HideCompareChatLink_Item()
{
	CompareChatItem_Show.setvisible(0);
}

//=============================================================================
// 其它

//显示装备绑定确认对话窗口
function ShowBindWnd(%index, %flag)
{
	OpenDialog_Small_OK_Cancel();
	Dialog_Small_OK_Cancel.SetContent("装备的物品将与你绑定");
	Dialog_Small_OK_Cancel.SetOK("确 定", "equipBindItem(" @%index @", " @%flag @");");
	Dialog_Small_OK_Cancel.SetCancel("取 消", "cancelAction();");
}

//显示拾取绑定确认对话窗口
function ShowPickUpBind(%index)
{
	OpenDialog_Small_OK_Cancel();
	Dialog_Small_OK_Cancel.SetContent("拾取的物品将与你绑定");
	Dialog_Small_OK_Cancel.SetOK("确 定", "pickupBindItem(%index );");
	Dialog_Small_OK_Cancel.SetCancel("取 消", "cancelAction();");
}

function ForceOpenBag()
{	
	if($BagState $= 0)
	{
		if(!SingleBagWndGui.IsVisible())			
			OpenSingleBag();
	}
	else
	{
		if(!AllBagWndGui.IsVisible())			
			OpenAllBag();
	}
}