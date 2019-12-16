#include "Gameplay/Item/Res.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/item/Player_GemMount.h"
#include "Gameplay/GameObjects/SpiritOperationManager.h"

TempList::TempList() : BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_TEMP;
	m_optType = OptType_None;
}

void TempList::Clean(Player* pPlayer)
{
	GameConnection *conn = NULL;
#ifdef NTJ_SERVER
	conn = pPlayer->getControllingClient();
	if (!conn)
		return;

	for (S32 i = 0; i < MAXSLOTS; i++)
	{
		if (!mSlots[i])
			continue;

		S32 nLockedIndex = mSlots[i]->getLockedItemIndex();
		if (nLockedIndex == -1)
			continue;

		switch (mSlots[i]->getType())
		{
		case ShortcutObject::SHORTCUT_ITEM:
			{
				ItemShortcut *pItem = dynamic_cast<ItemShortcut*>( pPlayer->inventoryList.GetSlot(nLockedIndex) );
				if (pItem)
				{
					pItem->setSlotState(ShortcutObject::SLOT_COMMON);
					pPlayer->inventoryList.UpdateToClient(conn, nLockedIndex, ITEM_NOSHOW);
				}
			}
			break;
		case ShortcutObject::SHORTCUT_SKILL:
			{
				/*
				SkillShortcut *pSkill = dynamic_cast<SkillShortcut*>( pPlayer->skillList_Common.GetSlot(nLockedIndex) );
				if (pSkill)
				{
					pSkill->setSlotState(ShortcutObject::SLOT_COMMON);
					pPlayer->inventoryList.UpdateToClient(conn, )
				}
				*/
			}
			break;
		case ShortcutObject::SHORTCUT_PET:
			{
				PetShortcut *pPet = dynamic_cast<PetShortcut*>( pPlayer->petList.GetSlot(nLockedIndex) );
				if (pPet)
				{
					pPet->setSlotState(ShortcutObject::SLOT_COMMON);
					pPlayer->petList.UpdateToClient(conn, nLockedIndex, ITEM_NOSHOW);
				}
			}
			break;
		case ShortcutObject::SHORTCUT_MOUNT_PET:
			{
				MountShortcut *pMount = dynamic_cast<MountShortcut*>( pPlayer->mountPetList.GetSlot(nLockedIndex) );
				if (pMount)
				{
					pMount->setSlotState(ShortcutObject::SLOT_COMMON);
					pPlayer->mountPetList.UpdateToClient(conn, nLockedIndex, ITEM_NOSHOW);
				}
			}
			break;
		case ShortcutObject::SHORTCUT_LIVINGSKILL:
			{
				
			}
			break;
		}
	}
#endif

	BaseItemList::Reset();
	SetOptType(conn, OptType_None);

#ifdef NTJ_SERVER
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TEMPLIST);
	if (!ev)
		return;
	ev->SetIntArgValues(2, 2);
	conn->postNetEvent(ev);
#endif
}

//服务端调用SetOptType(pPlayer.getControllingClient(), type);
//客户端调用SetOptType(NULL, type);
bool TempList::SetOptType(GameConnection* conn, OptType type)
{
	if (mType == type)
		return false;
	m_optType = type;
#ifdef NTJ_SERVER
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TEMPLIST);
	if (!ev || !conn)
		return false;
	ev->SetIntArgValues(2, 1, m_optType);
	conn->postNetEvent(ev);
#endif
	return true;
}

bool TempList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	TempListEvent* ev = new TempListEvent(index, flag);
	conn->postNetEvent(ev);
#endif
	return true;
}

#ifdef NTJ_SERVER
enWarnMessage TempList::TempListMoveToTempList(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = param->player;
	if (!pPlayer)
		return msg;

	switch(pPlayer->tempList.GetOptType())
	{
	case OptType_None:
		break;
	default:
		break;
	}

	return msg;
}

