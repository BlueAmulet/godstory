//===========================================================================================================
// 文档说明:宠物信息界面操作
// 创建时间:2009-8-13
// 创建人: soar
//=========================================================================================================== 
$PetInfoWnd_PetSlot = 0;
$PetInfoWnd_MountSlot = 0;
function OpenOrClosePetInfoWnd()
{
	if(PetInfoGui.isVisible())
	{
		ClosePetInfoWnd();
	}
	else
	{
		OpenPetInfoWnd();
	}
}

function OpenPetInfoWnd()
{
	PetInfoGui.setvisible(1);
	GameMainWndGui_Layer3.PushToBack(PetInfoGui);
	PetUpgradeHintButton.setVisible(0);
	PetInfoWnd_SelectSlot($nSpawnedSlot);
	PetInfoWnd_SelectMountSlot($PetInfoWnd_MountSlot);
	$IsDirty++;
}

function SeePetInfo(%index)
{
	$PetInfoWnd_PetSlot = %index;
	PetInfoGui.setvisible(1);
	PetUpgradeHintButton.setVisible(0);
	PetInfoWnd_SelectSlot($PetInfoWnd_PetSlot);
	$IsDirty++;
}

function ClosePetInfoWnd()
{
	PetInfoGui.setvisible(0);
	$IsDirty--;
}

function  ClickPetInfoWnd()
{
	PetInfoWnd_Pet.setVisible(1);
	PetInfoWnd_QiPet.setVisible(0);
	PetModelView.setVisible(1);
	MountModelView.setVisible(0);
	PetInfoWnd_SelectSlot($nSpawnedSlot);
}

function ClickQiPetInfoWnd()
{
	PetInfoWnd_Pet.setVisible(0);
	PetInfoWnd_QiPet.setVisible(1);
	PetModelView.setVisible(0);
	MountModelView.setVisible(1);
	PetInfoWnd_SelectMountSlot($PetInfoWnd_MountSlot);
}

function OpenMoveWnd1_Attack()
{
	MoveWnd1_Attack.setvisible(1);
	MoveWnd1_Recovery.setvisible(0);
	MoveWnd1_Other.setvisible(0);
}

function OpenMoveWnd1_Recovery()
{
	MoveWnd1_Attack.setvisible(0);
	MoveWnd1_Recovery.setvisible(1);
	MoveWnd1_Other.setvisible(0);
}

function OpenMoveWnd1_Other()
{
	MoveWnd1_Attack.setvisible(0);
	MoveWnd1_Recovery.setvisible(0);
	MoveWnd1_Other.setvisible(1);
}

function OpenPetAbility()
{
	PetInfoWnd_AbilityInfo.setvisible(1);
	PetInfoWnd_SkillInfo.setvisible(0);
}

function OpenPetSkill()
{
	PetInfoWnd_AbilityInfo.setvisible(0);
	PetInfoWnd_SkillInfo.setvisible(1);
}

function OpenMoveWnd(%index)
{
	for(%i = 1; %i <= 4; %i++)
	{
		("SmallInfoWnd_MoveWnd" @ %i).setvisible(0);
	}
	("SmallInfoWnd_MoveWnd" @ %index).setvisible(1);
	switch(%index)
	{
		case 1:
			SmallInfoWnd_MoveButton2.setposition(0,163);
			SmallInfoWnd_MoveButton3.setposition(0,181);
			SmallInfoWnd_MoveButton4.setposition(0,199);
		case 2:
			SmallInfoWnd_MoveButton2.setposition(0,18);
			SmallInfoWnd_MoveButton3.setposition(0,181);
			SmallInfoWnd_MoveButton4.setposition(0,199);
		case 3:
			SmallInfoWnd_MoveButton2.setposition(0,18);
			SmallInfoWnd_MoveButton3.setposition(0,36);
			SmallInfoWnd_MoveButton4.setposition(0,199);
		case 4:
			SmallInfoWnd_MoveButton2.setposition(0,18);
			SmallInfoWnd_MoveButton3.setposition(0,36);
			SmallInfoWnd_MoveButton4.setposition(0,54);	
	}
}

function SpawnPet()
{
	echo("function SpawnPet");
	PetInfoWnd_PetOpen.setActive(0);
	PetInfoWnd_PetSetFree.setActive(0);
	Pet_SpawnPet($PetInfoWnd_PetSlot);
}

function OnSpawnPetSuccess()
{
	PetInfoWnd_RefreshWnd();
}

function OnSpawnPetFailure()
{
	echo("function OnSpawnPetFailure");
	PetInfoWnd_PetSetFree.setActive(1);
	PetInfoWnd_PetOpen.setActive(1);
}

function DisbandPet()
{
	echo("function DisbandPet");
	PetInfoWnd_PetClose.setActive(0);
	Pet_DisbandPet($PetInfoWnd_PetSlot);
}

function OnDisbandPetSuccess()
{
	echo("function OnDisbandPetSuccess");
	PetInfoWnd_PetOpen.setVisible(1);
	PetInfoWnd_PetOpen.setActive(1);
	PetInfoWnd_PetClose.setVisible(0);
	PetInfoWnd_PetSetFree.setActive(1);
}

function OnDisbandPetFailure()
{
	echo("function OnDisbandPetFailure");
	PetInfoWnd_PetClose.setActive(1);
}

function ReleasePet()
{
	echo("function ReleasePet");
	PetInfoWnd_PetSetFree.setActive(0);
	Pet_ReleasePet($PetInfoWnd_PetSlot);
}

function OnReleasePetSuccess()
{
	PetInfoWnd_RefreshWnd();
}

function OnReleasePetFailure()
{
	echo("function OnReleasePetSuccess");
	PetInfoWnd_PetSetFree.setActive(1);
}

