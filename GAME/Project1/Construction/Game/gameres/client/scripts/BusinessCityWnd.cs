//===========================================================================================================
// 文档说明:商城操作
// 创建时间:2010-1-14
// 创建人: Batcel
//===========================================================================================================


$FirstOpenBusinessCity =0;
$SuperMarketType = 1;
$LastSelectedIndex = -1;	//last selected
$CurrentShowItemPage = 0;	//current page
$AllShowItemPage = 0;			//all page
$MarketItemPrice = 0;			//购买价格				
$MarketItemMoney = 0;			//索要价格
$LastSelectedBasketIndex = -1;// 购物篮选中UIShortcut
$LastSelectedBasketBtnIndex = -1; // 购物篮选中button
function OpenBusinessCity()
{
	if(BusinessCityWndGui.IsVisible())
	{
		BusinessCityWndGui.setVisible(0);
		$IsDirty--;
	}
	else
	{
		if($FirstOpenBusinessCity ==0)
		{
			$FirstOpenBusinessCity =1;
			CreateBusinessCityItem(); 
			ClickBusinessCitySeries(0);
		}
		GameMainWndGui_Layer3.pushToBack(BusinessCityWndGui);
		BusinessCityWndGui.setVisible(1);
		$IsDirty++;
		ClickOnCancleSuperItem();
  }	
}

