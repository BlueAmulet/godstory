//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/BuffData.h"
#include "console/simObject.h"
#include "Gameplay/GameObjects/Talent.h"
#include "core/bitStream.h"

// ========================================================================================================================================
//  CountBitSet
// ========================================================================================================================================

struct CountBitSet 
{
	enum
	{
		CountBitSet_Max = 32,
	};

	U32 _bits;
	U32 _counts[CountBitSet_Max];

	CountBitSet();

	/// Sets all bits to 0
	void clearAll();
	/// Sets these bits to 0
	void clear(U32 bits);
	/// Set these bit to counts
	void set(U32 bits, U32 counts);
	/// Is this bit true?
	bool test(U32 bits) const;

	void add(U32 bits, U32 counts = 1);
	void sub(U32 bits, U32 counts = 1);
};

// ========================================================================================================================================
//  Buff
// ========================================================================================================================================

enum enFamily;
class GameObject;
class Player;
struct stPlayerStruct;
class Buff
{
public:

	enum Origin
	{
		Origin_Base = 0,				// 自身属性
		Origin_PassiveSkill,			// 技能，只能是被动技能
		Origin_Buff,					// 状态
		Origin_Equipment,				// 装备
		Origin_Pet,						// 宠物加成
		Origin_Talisman,				// 法宝加成
	};

	enum
	{
		Mask_Remove = BuffData::Trigger_RemoveTimer | BuffData::Trigger_RemoveCounter | BuffData::Trigger_RemoveA | BuffData::Trigger_RemoveB | BuffData::Trigger_RemoveC,
		Mask_Remove_Sys = BIT(0),		// 跟其他移除标志不同即可
		Mask_Remove_Common = BIT(1),	// 普通
	};

	BuffData*	m_BuffData;				// 状态
	Stats		m_BuffStats;			// 状态经过加成后的最终属性

	Origin		m_Origin;				// 这个状态的来源
	U32			m_Count;				// 叠加数
	SimTime		m_RemainTime;			// 剩余时间
	U32			m_RemainTimes;			// 剩余次数
	SimTime		m_LimitTime;			// 总时间
	U32			m_LimitTimes;			// 总次数
	SimTime		m_TriggerCDTime;		// 触发CD结束时间
	F32			m_TimeScale;			// 速度改变

	U32			m_RemoveMask;			// 移除标志 对应BuffData::Trigger_RemoveA~D
	U32			m_TickMask;				// 跳了一下的标志
	Vector<U32> m_Plus;					// 记录相关天赋对该状态的加成
	SimObjectPtr<GameObject> m_pSource;	// 释放这个状态的对象

	Buff();
	~Buff();

	void Remove(GameObject* pObj, U32 mask);
	bool Process(GameObject* pObj, SimTime dt);
	void ProcessTick(GameObject* pObj);

	// 触发
	void onTrigger(U32 mask, GameObject* carrier/*本体*/, GameObject* trigger);

	static void triggerSpell(U32 skillId, U32 tgtType, GameObject* source, GameObject* carrier, GameObject* trigger);
};


// ========================================================================================================================================
//  BuffTable
// ========================================================================================================================================

struct TriggerUnit
{
	U32 mask;
	SimObjectPtr<GameObject> trigger;
};

class BuffTable
{
public:
	typedef Vector<Buff*> BuffTableType;
	typedef stdext::hash_multimap<U32, Buff*> BuffTableMap;
	typedef Vector<TriggerUnit*> TriggerTableType;
	typedef stdext::hash_map<U32, U32> EPMap;

	enum
	{
		StatsBits = 24,
		StatsWuXingBits = 4,

		BuffNumBits = 5,
		BuffIdBits = 32,
		BuffTimeBits = 28,	// 279Min
		BuffCountBits = 16,

		BuffTriggerMax = 8,
	};

private:
	// 是否需要刷新
	bool m_Refresh;
	bool m_RefreshSet;
	bool m_TriggerLocked;

	// 人物最终属性
	Stats m_Stats;

	// 人物属性总和（未经最终计算）
	Stats m_Summation;

	// 这些不会推进，只是记录某些信息（触发的内置CD等）
	// 其中Buff::m_Plus和主角的PlusTable一致
	// 为节约内存，合并到一个hash表
	BuffTableMap m_General;

