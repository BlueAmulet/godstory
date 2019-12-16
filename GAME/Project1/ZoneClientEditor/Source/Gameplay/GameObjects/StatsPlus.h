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

// ��С�ڸ������
#define PLUS_GROUP_BITS 128

struct StatsPlus
{
	// ��ǿ������ֵ
	enum PlusType
	{
		PlusType_None		= 0,	// ��
		PlusType_Base,				// ��������
		PlusType_Skill,				// ���ܣ�ֻ������������
		PlusType_PassiveSkill,		// ���ܣ�ֻ���Ǳ�������
		PlusType_Buff,				// ״̬
		PlusType_Equipment,			// װ��
		PlusType_Pet,				// ����ӳ�
		PlusType_Talisman,			// �����ӳ�
	};

	U32 statsPlusId;
	U32 generalId;					// ������BUFF��SKILL��ID
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
