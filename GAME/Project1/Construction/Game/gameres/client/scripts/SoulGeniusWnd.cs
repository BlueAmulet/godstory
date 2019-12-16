//===========================================================================================================
// �ĵ�˵��:Ԫ�����
// ����ʱ��:2010-1-20
// ������: Batcel
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

//ѡ��Ԫ���
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
		//SpiritOperationManager_OpenSpiritSlot(%nSlot);		//��Ԫ����
	}
}

//ˢ��Ԫ��������
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
	
	//ˢ��Ԫ���Ӵ���
	SoulGenius_RefreshFaceWnd_1();
	SoulGenius_RefreshFaceWnd_2();
}

//ˢ��Ԫ�������水ť
function SoulGenius_RefreshMainBtns()
{
	//Ԫ��۵ĸ���bitmap
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

//ˢ��Ԫ���츳����������(��������ť)
function SoulGenius_RefreshMainWnd()
{
	//Ԫ������
	//��������ͼƬѡ��		PetInfoWnd_PetRace
	%nRace = PetOperation_GetPetRace($PetInfoWnd_PetSlot);
	switch(%nRace)
	{
	case 0:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_030.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("ʥ");
		break;	
	case 1:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_031.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("��");
		break;
	case 2:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_032.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("��");
		break;
	case 3:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_033.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("��");
		break;
	case 4:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_034.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("��");
		break;
	case 5:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_035.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("��");
		break;
	case 6:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_036.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("��");
		break;
	case 7:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow33_1_037.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("ħ");
		break;
	case 8:
		SoulGenius_SpiritRace_Img.setBitmap("gameres/gui/images/GUIWindow35_1_049.png");
		SoulGenius_SpiritRace_Img.setVisible(1);
		SoulGenius_SpiritType.setText("��");
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

//ˢ��Ԫ���츳�Ӵ���
function SoulGenius_RefreshFaceWnd_1()
{
	SoulGenius_TalentLimitPts.setText("û��");
	SoulGenius_TalentPts.setText(SpiritOperationManager_GetTalentPts($SoulGeniusWnd_SpiritSlot));
	SoulGenius_TalentLeftPts.setText(SpiritOperationManager_GetTalentLeftPts($SoulGeniusWnd_SpiritSlot));
	//SoulGenius_TalentLimitStrengthenBtn


	//��ȡԪ���츳���ܵ�ͼ���������йأ�
	%nSpiritRace = SpiritOperationManager_GetSpiritType($SoulGeniusWnd_SpiritSlot);
	switch(%nSpiritRace)
	{
	case 0:		//ʥ
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_020.png");
		break;
	case 1:		//��
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_022.png");
		break;
	case 2:		//��
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_021.png");
		break;
	case 3:		//��
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_024.png");
		break;
	case 4:		//��
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_026.png");
		break;
	case 5:		//��
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_023.png");
		break;
	case 6:		//��
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_025.png");
		break;
	case 7:		//ħ
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_027.png");
		break;
	case 8:		//��
		//SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_002.png");
		break;
	default:
		SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_002.png");
		break;
	}
	
	
	//��ȡԪ���츳���ܵ�ͼ�����츳���йأ�
	%bmpBackground = SpiritOperationManager_GetTalentBackground($SoulGeniusWnd_SpiritSlot);
	SoulGeniusWnd_TalentSkillBackImg.setBitmap("gameres/gui/images/" @ %bmpBackground);
	SoulGeniusWnd_TalentSkillBackImg.setVisible(1);
	
	//��ʾ���м���slot��text�ؼ��������ض���Ŀؼ�
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

//ˢ��Ԫ������Ӵ���
function SoulGenius_RefreshFaceWnd_2()
{
	//SoulGenius_JueZhao_OpenSlotBtn
	for (%i = 0; %i < 6; %i++)
	{
		%obj = "SoulGenius_JueZhao_Image_" @ %i;		
		%obj.setVisible(!SpiritOperationManager_IsJueZhaoOpened($SoulGeniusWnd_SpiritSlot, %i));
	}	
}

//���Ԫ��������
function SoulGenius_ClearWnd()
{
	SoulGenius_SpiritRace_Img.setVisible(0);
	SoulGenius_SpiritType.setText("��");
	SoulGenius_SpiritLevel.setText("0��");
	SoulGenius_ExpPercent.setPercentum("0");
	SoulGenius_ExpText.setText("0/0");
	
	SoulGenius_ClearMainBtns();
	SoulGenius_ClearFaceWnd_1();
	SoulGenius_ClearFaceWnd_2();
}

//���Ԫ���츳�����ڰ�ť��Image
function SoulGenius_ClearMainBtns()
{
	//Ԫ��۵ĸ���bitmap
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

//���Ԫ���츳�Ӵ���
function SoulGenius_ClearFaceWnd_1()
{
	SoulGenius_TalentLimitPts.setText("0");
	SoulGenius_TalentPts.setText("0");
	SoulGenius_TalentLeftPts.setText("0");
	SoulGenius_TalentLimitStrengthenBtn.setActive(0);
	SoulGeniusWnd_TalentSkillBackImg.setVisible(0);		//�츳���ܵĵ�ͼ���ɼ�
	SoulGeniusFace_1.setBitmap("gameres/gui/images/GUIWindow39_1_002.png");
}

//���Ԫ������Ӵ���
function SoulGenius_ClearFaceWnd_2()
{
	SoulGenius_JueZhao_OpenSlotBtn.setActive(0);
	for (%i = 0; %i < 6; %i++)
	{
		%obj = "SoulGenius_JueZhao_Image_" @ %i;		
		%obj.setVisible(0);
	}	
}

//����Ԫ��
function SoulGenius_RemoveSpirit(%nSlot)
{
	SpiritOperationManager_DeleteSpirit($SoulGeniusWnd_SpiritSlot);
}

//����Ԫ��
function SoulGenius_ActiveSpirit(%nSlot)
{
	SpiritOperationManager_ActiviateSpirit($SoulGeniusWnd_SpiritSlot);
}

//�û�Ԫ��
function SoulGenius_HuanHuaSpirit(%nSlot)
{
	SpiritOperationManager_HuanHuaSpirit($SoulGeniusWnd_SpiritSlot);
	SoulGenius_HuanHuaSpiritBtn.setActive(0);
}

//ȡ���û�Ԫ��
function SoulGenius_CancelHuanHuaSpirit(%nSlot)
{
	SpiritOperationManager_CancelHuanHuaSpirit($SoulGeniusWnd_SpiritSlot);
	SoulGenius_CancelHuanHuaSpiritBtn.setActive(0);
}

//����Ԫ��
function SoulGenius_LockSpirit(%nSlot)
{
	
}

//���������
function SoulGeniusWnd_OpenJueZhaoSlot()
{
	
}

//��Ӧ��Ԫ����
function OnOpenSpiritSlotSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshWnd();
}

function OnOpenSpiritSlotFailure(%nSpiritSlot)
{
	
}

//��Ӧ��Ԫ�������
function OnOpenJueZhaoSlotSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshWnd();
}

function OnOpenJueZhaoSlotFailure(%nSpiritSlot)
{
	
}

//��Ӧ���Ԫ��
function OnAddSpiritSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshFaceWnd_2();
}

function OnAddSpiritFailure(%nSpiritSlot)
{
	
}

//��Ӧɾ��Ԫ��
function OnRemoveSpiritSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshWnd();
}

function OnRemoveSpiritFailure(%nSpiritSlot)
{
	
}

//��ӦԪ�񼤻�
function OnActivateSpiritSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshMainBtns();
}

