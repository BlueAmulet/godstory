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
		Origin_Base = 0,				// ��������
		Origin_PassiveSkill,			// ���ܣ�ֻ���Ǳ�������
		Origin_Buff,					// ״̬
		Origin_Equipment,				// װ��
		Origin_Pet,						// ����ӳ�
		Origin_Talisman,				// �����ӳ�
	};

	enum
	{
		Mask_Remove = BuffData::Trigger_RemoveTimer | BuffData::Trigger_RemoveCounter | BuffData::Trigger_RemoveA | BuffData::Trigger_RemoveB | BuffData::Trigger_RemoveC,
		Mask_Remove_Sys = BIT(0),		// �������Ƴ���־��ͬ����
		Mask_Remove_Common = BIT(1),	// ��ͨ
	};

	BuffData*	m_BuffData;				// ״̬
	Stats		m_BuffStats;			// ״̬�����ӳɺ����������

	Origin		m_Origin;				// ���״̬����Դ
	U32			m_Count;				// ������
	SimTime		m_RemainTime;			// ʣ��ʱ��
	U32			m_RemainTimes;			// ʣ�����
	SimTime		m_LimitTime;			// ��ʱ��
	U32			m_LimitTimes;			// �ܴ���
	SimTime		m_TriggerCDTime;		// ����CD����ʱ��
	F32			m_TimeScale;			// �ٶȸı�

	U32			m_RemoveMask;			// �Ƴ���־ ��ӦBuffData::Trigger_RemoveA~D
	U32			m_TickMask;				// ����һ�µı�־
	Vector<U32> m_Plus;					// ��¼����츳�Ը�״̬�ļӳ�
	SimObjectPtr<GameObject> m_pSource;	// �ͷ����״̬�Ķ���

	Buff();
	~Buff();

	void Remove(GameObject* pObj, U32 mask);
	bool Process(GameObject* pObj, SimTime dt);
	void ProcessTick(GameObject* pObj);

	// ����
	void onTrigger(U32 mask, GameObject* carrier/*����*/, GameObject* trigger);

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
	// �Ƿ���Ҫˢ��
	bool m_Refresh;
	bool m_RefreshSet;
	bool m_TriggerLocked;

	// ������������
	Stats m_Stats;

	// ���������ܺͣ�δ�����ռ��㣩
	Stats m_Summation;

	// ��Щ�����ƽ���ֻ�Ǽ�¼ĳЩ��Ϣ������������CD�ȣ�
	// ����Buff::m_Plus�����ǵ�PlusTableһ��
	// Ϊ��Լ�ڴ棬�ϲ���һ��hash��
	BuffTableMap m_General;

	// ��Щ״̬���ƽ�
	BuffTableType m_Buff;
	BuffTableType m_Debuff;
	BuffTableType m_System;

	// ӵ����Щ״̬�Ķ���
	GameObject* m_pObj;

	// ����״̬���������ļ���
	CountBitSet m_TriggerMask;
	TriggerTableType m_TriggerTable;

	// ����״̬Ч���ļ���
	U32 m_EffectMask;
	// ��������Ч���ļ���
	std::bitset<BUFF_GROUP_BITS> m_ImmunitySet;
	// ��������Ч���ļ���
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
	// ȥ��m_General�е���ֵ
	bool RemoveBuff(U32 _buffId);
	// ȥ��m_Buff��m_Debuff��m_System�е�״̬
	bool RemoveBuff(BuffTableType& _buffTab, U32 _buffId, U32 index, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys);

	bool RemoveBuffById(U32 _buffId, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys, bool seriesId = false);		// ȥ��m_Buff��m_Debuff��m_System�е�״̬��һ�����ڷ���˽ű�
	bool RemoveBuffBySrc(U32 _buffId, GameObject* src, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys, bool seriesId = false);
	bool HandleRemoveRequest(U32 _buffId, U32 index);				// Ӧ�ͻ���Ҫ��ȥ��m_Buff�е�״̬
	void ClearDebuff();												// �������debuff

	void Process(SimTime dt);
	bool CalculateStats(const enFamily family);
	bool CalculateStats_Pet(const Stats& stats, const CalcFactor& factor);

	bool checkBuff(U32 id, S32 count, bool seriesId = false) const;	// ���m_Buff��m_Debuff��m_System�е�״̬
	U32 getBuffCount(U32 seriesId, U32 level) const;	// ���m_Buff��m_Debuff��m_System�е�״̬

	// ��ȡ������
	GameObject* getMocker() const;

	// ���ú���
	inline void setHP(S32 val) { m_Stats.HP = mClamp(val, 0, m_Stats.MaxHP);}
	inline void setMP(S32 val) { m_Stats.MP = mClamp(val, 0, m_Stats.MaxMP);}
	inline void setPP(S32 val) { m_Stats.PP = mClamp(val, 0, m_Stats.MaxPP);}
	inline void addHP(S32 val) { m_Stats.HP = mClamp(m_Stats.HP + val, 0, m_Stats.MaxHP);}
	inline void addMP(S32 val) { m_Stats.MP = mClamp(m_Stats.MP + val, 0, m_Stats.MaxMP);}
	inline void addPP(S32 val) { m_Stats.PP = mClamp(m_Stats.PP + val, 0, m_Stats.MaxPP);}

	// ֻ����m_Stats
	void packStats(BitStream *stream);
	void unpackStats(BitStream *stream);
	// ֻ���¹������ƶ��ٶ�
	void packSpeed(BitStream *stream);
	void unpackSpeed(BitStream *stream);
	// ֻ�������ֵ
	void packMaxStats(BitStream* stream);
	void unpackMaxStats(BitStream* stream);
	// ֻ����m_Buff��m_Debuff
	void packBuff(BitStream *stream);
	void unpackBuff(BitStream *stream);
	// ȡ��m_Buff��m_Debuff
	const inline BuffTableType& GetBuff()  const { return m_Buff; }
	const inline BuffTableType& GetDebuff()  const { return m_Debuff; }

	// ����
	void clearTrigger();
	void processTrigger();
	void onTrigger(U32 mask, GameObject* trigger);
	bool onTriggerGroup(BuffData* pBuffData, GameObject* src);
	bool onTriggerShield(S32& damage);
	void onObjectDisabled();
};


