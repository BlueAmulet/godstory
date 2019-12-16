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
// �����ݿ�������װ��������
bool EquipList::LoadData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	for(U32 i = 0; i < mMaxSlots; ++i)
	{
		stItemInfo& stTemp = playerInfo->DispData.EquipInfo[i];
		if(stTemp.ItemID == 0)
			continue;
		//�����½�ɫ��ʼ��װ����Ϣ
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
// ���ͳ�ʼ����װ�������ݵ��ͻ���
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
// ��ȡװ����װ����չ������������С
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
// ��ȡ��չ������λ�ô�С
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
// ��ȡװ���;���Ҫ����ļ۸�
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
// �洢���װ���������ݿ�
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
			// ������������ʧ����Ʒ
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
// �������װ�������ݵ��ͻ���
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
// �ж��Ƿ����϶���Ʒ��װ����
enWarnMessage ItemManager::canDragToEquipment(Player* player, ItemShortcut* pItem, S32 SlotIndex /*=-1*/)
{
	if(!player || !pItem)
		return MSG_INVALIDPARAM;

	//�Ƿ�Ϊװ����Ʒ
	if(!pItem->getRes()->isEquip())
		return MSG_ITEM_CANNOTMOUNT;
	//�ȼ��ж�
	if(!pItem->getRes()->canLevelLimit(player->getLevel()))
		return MSG_PLAYER_LEVEL;
	//ְҵ�ж�
	if(!pItem->getRes()->canFamilyLimit(player->getFamily()))
		return MSG_PLAYER_FAMILY;
	//�Ա��ж�
	if(!pItem->getRes()->canSexLimit(player->getSex()))
		return MSG_PLAYER_SEX;
	//ʹ�ô����ж�
	if(pItem->getRes()->getResUsedTimes() != 0 && pItem->getRes()->getUsedTimes() == 0)
		return MSG_ITEM_CANNOTUSE;
#ifdef NTJ_SERVER
	//ʱЧװ���ж�
	if(pItem->getRes()->getTimeMode() != Res::TIMEMODE_NONE)
	{
		U32 dis = Platform::getTime() - player->mBeginPlayerTime;
		if(pItem->getRes()->getLapseTime() - dis <= 0)
			return MSG_ITEM_CANNOTUSE;
	}
#endif
	// ��ָ��������λ��
	if(SlotIndex != -1 && !checkDragToEquipment(player, pItem, SlotIndex))
		return MSG_ITEM_MOUNTPOSITIONERROR;	
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ����϶���Ʒ��װ������λ���Ƿ���ȷ
bool ItemManager::checkDragToEquipment(Player* player, ItemShortcut* pItem, S32 index)
{
	if(!player || !pItem)
		return false;
	//�жϱ�����Ʒ
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
// ��ȡ��Ʒ��װ�����Ķ�Ӧλ��
S32 ItemManager::getItemEquipPos(Player* player, ItemShortcut* pItem, bool ignoreExist/*=false*/)
{
	if(!player || !pItem)
		return -1;

	if(pItem->getRes()->isInventoryBag())		//����������չ����
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
	else if(pItem->getRes()->isBankBag())		//����������չ����
	{
		ItemShortcut* BagItem1 = (ItemShortcut*)player->equipList.GetSlot(Res::EQUIPPLACEFLAG_BANKPACK);
		if(!BagItem1 || ignoreExist)
			return Res::EQUIPPLACEFLAG_BANKPACK;
	}
	else if(pItem->getRes()->isStallBag())		//��̯������չ����
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
	else if(pItem->getRes()->isStallStyleBag())	//��̯������ʽ����
	{
		ItemShortcut* BagItem1 = (ItemShortcut*)player->equipList.GetSlot(Res::EQUIPPLACEFLAG_STALLPACK3);
		if(!BagItem1 || ignoreExist)
			return Res::EQUIPPLACEFLAG_STALLPACK3;
	}
	else if(pItem->getRes()->isEquip())			//װ����Ʒ
	{
		return pItem->getRes()->getEquipPos();
	}
	return -1;
}

#ifdef NTJ_SERVER

// ----------------------------------------------------------------------------
// װ��ȫ��
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
// ������Ʒ
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
// ������λ��С
// index  Ϊָ������չ��λ����
enWarnMessage ItemManager::setChangeSize(Player* player, U32 index, ItemShortcut* pItem)
{
	if(!player)
		return MSG_INVALIDPARAM;

	enWarnMessage msg = MSG_NONE;
	// ������С�洢�ڱ����ֶ���
	U32 changeNum = 0;
	if(pItem)
	 changeNum = pItem->getRes()->getReserveValue();
	switch (index)
	{
	case Res::EQUIPPLACEFLAG_PACK1:		//��������չ��λ1
		{
			U32 iSrc1Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK1);
			U32 iSrc2Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK2);
			msg = player->inventoryList.CanChangeMaxSlots(iSrc1Num, iSrc2Num, changeNum, iSrc2Num);
			if(msg != MSG_NONE)
				return msg;
			player->inventoryList.ChangeMaxSlots(player, iSrc1Num, iSrc2Num, changeNum, iSrc2Num);
		}
		break;
	case Res::EQUIPPLACEFLAG_PACK2:		//��������չ��λ2
		{
			U32 iSrc1Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK1);
			U32 iSrc2Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK2);
			msg = player->inventoryList.CanChangeMaxSlots(iSrc1Num, iSrc2Num, iSrc1Num, changeNum);
			if(msg != MSG_NONE)
				return msg;
			player->inventoryList.ChangeMaxSlots(player, iSrc1Num, iSrc2Num, iSrc1Num, changeNum);
		}
		break;
	case Res::EQUIPPLACEFLAG_BANKPACK:	//���вֿ�����չ��λ
		{
			U32 iSrcNum = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_BANKPACK);
			msg = player->bankList.CanChangeMaxSlots(iSrcNum, changeNum);
			if(msg != MSG_NONE)
				return msg;
			player->bankList.ChangeMaxSlots(player, iSrcNum, changeNum);
		}
		break;
	case Res::EQUIPPLACEFLAG_STALLPACK1://��̯����չ��λ1
		{
			U32 iSrc1Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK1);
			U32 iSrc2Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK2);
			msg = player->individualStallList.CanChangeMaxSlots(iSrc1Num, iSrc2Num, changeNum, iSrc2Num);
			if(msg != MSG_NONE)
				return msg;
			player->individualStallList.ChangeMaxSlots(player, iSrc1Num, iSrc2Num, changeNum, iSrc2Num);
		}
		break;
	case Res::EQUIPPLACEFLAG_STALLPACK2://��̯����չ��λ2
		{
			U32 iSrc1Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK1);
			U32 iSrc2Num = player->equipList.GetBagSize(Res::EQUIPPLACEFLAG_STALLPACK2);
			msg = player->individualStallList.CanChangeMaxSlots(iSrc1Num, iSrc2Num, iSrc1Num, changeNum);
			if(msg != MSG_NONE)
				return msg;
			player->individualStallList.ChangeMaxSlots(player, iSrc1Num, iSrc2Num, iSrc1Num, changeNum);
		}
		break;
	//case Res::CATEGORY_STAL_STYLE_PACK://��̯����ʽ��λ
	//	{

	//	}
	//	break;
	}
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �ƶ�װ������Ʒ����Ʒ��
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
		// �ж�Ŀ����Ʒ�Ƿ����ϵ�װ����
		if(!checkDragToEquipment(player, pDestItem, param->SrcIndex))
			return MSG_ITEM_CANNOTDRAGTOOBJECT;
		TagItemID = pDestItem->getRes()->getItemID();
		//װ��ʱ����
		if((pDestItem->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_EQUIP) && (!pDestItem->getRes()->IsActivatePro(EAPF_ATTACH)) )
		{
			gIdentifyManager->setEquipIdntifyResult(player, pDestItem);
		}
		//װ��ʱ��
		if(pDestItem->getRes()->getBindMode() == Res::BINDMODE_EQUIP && !pDestItem->getRes()->IsActivatePro(EAPF_BIND))
		{
			pDestItem->getRes()->setBindPlayerID(player->getPlayerID());
			pDestItem->getRes()->setActivatePro(EAPF_BIND);
		}
	}
	else
	{
		// �ж��Ƿ��пղۿɷ�Ŀ����Ʒ
		if(player->inventoryList.FindEmptySlot() == -1)
			return MSG_ITEM_ITEMNOSPACE;		
	}

	// �Ǳ���,�ǰ�̯��ʽ����
	if(pSrcItem->getRes()->isBagItem() && !pSrcItem->getRes()->isStallStyleBag())
	{
		// ���ܽ������ŵ�������չ��λ��
		if(param->DestIndex >= EXTBAG_BEGINSLOT)
			return MSG_ITEM_CANNOTDRAGTOOBJECT;
		enWarnMessage msg = setChangeSize(player, param->SrcIndex, NULL);
		if(msg != MSG_NONE)
			return msg;
	}//�Ǳ���,�ǰ�̯��ʽ����
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
	else//��װ��������ͨװ������ͨʱװ������ʱװ
	{
		unmountEquipModel(player, pSrcItem);
		mountEquipModel(player, pDestItem);
		unmountEquipAttribute(player, pSrcItem);
		mountEquipAttribute(player, pDestItem);
	}

	//�ж��Ƿ�жװ����ʧ
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
	
	//�ж��Ƿ���Ʒ��������Ʒ
	if(pSrcItem->getRes()->isInventoryBag())
	{
		player->inventoryList.UpdateMaxSlots(player->getControllingClient());
	}//�����б���
	else if(pSrcItem->getRes()->isBankBag())
	{
		player->bankList.UpdateMaxSlots(player->getControllingClient());
	}//��̯������
	else if(pSrcItem->getRes()->isStallBag())
	{
		player->individualStallList.UpdateMaxSlots(player->getControllingClient());
	}

	//��Ҫ����������Ʒ����
	g_MissionManager->UpdateTaskItem(player, SrcItemID);
	g_MissionManager->UpdateTaskItem(player, TagItemID);

	ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_SHORTCUT_EXCHANGE);
	evt->SetInt32ArgValues(1, SHORTCUTTYPE_EQUIP);
	player->getControllingClient()->postNetEvent(evt);

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �����Ʒ��װ����
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
// �����Ʒ��װ����
// ע��3��:
// 1.�˷���ֻ����װ����Ʒ��װ������Ӧ�Ŀղ�λ��
// 2.�˷������Զ����á�ǿ���ȼ����ԡ������
// 2.�˷������Զ����á�װ���������͡�װ���󶨡�
enWarnMessage ItemManager::addItemToEquip(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return MSG_INVALIDPARAM;
	// ����ǰ��Ʒ��Ψһ����Ʒ
	if(pItem->getRes()->isOnlyOne())
	{
		if(hasItem(player, pItem->getRes()->getItemID()))
			return MSG_ITEM_CANNOTMULTI;
	}

	// ��ȡװ��λ��,���жϸ�λ���Ƿ��Ѿ�������Ʒ
	S32 equipPos = getItemEquipPos(player, pItem, true);
	// �ж���Ʒ�Ƿ��ܷŵ�װ����
	enWarnMessage msg = canDragToEquipment(player, pItem, equipPos);
	if(msg != MSG_NONE)
		return msg;
	ItemShortcut* pDestItem = (ItemShortcut*)player->equipList.GetSlot(equipPos);
	if(pDestItem)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	ItemShortcut* pCloneItem = ItemShortcut::CreateItem(pItem);
	//װ��ʱǿ���ȼ������������
	if(!pCloneItem->getRes()->isRandStengthens())
	{
		pCloneItem->RandStrengthenExtData();
	}

	//ʰȡʱ��װ��ʱ����
	if((pCloneItem->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_EQUIP || pCloneItem->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_DROP) && (!pCloneItem->getRes()->IsActivatePro(EAPF_ATTACH)) )
	{
		gIdentifyManager->setEquipIdntifyResult(player, pCloneItem);
	}

	//ʰȡʱ��װ��ʱ��
	if((pCloneItem->getRes()->getBindMode() == Res::BINDMODE_EQUIP || pCloneItem->getRes()->getBindMode() == Res::BINDMODE_PICKUP) && !pCloneItem->getRes()->IsActivatePro(EAPF_BIND))
	{
		pCloneItem->getRes()->setBindPlayerID(player->getPlayerID());
		pCloneItem->getRes()->setActivatePro(EAPF_BIND);
	}

	//�ж��Ƿ񱳰���Ʒ
	if(pCloneItem->getRes()->isBagItem())
	{
		enWarnMessage msg = setChangeSize(player, equipPos, pCloneItem);
		if(msg != MSG_NONE)
			return msg;
	}
	else
	{
		mountEquipAttribute(player, pCloneItem);
		// ������װ���������ʱװ����
		mountEquipModel(player, pCloneItem);
	}

	player->equipList.SetSlot(equipPos, pCloneItem);
	player->equipList.UpdateToClient(player->getControllingClient(), equipPos, ITEM_NOSHOW);

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ��װ����ɾ����Ʒ
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
// ��װ����ɾ����Ʒ
enWarnMessage ItemManager::delItemFromEquip(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return MSG_INVALIDPARAM;
	// ��ȡװ��λ��
	S32 equipPos = getItemEquipPos(player, pItem, true);
	ItemShortcut* pDestItem = (ItemShortcut*)player->equipList.GetSlot(equipPos);
	if(!pDestItem)
		return MSG_ITEM_NOFINDITEM;
	//װ���Ƿ���������
	//.....

	//�ж��Ƿ񱳰���Ʒ
	if(pDestItem->getRes()->isBagItem())
	{
		enWarnMessage msg = setChangeSize(player, equipPos, pDestItem);
		if(msg != MSG_NONE)
			return msg;
	}
	else
	{
		// ������װ���������ʱװ����
		unmountEquipModel(player, pDestItem);
	}
	player->equipList.SetSlot(equipPos, NULL);
	player->equipList.UpdateToClient(player->getControllingClient(), equipPos, ITEM_NOSHOW);

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ����װ����ֵ����
void ItemManager::mountEquipAttribute(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return;
	//װ��ǿ������
	U32 nCurrStrengthens = 0;
	U32 iStatusID = 0;
	//�;ö�Ϊ0����״̬
	if(pItem->getRes()->getCurrentWear() == 0)
		return;
	if (pItem->getRes()->getExtData() && (nCurrStrengthens = pItem->getRes()->getExtData()->EquipStrengthens))
	{
		//��������
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
	
	//��Ƕ����
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

	// ���װ���ϵļ���
	//...
}

// ----------------------------------------------------------------------------
// ����װ��ģ��
void ItemManager::mountEquipModel(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem || !pItem->getRes())
		return;

	// �Ƿ��������ģ��
	U32 ssId = pItem->getRes()->getShapesSetID(player->getSex());

	ShapeStack::Stack stack = ShapeStack::Stack_Equipment;
	U32 slot = pItem->getRes()->getEquipPos();
	if(slot == Res::EQUIPPLACEFLAG_FAMLIYFASHION)
		stack = ShapeStack::Stack_FamilyFashion;
	else if(slot == Res::EQUIPPLACEFLAG_FASHIONBODY || slot == Res::EQUIPPLACEFLAG_FASHIONHEAD || slot == Res::EQUIPPLACEFLAG_FASHIONBACK)
		stack = ShapeStack::Stack_Fashion;

	// �ı�ģ��
	if(ssId > 0)
	{
		player->mShapeStack.add(stack, ssId, pItem->getRes()->getEquipEffectID());
	}

	// ���������������Ӧ���߼��޸�
	if(pItem->getRes()->isWeapon() && !player->getShapeShifting())
	{
		player->setArmStatus((GameObjectData::ArmStatus)(GETSUB(pItem->getRes()->getSubCategory())));
		player->setEdgeBlurTexture(pItem->getRes()->getTrailEffectTexture());
	}

	if(pItem->getRes()->hasEqupScript())
		Con::executef("Item_On", Con::getIntArg(player->getId()), Con::getIntArg(pItem->getRes()->getItemID()));
}

// ----------------------------------------------------------------------------
// ж��װ�����¼�����ֵ
void ItemManager::unmountEquipAttribute(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return;

	Res* pRes = pItem->getRes();
	if(!pRes)
		return;

	U32 nCurrStrengthens = 0;
	U32 iStatusID = 0;
	//�;ö�Ϊ0����״̬
	if(pRes->getCurrentWear() == 0)
		return;

	if (pRes->getExtData() && (nCurrStrengthens = pRes->getExtData()->EquipStrengthens) )
	{
		//ǿ������
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
		// �Ƴ�װ����״̬��ֵ
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

	//��Ƕ����
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
	// �Ƴ�װ�����ӵļ���
	// ....
}

// ----------------------------------------------------------------------------
// ж��װ��ģ��
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

	// ж��װ��ģ��
	U32 ssId = pItem->getRes()->getShapesSetID(player->getSex());
	if(ssId > 0)
	{
		player->mShapeStack.remove(stack,ssId);
	}
	// ���������������Ӧ���߼��޸�
	if(pItem->getRes()->isWeapon() && !player->getShapeShifting())
	{
		player->setArmStatus(GameObjectData::Arm_A);
		player->setEdgeBlurTexture(NULL);
	}

	if(pItem->getRes()->hasUnequpScript())
		Con::executef("Item_Off", Con::getIntArg(player->getId()), Con::getIntArg(pItem->getRes()->getItemID()));
}

// ----------------------------------------------------------------------------
// �������װ��״̬����ֵ�����ģ��
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
// �������װ��״̬����ֵ�����ģ��
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
//���ٻ�װ
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
//��ʾװ������Ƕ��ʯ����Ʒ��
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