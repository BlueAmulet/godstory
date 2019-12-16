//===========================================================================================================
// 文档说明:生活技能操作
// 创建时间:2009-9-11
// 创建人: Batcel
//=========================================================================================================== 
//显示生活技能
function UI_DisplayLivingSkill(%index)
{
  $LivingCurrentSelectSeries = %index;
  for(%i=1;%i<10;%i++)
  {
  	("LivingSkillInfo_" @ %i) .setVisible(0);
  }
  if(%index ==0)
  {
  	ShortCut_LivingSkill_0.setVisible(0);
  	//Bitmap_LivingSkill_0.setVisible(0);
  	Bitmap_LivingSkill_Blackground.setbitmap($GUI::GUISkinDirectory @ "/GUIWindow32_1_066.png");
  }
  else
  {
  	ShortCut_LivingSkill_0.setVisible(1);
  	//Bitmap_LivingSkill_0.setVisible(1);     
  	Bitmap_LivingSkill_Blackground.setbitmap($GUI::GUISkinDirectory @ "/GUIWindow32_1_054.png");
  }	 
  
  DisplayLivingSkill(%index);
  
  %player = GetPlayer();
  if(%player == 0)
     return;
  
  %CurRipe = %player.getLivingExp();
  %MaxRipe = %player.getLivingCurrentLevelExp();
  if(%MaxRipe == 0) 
     %MaxRipe=1;
  
	LivingSkillInfoExpBar.setExtent((%CurRipe*125)/%MaxRipe,12);
	LivingSkillInfoExpText.setText(%CurRipe @ "/" @ %MaxRipe);
	
	%LivingLevel = %player.getLivingLevel();
	if(%LivingLevel == 0)
	   LivingSkillInfoLevelText.setText("");
	else
	   LivingSkillInfoLevelText.setText(%LivingLevel @ "级");
}

//显示生活技能信息
function UI_ShowLivingSkillInfo(%Series,%index,%name,%level,%CurRipe,%MaxRipe)
{	
	if($LivingCurrentSelectSeries != %Series || %index == 0)
	   return;
	   
	("LivingSkillInfo_" @ %index) .setVisible(1); 
  if(%level>0)
  {
  	("LivingSkillInfoName_" @ %index).setText(%name);
    ("LivingSkillInfoLevel_" @ %index).setText(%level @ "级");
    ("LivingSkillInfoLevel_" @ %index).setColor(255,255,255);
    ("LivingSkillInfoRipe_" @ %index).setVisible(1);
    ("LivingSkillInfoRipe_" @ %index).setText(%CurRipe @ "/" @ %MaxRipe);
    ("Bitmap_LivingSkill_Background_" @ %index).setbitmap("gameres/gui/images/GUIWindow32_1_046.png");
  }
  else
  {                                                     
    ("LivingSkillInfoName_" @ %index).setText(%name);                                                  
    ("LivingSkillInfoLevel_" @ %index).setText("未学");                                                
    ("LivingSkillInfoLevel_" @ %index).setColor(255,0,0);                                              
    ("LivingSkillInfoRipe_" @ %index).setVisible(0);                                                   
    ("Bitmap_LivingSkill_Background_" @ %index).setbitmap("gameres/gui/images/GUIWindow32_1_047.png"); 
  }
		UI_UpdateRipeInfo(%index,%CurRipe,%MaxRipe);
}

//更新对应位置生活技能熟练度
function UI_UpdateRipeInfo(%index,%CurRipe,%MaxRipe)
{
	if(%MaxRipe == 0) %MaxRipe=1;
	("LivingSkillInfoRipeBar_" @ %index).setExtent((%CurRipe*80)/%MaxRipe,12);
	("LivingSkillInfoRipe_" @ %index).setText(%CurRipe @ "/" @ %MaxRipe);
}

