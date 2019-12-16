//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "T3D/gameConnection.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/Mission/PlayerMission.h"
#include "Gameplay/item/Player_EquipIdentify.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

EquipList::EquipList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_EQUIP;
}

// ----------------------------------------------------------------------------
// 从数据库加载玩家装备栏数据
bool EquipList::LoadData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	for(U32 i = 0; i < mMaxSlots; ++i)
	{
		stItemInfo& stTemp = playerInfo->DispData.EquipInfo[i];
		if(stTemp.ItemID == 0)
			continue;
		//创建新角色初始化装备信息
		if(playerInfo->DispData.OtherFlag & Player::OTHERFLAG_NEWBIE)
		{
			mSlots[i] = ItemShortcut::CreateItem(stTemp.ItemID, 1);
			ItemShortcut* pItem = (ItemShortcut*)mSlots[i];
			Res* pRes = pItem->getRes();
			if(pRes && pRes->getBindMode() == Res::BINDMODE_PICKUP && !pRes->IsActivatePro(EAPF_BIND))
			{
				pRes->setBindPlayerID(playerInfo->BaseData.PlayerId);
				pRes->setActivatePro(EAPF_BIND);
			}
		}
		else
			mSlots[i] = ItemShortcut::CreateItem(stTemp);		
	}
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 发送初始化的装备栏数据到客户端
void EquipList::SendInitialData(GameConnection* conn)
{
	for(S32 i = 0; i < mMaxSlots; ++i)
	{
		if(mSlots[i])
		{
			EquipEvent* event = new EquipEvent(i, ITEM_LOAD);
			conn->postNetEvent(event);
		}
	}
}

// ----------------------------------------------------------------------------
// 获取装备栏装备扩展背包的容量大小
S32  EquipList::GetBagSize(S32 index)
{
	if(index < 0 || index >= mMaxSlots)
		return 0;
	if(mSlots[index])
	{
		ItemShortcut* pItem = (ItemShortcut*)mSlots[index];
		return pItem->getRes()->isBagItem() ? pItem->getRes()->getReserveValue() : 0;
	}
	return 0;
}

