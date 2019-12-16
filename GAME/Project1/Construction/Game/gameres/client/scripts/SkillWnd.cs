//===========================================================================================================
// �ĵ�˵��:���ܲ���
// ����ʱ��:2009-6-08
// ������: Batcel
//=========================================================================================================== 

function  OpenSkillWnd()
{
	PlayOpenOrCloseGuiSound();
	if(SkillWndGui.IsVisible())
	{		
	   SkillWndGui.setVisible(0);
	   $IsDirty--;
	}
	else
	{
		 GameMainWndGui_Layer3.PushToBack(SkillWndGui);
		 Skill_BntCtrl.setStateOn(true);
		 OpenColligateSkillFace();
	   SkillWndGui.setVisible(1);
	   $IsDirty++;
	   
	}
}

function CloseSkillWnd()
{
	if(SkillWndGui.IsVisible())
	{
	  SkillWndGui.setVisible(0);
	  $IsDirty--;
  }
}

function AddSkillSeriesButton()
{
	%player = getPlayer();
  if(%player == 0)
     return;
    
   if(isObject(SkillMainSeriesBtn)) 
      SkillMainSeriesBtn.delete();
   if(isObject(SkillAssistSeriesBtn))
      SkillAssistSeriesBtn.delete();
   if(isObject(SkillMainSeriesBitmap))
      SkillMainSeriesBitmap.delete();
   if(isObject(SkillAssistSeriesBitmap))
     SkillAssistSeriesBitmap.delete();
     
	%Series_1 = %player.getClasses(0);
	%Series_2 = %player.getClasses(1);
	if(%Series_1 != 0)
	{
		%Series_1_BtnProfile = "GuiSkillSelectBtnProfile_Mu";
		switch(%Series_1)
		{
			case 1:%Series_1_BtnProfile = "GuiSkillSelectBtnProfile_Mu";
			case 2:%Series_1_BtnProfile = "GuiSkillSelectBtnProfile_Huo";
			case 3:%Series_1_BtnProfile = "GuiSkillSelectBtnProfile_Tu";
			case 4:%Series_1_BtnProfile = "GuiSkillSelectBtnProfile_Jin";
			case 5:%Series_1_BtnProfile = "GuiSkillSelectBtnProfile_Shui";
		}
				 
		 	  new GuiButtonCtrl(SkillMainSeriesBtn) {
          canSaveDynamicFields = "0";
          Enabled = "1";
          isContainer = "0";
          Profile = %Series_1_BtnProfile ;
          HorizSizing = "right";
          VertSizing = "bottom";
          position = "10 61";
          Extent = "128 48";
          MinExtent = "8 2";
          canSave = "1";
          Visible = "1";
          hovertime = "1000";
          text = "";
          groupNum = "2";
          buttonType = "RadioButton";
          useMouseEvents = "0";
          IsStateOn = "0";
          Command = "UI_DisplaySkillBySchool(" @%Series_1 @");";
        };
		     WuXingSkill_Face.add(SkillMainSeriesBtn);
		   
		    new GuiBitmapCtrl(SkillMainSeriesBitmap) {
          canSaveDynamicFields = "0";
          Enabled = "1";
          isContainer = "0";
          profile = "GuiModelessDialogProfile";
          HorizSizing = "right";
          VertSizing = "bottom";
          position = "59 69";
          Extent = "32 17";
          MinExtent = "8 2";
          canSave = "1";
          Visible = "1";
          hovertime = "1000";
          bitmap = "gameres/gui/images/GUIWindow32_1_032.png";
          wrap = "1";
        };
        WuXingSkill_Face.add(SkillMainSeriesBitmap);
	}
	
	if(%Series_2 != 0)
	{
		%Series_2_BtnProfile = "GuiSkillSelectBtnProfile_Mu";
		switch(%Series_2)
		{
			case 1:%Series_2_BtnProfile = "GuiSkillSelectBtnProfile_Mu";
			case 2:%Series_2_BtnProfile = "GuiSkillSelectBtnProfile_Huo";
			case 3:%Series_2_BtnProfile = "GuiSkillSelectBtnProfile_Tu";
			case 4:%Series_2_BtnProfile = "GuiSkillSelectBtnProfile_Jin";
			case 5:%Series_2_BtnProfile = "GuiSkillSelectBtnProfile_Shui";
		}   
		
			new GuiButtonCtrl(SkillAssistSeriesBtn) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "0";
         Profile = %Series_2_BtnProfile ;
         HorizSizing = "right";
         VertSizing = "bottom";
         position = "10 107";
         Extent = "128 48";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         hovertime = "1000";
         text = "";
         groupNum = "2";
         buttonType = "RadioButton";
         useMouseEvents = "0";
         IsStateOn = "0";
         Command = "UI_DisplaySkillBySchool("@ %Series_2 @");";
      };
		  WuXingSkill_Face.add(SkillAssistSeriesBtn);
		
		  new GuiBitmapCtrl(SkillAssistSeriesBitmap) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "0";
         profile = "GuiModelessDialogProfile";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = "59 113";
         Extent = "32 17";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         hovertime = "1000";
         bitmap = "gameres/gui/images/GUIWindow32_1_033.png";
         wrap = "1";
      };
      WuXingSkill_Face.add(SkillAssistSeriesBitmap);		
	}
	
}