enWarnMessage TempList::TempListMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = param->player;
	if (!pPlayer)
		return msg;

	switch(pPlayer->tempList.GetOptType())
	{
	case OptType_None:
		break;
	case OptType_PetInsight:
		{
			return PetInsighMoveToInventory(param);
		}
	case OptType_PetHuanTong:
		{
			return PetHuanTongMoveToInventory(param);
		}
	case OptType_SpiritSkill:
		{
			return SpiritSkillMoveToInventory(param);
		}
	default:
		break;
	}
	return msg;
}

enWarnMessage TempList::InventoryMoveToTempList(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = param->player;
	if (!pPlayer)
		return msg;
	ShortcutObject *pShortcut = pPlayer->inventoryList.GetSlot(param->SrcIndex);
	if (!pShortcut || pShortcut->getSlotState() == ShortcutObject::SLOT_LOCK)
		return msg;

	switch(pPlayer->tempList.GetOptType())
	{
	case OptType_None:
		break;
	case OptType_PetInsight:
		{
			return InventoryMoveToPetInsight(param);
		}
		break;
	case OptType_PetHuanTong:
		{
			return InventoryMoveToPetHuanTong(param);
		}
	case OptType_SpiritSkill:
		{
			return InventoryMoveToSpiritSkill(param);
		}
	default:
		break;
	}
	return msg;
}

enWarnMessage TempList::EquipMoveToTempList(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	return msg;
}

enWarnMessage TempList::TempListMoveToEquip(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	return msg;
}

enWarnMessage TempList::PetInsighMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = param->player;

	if (param->SrcIndex != 0 || !pPlayer)
		return msg;

	ItemShortcut *pItemSrc = dynamic_cast<ItemShortcut *>(pPlayer->tempList.GetSlot(param->SrcIndex));
	if (pItemSrc)
	{
		U32 nLockIndex = pItemSrc->getLockedItemIndex();
		ShortcutObject *pShortcutObject = pPlayer->inventoryList.GetSlot(nLockIndex);
		pShortcutObject->setSlotState(ShortcutObject::SLOT_COMMON);
		pPlayer->tempList.SetSlot(0, NULL);
		pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), nLockIndex, ITEM_NOSHOW);
		pPlayer->tempList.UpdateToClient(pPlayer->getControllingClient(), 0, ITEM_NOSHOW);
	}

	return MSG_NONE;
}

enWarnMessage TempList::InventoryMoveToPetInsight(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = param->player;

	if (param->DestIndex != 0 || !pPlayer || !pPlayer->inventoryList.IsVaildSlot(param->SrcIndex))
		return msg;

	ItemShortcut *pItemSrc = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(param->SrcIndex));
	ItemShortcut *pItemDest = dynamic_cast<ItemShortcut *>(pPlayer->tempList.GetSlot(param->DestIndex));
	if (!pItemSrc)
		return msg;

	//检查
	PetTable table = pPlayer->getPetTable();
	msg = CheckInventoryMoveToPetInsight(pPlayer->getPetTable(), 0, pItemSrc);
	if (msg != MSG_NONE)
		return msg;

	if (pItemDest)
	{
		S32 nLockedIndex = pItemDest->getLockedItemIndex();
		if (nLockedIndex != -1)
		{
			ShortcutObject *pShortcutInventory = pPlayer->inventoryList.GetSlot(nLockedIndex);
			pShortcutInventory->setSlotState(ShortcutObject::SLOT_COMMON);
			pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), nLockedIndex, ITEM_NOSHOW);
		}
		pPlayer->tempList.SetSlot(0, NULL);
	}
	
	pItemDest = ItemShortcut::CreateItem(pItemSrc);
	if (!pItemDest)
		return MSG_UNKOWNERROR;

	pItemDest->setLockedItemIndex(param->SrcIndex);
	pItemSrc->setSlotState(ShortcutObject::SLOT_LOCK);
	pPlayer->tempList.SetSlot(0, pItemDest);
	pPlayer->tempList.UpdateToClient(pPlayer->getControllingClient(), 0, ITEM_NOSHOW);
	pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);

	return MSG_NONE;
}

