#include "Gameplay/Item/PetShortcut.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/PetObjectData.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/PetMountOperation.h"
#include "Gameplay/GameObjects/TimerTrigger.h"

PetList::PetList() : BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_PET;
}

bool PetList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
	PetListEvent *ev = new PetListEvent(index, flag);
	if (ev && conn)
	{
		conn->postNetEvent(ev);
		return true;
	}
	return false;
}

void PetList::Initialize()
{
	for (S32 i = 0; i < MAXSLOTS; i++)
	{		
		PetShortcut *pPetShortcut = PetShortcut::CreatePetItem(i);
		SetSlot(i, pPetShortcut);
	}
}

void MountPetList::Initialize()
{
	for (S32 i = 0; i < MAXSLOTS; i++)
	{		
		MountShortcut *pMountShortcut = MountShortcut::CreateMountItem(i);
		SetSlot(i, pMountShortcut);
	}
}

MountPetList::MountPetList() : BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_MOUNT_PET_LIST;
}

bool MountPetList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
	MountPetListEvent *ev = new MountPetListEvent(index, flag);
	if (ev && conn)
	{
		conn->postNetEvent(ev);
		return true;
	}
	return false;
}
/////////////////////////////////////////
IMPLEMENT_CO_CLIENTEVENT_V1(PetListEvent);
PetListEvent::PetListEvent()
{
	mIndex = 0;
	mFlag = 0;
}

PetListEvent::PetListEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void PetListEvent::packData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (_conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	ShortcutObject *pShortcut = pPlayer->petList.GetSlot(mIndex);
	PetShortcut *pPetShortcut = NULL;
	valid = (pShortcut && (pPetShortcut = dynamic_cast<PetShortcut *>(pShortcut)));
	if (!stream->writeFlag(valid))
		return;
	pPetShortcut->packUpdate(stream);
#endif
}

void PetListEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;
	mIndex = stream->readInt(Base::Bit8);
	mFlag = stream->readInt(Base::Bit8);

	if(stream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->petList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			PetShortcut* pShortcut = PetShortcut::CreateEmptyPetItem();
			pShortcut->unpackUpdate(stream);
			pPlayer->petList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		pPlayer->petList.SetSlot(mIndex, NULL);
	}
#endif
}



///////////////////////////////////////////////
/////////////////////////////////////////
IMPLEMENT_CO_CLIENTEVENT_V1(MountPetListEvent);
MountPetListEvent::MountPetListEvent()
{
	mIndex = 0;
	mFlag = 0;
}

MountPetListEvent::MountPetListEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void MountPetListEvent::packData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (_conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	ShortcutObject *pShortcut = pPlayer->mountPetList.GetSlot(mIndex);
	MountShortcut *pMountShortcut = NULL;
	valid = (pShortcut && (pMountShortcut = dynamic_cast<MountShortcut *>(pShortcut)));
	if (!stream->writeFlag(valid))
		return;
	pMountShortcut->packUpdate(stream);
#endif
}

void MountPetListEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;
	mIndex = stream->readInt(Base::Bit8);
	mFlag = stream->readInt(Base::Bit8);

	if(stream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->mountPetList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			MountShortcut* pShortcut = MountShortcut::CreateEmptyMountItem();
			pShortcut->unpackUpdate(stream);
			pPlayer->mountPetList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		pPlayer->mountPetList.SetSlot(mIndex, NULL);
	}
#endif
}

