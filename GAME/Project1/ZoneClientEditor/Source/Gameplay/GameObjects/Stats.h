//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "platform/types.h"
#include "core/tVector.h"
#include <hash_map>

// ========================================================================================================================================
//  CalcFactor
// ========================================================================================================================================
struct CalcFactor
{
	F32			Stamina_HP;					// 体质->生命
	F32			Mana_MP;					// 法力->真气
	F32			Strength_PhyDamage;			// 力量->物攻
	F32			Intellect_SplDamage;		// 智力->法攻
	F32			Pneuma_PnmDamage;			// 元力->元攻	
	F32			Agility_DodgeRating;		// 敏捷->闪避
	F32			Agility_CriticalRating;		// 敏捷->暴击
};

extern CalcFactor g_CalcFactor[];

// ========================================================================================================================================
//  Stats
// ========================================================================================================================================

// 通用属性
struct Stats
{
	S32			HP;					// 生命
	S32			HP_Pc;				// 生命百分比
	S32			HP_gPc;				// 生命全局百分比
	S32			MP;					// 真气
	S32			MP_Pc;				// 真气百分比
	S32			MP_gPc;				// 真气全局百分比
	S32			PP;					// 元气
	S32			PP_Pc;				// 元气百分比
	S32			PP_gPc;				// 元气全局百分比

	S32			MaxHP;				// 最大生命
	S32			MaxHP_Pc;			// 最大生命百分比
	S32			MaxHP_gPc;			// 最大生命全局百分比
	S32			MaxMP;				// 最大真气
	S32			MaxMP_Pc;			// 最大真气百分比
	S32			MaxMP_gPc;			// 最大真气全局百分比
	S32			MaxPP;				// 最大元气
	S32			MaxPP_Pc;			// 最大元气百分比
	S32			MaxPP_gPc;			// 最大元气全局百分比

	S32			Stamina;			// 体质
	S32			Stamina_Pc;			// 体质百分比
	S32			Stamina_gPc;		// 体质全局百分比
	S32			Mana;				// 法力
	S32			Mana_Pc;			// 法力百分比
	S32			Mana_gPc;			// 法力全局百分比
	S32			Strength;			// 力量
	S32			Strength_Pc;		// 力量百分比
	S32			Strength_gPc;		// 力量全局百分比
	S32			Intellect;			// 智力
	S32			Intellect_Pc;		// 智力百分比
	S32			Intellect_gPc;		// 智力全局百分比
	S32			Agility;			// 身法
	S32			Agility_Pc;			// 身法百分比
	S32			Agility_gPc;		// 身法全局百分比
	S32			Pneuma;				// 元力
	S32			Pneuma_Pc;			// 元力百分比
	S32			Pneuma_gPc;			// 元力全局百分比
	S32			Insight;			// 悟性
	S32			Insight_Pc;			// 悟性百分比
	S32			Insight_gPc;		// 悟性全局百分比
	S32			Luck;				// 福缘
	S32			Luck_Pc;			// 福缘百分比
	S32			Luck_gPc;			// 福缘全局百分比

	S32			Velocity_gPc;		// 移动速度全局百分比
	S32			MountedVelocity_gPc;// 骑乘速度全局百分比
	S32			AttackSpeed;		// 攻击速度时间间隔（毫秒）
	S32			AttackSpeed_gPc;	// 攻击速度全局百分比
	S32			SpellSpeed_gPc;		// 施放速度全局百分比

	S32			CriticalRating;		// 重击等级 影响重击几率
	S32			CriticalTimes;		// 重击全局百分比伤害倍数
	S32			Critical_gPc;		// 重击全局百分比几率
	S32			AntiCriticalRating;	// 防重击等级 影响重击几率
	S32			AntiCritical_gPc;	// 防重击全局百分比几率
	S32			FatalRating;		// 致命等级 影响致命几率
	S32			Fatal_gPc;			// 致命全局百分比几率
	S32			DodgeRating;		// 闪避等级 影响闪避几率
	S32			Dodge_gPc;			// 闪避全局百分比几率
	S32			HitRating;			// 命中等级 影响命中几率
	S32			Hit_gPc;			// 命中全局百分比几率
	S32			DamageHandle_gPc;	// 伤害随机下限全局百分比

