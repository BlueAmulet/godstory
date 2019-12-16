//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/item/Res.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Player_GemMount.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/item/NpcShopData.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/Spell.h"
#ifdef NTJ_CLIENT
#include "UI/dGuiShortCut.h"
#endif

ItemManager* g_ItemManager;
ItemManager gItemManager;

// ============================================================================
// ��λ���෽��
//     ������λ��ͨ�÷�����ʵ��
// ============================================================================
BaseItemList::BaseItemList(S32 maxSlots)
{
	mMaxSlots = maxSlots;
	mType = SHORTCUTTYPE_NONE;
	mSlots = new ShortcutObject*[mMaxSlots];
	for(S32 i = 0; i < mMaxSlots; ++i)
		mSlots[i] = NULL;
}

BaseItemList::~BaseItemList()
{
	Clear();
}

// ----------------------------------------------------------------------------
// ��������λ�û�ȡ��λ����
ShortcutObject* BaseItemList::GetSlot(S32 index)
{
	if(index >= 0 && index < mMaxSlots)
		return mSlots[index];
	return NULL;
}

// ----------------------------------------------------------------------------
// ���ҵ�һ���ղ�λ������λ��
S32 BaseItemList::FindEmptySlot()
{
	for(S32 i = 0; i < mMaxSlots; ++i)
	{
		if(!mSlots[i])
			return i;
	}
	return -1;
}

//������ͬUID����Ʒ
S32 BaseItemList::findSlot(U64 uId)
{
	for(S32 i = 0; i < mMaxSlots; i++)
	{
		ItemShortcut* pItem = (ItemShortcut*)mSlots[i];
		if(pItem && pItem->getRes()->getUID() == uId)
			return i;
	}
	return -1;
}
// ----------------------------------------------------------------------------
// ��ѯ�ղ�λ������,���Ǽǿղ�λ����λ��
// slotlist Ϊ�Ǽǿղ�λ����λ�õ��б�
// ����ֵΪ�ղ�λ����
S32 BaseItemList::QueryEmptySlot(Vector<S32>& slotlist)
{
	slotlist.clear();
	for(S32 i = 0; i< mMaxSlots; ++i)
	{
		if(!mSlots[i])
			slotlist.push_back(i);
	}
	return slotlist.size();
}

// ----------------------------------------------------------------------------
// �����Ʒ����Ŀղ�λ����
S32 BaseItemList::GetEmptyCount()
{
	S32 iCount = 0;
	for(S32 i = 0; i< mMaxSlots; ++i)
	{
		if(!mSlots[i])
			iCount++;
	}
	return iCount;
}
// ----------------------------------------------------------------------------
// ��ȡ��λ�������
S32 BaseItemList::GetMaxSlots()
{
	return mMaxSlots;
}

// ----------------------------------------------------------------------------
// ������Ʒ����Ʒ��ָ��λ��
// ��pSlotΪNULL, ���ʾɾ��
// isClear	�Ƿ���Ҫɾ��ԭ��λ����
bool BaseItemList::SetSlot(S32 index, ShortcutObject* pSlot, bool isClear)
{
	if(index < 0 || index >= mMaxSlots)
		return false;

	if(mSlots[index] && isClear)
		delete mSlots[index];
	mSlots[index] = pSlot;
	return true;
}

// ----------------------------------------------------------------------------
// ��������Ʒ��
void BaseItemList::Clear()
{
	Reset();

    delete[] mSlots;
}

// ----------------------------------------------------------------------------
// �����Ʒ���ܶ��󣬵������slots
void BaseItemList::Reset()
{
	for( S32 i = 0; i < mMaxSlots; i++)
	{
		if(mSlots[i])
		{
			delete mSlots[i];
			mSlots[i] = NULL;
		}
	}
}

// ----------------------------------------------------------------------------
// ��������ID������λ����ҵ�һ��ͬ����������λ��
// id				��λ�����ţ���Ʒ���ܱ�ţ�
// checkSlotState	�Ƿ�����λ������״̬
// ����ֵΪ��һ��ͬ����Ʒ���ܵ�����λ��
S32 BaseItemList::FindSameObjectByID(S32 id, bool checkSlotState)
{
	for(S32 i = 0; i < mMaxSlots; ++i)
	{
		if(!mSlots[i])
			continue;
		if (checkSlotState && mSlots[i]->getSlotState() == ShortcutObject::SLOT_LOCK)
			continue;
		if(mSlots[i]->isItemObject())
		{
			ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(mSlots[i]);
			if(pItem && pItem->getRes()->getItemID() == id)
				return i;
		}
		else if(mSlots[i]->isSkillObject())
		{
			SkillShortcut* pSkillShort = (SkillShortcut*)(mSlots[i]);
			if(pSkillShort->getSkill()->GetBaseSkillId() == id)
				return i;
		}
	}
	return -1;
}

// ----------------------------------------------------------------------------
// ��������ID������λ����������ͬ�����
// id				��λ�����ţ���Ʒ���ܱ�ţ�
// slotlist			�Ǽ�ͬ���������λ�õ��б�
// checkSlotState	�Ƿ�����λ������״̬
// ����ֵΪ�ܹ��ҵ�ͬ�����ĸ���
S32 BaseItemList::QuerySameObjectByID(S32 id, Vector<S32>& slotlist, bool checkSlotState)
{
	slotlist.clear();
	for(S32 i = 0; i < mMaxSlots; ++i)
	{
		if(!mSlots[i])
			continue;

		if(mSlots[i]->isItemObject() && (checkSlotState && mSlots[i]->getSlotState() != ShortcutObject::SLOT_LOCK))
		{
			ItemShortcut* pItem = (ItemShortcut*)(mSlots[i]);
			if(pItem && pItem->getRes()->getItemID() == id)
				slotlist.push_back(i);
		}
		else if(mSlots[i]->isSkillObject())
		{
			SkillShortcut* pSkillShort = (SkillShortcut*)(mSlots[i]);
			if(pSkillShort->getSkill()->GetBaseSkillId() == id)
				slotlist.push_back(i);
		}
	}
	return slotlist.size();
}

// ----------------------------------------------------------------------------
// ����λ����ҵ�һ��ͬ����������λ��
// obj				��λ������Ʒ���ܶ���
// checkSlotState	�Ƿ�����λ������״̬
// ignoreExtPro		�Ƿ������Ʒ�ĸ������Լ��
// ����ֵΪ�ҵ���һ��ͬ����������λ��
S32 BaseItemList::FindSameObject(ShortcutObject* obj, bool checkSlotState, bool ignoreExtPro)
{
	if(obj == NULL)
		return -1;
	if(obj->isItemObject())
	{
		for(S32 i = 0; i < mMaxSlots; ++i)
		{
			if(!mSlots[i]|| !mSlots[i]->isItemObject() ||
				(checkSlotState && mSlots[i]->getSlotState() == ShortcutObject::SLOT_LOCK))
				continue;
			if(g_ItemManager->isSameItem(mSlots[i], obj, ignoreExtPro))
				return i;
		}
	}
	else if(obj->isSkillObject())
	{
		for(S32 i = 0; i < mMaxSlots; ++i)
		{
			if(!mSlots[i] || !mSlots[i]->isSkillObject())
				continue;
			SkillShortcut* pSkillShort = (SkillShortcut*)(mSlots[i]);
			SkillShortcut* pSkillShortSrc = (SkillShortcut*)(obj);
			if(pSkillShort->getSkill() == pSkillShortSrc->getSkill())
				return i;
		}
	}
	
	return -1;
}

// ----------------------------------------------------------------------------
// ����λ���������ͬ�����
// obj				��λ������Ʒ���ܶ���
// slotlist			�Ǽ�ͬ���������λ�õ��б�
// checkSlotState	�Ƿ�����λ������״̬
// ignoreExtPro		�Ƿ������Ʒ�ĸ������Լ��
// ����ֵΪ�ܹ��ҵ�ͬ�����ĸ���
S32 BaseItemList::QuerySameObject(ShortcutObject* obj, Vector<S32>& slotlist, bool checkSlotState, bool ignoreExtPro /* = true */)
{
	if(obj == NULL)
		return 0;

	slotlist.clear();
	if(obj->isItemObject())
	{		
		for(S32 i = 0; i < mMaxSlots; ++i)
		{
			if(!mSlots[i] || !mSlots[i]->isItemObject() ||
				(checkSlotState && mSlots[i]->getSlotState() == ShortcutObject::SLOT_LOCK))
				continue;
			if(g_ItemManager->isSameItem(mSlots[i], obj, ignoreExtPro))
				slotlist.push_back(i);
		}
	}
	else if(obj->isSkillObject())
	{
		for(S32 i = 0; i < mMaxSlots; ++i)
		{
			if(!mSlots[i] || !mSlots[i]->isSkillObject())
				continue;
			SkillShortcut* pSkillShort = (SkillShortcut*)mSlots[i];
			SkillShortcut* pSkillShortSrc = (SkillShortcut*)obj;
			if(pSkillShort->getSkill() == pSkillShortSrc->getSkill())
				slotlist.push_back(i);
		}
	}
	return slotlist.size();
}

// ----------------------------------------------------------------------------
// �ж��Ƿ���Ч�Ĳ�λ
bool BaseItemList::IsVaildSlot(S32 index)
{
	return (index >=0 && index < mMaxSlots);
}

