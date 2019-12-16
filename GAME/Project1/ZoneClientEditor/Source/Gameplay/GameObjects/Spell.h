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
	SimTime m_BeginCastTime;			// ��ʼʩ��ʱ��
	SimTime m_CastCooldown;				// ��ȴ��ʱ����
	SimTime m_BeginAnimateTime;			// ������ʼʱ�䣬��m_BeginCastTimeһ��
	SimTime m_AnimateDuration;			// ��������ʱ�䣬Transition��sequence��ʱ���ܺ�
	SimTime m_CooldownTime;				// ��ȴ��ɵ�ʱ��

	F32 m_TimeScale;					// ��������
	bool m_Enable;						// �Ƿ������Զ�ʩ��

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
	S32							mRemainTime;				// ʣ��ʱ��
	S32							mLimitTime;					// ��ʱ��
	S32							mRemainTimes;				// ʣ�����
	S32							mLimitTimes;				// �ܴ���

	SimObjectPtr<GameObject>	mSource;					// Դ����
	bool						mIsTarget;					// ʩ�ŵĶ�����Ŀ��������������
	SimObjectPtr<GameObject>	mTarget;					// Ŀ�����
	Point3F						mDest;						// Ŀ������
	SkillData*					mSkillData;					// ��ǰ��������
	Stats						mDamageStats;				// �����˺�����
	U32							mEffectPacket;				// ��Ч��
	SkillData::Selectable		mSelectable;				// Ŀ��ѡ��ͨ��

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

	//��ǰ���ܵ��ƽ�״̬
	enum Status
	{
		Status_None				= 0,						// ��
		Status_Begin,										// ��ʼ
		Status_ReadySpell,									// ����
		Status_SpellCast,									// ʩ��
		Status_ChannelCast,									// ����ʩ��
		Status_End,											// ����
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
		SkillData*				pSkillData;					// ��ǰ��������
		ItemInterface			itemInter;					// ��Ʒʹ�ýӿ�

		PreSpell():pSkillData(NULL) {}
	};

	struct ChainEP
	{
		bool						overwrite;					// �ɷ�д��
		S32							remainTime;					// ʣ��ʱ��
		U32							skillId;					// ʣ��ʱ��
		Vector<U32>					carriers;					// ���еĶ���

		void clear() {overwrite=true; remainTime=ChainEPTimeMs; skillId=0; carriers.clear();}

		ChainEP() {clear();}
	};

protected:

	U32							m_SkillId;
	bool						m_bCast;					// �Ƿ���ʩ�ţ��ͻ��ˣ�
	bool						m_bReady;					// �Ƿ��ѿ�ʼ�������ͻ��ˣ�
	bool						m_bChannel;					// �Ƿ��ѿ�ʼ�����ͷţ��ͻ��ˣ�
	bool						m_bConsume;					// �Ƿ��Ѿ��۳�����������
	bool						m_bTarget;					// ʩ�ŵĶ�����Ŀ��������������
	bool						m_bBreak;					// �Ƿ�����Ϊ����϶�ֹͣ
	bool						m_bLastBreak;				// �Ƿ�����Ϊ����϶�ֹͣ
	bool						m_bCastSuc;					// �Ƿ�ʩ�ųɹ�

	Status						m_Status;					// �ƽ�״̬
	S32							m_ReadySpellTime;			// ����ʱ��
	S32							m_ChannelCastTime;			// ����ʩ��ʱ��
	S32							m_RemainTimes;				// ʣ�����
	S32							m_LimitTimes;				// �ܴ���
	S32							m_VelDelayTicks;			// �ж��ٶ�ʱ��������ʱ

	SimObjectPtr<GameObject>	m_Source;					// ʩ�Ŷ���
	SimObjectPtr<GameObject>	m_Target;					// Ŀ�����
	Point3F						m_Dest;						// Ŀ������
	SkillData::Selectable		m_Selectable;				// Ŀ��ѡ��ͨ��

	Skill*						m_pSkill;					// ��ǰ��������
	SkillData*					m_pSkillData;				// ��ǰ��������
	SkillData					m_PlusData;					// ��ǰ�����ܼӳɺ�����ݣ���Щֵ��ȫ

	U32							m_SourceEffect;				// Դ������Ч
	U32							m_TargetEffect;				// Ŀ�������Ч

	Vector<ScheduleSpell*>		m_ScheduleSpell;			// ��ʱʩ������켣����
	PreSpell					m_PreSpell;					// Ԥ������
	ChainEP						m_ChainEP;					// ��ʽ��Ч
	ItemInterface				m_ItemInterface;			// ʹ����Ʒ�ӿ�
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
	void						BreakSpell					();										// �жϼ���ʩ��
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

	// ����ļ����˺�����
	static bool					CalculateDamage				(Stats& stats, GameObject* pSource, U32 buffId, Vector<U32>* pPlusList);

	static void					SpellDamage					(GameObject* pSource, GameObject* pTarget, U32 buffId, Vector<U32>* pPlusList = NULL);
	static void					SpellDamage_Region			(GameObject* pSource, Point3F& pos, U32 skillId, bool skillEP = false, Vector<U32>* pPlusList = NULL);

	static void					SpellDamage					(Stats& stats, GameObject* pSource, GameObject* pTarget, U32 buffId);
	static void					SpellDamage_Region			(Stats& stats, GameObject* pSource, Point3F& pos, U32 skillId);

	// �����ʱ��Ч
	static void					AddDelayEP					(SkillData* pSkillData, GameObject* source, GameObject* target);
	static void					OnTrigger					(GameObject* src, GameObject* tgt, SkillData* pData, SkillData::Selectable sl, Stats& stats, bool triSrc = true, bool triTgt = true);

protected:

	bool						OnBegin						();
	bool						OnReadySpell				();
	bool						OnSpellCast					();
	bool						OnChannelCast				();
	bool						OnEnd						();

	void						GotoSpell					(U32 skillid, Spell::Status status);// ǿ���ƽ�����
	inline bool					CheckObjects				();									// ���Soure �� Target,�Ƿ���Ч
	inline bool					CheckDistance				();									// ����Ƿ񳬳�ʩ������
	void						SetStatus					(Status status);
	void						AddScheduleSpell			();									// �ѵ�ǰ���ܼ��뵽��ʱʩ��
	bool						OnConsume					();									// ����

	static bool					IsEffectObject				(GameObject* pSource, GameObject* pObject, GameObject* pTarget, U32 effectTarget, bool canAtk);
};


