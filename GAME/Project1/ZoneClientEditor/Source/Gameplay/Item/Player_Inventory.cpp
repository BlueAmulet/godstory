//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include <map>
#include <time.h>
#include <xtree>
#include "Gameplay/GameObjects/PlayerObject.h"
#include "T3D/gameConnection.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/item/Res.h"
#include "Gameplay/Mission/PlayerMission.h"

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/Item/Player_EquipIdentify.h"
#endif

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayAction.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/Item/IndividualStall.h"
#include "ts/tsLogicMarkNode.h"
#endif

InventoryList::InventoryList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_INVENTORY;
}

InventoryList::InventoryList(U32 maxSlots):BaseItemList(maxSlots)
{
	mType = SHORTCUTTYPE_INVENTORY;
}

// ----------------------------------------------------------------------------
// 从数据库加载玩家物品栏数据
bool InventoryList::LoadData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	U32 currentTime = time((time_t*)NULL);
	for(S32 i = 0; i < mMaxSlots; i++)
	{
		stItemInfo& stTemp = playerInfo->MainData.InventoryInfo[i];
		if(stTemp.ItemID == 0)
			continue;
		ItemShortcut* pItem = ItemShortcut::CreateItem(stTemp);
		if(pItem)
		{
			
		}
		mSlots[i] = pItem;
	}
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 发送初始化的物品栏数据到客户端
void InventoryList::SendInitialData(GameConnection* conn)
{
	for(S32 i = 0;  i < mMaxSlots; ++i)
	{
		if(mSlots[i])
		{
			InventoryEvent* event = new InventoryEvent(i, ITEM_LOAD);
			conn->postNetEvent(event);
		}
	}
}

// ----------------------------------------------------------------------------
// 存储玩家物品栏到数据库
bool InventoryList::SaveData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	for(S32 i = 0; i < mMaxSlots; i++)
	{
		ItemShortcut* pItem = (ItemShortcut*)mSlots[i];
		if(pItem)
		{
			if(pItem->getRes()->canBaseLimit(Res::ITEM_DOWNLINE))
				continue;
			stItemInfo* pTemp = pItem->getRes()->getExtData();
			if(pTemp)
				dMemcpy(&playerInfo->MainData.InventoryInfo[i], pTemp, sizeof(stItemInfo));
		}
	}
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 根据物品唯一ID获得物品对象
ItemShortcut* InventoryList::FindSlot(U64 uid)
{
	for(S32 i = 0; i < mMaxSlots; i++)
	{
		ItemShortcut* pItem = (ItemShortcut*)mSlots[i];
		if(pItem && pItem->getRes()->getUID() == uid)
			return pItem;
	}
	return NULL;
}


// ----------------------------------------------------------------------------
// 获取物品的数量
S32 InventoryList::GetItemCount(ShortcutObject* obj, bool checkSlotState/* = true */, bool ignoreExtPro /* = true */)
{
	if(obj == NULL)
		return 0;
	
	S32 iCount = 0;
	if(obj->isItemObject())
	{
		for(S32 i = 0; i < mMaxSlots; ++i)
		{
			if(!mSlots[i] || (checkSlotState && mSlots[i]->getSlotState() == ShortcutObject::SLOT_LOCK))
				continue;

			if(mSlots[i]->isItemObject())
			{
				if(g_ItemManager->isSameItem(mSlots[i], obj, ignoreExtPro))
				{
					ItemShortcut* pItem = (ItemShortcut*)mSlots[i];
					iCount += pItem->getRes()->getQuantity();
				}
			}
		}
	}
	return iCount;
}

// ----------------------------------------------------------------------------
// 获取物品的数量
S32 InventoryList::GetItemCount(S32 id, bool checkSlotState/* = true */, bool ignoreExtPro /* = true */)
{
	S32 iCount = 0;
	for(S32 i = 0; i < mMaxSlots; ++i)
	{
		if(!mSlots[i] || (checkSlotState && mSlots[i]->getSlotState() == ShortcutObject::SLOT_LOCK))
			continue;

		if(mSlots[i]->isItemObject())
		{
			ItemShortcut* pItem = (ItemShortcut*)mSlots[i];
			if(pItem->getRes()->getItemID() == id)
				iCount += pItem->getRes()->getQuantity();
		}
	}
	return iCount;
}

// ----------------------------------------------------------------------------
// 更新玩家物品栏数据到客户端
bool InventoryList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	InventoryEvent* ev = new InventoryEvent(index, flag);
	conn->postNetEvent(ev);
#endif                                                                                                                       
	return true;
}

// ----------------------------------------------------------------------------
// 物品栏物品解除虚影锁定
void InventoryList::Unlock(GameConnection* conn)
{
	for(S32 i = 0; i < mMaxSlots; ++i)
	{
		if(mSlots[i] && mSlots[i]->getSlotState() == ShortcutObject::SLOT_LOCK)
		{
			mSlots[i]->setSlotState(ShortcutObject::SLOT_COMMON);
			UpdateToClient(conn, i, ITEM_NOSHOW);
		}
	}
}

InventoryList* InventoryList::CloneList()
{
	InventoryList* list = new InventoryList(mMaxSlots);
	dMemcpy(list->mSlots, mSlots, sizeof(ShortcutObject*) * mMaxSlots);
	return list;
}

// ----------------------------------------------------------------------------
// 判断是否能变更物品栏位大小
enWarnMessage InventoryList::CanChangeMaxSlots(U32 srcBag1, U32 srcBag2, U32 tagBag1, U32 tagBag2)
{
	if(srcBag1 == tagBag1 && srcBag2 == tagBag2)
		return MSG_NONE;

	if(tagBag1 + tagBag2 > InventoryList::MAXSLOTS - ItemManager::EXTBAG_BEGINSLOT)
		return MSG_ITEM_BAGLIMIT;

	if(tagBag1 > 40 || tagBag2 > 40)
		return MSG_ITEM_BAGLIMIT;

	if(srcBag1 > tagBag1)
	{
		for(S32 i = ItemManager::EXTBAG_BEGINSLOT; i < ItemManager::EXTBAG_BEGINSLOT + srcBag1; i++)
			if(mSlots[i])
				return MSG_ITEM_BAGNOEMPTY1;
	}

	if(srcBag2 > tagBag2)
	{
		for(S32 i = ItemManager::EXTBAG_BEGINSLOT + srcBag1; i < ItemManager::EXTBAG_BEGINSLOT + srcBag1 + srcBag2; i++)
			if(mSlots[i])
				return MSG_ITEM_BAGNOEMPTY2;
	}
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 变更栏位大小(使用此函数之前，需要经CanChangeMaxSlots检查)
void InventoryList::ChangeMaxSlots(Player* player, U32 srcBag1, U32 srcBag2, U32 tagBag1, U32 tagBag2)
{
	U32 srcbags = srcBag1 + srcBag2;
	U32 tagbags = tagBag1 + tagBag2;
	U32 extBags = InventoryList::MAXSLOTS - ItemManager::EXTBAG_BEGINSLOT;
	ShortcutObject** tmpSlots = new ShortcutObject*[extBags];
	for(S32 i = 0; i < extBags; ++i)
	{
		if(i < srcbags)
			tmpSlots[i] = mSlots[ItemManager::EXTBAG_BEGINSLOT + i];
		else
			tmpSlots[i] = NULL;
	}

	//注：大包换小包必须大包为空包
	if(tagBag1 > srcBag1)
	{
		for(S32 i = ItemManager::EXTBAG_BEGINSLOT + srcBag1; i < ItemManager::EXTBAG_BEGINSLOT + srcbags; i++)
		{
			mSlots[i + (tagBag1 - srcBag1)] = tmpSlots[i - ItemManager::EXTBAG_BEGINSLOT];
			
			if(i < ItemManager::EXTBAG_BEGINSLOT + tagBag1)
				mSlots[i] = NULL;
		}
	}
	else if(tagBag1 < srcBag1)
	{
		for(S32 i = ItemManager::EXTBAG_BEGINSLOT + tagBag1; i < ItemManager::EXTBAG_BEGINSLOT + extBags; i++)
		{
			mSlots[i] = tmpSlots[i - ItemManager::EXTBAG_BEGINSLOT - tagBag1 + srcBag1];
			if(i >= ItemManager::EXTBAG_BEGINSLOT + srcbags)
				mSlots[i] = NULL;
		}
	}
	delete[] tmpSlots;
	SetMaxSlot(ItemManager::EXTBAG_BEGINSLOT + tagBag1 + tagBag2);	
}

void InventoryList::SetMaxSlot(S32 maxslots)
{
	mMaxSlots = maxslots;
}

void InventoryList::UpdateMaxSlots(GameConnection* conn)
{
#ifdef NTJ_SERVER
	if(conn)
	{
		ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_CHANGEMAXSLOTS);
		ev->SetInt32ArgValues(2, SHORTCUTTYPE_INVENTORY, mMaxSlots);
		conn->postNetEvent(ev);

		for(S32 i = ItemManager::EXTBAG_BEGINSLOT; i < InventoryList::MAXSLOTS; i++)
		{
			InventoryEvent* ev = new InventoryEvent(i, ITEM_NOSHOW);
			conn->postNetEvent(ev);
		}
	}
#endif
}


