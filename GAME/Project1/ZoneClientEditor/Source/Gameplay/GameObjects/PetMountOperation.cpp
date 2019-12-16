#include "console/console.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/GameObjects/PetMountOperation.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/MountObject.h"

#ifdef NTJ_CLIENT
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#endif
#ifdef NTJ_SERVER
#include "Gameplay/GameEvents/SimCallBackEvents.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/ServerGameplayState.h"
#endif


enWarnMessage PetMountOperationManager::CanUseMountPetEgg(Player *pPlayer, S32 iType, S32 iIndex)
{
	return MSG_NONE;
}

bool PetMountOperationManager::UseMountPetEgg(Player *pPlayer, S32 iType, S32 iIndex)
{
	enWarnMessage msg = CanUseMountPetEgg(pPlayer, iType, iIndex);
	if(msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	ClientSendUseMountPetEggMsg(pPlayer, iType, iIndex);
	BeginVocalEvent(pPlayer, 5, SHOWTYPE_NOTIFY, -1);  //
#endif

#ifdef NTJ_SERVER
	if (pPlayer->pScheduleEvent->getEventID() == 0)
	{
		//需要吟唱，服务端进行同步吟唱
		simCallBackEvents* Event = new simCallBackEvents(INFO_MOUNT_PET_OPERATE);
		Event->SetIntArgValues(3, Player::PetOp_UseMountPetEgg, iType, iIndex);
		U32 ret = Sim::postEvent(pPlayer, Event,Sim::getCurrentTime() + 5000);
		pPlayer->pScheduleEvent->setEvent(ret, 5000);
		return true;
	}
	else
	{
		pPlayer->pScheduleEvent->cancelEvent(pPlayer);
		return false;
	}
#endif
	return true;
}

enWarnMessage PetMountOperationManager::CanSpawnMountPet( Player *pPlayer, U32 nSlot, MountObjectData **ppData)
{
	const stMountInfo *pMountInfo = pPlayer->getPetTable().getMountInfo(nSlot);
	if(!pMountInfo || pMountInfo->mDataId == 0)
		return MSG_MOUNT_NOT_EXIST;

	*ppData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!*ppData)
		return MSG_MOUNT_DATA_NOT_EXIST;

	if (pMountInfo->mStatus != MountObject::MountStatus_Idle)
		return MSG_MOUNT_STATUS_NOT_IDLE;

	// 当骑乘不是自己的宠物时
	if(pPlayer->getPetTable().getMountedSlot() == -1 && pPlayer->getObjectMount())
		return MSG_SKILL_CANNOTDOTHAT;

	return MSG_NONE;
}

bool PetMountOperationManager::SpawnMountPet( Player *pPlayer, U32 nSlot )
{
	MountObjectData *pMountData = NULL;
	enWarnMessage msg = CanSpawnMountPet(pPlayer, nSlot, &pMountData);
	if(msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	if (!ClientSendPetMountOptMsg(pPlayer, Player::PetOp_SpawnMountPet, nSlot))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}
#endif

	return BeginVocalEvent(pPlayer, 6, Player::PetOp_SpawnMountPet, nSlot);
}

enWarnMessage PetMountOperationManager::CanDisbandMountPet( Player *pPlayer, U32 nSlot )
{	
	const stMountInfo *pMountInfo = pPlayer->getPetTable().getMountInfo(nSlot);
	if (!pMountInfo)
		return MSG_MOUNT_NOT_EXIST;
	if (pMountInfo->mStatus != MountObject::MountStatus_Mount)
		return MSG_MOUNT_STATUS_IDLE;
	return MSG_NONE;
}

bool PetMountOperationManager::DisbandMountPet( Player *pPlayer, U32 nSlot, bool bDelBuff /* = true */)
{
	enWarnMessage msg = CanDisbandMountPet(pPlayer, nSlot);
	if(msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	// 发送回收请求到服务端
	if (!ClientSendPetMountOptMsg(pPlayer, Player::PetOp_DisbandMountPet, nSlot))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}
#endif

#ifdef NTJ_SERVER
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	ShapeBase* pMount = pPlayer->getObjectMount();
	if(!pMount)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}

	if(bDelBuff)
	{
		const stMountInfo* pInfo = petTable.getMountInfo(nSlot);
		if(!pInfo)
			return false;
		MountObjectData* pData = g_MountRepository.GetMountData(pInfo->mDataId);
		if(pData)
			pPlayer->RemoveBuffById(Macro_GetBuffSeriesId(pData->buffId), 1, Buff::Mask_Remove_Sys, true);
	}
	//开始回收操作
	pMount->deleteObject();
	pPlayer->setControlObject(NULL);
	pPlayer->unmount();
	petTable.setMountStatus(nSlot, MountObject::MountStatus_Idle);
	
	ServerSendUpdateMountPetSlotMsg(pPlayer, nSlot);
	pPlayer->setMaskBits(Player::MountPetMask);
