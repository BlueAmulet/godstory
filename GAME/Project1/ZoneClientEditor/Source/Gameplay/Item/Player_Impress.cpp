//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/ClientGameplayAction.h"
#include "Gameplay/ClientGameplayState.h"

enWarnMessage EquipImpress::canImpress(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2)
{
	if(!player || !pShortCut1 || !pShortCut2)
		return MSG_ITEM_ITEMDATAERROR;
	ItemShortcut* pItem1 = dynamic_cast<ItemShortcut*>(pShortCut1);
	ItemShortcut* pItem2 = dynamic_cast<ItemShortcut*>(pShortCut2);
	Res* pRes1 = NULL;
	Res* pRes2 = NULL;
	if(!pItem1 || !(pRes1 = pItem1->getRes()))
		return MSG_ITEM_ITEMDATAERROR;
	if(pRes1->getBaseAttribute() == 0)
		return MSG_ITEM_ITEMDATAERROR;
	if(!pItem2 || !(pRes2 = pItem2->getRes()))
		return MSG_ITEM_ITEMDATAERROR;
	//是否是装备
	if(!pRes2->isBody() && !pRes2->isWeapon())
		return MSG_UNKOWNERROR;
	return MSG_NONE;
}

#ifdef NTJ_SERVER
enWarnMessage EquipImpress::setImpress(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2)
{
	ItemShortcut* pItem1 = dynamic_cast<ItemShortcut*>(pShortCut1);
	ItemShortcut* pItem2 = dynamic_cast<ItemShortcut*>(pShortCut2);
	Res* pRes1 = pItem1->getRes();
	Res* pRes2 = pItem2->getRes();
	if(!pRes1 || !pRes2)
		return MSG_ITEM_ITEMDATAERROR;
	U32 iStatsID = pRes1->getBaseAttribute();
	pRes2->setImpressID(iStatsID);
	pRes2->setActivatePro(EAPF_IMPRESS);
	//
	//g_ItemManager->delItemFromInventory(player, pItem1);
	U32 iIndex = pShortCut1->getLockedItemIndex();
	player->inventoryList.SetSlot(iIndex, NULL);
	player->inventoryList.UpdateToClient(player->getControllingClient(), iIndex, ITEM_DROP);

	return MSG_NONE;
}
#endif

#ifdef NTJ_CLIENT
ConsoleFunction(doImpress, void, 2, 2, "doImpress(%index)")
{
	
}
ConsoleFunction(setImpressAction, void, 2, 2, "setImpressAction(%index)")
{
	U32 iIndex = dAtoi(argv[1]);
	ClientGameplayAction* action = new ClientGameplayAction(2, INFO_ITEM_IMPRESS);
	g_ClientGameplayState->setCurrentAction(action);

	ClientGameplayParam* param0 = new ClientGameplayParam;
	param0->setIntArgValues(1, iIndex);
	g_ClientGameplayState->setCurrentActionParam(param0);
}
#endif