//根据生活技能ID, 更新熟练度
function UI_UpdateRipeInfoByID(%LivingSkillID,%CurRipe,%MaxRipe)
{
	if(!LivingSkillWndGui.isVisible())
	   return;
	
	if(%LivingSkillID < 501000000) // 生活技能起始ID
	   return;
	   
	%cur = %CurRipe;
	%max = %MaxRipe;
	
	for(%i=1; %i<10; %i++)
	{
		%id = ("ShortCut_LivingSkill_"@%i).getShortcutLivingSkillID();
		if(%id == %LivingSkillID)
		   UI_UpdateRipeInfo(%i,%cur,%max);
	}
}



//===========================================================================================================
// 文档说明:生活技能学习操作
// 创建时间:2009-9-11
// 创建人: Batcel
//=========================================================================================================== 

$LivingSkillSeries =0 ;
$LivingSkillSelectNum=0;
function  OpenStudyLivingSkillWnd(%Series)
{	
	LivingSkillStudyList.clear();
	$LivingSkillSeries = %Series;
	GetLivingSkillSeries(%Series);
	LivingSkillStudy_Menu.clear();
	LivingSkillStudy_Menu.add("可学",1);
	LivingSkillStudy_Menu.add("全部",2);
	LivingSkillStudy_Menu.setSelected(2);
	
	GameMainWndGui_Layer3.pushToBack(LivingSkillStudyGui);
	LivingSkillStudyGui.setVisible(1);
	$IsDirty++;
	
	LivingSkillStudyList.setSelectedRow($LivingSkillSelectNum);
	LivingSkillStudyListSelectedOne();		
}

function CloseStudyLivingSkillWnd()
{
	if(LivingSkillStudyGui.IsVisible())
	{
    LivingSkillStudyGui.setVisible(0);
	  $IsDirty--;
	}
}

function  AddLivingSkillStudyList(%SkillId,%Name,%Ripe,%Level,%NeedExp,%NeedLevel,%NeedMoney,%NeedRipe,%Desc)
{    
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	  
	%color = 0xffffff;
	
	if(IsCanLearnLivingSkill(%SkillId) != 4002 && %NeedLevel <= %player.getLevel()  )
	 %color = 0xffffff;	
	else
	 %color = 0xff0000;
	LivingSkillStudyList.addRowTakeColor(%SkillId,%Name @ "(等级"@ %Level @")" TAB %Ripe TAB %NeedExp TAB %NeedLevel TAB %NeedMoney TAB %NeedRipe TAB %Desc,%color);	
}