//���ۺϼ��ܽ���
function OpenColligateSkillFace()
{ 
	SKillTitleText.setText("�ۺϼ���");  
	
	if(LivingSkill_Face.isVisible())
	   LivingSkill_Face.setVisible(false); 
	   
	AddSkillSeriesButton();  
	UI_DisplaySkillBySchool(0);
	WuXingFaceSeriesBtn_ZH.setStateOn(1);
	WuXingSkill_Face.setVisible(1);
  
}

//�����м��ܽ���
function OpenWuXingSkillFace()
{   
	SKillTitleText.setText("ս������");
	WuXingSkill_Face.setVisible(1);
	LivingSkill_Face.setVisible(0);
}

//������ܽ���
function OpenLivingSkillFace()
{
	SKillTitleText.setText("�����");
	GatherSkill_bntCtrl.setStateOn(true);   
	UI_DisplayLivingSkill(0);
	WuXingSkill_Face.setvisible(0);
	LivingSkill_Face.setvisible(1);
}

$SkillSeriesViewIndex = 0;
function UI_DisplaySkillBySchool(%series)
{
	$SkillSeriesViewIndex = %series;
	if(%series != 0)
	  WuXingDescBitmap.setbitmap("gameres/gui/images/GUIWindow32_1_0" @ (21+%series) @ ".png");
	else
		WuXingDescBitmap.setbitmap("gameres/gui/images/GUIWindow32_1_080.png");
	for(%i =0; %i<9;%i++)
	{
		("WuXingSkillInfo_" @ %i).setVisible(0);
	}
	DisplaySkillBySchool(%series);
}


function ShowSkillNameAndLevel(%index,%Name,%Level)
{
	("WuXingSkillInfo_" @ %index).setvisible(1);
	("WuXingSkillName_" @ %index).setText(%Name);
	("WuXingSkillLevel_" @ %index).setText(%Level @ "��");
}

function UpdateSkillSeriesView(%School)
{
	if($SkillSeriesViewIndex == %School)
	{
		UI_DisplaySkillBySchool(%School);
	}
}

