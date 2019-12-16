//===========================================================================================================
// 文档说明:交易界面操作
// 创建时间:2009-5-26
// 创建人: soar
//=========================================================================================================== 
//打开交易窗口
function OpenTradeWnd(%selfname, %targetname)
{
	TradeAllWndGui.setVisible(1);
	ForceOpenBag();
	TradeSelfName.setText(%selfname);
	TradeObjectName.setText(%targetname);
	MySelfMoney_Gold.setText("0");
	MySelfMoney_Silver.setText("0");
	MySelfMoney_Copper.setText("0");
	ObjectMoney_Gold.setText("0");
	ObjectMoney_Silver.setText("0");
	ObjectMoney_Copper.setText("0");
	TradeLockButton.setactive(1);
	TradeLockButton.setText("锁定交易");
	TradeOkButton.setactive(0);
	TradeWndGui_LockImage.setvisible(0);
	TradeWndGui_Self_LockImage.setvisible(0);
	$IsDirty++;
}

//关闭交易窗口
function CloseTradeWnd()
{
	TradeAllWndGui.setVisible(0);
	$IsDirty--;
}

//向服务端发送关闭交易消息
function CancelTradeWnd()
{
	CancelTrade();
}

//锁定自己的交易窗口
function LockMySelfTradeWnd(%bothlock)
{	
	TradeWndGui_Self_LockImage.setvisible(1);
	TradeLockButton.setText("取消锁定");
	TradeOkButton.setactive(%bothlock);
}

//锁定对方交易窗口
function LockTargetTradeWnd(%money, %bothlock)
{
	TradeWndGui_LockImage.setvisible(1);
	%Gold = mfloor(%money / 10000);
	%Silver = mfloor((%money - %Gold * 10000) / 100);
	%Copper = (%money % 100);
	ObjectMoney_Gold.SetText(%Gold);
	ObjectMoney_Silver.SetText(%Silver);
	ObjectMoney_Copper.SetText(%Copper);
	TradeOkButton.setactive(%bothlock);
}

//解除锁定自己的交易
function UnLockMySelfTradeWnd()
{	
	TradeWndGui_Self_LockImage.setvisible(0);
	TradeLockButton.setText("锁定交易");
	TradeOkButton.setactive(0);
}

//解除锁定对方交易
function UnlockTargetTradeWnd()
{
	TradeWndGui_LockImage.setvisible(0);
	TradeOkButton.setactive(0);
}

//”确认“自己的交易
function EndMySelfTradeWnd()
{
	TradeOkButton.setactive(0);
	TradeLockButton.setactive(0);
}

//对方"确认"交易
function EndTargetTradeWnd()
{
	TradeLockButton.setactive(0);
}

//点击”锁定交易“按钮或“取消锁定”按钮
function ClickLockTrade()
{
	if(TradeLockButton.getText() $= "锁定交易")
	{
		%TradeMoney = MySelfMoney_Gold.getvalue() * 10000 + MySelfMoney_Silver.getvalue() * 100 + MySelfMoney_Copper.getvalue();
		LockTrade(%TradeMoney);
	}
	else
	{
		cancelLockTrade();
	}
}

function OpenOrCloseTradeHelpWnd()
{
	if(TradeHelpWndGui.IsVisible())
	{
		CloseTradeHelpWnd();
		//TradeHelpButton.setStateOn(0);
	}
	else
	{
		OpenTradeHelpWnd();
	}
}

function OpenTradeHelpWnd()
{
	if(TradeHelpWndGUi.isvisible())
		return;
	clearStallHelpItem();
	TradeAllWndGui.setextent(getword(TradeAllWndGui.getextent(),0) + getword(TradeHelpWndGui.getextent(),0),getword(TradeAllWndGui.getextent(),1));
	TradeHelpWndGui.setposition(getword(TradeWndGui.getposition(),0) + getword(TradeWndGui.getextent(),0),getword(TradeHelpWndGui.getposition(),1));
	TradeHelpWndGui.setVisible(1);
	//GameMainWndGui_Layer4.pushtoback(TradeHelpeWndGUi);
	$IsDirty++;
}

function CloseTradeHelpWnd()
{
	if(!TradeHelpWndGUi.isvisible())
			return;
	TradeAllWndGui.setextent(getword(TradeAllWndGui.getextent(),0) - getword(TradeHelpWndGui.getextent(),0),getword(TradeAllWndGui.getextent(),1));
	//TradeHelpWndGui.setposition(getword(TradeWndGui.getposition(),0) - getword(TradeWndGui.getextent(),0) ,getword(TradeHelpWndGui.getposition(),1));
	TradeHelpWndGui.setVisible(0);
	$IsDirty--;
}

function OpenOrCloseTradeNoteWndGui()
{
	if(TradeNoteWndGui.IsVisible())
	{
		CloseTradeNoteWnd();
	}
	else
	{
		OpenTradeNoteWnd();
	}
}

function OpenTradeNoteWnd()
{
	if(TradeNoteWndGui.isvisible())
		return;
	TradeAllWndGui.setposition(getword(TradeAllWndGui.getposition(),0) - getword(TradeNoteWndGui.getextent(),0),getword(TradeAllWndGui.getposition(),1));
	TradeAllWndGui.setextent(getword(TradeAllWndGui.getextent(),0) + getword(TradeNoteWndGui.getextent(),0),getword(TradeAllWndGui.getextent(),1));
	TradeWndGui.setposition(getword(TradeNoteWndGui.getextent(),0) ,getword(TradeWndGui.getposition(),1));
	TradeHelpWndGui.setposition(getword(TradeWndGui.getposition(),0) + getword(TradeWndGui.getextent(),0) ,getword(TradeHelpWndGui.getposition(),1));
	TradeNoteWndGui.setVisible(1);
	$IsDirty++;
}