// ----------------------------------------------------------------------------
// ������λ(����ͬ����Ʒ���ӵ�һ��(����ܵ��ӵĻ�))
bool BaseItemList::CompactSlots()
{
	for(S32 i = 0; i < mMaxSlots; ++i)
	{
		// ����λ��Ϊ��Ʒ����Ӱ��������
		if(mSlots[i] != NULL && (!mSlots[i]->isItemObject() ||
			mSlots[i]->getSlotState() == ShortcutObject::SLOT_LOCK))
			continue;

		if(mSlots[i] == NULL)
		{
			// ����λΪ�գ��򽻻���һ����Ϊ�յĲ�λ����ǰλ��
			for(S32 j = i + 1; j < mMaxSlots; ++j)
			{
				if(mSlots[j] && mSlots[j]->getSlotState() == ShortcutObject::SLOT_COMMON)
				{
					mSlots[i] = mSlots[j];
					mSlots[j] = NULL;
					break;
				}
			}
		}
		
		// �����ȻΪ�գ�˵������ȫ�ǿղ�
		if(mSlots[i] == NULL)
			break;

		ItemShortcut* pItem = (ItemShortcut*)(mSlots[i]);
		U32 MaxOverNum = pItem->getRes()->getMaxOverNum();

		S32 iNextPos = -1;
		bool bFindSame = false;
		// Ѱ�Һ�����λ�з���ͬ��Ʒ����Ҫ������ͬ����Ʒ��ǰһ����Ʒ����
		for(S32 j = i + 1; j < mMaxSlots; ++j)
		{
			ItemShortcut* pNextItem = (ItemShortcut*)(mSlots[j]);
			if(!pNextItem || pNextItem->getSlotState() == ShortcutObject::SLOT_LOCK)
				continue;
			if(iNextPos == -1) iNextPos = j;
			if(g_ItemManager->isSameItem(pNextItem, pItem, true))
			{
				if(MaxOverNum > 1)
				{
					U32 CurrentNum	= pItem->getRes()->getQuantity();
					U32 NextNum	= pNextItem->getRes()->getQuantity();

					if(CurrentNum + NextNum > MaxOverNum)
					{
						bFindSame = true;
						pItem->getRes()->setQuantity(MaxOverNum);
						pNextItem->getRes()->setQuantity(CurrentNum + NextNum - MaxOverNum);

						// ��Ҫ������ͬ����Ʒ��ǰһ����Ʒ����
						if(j != iNextPos)
						{							
							mSlots[j] = mSlots[iNextPos];
							mSlots[iNextPos] = pNextItem;
						}
						break;
					}
					else
					{
						pItem->getRes()->setQuantity(CurrentNum + NextNum);
						delete mSlots[j];
						mSlots[j] = NULL;
					}
				}
				else
				{
					bFindSame = true;
					if(j != iNextPos)
					{
						mSlots[j] = mSlots[iNextPos];
						mSlots[iNextPos] = pNextItem;
					}
					break;
				}					
			}			
		}

		//���û�ҵ���ͬ��Ʒ�������Ѱ��ͬ��������Ʒ��ǰһ����Ʒ����
		if(!bFindSame)
		{
			iNextPos = -1;
			for(S32 j = i + 1; j < mMaxSlots; ++j)
			{
				ItemShortcut* pNextItem = (ItemShortcut*)(mSlots[j]);
				if(!pNextItem || pNextItem->getSlotState() == ShortcutObject::SLOT_LOCK)
					continue;
				if(iNextPos == -1) iNextPos = j;
				if(pNextItem->getRes()->getSubCategory() == pItem->getRes()->getSubCategory())
				{
					bFindSame = true;
					if(j != iNextPos)
					{						
						mSlots[j] = mSlots[iNextPos];
						mSlots[iNextPos] = pNextItem;
					}
					break;
				}
			}
		}

		//���û�ҵ���ͬ��������Ʒ�������Ѱ��ͬ��������Ʒ��ǰһ����Ʒ����
		if(!bFindSame)
		{
			iNextPos = -1;
			for(S32 j = i + 1; j < mMaxSlots; ++j)
			{
				ItemShortcut* pNextItem = (ItemShortcut*)(mSlots[j]);
				if(!pNextItem || pNextItem->getSlotState() == ShortcutObject::SLOT_LOCK)
					continue;
				if(iNextPos == -1) iNextPos = j;
				if(pNextItem->getRes()->getCategory() == pItem->getRes()->getCategory())
				{
					bFindSame = true;
					if(j != iNextPos)
					{						
						mSlots[j] = mSlots[iNextPos];
						mSlots[iNextPos] = pNextItem;
					}
					break;
				}
			}
		}
	}
	return true;
}

// ============================================================================
// ItemManager ��Ʒ����������
// 
// ============================================================================
ItemManager::ItemManager()
{
	g_ItemManager = this;
}