//================����ϵѡ��==================
//============================================
$IsMainSeries=0;
$SkillSeriesId=0;
function OpenSkillSelectWnd(%MainSeries,%Series_2,%Series_3,%Series_4)
{
	SkillSelectBtn_MU.setStateOn(0);
	SkillSelectBtn_HUO.setStateOn(0);
	SkillSelectBtn_TU.setStateOn(0);
	SkillSelectBtn_JIN.setStateOn(0);
	SkillSelectBtn_SHUI.SetStateOn(0);
	//0��1ľ2��3��4��5ˮ
	if(%MainSeries==0)
	{
		$IsMainSeries=0;
		SkillSelectBtn_MU.setActive(1);
	  SkillSelectBtn_HUO.setActive(1);
	  SkillSelectBtn_TU.setActive(1);
	  SkillSelectBtn_JIN.setActive(1);
	  SkillSelectBtn_SHUI.setActive(1);
	}
	else
	{
		switch(%MainSeries)
		{
			case 1:SkillSelectBtn_MU.setActive(0);
			case 2:SkillSelectBtn_HUO.setActive(0);
			case 3:SkillSelectBtn_TU.setActive(0);
			case 4:SkillSelectBtn_JIN.setActive(0);
			case 5:SkillSelectBtn_SHUI.setActive(0);
		}
		$IsMainSeries=1;
	}
	SkillSelectBtn_Sure.setActive(1);
	$IsDirty++;
	GameMainWndGui_Layer3.PushToBack(SkillSelectWndGui);
	SkillSelectWndGui.setVisible(1);
}
function SkillSelectedSeries(%Series)
{
	GetSkillSeriesInfo(%Series,$IsMainSeries);
  SkillSeriesBitmap.setBitmap("gameres/gui/images/GUIWindow32_1_0" @ (16+%Series) @ ".png");
  SkillSeriesDesc_Bitmap.setBitmap("gameres/gui/images/GUIWindow32_1_0" @ (33+%Series) @ ".png");
  SkillSeriesName_Bitmap.setBitmap("gameres/gui/images/GUIWindow32_1_0" @ (9+%Series) @ ".png");
  
  SkillSeriesEffect_Bitmap.setVisible(1);
  switch(%Series)
  {
  	case 1:SkillSeriesEffect_Bitmap.setPosition(65, 52);
  	case 2:SkillSeriesEffect_Bitmap.setPosition(119, 82);
  	case 3:SkillSeriesEffect_Bitmap.setPosition(101, 147);
  	case 4:SkillSeriesEffect_Bitmap.setPosition(33, 149);
  	case 5:SkillSeriesEffect_Bitmap.setPosition(9, 86);
  }  
}

function SkillBeSureSelected()
{
	%MainSeriestxt="��";
	if($IsMainSeries==0) 
	   %MainSeriestxt = "��";
	else 
	   %MainSeriestxt = "��";
	if(SkillSelectBtn_MU.IsStateOn())
	{
	   %text = "ȷ��ѡ��ľϵ����Ϊ����"@%MainSeriestxt @"����ϵ��?" @ "<b/> <t c='0xff0000ff'>(��ʾ:��Ҳ����ѡ���������м���ϵ,ѡ���󽫲��ܸı�" @ %MainSeriestxt @ "����ϵ)</t>";
	   $SkillSeriesId=1;
	   OpenSkillSeriesSelectDialog(%text);

	}
	else if(SkillSelectBtn_HUO.IsStateOn())
	{
	   %text = "ȷ��ѡ�񡾻�ϵ����Ϊ����"@%MainSeriestxt @"����ϵ��?" @ "<b/> <t c='0xff0000ff'>(��ʾ:��Ҳ����ѡ���������м���ϵ,ѡ���󽫲��ܸı�" @ %MainSeriestxt @ "����ϵ)</t>";
	   $SkillSeriesId =2;
	   OpenSkillSeriesSelectDialog(%text);
	}
	else if(SkillSelectBtn_TU.IsStateOn())
	{
	   %text = "ȷ��ѡ����ϵ����Ϊ����"@%MainSeriestxt @"����ϵ��?" @ "<b/> <t c='0xff0000ff'>(��ʾ:��Ҳ����ѡ���������м���ϵ,ѡ���󽫲��ܸı�" @ %MainSeriestxt @ "����ϵ)</t>";
	   $SkillSeriesId=3;
	   OpenSkillSeriesSelectDialog(%text);
	}
	else if(SkillSelectBtn_JIN.IsStateOn())
	{
	   %text = "ȷ��ѡ�񡾽�ϵ����Ϊ����"@%MainSeriestxt @"����ϵ��?" @ "<b/> <t c='0xff0000ff'>(��ʾ:��Ҳ����ѡ���������м���ϵ,ѡ���󽫲��ܸı�" @ %MainSeriestxt @ "����ϵ)</t>";
	   $SkillSeriesId = 4;
	   OpenSkillSeriesSelectDialog(%text);
	}
	else if(SkillSelectBtn_SHUI.IsStateOn())
	{
	   %text = "ȷ��ѡ��ˮϵ����Ϊ����"@%MainSeriestxt @"����ϵ��?" @ "<b/> <t c='0xff0000ff'>(��ʾ:��Ҳ����ѡ���������м���ϵ,ѡ���󽫲��ܸı�" @ %MainSeriestxt @ "����ϵ)</t>";
	   $SkillSeriesId =5;
	   OpenSkillSeriesSelectDialog(%text);
	}
	else
	{
		
	}
}

