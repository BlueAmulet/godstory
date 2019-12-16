//===========================================================================================================
// �ĵ�˵��:�ֿ�������
// ����ʱ��:2009-9-7
// ������: soar
//=========================================================================================================== 

%BitMapX = 0;
%BitMapY = 0;

//for(%j = 0; %j < 4; %j++)
//{
//   for(%i = 0; %i < 6; %i++)
//   {
//   		%num = %i+(%j * 5);
//   	  new GuiBitmapCtrl("BankWnd_SpaceBitMap" @ %num) {
//         profile = "GuiDefaultProfile";
//         horizSizing = "center";
//         vertSizing = "center";
//         position = %BitMapX @ " " @ %BitMapY;
//         extent = "237 42";
//         minExtent = "8 8";
//         visible = "0";
//         bitmap = "gameres/gui/images/GUIWindow53_1_002";
//         wrap = "0";
//         helpTag = "0";
//      };
//      ("BankWnd_BankPage_" @ %j).add("BankWnd_SpaceBitMap" @ %num);
//      %BitMapY += 42;
//   }
//    %BitMapY = 0;
//}

%ShortCutX = 2;
%ShortCutY = 2;

for(%k = 0; %k < 4; %k++)
{
	 for(%j = 0; %j < 5; %j++)
	 {
	 		for(%i = 0; %i < 6; %i++)
      {
      	 %num = %i + (%j * 6) + (%k * 30);
         new GuiBitmapCtrl("BankWnd_SpaceBitMap" @ %num) {
            profile = "GuiDefaultProfile";
            horizSizing = "center";
            vertSizing = "center";
            position = %BitMapX @ " " @ %BitMapY;
            extent = "36 36";
            minExtent = "8 8";
            visible = "0";
            bitmap = "gameres/gui/images/GUIWindow31_1_006";
            wrap = "0";
            helpTag = "0";
         };
         ("BankWnd_BankPage_" @ %k).add("BankWnd_SpaceBitMap" @ %num);
      	 new dGuiShortCut("BankWnd_SpaceShortCut" @ %num) {
            canSaveDynamicFields = "0";
            Enabled = "1";
            isContainer = "0";
            Profile = "GuiDefaultProfile";
            HorizSizing = "right";
            VertSizing = "bottom";
            position = %ShortCutX @ " " @ %ShortCutY;
            Extent = "32 32";
            MinExtent = "8 2";
            canSave = "1";
            Visible = "0";
            hovertime = "1000";
            slotType = "4";
            slotCol = %num;
            boudary = "0 0 32 32";
         };
         ("BankWnd_BankPage_" @ %k).add("BankWnd_SpaceShortCut" @ %num);
         %BitMapX += 39;
         %ShortCutX += 39;
      }
      %BitMapX = 0;
      %BitMapY += 41;
      %ShortCutX = 2;
      %ShortCutY += 41;
	 }
	 %BitMapX = 0;
   %BitMapY = 0;
	 %ShortCutX = 2;
   %ShortCutY = 2;
}


function CloseBankWnd()
{
	closeBank();
	for(%i=0; %i<120; %i++)
	{
		("BankWnd_SpaceBitMap" @ %i).setVisible(0);
		("BankWnd_SpaceShortCut" @ %i).setVisible(0);
	}
}

function openBank(%maxSlots, %level)
{
	ForceOpenBag();
	BankWnd.setvisible(1);
	$IsDirty++;
	setActive();
	showSlots(%maxSlots);
	showBankText(%Level);
}
function setActive()
{
	BankWnd_ChangePageButton_1.setActive(1);
	BankWnd_ChangePageButton_2.setActive(0);
	BankWnd_ChangePageButton_3.setActive(0);
	BankWnd_ChangePageButton_4.setActive(0);
}
function HideBank()
{
	BankWnd.setvisible(0);
	$IsDirty--;
	if(BankWnd_InputWnd.isvisible())
		BankWnd_InputWnd.setvisible(0);
		$IsDirty--;
}

