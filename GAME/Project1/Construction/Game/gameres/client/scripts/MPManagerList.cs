//===========================================================================================================
// �ĵ�˵��:ʦͽ����
// ����ʱ��:2010-02-02
// ������: thinking
//===========================================================================================================
function openPrenticeListWnd()
{
	if(!guiPrenticeList.isVisible())
	{
		GameMainWndGui_Layer3.pushToBack(guiPrenticeList) ;
	  guiPrenticeList.setVisible(true);
	  getPrenticeListInfo();
	}
}

function closePrenticeListWnd()
{
	if(guiPrenticeList.isVisible())
	{
		guiPrenticeList.setVisible(false);
		guiPrenticeList.clearData();
	}
}

function guiPrenticeList::clearData()
{
	mpm_Prentice_ListCtl.clear();
	mpm_Callboard.setText("");
}

function uiAddPrenticeList(%Id,%Name,%level,%family,%status)
{
	%familyName = "";
	%statusName = "";
	switch(%family)
	{
		case 0:
			%familyName = "��";
		case 1:
			%familyName = "������";
		case 2:
			%familyName = "������";
		case 3:
			%familyName = "������";
		case 4:
			%familyName = "�ɻ���";
		case 5:
			%familyName = "���Ľ�";
		case 6:
			%familyName = "ɽ����";
		case 7:
			%familyName = "���鹬";
		case 8:
			%familyName = "��ħ��";
	}
	switch(%status)
	{
		case 0:
			%statusName = "����";
		case 1:
			%statusName = "����";
		case 2:
			%statusName = "����";
		case 3:
			%statusName = "���";
		case 4:
			%statusName = "ս��";
		case 5:
			%statusName = "��̯";
		case 6:
			%statusName = "����";
	}
	
	%text = %Name TAB %level TAB %familyName TAB %statusName;
	mpm_Prentice_ListCtl.addRow(%Id,%text);
}

function uiUpdateMasterInfo(%masterName,%curNum,%maxNum,%string)
{
	mpm_MasterName.setText(%masterName);
	mpm_Master_PrenNum.setText(%curNum @ "/" @ %maxNum);
	mpm_Callboard.setText(%string);
}

function mpm_ChangeText_bnt::ChangeText(%this)
{
	%Text = mpm_Callboard.getText();
	if(%Text !$="" && %Text $= getMPMCallboardText())
	  return;
	else
	  setMPMCallboardText(%Text);
}