//===========================================================================================================
// PowerEngine 
// Copyright (C) FireRain.com, Inc.

// 文档说明:物品分解
// 创建时间:2010-01-18
// 创建人: thinking
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