// ============================================================================
// 仅服务端代码
// ============================================================================
#ifdef NTJ_SERVER
// ----------------------------------------------------------------------------
// 移动物品栏对象到快捷栏位置
enWarnMessage ItemManager::InventoryMoveToPanel(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = NULL;
	if(param == NULL || (player = param->player) == NULL)
		return msg;
	
	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
		!player->panelList.IsVaildSlot(param->DestIndex))
		return msg;	

	ItemShortcut* pItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->SrcIndex);
	ShortcutObject* pTargetSlot = player->panelList.GetSlot(param->DestIndex);
	if(!pItem)
		return MSG_ITEM_ITEMDATAERROR;

	// 判断是否装备（因为装备不能被拖到快捷栏上）
	if(pItem->getRes()->isEquip())
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	// 判断是否快捷栏有锁定
	if(pTargetSlot && pTargetSlot->isLocked())
		return MSG_ITEM_PANELLOCKED;

	ItemShortcut* pTargetItem = ItemShortcut::CreateItem(pItem);
	if(pTargetItem)
	{
		//快捷栏的物品的唯一ID必须与源物品一样
		pTargetItem->getRes()->setUID(pItem->getRes()->getUID());
		//判断是否非使用次数限制的物品
		if(pItem->getRes()->getResUsedTimes() <= 0)
		{			
			//若物品可以叠加，则叠加统计数量
			if(pTargetItem->getRes()->getMaxOverNum() > 1)
			{
				S32 iTotalNum = player->inventoryList.GetItemCount(pItem, false, false);
				pTargetItem->getRes()->setQuantity(iTotalNum, true);
				//g_ItemManager->syncPanel(player, pItem->getRes()->getItemID());
			}
			else
				pTargetItem->getRes()->setQuantity(1, true);
		}
		player->panelList.SetSlot(param->DestIndex, pTargetItem, true);
		player->panelList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	}
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 移动物品栏对象到物品栏位置
enWarnMessage ItemManager::InventoryMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
		!player->inventoryList.IsVaildSlot(param->DestIndex))
		return msg;	

	ItemShortcut* pSrcItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->SrcIndex);
	ItemShortcut* pDestItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->DestIndex);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;

	// 判断是否同一类物品
	if(pDestItem && isSameItem(pSrcItem, pDestItem))
	{
		// 目标有物品且是同类物品，物品数量叠加			
		if(pDestItem->getSlotState() == ShortcutObject::SLOT_COMMON)
		{
			S32 iCount = pSrcItem->getRes()->getQuantity() + pDestItem->getRes()->getQuantity();
			S32 iMaxNum = pDestItem->getRes()->getMaxOverNum();
			if(iCount <= iMaxNum)
			{
				player->inventoryList.SetSlot(param->SrcIndex, NULL, true);
				pDestItem->getRes()->setQuantity(iCount);
			}
			else
			{
				pSrcItem->getRes()->setQuantity(iCount - iMaxNum);
				pDestItem->getRes()->setQuantity(iMaxNum);
			}
		}
		else
			return MSG_ITEM_CANNOTDRAGTOOBJECT;		
	}
	else
	{
		// 目标为空或有物品但非同类物品，直接交换
		player->inventoryList.SetSlot(param->DestIndex, pSrcItem, false);
		player->inventoryList.SetSlot(param->SrcIndex, pDestItem, false);
	}
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 移动物品栏对象到装备栏位置
enWarnMessage ItemManager::InventoryMoveToEquip(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
		!player->equipList.IsVaildSlot(param->DestIndex))
		return msg;	

	ItemShortcut* pSrcItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->SrcIndex);
	ItemShortcut* pDestItem	= (ItemShortcut*)player->equipList.GetSlot(param->DestIndex);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;
	
	// 判断是否为装备物品,并判断装备物品的放置是否正确
	msg = canDragToEquipment(player, pSrcItem, param->DestIndex);
	if(msg != MSG_NONE)
		return msg;
	
	//装备时鉴定
	if((pSrcItem->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_EQUIP) && (!pSrcItem->getRes()->IsActivatePro(EAPF_ATTACH)) )
	{
		gIdentifyManager->setEquipIdntifyResult(player, pSrcItem);
	}
	//装备时绑定
	if(pSrcItem->getRes()->getBindMode() == Res::BINDMODE_EQUIP && !pSrcItem->getRes()->IsActivatePro(EAPF_BIND))
	{
		pSrcItem->getRes()->setBindPlayerID(player->getPlayerID());
		pSrcItem->getRes()->setActivatePro(EAPF_BIND);
	}

	// 是背包,非摆摊样式背包
	if(pSrcItem->getRes()->isBagItem() && !pSrcItem->getRes()->isStallStyleBag())
	{
		msg = g_ItemManager->setChangeSize(player, param->DestIndex, pSrcItem);
		if(msg != MSG_NONE)
			return msg;
	}
	
	//是背包,是摆摊样式背包且摆摊状态下
	else if(pSrcItem->getRes()->isStallStyleBag() && player->getInteractionState() == Player::INTERACTION_STALLTRADE)
	{
		//if(pDestItem)
		//	player->mShapeStack.remove(ShapeStack::Stack_Stall, -1);
		//StringTableEntry szModeName = pSrcItem->getRes()->getLinkModelName(0, player->getSex());
		//if(szModeName && szModeName[0] != 0)
		//	player->mShapeStack.add(ShapeStack::Stack_Stall, -1, szModeName, NULL, 0, 0);
		//else
		//	player->mShapeStack.add(ShapeStack::Stack_Stall, -1, Default_Stall_ModeName, NULL, 0, 0);
	}
	else//是装备，换普通装备，普通时装或门宗时装
	{
		// 换装
		unmountEquipModel(player, pDestItem);
		mountEquipModel(player, pSrcItem);
		unmountEquipAttribute(player, pDestItem);
		mountEquipAttribute(player, pSrcItem);
	}

	U32 SrcItemID = pSrcItem->getRes()->getItemID();
	U32 TagItemID = 0;
	// 如果目标装备栏槽位有物品，先卸下装备
	if(pDestItem)
	{
		TagItemID = pDestItem->getRes()->getItemID();		
		// 判断是否卸装后消失
		if(pDestItem->getRes()->canBaseLimit(Res::ITEM_DROPEQUIP))
		{
			player->equipList.SetSlot(param->DestIndex, pSrcItem, true);
			player->inventoryList.SetSlot(param->SrcIndex, NULL, false);
		}
		else
		{
			player->equipList.SetSlot(param->DestIndex, pSrcItem, false);
			player->inventoryList.SetSlot(param->SrcIndex, pDestItem, false);
		}
	}
	else
	{
		player->equipList.SetSlot(param->DestIndex, pSrcItem, false);
		player->inventoryList.SetSlot(param->SrcIndex, NULL, false);
	}

	player->equipList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);

	//需要更新任务物品计数
	g_MissionManager->UpdateTaskItem(player, SrcItemID);
	g_MissionManager->UpdateTaskItem(player, TagItemID);

	//判断是否物品栏背包物品
	if(pSrcItem->getRes()->isInventoryBag())
	{
		player->inventoryList.UpdateMaxSlots(player->getControllingClient());
	}//是银行背包
	else if(pSrcItem->getRes()->isBankBag())
	{
		player->bankList.UpdateMaxSlots(player->getControllingClient());
	}//摆摊栏背包
	else if(pSrcItem->getRes()->isStallBag())
	{
		player->individualStallList.UpdateMaxSlots(player->getControllingClient());
	}
	
	ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_SHORTCUT_EXCHANGE);
	evt->SetInt32ArgValues(1, SHORTCUTTYPE_INVENTORY);
	player->getControllingClient()->postNetEvent(evt);

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 移动物品栏对象到银行栏位置
enWarnMessage ItemManager::InventoryMoveToBank(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;
	//判断是否银行交易状态
	msg = player->isBusy(Player::INTERACTION_BANKTRADE);
	if(msg != MSG_NONE)
		return msg;

	//判断银行是否交易保护
	if(player->getBankFlag())
		return MSG_BANK_LOCK;	

	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
				!player->bankList.IsVaildSlot(param->DestIndex))
		return msg;	

	ItemShortcut* pSrcItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->SrcIndex);
	ItemShortcut* pDestItem	= (ItemShortcut*)player->bankList.GetSlot(param->DestIndex);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;
	//判断物品可否存仓库
	if(!pSrcItem->getRes()->canBaseLimit(ITEM_SAVEBANK))
		return MSG_ITEM_CANNOTSAVEBANK;

	U32 SrcItemID = pSrcItem->getRes()->getItemID();
	U32 TagItemID = 0;
    if(pDestItem && isSameItem(pSrcItem, pDestItem))
	{
		TagItemID = pDestItem->getRes()->getItemID();
		S32 iCount = pSrcItem->getRes()->getQuantity() + pDestItem->getRes()->getQuantity();
		S32 iMaxNum = pDestItem->getRes()->getMaxOverNum();
		if(iCount <= iMaxNum)
		{
			player->inventoryList.SetSlot(param->SrcIndex, NULL, true);
			pDestItem->getRes()->setQuantity(iCount);
		}
		else
		{
			pSrcItem->getRes()->setQuantity(iCount - iMaxNum);
			pDestItem->getRes()->setQuantity(iMaxNum);
		}
	}
	else
	{
		// 目标为空或有物品但非同类物品，直接交换
		player->bankList.SetSlot(param->DestIndex, pSrcItem, false);
		player->inventoryList.SetSlot(param->SrcIndex, pDestItem, false);
	}
	player->bankList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);

	//需要更新任务物品计数
	g_MissionManager->UpdateTaskItem(player, SrcItemID);
	g_MissionManager->UpdateTaskItem(player, TagItemID);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 批量物品预操作（为玩家添加或删除物品，需要搭配BatchItem使用)
