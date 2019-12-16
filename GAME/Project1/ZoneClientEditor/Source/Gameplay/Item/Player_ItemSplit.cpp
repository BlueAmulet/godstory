//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "util/stringUnit.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/Item/Player_ItemSplit.h"


ItemSplit::ItemSplit() :BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_ITEMSPLIT;
	mLock = false;
}

enWarnMessage ItemSplit::canItemSplit(Player* pPlayer)
{
	if(!pPlayer)
		return MSG_UNKOWNERROR;

	//交互状态
	enWarnMessage msg = MSG_NONE;
	msg = pPlayer->isBusy(Player::INTERACTION_NONE);
	if(msg != MSG_NONE)
		return msg;

	//坐骑状态
	if(pPlayer->isMounted())
		return MSG_PLAYER_RIDE;

	ItemShortcut* pItem = NULL;
	//检查是否还有材料未收取
	bool bSplit = true;
	for (U8 i=1; i<MAXSLOTS; i++)
	{
		if(!mSlots[i] || !(pItem = dynamic_cast<ItemShortcut*>(mSlots[i])))
		{
			continue;
		}
		else
		{
			bSplit = false;
			break;
		}
	}

	if(!bSplit)
		return MSG_ITEMSPLIT_NOSPLIT;

	ShortcutObject* pShortcut = mSlots[0];
	if(!pShortcut)
		return MSG_ITEMSPLIT_NOSHORTCUT;

	if(!pShortcut->isItemObject())
		return MSG_ITEM_CANNOOPERATE;

	pItem = dynamic_cast<ItemShortcut*>(pShortcut);
	if(!pItem || !pItem->getRes())
		return MSG_ITEM_ITEMDATAERROR;

	return MSG_NONE;
}

bool ItemSplit::startItemSplit(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	enWarnMessage msg = pPlayer->mItemSplitList.canItemSplit(pPlayer);

#ifdef NTJ_CLIENT
		if(msg == MSG_NONE)
			return sendToStartEvent(pPlayer);
		else
			MessageEvent::show(SHOWTYPE_ERROR,msg);
#endif

#ifdef NTJ_SERVER
		if(msg == MSG_NONE)
		{
			ShortcutObject* pShortcut = mSlots[0];
			if(!pShortcut || !pShortcut->isItemObject())
				return false;

			Res* pRes = NULL;
			ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
			if(!pItem || !(pRes = pItem->getRes()))
				return false;

			std::string szItemInfo;
			szItemInfo = Con::executef("serverItemSplit",
				              Con::getIntArg(pPlayer->getId()),
							  Con::getIntArg(pRes->getItemID()),
							  Con::getIntArg(pRes->getQuantity()));
			//分解失败
			if (dStrcmp(szItemInfo.c_str(),"0") == 0)
			{
				MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR,MSG_ITEMSPLIT_ERROR);
				return false;
			}

			//删除包裹栏物品
			S32 iIndex = pShortcut->getLockedItemIndex();
			pPlayer->inventoryList.SetSlot(iIndex,NULL);
			pPlayer->inventoryList.UpdateToClient(conn,iIndex,ITEM_SPLIT);

			//删除分解栏物品
			SAFE_DELETE(mSlots[0]);
			UpdateToClient(conn,0);
			

			//添加分解后的材料
			U8 fieldCount = StringUnit::getUnitCount(szItemInfo.c_str(),"|\n");
			U32 itemId =0;
			U16 quantity =0;
			std::string fieldSting;
			for (U8 i=0; i<fieldCount; i++)
			{
				fieldSting = StringUnit::getUnit(szItemInfo.c_str(),i,"|\n");
				itemId = atoi(StringUnit::getUnit(fieldSting.c_str(),0," \n"));
				quantity= atoi(StringUnit::getUnit(fieldSting.c_str(),1," \n"));
				mSlots[i+1] = ItemShortcut::CreateItem(itemId,quantity);
				UpdateToClient(pPlayer->getControllingClient(),i+1);
			}
			return true;
		}
		else
			MessageEvent::send(conn,SHOWTYPE_ERROR,msg);
#endif
		return false;
}

void ItemSplit::finishItemSplit(Player* pPlayer)
{
	if(!pPlayer)
		return;

	if(isLock())
		setLock(false);

#ifdef NTJ_SERVER
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

	ServerGameNetEvent* pEvent =  new ServerGameNetEvent(INFO_ITEMSPLIT);
	pEvent->SetIntArgValues(1,ITEMSPLIT_END);
	conn->postNetEvent(pEvent);
#endif
#ifdef NTJ_CLIENT
	Con::executef("CloseFetchTimeProgress");
#endif
	
}

