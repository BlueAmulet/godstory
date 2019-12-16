//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/Item/GatherMaterial.h"

GatherMaterial g_GatherMaterial;
GatherMaterial::GatherMaterial()
{

}

enWarnMessage GatherMaterial::canGather(Player* pPlayer,U32 LivingSkillId)
{
	if(!pPlayer || !pPlayer->pLivingSkill)
		return MSG_UNKOWNERROR;

	LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(LivingSkillId);
	if(!pData)
		return MSG_LIVINGSKILL_DATAERROR;

	U16 iSUB = GETSUBCATEGORY(LivingSkillId);
	LivingSkillTable::LivingSkillTableMap& SkillTabMap =  pPlayer->pLivingSkill->getLivingSkillTable();
	LivingSkillTable::LivingSkillTableMap::iterator it = SkillTabMap.find(iSUB);
	//是否学习该类技能
	if(it == SkillTabMap.end())
	{
		switch(iSUB)
		{
		case LivingSkillData::GATHER_MINING:
			{
				return MSG_LIVINGSKILL_NOMINING;
			}
			break;
		case LivingSkillData::GATHER_FELLING:
			{
				return MSG_LIVINGSKILL_NOFELLING;
			}
			break;
		case LivingSkillData::GATHER_FISHING:
			{
				return MSG_LIVINGSKILL_FISHING;
			}
			break;
		case LivingSkillData::GATHER_PLANTING:
			{
				return MSG_LIVINGSKILL_PLANTING;
			}
			break;
		case LivingSkillData::GATHER_HERB:
			{
				return MSG_LIVINGSKILL_HERB;
			}
			break;
		case LivingSkillData::GATHER_SHIKAR:
			{
				return MSG_LIVINGSKILL_SHIKAR;
			}
			break;
		default:
			return MSG_UNKOWNERROR;
		}
	}

	LivingSkill* pSkill  = it->second;
	if(!pSkill || !pSkill->getData())
		return MSG_LIVINGSKILL_DATAERROR;

	//是否满足采集资源技能限制
	if(pData->getLevel() > pSkill->getData()->getLevel())
	{
		switch(iSUB)
		{
		case LivingSkillData::GATHER_MINING:
			{
				return MSG_LIVINGSKILL_NOMININGLOW;
			}
			break;
		case LivingSkillData::GATHER_FELLING:
			{
				return MSG_LIVINGSKILL_NOFELLINGLOW;
			}
			break;
		case LivingSkillData::GATHER_FISHING:
			{
				return MSG_LIVINGSKILL_FISHINGLOW;
			}
			break;
		case LivingSkillData::GATHER_PLANTING:
			{
				return MSG_LIVINGSKILL_PLANTINGLOW;
			}
			break;
		case LivingSkillData::GATHER_HERB:
			{
				return MSG_LIVINGSKILL_HERBLOW;
			}
			break;
		case LivingSkillData::GATHER_SHIKAR:
			{
				return MSG_LIVINGSKILL_SHIKARLOW;
			}
			break;
		default:
			return MSG_UNKOWNERROR;
		}
	}

	//活力
	LivingSkillData::Cost& pCost = pData->getCost();
	if(pPlayer->getVigor() < pCost.vigour)
		return MSG_PLAYER_VIGOUR;

	//道具消耗
	if (pCost.mode != LivingSkillData::CAST_NOTHING)
	{
		ShortcutObject*  pShortcut = pPlayer->equipList.GetSlot(Res::EQUIPPLACEFLAG_GATHER);
		if(!pShortcut)
			return MSG_ITEM_TOOLS_NOMOUNT;

		Res* pRes = NULL;
		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
		if(!pItem || !(pRes = pItem->getRes()))
			return MSG_ITEM_ITEMDATAERROR;
		//是否合适的采集工具
		if(pCost.itemSubCategory != pItem->getRes()->getSubCategory())
			return MSG_ITEM_TOOLS_NOEQUAL;
	}

	return MSG_NONE;
}

enWarnMessage GatherMaterial::spellLivingSkill(Player* pPlayer,CollectionObject* obj)
{
	if(!pPlayer || !obj || !obj->mDataBlock)
		return MSG_UNKOWNERROR;

	EventTriggerData  *pEventTriggerData = gEventTriggerRepository.getEventTriggerData(obj->mDataBlock->mEventTriggerId);
	if(!pEventTriggerData)
		return MSG_UNKOWNERROR;

	LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(pEventTriggerData->mLivingSkillId);
	if(!pData)
		return MSG_LIVINGSKILL_DATAERROR;

	LivingSkillData::Cost& pCost = pData->getCost();
	//处理cd时间(以后加)

	//放动画，特效
	return MSG_NONE;
}
#ifdef NTJ_SERVER
enWarnMessage GatherMaterial::gatherCast(Player* pPlayer,U32 LivingSkillId)
{
	if(!pPlayer || !pPlayer->pLivingSkill)
		return MSG_UNKOWNERROR;

	enWarnMessage msg = canGather(pPlayer,LivingSkillId);
	if(msg != MSG_NONE)
		return msg;

	LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(LivingSkillId);
	if(!pData)
		return MSG_LIVINGSKILL_DATAERROR;

	LivingSkillData::Cost& pCost = pData->getCost();
	//活力
	S32 tempVigour = pCost.vigour;
	S32 iVigour = pPlayer->getVigor() - tempVigour;
	if(iVigour < 0)
		return MSG_PLAYER_VIGOUR;
	else
		pPlayer->addVigor(-tempVigour);
	//扣道具
	Res* pRes = NULL;
	if (pCost.mode != LivingSkillData::CAST_NOTHING)
	{
		ShortcutObject*  pShortcut = pPlayer->equipList.GetSlot(Res::EQUIPPLACEFLAG_GATHER);
		if(!pShortcut)
			return MSG_ITEM_TOOLS_NOMOUNT;

		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
		if(!pItem || !(pRes= pItem->getRes()))
			return MSG_ITEM_ITEMDATAERROR;
	}
	
	//使用次数
	if (pCost.mode == LivingSkillData::CAST_ITEMUSETIMES)
	{
		GameConnection* conn = pPlayer->getControllingClient();
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

		pPlayer->equipList.UpdateToClient(conn,Res::EQUIPPLACEFLAG_GATHER,ITEM_NOSHOW);
	}
	//奖励
	LivingSkillData::Guerdon& pGuerdon = pData->getGuerdon();
	if(pGuerdon.num > 0 && pGuerdon.type == LivingSkillData::GUERDON_RIPE)
	{
		//生活技能等级对应的资源点加熟练度
		if (pPlayer->getLivingSkillRipe(LivingSkillId) < pData->getRipe())
			pPlayer->setLivingSkillGuerdon(LivingSkillId);
	}

	return MSG_NONE;
}
#endif
