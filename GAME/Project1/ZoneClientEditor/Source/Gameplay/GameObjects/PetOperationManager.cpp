#include "console/console.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/Data/PetStudyData.h"
#include "Gameplay/GameObjects/TimerTrigger.h"

#ifdef NTJ_CLIENT
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"
#endif
#ifdef NTJ_SERVER
#include "Gameplay/GameEvents/SimCallBackEvents.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/ServerGameplayState.h"
#endif

bool PetOperationManager::PetDead(Player *pPlayer, PetObject *pPetObject)
{
	if (!pPetObject)
		return false;

	U32 nSlot = pPetObject->getPetSlot();
	U32 nPetIndex = pPetObject->getPetIndex();

	if (nSlot >= PET_MAXSLOTS || !pPlayer)
		return false;
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	stPetInfo *pPetInfo = (stPetInfo *)petTable.getPetInfo(nSlot);
	if (!pPetInfo)
		return false;

#ifdef NTJ_SERVER
	pPetInfo->status = PetObject::PetStatus_Idle;
	U8 nReduceHappiness = 0;
	if (pPetInfo->level >= 1 && pPetInfo->level <= 30)
		nReduceHappiness = 10;
	if (pPetInfo->level >= 31 && pPetInfo->level <= 60)
		nReduceHappiness = 20;
	if (pPetInfo->level >= 61 && pPetInfo->level <= 90)
		nReduceHappiness = 30;
	if (pPetInfo->level >= 91 && pPetInfo->level <= 120)
		nReduceHappiness = 40;

	pPetInfo->happiness = (pPetInfo->happiness >= nReduceHappiness) ? (pPetInfo->happiness - nReduceHappiness) : 0;
	pPetInfo->lives -= 0;
	pPetInfo->curHP = 1;
	ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetStatus_Mask | stPetInfo::PetHappiness_Mask | stPetInfo::PetOther_Mask);
	bool bSuccess = petTable.unlinkPet(nPetIndex);
	ServerSendPetOptMsg(pPlayer, Player::PetOp_UnLink, nPetIndex, bSuccess);	
#endif
	return true;
}

bool PetOperationManager::PetChangeName(Player *pPlayer, U32 nSlot, StringTableEntry sNewName)
{
	if (nSlot >= PET_MAXSLOTS || !pPlayer)
		return false;
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return false;

	int nLen = dStrlen(sNewName);
	if (nLen > COMMON_STRING_LENGTH)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_PET_NAME_TOO_LONG);
		return false;
	}

#ifdef NTJ_CLIENT
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (ev && conn)
	{
		ev->SetIntArgValues(2, Player::PetOp_ChangeName, nSlot);
		ev->SetStringArgValues(1, sNewName);
		conn->postNetEvent(ev);
	}
	else
		return false;
#endif

#ifdef NTJ_SERVER
	if (dStrcmp(sNewName,"") == 0)
	{
		PetObjectData *pData = g_PetRepository.GetPetData(pPetInfo->petDataId);
		if (pData)
			dStrcpy(pPetInfo->name, COMMON_STRING_LENGTH, pData->petName);
	}
	else
	{
		dStrcpy(pPetInfo->name, COMMON_STRING_LENGTH, sNewName);
	}
	PetTable &table = (PetTable &)pPlayer->getPetTable();
	if (pPetInfo && nSlot == table.getSpawnPetSlot())
	{
		PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
		if (!pPetData)
			return false;
		PetObject *pPet = table.getPet(pPetData->combatPetIdx);
		if (!pPet)
			return false;
		pPet->changeName();
		pPet->setShapeName(pPetInfo->name);
	}

	ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetName_Mask);
#endif
	return true;
}

bool PetOperationManager::PetChangeProperties(Player *pPlayer, U32 nSlot, S32 nTiPo, S32 nJingLi, S32 nLiDao, S32 nLingLi, S32 nMinJie, S32 nYuanLi)
{
	stPetInfo *pPetInfo = (stPetInfo*)pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return false;
	
	PetTable& petTable = (PetTable&)pPlayer->getPetTable();

	S32 nCurrTiPo	= petTable.getPetTiPo(nSlot);
	S32 nCurrJingLi = petTable.getPetJingLi(nSlot);
	S32 nCurrLiDao	= petTable.getPetLiDao(nSlot);
	S32 nCurrLingLi	= petTable.getPetLingLi(nSlot);
	S32 nCurrMinJie	= petTable.getPetMingJie(nSlot);
	S32 nCurrYuanLi	= petTable.getPetYuanLi(nSlot);
	S32 nCurrQianLi	= petTable.getPetQianLi(nSlot);

	//检查是否能修改属性
	if (nTiPo < nCurrTiPo || nJingLi < nCurrJingLi || nLiDao < nCurrLiDao || 
		nLingLi < nCurrLingLi ||nMinJie < nCurrMinJie || nYuanLi < nCurrYuanLi )
		return false;
	S32 nUsedPts = (nTiPo - nCurrTiPo) + (nJingLi - nCurrJingLi) + (nLiDao - nCurrLiDao) +
					(nLingLi - nCurrLingLi) + (nMinJie - nCurrMinJie) + (nYuanLi - nCurrYuanLi);
	if ( nUsedPts > nCurrQianLi)
		return false;
	if ( nUsedPts == 0 )	//属性没有修改，不需要向服务端发消息
		return false;

#ifdef NTJ_CLIENT
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (ev && conn)
	{
		ev->SetIntArgValues(2, Player::PetOp_ChangeProperties, nSlot);
		ev->SetInt32ArgValues(6, nTiPo, nJingLi, nLiDao, nLingLi, nMinJie, nYuanLi);
		conn->postNetEvent(ev);
	}
	else
		return false;
#endif

#ifdef NTJ_SERVER
	pPetInfo->staminaPts	+= (nTiPo - nCurrTiPo);
	pPetInfo->ManaPts		+= (nJingLi - nCurrJingLi);
	pPetInfo->strengthPts	+= (nLiDao - nCurrLiDao);
	pPetInfo->intellectPts	+= (nLingLi - nCurrLingLi);
	pPetInfo->agilityPts	+= (nMinJie - nCurrMinJie);
	pPetInfo->pneumaPts		+= (nYuanLi - nCurrYuanLi);
	pPetInfo->statsPoints	-= nUsedPts;

	/*
	if (nTiPo - nCurrTiPo > 0)
	{
		pPetInfo->curHP += (nTiPo - nCurrTiPo);
	}
	if (nJingLi - nCurrJingLi > 0)
	{
		pPetInfo->curMP += (nJingLi - nCurrJingLi);
	}
	*/

	ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetOther_Mask);

	//判断当前宠物是否处于召唤状态，如果是，得到宠物对象设置状态刷新
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	PetObject *pPetObject = petTable.getPet(pPetData->combatPetIdx);
	if ( pPetObject&& nSlot == pPetObject->getPetSlot() )
	{
		pPetObject->SetRefreshStats(true);
	}

#endif

	return true;
}

enWarnMessage PetOperationManager::OpenPetIdentify(Player *pPlayer, NpcObject *pNpc)
{
#ifdef NTJ_SERVER
	if ( !pPlayer->setInteraction(pNpc, Player::INTERACTION_PETIDENTIFY) )
		return MSG_NONE;

	enWarnMessage MsgCode = pPlayer->isBusy(Player::INTERACTION_PETIDENTIFY);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
	pPlayer->tempList.SetOptType(pPlayer->getControllingClient(), TempList::OptType_PetIdentify);
	if (!ServerSendPetOpenMsg(pPlayer, Player::PetOp_Identify))
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
#endif

#ifdef NTJ_CLIENT
	Con::executef("OpenPetCheckupWnd");
#endif

	return MSG_NONE;
}

void PetOperationManager::ClosePetIdentify(Player *pPlayer)
{
	if(!pPlayer)
		return;

#ifdef NTJ_SERVER
	pPlayer->tempList.Clean(pPlayer);
#endif

#ifdef NTJ_CLIENT
	Con::executef("ClosePetCheckupWnd");
#endif
}

