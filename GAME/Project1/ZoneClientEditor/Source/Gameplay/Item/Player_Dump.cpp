//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "T3D/gameConnection.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"

#ifdef NTJ_SERVER
#include "Common/LogHelper.h"
#include "Common/Log/LogTypes.h"
#include "Gameplay/ServerGameplayState.h"
#endif//NTJ_SERVER

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif//NTJ_CLIENT

DumpList::DumpList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_DUMP;
	mCurrentIndex = 0;
}

// ----------------------------------------------------------------------------
// 更新玩家回购栏数据到客户端
bool DumpList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	ReBuyEvent* ev = new ReBuyEvent(index,flag);
	conn->postNetEvent(ev);
#endif
	return true;
}

#ifdef NTJ_SERVER

enWarnMessage ItemManager::addItemToDumpList(Player* player, ItemShortcut* item, S32 num)
{
	if(!player)
		return MSG_UNKOWNERROR;
	if(!item)
		return MSG_ITEM_ITEMDATAERROR;
	Res* pRes = item->getRes();
	if(!pRes)
		return MSG_ITEM_ITEMDATAERROR;

	Vector<S32> emptyslotlist;
	S32 iEmptySlots = 0;
	iEmptySlots = player->dumpList.QueryEmptySlot(emptyslotlist);
	S32 iMaxOverNum = pRes->getMaxOverNum();
	S32 iNeed = num;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;

	if(iMaxOverNum > 1)
	{
		Vector<S32> slotlist;
		S32 iCount = player->dumpList.QuerySameObject(item, slotlist, true, false);
		if(iCount > 0)
		{
			int iNum = 0;
			for(S32 i = 0; i < iCount; i++)
			{
				ShortcutObject* pSlot = player->dumpList.GetSlot(slotlist[i]);
				ItemShortcut* pItem1 = NULL;
				if(pSlot && (pItem1 = dynamic_cast<ItemShortcut*>(pSlot)))
				{
					iNum += (iMaxOverNum - pItem1->getRes()->getQuantity());
				}						
			}
			if(iNeed > iNum)
				iNeed -= iNum;
			else
				iNeed = 0;
		}


		iNeed = num;		
		for(S32 i = 0; i < iCount; ++i)
		{
			ShortcutObject* pSlot = player->dumpList.GetSlot(slotlist[i]);
			ItemShortcut* pTargetItem = NULL;
			if(pSlot && (pTargetItem = dynamic_cast<ItemShortcut*>(pSlot)))
			{
				if(pTargetItem->getRes()->getQuantity() + iNeed >= iMaxOverNum)
				{
					iNeed -= (iMaxOverNum - pTargetItem->getRes()->getQuantity());
					pTargetItem->getRes()->setQuantity(iMaxOverNum);
				}
				else
				{
					pTargetItem->getRes()->setQuantity(pTargetItem->getRes()->getQuantity() + iNeed);
					iNeed = 0;
				}
				player->dumpList.UpdateToClient(conn, slotlist[i], ITEM_NOSHOW);
			}
		}

		if(iNeed > 0)
		{
			S32 pos = 0;
			do 
			{
				ItemShortcut* pItem1 = ItemShortcut::CreateItem(item);
				if(!pItem1)
					return MSG_ITEM_ITEMDATAERROR;
				
				if(iNeed > iMaxOverNum)
				{
					pItem1->getRes()->setQuantity(iMaxOverNum);
					iNeed -= iMaxOverNum;
				}
				else
				{
					pItem1->getRes()->setQuantity(iNeed);
					iNeed = 0;
				}
				if(iEmptySlots > 0)
				{
					player->dumpList.SetSlot(emptyslotlist[pos], pItem1);
					player->dumpList.UpdateToClient(conn, emptyslotlist[pos], ITEM_NOSHOW);
					iEmptySlots --;
					pos++;
				}
				else
				{
					S32 iIndex = player->dumpList.mCurrentIndex;
					player->dumpList.SetSlot(iIndex, pItem1);
					player->dumpList.UpdateToClient(conn, iIndex, ITEM_NOSHOW);
					player->dumpList.mCurrentIndex++;
					if(iIndex >= player->dumpList.GetMaxSlots() - 1)
						player->dumpList.mCurrentIndex = 0;
				}
				
			} while (iNeed > 0);
		}

	}
	else
	{
		ItemShortcut* pItem1 = ItemShortcut::CreateItem(item);
		if(!pItem1)
			return MSG_ITEM_ITEMDATAERROR;
		pItem1->getRes()->setQuantity(1);
		if(iEmptySlots > 0)
		{
			player->dumpList.SetSlot(emptyslotlist[0], pItem1);
			player->dumpList.UpdateToClient(conn, emptyslotlist[0], ITEM_NOSHOW);
		}
		else
		{
			S32 iIndex = player->dumpList.mCurrentIndex; 
			player->dumpList.SetSlot(iIndex, pItem1);
			player->dumpList.UpdateToClient(conn, iIndex, ITEM_NOSHOW);
			player->dumpList.mCurrentIndex++;
			if(iIndex >= player->dumpList.GetMaxSlots() -1)
				player->dumpList.mCurrentIndex = 0;
		}
	}
	

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 卖东西到商店
enWarnMessage ItemManager::sellItem(Player* player, S32 index)
{
	if(!player)
		return MSG_UNKOWNERROR;
	ShortcutObject* pSlot = player->inventoryList.GetSlot(index);
	ItemShortcut* pItem = NULL;
	if (pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot)))
	{
		// 判断是否能出售给NPC
		if(!pItem->getRes()->canBaseLimit(Res::ITEM_SELLNPC))
			return MSG_TRADE_FAILED;

		S32 iCount = 0;
		U32 uItemID = pItem->getRes()->getItemID();
		S32 iSellPrice = 0;
		switch(pItem->getRes()->getSaleType())
		{
			case Res::SALETYPE_MONEY:
				{					
					iCount = pItem->getRes()->getQuantity();
					iSellPrice = (pItem->getRes()->getSalePrice())*iCount;
					player->addMoney(iSellPrice, 1);
				}
				break;
			case Res::SALETYPE_GOLD:
				{
					iCount = pItem->getRes()->getQuantity();
					iSellPrice = (pItem->getRes()->getSalePrice())*iCount;
					player->addMoney(iSellPrice, 2);
				}
				break;
			case Res::SALETYPE_HONOR:
				{				
				}
				break;
			case Res::SALETYPE_BINDMONEY:
				{

				}
				break;
			case Res::SALETYPE_CREDIT:
				{

				}
				break;
			case Res::SALETYPE_BINDGOLD:
				{
					
				}
				break;
			default:
				{
					if(pItem->getRes()->getSaleType() > 100)
					{
						S32 index;
						enWarnMessage msg = g_ItemManager->addItemToInventory(player, pItem->getRes()->getSaleType(), index, pItem->getRes()->getSalePrice(), ITEM_GET);
						if(msg != MSG_NONE)
							return msg;
					}
				}
				break;
		}
	
		g_ItemManager->addItemToDumpList(player, pItem, iCount);

        //lg it
        lg_shopping_sell lg;

        lg.playerName = player->getPlayerName();
        lg.itemName   = pItem->getRes()->getItemName();
        lg.itemUID    = pItem->getRes()->getUID();
        lg.itemCount  = iCount;
        lg.itemPrice  = iSellPrice;

        g_ServerGameplayState->GetLog()->writeLog(&lg);

		player->inventoryList.SetSlot(index, NULL);
		player->inventoryList.UpdateToClient(player->getControllingClient(), index, ITEM_SELL);
		
		// 同步快捷栏此物品的数量
		g_ItemManager->syncPanel(player, uItemID);
		//同步自动售物
		ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_NPCSHOP);
		ev->SetIntArgValues(1, NPCSHOP_SELL);
		player->getControllingClient()->postNetEvent(ev);
	}
	return MSG_ITEM_ITEMDATAERROR;		// 物品卖出物品失败
}