function OpenOrCloseBankWnd_InputWnd()
{
	if(BankWnd_InputWnd.isvisible())
	{
		BankWnd_InputWnd.setvisible(0);
		$IsDirty--;
	}
	else
	{
		BankWnd_InputWnd.setvisible(1);
		$IsDirty++;
	}
}

function OpenBankWnd_InputWnd()
{
	BankWnd_InputWnd.setvisible(1);
	$IsDirty++;
}

function CloseBankWnd_InputWnd()
{
	BankWnd_InputWnd.setvisible(0);
	$IsDirty--;
}

function OpenBankMoneyInputWnd(%type)
{
	if(%type)
	{
		BankWnd_InputWnd_Name.settext("ȡ Ǯ");
		BankWnd_InputWnd_InputMoneyNum.settext("����ȡ������");
		BankWnd_InputWnd_AllButton.settext("ȫ ȡ");
		clearMoney();
		BankWnd_InputWnd_AllButton.Command ="takeAllMoney();";
		BankWnd_InputWnd_OK.Command = "onDoOk(1);";
	}
	else
	{
		BankWnd_InputWnd_Name.settext("�� Ǯ");
		BankWnd_InputWnd_InputMoneyNum.settext("����洢����");
		BankWnd_InputWnd_AllButton.settext("ȫ ��");
		clearMoney();
		BankWnd_InputWnd_OK.Command = "onDoOk(0);";
		BankWnd_InputWnd_AllButton.Command = "saveAllMoney();";
	}
	
	BankWnd_InputWnd.setvisible(1);
	$IsDirty++;
}

function BankWndInputNum(%num)
{
	if(BankWnd_InputWnd_Gold.isFirstResponder())
  {
      BankWnd_InputWnd_Gold.InsertString(%num);
  }
  else if(BankWnd_InputWnd_Siller.isFirstResponder())
  {
      BankWnd_InputWnd_Siller.InsertString(%num);
  }
  else if(BankWnd_InputWnd_Copper.isFirstResponder())
  {
  	BankWnd_InputWnd_Copper.InsertString(%num);
  }
}

function DelectInputNum()
{
	if(BankWnd_InputWnd_Gold.isFirstResponder())
  {
      BankWnd_InputWnd_Gold.DeleteString();
  }
  else if(BankWnd_InputWnd_Siller.isFirstResponder())
  {
      BankWnd_InputWnd_Siller.DeleteString();
  }
  else if(BankWnd_InputWnd_Copper.isFirstResponder())
  {
  	BankWnd_InputWnd_Copper.DeleteString();
  }
}

function BuyBankSpace()
{
	OpenDialog_OK_Cancel();
	Dialog_OK_Cancel.SetContent("����ռ���Ҫ����XXX������ȷ��Ҫ������");
	Dialog_OK_Cancel.SetOK("ȷ ��", "BuyBankSpace_OK();");
	Dialog_OK_Cancel.SetCancel("ȡ ��", "");
}

function BuyBankSpace_OK()
{
	//Dialog_OK_Cancel.SetContent("��Ǯ���㣬�޷�����ռ䣡");
	buySpace();
	//Dialog_OK_Cancel.SetOK("ȷ ��", "");
	//Dialog_OK_Cancel.setVisible(1);
}

function ChangeBankSpace(%index)
{
	switch(%index)
	{
		case 0:
			BankWnd_BankPage_0.setvisible(1);
			BankWnd_BankPage_1.setvisible(0);
			BankWnd_BankPage_2.setvisible(0);
			BankWnd_BankPage_3.setvisible(0);
		case 1:
			BankWnd_BankPage_0.setvisible(0);
			BankWnd_BankPage_1.setvisible(1);
			BankWnd_BankPage_2.setvisible(0);
			BankWnd_BankPage_3.setvisible(0);
		case 2:
			BankWnd_BankPage_0.setvisible(0);
			BankWnd_BankPage_1.setvisible(0);
			BankWnd_BankPage_2.setvisible(1);
			BankWnd_BankPage_3.setvisible(0);
		case 3:
			BankWnd_BankPage_0.setvisible(0);
			BankWnd_BankPage_1.setvisible(0);
			BankWnd_BankPage_2.setvisible(0);
			BankWnd_BankPage_3.setvisible(1);
	}
}
//�������
function clearMoney()
{
	BankWnd_InputWnd_Gold.setText("");
	BankWnd_InputWnd_Siller.setText("");
	BankWnd_InputWnd_Copper.setText("");
}

