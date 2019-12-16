//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "T3D/gameConnection.h"
#include "Gameplay/item/Res.h"
#include "Gameplay/item/NpcShopData.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#include "Common/LogHelper.h"
#include "Common/Log/LogTypes.h"
#endif

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ClientGameplayAction.h"
#include "Gameplay/Item/AutoNpcSell.h"
#endif

// ----------------------------------------------------------------------------
// 商店列表构造方法
NpcShopList::NpcShopList()
{
	for(U32 i = 0; i < MAXSLOTS; i++)
		mSlots[i] = NULL;
	mCurrentNpcShopID = 0;
	mCurrentPage = 0;
	mTotalPage = 0;
	mLastQueryTime = 0;
	mShopType = 0;
	mFilterCase = 1;
}

NpcShopList::~NpcShopList()
{
	Clear();
}

// ----------------------------------------------------------------------------
// 清除NPC商店列表数据
void NpcShopList::Clear()
{

	for(U32 i = 0; i < MAXSLOTS; i++)
	{
		if(mSlots[i])
		{
			if(mSlots[i]->pItem)
			{
				delete mSlots[i]->pItem;
				mSlots[i]->pItem = NULL;
			}
#ifdef NTJ_CLIENT
			delete mSlots[i];
#endif
		}
		mSlots[i] = NULL;
	}
}

ShortcutObject* NpcShopList::GetSlot(S32 index)
{
#ifdef NTJ_CLIENT
	if(index < 0 || index >= MAXSLOTS)
		return NULL;
	if(mSlots[index] && mSlots[index]->pItem)
	{
		U32 curTime = Platform::getTime();
		if(mSlots[index]->IsShow(curTime))
		{
			//Con::executef("setCoolImage", Con::getIntArg(index), Con::getIntArg(0));
			return mSlots[index]->pItem;
		}
		else
		{
			Con::executef("clearNPCShopText", Con::getIntArg(index));
			Con::executef("setCoolImage", Con::getIntArg(index), Con::getIntArg(1));
			return NULL;
		}
	}
	return NULL;
#else
	return GetSlotTrue(index);
#endif
}

ShortcutObject* NpcShopList::GetSlotTrue(S32 index)
{
	if(index < 0 || index >= MAXSLOTS)
		return NULL;
	return mSlots[index] ? mSlots[index]->pItem : NULL;
}

// ----------------------------------------------------------------------------
// 打开NPC系统商店
bool ItemManager::openNpcShop(Player* player, U32 shopid, U32 shoptype)
{	
#ifdef NTJ_SERVER
	SceneObject* obj = player->getInteraction();
	player->setInteraction(obj, Player::INTERACTION_NPCTRADE);

	// 判断如果已经存在一种事件状态就不能打开商店
	enWarnMessage MsgCode = player->isBusy(Player::INTERACTION_NPCTRADE);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(player->getControllingClient(), SHOWTYPE_ERROR, MsgCode);
		return false;
	}
	U32 totalPages = g_ShopListRepository->getShopData(player, shopid, &player->npcShopList, 0, 1);
	for(U32 i = 0; i < NpcShopList::MAXSLOTS; i++)
	{
		NpcShopEvent* ev = new NpcShopEvent(i, shopid, shoptype, 0, totalPages);
		player->getControllingClient()->postNetEvent(ev);
	}
	player->npcShopList.SetNpcShopPage(0,totalPages);
	player->npcShopList.SetShopType(shoptype);
	
#endif
#ifdef NTJ_CLIENT
	if(player->isBusy(Player::INTERACTION_NPCTRADE) == MSG_NONE)
	{
		// 打开NPC交易窗口
		Con::executef("ShowNpcShopWnd");	
	}
	// 载入玩家自动出售列表数据
	player->autoSellList.autoSell();
#endif
	player->npcShopList.SetQueryTime(Platform::getTime());
	player->npcShopList.SetNpcShopID(shopid);
	return true;
}

