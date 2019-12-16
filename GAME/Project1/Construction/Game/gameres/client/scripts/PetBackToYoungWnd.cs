//===========================================================================================================
// 文档说明:宠物还童界面操作
// 创建时间:2009-12-18
// 创建人: hly
//=========================================================================================================== 

function OpenPetHuanTongWnd()
{
	ForceOpenBag();
	PetBackToYoungWnd.setVisible(1);	
	OpenPetHelpWnd(4);
	PetBackToYoungWnd_OnRefreshWnd();
	%pos= PetBackToYoungWnd.getPosition();
	PetHelpWnd.setPosition(getWord(%pos,0)+339, getWord(%pos,1)+10);
	$IsDirty++;
}

function PetBackToYoungWnd::OnPositionChanged(%this, %x, %y)
{
	PetHelpWnd.setPosition(%x + 339, %y + 10);
}

function ClosePetHuanTongWnd()
{
	PetBackToYoungWnd.setVisible(0);
	ClosePetHelpWnd();
	$IsDirty--;
}

function PetBackToYoungWnd_OnRefreshWnd()
{
	PetBackToYoungWnd_PetName.setText( PetOperation_GetPetName(PetBackToYoungWnd_GetSlotIndex()) );
	
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	  
	//宠物还童所需金钱
	%nMoney = PetHuanTong_GetCostMoney();
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	if (%money1 > 0)
	{
		PetBackToYoungWnd_ExpendMoney_Gold.setVisible(1);
		PetBackToYoungWnd_ExpendMoney_GoldText.setVisible(1);
		PetBackToYoungWnd_ExpendMoney_GoldText.setText(%money1);
	}
	else
	{
		PetBackToYoungWnd_ExpendMoney_Gold.setVisible(0);
		PetBackToYoungWnd_ExpendMoney_GoldText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetBackToYoungWnd_ExpendMoney_Silver.setVisible(1);
		PetBackToYoungWnd_ExpendMoney_SilverText.setVisible(1);
		PetBackToYoungWnd_ExpendMoney_SilverText.setText(%money2);
	}
	else
	{
		PetBackToYoungWnd_ExpendMoney_Silver.setVisible(0);
		PetBackToYoungWnd_ExpendMoney_SilverText.setVisible(0);
	}
	PetBackToYoungWnd_ExpendMoney_CopperText.setText(%money3);
	
	//玩家拥有金元
	%nMoney = %player.getMoney(2);	
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	
	if (%money1 > 0)
	{
		PetBackToYoungWnd_CurrentlyMoney_GoldRock.setVisible(1);
		PetBackToYoungWnd_CurrentlyMoney_GoldRockText.setVisible(1);
		PetBackToYoungWnd_CurrentlyMoney_GoldRockText.setText(%money1);
	}
	else
	{
		PetBackToYoungWnd_CurrentlyMoney_GoldRock.setVisible(0);
		PetBackToYoungWnd_CurrentlyMoney_GoldRockText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetBackToYoungWnd_CurrentlyMoney_SilverRock.setVisible(1);
		PetBackToYoungWnd_CurrentlyMoney_SilverRockText.setVisible(1);
		PetBackToYoungWnd_CurrentlyMoney_SilverRockText.setText(%money2);
	}
	else
	{
		PetBackToYoungWnd_CurrentlyMoney_SilverRock.setVisible(0);
		PetBackToYoungWnd_CurrentlyMoney_SilverRockText.setVisible(0);
	}
	PetBackToYoungWnd_CurrentlyMoney_CopperRockText.setText(%money3);
	
		//玩家拥有金钱
	%nMoney = %player.getMoney(1);
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	
	if (%money1 > 0)
	{
		PetBackToYoungWnd_CurrentlyMoney_Gold.setVisible(1);
		PetBackToYoungWnd_CurrentlyMoney_GoldText.setVisible(1);
		PetBackToYoungWnd_CurrentlyMoney_GoldText.setText(%money1);
	}
	else
	{
		PetBackToYoungWnd_CurrentlyMoney_Gold.setVisible(0);
		PetBackToYoungWnd_CurrentlyMoney_GoldText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetBackToYoungWnd_CurrentlyMoney_Silver.setVisible(1);
		PetBackToYoungWnd_CurrentlyMoney_SilverText.setVisible(1);
		PetBackToYoungWnd_CurrentlyMoney_SilverText.setText(%money2);
	}
	else
	{
		PetBackToYoungWnd_CurrentlyMoney_Silver.setVisible(0);
		PetBackToYoungWnd_CurrentlyMoney_SilverText.setVisible(0);
	}
	
	PetBackToYoungWnd_CurrentlyMoney_CopperText.setText(%money3);
	
	
	//刷新还童按钮
	//PetBackToYoung_BackToYoungButton.SetActive(Pet_CanHuanTong());
}

function OnPetHuanTongSuccess()
{
	PetBackToYoungWnd_OnRefreshWnd();
}

function OnPetHuanTongFailure()
{
	
}