function SkillSeriesSelectedOk()
{
	SetClasses($IsMainSeries,$SkillSeriesId);
	SkillSelectBtn_Sure.setActive(0);
}

function OpenSkillSeriesSelectDialog(%text)
{
	  OpenDialog_OK_Cancel();		
		Dialog_OK_Cancel.SetContent(%text);
		Dialog_OK_Cancel.setOk("ȷ��", "SkillSeriesSelectedOk();");
		Dialog_OK_Cancel.SetCancel("ȡ��", "");
}

function CloseSkillSelect()
{
	if(SkillSelectWndGui.IsVisible())
	{
	  SkillSelectWndGui.setVisible(0);
	  $IsDirty--;
  }
}



//==========================================================
//============����ѧϰ======================================
//==========================================================

$StudyMainID=0;
$StudyAssistID=0;
$StudySelectIndex = 0;
function  OpenSkillStudyWnd(%MainID,%AssistID,%Series_3,%Series_4,%Series_5)
{
	$StudyMainID = %MainID;
	$StudyAssistID= %AssistID;
	if($StudyMainID!=0)  //��ϵ
	{
		switch(%MainID)
		{
		 case 1:SkillStudyBtn_Main.setText("ľϵ����");
		 case 2:SkillStudyBtn_Main.setText("��ϵ����");
		 case 3:SkillStudyBtn_Main.setText("��ϵ����");
		 case 4:SkillStudyBtn_Main.setText("��ϵ����");
		 case 5:SkillStudyBtn_Main.setText("ˮϵ����");
		}
		GetSkillSeriesInfo(%MainID,0);
		SkillStudyBtn_Main.setStateOn(1);
		$StudySelectIndex = 0;
		SkillStudyShortcutSelectIndex($StudySelectIndex);
	}
	
	 if(%AssistID == 0) //��ϵ
		{
			SkillStudyBtn_Assist.setVisible(0);
		}
		else
		{
			SkillStudyBtn_Assist.setVisible(1);
		  switch(%AssistID)
		  {
		   case 1:SkillStudyBtn_Assist.setText("ľϵ����");
		   case 2:SkillStudyBtn_Assist.setText("��ϵ����");
		   case 3:SkillStudyBtn_Assist.setText("��ϵ����");
		   case 4:SkillStudyBtn_Assist.setText("��ϵ����");
		   case 5:SkillStudyBtn_Assist.setText("ˮϵ����");
		  }
    }
    GameMainWndGui_Layer3.PushToBack(SkillStudyWndGui);
    SkillStudyWndGui.setVisible(1);
	  $IsDirty++;		
 }

function  SkillMainSeries()
{
	GetSkillSeriesInfo($StudyMainID,0);
	SkillStudyTxt_MainSeries.settext("��ϵ����");
	SkillStudyShortcutSelectIndex($StudySelectIndex);
}

