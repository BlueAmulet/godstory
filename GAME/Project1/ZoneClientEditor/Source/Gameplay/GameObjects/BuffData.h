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
	// 枚举 & 子结构
	//--------------------------------------------------------------------------------------------------------

public:

	enum
	{
		TriggerMax					= 4,		// 总的触发数
	};

	// 状态效果
	enum BuffEffect
	{
		Effect_ImmunityPhy	  		= BIT(0),	// 物理免疫
		Effect_ImmunitySpl			= BIT(1),	// 法术免疫
		Effect_Invisibility			= BIT(2),	// 隐身
		Effect_GuaranteeHit			= BIT(3),	// 必定命中
		Effect_GuaranteeCritical	= BIT(4),	// 必定暴击
		Effect_Silence				= BIT(5),	// 沉默
		Effect_Stun					= BIT(6),	// 昏迷
		Effect_Slumber				= BIT(7),	// 睡眠
		Effect_Fear					= BIT(8),	// 恐惧
		Effect_Wilder				= BIT(9),	// 迷失
		Effect_Immobility			= BIT(10),	// 定身
		Effect_Gravitation			= BIT(11),	// 牵引
		Effect_KnockDown			= BIT(12),	// 击倒
		Effect_BeatBack				= BIT(13),	// 击退
		Effect_BeatFly				= BIT(14),	// 击飞
		Effect_Unarm				= BIT(15),	// 缴械
		Effect_Rebound				= BIT(16),	// 反弹
		Effect_Reflect				= BIT(17),	// 反射
		Effect_Mock					= BIT(18),	// 嘲讽
		Effect_PhyDefMax			= BIT(19),	// 护甲极限
		Effect_SplDefMax			= BIT(20),	// 魔抗极限
		Effect_IgnorePhyDef			= BIT(21),	// 无视护甲
		Effect_IgnoreSplDef			= BIT(22),	// 无视魔抗
		Effect_AntiDodge			= BIT(23),	// 禁闪，无法发动闪避
		Effect_AntiInvisibility		= BIT(24),	// 反隐

		// 状态效果分类
		Buff_Immobility				= Effect_Stun | Effect_Slumber | Effect_Immobility | Effect_KnockDown,
		Buff_Silence				= Effect_Silence | Effect_Stun | Effect_Slumber | Effect_Fear | Effect_Wilder \
										| Effect_Gravitation | Effect_KnockDown | Effect_BeatBack | Effect_BeatFly,
		Buff_CanNotAttack			= Effect_Stun | Effect_Slumber | Effect_Fear | Effect_Wilder \
										| Effect_Gravitation | Effect_KnockDown | Effect_BeatBack | Effect_BeatFly,
		Buff_Animations				= Effect_Stun,
	};

	enum BuffTrigger
	{
		Trigger_None			= 0,			// 无触发
		Trigger_Death			= BIT(0),		// 死亡
		Trigger_Rise			= BIT(1),		// 复活
		Trigger_PhyAttack		= BIT(2),		// 物理攻击
		Trigger_UnderPhyAttack	= BIT(3),		// 被物理攻击
		Trigger_Hit				= BIT(4),		// 击中
		Trigger_Miss			= BIT(5),		// 未击中
		Trigger_Dodge			= BIT(6),		// 闪避
		Trigger_Wound			= BIT(7),		// 未闪避、可视为被击中
		Trigger_CriticalHit		= BIT(8),		// 暴击
		Trigger_BeCriticalHit	= BIT(9),		// 被暴击
		Trigger_Damage			= BIT(10),		// 造成伤害
		Trigger_Hurt			= BIT(11),		// 受到伤害
		Trigger_RemoveTimer		= BIT(12),		// 计时器移除通道
		Trigger_RemoveCounter	= BIT(13),		// 计数器移除通道
		Trigger_RemoveA			= BIT(14),		// 移除通道A
		Trigger_RemoveB			= BIT(15),		// 移除通道B
		Trigger_RemoveC			= BIT(16),		// 移除通道C
		Trigger_Heal			= BIT(17),		// 治疗
		Trigger_Healed			= BIT(18),		// 受到治疗
		Trigger_SplAttack		= BIT(19),		// 法术攻击
		Trigger_UnderSplAttack	= BIT(20),		// 被法术攻击
		Trigger_BuffTick		= BIT(21),		// 状态每一跳触发（只触发技能）

		Trigger_RemoveMask		= Trigger_RemoveTimer | Trigger_RemoveCounter | Trigger_RemoveA | Trigger_RemoveB | Trigger_RemoveC,
	};

	enum Flags
	{
		Flags_System			= BIT(0),		// 系统状态（不会显示）
		Flags_CalcStats			= BIT(1),		// 计算属性，反之则计算伤害
		Flags_PlusStats			= BIT(2),		// 以m_Count叠加数值
		Flags_RemoveSelf		= BIT(3),		// 在移除其他状态之后，是否移除自己
		Flags_Buff				= BIT(4),		// Buff/Debuff
		Flags_ShowCount			= BIT(5),		// 显示个数
		Flags_ShowLimitTime		= BIT(6),		// 显示剩余时间（光环类不应显示）
		Flags_MinusDefence		= BIT(7),		// 防御可否为负，只在计算伤害的情况下有效
		Flags_PlusSrc			= BIT(8),		// 计算伤害时是否叠加施放者属性
		Flags_Coexist			= BIT(9),		// 不同来源的同系列状态能否共存
		Flags_RemoveSameSrc		= BIT(10),		// 只移除同样来源的状态
		Flags_ShieldCountDam	= BIT(11),		// 盾是否记录伤害/记录次数
		Flags_ShieldConsumeMana	= BIT(12),		// 盾是否消耗魔法（盾必定消耗计数器）
		Flags_ShieldDamage		= BIT(13),		// 盾抵挡伤害/治疗
		Flags_Infinity			= BIT(14),		// 无限时间
		Flags_DamDeriveMP		= BIT(15),		// 伤害吸蓝/法力燃烧吸蓝
		Flags_TgtTriDisabled	= BIT(16),		// 使目标无法被动触发
		Flags_OnAddTickSpell	= BIT(17),		// 添加该状态时触发状态一跳
		Flags_CanNotKill		= BIT(18),		// 不可致死的
		Flags_Tame				= BIT(19),		// 捕获技
		Flags_Zero				= BIT(20),		// 伤害可为0
		Flags_DisableCombatLog	= BIT(21),		// 屏蔽战斗记录
		Flags_ExistOnDisabled	= BIT(22),		// 死亡时移除
		Flags_ReplaceUpper		= BIT(23),		// 同系低等级的可以替换高等级的
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
		TT_Source				= 0,			// 源施放者对象
		TT_Carrier,								// 本体携带者
		TT_Trigger,								// 触发者
		TT_Target,								// 目标
		TT_NearestFriend,						// 最近的友方
		TT_NearestEnemy,						// 最远的友方
	};

	struct Trigger
	{
		U32						mask;			// 触发条件，在什么情况下触发
		U32						rate;			// 触发几率
		TriggerTarget			skillTT_A;		// 技能A对象
		U32						skillId_A;		// 触发技能A
		TriggerTarget			skillTT_B;		// 技能B对象
		U32						skillId_B;		// 触发技能B
		TriggerTarget			skillTT_C;		// 技能C对象
		U32						skillId_C;		// 触发技能C
		TriggerTarget			buffTT_A;		// 状态A对象
		U32						buffId_A;		// 触发状态A
		S32						buffCount_A;	// 状态A叠加数
		TriggerTarget			buffTT_B;		// 状态B对象
		U32						buffId_B;		// 触发状态B
		S32						buffCount_B;	// 状态B叠加数
		TriggerTarget			buffTT_C;		// 状态C对象
		U32						buffId_C;		// 对施放者触发状态C
		S32						buffCount_C;	// 状态C叠加数
		StringTableEntry		scriptFun;		// 触发脚本
	};

	struct Shield
	{
		U32						absorbRate;		// 吸收率
		U32						triggerRate;	// 触发几率，非零则为盾
	};

	//--------------------------------------------------------------------------------------------------------
	// 成员变量
	//--------------------------------------------------------------------------------------------------------

