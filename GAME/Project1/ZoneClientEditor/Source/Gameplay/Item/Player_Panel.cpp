//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "T3D/gameConnection.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameObjects/SkillShortcut.h"
#include "Gameplay/GameObjects/LivingSkillShortcut.h"
#include "Gameplay/Item/PetShortcut.h"
#include "Gameplay/Data/readDataFile.h"
#include "core/fileStream.h"

PanelList::PanelList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_PANEL;
#ifdef NTJ_CLIENT
	dMemset(&mUI, 0, MAXSLOTS * sizeof(U32));
#endif//NTJ_CLIENT
}

// ----------------------------------------------------------------------------
// �����ݿ������ҿ��������
bool PanelList::LoadData(stPlayerStruct* playerInfo,Player* player)
{
#ifdef NTJ_SERVER
	if(!player)
		return false;

	for(U32 i=0; i<mMaxSlots; ++i)
	{
		stShortCutInfo& temp = playerInfo->MainData.PanelInfo[i];
		if(temp.ShortCutID ==0)
			continue;
		
		if(temp.ShortCutType == ShortcutObject::SHORTCUT_ITEM)				//��Ʒ
		{
			mSlots[i] = ItemShortcut::CreateItem(temp.ShortCutID, temp.ShortCutNum, true);
		}
		
		else if(temp.ShortCutType == ShortcutObject::SHORTCUT_SKILL)		//����
		{
			Skill* pSkill = NULL;
			pSkill = player->GetSkillTable().GetSkill(Macro_GetSkillSeriesId(temp.ShortCutID));
			if(pSkill)
				mSlots[i] = SkillShortcut::CreateSkill(pSkill);
		}
		else if(temp.ShortCutType == ShortcutObject::SHORTCUT_LIVINGSKILL)	//�����
		{
			mSlots[i] = LivingSkillShortcut::Create(temp.ShortCutID,temp.ShortCutNum);
		}
		else if (temp.ShortCutType == ShortcutObject::SHORTCUT_PET)			//����
		{
			mSlots[i] = PetShortcut::CreatePetItem(temp.ShortCutID - 1);
		}
		else if (temp.ShortCutType == ShortcutObject::SHORTCUT_MOUNT_PET)	//���
		{
			mSlots[i] = MountShortcut::CreateMountItem(temp.ShortCutID - 1);
		}
	}
#endif
	return true;
}

// ----------------------------------------------------------------------------
// ���ͳ�ʼ���Ŀ�������ݵ��ͻ���
void PanelList::SendInitialData(GameConnection* conn)
{
	for (S32 i = 0; i <mMaxSlots; ++i)
	{
		if(mSlots[i])
		{
			PanelEvent* event = new PanelEvent(i);
			conn->postNetEvent(event);
		}
	}
}

// ----------------------------------------------------------------------------
// �洢��ҿ���������ݿ�
bool PanelList::SaveData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	for(U32 i = 0; i < mMaxSlots; ++i)
	{
		if(mSlots[i] == NULL)
			continue;
		ShortcutObject* pShortObject = mSlots[i];
		if(pShortObject)
		{
			stShortCutInfo* temp = &playerInfo->MainData.PanelInfo[i];
			if(pShortObject->isItemObject())			//��Ʒ
			{
				ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortObject);
				if(pItem && pItem->getRes())
				{
					temp->ShortCutID = pItem->getRes()->getItemID();
					temp->ShortCutNum = pItem->getRes()->getQuantity();//��Ʒ����
					temp->ShortCutType = ShortcutObject::SHORTCUT_ITEM;
				}
			}
			else if(pShortObject->isSkillObject())		//����
			{
				SkillShortcut* pSkill = dynamic_cast<SkillShortcut*>(pShortObject);
				if(pSkill && pSkill->getSkill())
				{
					temp->ShortCutID = pSkill->getSkill()->GetData()->GetID();
					temp->ShortCutNum = pSkill->getSkill()->GetBaseLevel();//���ܵȼ�
					temp->ShortCutType = ShortcutObject::SHORTCUT_SKILL;
				}
			}
			else if (pShortObject->isLivingSkillObject())//�����
			{
				LivingSkillShortcut* pSkill = dynamic_cast<LivingSkillShortcut*>(pShortObject);
				if (pSkill && pSkill->getLivingSkill() && pSkill->getLivingSkill()->getData())
				{
					temp->ShortCutID   = pSkill->getLivingSkill()->getData()->getID();
					temp->ShortCutNum  = pSkill->getLivingSkill()->getRipe();//������
					temp->ShortCutType = ShortcutObject::SHORTCUT_LIVINGSKILL;
				}
			}
			else if (pShortObject->isPetObject())		//����
			{
				PetShortcut *pPet = dynamic_cast<PetShortcut*>(pShortObject);
				if (pPet && pPet->getSlotIndex() != -1)
				{
					temp->ShortCutID = pPet->getSlotIndex() + 1;
					temp->ShortCutType = ShortcutObject::SHORTCUT_PET;
				}
			}
			else if (pShortObject->isMountObject())		//���
			{
				MountShortcut *pMount = dynamic_cast<MountShortcut*>(pShortObject);
				if (pMount && pMount->getSlotIndex() != -1)
				{
					temp->ShortCutID = pMount->getSlotIndex() + 1;
					temp->ShortCutType = ShortcutObject::SHORTCUT_MOUNT_PET;
				}
			}
		}
	}
