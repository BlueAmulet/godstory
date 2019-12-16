//===========================================================================================================
// 文档说明:宠物强化界面操作
// 创建时间:2009-12-1
// 创建人: soar
//=========================================================================================================== 

function OpenPetStrengthenWnd()
{
	PetStrengthenWnd_Explain.setContent("可以提高灵兽的慧根，每次需要消耗1颗根骨丹，灵兽的慧根越高成功率就越低");
	PetStrengthenWnd_UseExplain.setContent("可右键单击灵兽列表内灵兽，或直接从灵兽栏内将灵兽拖进鉴定栏。");
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
		PetStrengthenWnd_PetName.setText( %sPetName @ " 慧根+" @ %nInsight);
		PetStrengthenWnd_NeedMaterialNum.setText(%nTotalMaterialNum @ "/1");
	}
	else
	{
		PetStrengthenWnd_PetName.setText("");
		PetStrengthenWnd_NeedMaterialNum.setText("0/0");
	}
	
	//宠物提高灵慧所需金钱
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
	
	//玩家拥有金元
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
	
		//玩家拥有金钱
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