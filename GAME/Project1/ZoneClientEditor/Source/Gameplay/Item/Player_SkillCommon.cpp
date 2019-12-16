#include "gameplay/GameObjects/PlayerObject.h"

#ifdef NTJ_SERVER
enWarnMessage ItemManager::SkillCommonMoveToPanel(stExChangeParam* param)
{
	Player* pPlayer = param->player;
	if(!pPlayer)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	if(!pPlayer->panelList.IsVaildSlot(param->DestIndex))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;	

	Skill* pSkill = pPlayer->GetSkillTable().GetSkill(param->SkillSeriesId);
	if(!pSkill || !pSkill->CanDrag())
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	pPlayer->panelList.SetSlot(param->DestIndex, SkillShortcut::CreateSkill(pSkill), true);
	pPlayer->panelList.UpdateToClient(pPlayer->getControllingClient(), param->DestIndex, ITEM_NOSHOW);

	return MSG_NONE;
}

enWarnMessage ItemManager::SkillHintMoveToPanel(stExChangeParam* param)
{
	Player* player = param->player;

	if (!player)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;
	if(!player->panelList.IsVaildSlot(param->DestIndex) || !player->skillList_Hint.IsVaildSlot(param->SrcIndex))
        return MSG_ITEM_CANNOTDRAGTOOBJECT;

	Skill* pSkill = player->GetSkillTable().GetSkill(param->SkillSeriesId);
	if(!pSkill || !pSkill->CanDrag())
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	player->panelList.SetSlot(param->DestIndex,SkillShortcut::CreateSkill(pSkill),true);
	player->skillList_Hint.SetSlot(param->SrcIndex,NULL,true);

	player->panelList.UpdateToClient(player->getControllingClient(),param->DestIndex,ITEM_NOSHOW);
	player->skillList_Hint.UpdateToClient(player->getControllingClient(),param->SrcIndex,ITEM_NOSHOW);

	return MSG_NONE;
}
#endif

#ifdef NTJ_CLIENT
SkillList_Common::SkillList_Common():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_SKILL_COMMON;
}

bool SkillList_Common::AddSkillShortcut(Player* player, U32 seriesId)
{
	Skill* pSkill = player->GetSkillTable().GetSkill(seriesId);
	if(!pSkill && Macro_GetSkillSeriesId(player->GetSkillTable().GetDefaultSkillId()) == seriesId)
		pSkill = &player->GetSkillTable().GetDefaultSkill();
	if(pSkill && !IsAdd(pSkill))
	{
		S32 slot = FindEmptySlot();
		if(slot != -1)
			mSlots[slot] = SkillShortcut::CreateSkill(pSkill);
	}
	return false;
}

bool SkillList_Common::IsAdd(Skill* pSkill)
{
	for(int i = 0; i < mMaxSlots; ++i)
	{
		if(mSlots[i] && ((SkillShortcut*)mSlots[i])->getSkill() == pSkill)
			return true;
	}
	return false;
}
#endif//NTJ_CLIENT



//------------------------------------------------------------------------
//技能学习栏
//------------------------------------------------------------------------
#if NTJ_CLIENT
SkillList_Study::SkillList_Study():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_SKILL_STUDY;
}

bool SkillList_Study::AddSkillStudyShortcut(Player* player, U32 seriesId)
{
   	
	Skill* pSkill = player->GetSkillTable().GetSkill(seriesId);
	SkillData * pData;
	if (!pSkill)
	{
		pData = g_SkillRepository.GetSkill(seriesId,1);		
	}
	else
	{
		pData = g_SkillRepository.GetSkill(seriesId,(pSkill->GetData()->GetLevel()+1));
	}
	if(pData && !IsAdd(pData))
	{
		S32 slot = FindEmptySlot();
		if(slot != -1)
			mSlots[slot] = SkillStudyShortcut::CreateSkillData(pData);
	}
	return false;

}

bool SkillList_Study::IsAdd(SkillData* pData)
{
	for(int i = 0; i < mMaxSlots; ++i)
	{
		if(mSlots[i] && ((SkillStudyShortcut*)mSlots[i])->getSkillData() == pData)
			return true;
	}
	return false;
}

bool SkillList_Study::UpdataSkillStudyUI(U32 skillId)
{
	SkillData* mData = g_SkillRepository.GetSkill(skillId); 
	for (int i=0;i<mMaxSlots;++i)
	{
		
		if (mSlots[i] && ((SkillStudyShortcut*)mSlots[i])->getSkillData() == mData )
		{
			SkillData* temp = g_SkillRepository.GetSkill((mData->GetID()+1));
			if (temp)
			{
			  delete mSlots[i];
			  mSlots[i] = NULL;
			  mSlots[i] = SkillStudyShortcut::CreateSkillData(temp);
			}
			//这里要处理技能最高级后状态锁定
			else 
			{
				mSlots[i]->setSlotState(ShortcutObject::SLOT_LOCK);
			}
			Con::executef("UpdataSkillStudyInfo");
			break;
		}
	}
  return false;
}
#endif

//-------------------------------------------------------------------
//技能提示栏
//-------------------------------------------------------------------
SkillList_Hint::SkillList_Hint():BaseItemList(MAXSlOTS)
{
	mType = SHORTCUTTYPE_SKILL_HINT;
	mStartIndex = 0;
}

bool SkillList_Hint::AddSkillHintShortcut(Player* player,U32 seriesId)
{
   Skill* pSkill = player ->GetSkillTable().GetSkill(seriesId);
   if(pSkill && !IsAdd(pSkill))
   {
	   S32 slot = FindEmptySlot();
	   if(slot != -1)
	   {
		   mSlots[slot] = SkillShortcut::CreateSkill(pSkill);
		   if(slot >= 4 )
		   {
			   Con::evaluatef("SetSkillHintShortcutIndex(%d);",(slot-3));
			   mStartIndex = (slot-3);
		   }
	   }
   }
   return false;
}

bool SkillList_Hint::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
   if(!conn)
	   return false;
   SkillHintEvent* ev = new SkillHintEvent(index,flag);
   conn->postNetEvent(ev);
#endif 
	return true;
}

bool SkillList_Hint::IsAdd(Skill* pSkill)
{
	for(int i = 0; i < mMaxSlots; ++i)
	{
		if(mSlots[i] && ((SkillShortcut*)mSlots[i])->getSkill() == pSkill)
			return true;
	}
	return false;
}

void SkillList_Hint::CheckCurrentSlot()
{
	for (U32 i = mStartIndex;i<=(mStartIndex+3);i++)
	{
		if(mSlots[i]) return;
	}
	Con::evaluatef("OpenSkillHintGui(0);");
}