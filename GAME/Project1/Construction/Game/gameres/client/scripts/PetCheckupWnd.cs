//===========================================================================================================
// 文档说明:宠物鉴定界面操作
// 创建时间:2009-12-1
// 创建人: soar
//=========================================================================================================== 

function OpenPetCheckupWnd()
{
	PetCheckupWnd.setVisible(1);	
	OpenPetHelpWnd(1);
	PetCheckupWnd_OnRefreshWnd();
	%pos= PetCheckupWnd.getPosition();
	PetHelpWnd.setPosition(getWord(%pos,0)+270, getWord(%pos,1)+10);
	$IsDirty++;
}

function PetCheckupWnd::OnPositionChanged(%this, %x, %y)
{
	PetHelpWnd.setPosition(%x + 270, %y + 10);
}

function ClosePetCheckupWnd()
{
	PetCheckupWnd.setVisible(0);
	ClosePetHelpWnd();
	$IsDirty--;
}

function PetCheckupWnd_OnRefreshWnd()
{
	PetCheckupWnd_PetName.setText( PetOperation_GetPetName(PetCheckupWnd_GetSlotIndex()) );
	
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	
	//宠物鉴定所需金钱
	%nMoney = PetIdentify_GetCostMoney();
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	if (%money1 > 0)
	{
		PetCheckupWnd_ExpendMoney_Gold.setVisible(1);
		PetCheckupWnd_ExpendMoney_GoldText.setVisible(1);
		PetCheckupWnd_ExpendMoney_GoldText.setText(%money1);
	}
	else
	{
		PetCheckupWnd_ExpendMoney_Gold.setVisible(0);
		PetCheckupWnd_ExpendMoney_GoldText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetCheckupWnd_ExpendMoney_Silver.setVisible(1);
		PetCheckupWnd_ExpendMoney_SilverText.setVisible(1);
		PetCheckupWnd_ExpendMoney_SilverText.setText(%money2);
	}
	else
	{
		PetCheckupWnd_ExpendMoney_Silver.setVisible(0);
		PetCheckupWnd_ExpendMoney_SilverText.setVisible(0);
	}
	PetCheckupWnd_ExpendMoney_CopperText.setText(%money3);
	
	//玩家拥有金元
	%nMoney = %player.getMoney(2);	
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	
	if (%money1 > 0)
	{
		PetCheckupWnd_CurrentlyMoney_GoldRock.setVisible(1);
		PetCheckupWnd_CurrentlyMoney_GoldRockText.setVisible(1);
		PetCheckupWnd_CurrentlyMoney_GoldRockText.setText(%money1);
	}
	else
	{
		PetCheckupWnd_CurrentlyMoney_GoldRock.setVisible(0);
		PetCheckupWnd_CurrentlyMoney_GoldRockText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetCheckupWnd_CurrentlyMoney_SilverRock.setVisible(1);
		PetCheckupWnd_CurrentlyMoney_SilverRockText.setVisible(1);
		PetCheckupWnd_CurrentlyMoney_SilverRockText.setText(%money2);
	}
	else
	{
		PetCheckupWnd_CurrentlyMoney_SilverRock.setVisible(0);
		PetCheckupWnd_CurrentlyMoney_SilverRockText.setVisible(0);
	}
	PetCheckupWnd_CurrentlyMoney_CopperRockText.setText(%money3);
	
		//玩家拥有金钱
	%nMoney = %player.getMoney(1);
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	
	if (%money1 > 0)
	{
		PetCheckupWnd_CurrentlyMoney_Gold.setVisible(1);
		PetCheckupWnd_CurrentlyMoney_GoldText.setVisible(1);
		PetCheckupWnd_CurrentlyMoney_GoldText.setText(%money1);
	}
	else
	{
		PetCheckupWnd_CurrentlyMoney_Gold.setVisible(0);
		PetCheckupWnd_CurrentlyMoney_GoldText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetCheckupWnd_CurrentlyMoney_Silver.setVisible(1);
		PetCheckupWnd_CurrentlyMoney_SilverText.setVisible(1);
		PetCheckupWnd_CurrentlyMoney_SilverText.setText(%money2);
	}
	else
	{
		PetCheckupWnd_CurrentlyMoney_Silver.setVisible(0);
		PetCheckupWnd_CurrentlyMoney_SilverText.setVisible(0);
	}
	
	PetCheckupWnd_CurrentlyMoney_CopperText.setText(%money3);
	
	//刷新成长率结果
	%nChengZhangLv = PetOperation_GetPetChengZhangLv(PetCheckupWnd_GetSlotIndex());
	if (%nChengZhangLv > 0)
	{
		%desc = getPetChengZhangLvDesc(%nChengZhangLv);
		%result = "<t v='1' c='0xFFFFFFFF'>成长率：</t><t v='1' c='0xFFFFFFFF'>" @ %desc @ "（" @ %nChengZhangLv @ "）</t>";
		PetCheckupWnd_PetCresciveOdds.setContent(%result);
	}
	else
	{
		PetCheckupWnd_PetCresciveOdds.setContent("");
	}
	
	//刷新鉴定按钮
	//PetCheckupWnd_CheckupButton.SetActive(Pet_CanIdentify());
}

function OnPetIdentifySuccess()
{
	PetCheckupWnd_OnRefreshWnd();
}

function OnPetIdentifyFailure()
{
	
}

function getPetChengZhangLvDesc(%chengZhangLv)
{
	%fChengZhangLv = %chengZhangLv / 20.0;
	%desc = "普通";
	if (%fChengZhangLv >= 0.9 && %fChengZhangLv < 42)
		%desc = "普通";
	else if (%fChengZhangLv >= 42 && %fChengZhangLv < 78)
		%desc = "精良";
	else if (%fChengZhangLv >= 78 && %fChengZhangLv < 111)
		%desc = "优秀";
	else if (%fChengZhangLv >= 111 && %fChengZhangLv < 143)
		%desc = "完美";
	else if (%fChengZhangLv >= 143 && %fChengZhangLv < 165)
		%desc = "传说";
	else if (%fChengZhangLv >= 165)
		%desc = "逆天";
		
	return %desc;
}