function  SkillAssistSeries()
{
	GetSkillSeriesInfo($StudyAssistID,1);
	SkillStudyTxt_MainSeries.settext("��ϵ����");
	SkillStudyShortcutSelectIndex($StudySelectIndex);
}

function  RecvStudySkillInfo(%Icon,%SkillName,%SkillLevel,%NeedLevel,%NeedExp,%money,%Desc)
{	
	
	SkillStudyBitmap_Current.setBitmap("gameres/data/icon/skill/"@ %Icon);
	SkillStudyTxt_SkillName.settext(%SkillName);
	SkillStudyTxt_SkillLevel.settext(%SkillLevel @ "��");
	SkillStudyNeedLevelTxt.settext(%NeedLevel @ "��");
	SkillStudyNeedExpTxt.settext(%NeedExp);
  SkillStudyTxt_SkillDesc.setContent(%Desc);
  
    //�ж��Ƿ�����ѧϰ������
  	%player =getPlayer();
  	if(%NeedLevel <= %player.getLevel() && %NeedExp <= %player.getExp() && %money <= (%player.GetMoney() + %player.getMoney(2)))
    {
      SkillStudyBtn_Study.setActive(1);
      SkillStudyTxt_SkillName.setColor(255,255,255);
    }
    else
    {
    	SkillStudyBtn_Study.setActive(0);
    	SkillStudyTxt_SkillName.setColor(255,0,0);
    }

  if(%NeedExp > %player.getExp())
  {
    SkillStudyNeedExpTxt.setColor(255,0,0);
  }
  else
  {
    SkillStudyNeedExpTxt.setColor(255,255,255);
  }
  
  if(%NeedLevel > %player.getLevel())
     SkillStudyNeedLevelTxt.setColor(255,0,0);
  else
     SkillStudyNeedLevelTxt.setColor(255,255,255);
     
  
    
  
  if(%money >= 10000)
	{
		%gold = mfloor(%money / 10000);
	  %silver	=mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  SkillStudyNeedMoneyTxt_Gold.settext(%gold);
	  SkillStudyNeedMoneyTxt_Silver.settext(%silver);
	  SkillStudyNeedMoneyTxt_Copper.settext(%copper);
	  
	  SkillStudyNeedMoneyBitmap_Gold.setVisible(1);
	  SkillStudyNeedMoneyBitmap_Silver.setVisible(1);
	}
	else if(%money < 100)
	{
		%copper = %money;
		SkillStudyNeedMoneyTxt_Gold.settext("");
	  SkillStudyNeedMoneyTxt_Silver.settext("");
	  SkillStudyNeedMoneyTxt_Copper.settext(%copper);
	  
	  SkillStudyNeedMoneyBitmap_Gold.setVisible(0);
	  SkillStudyNeedMoneyBitmap_Silver.setVisible(0);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
		SkillStudyNeedMoneyTxt_Gold.settext("");
	  SkillStudyNeedMoneyTxt_Silver.settext(%silver);
	  SkillStudyNeedMoneyTxt_Copper.settext(%copper);
	  
	  SkillStudyNeedMoneyBitmap_Gold.setVisible(0);
	  SkillStudyNeedMoneyBitmap_Silver.setVisible(1);
	}
	
	if(%money > (%player.GetMoney() + %player.getMoney(2)))
  {
  	SkillStudyNeedMoneyTxt_Gold.setColor(255,0,0);
  	SkillStudyNeedMoneyTxt_Silver.setColor(255,0,0);
  	SkillStudyNeedMoneyTxt_Copper.setColor(255,0,0);
  }
  else
  {
  	SkillStudyNeedMoneyTxt_Gold.setColor(255,255,255);
  	SkillStudyNeedMoneyTxt_Silver.setColor(255,255,255);
  	SkillStudyNeedMoneyTxt_Copper.setColor(255,255,255);
  }  
}