function PetInfoWnd_SelectSlot(%nSlot)
{
	if (%nSlot >= 0 && %nSlot <= 7)
	{
		//取消原PetSlot的选中状态
		%PetSlotObj = "PetInfoWnd_PetShortCut" @ ($PetInfoWnd_PetSlot+1);
		if (%PetSlotObj.getBeSelected())
			%PetSlotObj.setBeSelected(0);
		
		%PetStudySlotObj = "PetStudyWnd_PetShortCut_" @ ($PetInfoWnd_PetSlot);
		if (%PetStudySlotObj.getBeSelected())
			%PetStudySlotObj.setBeSelected(0);
			
		//记录当前被选中的槽位
		$PetInfoWnd_PetSlot = %nSlot;
		//设置当前PetSlot的选中状态
		%PetSlotObj = "PetInfoWnd_PetShortCut" @ ($PetInfoWnd_PetSlot+1);
		%PetSlotObj.setBeSelected(1);
		%PetStudySlotObj = "PetStudyWnd_PetShortCut_" @ ($PetInfoWnd_PetSlot);
		%PetStudySlotObj.setBeSelected(1);
		
		if (Pet_IsPetExist($PetInfoWnd_PetSlot))
			PetInfoWnd_RefreshWnd();
		else
			PetInfoWnd_ClearWnd();
			
		if (PetInfoGui_StudyWnd.isVisible())
		{			
			PetStudyWnd_Description.setVisible(!Pet_IsPetExist($PetInfoWnd_PetSlot));
		}
	}
}

function PetInfoWnd_Feed(%nFeedType)
{
	if (!Pet_IsPetExist($PetInfoWnd_PetSlot))
		return;
	Pet_WeiShi($PetInfoWnd_PetSlot, %nFeedType);
}

function PetInfoWnd_ClearWnd()
{	
	echo("PetInfoWnd_ClearWnd");
	
	refreshPetModelView($PetInfoWnd_PetSlot);
	
	PetInfoWnd_PetOpen.setVisible(1);
	PetInfoWnd_PetOpen.setActive(0);
	PetInfoWnd_NeedPlayerLevel.setText("0级");
	PetInfoWnd_PetSetFree.setActive(0);
	
	PetInfoWnd_PetName.setText("暂无名称");
	PetInfoWnd_PetLevel.setText("0级");
	PetInfoWnd_PetDevelopRate.setText("0");
	PetInfoWnd_PetHuiGen.setText("0");
		
	PetInfoWnd_PetGenGu.setText("0");
	PetInfoWnd_PetTiPo.setText("0");
	PetInfoWnd_PetJingLi.setText("0");
	PetInfoWnd_PetLiDao.setText("0");
	PetInfoWnd_PetLingLi.setText("0");
	PetInfoWnd_PetMingJie.setText("0");
	PetInfoWnd_PetYuanLi.setText("0");
	PetInfoWnd_PetQianLi.setText("0");
	
	//伤害
	Attack_Physics.setText("0");
	Attack_YuanLi.setText("0");
	Attack_Mu.setText("0");
	Attack_Huo.setText("0");
	Attack_Shui.setText("0");
	Attack_Jin.setText("0");
	Attack_Tu.setText("0");
	
	//防御
	Recovery_Physics.setText("0");
	Recovery_YuanLi.setText("0");
	Recovery_Mu.setText("0");
	Recovery_Huo.setText("0");
	Recovery_Shui.setText("0");
	Recovery_Jin.setText("0");
	Recovery_Tu.setText("0");
	
	//资质
	MoveWnd2_TiPo.setText("0");
	MoveWnd2_JingLi.setText("0");
	MoveWnd2_TiPo.MoveWnd2_LiDao("0");
	MoveWnd2_TiPo.MoveWnd2_LingLi("0");
	MoveWnd2_MinJie.setText("0");
	MoveWnd2_YuanLi.setText("0");
	MoveWnd2_ShouMing.setText("0");
	
	//其他
	Other_ZhiLiao.setText("0");
	Other_ShanBi.setText("0");
	Other_BaoJi.setText("0");
	Other_BaoJiLv.setText("0%");
	
	//比例条及其中文本显示
	PetInfoWnd_HP.setText("0/0");
	PetInfoWnd_MP.setText("0/0");
	PetInfoWnd_Happy.setText("0/0");
	PetInfoWnd_Exp.setText("0/0");
	
	PetInfoWnd_HPImage.setPercentum(0);
	PetInfoWnd_MPImage.setPercentum(0);
	PetInfoWnd_HappyImage.setPercentum(0);
	PetInfoWnd_ExpImage.setPercentum(0);
	
	//宠物各种图片隐藏
	//PetInfoWnd_PetStatus.setBitmap("");
	PetInfoWnd_PetStatus.setVisible(0);
	PetInfoWnd_Generation.setBitmap("");
	PetInfoWnd_Generation.setVisible(0);
	PetInfoWnd_PetRace.setBitmap("");
	PetInfoWnd_PetRace.setVisible(0);
	PetInfoWnd_PetSex.setBitmap("");
	PetInfoWnd_PetSex.setVisible(0);
	PetInfoWnd_PetStyle.setBitmap("");
	PetInfoWnd_PetStyle.setVisible(0);
	
	//按钮
	PetInfoWnd_PetOpen.setActive(0);
	PetInfoWnd_PetOpen.setVisible(1);
	PetInfoWnd_PetClose.setActive(0);
	PetInfoWnd_PetClose.setVisible(0);
	PetInfoWnd_PetSetFree.setActive(0);
	PetInfoWnd_PetLock.setActive(0);
	PetInfoWnd_PetAssembled.setActive(0);
	PetInfoWnd_PetStudy.setActive(0);
	PetInfoWnd_PetDomestication.setActive(0);
	PetInfoWnd_PetFeeding1.setActive(0);
	PetInfoWnd_PetFeeding2.setActive(0);
	
	//加点和减点按钮
	PetInfoWnd_ClearAddDecBtns();
}