U32 ItemManager::putItem(U32 PlayerID, U32 ItemID, S32 ItemNum)
{
	stItemResult* pResult = new stItemResult;
	pResult->srcEntry	= ItemShortcut::CreateItem(ItemID, ItemNum);
	pResult->newEntry	= NULL;
	pResult->flag		= OP_NONE;
	pResult->num		= ItemNum;
	pResult->PlayerID	= PlayerID;
	mTempItemList.push_back(pResult);
	return mTempItemList.size() - 1;
}

// ----------------------------------------------------------------------------
// 预处理设置物品属性
bool ItemManager::setPutItemProperty(Player* player, U32 index, U32 type, S32 argc, ...)
{
	if(!player || index >= mTempItemList.size() || argc > 5)
		return false;

	stItemResult* pResult = mTempItemList[index];
	if(pResult || !pResult->srcEntry || pResult->PlayerID != player->getPlayerID())
		return false;

	char szArgParam[5][32];
	dMemset(szArgParam, 0, sizeof(szArgParam));
	va_list args;
	va_start(args, argc);
	for(S32 i = 0; i < argc; i++)
		sprintf_s(szArgParam[i], 32, va_arg(args, char*), 32);
	va_end(args);

	switch(type)
	{
	case 4:		//BindFriend
		{
			pResult->srcEntry->getRes()->setBindFriendID(dAtoi(szArgParam[0]));
		}
		break;
	}
	return true;
}
// ----------------------------------------------------------------------------
// 检查是否能进行批量物品操作
enWarnMessage ItemManager::detectPutItem(Player* player, Vector<stItemResult*> itemList, Vector<stItemResult*>& resultList, bool ignoreLock/* = false*/)
{
	resultList.clear();
	if(itemList.empty())
		return MSG_NONE;
	if(!player)
		return MSG_INVALIDPARAM;

	enWarnMessage msgCode = MSG_NONE;

	//=======首先将物品栏物品映射到一个临时列表里准备操作======================
	for(S32 i = 0; i < player->inventoryList.GetMaxSlots(); ++i)
	{
		stItemResult* pResult	= new stItemResult;
		pResult->flag			= OP_NONE;
		pResult->newEntry		= NULL;
		pResult->PlayerID		= player->getPlayerID();
		ItemShortcut* pItem		= (ItemShortcut*)player->inventoryList.GetSlot(i);
		if(pItem)
		{
			pResult->srcEntry	= pItem;
			pResult->num		= pItem->getRes()->getQuantity();
		}
		else
		{
			pResult->srcEntry	= NULL;
			pResult->num		= 0;
		}
		resultList.push_back(pResult);
	}

	//=======首先操作处理需要删除的物品========================================
	for(S32 i = 0; i < itemList.size(); ++i)
	{
		stItemResult* pTempSrc = itemList[i];
		ItemShortcut* pTempItem = NULL;
		if(!pTempSrc || !(pTempItem = pTempSrc->srcEntry))
			continue;

		//判断是否删除物品操作
		if(pTempSrc->num >= 0)
			continue;

		S32 iMaxOverNum  = pTempItem->getRes()->getMaxOverNum();
		S32 iSrcQuantity = 0 - pTempSrc->num;

		// 注:这里使用map是为了优化扣减物品的顺序（优先扣数量小的）
		std::map<U32, stItemResult* > tmpList;
		for(S32 j = 0; j < resultList.size(); ++j)
		{
			stItemResult* pResult = resultList[j];
			if(pResult->srcEntry && pResult->num != 0 &&
				(ignoreLock || (!ignoreLock && pResult->srcEntry->getSlotState() != ShortcutObject::SLOT_LOCK)) &&
				isSameItem(pResult->srcEntry, pTempItem))
			{
				U32 id = pResult->num * 100 + j;
				tmpList[id] = pResult; 
			}
		}

		for(std::map<U32,stItemResult* >::iterator itB = tmpList.begin(); itB != tmpList.end(); ++itB)
		{
			stItemResult* pResult = itB->second;
			if(pResult->num <= iSrcQuantity)
			{
				iSrcQuantity	-= pResult->num;
				pResult->num	= 0;
				pResult->flag	|= OP_FINDDELEMPTY;
			}
			else
			{
				pResult->num	-= iSrcQuantity;
				pResult->flag	|= OP_FINDDEL;
				iSrcQuantity	= 0;
				break;
			}
		}

		// 若物品数量仍然大于0, 则说明物品栏无法找到指定数量可删除的物品
		if(iSrcQuantity > 0)
		{
			msgCode = MSG_ITEM_NOENOUGHNUM;
			goto ClearAll;
		}
	}

	//========接着再处理需要添加的物品=========================================
	for(S32 i = 0; i < itemList.size(); ++i)
	{
		stItemResult* pTempSrc = itemList[i];
		ItemShortcut* pTempItem = NULL;
		if(!pTempSrc || !(pTempItem = pTempSrc->srcEntry))
			continue;

		//判断是否添加物品操作
		if(pTempSrc->num <= 0)
			continue;

		// 判断是否唯一性物品，如果需求数量超过1个，说明输入参数有误
		if(pTempItem->getRes()->isOnlyOne() && pTempSrc->num > 1)
		{
			msgCode = MSG_ITEM_CANNOTMULTI;
			goto ClearAll;
		}

		//若为装备物品且未生成过强化附加属性，这里必须随机强化附加属性
		if(pTempItem->getRes()->isEquip() && !pTempItem->getRes()->isRandStengthens())
			pTempItem->RandStrengthenExtData();

		S32 iMaxOverNum  = pTempItem->getRes()->getMaxOverNum();
		S32 iSrcQuantity = pTempSrc->num;

		bool bFind = false;		// 在物品栏是否存在至少一个同样的物品
		// 判断是否需要叠加
		if(iMaxOverNum > 1)
		{
			// 首先叠加能叠加的物品
			for(S32 j = 0; j < resultList.size(); ++j)
			{
				stItemResult* pResult = resultList[j];
				if(pResult->srcEntry && pResult->num != 0 &&
					(ignoreLock || (!ignoreLock && pResult->srcEntry->getSlotState() != ShortcutObject::SLOT_LOCK)) &&
					isSameItem(pResult->srcEntry, pTempItem))
				{
					// 对于物品的唯一性，却在物品栏找到同样的一个物品
					if(pResult->srcEntry->getRes()->isOnlyOne())
					{
						bFind = true;
						break;
					}

					if(pResult->num >= iMaxOverNum)
						continue;

					pResult->flag |= OP_FINDADD;
					if(pResult->num + iSrcQuantity > iMaxOverNum)
					{
						iSrcQuantity -= (iMaxOverNum -pResult->num);
						pResult->num = iMaxOverNum;
					}
					else
					{
						pResult->num += iSrcQuantity;
						iSrcQuantity = 0;
						break;
					}
				}
			}
		}
		else
		{
			// 对于不可叠加的唯一性物品，仍然需要查找是否已经在物品栏里
			if(pTempItem->getRes()->isOnlyOne())
			{
				for(S32 j = 0; j < resultList.size(); ++j)
				{
					stItemResult* pResult = resultList[j];
					if(pResult->srcEntry && pResult->num != 0 &&
						(ignoreLock || (!ignoreLock && pResult->srcEntry->getSlotState() != ShortcutObject::SLOT_LOCK)) &&
						isSameItem(pResult->srcEntry, pTempItem))
					{
						bFind = true;
						break;
					}
				}
			}
		}

		//若已经找到一个唯一性物品存在，则失败退出
		if(bFind)
		{
			msgCode = MSG_ITEM_CANNOTMULTI;
			goto ClearAll;
		}

		// 接着寻找空槽放置剩余的物品数量
		if(iSrcQuantity > 0)
		{
			for(S32 j = 0; j < resultList.size(); ++j)
			{
				stItemResult* pResult = resultList[j];
				if(pResult->newEntry)
				{
					if(pResult->num >= iMaxOverNum || !isSameItem(pResult->newEntry, pTempItem))
						continue;
					if(pResult->num + iSrcQuantity > iMaxOverNum)
					{
						iSrcQuantity -= (iMaxOverNum -pResult->num);
						pResult->num = iMaxOverNum;
					}
					else
					{
						pResult->num += iSrcQuantity;
						iSrcQuantity = 0;
						break;
					}
				}
				else if(!pResult->srcEntry || (pResult->srcEntry && pResult->num == 0))
				{
					ItemShortcut* pItem = ItemShortcut::CreateItem(pTempItem);
					if(pItem)
					{
						pResult->flag |= OP_FINDEMPTY;
						pResult->newEntry = pItem;
						if(iSrcQuantity > iMaxOverNum)
						{
							pResult->num = iMaxOverNum;
							iSrcQuantity -= iMaxOverNum;
						}
						else
						{
							pResult->num = iSrcQuantity;
							iSrcQuantity = 0;
							break;
						}
					}
				}				
			}
		}

		// 若物品数量仍然大于0, 则说明物品栏无法放入指定数量的物品
		if(iSrcQuantity > 0)
		{
			msgCode = MSG_ITEM_ITEMNOSPACE;
			goto ClearAll;
		}
	}
	return msgCode;
ClearAll:
	for(S32 i = 0; i < resultList.size(); ++i)
		delete resultList[i];
	resultList.clear();
	return msgCode;
}