// ----------------------------------------------------------------------------
// ��ȡ��λ��Ʒ����
ShortcutObject* ItemManager::getShortcutSlot(Player* player, U32 slottype, S32 index)
{
	if(!player)
		return NULL;
	switch(slottype)
	{
	case SHORTCUTTYPE_PANEL:			return player->panelList.GetSlot(index);
	case SHORTCUTTYPE_INVENTORY:		return player->inventoryList.GetSlot(index);
	case SHORTCUTTYPE_EQUIP:			return player->equipList.GetSlot(index);
	case SHORTCUTTYPE_BANK:				return player->bankList.GetSlot(index);
	case SHORTCUTTYPE_TRADE:			return player->tradeList.GetSlot(index);
	case SHORTCUTTYPE_DUMP:				return player->dumpList.GetSlot(index);
	case SHORTCUTTYPE_NPCSHOP:			return player->npcShopList.GetSlot(index);
	case SHORTCUTTYPE_PICKUP:			return player->pickupList.GetSlotForGuiControl(index);
	case SHORTCUTTYPE_STALL:			return player->individualStallList.GetSlot(index);
	case SHORTCUTTYPE_STALL_PET:		return player->stallPetList.GetSlot(index);
	case SHORTCUTTYPE_STALL_BUYLIST:	return player->stallBuyItemList.GetSlot(index);
	case SHORTCUTTYPE_IDENTIFY:			return player->identifylist.GetSlot(index);
	case SHORTCUTTYPE_STRENGTHEN:		return player->strengthenlist.GetSlot(index);
	case SHORTCUTTYPE_GEM_MOUNT:		return player->mountGemList.GetSlot(index);
	case SHORTCUTTYPE_EQUIP_EMBEDABLE:	return EquipMountAble_List::getInstance()->getSlot(index);
	case SHORTCUTTYPE_EQUIP_PUNCHHOLE:	return player->punchHoleList.GetSlot(index);
	case SHORTCUTTYPE_PET:				return player->petList.GetSlot(index);
	case SHORTCUTTYPE_MOUNT_PET_LIST:	return player->mountPetList.GetSlot(index);
	case SHORTCUTTYPE_SPIRITLIST:		return player->mSpiritList.GetSlot(index);
	case SHORTCUTTYPE_REPAIR:			return player->mRepairList.GetSlot(index);
	case SHORTCUTTYPE_TEMP:				return player->tempList.GetSlot(index);
#ifndef NTJ_EDITOR
	case SHORTCUTTYPE_ITEMSPLIT:        return player->mItemSplitList.GetSlot(index);
	case SHORTCUTTYPE_ITEMCOMPOSE:      return player->mItemComposeList.GetSlot(index);
#endif
#ifdef NTJ_CLIENT
	case SHORTCUTTYPE_SKILL_COMMON:		return player->skillList_Common.GetSlot(index);
	case SHORTCUTTYPE_LIVINGSKILL:		return player->mLivingSkillList->GetSlot(index);
	case SHORTCUTTYPE_LIVINGCATEGORY:	return player->mLivingSkillList->GetSlot(index);
	case SHORTCUTTYPE_PRESCRIPTION:		return player->mPrescriptionList->GetSlot(index);
	case SHORTCUTTYPE_AUTOSELL:			return player->autoSellList.GetSlot(index);
	case SHORTCUTTYPE_MISSIONITEM:		return player->missionItemList.GetSlot(0, index);
	case SHORTCUTTYPE_FIXEDMISSIONITEM: return player->missionItemList.GetSlot(1, index);
	case SHORTCUTTYPE_SKILL_STUDY:		return player->skillList_Study.GetSlot(index);
	case SHORTCUTTYPE_SKILL_HINT:		return player->skillList_Hint.GetSlot(index);
	case SHORTCUTTYPE_PET_HELP_LIST:	return player->mPetHelpList.GetSlot(index);
	case SHORTCUTTYPE_TARGET_TRADE:
		{
			SceneObject* pTargetObject = player->getInteraction();
			Player* pTarget = NULL;
			if(pTargetObject && (pTarget = dynamic_cast<Player*>(pTargetObject)))
				return pTarget->tradeList.GetSlot(index);
			break;
		}
	case SHORTCUTTYPE_TARGET_STALL:
		{
			SceneObject* pTargetObject = player->getInteraction();
			Player* pTarget = NULL;
			if(pTargetObject && (pTarget = dynamic_cast<Player*>(pTargetObject)))
				return pTarget->individualStallList.GetSlot(index);
		}
		break;
	case SHORTCUTTYPE_TARGET_STALL_PET:
		{
			SceneObject* pTargetObject = player->getInteraction();
			Player* pTarget = NULL;
			if(pTargetObject && (pTarget = dynamic_cast<Player*>(pTargetObject)))
				return pTarget->stallPetList.GetSlot(index);
		}
		break;
	case SHORTCUTTYPE_TARGET_STALLBUYLIST:
		{
			SceneObject* pTargetObject = player->getInteraction();
			Player* pTarget = NULL;
			if(pTargetObject && (pTarget = dynamic_cast<Player*>(pTargetObject)))
				return pTarget->stallBuyItemList.GetSlot(index);
		}
		break;
	case SHORTCUTTYPE_STALL_LOOKUP:		return  player->lookUpList.GetSlot(index);
	case SHORTCUTTYPE_TRADE_STALL_HELP: return player->mTradeStallHelpList.GetSlot(index);
	case SHORTCUTTYPE_SUPERMARKET:	return player->mSuperMarketList.GetSlot(index);
	case SHORTCUTTYPE_RECOMMEND:	return player->mRecommendList.GetSlot(index);
	case SHORTCUTTYPE_SHOPBASKET:	return player->mShopBaksetList.GetSlot(index); 
	case SHORTCUTTYPE_SPIRITSKILLLIST:		return player->getSpiritTable().getCurrentSpiritSkillSlot(index);
	case SHORTCUTTYPE_SPIRITSKILLLIST_2:	return player->getSpiritTable().getCurrentSpiritSkillSlot_2(index);
	case SHORTCUTTYPE_TALENTSKILLLIST:		return player->getSpiritTable().getCurrentTalentSkillSlot(index);
#endif	
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// ��ȡ��λ��Ʒ����
ItemShortcut* ItemManager::getItemByUID(Player* player, U64 UID, S32& idx)
{
	if(!player)
		return NULL;
	ShortcutObject* pShort = NULL;
	ItemShortcut* pItem = NULL;
	// ���װ����
	for (S32 i=0; i<EQUIP_MAXSLOTS; ++i)
	{
		pShort = player->equipList.GetSlot(i);
		if(pShort && pShort->isItemObject())
		{
			pItem = (ItemShortcut*)(pShort);
			if(pItem->getRes() && pItem->getRes()->getUID() == UID)
			{
				idx = i;
				return pItem;
			}
		}
	}
	// �����Ʒ��
	for (S32 i=0; i<INVENTROY_MAXSLOTS; ++i)
	{
		pShort = player->inventoryList.GetSlot(i);
		if(pShort && pShort->isItemObject())
		{
			pItem = (ItemShortcut*)(pShort);
			if(pItem->getRes() && pItem->getRes()->getUID() == UID)
			{
				idx = i;
				return pItem;
			}
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// ��¡��Ʒ(ע���˴�������ʵ�����������ڴ��ͷ�)
ItemShortcut* ItemManager::cloneItem(ShortcutObject* pShortObject)
{
	if(pShortObject == NULL || !pShortObject->isItemObject())
		return NULL;
	return ItemShortcut::CreateItem((ItemShortcut*)pShortObject);
}

// ----------------------------------------------------------------------------
// �ж�������Ʒ�Ƿ���ͬһ����Ʒ
// ignoreExtPro ���Ծ�ȷƥ�丽������
bool ItemManager::isSameItem(ShortcutObject* pObj1, ShortcutObject* pObj2, bool ignoreExtPro)
{
	if(NULL == pObj1 || !pObj1->isItemObject() || NULL == pObj2 || !pObj2->isItemObject())
		return false;

	ItemShortcut* pItem1 = (ItemShortcut*)pObj1;
	ItemShortcut* pItem2 = (ItemShortcut*)pObj2;
	if(ignoreExtPro)
		return pItem1->getRes()->isSameBaseData(pItem2->getRes());
	else
		return pItem1->getRes()->isSameRes(pItem2->getRes());
}

// ----------------------------------------------------------------------------
// ��ȡָ����Ʒ��������λ���λ��
void ItemManager::getSameObject(Player* player, S32 slottype, ShortcutObject* pSlot, S32 &index)
{
	if(!player)
		return;
	switch(slottype)
	{
	case SHORTCUTTYPE_PANEL:
		index = player->panelList.FindSameObject(pSlot, true, false);
		break;
	case SHORTCUTTYPE_INVENTORY:
		index = player->inventoryList.FindSameObject(pSlot, true, false);
		break;
	case SHORTCUTTYPE_EQUIP:
		index = player->equipList.FindSameObject(pSlot, true, false);
		break;
	case SHORTCUTTYPE_BANK: 
		index = player->bankList.FindSameObject(pSlot, true, false);
		break;
	case SHORTCUTTYPE_TRADE:
		index = player->tradeList.FindSameObject(pSlot, true, false);
		break;
	case SHORTCUTTYPE_DUMP:
		index = player->dumpList.FindSameObject(pSlot, true, false);
		break;
	}
}

// ----------------------------------------------------------------------------
// �ж��Ƿ���ʹ����Ʒ
enWarnMessage ItemManager::canUseItem(Player* player, ItemShortcut* pItem, GameObject* pObj)
{
	if(!player || !pObj || !pItem || !pItem->getRes())
		return MSG_ITEM_USEITEMNOOBJECT;

	//�ж����״̬(������ս���ȵ�)
	if(player->isDisabled())
		return MSG_PLAYER_ISDEAD;
	
	//�ж�����Ƿ�����ʹ�ü���
	if(player->GetSpell().IsSpellRunning() && !(player->GetSpell().GetStatus() == Spell::Status_SpellCast && player->GetSpell().GetId() == pItem->getRes()->getUseStateID()))
		return MSG_PLAYER_SPELLRUNNING;

	//�ж���Ʒ�����Ƿ���ʹ��
	if(!pItem->getRes()->canBaseLimit(Res::ITEM_CANUSE))
		return MSG_ITEM_CANNOTUSE;

	//�ȼ��ж�
	if(pItem->getRes()->getLimitLevel() > player->getLevel())
		return MSG_PLAYER_LEVEL;

	//ְҵ�ж�
	if(!pItem->getRes()->canFamilyLimit(player->getFamily()))
		return MSG_PLAYER_FAMILY;

	//�Ա��ж�
	if(!pItem->getRes()->canSexLimit(player->getSex()))
		return MSG_PLAYER_SEX;

	//��Ʒ��ȴ�ж�
	SimTime cdTime, totalTime;
	if(player->GetCooldown(pItem->getRes()->getCoolTimeType(), cdTime, totalTime, false))
		return MSG_ITEM_ITEMBEUSED;
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ����϶���Ʒ
bool ItemManager::canDragItem(Player* player, ItemShortcut* pItem /* = NULL */)
{
	//�ж��������״̬
	if(!player || player->isDisabled())
		return false;
	//�ж���Ʒ�Ƿ�����λ��
	if(pItem && pItem->getSlotState() == ShortcutObject::SLOT_LOCK)
		return false;
	return true;
}

// ----------------------------------------------------------------------------
// ��������λѰ��ĳ��Ʒ
bool ItemManager::hasItem(Player* player, U32 itemID)
{
	if(!player || itemID == 0)
		return false;

	for( S32 i = 0; i < player->inventoryList.GetMaxSlots(); ++i)
	{
		ShortcutObject* pSlot = player->inventoryList.GetSlot(i);
		if(pSlot)
		{
			ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSlot);
			if(pItem && pItem->getRes()->getItemID() == itemID)
				return true;
		}
	}

	for( S32 i = 0; i < player->bankList.GetMaxSlots(); ++i)
	{
		ShortcutObject* pSlot = player->bankList.GetSlot(i);
		if(pSlot)
		{
			ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSlot);
			if(pItem && pItem->getRes()->getItemID() == itemID)
				return true;
		}
	}

	for( S32 i = 0; i < player->equipList.GetMaxSlots(); ++i)
	{
		ShortcutObject* pSlot = player->equipList.GetSlot(i);
		if(pSlot)
		{
			ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSlot);
			if(pItem && pItem->getRes()->getItemID() == itemID)
				return true;
		}
	}

	return false;
}

#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// ��Ʒ�ȸ���ʾ�ı�
bool ItemManager::insertText(Player* pPlayer, Res* pRes, char* buff,S32 mSlotType,S32 mSlotIndex)
{
	if(!pPlayer || !pRes)
		return false;

	char tempbuffer[1024] = "\n";
    
	//ͳ�ƻ��д���
    S32 iNewLineTime =0;  
	//��Ʒ����
	if(pRes->getItemName())
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t h='1' m='0' f='����_GB2312' n='16' c='%d'>%s</t>", Res::sColorValue[pRes->getColorLevel()], pRes->getItemName());
		dStrcpy(buff, 4096, tempbuffer);
	}

	//��Ʒ����
	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xff9600ff'>%s</t>", pRes->getCategoryName());
	dStrcat(buff, 4096, tempbuffer);

	//����ǰ׺
	if (pRes->IsActivatePro(EAPF_ATTACH) && (pRes->isWeapon() || pRes->isBody() || pRes->isPetEquip()))
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t h = '2' c='%d'>%s           </t>", Res::sColorValue[pRes->getPostfixQuality()+1],pRes->getPostfixName());
		dStrcat(buff, 4096, tempbuffer);
	}
	else
	{
		dStrcat(buff, 4096, "<b/>");
	}	
	
	// ʹ�õȼ�����
	if (pRes->getLimitLevel() > 1)
	{
		// ���Ǳ�ʯ
		if(pRes->isGem())
		{
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>��ʯ�ȼ�: %d��</t><b/>", pRes->getLimitLevel());
			dStrcat(buff, 4096, tempbuffer);
			
		}
		else
		{
			if (pRes->getLimitLevel() > pPlayer->getLevel())
			{
				dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xff1200ff'>ʹ�õȼ�: %d��</t><b/>", pRes->getLimitLevel());
				dStrcat(buff, 4096, tempbuffer);
			}
			else
			{
				dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʹ�õȼ�: %d��</t><b/>", pRes->getLimitLevel());
				dStrcat(buff, 4096, tempbuffer);
			}
		}
		iNewLineTime++;
	}

	//�;ö�
	if (pRes->getMaxWear() > 0)
	{
		S32 iCurMaxWear = 0;
		if(pRes->getExtData())
			iCurMaxWear = pRes->getExtData()->CurWear;
		S32 iMaxWear = 0;
		if(pRes->getCurrentMaxWear() < 0)
			iMaxWear = pRes->getMaxWear();
		else
			iMaxWear = pRes->getCurrentMaxWear();
		if (iCurMaxWear < pRes->getMaxWear() / 10)
		{
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xff1200ff'>�;ö�: %d/%d</t><b/>", iCurMaxWear, iMaxWear);
			dStrcat(buff, 4096, tempbuffer);
		}
		else
		{
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>�;ö�: %d/%d</t><b/>", iCurMaxWear, iMaxWear);
			dStrcat(buff, 4096, tempbuffer);
		}
		iNewLineTime++;
	}

	//ʹ�ô���
	else if (pRes->getUsedTimes() > 0)
	{
		if(pRes->getExtData())
		{
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʹ�ô���: %d</t><b/>", pRes->getExtData()->RemainUseTimes);
			dStrcat(buff, 4096, tempbuffer);
			iNewLineTime++;
		}
	}
	else
	{
		dStrcat(buff, 4096, "<b/>");
		iNewLineTime++;
	}

	//����
	if (pRes->isWeapon() || pRes->isBody() /*|| pRes->isPetEquip()*/)
	{
		if (pRes->getBindMode() == Res::BINDMODE_NONE)
		{
			dStrcat(buff, 4096, "<b/>");
			iNewLineTime++;
		}
	}
	else
	{
		dStrcat(buff, 4096, "<b/>");
		iNewLineTime++;
	}

	if (pRes->getLimitLevel() <= 1)
	{
		//dStrcat(buff, 4096, "<b/>");
		//iNewLineTime++;
	}

	if (pRes->isWeapon() || pRes->isBody())
	{
		if(pRes->getSexLimit() == Res::SEXLIMIT_MALE && pPlayer->getSex() == pRes->getSexLimit())
		{
			dStrcat(buff, 4096, "<t c='0xffffffff'>�Ա�����: ��</t><b/>");
			iNewLineTime++;
		}
		else if(pRes->getSexLimit() == Res::SEXLIMIT_MALE && pPlayer->getSex() != pRes->getSexLimit())
		{
			dStrcat(buff, 4096, "<t c='0xff1200ff'>�Ա�����: ��</t><b/>");
			iNewLineTime++;
		}
		else if(pRes->getSexLimit() == Res::SEXLIMIT_FEMALE && pPlayer->getSex() == pRes->getSexLimit())
		{
			dStrcat(buff, 4096, "<t c='0xffffffff'>�Ա�����: Ů</t><b/>");
			iNewLineTime++;
		}
		else if(pRes->getSexLimit() == Res::SEXLIMIT_FEMALE && pPlayer->getSex() != pRes->getSexLimit())
		{
			dStrcat(buff, 4096, "<t c='0xff1200ff'>�Ա�����: Ů</t><b/>");
			iNewLineTime++;
		}

		if(pRes->getFamilyCredit() > 0 && pRes->getFamilyCredit() < 511)
		{
			dStrcat(buff, 4096, "<t c='0xffffffff'>��������: </t>");
			bool first = true;
			for(S32 j = 0; j < MaxFamilies; j++)
			{
				if(pRes->getFamilyCredit() & BIT(j))
				{
					dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='%s'>%s</t>%s", 
							pPlayer->getFamily() == j ? "0xffffffff" : "0xaff1200ff",
							g_strFamily[j],
							first ? "<b/>" : " ");
					dStrcat(buff, 4096, tempbuffer);
					if(first) first = false;
				}
			}
			if (!first) 
				iNewLineTime++;
			//dStrcat(buff, 4096, first ? "�� </t><b/>" : "</t><b/>");
			//iNewLineTime++;
		}
	}	

	//���ݻ��д���������
	for (int i=0;i<3-iNewLineTime;i++)
	{
       dStrcat(buff, 4096, "<b/>");
	}

	/*if (!pRes->canBaseLimit(Res::ITEM_TRADE) || (pRes->isEquip() && !(pRes->canBaseLimit(Res::ITEM_DROP))) || !pRes->canBaseLimit(Res::ITEM_SAVEBANK)
				|| (pRes->isEquip() && !(pRes->canBaseLimit(Res::ITEM_FIX))))
	{
		dStrcat(buff, 4096, "<b/>");
	}*/	

	if (pRes->isWeapon() || pRes->isBody())
	{
		dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIWindow99_1_007' />");
	}

	//ǿ����ʾ
	if (pRes->isWeapon() || pRes->isBody() || pRes->isPetEquip())
	{
		if (pRes->canBaseLimit(Res::ITEM_UPGRADE))
		{
			S32 tempNum = 0;
			for (int i = 0; i < pRes->getBaseData()->MAX_EQUIPSTRENGTHEN_LEVEL; i++)
			{
				if (pRes->getBaseData()->getEquipStrengthenNum(i) <= 0)
					break;
				tempNum++;
			}
			if (pRes->getEquipStrengthens() > 0)
			{
				for (int i = 0; i < pRes->getEquipStrengthens(); i++)
				{
					dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIbutton03_1_014' />");
				}
				for (int i = 0; i < tempNum - pRes->getEquipStrengthens(); i++)
				{
					dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIbutton03_1_013' />");
				}
                
                dSprintf(tempbuffer, sizeof(tempbuffer), "<t h = '2' c='0x00b0f0ff'>ǿ��:+%d</t>",pRes->getEquipStrengthens());
				dStrcat(buff, 4096, tempbuffer);
			}
			else
			{
				for (S32 i = 0; i < tempNum; i++)
				{
					dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIbutton03_1_013' />");
				}				
				dStrcat(buff, 4096, "<b/>");
			}
		}
	}

	if (pRes->isWeapon() || pRes->isBody() /*|| pRes->isPetEquip()*/)
	{
		////����
		//strcat(buff, "<t c='0xad122eff'>δ����</t><b/>");
		//��ף���ʯ��Ƕ
		if (pRes->getAllowedHoles() > 0)
		{
			//dStrcat(buff, 4096, "<t c='0x71bfd2ff'>��ʯ��Ƕ:</t><b/>");
			//for (int i = 0; i < pRes->getOpenedHoles(); i++)
			//{
			//	dSprintf(tempbuffer,sizeof(tempbuffer), "<i s='gameres/gui/images/GUIbutton03_1_001' w='16' h='16' /><t c=%s>�ѿ����(����Ƕ)</t><b/>", pRes->GetSlotColor(i));
			//	dStrcat(buff, 4096, tempbuffer);
			//}
			//for (int i = 0; i < pRes->getLeftHoles(); i++)
			//{
			//	strcat(buff, "<i s='gameres/gui/images/GUIbutton03_1_001' w='16' h='16' /><t c='0xad122eff'>δ֪���(�ɴ��)</t><b/>");
			//}

			for (S32 i = 0; i < 3; i++)
			{
				char mGamColor[32] = {0};
				char mGamImage[256] = {0};
				char mSlotName[256] = {0};
				switch (pRes->GetSlotColor(i+1))
				{
				case 1:
					{
						dSprintf(mGamColor, sizeof(mGamColor), "%s", "0xff0000ff");
						if (!pRes->IsSlotEmbeded(i+1))
						{
							dSprintf(mGamImage, sizeof(mGamImage), "%s", "gameres/gui/images/GUIbutton03_1_001");
							//dSprintf(mSlotName, sizeof(mSlotName), "%s", "��ɫ��ۣ�����Ƕ��");
						}
						else
							dSprintf(mGamImage, sizeof(mGamImage), "%s", "gameres/gui/images/GUIbutton03_1_002");
					}
					break;
				case 2:
					{
						dSprintf(mGamColor, sizeof(mGamColor), "%s", "0x00ff00ff");
						if (!pRes->IsSlotEmbeded(i+1))
						{
							dSprintf(mGamImage, sizeof(mGamImage), "%s", "gameres/gui/images/GUIbutton03_1_005");
							//dSprintf(mSlotName, sizeof(mSlotName), "%s", "��ɫ��ۣ�����Ƕ��");
						}
						else
							dSprintf(mGamImage, sizeof(mGamImage), "%s", "gameres/gui/images/GUIbutton03_1_006");
					}

					break;
				case 3:
					{
						dSprintf(mGamColor, sizeof(mGamColor), "%s", "0x6464ffff");
						if (!pRes->IsSlotEmbeded(i+1))
						{
							dSprintf(mGamImage, sizeof(mGamImage), "%s", "gameres/gui/images/GUIbutton03_1_003");
							//dSprintf(mSlotName, sizeof(mSlotName), "%s", "��ɫ��ۣ�����Ƕ��");
						}
						else
							dSprintf(mGamImage, sizeof(mGamImage), "%s", "gameres/gui/images/GUIbutton03_1_004");
					}
					break;
				default:;
				}
				if (!pRes->IsSlotAllowEmbed(i+1))
					continue;
				if (!pRes->IsSlotOpened(i+1))
				{
					//dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIbutton03_1_009' />"/*<t c='0x646464ff'>δ֪���(�ɴ��)</t><b/>*/);
				}
				else if (!pRes->IsSlotEmbeded(i+1))
				{
					dSprintf(tempbuffer,sizeof(tempbuffer), "<i s='%s' />"  /*<t c='%s'>%s</t>*/, mGamImage/*,mGamColor,mSlotName*/);
					dStrcat(buff, 4096, tempbuffer);
				}
				else
				{
					if(pRes->getExtData())
					{
						//����Ƕ��ʯ
						U32 nGemId = pRes->getExtData()->EmbedSlot[i];
						ItemBaseData* pBase = g_ItemRepository->getItemData(nGemId);
						U32 mGamAtt = pBase->getBaseAttribute();
						Stats* pStats = g_BuffRepository.GetBuffStats(mGamAtt);
						if (pStats)
						{
							char statsBuff[1024] = {0};
							GetStatsDesc(pStats, mGamColor, statsBuff);
							dSprintf(tempbuffer,sizeof(tempbuffer), "<i s='%s' />"/*<t c='%s'>%s</t>"*/, mGamImage/*,mGamColor,statsBuff*/);
							dStrcat(buff, 4096, tempbuffer);
						}
					}					
				}
			}
           if (pRes->getLeftHoles()>0)
           {
               dSprintf(tempbuffer,sizeof(tempbuffer), "<t c='0xffffffff'>���Դ��:%d</t>",pRes->getLeftHoles());
			   dStrcat(buff, 4096, tempbuffer);
           }		  
		}

	    //����װ��
		if (pRes->getCurrentMaxWear() ==-1)
		{
			dStrcat(buff, 4096, "<t h = '2' c='0xff7171ff'>����װ��</t>");
		}
		else
			dStrcat(buff, 4096, "<b/>");
	}	

	

	// �����
	if (pRes->isWeapon() || pRes->isBody() /*|| pRes->isPetEquip()*/)
	{
		if(pRes->getBindPlayerID() > 0)
		{
			dStrcat(buff, 4096, "<t c='0xffff00ff'>�Ѱ�</t><b/>");
		}
		else
		{
			switch (pRes->getBindMode())
			{
			case Res::BINDMODE_EQUIP:
				{
					dStrcat(buff, 4096, "<t c='0xffffffff'>װ��ʱ��</t><b/>");
				}
				break;
			case Res::BINDMODE_PICKUP:
				{
					dStrcat(buff, 4096, "<t c='0xffffffff'>ʰȡʱ��</t><b/>");
				}
				break;
			case Res::BINDMODE_NONE:
				{
					//strcat(buff, "<b/>");
				}
				break;
			}
		}
	}

	Stats* tempStats = g_BuffRepository.GetBuffStats(pRes->getBaseAttribute());
	Stats* conStats = NULL;
	if (pRes->getEquipStrengthens() > 0)
		conStats = g_BuffRepository.GetBuffStats(pRes->getBaseData()->getEquipStrengthenNum(pRes->getEquipStrengthens()-1));
	
	if (!pRes->isPetEgg())
	{
		if(pRes->getCategory() == Res::CATEGORY_WEAPON)
		{
			if(tempStats)
			{
				if (conStats && pRes->getEquipStrengthens() > 0)
				{
					U32 playerBuffId = Macro_GetBuffId(g_FamilyBuff[pPlayer->getFamily()], pPlayer->getLevel());
					Stats* playerStats = g_BuffRepository.GetBuffStats(playerBuffId);
					dSprintf(tempbuffer,sizeof(tempbuffer), "<t c='0xffffffff'>������: %d </t><t c='0x00b0f0ff'>+%d%%</t><b/><t c='0xffffffff'>���鹥��: %d </t><t c='0x00b0f0ff'>+%d%%</t><b/><t c='0xffffffff'>�����ٶ�: %0.1f </t><b/>",
						tempStats->PhyDamage, S32((float(conStats->PhyDamage - tempStats->PhyDamage)/tempStats->PhyDamage)*100),
						tempStats->MuDamage, S32((float(conStats->MuDamage - tempStats->MuDamage)/tempStats->MuDamage)*100),
						(tempStats->AttackSpeed + playerStats->AttackSpeed) / 1000.0f);
					dStrcat(buff, 4096, tempbuffer);
				}
				else
				{
					U32 playerBuffId = Macro_GetBuffId(g_FamilyBuff[pPlayer->getFamily()], pPlayer->getLevel());
					Stats* playerStats = g_BuffRepository.GetBuffStats(playerBuffId);
					dSprintf(tempbuffer,sizeof(tempbuffer), "<t c='0xffffffff'>������: %d </t><b/><t c='0xffffffff'>���鹥��: %d </t><b/><t c='0xffffffff'>�����ٶ�: %0.1f </t><b/>",
						tempStats->PhyDamage, tempStats->MuDamage, (tempStats->AttackSpeed + playerStats->AttackSpeed) / 1000.0f);
					dStrcat(buff, 4096, tempbuffer);
				}
			}
		}
		else if(pRes->getCategory() == Res::CATEGORY_EQUIPMENT)
		{
			if(tempStats)
			{
				if (conStats && pRes->getEquipStrengthens() > 0)
				{
					dSprintf(tempbuffer,sizeof(tempbuffer), "<t c='0xffffffff'>�������: %d </t><t c='0x00b0f0ff'>+%d%%</t><b/><t c='0xffffffff'>���鿹��: %d </t><t c='0x00b0f0ff'>+%d%%</t><b/>", 
						tempStats->PhyDefence, S32((float(conStats->PhyDefence - tempStats->PhyDefence)/tempStats->PhyDefence)*100),
						tempStats->MuDefence, S32((float(conStats->MuDefence - tempStats->MuDefence)/tempStats->MuDefence)*100));
					dStrcat(buff, 4096, tempbuffer);
				}
				else
				{
					dSprintf(tempbuffer,sizeof(tempbuffer), "<t c='0xffffffff'>�������: %d </t><b/><t c='0xffffffff'>���鿹��: %d </t><b/>", tempStats->PhyDefence, tempStats->MuDefence);
					dStrcat(buff, 4096, tempbuffer);
				}
			}
		}
		else if(pRes->getCategory() == Res::CATEGORY_ORNAMENT)
		{
			if(tempStats)
			{
				if (conStats && pRes->getEquipStrengthens() > 0)
				{
					dSprintf(tempbuffer,sizeof(tempbuffer), "<t c='0xffffffff'>���鿹��: %d </t><t c='0x00b0f0ff'>+%d%%</t><b/>", tempStats->MuDefence, S32((float(conStats->MuDefence - tempStats->MuDefence)/tempStats->MuDefence)*100));
					dStrcat(buff, 4096, tempbuffer);
				}
				else
				{
					dSprintf(tempbuffer,sizeof(tempbuffer), "<t c='0xffffffff'>���鿹��: %d</t><b/>", tempStats->MuDefence);
					dStrcat(buff, 4096, tempbuffer);
				}
			}
		}
	}

	if (pRes->isGem())
	{
		U32 gemID = pRes->getBaseAttribute();
		if (gemID > 0)
		{
			Stats* stats = g_BuffRepository.GetBuffStats(gemID);
			if (stats)
			{
				char statsBuff[1024] = {0};
				GetStatsDesc(stats,"0xe08226ff",statsBuff);
				dStrcat(buff, 4096, statsBuff);
			}
		}
	}

	//����
	if (pRes->isWeapon() || pRes->isBody() /*|| pRes->isPetEquip()*/)
	{
		if (pRes->getIdentifyType() == Res::IDENTIFYTYPE_NONE)
		{
			for (int i = 0; i < 12; i++)
			{
				U32 statsID = pRes->getAppendAttributeID(i);
				if(statsID > 0)
				{
					Stats* stats = g_BuffRepository.GetBuffStats(statsID);
					if(stats)
					{
						char statsBuff[1024] = {0};
						GetStatsDesc(stats, "0x6bc23dff",statsBuff);
						dStrcat(buff, 4096, statsBuff);
					}
				}
			}
		}
		else
		{
			if(pRes->getExtData())
			{
				if (pRes->IsActivatePro(EAPF_ATTACH))
				{
					for (int i = 0; i < pRes->getExtData()->IDEProNum; i++)
					{
						U32 statsID = pRes->getExtData()->IDEProValue[i];
						if(statsID > 0)
						{
							Stats* stats = g_BuffRepository.GetBuffStats(statsID);
							if(stats)
							{
								char statsBuff[1024] = {0};
								GetStatsDesc(stats, "0x6bc23dff",statsBuff);
								dStrcat(buff, 4096, statsBuff);
							}
						}
					}
				}
				else
				{
					dStrcat(buff, 4096, "<t c='0xff1200ff'>����δ����</t><b/>");
				}
			}			
		}
	}


	//��Ƕ����
	if (pRes->isWeapon() || pRes->isBody())
	{
		if(pRes->getAllowedHoles() > 0 && pRes->getExtData())
		{
			
			for(int i=0; i<3;i++)
			{		
				char mGamColor[32] = {0};
				char mGamType[256] = {0};
				switch (pRes->GetSlotColor(i+1))
				{
				case 1:
					{
						dSprintf(mGamColor, sizeof(mGamColor), "%s", "0xff0000ff");
						dSprintf(mGamType, sizeof(mGamType), "%s", " ��ɫ��ʯ");
					}
					break;
				case 2:
					{
					 dSprintf(mGamColor, sizeof(mGamColor), "%s", "0x00ff00ff");
					 dSprintf(mGamType, sizeof(mGamType), "%s", " ��ɫ��ʯ");
					}
					break;
				case 3:
					{
                      dSprintf(mGamColor, sizeof(mGamColor), "%s", "0x6464ffff");
					  dSprintf(mGamType, sizeof(mGamType), "%s", " ��ɫ��ʯ");
					}					
					break;
				default:;
				}
				if (pRes->IsSlotEmbeded(i+1))
				{
					//����Ƕ��ʯ
					U32 nGemId = pRes->getExtData()->EmbedSlot[i];
					ItemBaseData* pBase = g_ItemRepository->getItemData(nGemId);
					U32 nGemLevel = pBase ? pBase->mLevelLimit : 0;
					U32 mGamAtt = pBase->getBaseAttribute();
					Stats* pStats = g_BuffRepository.GetBuffStats(mGamAtt);
					if (pStats)
					{
						char statsBuff[1024] = {0};						
						GetStatsDesc(pStats, "0x5a996aff", statsBuff);
						int index = dStrlen( statsBuff );
						statsBuff[index-4] = '\0';
						dSprintf(tempbuffer,sizeof(tempbuffer), "<t>%s</t><t c= '0xffff00ff'>%s (%d��)</t><b/>",statsBuff,mGamType,nGemLevel);
						dStrcat(buff, 4096, tempbuffer);
					}
				}
			}
		}
	}
	

	//��������
	//if (pRes->isTrump())
	//{
	//	if(pRes->canBaseLimit(Res::ITEM_SHENQI))
	//	{
	//		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x9d3d35ff'>����: </t><b/><t c='0x9d3d35ff'>������: %d</t><b/>", pRes->getExtData()->CurAdroit);
	//		dStrcat(buff, 4096, tempbuffer);		
	//	}
	//}

	if (pRes->isWeapon() || pRes->isBody() /*|| pRes->isPetEquip()*/)
	{
		////����
		//if(pRes->canBaseLimit(Res::ITEM_SHENQI ) || pRes->getExtData()->BindProID > 0)
		//{
		//	//dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x71bfd2ff'>������:(����������)</t><b/><t c='0x00b050ff'>��Ч��:����+%d</t><b/>", 10);
		//	//dStrcat(buff, 4096, tempbuffer);
		//}
		//else
		//{
		//	dStrcat(buff, 4096, "<t c='0xad122eff'>δ����:ʹ���������԰�</t><b/>");
		//}

		////��������
		//if(pRes->getExtData()->WuXingID)
		//{
		//	dStrcat(buff, 4096, "<t c='0xffaa00ff'>��������: </t><b/>");
		//}

		//ǿ������Ч��
		if (pRes->canBaseLimit(Res::ITEM_UPGRADE))
		{
			bool mShowSign = true;

			for (int i = 0; i < pRes->getBaseData()->MAX_EQUIPSTRENGTHEN_ADDLEVEL; i++)
			{
				if (pRes->getExtData() && pRes->getExtData()->EquipStrengthenValue[i][1] != 0)
				{
					char statsBuff[1024] = {0};
					Stats* appendStats = g_BuffRepository.GetBuffStats(pRes->getExtData()->EquipStrengthenValue[i][1]);
					if (appendStats)
					{
						if (pRes->getEquipStrengthens() >= pRes->getExtData()->EquipStrengthenValue[i][0])
						{
							/*if (mShowSign)
							{
								dStrcat(buff, 4096, "<t c='0x71bfd2ff'>ǿ������Ч��:</t><b/>");
								mShowSign = false;
							}*/

							GetStatsDesc(appendStats,"0x5A996Aff",statsBuff);
							int index = dStrlen( statsBuff );
							statsBuff[index-4] = '\0';
							dSprintf(tempbuffer,sizeof(tempbuffer), "<t>%s</t><t c= '0x00b0f0ff'> ǿ��(+%d)</t><b/>",statsBuff,pRes->getExtData()->EquipStrengthenValue[i][0]);
							dStrcat(buff, 4096, tempbuffer);
						}
						//dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='%s'>%s%d%s</t>", textColor,"ǿ��",pRes->getExtData()->EquipStrengthenValue[i][0],": ");
						//dStrcat(buff, 4096, tempbuffer);
					}
				}
			}
		}

		////��װ
		//if(pRes->getSuitID() > 0)
		//{
		//	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x9d3d35ff'>%s</t><b/>", "��װ: ");
		//	dStrcat(buff, 4096, tempbuffer);
		//}
	}
    
	//װ��Ʒ��
	if ((pRes->isWeapon() || pRes->isBody()) && pRes->getItemQualityValue()!=0)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffff00ff'>װ��Ʒ��:%d</t><b/>",pRes->getItemQualityValue());
		dStrcat(buff, 4096, tempbuffer);
	}
   
	//��ʯƷ��
	if ((pRes->isWeapon() || pRes->isBody()) && pRes->getGemQualityValue()!=0)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffff00ff'>��ʯƷ��:%d</t><b/>",pRes->getGemQualityValue());
		dStrcat(buff, 4096, tempbuffer);
	}

	if (pRes->getExtData())
	{
		if ((pRes->isWeapon() || pRes->isBody()) && (dStrlen(pRes->getExtData()->Producer) > 0 || dStrlen(pRes->getDescription()) > 0 || dStrlen(pRes->getHelp()) > 0 || dStrlen(pRes->getPurpose()) > 0))
		{
			dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIWindow99_1_007' />");
		}
	}
	
	//������
	if(pRes->getExtData() && dStrlen(pRes->getExtData()->Producer) > 0)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t h = '1' c='0xf28226ff'>%s</t><b/>", pRes->getExtData()->Producer);
		dStrcat(buff, 4096, tempbuffer);
	}

	//��������
	if(dStrlen(pRes->getDescription()) > 0)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>%s</t><b/>", pRes->getDescription());
		dStrcat(buff, 4096, tempbuffer);
	}

	//����˵��
	if(dStrlen(pRes->getHelp()) > 0)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x00b050ff'>%s</t><b/>", pRes->getHelp());
		dStrcat(buff, 4096, tempbuffer);
	}

	//ʹ��˵��
	if (dStrlen(pRes->getPurpose()) > 0)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x00b050ff'>ʹ��˵��:%s</t><b/>", pRes->getPurpose());
		dStrcat(buff, 4096, tempbuffer);
	}

	//�Ƿ���Խ���
	if (!pRes->canBaseLimit(Res::ITEM_TRADE))
	{
		dStrcat(buff, 4096, "<t c='0xffffffff'>���ɽ���  </t>");
	}

	//�Ƿ���Զ���
	if(pRes->isEquip() && !(pRes->canBaseLimit(Res::ITEM_DROP)))
	{
		dStrcat(buff, 4096, "<t c='0xffffffff'>���ɶ���  </t>");
	}

	//�Ƿ���Դ���
	if(!pRes->canBaseLimit(Res::ITEM_SAVEBANK))
	{
		dStrcat(buff, 4096, "<t c='0xffffffff'>���ɴ���  </t>");
	}

	//��������
	if(pRes->isEquip() && !(pRes->canBaseLimit(Res::ITEM_FIX)))
	{
		dStrcat(buff, 4096, "<t c='0xffffffff'>��������  </t>");
	}

	if (pRes->isWeapon() || pRes->isBody())
	{
		dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIWindow99_1_007' />");
	}

	S32 goldText = 0, sillerText = 0, copperText = 0;
	//���߼۸�
	if(mSlotType == SHORTCUTTYPE_NPCSHOP)
	{
		S32 iPrice = pPlayer->npcShopList.mSlots[mSlotIndex]->CurrencyValue;
		if (iPrice < 100)
		{
			copperText = iPrice;
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>����۸�:</t><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_026' />",copperText);
			dStrcat(buff, 4096, tempbuffer);
		}
		else if (iPrice >=100 && iPrice < 10000)
		{
			sillerText = mFloor(iPrice / 100);
			copperText = iPrice - sillerText * 100;
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>����۸�:</t><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_025' /><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_026' />",sillerText,copperText);
			dStrcat(buff, 4096, tempbuffer);
		}
		else if (iPrice >= 10000)
		{
			goldText = mFloor(iPrice / 10000);
			sillerText = mFloor((iPrice - goldText * 10000) / 100);
			copperText = iPrice - (goldText * 10000 + sillerText * 100);
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>����۸�:</t><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_024' /><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_025' /><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_026' />",goldText,sillerText,copperText);
			dStrcat(buff, 4096, tempbuffer);
		}
	}
	else
	{
		if( (pPlayer->getInteractionState() == Player::INTERACTION_NPCTRADE) && pRes->canBaseLimit(Res::ITEM_SELLNPC) )
		{
			S32 iPrice = pRes->getSalePrice();
			if (iPrice < 100)
			{
				copperText = iPrice;
				dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x777777ff'>���ۼ۸�:</t><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_026' />",copperText);
				dStrcat(buff, 4096, tempbuffer);
			}
			else if (iPrice >=100 && iPrice < 10000)
			{
				sillerText = mFloor(iPrice / 100);
				copperText = iPrice - sillerText * 100;
				dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x777777ff'>���ۼ۸�:</t><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_025' /><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_026' />",sillerText,copperText);
				dStrcat(buff, 4096, tempbuffer);
			}
			else if (iPrice >= 10000)
			{
				goldText = mFloor(iPrice / 10000);
				sillerText = mFloor((iPrice - goldText * 10000) / 100);
				copperText = iPrice - (goldText * 10000 + sillerText * 100);
				dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x777777ff'>���ۼ۸�:</t><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_024' /><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_025' /><t c='0xffffffff'>%d</t><i s='gameres/gui/images/GUIWindow31_1_026' />",goldText,sillerText,copperText);
				dStrcat(buff, 4096, tempbuffer);
			}
		}
	}

	if (mSlotType == SHORTCUTTYPE_INVENTORY)
	{
		if (pPlayer->getInteractionState() == Player::INTERACTION_NPCTRADE && pRes->canBaseLimit(Res::ITEM_SELLNPC))
		{
			dStrcat(buff, 4096, "<b/><t c='0x00b050ff'>�Ҽ��������</t>");
		}
		else if (pPlayer->getInteractionState() != Player::INTERACTION_PLAYERTRADE)
		{
			if (pRes->isWeapon() || pRes->isBody())
			{
// 				if((pPlayer->getInteractionState() != Player::INTERACTION_NPCTRADE) && pRes->IsSlotOpened(1))
// 				{					
// 					dStrcat(buff, 4096, "<t h = '1' c='0x00b050ff'>����SHIFT+�Ҽ��������������Ƕ��</t>");
// 				}
// 				else
//                   dStrcat(buff, 4096, "<b/>");

				dStrcat(buff, 4096, "<b/><t c='0x00b050ff'>�Ҽ����װ��</t>");
			}
			else if (pRes->canBaseLimit(Res::ITEM_CANUSE))
			{
				dStrcat(buff, 4096, "<b/><t c='0x00b050ff'>�Ҽ����ʹ��</t>");
			}
		}
	}
	
	return true;
}