// ----------------------------------------------------------------------------
// 获取扩展背包的位置大小
ItemShortcut* EquipList::GetBagItem(S32 index)
{
	if(index < 0 || index >= mMaxSlots)
		return NULL;
	if(mSlots[index])
	{
		ItemShortcut* pItem = (ItemShortcut*)mSlots[index];
		return pItem->getRes()->isBagItem() ? pItem: NULL;
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// 获取装备耐久需要修理的价格
S32 EquipList::GetRepairCost()
{
	U32 iCost = 0;
	ItemShortcut* pItem = NULL;
	for(int i=Res::EQUIPPLACEFLAG_HEAD; i<Res::EQUIPPLACEFLAG_TRUMP; ++i)
	{
		if(i != Res::EQUIPPLACEFLAG_NECK && i != Res::EQUIPPLACEFLAG_RING && i != Res::EQUIPPLACEFLAG_AMULET)
		{
			pItem = (ItemShortcut*)mSlots[i];
			if(!pItem)
				continue;
			if(!pItem->getRes()->canBaseLimit(Res::ITEM_FIX))
				continue;
			if(pItem->getRes()->getCurrentWear() == pItem->getRes()->getCurrentMaxWear())
				continue;
			iCost += pItem->getRes()->getRepairNeedMoney();
		}
	}
	return iCost;
}

// ----------------------------------------------------------------------------
// 存储玩家装备栏到数据库
bool EquipList::SaveData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	for(S32 i = 0; i < mMaxSlots; ++i)
	{
		if(mSlots[i] == NULL)
			continue;
		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(mSlots[i]);
		stItemInfo* pTemp = NULL;
		if(pItem)
		{
			// 不保存下线消失的物品
			if(pItem->getRes()->canBaseLimit(Res::ITEM_DOWNLINE))
				continue;
			pTemp = pItem->getRes()->getExtData();
			if(pTemp)
				dMemcpy(&playerInfo->DispData.EquipInfo[i], pTemp, sizeof(stItemInfo));
		}
	}
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 更新玩家装备栏数据到客户端
bool EquipList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	EquipEvent* ev = new EquipEvent(index, flag);
	conn->postNetEvent(ev);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 判断是否能拖动物品到装备栏
enWarnMessage ItemManager::canDragToEquipment(Player* player, ItemShortcut* pItem, S32 SlotIndex /*=-1*/)
{
	if(!player || !pItem)
		return MSG_INVALIDPARAM;

	//是否为装备物品
	if(!pItem->getRes()->isEquip())
		return MSG_ITEM_CANNOTMOUNT;
	//等级判断
	if(!pItem->getRes()->canLevelLimit(player->getLevel()))
		return MSG_PLAYER_LEVEL;
	//职业判断
	if(!pItem->getRes()->canFamilyLimit(player->getFamily()))
		return MSG_PLAYER_FAMILY;
	//性别判断
	if(!pItem->getRes()->canSexLimit(player->getSex()))
		return MSG_PLAYER_SEX;
	//使用次数判断
	if(pItem->getRes()->getResUsedTimes() != 0 && pItem->getRes()->getUsedTimes() == 0)
		return MSG_ITEM_CANNOTUSE;
#ifdef NTJ_SERVER
	//时效装备判断
	if(pItem->getRes()->getTimeMode() != Res::TIMEMODE_NONE)
	{
		U32 dis = Platform::getTime() - player->mBeginPlayerTime;
		if(pItem->getRes()->getLapseTime() - dis <= 0)
			return MSG_ITEM_CANNOTUSE;
	}
#endif
	// 若指定检查具体位置
	if(SlotIndex != -1 && !checkDragToEquipment(player, pItem, SlotIndex))
		return MSG_ITEM_MOUNTPOSITIONERROR;	
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 检查拖动物品到装备栏的位置是否正确
bool ItemManager::checkDragToEquipment(Player* player, ItemShortcut* pItem, S32 index)
{
	if(!player || !pItem)
		return false;
	//判断背包物品
	if(pItem->getRes()->isBagItem())
	{
		if((pItem->getRes()->isInventoryBag() && index != Res::EQUIPPLACEFLAG_PACK1 && index != Res::EQUIPPLACEFLAG_PACK2) ||
			(pItem->getRes()->isBankBag() && index != Res::EQUIPPLACEFLAG_BANKPACK) ||
			(pItem->getRes()->isStallBag() && index != Res::EQUIPPLACEFLAG_STALLPACK1 && index != Res::EQUIPPLACEFLAG_STALLPACK2) ||
			(pItem->getRes()->isStallStyleBag() && index != Res::EQUIPPLACEFLAG_STALLPACK3))
			return false;
	}
	else if(pItem->getRes()->getEquipPos() != index)
	{
		return false;
	}
	return true;
}

// ----------------------------------------------------------------------------
// 获取物品在装备栏的对应位置
S32 ItemManager::getItemEquipPos(Player* player, ItemShortcut* pItem, bool ignoreExist/*=false*/)
{
	if(!player || !pItem)
		return -1;

	if(pItem->getRes()->isInventoryBag())		//包裹栏的扩展背包
	{
		ItemShortcut* BagItem1 = (ItemShortcut*)player->equipList.GetSlot(Res::EQUIPPLACEFLAG_PACK1);
		ItemShortcut* BagItem2 = (ItemShortcut*)player->equipList.GetSlot(Res::EQUIPPLACEFLAG_PACK2);
		if(!BagItem1)
			return Res::EQUIPPLACEFLAG_PACK1;
		if(!BagItem2)
			return Res::EQUIPPLACEFLAG_PACK2;
		if(ignoreExist)
			return Res::EQUIPPLACEFLAG_PACK1;
	}
	else if(pItem->getRes()->isBankBag())		//银行栏的扩展背包
	{
		ItemShortcut* BagItem1 = (ItemShortcut*)player->equipList.GetSlot(Res::EQUIPPLACEFLAG_BANKPACK);
		if(!BagItem1 || ignoreExist)
			return Res::EQUIPPLACEFLAG_BANKPACK;
	}
	else if(pItem->getRes()->isStallBag())		//摆摊栏的扩展背包
	{
		ItemShortcut* BagItem1 = (ItemShortcut*)player->equipList.GetSlot(Res::EQUIPPLACEFLAG_STALLPACK1);
		ItemShortcut* BagItem2 = (ItemShortcut*)player->equipList.GetSlot(Res::EQUIPPLACEFLAG_STALLPACK2);
		if(!BagItem1)
			return Res::EQUIPPLACEFLAG_STALLPACK1;
		if(!BagItem2)
			return Res::EQUIPPLACEFLAG_STALLPACK2;
		if(ignoreExist)
			return Res::EQUIPPLACEFLAG_STALLPACK1;
	}
	else if(pItem->getRes()->isStallStyleBag())	//摆摊栏的样式背包
	{
		ItemShortcut* BagItem1 = (ItemShortcut*)player->equipList.GetSlot(Res::EQUIPPLACEFLAG_STALLPACK3);
		if(!BagItem1 || ignoreExist)
			return Res::EQUIPPLACEFLAG_STALLPACK3;
	}
	else if(pItem->getRes()->isEquip())			//装备物品
	{
		return pItem->getRes()->getEquipPos();
	}
	return -1;
}

#ifdef NTJ_SERVER

// ----------------------------------------------------------------------------
// 装备全修
enWarnMessage ItemManager::repairAllEquip(Player* player)
{
	if(!player)
		return MSG_UNKOWNERROR;

	for (S32 i = Res::EQUIPPLACEFLAG_HEAD; i < Res::EQUIPPLACEFLAG_TRUMP; ++i)
	{
		if(i != Res::EQUIPPLACEFLAG_NECK && i != Res::EQUIPPLACEFLAG_RING && i != Res::EQUIPPLACEFLAG_AMULET)
		{
			ItemShortcut* pItem = (ItemShortcut*)(player->equipList.GetSlot(i));
			if(!pItem)
				continue;
			if(!pItem->getRes()->canBaseLimit(Res::ITEM_FIX))
				continue;
			if(pItem->getRes()->getCurrentWear() == pItem->getRes()->getCurrentMaxWear())
				continue;
			pItem->getRes()->setRepairResult(Res::REPAIR_NORMAL);
			player->equipList.SetSlot(i, pItem, false);
			player->equipList.UpdateToClient(player->getControllingClient(), i, ITEM_REPAIRSUCC);
		}
	}
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 单修物品
enWarnMessage ItemManager::repairOneEquip(Player* player, U32 type, U32 slotType, S32 index)
{
	if(!player)
		return MSG_UNKOWNERROR;
	switch(slotType)
	{
	case SHORTCUTTYPE_EQUIP:
		{
			ItemShortcut* pItem = (ItemShortcut*)player->equipList.GetSlot(index);
			if(!pItem)
				return MSG_UNKOWNERROR;
			if(!pItem->getRes()->canBaseLimit(Res::ITEM_FIX))
				return MSG_ITEM_CANNOTFIXED;
			if(pItem->getRes()->getCurrentWear() == pItem->getRes()->getCurrentMaxWear())
				return MSG_ITEM_CANNOTNEEDFIXED;
			pItem->getRes()->setRepairResult(type);
			player->equipList.SetSlot(index, pItem, false);
			player->equipList.UpdateToClient(player->getControllingClient(), index, ITEM_REPAIRSUCC);
		}
		break;
	case SHORTCUTTYPE_INVENTORY:
		{
			ItemShortcut* pItem = (ItemShortcut*)player->inventoryList.GetSlot(index);
			if(!pItem)
				return MSG_UNKOWNERROR;
			if(!pItem->getRes()->canBaseLimit(Res::ITEM_FIX))
				return MSG_ITEM_CANNOTFIXED;
			if(pItem->getRes()->getCurrentWear() == pItem->getRes()->getCurrentMaxWear())
				return MSG_ITEM_CANNOTNEEDFIXED;
			pItem->getRes()->setRepairResult(type);
			player->inventoryList.SetSlot(index, pItem, false);
			player->inventoryList.UpdateToClient(player->getControllingClient(), index, ITEM_REPAIRSUCC);
		}
		break;
	case SHORTCUTTYPE_REPAIR:
		{
			ItemShortcut* pItem = (ItemShortcut*)player->mRepairList.GetSlot(0);
			if(!pItem)
				return MSG_UNKOWNERROR;
			if(!pItem->getRes()->canBaseLimit(Res::ITEM_FIX))
				return MSG_ITEM_CANNOTFIXED;
			if(type == Res::REPAIR_DURABLE)
			{
				if(pItem->getRes()->getCurrentMaxWear() == -1)
					return MSG_ITEM_CANNOTNEEDFIXED;
			}
			else if(type == Res::REPAIR_RECOVER)
			{
				if(pItem->getRes()->getCurrentWear() == pItem->getRes()->getCurrentMaxWear())
					return MSG_ITEM_CANNOTNEEDFIXED;
			}
			
			pItem->getRes()->setRepairResult(type);
			S32 iLockItemIndex = pItem->getLockedItemIndex();
			player->inventoryList.SetSlot(iLockItemIndex, pItem);
			player->inventoryList.UpdateToClient(player->getControllingClient(), iLockItemIndex, ITEM_REPAIRSUCC);
			player->mRepairList.SetSlot(0, NULL, false);
			player->mRepairList.UpdateToClient(player->getControllingClient(), 0, ITEM_NOSHOW);
		}
		break;
	}	
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 更改栏位大小
// index  为指定的扩展槽位索引
enWarnMessage ItemManager::setChangeSize(Player* player, U32 index, ItemShortcut* pItem)
{
	if(!player)
		return MSG_INVALIDPARAM;

	enWarnMessage msg = MSG_NONE;
	// 背包大小存储在保留字段里
	U32 changeNum = 0;
	if(pItem)
	 changeNum = pItem->getRes()->getReserveValue();
	switch (index)
	{
	case Res::EQUIPPLACEFLAG_PACK1:		//包裹栏扩展槽位1
		{
			U32 iSrc1Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK1);
			U32 iSrc2Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK2);
			msg = player->inventoryList.CanChangeMaxSlots(iSrc1Num, iSrc2Num, changeNum, iSrc2Num);
			if(msg != MSG_NONE)
				return msg;
			player->inventoryList.ChangeMaxSlots(player, iSrc1Num, iSrc2Num, changeNum, iSrc2Num);
		}
		break;
	case Res::EQUIPPLACEFLAG_PACK2:		//包裹栏扩展槽位2
		{
			U32 iSrc1Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK1);
			U32 iSrc2Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK2);
			msg = player->inventoryList.CanChangeMaxSlots(iSrc1Num, iSrc2Num, iSrc1Num, changeNum);
			if(msg != MSG_NONE)
				return msg;
			player->inventoryList.ChangeMaxSlots(player, iSrc1Num, iSrc2Num, iSrc1Num, changeNum);
		}
		break;
	case Res::EQUIPPLACEFLAG_BANKPACK:	//银行仓库栏扩展槽位
		{
			U32 iSrcNum = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_BANKPACK);
			msg = player->bankList.CanChangeMaxSlots(iSrcNum, changeNum);
			if(msg != MSG_NONE)
				return msg;
			player->bankList.ChangeMaxSlots(player, iSrcNum, changeNum);
		}
		break;
	case Res::EQUIPPLACEFLAG_STALLPACK1://摆摊栏扩展槽位1
		{
			U32 iSrc1Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK1);
			U32 iSrc2Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK2);
			msg = player->individualStallList.CanChangeMaxSlots(iSrc1Num, iSrc2Num, changeNum, iSrc2Num);
			if(msg != MSG_NONE)
				return msg;
			player->individualStallList.ChangeMaxSlots(player, iSrc1Num, iSrc2Num, changeNum, iSrc2Num);
		}
		break;
	case Res::EQUIPPLACEFLAG_STALLPACK2://摆摊栏扩展槽位2
		{
			U32 iSrc1Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK1);
			U32 iSrc2Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK2);
			msg = player->individualStallList.CanChangeMaxSlots(iSrc1Num, iSrc2Num, iSrc1Num, changeNum);
			if(msg != MSG_NONE)
				return msg;
			player->individualStallList.ChangeMaxSlots(player, iSrc1Num, iSrc2Num, iSrc1Num, changeNum);
		}
		break;
	//case Res::CATEGORY_STAL_STYLE_PACK://摆摊栏样式槽位
	//	{

	//	}
	//	break;
	}
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 移动装备栏物品到物品栏
enWarnMessage ItemManager::EquipMoveToInventory(stExChangeParam* param)
{
	Player* player = param->player;
	if(!player)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	if(!player->inventoryList.IsVaildSlot(param->DestIndex) ||
		!player->equipList.IsVaildSlot(param->SrcIndex))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;	

	ItemShortcut* pSrcItem = (ItemShortcut*)player->equipList.GetSlot(param->SrcIndex);
	ItemShortcut* pDestItem = (ItemShortcut*)player->inventoryList.GetSlot(param->DestIndex);	
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;
	U32 SrcItemID = pSrcItem->getRes()->getItemID();
	U32 TagItemID = 0;
	if (pDestItem)
	{
		// 判断目标物品是否能拖到装备栏
		if(!checkDragToEquipment(player, pDestItem, param->SrcIndex))
			return MSG_ITEM_CANNOTDRAGTOOBJECT;
		TagItemID = pDestItem->getRes()->getItemID();
		//装备时鉴定
		if((pDestItem->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_EQUIP) && (!pDestItem->getRes()->IsActivatePro(EAPF_ATTACH)) )
		{
			gIdentifyManager->setEquipIdntifyResult(player, pDestItem);
		}
		//装备时绑定
		if(pDestItem->getRes()->getBindMode() == Res::BINDMODE_EQUIP && !pDestItem->getRes()->IsActivatePro(EAPF_BIND))
		{
			pDestItem->getRes()->setBindPlayerID(player->getPlayerID());
			pDestItem->getRes()->setActivatePro(EAPF_BIND);
		}
	}
	else
	{
		// 判断是否有空槽可放目标物品
		if(player->inventoryList.FindEmptySlot() == -1)
			return MSG_ITEM_ITEMNOSPACE;		
	}

	// 是背包,非摆摊样式背包
	if(pSrcItem->getRes()->isBagItem() && !pSrcItem->getRes()->isStallStyleBag())
	{
		// 不能将背包放到背包扩展的位置
		if(param->DestIndex >= EXTBAG_BEGINSLOT)
			return MSG_ITEM_CANNOTDRAGTOOBJECT;
		enWarnMessage msg = setChangeSize(player, param->SrcIndex, NULL);
		if(msg != MSG_NONE)
			return msg;
	}//是背包,是摆摊样式背包
	else if(pSrcItem->getRes()->isStallStyleBag())
	{
		if(player->getInteractionState() == Player::INTERACTION_STALLTRADE)
		{
			//player->mShapeStack.remove(ShapeStack::Stack_Stall, -1);
			//if(pDestItem)
			//{
			//	StringTableEntry modeName = pDestItem->getRes()->getLinkModelName(0, player->getSex());
			//	if(modeName && modeName[0] != 0)
			//		player->mShapeStack.add(ShapeStack::Stack_Stall, -1, modeName, NULL, 0, 0);
			//	else
			//		player->mShapeStack.add(ShapeStack::Stack_Stall, -1, "npcD04_001", NULL, 0, 0);
			//}
		}
		
	}
	else//是装备，换普通装备，普通时装或门宗时装
	{
		unmountEquipModel(player, pSrcItem);
		mountEquipModel(player, pDestItem);
		unmountEquipAttribute(player, pSrcItem);
		mountEquipAttribute(player, pDestItem);
	}

	//判断是否卸装后消失
	if(pSrcItem->getRes()->canBaseLimit(Res::ITEM_DROPEQUIP))
	{
		player->equipList.SetSlot(param->SrcIndex, pDestItem, true);
		player->inventoryList.SetSlot(param->DestIndex, NULL, false);
	}
	else
	{
		player->equipList.SetSlot(param->SrcIndex, pDestItem, false);
		player->inventoryList.SetSlot(param->DestIndex, pSrcItem, false);
	}
	player->equipList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	
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

	//需要更新任务物品计数
	g_MissionManager->UpdateTaskItem(player, SrcItemID);
	g_MissionManager->UpdateTaskItem(player, TagItemID);

	ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_SHORTCUT_EXCHANGE);
	evt->SetInt32ArgValues(1, SHORTCUTTYPE_EQUIP);
	player->getControllingClient()->postNetEvent(evt);

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 添加物品到装备栏
enWarnMessage ItemManager::addItemToEquip(Player* player, U32 ItemID)
{
	ItemShortcut* pItem = ItemShortcut::CreateItem(ItemID, 1);
	if(pItem)
	{
		enWarnMessage msg = addItemToEquip(player, pItem);
		delete pItem;
		return msg;
	}
	return MSG_UNKOWNERROR;
}

