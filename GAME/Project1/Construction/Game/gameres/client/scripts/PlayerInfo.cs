//===========================================================================================================
// 文档说明:人物显示信息操作
// 创建时间:2009-6-9
// 创建人: soar
//=========================================================================================================== 

function RefreshPlayerInfoGui(%player)
{
	if(PlayerInfoGui.isvisible())
	{
	  PlayerInfo_HP.settext(%player.getHP() @ "/" @ %player.getMaxHP());
	  PlayerInfo_HPImage.setPercentum(%player.getHP()/%player.getMaxHP());
	  PlayerInfo_MP.settext(%player.getMP() @ "/" @ %player.getMaxMP());
	  PlayerInfo_MPImage.setPercentum(%player.getMP()/%player.getMaxMP());
	  PlayerInfo_PP.settext(%player.getPP() @ "/" @ %player.getMaxPP());
	  PlayerInfo_PPImage.setPercentum(%player.getPP()/%player.getMaxPP());
	  PlayerInfo_Vigor.setText(%player.getVigor() @ "/" @ %player.getMaxVigor());
	  PlayerInfo_VigorImage.setPercentum(%player.getVigor()/%player.getMaxVigor());
	  PlayerInfo_Stamina.settext(%player.getStamina());
	  PlayerInfo_Mana.settext(%player.getMana());
	  PlayerInfo_Strength.settext(%player.getStrength());
	  PlayerInfo_Intellect.settext(%player.getIntellect());
	  PlayerInfo_Pneuma.settext(%player.getPneuma());
	  PlayerInfo_Agility.settext(%player.getAgility());
	  PlayerInfo_PhyDamage.settext(%player.getPhyDamage());
	  PlayerInfo_MuDamage.settext(%player.getMuDamage());
	  PlayerInfo_HuoDamage.settext(%player.getHuoDamage());
	  PlayerInfo_TuDamage.settext(%player.getTuDamage());
	  PlayerInfo_JinDamage.settext(%player.getJinDamage());
	  PlayerInfo_ShuiDamage.settext(%player.getShuiDamage());
	  PlayerInfo_PneumaDamage.settext(%player.getPneumaDamage());
	  PlayerInfo_PhyDefence.settext(%player.getPhyDefence());
	  PlayerInfo_MuDefence.settext(%player.getMuDefence());
	  PlayerInfo_HuoDefence.settext(%player.getHuoDefence());
	  PlayerInfo_TuDefence.settext(%player.getTuDefence());
	  PlayerInfo_JinDefence.settext(%player.getJinDefence());
	  PlayerInfo_ShuiDefence.settext(%player.getShuiDefence());
	  PlayerInfo_PneumaDefence.settext(%player.getPneumaDefence());
	  PlayerInfo_Savvy.settext(%player.getInsight());
	  PlayerInfo_Jink.settext(%player.getDodgeRating());
	  PlayerInfo_FuryAttack.settext(%player.getCriticalRating());
	  PlayerInfo_FuryAttackOdds.settext(%player.getCriticalTimes() @ "%");
	  PlayerInfo_Toughness.settext(%player.getAntiCriticalRating());
	  PlayerInfo_Treatment.settext(%player.getHeal());
	}
}

function SetPlayerInfo_Name(%name)
{
	PlayerInfo_PlayerName.settext(%name);
}

function SetPlayerInfo_Level(%level)
{
	PlayerInfo_Level.settext(%level @ "级");
}

