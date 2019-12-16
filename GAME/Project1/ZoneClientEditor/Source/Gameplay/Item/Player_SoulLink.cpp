//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Res.h"


enWarnMessage SoulLink::canSoulLink(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2)
{
	if(!player || !pShortCut1 || !pShortCut2)
		return MSG_ITEM_ITEMDATAERROR;
	ItemShortcut* pItem1 = dynamic_cast<ItemShortcut*>(pShortCut1);
	ItemShortcut* pItem2 = dynamic_cast<ItemShortcut*>(pShortCut2);
	Res* pRes1 = NULL;
	Res* pRes2 = NULL;
	if(!pItem1 || !(pRes1 = pItem1->getRes()))
		return MSG_ITEM_ITEMDATAERROR;
	if(!pItem2 || !(pRes2 = pItem2->getRes()))
		return MSG_ITEM_ITEMDATAERROR;
	//是否是装备
	if(!pRes2->isBody() && !pRes2->isWeapon())
		return MSG_UNKOWNERROR;
	//是否在绿装以上
	if(pRes2->getColorLevel() < Res::COLORLEVEL_GREEN)
		return MSG_UNKOWNERROR;
	//是否已经灵魂链接
	if(pRes2->getSoulLinkStatsID() > 0)
		return MSG_ITEM_HADINDENTIFIED;
	
	return MSG_NONE;
}

#ifdef NTJ_SERVER
enWarnMessage SoulLink::setSoulLink(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2)
{
	

	return MSG_NONE;
}
#endif