function CloseTradeNoteWnd()
{
	if(!TradeNoteWndGui.isvisible())
		return;
	TradeAllWndGui.setposition(getword(TradeAllWndGui.getposition(),0) + getword(TradeNoteWndGui.getextent(),0),getword(TradeAllWndGui.getposition(),1));
	TradeAllWndGui.setextent(getword(TradeAllWndGui.getextent(),0) - getword(TradeNoteWndGui.getextent(),0),getword(TradeAllWndGui.getextent(),1));
	TradeWndGui.setposition(0,getword(TradeWndGui.getposition(),1));
	TradeHelpWndGui.setposition(getword(TradeWndGui.getposition(),0) + getword(TradeWndGui.getextent(),0),getword(TradeHelpWndGui.getposition(),1));
	TradeNoteWndGui.setVisible(0);
	$IsDirty--;
}

function OpenCommonScroll()
{
	TradeScroll_Common.setVisible(1);
	TradeScroll_Excellence.setVisible(0);
	TradeScroll_Singularity.setVisible(0);
	TradeScroll_Pet.setVisible(0);
}

function OpenExcellenceScroll()
{
	TradeScroll_Common.setVisible(0);
	TradeScroll_Excellence.setVisible(1);
	TradeScroll_Singularity.setVisible(0);
	TradeScroll_Pet.setVisible(0);
}

function OpenSingularityScroll()
{
	TradeScroll_Common.setVisible(0);
	TradeScroll_Excellence.setVisible(0);
	TradeScroll_Singularity.setVisible(1);
	TradeScroll_Pet.setVisible(0);
}

function OpenPetScroll()
{
	TradeScroll_Common.setVisible(0);
	TradeScroll_Excellence.setVisible(0);
	TradeScroll_Singularity.setVisible(0);
	TradeScroll_Pet.setVisible(1);
}

//打开或关闭输入金钱窗口
function OpenOrCloseTradeInputMoneyWnd()
{
	if(TradeInputMoneyWndGui.IsVisible())
		CloseTradeInputMoneyWnd();
	else
		OpenTradeInputMoneyWnd();
}

//打开输入金钱窗口
function OpenTradeInputMoneyWnd()
{
	TradeInputMoneyWndGui.setVisible(1);
	MySelfMoney_Gold.setText("");
	MySelfMoney_GoldImage.setvisible(0);
	MySelfMoney_Silver.setText("");
	MySelfMoney_SilverImage.setvisible(0);
	MySelfMoney_Copper.setText("0");
	$IsDirty++;
}

// 关闭输入金钱窗口
function CloseTradeInputMoneyWnd()
{
	TradeInputMoneyWndGui.setVisible(0);
	InputMoney_Gold.setText("0");
	InputMoney_Silver.setText("0");
	InputMoney_Copper.setText("0");
	$IsDirty--;
}

// 从输入金钱窗口将金钱值记录到交易主窗口
function SetTradeMoneyInfo()
{
	if(InputMoney_Gold.getText() $= "0" && InputMoney_Silver.getText() $= "0")
	{
		MySelfMoney_Gold.setText("");
		MySelfMoney_GoldImage.setvisible(0);
		MySelfMoney_Silver.setText("");
		MySelfMoney_SilverImage.setvisible(0);
		MySelfMoney_Copper.setText(InputMoney_Copper.getText());
		echo("情况1:" @ MySelfMoney_GoldImage.isvisible @ "," @ MySelfMoney_SilverImage.isvisible());
	}
	else if(InputMoney_Gold.getText() $= "0" && InputMoney_Silver.getText() !$= "0")
	{
		MySelfMoney_Gold.setText("");
		MySelfMoney_GoldImage.setvisible(0);
		MySelfMoney_Silver.setText(InputMoney_Silver.getText());
		MySelfMoney_SilverImage.setvisible(1);
		MySelfMoney_Copper.setText(InputMoney_Copper.getText());
		echo("情况2:" @ MySelfMoney_GoldImage.isvisible @ "," @ MySelfMoney_SilverImage.isvisible());
	}
	else
	{
		MySelfMoney_Gold.setText(InputMoney_Gold.getText());
		MySelfMoney_GoldImage.setvisible(1);
		MySelfMoney_Silver.setText(InputMoney_Silver.getText());
		MySelfMoney_SilverImage.setvisible(1);
		MySelfMoney_Copper.setText(InputMoney_Copper.getText());
		echo("情况3:" @ MySelfMoney_GoldImage.isvisible @ "," @ MySelfMoney_SilverImage.isvisible());
	}
	
	//if(InputMoney_Gold.getText() !$= "0" && InputMoney_Gold.getText() !$= "")
	//{
	//	MySelfMoney_Gold.setText(InputMoney_Gold.getText());
	//	MySelfMoney_GoldImage.setvisible(1);
	//	MySelfMoney_Silver.setText(InputMoney_Silver.getText());
	//	MySelfMoney_SilverImage.setvisible(1);
	//	MySelfMoney_Copper.setText(InputMoney_Copper.getText());
	//}
	//else if(InputMoney_Silver.getText() !$= "0" && InputMoney_Silver.getText() !$= "")
	//{
	//	MySelfMoney_Silver.setText(InputMoney_Silver.getText());
	//	MySelfMoney_SilverImage.setvisible(1);
	//	MySelfMoney_Copper.setText(InputMoney_Copper.getText());
	//}
	//else if(InputMoney_Copper.getText() !$= "0" && InputMoney_Copper.getText() !$= "")
	//{
	//	MySelfMoney_Copper.setText(InputMoney_Copper.getText());
	//}
}