function SetPlayerInfo_Family(%family)
{
	
	switch(%player.getFamily())
	{
		case 0:
			PlayerInfo_Family.settext("无门宗"); 
			PlayerInfo_FamilyIcon.setvisible(0); 
		case 1:
			PlayerInfo_Family.settext("昆仑宗");
			PlayerInfo_FamilyIcon.setbitmap("gameres/gui/images/GUIWindow33_1_030.png");
			PlayerInfo_FamilyIcon.setvisible(1); 
		case 2:
			PlayerInfo_Family.settext("金禅寺");
			PlayerInfo_FamilyIcon.setbitmap("gameres/gui/images/GUIWindow33_1_031.png");
			PlayerInfo_FamilyIcon.setvisible(1); 
		case 3:
			PlayerInfo_Family.settext("蓬莱派");
			PlayerInfo_FamilyIcon.setbitmap("gameres/gui/images/GUIWindow33_1_032.png");
			PlayerInfo_FamilyIcon.setvisible(1); 
		case 4:
			PlayerInfo_Family.settext("飞花谷");
			PlayerInfo_FamilyIcon.setbitmap("gameres/gui/images/GUIWindow33_1_033.png");
			PlayerInfo_FamilyIcon.setvisible(1); 
		case 5:
			PlayerInfo_Family.settext("九幽教");
			PlayerInfo_FamilyIcon.setbitmap("gameres/gui/images/GUIWindow33_1_034.png");
			PlayerInfo_FamilyIcon.setvisible(1); 
		case 6:
			PlayerInfo_Family.settext("山海宗");
			PlayerInfo_FamilyIcon.setbitmap("gameres/gui/images/GUIWindow33_1_035.png");
			PlayerInfo_FamilyIcon.setvisible(1); 
		case 7:
			PlayerInfo_Family.settext("幻灵宫");
			PlayerInfo_FamilyIcon.setbitmap("gameres/gui/images/GUIWindow33_1_036.png");
			PlayerInfo_FamilyIcon.setvisible(1); 
		case 8:
			PlayerInfo_Family.settext("天魔门");
			PlayerInfo_FamilyIcon.setbitmap("gameres/gui/images/GUIWindow33_1_037.png");
			PlayerInfo_FamilyIcon.setvisible(1); 
	//	default:
			
	}

}

function ShowPlayerExp(%exp,%levelexp, %isfirst, %newexp, %oldexp)
{
	//去掉了代码人物修为
	PlayerInfo_NowExp.settext("" @ %exp @ "/" @ %levelexp);
	//如果经验已满且没有显示人物信息界面，则显示升级提示按钮
	if(%exp >= %levelexp && !PlayerInfoGui.IsVisible())
	{
		UpgradeHintButton.setVisible(1);
	}
	else
	{
		UpgradeHintButton.setVisible(0);
	}
	
	Exp.setPercentum(%exp/%levelexp);
	PlayerInfo_ExpImage.setPercentum(%exp/%levelexp);
	SkillStudyHavaExpTxt.settext(%exp);
	LivingSkillInfo_HavaExp.settext(%exp);
	%distance = %newexp - %oldexp;
	if(!%isfirst && %distance > 0)
	{
		AddChatMessage("13", "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x010101ff'>您获得了经验:" @ %distance @ "</t>", "0", "0");
	}
}

function ShowPlayerMoney(%gold,%silver,%copper)
{
	RefreshPlayerBagMoney(%gold,%silver,%copper);
	RefreshPlayerSkillMoney(%gold,%silver,%copper);
	RefreshPlayerLivingSkillMoney(%gold,%silver,%copper);
	RefreshPlayerEquipStrengthenMoney(%gold,%silver,%copper);
	RefreshPlayerEquipPunchHoleMoney(%gold,%silver,%copper);
	RefreshPlayerProduceMoney(%gold,%silver,%copper);
	RefreshPlayerIdentifyMoney(%gold,%silver,%copper);
	RefreshPlayerDurabilityMoney(%gold,%silver,%copper);
	RefreshPlayerItemRepairMoney(%gold,%silver,%copper);	
}

//灵元
function ShowPlayerBindMoney(%gold,%silver,%copper)
{
	RefreshPlayerBagBindMoney(%gold,%silver,%copper);
	RefreshPlayerSkillBindMoney(%gold,%silver,%copper);
	RefreshPlayerLivingSkillBindMoney(%gold,%silver,%copper);
	RefreshPlayerEquipStrengthenBindMoney(%gold,%silver,%copper);
	RefreshPlayerEquipPunchHoleBindMoney(%gold,%silver,%copper);
	RefreshPlayerProduceBindMoney(%gold,%silver,%copper);
	RefreshPlayerIdentifyBindMoney(%gold,%silver,%copper);
	RefreshPlayerDurabilityBindMoney(%gold,%silver,%copper);
	RefreshPlayerItemRepairBindMoney(%gold,%silver,%copper);
}

