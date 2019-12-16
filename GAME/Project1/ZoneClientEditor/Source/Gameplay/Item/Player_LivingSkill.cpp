//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/item/Res.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Player_GemMount.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/LivingSkillShortcut.h"
#include "Gameplay/Item/Prescription.h"
// ----------------------------------------------------------------------------
// 生活技能栏
// ----------------------------------------------------------------------------
#ifdef NTJ_CLIENT
LivingSkillList::LivingSkillList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_LIVINGSKILL;
}

bool LivingSkillList::AddLivingSkillShortcut(U32 skillID,U32 ripe)
{
	LivingSkillShortcut* pShortcut = NULL;
	LivingSkill* pSkill = NULL; 
	LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(skillID);
	if(!pData)
		return false;

	for (int i=1; i<mMaxSlots; i++)
	{
		//0槽位放大类生活技能
		if(pData->getID() < LIVINGSKILL_ID_LIMIT)
		{
			Reset();
			break;
		}

		pShortcut = dynamic_cast<LivingSkillShortcut*>(mSlots[i]);
		if(pShortcut &&  (pSkill = pShortcut->getLivingSkill()) && pSkill->getData())
		{
			if (pSkill->getData()->getID() == skillID)
			{
				setState(i,pData);
				return true;
			}
			//处理专精技能
			if (pData->getType() == LivingSkillData::LIVINGSKILL_PRO)
			{
				if(pSkill->getData()->getType() == LivingSkillData::LIVINGSKILL_NORM)
					continue;		
				else if(pSkill->getData()->getSubCategory() != GETSUBCATEGORY(skillID) || (pSkill->getData()->getSubCategory() == GETSUBCATEGORY(skillID) 
					&& pSkill->getData()->getLevel() >= pData->getLevel()))
					continue;
				else
				{
					delete mSlots[i];
					mSlots[i] = NULL;
					break;
				}
			}
			else//处理普能技能
			{
				if(pSkill->getData()->getType() == LivingSkillData::LIVINGSKILL_PRO)
					continue;
				else if(pSkill->getData()->getSubCategory() != GETSUBCATEGORY(skillID) || (pSkill->getData()->getSubCategory() == GETSUBCATEGORY(skillID) 
					&& pSkill->getData()->getLevel() >= pData->getLevel()))
					continue;
				else
				{
					delete mSlots[i];
					mSlots[i] = NULL;
					break;
				}
			}		
		}
	}

	S32 slot = FindEmptySlot();
	if (slot != -1)
	{
		mSlots[slot] = LivingSkillShortcut::Create(skillID,ripe);
		setState(slot,pData);
		return true;
	}
	return false;
}
void LivingSkillList::setState(U32 slotIndex, LivingSkillData* pData)
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!pData || !conn)
		return;

	Player* pPlayer = dynamic_cast<Player*>(conn->getControlObject());
	if(!pPlayer)
		return;

	char index[8]  = {0,};
	char level[8]  = {0,};
	char name [32] = {0,};
	char curRipe[32]  = {0,};
	char maxRipe[32]  = {0,};
	char strDes[512] = {0,};

	dSprintf(index,sizeof(index),"%d",slotIndex);
	dSprintf(strDes,sizeof(strDes),"%s",pData->getDesc());

	if(pData->getID() < LIVINGSKILL_ID_LIMIT) 
	{
		Con::executef("UI_ShowLivingSkillInfo",index,strDes,"0","0","0");
		return;
	}

	LivingSkillTable::LivingSkillTableMap::iterator it = pPlayer->pLivingSkill->getLivingSkillTable().find(GETSUBCATEGORY(pData->getID()));
	if(it !=  pPlayer->pLivingSkill->getLivingSkillTable().end())
	{
		LivingSkill* pSkill = it->second;
		if(pSkill && pSkill->getData() && pSkill->getData()->getID() == pData->getID())
			mSlots[slotIndex]->setSlotState(ShortcutObject::SLOT_COMMON);
		else 
			mSlots[slotIndex]->setSlotState(ShortcutObject::SLOT_LOCK);

		dSprintf(curRipe,sizeof(curRipe),"%d",pSkill->getRipe());
		dSprintf(level,sizeof(level),"%d",pData->getLevel());
	}
	else
	{
		mSlots[slotIndex]->setSlotState(ShortcutObject::SLOT_LOCK);
		dSprintf(curRipe,sizeof(curRipe),"%d",0);
		dSprintf(level,sizeof(level),"%d",0);

	}

	//更新界面技能信息
	dSprintf(name,sizeof(name),"%s",pData->getName());
	//dSprintf(level,sizeof(level),"%d",pData->getLevel());
	dSprintf(maxRipe,sizeof(maxRipe),"%d",pData->getRipe());
	Con::executef("UI_ShowLivingSkillInfo",Con::getIntArg(pData->getCategory()-1),index,name,level,curRipe,maxRipe);
	Con::executef("UpdataLivingSkillStudyList");
}
#endif

#ifdef NTJ_SERVER
enWarnMessage ItemManager::LivingSkillMoveToPanel(stExChangeParam* param)
{
	Player* pPlayer = param->player;
	if(!pPlayer)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	if(!pPlayer->panelList.IsVaildSlot(param->DestIndex))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;	
	if(param->SkillSeriesId < LIVINGSKILL_ID_CATEGORYLIMIT && param->SkillSeriesId >= LIVINGSKILL_ID_LIMIT)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	pPlayer->panelList.SetSlot(param->DestIndex, LivingSkillShortcut::Create(param->SkillSeriesId,param->Price), true);
	pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), param->DestIndex, ITEM_NOSHOW);

	return MSG_NONE;
}
#endif
// ----------------------------------------------------------------------------
// 配方材料栏
// ----------------------------------------------------------------------------
#ifdef NTJ_CLIENT

PrescriptionList::PrescriptionList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_PRESCRIPTION;
}

bool PrescriptionList::initPrescriptionList(U32 SerialID)
{
	PrescriptionData* pData = g_PrescriptionRepository.getPrescriptionData(SerialID);
	if (pData)
	{
		PrescriptionData::stMaterial* pMat = pData->getMaterial();
		if(!pMat)
			return false;

		Reset();
		mSlots[0] = ItemShortcut::CreateItem(pData->getBaseItemID(),1); 
		U8 iCount = g_Prescription.getColResCount(pData);
		for (int i=1,j =0; i<mMaxSlots && j<iCount; i++,j++)
			mSlots[i] = ItemShortcut::CreateItem(pMat[j].resID,pMat[j].resNum,true);

		return true;
	}
	return false;
}
#endif