enWarnMessage PetOperationManager::CanPetIdentify(Player *pPlayer, U32 nSlot, U32 &nMoney)
{
	if (!pPlayer || !pPlayer->getControllingClient() || nSlot >= PET_MAXSLOTS)
		return MSG_UNKOWNERROR;

	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;

	if (pPetInfo->chengZhangLv != 0)
		return MSG_PET_HAD_BEEN_IDENTIFIED;

	if (pPetInfo->level < 10)
		return MSG_PET_LOW_LEVEL;

	// 计算宠物鉴定所需金钱
	nMoney = atoi(Con::executef("PetIdentify_GetCostMoney"));
	if (!pPlayer->canReduceMoney(nMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	return MSG_NONE;
}

bool PetOperationManager::PetIdentify(Player *pPlayer, U32 nSlot)
{
	U32 nCostMoney = 0;
	enWarnMessage msg = CanPetIdentify(pPlayer, nSlot, nCostMoney);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	if (!ev)
		return false;
	ev->SetIntArgValues(3, Player::PetOp_Identify, 1, nSlot);
	pPlayer->getControllingClient()->postNetEvent(ev);
#endif

#ifdef NTJ_SERVER
	//减去金钱
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	pPlayer->reduceMoney(nCostMoney, 21);

	PetTable &table = (PetTable &)pPlayer->getPetTable();
	F32 qualityFactor = table.getQualityFactor(pPetInfo->quality);
	pPetInfo->chengZhangLv = pPetInfo->qianLi * qualityFactor * 20;	//数据库里保存小数点后两位精度(*100)
	ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetChengZhang_Mask);

#endif

	return true;
}

//宠物还童相关函数
enWarnMessage PetOperationManager::OpenPetHuanTong(Player *pPlayer, NpcObject *pNpc)
{
#ifdef NTJ_SERVER
	if ( !pPlayer->setInteraction(pNpc, Player::INTERACTION_PETHUANTONG) )
		return MSG_NONE;
	enWarnMessage MsgCode = pPlayer->isBusy(Player::INTERACTION_PETHUANTONG);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
	pPlayer->tempList.SetOptType(pPlayer->getControllingClient(), TempList::OptType_PetHuanTong);
	if (!ServerSendPetOpenMsg(pPlayer, Player::PetOp_BecomeBaby))
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
#endif

#ifdef NTJ_CLIENT
	Con::executef("OpenPetHuanTongWnd");
#endif

	return MSG_NONE;
}
void PetOperationManager::ClosePetHuanTong(Player *pPlayer)
{
	if(!pPlayer)
		return;

#ifdef NTJ_SERVER
	pPlayer->tempList.Clean(pPlayer);
#endif

#ifdef NTJ_CLIENT
	Con::executef("ClosePetHuanTongWnd");
#endif
}

enWarnMessage PetOperationManager::CanPetHuanTong(Player *pPlayer, U32 nSlot, U32 &nMoney, bool bNeedConfirm)
{
	if (!pPlayer)
		return MSG_UNKOWNERROR;

	if (pPlayer->tempList.GetOptType() != TempList::OptType_PetHuanTong)
		return MSG_UNKOWNERROR;

	ItemShortcut *pHuanTongDan = dynamic_cast<ItemShortcut*>(pPlayer->tempList.GetSlot(1));
	if (!pHuanTongDan || !pHuanTongDan->getRes())
		return MSG_PET_ITEM_NO_HUANTONGDAN;

	if (!pHuanTongDan->getRes()->isHuanTongDan())
		return MSG_PET_ITEM_NOT_HUANTONG;

	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;

	// 对当前界面中槽类型的判断
	if (pPlayer->tempList.GetOptType() != TempList::OptType_PetHuanTong)
		return MSG_UNKOWNERROR;
	if (pPetInfo->style != 1 && pPetInfo->style != 2)
		return MSG_UNKOWNERROR;	

	//宠物等级上限与下限判断
	if (pPetInfo->level < 10)
		return MSG_PET_LOW_LEVEL;

	if (pPetInfo->happiness <= 60)
		return MSG_PET_LOW_HAPPINESS;
	if (pPetInfo->curHP <= 100)
		return MSG_PET_LOW_HP;

	//金钱判断
	nMoney = atoi(Con::executef("PetHuanTong_GetCostMoney"));
	if (!pPlayer->canReduceMoney(nMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	//宠物类型的判断

	//客户端弹出确认宠物还童对话框
#ifdef NTJ_CLIENT
	if (bNeedConfirm)
	{
		Con::executef("PetHuanTong_Check", Con::getIntArg(nSlot), pPetInfo->name);
		return MSG_WAITING_CONFIRM;
	}
#endif

	return MSG_NONE;
}

bool PetOperationManager::PetHuanTong(Player *pPlayer, U32 nSlot)
{
	U32 nCostMoney = 0;
	enWarnMessage msg = CanPetHuanTong(pPlayer, nSlot, nCostMoney);
	if ( msg != MSG_NONE && msg != MSG_WAITING_CONFIRM)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		Con::executef("OnPetHuanTongFailure");
		return false;
	}

#ifdef NTJ_SERVER
	ItemShortcut *pHuanTongDan = dynamic_cast<ItemShortcut*>(pPlayer->tempList.GetSlot(1));
	S32 nHuanTongDanLevel = pHuanTongDan->getRes()->getReserveValue();
	nHuanTongDanLevel = mClamp(nHuanTongDanLevel, 1, 3);		//还童丹等级范围为[1, 3]
	//扣除还童丹
	S32 nLockIndex = pHuanTongDan->getLockedItemIndex();
	if (nLockIndex == -1)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}
	ItemShortcut *pSrcInventory = dynamic_cast<ItemShortcut*>(pPlayer->inventoryList.GetSlot(nLockIndex));
	S32 nQuantity = pSrcInventory->getRes()->getQuantity();
	//g_ItemManager->delItemFromInventoryByIndex(pPlayer, nLockIndex, 1);
	if (nQuantity > 1)
	{
		pSrcInventory->getRes()->setQuantity(nQuantity - 1);
		pSrcInventory->setSlotState(ShortcutObject::SLOT_COMMON);
	}
	else
	{
		pPlayer->inventoryList.SetSlot(nLockIndex, NULL);
	}
	//根据还童类型计算宠物初始属性
	((PetTable&)pPlayer->getPetTable()).randHuanTongProps(nSlot, nHuanTongDanLevel);
	pPlayer->tempList.SetSlot(1, NULL);

	PetShortcut *pPetShortcut = dynamic_cast<PetShortcut*>(pPlayer->tempList.GetSlot(0));
	if (pPetShortcut)
	{
		S32 nLockIndex = pPetShortcut->getLockedItemIndex();
		if (nLockIndex != -1)
		{
			PetShortcut *pSrc = dynamic_cast<PetShortcut*>(pPlayer->petList.GetSlot(nLockIndex));
			if (pSrc)
			{
				pSrc->setSlotState(ShortcutObject::SLOT_COMMON);
				pPlayer->petList.UpdateToClient(pPlayer->getControllingClient(), nLockIndex, ITEM_NOSHOW);
			}
		}
	}
	pPlayer->tempList.SetSlot(0, NULL);
	pPlayer->tempList.UpdateToClient(pPlayer->getControllingClient(), 0, ITEM_NOSHOW);
	pPlayer->tempList.UpdateToClient(pPlayer->getControllingClient(), 1, ITEM_NOSHOW);
	pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), nLockIndex, ITEM_NOSHOW);

	ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetLevel_Mask | stPetInfo::PetOther_Mask);
#endif

	return true;
}


enWarnMessage PetOperationManager::OpenPetInsight(Player *pPlayer, NpcObject *pNpc)
{
#ifdef NTJ_SERVER
	if ( !pPlayer->setInteraction(pNpc, Player::INTERACTION_PETINSIGHT) )
		return MSG_NONE;
	enWarnMessage MsgCode = pPlayer->isBusy(Player::INTERACTION_PETINSIGHT);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
	pPlayer->tempList.SetOptType(pPlayer->getControllingClient(), TempList::OptType_PetInsight);
	if (!ServerSendPetOpenMsg(pPlayer, Player::PetOp_Insight))
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
#endif

#ifdef NTJ_CLIENT
	Con::executef("OpenPetStrengthenWnd");
#endif

	return MSG_NONE;
}

void PetOperationManager::ClosePetInsight(Player *pPlayer)
{
	if(!pPlayer)
		return;

#ifdef NTJ_SERVER
	pPlayer->tempList.Clean(pPlayer);
#endif

#ifdef NTJ_CLIENT
	Con::executef("ClosePetStrengthenWnd");
#endif
}

enWarnMessage PetOperationManager::CanIncreasePetInsight(Player *pPlayer, U32 nSlot, S32 &nItemIndex, U32 &nMoney)
{
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetInsight)
		return MSG_UNKOWNERROR;

	const stPetInfo *pPetInfo = pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo || pPetInfo->petDataId == 0)
		return MSG_PET_NOT_EXIST;

	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;

	if (pPetInfo->insight == 10)
		return MSG_PET_MAXIMUM_INSIGHT;

	//通过灵兽灵慧等级得到根骨丹ID
	U32 nGenGuLevel = atoi( Con::executef("GetGenGuLevelByInsight", Con::getIntArg(pPetInfo->insight)) );
	if (nGenGuLevel == 0)
		return MSG_UNKOWNERROR;
	U32 nGenGuId = 116070000 + nGenGuLevel;
	//查找玩家包裹中的根骨丹
	nItemIndex = pPlayer->inventoryList.FindSameObjectByID(nGenGuId);
	if (nItemIndex == -1)
		return MSG_PET_N0_GENGUDAN;

	// 计算升级灵慧所需金钱
	nMoney = atoi(Con::executef("PetInsight_GetCostMoney"));
	if (!pPlayer->canReduceMoney(nMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	return MSG_NONE;
}

bool PetOperationManager::IncreasePetInsight(Player *pPlayer, U32 nSlot)
{
	U32 nCostMoney = 0;
	if (!pPlayer)
		return MSG_UNKOWNERROR;

	S32 nItemIndex = 0;
	enWarnMessage msg = CanIncreasePetInsight(pPlayer, nSlot, nItemIndex, nCostMoney);
	if ( msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	if (!PetOperationManager::ClientSendPetOptMsg(pPlayer, Player::PetOp_Insight, nSlot))
		return false;
#endif

#ifdef NTJ_SERVER
	//扣除根骨丹
	g_ItemManager->delItemFromInventoryByIndex(pPlayer, nItemIndex);

	//扣除金钱
	pPlayer->reduceMoney(nCostMoney, 21);
	//校验成功
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);

	U32 nSuccessRate = atoi( Con::executef("GetIncreaseInsightSuccessRate", Con::getIntArg(pPetInfo->insight)) );
	if ( Platform::getRandomI(1, 10000) <= nSuccessRate)
	{
		//升级灵慧成功
		pPetInfo->insight += 1;
		F32 fQuatityFactor = PetTable::getQualityFactor(pPetInfo->quality);
		pPetInfo->staminaGift		+= 100 * fQuatityFactor;
		pPetInfo->ManaGift			+= 100 * fQuatityFactor;
		pPetInfo->strengthGift		+= 100 * fQuatityFactor;
		pPetInfo->intellectGift		+= 100 * fQuatityFactor;
		pPetInfo->agilityGift		+= 100 * fQuatityFactor;
		pPetInfo->pneumaGift		+= 100 * fQuatityFactor;
		ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetOther_Mask);
		return true;
	}
	else
	{
		//升级灵慧失败
		U8 curInsight = atoi( Con::executef("GetDecreasedInsightLevel", Con::getIntArg(pPetInfo->insight)) );
		U8 diff = pPetInfo->insight - curInsight;
		pPetInfo->insight = curInsight;
		if (diff != 0)
		{
			F32 fQuatityFactor = PetTable::getQualityFactor(pPetInfo->quality);
			pPetInfo->staminaGift		-= diff * 100 * fQuatityFactor;
			pPetInfo->ManaGift			-= diff * 100 * fQuatityFactor;
			pPetInfo->strengthGift		-= diff * 100 * fQuatityFactor;
			pPetInfo->intellectGift		-= diff * 100 * fQuatityFactor;
			pPetInfo->agilityGift		-= diff * 100 * fQuatityFactor;
			pPetInfo->pneumaGift		-= diff * 100 * fQuatityFactor;
			ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetOther_Mask);
		}
		return false;
	}
#endif

	return false;
}

enWarnMessage PetOperationManager::OpenPetLianHua(Player *pPlayer, NpcObject *pNpc)
{
#ifdef NTJ_SERVER
	if ( !pPlayer->setInteraction(pNpc, Player::INTERACTION_PETLIANHUA) )
		return MSG_NONE;
	enWarnMessage MsgCode = pPlayer->isBusy(Player::INTERACTION_PETLIANHUA);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
	pPlayer->tempList.SetOptType(pPlayer->getControllingClient(), TempList::OptType_PetLianHua);
	if (!ServerSendPetOpenMsg(pPlayer, Player::PetOp_LianHua))
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
#endif

#ifdef NTJ_CLIENT
	Con::executef("OpenPetDecompoundWnd");
#endif

	return MSG_NONE;
}

void PetOperationManager::ClosePetLianHua(Player *pPlayer)
{
	if(!pPlayer)
		return;

#ifdef NTJ_SERVER
	pPlayer->tempList.Clean(pPlayer);
#endif

#ifdef NTJ_CLIENT
	Con::executef("ClosePetDecompoundWnd");	
#endif
}

