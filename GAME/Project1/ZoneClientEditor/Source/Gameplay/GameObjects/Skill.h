//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once
#include "Gameplay/GameObjects/SkillData.h"
#include "Gameplay/GameObjects/Cooldown.h"
#include "Gameplay/GameObjects/StatsPlus.h"
#include "Common/PlayerStruct.h"

#define DefaultSkill_NPC 201110001
#define DefaultSkill_Min 20101
#define DefaultSkill_Max 20120
#define Macro_IsDefaultSkill(id) (Macro_GetSkillSeriesId(id) >= DefaultSkill_Min && Macro_GetSkillSeriesId(id) <= DefaultSkill_Max)

// ============================================================================
//  Skill（注意：Skill是依赖于SkillData而存在的）
// ============================================================================
class SkillTable;

class Skill
{
	friend class SkillTable;

public:
	typedef Vector<U32> PlusList;
protected:
	SkillData* m_SkillData;			// 技能数据
	U32 m_BaseLevel;				// 原始技能等级
	PlusList m_Plus;				// 记录相关天赋对该技能的加成
	SkillData* m_PlusData;			// 增强的技能数据

public:
	Skill();
	~Skill();
	static Skill*					Create						(SkillData* pData, PlusList* list);
	void							SetData						(SkillData* data, PlusList* list);
	inline SkillData*				GetData						() { return m_PlusData ? m_PlusData : m_SkillData;}
	inline U32						GetBaseLevel				() { return m_BaseLevel;}
	inline U32						GetBaseSkillId				() { return m_SkillData ? Macro_GetSkillId(m_SkillData->GetSeriesID(), m_BaseLevel) : 0;}
	inline bool						CanDrag						() { return m_SkillData ? (!m_SkillData->IsFlags(SkillData::Flags_Passive)) : false;}	

	void							AddPlus						(U32 id);
	void							RemovePlus					(U32 id);
	void							SetPlus						(PlusList* plus);
	inline PlusList&				GetPlus						() { return m_Plus;}
	void							UpdatePlus					(U32 id = 0, bool add = true);
};


// ============================================================================
//  SkillTable
// ============================================================================

class SkillTable
{
public:
	typedef stdext::hash_map<U32, Skill*> SkillTableType;

	SkillTable(GameObject* obj);
	~SkillTable();
	void Clear();
	void Update();
	bool SaveData(stPlayerStruct* playerInfo);

	Skill* AddSkill(U32 skillId);
	Skill* AddSkill(SkillData* pData);
	Skill* AddDefaultSkill(U32 skillId);
	Skill* AddDefaultSkill(SkillData* pData);

	U32 GetSkillId(U32 seriesId);
	U32 GetBaseSkillId(U32 seriesId);
	Skill* GetSkill(U32 seriesId);
	inline U32 GetDefaultSkillId();
	inline Skill& GetDefaultSkill() { return m_DefaultSkill;}
	inline SkillTableType& GetSkill() { return m_Table;}

	inline bool IsDefaultValid() { return (bool)m_DefaultSkill.GetData();}
	inline bool InDefaultRange(F32 len);
	inline bool InDefaultRangeSquared(F32 lenSq);
	inline F32 GetDefaultRange() { if(m_DefaultSkill.GetData()) return m_DefaultSkill.GetData()->GetRangeMax(); return 1.0f;}

	void RemoveSkill(U32 seriesId);

	void OnAddPlus(StatsPlus* _plus);			// 改变plus
	void OnRemovePlus(StatsPlus* _plus);		// 改变plus

	bool LearnSkill(Player* player,U32 skillId);              //学习技能
	enWarnMessage IsCanLearnSkill(Player* player,U32 skillId);//是否可以学
protected:
	Skill m_DefaultSkill;
	SkillTableType m_Table;
	GameObject* m_pObj;
};


inline U32 SkillTable::GetDefaultSkillId()
{
	if(IsDefaultValid())
		return m_DefaultSkill.GetData()->GetID();
	return 0;
}

inline bool SkillTable::InDefaultRange(F32 len)
{
	if(IsDefaultValid())
		return m_DefaultSkill.GetData()->InRange(len);
	return false;
}

inline bool SkillTable::InDefaultRangeSquared(F32 lenSq)
{
	if(IsDefaultValid())
		return m_DefaultSkill.GetData()->InRangeSquared(lenSq);
	return false;
}

