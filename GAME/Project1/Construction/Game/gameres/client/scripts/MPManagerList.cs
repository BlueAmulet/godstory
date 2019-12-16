//===========================================================================================================
// 文档说明:师徒界面
// 创建时间:2010-02-02
// 创建人: thinking
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
			%familyName = "无";
		case 1:
			%familyName = "昆仑宗";
		case 2:
			%familyName = "雷音寺";
		case 3:
			%familyName = "蓬莱派";
		case 4:
			%familyName = "飞花谷";
		case 5:
			%familyName = "九幽教";
		case 6:
			%familyName = "山海宗";
		case 7:
			%familyName = "幻灵宫";
		case 8:
			%familyName = "天魔门";
	}
	switch(%status)
	{
		case 0:
			%statusName = "离线";
		case 1:
			%statusName = "空闲";
		case 2:
			%statusName = "副本";
		case 3:
			%statusName = "组队";
		case 4:
			%statusName = "战斗";
		case 5:
			%statusName = "摆摊";
		case 6:
			%statusName = "暂离";
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