protected:
	U32			m_BuffId;				// 状态ID
	U32			m_SeriesId;				// 状态系列ID
	U32			m_Level;				// 状态等级
	StringTableEntry m_Name;			// 状态名称
	StringTableEntry m_Icon;			// 状态icon
	StringTableEntry m_Text;			// 状态说明
	U32			m_PlusId;				// 增强ID
	U32			m_BuffEffect;			// 状态效果

	Stats		m_Stats;				// 数值属性模板
	U32			m_Flags;
	U32			m_DamageType;			// 伤害类型
	U32			m_PlusSrcRate;			// 叠加人物属性百分比

	U32			m_TotalMask;			// 触发总计
	U32			m_CountMask;			// 触发计数(m_Count--)
	U32			m_RemoveMask;			// 触发移除状态
	Trigger		m_Trigger[TriggerMax];	// 触发
	Shield		m_ShieldInfo;			// 盾 的状态数据

	U32			m_LimitCount;			// 最大叠加数
	SimTime		m_LimitTime;			// 持续时间
	U32			m_LimitTimes;			// 生效次数

	std::bitset<BUFF_GROUP_BITS> m_Group;			// 状态组
	std::bitset<BUFF_GROUP_BITS> m_RemoveGroupA;	// 用A通道移除的状态组
	std::bitset<BUFF_GROUP_BITS> m_RemoveGroupB;	// 用B通道移除的状态组
	std::bitset<BUFF_GROUP_BITS> m_RemoveGroupC;	// 用C通道移除的状态组
	std::bitset<BUFF_GROUP_BITS> m_ImmunityGroup;	// 被某组免疫
	std::bitset<BUFF_GROUP_BITS> m_RestrainGroup;	// 被某组抑制

	U32			m_BuffEP;				// 状态特效
	U32			m_ShapesSetId;			// 状态模型
	StringTableEntry m_OnAddScript;		// 添加状态脚本
	StringTableEntry m_OnRemoveScript;	// 移除状态脚本

	//--------------------------------------------------------------------------------------------------------
	// 成员函数
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
	// 状态增强相关函数
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


