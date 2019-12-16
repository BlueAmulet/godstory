//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/Skill.h"
//#include "Gameplay/GameObjects/GameObject.h"


// ========================================================================================================================================
//  AutoCastTimer
// ========================================================================================================================================

class AutoCastTimer
{
	SimTime m_BeginCastTime;			// 开始施放时间
	SimTime m_CastCooldown;				// 冷却的时间间隔
	SimTime m_BeginAnimateTime;			// 动画开始时间，与m_BeginCastTime一致
	SimTime m_AnimateDuration;			// 动画持续时间，Transition和sequence的时间总和
	SimTime m_CooldownTime;				// 冷却完成的时间

	F32 m_TimeScale;					// 动画缩放
	bool m_Enable;						// 是否启用自动施放

public:
	AutoCastTimer();
	void						UpdateTimer					(SimTime CastCooldown, SimTime AnimateDuration);
	void						SetDelay					(SimTime EndTime);
	void						SetEnable					(bool enable) { m_Enable = enable; }
	inline bool					GetEnable					() { return m_Enable; }
	inline bool					CanAutoCast					() {return m_Enable && m_CooldownTime < Platform::getVirtualMilliseconds();}
	inline F32					GetScale					() {return m_Enable ? m_TimeScale : 1.0f; }
};


// ========================================================================================================================================
//  ScheduleSpell
// ========================================================================================================================================
class GameObject;
class SpellObject;

class ScheduleSpell
{
	friend class Spell;
	friend class SpellObject;

protected:
	S32							mRemainTime;				// 剩余时间
	S32							mLimitTime;					// 总时间
	S32							mRemainTimes;				// 剩余次数
	S32							mLimitTimes;				// 总次数

	SimObjectPtr<GameObject>	mSource;					// 源对象
	bool						mIsTarget;					// 施放的对象是目标对象还是坐标对象
	SimObjectPtr<GameObject>	mTarget;					// 目标对象
	Point3F						mDest;						// 目标坐标
	SkillData*					mSkillData;					// 当前技能数据
	Stats						mDamageStats;				// 最终伤害数据
	U32							mEffectPacket;				// 特效包
	SkillData::Selectable		mSelectable;				// 目标选择通道

public:
	ScheduleSpell();
	ScheduleSpell(SimTime time, S32 times, GameObject* src, bool bTgt, GameObject* tgt, Point3F& pos, SkillData* pData, Stats& stats, U32 ep, SkillData::Selectable sl);
	~ScheduleSpell();

	bool Advance(F32 dt);
};


// ========================================================================================================================================
//  Spell
// ========================================================================================================================================
class NetConnection;
class BitStream;
class EffectPacketItem;

class Spell
{
public:
	friend class GameObject;

	enum
	{
		MaxVelDelayTicks		= 4,
		ChainEPCountMax			= 8,
		ChainEPTimeMs			= 2000,
	};

	enum TranslateBits
	{
		StatusBits				= 3,
	};

	//当前技能的推进状态
	enum Status
	{
		Status_None				= 0,						// 无
		Status_Begin,										// 开始
		Status_ReadySpell,									// 吟唱
		Status_SpellCast,									// 施放
		Status_ChannelCast,									// 持续施法
		Status_End,											// 结束
	};

	struct ItemInterface
	{
		S32 type;
		S32 index;
		U64 UID;

		void clear() {type = 0; index = 0; UID = 0;}
		bool onUse(Player* player, GameObject* target);

		ItemInterface() { clear();}
	};

	struct PreSpell
	{
		SkillData*				pSkillData;					// 当前技能数据
		ItemInterface			itemInter;					// 物品使用接口

		PreSpell():pSkillData(NULL) {}
	};

	struct ChainEP
	{
		bool						overwrite;					// 可否写入
		S32							remainTime;					// 剩余时间
		U32							skillId;					// 剩余时间
		Vector<U32>					carriers;					// 链中的对象

		void clear() {overwrite=true; remainTime=ChainEPTimeMs; skillId=0; carriers.clear();}

		ChainEP() {clear();}
	};

protected:

	U32							m_SkillId;
	bool						m_bCast;					// 是否已施放（客户端）
	bool						m_bReady;					// 是否已开始吟唱（客户端）
	bool						m_bChannel;					// 是否已开始持续释放（客户端）
	bool						m_bConsume;					// 是否已经扣除技能消耗了
	bool						m_bTarget;					// 施放的对象是目标对象还是坐标对象
	bool						m_bBreak;					// 是否是因为被打断而停止
	bool						m_bLastBreak;				// 是否是因为被打断而停止
	bool						m_bCastSuc;					// 是否施放成功

