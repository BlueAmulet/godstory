//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/StatsPlus.h"


// ========================================================================================================================================
//  StatsPlus
// ========================================================================================================================================

StatsPlus::StatsPlus():
	statsPlusId(0),
	generalId(0),
	type(PlusType_None)
{
}


// ========================================================================================================================================
//  StatsPlusTable
// ========================================================================================================================================

bool StatsPlusTable::AddPlus(U32 plusId)
{
	StatsPlus* pPlus = g_StatsPlusRepository.GetPlus(plusId);
	if(!pPlus)
		return false;

	switch(pPlus->type)
	{
	case StatsPlus::PlusType_Base:
		m_BaseTable.push_back(pPlus);
		break;
	case StatsPlus::PlusType_Skill:
		m_SkillTable.push_back(pPlus);
		break;
	case StatsPlus::PlusType_PassiveSkill:
		m_PassiveSkillTable.push_back(pPlus);
		break;
	case StatsPlus::PlusType_Buff:
		m_BuffTable.push_back(pPlus);
		break;
	case StatsPlus::PlusType_Equipment:
		m_EquipmentTable.push_back(pPlus);
		break;
	case StatsPlus::PlusType_Pet:
		m_PetTable.push_back(pPlus);
		break;
	case StatsPlus::PlusType_Talisman:
		m_TalismanTable.push_back(pPlus);
		break;
	default:
		return false;
	}
	return true;
}

bool StatsPlusTable::RemovePlus(U32 plusId)
{
	StatsPlus* pPlus = g_StatsPlusRepository.GetPlus(plusId);
	if(!pPlus)
		return false;

	StatsPlusTableType& table = GetTable(pPlus->type);
	for(S32 i=0; i<table.size(); i++)
		if(table[i]->statsPlusId == plusId)
		{
			delete table[i];
			table.erase(i);
			--i;
			return true;
		}

	return false;
}

StatsPlusTable::StatsPlusTableType& StatsPlusTable::GetTable(U32 type)
{
	switch(type)
	{
	case StatsPlus::PlusType_Base:
		return m_BaseTable;
	case StatsPlus::PlusType_Skill:
		return m_SkillTable;
	case StatsPlus::PlusType_PassiveSkill:
		return m_PassiveSkillTable;
	case StatsPlus::PlusType_Buff:
		return m_BuffTable;
	case StatsPlus::PlusType_Equipment:
		return m_EquipmentTable;
	case StatsPlus::PlusType_Pet:
		return m_PetTable;
	case StatsPlus::PlusType_Talisman:
		return m_TalismanTable;
	default:
		AssertFatal(false, "StatsPlusTable::GetTable");
	}
	return m_BaseTable;
}


// ========================================================================================================================================
//  StatsPlusRepository
// ========================================================================================================================================

StatsPlusRepository g_StatsPlusRepository;

StatsPlusRepository::StatsPlusRepository()
{
}

StatsPlusRepository::~StatsPlusRepository()
{
	Clear();
}

void StatsPlusRepository::Clear()
{
	PlusDataMap::iterator pos;
	for(pos = m_PlusDataMap.begin(); pos != m_PlusDataMap.end(); ++pos)
		delete pos->second;
	m_PlusDataMap.clear();
}

StatsPlus* StatsPlusRepository::GetPlus(U32 PlusId)
{
	PlusDataMap::iterator itr = m_PlusDataMap.find(PlusId);
	if(itr == m_PlusDataMap.end())
		return NULL;
	return itr->second;
}

bool StatsPlusRepository::Insert(StatsPlus& _Plus)
{
	if(GetPlus(_Plus.statsPlusId))
		return false;
	m_PlusDataMap.insert(PlusDataMap::value_type(_Plus.statsPlusId, &_Plus));
	return true;
}


