//===========================================================================================================
// �ĵ�˵��:�����������
// ����ʱ��:2009-6-2
// ������: soar
// �޸��ˣ� hanfei
// �޸����ڣ�2009-12-23
//===========================================================================================================

//=============================================================================
//�������ĺϰ�����
// 

//�򿪻�رպϰ�������
function OpenOrCloseAllBag()
{
	if(AllBagWndGui.IsVisible())
		CloseAllBag();
	else
		OpenAllBag();
}

//�򿪺ϰ�������
function OpenAllBag()
{
	GameMainWndGui_Layer3.PushToBack(AllBagWndGui);
	AllBagWndGui.setVisible(1);
	initIventory();
	FilterItem(-1, 0);
	$IsDirty++;
}

//�رպϰ�������
function CloseAllBag()
{
	AllBagWndGui.setVisible(0);
	$IsDirty--;
}

//�رշְ���������ʾ���򿪺ϰ���������ʾ
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

//�����ϰ��������ı���UI����
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

//�ϰ�����������
function AllBag_LockCom()
{
	//AllBag_LockButton.setVisible(0);
	//AllBag_UnLockButton.setVisible(1);
}

//�ϰ�����������
function AllBag_UnLockCom()
{
	//AllBag_LockButton.setVisible(1);
	//AllBag_UnLockButton.setVisible(0);
}

//�򿪻�رպϰ�������������Ʒ����
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

//��ʾ������Ʒ����
function OpenAllBagHotItem()
{
	HotItemWnd_ShowPage.setPosition(1,28);
	HotItemWnd_DiscountItemButton.setPosition(1,358);
}

//��ʾ�ۿ���ڴ���
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
//�������ķְ�����
//

//�򿪻�رշְ�������
function OpenOrCloseSingleBag()
{
	if(SingleBagWndGui.IsVisible())
		CloseSingleBag();
	else
		OpenSingleBag();
}

//�򿪷ְ�������
function OpenSingleBag()
{
	GameMainWndGui_Layer3.PushToBack(SingleBagWndGui);
	SingleBagWndGui.setVisible(1);
	ChooseTabItem(0);
	initIventory();
	$IsDirty++;
}

//�رշְ�������
function CloseSingleBag()
{
	SingleBagWndGui.setVisible(0);
	$IsDirty--;
}

$ChoiceBagIndex = 0;	//��ǰѡ��İ�������Ŀǰ��0,1,2,-1����)
//�л���������������λ
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

//�رպϰ����򿪷ְ�������
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

//�ְ�����������
function SingleBag_LockCom()
{
	//SingleBag_LockButton.setVisible(0);
	//SingleBag_UnLockButton.setVisible(1);
}

//�ְ�����������
function SingleBag_UnLockCom()
{
	//SingleBag_LockButton.setVisible(1);
	//SingleBag_UnLockButton.setVisible(0);
}

//�򿪷ְ�����������ͨ����
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

//�򿪷ְ��������ĵ�һ����չ����
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

//�򿪷ְ��������ĵڶ�����չ����
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

//�򿪻�رշְ���������������Ʒ����
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

//��ʾ�ְ���������������Ʒ����
function OpenSingleBagHotItem()
{
	HotItemWnd_Single_ShowPage.setPosition(2,26);
	HotItemWnd_Single_DiscountItemButton.setPosition(1,365);
}

//��ʾ�ְ����������ۿ���Ʒ����
function OpenSingleBagDiscountItem()
{
	HotItemWnd_Single_ShowPage.setPosition(2,50);
	HotItemWnd_Single_DiscountItemButton.setPosition(1,22);
}

function OpenSingleBagHomeItem()
{	
}

