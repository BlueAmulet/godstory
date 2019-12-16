//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/Skill.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "BuildPrefix.h"
#ifdef NTJ_SERVER
#include "Common/LogHelper.h"
#include "Common/Log/LogTypes.h"
#include "Gameplay/ServerGameplayState.h"
#endif

// ============================================================================
//  Skill
// ============================================================================


Skill::Skill():m_SkillData(NULL),m_BaseLevel(0)
{
	m_PlusData = NULL;
}

Skill::~Skill()
{
	m_SkillData = NULL;
	m_BaseLevel = 0;
	m_Plus.clear();
	SAFE_DELETE(m_PlusData);
}

// ----------------------------------------------------------------------------
// ��̬����:�ɼ������ݲ���һ�����ܶ���
Skill* Skill::Create(SkillData* pData, PlusList* list)
{
	if(pData)
	{
		Skill* pSkill = new Skill();
		pSkill->m_SkillData = pData;
		if(list)
		{
			pSkill->m_Plus = *list;
			pSkill->UpdatePlus();
		}
		else
		{
			pSkill->m_Plus.clear();
			SAFE_DELETE(pSkill->m_PlusData);
		}
		return pSkill;
	}
	return NULL;
}

void Skill::SetData(SkillData* data, PlusList* list)
{
	// ֻ������ͬһϵ�ļ���
	if(m_SkillData && data && m_SkillData->GetSeriesID() == data->GetSeriesID())
	{
		m_SkillData = data;
		if(list)
		{
			m_Plus = *list;
			UpdatePlus();
		}
		else
		{
			m_Plus.clear();
			SAFE_DELETE(m_PlusData);
		}
	}
}

void Skill::AddPlus(U32 id)
{
	m_Plus.push_back(id);
	UpdatePlus(id);
}

void Skill::RemovePlus(U32 id)
{
	PlusList::iterator it = m_Plus.begin();
	while (it != m_Plus.end())
	{
		if((*it) == id)
		{
			m_Plus.erase(it);
			UpdatePlus(id, false);
			break;
		}
	}
}

void Skill::SetPlus(PlusList* plus)
{
	if(plus)
	{
		m_Plus = *plus;
		UpdatePlus();
	}
}

void Skill::UpdatePlus(U32 id /* = 0 */, bool add /* = true */)
{
	if(m_Plus.empty() || !m_SkillData)
	{
		m_Plus.clear();
		SAFE_DELETE(m_PlusData);
		return;
	}
	if(!m_PlusData)
	{
		if(!add)
		{
			// ɾ����ʱ��ִ�е�����˵��������
			AssertFatal(false, "Skill::UpdatePlus");
			// ȫ��ˢ�°�
			id = 0;
		}
		m_PlusData = new SkillData();
		dMemcpy(m_PlusData, m_SkillData, sizeof(SkillData));
	}
	// ����������ǿ
	if(id)
	{
		SkillData* pPlus = g_SkillRepository.GetSkill(id);
		if(pPlus)
		{
			if(add)
				m_PlusData->Plus(*m_PlusData, *pPlus);
			else
				m_PlusData->Minus(*m_PlusData, *pPlus);
			m_PlusData->PlusDone();
		}
	}
	// ����ˢ����ǿ�б�
	else
	{
		dMemcpy(m_PlusData, m_SkillData, sizeof(SkillData));
		for (S32 i=m_Plus.size()-1; i>=0; --i)
		{
			SkillData* pPlus = g_SkillRepository.GetSkill(m_Plus[i]);
			if(pPlus)
				m_PlusData->Plus(*m_PlusData, *pPlus);
		}
		m_PlusData->PlusDone();
	}
}

// ============================================================================
//  SkillTable
// ============================================================================

SkillTable::SkillTable(GameObject* obj) : m_pObj(obj)
{
	Clear();
}

SkillTable::~SkillTable()
{
	Clear();
}

bool SkillTable::SaveData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	if(!playerInfo || m_Table.size() > SKILL_MAXSLOTS)
	{
		AssertFatal(false, "SkillTable::SaveData error!!");
		return false;
	}

	S32 i = 0;
	SkillTableType::iterator it = m_Table.begin();
	for(; it != m_Table.end(); ++it)
	{
		if(it->second && it->second->m_SkillData)
		{
			playerInfo->MainData.SkillInfo[i].SkillId = Macro_GetSkillId(it->second->m_SkillData->GetSeriesID(), it->second->m_BaseLevel);
			++i;
		}
	}
#endif
	return true;
}

// ----------------------------------------------------------------------------
// ������ܱ�����
void SkillTable::Clear()
{
	SkillTableType::iterator itr;
	for(itr = m_Table.begin(); itr != m_Table.end(); ++itr)
		delete itr->second;
	m_Table.clear();
}