#endif

	return true;
}

enWarnMessage PetMountOperationManager::CanReleaseMountPet( Player *pPlayer, U32 nSlot, bool bNeedConfirm /*= false*/)
{
	const stMountInfo *pMountInfo = pPlayer->getPetTable().getMountInfo(nSlot);
	if (!pMountInfo)
		return MSG_MOUNT_DATA_NOT_EXIST;
	if (pMountInfo->mStatus != MountObject::MountStatus_Idle)
		return MSG_MOUNT_STATUS_NOT_IDLE;

	MountObjectData *pData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pData)
		return MSG_MOUNT_DATA_NOT_EXIST;

#ifdef NTJ_CLIENT
	if (bNeedConfirm)
	{
		//弹出对话框等待用户确认放生该宠物
		Con::executef("ReleaseMountPet_Check", Con::getIntArg(nSlot), pData->mName);
		return MSG_WAITING_CONFIRM;
	}
#endif

	return MSG_NONE;
}

bool PetMountOperationManager::ReleaseMountPet(Player *pPlayer, U32 nSlot)
{
	enWarnMessage msg = CanReleaseMountPet(pPlayer, nSlot);
	if(msg != MSG_NONE && msg != MSG_WAITING_CONFIRM)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

	//客户端到这里只能是MSG_WAITING_CONFIRM
#ifdef NTJ_SERVER
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	stMountInfo *pMountInfo = (stMountInfo *)petTable.getMountInfo(nSlot);	//这里pPetInfo不可能为NULL
	pMountInfo->clear();
	ServerSendUpdateMountPetSlotMsg(pPlayer, nSlot);
#endif

	return true;
}

//骑宠剩余时间为0时的操作
enWarnMessage PetMountOperationManager::CanForceReleaseMountPet(Player *pPlayer, U32 nSlot)
{
#ifdef NTJ_SERVER
	if (!pPlayer)
		return MSG_UNKOWNERROR;

	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	const stMountInfo *pMountInfo = petTable.getMountInfo(nSlot);
	if (!pMountInfo)
		return MSG_MOUNT_NOT_EXIST;
	if (pMountInfo->mLeftTime != 0)
		return MSG_UNKOWNERROR;	
#endif

	return MSG_NONE;
}

bool PetMountOperationManager::ForceReleaseMountPet(Player *pPlayer, U32 nSlot)
{
#ifdef NTJ_SERVER
	enWarnMessage msg = CanForceReleaseMountPet(pPlayer, nSlot);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	stMountInfo *pMountInfo = (stMountInfo *)petTable.getMountInfo(nSlot);
	MountObjectData* pData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if(!pData)
		return false;

	//如果在坐骑状态，收回玩家坐骑
	char message[256];
	if (pMountInfo->mStatus = MountObject::MountStatus_Mount)
	{
		ShapeBase* pMount = pPlayer->getObjectMount();
		if (pMount)
		{
			pPlayer->RemoveBuffById(Macro_GetBuffSeriesId(pData->buffId), 1, Buff::Mask_Remove_Sys, true);
			//回收操作
			pMount->deleteObject();
			pPlayer->setControlObject(NULL);
			pPlayer->unmount();		
			pPlayer->setMaskBits(Player::MountPetMask);
		}
	}

	dSprintf(message, sizeof(message), "由于[%s]的使用时间到期，系统将之删除", pData->mName);
	pMountInfo->clear();		//删除当前骑宠信息
	ServerSendUpdateMountPetSlotMsg(pPlayer, nSlot);
	MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, message);
#endif
	return true;
}