enWarnMessage TempList::PetHuanTongMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = param->player;

	if (param->SrcIndex != 0 || !pPlayer)
		return msg;

	ItemShortcut *pItemSrc = dynamic_cast<ItemShortcut *>(pPlayer->tempList.GetSlot(param->SrcIndex));
	if (pItemSrc)
	{
		U32 nLockIndex = pItemSrc->getLockedItemIndex();
		ShortcutObject *pShortcutObject = pPlayer->inventoryList.GetSlot(nLockIndex);
		pShortcutObject->setSlotState(ShortcutObject::SLOT_COMMON);
		pPlayer->tempList.SetSlot(0, NULL);
		pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), nLockIndex, ITEM_NOSHOW);
		pPlayer->tempList.UpdateToClient(pPlayer->getControllingClient(), 0, ITEM_NOSHOW);
	}

	return MSG_NONE;
}

enWarnMessage TempList::InventoryMoveToPetHuanTong(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = param->player;

	if (param->DestIndex != 1 || !pPlayer || !pPlayer->inventoryList.IsVaildSlot(param->SrcIndex))
		return msg;

	ItemShortcut *pItemSrc = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(param->SrcIndex));
	ItemShortcut *pItemDest = dynamic_cast<ItemShortcut *>(pPlayer->tempList.GetSlot(param->DestIndex));
	if (!pItemSrc)
		return msg;

	PetTable table = pPlayer->getPetTable();
	msg = CheckInventoryMoveToPetHuanTong(pPlayer->getPetTable(), -1, pItemSrc);
	if (msg != MSG_NONE)
		return msg;

	if (pItemDest)
	{
		S32 nLockedIndex = pItemDest->getLockedItemIndex();
		if (nLockedIndex != -1)
		{
			ShortcutObject *pShortcutInventory = pPlayer->inventoryList.GetSlot(nLockedIndex);
			pShortcutInventory->setSlotState(ShortcutObject::SLOT_COMMON);
			pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), nLockedIndex, ITEM_NOSHOW);
		}
		pPlayer->tempList.SetSlot(1, NULL);
	}

	pItemDest = ItemShortcut::CreateItem(pItemSrc);
	if (!pItemDest)
		return MSG_UNKOWNERROR;

	pItemDest->setLockedItemIndex(param->SrcIndex);
	pItemSrc->setSlotState(ShortcutObject::SLOT_LOCK);
	pPlayer->tempList.SetSlot(1, pItemDest);
	pPlayer->tempList.UpdateToClient(pPlayer->getControllingClient(), 1, ITEM_NOSHOW);
	pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);

	return MSG_NONE;
}
#endif

#ifdef NTJ_CLIENT
enWarnMessage TempList::CheckInventoryMoveToTempList(Player *pPlayer, ItemShortcut *pSrcItem, S32 nDestIndex)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	if (!pPlayer)
		return msg;

	U32 nOptType = (U32)pPlayer->tempList.GetOptType();
	switch (nOptType)
	{
	case OptType_PetInsight:
		{
			U32 nSelectPetSlot = Con::getIntVariable("$PetInfoWnd_PetSlot");
			return CheckInventoryMoveToPetInsight(pPlayer->getPetTable(), nSelectPetSlot, pSrcItem);
		}
	case OptType_PetHuanTong:
		{
			U32 nSelectPetSlot = Con::getIntVariable("$PetInfoWnd_PetSlot");
			return CheckInventoryMoveToPetHuanTong(pPlayer->getPetTable(), nSelectPetSlot, pSrcItem);
		}
	case OptType_EquipStrengthen:
		{
			return EquipStrengthen::checkMoveFromInventory(pPlayer, pSrcItem, nDestIndex);
		}
	case OptType_EquipGemEmbed:
		{
			return EquipMountGem::checkMoveFromInventory(pPlayer, pSrcItem, nDestIndex);
		}
	case OptType_EquipPunchHole:
		{
			return EquipPunchHole::checkMoveFromInventory(pPlayer, pSrcItem, nDestIndex);
		}
	case OptType_SpiritSkill:
		{
			return CheckInventoryMoveToSpiritSkill(pPlayer, pSrcItem, nDestIndex);
		}
	default:
		break;
	}
	return msg;
}
#endif