#endif
	return true;
}

// ----------------------------------------------------------------------------
// ������ҿ�������ݵ��ͻ���
bool PanelList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	PanelEvent* ev = new PanelEvent(index);
	conn->postNetEvent(ev);
#endif
	return true;
}

#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// ���ر��ش洢�Ŀ������λ����
void PanelList::LoadCacheUI(Player* player)
{
	if(!player)
		return;	

	char szFilePath[MAX_PATH];
	dSprintf(szFilePath, MAX_PATH, GAME_PANELFILE, player->getPlayerName());
	char filename[256];
	Platform::makeFullPathName(szFilePath, filename, sizeof(filename));
	Stream* stream = ResourceManager->openStream(filename);
	if(stream)
	{
		for(S32 i = 0; i < mMaxSlots; i++)
		{
			stream->read(&mUI[i]);
		}
		stream->close();
	}
}

// ----------------------------------------------------------------------------
// �洢�������λ���ݵ�����cache
void PanelList::SaveCacheUI(Player* player)
{
	if(!player)
		return;
	char szFilePath[MAX_PATH];
	dSprintf(szFilePath, MAX_PATH, GAME_PANELFILE, player->getPlayerName());
	Platform::createPath(szFilePath);
	FileStream stream;
	if(ResourceManager->openFileForWrite(stream, szFilePath)) 
	{
		for(S32 i = 0; i < mMaxSlots; i++)
		{
			stream.write(&mUI[i]);
		}
		stream.close();
	}
}
#endif//NTJ_CLIENT

// ============================================================================
// ItemManager ��Ʒ������
// 
// ============================================================================
#ifdef NTJ_SERVER