bool PetMountOperationManager::Mount(Player *pPlayer, U32 dataId)
{
#ifdef NTJ_SERVER
	// 有其他骑乘
	if(pPlayer->getObjectMount())
		return false;

	MountObjectData *pData = g_MountRepository.GetMountData(dataId);
	if (!pData)
		return false;

	MountObject* pMountObject = new MountObject;
	if(!pMountObject)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}

	pMountObject->setDataID(dataId);
	pMountObject->setLayerID(pPlayer->getLayerID());
	// 注意：增加状态应该是最后一步
	if(!pMountObject->registerObject() || !pPlayer->AddBuff(Buff::Origin_Buff, pData->buffId, pPlayer))
	{
		delete pMountObject;
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_PET_DATA_NOT_EXIST);
		return false;
	}
	pMountObject->setPosition(pPlayer->getPosition(), pPlayer->getRotation());
	pMountObject->mountObject(pPlayer, "link14Point", "link08Point");
	pPlayer->setControlObject(pMountObject);
	pPlayer->setMaskBits(Player::MountPetMask);
#endif
	return true;
}

void PetMountOperationManager::Unmount(Player *pPlayer, U32 dataId)
{
#ifdef NTJ_SERVER
	MountObjectData* pData = g_MountRepository.GetMountData(dataId);
	if(!pData)
		return;
	ShapeBase* pMount = pPlayer->getObjectMount();
	if (pMount)
	{
		pPlayer->RemoveBuffById(Macro_GetBuffSeriesId(pData->buffId), 1, Buff::Mask_Remove_Sys, true);
		//回收操作
		pMount->safeDeleteObject();
		pPlayer->setControlObject(NULL);
		pPlayer->unmount();
		pPlayer->setMaskBits(Player::MountPetMask);
	}
#endif
}

#ifdef NTJ_CLIENT
bool PetMountOperationManager::ClientSendUseMountPetEggMsg(Player *pPlayer, S32 iType, S32 iIndex)
{
	if (!pPlayer)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_MOUNT_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(3, Player::PetOp_UseMountPetEgg, iType, iIndex);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool PetMountOperationManager::ClientSendPetMountOptMsg(Player *pPlayer, S32 opType, U32 nSlot)
{
	if (!pPlayer || nSlot >= MOUNTPET_MAXSLOTS)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_MOUNT_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(2, opType, nSlot);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}
#endif

#ifdef NTJ_SERVER
void PetMountOperationManager::InitActiveMountObject(Player *pPlayer, U32 nSlot)
{
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	const stMountInfo *pMountInfo = petTable.getMountInfo(nSlot);
	if(!pMountInfo)
		return;

	Mount(pPlayer, pMountInfo->mDataId);
}

bool PetMountOperationManager::CallBackUseMountPetEgg(Player *pPlayer, S32 iType, S32 iIndex)
{
	enWarnMessage msg = CanUseMountPetEgg(pPlayer, iType, iIndex);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

	U32 nSlot = -1;
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	ItemShortcut *pMountPetEggItem = dynamic_cast<ItemShortcut*>(g_ItemManager->getShortcutSlot(pPlayer, iType, iIndex));
	Res *pMountPetEggRes = NULL;
	if (!pMountPetEggItem || !(pMountPetEggRes = pMountPetEggItem->getRes()))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_ITEM_ITEMDATAERROR);
		return false;
	}
	stMountInfo* info = petTable.getEmptyMountSlot(nSlot);
	if(!info)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_PET_INFO_SLOT_FULL);
		return false;
	}
	MountObjectData* pMountData = g_MountRepository.GetMountData(pMountPetEggRes->getBaseAttribute());
	if(!pMountData)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_PET_DATA_NOT_EXIST);
		return false;
	}
	//生成骑宠信息
	info->mDataId = pMountPetEggRes->getBaseAttribute();
	info->mLevel = pMountData->mLevel;
	info->mLeftTime = pMountData->mDuringTime * 1000;		//骑宠Data中的骑乘时间单位为: 秒
	info->mStatus = MountObject::MountStatus_Idle;

	//删除物品
	g_ItemManager->delItemFromInventoryByIndex(pPlayer, iIndex);
	ServerSendUpdateMountPetSlotMsg(pPlayer, nSlot);
	//ServerSendMountPetOptMsg(pPlayer, Player::PetOp_UseMountPetEgg, nSlot, true);

	//增加骑宠开始计时
	petTable.setMountStartTime(nSlot, Platform::getRealMilliseconds());

	return true; 
}