//Server: 拖动时 nSlot设置-1， 操作时客户端传入nSlot值
//Client: 拖动和操作时都传入nSlot值
enWarnMessage TempList::CheckInventoryMoveToPetInsight(const PetTable &table, U32 nSlot, ItemShortcut *pGenGuItem)
{
	if (nSlot >= PET_MAXSLOTS)
		return MSG_UNKOWNERROR;

	if (!pGenGuItem || !pGenGuItem->getRes())
		return MSG_ITEM_ITEMDATAERROR;

	Res *pGenGuRes = pGenGuItem->getRes();
	if (!pGenGuRes->isGenGuDan())
		return MSG_PET_ITEM_NOT_GENGUDAN;

#ifdef NTJ_CLIENT
	stPetInfo *pPetInfo = (stPetInfo *)table.getPetInfo(nSlot);
	if (pPetInfo)
		return MSG_UNKOWNERROR;

	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;

	if (pPetInfo->insight == 10)
		return MSG_PET_MAXIMUM_INSIGHT;
#endif

	return MSG_NONE;
}

enWarnMessage TempList::CheckInventoryMoveToPetHuanTong(const PetTable &table, U32 nSlot, ItemShortcut *pHuanTongDan)
{
	if (!pHuanTongDan || !pHuanTongDan->getRes())
		return MSG_ITEM_ITEMDATAERROR;

	Res *pHuanTongDanRes = pHuanTongDan->getRes();
	if (!pHuanTongDanRes->isHuanTongDan())
		return MSG_PET_ITEM_NOT_HUANTONG;

#ifdef NTJ_CLIENT
	if (nSlot >= PET_MAXSLOTS)
		return MSG_UNKOWNERROR;
	const stPetInfo* pPetInfo = table.getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
#endif

	return MSG_NONE;
}

enWarnMessage TempList::CheckInventoryMoveToSpiritSkill(Player *pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	if( pPlayer == NULL || nDestIndex != 0 )		//只有装备栏位可以拖动
		return msg;
	if (pSrcItem == NULL)
		return MSG_ITEM_ITEMDATAERROR;
	Res *pSrcRes = pSrcItem->getRes();
	if (pSrcRes == NULL)
		return MSG_ITEM_ITEMDATAERROR;
	if (!pSrcRes->isSpiritStone1() && !pSrcRes->isSpiritStone2() && !pSrcRes->isSkillBook())
		return msg;
	return MSG_NONE;
}