// ----------------------------------------------------------------------------
// ����
void SkillTable::Update()
{
	U32 mask = 0;
	for (S32 i=SkillData::School_Mu; i<=SkillData::School_Shui; ++i)
	{
		S32 idx = i - SkillData::School_Mu;
		if(m_pObj->m_WuXing[idx] != m_pObj->getWuXing(idx))
		{
			mask |= BIT(i);
			m_pObj->m_WuXing[idx] = m_pObj->getWuXing(idx);
		}
	}
	if(!mask)
		return;
	SkillTableType::iterator itr;
	SkillData* pData = NULL;
	for(itr = m_Table.begin(); itr != m_Table.end(); ++itr)
	{
		if(itr->second && (pData = itr->second->GetData()))
		{
			if (mask & BIT(pData->GetSchool()))
			{
				S32 level = getMax((S32)itr->second->GetBaseLevel() + m_pObj->getWuXing(pData->GetSchool() - SkillData::School_Mu), 1);
				pData = g_SkillRepository.GetSkill(pData->m_SeriesId, level);
				if(pData)
				{
					Skill::PlusList* list = (m_pObj->getGameObjectMask() & PlayerObjectType) ? ((Player*)m_pObj)->mTalentTable.getIdList(pData->m_SeriesId) : NULL;
					itr->second->SetData(pData, list);
				}
				else
					AssertFatal(false, "error : SkillTable::Update()");
			}
		}
	}
}

// ----------------------------------------------------------------------------
// ���ݼ���IDΪ���ܱ����һ������
Skill* SkillTable::AddSkill(U32 skillId)
{
	return AddSkill(g_SkillRepository.GetSkill(skillId));
}

// ----------------------------------------------------------------------------
// ���ݼ�������Ϊ���ܱ����һ������
Skill* SkillTable::AddSkill(SkillData* pData)
{
	if(!pData || !m_pObj)
		return NULL;

	// ���Զ���������
	if(pData->IsFlags(SkillData::Flags_AutoCast))
		return AddDefaultSkill(pData);

	S32 baseLevel = pData->GetLevel();
	SkillTableType::iterator itr = m_Table.find(pData->m_SeriesId);
	// ���и�ϵ�еļ���
	if(itr != m_Table.end())
	{
		// �Ѿ��иõȼ��ļ���
		if(itr->second && itr->second->m_BaseLevel == baseLevel)
			return itr->second;
		// �������Դ����ĵȼ�����
		if (pData->GetSchool() >= SkillData::School_Mu && pData->GetSchool() <= SkillData::School_Shui)
		{
			S32 level = getMax(baseLevel + m_pObj->getWuXing(pData->GetSchool() - SkillData::School_Mu), 1);
			pData = g_SkillRepository.GetSkill(pData->m_SeriesId, level);
			AssertFatal(pData, "error: SkillTable::AddSkill !");
			if(!pData)
				return itr->second;
		}
		// �и�ϵ�в�ͬ�ȼ��ļ���
		if(itr->second)
		{
			itr->second->m_SkillData = pData;
			itr->second->m_BaseLevel = baseLevel;
			return itr->second;
		}
		// error: ����ָ��Ϊ��
		if(!itr->second)
		{
			AssertFatal(false, "error: SkillTable::AddSkill !");
			Skill::PlusList* list = (m_pObj->getGameObjectMask() & PlayerObjectType) ? ((Player*)m_pObj)->mTalentTable.getIdList(pData->m_SeriesId) : NULL;
			Skill* pSkill = Skill::Create(pData, list);
			if(pSkill)
			{
				pSkill->m_BaseLevel = baseLevel;
				itr->second = pSkill;
				return pSkill;
			}
			else
				return NULL;
		}
	}

	// �����¼���
	if (pData->GetSchool() >= SkillData::School_Mu && pData->GetSchool() <= SkillData::School_Shui)
	{
		S32 level = getMax(baseLevel + m_pObj->getWuXing(pData->GetSchool() - SkillData::School_Mu), 1);
		pData = g_SkillRepository.GetSkill(pData->m_SeriesId, level);
		AssertFatal(pData, "error: SkillTable::AddSkill !");
		if(!pData)
			return NULL;
	}
	// �츳��ǿ
	Skill::PlusList* list = (m_pObj->getGameObjectMask() & PlayerObjectType) ? ((Player*)m_pObj)->mTalentTable.getIdList(pData->m_SeriesId) : NULL;
	Skill* pSkill = Skill::Create(pData, list);
	if(pSkill)
	{
		pSkill->m_BaseLevel = baseLevel;
		m_Table.insert(SkillTableType::value_type(pData->GetSeriesID(), pSkill));
		return pSkill;
	}
	return NULL;
}