bool PetMountOperationManager::CallBackSpawnMountPet(Player *pPlayer, U32 nSlot)
{
	MountObjectData *pMountData = NULL;
	enWarnMessage msg = CanSpawnMountPet(pPlayer, nSlot, &pMountData);
	if(msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}
	const stMountInfo *pMountInfo = pPlayer->getPetTable().getMountInfo(nSlot);
	if(!pMountInfo)
		return false;

	// 首先检查是否需要收回当前宠物
	S32 nPrevSlot = pPlayer->getPetTable().getMountedSlot();
	if(nPrevSlot != -1) 
		DisbandMountPet(pPlayer, nPrevSlot);

	if(!Mount(pPlayer, pMountInfo->mDataId))
		return false;

	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	petTable.setMountStatus(nSlot, MountObject::MountStatus_Mount);
	ServerSendUpdateMountPetSlotMsg(pPlayer, nSlot);
	return true;
}

void PetMountOperationManager::ServerSendUpdateMountPetSlotMsg(Player *pPlayer, U32 nSlot)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return;

	GameConnection *conn = pPlayer->getControllingClient();
	MountPetEvent *ev = new MountPetEvent(nSlot);
	if (conn && ev)
		conn->postNetEvent(ev);
}

bool PetMountOperationManager::ServerSendMountPetOptMsg(Player *pPlayer, S32 opType, U32 nSlot, bool bOptSuccess)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return false;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_MOUNT_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(3, opType, nSlot, bOptSuccess);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}
#endif

void PetMountOperationManager::ProcessingErrorMessage(Player *pPlayer, U32 nMsgType, enWarnMessage msg)
{
#ifdef NTJ_CLIENT
	MessageEvent::show(nMsgType, msg);
#endif

#ifdef NTJ_SERVER
	MessageEvent::send(pPlayer->getControllingClient(), nMsgType, msg);
#endif
}

bool PetMountOperationManager::BeginVocalEvent(Player *pPlayer, U32 vocalType, U32 nOptType, U32 nSlot)
{
#ifdef NTJ_CLIENT
	//客户端开始吟唱
	if ( g_ClientGameplayState->pVocalStatus->getStatusType() != VocalStatus::VOCALSTATUS_NONE &&
		g_ClientGameplayState->pVocalStatus->getStatus() )
	{
		g_ClientGameplayState->pVocalStatus->cancelVocalEvent();
		return false;
	}
	Con::executef("InitFetchTimeProgress",Con::getIntArg(5000), Con::getIntArg(vocalType),"");
	return true;
#endif

#ifdef NTJ_SERVER
	if (pPlayer->pScheduleEvent->getEventID() == 0)
	{
		//需要吟唱，服务端进行同步吟唱
		simCallBackEvents* Event = new simCallBackEvents(INFO_MOUNT_PET_OPERATE);
		Event->SetIntArgValues(2, nOptType, nSlot);
		U32 ret = Sim::postEvent(pPlayer, Event,Sim::getCurrentTime() + 5000);
		pPlayer->pScheduleEvent->setEvent(ret, 5000);
		return true;
	}
	else
	{
		pPlayer->pScheduleEvent->cancelEvent(pPlayer);
		return false;
	}
#endif

	return false;
}

#ifdef NTJ_CLIENT
ConsoleFunction(Mount_GetSpawnedMount, S32, 1, 1, "Mount_GetSpawnedMount();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return -1;
	PetTable &table = (PetTable &)pPlayer->getPetTable();
	for (S32 i = 0; i < MOUNTPET_MAXSLOTS; i++)
	{
		const stMountInfo *info = table.getMountInfo(i);
		if (!info)
			continue;
		if (info->mStatus == MountObject::MountStatus_Mount)
			return i;
	}
	return -1;
}

ConsoleFunction(Mount_GetFirstMountSlot, S32, 1, 1, "Mount_GetFirstMountSlot();")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	PetTable &table = (PetTable &)pPlayer->getPetTable();
	for(S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		const stMountInfo *info = table.getMountInfo(i);
		if (info)
			return i;
	}
	return -1;
}

ConsoleFunction(Mount_IsMountExist, bool, 2, 2, "Mount_IsMountExist(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return ((PetTable&)pPlayer->getPetTable()).isMountExist(nSlot);
}

ConsoleFunction(Mount_CanSpawnMount, bool, 2, 2, "Mount_CanSpawnMount(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	MountObjectData *pData = NULL;
	return (PetMountOperationManager::CanSpawnMountPet(pPlayer, nSlot, &pData) == MSG_NONE);
}

ConsoleFunction(Mount_CanDisbandMount, bool, 2, 2, "Mount_CanDisbandMount(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return (PetMountOperationManager::CanDisbandMountPet(pPlayer, nSlot) == MSG_NONE);
}

