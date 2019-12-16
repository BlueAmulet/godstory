//===========================================================================================================
// PowerEngine 
// Copyright (C) FireRain.com, Inc.

// 文档说明:物品合成
// 创建时间:2010-01-18
// 创建人: thinking
//===========================================================================================================
function openItemComposeGui(%appendInfo)
{
	if(%appendInfo)
		itemComposeGui.setExtent(459,318);
	else
	  itemComposeGui.setExtent(295,318);
	  
	if(!itemComposeGui.isVisible())
	{
		GameMainWndGui_Layer3.pushToBack(itemComposeGui);
		itemComposeGui.setVisible(true);
		ForceOpenBag();
	}
}

function closeItemComposeGui()
{
	if(itemComposeGui.isVisible())
	{
		itemComposeGui.setVisible(false);
		itemComposeGui.setExtent(295,318);
		itemSplit_ItemDesc_Text.setText("");
		itemSplit_Material_Text.setContent("");
		uiItemComposeCancel();
	}
}

function updateItemComposeInfo(%itemID,%TextInfo)
{
	 %text =  "<l i='" @ %itemID @ "' t='itemid'/>";
	 itemSplit_ItemDesc_Text.setContent(%text);
	 itemSplit_Material_Text.setContent(%TextInfo);
}