function CloseBussnessCity()
{
	if(BusinessCityWndGui.IsVisible())
	{
		BusinessCityWndGui.setVisible(0);
		$IsDirty--;
	}
	CloseShoppingBasketWnd();
	CloseShoppingBasketWnd();
	CloseBusinessCityBuyWnd();
	CloseAskForItemWnd();
	ClickOnCancleSuperItem();
	%obj = "BusinessCityItem_ShorCut_" @$LastSelectedIndex;
		if(isObject(%obj))
			%obj.setBeSelected(0);
	$LastSelectedIndex = -1;
}
//选中商品种类
function ClickBusinessCitySeries(%index)
{
	for(%i=0; %i<=%index; %i++)
	{
		("BusinessCity_SeriesBtn_" @%i).setPosition(6,63+(%i*18));
		("BusinessCitySeriesTab_" @%i).setVisible(0);
		if(%i == %index)
		  ("BusinessCitySeriesTab_" @%i).setVisible(1);
	}
	
	for(%i=9; %i>%index; %i--)
	{
		("BusinessCity_SeriesBtn_" @%i).setPosition(6,440-((9-%i)*18));
		("BusinessCitySeriesTab_" @%i).setVisible(0);
	}
	filterSuperMarket(%index+1, 1);
//	%obj = "isStateButton_" @ %index;
//	if(isObject(%obj))
//		%obj.setStateOn(1);
}
//创建商品显示控件
function CreateBusinessCityItem()
{
	for(%i=0; %i<4; %i++)
	{
     for(%j=0; %j<3; %j++)
     {
	    new GuiControl("BusinessCityItem_" @ (%i*3+%j)) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "1";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = (%j*161+101) SPC (%i*92+66);
         Extent = "160 90";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "0";
         mustPrerender = "0";
         hovertime = "1000";
				 
         new GuiRichTextCtrl("BusinessCityItem_Name_" @ (%i*3+%j)) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiCurrencyShowInfoTextProfile_1";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "7 4";
            Extent = "145 19";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
         };
         new dGuiShortCut("BusinessCityItem_ShorCut_" @ (%i*3+%j)) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "7 31";
            Extent = "48 48";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            slotType = "40";
            slotCol =(%i*3+%j);
            boudary = "0 0 48 48";
            disabledrag = "1";
            MouseDownCommand = "setSelectedItem(" @%i*3+%j @");";
         };
         new GuiTextCtrl("BusinessCityItem_Price_" @ (%i*3+%j)) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiTextProfile5";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "61 29";
            Extent = "69 18";
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
            text = "99999";
            maxLength = "1024";
         };
         new GuiTextCtrl("BusinessCityItem_Usefullife_" @ (%i*3+%j)) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiCurrencyShowInfoTextProfile_4";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "63 48";
            Extent = "90 18";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "0";
            mustPrerender = "0";
            hovertime = "1000";
            Margin = "0 0 0 0";
            Padding = "0 0 0 0";
            AnchorTop = "1";
            AnchorBottom = "0";
            AnchorLeft = "1";
            AnchorRight = "0";
            text = "有效期:  230天";
            maxLength = "1024";
         };
         new GuiButtonCtrl("BusinessCityItem_Compliment_" @ (%i*3+%j)) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiBusinessCityButtonProfile_1";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "62 67";
            Extent = "43 19";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            text = "赠取";
            groupNum = "-1";
            buttonType = "PushButton";
            useMouseEvents = "0";
            IsStateOn = "0";
            command = "showComplementMarketItemInfo(" @ (%i*3+%j) @ ");";
         };
         new GuiButtonCtrl("BusinessCityItem_Buy_" @ (%i*3+%j) ) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiBusinessCityButtonProfile_1";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "108 67";
            Extent = "43 19";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            text = "购买";
            groupNum = "-1";
            buttonType = "PushButton";
            useMouseEvents = "0";
            IsStateOn = "0";
            command = "showBuyMarketItemInfo(" @(%i*3+%j) @ ");";
         };
         new GuiBitmapCtrl("BusinessCityItem_CoinBmp_" @ (%i*3+%j)) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "130 29";
            Extent = "16 16";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            alpha = "255";
            useFadeout = "1";
            bitmap = "gameres/gui/images/GUIWindow31_1_027.png";
            wrap = "0";
         };
         new GuiBitmapCtrl("BusinessCityItem_CutPrice_" @ (%i*3+%j)) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "2 65";
            Extent = "29 23";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            alpha = "255";
            useFadeout = "1";
            bitmap = "gameres/gui/images/GUIWindow45_1_009.png";
            wrap = "0";
         };
      };
      
    BusinessCityWndGui.Add("BusinessCityItem_" @ (%i*3+%j));
   }
  }
}
//显示商品的相关信息
function showMarketItemInfo(%index, %name, %money, %data, %type)
{
	%obj = "BusinessCityItem_" @ %index;
	if(!isObject(%obj))
		return;
	%obj.setVisible(1);
	%obj = "BusinessCityItem_Name_" @ %index;
	if(isObject(%obj))
	  %obj.setContent(%name);
	
	%obj = "BusinessCityItem_Price_" @ %index;
	if(isObject(%obj))
		%obj.setText(%money);
	if(%data > 0)
	{
		%timeText = "";
		%obj = "BusinessCityItem_Usefullife_" @ %index;
		if(isObject(%obj))
		{
			%time = %data % 24;
			if(%time > 0)
				%timeText = "有效期:  "@ %data @ "小时";
			else 
				%timeText = "有效期:  "@ %data % 24 @ "天";
			%obj.setVisible(1);
			%obj.setText(%timeText);
		}
	}
	%obj = "BusinessCityItem_CoinBmp_" @ %index;
	if(isObject(%obj))
	{
		if(xianshi_btn.isStateOn())
			%obj.setBitmap("gameres/gui/images/GUIWindow31_1_027.png");
		else
			%obj.setBitmap("gameres/gui/images/GUIWindow31_1_028.png");
	}
	%obj = "BusinessCityItem_Compliment_" @ %index;
	if(isObject(%obj))
	{
		if(%type == 1)
			%obj.setVisible(1);
		else
			%obj.setVisible(0);
	}
}
//打开商城
function openSuperMarket(%current,%num)
{
	OpenBusinessCity();
	showMarketItemPage(%current,%num);
}

