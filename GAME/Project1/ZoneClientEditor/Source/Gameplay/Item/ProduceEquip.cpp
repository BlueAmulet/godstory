//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/Item/Prescription.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/Item/ProduceEquip.h"

ProduceEquip g_ProduceEquip;
ProduceEquip::ProduceEquip()
{

}

enWarnMessage ProduceEquip::canProduce(Player* pPlayer,U32 SerialID)
{
	if(!pPlayer || !pPlayer->pPrescription)
		return MSG_UNKOWNERROR;

	Prescription::PrescriptionMap& preTabMap = pPlayer->pPrescription->getPrescriptionTab();
	Prescription::PrescriptionMap::iterator  it = preTabMap.find(SerialID);
	if(it == preTabMap.end())
		return MSG_UNKOWNERROR;

	PrescriptionData* pData = g_PrescriptionRepository.getPrescriptionData(SerialID);
	if(!pData)
		return MSG_PRESCRIPTION_DATAERROR;

	PrescriptionData::stMaterial* pMaterial = pData->getMaterial();
	if(!pMaterial)
		return MSG_PRESCRIPTION_MATERIALERROR;

	//交互状态
	enWarnMessage msg = MSG_NONE;
	msg = pPlayer->isBusy(Player::INTERACTION_NONE);
	if(msg != MSG_NONE)
		return msg;

	//坐骑状态
	if(pPlayer->isMounted())
		return MSG_PLAYER_RIDE;
	
	//前置脚本判断
	if(pData->getScript())
	{
		msg =(enWarnMessage)atoi(Con::executef("ProduceCheck",Con::getIntArg(pPlayer->getId()),Con::getIntArg(SerialID)));
		if(msg != MSG_NONE)
			return msg;
	}

	LivingSkillData* pSkillData = g_LivingSkillRespository.getLivingSkillData(pData->getLivingSkillID());
	if(!pSkillData)
		return MSG_LIVINGSKILL_DATAERROR;
	//道具判断
	LivingSkillData::Cost& pCost = pSkillData->getCost();
	if(pCost.mode != LivingSkillData::CAST_NOTHING)
	{
		ShortcutObject*  pShortcut = pPlayer->equipList.GetSlot(Res::EQUIPPLACEFLAG_GATHER);
		if(!pShortcut)
			return MSG_ITEM_TOOLS_NOMOUNT;

		Res* pRes = NULL;
		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
		if(!pItem || !(pRes = pItem->getRes()))
			return MSG_ITEM_ITEMDATAERROR;
		//是否合适的工具
		if(pData->getToolsType() != pItem->getRes()->getSubCategory())
			return MSG_ITEM_TOOLS_NOEQUAL;
	}

	if(pPlayer->getMoney() < pData->getMoney())
		return MSG_PLAYER_MONEYNOTENOUGH;

	if(pPlayer->GetStats().Insight < pData->getInsight())
		return MSG_PLAYER_INSIGHT;

	if(pPlayer->getVigor() < pData->getVigour())
		return MSG_PLAYER_VIGOUR;

	if(pPlayer->pPrescription->canCompose(pPlayer,SerialID) == 0)
		return MSG_PLAYER_MATERIAL_NOTENOUGH;

	if(pPlayer->inventoryList.GetEmptyCount() == 0)
		return MSG_ITEM_ITEMNOSPACE;

	return MSG_NONE;
}

bool ProduceEquip::Produce(Player* pPlayer,U32 SerialId,S32 Count)
{
	if(!pPlayer || Count <= 0)
		return false;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return false;

	enWarnMessage msg = canProduce(pPlayer,SerialId);;
#ifdef NTJ_CLIENT
	if(msg == MSG_NONE)
		return sendCurrentEvent(pPlayer,SerialId,Count);
	else
		MessageEvent::show(SHOWTYPE_ERROR,msg);
#endif

#ifdef NTJ_SERVER
	if(msg == MSG_NONE)
	{
		PrescriptionData* pData = g_PrescriptionRepository.getPrescriptionData(SerialId);
		if(!pData)
			return false;

		PrescriptionData::stMaterial* pMaterial = pData->getMaterial();
		if(!pMaterial)
			return false;

		LivingSkillData* pSkill = g_LivingSkillRespository.getLivingSkillData(pData->getLivingSkillID());
		if(!pSkill)
			return false;

		//道具判断
		Res* pRes = NULL;
		ItemShortcut* pItem = NULL;
		LivingSkillData::Cost& pCost = pSkill->getCost();
		if(pCost.mode != LivingSkillData::CAST_NOTHING)
		{
			ShortcutObject*  pShortcut = pPlayer->equipList.GetSlot(Res::EQUIPPLACEFLAG_GATHER);
			if(!pShortcut)
				return false;

			pItem = dynamic_cast<ItemShortcut*>(pShortcut);
			if(!pItem || !(pRes= pItem->getRes()))
				return false;
		}

		msg = MSG_PRESCRIPTION_ERROR;//防止脚本没有加载
		msg = (enWarnMessage)atoi(Con::executef("ProduceAll",Con::getIntArg(pPlayer->getId()),Con::getIntArg(SerialId)));
		//扣道具使用次数
		if (msg == MSG_NONE)
		{
			if(pCost.mode == LivingSkillData::CAST_ITEMUSETIMES)
			{
				U16 iRemain = pRes->getUsedTimes() - pCost.itemUseTimes;
				if(iRemain>0)
				{
					pRes->setUsedTimes(iRemain);
				}
				else
				{
					MessageEvent::send(conn,SHOWTYPE_NOTIFY,avar("您失去了%s",pRes->getItemName()),SHOWPOS_SCRANDCHAT);
					pPlayer->equipList.SetSlot(Res::EQUIPPLACEFLAG_GATHER, NULL, true);
				}
			}
			return true;
		}
	}

	MessageEvent::send(conn,SHOWTYPE_ERROR,msg);
#endif
	return false;
}