enWarnMessage PetOperationManager::CanLianHuaPet(Player *pPlayer, U32 nSlot, U32 &nMoney, bool bNeedConfirm)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return MSG_UNKOWNERROR;

	const stPetInfo *pPetInfo = pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;

	if (pPetInfo->level < 10)
		return MSG_PET_LIANHUA_LEVEL;

	if (pPetInfo->happiness < 60)
		return MSG_PET_LOW_HAPPINESS;

	if (pPetInfo->petTalent == 0)
		return MSG_PET_TALENT_ZERO;
	
	nMoney = atoi(Con::executef("PetLianHua_GetCostMoney"));
	if (!pPlayer->canReduceMoney(nMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

#ifdef NTJ_CLIENT
	if (bNeedConfirm)
	{
		//显示对话框等待用户确认炼化该宠物
		Con::executef("LianHuaPet_Check", Con::getIntArg(nSlot), pPetInfo->name);
		return MSG_WAITING_CONFIRM;
	}
#endif

	return MSG_NONE;
}

bool PetOperationManager::LianHuaPet(Player *pPlayer, U32 nSlot)
{
	U32 nCostMoney = 0;
	enWarnMessage msg = CanLianHuaPet(pPlayer, nSlot, nCostMoney);

	if (msg != MSG_NONE && msg != MSG_WAITING_CONFIRM)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		Con::executef("OnPetLianHuaFailure");
		return false;
	}

#ifdef NTJ_SERVER
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	stPetInfo *pPetInfo = (stPetInfo *)petTable.getPetInfo(nSlot);	//这里pPetInfo不可能为NULL

	pPetInfo->petTalent = 1;	//测试测试测试测试测试测试测试测试测试测试测试测试测试测试
	
	//todo：获得对应根骨丹道具
	U32 nGenGuID = atoi( Con::executef("PetLianHua_GetGenGuDanId", Con::getIntArg(pPetInfo->petTalent)) );
	if (nGenGuID == 0)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_ITEM_ITEMDATAERROR);
		return false;
	}
	S32 index = -1;
	msg = g_ItemManager->addItemToInventory(pPlayer, nGenGuID, index);
	if (msg != MSG_NONE)
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_ERROR, msg);
		return false;
	}

	//扣除金钱
	pPlayer->reduceMoney(nCostMoney, 21);

	//删除对应的宠物信息
	DeletePetInfo(pPlayer, nSlot);

	//解除宠物栏的锁定
	PetShortcut *pSlot = dynamic_cast<PetShortcut *>(pPlayer->petList.GetSlot(nSlot));
	if (pSlot)
	{
		pSlot->setSlotState(ShortcutObject::SLOT_COMMON);
		pPlayer->petList.UpdateToClient(pPlayer->getControllingClient(), nSlot, ITEM_NOSHOW);
	}

	//清空TempList的宠物槽,即0槽
	if (pPlayer->tempList.GetOptType() == TempList::OptType_PetLianHua)
	{
		pPlayer->tempList.SetSlot(0, NULL);
		pPlayer->tempList.UpdateToClient(pPlayer->getControllingClient(), 0, ITEM_NOSHOW);
	}
#endif
	return true;
}

enWarnMessage PetOperationManager::CanReleasePet(Player *pPlayer, U32 nSlot, bool bNeedConfirm /*= true*/)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return MSG_UNKOWNERROR;

	const stPetInfo *pPetInfo = pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;

#ifdef NTJ_CLIENT
	if (bNeedConfirm)
	{
		//显示对话框等待用户确认放生该宠物
		Con::executef("ReleasePet_Check", Con::getIntArg(nSlot), pPetInfo->name);
		return MSG_WAITING_CONFIRM;
	}
#endif

	return MSG_NONE;
}

bool PetOperationManager::ReleasePet(Player *pPlayer, U32 nSlot)
{
	enWarnMessage msg = CanReleasePet(pPlayer, nSlot);

	if (msg != MSG_NONE && msg != MSG_WAITING_CONFIRM)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_SERVER
	msg = DeletePetInfo(pPlayer, nSlot);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}
	//RefreshPetTable(pPlayer, nSlot);
#endif

	return true;
}

enWarnMessage PetOperationManager::CanTameNpc(Player *pPlayer, NpcObject *pNpc)
{
	if (!pPlayer || !pNpc)
		return MSG_UNKOWNERROR;

	if (!pNpc->canTame())
		return MSG_NPC_CAN_NOT_TAME;

	return MSG_NONE;
}

bool PetOperationManager::TameNpc(Player *pPlayer, NpcObject *pNpc)
{
	enWarnMessage msg = CanTameNpc(pPlayer, pNpc);
	if(msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

	// to do
	return true;
}

enWarnMessage PetOperationManager::CanSpawnPet(Player *pPlayer, U32 nSlot, PetObjectData **ppData)
{
	const stPetInfo *pPetInfo = pPlayer->getPetTable().getPetInfo(nSlot);
	if(!pPetInfo || pPetInfo->petDataId == 0)
		return MSG_PET_NOT_EXIST;

	PetObjectData* pData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pData)
		return MSG_PET_DATA_NOT_EXIST;

	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;

	/*	测试时注释
	if (pPlayer->getLevel() + pPetInfo->insight <= pPetInfo->level)
		return MSG_PET_HIGH_LEVEL;
	*/
	
	if (pPetInfo->happiness <= 60)
		return MSG_PET_LOW_HAPPINESS;
	
	if (pPetInfo->curHP < 100)
		return MSG_PET_LOW_HP;

	return MSG_NONE;
}

bool PetOperationManager::SpawnPet(Player *pPlayer, U32 nSlot)
{
	PetObjectData* pData = NULL;
	enWarnMessage msg = CanSpawnPet(pPlayer, nSlot, &pData);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	if (!ClientSendPetOptMsg(pPlayer, Player::PetOp_Spawn, nSlot))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}
#endif

	return BeginVocalEvent(pPlayer, 3, Player::PetOp_Spawn, nSlot);		// VocalStatus::VOCALSTATUS_SPAWN_PET
}

enWarnMessage PetOperationManager::CanDisbandPet(Player *pPlayer, U32 nSlot)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return MSG_UNKOWNERROR;
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	if (pPetInfo->status != PetObject::PetStatus_Combat)
		return MSG_PET_STATUS_NOT_COMBAT;

	return MSG_NONE;
}

bool PetOperationManager::DisbandPet(Player *pPlayer, U32 nSlot)
{
	enWarnMessage msg = CanDisbandPet(pPlayer, nSlot);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	// 发送回收请求到服务端
	if (!ClientSendPetOptMsg(pPlayer, Player::PetOp_Disband, nSlot))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}
#endif

#ifdef NTJ_SERVER
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	PetObject* pet = petTable.getCombatPetBySlot(nSlot);
	if(!pet)
		return false;

	// 先保存对象
	petTable.savePetInfo(pet->getPetIndex());	
	pet->deleteObject();
	petTable.setPetStatus(nSlot, PetObject::PetStatus_Idle);
	ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetStatus_Mask);
	pPlayer->setMaskBits(Player::PetMask);
#endif

	return true;
}

enWarnMessage PetOperationManager::CanUsePetEgg(Player *pPlayer, S32 iType, S32 iIndex)
{
	if (iType != SHORTCUTTYPE_INVENTORY)
		return MSG_ITEM_USEITEMNOOBJECT;

	ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(pPlayer, iType, iIndex);
	if(!pShortcut || !pShortcut->isItemObject())
		return MSG_ITEM_ITEMDATAERROR;

	if (pShortcut->getSlotState() != ShortcutObject::SLOT_COMMON)
		return MSG_ITEM_USEITEMNOOBJECT;

	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
	Res *pPetEggRes = NULL;
	if(!pItem || !(pPetEggRes = pItem->getRes()))
		return MSG_ITEM_ITEMDATAERROR;

	if (pPlayer->getLevel() < pPetEggRes->getLimitLevel())
		return MSG_PLAYER_LEVEL;

	U32 nSlot = 0;
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	if (!petTable.getEmptyPetSlot(nSlot))
		return MSG_PET_INFO_SLOT_FULL;

	return MSG_NONE;
}