// ----------------------------------------------------------------------------
// 提交批量操作物品结果
void ItemManager::donePutItem(Player* player, Vector<stItemResult*>& resultList)
{
	for(S32 i = 0; i < resultList.size(); ++i)
	{
		if(resultList[i]->flag == 0)
		{
			delete resultList[i];
			continue;
		}
		U32 itemID = 0;
		if(resultList[i]->srcEntry)
		{
			itemID = resultList[i]->srcEntry->getRes()->getItemID();
		}
		// 判断是否有删除到空槽的操作
		if(resultList[i]->flag & OP_FINDDELEMPTY)
		{
			if(resultList[i]->srcEntry)
			{
				player->inventoryList.SetSlot(i, NULL);
			}
			resultList[i]->srcEntry = NULL;
		}
		// 判断是否有删除减数量的操作
		if(resultList[i]->flag & OP_FINDDEL)
		{
			if(resultList[i]->srcEntry)
			{
				resultList[i]->srcEntry->getRes()->setQuantity(resultList[i]->num);
			}
		}
		// 判断是否有新增物品的操作
		if(resultList[i]->flag & OP_FINDEMPTY)
		{
			if(resultList[i]->newEntry)
			{
				U32 newItemID = resultList[i]->newEntry->getRes()->getItemID();
				// 同步快捷栏
				g_ItemManager->syncPanel(player, newItemID);
				resultList[i]->newEntry->getRes()->setQuantity(resultList[i]->num);
				player->inventoryList.SetSlot(i, resultList[i]->newEntry, false);
				g_MissionManager->UpdateTaskItem(player, newItemID);
			}				
		}
		// 判断是否有添加物品数量的操作
		if(resultList[i]->flag & OP_FINDADD)
		{
			if(resultList[i]->srcEntry)
			{
				resultList[i]->srcEntry->getRes()->setQuantity(resultList[i]->num);
			}
		}

		if(itemID != 0)
		{
			// 同步快捷栏
			g_ItemManager->syncPanel(player, itemID);
			g_MissionManager->UpdateTaskItem(player, itemID);
		}
		player->inventoryList.UpdateToClient(player->getControllingClient(), i, ITEM_GET);
		delete resultList[i];
	}
	resultList.clear();
}