//选中
function setSelectedItem(%index)
{
	if($LastSelectedIndex != %index)
	{
		%obj = "BusinessCityItem_ShorCut_" @$LastSelectedIndex;
		if(isObject(%obj))
			%obj.setBeSelected(0);
		%obj = "BusinessCityItem_ShorCut_" @%index;
		if(isObject(%obj))
			%obj.setBeSelected(1);
		
		$LastSelectedIndex = %index;		
	}
	ClickOnSuperItem($LastSelectedIndex);
}
//选择仙石还是神石商城
function setSuperMarketType(%type)
{
	setMarketType(%type);
	ClickBusinessCitySeries(0);
}

//隐藏
function hideMarketItem(%index)
{
	%obj = "BusinessCityItem_" @%index;
	if(isObject(%obj))
		 %obj.setVisible(0);	
}

//显示页数
function showMarketItemPage(%current,%all)
{
	if(%all == 0)
		%all = 1;
	$CurrentShowItemPage = %current + 1;
	$AllShowItemPage = %all;
	%text = $CurrentShowItemPage @"/"@ $AllShowItemPage;
	BusinessCity_CurrentPage.setText(%text);
}
//点击首页
function ClickOnFirst()
{
	$CurrentShowItemPage = 0;
	showMarketItemPage(0,$AllShowItemPage);
	changeMarketItemPage(0);
}
//点击末页
function ClickOnEnd()
{
	showMarketItemPage($AllShowItemPage-1, $AllShowItemPage);
	changeMarketItemPage($AllShowItemPage-1);
}
function clickOnFrontPage()
{
	$CurrentShowItemPage--;
	if($CurrentShowItemPage < 1)
		$CurrentShowItemPage = 1;
	showMarketItemPage($CurrentShowItemPage-1, $AllShowItemPage);
	changeMarketItemPage($CurrentShowItemPage-1);
}
function clickOnNextPage()
{
	$CurrentShowItemPage++;
	if($CurrentShowItemPage > $AllShowItemPage)
		$CurrentShowItemPage = $AllShowItemPage;
	showMarketItemPage($CurrentShowItemPage-1, $AllShowItemPage);
	changeMarketItemPage($CurrentShowItemPage-1);
}
//打开购物篮
function OpenShoppingBasketWnd()
{
	if(!ShoppingBasketWndGui.isVisible())
	{
		GameMainWndGui_Layer3.pushToBack(ShoppingBasketWndGui);
		ShoppingBasketWndGui.setVisible(1);
		$IsDirty++;
	}
}
//关闭购物篮
function CloseShoppingBasketWnd()
{
	if(ShoppingBasketWndGui.isVisible())
	{
		ShoppingBasketWndGui.setVisible(0);
		$IsDirty--;
	}
}
//打开商城购买
function OpenBusinessCityBuyWnd()
{
	if(!BusinessCityBuyItemWndGui.isVisible())
	{
		GameMainWndGui_Layer4.pushToBack(BusinessCityBuyItemWndGui);
		BusinessCityBuyItemWndGui.setVisible(1);
		CloseComplimentAwayWnd();
		$IsDirty++;
	}
}
//关闭商城购买
function CloseBusinessCityBuyWnd()
{
	if(BusinessCityBuyItemWndGui.isVisible())
	{
		BusinessCityBuyItemWndGui.setVisible(0);
		$IsDirty--;
		$MarketItemPrice = 0;
	}
}
//打开商城赠取
function OpenComplimentAwayWnd()
{
	if(!ComplimentAwayWndGui.isVisible())
	{
		GameMainWndGui_Layer3.pushToBack(ComplimentAwayWndGui);
		ComplimentAwayWndGui.setVisible(1);
		$IsDirty++;
		showComplementWnd(1);
		CloseBusinessCityBuyWnd();
	}
}
//关闭商城赠取
function CloseComplimentAwayWnd()
{
	if(ComplimentAwayWndGui.isVisible())
	{
		ComplimentAwayWndGui.setVisible(0);
		$IsDirty--;
	}
}
//打开商城索要
function OpenAskForItemWnd()
{
	if(!AskForItemWndGui.isVisible())
	{
		GameMainWndGui_Layer3.pushToBack(AskForItemWndGui);
		AskForItemWndGui.setVisible(1);
		$IsDirty++;
	}
}
//关闭商城索要
function CloseAskForItemWnd()
{
	if(AskForItemWndGui.isVisible())
	{
		AskForItemWndGui.setVisible(0);
		$IsDirty--;
	}
}