function PetInfoWnd_RefreshBtn()
{	
	%IsPetExist = Pet_IsPetExist($PetInfoWnd_PetSlot);
	
	PetInfoWnd_PetSetFree.setActive(%IsPetExist);
	PetInfoWnd_PetLock.setActive(%IsPetExist);
	PetInfoWnd_PetAssembled.setActive(%IsPetExist);
	PetInfoWnd_PetStudy.setActive(%IsPetExist);
	PetInfoWnd_PetDomestication.setActive(%IsPetExist);
	PetInfoWnd_PetFeeding1.setActive(%IsPetExist);
	PetInfoWnd_PetFeeding2.setActive(%IsPetExist);

	if (!%IsPetExist)
	{		
		PetInfoWnd_PetOpen.setVisible(1);
		PetInfoWnd_PetOpen.setActive(0);
		PetInfoWnd_PetClose.setVisible(0);
		PetInfoWnd_ClearAddDecBtns();	
	}
	else
	{
		%IsPetSpawned = Pet_IsPetSpawned($PetInfoWnd_PetSlot);
		PetInfoWnd_PetOpen.setVisible(!%IsPetSpawned);
		PetInfoWnd_PetClose.setVisible(%IsPetSpawned);
		PetInfoWnd_PetOpen.setActive(!%IsPetSpawned);
		PetInfoWnd_PetClose.setActive(%IsPetSpawned);
		PetInfoWnd_RefreshAddDecBtns();
	}
}

function PetInfoWnd_RefreshAddDecBtns()
{
	//刷新各个加点按钮
	%canAddPts = PetInfoWnd_CanPetAddProPerty();
	PetInfoWnd_PetTiPoAdd.setActive(%canAddPts);
	PetInfoWnd_PetJingLiAdd.setActive(%canAddPts);
	PetInfoWnd_PetLiDaoAdd.setActive(%canAddPts);
	PetInfoWnd_PetLingLiAdd.setActive(%canAddPts);
	PetInfoWnd_PetMingJieAdd.setActive(%canAddPts);
	PetInfoWnd_PetYuanLiAdd.setActive(%canAddPts);
	
	//刷新各个减点按钮
	%canDecPts = PetInfoWnd_CanPetDecProPerty(1);
	PetInfoWnd_PetTiPoDec.setActive(%canDecPts);
	%canDecPts = PetInfoWnd_CanPetDecProPerty(2);
	PetInfoWnd_PetJingLiDec.setActive(%canDecPts);
	%canDecPts = PetInfoWnd_CanPetDecProPerty(3);
	PetInfoWnd_PetLiDaoDec.setActive(%canDecPts);
	%canDecPts = PetInfoWnd_CanPetDecProPerty(4);
	PetInfoWnd_PetLingLiDec.setActive(%canDecPts);
	%canDecPts = PetInfoWnd_CanPetDecProPerty(5);
	PetInfoWnd_PetMingJieDec.setActive(%canDecPts);
	%canDecPts = PetInfoWnd_CanPetDecProPerty(6);
	PetInfoWnd_PetYuanLiDec.setActive(%canDecPts);
}

function PetInfoWnd_ClearAddDecBtns()
{
	//清除各个加点按钮
	PetInfoWnd_PetTiPoAdd.setActive(0);
	PetInfoWnd_PetJingLiAdd.setActive(0);
	PetInfoWnd_PetLiDaoAdd.setActive(0);
	PetInfoWnd_PetLingLiAdd.setActive(0);
	PetInfoWnd_PetMingJieAdd.setActive(0);
	PetInfoWnd_PetYuanLiAdd.setActive(0);
	
	//清除各个减点按钮
	PetInfoWnd_PetTiPoDec.setActive(0);
	PetInfoWnd_PetJingLiDec.setActive(0);
	PetInfoWnd_PetLiDaoDec.setActive(0);
	PetInfoWnd_PetLingLiDec.setActive(0);
	PetInfoWnd_PetMingJieDec.setActive(0);
	PetInfoWnd_PetYuanLiDec.setActive(0);
}

