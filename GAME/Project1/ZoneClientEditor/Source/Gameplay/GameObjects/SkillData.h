//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/Stats.h"
#include "Gameplay/GameObjects/GameObjectData.h"
#include "math/mPoint.h"
#include "console/sim.h"
#include "Common/PlayerStruct.h"

#include "Common/rtti.h"

#include <hash_map>
#include <hash_set>


// ========================================================================================================================================
//  SkillData
// ========================================================================================================================================

#define SkillLevelDecimalBits 10000
#define Macro_GetSkillId(s,l) ((s)*SkillLevelDecimalBits + l)
#define Macro_GetSkillSeriesId(s) ((s)/SkillLevelDecimalBits)
#define Macro_GetSkillLevel(s) ((s)%SkillLevelDecimalBits)

class SkillData
{
	friend class SkillRepository;
	friend class Spell;
	friend class Skill;
	friend class SkillTable;
	friend class GameObject;
	//--------------------------------------------------------------------------------------------------------
	// ö�� & �ӽṹ
	//--------------------------------------------------------------------------------------------------------

public:

	enum Flags
	{
		Flags_Passive			= BIT(0),	// ��������
		Flags_TargetAlive		= BIT(1),	// Ŀ��������/����
		Flags_BreakChannel		= BIT(2),	// ��channelCastʱ���ܷ�ʩ����������
		Flags_AutoCast			= BIT(3),	// �Ƿ����Զ���������
		Flags_AutoCastDelay		= BIT(4),	// �Զ�������ʱ
		Flags_AntiSilence		= BIT(5),	// �ڱ���ħ״̬ʱ���ܹ��ͷ�
		Flags_AntiStun			= BIT(6),	// �ڱ�����״̬ʱ���ܹ��ͷ�
		Flags_AntiFear			= BIT(7),	// �ڱ��־�״̬ʱ���ܹ��ͷ�
		Flags_AntiWilder		= BIT(8),	// �ڱ���ʧ״̬ʱ���ܹ��ͷ�
		Flags_ScheduleSpell		= BIT(9),	// ��ʱ���������й켣�ļ��ܣ�
		Flags_SetCDGlobal		= BIT(10),	// �Ƿ��������CD
		Flags_IgnoreCDGlobal	= BIT(11),	// �Ƿ���Թ���CD
		Flags_Learnable			= BIT(12),	// ��ѧϰ��
		Flags_Show				= BIT(13),	// ��ĻЧ��
		Flags_TriPhyEnabled		= BIT(14),	// ��������ʱ����
		Flags_TriSplEnabled		= BIT(15),	// ����ʩ��ʱ����
		Flags_ChainEP			= BIT(16),	// ��ʽ��Ч
		Flags_Tame				= BIT(17),	// ץ������
		Flags_Melee				= BIT(18),	// ��ս����
		Flags_HideChannel		= BIT(19),	// ����������
	};

	// ����ָ�����Ŀ��
	enum Object
	{
		Object_Destine			= 0,		// �ֶ�ָ���������Ƿ�Χ����
		Object_Self,						// ����
		Object_Pet,							// �Լ��ĳ���
		Object_Target,						// ��ǰĿ�����
	};

	// Ŀ��������ƣ�ֻ�е�����ָ�����ΪObject_Targetʱ����
	enum Target
	{
		Target_Friend			= BIT(0),	// �ѷ�
		Target_Enemy			= BIT(1),	// �з�
		Target_Neutral			= BIT(2),	// ����������ʱ����
		Target_DisableSelf		= BIT(3),	// Ŀ�겻��Ϊ����
	};

	// Ŀ��ѡ��ͨ��
	enum Selectable
	{
		Selectable_A			= 0,		// Ŀ��ѡ��ͨ��A
		Selectable_B,						// Ŀ��ѡ��ͨ��B
		SelectableNum,						// Ŀ��ѡ��ͨ������
	};

	// �������ö���
	enum EffectTarget
	{
		EffectTarget_Self		= BIT(0),	// ����
		EffectTarget_Target		= BIT(1),	// Ŀ�꣨��SkillData::Objectָ��Ķ���
		EffectTarget_Friend		= BIT(2),	// �ѷ�������������Ŀ�꣬��ͬ��
		EffectTarget_Enemy		= BIT(3),	// �з�
		EffectTarget_Neutral	= BIT(4),	// ������
		EffectTarget_Team		= BIT(5),	// Ŀ��Ķ���
		EffectTarget_Raid		= BIT(6),	// Ŀ����Ŷ�
	};

	// ��������
	enum Region
	{
		Region_Point			= 0,		// �㣬��һĿ��
		Region_Rectangle,					// ���Σ�ֱ��
		Region_Circle,						// Բ
	};

	enum SkillEffect
	{
		Effect_None				= 0,		// ��
		Effect_Charge			= BIT(0),	// ���
		Effect_Blink			= BIT(1),	// ����
		Effect_Jump				= BIT(2),	// ��Ծ
	};