bool PetOperationManager::UsePetEgg(Player *pPlayer, S32 iType, S32 iIndex)
{
	enWarnMessage msg = CanUsePetEgg(pPlayer, iType, iIndex);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	ClientSendUsePetEggMsg(pPlayer, iType, iIndex);
	BeginVocalEvent(pPlayer, 4, SHOWTYPE_NOTIFY, -1);  //VocalStatus::VOCALSTATUS_USE_PET_EGG
#endif

#ifdef NTJ_SERVER
	if (pPlayer->pScheduleEvent->getEventID() == 0)
	{
		//需要吟唱，服务端进行同步吟唱
		simCallBackEvents* Event = new simCallBackEvents(INFO_PET_OPERATE);
		Event->SetIntArgValues(3, Player::PetOp_UseEgg, iType, iIndex);
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
//在物品栏中得到满足喂食类型并能够使用的喂食物品
enWarnMessage PetOperationManager::CanPetWeiShi(Player *pPlayer, U32 nSlot, FeedType nFeedType, S32 &nItemIndex)
{
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;

	bool bInvalidStatus = false;
	if (pPetInfo->status != PetObject::PetStatus_Idle &&
		pPetInfo->status != PetObject::PetStatus_Combat &&
		pPetInfo->status != PetObject::PetStatus_Combo)
		bInvalidStatus = true;

	switch(nFeedType)
	{
	case FeedTypeFood:
		{
			if (bInvalidStatus)
				return MSG_PET_CANNOT_FEED;
		}
		break;
	case FeedTypeMedical:
		{
			if (bInvalidStatus)
				return MSG_PET_CANNOT_MEDICAL;
		}
		break;
	case FeedTypeToy:
		{
			if (bInvalidStatus)
				return MSG_PET_CANNOT_TAME;
		}
		break;
	default:
		return MSG_UNKOWNERROR;
	}

	bool bFind = false;
	//在物品栏中搜索当前喂食类型的物品
	for (S32 i = 0; i < InventoryList::MAXSLOTS; i++)
	{
		ItemShortcut *pItem = dynamic_cast<ItemShortcut*>(pPlayer->inventoryList.GetSlot(i));
		if (!pItem || pItem->getSlotState() == ShortcutObject::SLOT_LOCK || !pItem->getRes())
			continue;
		if (CanPetWeiShi(pPlayer, nSlot, nFeedType, i, pItem) == MSG_NONE)
		{
			nItemIndex = i;
			bFind = true;
			break;
		}
	}

	switch(nFeedType)
	{
	case FeedTypeFood:
		{
			if (!bFind)
				return MSG_PET_FOUNDNO_FOOD;
		}
		break;
	case FeedTypeMedical:
		{
			if (!bFind)
				return MSG_PET_FOUNDNO_MEDICAL;
		}
		break;
	case FeedTypeToy:
		{
			if (!bFind)
				return MSG_PET_FOUNDNO_TOY;
		}
		break;
	default:
		return MSG_UNKOWNERROR;
	}

	return MSG_NONE;
}

//判断喂食物品能否使用
enWarnMessage PetOperationManager::CanPetWeiShi(Player *pPlayer, U32 nSlot, FeedType nFeedType, S32 nItemIndex, ItemShortcut *pItem)
{
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;

	enWarnMessage msg = MSG_ITEM_NOFINDITEM;
	Res *pItemRes = pItem->getRes();
	switch(nFeedType)
	{
	case FeedTypeFood:
		{
			if (!pItemRes->isPetFood())
				return msg;
		}
		break;
	case FeedTypeMedical:
		{
			if (!pItemRes->isPetMedical())
				return msg;
		}
		break;
	case FeedTypeToy:
		{
			if (!pItemRes->isPetToy())
				return msg;
		}
		break;
	default:
		return MSG_UNKOWNERROR;
	}

	return MSG_NONE;
}

//玩家在宠物界面中点击按钮喂食时的服务端调用，这里需要获得食物在物品栏的位置
bool PetOperationManager::PetWeiShi_1(Player *pPlayer, U32 nSlot, FeedType nFeedType)
{
	S32 nItemIndex = -1;
	enWarnMessage msg = CanPetWeiShi(pPlayer,nSlot, nFeedType, nItemIndex);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	ClientSendPetWeiShiMsg(pPlayer, nSlot, SHORTCUTTYPE_INVENTORY, nItemIndex);
#endif

#ifdef NTJ_SERVER
	return UsePetFood(pPlayer, nSlot, nFeedType, nItemIndex);
#endif

	return true;
}

//玩家在快捷栏使用宠物喂食类物品时的服务端调用，这里食物在物品栏的位置被直接传入,需要获得食物的喂食类型
bool PetOperationManager::PetWeiShi_2(Player *pPlayer, U32 nSlot, S32 nItemIndex)
{
	ItemShortcut *pItem = dynamic_cast<ItemShortcut*>(pPlayer->inventoryList.GetSlot(nItemIndex));
	Res *pItemRes = NULL;
	if (!pItem || !(pItemRes = pItem->getRes()))
		return false;
	//获得喂食类型
	FeedType nFeedType;
	if (!pItemRes->getPetFoodType((S32 &)nFeedType))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_ITEM_CANNOTUSE);
		return false;
	}
	enWarnMessage msg = CanPetWeiShi(pPlayer, nSlot, nFeedType, nItemIndex, pItem);
	if ( msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	ClientSendPetWeiShiMsg(pPlayer, nSlot, SHORTCUTTYPE_INVENTORY, nItemIndex);
#endif

#ifdef NTJ_SERVER
	return UsePetFood(pPlayer, nSlot, nFeedType, nItemIndex);
#endif

	return true;
}

enWarnMessage PetOperationManager::CanPetXiuXing(Player *pPlayer, U32 nSlot, U32 nPetStudyDataId)
{
	if (!pPlayer || !pPlayer->mTimerPetStudyMgr)
		return MSG_UNKOWNERROR;

	const stPetInfo *pPetInfo = pPlayer->getPetTable().getPetInfo(nSlot);
	if(!pPetInfo || pPetInfo->petDataId == 0)
		return MSG_PET_NOT_EXIST;

	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;
	if (pPetInfo->happiness < 60)
		return MSG_PET_LOW_HAPPINESS;
	PetStudyData *pPetStudyData = g_PetStudyRepository.GetPetStudyData(nPetStudyDataId);
	if (!pPetStudyData)
		return MSG_UNKOWNERROR;
	if (pPetInfo->level < pPetStudyData->mPetLevel)
		return MSG_PET_LOW_LEVEL;
	//TODO: 判断是否有别的宠物正在进行此种修行

	if (pPetInfo->curHP < 100)
		return MSG_PET_LOW_HP;
	//TODO: 人物满足游戏币要求
	//TODO: 人物满足特殊货币要求
	//TODO: 人物满足道具要求

	//判断玩家是否有将消耗的东西
	if (!pPlayer->canReduceMoney(pPetStudyData->mCostMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;
	/**/

	return MSG_NONE;
}

bool PetOperationManager::PetXiuXing(Player *pPlayer, U32 nSlot, U32 nPetStudyDataId)
{
	enWarnMessage msg = CanPetXiuXing(pPlayer, nSlot, nPetStudyDataId);
	if ( msg != MSG_NONE && msg != MSG_WAITING_CONFIRM)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	// 发送回收请求到服务端
	if (!ClientSendPetXiuXingMsg(pPlayer, nSlot, nPetStudyDataId))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}
#endif

#ifdef NTJ_SERVER
	PetStudyData *pPetStudyData = g_PetStudyRepository.GetPetStudyData(nPetStudyDataId);
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	
	if (!pPlayer->mTimerPetStudyMgr || 
		!pPlayer->mTimerPetStudyMgr->CreateTimerPetStudy(nSlot+1, pPetStudyData->mStudyTime, 
														pPetStudyData->mStudyTime, 60000, 
														pPetStudyData->mTimeFlag, nPetStudyDataId))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);		
		return false;
	}
	//扣除修行所需要的消耗物
	pPlayer->reduceMoney(pPetStudyData->mCostMoney, 21);
	pPetInfo->happiness = (pPetInfo->happiness >= pPetStudyData->mCostHappiness) ?  (pPetInfo->happiness-pPetStudyData->mCostHappiness) : 0;
	pPetInfo->status = PetObject::PetStatus_Learn;
	ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetOther_Mask | stPetInfo::PetStatus_Mask);
#endif

	return true;
}

// ----------------------------------------------------------------------------
// 判定宠物是否可以交易
enWarnMessage PetOperationManager::CanPetTrade(Player* pPlayer, Player* pTargetPlayer, U32 nSlot)
{
	if (!pPlayer || !pTargetPlayer)
		return MSG_UNKOWNERROR;
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	const stPetInfo *pPetInfo = petTable.getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	if (!pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;
	if (pPetInfo->level > pTargetPlayer->getLevel())
		return MSG_PET_HIGH_LEVEL;

	return MSG_NONE;
}

bool PetOperationManager::BeginVocalEvent(Player *pPlayer, U32 vocalType, U32 nOptType, U32 nSlot)
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
		simCallBackEvents* Event = new simCallBackEvents(INFO_PET_OPERATE);
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
bool PetOperationManager::ClientSendUsePetEggMsg(Player *pPlayer, S32 iType, S32 iIndex)
{
	if (!pPlayer)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(3, Player::PetOp_UseEgg, iType, iIndex);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool PetOperationManager::ClientSendPetXiuXingMsg(Player *pPlayer, U32 nSlot, U32 nPetStudyDataId)
{
	if (!pPlayer)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(2, Player::PetOp_Learn, nSlot);
		ev->SetInt32ArgValues(1, nPetStudyDataId);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

/////////////////////////// 客户端发送各种宠物操作类消息到服务端 //////////////////////////////
///////////////////////////        这里不包括使用宠物蛋消息      //////////////////////////////
bool PetOperationManager::ClientSendPetOptMsg(Player *pPlayer, S32 opType, U32 nSlot)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		if ( opType == Player::PetOp_Identify || opType == Player::PetOp_Insight || 
			 opType == Player::PetOp_LianHua  || opType == Player::PetOp_BecomeBaby)
		{
			ev->SetIntArgValues(3, opType, 1, nSlot);
		}
		else
			ev->SetIntArgValues(2, opType, nSlot);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool PetOperationManager::ClientSendPetWeiShiMsg(Player *pPlayer, U32 nSlot, S32 iType, S32 iIndex)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(4, Player::PetOp_Feed, nSlot, iType, iIndex);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool PetOperationManager::ClientSendPetSlotExchangeMsg(Player *pPlayer, S32 nSrcIndex, S32 nDestType, S32 nDestIndex)
{
	if (!pPlayer || nSrcIndex < 0 || nSrcIndex >= PET_MAXSLOTS)
		return false;
	if (pPlayer->tempList.GetOptType() != TempList::OptType_PetIdentify &&
		pPlayer->tempList.GetOptType() != TempList::OptType_PetHuanTong &&
		pPlayer->tempList.GetOptType() != TempList::OptType_PetInsight &&
		pPlayer->tempList.GetOptType() != TempList::OptType_PetLianHua )
		return false;
	GameConnection *conn = conn = pPlayer->getControllingClient();	
	if (!conn)
		return false;
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_SHORTCUT_EXCHANGE);
	if (!ev)
		return false;
	ev->SetIntArgValues(4, SHORTCUTTYPE_PET, nSrcIndex, nDestType, nDestIndex);
	ev->SetInt32ArgValues(1, 0);
	conn->postNetEvent(ev);
	return true;
}
#endif

#ifdef NTJ_SERVER
bool PetOperationManager::CallBackSpawnPet(Player *pPlayer, U32 nSlot)
{
	PetObjectData *pData = NULL;
	enWarnMessage msg = CanSpawnPet(pPlayer, nSlot, &pData);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

	// 首先检查是否需要收回当前宠物
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	stPetInfo *pPetInfo = (stPetInfo *)petTable.getPetInfo(nSlot);
	//for (S32 i = 0; i < PetObjectData::PetMax; i++)
	//{
	//	PetObject *pOldPet = petTable.getPet(i);
	//	if (!pOldPet)
	//		continue;
	//	if (pOldPet->getPetSlot() != nSlot)
	//	{
	//		DisbandPet(pPlayer, i);
	//	}
	//	else
	//	{
	//		//当前宠物已被召出，不用再进行召唤
	//		return true;
	//	}
	//}
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return false;

	PetObject *pOldPet = petTable.getPet(pPetData->combatPetIdx);
	if (pOldPet)
	{
		DisbandPet(pPlayer, pOldPet->getPetSlot());
	}

	PetObject* pPet = new PetObject;
	if(!pPet)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}

	pPet->setDataID(pPetInfo->petDataId);
	pPet->setPetSlot(nSlot);
	pPet->mMaster = pPlayer;
	pPet->setLayerID(pPlayer->getLayerID());
	if(!pPet->registerObject())
	{
		delete pPet;
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_PET_DATA_NOT_EXIST);
		return false;
	}
	pPet->mMaster = NULL;
	pPet->setMaster(pPlayer);
	pPet->setTransform(pPlayer->getTransform());

	petTable.loadPetInfo(pPet->getPetIndex());
	petTable.setPetStatus(nSlot, PetObject::PetStatus_Combat);
	petTable.setPet(pPet);
	petTable.setPetStartTime(nSlot, Platform::getRealMilliseconds());	//重设计时开始时间
	if (pPetInfo->mLeftTime == 0)
		pPetInfo->mLeftTime	= 3600000;		//1小时，快乐度计时扣减(第一次召唤出来把剩余时间初始化)

	ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetStatus_Mask | stPetInfo::PetHappiness_Mask | stPetInfo::PetOther_Mask);
	pPlayer->setMaskBits(Player::PetMask);
	return true;
}