// ----------------------------------------------------------------------------
// �����ȸ���ʾ�ı�
bool ItemManager::insertText(Player* pPlayer, SkillData* data, char* buff, S32 mSlotType,S32 mSlotIndex)
{
	if(!pPlayer || !data)
		return false;

	char tempbuffer[1024] = "\n";

	if (data->GetName())
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t m='0' f='����_GB2312' n='16' c='0xffffffff'>%s</t>", data->GetName());
		dStrcat(buff, 4096, tempbuffer);
	}

	if (data->GetLevel())
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t h = '2' c='0xffffffff'>�ȼ�:%d��</t>", data->GetLevel());
		dStrcat(buff, 4096, tempbuffer);
	}

	if (data->IsFlags(SkillData::Flags_Passive))
	{
		dStrcat(buff, 4096, "<t c='0xf28226ff'>��������</t><b/>");
	}
	else
	{
		dStrcat(buff, 4096, "<t c='0xf28226ff'>��������</t><b/>");
	}

	if (data->GetCast().cooldown == 0)
	{
		dStrcat(buff, 4096, "<t c='0xffffffff'>��ȴʱ��:��</t><b/>");
	}
	else if (data->GetCast().cooldown > 0 && data->GetCast().cooldown < 60000)
	{
		F32 timeNum = data->GetCast().cooldown/1000.0f;
		if (timeNum > (int)timeNum)
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>��ȴʱ��:%0.1f��</t><b/>", timeNum);
		else
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>��ȴʱ��:%d��</t><b/>", (int)timeNum);
		dStrcat(buff, 4096, tempbuffer);
	}
	else if (data->GetCast().cooldown >= 60000 && data->GetCast().cooldown < 3600000)
	{
		F32 timeNum = data->GetCast().cooldown/1000.0f/60.0f;
		if (timeNum > (int)timeNum)
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>��ȴʱ��:%0.1f��</t><b/>", timeNum);
		else
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>��ȴʱ��:%d��</t><b/>", (int)timeNum);
		dStrcat(buff, 4096, tempbuffer);
	}
	else if (data->GetCast().cooldown >= 3600000)
	{
		F32 timeNum = data->GetCast().cooldown/1000.0f/60.0f/60.0f;
		if (timeNum > (int)timeNum)
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>��ȴʱ��:%0.1fʱ</t><b/>",timeNum);
		else
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>��ȴʱ��:%dʱ</t><b/>", (int)timeNum);
		dStrcat(buff, 4096, tempbuffer);
	}	

	if (data->GetCast().readyTime == 0)
	{
		strcat(buff, "<t c='0xffffffff'>ʩ��ʱ��:˲��</t><b/>");
	}
	else if (data->GetCast().readyTime > 0 && data->GetCast().readyTime < 60000)
	{
		F32 timeNum = data->GetCast().readyTime/1000.0f;
		if (timeNum > (int)timeNum)
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʩ��ʱ��:%0.1f��</t><b/>", timeNum);
		else
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʩ��ʱ��:%d��</t><b/>", (int)timeNum);
		dStrcat(buff, 4096, tempbuffer);
	}
	else if (data->GetCast().readyTime >= 60000 && data->GetCast().readyTime < 3600000)
	{
		F32 timeNum = data->GetCast().readyTime/1000.0f/60.0f;
		if (timeNum > (int)timeNum)
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʩ��ʱ��:%0.1f��</t><b/>", timeNum);
		else
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʩ��ʱ��:%d��</t><b/>", (int)timeNum);
		dStrcat(buff, 4096, tempbuffer);
	}
	else if (data->GetCast().readyTime >= 3600000)
	{
		F32 timeNum = data->GetCast().readyTime/1000.0f/60.0f/60.0f;
		if (timeNum > (int)timeNum)
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʩ��ʱ��:%0.1fʱ</t><b/>", "", timeNum);
		else
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʩ��ʱ��:%dʱ</t><b/>", (int)timeNum);
		dStrcat(buff, 4096, tempbuffer);
	}

	F32 rangeNum = data->GetCastLimit().rangeMax;
	if (rangeNum > (int)rangeNum)
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʩ�ž���:%0.1f��</t><b/>", rangeNum);
	else
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>ʩ�ž���:%d��</t><b/>", (int)rangeNum);
	dStrcat(buff, 4096, tempbuffer);

	dStrcat(buff, 4096, "<t c='0xffffffff'>��������:</t>");

	SkillData::Cost& cost = data->GetCost();
	if(cost.HP || cost.MP || cost.PP || cost.vigor || cost.vigour || cost.item)
	{
		if (data->GetCost().HP)
		{
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>%dHP </t>", data->GetCost().HP);
			dStrcat(buff, 4096, tempbuffer);
		}

		if (data->GetCost().MP)
		{
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>%dMP </t>", data->GetCost().MP);
			dStrcat(buff, 4096, tempbuffer);
		}

		if (data->GetCost().PP)
		{
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>%dPP </t>", data->GetCost().PP);
			dStrcat(buff, 4096, tempbuffer);
		}

		//if (data->GetCost().vigor)
		//{
		//	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>%d���� </t>", data->GetCost().vigor);
		//	dStrcat(buff, 4096, tempbuffer);
		//}

		if (data->GetCost().vigour)
		{
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>%dSP</t>", data->GetCost().vigour);
			dStrcat(buff, 4096, tempbuffer);
		}

		if (data->GetCost().item)
		{
			ItemBaseData* pBaseData = g_ItemRepository->getItemData(data->GetCost().item);
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>%s</t>", pBaseData->getItemName());
			dStrcat(buff, 4096, tempbuffer);
		}
	}
	else
	{
		dStrcat(buff, 4096, "<t c='0xffffffff'>��</t>");
	}

	if (data->GetText())
	{
		dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIWindow99_1_007' />");
		dSprintf(tempbuffer, sizeof(tempbuffer), "<b/><t c='0xffffffff'>%s</t><b/>", data->GetText());
		dStrcat(buff, 4096, tempbuffer);
	}

 
	if (!data->IsFlags(SkillData::Flags_Passive))
	{
		dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIWindow99_1_007' />");
		dStrcat(buff, 4096, "<t c='0xffffffff'>��굥��ʹ��</t>");
	}

	return true;
}