#ifdef NTJ_SERVER
enWarnMessage TempList::PetListMoveToPetIdentify(stExChangeParam* param)
{
	if (param->DestIndex != 0)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	PetShortcut *pPetShortcut = dynamic_cast<PetShortcut*>(param->player->petList.GetSlot(param->SrcIndex));
	if (!pPetShortcut)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	stPetInfo *pPetInfo = (stPetInfo*)param->player->getPetTable().getPetInfo(pPetShortcut->getSlotIndex());
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	PetShortcut *pNewSlot = PetShortcut::CreatePetItem(pPetShortcut->getSlotIndex());
	if (!pNewSlot)
		return MSG_UNKOWNERROR;

	PetShortcut *pDestSlot = dynamic_cast<PetShortcut*>(param->player->tempList.GetSlot(param->DestIndex));
	if (pDestSlot)
	{
		//释放已有槽的锁定
		S32 nLockedIndex = pDestSlot->getLockedItemIndex();
		if (nLockedIndex != -1)
		{
			PetShortcut *pLockedSlot = dynamic_cast<PetShortcut*>(param->player->petList.GetSlot(nLockedIndex));
			if (pLockedSlot)
			{
				pLockedSlot->setSlotState(ShortcutObject::SLOT_COMMON);
				param->player->petList.UpdateToClient(param->player->getControllingClient(), nLockedIndex, ITEM_NOSHOW);
			}			
		}
	}
	pPetShortcut->setSlotState(ShortcutObject::SLOT_LOCK);
	pNewSlot->setLockedItemIndex(param->SrcIndex);
	param->player->tempList.SetSlot(param->DestIndex, pNewSlot);
	param->player->petList.UpdateToClient(param->player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	param->player->tempList.UpdateToClient(param->player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);

	return MSG_NONE;
}

enWarnMessage TempList::PetListMoveToPetInsight(stExChangeParam* param)
{
	if (param->DestIndex != 0)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	PetShortcut *pPetShortcut = dynamic_cast<PetShortcut*>(param->player->petList.GetSlot(param->SrcIndex));
	if (!pPetShortcut)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	stPetInfo *pPetInfo = (stPetInfo*)param->player->getPetTable().getPetInfo(pPetShortcut->getSlotIndex());
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	PetShortcut *pNewSlot = PetShortcut::CreatePetItem(pPetShortcut->getSlotIndex());
	if (!pNewSlot)
		return MSG_UNKOWNERROR;

	PetShortcut *pDestSlot = dynamic_cast<PetShortcut*>(param->player->tempList.GetSlot(param->DestIndex));
	if (pDestSlot)
	{
		//释放已有槽的锁定
		S32 nLockedIndex = pDestSlot->getLockedItemIndex();
		if (nLockedIndex != -1)
		{
			PetShortcut *pLockedSlot = dynamic_cast<PetShortcut*>(param->player->petList.GetSlot(nLockedIndex));
			if (pLockedSlot)
			{
				pLockedSlot->setSlotState(ShortcutObject::SLOT_COMMON);
				param->player->petList.UpdateToClient(param->player->getControllingClient(), nLockedIndex, ITEM_NOSHOW);
			}			
		}
	}
	pPetShortcut->setSlotState(ShortcutObject::SLOT_LOCK);
	pNewSlot->setLockedItemIndex(param->SrcIndex);
	param->player->tempList.SetSlot(param->DestIndex, pNewSlot);
	param->player->petList.UpdateToClient(param->player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	param->player->tempList.UpdateToClient(param->player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

enWarnMessage TempList::PetListMoveToPetLianHua(stExChangeParam* param)
{
	if (param->DestIndex != 0)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	PetShortcut *pPetShortcut = dynamic_cast<PetShortcut*>(param->player->petList.GetSlot(param->SrcIndex));
	if (!pPetShortcut)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	stPetInfo *pPetInfo = (stPetInfo*)param->player->getPetTable().getPetInfo(pPetShortcut->getSlotIndex());
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	PetShortcut *pNewSlot = PetShortcut::CreatePetItem(pPetShortcut->getSlotIndex());
	if (!pNewSlot)
		return MSG_UNKOWNERROR;

	PetShortcut *pDestSlot = dynamic_cast<PetShortcut*>(param->player->tempList.GetSlot(param->DestIndex));
	if (pDestSlot)
	{
		//释放已有槽的锁定
		S32 nLockedIndex = pDestSlot->getLockedItemIndex();
		if (nLockedIndex != -1)
		{
			PetShortcut *pLockedSlot = dynamic_cast<PetShortcut*>(param->player->petList.GetSlot(nLockedIndex));
			if (pLockedSlot)
			{
				pLockedSlot->setSlotState(ShortcutObject::SLOT_COMMON);
				param->player->petList.UpdateToClient(param->player->getControllingClient(), nLockedIndex, ITEM_NOSHOW);
			}			
		}
	}
	pPetShortcut->setSlotState(ShortcutObject::SLOT_LOCK);
	pNewSlot->setLockedItemIndex(param->SrcIndex);
	param->player->tempList.SetSlot(param->DestIndex, pNewSlot);
	param->player->petList.UpdateToClient(param->player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	param->player->tempList.UpdateToClient(param->player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

enWarnMessage TempList::PetListMoveToPetHuanTong(stExChangeParam* param)
{
	if (param->DestIndex != 0)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	PetShortcut *pPetShortcut = dynamic_cast<PetShortcut*>(param->player->petList.GetSlot(param->SrcIndex));
	if (!pPetShortcut)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	stPetInfo *pPetInfo = (stPetInfo*)param->player->getPetTable().getPetInfo(pPetShortcut->getSlotIndex());
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	PetShortcut *pNewSlot = PetShortcut::CreatePetItem(pPetShortcut->getSlotIndex());
	if (!pNewSlot)
		return MSG_UNKOWNERROR;

	PetShortcut *pDestSlot = dynamic_cast<PetShortcut*>(param->player->tempList.GetSlot(param->DestIndex));
	if (pDestSlot)
	{
		//释放已有槽的锁定
		S32 nLockedIndex = pDestSlot->getLockedItemIndex();
		if (nLockedIndex != -1)
		{
			PetShortcut *pLockedSlot = dynamic_cast<PetShortcut*>(param->player->petList.GetSlot(nLockedIndex));
			if (pLockedSlot)
			{
				pLockedSlot->setSlotState(ShortcutObject::SLOT_COMMON);
				param->player->petList.UpdateToClient(param->player->getControllingClient(), nLockedIndex, ITEM_NOSHOW);
			}			
		}
	}
	pPetShortcut->setSlotState(ShortcutObject::SLOT_LOCK);
	pNewSlot->setLockedItemIndex(param->SrcIndex);
	param->player->tempList.SetSlot(param->DestIndex, pNewSlot);
	param->player->petList.UpdateToClient(param->player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	param->player->tempList.UpdateToClient(param->player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

enWarnMessage TempList::SpiritSkillMoveToInventory(stExChangeParam* param)
{
	Player *pPlayer = NULL;
	if (!param || param->SrcIndex != 0 || !(pPlayer = param->player) || 
		!pPlayer->inventoryList.IsVaildSlot(param->DestIndex))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	ItemShortcut *pSrcItem = dynamic_cast<ItemShortcut*>( pPlayer->tempList.GetSlot(param->SrcIndex) );
	if (pSrcItem == NULL)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	S32 nLockIndex = pSrcItem->getLockedItemIndex();
	GameConnection *conn = pPlayer->getControllingClient();
	ShortcutObject *pOldItem = pPlayer->inventoryList.GetSlot(nLockIndex);
	if (pOldItem)
	{
		pOldItem->setSlotState(ShortcutObject::SLOT_COMMON);
		pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
	}
	pPlayer->tempList.SetSlot(0, NULL);
	pPlayer->tempList.UpdateToClient(conn, 0, ITEM_NOSHOW);

	return MSG_NONE;
}

enWarnMessage TempList::InventoryMoveToSpiritSkill(stExChangeParam* param)
{
	Player *pPlayer = NULL;
	if (!param || param->DestIndex != 0 || !(pPlayer = param->player) || 
		!pPlayer->inventoryList.IsVaildSlot(param->SrcIndex))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	ItemShortcut *pSrcItem = dynamic_cast<ItemShortcut*>( pPlayer->inventoryList.GetSlot(param->SrcIndex) );
	enWarnMessage msg = CheckInventoryMoveToSpiritSkill(pPlayer, pSrcItem, param->DestIndex);
	if (msg != NULL)
		return msg;

	ItemShortcut *pDestItem = dynamic_cast<ItemShortcut*>( pPlayer->tempList.GetSlot(param->DestIndex) );
	GameConnection *conn = pPlayer->getControllingClient();
	if (pDestItem)
	{
		S32 nLockIndex = pDestItem->getLockedItemIndex();
		ShortcutObject *pOldItem = pPlayer->inventoryList.GetSlot(nLockIndex);
		if (pOldItem)
		{
			pOldItem->setSlotState(ShortcutObject::SLOT_COMMON);
			pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
		}
	}

	ItemShortcut *pNewItem = ItemShortcut::CreateItem(pSrcItem->getRes()->getItemID(), 1);
	if (!pNewItem)
		return MSG_UNKOWNERROR;
	pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);
	pNewItem->setLockedItemIndex(param->SrcIndex);
	pPlayer->tempList.SetSlot(0, pNewItem);
	pPlayer->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
	pPlayer->tempList.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);

	return MSG_NONE;
}
#endif