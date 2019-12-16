//===========================================================================================================
// 文档说明:元神操作
// 创建时间:2010-1-20
// 创建人: Batcel
//===========================================================================================================
$SoulGeniusWnd_SpiritSlot = 0;
$SoulGeniusWnd_Face = 1;

function OpenSoulGeniusWnd()
{
	if(!SoulGeniusWndGui.IsVisible())
	{
		GameMainWndGui_Layer3.PushToBack(SoulGeniusWndGui);
		SoulGeniusWndGui.setVisible(1);
		SoulGenius_RefreshWnd();
		$IsDirty++;
	}
}

function CloseSoulGeniusWnd()
{
	if(SoulGeniusWndGui.IsVisible())
	{
		SoulGeniusWndGui.setVisible(0);
		$IsDirty--;
	}
}

function SelectSoulGeniusFace(%face)
{
	$SoulGeniusWnd_Face = %face;
	if(%face == 1)
	{
		SoulGeniusFace_1.setVisible(1);	
		SoulGeniusFace_2.setVisible(0);
	}
	else
	{
		SoulGeniusFace_1.setVisible(0);
		SoulGeniusFace_2.setVisible(1);
	}
}

//选中元神槽
function SoulGenius_SelectSpiritSlot(%nSlot)
{
	%bOpened = SpiritOperationManager_IsSpiritOpened(%nSlot);
	if (%bOpened)
	{
		$SoulGeniusWnd_SpiritSlot = %nSlot;
		SoulGenius_RefreshWnd();
	}
	else
	{
		//SpiritOperationManager_OpenSpiritSlot(%nSlot);		//打开元神栏
	}
}

//刷新元神主界面
function SoulGenius_RefreshWnd()
{
	if(!SoulGeniusWndGui.IsVisible())
		return;
	
	if ( !SpiritOperationManager_IsSpiritExist($SoulGeniusWnd_SpiritSlot) )
	{
		SoulGenius_ClearWnd();
		return;	
	}
	SoulGenius_RefreshMainWnd();
	SoulGenius_RefreshMainBtns();
	
	//刷新元神子窗口
	SoulGenius_RefreshFaceWnd_1();
	SoulGenius_RefreshFaceWnd_2();
}

//刷新元神主界面按钮
function SoulGenius_RefreshMainBtns()
{
	//元神槽的各个bitmap
	for (%i = 0; %i < 5; %i++)
	{
		%obj = "SoulGenius_SpiritSlot_Image_" @ %i;
		%obj.setVisible(!SpiritOperationManager_IsSpiritOpened(%i));
	}
	
	SoulGenius_RemoveSpiritBtn.setActive(SpiritOperationManager_CanRemoveSpirit($SoulGeniusWnd_SpiritSlot));
	SoulGenius_LockSpiritBtn.setActive(0);
	SoulGenius_ActiveSpiritBtn.setActive(SpiritOperationManager_CanActiviateSpirit($SoulGeniusWnd_SpiritSlot));
	SoulGenius_HuanHuaSpiritBtn.setActive();
	
	%bActive = SpiritOperationManager_IsSpiritActive($SoulGeniusWnd_SpiritSlot);
	%bHuanHua = SpiritOperationManager_IsSpiritHuanHua($SoulGeniusWnd_SpiritSlot);
	if (%bActive)
	{
		if (%bHuanHua)
		{
			SoulGenius_HuanHuaSpiritBtn.setVisible(0);
			SoulGenius_CancelHuanHuaSpiritBtn.setVisible(1);
			SoulGenius_CancelHuanHuaSpiritBtn.setActive(1);
		}
		else
		{
			SoulGenius_HuanHuaSpiritBtn.setVisible(1);
			SoulGenius_HuanHuaSpiritBtn.setActive(1);
			SoulGenius_CancelHuanHuaSpiritBtn.setVisible(0);			
		}
	}
	else
	{
		SoulGenius_HuanHuaSpiritBtn.setVisible(1);
		SoulGenius_HuanHuaSpiritBtn.setActive(0);
		SoulGenius_CancelHuanHuaSpiritBtn.setVisible(0);
	}
}