//-----------------------------------------------------------------------------
//������ȸ��ı�
bool ItemManager::insertText(Player* pPlayer, LivingSkillData* pData,char* buff, S32 mSlotType, S32 mSlotIndex)
{
    if (!pData || !pPlayer)
	  return false;

   char tempbuffer[1024] = "\n";

   if (pData->getName())
   {
	   dSprintf(tempbuffer, sizeof(tempbuffer), "<t m='0' f='����_GB2312' n='16' c='0xffffffff'>%s</t><b/>",pData->getName());
	   dStrcat(buff, 4096, tempbuffer);
   }

   dStrcat(buff, 4096, "<t c='0xf28226ff'>�����</t><b/>");

   if (pData->getLevel())
   {
	   dSprintf(tempbuffer, sizeof(tempbuffer),"<t c= '0xffffffff'>�ȼ�:%d��</t><b/>",pData->getLevel());
	   dStrcat(buff, 4096, tempbuffer);
   }

   if(pData->getCategory()<10 && !pData->getSubCategory())
   {
     dStrcat(buff, 4096, "<b/>");
   }
   for (int i=0;i<2;i++)
   {
	   dStrcat(buff, 4096, "<b/>");
   }

   if(pData->getDesc())
   {
     dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIWindow99_1_007' />");
     dSprintf(tempbuffer, sizeof(tempbuffer),"<t c= '0xffffffff'>%s</t>",pData->getDesc());
	 dStrcat(buff, 4096, tempbuffer);
   }

   if(pData->getCategory()<10 && !pData->getSubCategory())
   {
	   dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIWindow99_1_007' />");
	   dStrcat(buff, 4096, "<t c='0x00b0f0ff'>�Ҽ��������������</t><b/>");
   }
   else
   {
     dStrcat(buff, 4096, "<b/>");
   }
   

   return true;
}