// ----------------------------------------------------------------------------
// 为玩家批量操作物品(需要搭配putItem使用)
enWarnMessage ItemManager::batchItem(U32 PlayerID)
{
	Player* player = g_ServerGameplayState->GetPlayer(PlayerID);
	if(player == NULL)
	{
		clearTempItemList();
		return MSG_UNKOWNERROR;
	}

	Vector<stItemResult*> resultList;
	enWarnMessage msgCode = detectPutItem(player, mTempItemList, resultList, false);
	if(msgCode == MSG_NONE)
	{
		donePutItem(player, resultList);
		msgCode = MSG_NONE;		
	}

	// 清空临时物品
	clearTempItemList();
	return msgCode;
}

// ----------------------------------------------------------------------------
// 从物品栏中删除指定位置的物品
enWarnMessage ItemManager::delItemFromInventoryByIndex(Player* player, S32 index, S32 num /* = 1 */)
{
	if(!player || index < 0 || index >= InventoryList::MAXSLOTS || num <= 0)
		return MSG_UNKOWNERROR;
	ItemShortcut* pItem = (ItemShortcut*)(player->inventoryList.GetSlot(index));
	if(!pItem)
		return MSG_ITEM_NOFINDITEM;

	U32 itemid = pItem->getRes()->getItemID();
	S32 iNum = pItem->getRes()->getQuantity() - num;

	if(iNum > 0)
		pItem->getRes()->setQuantity(iNum);
	else
		player->inventoryList.SetSlot(index, NULL);
	
	//对于扣减数量不够，仍需继续查找相同物品进行扣减
	if(iNum < 0)
	{
		Vector<S32> samelist;
		S32 count = player->inventoryList.QuerySameObject(pItem, samelist, true, false);
		if(count > 0)
		{
			for(S32 i = 0; i < count; i++)
			{
				ItemShortcut* pTempItem = (ItemShortcut*)player->inventoryList.GetSlot(samelist[i]);
				if(pTempItem)
				{
					S32 iQuantity = pTempItem->getRes()->getQuantity();
					if(iQuantity > iNum)
					{
						pTempItem->getRes()->setQuantity(iQuantity - iNum);
						player->inventoryList.UpdateToClient(player->getControllingClient(), samelist[i], ITEM_LOSE);
						break;
					}
					else
					{
						player->inventoryList.SetSlot(samelist[i], NULL);
						player->inventoryList.UpdateToClient(player->getControllingClient(), samelist[i], ITEM_LOSE);
					}
				}
			}
		}
	}

	player->inventoryList.UpdateToClient(player->getControllingClient(), index, ITEM_LOSE);
	// 同步快捷栏
	g_ItemManager->syncPanel(player, itemid);
	//需要对任务物品判定
	g_MissionManager->UpdateTaskItem(player, itemid);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 从物品栏中删除单个物品(用物品ID实现)
enWarnMessage ItemManager::delItemFromInventory(Player* player, S32 itemID, S32 itemNum /* = 1 */, U32 flag /* = ITEM_LOSE */)
{
	ItemShortcut* pTempItem = ItemShortcut::CreateItem(itemID, 1);
	if(!pTempItem)
		return MSG_ITEM_ITEMDATAERROR;
	enWarnMessage enMsg = delItemFromInventory(player, pTempItem, itemNum, flag, true);
	delete pTempItem;
	return enMsg;
}

// ----------------------------------------------------------------------------
// 从物品栏中删除单个物品(用物品对象实现)
enWarnMessage ItemManager::delItemFromInventory(Player* player, ItemShortcut* pItem, S32 itemNum /* = 1 */, U32 flag /* = ITEM_LOSE */, bool ignoreExpro /*= false*/)
{
	if(!player || !pItem || itemNum <= 0)
		return MSG_ITEM_ITEMDATAERROR;

	Vector<S32> slotlist;
	S32 iCount = player->inventoryList.QuerySameObject(pItem, slotlist, true, ignoreExpro);
	U32 itemid = pItem->getRes()->getItemID();
	S32 num = itemNum;
	// 判断物品是否能重叠
	if(!pItem->getRes()->canLapOver())
	{	
		if(iCount < itemNum)
			return MSG_ITEM_NOENOUGHNUM;
		for (S32 i = iCount - 1; i >= 0; --i)
		{
			if(num == 0)
				break;
			player->inventoryList.SetSlot(slotlist[i],NULL, true);
			player->inventoryList.UpdateToClient(player->getControllingClient(), slotlist[i], flag);
			num--;
		}
	}
	else
	{
		int iNum = 0;
		for(S32 i = 0; i < iCount; i++)
		{
			ItemShortcut* pItem1 = (ItemShortcut*)player->inventoryList.GetSlot(slotlist[i]);
			if(pItem1)
				iNum += pItem1->getRes()->getQuantity();
		}

		if(iNum < num)
			return MSG_ITEM_NOENOUGHNUM;		

		for (S32 i = iCount-1; i>= 0; --i)
		{
			ItemShortcut* pItem1 = (ItemShortcut*)player->inventoryList.GetSlot(slotlist[i]);
			if(pItem1)
			{
				if(num < pItem1->getRes()->getQuantity())
				{
					pItem1->getRes()->setQuantity(pItem1->getRes()->getQuantity() - num);					
					num = 0;
					player->inventoryList.UpdateToClient(player->getControllingClient(), slotlist[i], flag);
					break;
				}
				else
				{
					num -= pItem1->getRes()->getQuantity();
					player->inventoryList.SetSlot(slotlist[i], NULL, true);
					player->inventoryList.UpdateToClient(player->getControllingClient(), slotlist[i], flag);
				}
			}
		}
	}

	// 同步快捷栏
	g_ItemManager->syncPanel(player, itemid);
	//需要对任务物品判定
	g_MissionManager->UpdateTaskItem(player, itemid);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 添加单个物品到物品栏（用物品ID实现）
enWarnMessage ItemManager::addItemToInventory(Player* player, S32 itemID, S32& index, S32 num, U32 flag)
{
	ItemShortcut* pTempItem = ItemShortcut::CreateItem(itemID, 1);
	if(!pTempItem)
		return MSG_ITEM_ITEMDATAERROR;
	enWarnMessage enMsg = addItemToInventory(player, pTempItem, index, num, flag);
	delete pTempItem;
	return enMsg;
}

// ----------------------------------------------------------------------------
// 添加单个物品到物品栏（用物品对象实现）
enWarnMessage ItemManager::addItemToInventory(Player* player, ItemShortcut* pItem, S32& index, S32 num, U32 flag)
{
	index = -1;
	if(!player || !pItem || num <= 0)
		return MSG_ITEM_ITEMDATAERROR;

	// 若待添加的物品是唯一性物品并且数量超过1，则直接返回
	if(pItem->getRes()->isOnlyOne() && num > 1)
		return MSG_ITEM_CANNOTMULTI;

	ItemShortcut* pCloneItem = ItemShortcut::CreateItem(pItem);
	if(!pCloneItem)
		return MSG_ITEM_ITEMDATAERROR;
	else
	{
		//若此装备之前未曾随机过强化等级附加属性，则这里必须做一次
		if(!pCloneItem->getRes()->isRandStengthens())
			pCloneItem->RandStrengthenExtData();
		Res* pRes = pCloneItem->getRes();
		if(!pRes)
			return MSG_ITEM_ITEMDATAERROR;
		//拾取时绑定
		if(pRes->getBindMode() == Res::BINDMODE_PICKUP && !pRes->IsActivatePro(EAPF_BIND))
		{
			pRes->setBindPlayerID(player->getPlayerID());
			pRes->setActivatePro(EAPF_BIND);
		}		
	}

	enWarnMessage msgCode = MSG_NONE;
	Vector<stItemResult*> resultList;
	for(S32 i = 0; i < player->inventoryList.GetMaxSlots(); ++i)
	{
		stItemResult* pResult	= new stItemResult;
		pResult->flag			= OP_NONE;
		ItemShortcut* pItemSrc	= (ItemShortcut*)player->inventoryList.GetSlot(i);
		if(pItemSrc)
		{
			pResult->srcEntry	= pItemSrc;
			pResult->num		= pItemSrc->getRes()->getQuantity();
		}
		else
		{
			pResult->srcEntry	= NULL;
			pResult->num		= 0;
		}
		resultList.push_back(pResult);
	}

	S32 iMaxOverNum  = pCloneItem->getRes()->getMaxOverNum();
	bool bFind = false;		// 在物品栏是否存在至少一个同样的物品
	// 判断待添加的物品是否可叠加
	if(iMaxOverNum > 1)
	{
		// 首先叠加能叠加的物品
		for(S32 i = 0; i < resultList.size(); ++i)
		{
			stItemResult* pResult = resultList[i];
			if(pResult->srcEntry && (pResult->srcEntry->getSlotState() != ShortcutObject::SLOT_LOCK) &&
				isSameItem(pResult->srcEntry, pCloneItem))
			{
				// 对于物品的唯一性，却在物品栏找到同样的一个物品
				if(pResult->srcEntry->getRes()->isOnlyOne())
				{
					bFind = true;
					break;
				}

				if(pResult->num >= iMaxOverNum)
					continue;

				pResult->flag = OP_FINDADD;
				if(pResult->num + num > iMaxOverNum)
				{
					num -= (iMaxOverNum -pResult->num);
					pResult->num = iMaxOverNum;
				}
				else
				{
					pResult->num += num;
					num = 0;
					break;
				}
			}
		}
	}
	else
	{
		// 对于不可叠加的唯一性物品，仍然需要查找是否已经在物品栏里
		if(pCloneItem->getRes()->isOnlyOne())
		{
			for(S32 i = 0; i < resultList.size(); ++i)
			{
				stItemResult* pResult = resultList[i];
				if(pResult->srcEntry &&	(pResult->srcEntry->getSlotState() != ShortcutObject::SLOT_LOCK) &&
					isSameItem(pResult->srcEntry, pCloneItem))
				{
					bFind = true;
					break;
				}
			}
		}
	}

	//查找到唯一性物品在物品栏已经存在
	if(bFind)
	{
		msgCode = MSG_ITEM_CANNOTMULTI;
		goto ClearAll;
	}

	// 接着寻找空槽放置剩余的物品数量
	if(num > 0)
	{
		for(S32 i = 0; i < resultList.size(); ++i)
		{
			stItemResult* pResult = resultList[i];
			if(!pResult->srcEntry)
			{
				ItemShortcut* pTagetItem = ItemShortcut::CreateItem(pCloneItem);
				if(pTagetItem)
				{					
					pResult->flag = OP_FINDEMPTY;
					pResult->srcEntry = pTagetItem;
					if(num > iMaxOverNum)
					{
						pResult->num = iMaxOverNum;
						num -= iMaxOverNum;
					}
					else
					{
						pResult->num = num;
						num = 0;
						break;
					}
				}
			}				
		}
	}

	// 若物品数量仍然大于0, 则说明物品栏无法放入指定数量的物品
	if(num > 0)
	{
		msgCode = MSG_ITEM_ITEMNOSPACE;
		goto ClearAll;
	}
	else
	{
		U32 itemID;
		for(S32 i = 0; i < resultList.size(); ++i)
		{
			if(!resultList[i]->srcEntry || resultList[i]->flag == OP_NONE)
				continue;
			// 判断是否有新增物品的操作
			if(resultList[i]->flag == OP_FINDEMPTY)
			{
				itemID = resultList[i]->srcEntry->getRes()->getItemID();
				resultList[i]->srcEntry->getRes()->setQuantity(resultList[i]->num);
				player->inventoryList.SetSlot(i, resultList[i]->srcEntry);
				RegisterTimeItem(player, resultList[i]->srcEntry);
			}
			// 判断是否有添加物品数量的操作
			else if(resultList[i]->flag == OP_FINDADD)
			{
				itemID = resultList[i]->srcEntry->getRes()->getItemID();
				resultList[i]->srcEntry->getRes()->setQuantity(resultList[i]->num);
				RegisterTimeItem(player, resultList[i]->srcEntry);
			}

			index = i;
			g_ItemManager->syncPanel(player, itemID);
			g_MissionManager->UpdateTaskItem(player, itemID);
			player->inventoryList.UpdateToClient(player->getControllingClient(), i, flag);
		}
	}
ClearAll:
	if(pCloneItem)
		delete pCloneItem;
	for(S32 i = 0; i < resultList.size(); ++i)
		delete resultList[i];
	resultList.clear();
	return msgCode;
}

enWarnMessage ItemManager::RegisterTimeItem(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return MSG_UNKOWNERROR;

	Res* pRes = pItem->getRes();
	//拾取时开始计时
	if(pRes->getTimeMode() == Res::TIMEMODE_ONLINEGET || pRes->getTimeMode() == Res::TIMEMODE_ALLGET)
	{
		pRes->setLapseTime(pRes->getDuration());
		Player::stTimeItem* item = new Player::stTimeItem;
		item->uID = pRes->getUID();
		item->uLeaveTime = pRes->getDuration();
		item->uStartTime = Platform::getTime();
		player->pushToTimeItemVec(item);
	}
	else if(pRes->getTimeMode() == Res::TIMEMODE_ALL || pRes->getTimeMode() == Res::TIMEMODE_ONLINEUSE)
	{
		pRes->setLapseTime(pRes->getDuration());
		Player::stTimeItem* item = new Player::stTimeItem;
		item->uID = pRes->getUID();
		item->uLeaveTime = pRes->getDuration();
		item->uStartTime = Platform::getTime();
		player->pushToTimeItemVec(item);
	}
}


//从物品栏移动到修理栏
enWarnMessage ItemManager::InventoryMoveToRepairList(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
		!player->mRepairList.IsVaildSlot(param->DestIndex))
		return msg;	
	if(param->DestIndex != 0)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	ItemShortcut* pSrcItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->SrcIndex);
	ItemShortcut* pDest	= (ItemShortcut*)player->mRepairList.GetSlot(0);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;

	if(!pSrcItem->getRes()->isWeapon() && !(pSrcItem->getRes()->isEquipment() && pSrcItem->getRes()->getSubCategory() < Res::CATEGORY_FASHION_FAMILY))
		return MSG_ITEM_CANNOTFIXED;
	if(!pSrcItem->getRes()->canBaseLimit(Res::ITEM_FIX))
		return MSG_ITEM_CANNOTFIXED;

	
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_PLAYER_TARGET_INVALID;
	if(pDest)
	{
		U32 iSrcIndex = pDest->getLockedItemIndex();
		ItemShortcut* pDestItem = ItemShortcut::CreateItem(pSrcItem);
		if(!pDestItem)
			return MSG_ITEM_ITEMDATAERROR;

		player->mRepairList.SetSlot(0, pDestItem, true);
		player->mRepairList.GetSlot(0)->setLockedItemIndex(param->SrcIndex);
		player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);		
		pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}
	else
	{
		ItemShortcut* pTargetItem = ItemShortcut::CreateItem(pSrcItem);
		if(!pTargetItem)
			return MSG_ITEM_ITEMDATAERROR;

		player->mRepairList.SetSlot(0, pTargetItem, true);
		player->mRepairList.GetSlot(0)->setLockedItemIndex(param->SrcIndex);

		pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);	
	}

	player->mRepairList.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);

	return MSG_NONE;
}