//打开购买界面并显示信息
function ShowAndOpenMarketBuyWnd(%name,%money,%type)
{
	BuyItemInfo_Name.setContent(%name);
	BuyItemInfo_Price.setText(%money);
	BuyItemInfo_AllPrice.setText(%money);
	buyMarketItem_Num.setText(1);
	OpenBusinessCityBuyWnd();
	if(%type == 1)
	{
		BuyItemInfo_Bitmap1.setBitmap("gameres/gui/images/GUIWindow31_1_027.png"); 
		BuyItemInfo_Bitmap2.setBitmap("gameres/gui/images/GUIWindow31_1_027.png"); 
	}
	else if(%type == 2)
	{
		BuyItemInfo_Bitmap1.setBitmap("gameres/gui/images/GUIWindow31_1_028.png");
		BuyItemInfo_Bitmap2.setBitmap("gameres/gui/images/GUIWindow31_1_028.png");  
	}
	$MarketItemPrice = %money;
}
//打开赠取界面
function ShowAndOpenComplimentAwayWnd(%name, %money)
{
	ComplementItem_Name.setContent(%name);
	ComplementItem_Price.setText(%money);
	ComplementItem_AllPrice.setText(%money);
	ComplementItem_Num.setText(1);
	OpenComplimentAwayWnd();
	$MarketItemMoney = %money;
}
//打开索取界面
function showAskForWnd(%playerName, %ItemName, %money, %num)
{
	AskForFriend_Name.setText(%playerName);
	AskForItem_Name.setContent(%ItemName);
	AskForItem_Price.setText(%money);
	AskForItem_Num.setText(%num);
	AskForItem_AllPrice.setText(%money * %num);
	OpenAskForItemWnd();
}
//增加购买数量
function addBuyMarketItemNum()
{
	%num = buyMarketItem_Num.getValue();
	%num++;
	if(%num > 99)
	 %num = 99;
	buyMarketItem_Num.setText(%num);
	BuyItemInfo_AllPrice.setText($MarketItemPrice * %num);
}
//减少购买数量
function reduceBuyMarketItemNum()
{
	%num = buyMarketItem_Num.getValue();
	%num--;
	if(%num < 1)
	 %num = 1;
	buyMarketItem_Num.setText(%num);
	BuyItemInfo_AllPrice.setText($MarketItemPrice * %num);
}
//确定购买
function clickOnBuyItemOk()
{
	%num = buyMarketItem_Num.getValue();
	doBuyMarketItem(%num);
	//CloseShoppingBasketWnd();
}

function showComplementWnd(%type)
{
	if(%type == 1)
	{
		ComplementItem_Wnd.setBitmap("gameres/gui/images/GUIWindow45_1_005.png");
		Complement_Ask_Btn.setText("确定赠送");
		Complement_Ask_Btn.Command = "doComplementOk();";
	}
	else
	{
		ComplementItem_Wnd.setBitmap("gameres/gui/images/GUIWindow45_1_006.png");
		Complement_Ask_Btn.setText("确定索要");		
		Complement_Ask_Btn.Command = "doAskForOk();";
	}
}
//显示好友列表
function AddFriendToComplementList(%name,%id)
{
	Complement_FriendList.Add(%name,%id);
}
//清除好友列表
function ClearFriendComplementList()
{
	Complement_FriendList.clear();
}
//增加或减少索取或赠送数量
function AddComplentCount()
{
	%num = ComplementItem_Num.getValue();
	%num++;
	if(%num > 99)
		%num = 99;
	ComplementItem_Num.setText(%num);
	ComplementItem_AllPrice.setText($MarketItemMoney * %num);
}