// ----------------------------------------------------------------------------
// 添加物品到装备栏
// 注意3点:
// 1.此方法只允许装备物品到装备栏对应的空槽位上
// 2.此方法会自动设置“强化等级属性”的随机
// 2.此方法会自动设置“装备鉴定”和“装备绑定”
enWarnMessage ItemManager::addItemToEquip(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return MSG_INVALIDPARAM;
	// 若当前物品是唯一性物品
	if(pItem->getRes()->isOnlyOne())
	{
		if(hasItem(player, pItem->getRes()->getItemID()))
			return MSG_ITEM_CANNOTMULTI;
	}

	// 获取装备位置,并判断该位置是否已经存在物品
	S32 equipPos = getItemEquipPos(player, pItem, true);
	// 判断物品是否能放到装备栏
	enWarnMessage msg = canDragToEquipment(player, pItem, equipPos);
	if(msg != MSG_NONE)
		return msg;
	ItemShortcut* pDestItem = (ItemShortcut*)player->equipList.GetSlot(equipPos);
	if(pDestItem)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	ItemShortcut* pCloneItem = ItemShortcut::CreateItem(pItem);
	//装备时强化等级附加属性随机
	if(!pCloneItem->getRes()->isRandStengthens())
	{
		pCloneItem->RandStrengthenExtData();
	}

	//拾取时或装备时鉴定
	if((pCloneItem->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_EQUIP || pCloneItem->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_DROP) && (!pCloneItem->getRes()->IsActivatePro(EAPF_ATTACH)) )
	{
		gIdentifyManager->setEquipIdntifyResult(player, pCloneItem);
	}

	//拾取时或装备时绑定
	if((pCloneItem->getRes()->getBindMode() == Res::BINDMODE_EQUIP || pCloneItem->getRes()->getBindMode() == Res::BINDMODE_PICKUP) && !pCloneItem->getRes()->IsActivatePro(EAPF_BIND))
	{
		pCloneItem->getRes()->setBindPlayerID(player->getPlayerID());
		pCloneItem->getRes()->setActivatePro(EAPF_BIND);
	}

	//判断是否背包物品
	if(pCloneItem->getRes()->isBagItem())
	{
		enWarnMessage msg = setChangeSize(player, equipPos, pCloneItem);
		if(msg != MSG_NONE)
			return msg;
	}
	else
	{
		mountEquipAttribute(player, pCloneItem);
		// 若纯粹装备，则更换时装处理
		mountEquipModel(player, pCloneItem);
	}

	player->equipList.SetSlot(equipPos, pCloneItem);
	player->equipList.UpdateToClient(player->getControllingClient(), equipPos, ITEM_NOSHOW);

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 从装备栏删除物品
enWarnMessage ItemManager::delItemFromEquip(Player* player, U32 ItemID)
{
	ItemShortcut* pItem = ItemShortcut::CreateItem(ItemID, 1);
	if(pItem)
	{
		enWarnMessage msg = delItemFromEquip(player, pItem);
		delete pItem;
		return msg;
	}
	return MSG_UNKOWNERROR;
}

// ----------------------------------------------------------------------------
// 从装备栏删除物品
enWarnMessage ItemManager::delItemFromEquip(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return MSG_INVALIDPARAM;
	// 获取装备位置
	S32 equipPos = getItemEquipPos(player, pItem, true);
	ItemShortcut* pDestItem = (ItemShortcut*)player->equipList.GetSlot(equipPos);
	if(!pDestItem)
		return MSG_ITEM_NOFINDITEM;
	//装备是否锁定保护
	//.....

	//判断是否背包物品
	if(pDestItem->getRes()->isBagItem())
	{
		enWarnMessage msg = setChangeSize(player, equipPos, pDestItem);
		if(msg != MSG_NONE)
			return msg;
	}
	else
	{
		// 若纯粹装备，则更换时装处理
		unmountEquipModel(player, pDestItem);
	}
	player->equipList.SetSlot(equipPos, NULL);
	player->equipList.UpdateToClient(player->getControllingClient(), equipPos, ITEM_NOSHOW);

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 穿上装备数值计算
void ItemManager::mountEquipAttribute(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return;
	//装备强化属性
	U32 nCurrStrengthens = 0;
	U32 iStatusID = 0;
	//耐久度为0不加状态
	if(pItem->getRes()->getCurrentWear() == 0)
		return;
	if (pItem->getRes()->getExtData() && (nCurrStrengthens = pItem->getRes()->getExtData()->EquipStrengthens))
	{
		//基本属性
		U32 nBasePro = pItem->getRes()->getBaseData()->getEquipStrengthenNum(nCurrStrengthens - 1);
		if(nBasePro > 0)
			player->AddBuff(Buff::Origin_Equipment, nBasePro, player);
		for(int i=0; i<3; i++)
		{
			if( pItem->getRes()->getExtData()->EquipStrengthenValue[i][0] == 0 || 
				pItem->getRes()->getExtData()->EquipStrengthenValue[i][1] == 0 )
				continue;

			if (pItem->getRes()->getExtData()->EquipStrengthenValue[i][0] > nCurrStrengthens )
				continue;

			player->AddBuff(Buff::Origin_Equipment, pItem->getRes()->getExtData()->EquipStrengthenValue[i][1], player);
		}
	}
	else
	{
		iStatusID = pItem->getRes()->getBaseAttribute();
		if(iStatusID > 0)
			player->AddBuff(Buff::Origin_Equipment, iStatusID, player);
	}

	if(pItem->getRes()->IsActivatePro(EAPF_ATTACH))
	{
		for(int i=0; i<pItem->getRes()->getIDEProNum(); ++i)
		{
			iStatusID = pItem->getRes()->getExtData()->IDEProValue[i];
			if(iStatusID > 0)
				player->AddBuff(Buff::Origin_Equipment, iStatusID, player);
		}
	}
	
	//镶嵌属性
	if (pItem->getRes()->getAllowedHoles() > 0)
	{
		for(int i = 0; i < 3; i++)
		{
			if ( !pItem->getRes()->IsSlotEmbeded(i+1) )
				continue;
			ItemShortcut* gemItem = ItemShortcut::CreateItem(pItem->getRes()->getExtData()->EmbedSlot[i], 1);
			if(gemItem)
			{
				player->AddBuff(Buff::Origin_Equipment, gemItem->getRes()->getBaseAttribute(), player);
				delete gemItem;
			}
			
		}
	}

	// 添加装备上的技能
	//...
}

// ----------------------------------------------------------------------------
// 穿上装备模型
void ItemManager::mountEquipModel(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem || !pItem->getRes())
		return;

	// 是否更换身体模型
	U32 ssId = pItem->getRes()->getShapesSetID(player->getSex());

	ShapeStack::Stack stack = ShapeStack::Stack_Equipment;
	U32 slot = pItem->getRes()->getEquipPos();
	if(slot == Res::EQUIPPLACEFLAG_FAMLIYFASHION)
		stack = ShapeStack::Stack_FamilyFashion;
	else if(slot == Res::EQUIPPLACEFLAG_FASHIONBODY || slot == Res::EQUIPPLACEFLAG_FASHIONHEAD || slot == Res::EQUIPPLACEFLAG_FASHIONBACK)
		stack = ShapeStack::Stack_Fashion;

	// 改变模型
	if(ssId > 0)
	{
		player->mShapeStack.add(stack, ssId, pItem->getRes()->getEquipEffectID());
	}

	// 如果是武器，做相应的逻辑修改
	if(pItem->getRes()->isWeapon() && !player->getShapeShifting())
	{
		player->setArmStatus((GameObjectData::ArmStatus)(GETSUB(pItem->getRes()->getSubCategory())));
		player->setEdgeBlurTexture(pItem->getRes()->getTrailEffectTexture());
	}

	if(pItem->getRes()->hasEqupScript())
		Con::executef("Item_On", Con::getIntArg(player->getId()), Con::getIntArg(pItem->getRes()->getItemID()));
}

// ----------------------------------------------------------------------------
// 卸除装备重新计算数值
void ItemManager::unmountEquipAttribute(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return;

	Res* pRes = pItem->getRes();
	if(!pRes)
		return;

	U32 nCurrStrengthens = 0;
	U32 iStatusID = 0;
	//耐久度为0不减状态
	if(pRes->getCurrentWear() == 0)
		return;

	if (pRes->getExtData() && (nCurrStrengthens = pRes->getExtData()->EquipStrengthens) )
	{
		//强化属性
		U32 nBasePro = pRes->getBaseData()->getEquipStrengthenNum(nCurrStrengthens - 1);
		if(nBasePro > 0)
			player->RemoveBuff(nBasePro);
		for(int i=0; i<3; i++)
		{
			if( pRes->getExtData()->EquipStrengthenValue[i][0] == 0 || 
				pRes->getExtData()->EquipStrengthenValue[i][1] == 0 )
				continue;

			if (pRes->getExtData()->EquipStrengthenValue[i][0] > nCurrStrengthens )
				continue;

			player->RemoveBuff(pRes->getExtData()->EquipStrengthenValue[i][1]);
		}
	}
	else
	{
		// 移除装备上状态数值
		iStatusID = pItem->getRes()->getBaseAttribute();
		if(iStatusID > 0)
			player->RemoveBuff(iStatusID);		
	}

	if(pRes->IsActivatePro(EAPF_ATTACH))
	{
		for(S32 i=0; i<pRes->getIDEProNum(); ++i)
		{
			iStatusID = pRes->getExtData()->IDEProValue[i];
			if(iStatusID > 0)
				player->RemoveBuff(iStatusID);
		}
	}

	//镶嵌属性
	if (pRes->getAllowedHoles() > 0)
	{
		for(int i = 0; i < 3; i++)
		{
			if ( !pRes->IsSlotEmbeded(i+1) )
				continue;
			ItemShortcut* gemItem = ItemShortcut::CreateItem(pRes->getExtData()->EmbedSlot[i], 1);
			if(gemItem)
			{
				player->RemoveBuff(gemItem->getRes()->getBaseAttribute());
				delete gemItem;
			}

		}
	}
	// 移除装备附加的技能
	// ....
}

// ----------------------------------------------------------------------------
// 卸除装备模型
void ItemManager::unmountEquipModel(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem || !pItem->getRes())
		return;

	ShapeStack::Stack stack = ShapeStack::Stack_Equipment;
	U32 slot = pItem->getRes()->getEquipPos();
	if(slot == Res::EQUIPPLACEFLAG_FAMLIYFASHION)
		stack = ShapeStack::Stack_FamilyFashion;
	else if(slot == Res::EQUIPPLACEFLAG_FASHIONBODY || slot == Res::EQUIPPLACEFLAG_FASHIONHEAD || slot == Res::EQUIPPLACEFLAG_FASHIONBACK)
		stack = ShapeStack::Stack_Fashion;

	// 卸除装备模型
	U32 ssId = pItem->getRes()->getShapesSetID(player->getSex());
	if(ssId > 0)
	{
		player->mShapeStack.remove(stack,ssId);
	}
	// 如果是武器，做相应的逻辑修改
	if(pItem->getRes()->isWeapon() && !player->getShapeShifting())
	{
		player->setArmStatus(GameObjectData::Arm_A);
		player->setEdgeBlurTexture(NULL);
	}

	if(pItem->getRes()->hasUnequpScript())
		Con::executef("Item_Off", Con::getIntArg(player->getId()), Con::getIntArg(pItem->getRes()->getItemID()));
}