//刷新元神天赋主窗口内容(不包括按钮)
function SoulGenius_RefreshMainWnd()
{
	//元神种族
	//宠物种族图片选择		PetInfoWnd_PetRace
	%nRace = PetOperation_GetPetRace($PetInfoWnd_PetSlot);
	switch(%nRace)
	{
	case 0:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_030.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("圣");
		break;	
	case 1:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_031.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("佛");
		break;
	case 2:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_032.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("仙");
		break;
	case 3:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_033.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("精");
		break;
	case 4:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_034.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("鬼");
		break;
	case 5:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_035.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("怪");
		break;
	case 6:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_036.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("妖");
		break;
	case 7:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_037.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("魔");
		break;
	case 8:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow35_1_049.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("神");
		break;
	default:
		SoulGenius_SpiritRace_Img.setVisible(0);
		break;
	}
	
	SoulGenius_SpiritLevel.setText(SpiritOperationManager_GetSpiritLevel($SoulGeniusWnd_SpiritSlot));
	%nCurExp = SpiritOperationManager_GetSpiritCurrExp($SoulGeniusWnd_SpiritSlot);
	%nMaxExp = SpiritOperationManager_GetSpiritMaxExp($SoulGeniusWnd_SpiritSlot);
	if (%nMaxExp == 0)
	{
		SoulGenius_ExpText.setText("0/0");
		SoulGenius_ExpPercent.setPercentum("0");
	}
	else
	{
		SoulGenius_ExpText.setText(%nCurExp @ "/" @ %nMaxExp);
		SoulGenius_ExpPercent.setPercentum(%nCurExp / %nMaxExp);
	}
}

//刷新元神天赋子窗口
function SoulGenius_RefreshFaceWnd_1()
{
	SoulGenius_TalentLimitPts.setText("没做");
	SoulGenius_TalentPts.setText(SpiritOperationManager_GetTalentPts($SoulGeniusWnd_SpiritSlot));
	SoulGenius_TalentLeftPts.setText(SpiritOperationManager_GetTalentLeftPts($SoulGeniusWnd_SpiritSlot));
	//SoulGenius_TalentLimitStrengthenBtn


	//获取元神天赋技能底图（与种族有关）
	%nSpiritRace = SpiritOperationManager_GetSpiritType($SoulGeniusWnd_SpiritSlot);
	switch(%nSpiritRace)
	{
	case 0:		//圣
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_020.png");
		break;
	case 1:		//佛
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_022.png");
		break;
	case 2:		//仙
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_021.png");
		break;
	case 3:		//精
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_024.png");
		break;
	case 4:		//鬼
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_026.png");
		break;
	case 5:		//怪
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_023.png");
		break;
	case 6:		//妖
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_025.png");
		break;
	case 7:		//魔
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_027.png");
		break;
	case 8:		//神
		//SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_002.png");
		break;
	default:
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_002.png");
		break;
	}
	
	
	//获取元神天赋技能底图（与天赋树有关）
	%bmpBackground = SpiritOperationManager_GetTalentBackground($SoulGeniusWnd_SpiritSlot);
	SoulGeniusWnd_TalentSkillBackImg.setBitmap("gameres/gui/images/" @ %bmpBackground);
	SoulGeniusWnd_TalentSkillBackImg.setVisible(1);
	
	//显示所有技能slot及text控件，并隐藏多余的控件
	for(%i = 0; %i < 36; %i++)
	{
		%slotObj = "SoulGenius_TalentSkillSlot_" @ %i;
		%txtObj  = "SoulGenius_TalentPtsText_" @ %i;
		
		%nMaxLevel = SpiritOperationManager_GetTalentMaxLevel($SoulGeniusWnd_SpiritSlot, %i);
		if (%nMaxLevel != 0)
		{
			%nLevel = SpiritOperationManager_GetTalentLevel($SoulGeniusWnd_SpiritSlot, %i);
			%txtObj.setText(%nLevel @ "/" @ %nMaxLevel);
			%slotObj.setVisible(1);
			%txtObj.setVisible(1);			
		}
		else
		{
			%slotObj.setVisible(0);
			%txtObj.setVisible(0);
		}
	}
}

//刷新元神绝招子窗口
function SoulGenius_RefreshFaceWnd_2()
{
	//SoulGenius_JueZhao_OpenSlotBtn
	for (%i = 0; %i < 6; %i++)
	{
		%obj = "SoulGenius_JueZhao_Image_" @ %i;		
		%obj.setVisible(!SpiritOperationManager_IsJueZhaoOpened($SoulGeniusWnd_SpiritSlot, %i));
	}	
}

//清空元神主界面
function SoulGenius_ClearWnd()
{
	SoulGenius_SpiritRace_Img.setVisible(0);
	SoulGenius_SpiritType.setText("无");
	SoulGenius_SpiritLevel.setText("0级");
	SoulGenius_ExpPercent.setPercentum("0");
	SoulGenius_ExpText.setText("0/0");
	
	SoulGenius_ClearMainBtns();
	SoulGenius_ClearFaceWnd_1();
	SoulGenius_ClearFaceWnd_2();
}

