//===========================================================================================================
// �ĵ�˵��:���׽������
// ����ʱ��:2009-5-26
// ������: soar
//=========================================================================================================== 
//�򿪽��״���
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
	TradeLockButton.setText("��������");
	TradeOkButton.setactive(0);
	TradeWndGui_LockImage.setvisible(0);
	TradeWndGui_Self_LockImage.setvisible(0);
	$IsDirty++;
}

//�رս��״���
function CloseTradeWnd()
{
	TradeAllWndGui.setVisible(0);
	$IsDirty--;
}

//�����˷��͹رս�����Ϣ
function CancelTradeWnd()
{
	CancelTrade();
}

//�����Լ��Ľ��״���
function LockMySelfTradeWnd(%bothlock)
{	
	TradeWndGui_Self_LockImage.setvisible(1);
	TradeLockButton.setText("ȡ������");
	TradeOkButton.setactive(%bothlock);
}

//�����Է����״���
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

//��������Լ��Ľ���
function UnLockMySelfTradeWnd()
{	
	TradeWndGui_Self_LockImage.setvisible(0);
	TradeLockButton.setText("��������");
	TradeOkButton.setactive(0);
}

//��������Է�����
function UnlockTargetTradeWnd()
{
	TradeWndGui_LockImage.setvisible(0);
	TradeOkButton.setactive(0);
}

//��ȷ�ϡ��Լ��Ľ���
function EndMySelfTradeWnd()
{
	TradeOkButton.setactive(0);
	TradeLockButton.setactive(0);
}

//�Է�"ȷ��"����
function EndTargetTradeWnd()
{
	TradeLockButton.setactive(0);
}

//������������ס���ť��ȡ����������ť
function ClickLockTrade()
{
	if(TradeLockButton.getText() $= "��������")
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

//�򿪻�ر������Ǯ����
function OpenOrCloseTradeInputMoneyWnd()
{
	if(TradeInputMoneyWndGui.IsVisible())
		CloseTradeInputMoneyWnd();
	else
		OpenTradeInputMoneyWnd();
}

//�������Ǯ����
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

// �ر������Ǯ����
function CloseTradeInputMoneyWnd()
{
	TradeInputMoneyWndGui.setVisible(0);
	InputMoney_Gold.setText("0");
	InputMoney_Silver.setText("0");
	InputMoney_Copper.setText("0");
	$IsDirty--;
}

// �������Ǯ���ڽ���Ǯֵ��¼������������
function SetTradeMoneyInfo()
{
	if(InputMoney_Gold.getText() $= "0" && InputMoney_Silver.getText() $= "0")
	{
		MySelfMoney_Gold.setText("");
		MySelfMoney_GoldImage.setvisible(0);
		MySelfMoney_Silver.setText("");
		MySelfMoney_SilverImage.setvisible(0);
		MySelfMoney_Copper.setText(InputMoney_Copper.getText());
		echo("���1:" @ MySelfMoney_GoldImage.isvisible @ "," @ MySelfMoney_SilverImage.isvisible());
	}
	else if(InputMoney_Gold.getText() $= "0" && InputMoney_Silver.getText() !$= "0")
	{
		MySelfMoney_Gold.setText("");
		MySelfMoney_GoldImage.setvisible(0);
		MySelfMoney_Silver.setText(InputMoney_Silver.getText());
		MySelfMoney_SilverImage.setvisible(1);
		MySelfMoney_Copper.setText(InputMoney_Copper.getText());
		echo("���2:" @ MySelfMoney_GoldImage.isvisible @ "," @ MySelfMoney_SilverImage.isvisible());
	}
	else
	{
		MySelfMoney_Gold.setText(InputMoney_Gold.getText());
		MySelfMoney_GoldImage.setvisible(1);
		MySelfMoney_Silver.setText(InputMoney_Silver.getText());
		MySelfMoney_SilverImage.setvisible(1);
		MySelfMoney_Copper.setText(InputMoney_Copper.getText());
		echo("���3:" @ MySelfMoney_GoldImage.isvisible @ "," @ MySelfMoney_SilverImage.isvisible());
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