#ifdef NTJ_SERVER
bool ItemSplit::LoadData(stPlayerStruct* pPlayerData)
{
	for (int i=1; i<MAXSLOTS; i++)
	{
		stShortCutInfo& pShortCutInfo = pPlayerData->MainData.SplitInfo[i-1];
		if(pShortCutInfo.ShortCutID == 0 && pShortCutInfo.ShortCutType != ShortcutObject::SHORTCUT_ITEM)
			continue;

		mSlots[i] = ItemShortcut::CreateItem(pShortCutInfo.ShortCutID,pShortCutInfo.ShortCutNum);
	}

	return true;
}

bool ItemSplit::UpdateToClient(GameConnection* conn, S32 index)
{
	if(!conn || index<0 || index >= MAXSLOTS)
		return false;

	ItemSplitEvent* pEvent = new ItemSplitEvent(index);
	conn->postNetEvent(pEvent);
	return true;
}

bool ItemSplit::SaveData(stPlayerStruct* pPlayerData)
{
	ShortcutObject* pShortCutObj = NULL;
	ItemShortcut* pItem = NULL;
	Res* pRes = NULL;
	for (int i=1;i<MAXSLOTS; i++)
	{
		pShortCutObj = mSlots[i];
		if(!pShortCutObj || !pShortCutObj->isItemObject())
			continue;

		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortCutObj);
		if(!pItem || !(pRes = pItem->getRes()))
			continue;

		pPlayerData->MainData.SplitInfo[i-1].ShortCutID = pRes->getItemID();
		pPlayerData->MainData.SplitInfo[i-1].ShortCutNum = pRes->getQuantity();
		pPlayerData->MainData.SplitInfo[i-1].ShortCutType = ShortcutObject::SHORTCUT_ITEM;
	}

	return true;
}

bool ItemSplit::sendInitialData(Player* pPlayer)
{
	if(!pPlayer)
		return false;

	for (int i=0; i<MAXSLOTS; i++)
		UpdateToClient(pPlayer->getControllingClient(),i);

	return true;
}

void ItemSplit::cancelItemSplit(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return;

	if(!mSlots[0])
		return;

	S32 iIndex = mSlots[0]->getLockedItemIndex();

	//删除分解栏物品
	SAFE_DELETE(mSlots[0]);
	UpdateToClient(conn,0);

	if(isLock())
		setLock(false);

	//解除包裹栏锁定
	ShortcutObject* pShortcut = pPlayer->inventoryList.GetSlot(iIndex);
	if(!pShortcut)
		return;

	pShortcut->setSlotState(ShortcutObject::SLOT_COMMON);
	pPlayer->inventoryList.UpdateToClient(conn,iIndex,ITEM_NOSHOW);

	if(pPlayer->pScheduleEvent)
		pPlayer->pScheduleEvent->cancelEvent(pPlayer);
}

enWarnMessage ItemSplit::pickupAllItem(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return MSG_UNKOWNERROR;

	ItemShortcut* pItem = NULL;
	Res* pRes = NULL;
	enWarnMessage msg = MSG_NONE;

	for (U8 i=1; i<MAXSLOTS; i++)
	{
		if(!mSlots[i])
			continue;
		pItem = dynamic_cast<ItemShortcut*>(mSlots[i]);
		if(!pItem || !(pRes = pItem->getRes()))
			continue;

		U32 playerId = pPlayer->getPlayerID();
		g_ItemManager->putItem(playerId,pRes->getItemID(),pRes->getQuantity());
		msg = g_ItemManager->batchItem(playerId);
		if(msg != MSG_NONE)
			return msg;

		SAFE_DELETE(mSlots[i]);
		UpdateToClient(conn,i);
	}

	return msg;
}

enWarnMessage ItemSplit::pickupItemByIndex(Player* pPlayer,U8 index)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return MSG_UNKOWNERROR;
	
	if(index < 1 || index >= MAXSLOTS)
		return MSG_ITEMSPLIT_INDEXERROR;

	ShortcutObject* pShortcut = mSlots[index];
	if(!pShortcut)
		return MSG_ITEMSPLIT_NOSHORTCUT;

	if(!pShortcut->isItemObject())
		return MSG_ITEM_CANNOTPICKUP;

	Res* pRes = NULL;
	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
	if(!pItem || !(pRes =  pItem->getRes()))
		return MSG_ITEMSPLIT_NOSHORTCUT;
	
	U32 playerId = pPlayer->getPlayerID();
	g_ItemManager->putItem(playerId,pRes->getItemID(),pRes->getQuantity());
	enWarnMessage msg = MSG_NONE;
	msg = g_ItemManager->batchItem(playerId);
	if(msg != MSG_NONE)
		return msg;

	SAFE_DELETE(mSlots[index]);
	UpdateToClient(conn,index);

	return msg;
}