bool PetOperationManager::CallBackUsePetEgg(Player *pPlayer, S32 iType, S32 iIndex)
{
	enWarnMessage msg = CanUsePetEgg(pPlayer, iType, iIndex);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}
	
	U32 nSlot = -1;
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	ItemShortcut *pPetEggItem = dynamic_cast<ItemShortcut*>(g_ItemManager->getShortcutSlot(pPlayer, iType, iIndex));
	Res *pPetEggRes = NULL;
	if (!pPetEggItem || !(pPetEggRes = pPetEggItem->getRes()))
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_ITEM_ITEMDATAERROR);
		return false;
	}
	stPetInfo* info = petTable.getEmptyPetSlot(nSlot);
	if(!info)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_PET_INFO_SLOT_FULL);
		return false;
	}
	PetObjectData* pPetData = g_PetRepository.GetPetData(pPetEggRes->getBaseAttribute());
	if(!pPetData)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_PET_DATA_NOT_EXIST);
		return false;
	}

	U32 nSubCategory = pPetEggRes->getSubCategory();

	info->petDataId = pPetEggRes->getBaseAttribute();
	info->style = GETSUB(pPetEggRes->getSubCategory());	//取值范围：[1, 4]

	info->quality = mClamp(pPetEggRes->getQualityLevel(), 0, MAX_PET_QUALITY);
	info->mature = (nSubCategory == Res::CATEGORY_PETEGG_GROWN_NORMAL || nSubCategory == Res::CATEGORY_PETEGG_GROWN_VARIATION);
	
	//成年(变异)蛋 -- 最大耐久度
	if (info->mature)
		info->level = mClamp(pPetEggRes->getMaxWear(), 1, MAX_PET_LEVEL);
	else
		info->level = 1;

	info->sex = gRandGen.randI(0, 1);		// 0 :male   1:female
	info->statsPoints = 0;
	info->insight = pPetData->insight;
	info->petTalent = pPetData->petTalent;
	dStrcpy(info->name, 32, pPetData->petName);

	info->curHP = 0xFFFFFF;
	info->curMP = 0xFFFFFF;
	info->curPP = 0;

	info->staminaGift		= gRandGen.randI(pPetData->minTiPo, pPetData->maxTiPo);
	info->ManaGift			= gRandGen.randI(pPetData->minJingLi, pPetData->maxJingLi);
	info->strengthGift		= gRandGen.randI(pPetData->minStrength, pPetData->maxStrength);
	info->intellectGift		= gRandGen.randI(pPetData->minLingLi, pPetData->maxLingLi);
	info->agilityGift		= gRandGen.randI(pPetData->minMinJie, pPetData->maxMinJie);
	info->pneumaGift		= gRandGen.randI(pPetData->minYuanLi, pPetData->maxYuanLi);
	info->randBuffId		= pPetData->getRandBuffId();
	info->qianLi			= gRandGen.randI(pPetData->minQianLi, pPetData->maxQianLi);
	info->happiness			= 100;
	g_ItemManager->delItemFromInventoryByIndex(pPlayer, iIndex);

	info->staminaPts		= 0;
	info->ManaPts			= 0;
	info->strengthPts		= 0;
	info->intellectPts		= 0;
	info->agilityPts		= 0;
	info->pneumaPts			= 0;

	if (info->level > 1)
	{
		//成年封印或变异成年封印，随机加上六个属性点，总点数为: (info->level - 1) * 5
		S32 nAddPts = (info->level - 1) * 5;
		S32 nAddstaminaPts	=	gRandGen.randI(0, nAddPts);
		info->staminaPts	+=	nAddstaminaPts;
		
		nAddPts				-=	nAddstaminaPts;
		S32 nAddManaPts		=	gRandGen.randI(0, nAddPts);
		info->ManaPts		+=	nAddManaPts;
		
		nAddPts				-=	nAddManaPts;
		S32 nAddstrengthPts	=	gRandGen.randI(0, nAddPts);
		info->strengthPts	+=	nAddManaPts;
		
		nAddPts				-=	nAddstrengthPts;
		S32 nAddintellectPts =	gRandGen.randI(0, nAddPts);
		info->intellectPts	+=	nAddintellectPts;

		nAddPts				-=	nAddintellectPts;
		S32 nAddagilityPts	 =	gRandGen.randI(0, nAddPts);
		info->agilityPts	+=	nAddagilityPts;

		nAddPts				-=	nAddagilityPts;
		info->pneumaPts		+=	nAddPts;
	}

	if (nSubCategory == Res::CATEGORY_PETEGG_VARIATION)
	{
		info->qianLi		+= 6;
	}
	else if (nSubCategory == Res::CATEGORY_PETEGG_GROWN_VARIATION)
	{
		info->qianLi		+= 4;
	}

	info->generation		= 0;

	//增加快乐度计时
	petTable.setPetStartTime(nSlot, Platform::getRealMilliseconds());

	ServerSendUpdatePetSlotMsg(pPlayer, nSlot);
	return true;
}

void PetOperationManager::ServerSendUpdatePetSlotMsg(Player *pPlayer, U32 nSlot,  U32 flag /*= 0xFFFFFFFF*/)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return;

	GameConnection *conn = pPlayer->getControllingClient();
	PetEvent *ev = new PetEvent(nSlot, flag);
	if (conn && ev)
		conn->postNetEvent(ev);
}

bool PetOperationManager::ServerSendPetOptMsg(Player *pPlayer, S32 opType, U32 nSlot, bool bOptSuccess)
{
	if (!pPlayer || nSlot >= PET_MAXSLOTS)
		return false;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		if (opType == Player::PetOp_Identify || opType == Player::PetOp_Insight || 
			opType == Player::PetOp_LianHua || opType == Player::PetOp_BecomeBaby)
		{
			if (bOptSuccess)
				ev->SetIntArgValues(3, opType, nSlot, 2);
			else
				ev->SetIntArgValues(3, opType, nSlot, 3);
		}
		else
			ev->SetIntArgValues(3, opType, nSlot, bOptSuccess);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool PetOperationManager::ServerSendPetOpenMsg(Player *pPlayer, S32 opType)
{
	if (!pPlayer)
		return false;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_PET_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(3, opType, 0, 1);
		return conn->postNetEvent(ev);
	}

	return false;
}

bool PetOperationManager::UsePetFood(Player *pPlayer, U32 nSlot, FeedType nFeedType, S32 iIndex)
{
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	stPetInfo *pPetInfo = (stPetInfo *)petTable.getPetInfo(nSlot);
	ItemShortcut *pItem = dynamic_cast<ItemShortcut*>(pPlayer->inventoryList.GetSlot(iIndex));
	S32 nRewardPts = pItem->getRes()->getReserveValue();
	enWarnMessage msg = g_ItemManager->delItemFromInventoryByIndex(pPlayer, iIndex);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

	switch(nFeedType)
	{
	case FeedTypeFood:
		{
			//增加当前生命	
			PetObjectData *pData = g_PetRepository.GetPetData(pPetInfo->petDataId);
			if (!pData)
				return false;
			PetObject *pPetObject = petTable.getPet(pData->combatPetIdx);
			if (pPetObject)
			{
				pPetObject->addHP(nRewardPts);
			}
			else
			{
				pPetInfo->curHP += nRewardPts;
				ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetOther_Mask);
			}
		}
		break;
	case FeedTypeMedical:
		{
			//增加当前法力
			PetObjectData *pData = g_PetRepository.GetPetData(pPetInfo->petDataId);
			if (!pData)
				return false;
			PetObject *pPetObject = petTable.getPet(pData->combatPetIdx);
			if (pPetObject)
			{
				pPetObject->addMP(nRewardPts);
			}
			else
			{
				pPetInfo->curMP += nRewardPts;
				ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetOther_Mask);
			}
		}
		break;
	case FeedTypeToy:
		{
			pPetInfo->happiness = (pPetInfo->happiness + nRewardPts) < 100 ? (pPetInfo->happiness + nRewardPts) : 100;
			ServerSendUpdatePetSlotMsg(pPlayer, nSlot, stPetInfo::PetHappiness_Mask);
		}
		break;
	default:
		return false;
	}
	
	return true;
}

// ----------------------------------------------------------------------------
// 还原宠物状态到Idle状态,停止相关逻辑行为
bool PetOperationManager::ResetPetIdleStatus(Player *pPlayer, U32 nSlot)
{
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	stPetInfo *pPetInfo = (stPetInfo *)petTable.getPetInfo(nSlot);
	if (!pPetInfo)
		return true;	//宠物不存在，不需要进行下面的操作，并且继续其他槽宠物的ResetPetIdleStatus操作

	switch(pPetInfo->status)
	{
	case PetObject::PetStatus_Learn:
		{
			return true;
		}
		break;
	case PetObject::PetStatus_Combat:
		{
			return DisbandPet(pPlayer, nSlot);
		}
		break;
	default:
		{
			return false;
		}
		break;
	}
	return true;
}


// ----------------------------------------------------------------------------
// 设置宠物冻结状态
void PetOperationManager::SetPetFreeze(Player* pPlayer, bool bEnableFreeze /* = true */)
{
	if(!pPlayer)
		return;

	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	if(bEnableFreeze)
	{
		for(S32 i = 0; i < PET_MAXSLOTS; ++i)
		{			
			if (!ResetPetIdleStatus(pPlayer, i))
			{
				SetPetFreeze(pPlayer, false);
				return;
			}
			petTable.setPetStatus(i, PetObject::PetStatus_Freeze);
			ServerSendUpdatePetSlotMsg(pPlayer, i, stPetInfo::PetStatus_Mask);
		}
	}
	else
	{
		for(S32 i = 0; i < PET_MAXSLOTS; ++i)
		{
			petTable.setPetStatus(i, PetObject::PetStatus_Idle);
			ServerSendUpdatePetSlotMsg(pPlayer, i, stPetInfo::PetStatus_Mask);
		}
	}
}

//服务端宠物添加函数
enWarnMessage PetOperationManager::AddPetInfo(Player *pPlayer, stPetInfo *pPetInfo)
{
	if (!pPetInfo || !pPetInfo->petDataId)
		return MSG_PET_DATA_NOT_EXIST;
	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;

	U32 nSlot = 0;
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();

	stPetInfo *pDestPetInfo = petTable.getEmptyPetSlot(nSlot);
	if (!pDestPetInfo)
		return MSG_PET_INFO_SLOT_FULL;
	*pDestPetInfo = *pPetInfo;
	ServerSendUpdatePetSlotMsg(pPlayer, nSlot);
	return MSG_NONE;
}

//服务端宠物删除函数
enWarnMessage PetOperationManager::DeletePetInfo(Player *pPlayer, U32 nSlot)
{
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(nSlot);
	if (!pPetInfo)
		return MSG_PET_NOT_EXIST;
	if (pPetInfo->status != PetObject::PetStatus_Idle)
		return MSG_PET_STATUS_NOT_IDLE;

	pPetInfo->clear();
	ServerSendUpdatePetSlotMsg(pPlayer, nSlot);
	g_ItemManager->syncPetPanel(pPlayer, nSlot, -1);
	return MSG_NONE;
}

//服务端通知宠物升级与获得经验的消息
bool PetOperationManager::ServerSendPetAddExpMsg(Player *pPlayer, U32 nSlot, U32 nAddedExp)
{
	GameConnection *conn = pPlayer->getControllingClient();
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_PET_OPERATE);
	if (!ev || !conn)
		return false;
	ev->SetIntArgValues(3, Player::PetOp_AddExp,  nSlot, nAddedExp);
	return conn->postNetEvent(ev);
}