	// 这些状态会推进
	BuffTableType m_Buff;
	BuffTableType m_Debuff;
	BuffTableType m_System;

	// 拥有这些状态的对象
	GameObject* m_pObj;

	// 所有状态触发条件的集合
	CountBitSet m_TriggerMask;
	TriggerTableType m_TriggerTable;

	// 所有状态效果的集合
	U32 m_EffectMask;
	// 所有免疫效果的集合
	std::bitset<BUFF_GROUP_BITS> m_ImmunitySet;
	// 所有抑制效果的集合
	std::bitset<BUFF_GROUP_BITS> m_RestrainSet;

#ifdef NTJ_CLIENT
	EPMap m_EP;
#endif

public:

	BuffTable();
	~BuffTable();
	void Clear();
	bool SaveData(stPlayerStruct* playerInfo);
	inline void SetObj(GameObject* pObj) {m_pObj = pObj;}
	inline bool GetRefresh() { return m_Refresh; }
	inline void SetRefresh(bool val) { m_Refresh = val; }
	inline const Stats& GetStats() { return m_Stats; }
	inline U32 GetEffect() const { return m_EffectMask; }

	bool AddBuff(Buff::Origin _origin, U32 _buffId, GameObject* _src, S32 _count = 1, SimTime _time = 0);
	// 去除m_General中的数值
	bool RemoveBuff(U32 _buffId);
	// 去除m_Buff、m_Debuff、m_System中的状态
	bool RemoveBuff(BuffTableType& _buffTab, U32 _buffId, U32 index, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys);

	bool RemoveBuffById(U32 _buffId, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys, bool seriesId = false);		// 去除m_Buff、m_Debuff、m_System中的状态，一般用于服务端脚本
	bool RemoveBuffBySrc(U32 _buffId, GameObject* src, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys, bool seriesId = false);
	bool HandleRemoveRequest(U32 _buffId, U32 index);				// 应客户端要求，去除m_Buff中的状态
	void ClearDebuff();												// 清除所有debuff

	void Process(SimTime dt);
	bool CalculateStats(const enFamily family);
	bool CalculateStats_Pet(const Stats& stats, const CalcFactor& factor);

	bool checkBuff(U32 id, S32 count, bool seriesId = false) const;	// 检查m_Buff、m_Debuff、m_System中的状态
	U32 getBuffCount(U32 seriesId, U32 level) const;	// 检查m_Buff、m_Debuff、m_System中的状态

	// 获取嘲讽者
	GameObject* getMocker() const;

	// 设置函数
	inline void setHP(S32 val) { m_Stats.HP = mClamp(val, 0, m_Stats.MaxHP);}
	inline void setMP(S32 val) { m_Stats.MP = mClamp(val, 0, m_Stats.MaxMP);}
	inline void setPP(S32 val) { m_Stats.PP = mClamp(val, 0, m_Stats.MaxPP);}
	inline void addHP(S32 val) { m_Stats.HP = mClamp(m_Stats.HP + val, 0, m_Stats.MaxHP);}
	inline void addMP(S32 val) { m_Stats.MP = mClamp(m_Stats.MP + val, 0, m_Stats.MaxMP);}
	inline void addPP(S32 val) { m_Stats.PP = mClamp(m_Stats.PP + val, 0, m_Stats.MaxPP);}

	// 只更新m_Stats
	void packStats(BitStream *stream);
	void unpackStats(BitStream *stream);
	// 只更新攻击、移动速度
	void packSpeed(BitStream *stream);
	void unpackSpeed(BitStream *stream);
	// 只更新最大值
	void packMaxStats(BitStream* stream);
	void unpackMaxStats(BitStream* stream);
	// 只更新m_Buff、m_Debuff
	void packBuff(BitStream *stream);
	void unpackBuff(BitStream *stream);
	// 取得m_Buff、m_Debuff
	const inline BuffTableType& GetBuff()  const { return m_Buff; }
	const inline BuffTableType& GetDebuff()  const { return m_Debuff; }

	// 触发
	void clearTrigger();
	void processTrigger();
	void onTrigger(U32 mask, GameObject* trigger);
	bool onTriggerGroup(BuffData* pBuffData, GameObject* src);
	bool onTriggerShield(S32& damage);
	void onObjectDisabled();
};