function LivingSkillStudyListSelectedOne()
{
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	  
	%RowId=LivingSkillStudyList.getSelectedRow();
	$LivingSkillSelectNum = %RowId;
	%text = LivingSkillStudyList.getRowText(%RowId);
  %SkillId = LivingSkillStudyList.getRowId(%RowId);
  LivingSkillInfo_Desc.setContent(getWord(%text,6));
  %NeedExp =getWord(%text,2);
  LivingSkillInfo_NeedExp.settext(%NeedExp);
  LivingSkillInfo_Icon.setBitmap("gameres/data/icon/skill/"@ getLivingSkillData(%SkillId,7));
  %money=getWord(%text,4);
  %NeedLevel = getWord(%text,3);
  LivingSkillInfo_Level.settext(getLivingSkillData(%SkillId,5) @ "级");
  LivingSkillInfo_NeedplayerLevel.settext(%NeedLevel @ "级");  
  
  if(getLivingSkillData(%SkillId,4) == 1 && getLivingSkillData(%SkillId,5) == 1)
  {
  	  LivingSkillInfo_NeedRipe.setVisible(0);
  }
  else
  {
   LivingSkillInfo_NeedRipe.setVisible(1);
   LivingSkillInfo_NeedRipe.settext("需要" @ getLivingSkillData(getLivingSkillData(%SkillId,10),1) @ "熟练度" @ getWord(%text,5)); 
  }
  
    //判断是否满足学习的条件
  	if(IsCanLearnLivingSkill(%SkillId) != 4002 && %NeedLevel <= %player.getLevel() && %NeedExp <= %player.getExp() && %money <= (%player.GetMoney() + %player.getMoney(2)) &&IsCanLearnLivingSkill(%SkillId) != 4001 )
    {
      LivingSkillStudyBtn_Study.setActive(1);
    }
    else
    {
    	LivingSkillStudyBtn_Study.setActive(0);
    }
    
  if(IsCanLearnLivingSkill(%SkillId) != 4001 && IsCanLearnLivingSkill(%SkillId) != 4002)  
    LivingSkillInfo_NeedRipe.setColor(255,255,255);
  else
    LivingSkillInfo_NeedRipe.setColor(255,0,0);  
    
    
  if(%NeedLevel <= %player.getLevel())
     LivingSkillInfo_NeedplayerLevel.setColor(255,255,255);
  else
     LivingSkillInfo_NeedplayerLevel.setColor(255,0,0);
    
  if( %NeedExp <= %player.getExp())  
     LivingSkillInfo_NeedExp.setColor(255,255,255);
  else
     LivingSkillInfo_NeedExp.setColor(255,0,0);
    
  if(%money > (%player.GetMoney() + %player.getMoney(2)))
  {
  	LivingSkillStudyNeedMoneyTxt_Gold.setColor(255,0,0);
  	LivingSkillStudyNeedMoneyTxt_Silver.setColor(255,0,0);
  	LivingSkillStudyNeedMoneyTxt_Copper.setColor(255,0,0);
  }
  else
  {
  	LivingSkillStudyNeedMoneyTxt_Gold.setColor(255,255,255);
  	LivingSkillStudyNeedMoneyTxt_Silver.setColor(255,255,255);
  	LivingSkillStudyNeedMoneyTxt_Copper.setColor(255,255,255);
  }
  
  
  if(%money >= 10000)
	{
		%gold = mfloor(%money / 10000);
	  %silver	=mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  LivingSkillStudyNeedMoneyTxt_Gold.settext(%gold);
	  LivingSkillStudyNeedMoneyTxt_Silver.settext(%silver);
	  LivingSkillStudyNeedMoneyTxt_Copper.settext(%copper);
	  
	  LivingSkillStudyNeedMoneyBmp_Gold.setVisible(1);
	  LivingSkillStudyNeedMoneyBmp_Silver.setVisible(1);
	  LivingSkillStudyNeedMoneyBmp_Copper.setVisible(1);	  
	}
	else if(%money < 100)
	{
		%copper = %money;
		LivingSkillStudyNeedMoneyTxt_Gold.settext("");
	  LivingSkillStudyNeedMoneyTxt_Silver.settext("");
	  LivingSkillStudyNeedMoneyTxt_Copper.settext(%copper);
	  
	  
	  LivingSkillStudyNeedMoneyBmp_Gold.setVisible(0);
	  LivingSkillStudyNeedMoneyBmp_Silver.setVisible(0);
	  LivingSkillStudyNeedMoneyBmp_Copper.setVisible(1);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
		LivingSkillStudyNeedMoneyTxt_Gold.settext("");
	  LivingSkillStudyNeedMoneyTxt_Silver.settext(%silver);
	  LivingSkillStudyNeedMoneyTxt_Copper.settext(%copper);
	  
	  LivingSkillStudyNeedMoneyBmp_Gold.setVisible(0);
	  LivingSkillStudyNeedMoneyBmp_Silver.setVisible(1);
	  LivingSkillStudyNeedMoneyBmp_Copper.setVisible(1);
	}
}

function LivingSkillStudyShowType()
{
	if(LivingSkillStudy_Menu.getSelected() == 1)
	   ShowCanStudyLivingSkill();
	else
	   ShowAllLivingSkill();
}

function ShowCanStudyLivingSkill()
{
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	
	%Count =LivingSkillStudyList.rowCount();
	for(%i=%j=0; %i<%Count; %i++)
	{		
		%text = LivingSkillStudyList.getRowText(%j);
  	if(getWord(%text,3) > %player.getLevel() || getWord(%text,2) > %player.getExp() || IsCanLearnLivingSkill(LivingSkillStudyList.getRowId(%j)) == 4002)
    {
      LivingSkillStudyList.RemoveRow(%j);    
    }
    else
      %j++;         
	}
}