function PetInfoWnd_RefreshWnd()
{
	if (!PetInfoGui.isVisible())
		return;
	
	refreshPetModelView($PetInfoWnd_PetSlot);
	PetInfoWnd_PetName.setText(PetOperation_GetPetName($PetInfoWnd_PetSlot));
	PetInfoWnd_PetLevel.setText(PetOperation_GetPetLevel($PetInfoWnd_PetSlot) @ "级");
	PetInfoWnd_NeedPlayerLevel.setText(PetOperation_GetPetSpawnLevel($PetInfoWnd_PetSlot) @ "级");
	
	%nChengZhangLv = PetOperation_GetPetChengZhangLv($PetInfoWnd_PetSlot);
	if (%nChengZhangLv != 0)
	{
		PetInfoWnd_PetDevelopRate.setColor(255, 255, 255);
		PetInfoWnd_PetDevelopRate.setText(%nChengZhangLv);
	}
	else
	{
		PetInfoWnd_PetDevelopRate.setColor(255, 0, 0);
		PetInfoWnd_PetDevelopRate.setText("未鉴定");
	}
		
	PetInfoWnd_PetHuiGen.setText(PetOperation_GetPetHuiGen($PetInfoWnd_PetSlot));
	PetInfoWnd_PetGenGu.setText(PetOperation_GetPetGenGu($PetInfoWnd_PetSlot));
	PetInfoWnd_PetTiPo.setText(PetOperation_GetPetTiPo($PetInfoWnd_PetSlot));
	PetInfoWnd_PetJingLi.setText(PetOperation_GetPetJingLi($PetInfoWnd_PetSlot));
	PetInfoWnd_PetLiDao.setText(PetOperation_GetPetLiDao($PetInfoWnd_PetSlot));
	PetInfoWnd_PetLingLi.setText(PetOperation_GetPetLingLi($PetInfoWnd_PetSlot));
	PetInfoWnd_PetMingJie.setText(PetOperation_GetPetMingJie($PetInfoWnd_PetSlot));
	PetInfoWnd_PetYuanLi.setText(PetOperation_GetPetYuanLi($PetInfoWnd_PetSlot));
	PetInfoWnd_PetQianLi.setText(PetOperation_GetPetQianLi($PetInfoWnd_PetSlot));
		
	//伤害
	Attack_Physics.setText(PetOperation_GetPetWuAttack($PetInfoWnd_PetSlot));
	Attack_YuanLi.setText(PetOperation_GetPetYuanAttack($PetInfoWnd_PetSlot));
	Attack_Mu.setText(PetOperation_GetPetMuAttack($PetInfoWnd_PetSlot));
	Attack_Huo.setText(PetOperation_GetPetHuoAttack($PetInfoWnd_PetSlot));
	Attack_Shui.setText(PetOperation_GetPetShuiAttack($PetInfoWnd_PetSlot));
	Attack_Jin.setText(PetOperation_GetPetJinAttack($PetInfoWnd_PetSlot));
	Attack_Tu.setText(PetOperation_GetPetTuAttack($PetInfoWnd_PetSlot));
	
	//防御
	Recovery_Physics.setText(PetOperation_GetPetWuDefence($PetInfoWnd_PetSlot));
	Recovery_YuanLi.setText(PetOperation_GetPetYuanDefence($PetInfoWnd_PetSlot));
	Recovery_Mu.setText(PetOperation_GetPetMuDefence($PetInfoWnd_PetSlot));
	Recovery_Huo.setText(PetOperation_GetPetHuoDefence($PetInfoWnd_PetSlot));
	Recovery_Shui.setText(PetOperation_GetPetShuiDefence($PetInfoWnd_PetSlot));
	Recovery_Jin.setText(PetOperation_GetPetJinDefence($PetInfoWnd_PetSlot));
	Recovery_Tu.setText(PetOperation_GetPetTuDefence($PetInfoWnd_PetSlot));
	
	//资质
	MoveWnd2_TiPo.setText(PetOperation_GetPetStaminaGift($PetInfoWnd_PetSlot));
	MoveWnd2_JingLi.setText(PetOperation_GetPetManaGift($PetInfoWnd_PetSlot));
	MoveWnd2_LiDao.setText(PetOperation_GetPetStrengthGift($PetInfoWnd_PetSlot));
	MoveWnd2_LingLi.setText(PetOperation_GetPetIntellectGift($PetInfoWnd_PetSlot));
	MoveWnd2_MinJie.setText(PetOperation_GetPetAgilityGift($PetInfoWnd_PetSlot));
	MoveWnd2_YuanLi.setText(PetOperation_GetPetPneumaGift($PetInfoWnd_PetSlot));
	MoveWnd2_ShouMing.setText("000");		//寿命未做
	
	//其他
	Other_ZhiLiao.setText(PetOperation_GetPetZhiLiao($PetInfoWnd_PetSlot));
	Other_ShanBi.setText(PetOperation_GetPetShanBi($PetInfoWnd_PetSlot));
	Other_BaoJi.setText(PetOperation_GetPetBaoJi($PetInfoWnd_PetSlot));
	
	%nBaojilv = PetOperation_GetPetBaoJiLv($PetInfoWnd_PetSlot);
	Other_BaoJiLv.setText(%nBaojiLv / 100 @ "%");

	//比例条及其中文本显示
	//宠物生命比例
	%nPetCurHp = PetOperation_GetPetHp($PetInfoWnd_PetSlot);
	%nPetMaxHp = PetOperation_GetPetMaxHp($PetInfoWnd_PetSlot);
	if (%nPetCurHp > %nPetMaxHp)
	{
		PetInfoWnd_Hp.setText(%nPetMaxHp @ "/" @ %nPetMaxHp);
		PetInfoWnd_HPImage.setPercentum(1);
	}
	else
	{
		PetInfoWnd_Hp.setText(%nPetCurHp @ "/" @ %nPetMaxHp);
		PetInfoWnd_HPImage.setPercentum(%nPetCurHp / %nPetMaxHp);
	}
	
	//宠物法力比例
	%nPetCurMp = PetOperation_GetPetMp($PetInfoWnd_PetSlot);
	%nPetMaxMp = PetOperation_GetPetMaxMp($PetInfoWnd_PetSlot);
	if (%nPetCurMp > %nPetMaxMp)
	{
		PetInfoWnd_Mp.setText(%nPetCurMp @ "/" @ %nPetMaxMp);
		PetInfoWnd_MPImage.setPercentum(1);
	}
	else
	{
		PetInfoWnd_Mp.setText(%nPetCurMp @ "/" @ %nPetMaxMp);
		PetInfoWnd_MPImage.setPercentum(%nPetCurMp / %nPetMaxMp);
	}
		
	//宠物快乐度比例
	%nPetCurHappy = PetOperation_GetPetHappiness($PetInfoWnd_PetSlot);
	%nPetMaxHappy = 100;
	PetInfoWnd_Happy.setText(%nPetCurHappy @ "/" @ %nPetMaxHappy);
	PetInfoWnd_HappyImage.setPercentum(%nPetCurHappy / %nPetMaxHappy);
	
	%nPetCurExp = PetOperation_GetPetExp($PetInfoWnd_PetSlot);
	%nPetMaxExp = PetOperation_GetPetMaxExp($PetInfoWnd_PetSlot);
	PetInfoWnd_Exp.setText(%nPetCurExp @ "/" @ %nPetMaxExp);
	PetInfoWnd_ExpImage.setPercentum(%nPetCurExp / %nPetMaxExp);
	
	
	//各个图片选择
	//宠物状态图片选择    PetInfoWnd_PetStatus
	%nStatus = PetOperation_GetPetStatus($PetInfoWnd_PetSlot);
	switch(%nStatus)
	{
	case 0:			//空闲状态
		echo("就是这里出的问题================"@ %nStatus);
		//PetInfoWnd_PetStatus.setBitmap("");
		PetInfoWnd_PetStatus.setVisible(0);
		break;
	case 1:			//出战状态
		PetInfoWnd_PetStatus.setBitmap("gameres/gui/images/GUIWindow35_1_053.png");
		PetInfoWnd_PetStatus.setVisible(1);
		break;
	case 2:			//合体状态
		PetInfoWnd_PetStatus.setBitmap("gameres/gui/images/GUIWindow35_1_055.png");
		PetInfoWnd_PetStatus.setVisible(1);
		break;
	case 4:			//修行状态
		PetInfoWnd_PetStatus.setBitmap("gameres/gui/images/GUIWindow35_1_054.png");
		PetInfoWnd_PetStatus.setVisible(1);
		break;
	default:
		echo("就是这里出的问题++++++++++++++"@ %nStatus);
		PetInfoWnd_PetStatus.setBitmap("");
		break;
	}
	
	//宠物代数图片选择		PetInfoWnd_Generation
	%nGeneration = PetOperation_GetPetGeneration($PetInfoWnd_PetSlot);
	switch(%nGeneration)
	{
	case 0:
		PetInfoWnd_Generation.setBitmap("gameres/gui/images/GUIWindow35_1_035.png");
		PetInfoWnd_Generation.setVisible(1);
		break;
	case 1:
		PetInfoWnd_Generation.setBitmap("gameres/gui/images/GUIWindow35_1_036.png");
		PetInfoWnd_Generation.setVisible(1);
		break;
	case 2:
		PetInfoWnd_Generation.setBitmap("gameres/gui/images/GUIWindow35_1_023.png");
		PetInfoWnd_Generation.setVisible(1);
		break;
	default:
		PetInfoWnd_Generation.setVisible(0);
		break;	
	}
	
	//宠物种族图片选择		PetInfoWnd_PetRace
	%nRace = PetOperation_GetPetRace($PetInfoWnd_PetSlot);
	switch(%nRace)
	{
	case 0:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_041.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;	
	case 1:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_043.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;
	case 2:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_042.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;
	case 3:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_045.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;
	case 4:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_047.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;
	case 5:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_044.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;
	case 6:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_046.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;
	case 7:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_048.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;
	case 8:
		PetInfoWnd_PetRace.setBitmap("gameres/gui/images/GUIWindow35_1_049.png");
		PetInfoWnd_PetRace.setVisible(1);
		break;
	default:
		PetInfoWnd_PetRace.setVisible(0);
		break;
	}
	
	//宠物类型（宝宝、变异。。。）图片选择		PetInfoWnd_PetStyle
	%nStyle = PetOperation_GetPetStyle($PetInfoWnd_PetSlot);
	switch(%nStyle)
	{
	case 1:
		PetInfoWnd_PetStyle.setBitmap("gameres/gui/images/GUIWindow35_1_037.png");
		PetInfoWnd_PetStyle.setVisible(1);
		break;
	case 2:
		PetInfoWnd_PetStyle.setBitmap("gameres/gui/images/GUIWindow35_1_038.png");
		PetInfoWnd_PetStyle.setVisible(1);
		break;
	case 3:
		PetInfoWnd_PetStyle.setBitmap("gameres/gui/images/GUIWindow35_1_039.png");
		PetInfoWnd_PetStyle.setVisible(1);
		break;
	case 4:
		PetInfoWnd_PetStyle.setBitmap("gameres/gui/images/GUIWindow35_1_040.png");
		PetInfoWnd_PetStyle.setVisible(1);
		break;
	default:
		PetInfoWnd_PetStyle.setVisible(0);
		break;
	}
	
	//宠物性别图片选择		PetInfoWnd_PetSex
	%nSex = PetOperation_GetPetSex($PetInfoWnd_PetSlot);
	switch(%nSex)
	{
	case 0:		//雄性
		PetInfoWnd_PetSex.setBitmap("gameres/gui/images/GUIWindow35_1_024.png");
		PetInfoWnd_PetSex.setVisible(1);
		break;
	case 1:		//雌性
		PetInfoWnd_PetSex.setBitmap("gameres/gui/images/GUIWindow35_1_034.png");
		PetInfoWnd_PetSex.setVisible(1);
		break;
	default:
		PetInfoWnd_PetSex.setVisible(0);
		break;	
	}

	PetInfoWnd_RefreshBtn();
}