Skill* SkillTable::AddDefaultSkill(U32 skillId)
{
	return AddDefaultSkill(g_SkillRepository.GetSkill(skillId));
}

Skill* SkillTable::AddDefaultSkill(SkillData* pData)
{
	if(!pData || !pData->IsFlags(SkillData::Flags_AutoCast))
		return NULL;

	m_DefaultSkill.m_SkillData = pData;
	m_DefaultSkill.m_BaseLevel = pData->GetLevel();
	Skill::PlusList* list = (m_pObj->getGameObjectMask() & PlayerObjectType) ? ((Player*)m_pObj)->mTalentTable.getIdList(pData->m_SeriesId) : NULL;
	if(list)
	{
		m_DefaultSkill.m_Plus = *list;
		m_DefaultSkill.UpdatePlus();
	}
	else
	{
		m_DefaultSkill.m_Plus.clear();
		SAFE_DELETE(m_DefaultSkill.m_PlusData);
	}
	return &m_DefaultSkill;
}

// ----------------------------------------------------------------------------
// �Ӽ��ܱ���ɾ��һ������
void SkillTable::RemoveSkill(U32 seriesId)
{
	if(m_DefaultSkill.GetData() && m_DefaultSkill.GetData()->GetSeriesID() == seriesId)
		return;

	SkillTableType::iterator itr = m_Table.find(seriesId);
	if(itr != m_Table.end())
	{
		delete itr->second;
		m_Table.erase(itr);
	}
}

U32 SkillTable::GetSkillId(U32 seriesId)
{
	if(m_DefaultSkill.GetData() && seriesId == m_DefaultSkill.GetData()->GetSeriesID())
		return m_DefaultSkill.GetData()->GetID();
	SkillTableType::iterator itr = m_Table.find(seriesId);
	if(itr != m_Table.end() && itr->second && itr->second->GetData())
	{
		return itr->second->GetData()->GetID();
	}
	return 0;
}

U32 SkillTable::GetBaseSkillId(U32 seriesId)
{
	if(m_DefaultSkill.GetData() && seriesId == m_DefaultSkill.GetData()->GetSeriesID())
		return m_DefaultSkill.GetBaseSkillId();
	SkillTableType::iterator itr = m_Table.find(seriesId);
	if(itr != m_Table.end() && itr->second && itr->second->GetData())
	{
		return itr->second->GetBaseSkillId();
	}
	return 0;
}

Skill* SkillTable::GetSkill(U32 seriesId)
{
	if(m_DefaultSkill.GetData() && seriesId == m_DefaultSkill.GetData()->GetSeriesID())
		return &m_DefaultSkill;
	SkillTableType::iterator itr = m_Table.find(seriesId);
	if(itr != m_Table.end() && itr->second)
	{
		return itr->second;
	}
	return NULL;
}

bool SkillTable::LearnSkill(Player* player,U32 skillId)
{
	if (!player)
         return false;

     GameConnection* conn = player->getControllingClient();
	 if(!conn)
		return false;
	 enWarnMessage msg = IsCanLearnSkill(player,skillId);

#ifdef NTJ_SERVER
	 if(msg == MSG_NONE)
	 {
		 SkillData* pData = g_SkillRepository.GetSkill(skillId);
		 if (pData)
		 {
			 SkillData::LearnLimit &learlimit =pData->GetLearnLimit();
             
			 player->reduceMoney(learlimit.money, 21);
			 player->addExp(-(S32)learlimit.exp);

			 lg_skill_learn lg;
			 lg.skillName = pData->GetName();
			 lg.costExp   = learlimit.exp;
			 lg.costGold  = learlimit.money;
			 lg.playerName = player->getName();

			 g_ServerGameplayState->GetLog()->writeLog(&lg);

			 player->AddSkill(skillId);

             return true;	 
		 }
	 }
	 else
	 {
		 MessageEvent::send(conn,SHOWTYPE_ERROR,msg);
		 return false;
	 }
#endif	 

	return false;
}

enWarnMessage SkillTable::IsCanLearnSkill(Player* player,U32 skillId)
{
   if(!player)
	   return MSG_UNKOWNERROR;
   
   SkillData* pData = g_SkillRepository.GetSkill(skillId);
   if (!pData)
	   return MSG_UNKOWNERROR;

   if (pData->GetLearnLimit().level > player->getLevel())
	   return MSG_PLAYER_LEVEL;
   if (pData->GetLearnLimit().exp > player->getExp())
        return MSG_PLAYER_EXP;
   if(!player->canReduceMoney(pData->GetLearnLimit().money, 21))
	   return MSG_PLAYER_MONEYNOTENOUGH;

   return MSG_NONE;
   
}


