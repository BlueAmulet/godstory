//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/Stats.h"
#include "console/sim.h"
#include <bitset>


// ========================================================================================================================================
//  BuffData
// ========================================================================================================================================

#define BuffLevelDecimalBits 10000
#define Macro_GetBuffId(s,l) ((s)*BuffLevelDecimalBits + l)
#define Macro_GetBuffSeriesId(s) ((s)/BuffLevelDecimalBits)
#define Macro_GetBuffLevel(s) ((s) - Macro_GetBuffSeriesId(s))
#define BUFF_GROUP_BITS 128

class BuffData
{
	friend class BuffRepository;
	friend class Buff;
	friend class BuffTable;
	friend class Spell;
	//--------------------------------------------------------------------------------------------------------
	// ö�� & �ӽṹ
	//--------------------------------------------------------------------------------------------------------

public:

	enum
	{
		TriggerMax					= 4,		// �ܵĴ�����
	};

	// ״̬Ч��
	enum BuffEffect
	{
		Effect_ImmunityPhy	  		= BIT(0),	// ��������
		Effect_ImmunitySpl			= BIT(1),	// ��������
		Effect_Invisibility			= BIT(2),	// ����
		Effect_GuaranteeHit			= BIT(3),	// �ض�����
		Effect_GuaranteeCritical	= BIT(4),	// �ض�����
		Effect_Silence				= BIT(5),	// ��Ĭ
		Effect_Stun					= BIT(6),	// ����
		Effect_Slumber				= BIT(7),	// ˯��
		Effect_Fear					= BIT(8),	// �־�
		Effect_Wilder				= BIT(9),	// ��ʧ
		Effect_Immobility			= BIT(10),	// ����
		Effect_Gravitation			= BIT(11),	// ǣ��
		Effect_KnockDown			= BIT(12),	// ����
		Effect_BeatBack				= BIT(13),	// ����
		Effect_BeatFly				= BIT(14),	// ����
		Effect_Unarm				= BIT(15),	// ��е
		Effect_Rebound				= BIT(16),	// ����
		Effect_Reflect				= BIT(17),	// ����
		Effect_Mock					= BIT(18),	// ����
		Effect_PhyDefMax			= BIT(19),	// ���׼���
		Effect_SplDefMax			= BIT(20),	// ħ������
		Effect_IgnorePhyDef			= BIT(21),	// ���ӻ���
		Effect_IgnoreSplDef			= BIT(22),	// ����ħ��
		Effect_AntiDodge			= BIT(23),	// �������޷���������
		Effect_AntiInvisibility		= BIT(24),	// ����

		// ״̬Ч������
		Buff_Immobility				= Effect_Stun | Effect_Slumber | Effect_Immobility | Effect_KnockDown,
		Buff_Silence				= Effect_Silence | Effect_Stun | Effect_Slumber | Effect_Fear | Effect_Wilder \
										| Effect_Gravitation | Effect_KnockDown | Effect_BeatBack | Effect_BeatFly,
		Buff_CanNotAttack			= Effect_Stun | Effect_Slumber | Effect_Fear | Effect_Wilder \
										| Effect_Gravitation | Effect_KnockDown | Effect_BeatBack | Effect_BeatFly,
		Buff_Animations				= Effect_Stun,
	};

	enum BuffTrigger
	{
		Trigger_None			= 0,			// �޴���
		Trigger_Death			= BIT(0),		// ����
		Trigger_Rise			= BIT(1),		// ����
		Trigger_PhyAttack		= BIT(2),		// ������
		Trigger_UnderPhyAttack	= BIT(3),		// ��������
		Trigger_Hit				= BIT(4),		// ����
		Trigger_Miss			= BIT(5),		// δ����
		Trigger_Dodge			= BIT(6),		// ����
		Trigger_Wound			= BIT(7),		// δ���ܡ�����Ϊ������
		Trigger_CriticalHit		= BIT(8),		// ����
		Trigger_BeCriticalHit	= BIT(9),		// ������
		Trigger_Damage			= BIT(10),		// ����˺�
		Trigger_Hurt			= BIT(11),		// �ܵ��˺�
		Trigger_RemoveTimer		= BIT(12),		// ��ʱ���Ƴ�ͨ��
		Trigger_RemoveCounter	= BIT(13),		// �������Ƴ�ͨ��
		Trigger_RemoveA			= BIT(14),		// �Ƴ�ͨ��A
		Trigger_RemoveB			= BIT(15),		// �Ƴ�ͨ��B
		Trigger_RemoveC			= BIT(16),		// �Ƴ�ͨ��C
		Trigger_Heal			= BIT(17),		// ����
		Trigger_Healed			= BIT(18),		// �ܵ�����
		Trigger_SplAttack		= BIT(19),		// ��������
		Trigger_UnderSplAttack	= BIT(20),		// ����������
		Trigger_BuffTick		= BIT(21),		// ״̬ÿһ��������ֻ�������ܣ�

