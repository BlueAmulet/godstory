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
#endif

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayAction.h"
#include "Gameplay/ClientGameplayState.h"
#endif

MissionSubmitList::MissionSubmitList() :BaseItemList(MAXSLOT)
{
	mType = SHORTCUTTYPE_MISSIONSUBMITLIST;
}

bool MissionSubmitList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
#endif
	return true;
}

#ifdef NTJ_SERVER

// ----------------------------------------------------------------------------
// 物品栏换物品到任务提交物品栏
enWarnMessage ItemManager::InventoryMoveToMissionSubmit(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_PLAYER_TARGET_INVALID;

	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
		!player->missionSubmitList.IsVaildSlot(param->DestIndex))
		return msg;
	if(param->DestIndex != 0)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	ItemShortcut* pSrcItem	= (ItemShortcut*)player->inventoryList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->missionSubmitList.GetSlot(param->DestIndex);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;	

	//若目标存在物品对象，需先解除源位置锁定
	if(pDest && pDest->isItemObject())
	{
		U32 iSrcIndex = pDest->getLockedItemIndex();
		ItemShortcut* pTempItem = (ItemShortcut*)player->inventoryList.GetSlot(iSrcIndex);
		if(!pTempItem)
			return MSG_ITEM_ITEMDATAERROR;
		pTempItem->setSlotState(ShortcutObject::SLOT_COMMON);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}

	ItemShortcut* pTargetItem = ItemShortcut::CreateItem(pSrcItem);
	if(!pTargetItem)
		return MSG_ITEM_ITEMDATAERROR;
	pTargetItem->setLockedItemIndex(param->SrcIndex);
	pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);
	player->missionSubmitList.SetSlot(param->DestIndex, pTargetItem);
	player->missionSubmitList.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// 宠物栏换宠物到任务提交物品栏
enWarnMessage ItemManager::PetListMoveToMissionSubmit(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAG;
	Player* player = param->player;
	if (!param || !player || !player->petList.IsVaildSlot(param->SrcIndex) || 
		!player->missionSubmitList.IsVaildSlot(param->DestIndex))
		return msg;

	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;

	PetShortcut *pSrcPetShortcut = dynamic_cast<PetShortcut*>(player->petList.GetSlot(param->SrcIndex));
	ShortcutObject* pDest =	player->missionSubmitList.GetSlot(param->DestIndex);
	if (!pSrcPetShortcut)
		return msg;

	stPetInfo *pPetInfoSrc = (stPetInfo *)player->getPetTable().getPetInfo(pSrcPetShortcut->getSlotIndex());
	if (!pPetInfoSrc)
		return MSG_NONE;

	if (pPetInfoSrc->status != PetObject::PetStatus_Freeze)
		return MSG_PET_STATUS_NOT_IDLE;
	
	//若目标存在物品对象，需先解除源位置锁定
	if(pDest && pDest->isItemObject())
	{
		U32 iSrcIndex = pDest->getLockedItemIndex();
		ItemShortcut* pTempItem = (ItemShortcut*)player->inventoryList.GetSlot(iSrcIndex);
		if(!pTempItem)
			return MSG_ITEM_ITEMDATAERROR;
		pTempItem->setSlotState(ShortcutObject::SLOT_COMMON);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}	

	PetShortcut *pNewShortcut = PetShortcut::CreatePetItem(param->SrcIndex);
	if(!pNewShortcut)
		return MSG_UNKOWNERROR;

	player->missionSubmitList.SetSlot(param->DestIndex, pNewShortcut);
	player->missionSubmitList.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
	return MSG_NONE;
}



#endif//NTJ_SERVER