	// ����ǰ��״̬�����
	enum PreBuffOp
	{
		Op_OR_0					= BIT(0),	// ��/��
		Op_NOT_0				= BIT(1),	// ��
		Op_NOT_1				= BIT(2),	// ��
	};

	// ��������
	enum School
	{
		School_Null,						// �ۺϼ���
		School_Mu,
		School_Huo,
		School_Tu,
		School_Jin,
		School_Shui,
		School_Pneuma,						// Ԫ����
		School_Trump,						// Ԫ�����
	};

	// ��������
	enum FamilyLimit
	{
		FL_None					= BIT(Family_None),
		FL_Sheng				= BIT(Family_Sheng),
		FL_Fo					= BIT(Family_Fo),
		FL_Xian					= BIT(Family_Xian),
		FL_Jing					= BIT(Family_Jing),
		FL_Gui					= BIT(Family_Gui),
		FL_Guai					= BIT(Family_Guai),
		FL_Yao					= BIT(Family_Yao),
		FL_Mo					= BIT(Family_Mo),
	};

	// ְҵ��ϵ������
	enum ClassesLimit
	{
		CL_1st					= BIT(_1st),	// ��ϵ����
		CL_2nd					= BIT(_2nd),	// ��ϵ����
		CL_3rd					= BIT(_3rd),
		CL_4th					= BIT(_4th),
		CL_5th					= BIT(_5th),
	};

	struct LearnLimit 
	{
		U32 prepSkill;						// ǰ�ü���
		U32 level;							// �ȼ�
		U32 money;							// ��Ǯ
		U32 exp;							// ����
		U32 item;							// ������Ʒ
		U32 family[2];						// ��������ְҵ����
		StringTableEntry scriptFunction;	// �ű��ж�
	};

	struct CastLimit 
	{
		Object		object;					// ����ָ�����
		U32			target;					// Ŀ������
		U32			targetSl[SelectableNum];// Ŀ��ѡ��ͨ����0��Ϊͨ���ر�
		F32			rangeMin;				// ����ʹ����С����
		F32			rangeMax;				// ����ʹ��������
		U32			armStatus;				// ��������

		U32			srcPreBuff_A;			// Դǰ��״̬SeriesId
		S32			srcPreBuffCount_A;		// Դǰ��״̬����
		U32			srcPreBuff_B;			// Դǰ��״̬SeriesId
		S32			srcPreBuffCount_B;		// Դǰ��״̬����
		U32			srcPreBuffOp;			// Դ����ǰ��״̬�����
		U32			tgtPreBuff_A;			// Ŀ��ǰ��״̬SeriesId
		S32			tgtPreBuffCount_A;		// Ŀ��ǰ��״̬����
		U32			tgtPreBuff_B;			// Ŀ��ǰ��״̬SeriesId
		S32			tgtPreBuffCount_B;		// Ŀ��ǰ��״̬����
		U32			tgtPreBuffOp;			// Ŀ�꼼��ǰ��״̬�����
	};

	struct EffectLimit 
	{
		U32			effectTarget[SelectableNum];// �������ö���
		U32			maxTargets;				// ������ø���
		Region		region;					// ���÷�Χ
		Point2F		regionRect;				// ���÷�Χ��С
	};

	struct Cost
	{
		S32 HP;								// ����
		S32 MP;								// ����
		S32 PP;								// Ԫ��
		S32 vigor;							// ����
		S32 vigour;							// ����
		U32	item;							// ������Ʒ
	};

	struct Cast
	{
		SimTime readyTime;					// ����ʱ��
		SimTime channelTime;				// ����ʩ��ʱ��
		SimTime cooldown;					// CDʱ��
		S32 cdGroup;						// CD��

		GameObjectData::Animations readySpell;
		GameObjectData::Animations spellCast;
		GameObjectData::Animations spellCastCritical;
		GameObjectData::Animations channelCast;

		U32 sourceEP;						// Դ������Ч
		U32 targetEP;						// Ŀ�������Ч
		U32 sourceDelayEP;					// ��ʱ��Ч
		U32 targetDelayEP;					// ��ʱ��Ч���籬ը��Ч��
		U32 environmentDelayEP;				// ������ʱ��Ч

		F32 projectileVelocity;				// �ӵ������ٶ�
		F32 attenuation;					// �˺�˥����0��1��

		S32 hate;							// ���
		F32 modifyHate;						// �����޸ĳ�ޣ�-1��1���ٷֱ�
	};

	struct Operation 
	{
		U32 buffId;							// ��ֵ����ģ��ID

		U32 channelSrcBuff;					// ����ʩ��������ӵ�BUFF
		U32 channelTgtBuff;					// ����ʩ��Ŀ����ӵ�BUFF
	};