#ifdef NTJ_CLIENT
bool ItemManager::insertText(Player* pPlayer, stPetInfo* pPetInfo, char* buff,S32 mSlotType,S32 mSlotIndex)
{
	if(!pPlayer || !pPetInfo)
		return false;
	
	char tempbuffer[1024] = "\n";

	if (pPetInfo->petDataId == 0)
		return false;

	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);

	if (pPetInfo->name && pPetInfo->name[0])
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t m='0' h='1' f='楷体_GB2312' n='16' c='0xffffffff'>灵兽:%s</t>", pPetInfo->name);
		dStrcat(buff,4096, tempbuffer);
	}   
	
	char* mPetStyle[4] = {"灵兽宝宝","变异宝宝","灵兽","变异灵兽",};
	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c = '0xf28226ff'>%s</t>", mPetStyle[pPetInfo->style-1]);
	dStrcat(buff, 4096,tempbuffer);
	
	if (pPetInfo->generation ==0)
		dStrcat(buff, 4096,"<t h='2' c= '0xffffffff'>一代            </t>");
	else if(pPetInfo->generation ==1)
		dStrcat(buff, 4096,"<t h='2' c= '0xffffffff'>二代            </t>");
	else if(pPetInfo->generation ==2)
		dStrcat(buff, 4096,"<t h='2' c= '0xffffffff'>三代            </t>");

	if (pPetInfo->level)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t  c='0xffffffff'>灵兽等级:%d级</t>", pPetInfo->level);
		dStrcat(buff,4096, tempbuffer);
	}

	if (pPetInfo->sex)
		dStrcat(buff, 4096,"<t h='2' c= '0xffffffff'>雄性            </t>");
	else
		dStrcat(buff, 4096,"<t h='2' c= '0xffffffff'>雌性            </t>");

	char* mPetType[9] = {"圣","佛","仙","精","鬼","怪","妖","魔","神",};
	dSprintf(tempbuffer, sizeof(tempbuffer), "<t  c='0xffffffff'>种族:%s</t><b/>", mPetType[pPetData->race]);
    dStrcat(buff,4096, tempbuffer);

	if (pPetData->spawnLevel>pPlayer->getLevel())
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t  c='0xff1200ff'>携带等级:%d级</t><b/>", pPetData->spawnLevel);
		dStrcat(buff,4096, tempbuffer);
	}
	else
	{
        dSprintf(tempbuffer, sizeof(tempbuffer), "<t  c='0xffffffff'>携带等级:%d级</t><b/>", pPetData->spawnLevel);
		dStrcat(buff,4096, tempbuffer);
	}
	dStrcat(buff,4096,"<i s='gameres/gui/images/GUIWindow99_1_007' />");

	//灵兽鉴定
	if (pPetInfo->chengZhangLv == 0)
	{
      dSprintf(tempbuffer, sizeof(tempbuffer), "<t c= '0xff1200ff'>成长率未鉴定</t><b/>");
	}
	else if (pPetInfo->chengZhangLv >= 18 && pPetInfo->chengZhangLv < 840)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c= '0xffffffff'>成长率:%d(普通) </t><b/>", pPetInfo->chengZhangLv);
		
	}
	else if (pPetInfo->chengZhangLv >= 840 && pPetInfo->chengZhangLv < 1560)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c= '0xffffffff'>成长率:%d(精良) </t><b/>", pPetInfo->chengZhangLv);
	}
	else if (pPetInfo->chengZhangLv >= 1560 && pPetInfo->chengZhangLv < 2220)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c= '0xffffffff'>成长率:%d(优秀) </t><b/>", pPetInfo->chengZhangLv);
	}
	else if (pPetInfo->chengZhangLv >= 2220 && pPetInfo->chengZhangLv < 2860)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c= '0xffffffff'>成长率:%d(完美) </t><b/>", pPetInfo->chengZhangLv);
	}
	else if (pPetInfo->chengZhangLv >= 2860 && pPetInfo->chengZhangLv < 3300)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t c= '0xffffffff'>成长率:%d(传说) </t><b/>", pPetInfo->chengZhangLv);
	}
	else if (pPetInfo->chengZhangLv >= 3300)
	{
	  dSprintf(tempbuffer, sizeof(tempbuffer), "<t c= '0xffffffff'>成长率:%d(逆天) </t><b/>", pPetInfo->chengZhangLv);
	}
   dStrcat(buff,4096,tempbuffer);

	if (pPetInfo->message[0] == 0)
	{
      dStrcat(buff,4096,"<t c='0xffffffff'>此主人很懒，什么也没留下!</t><b/>");
	}
	else
	{
      dSprintf(tempbuffer, sizeof(tempbuffer), "<t c= '0xffffffff'>%s</t><b/>", pPetInfo->message);
	   dStrcat(buff,4096,tempbuffer);
	}
	return true;
}