ConsoleFunction(Mount_CanReleaseMount, bool, 2, 2, "Mount_CanReleaseMount(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return (PetMountOperationManager::CanReleaseMountPet(pPlayer, nSlot, false) == MSG_NONE);
}

ConsoleFunction(Mount_SpawnMount, void, 2, 2, "Mount_SpawnMount(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetMountOperationManager::SpawnMountPet(pPlayer, nSlot);
}

ConsoleFunction(Mount_DisbandMount, void, 2, 2, "Mount_DisbandMount(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetMountOperationManager::DisbandMountPet(pPlayer, nSlot);
}

ConsoleFunction(Mount_ReleaseMount, void, 2, 2, "Mount_ReleaseMount(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetMountOperationManager::ReleaseMountPet(pPlayer, nSlot);
}

ConsoleFunction(ReleaseMountPet_Confirm, void, 2, 2, "ReleaseMountPet_Confirm(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetMountOperationManager::ClientSendPetMountOptMsg(pPlayer, Player::PetOp_ReleaseMountPet, nSlot);
}

ConsoleFunction(Mount_GetMountStatus, S32, 2, 2, "Mount_GetMountStatus(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return -1;
	MountObject::enMountStatus status;
	bool bSuccess = ((PetTable&)pPlayer->getPetTable()).getMountStatus(nSlot, status);
	if (!bSuccess)
		return -1;
	return (S32)status;
}

ConsoleFunction(Mount_GetMountName, StringTableEntry, 2, 2, "Mount_GetMountName(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	return ((PetTable&)pPlayer->getPetTable()).getMountName(nSlot);
}

ConsoleFunction(Mount_GetMountDesction, StringTableEntry, 2, 2, "Mount_GetMountDesction(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	return ((PetTable&)pPlayer->getPetTable()).getMountInfoDesc(nSlot);
}

ConsoleFunction(Mount_GetMountLevel, S32, 2, 2, "Mount_GetMountLevel(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable&)pPlayer->getPetTable()).getMountLevel(nSlot);
}

ConsoleFunction(Mount_GetMountPlayerLevel, S32, 2, 2, "Mount_GetMountPlayerLevel(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable&)pPlayer->getPetTable()).getMountPlayerLevel(nSlot);
}

ConsoleFunction(Mount_GetMountSpeedupPercent, S32, 2, 2, "Mount_GetMountSpeedupPercent(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable&)pPlayer->getPetTable()).getMountSpeedupPercent(nSlot);
}

ConsoleFunction(Mount_GetMountLeftTime, S32, 2, 2, "Mount_GetMountLeftTime(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable&)pPlayer->getPetTable()).getMountLeftTime(nSlot);
}

ConsoleFunction(Mount_GetMountType, S32, 2, 2, "Mount_GetMountType(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable&)pPlayer->getPetTable()).getMountType(nSlot);
}

ConsoleFunction(Mount_GetMountPlayerCount, S32, 2, 2, "Mount_GetMountPlayerCount(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable&)pPlayer->getPetTable()).getMountPlayerCount(nSlot);
}

ConsoleFunction(Mount_GetMountRace, S32, 2, 2, "Mount_GetMountRace(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable&)pPlayer->getPetTable()).getMountRace(nSlot);
}
#endif

#ifdef NTJ_SERVER
ConsoleFunction( DisbandMountPet, void, 3, 3, "DisbandMountPet(%player, %delBuff)")
{
	Player *pPlayer = dynamic_cast<Player*>(Sim::findObject(dAtoi(argv[1])));
	if (!pPlayer)
		return;
	S32 slot = pPlayer->getPetTable().getMountedSlot();
	if(slot == -1)
		return;
	PetMountOperationManager::DisbandMountPet(pPlayer, slot, dAtob(argv[2]));
}
#endif


#ifdef NTJ_SERVER
//这两个函数只是用于测试方便
ConsoleFunction(PetOperation_TestOnly_ReleaseMount, void, 3, 3, "PetOperation_TestOnly_ReleaseMount(%playerId, %nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	U32 nPlayerId = atoi(argv[2]);
	Player *pPlayer = g_ServerGameplayState->GetPlayer(nPlayerId);
	if (!pPlayer)
		return;

	stMountInfo *pMountInfo = (stMountInfo*)pPlayer->getPetTable().getMountInfo(nSlot);
	pMountInfo->clear();
	PetMountOperationManager::ServerSendUpdateMountPetSlotMsg(pPlayer, nSlot);
}
#endif