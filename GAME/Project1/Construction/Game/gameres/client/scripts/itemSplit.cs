//===========================================================================================================
// PowerEngine 
// Copyright (C) FireRain.com, Inc.

// �ĵ�˵��:��Ʒ�ֽ�
// ����ʱ��:2010-01-18
// ������: thinking
//===========================================================================================================
function openItemSplitGui()
{
	if(!itemSplitGui.isVisible())
	{
		GameMainWndGui_Layer3.pushToBack(itemSplitGui);
		itemSplitGui.setVisible(true);
		ForceOpenBag();
	}
}

function closeItemSplitGui()
{
	if(itemSplitGui.isVisible())
	{
		itemSplitGui.setVisible(false);
		uiItemSplitCancel();
	}
}