function PetInfoWnd_SelectMountSlot(%nSlot)
{
	if (%nSlot >= 0 && %nSlot <= 7)
	{
		//取消原PetSlot的选中状态
		%MountSlotObj = "PetInfoWnd_MountShortCut" @ ($PetInfoWnd_MountSlot+1);
		if (%MountSlotObj.getBeSelected())
			%MountSlotObj.setBeSelected(0);
		//记录当前被选中的槽位
		$PetInfoWnd_MountSlot = %nSlot;
		//设置当前PetSlot的选中状态
		%MountSlotObj = "PetInfoWnd_MountShortCut" @ ($PetInfoWnd_MountSlot+1);
		%MountSlotObj.setBeSelected(1);
		if (Mount_IsMountExist($PetInfoWnd_MountSlot))
			PetInfoWnd_RefreshMountWnd();
		else
			PetInfoWnd_ClearMountWnd();
	}
}

function PetInfoWnd_RefreshMountWnd()
{
	refreshMountModelView($PetInfoWnd_MountSlot);
	
	PetInfoWnd_QiPet_Name.setText(Mount_GetMountName($PetInfoWnd_MountSlot));
	PetInfoWnd_QiPet_InfoDesc.setText(Mount_GetMountDesction($PetInfoWnd_MountSlot));
	PetInfoWnd_QiPet_Level.setText(Mount_GetMountPlayerLevel($PetInfoWnd_MountSlot) @ "级");
	PetInfoWnd_QiPet_SpeedupPercent.setText( (Mount_GetMountSpeedupPercent($PetInfoWnd_MountSlot) / 100) @ "%" );
	
	// 剩余时间
	PetInfoWnd_setLeftTimeDesc();
	
	//骑宠状态图片选择    PetInfoWnd_QiPet_Status
	%nStatus = Mount_GetMountStatus($PetInfoWnd_MountSlot);
	switch(%nStatus)
	{
	case 1:			//骑乘状态
		PetInfoWnd_QiPet_Status.setBitmap("gameres/gui/images/GUIWindow35_1_056.png");
		PetInfoWnd_QiPet_Status.setVisible(1);
		break;
	default:
		//PetInfoWnd_QiPet_Status.setBitmap("");
		PetInfoWnd_QiPet_Status.setVisible(0);
		break;
	}
	
	//骑宠种族图片选择		PetInfoWnd_PetRace
	%nRace = Mount_GetMountRace($PetInfoWnd_MountSlot);
	switch(%nRace)
	{
	case 0:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_041.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;	
	case 1:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_043.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;
	case 2:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_042.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;
	case 3:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_045.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;
	case 4:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_047.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;
	case 5:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_044.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;
	case 6:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_046.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;
	case 7:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_048.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;
	case 8:
		PetInfoWnd_QiPet_MountRace.setBitmap("gameres/gui/images/GUIWindow35_1_049.png");
		PetInfoWnd_QiPet_MountRace.setVisible(1);
		break;
	default:
		PetInfoWnd_QiPet_MountRace.setVisible(0);
		break;
	}
	
	// 骑乘类型：单人、多人
	%nMountCount = Mount_GetMountPlayerCount($PetInfoWnd_MountSlot);
	switch(%nMountCount)
	{
	case 0:
		PetInfoWnd_QiPet_MountCount.setText("无人");			//不能出现这种情况
		break;
	case 1:
		PetInfoWnd_QiPet_MountCount.setText("1人");
		break;
	case 2:
		PetInfoWnd_QiPet_MountCount.setText("2人");
		break;
	case 3:
		PetInfoWnd_QiPet_MountCount.setText("3人");
		break;
	case 4:
		PetInfoWnd_QiPet_MountCount.setText("4人");
		break;
	default:
		PetInfoWnd_QiPet_MountCount.setText("多于4人");
		break;
	}
		
	// 骑宠行为类型：骑宠、战斗宠
	%nBehaviorType = Mount_GetMountType($PetInfoWnd_MountSlot);
	if (%nBehaviorType == 1)				//骑宠
	{
		PetInfoWnd_QiPet_MountType.setBitmap("gameres/gui/images/GUIWindow35_1_051.png");
		PetInfoWnd_QiPet_MountType.setVisible(1);
	}
	else if (%nBehaviorType == 2)		//战斗宠
	{
		PetInfoWnd_QiPet_MountType.setBitmap("gameres/gui/images/GUIWindow35_1_052.png");
		PetInfoWnd_QiPet_MountType.setVisible(1);
	}
	else
	{
		PetInfoWnd_QiPet_MountType.setVisible(0);				//不能出现这种情况
	}
	
	//骑宠等级显示
	%nMountLevel = Mount_GetMountLevel($PetInfoWnd_MountSlot);
	for (%i = 1; %i <= 9; %i++)
	{
		%obj = "PetInfoWnd_QiPet_star" @ %i;
		%obj.setVisible(%i <= %nMountLevel);
	}
	
	PetInfoWnd_RefreshMountBtn();
}