function ShowAllLivingSkill()
{
	LivingSkillStudyList.clear();
	GetLivingSkillSeries($LivingSkillSeries);

	LivingSkillStudyList.setSelectedRow($LivingSkillSelectNum);
	LivingSkillStudyListSelectedOne();
}


function LivingSkillStudy()
{
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	  
	if(LivingSkillStudyList.getSelectedRow()!=-1)
	{
		//LivingSkillStudyBtn_Study.setActive(1);
		LearnLivingSkill(%player,LivingSkillStudyList.getRowId(LivingSkillStudyList.getSelectedRow()));		
	}
}

function UpdataLivingSkillStudyList()
{
	LivingSkillStudyList.clear();
	GetLivingSkillSeries($LivingSkillSeries);
	LivingSkillStudyList.setSelectedRow($LivingSkillSelectNum);
	LivingSkillStudyListSelectedOne();
}


function RefreshPlayerLivingSkillMoney(%gold,%silver,%copper)
{
 	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		LivingSkillStudyHavaMoneyBmp_Gold.setVisible(0);
		LivingSkillStudyHavaMoneyBmp_Silver.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		LivingSkillStudyHavaMoneyBmp_Gold.setVisible(0);
		LivingSkillStudyHavaMoneyBmp_Silver.setVisible(1);
	}
	else
	{
	  LivingSkillStudyHavaMoneyBmp_Gold.setVisible(1);
		LivingSkillStudyHavaMoneyBmp_Silver.setVisible(1);
  }
  LivingSkillStudyHaveMoneyTxt_Gold.settext(%gold);
	LivingSkillStudyHaveMoneyTxt_Silver.settext(%silver);
	LivingSkillStudyHaveMoneyTxt_Copper.settext(%copper);
}

function RefreshPlayerLivingSkillBindMoney(%gold,%silver,%copper)
{
 	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		LivingSkillStudy_BindGoldBitmap.setVisible(0);
		LivingSkillStudy_BindSilverBitmap.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		LivingSkillStudy_BindGoldBitmap.setVisible(0);
		LivingSkillStudy_BindSilverBitmap.setVisible(1);
	}
	else
	{
	  LivingSkillStudy_BindGoldBitmap.setVisible(1);
		LivingSkillStudy_BindSilverBitmap.setVisible(1);
  }
   
   LivingSkillStudy_BindGold.settext(%gold);
	 LivingSkillStudy_BindSilver.setText(%silver);
	 LivingSkillStudy_BindCopper.setText(%copper);
}




//===========================================================================================================
// 文档说明:配方
// 创建时间:2009-9-11
// 创建人: Batcel
//=========================================================================================================== 
function InitPrescriptionColorLevl()
{
	Prescription_PopUpMenuCtl2.add("全部",0);
	Prescription_PopUpMenuCtl2.add("灰色",1);
	Prescription_PopUpMenuCtl2.add("白色",2);
	Prescription_PopUpMenuCtl2.add("绿色",3);
	Prescription_PopUpMenuCtl2.add("蓝色",4);
	Prescription_PopUpMenuCtl2.add("紫色",5);
	Prescription_PopUpMenuCtl2.add("橙色",6);
	Prescription_PopUpMenuCtl2.add("红色",7);
}

//打开配方界面
function UI_OpenPrescription(%tab)
{
	clearGUIData();
	
	//配方颜色
	if(Prescription_PopUpMenuCtl2.size()== 0)
	   InitPrescriptionColorLevl();
	Prescription_PopUpMenuCtl2.setSelected(0,false);
	
	%player = getPlayer();
	if(%player)//活力   
	{
		%curVigor = %player.getVigor();
		%maxVigor = %player.getMaxVigor();
		
  	if(%curVigor >= %maxVigor)
	     %ripeX = 120;
	  else
	     %ripeX = 120* %curVigor/%maxVigor;

	  Vigor_Bitmap_ctl.setExtent(%ripeX,14);
	  Vigor_Text_ctl.setText(%curVigor@ "/" @%maxVigor);
	}

	if(%tab>0 && %tab<9)
	{
	  GameMainWndGui_Layer3.pushToBack(LivingPrescriptionGui);
		LivingPrescriptionGui.setVisible(true);
		QueryPrescription(%tab);
		$LivingCurrentSelectSeries = %tab;
		$IsDirty++;
	}
}