bool PetOperationManager::ServerSendPetAddLevelMsg(Player *pPlayer, U32 nSlot, U32 nAddedLevel)
{
	GameConnection *conn = pPlayer->getControllingClient();
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_PET_OPERATE);
	if (!ev || !conn)
		return false;
	ev->SetIntArgValues(3, Player::PetOp_AddLevel, nSlot, nAddedLevel);
	return conn->postNetEvent(ev);
}

void PetOperationManager::InitActivePetObject(Player *pPlayer, U32 nSlot)
{
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	const stPetInfo *pPetInfo = petTable.getPetInfo(nSlot);
	PetObjectData *pData = g_PetRepository.GetPetData(pPetInfo->petDataId);

	PetObject* pPet = new PetObject;
	if(!pPet)
	{
		return;
	}

	pPet->setDataID(pPetInfo->petDataId);
	pPet->setPetSlot(nSlot);
	pPet->mMaster = pPlayer;
	pPet->setLayerID(pPlayer->getLayerID());
	if(!pPet->registerObject())
	{
		pPet->deleteObject();
		return;
	}
	pPet->mMaster = NULL;
	pPet->setMaster(pPlayer);
	pPet->setTransform(pPlayer->getTransform());

	petTable.loadPetInfo(pPet->getPetIndex());
	petTable.setPetStatus(nSlot, PetObject::PetStatus_Combat);
	pPlayer->setMaskBits(Player::PetMask);
}

//用于放生宠物时，当nBeginSlot对应的宠物信息被删除之后，
//把PetTable从nBeginSlot后的所有信息向前移动，并且更新对应的召唤中宠物的槽位
void PetOperationManager::RefreshPetTable(Player *pPlayer, U32 nBeginSlot)
{
	if (!pPlayer)
		return;
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();
	U32 nEndSlot = 0;
	bool bUpdatePetObject = petTable.RefreshPetInfos(nBeginSlot, nEndSlot);
	for (S32 i = nBeginSlot; i <= nEndSlot; i++)
		ServerSendUpdatePetSlotMsg(pPlayer, i);

	if (bUpdatePetObject)
		pPlayer->setMaskBits(Player::PetMask);
}
#endif

void PetOperationManager::ProcessingErrorMessage(Player *pPlayer, U32 nMsgType, enWarnMessage msg)
{
#ifdef NTJ_CLIENT
	MessageEvent::show(nMsgType, msg);
#endif

#ifdef NTJ_SERVER
	MessageEvent::send(pPlayer->getControllingClient(), nMsgType, msg);
#endif
}

#ifdef NTJ_CLIENT
ConsoleFunction(Pet_ChangeName, void, 3, 3, "Pet_ChangeName(%nSlot, %sNewName);")
{
	S32 nSlot = atoi(argv[1]);
	char NameBuf[COMMON_STRING_LENGTH];
	if (!argv[2])
		return;
	dStrcpy(NameBuf, sizeof(NameBuf), argv[2]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetOperationManager::PetChangeName(pPlayer, nSlot, NameBuf);
}

ConsoleFunction(Pet_GetFirstPetSlot, S32, 1, 1, "Pet_GetFirstPetSlot();")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	PetTable &table = (PetTable &)pPlayer->getPetTable();
	for(S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		const stPetInfo *info = table.getPetInfo(i);
		if (info)
			return i;
	}
	return -1;
}


ConsoleFunction(Pet_IsPetExist, bool, 2, 2, "Pet_IsPetExist(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return ((PetTable&)pPlayer->getPetTable()).isPetExist(nSlot);
}

ConsoleFunction(Pet_IsPetSpawned, bool, 2, 2, "Pet_IsPetSpawned(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return ((PetTable&)pPlayer->getPetTable()).isPetSpawned(nSlot);
}

ConsoleFunction(Pet_CanSpawnPet, bool, 2, 2, "Pet_CanSpawnPet(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	PetObjectData *pData = NULL;
	return (PetOperationManager::CanSpawnPet(pPlayer, nSlot, &pData) == MSG_NONE);
}

ConsoleFunction(Pet_CanDisbandPet, bool, 2, 2, "Pet_CanDisbandPet(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return (PetOperationManager::CanDisbandPet(pPlayer, nSlot) == MSG_NONE);
}

ConsoleFunction(Pet_CanReleasePet, bool, 2, 2, "Pet_CanReleasePet(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return (PetOperationManager::CanReleasePet(pPlayer, nSlot, false) == MSG_NONE);
}

ConsoleFunction(Pet_CanXiuXing, bool, 3, 3, "Pet_CanXiuXing(%nSlot, %nPetStudyDataId);")
{
	U32 nSlot = atoi(argv[1]);
	U32 nPetStudyDataId = atoi(argv[2]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return (PetOperationManager::CanPetXiuXing(pPlayer, nSlot, nPetStudyDataId) == MSG_NONE);
}

ConsoleFunction(Pet_CanIdentify, bool, 1, 1, "Pet_CanIdentify()")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetIdentify)
		return false;
	
	PetShortcut *pSlot = dynamic_cast<PetShortcut *>( pPlayer->tempList.GetSlot(0) );
	if (!pSlot)
		return false;
	
	U32 nCostMoney = 0;
	return (PetOperationManager::CanPetIdentify(pPlayer, pSlot->getSlotIndex(), nCostMoney) == MSG_NONE);
}

ConsoleFunction(Pet_CanHuanTong, bool, 1, 1, "Pet_CanHuanTong()")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetHuanTong)
		return false;

	PetShortcut *pSlot = dynamic_cast<PetShortcut *>( pPlayer->tempList.GetSlot(0) );
	if (!pSlot)
		return false;

	U32 nCostMoney = 0;
	return (PetOperationManager::CanPetHuanTong(pPlayer, pSlot->getSlotIndex(), nCostMoney) == MSG_NONE);
}

ConsoleFunction(Pet_CanIncreaseInsight, bool, 1, 1, "Pet_CanIncreaseInsight()")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetInsight)
		return false;

	PetShortcut *pSlot = dynamic_cast<PetShortcut *>( pPlayer->tempList.GetSlot(0) );
	if (!pSlot)
		return false;

	S32 nItemIndex = 0;
	U32 nCostMoney = 0;
	return (PetOperationManager::CanIncreasePetInsight(pPlayer, pSlot->getSlotIndex(), nItemIndex, nCostMoney) == MSG_NONE);
}

ConsoleFunction(Pet_CanLianHua, bool, 1, 1, "Pet_CanLianHua()")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetLianHua)
		return false;

	PetShortcut *pSlot = dynamic_cast<PetShortcut *>( pPlayer->tempList.GetSlot(0) );
	if (!pSlot)
		return false;

	U32 nCostMoney = 0;
	return (PetOperationManager::CanLianHuaPet(pPlayer, pSlot->getSlotIndex(), nCostMoney) == MSG_NONE);
}

ConsoleFunction(Pet_SpawnPet, void, 2, 2, "Pet_SpawnPet(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetOperationManager::SpawnPet(pPlayer, nSlot);
}

ConsoleFunction(Pet_DisbandPet, void, 2, 2, "Pet_DisbandPet(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetOperationManager::DisbandPet(pPlayer, nSlot);
}

ConsoleFunction(Pet_ReleasePet, void, 2, 2, "Pet_ReleasePet();")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetOperationManager::ReleasePet(pPlayer, nSlot);
}

ConsoleFunction(Pet_Identify, void, 1, 1, "Pet_Identify();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetIdentify)
		return;
	PetShortcut *pSlot = dynamic_cast<PetShortcut*>( pPlayer->tempList.GetSlot(0) );
	if (!pSlot)
		return;

	U32 nSlot = pSlot->getSlotIndex();
	PetOperationManager::PetIdentify(pPlayer, nSlot);
}

ConsoleFunction(Pet_IncreaseInsight, void, 1, 1, "Pet_IncreaseInsight();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetInsight)
		return;
	PetShortcut *pSlot = dynamic_cast<PetShortcut*>( pPlayer->tempList.GetSlot(0) );
	if (!pSlot)
		return;

	U32 nSlot = pSlot->getSlotIndex();
	PetOperationManager::IncreasePetInsight(pPlayer, nSlot);
}

ConsoleFunction(Pet_LianHua, void, 1, 1, "Pet_LianHua();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetLianHua)
		return;
	PetShortcut *pSlot = dynamic_cast<PetShortcut*>( pPlayer->tempList.GetSlot(0) );
	if (!pSlot)
		return;

	U32 nSlot = pSlot->getSlotIndex();
	PetOperationManager::LianHuaPet(pPlayer, nSlot);
}

ConsoleFunction(Pet_HuanTong, void, 1, 1, "Pet_HuanTong();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetHuanTong)
		return;
	PetShortcut *pSlot = dynamic_cast<PetShortcut*>( pPlayer->tempList.GetSlot(0) );
	if (!pSlot)
		return;

	U32 nSlot = pSlot->getSlotIndex();
	PetOperationManager::PetHuanTong(pPlayer, nSlot);
}

ConsoleFunction(PetOperation_ChangeProperties, void, 8, 8, "")
{
	//PetOperation_PetChangeProperties(%nSlot, %nTiPo, %nJingLi, %nLiDao, %nLingLi, %nMinJie, %nYuanLi);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetOperationManager::PetChangeProperties(pPlayer, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), 
												atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
}

ConsoleFunction(Pet_Release, void, 2, 2, "Pet_Release(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	PetOperationManager::ReleasePet(pPlayer, nSlot);
}