function RefreshPlayerSkillMoney(%gold,%silver,%copper)
{
 	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		SkillStudyHavaMoneyBitmap_Gold.setVisible(0);
		SkillStudyHavaMoneyBitmap_Silver.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		SkillStudyHavaMoneyBitmap_Gold.setVisible(0);
		SkillStudyHavaMoneyBitmap_Silver.setVisible(1);
	}
	else
	{
	  SkillStudyHavaMoneyBitmap_Gold.setVisible(1);
		SkillStudyHavaMoneyBitmap_Silver.setVisible(1);
  }
  SkillStudyHavaMoneyTxt_Gold.settext(%gold);
	SkillStudyHavaMoneyTxt_Silver.settext(%silver);
	SkillStudyHavaMoneyTxt_Copper.settext(%copper);
}

function RefreshPlayerSkillBindMoney(%gold,%silver,%copper)
{
 	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		SkillStudy_BindGoldBitmap.setVisible(0);
		SkillStudy_BindSliverBitmap.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		SkillStudy_BindGoldBitmap.setVisible(0);
		SkillStudy_BindSliverBitmap.setVisible(1);
	}
	else
	{
	  SkillStudy_BindGoldBitmap.setVisible(1);
		SkillStudy_BindSliverBitmap.setVisible(1);
  }
   
   SkillStudy_BindGold.settext(%gold);
	 SkillStudy_BindSliver.setText(%silver);
	 SkillStudy_BindCopper.setText(%copper);
}

function SkillStudyShortcutSelectIndex(%index)
{
	$StudySelectIndex = %index;
  ("SkillStudySelectBtn_"@ %index).setStateOn(1);
  %SkillId = ("SkillStudyShortCut_" @ %index).getShortcutStudySkillID();
  GetStudySkillInfo(%SkillId);
}

function SkillStudyShowType()
{
	if(SkillStudyMenu.getSelected() == 1)
	   ShowCanStudySkill();
	else
	   ShowAllSkill();
}

function ShowCanStudySkill()
{
	%player =getPlayer();
	
	%Count =SkillStudyList.rowCount();
	for(%i=%j=0; %i<%Count; %i++)
	{		
		%text = SkillStudyList.getRowText(%j);
  	if(getWord(%text,2) > %player.getLevel() || getWord(%text,4) > %player.getExp() || getWord(%text,5) > %player.GetMoney() )
    {
      SkillStudyList.RemoveRow(%j);    
    }
    else
      %j++;         
	}
}

function ShowAllSkill()
{

	if(SkillStudyBtn_Main.IsStateOn())
	{
		GetSkillStudyInfo($StudyMainID);
	}
	else
	  GetSkillStudyInfo($StudyAssistID);
	SkillStudyList.sortID(true);
	SkillStudyList.setSelectedRow($SkillStudySelectNum);
	SkillStudyListSelectedOne();
}

function GetSkillStudyShortcutSelectIndex()
{
	for(%i = 0; %i<8;%i++)
	{
		if(("SkillStudySelectBtn_"@ %i).IsStateOn())
		    return %i;
	}
	return -1;
}

function  SkillStudy()
{
	%index = GetSkillStudyShortcutSelectIndex();
	if(%index != -1)
	{
		StudySkill(("SkillStudyShortCut_" @ %index).getShortcutStudySkillID());		 
	}
}

function UpdataSkillStudyInfo()
{
  SkillStudyShortcutSelectIndex($StudySelectIndex);
}

function CloseSkillStudy()
{
	if(SkillStudyWndGui.IsVisible())
	{
	  SkillStudyWndGui.SetVisible(0);
	  $IsDirty--;
  }
}



//===========================================================================================================
// �ĵ�˵��:������ʾ������
// ����ʱ��:2009-12-10
// ������: Batcel
//=========================================================================================================== 

function OpenSkillHintGui(%visible)
{
	if(SkillHintWndGui.IsVisible() != %visible)
	{
		SkillHintWndGui.setVisible(%visible);
	}
}

function SetSkillHintShortcutIndex(%index)
{
	for(%i = 0; %i<4;%i++)
	{
		("SkillHintShortCut_" @ %i).setSlotCol(%index+%i); 
	}
}