bool ItemManager::insertText(Player* pPlayer, stMountInfo* pMountInfo, char* buff,S32 mSlotType,S32 mSlotIndex)
{
	if(!pPlayer || !pMountInfo)
		return false;

	char tempbuffer[1024] = "\n";

	if (pMountInfo->mDataId == 0)
		return false;

	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (pMountData->mName && pMountData->mName[0])
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t m='0' h='1' f='楷体_GB2312' n='16' c='0xffffffff'>骑宠:%s</t>", pMountData->mName);
		dStrcat(buff,4096, tempbuffer);
	}   
 
    if (pMountData->mType == 1)
    {
      dStrcat(buff,4096, "<t c='0xf28226ff'>旅行宠</t><b/>");
    }
	else
	{
      dStrcat(buff,4096, "<t c='0xf28226ff'>战斗宠</t><b/>");
	}
    
	char* mMountType[9] = {"圣","佛","仙","精","鬼","怪","妖","魔","神",};
	
	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>种族:%s</t><b/>", mMountType[pMountData->mRace]);
    dStrcat(buff,4096, tempbuffer);

	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>可骑乘人数:%d人</t><b/>", pMountData->mCount);
	dStrcat(buff, 4096,tempbuffer);
    
	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>骑乘等级:%d级</t><b/>",pMountData->mMountLevel);
	dStrcat(buff,4096, tempbuffer);

	dStrcat(buff,4096,"<i s='gameres/gui/images/GUIWindow99_1_007' />");

	for (int i=0;i<pMountData->getCurrLevel();i++)
	{
       dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIbutton03_1_014' />");
	}

	for (int i=0;i<9-pMountData->getCurrLevel();i++)
	{
		dStrcat(buff, 4096, "<i s='gameres/gui/images/GUIbutton03_1_013' />");
	}

    dStrcat(buff,4096, "<b/>");

	if(pMountData->mTimeFlag == 0 )
	{
       dStrcat(buff,4096, "<t c='0x00B0F0ff'>(可永久使用)</t><b/>");
	}
	else
	{
		
	}

	S32 nSpeedupPercent = 0;
	Stats *pStats = g_BuffRepository.GetBuffStats(pMountData->buffId);
	if (!pStats)
		nSpeedupPercent = 0;
	else
		nSpeedupPercent = pStats->MountedVelocity_gPc;

	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0x46ff15ff'>骑乘后移动速度+%d%%</t><b/>", (nSpeedupPercent / 100) );
	dStrcat(buff,4096, tempbuffer);

	dSprintf(tempbuffer, sizeof(tempbuffer), "<t c='0xffffffff'>%s</t><b/>",pMountData->mDescription);
	dStrcat(buff,4096, tempbuffer);
	
	return true;
}
#endif

#ifdef NTJ_SERVER
enWarnMessage ItemManager::PetListMoveToPanel(stExChangeParam *param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAG;

	Player *pPlayer = param->player;
	if (!pPlayer || !pPlayer->petList.IsVaildSlot(param->SrcIndex) || !pPlayer->panelList.IsVaildSlot(param->DestIndex))
		return msg;

	stPetInfo *pPetInfoSrc = (stPetInfo *)pPlayer->getPetTable().getPetInfo(param->SrcIndex);
	if (!pPetInfoSrc)		//此时不进行交换操作,也不需要提示错误
		return MSG_NONE;

	PetShortcut *pNewShortcut = PetShortcut::CreatePetItem(param->SrcIndex);
	pPlayer->panelList.SetSlot(param->DestIndex, pNewShortcut);
	pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), param->DestIndex, ITEM_NOSHOW);

	return MSG_NONE;
}