function ClosePrescription()
{
	if(LivingPrescriptionGui.IsVisible())
	{
		LivingPrescriptionGui.setVisible(0);
		$IsDirty--;
	}
}

function Prescription_TreeCtl::onSelect(%this,%obj)
{
	eval(%this.getItemValue(%obj));
}
//完成制造后刷新
function uiUpdatePrescriptionItemInfo(%id)
{
	if(!LivingPrescriptionGui.isVisible())
	   return;
	   
	//树结构列表,如果选中的不是当前配方,则重新选中
	if(%id != $SelectPrescriptionID)
	{
		Prescription_TreeCtl.clearSelection();
	  Prescription_TreeCtl.selectItem($DoingTreeItemId,true);
	}
	
	//重新更新当前制造配方信息   
	QueryPrescriptionItemInfo(%id);
	
}

function clearMaterialItem()
{
	for(%i=1; %i<7; %i++)
	{
	   ("ShortCut_Prescription" @ %i).setVisible(0);
	   ("MaterialNumberTxt_" @ %i) .setVisible(0);
	   ("MaterialBitmap_" @ %i).setVisible(0);
	}
}
function clearGUIData()
{
	  InsightNeed_Value.settext("");
	  VigourNeed_Value.settext("");
		ProduceMoneyBitmap_Gold.setVisible(0);
	  ProduceMoneyBitmap_Silver.setVisible(0);
	  ProduceMoneyBitmap_Copper.setVisible(0);
	  ProduceNeedMoney_Gold.settext("");
	  ProduceNeedMoney_Silver.settext("");
	  ProduceNeedMoney_Copper.settext("");
	  clearMaterialItem();
	  ShortCut_Prescription0.setVisible(0);
	  ProduceItemTxt_Name.setText("");
    ProduceItemTxt_Des.setText("");
    ProduceMax_textCtl.setValue(0);
    PrescriptionNum_TextEdit.setText("0");
    Ripe_text_Ctl.setText("");
    $SelectPrescriptionID = 0;
    Prescription_TreeCtl.clearSelection();
}


function RefreshPrescriptionList(%Series)
{
	if($LivingCurrentSelectSeries == %Series && LivingPrescriptionGui.IsVisible())
    UI_OpenPrescription($LivingCurrentSelectSeries);
}

function displayMaterialInfo(%index,%resnum, %currCount)
{
	%Idx = %index + 1;   // 第一个为产物
	("ShortCut_Prescription" @ %Idx).setVisible(1);
	("MaterialNumberTxt_" @ %Idx) .setVisible(1);
	("MaterialBitmap_" @ %Idx).setVisible(1);
	
	("MaterialNumberTxt_" @ %Idx).setText(	%currCount @ "/" @ %resnum);
	if( %resnum <= %currCount )
	{
		("MaterialNumberTxt_" @ %Idx).setColor(255,255,255);
	}
	else
	{
		("MaterialNumberTxt_" @ %Idx).setColor(255,0,0);
	}
}