// ----------------------------------------------------------------------------
// 关闭商店买卖
void ItemManager::closeNpcShop(Player* player)
{
	if(!player)
		return;
#ifdef NTJ_CLIENT
	// 关闭NPC交易窗口
	 Con::executef("CloseNpcShopWnd");	 
	 player->autoSellList.clearGeneralItemList();
	 player->autoSellList.clearBestItemList();
	 player->autoSellList.isAutoSell = false;
	 g_ClientGameplayState->cancelCurrentAction();
#endif
	// 清除NPC系统商店交易
	player->npcShopList.Clear();
	player->npcShopList.setFilterCase(1);
}

#ifdef NTJ_SERVER
// ----------------------------------------------------------------------------
// 设置当前NPC系统商店的页数
enWarnMessage ItemManager::setNpcShopPage(Player* player, S32 page)
{
	U32 currentTime = Platform::getTime();
	// 判断是否允许查询商店列表
	if(player->npcShopList.CanQueryTime(currentTime))
	{
		NpcShopList tmpNpcShopList;
		U32 totalPages = g_ShopListRepository->getShopData(player,
							player->npcShopList.GetNpcShopID(),
							&tmpNpcShopList,
							page,
							player->npcShopList.getFilterCase());

		player->npcShopList.SetNpcShopPage(page, totalPages);
		player->npcShopList.SetQueryTime(currentTime);
		if (totalPages > 0)
		{
			for(U32 i = 0; i < NpcShopList::MAXSLOTS; i++)
			{
				player->npcShopList.mSlots[i] = tmpNpcShopList.mSlots[i];
				NpcShopEvent* ev = new NpcShopEvent(i, player->npcShopList.GetNpcShopID(),
					player->npcShopList.GetShopType(), page, totalPages);
				player->getControllingClient()->postNetEvent(ev);
			}
		}
		else
		{
			for(U32 i = 0; i < NpcShopList::MAXSLOTS; i++)
			{
				if(player->npcShopList.mSlots[i])
					delete player->npcShopList.mSlots[i];
				player->npcShopList.mSlots[i] = NULL;
				NpcShopEvent* ev = new NpcShopEvent(i, player->npcShopList.GetNpcShopID(),
					player->npcShopList.GetShopType(), 0, 0);
				player->getControllingClient()->postNetEvent(ev);
			}
		}
	}
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 从商店买东西
enWarnMessage ItemManager::buyItem(Player* player,S32 index,  S32 goodid, S32 num)
{
	if(index < 0 || index >= NpcShopList::MAXSLOTS)
		return MSG_ITEM_NOFINDITEM;

	NpcShopItem* pShopItem = player->npcShopList.mSlots[index];
	if(pShopItem == NULL)
		return MSG_ITEM_ITEMDATAERROR;

	if(pShopItem->GoodID != goodid)
	{
		setNpcShopPage(player, player->npcShopList.GetNpcShopPage());
		return MSG_ITEM_ITEMDATAERROR;
	}

	ItemBaseData* pItemData = g_ItemRepository->getItemData(pShopItem->ItemID);
	if(pItemData == NULL)
		return MSG_ITEM_ITEMDATAERROR;

	// 判断此物品是否有库存限量的
	if(pShopItem->StockNum && pShopItem->CurrentStockNum < num )
		return MSG_TRADE_NOENOUGH_GOODS;
	
	if(pShopItem->CurrencyID == 1)// 金元
	{
		S32 itemPrice = pShopItem->CurrencyValue * num;
		if(player->canReduceMoney(itemPrice, Player::Currentcy_Money))
		{
			S32 iIndex;
			enWarnMessage msg = g_ItemManager->addItemToInventory(player, pShopItem->ItemID, iIndex, num, ITEM_BUY);
			if(msg == MSG_NONE)
			{
				player->reduceMoney(itemPrice, 1);
			}
			else
				return msg;
		}
		else
		{
			return MSG_PLAYER_MONEYNOTENOUGH;
		}
	}
	else if(pShopItem->CurrencyID == 2)// 灵元
	{
		S32 itemPrice = pShopItem->CurrencyValue * num;
		// <Edit> [3/5/2010 iceRain] 按策划要求改的
		//灵元不够可以消耗金元
		if(player->canReduceMoney(itemPrice, 21))
		{
			S32 iIndex;
			enWarnMessage msg = g_ItemManager->addItemToInventory(player, pShopItem->ItemID, iIndex, num, ITEM_BUY);
			if(msg == MSG_NONE)
			{
				player->reduceMoney(itemPrice, 21);
			}
			else
				return msg;
		}
		else
		{
			return MSG_PLAYER_MONEYNOTENOUGH;
		}
	}
	else if(pShopItem->CurrencyID == 3)// 仙石
	{

	}
	else if(pShopItem->CurrencyID == 4)//神石
	{

	}
	else if(pShopItem->CurrencyID == 5)// 声望
	{

	}
	else if(pShopItem->CurrencyID > 100)// 物品编号
	{
		//所需物品
		U32 iHasCout = player->inventoryList.GetItemCount(pShopItem->CurrencyID);
		if(iHasCout < pShopItem->CurrencyValue)
			return MSG_TRADE_NOHAVE_ITEM;
		S32 iIndex;
		enWarnMessage msg = g_ItemManager->addItemToInventory(player, pShopItem->ItemID, iIndex, num, ITEM_BUY);
		if(msg == MSG_NONE)
			g_ItemManager->delItemFromInventory(player, pShopItem->CurrencyID, pShopItem->CurrencyValue);
	}

	if(pShopItem->StockNum)
	{
		pShopItem->CurrentStockNum -= num;
		player->npcShopList.SetQueryTime(0);
		if(pShopItem->CurrentStockNum <= 0)
			setNpcShopPage(player, player->npcShopList.GetNpcShopPage());
		else
		{
			NpcShopEvent* ev = new NpcShopEvent(index, player->npcShopList.GetNpcShopID(),
				player->npcShopList.GetShopType(),
				player->npcShopList.GetNpcShopPage(),
				player->npcShopList.GetNpcTotalPage());
			player->getControllingClient()->postNetEvent(ev);
		}
	}

    //log it
    lg_shopping_buy lg;
    lg.playerName = player->getPlayerName();
    lg.buyfrom    = "npc";
    lg.itemName   = pItemData->getItemName();
    lg.itemCount  = num;
    lg.costType   = pShopItem->CurrencyID;
	lg.costVal    = pShopItem->CurrencyValue * num;
    lg.posX       = player->getPosition().x;
    lg.posY       = player->getPosition().y;
    g_ServerGameplayState->GetLog()->writeLog(&lg);

	return MSG_NONE;		// 没有找到此物品
}

// ----------------------------------------------------------------------------
//打开NPC商店
ConsoleFunction(OpenNpcShop,bool, 3, 3, "OpenNpcShop(%player, %shopid)" )
{
	SimObject* obj = Sim::findObject(dAtoi(argv[1]));
	if(obj == NULL)
		return false;
	Player* player = dynamic_cast<Player*>(obj);
	if(player == NULL)
		return false;
	S32 iShopID = dAtoi(argv[2]);
	if(iShopID < 0)
		return false;
	return g_ItemManager->openNpcShop(player, iShopID);
}

// ----------------------------------------------------------------------------
//关闭NPC商店
ConsoleFunction(CloseNpcShop, void, 2, 2, "CloseNpcShop(%player)")
{
	SimObject* obj = Sim::findObject(dAtoi(argv[1]));
	if(obj == NULL)
		return;
	Player* player = dynamic_cast<Player*>(obj);
	if(player == NULL)
		return;
	g_ItemManager->closeNpcShop(player);
}

#endif

#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// 客户端NPC商店物品刷新
void NpcShopList::UpdateData()
{
	U32 time = Platform::getVirtualMilliseconds();
	static U32 mLastUpdateTime = Platform::getVirtualMilliseconds();
	if(time <= mLastUpdateTime + 30000)
		return;

	U32 currentTime = Platform::getTime();
	bool needUpdate = false;
	for(S32 i = 0; i < MAXSLOTS; i++)
	{
		NpcShopItem* npcitem = mSlots[i];
		if(!npcitem)
			continue;

		// 判定库存刷新
		if(npcitem->IsRefresh(currentTime))
		{			
			needUpdate = true;
			npcitem->LastUpdateTime = currentTime;
		}
	}

	if(needUpdate)
	{
		Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
		if(!pPlayer)
			return;
		// 发送商品浏览翻页消息给服务端
		ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
		event->SetIntArgValues(2, NPCSHOP_PAGE, mCurrentPage);
		pPlayer->getControllingClient()->postNetEvent(event);
	}
	mLastUpdateTime = time;
}

// ----------------------------------------------------------------------------
// 显示商店第page页的物品
ConsoleFunction(ShowNPCShop, void , 2, 2, "ShowNPCShop(%page)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	// 发送商品浏览翻页消息给服务端
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
	event->SetIntArgValues(2, NPCSHOP_PAGE, dAtoi(argv[1]));
	pPlayer->getControllingClient()->postNetEvent(event);
}

// ----------------------------------------------------------------------------
// 筛选显示商店物品
ConsoleFunction(FilterNPCShopItem, void , 2, 2, "FilterNPCShopItem(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	S32 id = dAtoi(argv[1]);
	//pPlayer->npcShopList.setFilterCase(id);
	// 发送商品浏览翻页消息给服务端
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
	event->SetIntArgValues(3, NPCSHOP_FILTER, 0, id);
	pPlayer->getControllingClient()->postNetEvent(event);
}

// ----------------------------------------------------------------------------
// 关闭NPC商店
ConsoleFunction(CloseNpcShop, void, 1, 1, "CloseNpcShop()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	// 发送关闭商店消息给服务端
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
	event->SetIntArgValues(1, NPCSHOP_CLOSE);
	pPlayer->getControllingClient()->postNetEvent(event);
}

// ----------------------------------------------------------------------------
// 从商店买东西
ConsoleFunction(BuyFromShop, void, 3, 3, "BuyFromShop(%index,%num)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 index = dAtoi(argv[1]);
	U32 num = dAtoi(argv[2]);
	NpcShopItem* pItem = pPlayer->npcShopList.mSlots[index];
	if(!pItem)
		return;

	// 发送回购物品消息给服务端
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
	event->SetIntArgValues(4, NPCSHOP_BUY, index, pItem->GoodID, num);
	pPlayer->getControllingClient()->postNetEvent(event);
}

// ----------------------------------------------------------------------------
// 单个物品购买
ConsoleFunction(setBuyAction, void, 1, 1, "setBuyAction()")
{
	ClientGameplayAction* oldAction = g_ClientGameplayState->getCurrentAction();
	if(oldAction && oldAction->getActionType() == INFO_NPCSHOP)
	{
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}

	ClientGameplayAction* action = new ClientGameplayAction(1, INFO_NPCSHOP);
	action->setCursorName("Buy");
	g_ClientGameplayState->setCurrentAction(action);
}

// ----------------------------------------------------------------------------
// 批量物品购买
ConsoleFunction(setBatchBuyAction, void, 1, 1, "setBatchBuyAction()")
{
	ClientGameplayAction* oldAction = g_ClientGameplayState->getCurrentAction();
	if(oldAction && oldAction->getActionType() == INFO_ITEM_BATCHBUY)
	{
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}

	ClientGameplayAction* action = new ClientGameplayAction(1, INFO_ITEM_BATCHBUY);
	action->setCursorName("BatchBuy");
	g_ClientGameplayState->setCurrentAction(action);
}

// ----------------------------------------------------------------------------
// 卖物品
ConsoleFunction(setSellAction, void, 1, 1, "setSellAction()")
{
	ClientGameplayAction* oldAction = g_ClientGameplayState->getCurrentAction();
	if(oldAction && oldAction->getActionType() == INFO_ITEM_BUY)
	{
		g_ClientGameplayState->cancelCurrentAction();
		Con::executef("setInventoryStats", Con::getIntArg(0));
		return;
	}

	ClientGameplayAction* action = new ClientGameplayAction(1, INFO_ITEM_BUY);
	action->setCursorName("Sell");
	g_ClientGameplayState->setCurrentAction(action);
	Con::executef("setInventoryStats", Con::getIntArg(1));
}

// ----------------------------------------------------------------------------
// 单修
ConsoleFunction(singleRepair, void, 1, 1, "singleRepair()")
{
	ClientGameplayAction* action = new ClientGameplayAction(1, INFO_ITEM_REPAIR);
	action->setCursorName("repair");
	g_ClientGameplayState->setCurrentAction(action);
}

// ----------------------------------------------------------------------------
// 全修
ConsoleFunction(allRepair, void, 1, 1, "allRepair()")
{
	Player* myself = g_ClientGameplayState->GetControlPlayer();
	if(!myself)
		return;
	U32 iCost = myself->equipList.GetRepairCost();
	if(!myself->canReduceMoney(iCost, 1))
	{
		MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
		return;
	}
	ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_ITEM_REPAIR);
	ev->SetInt32ArgValues(2, NPCSHOP_REPAIRALL, 0);
	myself->getControllingClient()->postNetEvent(ev);
}