function ReduceComplentCount()
{
	%num = ComplementItem_Num.getValue();
	%num--;
	if(%num < 1)
		%num = 1;
	ComplementItem_Num.setText(%num);
	ComplementItem_AllPrice.setText($MarketItemMoney * %num);
}

//确定赠送
function doComplementOk()
{
	%name = Complement_FriendList.getText();
	%num = ComplementItem_Num.getValue();
	EnsureComplement(%num, %name);
}
//确定索要
function doAskForOk()
{
	%name = Complement_FriendList.getText();
	%num = ComplementItem_Num.getValue();
	EnsureAskFor(%num, %name);
}
//搜索
function ClickOnSearch()
{
	%text = BusinessCity_ItemNameEdit.getValue();
	echo("text = " @ %text);
	if(%text !$= "")
	{
		ClickBusinessCitySeries(9);
		//filterSuperMarket(10, 1);
		doFindMarketItem(%text);
		BusinessCity_ItemNameEdit.setText("");
	}
	else
		return;
}
$BasketListCount = 0;
//显示购物篮物品信息
function showBasketballInfo(%count)
{
	BasketItemNum.setText(%count);
	$BasketListCount = %count;
	for(%i=0; %i<%count; %i++)
	{
		 new GuiControl("BasketballItem_" @ %i) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "1";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = 1 SPC (1+%i*56);
         Extent = "167 58";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         mustPrerender = "0";
         hovertime = "1000";
         
				 new GuiBitmapCtrl("BasketballItem_Back_" @ %i) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "1 1";
            Extent = "165 56";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            alpha = "255";
            useFadeout = "1";
            bitmap = "gameres/gui/images/GUIWindow45_1_010.png";
            wrap = "0";
         };
         new GuiControl("BasketballItemName_" @ %i) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "1";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = "60 5";
         Extent = "100 15";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         mustPrerender = "0";
         hovertime = "1000";
         
         new GuiRichTextCtrl("BasketballItem_Name_" @ %i) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiCurrencyShowInfoTextProfile_1";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "0 0";
            Extent = "100 19";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
         };
        };
         new GuiTextCtrl("BasketballItem_Price_" @ %i) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiTextProfile5";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "61 29";
            Extent = "69 18";
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
            text = "99999";
            maxLength = "1024";
         };
         new GuiBitmapCtrl("BasketballItem_CoinBmp_" @ %i) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "130 29";
            Extent = "16 16";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            alpha = "255";
            useFadeout = "1";
            bitmap = "gameres/gui/images/GUIWindow31_1_027.png";
            wrap = "0";
         };
         new GuiButtonCtrl("BasketballItem_Button_" @ %i) {
      			canSaveDynamicFields = "0";
     				Enabled = "1";
      			isContainer = "0";
      			Profile = "GuiCoverImageButtonProfile";
      			HorizSizing = "right";
      			VertSizing = "bottom";
      			position = "1 1";
      			Extent = "165 56";
      			MinExtent = "8 2";
      			canSave = "1";
      			Visible = "1";
      			Command = "setSelectBasketItemID(" @ %i @");";
      			hovertime = "1000";
      			groupNum = "1";
      			buttonType = "RadioButton";
      			useMouseEvents = "0";
      			IsStateOn = "0";
   			 };
         new dGuiShortCut("BasketballItem_ShorCut_" @ %i) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = "5 5";
            Extent = "48 48";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "1";
            mustPrerender = "0";
            hovertime = "1000";
            slotType = "44";
            slotCol =%i;
            boudary = "0 0 48 48";
            disabledrag = "1";
            MouseDownCommand = "setSelectedBasketItem(" @%i @");";
         };
      };
      BasketScroll_test.add("BasketballItem_" @ %i);
      BasketScroll_test.setExtent(179,58*(%i+1));
	}
}
//点击放入购物篮
function clickOnPutInBasket()
{
	%num = buyMarketItem_Num.getValue();
	addBasketBallList(%num);
	OpenShoppingBasketWnd();
	CloseBusinessCityBuyWnd();
}