function PrescriptionInfo(%Insight,%Vigour,%money,%ItemName,%ItemDes,%Count,%SerialID)
{
	ShortCut_Prescription0.setVisible(1);
	$SelectPrescriptionID = %SerialID;
	ProduceMax_textCtl.setText(%count);
	PrescriptionNum_TextEdit.SetMaxNum(%count);
	if(%count>0)
	  PrescriptionNum_TextEdit.setText("1");
	else
	  PrescriptionNum_TextEdit.setText("0");
	InsightNeed_Value.setText(%Insight);
  VigourNeed_Value.setText(%Vigour);
  
  ProduceItemTxt_Name.setText(%ItemName);
  ProduceItemTxt_Des.setText(%ItemDes);
	if(%money >= 10000)
	{
		%gold = mfloor(%money / 10000);
	  %silver	=mfloor((%money - (%gold * 10000)) / 100);
	  %copper	= (%money - (%gold * 10000 + (%silver * 100)));
	  ProduceNeedMoney_Gold.settext(%gold);
	  ProduceNeedMoney_Silver.settext(%silver);
	  ProduceNeedMoney_Copper.settext(%copper);
	  
	  ProduceNeedMoney_Gold.setPosition(275,139);
	  ProduceNeedMoney_Silver.setPosition(305,139);
	  ProduceNeedMoney_Copper.setPosition(335,139);
	  
	  ProduceMoneyBitmap_Gold.setVisible(1);
	  ProduceMoneyBitmap_Silver.setVisible(1);
	  ProduceMoneyBitmap_Copper.setVisible(1);
	  
	  ProduceMoneyBitmap_Gold.setPosition(295,138);
	  ProduceMoneyBitmap_Silver.setPosition(325,138);
	  ProduceMoneyBitmap_Copper.setPosition(355,138);
	}
	else if(%money < 100)
	{
		%copper = %money;
		ProduceNeedMoney_Gold.settext("");
	  ProduceNeedMoney_Silver.settext("");
	  ProduceNeedMoney_Copper.settext(%copper);
	  ProduceNeedMoney_Copper.setPosition(270,139);
	  
	  ProduceMoneyBitmap_Gold.setVisible(0);
	  ProduceMoneyBitmap_Silver.setVisible(0);
	  ProduceMoneyBitmap_Copper.setVisible(1);
	  ProduceMoneyBitmap_Copper.setPosition(290,138);
	}
	else
	{
		%silver = mfloor(%money / 100);
		%copper = (%money - (%silver * 100));
		ProduceNeedMoney_Gold.settext("");
	  ProduceNeedMoney_Silver.settext(%silver);
	  ProduceNeedMoney_Copper.settext(%copper);
	  
	  ProduceNeedMoney_Silver.setPosition(270,139);
	  ProduceNeedMoney_Copper.setPosition(300,139);
	  
	  ProduceMoneyBitmap_Gold.setVisible(0);
	  ProduceMoneyBitmap_Silver.setVisible(1);
	  ProduceMoneyBitmap_Copper.setVisible(1);
	  ProduceMoneyBitmap_Silver.setPosition(290,138);
	  ProduceMoneyBitmap_Copper.setPosition(320,138);
	}
}

function PrescriptionProduce(%type)
{
	if(%type == 0)   //制造
	{
	   %num = PrescriptionNum_TextEdit.getValue();
	   if(%num > 0)
	   {	   	  
	   	  startProduce($SelectPrescriptionID,%num);
	   } 
	}

  if(%type == 1)//全部制造
  {
  	  %num = ProduceMax_textCtl.getValue();
  	  if(%num > 0)
  	  {  	  	 
  	  	 startProduce($SelectPrescriptionID,%num);
  	  }  
  }
  
  $DoingTreeItemId = Prescription_TreeCtl.getSelectedItem();
}

function setProduceButton(%bFlag)
{
	if(%bFlag)
	{
		  Button_Produce_Norm.setVisible(true);
	    Button_Produce_All.setVisible(true);
	    Button_produce_cancel.setVisible(false);
	    $DoingTreeItemId = 0;
	}
	else
	{
		  Button_Produce_Norm.setVisible(false);
	    Button_Produce_All.setVisible(false);
	    Button_produce_cancel.setVisible(true);
	}
}

function cancelProduce()
{
	//功能以后再写
	setProduceButton(true);
}