function PetInfoWnd_ClearMountWnd()
{
	refreshMountModelView($PetInfoWnd_MountSlot);
	
	PetInfoWnd_QiPet_Mount.setVisible(1);
	PetInfoWnd_QiPet_UnMount.setVisible(0);
	PetInfoWnd_QiPet_Mount.setActive(0);
	PetInfoWnd_QiPet_Release.setActive(0);
	PetInfoWnd_QiPet_Lock.setActive(0);
	PetInfoWnd_QiPet_Name.setText("暂无名称");
	PetInfoWnd_QiPet_LeftTime.setText("0秒");
	PetInfoWnd_QiPet_Level.setText("0级");
	PetInfoWnd_QiPet_SpeedupPercent.setText("0%");
	PetInfoWnd_QiPet_MountCount.setText("");
	PetInfoWnd_QiPet_InfoDesc.setText("暂无骑宠信息说明");
	for (%i = 1; %i <= 9; %i++)
	{
		%obj = "PetInfoWnd_QiPet_star" @ %i;
		%obj.setVisible(0);
	}
	PetInfoWnd_QiPet_MountType.setVisible(0);
	PetInfoWnd_QiPet_MountRace.setVisible(0);
	//PetInfoWnd_PetStatus.setBitmap("");
		PetInfoWnd_QiPet_Status.setVisible(0);
}

function PetInfoWnd_RefreshMountBtn()
{
	%bCanSpawnMount = Mount_CanSpawnMount($PetInfoWnd_MountSlot);
	%bCanDisbandMount = Mount_CanDisbandMount($PetInfoWnd_MountSlot);
	%bCanReleaseMount = Mount_CanReleaseMount($PetInfoWnd_MountSlot);
	if (%bCanSpawnMount)
	{
		PetInfoWnd_QiPet_Mount.setVisible(1);
		PetInfoWnd_QiPet_UnMount.setVisible(0);
	}
	else if (%bCanDisbandMount)
	{
		PetInfoWnd_QiPet_Mount.setVisible(0);
		PetInfoWnd_QiPet_UnMount.setVisible(1);
	}
	
	PetInfoWnd_QiPet_Mount.setActive(%bCanSpawnMount);
	PetInfoWnd_QiPet_UnMount.setActive(%bCanDisbandMount);
	PetInfoWnd_QiPet_Release.setActive(%bCanReleaseMount);
}

function SpawnMount()
{
	echo("function SpawnMount");
	PetInfoWnd_QiPet_Mount.setActive(0);
	PetInfoWnd_QiPet_Release.setActive(0);
	Mount_SpawnMount($PetInfoWnd_MountSlot);
}

function OnSpawnMountSuccess()
{
	echo("function OnSpawnMountSuccess");
	PetInfoWnd_QiPet_UnMount.setVisible(1);
	PetInfoWnd_QiPet_UnMount.setActive(1);
	PetInfoWnd_QiPet_Mount.setVisible(0);
	PetInfoWnd_QiPet_Release.setActive(0);
}