enWarnMessage ItemManager::RepairListMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_PLAYER_TARGET_INVALID;

	if(!player->inventoryList.IsVaildSlot(param->DestIndex) ||
		!player->mRepairList.IsVaildSlot(param->SrcIndex))
		return msg;	

	ItemShortcut* pSrcItem	= (ItemShortcut*)player->mRepairList.GetSlot(param->SrcIndex);
	ItemShortcut* pDestItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->DestIndex);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;
	if(param->SrcIndex != 0)
		return MSG_ITEM_CANNOTDRAG;

	S32 iSrcIndex = pSrcItem->getLockedItemIndex();
	if(pDestItem)
	{		
		if((!pDestItem->getRes()->isWeapon() && !(pDestItem->getRes()->isEquipment() && pDestItem->getRes()->getSubCategory() < Res::CATEGORY_FASHION_FAMILY)) || !pDestItem->getRes()->canBaseLimit(Res::ITEM_FIX))
		{
			player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
			player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
			player->mRepairList.SetSlot(0, NULL);
			player->mRepairList.UpdateToClient(conn, 0, ITEM_NOSHOW);
			return MSG_NONE;
		}
		ItemShortcut* pNewItem = ItemShortcut::CreateItem(pDestItem);
		if(!pNewItem)
			return MSG_ITEM_ITEMDATAERROR;
		player->mRepairList.SetSlot(0, pNewItem);
		player->mRepairList.GetSlot(0)->setLockedItemIndex(param->DestIndex);
		
		player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
		player->inventoryList.GetSlot(param->DestIndex)->setSlotState(ShortcutObject::SLOT_LOCK);

		player->mRepairList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
		player->inventoryList.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}
	else
	{
		player->mRepairList.SetSlot(0, NULL);
		player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
		player->mRepairList.UpdateToClient(conn, 0, ITEM_NOSHOW);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}
	return MSG_NONE;
}