	S32			PhyDamage;			// 物理伤害
	S32			PhyDamage_Pc;		// 物理伤害百分比
	S32			PhyDamage_gPc;		// 物理伤害全局百分比
	S32			PhyDefence;			// 物理防御
	S32			PhyDefence_Pc;		// 物理防御百分比
	S32			PhyDefence_gPc;		// 物理防御全局百分比
	S32			MuDamage;			// 木伤害
	S32			MuDamage_Pc;		// 木伤害百分比
	S32			MuDamage_gPc;		// 木伤害全局百分比
	S32			MuDefence;			// 木防御
	S32			MuDefence_Pc;		// 木防御百分比
	S32			MuDefence_gPc;		// 木防御全局百分比
	S32			HuoDamage;			// 火伤害
	S32			HuoDamage_Pc;		// 火伤害百分比
	S32			HuoDamage_gPc;		// 火伤害全局百分比
	S32			HuoDefence;			// 火防御
	S32			HuoDefence_Pc;		// 火防御百分比
	S32			HuoDefence_gPc;		// 火防御全局百分比
	S32			TuDamage;			// 土伤害
	S32			TuDamage_Pc;		// 土伤害百分比
	S32			TuDamage_gPc;		// 土伤害全局百分比
	S32			TuDefence;			// 土防御
	S32			TuDefence_Pc;		// 土防御百分比
	S32			TuDefence_gPc;		// 土防御全局百分比
	S32			JinDamage;			// 金伤害
	S32			JinDamage_Pc;		// 金伤害百分比
	S32			JinDamage_gPc;		// 金伤害全局百分比
	S32			JinDefence;			// 金防御
	S32			JinDefence_Pc;		// 金防御百分比
	S32			JinDefence_gPc;		// 金防御全局百分比
	S32			ShuiDamage;			// 水伤害
	S32			ShuiDamage_Pc;		// 水伤害百分比
	S32			ShuiDamage_gPc;		// 水伤害全局百分比
	S32			ShuiDefence;		// 水防御
	S32			ShuiDefence_Pc;		// 水防御百分比
	S32			ShuiDefence_gPc;	// 水防御全局百分比
	S32			PneumaDamage;		// 元力伤害
	S32			PneumaDamage_Pc;	// 元力伤害百分比
	S32			PneumaDamage_gPc;	// 元力伤害全局百分比
	S32			PneumaDefence;		// 元力防御
	S32			PneumaDefence_Pc;	// 元力防御百分比
	S32			PneumaDefence_gPc;	// 元力防御全局百分比
	S32			Heal;				// 治疗
	S32			Heal_Pc;			// 治疗百分比
	S32			Heal_gPc;			// 治疗全局百分比

	S32			PhyScale_gPc;		// 物理伤害调整全局百分比(被动)
	S32			SplScale_gPc;		// 五行伤害调整全局百分比
	S32			HealScale_gPc;		// 治疗调整全局百分比
	S32			HateScale_gPc;		// 仇恨调整全局百分比

	S32			DeriveHP_gPc;		// 汲取生命全局百分比
	S32			DeriveMP_gPc;		// 汲取真气全局百分比
	S32			DerivePP_gPc;		// 汲取元气全局百分比

	S32			Mu;					// 木
	S32			Huo;				// 火
	S32			Tu;					// 土
	S32			Jin;				// 金
	S32			Shui;				// 水

	Stats();
	void Clear();

	Stats& operator+=(const Stats&);
	Stats& operator-=(const Stats&);
	Stats& operator*=(const F32);

	// 最终计算
	void Calc(const Stats& _add, const CalcFactor& factor);
	// 数值增强（用于状态、技能的增强）
	void Plus(const Stats& _orig, const Stats& _plus);
	// 增强后的值，+=和-=实际上是加上增强后的值
	void PlusDone();
	// 按比例叠加源的属性，有些值无需（不能）叠加
	void PlusSrc(const Stats& _orig, S32 rate);
	// 属性叠加，与+=相比增加了叠加数
	Stats& Add(const Stats& _add, S32 _count = 1);
};

extern Stats g_Stats;