enWarnMessage ItemManager::PetListMoveToTempList(stExChangeParam *param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAG;
	Player *pPlayer = param->player;
	PetTable &petTable = (PetTable&)pPlayer->getPetTable();

	if (!pPlayer || !pPlayer->petList.IsVaildSlot(param->SrcIndex))
		return msg;

	ShortcutObject *pShortcut = pPlayer->petList.GetSlot(param->SrcIndex);
	if (!pShortcut || pShortcut->getSlotState() == ShortcutObject::SLOT_LOCK)
		return msg;

	switch(pPlayer->tempList.GetOptType())
	{
	case TempList::OptType_PetIdentify:
		{
			return TempList::PetListMoveToPetIdentify(param);
		}
		break;
	case TempList::OptType_PetInsight:
		{
			return TempList::PetListMoveToPetInsight(param);
		}
		break;
	case TempList::OptType_PetLianHua:
		{
			return TempList::PetListMoveToPetLianHua(param);
		}
		break;
	case TempList::OptType_PetHuanTong:
		{
			return TempList::PetListMoveToPetHuanTong(param);
		}
		break;
	default:
		return msg;
	}

	return MSG_NONE;
}

enWarnMessage ItemManager::PetListMoveToPetList(stExChangeParam *param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAG;

	Player *pPlayer = param->player;
	PetTable &petTable = (PetTable&)pPlayer->getPetTable();
	if (!pPlayer || !pPlayer->petList.IsVaildSlot(param->SrcIndex) || !pPlayer->petList.IsVaildSlot(param->DestIndex))
		return msg;

	stPetInfo *pPetInfoSrc = (stPetInfo *)pPlayer->getPetTable().getPetInfo(param->SrcIndex);

	if (!pPetInfoSrc)			//此时不需要进行实际交换操作,也不需要提示错误
		return MSG_NONE;

	petTable.swapPetInfo(param->SrcIndex, param->DestIndex);

	//更改PetObject对应的槽
	bool bUpdatePetSlot = false;
	for (S32 i = 0; i < PetObjectData::PetMax; i++)
	{
		PetObject *pPet = petTable.getPet(i);
		if(!pPet)
			continue;
		bUpdatePetSlot = true;
		if (pPet->getPetSlot() == param->SrcIndex)
			pPet->setPetSlot(param->DestIndex);
		else if (pPet->getPetSlot() == param->DestIndex)
			pPet->setPetSlot(param->SrcIndex);
	}	
	PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, param->SrcIndex);
	PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, param->DestIndex);
	if (bUpdatePetSlot)
		pPlayer->setMaskBits(Player::PetMask);

	//更新宠物修行计时器对应的ID
	if (pPlayer->mTimerPetStudyMgr)
		pPlayer->mTimerPetStudyMgr->SwapTimerPetStudyId(param->SrcIndex + 1, param->DestIndex + 1);
	//同步快捷栏
	g_ItemManager->syncPetPanel(pPlayer, param->SrcIndex, param->DestIndex);

	return MSG_NONE;
}

enWarnMessage ItemManager::MountListMoveToPanel(stExChangeParam *param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAG;

	Player *pPlayer = param->player;
	if (!pPlayer || !pPlayer->mountPetList.IsVaildSlot(param->SrcIndex) || !pPlayer->panelList.IsVaildSlot(param->DestIndex))
		return msg;

	stMountInfo *pMountInfoSrc = (stMountInfo *)pPlayer->getPetTable().getMountInfo(param->SrcIndex);
	if (!pMountInfoSrc)		//此时不进行交换操作,也不需要提示错误
		return MSG_NONE;

	MountShortcut *pNewShortcut = MountShortcut::CreateMountItem(param->SrcIndex);
	pPlayer->panelList.SetSlot(param->DestIndex, pNewShortcut);
	pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), param->DestIndex, ITEM_NOSHOW);

	return MSG_NONE;
}