// ----------------------------------------------------------------------------
//回购物品
enWarnMessage ItemManager::reBuyItem(Player* player, S32 index)
{
	if(index < 0 || index > DumpList::MAXSLOTS)
		return MSG_ITEM_ITEMDATAERROR;

	ShortcutObject* pShortcut = player->dumpList.GetSlot(index);
	ItemShortcut* pItem = NULL;
	Res* pRes = NULL;
	if(!pShortcut || !(pItem = dynamic_cast<ItemShortcut*>(pShortcut)) || !(pRes = pItem->getRes()))
		return MSG_ITEM_ITEMDATAERROR;
	S32 iCout = pItem->getRes()->getQuantity();
	S32 iPrice = (pItem->getRes()->getSalePrice()) * iCout;
	// <Edit> [3/5/2010 iceRain] 从回购栏购买全部消耗金元
	
	if(!player->canReduceMoney(iPrice, Player::Currentcy_Money))
		return MSG_PLAYER_MONEYNOTENOUGH;

	S32 iItemID = pItem->getRes()->getItemID();
	S32 iIndex;
	if(g_ItemManager->addItemToInventory(player, pItem,  iIndex, iCout, ITEM_REBUY) == MSG_NONE)
	{
		ItemShortcut* pTemp = ItemShortcut::CreateItem(pItem);

		player->dumpList.SetSlot(index, NULL);
		player->dumpList.UpdateToClient(player->getControllingClient(), index, ITEM_BUY);
		player->reduceMoney(iPrice, Player::Currentcy_Money);
		
		
        //log it
        lg_shopping_rebuy lg;
        lg.playerName = player->getPlayerName();
        lg.buyfrom    = "npc";
        lg.itemName   = pTemp->getRes()->getItemName();
        lg.itemCount  = iCout;
        lg.costType   = 0;
        lg.costVal    = iPrice;
        lg.posX       = player->getPosition().x;
        lg.posY       = player->getPosition().y;

        g_ServerGameplayState->GetLog()->writeLog(&lg);
		delete pTemp;
		return MSG_NONE;
	}	
	return MSG_ITEM_ITEMDATAERROR;
}

#endif

#ifdef NTJ_CLIENT
//回购东西
ConsoleFunction(rebuyFromShop, void, 2, 2, "rebuyFromShop(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	// 发送回购物品消息给服务端
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
	event->SetIntArgValues(2, NPCSHOP_RECYCLE, dAtoi(argv[1]));
	pPlayer->getControllingClient()->postNetEvent(event);
}
#endif