enWarnMessage ItemManager::delFromRepairList(Player* pPlayer)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	if(!pPlayer)
		return MSG_PLAYER_TARGET_INVALID;
	ShortcutObject* pObject = pPlayer->mRepairList.GetSlot(0);
	if(!pObject)
		return MSG_NONE;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return MSG_PLAYER_TARGET_INVALID;
	else
	{
		S32 iSrcIndex = pObject->getLockedItemIndex();
		pPlayer->mRepairList.SetSlot(0, NULL);
		pPlayer->mRepairList.UpdateToClient(conn, 0, ITEM_NOSHOW);
		pPlayer->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
		pPlayer->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}
	return MSG_NONE;
}
#endif//NTJ_SERVER
 
#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// 物品拆分(设置状态)
ConsoleFunction(splitItem, void, 1, 1, "splitItem")
{
	ClientGameplayAction* old = g_ClientGameplayState->getCurrentAction();
	if(old)
	{
		if(old->getActionType() == INFO_ITEM_SPLIT)
		{
			g_ClientGameplayState->cancelCurrentAction();
			return;
		}
	}
	ClientGameplayAction* action = new ClientGameplayAction(3, INFO_ITEM_SPLIT);
	action->setCursorName("spilt");
	g_ClientGameplayState->setCurrentAction(action);	
}

// ----------------------------------------------------------------------------
// 物品拆分(拆分数量输入后)
ConsoleFunction(splitNums, void, 3, 3, "splitNums(%num, %index)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
	{
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}

	S32 uNum = dAtoi(argv[1]);
	S32 uIndex = dAtoi(argv[2]);
	ShortcutObject* pSrcSlot = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_INVENTORY, uIndex);
	ItemShortcut* pSrcItem = NULL;
	if(!pSrcSlot || !(pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot)) )
	{
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}

	if(uNum <= 0 || pSrcItem->getRes()->getQuantity() < uNum)
	{
		// 拆分物品数量不够
		//g_UIMouseGamePlay->AddScreenMessage(szWarnMessage[MSG_ITEM_SPLITNOENOUGH]);
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}

	ClientGameplayParam* param = new ClientGameplayParam;
	param->setIntArgValues(1, uNum);
	g_ClientGameplayState->setCurrentActionParam(param);
}

// ----------------------------------------------------------------------------
// 整理物品
ConsoleFunction(cleanUpInventory, void, 1, 1, "cleanUpInventory()")
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn)
	{
		ClientGameNetEvent* event = new ClientGameNetEvent(INFO_ITEM_COMPACT);
		conn->postNetEvent(event);
	}	
}

// ----------------------------------------------------------------------------
// 丢弃物品(设置状态)
ConsoleFunction(destroyItem, void, 1, 1, "destroyItem")
{
	ClientGameplayAction* old = g_ClientGameplayState->getCurrentAction();
	if(old)
	{
		if(old->getActionType() == INFO_ITEM_DROP)
		{
			g_ClientGameplayState->cancelCurrentAction();
			return;
		}
	}
	ClientGameplayAction* action = new ClientGameplayAction(1, INFO_ITEM_DROP);
	action->setCursorName("destory");
	g_ClientGameplayState->setCurrentAction(action);
}

// ----------------------------------------------------------------------------
// 丢弃物品(确定丢弃动作)
ConsoleFunction(dropItem, void, 2, 2, "dropItem(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
	{
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}
	U32 uIndex = dAtoi(argv[1]);

	ShortcutObject* pShortcut_slot = g_ItemManager->getShortcutSlot(pPlayer, SHORTCUTTYPE_INVENTORY, uIndex);
	if(!pShortcut_slot)
	{
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}

	ClientGameplayParam* param = new ClientGameplayParam;
	param->setIntArgValues(2, SHORTCUTTYPE_INVENTORY, uIndex);
	g_ClientGameplayState->setCurrentActionParam(param);
}

// ----------------------------------------------------------------------------
// 摆摊
ConsoleFunction(stallage, void, 1, 1, "stallage")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;

	if(player->getInteractionState() == Player::INTERACTION_STALLTRADE)
	{
		Con::executef("OpenOrCloseStallWnd");
		return;
	}
	if(!g_Stall->canOpenStall(player, stall_individual))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_TRADE_NOSET_STALL);
		return;
	}
	Point3F pos;
	if(gClientLogicMarkNodeManager->getClosestMarkNode( player->getPosition(), LMNT_STALL, pos))
		player->SetStallPath(pos);
	else
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_STALL_NOPOSITION);
		return;
	}
}


// ----------------------------------------------------------------------------
// 取消前一次操作
ConsoleFunction(cancelAction, void, 1, 1, "cancelAction")
{
	g_ClientGameplayState->cancelCurrentAction();
}

// ----------------------------------------------------------------------------
// 得到包裹大小
ConsoleFunction(GetBagSize, S32, 2, 2, "GetBagSize(%BagIndex)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return 0;
	S32 index = dAtoi(argv[1]);
	if(index == 0)
		return 40;
	else if(index == 1)
		return pPlayer->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK1);
	else if(index == 2)
		return pPlayer->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK2);
	else if(index == 3)
		return pPlayer->equipList.GetBagSize(Res::EQUIPPLACEFLAG_BANKPACK);
	else if(index == 4)
		return pPlayer->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK1);
	else if(index == 5)
		return pPlayer->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK2);
	else
		return 0;		
}