function RefreshProduceinfo(%curRipe,%maxRipe,%curVigor,%maxVigor,%skillName,%skillLevel,%count,%skillCategory)
{
	   //熟练度
	   if(%curRipe >= %maxRipe)
	   {
	     %ripeX = 324;
	     %curRipe = %maxRipe;
	   }
	   else
	   {
	     %ripeX = 324* %curRipe/%maxRipe;
	   }
	   Ripe_Bitmap_Ctl.setExtent(%ripeX,15);
	   Ripe_text_Ctl.setText(%skillName@"    "@%skillLevel@"级        "@%curRipe@"/"@%maxRipe);
	   //更新生活技能界面熟练度
	   if($LivingCurrentSelectSeries == %skillCategory)
	   {
	   	  for(%i =1; %i<10; %i++)
	   	  {
	   	  	%name = ("LivingSkillInfoName_"@%i).getValue();
	   	  	if(%name $= %skillName)
	   	  	{
	   	  	  ("LivingSkillInfoRipe_" @ %i).setText(%curRipe @ "/" @ %maxRipe);
	   	  	  UI_UpdateRipeInfo(%i,%curRipe,%maxRipe);
	   	  	}
	   	  }
	   }
	   //活力
	   if(%curVigor >= %maxVigor)
	     %ripeX = 120;
	   else
	     %ripeX = 120* %curVigor/%maxVigor;

	   Vigor_Bitmap_ctl.setExtent(%ripeX,14);
	   Vigor_Text_ctl.setText(%curVigor@ "/" @%maxVigor);
	   
	   //更新可制造总量
	   ProduceMax_textCtl.setValue(%count);
}

function AddOrSubProduceItemNum(%Val)
{
	%Number = %Val+PrescriptionNum_TextEdit.getText();
	%Num=mClamp(%Number,0,ProduceMax_textCtl.getValue());
	PrescriptionNum_TextEdit.setText(%Num);
}
//按配方类别过滤
function Prescription_PopUpMenuCtl::onSelect(%this,%Id,%text)
{    
	%ColorLevel = Prescription_PopUpMenuCtl2.getSelected();
	QueryPrescription($LivingCurrentSelectSeries,%text,%ColorLevel);  
}
//按配方颜色过滤
function Prescription_PopUpMenuCtl2::onSelect(%this,%Id,%text)
{
	%SubCategoryID = Prescription_PopUpMenuCtl.getSelected();
	%SubCategoryText = Prescription_PopUpMenuCtl.getTextById(%SubCategoryID);
	QueryPrescription($LivingCurrentSelectSeries,%SubCategoryText,%Id);
}

function RefreshPlayerProduceMoney(%gold,%silver,%copper)
{
 	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		ProduceHavaMoneyBmp_Gold.setVisible(0);
		ProduceHavaMoneyBmp_Silver.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		ProduceHavaMoneyBmp_Gold.setVisible(0);
		ProduceHavaMoneyBmp_Silver.setVisible(1);
	}
	else
	{
	  ProduceHavaMoneyBmp_Gold.setVisible(1);
		ProduceHavaMoneyBmp_Silver.setVisible(1);
  }
  ProduceHavaMoney_Gold.settext(%gold);
	ProduceHavaMoney_Silver.settext(%silver);
	ProduceHavaMoney_Copper.settext(%copper);
}

function RefreshPlayerProduceBindMoney(%gold,%silver,%copper)
{
 	if(%gold == 0 && %silver == 0)
	{
		%gold = "";
		%silver = "";
		ProduceRockImage_Gold.setVisible(0);
		ProduceRockImage_Silver.setVisible(0);
	}
	else if(%gold == 0 && %silver != 0)
	{
		%gold = "";
		ProduceRockImage_Gold.setVisible(0);
		ProduceRockImage_Silver.setVisible(1);
	}
	else
	{
	  ProduceRockImage_Gold.setVisible(1);
		ProduceRockImage_Silver.setVisible(1);
  }
   
   ProduceRockText_GoldText.settext(%gold);
	 ProduceRockText_SilverText.setText(%silver);
	 ProduceRockText_Copper.setText(%copper);
}