// ----------------------------------------------------------------------------
// �ƶ������������һ�����λ��
enWarnMessage ItemManager::PanelMoveToPanel(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(!player->panelList.IsVaildSlot(param->SrcIndex) ||
		!player->panelList.IsVaildSlot(param->DestIndex))
		return msg;	

	ShortcutObject* pSrc	= player->panelList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->panelList.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;

	// �жϿ�����Ƿ�������
	if(pSrc && pSrc->isLocked())
		return MSG_ITEM_PANELLOCKED;

	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;
	player->panelList.SetSlot(param->DestIndex, pSrc, false);
	player->panelList.SetSlot(param->SrcIndex, pDest, false);
	
	player->panelList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
	player->panelList.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �����Ʒ/����/�����/����/�����󵽿����
enWarnMessage ItemManager::addObjectToPanel(Player* player, U32 type, U32 id, S32 index)
{
	if(!player)
		return MSG_INVALIDPARAM;
	ShortcutObject* pShort = NULL;
	if(type == ShortcutObject::SHORTCUT_ITEM)	// ��Ʒ
	{
		pShort = ItemShortcut::CreateItem(id, 1);
	}
	else if(type == ShortcutObject::SHORTCUT_SKILL)
	{
		Skill* pSkill = player->GetSkillTable().GetSkill(Macro_GetSkillSeriesId(id));
		if(pSkill)
			pShort = SkillShortcut::CreateSkill(pSkill);
	}
	else if(type == ShortcutObject::SHORTCUT_LIVINGSKILL)
	{
		pShort = LivingSkillShortcut::Create(id, 0);
	}
	else if(type == ShortcutObject::SHORTCUT_PET)
	{
		
	}
	else if(type == ShortcutObject::SHORTCUT_MOUNT_PET)
	{

	}
	enWarnMessage msgCode = addObjectToPanel(player, pShort, index);
	if(pShort)
		delete pShort;
	return msgCode;
}

// ----------------------------------------------------------------------------
// �����Ʒ/����/�����/����/�����󵽿����
enWarnMessage ItemManager::addObjectToPanel(Player* player, ShortcutObject* object, S32 index)
{
	if(!player || !object || index < 0 || index >= PanelList::MAXSLOTS)
		return MSG_INVALIDPARAM;

	if(object->isItemObject())					//��Ʒ����
	{
		ItemShortcut* pItem = (ItemShortcut*)object;
		// �ж��Ƿ�װ������Ϊװ�����ܱ��ϵ�������ϣ�
		if(pItem->getRes()->isEquip())
			return MSG_ITEM_CANNOTDRAGTOOBJECT;
		S32 iTotalNum = player->inventoryList.GetItemCount(pItem, false, false);
		if(iTotalNum <= 0)
			return MSG_ITEM_NOFINDITEM;
		ItemShortcut* pCloneItem = ItemShortcut::CreateItem(pItem);
		if(pCloneItem)
		{
			if(pCloneItem->getRes()->getMaxOverNum() > 1)
				pCloneItem->getRes()->setQuantity(iTotalNum, true);
			else
				pCloneItem->getRes()->setQuantity(1, true);
			player->panelList.SetSlot(index, pCloneItem);
			player->panelList.UpdateToClient(player->getControllingClient(), index, ITEM_NOSHOW);		
		}
	}
	else if(object->isSkillObject())			//���ܶ���
	{
		SkillShortcut* pSkillShort = (SkillShortcut*)object;
		Skill* pSkill = pSkillShort->getSkill();
		if(!pSkill)
			return MSG_ITEM_ITEMDATAERROR;
		//�ж��Ƿ��Ǳ������ܣ���Ϊ�������ܲ��ܱ��ϵ�������ϣ�
		if(!pSkill->CanDrag())
			return MSG_ITEM_CANNOTDRAGTOOBJECT;
		SkillShortcut* pCloneSkillShort = SkillShortcut::CreateSkill(pSkill);
		player->panelList.SetSlot(index, pCloneSkillShort);
		player->panelList.UpdateToClient(player->getControllingClient(), index, ITEM_NOSHOW);
	}
	else if(object->isPetObject())				//�������
	{
		//��Ҫ��ӵ�PetTable�󣬲�����ӵ�panel
	}
	else if(object->isLivingSkillObject())		//����ܶ���
	{
		LivingSkillShortcut* pSkillShort = (LivingSkillShortcut*)object;
		LivingSkill* pSkill = pSkillShort->getLivingSkill();
		LivingSkillData* pData = NULL;

		if(!pSkill || !(pData = pSkill->getData()))
			return MSG_ITEM_ITEMDATAERROR;

		U32 skillId = pData->getID();
		if(skillId < LIVINGSKILL_ID_CATEGORYLIMIT && skillId >= LIVINGSKILL_ID_LIMIT)
			return MSG_ITEM_ITEMDATAERROR;

		LivingSkillShortcut* pCloneSkillShort =  LivingSkillShortcut::Create(skillId,0);
		player->panelList.SetSlot(index,pCloneSkillShort);
		player->panelList.UpdateToClient(player->getControllingClient(), index, ITEM_NOSHOW);

	}
	else if(object->isMountObject())			//������
	{

	}
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ͬ���������Ʒ
// ע:����ƷIDΪ�ο����ڿ����������ͬ��ƷID��1��������λ��λ�ã�Ȼ����ݿ��
// ����λλ�õ�LockedItemIndexֵ������Ʒ����Ӧλ�õ���Ʒʵ����Ȼ���ٸ��ݴ���Ʒ
// ʵ������Ʒ�ǿۼ����������������о�����������¿������
// ����:���ڿ��������Ʒ��ʹ�ô���Ϊ0������Ϊ0ʱ�������������ϣ�UI��ʾΪǳ��ɫ
void ItemManager::syncPanel(Player* player, U32 itemid)
{
	if(!player || itemid <= 10000000)
		return;
	
	for(S32 i = 0; i < player->panelList.GetMaxSlots(); i++)
	{
		ShortcutObject* pSlot = player->panelList.GetSlot(i);
		// ���Է���Ʒ����
		if(!pSlot || !pSlot->isItemObject())
			continue;
		
		ItemShortcut* pItem = (ItemShortcut*)pSlot;
		// ���Է�ͬ����Ʒ����
		if(pItem->getRes()->getItemID() != itemid)
			continue;
		
		U64 uUID = pItem->getRes()->getUID();
		ItemShortcut* pSrcItem = (ItemShortcut*)player->inventoryList.FindSlot(uUID);
		if(pSrcItem)
		{
			// ����Ʒ����������ʹ�ô�����
			if(pSrcItem->getRes()->getResUsedTimes() > 0)
			{
				pItem->getRes()->setUsedTimes(pSrcItem->getRes()->getUsedTimes());
			}
			// ����Ʒ������������ӵ�
			else if(pSrcItem->getRes()->getMaxOverNum() > 1)
			{
				S32 iTotalNum = player->inventoryList.GetItemCount(pSrcItem, false, false);
				//S32 iTotalNum = pSrcItem->getRes()->getQuantity();
				pItem->getRes()->setQuantity(iTotalNum, true);
			}
			else
			{
				pItem = ItemShortcut::CreateItem(pSrcItem);
				if(pItem)
					player->panelList.SetSlot(i, pItem);
			}
		}
		else
		{
			bool bFind = false;
			// ����Ʒ������������ӵ�
			if(pItem->getRes()->getMaxOverNum() > 1)
			{
				S32 iTotalNum = player->inventoryList.GetItemCount(pItem->getRes()->getItemID(), false, false);
				if(iTotalNum > 0)
				{
					pItem->getRes()->setQuantity(iTotalNum, true);
					bFind = true;
				}
			}

			if(!bFind)
				player->panelList.SetSlot(i, NULL);
		}

		player->panelList.UpdateToClient(player->getControllingClient(), i, ITEM_NOSHOW);
	}
}
#endif