enWarnMessage ItemSplit::InventoryToItemSplit(stExChangeParam* param)
{
	if(isLock())
		return MSG_ITEMSPLIT_ISLOCK;

	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	//参数有效性检查
	if(!param)
		return msg;

	GameConnection* conn = NULL;
	Player* pPlayer = param->player;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return msg;

	//位置检查
	if(param->DestIndex != 0 || !pPlayer->inventoryList.IsVaildSlot(param->SrcIndex))
		return msg;

	ShortcutObject* pShortcutSrc  = pPlayer->inventoryList.GetSlot(param->SrcIndex);
	ShortcutObject* pShortcutDest =  mSlots[param->DestIndex];
	
	Res* pRes = NULL;
	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcutSrc);
	if(!pItem || !(pRes = pItem->getRes()))
		return MSG_ITEM_ITEMDATAERROR;

	//物品锁定状态
	if(pItem->getSlotState() != ShortcutObject::SLOT_COMMON)
		return MSG_ITEM_CANNOTDRAG;

	ItemShortcut* pNewItem = ItemShortcut::CreateItem(pRes->getItemID(),pRes->getQuantity());
	if(!pNewItem)
		return MSG_ITEM_ITEMDATAERROR;

	//目标是否为空
	if(pShortcutDest)
	{
		S32 iIndex = pShortcutDest->getLockedItemIndex();
		ShortcutObject* pOldShortcut = pPlayer->inventoryList.GetSlot(iIndex);
		if(pOldShortcut)
		{
			pOldShortcut->setSlotState(ShortcutObject::SLOT_COMMON);
		    pPlayer->inventoryList.UpdateToClient(conn,iIndex,ITEM_NOSHOW);
		}
	}

	pShortcutSrc->setSlotState(ShortcutObject::SLOT_LOCK);
	pNewItem->setLockedItemIndex(param->SrcIndex);
	pPlayer->mItemSplitList.SetSlot(param->DestIndex,pNewItem);
	pPlayer->inventoryList.UpdateToClient(conn,param->SrcIndex,ITEM_NOSHOW);
	UpdateToClient(conn,param->DestIndex);

	return MSG_NONE;
}

enWarnMessage ItemSplit::ItemSplitToInventory(stExChangeParam* param)
{
	if(isLock())
		return MSG_ITEMSPLIT_ISLOCK;

	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	//参数有效性检查
	if(!param)
		return msg;

	GameConnection* conn = NULL;
	Player* pPlayer = param->player;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return msg;

	//位置检查
	if(!pPlayer->mItemComposeList.IsVaildSlot(param->SrcIndex) || !pPlayer->inventoryList.IsVaildSlot(param->DestIndex))
		return msg;

	ShortcutObject* pShortcutSrc  = mSlots[param->SrcIndex];
	ShortcutObject* pShortcutDest = pPlayer->inventoryList.GetSlot(param->DestIndex);
	if(!pShortcutSrc)
		return MSG_ITEM_ITEMDATAERROR;

	Res* pRes = NULL;
	ItemShortcut* pItem = NULL;

	if(param->SrcIndex == 0)
	{
		ShortcutObject* pOldShortcut = NULL;
		S32 iIndex = pShortcutSrc->getLockedItemIndex();

		//原有包裹栏物品
		pOldShortcut = pPlayer->inventoryList.GetSlot(iIndex);
		if(!pOldShortcut)
			return MSG_ITEM_ITEMDATAERROR;

		SAFE_DELETE(mSlots[param->SrcIndex]);
		UpdateToClient(conn,param->SrcIndex);

		pOldShortcut->setSlotState(ShortcutObject::SLOT_COMMON);
		pPlayer->inventoryList.UpdateToClient(conn,iIndex,ITEM_NOSHOW);

		//空槽位或原槽位
		if(!pShortcutDest || iIndex == param->DestIndex)
			return MSG_NONE;

		//交换目标物品
		pItem = dynamic_cast<ItemShortcut*>(pShortcutDest);
		if(!pItem || !(pRes = pItem->getRes()))
			return msg;

		ItemShortcut* pNewItem = ItemShortcut::CreateItem(pRes->getItemID(),pRes->getQuantity());
		if(!pNewItem)
			return MSG_ITEM_ITEMDATAERROR;

		pShortcutDest->setSlotState(ShortcutObject::SLOT_LOCK);
		pNewItem->setLockedItemIndex(param->DestIndex);
		pPlayer->mItemSplitList.SetSlot(param->SrcIndex,pNewItem);
		pPlayer->inventoryList.UpdateToClient(conn,param->DestIndex,ITEM_NOSHOW);
		UpdateToClient(conn,param->SrcIndex);
	}
	else//取回分解物
	{
		//目标已有物品
		if(pShortcutDest)
			return MSG_ITEM_OBJECTHASITEM;

		pItem = dynamic_cast<ItemShortcut*>(pShortcutSrc);
		if(!pItem || !(pRes = pItem->getRes()))
			return MSG_ITEM_ITEMDATAERROR;

		ItemShortcut* pNewItem = ItemShortcut::CreateItem(pRes->getItemID(),pRes->getQuantity());
		if(!pNewItem)
			return MSG_ITEM_ITEMDATAERROR;

		SAFE_DELETE(mSlots[param->SrcIndex]);
		UpdateToClient(conn,param->SrcIndex);
		pPlayer->inventoryList.SetSlot(param->DestIndex,pNewItem);
		pPlayer->inventoryList.UpdateToClient(conn,param->DestIndex,ITEM_SPLIT);
	}
	return MSG_NONE;
}