// ----------------------------------------------------------------------------
// BUFF���ȸ���ʾ�ı�
bool ItemManager::insertText(BuffData* pBuffData, char* buff )
{
	if (!pBuffData)
		return false;

	char tempBuffer[1024] = "\n";
	char buffNameColor[32] = {0};

	if(pBuffData->IsFlags(BuffData::Flags_Buff))
		dSprintf(buffNameColor, sizeof(buffNameColor), "0xffffffff");
	else
		dSprintf(buffNameColor, sizeof(buffNameColor), "0x9B9B9Bff");
	if (pBuffData->GetName())
	{
		dSprintf(tempBuffer, sizeof(tempBuffer), "<t f='����_GB2312' n='16' c='%s'>%s</t><b/>", buffNameColor, pBuffData->GetName());
		dStrcat(buff, 1024, tempBuffer);
	}
	
	if (pBuffData->GetText())
	{
		dSprintf(tempBuffer, sizeof(tempBuffer), "<t c='0xffffffff'>%s</t>", pBuffData->GetText());
		dStrcat(buff, 1024, tempBuffer);
	}
	return true;
}
#endif

#ifdef NTJ_SERVER
// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ָ��λ����Ʒ��ʹ�ô���
S32 ItemManager::getItemUsedTimes(Player* player, S32 index, U32 itemid)
{
	if(!player || index < 0 || index >= player->inventoryList.GetMaxSlots())
		return 0;
	ItemShortcut* pItem = (ItemShortcut*)player->inventoryList.GetSlot(index);
	if(!pItem || pItem->getRes()->getItemID() != itemid)
		return 0;
	return pItem->getRes()->getUsedTimes();
}

