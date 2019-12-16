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
	// 枚举 & 子结构
	//--------------------------------------------------------------------------------------------------------

public:

	enum Flags
	{
		Flags_Passive			= BIT(0),	// 被动技能
		Flags_TargetAlive		= BIT(1),	// 目标必须活着/死亡
		Flags_BreakChannel		= BIT(2),	// 在channelCast时，能否施放其他法术
		Flags_AutoCast			= BIT(3),	// 是否是自动攻击技能
		Flags_AutoCastDelay		= BIT(4),	// 自动攻击延时
		Flags_AntiSilence		= BIT(5),	// 在被封魔状态时仍能够释放
		Flags_AntiStun			= BIT(6),	// 在被昏迷状态时仍能够释放
		Flags_AntiFear			= BIT(7),	// 在被恐惧状态时仍能够释放
		Flags_AntiWilder		= BIT(8),	// 在被迷失状态时仍能够释放
		Flags_ScheduleSpell		= BIT(9),	// 延时法术（如有轨迹的技能）
		Flags_SetCDGlobal		= BIT(10),	// 是否产生公共CD
		Flags_IgnoreCDGlobal	= BIT(11),	// 是否忽略公共CD
		Flags_Learnable			= BIT(12),	// 可学习的
		Flags_Show				= BIT(13),	// 屏幕效果
		Flags_TriPhyEnabled		= BIT(14),	// 开启攻击时触发
		Flags_TriSplEnabled		= BIT(15),	// 开启施法时触发
		Flags_ChainEP			= BIT(16),	// 链式特效
		Flags_Tame				= BIT(17),	// 抓捕技能
		Flags_Melee				= BIT(18),	// 近战技能
		Flags_HideChannel		= BIT(19),	// 隐藏吟唱条
	};

	// 技能指向广义目标
	enum Object
	{
		Object_Destine			= 0,		// 手动指定，必须是范围技能
		Object_Self,						// 自身
		Object_Pet,							// 自己的宠物
		Object_Target,						// 当前目标对象
	};

	// 目标对象限制，只有当技能指向对象为Object_Target时有用
	enum Target
	{
		Target_Friend			= BIT(0),	// 友方
		Target_Enemy			= BIT(1),	// 敌方
		Target_Neutral			= BIT(2),	// 中立方，暂时无用
		Target_DisableSelf		= BIT(3),	// 目标不能为自身
	};

	// 目标选择通道
	enum Selectable
	{
		Selectable_A			= 0,		// 目标选择通道A
		Selectable_B,						// 目标选择通道B
		SelectableNum,						// 目标选择通道数量
	};

	// 技能作用对象
	enum EffectTarget
	{
		EffectTarget_Self		= BIT(0),	// 自身
		EffectTarget_Target		= BIT(1),	// 目标（即SkillData::Object指向的对象）
		EffectTarget_Friend		= BIT(2),	// 友方（不包括自身、目标，下同）
		EffectTarget_Enemy		= BIT(3),	// 敌方
		EffectTarget_Neutral	= BIT(4),	// 中立方
		EffectTarget_Team		= BIT(5),	// 目标的队友
		EffectTarget_Raid		= BIT(6),	// 目标的团队
	};

	// 技能区域
	enum Region
	{
		Region_Point			= 0,		// 点，单一目标
		Region_Rectangle,					// 矩形，直线
		Region_Circle,						// 圆
	};

	enum SkillEffect
	{
		Effect_None				= 0,		// 无
		Effect_Charge			= BIT(0),	// 冲锋
		Effect_Blink			= BIT(1),	// 闪现
		Effect_Jump				= BIT(2),	// 跳跃
	};

	// 技能前置状态运算符
	enum PreBuffOp
	{
		Op_OR_0					= BIT(0),	// 或/与
		Op_NOT_0				= BIT(1),	// 非
		Op_NOT_1				= BIT(2),	// 非
	};

	// 技能类型
	enum School
	{
		School_Null,						// 综合技能
		School_Mu,
		School_Huo,
		School_Tu,
		School_Jin,
		School_Shui,
		School_Pneuma,						// 元神技能
		School_Trump,						// 元神绝招
	};

	// 门宗限制
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

	// 职业（系）限制
	enum ClassesLimit
	{
		CL_1st					= BIT(_1st),	// 主系可用
		CL_2nd					= BIT(_2nd),	// 副系可用
		CL_3rd					= BIT(_3rd),
		CL_4th					= BIT(_4th),
		CL_5th					= BIT(_5th),
	};

	struct LearnLimit 
	{
		U32 prepSkill;						// 前置技能
		U32 level;							// 等级
		U32 money;							// 金钱
		U32 exp;							// 经验
		U32 item;							// 消耗物品
		U32 family[2];						// 门宗限制职业限制
		StringTableEntry scriptFunction;	// 脚本判断
	};

	struct CastLimit 
	{
		Object		object;					// 技能指向对象
		U32			target;					// 目标限制
		U32			targetSl[SelectableNum];// 目标选择通道，0即为通道关闭
		F32			rangeMin;				// 技能使用最小距离
		F32			rangeMax;				// 技能使用最大距离
		U32			armStatus;				// 武器限制

		U32			srcPreBuff_A;			// 源前置状态SeriesId
		S32			srcPreBuffCount_A;		// 源前置状态数量
		U32			srcPreBuff_B;			// 源前置状态SeriesId
		S32			srcPreBuffCount_B;		// 源前置状态数量
		U32			srcPreBuffOp;			// 源技能前置状态运算符
		U32			tgtPreBuff_A;			// 目标前置状态SeriesId
		S32			tgtPreBuffCount_A;		// 目标前置状态数量
		U32			tgtPreBuff_B;			// 目标前置状态SeriesId
		S32			tgtPreBuffCount_B;		// 目标前置状态数量
		U32			tgtPreBuffOp;			// 目标技能前置状态运算符
	};

	struct EffectLimit 
	{
		U32			effectTarget[SelectableNum];// 技能作用对象
		U32			maxTargets;				// 最大作用个数
		Region		region;					// 作用范围
		Point2F		regionRect;				// 作用范围大小
	};

	struct Cost
	{
		S32 HP;								// 生命
		S32 MP;								// 真气
		S32 PP;								// 元气
		S32 vigor;							// 精力
		S32 vigour;							// 活力
		U32	item;							// 消耗物品
	};

	struct Cast
	{
		SimTime readyTime;					// 吟唱时间
		SimTime channelTime;				// 持续施法时间
		SimTime cooldown;					// CD时间
		S32 cdGroup;						// CD组

		GameObjectData::Animations readySpell;
		GameObjectData::Animations spellCast;
		GameObjectData::Animations spellCastCritical;
		GameObjectData::Animations channelCast;

		U32 sourceEP;						// 源对象特效
		U32 targetEP;						// 目标对象特效
		U32 sourceDelayEP;					// 延时特效
		U32 targetDelayEP;					// 延时特效（如爆炸特效）
		U32 environmentDelayEP;				// 环境延时特效

		F32 projectileVelocity;				// 子弹飞行速度
		F32 attenuation;					// 伤害衰减（0～1）

		S32 hate;							// 仇恨
		F32 modifyHate;						// 立即修改仇恨（-1，1）百分比
	};

	struct Operation 
	{
		U32 buffId;							// 数值属性模板ID

		U32 channelSrcBuff;					// 持续施法自身添加的BUFF
		U32 channelTgtBuff;					// 持续施法目标添加的BUFF
	};

	struct Trigger
	{
		U32					rate;			// 触发几率
		U32					skillId_A;		// 触发技能A
		U32					skillId_B;		// 触发技能B
		U32					skillId_C;		// 对施放者触发技能C
		U32					buffId_A;		// 触发状态A
		S32					buffCount_A;	// 状态A叠加数
		U32					buffId_B;		// 触发状态B
		S32					buffCount_B;	// 状态B叠加数
		U32					buffId_C;		// 对施放者触发状态C
		S32					buffCount_C;	// 状态C叠加数
		StringTableEntry	scriptFun;		// 触发脚本
	};

	//--------------------------------------------------------------------------------------------------------
	// 成员变量
	//--------------------------------------------------------------------------------------------------------

public:
	U32				m_SkillId;				// 技能ID
	U32				m_SeriesId;				// 技能系列ID
	U32				m_Level;				// 技能等级
	StringTableEntry m_Name;				// 技能名称
	StringTableEntry m_Icon;				// 技能icon
	StringTableEntry m_Text;				// 技能说明
	StringTableEntry m_NameTexture;			// 技能名称图片
	School			m_School;				// 技能系

	U32				m_Flags;				// 技能标识
	U32				m_Effect;				// 技能效果

	LearnLimit		m_LearnLimit;			// 学习限制
	CastLimit		m_CastLimit;			// 施放限制
	EffectLimit		m_EffectLimit;			// 作用限制
	Cost			m_Cost;					// 技能施放消耗
	Cast			m_Cast;					// 施放
	Operation		m_Operation[SelectableNum];// 技能的数值处理，分通道
	Trigger			m_Trigger[SelectableNum];// 技能释放完成后的触发，分通道

#ifdef NTJ_CLIENT
	GFXTexHandle*	m_NameTexHandle;		// 技能名称图片
#endif
	static StringTableEntry sm_SchoolName[];
	//--------------------------------------------------------------------------------------------------------
	// 成员函数
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
	// 技能增强相关函数
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