ConsoleFunction(openItemSplitWnd,bool,2,2,"openItemSplitWnd(%playerId)")
{
	Player* player = g_ServerGameplayState->GetPlayer(dAtoi(argv[1]));
	if(!player)
		return false;

	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return false;
	
	ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_ITEMSPLIT);
	pEvent->SetIntArgValues(1,ItemSplit::ITEMSPLIT_OPENWND);
	return conn->postNetEvent(pEvent);
}
#endif

#ifdef NTJ_CLIENT
bool ItemSplit::sendToStartEvent(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_ITEMSPLIT);
	pEvent->SetIntArgValues(1,ITEMSPLIT_START);
	if(conn->postNetEvent(pEvent))
	{
		setLock(true);
		Con::executef("InitFetchTimeProgress",
			Con::getIntArg(mReadyTime),
			Con::getIntArg((S32)VocalStatus::VOCALSTATUS_ITEMSPLIT),
			"");
		return true;
	} 

	return false;
}

bool ItemSplit::sendToCancelEvent(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	if(isLock())
		setLock(false);

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_ITEMSPLIT);
	pEvent->SetIntArgValues(1,ITEMSPLIT_CANCEL);
	return conn->postNetEvent(pEvent);		
}

bool ItemSplit::sendToPickupAllItem(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	bool bSend = false;
	for (U8 i=1; i<MAXSLOTS; i++)
	{
		if(mSlots[i])
		{
			bSend = true;
			break;
		}
	}

	if(bSend)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_ITEMSPLIT);
		pEvent->SetIntArgValues(1,ITEMSPLIT_PICKUPAllITEM);
		return conn->postNetEvent(pEvent);
	}

	return false;
}

bool ItemSplit::sendToPickupItemByIndex(Player* pPlayer,U8 index)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	if(index<1 || index>= MAXSLOTS)
		return false;

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_ITEMSPLIT);
	pEvent->SetIntArgValues(2,ITEMSPLIT_PICKUPITEM,index);
	return conn->postNetEvent(pEvent);
}

ConsoleFunction(uiItemSplitStart,void,1,1,"uiItemSplitStart()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	VocalStatus* pStatus = g_ClientGameplayState->pVocalStatus ;
	if(!pStatus)
		return;

	// 玩家正忙
	if(pStatus->getStatus() && pStatus->getStatusType() != VocalStatus::VOCALSTATUS_NONE)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY,MSG_PLAYER_BUSY);
		return;
	}

	pPlayer->mItemSplitList.startItemSplit(pPlayer);
}

ConsoleFunction(uiItemSplitCancel,void,1,1,"uiItemSplitCancel()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	VocalStatus* pStatus = g_ClientGameplayState->pVocalStatus;
	if(pStatus && pStatus->getStatus() && pStatus->getStatusType() == VocalStatus::VOCALSTATUS_ITEMSPLIT)
		Con::executef("cancleFetchTimeProgress");
	
	pPlayer->mItemSplitList.sendToCancelEvent(pPlayer);
}

ConsoleFunction(uiItemSplitPickupAll,void,1,1,"uiItemSplitPickupAll()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	pPlayer->mItemSplitList.sendToPickupAllItem(pPlayer);
}
#endif