// ----------------------------------------------------------------------------
// 设置物品筛选
ConsoleFunction(isBagType, bool, 3, 3, "isBagType(%SlotIndex, %type)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return false;
	ItemShortcut* pItem = (ItemShortcut*)pPlayer->inventoryList.GetSlot(dAtoi(argv[1]));
	switch(dAtoi(argv[2]))
	{
	case 1: return pItem && !pItem->getRes()->isTaskItem() && !pItem->getRes()->isMaterial();
	case 2: return pItem && pItem->getRes()->isTaskItem();
	case 3: return pItem && pItem->getRes()->isMaterial();
	default: break;
	}
	return true;
}

// ----------------------------------------------------------------------------
//取消拖动操作
ConsoleFunction(cancleExchange, void, 1, 1, "cancleExchange()")
{
	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if(!currentAction)
		return;
	if(currentAction->getActionType() == INFO_SHORTCUT_EXCHANGE)
	{
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}
}

ConsoleFunction(equipBindItem, void, 3, 3, "equipBindItem(%index, %flag)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	U32 index = dAtoi(argv[1]);
	U32 flag = dAtoi(argv[2]);
	if(flag == 1)
	{
		ClientGameplayParam* param1 = new ClientGameplayParam;
		param1->setIntArgValues(2, SHORTCUTTYPE_EQUIP, index);
		g_ClientGameplayState->setCurrentActionParam(param1);
	}
	else 
	{
		GameConnection* conn = pPlayer->getControllingClient();
		if(!conn)
			return;
		
		ClientGameNetEvent *event = new ClientGameNetEvent(INFO_SHORTCUT_EXCHANGE);
		event->SetIntArgValues(4, SHORTCUTTYPE_INVENTORY, index, SHORTCUTTYPE_EQUIP, -1);
		event->SetInt32ArgValues(1, 0);
		conn->postNetEvent(event);
	}
	
}
//经久修理
ConsoleFunction(repairCurrentMaxWear, void, 1, 1, "repairCurrentMaxWear()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	ShortcutObject* pShortcut = pPlayer->mRepairList.GetSlot(0);
	if(!pShortcut)
		return;
	ItemShortcut* pItem = (ItemShortcut*)pShortcut;
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return;
	if(pRes->getCurrentMaxWear() == -1)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_CANNOTNEEDFIXED);
		return;
	}
	U32 needMoney1 = pRes->getRepairNeedMoney();
	if(!pPlayer->canReduceMoney(needMoney1, 1))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_PLAYER_MONEYNOTENOUGH);
		return;
	}
	S32 index = pPlayer->inventoryList.FindSameObjectByID(ItemManager::REPAIRCOST_ITEM, true);

	if(index < 0)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_NOENOUGHNUM);
		return;
	}
	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_ITEM_REPAIR);
	evt->SetInt32ArgValues(2, DURABLE_REPAIR, needMoney1);
	pPlayer->getControllingClient()->postNetEvent(evt);
}
//最大耐久恢复
ConsoleFunction(resumeMaxWear, void, 1, 1, "resumeMaxWear()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	ShortcutObject* pShortcut = pPlayer->mRepairList.GetSlot(0);
	if(!pShortcut)
		return;
	ItemShortcut* pItem = (ItemShortcut*)pShortcut;
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return;
	if(pRes->getCurrentWear() == pRes->getMaxWear())
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_CANNOTNEEDFIXED);
		return;
	}
	U32 needMoney1 = pRes->getRepairNeedMoney();
	U32 needmoney2 = pRes->getRepairNeedMoney1();
	if(!pPlayer->canReduceMoney(needMoney1 + needmoney2, 1))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_PLAYER_MONEYNOTENOUGH);
		return;
	}
	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_ITEM_REPAIR);
	evt->SetInt32ArgValues(2, RECOVER_REPAIR, needMoney1 + needmoney2);
	pPlayer->getControllingClient()->postNetEvent(evt);
}

ConsoleFunction(closeRepair, void, 1, 1, "closeRepair()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_ITEM_REPAIR);
	event->SetInt32ArgValues(2, REPAIR_CLOSE, 0);
	conn->postNetEvent(event);
}
#endif//NTJ_CLIENT

bool ItemManager::openRepair(Player* pPlayer, U32 type)
{
	if(!pPlayer)
		return false;
#ifdef NTJ_SERVER
	SceneObject* obj = pPlayer->getInteraction();
	pPlayer->setInteraction(obj, Player::INTERACTION_REPAIR);

	// 判断如果已经存在一种事件状态就不能打开修理
	enWarnMessage MsgCode = pPlayer->isBusy(Player::INTERACTION_REPAIR);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_ERROR, MsgCode);
		return false;
	}
	if(type == 1)
	{
		ItemShortcut* newItem = ItemShortcut::CreateItem(ItemManager::REPAIRCOST_ITEM, 1);
		if(!newItem)
			return false;
		pPlayer->mRepairList.SetSlot(1, newItem);
		pPlayer->mRepairList.UpdateToClient(pPlayer->getControllingClient(), 1, ITEM_NOSHOW);
	}
	ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_ITEM_REPAIR);
	ev->SetInt32ArgValues(1, type);
	pPlayer->getControllingClient()->postNetEvent(ev);
#endif
#ifdef NTJ_CLIENT
	Con::executef("openRepair", Con::getIntArg(type));
	if(type == 1)
	{
		bool isHave = false;
		ShortcutObject* pShortcut = pPlayer->mRepairList.GetSlot(1);
		if(!pShortcut)
			isHave = false;
		if(pPlayer->inventoryList.FindSameObject(pShortcut, false, true) >= 0)
			isHave = true;
		char text[64] = {0};
		if(isHave)
			dSprintf(text, sizeof(text), "<t c='0xffffffff'>%d/1</t>", 1);
		else
			dSprintf(text, sizeof(text), "<t c='0xff0000ff'>%d/1</t>", 0);
		Con::executef("showRepairNum", text);
	}
#endif
	return true;
}

void ItemManager::closeRepair(Player* pPlayer)
{
	if(!pPlayer)
		return;
#ifdef NTJ_CLIENT
	Con::executef("closeRepairWnd");
#endif
#ifdef NTJ_SERVER
	for(int i=0; i<2; ++i)
	{
		ShortcutObject* pObj = pPlayer->mRepairList.GetSlot(i);
		if(pObj)
		{
			if(i==0)
			{
				U32 iSrcIndex = pObj->getLockedItemIndex();
				pPlayer->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
				pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), iSrcIndex, ITEM_NOSHOW);

			}
			pPlayer->mRepairList.SetSlot(i, NULL, true);
			pPlayer->mRepairList.UpdateToClient(pPlayer->getControllingClient(), i, ITEM_NOSHOW);
		}
	}
#endif
}


#ifdef NTJ_SERVER
//ConsoleFunction(openRepair,bool, 3, 3, "openRepair(%player, %type)" )
//{
//	SimObject* obj = Sim::findObject(dAtoi(argv[1]));
//	if(obj == NULL)
//		return false;
//	Player* player = dynamic_cast<Player*>(obj);
//	if(player == NULL)
//		return false;
//	U32 type = dAtoi(argv[2]);
//	if(type < 0)
//		return false;
//	return g_ItemManager->openRepair(player, type);
//}
ConsoleMethod(Player, openRepair, bool, 3, 3, "openRepair(%openType)")
{
	return g_ItemManager->openRepair(object, dAtoi(argv[2]));
}

#endif

RepairList::RepairList() :BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_REPAIR;
}

bool RepairList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	RepairEvent* ev = new RepairEvent(index);
	conn->postNetEvent(ev);
#endif
	return true;
}