function OnSpawnMountFailure()
{
	echo("function OnSpawnMountFailure");
	PetInfoWnd_QiPet_Mount.setVisible(1);
	PetInfoWnd_QiPet_Mount.setActive(1);
	PetInfoWnd_QiPet_UnMount.setVisible(0);
	PetInfoWnd_QiPet_Release.setActive(1);
}

function DisbandMount()
{
	echo("function DisbandMount");
	PetInfoWnd_QiPet_UnMount.setActive(0);
	Mount_DisbandMount($PetInfoWnd_MountSlot);
}

function OnDisbandMountSuccess()
{
	echo("function OnDisbandMountSuccess");
	PetInfoWnd_QiPet_Mount.setVisible(1);
	PetInfoWnd_QiPet_Mount.setActive(1);
	PetInfoWnd_QiPet_UnMount.setVisible(0);
	PetInfoWnd_QiPet_Release.setActive(1);
}

function OnDisbandMountFailure()
{
	echo("function OnDisbandMountFailure");
	PetInfoWnd_QiPet_UnMount.setActive(1);
	PetInfoWnd_QiPet_Release.setActive(0);
}

function ReleaseMount()
{
	echo("function ReleaseMount");
	PetInfoWnd_QiPet_Release.setActive(0);
	Mount_ReleaseMount($PetInfoWnd_MountSlot);
}

function OnReleaseMountSuccess()
{
	echo("function OnReleaseMountSuccess");
	PetInfoWnd_RefreshMountWnd();
}

function OnReleaseMountFailure()
{
	echo("function OnReleaseMountFailure");
	PetInfoWnd_QiPet_Release.setActive(1);
}

function OpenOrClosePetInfoGui_StudyWnd()
{
	if(PetInfoGui_StudyWnd.isvisible())
	{
		ClosePetInfoGui_StudyWnd();
	}
	else
	{
		OpenPetInfoGui_StudyWnd();
	}
}

function OpenPetInfoGui_StudyWnd()
{
	PetStudyWnd_Description.setVisible(!Pet_IsPetExist($PetInfoWnd_PetSlot));
	PetStudyData_Initialize();
	PetInfoGui_StudyWnd.setvisible(1);
	PetStudyWnd_StartStudyButton.setActive( Pet_CanXiuXing($PetInfoWnd_PetSlot, %selectId) );
	$IsDirty++;
}

function ClosePetInfoGui_StudyWnd()
{
	PetInfoGui_StudyWnd.setvisible(0);
	PetStudyWnd_List.clear();
	$IsDirty--;
}

function PetStudyWnd_List::SelectIt(%this)
{
	%selectId = PetStudyWnd_List.getSelectedId();
	PetStudyWnd_StartStudyButton.setActive( Pet_CanXiuXing($PetInfoWnd_PetSlot, %selectId) );
}

function StartPetStudy()
{
	Pet_XiuXing($PetInfoWnd_PetSlot, PetStudyWnd_List.getSelectedId());
	PetStudyWnd_StartStudyButton.setActive( Pet_CanXiuXing($PetInfoWnd_PetSlot, %selectId) );
}


function OnTimerPetStudyTerminate()
{
	PetStudyWnd_StartStudyButton.setActive( Pet_CanXiuXing($PetInfoWnd_PetSlot, %selectId) );
}


function OnPetChangeNameSuccess()
{
	PetInfoWnd_RefreshWnd();
}

function OnPetChangeNameFailure()
{
	PetInfoWnd_RefreshWnd();
}

function OnPetChangePropertiesSuccess()
{
	PetInfoWnd_RefreshWnd();
}

function OnPetChangePropertiesFailure()
{
	PetInfoWnd_RefreshWnd();
}

function ChangePetName()
{
	%sNewName = PetInfoWnd_PetName.getText();
	Pet_ChangeName($PetInfoWnd_PetSlot, %sNewName);
}

function PetInfoWnd_CanPetAddProPerty()
{
	%nLeftQianLi = PetInfoWnd_PetQianLi.getValue();
	return (%nLeftQianLi != 0);
}

function PetInfoWnd_CanPetDecProPerty(%nType)
{
	%nLeftQianLi = PetInfoWnd_PetQianLi.getValue();
	switch(%nType)
	{
	case 1:		//体魄
		%obj = "PetInfoWnd_PetTiPo";
		%content = PetInfoWnd_PetTiPo.getValue();
		%value = PetOperation_GetPetTiPo($PetInfoWnd_PetSlot);
		break;
	case 2: 	//精力
		%obj = "PetInfoWnd_PetJingLi";
		%content = PetInfoWnd_PetJingLi.getValue();
		%value = PetOperation_GetPetJingLi($PetInfoWnd_PetSlot);
		break;
	case 3:		//力道
		%obj = "PetInfoWnd_PetLiDao";
		%content = PetInfoWnd_PetLiDao.getValue();
		%value = PetOperation_GetPetLiDao($PetInfoWnd_PetSlot);
		break;
	case 4:		//灵力
		%obj = "PetInfoWnd_PetLingLi";
		%content = PetInfoWnd_PetLingLi.getValue();
		%value = PetOperation_GetPetLingLi($PetInfoWnd_PetSlot);
		break;
	case 5: 	//敏捷
		%obj = "PetInfoWnd_PetMingJie";
		%content = PetInfoWnd_PetMingJie.getValue();
		%value = PetOperation_GetPetMingJie($PetInfoWnd_PetSlot);
		break;
	case 6:		//元力
		%obj = "PetInfoWnd_PetYuanLi";
		%content = PetInfoWnd_PetYuanLi.getValue();
		%value = PetOperation_GetPetYuanLi($PetInfoWnd_PetSlot);
		break;
	default:
		return;
	}
	return (%content > %value);
}