// ----------------------------------------------------------------------------
// 计算玩家装备状态属性值或更新模型
void ItemManager::mountAllEquipAttribute(Player* player)
{
	for(S32 i = 0; i < player->equipList.GetMaxSlots(); ++i)
	{
		ShortcutObject* pShortcut = player->equipList.GetSlot(i);
		ItemShortcut* pItem = NULL;
		if(pShortcut && (pItem = dynamic_cast<ItemShortcut*>(pShortcut)))
		{
			mountEquipAttribute(player, pItem);			
		}
	}
}

// ----------------------------------------------------------------------------
// 计算玩家装备状态属性值或更新模型
void ItemManager::mountAllEquipModel(Player* player)
{
	for(S32 i = 1; i <= Res::EQUIPPLACEFLAG_FAMLIYFASHION; ++i)
	{
		ShortcutObject* pShortcut = player->equipList.GetSlot(i);
		ItemShortcut* pItem = NULL;
		if(pShortcut && (pItem = dynamic_cast<ItemShortcut*>(pShortcut)))
		{
			mountEquipModel(player, pItem);
		}
	}
}

#endif

#ifdef NTJ_CLIENT
//快速换装
ConsoleFunction(quickChange, void, 2, 2, "quickChange(%flag)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_QUICKCHANGE);
	evt->SetInt32ArgValues(1, dAtoi(argv[1]));
	conn->postNetEvent(evt);
}
//显示装备和镶嵌宝石的总品质
ConsoleFunction(showAllEquipQuality, void, 1, 1, "showAllEquipQuality()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 uAllEquipQuality = 0;
	U32 uAllGemQuality = 0;
	ItemShortcut* pItem = NULL;
	Res* pRes = NULL;
	for(int i=Res::EQUIPPLACEFLAG_HEAD; i<Res::EQUIPPLACEFLAG_TRUMP; ++i)
	{
		ShortcutObject* pShortcut = pPlayer->equipList.GetSlot(i);
		if(!pShortcut)
			continue;
		pItem = dynamic_cast<ItemShortcut*>(pShortcut);
		if(!pItem)
			continue;
		pRes = pItem->getRes();
		if(!pRes)
			continue;
		uAllEquipQuality += pRes->getItemQualityValue();
		uAllGemQuality += pRes->getGemQualityValue();
	}
	Con::executef("showAllEqupAndGemQuality", Con::getIntArg(uAllEquipQuality), Con::getIntArg(uAllGemQuality));
}
#endif//NTJ_CLIENT