		Trigger_RemoveMask		= Trigger_RemoveTimer | Trigger_RemoveCounter | Trigger_RemoveA | Trigger_RemoveB | Trigger_RemoveC,
	};

	enum Flags
	{
		Flags_System			= BIT(0),		// ϵͳ״̬��������ʾ��
		Flags_CalcStats			= BIT(1),		// �������ԣ���֮������˺�
		Flags_PlusStats			= BIT(2),		// ��m_Count������ֵ
		Flags_RemoveSelf		= BIT(3),		// ���Ƴ�����״̬֮���Ƿ��Ƴ��Լ�
		Flags_Buff				= BIT(4),		// Buff/Debuff
		Flags_ShowCount			= BIT(5),		// ��ʾ����
		Flags_ShowLimitTime		= BIT(6),		// ��ʾʣ��ʱ�䣨�⻷�಻Ӧ��ʾ��
		Flags_MinusDefence		= BIT(7),		// �����ɷ�Ϊ����ֻ�ڼ����˺����������Ч
		Flags_PlusSrc			= BIT(8),		// �����˺�ʱ�Ƿ����ʩ��������
		Flags_Coexist			= BIT(9),		// ��ͬ��Դ��ͬϵ��״̬�ܷ񹲴�
		Flags_RemoveSameSrc		= BIT(10),		// ֻ�Ƴ�ͬ����Դ��״̬
		Flags_ShieldCountDam	= BIT(11),		// ���Ƿ��¼�˺�/��¼����
		Flags_ShieldConsumeMana	= BIT(12),		// ���Ƿ�����ħ�����ܱض����ļ�������
		Flags_ShieldDamage		= BIT(13),		// �ֵܵ��˺�/����
		Flags_Infinity			= BIT(14),		// ����ʱ��
		Flags_DamDeriveMP		= BIT(15),		// �˺�����/����ȼ������
		Flags_TgtTriDisabled	= BIT(16),		// ʹĿ���޷���������
		Flags_OnAddTickSpell	= BIT(17),		// ��Ӹ�״̬ʱ����״̬һ��
		Flags_CanNotKill		= BIT(18),		// ����������
		Flags_Tame				= BIT(19),		// ����
		Flags_Zero				= BIT(20),		// �˺���Ϊ0
		Flags_DisableCombatLog	= BIT(21),		// ����ս����¼
		Flags_ExistOnDisabled	= BIT(22),		// ����ʱ�Ƴ�
		Flags_ReplaceUpper		= BIT(23),		// ͬϵ�͵ȼ��Ŀ����滻�ߵȼ���
	};

	enum DamageType
	{
		Damage_Phy				= BIT(0),
		Damage_Mu				= BIT(1),
		Damage_Huo				= BIT(2),
		Damage_Tu				= BIT(3),
		Damage_Jin				= BIT(4),
		Damage_Shui				= BIT(5),
		Damage_Pneuma			= BIT(6),
		Damage_Heal				= BIT(7),

		Damage_Spl				= Damage_Mu | Damage_Huo | Damage_Tu | Damage_Jin | Damage_Shui,
	};

	enum TriggerTarget
	{
		TT_Source				= 0,			// Դʩ���߶���
		TT_Carrier,								// ����Я����
		TT_Trigger,								// ������
		TT_Target,								// Ŀ��
		TT_NearestFriend,						// ������ѷ�
		TT_NearestEnemy,						// ��Զ���ѷ�
	};

	struct Trigger
	{
		U32						mask;			// ������������ʲô����´���
		U32						rate;			// ��������
		TriggerTarget			skillTT_A;		// ����A����
		U32						skillId_A;		// ��������A
		TriggerTarget			skillTT_B;		// ����B����
		U32						skillId_B;		// ��������B
		TriggerTarget			skillTT_C;		// ����C����
		U32						skillId_C;		// ��������C
		TriggerTarget			buffTT_A;		// ״̬A����
		U32						buffId_A;		// ����״̬A
		S32						buffCount_A;	// ״̬A������
		TriggerTarget			buffTT_B;		// ״̬B����
		U32						buffId_B;		// ����״̬B
		S32						buffCount_B;	// ״̬B������
		TriggerTarget			buffTT_C;		// ״̬C����
		U32						buffId_C;		// ��ʩ���ߴ���״̬C
		S32						buffCount_C;	// ״̬C������
		StringTableEntry		scriptFun;		// �����ű�
	};