//选中购物篮中的物品
function setSelectedBasketItem(%index)
{
	if($LastSelectedBasketIndex != %index)
	{
		%obj = "BasketballItem_ShorCut_" @$LastSelectedBasketIndex;
		if(isObject(%obj))
			%obj.setBeSelected(0);
		%obj = "BasketballItem_ShorCut_" @%index;
		if(isObject(%obj))
			%obj.setBeSelected(1);
		$LastSelectedBasketIndex = %index;
	}
	setSelectBasketItemID(%index);
}

function setSelectBasketItemID(%index)
{
	echo("index = " @%index);
	if($LastSelectedBasketBtnIndex != %index)
	{
		%obj = "BasketballItem_Button_" @$LastSelectedBasketBtnIndex;
		if(isObject(%obj))
			%obj.setStateOn(0);
		%obj = "BasketballItem_Button_" @%index;
		if(isObject(%obj))
			%obj.setStateOn(1);
		$LastSelectedBasketBtnIndex = %index;
	}
}
$BasketAllMoney = 0;
//显示购物篮物品信息
function showBasketItemListInfo(%index, %name, %money, %num, %type)
{
	%obj = BasketballItem_Name_ @ %index;
	if(isObject(%obj))
		%obj.setContent(%name);
	%obj = BasketballItem_Price_ @ %index;
	if(isObject(%obj))
		%obj.setText(%money);
	%obj = BasketballItem_CoinBmp_ @ %index;
	if(isObject(%obj))
	{
		if(%type == 1)
			%obj.setBitmap("gameres/gui/images/GUIWindow31_1_027.png"); 
		else
			%obj.setBitmap("gameres/gui/images/GUIWindow31_1_028.png"); 
	}
	$BasketAllMoney = $BasketAllMoney + %money * %num;	
}
//清除购物篮
function clearBasketList(%index)
{
	%obj = BasketballItem_ShorCut_ @ %index;
	if(isobject(%obj))
			%obj.delete();
	%obj = BasketballItem_Button_ @ %index;
	if(isobject(%obj))
			%obj.delete();
	%obj = BasketballItem_CoinBmp_ @ %index;
	if(isobject(%obj))
			%obj.delete();
	%obj = BasketballItem_Price_ @ %index;
	if(isobject(%obj))
			%obj.delete();
	%obj = BasketballItem_Name_ @ %index;
	if(isobject(%obj))
			%obj.delete();
	%obj = BasketballItemName_ @ %index;
	if(isobject(%obj))
			%obj.delete();
	%obj = BasketballItem_Back_ @ %index;
	if(isobject(%obj))
			%obj.delete();		
	%obj = BasketballItem_ @ %index;
	if(isobject(%obj))
			%obj.delete();
}

//全部清除
function clearAllBasketList()
{
	for(%i=0; %i<$BasketListCount; %i++)
		clearBasketList(%i);
}

//删除一个物品
function clearItemFromBasket()
{
	if($LastSelectedBasketBtnIndex != -1)
		clearOneFromBasket($LastSelectedBasketBtnIndex);
	$LastSelectedBasketBtnIndex = -1;
}

//显示总价
function showAllBasketNeedMoney(%money, %type)
{
	if(%type == 1)
			AllBasket_Money.setBitmap("gameres/gui/images/GUIWindow31_1_027.png"); 
		else
			AllBasket_Money.setBitmap("gameres/gui/images/GUIWindow31_1_028.png"); 
		AllBasketNeed_Money.setText(%money);
}