function OnActivateSpiritFailure(%nSpiritSlot)
{
}

//��ӦԪ��û�
function OnHuanHuaSpiritSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshMainBtns();
}

function OnHuanHuaSpiritFailure(%nSpiritSlot)
{
}

//��Ӧȡ��Ԫ��û�
function OnCancelHuanHuaSuccess(%nSpiritSlot)
{
	if ($SoulGeniusWnd_SpiritSlot == %nSpiritSlot)
		SoulGenius_RefreshMainBtns();
}

function OnCancelHuanHuaFailure(%nSpiritSlot)
{
}

//��ӦԪ��ȼ�����
function OnAddSpiritLevel(%nSpiritSlot, %nAddedLevel)
{
	if ($SoulGeniusWnd_SpiritSlot != %nSpiritSlot)
		return;
	if(!SoulGeniusWndGui.IsVisible())
		return;
	
	SoulGenius_RefreshMainWnd();
	//ˢ��ʣ�����
	SoulGenius_TalentPts.setText(SpiritOperationManager_GetTalentPts($SoulGeniusWnd_SpiritSlot));
	SoulGenius_TalentLeftPts.setText(SpiritOperationManager_GetTalentLeftPts($SoulGeniusWnd_SpiritSlot));
}

//��ӦԪ��������
function OnAddSpiritExp(%nSpiritSlot, %nAddedExp)
{
	if ($SoulGeniusWnd_SpiritSlot != %nSpiritSlot)
		return;
	if(!SoulGeniusWndGui.IsVisible())
		return;
	
	SoulGenius_RefreshMainWnd();
	//ˢ��ʣ�����
	SoulGenius_TalentPts.setText(SpiritOperationManager_GetTalentPts($SoulGeniusWnd_SpiritSlot));
	SoulGenius_TalentLeftPts.setText(SpiritOperationManager_GetTalentLeftPts($SoulGeniusWnd_SpiritSlot));
}

//��ӦԪ���츳�ȼ�����
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

/////////////////////////////////////Ԫ����н�����ؽű�/////////////////////////////
$SpiritJueZhaoWnd_SpiritSlot = 0;
$SpiritJueZhaoWnd_SkillSlot = 0;

//�򿪾��н���
function SpiritJueZhaoWnd_OpenSpiritSkill()
{
	if(!SpiritJueZhaoWnd.IsVisible())
	{
		GameMainWndGui_Layer3.PushToBack(SpiritJueZhaoWnd);
		SpiritJueZhaoWnd.setVisible(1);
		//ˢ��Ԫ���
		for (%i = 0; %i < 5; %i++)
		{
			%obj = "SpiritJueZhaoWnd_SpiritSlot_" @ %i;		
			//%obj.setVisible(SpiritOperationManager_IsSpiritExist(%i));		//��ʱȥ���������
		}
		SpiritJueZhaoWnd_RefreshWnd();
		%SlotObj = "SpiritJueZhaoWnd_SpiritSlot_0";
		%SlotObj.setBeSelected(1);
		%SlotObj = "SpiritJueZhaoWnd_Slot_0";
		%SlotObj.setBeSelected(1);
	}
}

//�رվ��н���
function SpiritJueZhaoWnd_CloseSpiritSkill()
{
	$SpiritJueZhaoWnd_SpiritSlot = 0;
	$SpiritJueZhaoWnd_SkillSlot = 0;
	SpiritJueZhaoWnd.setVisible(0);
}

//ѡ��Ԫ���
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

//ѡ��Ԫ���ܲ�
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
	//ˢ�¼���ͼƬ
	for (%i = 0; %i < 6; %i++)
	{
		%obj = "SpiritJueZhaoWnd_Image_" @ %i;
		%bOpened = SpiritOperationManager_IsJueZhaoOpened($SpiritJueZhaoWnd_SpiritSlot, %i);
		%obj.setVisible(!%bOpened);
	}
	
	//ˢ�°�ť
	
}