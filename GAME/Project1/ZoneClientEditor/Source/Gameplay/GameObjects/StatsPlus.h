//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/Stats.h"
#include <bitset>


// ========================================================================================================================================
//  StatsPlus
// ========================================================================================================================================

// 不小于各类分组
#define PLUS_GROUP_BITS 128

struct StatsPlus
{
	// 增强哪种数值
	enum PlusType
	{
		PlusType_None		= 0,	// 无
		PlusType_Base,				// 自身属性
		PlusType_Skill,				// 技能，只能是主动技能
		PlusType_PassiveSkill,		// 技能，只能是被动技能
		PlusType_Buff,				// 状态
		PlusType_Equipment,			// 装备
		PlusType_Pet,				// 宠物加成
		PlusType_Talisman,			// 法宝加成
	};

	U32 statsPlusId;
	U32 generalId;					// 可以是BUFF或SKILL的ID
	PlusType type;
	std::bitset<PLUS_GROUP_BITS> groupMask;

	StatsPlus();
};


// ========================================================================================================================================
//  StatsPlusTable
// ========================================================================================================================================

class StatsPlusTable 
{
public:
	typedef Vector<StatsPlus*> StatsPlusTableType;

private:
	StatsPlusTableType m_BaseTable;
	StatsPlusTableType m_SkillTable;
	StatsPlusTableType m_PassiveSkillTable;
	StatsPlusTableType m_BuffTable;
	StatsPlusTableType m_EquipmentTable;
	StatsPlusTableType m_PetTable;
	StatsPlusTableType m_TalismanTable;

public:
	bool AddPlus(U32 plusId);
	bool RemovePlus(U32 plusId);
	StatsPlusTableType& GetTable(U32 type);
};


// ========================================================================================================================================
//  StatsPlusRepository
// ========================================================================================================================================

class StatsPlusRepository
{
public:
	typedef stdext::hash_map<U32, StatsPlus*> PlusDataMap;

	PlusDataMap m_PlusDataMap;

public:
	StatsPlusRepository();
	~StatsPlusRepository();

	void				Clear();
	StatsPlus*			GetPlus(U32 PlusId);
	bool				Insert(StatsPlus&);
};

extern StatsPlusRepository g_StatsPlusRepository;