void ProduceEquip::finishProduce(Player* pPlayer,U32 SerialId,S32 Count)
{
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

#ifdef NTJ_CLIENT
	Con::executef("CloseFetchTimeProgress");
	Con::executef("setProduceButton",Con::getIntArg(1));
#endif

#ifdef NTJ_SERVER
	ServerGameNetEvent* Event = new ServerGameNetEvent(INFO_PRESCRIPTION_PRODUCE);
	Event->SetInt32ArgValues(1,SerialId);
	Event->SetIntArgValues(2,ProduceEquip::PRODUCE_FINISHED,0);
	conn->postNetEvent(Event);
#endif

}

void ProduceEquip::cancelProduce(Player* pPlayer,U32 SerialId,S32 Count)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return;

#ifdef NTJ_CLIENT
	Con::executef("cancleFetchTimeProgress");
	Con::executef("setProduceButton",Con::getIntArg(1));
#endif

#ifdef NTJ_SERVER
	ServerGameNetEvent* Event = new ServerGameNetEvent(INFO_PRESCRIPTION_PRODUCE);
	Event->SetInt32ArgValues(1,SerialId);
	Event->SetIntArgValues(2,ProduceEquip::PRODUCE_CANCEL,Count);
	conn->postNetEvent(Event);
#endif

}
bool ProduceEquip::sendCurrentEvent(Player* pPlayer,U32 SerialId,S32 Count)
{
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return false;

#ifdef NTJ_CLIENT
	if(!g_ClientGameplayState->pVocalStatus)
		return false;

	if(g_ClientGameplayState->pVocalStatus->getStatus() && g_ClientGameplayState->pVocalStatus->getStatus() == VocalStatus::VOCALSTATUS_COMPOSE)
		Con::executef("CloseFetchTimeProgress");

	PrescriptionData* pData = g_PrescriptionRepository.getPrescriptionData(SerialId);
	if(!pData)
		return false;

	LivingSkillData* pLivingSkillData = g_LivingSkillRespository.getLivingSkillData(pData->getLivingSkillID());
	if(!pLivingSkillData)
		return false;

	LivingSkillData::Cast& pCast = pLivingSkillData->getCast();
	SimTime  vocalTime = pCast.readyTime;
	if(vocalTime <= 0)
	{
		AssertFatal(vocalTime>0,"LivingSkillData::readyTime=0");
		return false;
	}

	ClientGameNetEvent* Event = new ClientGameNetEvent(INFO_PRESCRIPTION_PRODUCE);
	Event->SetInt32ArgValues(1,SerialId);
	Event->SetIntArgValues(2,ProduceEquip::PRODUCE_START,Count); 
	if(conn->postNetEvent(Event))
	{
		Con::executef("setProduceButton",Con::getIntArg(0));
		Con::executef("InitFetchTimeProgress",Con::getIntArg(pCast.readyTime),Con::getIntArg((S32)VocalStatus::VOCALSTATUS_COMPOSE),"");
	}
	else
	{
		Con::executef("setProduceButton",Con::getIntArg(1));
		return false;
	}
#endif

#ifdef NTJ_SERVER
	ServerGameNetEvent* Event = new ServerGameNetEvent(INFO_PRESCRIPTION_PRODUCE);
	Event->SetInt32ArgValues(1,SerialId);
	Event->SetIntArgValues(2,ProduceEquip::PRODUCE_PROCESS,Count);
	conn->postNetEvent(Event);
#endif
	return true;
}

#ifdef NTJ_CLIENT
void ProduceEquip::refreshUI(Player* pPlayer,PrescriptionData* pData)
{
	if(!pPlayer || !pData)
		return;

	U32 skillId = pData->getLivingSkillID(); 
	LivingSkillTable::LivingSkillTableMap& tabMap  = pPlayer->pLivingSkill->getLivingSkillTable();
	LivingSkillTable::LivingSkillTableMap::iterator it  = tabMap.find(GETSUBCATEGORY(skillId));

	if(it != tabMap.end())
	{
		LivingSkill* pSkill = it->second;
		if(!pSkill || !pSkill->getData())
			return;

		LivingSkillData* pLivingSkillData = pSkill->getData();
		if(pLivingSkillData)
		{
			U32 count = pPlayer->pPrescription->canCompose(pPlayer,pData->getSerialID());
			count =  getMin(count,pPlayer->getVigor() / pData->getVigour());

			Con::executef("RefreshProduceinfo",
				Con::getIntArg(pPlayer->getLivingSkillRipe(skillId)),
				Con::getIntArg(pLivingSkillData->getRipe()),
				Con::getIntArg(pPlayer->getVigor()),
				Con::getIntArg(pPlayer->getMaxVigor()),
				avar("%s",pLivingSkillData->getName()),
				Con::getIntArg(pLivingSkillData->getLevel()),
				Con::getIntArg(count),
				Con::getIntArg(pLivingSkillData->getCategory()-1));
		}

	}
}
#endif