// ----------------------------------------------------------------------------
// 设置一组
ConsoleFunction(setOneGroup , S32, 2, 2, "setOneGroup(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return 1;
	U32 index = dAtoi(argv[1]);
	NpcShopItem* pItem = pPlayer->npcShopList.mSlots[index];
	if(!pItem)
		return 1;
	ItemShortcut* item = pItem->pItem;
	if(!item)
		return 1;
	return item->getRes()->getMaxOverNum();
}

// ----------------------------------------------------------------------------
// 获得物品名字
ConsoleFunction(getItemName, StringTableEntry, 2, 2, "getItemName(%id)")
{
	ItemBaseData* pBaseData = g_ItemRepository->getItemData(dAtoi(argv[1]));
	char* name = Con::getReturnBuffer(32);
	if(pBaseData)
		dSprintf(name, 32, "%s", pBaseData->getItemName());
	return name;
}

#endif


#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// 购买NPC商店物品
ConsoleFunction(BuyShopItem,void,2,2,"BuyShopItem(%index)")
{	
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player)
		return;

	S32 mSlotIndex = dAtoi(argv[1]);
	if(mSlotIndex < 0 || mSlotIndex >= NpcShopList::MAXSLOTS)
		return;

	NpcShopItem* pShopItem = player->npcShopList.mSlots[mSlotIndex];
	if(!pShopItem)
		return;

	ItemShortcut* pItem = (ItemShortcut*)player->npcShopList.GetSlot(mSlotIndex);
	if(!pItem)
		return;
	char tempbuffer[128] = "\0";
	pItem->getRes()->getItemName(tempbuffer, 128);
	Con::executef("buyGoodFromShop",
				Con::getIntArg(mSlotIndex),
				tempbuffer,
				Con::getIntArg(pShopItem->CurrencyID),
				Con::getIntArg(pShopItem->CurrencyValue));		
}

// ----------------------------------------------------------------------------
// 批量购买NPC商店物品
ConsoleFunction(BatchBuyGoodsFromShop, void, 2, 2, "BatchBuyGoodsFromShop(%index)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player)
		return;

	S32 mSlotIndex = dAtoi(argv[1]);
	if(mSlotIndex < 0 || mSlotIndex >= NpcShopList::MAXSLOTS)
		return;

	NpcShopItem* pShopItem = player->npcShopList.mSlots[mSlotIndex];
	if(!pShopItem)
		return;

	ItemShortcut* pItem = (ItemShortcut*)player->npcShopList.GetSlot(mSlotIndex);
	if(!pItem)
		return;
	char tempbuffer[128] = "\0";
	pItem->getRes()->getItemName(tempbuffer, 128);
	Con::executef("buyManyGoodsFromShop",
				Con::getIntArg(mSlotIndex),
				tempbuffer,
				Con::getIntArg(pShopItem->CurrencyID),
				Con::getIntArg(pShopItem->CurrencyValue));	
}

#endif//NTJ_CLIENT