function PetInfoWnd_PetAddProPerty(%nType, %nPts)
{
	%nLeftQianLi = PetInfoWnd_PetQianLi.getValue();
	if(%nLeftQianLi == 0)
		return;	
	switch(%nType)
	{
	case 1:		//体魄
		%obj = "PetInfoWnd_PetTiPo";
		%content = PetInfoWnd_PetTiPo.getValue();
		break;
	case 2: 	//精力
		%obj = "PetInfoWnd_PetJingLi";
		%content = PetInfoWnd_PetJingLi.getValue();
		break;
	case 3:		//力道
		%obj = "PetInfoWnd_PetLiDao";
		%content = PetInfoWnd_PetLiDao.getValue();
		break;
	case 4:		//灵力
		%obj = "PetInfoWnd_PetLingLi";
		%content = PetInfoWnd_PetLingLi.getValue();
		break;
	case 5: 	//敏捷
		%obj = "PetInfoWnd_PetMingJie";
		%content = PetInfoWnd_PetMingJie.getValue();
		break;
	case 6:		//元力
		%obj = "PetInfoWnd_PetYuanLi";
		%content = PetInfoWnd_PetYuanLi.getValue();
		break;
	default:
		return;
	}
	
	if (%nLeftQianLi >= %nPts)
	{
		%obj.setText(%content + %nPts);
		PetInfoWnd_PetQianLi.setText(%nLeftQianLi - %nPts);
	}
	else
	{
		%obj.setText(%content + %nLeftQianLi);
		PetInfoWnd_PetQianLi.setText("0");
	}
	
	PetInfoWnd_RefreshAddDecBtns();
}

function PetInfoWnd_PetDecProPerty(%nType, %nPts)
{
	%nLeftQianLi = PetInfoWnd_PetQianLi.getValue();
	switch(%nType)
	{
	case 1:		//体魄
		%obj = "PetInfoWnd_PetTiPo";
		%content = PetInfoWnd_PetTiPo.getValue();
		%value = PetOperation_GetPetTiPo($PetInfoWnd_PetSlot);
		break;
	case 2: 	//精力
		%obj = "PetInfoWnd_PetJingLi";
		%content = PetInfoWnd_PetJingLi.getValue();
		%value = PetOperation_GetPetJingLi($PetInfoWnd_PetSlot);
		break;
	case 3:		//力道
		%obj = "PetInfoWnd_PetLiDao";
		%content = PetInfoWnd_PetLiDao.getValue();
		%value = PetOperation_GetPetLiDao($PetInfoWnd_PetSlot);
		break;
	case 4:		//灵力
		%obj = "PetInfoWnd_PetLingLi";
		%content = PetInfoWnd_PetLingLi.getValue();
		%value = PetOperation_GetPetLingLi($PetInfoWnd_PetSlot);
		break;
	case 5: 	//敏捷
		%obj = "PetInfoWnd_PetMingJie";
		%content = PetInfoWnd_PetMingJie.getValue();
		%value = PetOperation_GetPetMingJie($PetInfoWnd_PetSlot);
		break;
	case 6:		//元力
		%obj = "PetInfoWnd_PetYuanLi";
		%content = PetInfoWnd_PetYuanLi.getValue();
		%value = PetOperation_GetPetYuanLi($PetInfoWnd_PetSlot);
		break;
	default:
		return;
	}
	
	if (%content > %value)
	{
		%nLeftPts = %content - %value;
		if (%nLeftPts >= %nPts)
		{	
			%obj.setText(%content - %nPts);
			PetInfoWnd_PetQianLi.setText(%nLeftQianLi + %nPts);
		}
		else
		{
			%obj.setText(%content - %nLeftPts);
			PetInfoWnd_PetQianLi.setText(%nLeftQianLi + %nLeftPts);
		}
	}
	
	PetInfoWnd_RefreshAddDecBtns();
}

function PetInfoWnd_PetChangeProperties()
{
	%nTiPo = PetInfoWnd_PetTiPo.getValue();
	%nJingLi = PetInfoWnd_PetJingLi.getValue();
	%nLiDao = PetInfoWnd_PetLiDao.getValue();
	%nLingLi = PetInfoWnd_PetLingLi.getValue();
	%nMinJie = PetInfoWnd_PetMingJie.getValue();
	%nYuanLi = PetInfoWnd_PetYuanLi.getValue();
	
	PetOperation_ChangeProperties($PetInfoWnd_PetSlot, %nTiPo, %nJingLi, %nLiDao, %nLingLi, %nMinJie, %nYuanLi);
}

function Mount_RefreshLeftTime(%slot)
{
	if ($PetInfoWnd_MountSlot != %slot || !PetInfoGui.isVisible())
		return;

	PetInfoWnd_setLeftTimeDesc();
}

function PetInfoWnd_setLeftTimeDesc()
{
	%nLeftTime = Mount_GetMountLeftTime($PetInfoWnd_MountSlot);	//以秒为单位
	
	/*%nMoney = %player.getMoney(1);
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;*/
	
	%nSecond = %nLeftTime % 60;
	%nMinute = (%nLeftTime - %nSecond) % 3600 / 60;
	%nHour = (%nLeftTime - %nSecond - %nMinute * 60) / 3600;
	%nDay = (%nLeftTime - %nSecond - %nMinute * 60 - %nHour * 3600) / 86400;
	
	echo("Day: " @ %nDay);
	echo("Hour: " @ %nHour);
	echo("Minute: " @ %nMinute);
	echo("Second: " @ %nSecond);
	
	%result = "";
	
	if (%nDay > 0)
		%result = %result @ %nDay @ "天";
	if (%nHour > 0)
		%result = %result @ %nHour @ "小时";
	if (%nMinute > 0)
		%result = %result @ %nDay @ "分钟";
	
	if (%nDay == 0 && %nHour == 0)
		%result = %result @ %nSecond @ "秒";
	
	PetInfoWnd_QiPet_LeftTime.setText(%result);
}

function UpdatePetInfo(%nSlot)
{
	if ($PetInfoWnd_PetSlot == %nSlot)
	{
		PetInfoWnd_RefreshWnd();	
	}
}

function ShowPetUpgradeHint()
{
	if (!PetInfoGui.isVisible())
	{	
		PetUpgradeHintButton.setVisible(1);
	}
}