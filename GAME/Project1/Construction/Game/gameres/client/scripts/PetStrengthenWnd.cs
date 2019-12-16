//===========================================================================================================
// �ĵ�˵��:����ǿ���������
// ����ʱ��:2009-12-1
// ������: soar
//=========================================================================================================== 

function OpenPetStrengthenWnd()
{
	PetStrengthenWnd_Explain.setContent("����������޵Ļ۸���ÿ����Ҫ����1�Ÿ��ǵ������޵Ļ۸�Խ�߳ɹ��ʾ�Խ��");
	PetStrengthenWnd_UseExplain.setContent("���Ҽ����������б������ޣ���ֱ�Ӵ��������ڽ������Ͻ���������");
	PetStrengthenWnd.setVisible(1);
	OpenPetHelpWnd(2);
	PetStrengthenWnd_OnRefreshWnd();
	%pos= PetStrengthenWnd.getPosition();
	PetHelpWnd.setPosition(getWord(%pos,0)+339, getWord(%pos,1)+10);
	$IsDirty++;
}

function PetStrengthenWnd::OnPositionChanged(%this, %x, %y)
{
	PetHelpWnd.setPosition(%x + 339, %y + 10);
}

function ClosePetStrengthenWnd()
{
	PetStrengthenWnd.setVisible(0);
	ClosePetHelpWnd();
	$IsDirty--;
}

function PetStrengthenWnd_OnRefreshWnd()
{
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	  
	%nGenGuId = PetOperation_PetInsight_InitGenGuDan();
	%nPetIndex = PetStrengthenWnd_GetSlotIndex();
	if (%nPetIndex != -1)
	{
		%sPetName = PetOperation_GetPetName(%nPetIndex);
		%nInsight = PetOperation_GetPetHuiGen(%nPetIndex);
		
		%nTotalMaterialNum = %player.getItemCount(%nGenGuId);
		PetStrengthenWnd_PetName.setText( %sPetName @ " �۸�+" @ %nInsight);
		PetStrengthenWnd_NeedMaterialNum.setText(%nTotalMaterialNum @ "/1");
	}
	else
	{
		PetStrengthenWnd_PetName.setText("");
		PetStrengthenWnd_NeedMaterialNum.setText("0/0");
	}
	
	//���������������Ǯ
	%nMoney = PetInsight_GetCostMoney();
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	if (%money1 > 0)
	{
		PetStrengthenWnd_ExpendMoney_Gold.setVisible(1);
		PetStrengthenWnd_ExpendMoney_GoldText.setVisible(1);
		PetStrengthenWnd_ExpendMoney_GoldText.setText(%money1);
	}
	else
	{
		PetStrengthenWnd_ExpendMoney_Gold.setVisible(0);
		PetStrengthenWnd_ExpendMoney_GoldText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetStrengthenWnd_ExpendMoney_Silver.setVisible(1);
		PetStrengthenWnd_ExpendMoney_SilverText.setVisible(1);
		PetStrengthenWnd_ExpendMoney_SilverText.setText(%money2);
	}
	else
	{
		PetStrengthenWnd_ExpendMoney_Silver.setVisible(0);
		PetStrengthenWnd_ExpendMoney_SilverText.setVisible(0);
	}
	PetStrengthenWnd_ExpendMoney_CopperText.setText(%money3);
	
	//���ӵ�н�Ԫ
	%nMoney = %player.getMoney(2);	
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	
	if (%money1 > 0)
	{
		PetStrengthenWnd_CurrentlyMoney_GoldRock.setVisible(1);
		PetStrengthenWnd_CurrentlyMoney_GoldRockText.setVisible(1);
		PetStrengthenWnd_CurrentlyMoney_GoldRockText.setText(%money1);
	}
	else
	{
		PetStrengthenWnd_CurrentlyMoney_GoldRock.setVisible(0);
		PetStrengthenWnd_CurrentlyMoney_GoldRockText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetStrengthenWnd_CurrentlyMoney_SilverRock.setVisible(1);
		PetStrengthenWnd_CurrentlyMoney_SilverRockText.setVisible(1);
		PetStrengthenWnd_CurrentlyMoney_SilverRockText.setText(%money2);
	}
	else
	{
		PetStrengthenWnd_CurrentlyMoney_SilverRock.setVisible(0);
		PetStrengthenWnd_CurrentlyMoney_SilverRockText.setVisible(0);
	}
	PetStrengthenWnd_CurrentlyMoney_CopperRockText.setText(%money3);
	
		//���ӵ�н�Ǯ
	%nMoney = %player.getMoney(1);
	%money3 = %nMoney % 100;
	%money2 = (%nMoney - %money3) % 10000 / 100;
	%money1 = (%nMoney - %money3 - %money2 * 100) / 10000;
	
	if (%money1 > 0)
	{
		PetStrengthenWnd_CurrentlyMoney_Gold.setVisible(1);
		PetStrengthenWnd_CurrentlyMoney_GoldText.setVisible(1);
		PetStrengthenWnd_CurrentlyMoney_GoldText.setText(%money1);
	}
	else
	{
		PetStrengthenWnd_CurrentlyMoney_Gold.setVisible(0);
		PetStrengthenWnd_CurrentlyMoney_GoldText.setVisible(0);
	}
	
	if (%money2 > 0)
	{
		PetStrengthenWnd_CurrentlyMoney_Silver.setVisible(1);
		PetStrengthenWnd_CurrentlyMoney_SilverText.setVisible(1);
		PetStrengthenWnd_CurrentlyMoney_SilverText.setText(%money2);
	}
	else
	{
		PetStrengthenWnd_CurrentlyMoney_Silver.setVisible(0);
		PetStrengthenWnd_CurrentlyMoney_SilverText.setVisible(0);
	}
	
	PetStrengthenWnd_CurrentlyMoney_CopperText.setText(%money3);
}

function OnPetInsightSuccess()
{
	PetStrengthenWnd_OnRefreshWnd();
}

function OnPetInsightFailure()
{
	
}