// %BagIndex  �����������
// %type 0-ȫ�� 1-���� 2-���� 3-����
function FilterItem(%BagIndex, %type)
{
	%ShortX = %ShortY = 0;
	%start = 0;
	%end = GetBagSize(0);	
	%posX = %posY = 0;
	%ChangeNumX = 4;
	%ChangeNumY = 5;
	%AllNum = 0;
	//�������������UI����
	clearBagSlot();	

	if(%BagIndex == 1)			 // ����1
	{
		%start = GetBagSize(0);
		%end = GetBagSize(%BagIndex) + %start;
		CreateSlotBackGround();	
		SingleBag_AddBagPage_1.Add("Bag_SlotBackground");	
	}
	else if(%BagIndex == 2)	 // ����2
	{
		%start = GetBagSize(0) + GetBagSize(1);
		%end = GetBagSize(2) + %start;
		CreateSlotBackGround();	
		SingleBag_AddBagPage_2.Add("Bag_SlotBackground");	
	}
	else if(%BagIndex == -1) // �ϰ�������
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

//�����ְ��������ı���UI����
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

//��հ���������UI����
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

//��ʼ��ز���
function initIventory()
{
	%bag0Num = GetBagSize(0);
	%bag1Num = GetBagSize(1);
	%bag2Num = GetBagSize(2);
	SingleBag_CommonButton.setText("     Ǭ����(40��)");
	if(%bag1Num > 0)
	{
		SingleBag_AddBagButton_1.setActive(1);
		SingleBag_AddBagButton_1.setText("     Ǭ����("@ %bag1Num @"��)");
	}
	else
	{
		SingleBag_AddBagButton_1.setActive(0);
		SingleBag_AddBagButton_1.setText("     Ǭ����");
	}
	if(%bag2Num > 0)
	{
		SingleBag_AddBagButton_2.setActive(1);
		SingleBag_AddBagButton_2.setText("     Ǭ����("@ %bag2Num @"��)");
	}
	else
	{
		SingleBag_AddBagButton_2.setActive(0);
		SingleBag_AddBagButton_2.setText("     Ǭ����");
	}
	
	%player = getPlayer();
	if(%player == 0)
	  return;
	%level = %player.getLevel();
	//�ȼ����ƣ����û��ֹ��ذ�ť
	if(%level < 10)
		SingleBag_BusinessButton.setActive(0);
	else
		SingleBag_BusinessButton.setActive(1);	
	
	%allNum = %bag0Num + %bag1Num + %bag2Num;	
	if(%allNum > 40)
	{
		if(%level > 10)
			SingleBag_SplitBagButton.setActive(1);
		AllBag_CurrentlyItemNum.setText("Ǭ���� ("@ %allNum @"��)");
	}
	else
	{
		SingleBag_SplitBagButton.setActive(0);
		AllBag_CurrentlyItemNum.setText("Ǭ���� ");
	}
	
	if($BagState == 0)
	{
		//�ְ�״̬
		FilterItem(0,0);
		OpenSingleBag_CommonPage();
	}
	else
	{
		//�ϰ�״̬
		FilterItem(-1,0);
	}
}

//ˢ�°������Ľ�Ԫ��ʾ
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

//ˢ�°���������Ԫ��ʾ
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
// ����������Ʒ������Ʒ���ȸ���ʾ����
//��ʾ�ȸ���Ʒ��ʾ����
function ShowItemHot(%x, %y, %height, %icon)
{
	GameMainWndGui_Layer5.pushtoback(ItemHot_Show);
	ItemHot_Show.setposition(%x, %y);
	ItemHot_Show.setExtent(220, %height+5);
	HotItem_Image.setBitmap(%icon);
	ItemHot_Show.setvisible(1);
}

//�����ȸ���Ʒ��ʾ����
function HideItemHot()
{
	ItemHot_Show.setvisible(0);
}

//��ʾ���ڶԱȵ��ȸ���Ʒ��ʾ����
function ShowCompareItemHot(%x, %y, %height, %icon)
{
		GameMainWndGui_Layer5.pushtoback(CompareItemHot_Show);
		CompareItemHot_Show.setposition(%x, %y);
		CompareItemHot_Show.setExtent(220, %height+24);
		CompareHotItem_Image.setBitmap(%icon);
		CompareItemHot_Show.setvisible(1);		
}

//�������ڶԱȵ��ȸ���Ʒ��ʾ����
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
// ���촰������Ʒ���ӵ��ȸ���ʾ����

// ��������������Ʒ�����ȸ���ʾ����
function closeChatLink()
{
	HideChatLink_Item();
	HideCompareChatLink_Item();
	
	ChatItem_Close.setVisible( 0 );
}

//��ʾ������Ʒ�����ȸ���ʾ����
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

//����������Ʒ�����ȸ���ʾ����
function HideChatLink_Item()
{
	ChatItem_Show.setvisible(0);	
}

//��ʾ������Ʒ�������ڶԱȵ��ȸ���ʾ����
function ShowCompareChatLink_Item(%x, %y, %height, %icon)
{
	GameMainWndGui_Layer5.pushtoback(CompareChatItem_Show);
	CompareChatItem_Show.setposition(%x + 220, %y);
	ChatItem_Close.setPosition( %x + 438, %y + 2 );
	CompareChatItem_Show.setExtent(220, %height+5);
	CompareChatItem_Image.setBitmap(%icon);
	CompareChatItem_Show.setvisible(1);
}

//����������Ʒ�������ڶԱȵ��ȸ���ʾ����
function HideCompareChatLink_Item()
{
	CompareChatItem_Show.setvisible(0);
}

//=============================================================================
// ����

//��ʾװ����ȷ�϶Ի�����
function ShowBindWnd(%index, %flag)
{
	OpenDialog_Small_OK_Cancel();
	Dialog_Small_OK_Cancel.SetContent("װ������Ʒ�������");
	Dialog_Small_OK_Cancel.SetOK("ȷ ��", "equipBindItem(" @%index @", " @%flag @");");
	Dialog_Small_OK_Cancel.SetCancel("ȡ ��", "cancelAction();");
}

//��ʾʰȡ��ȷ�϶Ի�����
function ShowPickUpBind(%index)
{
	OpenDialog_Small_OK_Cancel();
	Dialog_Small_OK_Cancel.SetContent("ʰȡ����Ʒ�������");
	Dialog_Small_OK_Cancel.SetOK("ȷ ��", "pickupBindItem(%index );");
	Dialog_Small_OK_Cancel.SetCancel("ȡ ��", "cancelAction();");
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