//清空元神天赋主窗口按钮及Image
function SoulGenius_ClearMainBtns()
{
	//元神槽的各个bitmap
	for (%i = 0; %i < 5; %i++)
	{
		%obj = "SoulGenius_SpiritSlot_Image_" @ %i;
		%obj.setVisible(!SpiritOperationManager_IsSpiritOpened(%i));
	}
	
	SoulGenius_RemoveSpiritBtn.setActive(0);
	SoulGenius_LockSpiritBtn.setActive(0);
	SoulGenius_ActiveSpiritBtn.setActive(0);
	SoulGenius_HuanHuaSpiritBtn.setActive(0);
	SoulGenius_HuanHuaSpiritBtn.setVisible(1);
	SoulGenius_CancelHuanHuaSpiritBtn.setVisible(0);
}

//清空元神天赋子窗口
function SoulGenius_ClearFaceWnd_1()
{
	SoulGenius_TalentLimitPts.setText("0");
	SoulGenius_TalentPts.setText("0");
	SoulGenius_TalentLeftPts.setText("0");
	SoulGenius_TalentLimitStrengthenBtn.setActive(0);
	SoulGeniusWnd_TalentSkillBackImg.setVisible(0);		//天赋技能的底图不可见
	SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_002.png");
}

//清空元神绝招子窗口
function SoulGenius_ClearFaceWnd_2()
{
	SoulGenius_JueZhao_OpenSlotBtn.setActive(0);
	for (%i = 0; %i < 6; %i++)
	{
		%obj = "SoulGenius_JueZhao_Image_" @ %i;		
		%obj.setVisible(0);
	}	
}

//丢弃元神
function SoulGenius_RemoveSpirit(%nSlot)
{
	SpiritOperationManager_DeleteSpirit($SoulGeniusWnd_SpiritSlot);
}

//启用元神
function SoulGenius_ActiveSpirit(%nSlot)
{
	SpiritOperationManager_ActiviateSpirit($SoulGeniusWnd_SpiritSlot);
}

//幻化元神
function SoulGenius_HuanHuaSpirit(%nSlot)
{
	SpiritOperationManager_HuanHuaSpirit($SoulGeniusWnd_SpiritSlot);
	SoulGenius_HuanHuaSpiritBtn.setActive(0);
}

//取消幻化元神
function SoulGenius_CancelHuanHuaSpirit(%nSlot)
{
	SpiritOperationManager_CancelHuanHuaSpirit($SoulGeniusWnd_SpiritSlot);
	SoulGenius_CancelHuanHuaSpiritBtn.setActive(0);
}

//锁定元神
function SoulGenius_LockSpirit(%nSlot)
{
	
}

//绝招栏解封
function SoulGeniusWnd_OpenJueZhaoSlot()
{
	
}

//响应打开元神栏
function OnOpenSpiritSlotSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshWnd();
}

function OnOpenSpiritSlotFailure(%nSpiritSlot)
{
	
}

//响应打开元神绝招栏
function OnOpenJueZhaoSlotSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshWnd();
}

function OnOpenJueZhaoSlotFailure(%nSpiritSlot)
{
	
}

//响应添加元神
function OnAddSpiritSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshFaceWnd_2();
}

function OnAddSpiritFailure(%nSpiritSlot)
{
	
}

//响应删除元神
function OnRemoveSpiritSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshWnd();
}

function OnRemoveSpiritFailure(%nSpiritSlot)
{
	
}

//响应元神激活
function OnActivateSpiritSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshMainBtns();
}

function OnActivateSpiritFailure(%nSpiritSlot)
{
}

//响应元神幻化
function OnHuanHuaSpiritSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshMainBtns();
}

function OnHuanHuaSpiritFailure(%nSpiritSlot)
{
}

//响应取消元神幻化
function OnCancelHuanHuaSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshMainBtns();
}

function OnCancelHuanHuaFailure(%nSpiritSlot)
{
}

//响应元神等级提升
function OnAddSpiritLevel(%nSpiritSlot, %nAddedLevel)
{
	if ($SoulGeniusWnd_SpiritSlot != %nSpiritSlot)
		return;
	if(!SoulGeniusWndGui.IsVisible())
		return;
	
	SoulGenius_RefreshMainWnd();
	//刷新剩余点数
	SoulGenius_TalentPts.setText(SpiritOperationManager_GetTalentPts($SoulGeniusWnd_SpiritSlot));
	SoulGenius_TalentLeftPts.setText(SpiritOperationManager_GetTalentLeftPts($SoulGeniusWnd_SpiritSlot));
}