function onDoOk(%type)
{
	%money = 0;   BankWnd_InputWnd_Copper.getText();
	if(BankWnd_InputWnd_Gold.getText()!= "")
	{
			%money = BankWnd_InputWnd_Gold.getText()*10000;
	}
	if(BankWnd_InputWnd_Siller.getText()!= "")
	{
		%money += BankWnd_InputWnd_Siller.getText()*100;
	}
	if(BankWnd_InputWnd_Copper.getText()!= "")
	{
		%money += BankWnd_InputWnd_Copper.getText();
	}
	if(%type)
		getMoney(%money);
	else
		saveMoney(%money);
	clearMoney();
}
//��ʾ���
function showBankMoney(%gold,%silver,%copper)
{
	if(%gold == "0" && %silver == "0")
	{
		%gold = "";
		%silver = "";
	}
	else if(%gold == "0" && %silver != "0")
	{
		%gold = "";
	}
	else
	{
	}
	
	BankWnd_Deposit_Gold.settext(%gold);
	BankWnd_Deposit_Siller.settext(%silver);
	BankWnd_Deposit_Copper.settext(%copper);  
}
function cancelBankInput()
{
	clearMoney();
	BankWnd_InputWnd.setvisible(0);
	$IsDirty--;
}

function showSlots(%num)
{
	for(%i=0; %i<%num; %i++)
	{
		("BankWnd_SpaceShortCut" @ %i).setVisible(1);
		("BankWnd_SpaceBitMap" @ %i).setVisible(1);
	}
	%row = %num/6;
		
	if(%row > 5 && %row <= 11)
		BankWnd_ChangePageButton_2.setActive(1);
	if(%row > 11 && %row <= 17)
	{
		BankWnd_ChangePageButton_2.setActive(1);
		BankWnd_ChangePageButton_3.setActive(1);
	}
	if(%row > 17 && %row <= 23)
	{
		BankWnd_ChangePageButton_2.setActive(1);
		BankWnd_ChangePageButton_3.setActive(1);
		BankWnd_ChangePageButton_4.setActive(1);
	}
	BankWnd_BagSpaceNum.setText(%num @"/120");
}

function setBuyActive(%bool)
{
	BankWnd_BuySpaceButton.setActive(%bool);
}	

function showBankText(%level)
{
	if(%level >= 10 && %level < 20)
		BankWnd_Hint.setText("��ҵȼ���10�����ɹ���ռ�6��");
	if(%level >= 20 && %level < 30)
		BankWnd_Hint.setText("��ҵȼ���10�����ɹ���ռ�6��");
	if(%level >= 30 && %level < 50)
		BankWnd_Hint.setText("��ҵȼ���10�����ɹ���ռ�6��");
	if(%level >= 50 && %level < 70)
		BankWnd_Hint.setText("��ҵȼ���10�����ɹ���ռ�12��");
	if(%level >= 70 && %level < 90)
		BankWnd_Hint.setText("��ҵȼ���10�����ɹ���ռ�12��");
	if(%level >= 90 && %level < 110)
		BankWnd_Hint.setText("��ҵȼ���10�����ɹ���ռ�12��");
	if(%level >= 110)
		BankWnd_Hint.setText("��ҵȼ���10�����ɹ���ռ�24��");
		
}

function setLockFlag(%flag)
{
	if(%flag)
	{
		BankWnd_LockButton.setText("����");
		BankWnd_LockButton.command = "";
	}
	else
	{
		BankWnd_LockButton.setText("����");
		BankWnd_LockButton.command = "setBankLock();";
	}
}