//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Res.h"


enWarnMessage SoulLink::canSoulLink(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2)
{
	if(!player || !pShortCut1)
		return MSG_ITEM_ITEMDATAERROR;
	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortCut1);
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return MSG_ITEM_ITEMDATAERROR;
	if(!pRes->isBody() && !pRes->isWeapon())
		return MSG_UNKOWNERROR;
	if(pRes->getColorLevel() < Res::COLORLEVEL_GREEN)
		return MSG_UNKOWNERROR;
	
	
	return MSG_NONE;
}