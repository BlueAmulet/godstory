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
// �����ݿ���������Ʒ������
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
// ���ͳ�ʼ������Ʒ�����ݵ��ͻ���
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
// �洢�����Ʒ�������ݿ�
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
// ������ƷΨһID�����Ʒ����
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
// ��ȡ��Ʒ������
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
// ��ȡ��Ʒ������
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
// ���������Ʒ�����ݵ��ͻ���
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
// ��Ʒ����Ʒ�����Ӱ����
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
// �ж��Ƿ��ܱ����Ʒ��λ��С
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
// �����λ��С(ʹ�ô˺���֮ǰ����Ҫ��CanChangeMaxSlots���)
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

	//ע�������С��������Ϊ�հ�
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
// ������˴���
// ============================================================================
#ifdef NTJ_SERVER
// ----------------------------------------------------------------------------
// �ƶ���Ʒ�����󵽿����λ��
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

	// �ж��Ƿ�װ������Ϊװ�����ܱ��ϵ�������ϣ�
	if(pItem->getRes()->isEquip())
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	// �ж��Ƿ�����������
	if(pTargetSlot && pTargetSlot->isLocked())
		return MSG_ITEM_PANELLOCKED;

	ItemShortcut* pTargetItem = ItemShortcut::CreateItem(pItem);
	if(pTargetItem)
	{
		//���������Ʒ��ΨһID������Դ��Ʒһ��
		pTargetItem->getRes()->setUID(pItem->getRes()->getUID());
		//�ж��Ƿ��ʹ�ô������Ƶ���Ʒ
		if(pItem->getRes()->getResUsedTimes() <= 0)
		{			
			//����Ʒ���Ե��ӣ������ͳ������
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
// �ƶ���Ʒ��������Ʒ��λ��
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

	// �ж��Ƿ�ͬһ����Ʒ
	if(pDestItem && isSameItem(pSrcItem, pDestItem))
	{
		// Ŀ������Ʒ����ͬ����Ʒ����Ʒ��������			
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
		// Ŀ��Ϊ�ջ�����Ʒ����ͬ����Ʒ��ֱ�ӽ���
		player->inventoryList.SetSlot(param->DestIndex, pSrcItem, false);
		player->inventoryList.SetSlot(param->SrcIndex, pDestItem, false);
	}
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �ƶ���Ʒ������װ����λ��
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
	
	// �ж��Ƿ�Ϊװ����Ʒ,���ж�װ����Ʒ�ķ����Ƿ���ȷ
	msg = canDragToEquipment(player, pSrcItem, param->DestIndex);
	if(msg != MSG_NONE)
		return msg;
	
	//װ��ʱ����
	if((pSrcItem->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_EQUIP) && (!pSrcItem->getRes()->IsActivatePro(EAPF_ATTACH)) )
	{
		gIdentifyManager->setEquipIdntifyResult(player, pSrcItem);
	}
	//װ��ʱ��
	if(pSrcItem->getRes()->getBindMode() == Res::BINDMODE_EQUIP && !pSrcItem->getRes()->IsActivatePro(EAPF_BIND))
	{
		pSrcItem->getRes()->setBindPlayerID(player->getPlayerID());
		pSrcItem->getRes()->setActivatePro(EAPF_BIND);
	}

	// �Ǳ���,�ǰ�̯��ʽ����
	if(pSrcItem->getRes()->isBagItem() && !pSrcItem->getRes()->isStallStyleBag())
	{
		msg = g_ItemManager->setChangeSize(player, param->DestIndex, pSrcItem);
		if(msg != MSG_NONE)
			return msg;
	}
	
	//�Ǳ���,�ǰ�̯��ʽ�����Ұ�̯״̬��
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
	else//��װ��������ͨװ������ͨʱװ������ʱװ
	{
		// ��װ
		unmountEquipModel(player, pDestItem);
		mountEquipModel(player, pSrcItem);
		unmountEquipAttribute(player, pDestItem);
		mountEquipAttribute(player, pSrcItem);
	}

	U32 SrcItemID = pSrcItem->getRes()->getItemID();
	U32 TagItemID = 0;
	// ���Ŀ��װ������λ����Ʒ����ж��װ��
	if(pDestItem)
	{
		TagItemID = pDestItem->getRes()->getItemID();		
		// �ж��Ƿ�жװ����ʧ
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

	//��Ҫ����������Ʒ����
	g_MissionManager->UpdateTaskItem(player, SrcItemID);
	g_MissionManager->UpdateTaskItem(player, TagItemID);

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
	
	ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_SHORTCUT_EXCHANGE);
	evt->SetInt32ArgValues(1, SHORTCUTTYPE_INVENTORY);
	player->getControllingClient()->postNetEvent(evt);

	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �ƶ���Ʒ������������λ��
enWarnMessage ItemManager::InventoryMoveToBank(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;
	//�ж��Ƿ����н���״̬
	msg = player->isBusy(Player::INTERACTION_BANKTRADE);
	if(msg != MSG_NONE)
		return msg;

	//�ж������Ƿ��ױ���
	if(player->getBankFlag())
		return MSG_BANK_LOCK;	

	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
				!player->bankList.IsVaildSlot(param->DestIndex))
		return msg;	

	ItemShortcut* pSrcItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->SrcIndex);
	ItemShortcut* pDestItem	= (ItemShortcut*)player->bankList.GetSlot(param->DestIndex);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;
	//�ж���Ʒ�ɷ��ֿ�
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
		// Ŀ��Ϊ�ջ�����Ʒ����ͬ����Ʒ��ֱ�ӽ���
		player->bankList.SetSlot(param->DestIndex, pSrcItem, false);
		player->inventoryList.SetSlot(param->SrcIndex, pDestItem, false);
	}
	player->bankList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);

	//��Ҫ����������Ʒ����
	g_MissionManager->UpdateTaskItem(player, SrcItemID);
	g_MissionManager->UpdateTaskItem(player, TagItemID);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ������ƷԤ������Ϊ�����ӻ�ɾ����Ʒ����Ҫ����BatchItemʹ��)
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
// Ԥ����������Ʒ����
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
// ����Ƿ��ܽ���������Ʒ����
enWarnMessage ItemManager::detectPutItem(Player* player, Vector<stItemResult*> itemList, Vector<stItemResult*>& resultList, bool ignoreLock/* = false*/)
{
	resultList.clear();
	if(itemList.empty())
		return MSG_NONE;
	if(!player)
		return MSG_INVALIDPARAM;

	enWarnMessage msgCode = MSG_NONE;

	//=======���Ƚ���Ʒ����Ʒӳ�䵽һ����ʱ�б���׼������======================
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

	//=======���Ȳ���������Ҫɾ������Ʒ========================================
	for(S32 i = 0; i < itemList.size(); ++i)
	{
		stItemResult* pTempSrc = itemList[i];
		ItemShortcut* pTempItem = NULL;
		if(!pTempSrc || !(pTempItem = pTempSrc->srcEntry))
			continue;

		//�ж��Ƿ�ɾ����Ʒ����
		if(pTempSrc->num >= 0)
			continue;

		S32 iMaxOverNum  = pTempItem->getRes()->getMaxOverNum();
		S32 iSrcQuantity = 0 - pTempSrc->num;

		// ע:����ʹ��map��Ϊ���Ż��ۼ���Ʒ��˳�����ȿ�����С�ģ�
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

		// ����Ʒ������Ȼ����0, ��˵����Ʒ���޷��ҵ�ָ��������ɾ������Ʒ
		if(iSrcQuantity > 0)
		{
			msgCode = MSG_ITEM_NOENOUGHNUM;
			goto ClearAll;
		}
	}

	//========�����ٴ�����Ҫ��ӵ���Ʒ=========================================
	for(S32 i = 0; i < itemList.size(); ++i)
	{
		stItemResult* pTempSrc = itemList[i];
		ItemShortcut* pTempItem = NULL;
		if(!pTempSrc || !(pTempItem = pTempSrc->srcEntry))
			continue;

		//�ж��Ƿ������Ʒ����
		if(pTempSrc->num <= 0)
			continue;

		// �ж��Ƿ�Ψһ����Ʒ�����������������1����˵�������������
		if(pTempItem->getRes()->isOnlyOne() && pTempSrc->num > 1)
		{
			msgCode = MSG_ITEM_CANNOTMULTI;
			goto ClearAll;
		}

		//��Ϊװ����Ʒ��δ���ɹ�ǿ���������ԣ�����������ǿ����������
		if(pTempItem->getRes()->isEquip() && !pTempItem->getRes()->isRandStengthens())
			pTempItem->RandStrengthenExtData();

		S32 iMaxOverNum  = pTempItem->getRes()->getMaxOverNum();
		S32 iSrcQuantity = pTempSrc->num;

		bool bFind = false;		// ����Ʒ���Ƿ��������һ��ͬ������Ʒ
		// �ж��Ƿ���Ҫ����
		if(iMaxOverNum > 1)
		{
			// ���ȵ����ܵ��ӵ���Ʒ
			for(S32 j = 0; j < resultList.size(); ++j)
			{
				stItemResult* pResult = resultList[j];
				if(pResult->srcEntry && pResult->num != 0 &&
					(ignoreLock || (!ignoreLock && pResult->srcEntry->getSlotState() != ShortcutObject::SLOT_LOCK)) &&
					isSameItem(pResult->srcEntry, pTempItem))
				{
					// ������Ʒ��Ψһ�ԣ�ȴ����Ʒ���ҵ�ͬ����һ����Ʒ
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
			// ���ڲ��ɵ��ӵ�Ψһ����Ʒ����Ȼ��Ҫ�����Ƿ��Ѿ�����Ʒ����
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

		//���Ѿ��ҵ�һ��Ψһ����Ʒ���ڣ���ʧ���˳�
		if(bFind)
		{
			msgCode = MSG_ITEM_CANNOTMULTI;
			goto ClearAll;
		}

		// ����Ѱ�ҿղ۷���ʣ�����Ʒ����
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

		// ����Ʒ������Ȼ����0, ��˵����Ʒ���޷�����ָ����������Ʒ
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
// �ύ����������Ʒ���
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
		// �ж��Ƿ���ɾ�����ղ۵Ĳ���
		if(resultList[i]->flag & OP_FINDDELEMPTY)
		{
			if(resultList[i]->srcEntry)
			{
				player->inventoryList.SetSlot(i, NULL);
			}
			resultList[i]->srcEntry = NULL;
		}
		// �ж��Ƿ���ɾ���������Ĳ���
		if(resultList[i]->flag & OP_FINDDEL)
		{
			if(resultList[i]->srcEntry)
			{
				resultList[i]->srcEntry->getRes()->setQuantity(resultList[i]->num);
			}
		}
		// �ж��Ƿ���������Ʒ�Ĳ���
		if(resultList[i]->flag & OP_FINDEMPTY)
		{
			if(resultList[i]->newEntry)
			{
				U32 newItemID = resultList[i]->newEntry->getRes()->getItemID();
				// ͬ�������
				g_ItemManager->syncPanel(player, newItemID);
				resultList[i]->newEntry->getRes()->setQuantity(resultList[i]->num);
				player->inventoryList.SetSlot(i, resultList[i]->newEntry, false);
				g_MissionManager->UpdateTaskItem(player, newItemID);
			}				
		}
		// �ж��Ƿ��������Ʒ�����Ĳ���
		if(resultList[i]->flag & OP_FINDADD)
		{
			if(resultList[i]->srcEntry)
			{
				resultList[i]->srcEntry->getRes()->setQuantity(resultList[i]->num);
			}
		}

		if(itemID != 0)
		{
			// ͬ�������
			g_ItemManager->syncPanel(player, itemID);
			g_MissionManager->UpdateTaskItem(player, itemID);
		}
		player->inventoryList.UpdateToClient(player->getControllingClient(), i, ITEM_GET);
		delete resultList[i];
	}
	resultList.clear();
}

// ----------------------------------------------------------------------------
// Ϊ�������������Ʒ(��Ҫ����putItemʹ��)
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

	// �����ʱ��Ʒ
	clearTempItemList();
	return msgCode;
}

// ----------------------------------------------------------------------------
// ����Ʒ����ɾ��ָ��λ�õ���Ʒ
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
	
	//���ڿۼ������������������������ͬ��Ʒ���пۼ�
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
	// ͬ�������
	g_ItemManager->syncPanel(player, itemid);
	//��Ҫ��������Ʒ�ж�
	g_MissionManager->UpdateTaskItem(player, itemid);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ����Ʒ����ɾ��������Ʒ(����ƷIDʵ��)
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
// ����Ʒ����ɾ��������Ʒ(����Ʒ����ʵ��)
enWarnMessage ItemManager::delItemFromInventory(Player* player, ItemShortcut* pItem, S32 itemNum /* = 1 */, U32 flag /* = ITEM_LOSE */, bool ignoreExpro /*= false*/)
{
	if(!player || !pItem || itemNum <= 0)
		return MSG_ITEM_ITEMDATAERROR;

	Vector<S32> slotlist;
	S32 iCount = player->inventoryList.QuerySameObject(pItem, slotlist, true, ignoreExpro);
	U32 itemid = pItem->getRes()->getItemID();
	S32 num = itemNum;
	// �ж���Ʒ�Ƿ����ص�
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

	// ͬ�������
	g_ItemManager->syncPanel(player, itemid);
	//��Ҫ��������Ʒ�ж�
	g_MissionManager->UpdateTaskItem(player, itemid);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ��ӵ�����Ʒ����Ʒ��������ƷIDʵ�֣�
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
// ��ӵ�����Ʒ����Ʒ��������Ʒ����ʵ�֣�
enWarnMessage ItemManager::addItemToInventory(Player* player, ItemShortcut* pItem, S32& index, S32 num, U32 flag)
{
	index = -1;
	if(!player || !pItem || num <= 0)
		return MSG_ITEM_ITEMDATAERROR;

	// ������ӵ���Ʒ��Ψһ����Ʒ������������1����ֱ�ӷ���
	if(pItem->getRes()->isOnlyOne() && num > 1)
		return MSG_ITEM_CANNOTMULTI;

	ItemShortcut* pCloneItem = ItemShortcut::CreateItem(pItem);
	if(!pCloneItem)
		return MSG_ITEM_ITEMDATAERROR;
	else
	{
		//����װ��֮ǰδ�������ǿ���ȼ��������ԣ������������һ��
		if(!pCloneItem->getRes()->isRandStengthens())
			pCloneItem->RandStrengthenExtData();
		Res* pRes = pCloneItem->getRes();
		if(!pRes)
			return MSG_ITEM_ITEMDATAERROR;
		//ʰȡʱ��
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
	bool bFind = false;		// ����Ʒ���Ƿ��������һ��ͬ������Ʒ
	// �жϴ���ӵ���Ʒ�Ƿ�ɵ���
	if(iMaxOverNum > 1)
	{
		// ���ȵ����ܵ��ӵ���Ʒ
		for(S32 i = 0; i < resultList.size(); ++i)
		{
			stItemResult* pResult = resultList[i];
			if(pResult->srcEntry && (pResult->srcEntry->getSlotState() != ShortcutObject::SLOT_LOCK) &&
				isSameItem(pResult->srcEntry, pCloneItem))
			{
				// ������Ʒ��Ψһ�ԣ�ȴ����Ʒ���ҵ�ͬ����һ����Ʒ
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
		// ���ڲ��ɵ��ӵ�Ψһ����Ʒ����Ȼ��Ҫ�����Ƿ��Ѿ�����Ʒ����
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

	//���ҵ�Ψһ����Ʒ����Ʒ���Ѿ�����
	if(bFind)
	{
		msgCode = MSG_ITEM_CANNOTMULTI;
		goto ClearAll;
	}

	// ����Ѱ�ҿղ۷���ʣ�����Ʒ����
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

	// ����Ʒ������Ȼ����0, ��˵����Ʒ���޷�����ָ����������Ʒ
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
			// �ж��Ƿ���������Ʒ�Ĳ���
			if(resultList[i]->flag == OP_FINDEMPTY)
			{
				itemID = resultList[i]->srcEntry->getRes()->getItemID();
				resultList[i]->srcEntry->getRes()->setQuantity(resultList[i]->num);
				player->inventoryList.SetSlot(i, resultList[i]->srcEntry);
				RegisterTimeItem(player, resultList[i]->srcEntry);
			}
			// �ж��Ƿ��������Ʒ�����Ĳ���
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
	//ʰȡʱ��ʼ��ʱ
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


//����Ʒ���ƶ���������
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
// ��Ʒ���(����״̬)
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
// ��Ʒ���(������������)
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
		// �����Ʒ��������
		//g_UIMouseGamePlay->AddScreenMessage(szWarnMessage[MSG_ITEM_SPLITNOENOUGH]);
		g_ClientGameplayState->cancelCurrentAction();
		return;
	}

	ClientGameplayParam* param = new ClientGameplayParam;
	param->setIntArgValues(1, uNum);
	g_ClientGameplayState->setCurrentActionParam(param);
}

// ----------------------------------------------------------------------------
// ������Ʒ
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
// ������Ʒ(����״̬)
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
// ������Ʒ(ȷ����������)
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
// ��̯
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
// ȡ��ǰһ�β���
ConsoleFunction(cancelAction, void, 1, 1, "cancelAction")
{
	g_ClientGameplayState->cancelCurrentAction();
}

// ----------------------------------------------------------------------------
// �õ�������С
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
// ������Ʒɸѡ
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
//ȡ���϶�����
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
//��������
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
//����;ûָ�
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

	// �ж�����Ѿ�����һ���¼�״̬�Ͳ��ܴ�����
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