function OpenOrClosePlayerInfo()
{
	if(PlayerInfoGui.IsVisible())
	{
		ClosePlayerInfo();
	}
	else
	{
		UpgradeHintButton.setVisible(0);
		OpenPlayerInfo();
		refreshPlayerModelView();
	}
}

function OpenPlayerInfo()
{
	PlayOpenOrCloseGuiSound();
	GameMainWndGui_Layer3.PushToBack(PlayerInfoGui);
	PlayerInfoGui.setVisible(1);
	
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	showAllEquipQuality(); 
	RefreshPlayerInfoGui(%player);
	$IsDirty++;
}

function ClosePlayerInfo()
{
	PlayOpenOrCloseGuiSound();
	PlayerInfoGui.setVisible(0);
	$IsDirty--;
}

function Changeaccouterorfashion()
{
	if(PlayerInfo_Faction.IsStateOn())
	{
		PlayerInfo_FashionImage.setvisible(1);
		PlayerInfo_AccouterImage.setvisible(0);
		if(PlayerInfo_FamilyFashionButton.IsStateOn())
			quickChange(1);
		else
			quickChange(2);
	}
	else
	{
		PlayerInfo_FashionImage.setvisible(0);
		PlayerInfo_AccouterImage.setvisible(1);
		quickChange(0);
	}
}

function ShowAttackInfo()
{
	PlayerInfo_DefenceButton.setposition(7,363);
	PlayerInfo_OtherButton.setposition(7,380);
	PlayerInfo_Attack.setVisible(1);
	PlayerInfo_Defence.setVisible(0);
	PlayerInfo_Other.setVisible(0);
}

function ShowDefenceInfo()
{
	PlayerInfo_DefenceButton.setposition(7,215);
	PlayerInfo_OtherButton.setposition(7,380);
	PlayerInfo_Attack.setVisible(0);
	PlayerInfo_Defence.setVisible(1);
	PlayerInfo_Other.setVisible(0);
}

function ShowOtherInfo()
{
	PlayerInfo_DefenceButton.setposition(7,215);
	PlayerInfo_OtherButton.setposition(7,232);
	PlayerInfo_Attack.setVisible(0);
	PlayerInfo_Defence.setVisible(0);
	PlayerInfo_Other.setVisible(1);
}

function PlayerInfo_OpenRightWnd()
{
	PlayerInfoGui_RightWnd.setvisible(1);
	PlayerInfoGui.setextent(500,438);
	PlayerInfoGui_OpenRightWndButton.setvisible(0);
	PlayerInfoGui_CloseRightWndButton.setvisible(1);
}

function PlayerInfo_CloseRightWnd()
{
	PlayerInfoGui_RightWnd.setvisible(0);
	PlayerInfoGui.setextent(315,438);
	PlayerInfoGui_OpenRightWndButton.setvisible(1);
	PlayerInfoGui_CloseRightWndButton.setvisible(0);
}

function OpenPlayerFashionWnd()
{
	PlayerInfo_FashionButton.setvisible(0);
	PlayerInfo_AccouterButton.setvisible(1);
	PlayerInfoGui_Tab1_Accouter.setvisible(0);
	PlayerInfoGui_Tab1_Fashion.setvisible(1);
	PlayerInfoEquipShort_11.setVisible(0);
	PlayerInfoEquipShort_13.setVisible(0);
}

function OpenPlayerAccouterWnd()
{
	PlayerInfo_FashionButton.setvisible(1);
	PlayerInfo_AccouterButton.setvisible(0);
	PlayerInfoGui_Tab1_Accouter.setvisible(1);
	PlayerInfoGui_Tab1_Fashion.setvisible(0);
	PlayerInfoEquipShort_11.setVisible(1);
	PlayerInfoEquipShort_13.setVisible(1);
}

