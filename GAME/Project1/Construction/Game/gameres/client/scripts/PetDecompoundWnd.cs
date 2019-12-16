//===========================================================================================================
// �ĵ�˵��:���������������
// ����ʱ��:2009-12-1
// ������: soar
//=========================================================================================================== 

function OpenPetDecompoundWnd()
{
	GameMainWndGui_Layer3.PushToBack(PetDecompoundWnd);
	ForceOpenBag();
	PetDecompoundWnd.setVisible(1);
	OpenPetHelpWnd(3);
	PetDecompoundWnd_OnRefreshWnd();
	%pos= PetDecompoundWnd.getPosition();
	PetHelpWnd.setPosition(getWord(%pos,0)+270, getWord(%pos,1)+10);
	$IsDirty++;
}

function PetDecompoundWnd::OnPositionChanged(%this, %x, %y)
{
	PetHelpWnd.setPosition(%x + 270, %y + 10);
}

function ClosePetDecompoundWnd()
{
	PetDecompoundWnd.setVisible(0);
	ClosePetHelpWnd();
	$IsDirty--;
}

function PetDecompoundWnd_OnRefreshWnd()
{
	PetDecompoundWnd_PetName.setText( PetOperation_GetPetName(PetDecompoundWnd_GetSlotIndex()) );
	
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	  
	//�õ����ǵ�shortcut
	%nGenGuId = PetOperation_PetLianHua_InitGenGuDan();
	//�������������Ǯ
	%nMoney = PetLianHua_GetCostMoney();
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	if (%money1 > 0)
	{
		PetDecompoundWnd_ExpendMoney_Gold.setVisible(1);
		PetDecompoundWnd_ExpendMoney_GoldText.setVisible(1);
		PetDecompoundWnd_ExpendMoney_GoldText.setText(%money1);
	}
	else
	{
		PetDecompoundWnd_ExpendMoney_Gold.setVisible(0);
		PetDecompoundWnd_ExpendMoney_GoldText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetDecompoundWnd_ExpendMoney_Silver.setVisible(1);
		PetDecompoundWnd_ExpendMoney_SilverText.setVisible(1);
		PetDecompoundWnd_ExpendMoney_SilverText.setText(%money2);
	}
	else
	{
		PetDecompoundWnd_ExpendMoney_Silver.setVisible(0);
		PetDecompoundWnd_ExpendMoney_SilverText.setVisible(0);
	}
	PetDecompoundWnd_ExpendMoney_CopperText.setText(%money3);
	
	//���ӵ�н�Ԫ
	%nMoney = %player.getMoney(2);	
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	
	if (%money1 > 0)
	{
		PetDecompoundWnd_CurrentlyMoney_GoldRock.setVisible(1);
		PetDecompoundWnd_CurrentlyMoney_GoldRockText.setVisible(1);
		PetDecompoundWnd_CurrentlyMoney_GoldRockText.setText(%money1);
	}
	else
	{
		PetDecompoundWnd_CurrentlyMoney_GoldRock.setVisible(0);
		PetDecompoundWnd_CurrentlyMoney_GoldRockText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetDecompoundWnd_CurrentlyMoney_SilverRock.setVisible(1);
		PetDecompoundWnd_CurrentlyMoney_SilverRockText.setVisible(1);
		PetDecompoundWnd_CurrentlyMoney_SilverRockText.setText(%money2);
	}
	else
	{
		PetDecompoundWnd_CurrentlyMoney_SilverRock.setVisible(0);
		PetDecompoundWnd_CurrentlyMoney_SilverRockText.setVisible(0);
	}
	PetDecompoundWnd_CurrentlyMoney_CopperRockText.setText(%money3);
	
		//���ӵ�н�Ǯ
	%nMoney = %player.getMoney(1);
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	
	if (%money1 > 0)
	{
		PetDecompoundWnd_CurrentlyMoney_Gold.setVisible(1);
		PetDecompoundWnd_CurrentlyMoney_GoldText.setVisible(1);
		PetDecompoundWnd_CurrentlyMoney_GoldText.setText(%money1);
	}
	else
	{
		PetDecompoundWnd_CurrentlyMoney_Gold.setVisible(0);
		PetDecompoundWnd_CurrentlyMoney_GoldText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetDecompoundWnd_CurrentlyMoney_Silver.setVisible(1);
		PetDecompoundWnd_CurrentlyMoney_SilverText.setVisible(1);
		PetDecompoundWnd_CurrentlyMoney_SilverText.setText(%money2);
	}
	else
	{
		PetDecompoundWnd_CurrentlyMoney_Silver.setVisible(0);
		PetDecompoundWnd_CurrentlyMoney_SilverText.setVisible(0);
	}
	
	PetDecompoundWnd_CurrentlyMoney_CopperText.setText(%money3);
}

function OnPetLianHuaSuccess()
{
	$PetHelpList_SelectedIndex = -1;
	PetHelpList_Clear();
	PetHelpList_Refresh(3);
	PetDecompoundWnd_OnRefreshWnd();
}

function OnPetLianHuaFailure()
{
	
}