// ----------------------------------------------------------------------------
// ������Ʒʹ�ô���
enWarnMessage ItemManager::setItemUsedTimes(Player* player, S32 index, U32 itemid, S32 num)
{
	if(!player || num < 0)
		return MSG_INVALIDPARAM;
	if(index < 0 || index >= player->inventoryList.GetMaxSlots())
		return MSG_INVALIDPARAM;
	ItemShortcut* pItem = (ItemShortcut*)player->inventoryList.GetSlot(index);
	if(!pItem || pItem->getRes()->getItemID() != itemid)
		return MSG_INVALIDPARAM;

	// ��Ʒʹ�ô���
	U32 times = pItem->getRes()->getResUsedTimes();
	if(times <= 0)
		return MSG_INVALIDPARAM;

	// ����Ʒʹ�ô���Ϊ0��ʹ�ú���ʧ���ã���ɾ������Ʒ
	if(num == 0 && pItem->getRes()->canBaseLimit(Res::ITEM_USEDESTROY))
	{
		player->inventoryList.SetSlot(index, NULL, true);
		player->inventoryList.UpdateToClient(player->getControllingClient(), index, ITEM_NOSHOW);
	}
	else
	{
		pItem->getRes()->setUsedTimes(num);
		player->inventoryList.UpdateToClient(player->getControllingClient(), index, ITEM_NOSHOW);
	}

	//ͬ�����¿����
	syncPanel(player, itemid);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ʹ����Ʒ
enWarnMessage ItemManager::useItem(Player* player, S32 type, S32 col, GameObject* pTarget)
{
	//�ж���Ʒʹ�õĶ���
	if(!pTarget || !player)
		return MSG_ITEM_USEITEMNOOBJECT; 

	ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(player, type, col);
	if(!pShortcut)
		return MSG_NONE;
	//�ж��Ƿ�Ϊ��Ʒ
	if(!pShortcut->isItemObject())
		return MSG_ITEM_CANNOTUSE;

	ItemShortcut* pItem = (ItemShortcut*)pShortcut;
	//�ж��Ƿ���ʹ�ô���Ʒ
	enWarnMessage msgCode = canUseItem(player, pItem, pTarget);
	if(msgCode != MSG_NONE)
		return msgCode;

	//�ж��Ƿ���ﵰ
	if (pItem->getRes()->isPetEgg())
	{
		msgCode = PetOperationManager::CanUsePetEgg(player, type, col);
		if (msgCode != MSG_NONE)
			return msgCode;
		PetTable& petTable = (PetTable&)player->getPetTable();
		if(!petTable.fillPetInfo(player, pItem))
			return MSG_PET_DATA_NOT_EXIST;
	}

	//���ݴ���ȴ��ز���ֵ
	bool isSetCoolTime  = pItem->getRes()->isSetCoolTime();
	U32 uCoolTimeType	= pItem->getRes()->getCoolTimeType();
	U32 uCoolTime		= pItem->getRes()->getCoolTime();
	
	RegisterTimeItem(player, pItem);
	const char* result = NULL;
	bool ret = true;

	S32 index = -1;
	ItemShortcut* pSrcItem = NULL;
	if(type == SHORTCUTTYPE_PANEL)
	{			
		// ����Ʒ����������ͬ����Ʒ
		//U64 uUID = pItem->getRes()->getUID();
		//index = player->inventoryList.findSlot(uUID);
		index = player->inventoryList.FindSameObject(pItem, true, false);
		pSrcItem = (ItemShortcut*)player->inventoryList.GetSlot(index);
	}
	else if(type == SHORTCUTTYPE_INVENTORY)
	{
		index = col;
		pSrcItem = pItem;
	}

	if(index != -1 && pSrcItem)
	{
		U32 objectType = pTarget->getGameObjectMask() /2 + 1;
		result = Con::executef("Item_Use", Con::getIntArg(player->getId()),
			Con::getIntArg(pItem->getRes()->getItemID()),
			Con::getIntArg(objectType),
			Con::getIntArg(pTarget->getId()),
			Con::getIntArg(index));
	}

	if(!result || dAtob(result) == false)
		return MSG_ITEM_ITEMDATAERROR;

	// ������Ҫ����Ʒ��ȴ�����趨
	if(isSetCoolTime)
		player->SetCooldown_Group(uCoolTimeType, uCoolTime, uCoolTime);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �����ʱ��Ʒ��
void ItemManager::clearTempItemList()
{
	for (S32 i = 0; i < mTempItemList.size(); i++)
	{
		if (!mTempItemList[i])
			continue;

		if(mTempItemList[i]->srcEntry)
			delete mTempItemList[i]->srcEntry;
		if(mTempItemList[i]->newEntry)
			delete mTempItemList[i]->newEntry;
		delete mTempItemList[i];
		mTempItemList[i] = NULL;
	}
	mTempItemList.clear();
}

// ----------------------------------------------------------------------------
// ������Ʒ
enWarnMessage ItemManager::ExchangeShortcut(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	if(!param || !(param->player))
		return msg;

	switch (param->SrcType)
	{
	case SHORTCUTTYPE_PANEL:
		{
			switch(param->DestType)
			{
			case SHORTCUTTYPE_PANEL:		return PanelMoveToPanel(param);
			case SHORTCUTTYPE_NONE:
				{
					param->player->panelList.SetSlot(param->SrcIndex, NULL, true);
					param->player->panelList.UpdateToClient(param->player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
					return MSG_NONE;
				}
			}
		}
		break;
	case SHORTCUTTYPE_INVENTORY:
		{
			ShortcutObject* pSlot = getShortcutSlot(param->player, param->SrcType, param->SrcIndex);
			if(pSlot->getSlotState() == ShortcutObject::SLOT_LOCK)
				return MSG_ITEM_CANNOTDRAG;
			switch(param->DestType)
			{
			case SHORTCUTTYPE_PANEL:			return InventoryMoveToPanel(param);
			case SHORTCUTTYPE_INVENTORY:		return InventoryMoveToInventory(param);
			case SHORTCUTTYPE_EQUIP:			return InventoryMoveToEquip(param);
			case SHORTCUTTYPE_BANK:				return InventoryMoveToBank(param);
			case SHORTCUTTYPE_IDENTIFY:			return InventoryMoveToIdentify(param);
			case SHORTCUTTYPE_STRENGTHEN:		return InventoryMoveToStrengthen(param);
			case SHORTCUTTYPE_GEM_MOUNT:		return InventoryMoveToMountGem(param);
			case SHORTCUTTYPE_EQUIP_PUNCHHOLE:	return InventoryMoveToPunchHole(param);
			case SHORTCUTTYPE_TRADE:			return InventoryMoveToTrade(param);
			case SHORTCUTTYPE_STALL:			return InventoryMoveToStall(param);
			case SHORTCUTTYPE_REPAIR:			return InventoryMoveToRepairList(param);
			case SHORTCUTTYPE_TEMP:				return TempList::InventoryMoveToTempList(param);
			case SHORTCUTTYPE_ITEMSPLIT:        return param->player->mItemSplitList.InventoryToItemSplit(param);
			case SHORTCUTTYPE_ITEMCOMPOSE:      return param->player->mItemComposeList.InventoryToItemCompose(param);
			case SHORTCUTTYPE_MISSIONSUBMITLIST:return InventoryMoveToMissionSubmit(param);
			}
		}
		break;
	case SHORTCUTTYPE_SKILL_COMMON:
		{
			if(param->DestType == SHORTCUTTYPE_PANEL)
				return SkillCommonMoveToPanel(param);
		}
		break;
	case SHORTCUTTYPE_SKILL_HINT:
		{
			switch(param->DestType)
			{
			    case SHORTCUTTYPE_PANEL:        return SkillHintMoveToPanel(param);
				case SHORTCUTTYPE_NONE:
					{
					  param->player->skillList_Hint.SetSlot(param->SrcIndex,NULL,true);
					  param->player->skillList_Hint.UpdateToClient(param->player->getControllingClient(),param->SrcIndex,ITEM_NOSHOW);
					  return MSG_NONE;
					}
			}
		}
		break;
	case SHORTCUTTYPE_EQUIP:
		{
			switch(param->DestType)
			{
				case SHORTCUTTYPE_INVENTORY:	return EquipMoveToInventory(param);
				case SHORTCUTTYPE_GEM_MOUNT:	return EquipMoveToMountGem(param);
				case SHORTCUTTYPE_TEMP:			return TempList::EquipMoveToTempList(param);
			}
		}
		break;
	case SHORTCUTTYPE_IDENTIFY:
		{
			if (param->DestType == SHORTCUTTYPE_INVENTORY)
				return IdentifyMoveToInventory(param);
		}
		break;
	case SHORTCUTTYPE_STRENGTHEN:
		{
			switch(param->DestType)
			{
				case SHORTCUTTYPE_INVENTORY:	return StrengthenMoveToInventory(param);
				case SHORTCUTTYPE_STRENGTHEN:	return StrengthenMoveToStrengthen(param);
			}
		}
		break;
	case SHORTCUTTYPE_EQUIP_PUNCHHOLE:
		{
			switch(param->DestType)
			{
				case SHORTCUTTYPE_INVENTORY:	return PunchHoleMoveToInventory(param);
			}
		}
		break;
	case SHORTCUTTYPE_GEM_MOUNT:
		{
			switch(param->DestType)
			{
				case SHORTCUTTYPE_INVENTORY:	return MountGemMoveToInventory(param);
			}
		}
		break;
	case SHORTCUTTYPE_LIVINGCATEGORY:
		{
			if(param->DestType == SHORTCUTTYPE_PANEL)
				return LivingSkillMoveToPanel(param);
		}
		break;	
	case SHORTCUTTYPE_BANK:
		{
			switch(param->DestType)
			{
			case SHORTCUTTYPE_BANK: return BankMoveToBank(param);
			case SHORTCUTTYPE_INVENTORY: return BankMoveToInventory(param);
			}
		}
		break;
	case SHORTCUTTYPE_TRADE:
		{
			if(param->DestType == SHORTCUTTYPE_INVENTORY)
				return TradeMoveToInventory(param);
		}
		break;
	case SHORTCUTTYPE_STALL:
		{
			if (param->DestType == SHORTCUTTYPE_INVENTORY)
				return StallMoveToInventory(param);
		}
		break;
	case SHORTCUTTYPE_TEMP:
		{
			switch(param->DestType)
			{
			case SHORTCUTTYPE_TEMP:			return TempList::TempListMoveToTempList(param);	
			case SHORTCUTTYPE_INVENTORY:	return TempList::TempListMoveToInventory(param);
			case SHORTCUTTYPE_EQUIP:		return TempList::TempListMoveToEquip(param);
			}
		}
		break;
	case SHORTCUTTYPE_PET:
		{
			switch(param->DestType)
			{
			case SHORTCUTTYPE_PET:			return PetListMoveToPetList(param);
			case SHORTCUTTYPE_PANEL:		return PetListMoveToPanel(param);
			case SHORTCUTTYPE_STALL_PET:	return PetListMoveToStallPetList(param);
			case SHORTCUTTYPE_TEMP:			return PetListMoveToTempList(param);
			case SHORTCUTTYPE_MISSIONSUBMITLIST: return PetListMoveToMissionSubmit(param);
			}
		}
		break;
	case SHORTCUTTYPE_MOUNT_PET_LIST:
		{
			switch(param->DestType)
			{
			case SHORTCUTTYPE_MOUNT_PET_LIST:	return MountListMoveToMountList(param);
			case SHORTCUTTYPE_PANEL:			return MountListMoveToPanel(param);
			}
		}
		break;
	case SHORTCUTTYPE_STALL_PET:
		{
			if(param->DestType == SHORTCUTTYPE_STALL_PET)
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
			else
			{
				PetShortcut *pPetShortcut = dynamic_cast<PetShortcut*>(param->player->stallPetList.GetSlot(param->SrcIndex));
				if (!pPetShortcut)
					return MSG_NONE;				
				stPetInfo *pPetInfo = (stPetInfo*)param->player->getPetTable().getPetInfo( pPetShortcut->getSlotIndex() );
				if (!pPetInfo)
					return MSG_NONE;
				
				pPetInfo->status = PetObject::PetStatus_Idle;
				PetOperationManager::ServerSendUpdatePetSlotMsg(param->player, pPetShortcut->getSlotIndex(), stPetInfo::PetStatus_Mask);
				param->player->stallPetList.SetSlot(param->SrcIndex, NULL, 0);
				param->player->stallPetList.UpdateToClient(param->player->getControllingClient(), param->SrcIndex);
				return MSG_NONE;
			}
		}
	case SHORTCUTTYPE_ITEMSPLIT:
		{
			if(param->DestType == SHORTCUTTYPE_INVENTORY)
				return param->player->mItemSplitList.ItemSplitToInventory(param);
		}
		break;
	case SHORTCUTTYPE_ITEMCOMPOSE:
		{
			if(param->DestType == SHORTCUTTYPE_INVENTORY)
				return param->player->mItemComposeList.ItemComposeToInventory(param);
		}
		break;
	case SHORTCUTTYPE_REPAIR:
		{
			if(param->DestType != SHORTCUTTYPE_INVENTORY)
				return g_ItemManager->delFromRepairList(param->player);
			else
				return g_ItemManager->RepairListMoveToInventory(param);
		}
		break;
	default:
		break;
	}
	return msg;
}

#endif // NTJ_SERVER