function setFaction(%flag)
{
	if(%flag == 1)
	{
		PlayerInfo_FashionImage.setvisible(1);
		PlayerInfo_AccouterImage.setvisible(0);	
		PlayerInfo_Faction.isStateOn = 1;
	}
	else
	{
		PlayerInfo_FashionImage.setvisible(0);
		PlayerInfo_AccouterImage.setvisible(1);
		PlayerInfo_Faction.isStateOn = 0;
	}
	
}

function setFamilyFaction(%flag)
{
	if(%flag == 1)
		PlayerInfo_FamilyFashionButton.setStateOn(1);
	else
		PlayerInfo_FamilyFashionButton.setStateOn(0);
}
function setChangeFaction()
{
	if(PlayerInfo_Faction.IsStateOn())
	{
		if(PlayerInfo_FamilyFashionButton.IsStateOn())
			quickChange(1);
		else
			quickChange(2);
	}
	else 
		quickChange(0);
}

function OpenPlayerInfoTab(%tab)
{
	if(%tab == 1)
	{
		PlayerInfoGui_Tab2.setVisible(0);   
		PlayerInfoGui_Tab1.setVisible(1);		
		PlayerInfoGui_ModelView.setVisible(1);
	}
	else
	{
		PlayerInfoGui_Tab1.setVisible(0); 
		PlayerInfoGui_ModelView.setVisible(0);
    PlayerInfoGui_Tab2.setVisible(1);     
    clearMpInfo();
    UpdataRelationLable();
	}
}    

function UpdataRelationLable()
{
	getMyselfMPInfo();
}

//更新师徒信息
function UpdataMpInfo(%TitleName,%curPrenNum,%maxPrenNum,%masterExp,%curExpPool,%maxExpPool)
{
	guiText_Mp_TitleName.setText(%TitleName);
	guiText_PrenticeNum.setText(%curPrenNum @ "/" @ %maxPrenNum);
	guiText_MasterExp.setText(%masterExp); 
	
	if(%maxExpPool == 0)
	  guiBitMap_ExpPool.setExtent(0,18);
	else
	  guiBitMap_ExpPool.setExtent((%curExpPool * 190)/%maxExpPool,18);
	guiText_ExpPool.setText(%curExpPool @ "/" @ %maxExpPool);
}

function clearMpInfo()
{
	guiText_Mp_TitleName.setText("");
	guiText_PrenticeNum.setText("");
	guiText_MasterExp.setText("");
	guiText_ExpPool.setText("");
	guiBitMap_ExpPool.setExtent(190,18);
}


// 显示装备物品对应人物属性窗口装备栏
function showEquipPos(%index, %isNoDrag)
{
	echo("===showDragToPos===");
	%obj = "PlayerInfoEquipShort_" @ %index;
	if(!isobject(%obj))
		return;
	
	if(!PlayerInfoGui.isVisible() && %isNoDrag)
		OpenPlayerInfo();
	if(PlayerInfoGui.isVisible())
		%obj.setEnableEffect(1, 3);
}


//========================================================================
//人物升级提示
//========================================================================
function ClosePlayerUpgradeHintWnd()
{
	if(PlayerUpgradeHintGui.IsVisible())
	{
		PlayerUpgradeHintGui.setVisible(0);
		$IsDirty--;
	}
}

function OpenPlayerUpgradeHintWnd(%level,%text)
{
	PlayerUpgradeHintGui.setVisible(1);
	$IsDirty--;
	PlayerUpgradeText.setContent(%text);
}

//function ClientEventLevelUp(%Id)
//{
//	echo("================fdsafasdfsa");
//	OpenPlayerUpgradeHintWnd(1,"蛮横无理在击中要害大地回春魂牵梦萦 地方工业非机动车 模压");
//}

function showAllEqupAndGemQuality(%equip, %gem)
{
	%text = "<t c= '0xb0f0b0ff' o =''>"@ %equip @ "/" @%gem @ "</t>";
	PlayerInfo_ItemQuality_1.setcontent(%text);
}