	struct Trigger
	{
		U32					rate;			// ��������
		U32					skillId_A;		// ��������A
		U32					skillId_B;		// ��������B
		U32					skillId_C;		// ��ʩ���ߴ�������C
		U32					buffId_A;		// ����״̬A
		S32					buffCount_A;	// ״̬A������
		U32					buffId_B;		// ����״̬B
		S32					buffCount_B;	// ״̬B������
		U32					buffId_C;		// ��ʩ���ߴ���״̬C
		S32					buffCount_C;	// ״̬C������
		StringTableEntry	scriptFun;		// �����ű�
	};

	//--------------------------------------------------------------------------------------------------------
	// ��Ա����
	//--------------------------------------------------------------------------------------------------------

public:
	U32				m_SkillId;				// ����ID
	U32				m_SeriesId;				// ����ϵ��ID
	U32				m_Level;				// ���ܵȼ�
	StringTableEntry m_Name;				// ��������
	StringTableEntry m_Icon;				// ����icon
	StringTableEntry m_Text;				// ����˵��
	StringTableEntry m_NameTexture;			// ��������ͼƬ
	School			m_School;				// ����ϵ

	U32				m_Flags;				// ���ܱ�ʶ
	U32				m_Effect;				// ����Ч��

	LearnLimit		m_LearnLimit;			// ѧϰ����
	CastLimit		m_CastLimit;			// ʩ������
	EffectLimit		m_EffectLimit;			// ��������
	Cost			m_Cost;					// ����ʩ������
	Cast			m_Cast;					// ʩ��
	Operation		m_Operation[SelectableNum];// ���ܵ���ֵ������ͨ��
	Trigger			m_Trigger[SelectableNum];// �����ͷ���ɺ�Ĵ�������ͨ��

#ifdef NTJ_CLIENT
	GFXTexHandle*	m_NameTexHandle;		// ��������ͼƬ
#endif
	static StringTableEntry sm_SchoolName[];
	//--------------------------------------------------------------------------------------------------------
	// ��Ա����
	//--------------------------------------------------------------------------------------------------------

public:
	SkillData();
	~SkillData();
	inline U32				GetID					()	{ return m_SkillId;}
	inline U32				GetSeriesID				()	{ return m_SeriesId;}
	inline U32				GetLevel				()	{ return m_Level;}
	inline bool				IsFlags					(U32 flags) { return m_Flags & flags; }
	inline bool				IsEffect				(U32 effect) { return m_Effect & effect; }
	inline StringTableEntry	GetName					() { return m_Name;}
	inline StringTableEntry	GetIcon					() { return m_Icon;}
	inline StringTableEntry	GetText					() { return m_Text;}
	inline StringTableEntry	GetNameTexture			() { return m_NameTexture;}
	inline StringTableEntry	GetSchoolName			() { return sm_SchoolName[m_School];}
	inline School           GetSchool               () { return m_School;};
	inline S32				GetCDGroup				() { return m_Cast.cdGroup;}

	inline bool				InRange					(F32 len) { return (len >= m_CastLimit.rangeMin && len <= m_CastLimit.rangeMax);}
	inline bool				InRangeSquared			(F32 lenSq) { return (lenSq >= m_CastLimit.rangeMin * m_CastLimit.rangeMin && lenSq <= m_CastLimit.rangeMax * m_CastLimit.rangeMax);}
	inline F32				GetRangeMin				() { return m_CastLimit.rangeMin;}
	inline F32				GetRangeMax				() { return m_CastLimit.rangeMax;}
	inline bool				IsArmStatus				(GameObjectData::ArmStatus arm) { return BIT(arm) & m_CastLimit.armStatus;}

	inline bool				IsRegion				() { return (m_EffectLimit.region != SkillData::Region_Point);}

	inline LearnLimit&		GetLearnLimit			() { return m_LearnLimit;}
	inline CastLimit&		GetCastLimit			() { return m_CastLimit;}
	inline EffectLimit&		GetEffectLimit			() { return m_EffectLimit;}
	inline Cost&			GetCost					() { return m_Cost;}
	inline Cast&			GetCast					() { return m_Cast;}

	const GFXTexHandle*		GetNameTexHandle		();
	// ������ǿ��غ���
	inline void				Clear					() { dMemset(this, 0, sizeof(SkillData));}
	void					Plus					(const SkillData& _orig, const SkillData& _plus);
	void					Minus					(const SkillData& _orig, const SkillData& _plus);
	void					PlusDone				();
};

extern SkillData g_SkillData;


// ========================================================================================================================================
//  SkillRepository
// ========================================================================================================================================

class SkillRepository
{
public:
	typedef stdext::hash_map<U32, SkillData*> SkillDataMap;
	typedef stdext::hash_map<U32, stdext::hash_set<U32> > SkillDataSeriesMap;

	SkillDataMap m_SkillDataMap;
	SkillDataSeriesMap m_SkillDataSeriesMap;

public:
	SkillRepository();
	~SkillRepository();

	bool				Read();
	void				Clear();
	SkillData*			GetSkill(U32 SkillId);
	SkillData*			GetSkill(U32 seriesId, U32 level);
	bool				Insert(SkillData&);

	SkillDataSeriesMap*	GetSeriesMap();
};

extern SkillRepository g_SkillRepository;