//响应元神经验提升
function OnAddSpiritExp(%nSpiritSlot, %nAddedExp)
{
	if ($SoulGeniusWnd_SpiritSlot != %nSpiritSlot)
		return;
	if(!SoulGeniusWndGui.IsVisible())
		return;
	
	SoulGenius_RefreshMainWnd();
	//刷新剩余点数
	SoulGenius_TalentPts.setText(SpiritOperationManager_GetTalentPts($SoulGeniusWnd_SpiritSlot));
	SoulGenius_TalentLeftPts.setText(SpiritOperationManager_GetTalentLeftPts($SoulGeniusWnd_SpiritSlot));
}

//响应元神天赋等级提升
function OnAddSpiritTalentLevel(%nSpiritSlot, %nAddedLevel)
{
	if ($SoulGeniusWnd_SpiritSlot != %nSpiritSlot)
		return;
	if(!SoulGeniusWndGui.IsVisible())
		return;
	
	SoulGenius_RefreshFaceWnd_1();
}

function SoulGenius_AddTalentLevel(%nTalentIndex, %nAddLevel)
{
	SpiritOperationManager_AddTalentLevel($SoulGeniusWnd_SpiritSlot, %nTalentIndex, %nAddLevel);
}

/////////////////////////////////////元神绝招界面相关脚本/////////////////////////////
$SpiritJueZhaoWnd_SpiritSlot = 0;
$SpiritJueZhaoWnd_SkillSlot = 0;

//打开绝招界面
function SpiritJueZhaoWnd_OpenSpiritSkill()
{
	if(!SpiritJueZhaoWnd.IsVisible())
	{
		GameMainWndGui_Layer3.PushToBack(SpiritJueZhaoWnd);
		SpiritJueZhaoWnd.setVisible(1);
		//刷新元神槽
		for (%i = 0; %i < 5; %i++)
		{
			%obj = "SpiritJueZhaoWnd_SpiritSlot_" @ %i;		
			//%obj.setVisible(SpiritOperationManager_IsSpiritExist(%i));		//暂时去掉方便测试
		}
		SpiritJueZhaoWnd_RefreshWnd();
		%SlotObj = "SpiritJueZhaoWnd_SpiritSlot_0";
		%SlotObj.setBeSelected(1);
		%SlotObj = "SpiritJueZhaoWnd_Slot_0";
		%SlotObj.setBeSelected(1);
	}
}

//关闭绝招界面
function SpiritJueZhaoWnd_CloseSpiritSkill()
{
	$SpiritJueZhaoWnd_SpiritSlot = 0;
	$SpiritJueZhaoWnd_SkillSlot = 0;
	SpiritJueZhaoWnd.setVisible(0);
}

//选中元神槽
function SpiritJueZhaoWnd_SelectSpiritSlot(%nSlot)
{
	if ($SpiritJueZhaoWnd_SpiritSlot == %nSlot)
		return;
	%bOpened = SpiritOperationManager_IsSpiritOpened(%nSlot);
	if (!%bOpened)
		return;
		
	%SlotObj = "SpiritJueZhaoWnd_SpiritSlot_" @ $SpiritJueZhaoWnd_SpiritSlot;
	%SlotObj.setBeSelected(0);
	$SpiritJueZhaoWnd_SpiritSlot = %nSlot;
	%SlotObj = "SpiritJueZhaoWnd_SpiritSlot_" @ $SpiritJueZhaoWnd_SpiritSlot;
	%SlotObj.setBeSelected(1);
	
	SpiritJueZhaoWnd_SelectSkillSlot(0);
	SpiritJueZhaoWnd_RefreshWnd();
}

//选中元神技能槽
function SpiritJueZhaoWnd_SelectSkillSlot(%nSlot)
{
	%SlotObj = "SpiritJueZhaoWnd_Slot_" @ $SpiritJueZhaoWnd_SkillSlot;
	%SlotObj.setBeSelected(0);
	$SpiritJueZhaoWnd_SkillSlot = %nSlot;
	%SlotObj = "SpiritJueZhaoWnd_Slot_" @ $SpiritJueZhaoWnd_SkillSlot;
	%SlotObj.setBeSelected(1);
}

function SpiritJueZhaoWnd_RefreshWnd()
{
	//刷新技能图片
	for (%i = 0; %i < 6; %i++)
	{
		%obj = "SpiritJueZhaoWnd_Image_" @ %i;
		%bOpened = SpiritOperationManager_IsJueZhaoOpened($SpiritJueZhaoWnd_SpiritSlot, %i);
		%obj.setVisible(!%bOpened);
	}
	
	//刷新按钮
	
}