ConsoleFunction(Pet_XiuXing, void, 3, 3, "Pet_XiuXing(%nSlot, %nPetStudyDataId);")
{
	U32 nSlot = atoi(argv[1]);
	U32 nPetStudyDataId = atoi(argv[2]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetOperationManager::PetXiuXing(pPlayer, nSlot, nPetStudyDataId);
}

ConsoleFunction(Pet_WeiShi, void, 3, 3, "Pet_WeiShi(%nSlot, %nFeedType);")
{
	U32 nSlot = atoi(argv[1]);
	PetOperationManager::FeedType nFeedType = (PetOperationManager::FeedType)atoi(argv[2]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetOperationManager::PetWeiShi_1(pPlayer, nSlot, nFeedType);
}


ConsoleFunction(LianHuaPet_Confirm, void, 2, 2, "LianHuaPet_Confirm(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	PetOperationManager::ClientSendPetOptMsg(pPlayer, Player::PetOp_LianHua, nSlot);
}

ConsoleFunction(ReleasePet_Confirm, void, 2, 2, "ReleasePet_Confirm(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	PetOperationManager::ClientSendPetOptMsg(pPlayer, Player::PetOp_Release, nSlot);
}

ConsoleFunction(PetHuanTong_Confirm, void, 2, 2, "PetHuanTong_Confirm(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	PetOperationManager::ClientSendPetOptMsg(pPlayer, Player::PetOp_BecomeBaby, nSlot);
}

/////////////////////////////////////////客户端界面显示相关脚本///////////////////////////////////////////
ConsoleFunction(PetOperation_GetPetName, StringTableEntry, 2, 2, "PetOperation_GetPetName(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	stPetInfo *pPetInfo = NULL;
	if (!pPlayer)
		return NULL;
	return ((PetTable &)pPlayer->getPetTable()).getPetName(nSlot);
}

ConsoleFunction(PetOperation_GetPetLevel, S32, 2, 2, "PetOperation_GetPetLevel(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	stPetInfo *pPetInfo = NULL;
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetLevel(nSlot);
}

ConsoleFunction(PetOperation_GetPetLife, S32, 2, 2, "PetOperation_GetPetLife(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	stPetInfo *pPetInfo = NULL;
	if (!pPlayer)
		return 0;
	PetTable &table = (PetTable &)pPlayer->getPetTable();
	const stPetInfo *info = table.getPetInfo(nSlot);
	if (!info)
		return 0;
	PetObjectData *pData = g_PetRepository.GetPetData(info->petDataId);
	if (!pData)
		return 0;
	return pData->lives;
}

ConsoleFunction(PetOperation_GetPetSpawnLevel, S32, 2, 2, "PetOperation_GetPetSpawnLevel(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetSpawnLevel(nSlot);
}

ConsoleFunction(PetOperation_GetPetHuiGen, S32, 2, 2, "PetOperation_GetPetHuiGen(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetHuiGen(nSlot);
}

ConsoleFunction(PetOperation_GetPetGenGu, S32, 2, 2, "PetOperation_GetPetGenGu(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetGenGu(nSlot);
}

ConsoleFunction(PetOperation_GetPetTiPo, S32, 2, 2, "PetOperation_GetPetTiPo(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetTiPo(nSlot);
}

ConsoleFunction(PetOperation_GetPetJingLi, S32, 2, 2, "PetOperation_GetPetJingLi(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetJingLi(nSlot);
}

ConsoleFunction(PetOperation_GetPetLiDao, S32, 2, 2, "PetOperation_GetPetLiDao(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetLiDao(nSlot);
}
ConsoleFunction(PetOperation_GetPetLingLi, S32, 2, 2, "PetOperation_GetPetLingLi(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetLingLi(nSlot);
}
ConsoleFunction(PetOperation_GetPetMingJie, S32, 2, 2, "PetOperation_GetPetMingJie(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetMingJie(nSlot);
}
ConsoleFunction(PetOperation_GetPetYuanLi, S32, 2, 2, "PetOperation_GetPetYuanLi(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetYuanLi(nSlot);
}
ConsoleFunction(PetOperation_GetPetQianLi, S32, 2, 2, "PetOperation_GetPetQianLi(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetQianLi(nSlot);
}

ConsoleFunction(PetOperation_GetPetChengZhangLv, S32, 2, 2, "PetOperation_GetPetChengZhangLv(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetChengZhangLv(nSlot);
}

//宠物伤害属性
ConsoleFunction(PetOperation_GetPetWuAttack, S32, 2, 2, "PetOperation_GetPetWuAttack(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetWuAttack(nSlot);
}
ConsoleFunction(PetOperation_GetPetYuanAttack, S32, 2, 2, "PetOperation_GetPetYuanAttack(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetYuanAttack(nSlot);
}
ConsoleFunction(PetOperation_GetPetMuAttack, S32, 2, 2, "PetOperation_GetPetMuAttack(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetMuAttack(nSlot);
}
ConsoleFunction(PetOperation_GetPetHuoAttack, S32, 2, 2, "PetOperation_GetPetHuoAttack(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetHuoAttack(nSlot);
}
ConsoleFunction(PetOperation_GetPetTuAttack, S32, 2, 2, "PetOperation_GetPetTuAttack(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetTuAttack(nSlot);
}
ConsoleFunction(PetOperation_GetPetJinAttack, S32, 2, 2, "PetOperation_GetPetJinAttack(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetJinAttack(nSlot);
}
ConsoleFunction(PetOperation_GetPetShuiAttack, S32, 2, 2, "PetOperation_GetPetShuiAttack(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetShuiAttack(nSlot);
}

//宠物防御属性
ConsoleFunction(PetOperation_GetPetWuDefence, S32, 2, 2, "PetOperation_GetPetWuDefence(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetWuDefence(nSlot);
}
ConsoleFunction(PetOperation_GetPetYuanDefence, S32, 2, 2, "PetOperation_GetPetYuanDefence(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetYuanDefence(nSlot);
}
ConsoleFunction(PetOperation_GetPetMuDefence, S32, 2, 2, "PetOperation_GetPetMuDefence(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetMuDefence(nSlot);
}
ConsoleFunction(PetOperation_GetPetHuoDefence, S32, 2, 2, "PetOperation_GetPetHuoDefence(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetHuoDefence(nSlot);
}
ConsoleFunction(PetOperation_GetPetTuDefence, S32, 2, 2, "PetOperation_GetPetTuDefence(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetTuDefence(nSlot);
}
ConsoleFunction(PetOperation_GetPetJinDefence, S32, 2, 2, "PetOperation_GetPetJinDefence(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetJinDefence(nSlot);
}
ConsoleFunction(PetOperation_GetPetShuiDefence, S32, 2, 2, "PetOperation_GetPetShuiDefence(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetShuiDefence(nSlot);
}

//宠物资质
ConsoleFunction(PetOperation_GetPetStaminaGift, S32, 2, 2, "PetOperation_GetPetStaminaGift(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetStaminaGift(nSlot);
}

ConsoleFunction(PetOperation_GetPetManaGift, S32, 2, 2, "PetOperation_GetPetManaGift(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetManaGift(nSlot);
}

ConsoleFunction(PetOperation_GetPetStrengthGift, S32, 2, 2, "PetOperation_GetPetStrengthGift(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetStrengthGift(nSlot);
}

ConsoleFunction(PetOperation_GetPetIntellectGift, S32, 2, 2, "PetOperation_GetPetIntellectGift(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetIntellectGift(nSlot);
}

ConsoleFunction(PetOperation_GetPetAgilityGift, S32, 2, 2, "PetOperation_GetPetAgilityGift(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetAgilityGift(nSlot);
}

ConsoleFunction(PetOperation_GetPetPneumaGift, S32, 2, 2, "PetOperation_GetPetPneumaGift(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetPneumaGift(nSlot);
}

//宠物其他属性
ConsoleFunction(PetOperation_GetPetZhiLiao, S32, 2, 2, "PetOperation_GetPetZhiLiao(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetZhiLiao(nSlot);
}

ConsoleFunction(PetOperation_GetPetShanBi, S32, 2, 2, "PetOperation_GetPetShanBi(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetShanBi(nSlot);
}

ConsoleFunction(PetOperation_GetPetBaoJi, S32, 2, 2, "PetOperation_GetPetBaoJi(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetBaoJi(nSlot);
}

ConsoleFunction(PetOperation_GetPetBaoJiLv, S32, 2, 2, "PetOperation_GetPetBaoJiLv(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetBaoJiLv(nSlot);
}
ConsoleFunction(PetOperation_GetPetGrowth, S32, 2, 2, "PetOperation_GetPetGrowth(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetGrowth(nSlot);
}

ConsoleFunction(PetOperation_GetPetHappiness, S32, 2, 2, "PetOperation_GetPetHappiness(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetHappiness(nSlot);
}

ConsoleFunction(PetOperation_GetPetGeneration, S32, 2, 2, "PetOperation_GetPetGeneration(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetGeneration(nSlot);
}

ConsoleFunction(PetOperation_GetPetRace, S32, 2, 2, "PetOperation_GetPetRace(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetRace(nSlot);
}

ConsoleFunction(PetOperation_GetPetSex, S32, 2, 2, "PetOperation_GetPetSex(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetSex(nSlot);
}

ConsoleFunction(PetOperation_GetPetStyle, S32, 2, 2, "PetOperation_GetPetStyle(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).getPetStyle(nSlot);
}

ConsoleFunction(PetOperation_GetPetStatus, S32, 2, 2, "PetOperation_GetPetStatus(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	PetObject::enPetStatus status;
	bool bSuccess = ((PetTable &)pPlayer->getPetTable()).getPetStatus(nSlot, status);
	if (!bSuccess)
		return -1;
	return (S32)status;
}

ConsoleFunction(PetOperation_GetPetHp, S32, 2, 2, "PetOperation_GetPetHp(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).GetPetHp(nSlot);
}

ConsoleFunction(PetOperation_GetPetMaxHp, S32, 2, 2, "PetOperation_GetPetMaxHp(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).GetPetMaxHp(nSlot);
}

ConsoleFunction(PetOperation_GetPetMp, S32, 2, 2, "PetOperation_GetPetMp(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).GetPetMp(nSlot);
}

ConsoleFunction(PetOperation_GetPetMaxMp, S32, 2, 2, "PetOperation_GetPetMaxMp(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).GetPetMaxMp(nSlot);
}

ConsoleFunction(PetOperation_GetPetExp, S32, 2, 2, "PetOperation_GetPetExp(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).GetPetExp(nSlot);
}

ConsoleFunction(PetOperation_GetPetMaxExp, S32, 2, 2, "PetOperation_GetPetMaxExp(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return ((PetTable &)pPlayer->getPetTable()).GetPetMaxExp(nSlot);
}

ConsoleFunction(PetOperation_ClosePetIdentify, void, 1, 1, "PetOperation_ClosePetIdentify();")
{
	//发送消息到服务端关闭鉴定
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection *conn = NULL;
	if (ev && pPlayer && (conn = pPlayer->getControllingClient()) )
	{
		ev->SetIntArgValues(2, Player::PetOp_Identify, 2);		// 2 表示关闭
		conn->postNetEvent(ev);
	}
}

ConsoleFunction(PetOperation_ClosePetInsight, void, 1, 1, "PetOperation_ClosePetInsight();")
{
	//发送消息到服务端关闭提高灵慧
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection *conn = NULL;
	if (ev && pPlayer && (conn = pPlayer->getControllingClient()) )
	{
		ev->SetIntArgValues(2, Player::PetOp_Insight, 2);		// 2 表示关闭
		conn->postNetEvent(ev);
	}
}

ConsoleFunction(PetOperation_ClosePetLianHua, void, 1, 1, "PetOperation_ClosePetLianHua();")
{
	//发送消息到服务端关闭炼化
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection *conn = NULL;
	if (ev && pPlayer && (conn = pPlayer->getControllingClient()) )
	{
		ev->SetIntArgValues(2, Player::PetOp_LianHua, 2);		// 2 表示关闭
		conn->postNetEvent(ev);
	}
}

ConsoleFunction(PetOperation_ClosePetHuanTong, void, 1, 1, "PetOperation_ClosePetHuanTong();")
{
	//发送消息到服务端关闭炼化
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_PET_OPERATE);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection *conn = NULL;
	if (ev && pPlayer && (conn = pPlayer->getControllingClient()) )
	{
		ev->SetIntArgValues(2, Player::PetOp_BecomeBaby, 2);		// 2 表示关闭
		conn->postNetEvent(ev);
	}
}

ConsoleFunction(PetOperation_PetLianHua_InitGenGuDan, S32, 1, 1, "PetOperation_PetLianHua_InitGenGuDan();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetLianHua)
		return 0;
	PetShortcut *pPetSlot = dynamic_cast<PetShortcut *>(pPlayer->tempList.GetSlot(0));
	if (!pPetSlot)
	{
		pPlayer->tempList.SetSlot(1, NULL);
		return 0;
	}
	const stPetInfo *pPetInfo = pPlayer->getPetTable().getPetInfo(pPetSlot->getSlotIndex());
	if (!pPetInfo)
	{
		pPlayer->tempList.SetSlot(1, NULL);
		return 0;
	}
	U32 nGenGuLevel = atoi( Con::executef("GetGenGuLevelByInsight", Con::getIntArg(pPetInfo->insight)) );
	if (nGenGuLevel == 0)
	{
		pPlayer->tempList.SetSlot(1, NULL);
		return 0;
	}
	U32 nGenGuId = 116070000 + nGenGuLevel;
	ItemShortcut *pGenGuDan = ItemShortcut::CreateItem(nGenGuId, 1);
	if (!pGenGuDan)
	{
		pPlayer->tempList.SetSlot(1, NULL);
		return 0;
	}
	pPlayer->tempList.SetSlot(1, pGenGuDan);
	return nGenGuId;
}

ConsoleFunction(PetOperation_PetInsight_InitGenGuDan, S32, 1, 1, "PetOperation_PetInsight_InitGenGuDan();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || pPlayer->tempList.GetOptType() != TempList::OptType_PetInsight)
		return 0;
	PetShortcut *pPetSlot = dynamic_cast<PetShortcut *>(pPlayer->tempList.GetSlot(0));
	if (!pPetSlot)
	{
		pPlayer->tempList.SetSlot(1, NULL);
		return 0;
	}
	const stPetInfo *pPetInfo = pPlayer->getPetTable().getPetInfo(pPetSlot->getSlotIndex());
	if (!pPetInfo)
	{
		pPlayer->tempList.SetSlot(1, NULL);
		return 0;
	}
	U32 nGenGuLevel = atoi( Con::executef("GetGenGuLevelByInsight", Con::getIntArg(pPetInfo->insight)) );
	if (nGenGuLevel == 0)
	{
		pPlayer->tempList.SetSlot(1, NULL);
		return 0;
	}
	U32 nGenGuID = 116070000 + nGenGuLevel;
	ItemShortcut *pGenGuDan = ItemShortcut::CreateItem(nGenGuID, 1);
	if (!pGenGuDan)
	{
		pPlayer->tempList.SetSlot(1, NULL);
		return 0;
	}
	pPlayer->tempList.SetSlot(1, pGenGuDan);
	return nGenGuID;
}

ConsoleFunction(PetOperation_DoPetIdentify, void, 2, 2, "PetOperation_DoPetIdentify(%nSlot);")
{
	U32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (pPlayer && nSlot < PET_MAXSLOTS)
	{
		PetOperationManager::PetIdentify(pPlayer, nSlot);
	}
}

ConsoleFunction(PetHelpWnd_GetSlotIndex, S32, 2, 2, "PetHelpWnd_GetSlotIndex(%nIndex);")
{
	S32 nSlotIndex = -1;
	S32 nIndex = atoi(argv[1]);	
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (pPlayer &&  (pPlayer->tempList.GetOptType() == TempList::OptType_PetIdentify) ||
					(pPlayer->tempList.GetOptType() == TempList::OptType_PetInsight) ||
					(pPlayer->tempList.GetOptType() == TempList::OptType_PetLianHua) )
	{
		PetShortcut *pPetSlot = dynamic_cast<PetShortcut*>(pPlayer->mPetHelpList.GetSlot(nIndex));
		if (pPetSlot)
			nSlotIndex = pPetSlot->getSlotIndex();
	}
	return nSlotIndex;
}

ConsoleFunction(PetBackToYoungWnd_GetSlotIndex, S32, 1, 1, "PetBackToYoungWnd_GetSlotIndex();")
{
	S32 nSlotIndex = -1;
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (pPlayer &&  (pPlayer->tempList.GetOptType() == TempList::OptType_PetHuanTong))
	{
		PetShortcut *pPetSlot = dynamic_cast<PetShortcut*>(pPlayer->tempList.GetSlot(0));
		if (pPetSlot)
			nSlotIndex = pPetSlot->getSlotIndex();
	}
	return nSlotIndex;
}

ConsoleFunction(PetCheckupWnd_GetSlotIndex, S32, 1, 1, "PetCheckupWnd_GetSlotIndex();")
{
	S32 nSlotIndex = -1;
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (pPlayer && pPlayer->tempList.GetOptType() == TempList::OptType_PetIdentify)
	{
		PetShortcut *pPetSlot = dynamic_cast<PetShortcut*>(pPlayer->tempList.GetSlot(0));
		if (pPetSlot)
			nSlotIndex = pPetSlot->getSlotIndex();
	}
	return nSlotIndex;
}

ConsoleFunction(PetStrengthenWnd_GetSlotIndex, S32, 1, 1, "PetStrengthenWnd_GetSlotIndex();")
{
	S32 nSlotIndex = -1;
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (pPlayer && pPlayer->tempList.GetOptType() == TempList::OptType_PetInsight)
	{
		PetShortcut *pPetSlot = dynamic_cast<PetShortcut*>(pPlayer->tempList.GetSlot(0));
		if (pPetSlot)
			nSlotIndex = pPetSlot->getSlotIndex();
	}
	return nSlotIndex;
}

ConsoleFunction(PetDecompoundWnd_GetSlotIndex, S32, 1, 1, "PetDecompoundWnd_GetSlotIndex();")
{
	S32 nSlotIndex = -1;
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (pPlayer && pPlayer->tempList.GetOptType() == TempList::OptType_PetLianHua)
	{
		PetShortcut *pPetSlot = dynamic_cast<PetShortcut*>(pPlayer->tempList.GetSlot(0));
		if (pPetSlot)
			nSlotIndex = pPetSlot->getSlotIndex();
	}
	return nSlotIndex;
}
#endif


/////////////////////////////////////////服务端界面显示相关脚本///////////////////////////////////////////
/////////////////////////////////////////这些界面操作均与NPC有关///////////////////////////////////////////
#ifdef NTJ_SERVER
//通过Npc对话打开宠物鉴定
ConsoleMethod(Player, PetOperation_OpenPetIdentify, void, 3, 3, "%player.PetOperation_OpenPetIdentify(%npc);")
{
	U32 nNpcId = atoi(argv[2]);
	NpcObject *pNpc = dynamic_cast<NpcObject*>( Sim::findObject(nNpcId) );
	if (pNpc)
	{
		PetOperationManager::OpenPetIdentify(object, pNpc);
	}
}

//通过Npc对话打开提高宠物灵慧
ConsoleMethod(Player, PetOperation_OpenPetInsight, void, 3, 3, "%player.PetOperation_OpenPetInsight(%npc);")
{
	U32 nNpcId = atoi(argv[2]);
	NpcObject *pNpc = dynamic_cast<NpcObject*>( Sim::findObject(nNpcId) );
	if (pNpc)
	{
		PetOperationManager::OpenPetInsight(object, pNpc);
	}
}

//通过Npc对话打开宠物炼化
ConsoleMethod(Player, PetOperation_OpenPetLianHua, void, 3, 3, "%player.PetOperation_OpenPetLianHua(%npc);")
{
	U32 nNpcId = atoi(argv[2]);
	NpcObject *pNpc = dynamic_cast<NpcObject*>( Sim::findObject(nNpcId) );
	if (pNpc)
	{
		PetOperationManager::OpenPetLianHua(object, pNpc);
	}
}

//通过Npc对话打开宠物还童
ConsoleMethod(Player, PetOperation_OpenPetHuanTong, void, 3, 3, "%player.PetOperation_OpenPetHuanTong(%npc);")
{
	U32 nNpcId = atoi(argv[2]);
	NpcObject *pNpc = dynamic_cast<NpcObject*>( Sim::findObject(nNpcId) );
	if (pNpc)
	{
		PetOperationManager::OpenPetHuanTong(object, pNpc);
	}
}
#endif

//得到当前召唤的宠物位置
ConsoleMethod( Player, GetSpawnedPetSlot, S32, 2, 2, "%player.GetSpawnedPetSlot();" )
{
    U32 slot = atoi(argv[2]);
    U32 exp = atoi(argv[3]);

    Player *pPlayer = (Player *)object;
    PetTable &petTable = (PetTable &)pPlayer->getPetTable();

    return petTable.getSpawnPetSlot();
}

//得到当前召唤的宠物ID
ConsoleMethod( Player, GetSpawnedPet, S32, 2, 2, "%player.GetSpawnedPet();" )
{
	U32 slot = atoi(argv[2]);
	U32 exp = atoi(argv[3]);

	Player *pPlayer = (Player *)object;
	PetTable &petTable = (PetTable &)pPlayer->getPetTable();

	return petTable.getSpawnPetID();
}

/////////////////////////////////////////服务端调用与宠物升级相关///////////////////////////////////////////
#ifdef NTJ_SERVER
//增加宠物经验
ConsoleMethod( Player, AddPetExp, void, 4, 4, "%player.AddPetExp(%slot, %exp);" )
{
	U32 slot = atoi(argv[2]);
	U32 exp = atoi(argv[3]);

	Player *pPlayer = (Player *)object;
	((PetTable &)pPlayer->getPetTable()).addExp(slot, exp, pPlayer);
}

//给宠物升级
ConsoleMethod( Player, AddPetLevel, void, 4, 4, "%player.AddPetLevel(%slot, %level);" )
{
	U32 slot = atoi(argv[2]);
	U32 level = atoi(argv[3]);

	Player *pPlayer = (Player *)object;
	((PetTable &)pPlayer->getPetTable()).addLevel(slot, level, pPlayer);
}
#endif

#ifdef NTJ_SERVER
//这两个函数只是用于测试方便
ConsoleFunction(PetOperation_TestOnly_ReleasePet, void, 3, 3, "PetOperation_TestOnly_ReleasePet(%player, %nSlot);")
{
	U32 nPlayerId = atoi(argv[1]);
	U32 nSlot = atoi(argv[2]);
	Player *pPlayer = g_ServerGameplayState->GetPlayer(nPlayerId);
	if (!pPlayer)
		return;

	stPetInfo *pPetInfo = (stPetInfo*)pPlayer->getPetTable().getPetInfo(nSlot);
	pPetInfo->clear();
	PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, nSlot);
}

ConsoleFunction(PetOperation_TestOnly_SetPetQianLi, void, 4, 4, "PetOperation_TestOnly_SetPetQianLi(%player, %nSlot, %nQianLi);")
{
	U32 nPlayerId = atoi(argv[1]);
	U32 nSlot = atoi(argv[2]);
	S32 nQianLi = atoi(argv[3]);
	Player *pPlayer = g_ServerGameplayState->GetPlayer(nPlayerId);
	if (!pPlayer)
		return;

	stPetInfo *pPetInfo = (stPetInfo*)pPlayer->getPetTable().getPetInfo(nSlot);
	pPetInfo->statsPoints = nQianLi;
	PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, nSlot);
}
#endif