	struct Shield
	{
		U32						absorbRate;		// ������
		U32						triggerRate;	// �������ʣ�������Ϊ��
	};

	//--------------------------------------------------------------------------------------------------------
	// ��Ա����
	//--------------------------------------------------------------------------------------------------------

protected:
	U32			m_BuffId;				// ״̬ID
	U32			m_SeriesId;				// ״̬ϵ��ID
	U32			m_Level;				// ״̬�ȼ�
	StringTableEntry m_Name;			// ״̬����
	StringTableEntry m_Icon;			// ״̬icon
	StringTableEntry m_Text;			// ״̬˵��
	U32			m_PlusId;				// ��ǿID
	U32			m_BuffEffect;			// ״̬Ч��

	Stats		m_Stats;				// ��ֵ����ģ��
	U32			m_Flags;
	U32			m_DamageType;			// �˺�����
	U32			m_PlusSrcRate;			// �����������԰ٷֱ�

	U32			m_TotalMask;			// �����ܼ�
	U32			m_CountMask;			// ��������(m_Count--)
	U32			m_RemoveMask;			// �����Ƴ�״̬
	Trigger		m_Trigger[TriggerMax];	// ����
	Shield		m_ShieldInfo;			// �� ��״̬����

	U32			m_LimitCount;			// ��������
	SimTime		m_LimitTime;			// ����ʱ��
	U32			m_LimitTimes;			// ��Ч����

	std::bitset<BUFF_GROUP_BITS> m_Group;			// ״̬��
	std::bitset<BUFF_GROUP_BITS> m_RemoveGroupA;	// ��Aͨ���Ƴ���״̬��
	std::bitset<BUFF_GROUP_BITS> m_RemoveGroupB;	// ��Bͨ���Ƴ���״̬��
	std::bitset<BUFF_GROUP_BITS> m_RemoveGroupC;	// ��Cͨ���Ƴ���״̬��
	std::bitset<BUFF_GROUP_BITS> m_ImmunityGroup;	// ��ĳ������
	std::bitset<BUFF_GROUP_BITS> m_RestrainGroup;	// ��ĳ������

	U32			m_BuffEP;				// ״̬��Ч
	U32			m_ShapesSetId;			// ״̬ģ��
	StringTableEntry m_OnAddScript;		// ���״̬�ű�
	StringTableEntry m_OnRemoveScript;	// �Ƴ�״̬�ű�

	//--------------------------------------------------------------------------------------------------------
	// ��Ա����
	//--------------------------------------------------------------------------------------------------------

public:
	BuffData();

	inline Stats&			GetStats				() { return m_Stats; }
	inline bool				IsFlags					(U32 flags) { return m_Flags & flags; }
	inline U32				GetBuffId				() { return m_BuffId; }
	inline U32				GetSeriesId				() { return m_SeriesId; }
	inline U32				GetLevel				() { return m_Level; }
	inline StringTableEntry	GetName					() { return m_Name; }
	inline StringTableEntry	GetIcon					() { return m_Icon; }
	inline StringTableEntry	GetText					() { return m_Text; }
	inline U32				GetDamageType			() { return m_DamageType; }
	inline U32				GetEffect				() { return m_BuffEffect; }
	// ״̬��ǿ��غ���
	inline void				Clear					() { dMemset(this, 0, sizeof(BuffData));}
	void					Plus					(const BuffData& _orig, const BuffData& _plus);
	void					PlusDone				();
};

extern BuffData g_BuffData;

// ========================================================================================================================================
//  BuffRepository
// ========================================================================================================================================

class BuffRepository
{
public:
	typedef stdext::hash_map<U32, BuffData*> BuffDataMap;

	BuffDataMap m_BuffDataMap;

public:
	BuffRepository();
	~BuffRepository();

	bool				Read();
	void				Clear();
	BuffData*			GetBuff(U32 BuffId);
	Stats*				GetBuffStats(U32 BuffId);
	bool				Insert(BuffData&);
};

extern BuffRepository g_BuffRepository;