	Status						m_Status;					// 推进状态
	S32							m_ReadySpellTime;			// 吟唱时间
	S32							m_ChannelCastTime;			// 持续施法时间
	S32							m_RemainTimes;				// 剩余次数
	S32							m_LimitTimes;				// 总次数
	S32							m_VelDelayTicks;			// 判断速度时可能有延时

	SimObjectPtr<GameObject>	m_Source;					// 施放对象
	SimObjectPtr<GameObject>	m_Target;					// 目标对象
	Point3F						m_Dest;						// 目标坐标
	SkillData::Selectable		m_Selectable;				// 目标选择通道

	Skill*						m_pSkill;					// 当前技能数据
	SkillData*					m_pSkillData;				// 当前技能数据
	SkillData					m_PlusData;					// 当前技能受加成后的数据，有些值不全

	U32							m_SourceEffect;				// 源对象特效
	U32							m_TargetEffect;				// 目标对象特效

	Vector<ScheduleSpell*>		m_ScheduleSpell;			// 延时施法，如轨迹法术
	PreSpell					m_PreSpell;					// 预备法术
	ChainEP						m_ChainEP;					// 链式特效
	ItemInterface				m_ItemInterface;			// 使用物品接口
public:
	Spell						();
	~Spell						();
	void						Clear						();

	bool						CanCast						(SkillData* pSkillData, GameObject* target);
	bool						CanCast						(SkillData* pSkillData, Point3F& dest);
	bool						Cast						(U32 skillId, GameObject& target, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);
	bool						Cast						(U32 skillId, Point3F& dest, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);
	bool						ClientCast					(U32 skillId, GameObject& target);
	bool						ClientCast					(U32 skillId, Point3F& dest);
	void						BreakSpell					();										// 中断技能施放
	bool						CheckVelocity				();

	inline bool					IsSpellRunning				() { return m_Status != Status_None; }

	void						Pack						(NetConnection* conn, BitStream* stream);
	void						Unpack						(NetConnection* conn, BitStream* stream);
	void						PackChainEP					(NetConnection* conn, BitStream* stream);
	void						UnpackChainEP				(NetConnection* conn, BitStream* stream);

	Status						GetStatus					() const { return m_Status; }

	void						AdvanceSpell				(F32 dt);
	void						AdvanceScheduleSpell		(F32 dt);
	void						AdvanceChainEP				(F32 dt);
	void						DeleteSpellEffect			(U32 id);

	U32							GetId						() { return m_SkillId; }
	void						SetSource					(GameObject* src) { m_Source = src; }
	GameObject*					GetSource					() { return m_Source.getObject(); }
	GameObject*					GetTarget					() { return m_Target.getObject(); }
	Point3F&					GetDest						() { return m_Dest; }

	bool						SetPreSpell					(U32 skillId, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);
	PreSpell&					GetPreSpell					();
	void						ResetPreSpell				();
	void						AddChainEP					(U32 skillId, U32 carrier);

	// 具体的计算伤害函数
	static bool					CalculateDamage				(Stats& stats, GameObject* pSource, U32 buffId, Vector<U32>* pPlusList);

	static void					SpellDamage					(GameObject* pSource, GameObject* pTarget, U32 buffId, Vector<U32>* pPlusList = NULL);
	static void					SpellDamage_Region			(GameObject* pSource, Point3F& pos, U32 skillId, bool skillEP = false, Vector<U32>* pPlusList = NULL);

	static void					SpellDamage					(Stats& stats, GameObject* pSource, GameObject* pTarget, U32 buffId);
	static void					SpellDamage_Region			(Stats& stats, GameObject* pSource, Point3F& pos, U32 skillId);

	// 添加延时特效
	static void					AddDelayEP					(SkillData* pSkillData, GameObject* source, GameObject* target);
	static void					OnTrigger					(GameObject* src, GameObject* tgt, SkillData* pData, SkillData::Selectable sl, Stats& stats, bool triSrc = true, bool triTgt = true);

protected:

	bool						OnBegin						();
	bool						OnReadySpell				();
	bool						OnSpellCast					();
	bool						OnChannelCast				();
	bool						OnEnd						();

	void						GotoSpell					(U32 skillid, Spell::Status status);// 强制推进技能
	inline bool					CheckObjects				();									// 检查Soure 和 Target,是否有效
	inline bool					CheckDistance				();									// 检查是否超出施法距离
	void						SetStatus					(Status status);
	void						AddScheduleSpell			();									// 把当前技能加入到延时施法
	bool						OnConsume					();									// 消耗

	static bool					IsEffectObject				(GameObject* pSource, GameObject* pObject, GameObject* pTarget, U32 effectTarget, bool canAtk);
};