enWarnMessage ItemManager::MountListMoveToMountList(stExChangeParam *param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAG;

	Player *pPlayer = param->player;
	PetTable &petTable = (PetTable&)pPlayer->getPetTable();
	if (!pPlayer || !pPlayer->mountPetList.IsVaildSlot(param->SrcIndex) || !pPlayer->mountPetList.IsVaildSlot(param->DestIndex))
		return msg;

	stMountInfo *pMountInfoSrc = (stMountInfo *)pPlayer->getPetTable().getMountInfo(param->SrcIndex);

	if (!pMountInfoSrc)			//此时不需要进行实际交换操作,也不需要提示错误
		return MSG_NONE;

	petTable.swapMountInfo(param->SrcIndex, param->DestIndex);

	//更改PetObject对应的槽
	bool bUpdatePetSlot = false;
	
	ShapeBase *pMount = pPlayer->getObjectMount();
	if(pMount)
	{
		bUpdatePetSlot = true;
	}
	PetMountOperationManager::ServerSendUpdateMountPetSlotMsg(pPlayer, param->SrcIndex);
	PetMountOperationManager::ServerSendUpdateMountPetSlotMsg(pPlayer, param->DestIndex);
	if (bUpdatePetSlot)
		pPlayer->setMaskBits(Player::MountPetMask);
	
	//同步快捷栏
	g_ItemManager->syncMountPanel(pPlayer, param->SrcIndex, param->DestIndex);

	return MSG_NONE;
}
#endif

#ifdef NTJ_SERVER
//同步快捷栏的宠物槽位
void ItemManager::syncPetPanel(Player* pPlayer, U32 nOldPetSlot, U32 nNewPetSlot)
{
	if(!pPlayer || nOldPetSlot >= PET_MAXSLOTS)
		return;

	for (S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		ShortcutObject *pShortcutObject = pPlayer->panelList.GetSlot(i);
		if (!pShortcutObject || !pShortcutObject->isPetObject())
			continue;
		PetShortcut *pPetShortcut = (PetShortcut *)pShortcutObject;
		S32 nSlotIndex = pPetShortcut->getSlotIndex();
		if (nSlotIndex == nOldPetSlot)
		{
			if (nNewPetSlot >=0 && nNewPetSlot < PET_MAXSLOTS)
			{
				pPetShortcut->setSlotIndex(nNewPetSlot);
				pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), i, ITEM_NOSHOW);
			}
			else if(nNewPetSlot == -1)
			{
				pPlayer->panelList.SetSlot(i, NULL);
				pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), i, ITEM_NOSHOW);
			}
		}
		else if (nSlotIndex == nNewPetSlot)
		{	
			if (nNewPetSlot >= 0 && nNewPetSlot < PET_MAXSLOTS)
			{
				pPetShortcut->setSlotIndex(nOldPetSlot);
				pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), i, ITEM_NOSHOW);
			}
		}
	}
}

//同步快捷栏的骑宠槽位
void ItemManager::syncMountPanel(Player* pPlayer, U32 nOldPetSlot, U32 nNewPetSlot)
{
	if(!pPlayer || nOldPetSlot >= PET_MAXSLOTS)
		return;

	for (S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		ShortcutObject *pShortcutObject = pPlayer->panelList.GetSlot(i);
		if (!pShortcutObject || !pShortcutObject->isMountObject())
			continue;
		MountShortcut *pMountShortcut = (MountShortcut *)pShortcutObject;
		S32 nSlotIndex = pMountShortcut->getSlotIndex();
		if (nSlotIndex == nOldPetSlot)
		{
			if (nNewPetSlot >=0 && nNewPetSlot < PET_MAXSLOTS)
			{
				pMountShortcut->setSlotIndex(nNewPetSlot);
				pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), i, ITEM_NOSHOW);
			}
			else if(nNewPetSlot == -1)
			{
				pPlayer->panelList.SetSlot(i, NULL);
				pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), i, ITEM_NOSHOW);
			}
		}
		else if (nSlotIndex == nNewPetSlot)
		{	
			if (nNewPetSlot >= 0 && nNewPetSlot < PET_MAXSLOTS)
			{
				pMountShortcut->setSlotIndex(nOldPetSlot);
				pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), i, ITEM_NOSHOW);
			}
		}
	}
}
#endif