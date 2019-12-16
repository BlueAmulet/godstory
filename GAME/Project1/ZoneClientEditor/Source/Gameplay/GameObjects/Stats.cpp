//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/Stats.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "math/mMathFn.h"

// ========================================================================================================================================
//  CalcFactor
// ========================================================================================================================================
CalcFactor g_CalcFactor[MaxFamilies] = 
{
	{13.0f, 22.8f, 1.50f, 1.26f, 2.83f, 9.0f, 3.0f},		// 无
	{13.0f, 22.8f, 1.57f, 1.32f, 2.96f, 9.0f, 3.0f},		// 圣
	{13.0f, 22.8f, 1.50f, 1.26f, 2.83f, 9.0f, 3.0f},		// 仙
	{13.0f, 22.8f, 1.57f, 1.32f, 2.96f, 9.0f, 3.0f},		// 佛
	{13.0f, 22.8f, 1.57f, 1.32f, 2.96f, 9.0f, 3.0f},		// 精
	{13.0f, 22.8f, 1.50f, 1.26f, 2.83f, 9.0f, 3.0f},		// 鬼
	{13.0f, 22.8f, 1.50f, 1.26f, 2.83f, 9.0f, 3.0f},		// 怪
	{13.0f, 22.8f, 1.57f, 1.32f, 2.96f, 9.0f, 3.0f},		// 妖
	{13.0f, 22.8f, 1.50f, 1.26f, 2.83f, 9.0f, 3.0f},		// 魔
};


// ========================================================================================================================================
//  Stats
// ========================================================================================================================================
Stats g_Stats;

Stats::Stats()
{
	Clear();
}

void Stats::Clear()
{
	dMemset(this, 0, sizeof(Stats));
}

// 做A+=B和A-=B时，注意A::HP和A::HP_Pc没有做运算。所以一般来说A的初始值都为0
Stats& Stats::operator+=(const Stats& _add)
{
	HP += mFloor(_add.HP + _add.HP * _add.HP_Pc / GAMEPLAY_PERCENT_SCALE);
	HP_gPc += _add.HP_gPc;
	MP += mFloor(_add.MP + _add.MP * _add.MP_Pc / GAMEPLAY_PERCENT_SCALE);
	MP_gPc += _add.MP_gPc;
	PP += mFloor(_add.PP + _add.PP * _add.PP_Pc / GAMEPLAY_PERCENT_SCALE);
	PP_gPc += _add.PP_gPc;

	MaxHP += mFloor(_add.MaxHP + _add.MaxHP * _add.MaxHP_Pc / GAMEPLAY_PERCENT_SCALE);
	MaxHP_gPc += _add.MaxHP_gPc;
	MaxMP += mFloor(_add.MaxMP + _add.MaxMP * _add.MaxMP_Pc / GAMEPLAY_PERCENT_SCALE);
	MaxMP_gPc += _add.MaxMP_gPc;
	MaxPP += mFloor(_add.MaxPP + _add.MaxPP * _add.MaxPP_Pc / GAMEPLAY_PERCENT_SCALE);
	MaxPP_gPc += _add.MaxPP_gPc;

	Stamina += mFloor(_add.Stamina + _add.Stamina * _add.Stamina_Pc /GAMEPLAY_PERCENT_SCALE);
	Stamina_gPc += _add.Stamina_gPc;
	Mana += mFloor(_add.Mana + _add.Mana * _add.Mana_Pc /GAMEPLAY_PERCENT_SCALE);
	Mana_gPc += _add.Mana_gPc;
	Strength += mFloor(_add.Strength + _add.Strength * _add.Strength_Pc /GAMEPLAY_PERCENT_SCALE);
	Strength_gPc += _add.Strength_gPc;
	Intellect += mFloor(_add.Intellect + _add.Intellect * _add.Intellect_Pc /GAMEPLAY_PERCENT_SCALE);
	Intellect_gPc += _add.Intellect_gPc;
	Agility += mFloor(_add.Agility + _add.Agility * _add.Agility_Pc /GAMEPLAY_PERCENT_SCALE);
	Agility_gPc += _add.Agility_gPc;
	Pneuma += mFloor(_add.Pneuma + _add.Pneuma * _add.Pneuma_Pc /GAMEPLAY_PERCENT_SCALE);
	Pneuma_gPc += _add.Pneuma_gPc;
	Insight += mFloor(_add.Insight + _add.Insight * _add.Insight_Pc /GAMEPLAY_PERCENT_SCALE);
	Insight_gPc += _add.Insight_gPc;
	Luck += mFloor(_add.Luck + _add.Luck * _add.Luck_Pc /GAMEPLAY_PERCENT_SCALE);
	Luck_gPc += _add.Luck_gPc;

	Velocity_gPc += _add.Velocity_gPc;
	MountedVelocity_gPc += _add.MountedVelocity_gPc;
	AttackSpeed += _add.AttackSpeed;
	AttackSpeed_gPc += _add.AttackSpeed_gPc;
	SpellSpeed_gPc += _add.SpellSpeed_gPc;

	CriticalRating += _add.CriticalRating;
	CriticalTimes += _add.CriticalTimes;
	Critical_gPc += _add.Critical_gPc;
	AntiCriticalRating += _add.AntiCriticalRating;
	AntiCritical_gPc += _add.AntiCritical_gPc;
	FatalRating += _add.FatalRating;
	Fatal_gPc += _add.Fatal_gPc;
	DodgeRating += _add.DodgeRating;
	Dodge_gPc += _add.Dodge_gPc;
	HitRating += _add.HitRating;
	Hit_gPc += _add.Hit_gPc;
	DamageHandle_gPc += _add.DamageHandle_gPc;

	PhyDamage += mFloor(_add.PhyDamage + _add.PhyDamage * _add.PhyDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	PhyDamage_gPc += _add.PhyDamage_gPc;
	PhyDefence += mFloor(_add.PhyDefence + _add.PhyDefence * _add.PhyDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	PhyDefence_gPc += _add.PhyDefence_gPc;
	MuDamage += mFloor(_add.MuDamage + _add.MuDamage * _add.MuDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	MuDamage_gPc += _add.MuDamage_gPc;
	MuDefence += mFloor(_add.MuDefence + _add.MuDefence * _add.MuDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	MuDefence_gPc += _add.MuDefence_gPc;
	HuoDamage += mFloor(_add.HuoDamage + _add.HuoDamage * _add.HuoDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	HuoDamage_gPc += _add.HuoDamage_gPc;
	HuoDefence += mFloor(_add.HuoDefence + _add.HuoDefence * _add.HuoDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	HuoDefence_gPc += _add.HuoDefence_gPc;
	TuDamage += mFloor(_add.TuDamage + _add.TuDamage * _add.TuDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	TuDamage_gPc += _add.TuDamage_gPc;
	TuDefence += mFloor(_add.TuDefence + _add.TuDefence * _add.TuDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	TuDefence_gPc += _add.TuDefence_gPc;
	JinDamage += mFloor(_add.JinDamage + _add.JinDamage * _add.JinDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	JinDamage_gPc += _add.JinDamage_gPc;
	JinDefence += mFloor(_add.JinDefence + _add.JinDefence * _add.JinDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	JinDefence_gPc += _add.JinDefence_gPc;
	ShuiDamage += mFloor(_add.ShuiDamage + _add.ShuiDamage * _add.ShuiDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	ShuiDamage_gPc += _add.ShuiDamage_gPc;
	ShuiDefence += mFloor(_add.ShuiDefence + _add.ShuiDefence * _add.ShuiDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	ShuiDefence_gPc += _add.ShuiDefence_gPc;
	PneumaDamage += mFloor(_add.PneumaDamage + _add.PneumaDamage * _add.PneumaDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	PneumaDamage_gPc += _add.PneumaDamage_gPc;
	PneumaDefence += mFloor(_add.PneumaDefence + _add.PneumaDefence * _add.PneumaDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	PneumaDefence_gPc += _add.PneumaDefence_gPc;
	Heal += mFloor(_add.Heal + _add.Heal * _add.Heal_Pc / GAMEPLAY_PERCENT_SCALE);
	Heal_gPc += _add.Heal_gPc;

	PhyScale_gPc += _add.PhyScale_gPc;
	SplScale_gPc += _add.SplScale_gPc;
	HealScale_gPc += _add.HealScale_gPc;
	HateScale_gPc += _add.HateScale_gPc;

	DeriveHP_gPc += _add.DeriveHP_gPc;
	DeriveMP_gPc += _add.DeriveMP_gPc;
	DerivePP_gPc += _add.DerivePP_gPc;

	Mu += _add.Mu;
	Huo += _add.Huo;
	Tu += _add.Tu;
	Jin += _add.Jin;
	Shui += _add.Shui;

	return *this;
}

Stats& Stats::operator-=(const Stats& _add)
{
	HP -= mFloor(_add.HP + _add.HP * _add.HP_Pc / GAMEPLAY_PERCENT_SCALE);
	HP_gPc -= _add.HP_gPc;
	MP -= mFloor(_add.MP + _add.MP * _add.MP_Pc / GAMEPLAY_PERCENT_SCALE);
	MP_gPc -= _add.MP_gPc;
	PP -= mFloor(_add.PP + _add.PP * _add.PP_Pc / GAMEPLAY_PERCENT_SCALE);
	PP_gPc -= _add.PP_gPc;

	MaxHP -= mFloor(_add.MaxHP + _add.MaxHP * _add.MaxHP_Pc / GAMEPLAY_PERCENT_SCALE);
	MaxHP_gPc -= _add.MaxHP_gPc;
	MaxMP -= mFloor(_add.MaxMP + _add.MaxMP * _add.MaxMP_Pc / GAMEPLAY_PERCENT_SCALE);
	MaxMP_gPc -= _add.MaxMP_gPc;
	MaxPP -= mFloor(_add.MaxPP + _add.MaxPP * _add.MaxPP_Pc / GAMEPLAY_PERCENT_SCALE);
	MaxPP_gPc -= _add.MaxPP_gPc;

	Stamina -= mFloor(_add.Stamina + _add.Stamina * _add.Stamina_Pc /GAMEPLAY_PERCENT_SCALE);
	Stamina_gPc -= _add.Stamina_gPc;
	Mana -= mFloor(_add.Mana + _add.Mana * _add.Mana_Pc /GAMEPLAY_PERCENT_SCALE);
	Mana_gPc -= _add.Mana_gPc;
	Strength -= mFloor(_add.Strength + _add.Strength * _add.Strength_Pc /GAMEPLAY_PERCENT_SCALE);
	Strength_gPc -= _add.Strength_gPc;
	Intellect -= mFloor(_add.Intellect + _add.Intellect * _add.Intellect_Pc /GAMEPLAY_PERCENT_SCALE);
	Intellect_gPc -= _add.Intellect_gPc;
	Agility -= mFloor(_add.Agility + _add.Agility * _add.Agility_Pc /GAMEPLAY_PERCENT_SCALE);
	Agility_gPc -= _add.Agility_gPc;
	Pneuma -= mFloor(_add.Pneuma + _add.Pneuma * _add.Pneuma_Pc /GAMEPLAY_PERCENT_SCALE);
	Pneuma_gPc -= _add.Pneuma_gPc;
	Insight -= mFloor(_add.Insight + _add.Insight * _add.Insight_Pc /GAMEPLAY_PERCENT_SCALE);
	Insight_gPc -= _add.Insight_gPc;
	Luck -= mFloor(_add.Luck + _add.Luck * _add.Luck_Pc /GAMEPLAY_PERCENT_SCALE);
	Luck_gPc -= _add.Luck_gPc;

	Velocity_gPc -= _add.Velocity_gPc;
	MountedVelocity_gPc -= _add.MountedVelocity_gPc;
	AttackSpeed -= _add.AttackSpeed;
	AttackSpeed_gPc -= _add.AttackSpeed_gPc;
	SpellSpeed_gPc -= _add.SpellSpeed_gPc;

	CriticalRating -= _add.CriticalRating;
	CriticalTimes -= _add.CriticalTimes;
	Critical_gPc -= _add.Critical_gPc;
	AntiCriticalRating -= _add.AntiCriticalRating;
	AntiCritical_gPc -= _add.AntiCritical_gPc;
	FatalRating -= _add.FatalRating;
	Fatal_gPc -= _add.Fatal_gPc;
	DodgeRating -= _add.DodgeRating;
	Dodge_gPc -= _add.Dodge_gPc;
	HitRating -= _add.HitRating;
	Hit_gPc -= _add.Hit_gPc;
	DamageHandle_gPc -= _add.DamageHandle_gPc;

	PhyDamage -= mFloor(_add.PhyDamage + _add.PhyDamage * _add.PhyDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	PhyDamage_gPc -= _add.PhyDamage_gPc;
	PhyDefence -= mFloor(_add.PhyDefence + _add.PhyDefence * _add.PhyDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	PhyDefence_gPc -= _add.PhyDefence_gPc;
	MuDamage -= mFloor(_add.MuDamage + _add.MuDamage * _add.MuDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	MuDamage_gPc -= _add.MuDamage_gPc;
	MuDefence -= mFloor(_add.MuDefence + _add.MuDefence * _add.MuDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	MuDefence_gPc -= _add.MuDefence_gPc;
	HuoDamage -= mFloor(_add.HuoDamage + _add.HuoDamage * _add.HuoDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	HuoDamage_gPc -= _add.HuoDamage_gPc;
	HuoDefence -= mFloor(_add.HuoDefence + _add.HuoDefence * _add.HuoDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	HuoDefence_gPc -= _add.HuoDefence_gPc;
	TuDamage -= mFloor(_add.TuDamage + _add.TuDamage * _add.TuDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	TuDamage_gPc -= _add.TuDamage_gPc;
	TuDefence -= mFloor(_add.TuDefence + _add.TuDefence * _add.TuDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	TuDefence_gPc -= _add.TuDefence_gPc;
	JinDamage -= mFloor(_add.JinDamage + _add.JinDamage * _add.JinDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	JinDamage_gPc -= _add.JinDamage_gPc;
	JinDefence -= mFloor(_add.JinDefence + _add.JinDefence * _add.JinDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	JinDefence_gPc -= _add.JinDefence_gPc;
	ShuiDamage -= mFloor(_add.ShuiDamage + _add.ShuiDamage * _add.ShuiDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	ShuiDamage_gPc -= _add.ShuiDamage_gPc;
	ShuiDefence -= mFloor(_add.ShuiDefence + _add.ShuiDefence * _add.ShuiDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	ShuiDefence_gPc -= _add.ShuiDefence_gPc;
	PneumaDamage -= mFloor(_add.PneumaDamage + _add.PneumaDamage * _add.PneumaDamage_Pc / GAMEPLAY_PERCENT_SCALE);
	PneumaDamage_gPc -= _add.PneumaDamage_gPc;
	PneumaDefence -= mFloor(_add.PneumaDefence + _add.PneumaDefence * _add.PneumaDefence_Pc / GAMEPLAY_PERCENT_SCALE);
	PneumaDefence_gPc -= _add.PneumaDefence_gPc;
	Heal -= mFloor(_add.Heal + _add.Heal * _add.Heal_Pc / GAMEPLAY_PERCENT_SCALE);
	Heal_gPc -= _add.Heal_gPc;

	PhyScale_gPc -= _add.PhyScale_gPc;
	SplScale_gPc -= _add.SplScale_gPc;
	HealScale_gPc -= _add.HealScale_gPc;
	HateScale_gPc -= _add.HateScale_gPc;

	DeriveHP_gPc -= _add.DeriveHP_gPc;
	DeriveMP_gPc -= _add.DeriveMP_gPc;
	DerivePP_gPc -= _add.DerivePP_gPc;

	Mu -= _add.Mu;
	Huo -= _add.Huo;
	Tu -= _add.Tu;
	Jin -= _add.Jin;
	Shui -= _add.Shui;

	return *this;
}

Stats& Stats::operator*=(const F32 scale)
{
	MaxHP *= scale;
	MaxMP *= scale;
	MaxPP *= scale;

	PhyDamage *= scale;
	PhyDefence *= scale;
	MuDamage *= scale;
	MuDefence *= scale;
	HuoDamage *= scale;
	HuoDefence *= scale;
	TuDamage *= scale;
	TuDefence *= scale;
	JinDamage *= scale;
	JinDefence *= scale;
	ShuiDamage *= scale;
	ShuiDefence *= scale;
	PneumaDamage *= scale;
	PneumaDefence *= scale;
	Heal *= scale;

	return *this;
}

void Stats::Calc(const Stats& _add, const CalcFactor& factor)
{
	Clear();

	Stamina = _add.Stamina + _add.Stamina * _add.Stamina_gPc /GAMEPLAY_PERCENT_SCALE;
	Mana = _add.Mana + _add.Mana * _add.Mana_gPc /GAMEPLAY_PERCENT_SCALE;
	Strength = _add.Strength + _add.Strength * _add.Strength_gPc /GAMEPLAY_PERCENT_SCALE;
	Intellect = _add.Intellect + _add.Intellect * _add.Intellect_gPc /GAMEPLAY_PERCENT_SCALE;
	Agility = _add.Agility + _add.Agility * _add.Agility_gPc /GAMEPLAY_PERCENT_SCALE;
	Pneuma = _add.Pneuma + _add.Pneuma * _add.Pneuma_gPc /GAMEPLAY_PERCENT_SCALE;
	Insight = _add.Insight + _add.Insight * _add.Insight_gPc /GAMEPLAY_PERCENT_SCALE;
	Luck = _add.Luck + _add.Luck * _add.Luck_gPc /GAMEPLAY_PERCENT_SCALE;

	MaxHP += mFloor(Stamina * factor.Stamina_HP);
	MaxMP += mFloor(Mana * factor.Mana_MP);
	PhyDamage += mFloor(Strength * factor.Strength_PhyDamage);
	MuDamage += mFloor(Intellect * factor.Intellect_SplDamage);
	HuoDamage += mFloor(Intellect * factor.Intellect_SplDamage);
	TuDamage += mFloor(Intellect * factor.Intellect_SplDamage);
	JinDamage += mFloor(Intellect * factor.Intellect_SplDamage);
	ShuiDamage += mFloor(Intellect * factor.Intellect_SplDamage);
	PneumaDamage += mFloor(Pneuma * factor.Pneuma_PnmDamage);

	HP = _add.HP + _add.HP * _add.HP_gPc / GAMEPLAY_PERCENT_SCALE;
	MP = _add.MP + _add.MP * _add.MP_gPc / GAMEPLAY_PERCENT_SCALE;
	PP = _add.PP + _add.PP * _add.PP_gPc / GAMEPLAY_PERCENT_SCALE;

	MaxHP = getMax(MaxHP + _add.MaxHP + _add.MaxHP * _add.MaxHP_gPc / GAMEPLAY_PERCENT_SCALE, 1.0f);
	MaxMP = getMax(MaxMP + _add.MaxMP + _add.MaxMP * _add.MaxMP_gPc / GAMEPLAY_PERCENT_SCALE, 1.0f);
	MaxPP = getMax(_add.MaxPP + _add.MaxPP * _add.MaxPP_gPc / GAMEPLAY_PERCENT_SCALE, 1.0f);

	Velocity_gPc = mClampF(_add.Velocity_gPc + GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT_SCALE, MAX_GAMEPLAY_PERCENT_SCALE);
	MountedVelocity_gPc = mClampF(_add.MountedVelocity_gPc + GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT_SCALE, MAX_GAMEPLAY_PERCENT_SCALE);
	AttackSpeed = mClampF(_add.AttackSpeed, MAX_ATTACK_SPEED, MIN_ATTACK_SPEED)/mClampF(1.0f + _add.AttackSpeed_gPc/GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT, MAX_GAMEPLAY_PERCENT);
	AttackSpeed_gPc = mClampF(_add.AttackSpeed_gPc + GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT_SCALE, MAX_GAMEPLAY_PERCENT_SCALE);
	SpellSpeed_gPc = mClampF(_add.SpellSpeed_gPc + GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT_SCALE, MAX_GAMEPLAY_PERCENT_SCALE);

	CriticalRating +=  mFloor(_add.CriticalRating + _add.Agility * factor.Agility_CriticalRating);
	CriticalRating = _add.CriticalRating + _add.Agility * factor.Agility_CriticalRating;
	CriticalTimes = _add.CriticalTimes;
	Critical_gPc = _add.Critical_gPc + getCriticalRate(_add.CriticalRating);
	AntiCriticalRating = _add.AntiCriticalRating;
	AntiCritical_gPc = _add.AntiCritical_gPc + getAntiCriticalRate(_add.AntiCriticalRating);
	FatalRating = _add.FatalRating;
	Fatal_gPc = _add.Fatal_gPc;
	DodgeRating = _add.DodgeRating + _add.Agility * factor.Agility_DodgeRating;
	Dodge_gPc = _add.Dodge_gPc;
	HitRating = _add.HitRating;
	Hit_gPc = _add.Hit_gPc;
	DamageHandle_gPc = _add.DamageHandle_gPc;

	PhyDamage = getMax(PhyDamage + _add.PhyDamage + _add.PhyDamage * _add.PhyDamage_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	PhyDefence = getMax(_add.PhyDefence + _add.PhyDefence * _add.PhyDefence_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	MuDamage = getMax(MuDamage + _add.MuDamage + _add.MuDamage * _add.MuDamage_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	MuDefence = getMax(_add.MuDefence + _add.MuDefence * _add.MuDefence_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	HuoDamage = getMax(HuoDamage + _add.HuoDamage + _add.HuoDamage * _add.HuoDamage_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	HuoDefence = getMax(_add.HuoDefence + _add.HuoDefence * _add.HuoDefence_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	TuDamage = getMax(TuDamage + _add.TuDamage + _add.TuDamage * _add.TuDamage_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	TuDefence = getMax(_add.TuDefence + _add.TuDefence * _add.TuDefence_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	JinDamage = getMax(JinDamage + _add.JinDamage + _add.JinDamage * _add.JinDamage_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	JinDefence = getMax(_add.JinDefence + _add.JinDefence * _add.JinDefence_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	ShuiDamage = getMax(ShuiDamage + _add.ShuiDamage + _add.ShuiDamage * _add.ShuiDamage_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	ShuiDefence = getMax(_add.ShuiDefence + _add.ShuiDefence * _add.ShuiDefence_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	PneumaDamage = getMax(PneumaDamage + _add.PneumaDamage + _add.PneumaDamage * _add.PneumaDamage_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	PneumaDefence = getMax(_add.PneumaDefence + _add.PneumaDefence * _add.PneumaDefence_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);
	Heal = getMax(_add.Heal + _add.Heal * _add.Heal_gPc / GAMEPLAY_PERCENT_SCALE, 0.0f);

	PhyScale_gPc = _add.PhyScale_gPc;
	SplScale_gPc = _add.SplScale_gPc;
	HealScale_gPc = _add.HealScale_gPc;
	HateScale_gPc = _add.HateScale_gPc;

	DeriveHP_gPc = _add.DeriveHP_gPc;
	DeriveMP_gPc = _add.DeriveMP_gPc;
	DerivePP_gPc = _add.DerivePP_gPc;

	Mu = _add.Mu;
	Huo = _add.Huo;
	Tu = _add.Tu;
	Jin = _add.Jin;
	Shui = _add.Shui;
}

void Stats::Plus(const Stats& _orig, const Stats& _plus)
{
	HP = _orig.HP + _plus.HP;
	HP_Pc = _orig.HP_Pc + _plus.HP_Pc;
	HP_gPc = _orig.HP_gPc + _plus.HP_gPc;
	MP = _orig.MP + _plus.MP;
	MP_Pc = _orig.MP_Pc + _plus.MP_Pc;
	MP_gPc = _orig.MP_gPc + _plus.MP_gPc;
	PP = _orig.PP + _plus.PP;
	PP_Pc = _orig.PP_Pc + _plus.PP_Pc;
	PP_gPc = _orig.PP_gPc + _plus.PP_gPc;

	MaxHP = _orig.MaxHP + _plus.MaxHP;
	MaxHP_Pc = _orig.MaxHP_Pc + _plus.MaxHP_Pc;
	MaxHP_gPc = _orig.MaxHP_gPc + _plus.MaxHP_gPc;
	MaxMP = _orig.MaxMP + _plus.MaxMP;
	MaxMP_Pc = _orig.MaxMP_Pc + _plus.MaxMP_Pc;
	MaxMP_gPc = _orig.MaxMP_gPc + _plus.MaxMP_gPc;
	MaxPP = _orig.MaxPP + _plus.MaxPP;
	MaxPP_Pc = _orig.MaxPP_Pc + _plus.MaxPP_Pc;
	MaxPP_gPc = _orig.MaxPP_gPc + _plus.MaxPP_gPc;

	Stamina = _orig.Stamina + _plus.Stamina;
	Stamina_Pc = _orig.Stamina_Pc + _plus.Stamina_Pc;
	Stamina_gPc = _orig.Stamina_gPc + _plus.Stamina_gPc;
	Mana = _orig.Mana + _plus.Mana;
	Mana_Pc = _orig.Mana_Pc + _plus.Mana_Pc;
	Mana_gPc = _orig.Mana_gPc + _plus.Mana_gPc;
	Strength = _orig.Strength + _plus.Strength;
	Strength_Pc = _orig.Strength_Pc + _plus.Strength_Pc;
	Strength_gPc = _orig.Strength_gPc + _plus.Strength_gPc;
	Intellect = _orig.Intellect + _plus.Intellect;
	Intellect_Pc = _orig.Intellect_Pc + _plus.Intellect_Pc;
	Intellect_gPc = _orig.Intellect_gPc + _plus.Intellect_gPc;
	Agility = _orig.Agility + _plus.Agility;
	Agility_Pc = _orig.Agility_Pc + _plus.Agility_Pc;
	Agility_gPc = _orig.Agility_gPc + _plus.Agility_gPc;
	Pneuma = _orig.Pneuma + _plus.Pneuma;
	Pneuma_Pc = _orig.Pneuma_Pc + _plus.Pneuma_Pc;
	Pneuma_gPc = _orig.Pneuma_gPc + _plus.Pneuma_gPc;
	Insight = _orig.Insight + _plus.Insight;
	Insight_Pc = _orig.Insight_Pc + _plus.Insight_Pc;
	Insight_gPc = _orig.Insight_gPc + _plus.Insight_gPc;
	Luck = _orig.Luck + _plus.Luck;
	Luck_Pc = _orig.Luck_Pc + _plus.Luck_Pc;
	Luck_gPc = _orig.Luck_gPc + _plus.Luck_gPc;

	Velocity_gPc = _orig.Velocity_gPc + _plus.Velocity_gPc;
	MountedVelocity_gPc = _orig.MountedVelocity_gPc + _plus.MountedVelocity_gPc;
	AttackSpeed = _orig.AttackSpeed + _plus.AttackSpeed;
	AttackSpeed_gPc = _orig.AttackSpeed_gPc + _plus.AttackSpeed_gPc;
	SpellSpeed_gPc = _orig.SpellSpeed_gPc + _plus.SpellSpeed_gPc;

	CriticalRating = _orig.CriticalRating + _plus.CriticalRating;
	CriticalTimes = _orig.CriticalTimes + _plus.CriticalTimes;
	Critical_gPc = _orig.Critical_gPc + _plus.Critical_gPc;
	AntiCriticalRating = _orig.AntiCriticalRating + _plus.AntiCriticalRating;
	AntiCritical_gPc = _orig.AntiCritical_gPc + _plus.AntiCritical_gPc;
	FatalRating = _orig.FatalRating + _plus.FatalRating;
	Fatal_gPc = _orig.Fatal_gPc + _plus.Fatal_gPc;
	DodgeRating = _orig.DodgeRating + _plus.DodgeRating;
	Dodge_gPc = _orig.Dodge_gPc + _plus.Dodge_gPc;
	HitRating = _orig.HitRating + _plus.HitRating;
	Hit_gPc = _orig.Hit_gPc + _plus.Hit_gPc;
	DamageHandle_gPc = _orig.DamageHandle_gPc + _plus.DamageHandle_gPc;

	PhyDamage = _orig.PhyDamage + _plus.PhyDamage;
	PhyDamage_Pc = _orig.PhyDamage_Pc + _plus.PhyDamage_Pc;
	PhyDamage_gPc = _orig.PhyDamage_gPc + _plus.PhyDamage_gPc;
	PhyDefence = _orig.PhyDefence + _plus.PhyDefence;
	PhyDefence_Pc = _orig.PhyDefence_Pc + _plus.PhyDefence_Pc;
	PhyDefence_gPc = _orig.PhyDefence_gPc + _plus.PhyDefence_gPc;
	MuDamage = _orig.MuDamage + _plus.MuDamage;
	MuDamage_Pc = _orig.MuDamage_Pc + _plus.MuDamage_Pc;
	MuDamage_gPc = _orig.MuDamage_gPc + _plus.MuDamage_gPc;
	MuDefence = _orig.MuDefence + _plus.MuDefence;
	MuDefence_Pc = _orig.MuDefence_Pc + _plus.MuDefence_Pc;
	MuDefence_gPc = _orig.MuDefence_gPc + _plus.MuDefence_gPc;
	HuoDamage = _orig.HuoDamage + _plus.HuoDamage;
	HuoDamage_Pc = _orig.HuoDamage_Pc + _plus.HuoDamage_Pc;
	HuoDamage_gPc = _orig.HuoDamage_gPc + _plus.HuoDamage_gPc;
	HuoDefence = _orig.HuoDefence + _plus.HuoDefence;
	HuoDefence_Pc = _orig.HuoDefence_Pc + _plus.HuoDefence_Pc;
	HuoDefence_gPc = _orig.HuoDefence_gPc + _plus.HuoDefence_gPc;
	TuDamage = _orig.TuDamage + _plus.TuDamage;
	TuDamage_Pc = _orig.TuDamage_Pc + _plus.TuDamage_Pc;
	TuDamage_gPc = _orig.TuDamage_gPc + _plus.TuDamage_gPc;
	TuDefence = _orig.TuDefence + _plus.TuDefence;
	TuDefence_Pc = _orig.TuDefence_Pc + _plus.TuDefence_Pc;
	TuDefence_gPc = _orig.TuDefence_gPc + _plus.TuDefence_gPc;
	JinDamage = _orig.JinDamage + _plus.JinDamage;
	JinDamage_Pc = _orig.JinDamage_Pc + _plus.JinDamage_Pc;
	JinDamage_gPc = _orig.JinDamage_gPc + _plus.JinDamage_gPc;
	JinDefence = _orig.JinDefence + _plus.JinDefence;
	JinDefence_Pc = _orig.JinDefence_Pc + _plus.JinDefence_Pc;
	JinDefence_gPc = _orig.JinDefence_gPc + _plus.JinDefence_gPc;
	ShuiDamage = _orig.ShuiDamage + _plus.ShuiDamage;
	ShuiDamage_Pc = _orig.ShuiDamage_Pc + _plus.ShuiDamage_Pc;
	ShuiDamage_gPc = _orig.ShuiDamage_gPc + _plus.ShuiDamage_gPc;
	ShuiDefence = _orig.ShuiDefence + _plus.ShuiDefence;
	ShuiDefence_Pc = _orig.ShuiDefence_Pc + _plus.ShuiDefence_Pc;
	ShuiDefence_gPc = _orig.ShuiDefence_gPc + _plus.ShuiDefence_gPc;
	PneumaDamage = _orig.PneumaDamage + _plus.PneumaDamage;
	PneumaDamage_Pc = _orig.PneumaDamage_Pc + _plus.PneumaDamage_Pc;
	PneumaDamage_gPc = _orig.PneumaDamage_gPc + _plus.PneumaDamage_gPc;
	PneumaDefence = _orig.PneumaDefence + _plus.PneumaDefence;
	PneumaDefence_Pc = _orig.PneumaDefence_Pc + _plus.PneumaDefence_Pc;
	PneumaDefence_gPc = _orig.PneumaDefence_gPc + _plus.PneumaDefence_gPc;
	Heal = _orig.Heal + _plus.Heal;
	Heal_Pc = _orig.Heal_Pc + _plus.Heal_Pc;
	Heal_gPc = _orig.Heal_gPc + _plus.Heal_gPc;

	PhyScale_gPc = _orig.PhyScale_gPc + _plus.PhyScale_gPc;
	SplScale_gPc = _orig.SplScale_gPc + _plus.SplScale_gPc;
	HealScale_gPc = _orig.HealScale_gPc + _plus.HealScale_gPc;
	HateScale_gPc = _orig.HateScale_gPc + _plus.HateScale_gPc;

	DeriveHP_gPc = _orig.DeriveHP_gPc + _plus.DeriveHP_gPc;
	DeriveMP_gPc = _orig.DeriveMP_gPc + _plus.DeriveMP_gPc;
	DerivePP_gPc = _orig.DerivePP_gPc + _plus.DerivePP_gPc;

	Mu = _orig.Mu + _plus.Mu;
	Huo = _orig.Huo + _plus.Huo;
	Tu = _orig.Tu + _plus.Tu;
	Jin = _orig.Jin + _plus.Jin;
	Shui = _orig.Shui + _plus.Shui;
}

void Stats::PlusDone()
{
	HP = mFloor(HP + HP * (HP_Pc / GAMEPLAY_PERCENT_SCALE * HP_gPc / GAMEPLAY_PERCENT_SCALE));
	HP_Pc = 0;
	MP = mFloor(MP + MP * (MP_Pc / GAMEPLAY_PERCENT_SCALE * MP_gPc / GAMEPLAY_PERCENT_SCALE));
	MP_Pc = 0;
	PP = mFloor(PP + PP * (PP_Pc / GAMEPLAY_PERCENT_SCALE * PP_gPc / GAMEPLAY_PERCENT_SCALE));
	PP_Pc = 0;

	MaxHP = mFloor(MaxHP + MaxHP * (MaxHP_Pc / GAMEPLAY_PERCENT_SCALE * MaxHP_gPc / GAMEPLAY_PERCENT_SCALE));
	MaxHP_Pc = 0;
	MaxMP = mFloor(MaxMP + MaxMP * (MaxMP_Pc / GAMEPLAY_PERCENT_SCALE * MaxMP_gPc / GAMEPLAY_PERCENT_SCALE));
	MaxMP_Pc = 0;
	MaxPP = mFloor(MaxPP + MaxPP * (MaxPP_Pc / GAMEPLAY_PERCENT_SCALE * MaxPP_gPc / GAMEPLAY_PERCENT_SCALE));
	MaxPP_Pc = 0;

	Stamina = mFloor(Stamina + Stamina * (Stamina_Pc /GAMEPLAY_PERCENT_SCALE * Stamina_gPc /GAMEPLAY_PERCENT_SCALE));
	Stamina_Pc = 0;
	Mana = mFloor(Mana + Mana * (Mana_Pc /GAMEPLAY_PERCENT_SCALE * Mana_gPc /GAMEPLAY_PERCENT_SCALE));
	Mana_Pc = 0;
	Strength = mFloor(Strength + Strength * (Strength_Pc /GAMEPLAY_PERCENT_SCALE * Strength_gPc /GAMEPLAY_PERCENT_SCALE));
	Strength_Pc = 0;
	Intellect = mFloor(Intellect + Intellect * (Intellect_Pc /GAMEPLAY_PERCENT_SCALE * Intellect_gPc /GAMEPLAY_PERCENT_SCALE));
	Intellect_Pc = 0;
	Agility = mFloor(Agility + Agility * (Agility_Pc /GAMEPLAY_PERCENT_SCALE * Agility_gPc /GAMEPLAY_PERCENT_SCALE));
	Agility_Pc = 0;
	Pneuma = mFloor(Pneuma + Pneuma * (Pneuma_Pc /GAMEPLAY_PERCENT_SCALE * Pneuma_gPc /GAMEPLAY_PERCENT_SCALE));
	Pneuma_Pc = 0;
	Insight = mFloor(Insight + Insight * (Insight_Pc /GAMEPLAY_PERCENT_SCALE * Insight_gPc /GAMEPLAY_PERCENT_SCALE));
	Insight_Pc = 0;
	Luck = mFloor(Luck + Luck * (Luck_Pc /GAMEPLAY_PERCENT_SCALE * Luck_gPc /GAMEPLAY_PERCENT_SCALE));
	Luck_Pc = 0;

	PhyDamage = mFloor(PhyDamage + PhyDamage * (PhyDamage_Pc / GAMEPLAY_PERCENT_SCALE * PhyDamage_gPc / GAMEPLAY_PERCENT_SCALE));
	PhyDamage_Pc = 0;
	PhyDefence = mFloor(PhyDefence + PhyDefence * (PhyDefence_Pc / GAMEPLAY_PERCENT_SCALE * PhyDefence_gPc / GAMEPLAY_PERCENT_SCALE));
	PhyDefence_Pc = 0;
	MuDamage = mFloor(MuDamage + MuDamage * (MuDamage_Pc / GAMEPLAY_PERCENT_SCALE * MuDamage_gPc / GAMEPLAY_PERCENT_SCALE));
	MuDamage_Pc = 0;
	MuDefence = mFloor(MuDefence + MuDefence * (MuDefence_Pc / GAMEPLAY_PERCENT_SCALE * MuDefence_gPc / GAMEPLAY_PERCENT_SCALE));
	MuDefence_Pc = 0;
	HuoDamage = mFloor(HuoDamage + HuoDamage * (HuoDamage_Pc / GAMEPLAY_PERCENT_SCALE * HuoDamage_gPc / GAMEPLAY_PERCENT_SCALE));
	HuoDamage_Pc = 0;
	HuoDefence = mFloor(HuoDefence + HuoDefence * (HuoDefence_Pc / GAMEPLAY_PERCENT_SCALE * HuoDefence_gPc / GAMEPLAY_PERCENT_SCALE));
	HuoDefence_Pc = 0;
	TuDamage = mFloor(TuDamage + TuDamage * (TuDamage_Pc / GAMEPLAY_PERCENT_SCALE * TuDamage_gPc / GAMEPLAY_PERCENT_SCALE));
	TuDamage_Pc = 0;
	TuDefence = mFloor(TuDefence + TuDefence * (TuDefence_Pc / GAMEPLAY_PERCENT_SCALE * TuDefence_gPc / GAMEPLAY_PERCENT_SCALE));
	TuDefence_Pc = 0;
	JinDamage = mFloor(JinDamage + JinDamage * (JinDamage_Pc / GAMEPLAY_PERCENT_SCALE * JinDamage_gPc / GAMEPLAY_PERCENT_SCALE));
	JinDamage_Pc = 0;
	JinDefence = mFloor(JinDefence + JinDefence * (JinDefence_Pc / GAMEPLAY_PERCENT_SCALE * JinDefence_gPc / GAMEPLAY_PERCENT_SCALE));
	JinDefence_Pc = 0;
	ShuiDamage = mFloor(ShuiDamage + ShuiDamage * (ShuiDamage_Pc / GAMEPLAY_PERCENT_SCALE * ShuiDamage_gPc / GAMEPLAY_PERCENT_SCALE));
	ShuiDamage_Pc = 0;
	ShuiDefence = mFloor(ShuiDefence + ShuiDefence * (ShuiDefence_Pc / GAMEPLAY_PERCENT_SCALE * ShuiDefence_gPc / GAMEPLAY_PERCENT_SCALE));
	ShuiDefence_Pc = 0;
	PneumaDamage = mFloor(PneumaDamage + PneumaDamage * (PneumaDamage_Pc / GAMEPLAY_PERCENT_SCALE * PneumaDamage_gPc / GAMEPLAY_PERCENT_SCALE));
	PneumaDamage_Pc = 0;
	PneumaDefence = mFloor(PneumaDefence + PneumaDefence * (PneumaDefence_Pc / GAMEPLAY_PERCENT_SCALE * PneumaDefence_gPc / GAMEPLAY_PERCENT_SCALE));
	PneumaDefence_Pc = 0;
	Heal = mFloor(Heal + Heal * (Heal_Pc / GAMEPLAY_PERCENT_SCALE * Heal_gPc / GAMEPLAY_PERCENT_SCALE));
	Heal_Pc = 0;
}

void Stats::PlusSrc(const Stats& _orig, S32 rate)
{
	// 加成比例
	F32 _rate = mClampF(rate/GAMEPLAY_PERCENT_SCALE, 0.0f, 3.0f);

	// 只是伤害属性按比例加成（事实上，某些属性必定受到全额加成）
	PhyDamage += _orig.PhyDamage * _rate * (getMax(PhyDamage_gPc + GAMEPLAY_PERCENT_SCALE, 0.0f) / GAMEPLAY_PERCENT_SCALE);
	MuDamage += _orig.MuDamage * _rate * (getMax(MuDamage_gPc + GAMEPLAY_PERCENT_SCALE, 0.0f) / GAMEPLAY_PERCENT_SCALE);
	HuoDamage += _orig.HuoDamage * _rate * (getMax(HuoDamage_gPc + GAMEPLAY_PERCENT_SCALE, 0.0f) / GAMEPLAY_PERCENT_SCALE);
	TuDamage += _orig.TuDamage * _rate * (getMax(TuDamage_gPc + GAMEPLAY_PERCENT_SCALE, 0.0f) / GAMEPLAY_PERCENT_SCALE);
	JinDamage += _orig.JinDamage * _rate * (getMax(JinDamage_gPc + GAMEPLAY_PERCENT_SCALE, 0.0f) / GAMEPLAY_PERCENT_SCALE);
	ShuiDamage += _orig.ShuiDamage * _rate * (getMax(ShuiDamage_gPc + GAMEPLAY_PERCENT_SCALE, 0.0f) / GAMEPLAY_PERCENT_SCALE);
	PneumaDamage += _orig.PneumaDamage * _rate * (getMax(PneumaDamage_gPc + GAMEPLAY_PERCENT_SCALE, 0.0f) / GAMEPLAY_PERCENT_SCALE);
	Heal += _orig.Heal * _rate * (getMax(Heal_gPc + GAMEPLAY_PERCENT_SCALE, 0.0f) / GAMEPLAY_PERCENT_SCALE);
}

Stats& Stats::Add(const Stats& _add, S32 _count /* = 1 */)
{
	HP += mFloor(_add.HP + _add.HP * _add.HP_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	HP_gPc += _add.HP_gPc * _count;
	MP += mFloor(_add.MP + _add.MP * _add.MP_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	MP_gPc += _add.MP_gPc * _count;
	PP += mFloor(_add.PP + _add.PP * _add.PP_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	PP_gPc += _add.PP_gPc * _count;

	MaxHP += mFloor(_add.MaxHP + _add.MaxHP * _add.MaxHP_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	MaxHP_gPc += _add.MaxHP_gPc * _count;
	MaxMP += mFloor(_add.MaxMP + _add.MaxMP * _add.MaxMP_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	MaxMP_gPc += _add.MaxMP_gPc * _count;
	MaxPP += mFloor(_add.MaxPP + _add.MaxPP * _add.MaxPP_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	MaxPP_gPc += _add.MaxPP_gPc * _count;

	Stamina += mFloor(_add.Stamina + _add.Stamina * _add.Stamina_Pc /GAMEPLAY_PERCENT_SCALE) * _count;
	Stamina_gPc += _add.Stamina_gPc * _count;
	Mana += mFloor(_add.Mana + _add.Mana * _add.Mana_Pc /GAMEPLAY_PERCENT_SCALE) * _count;
	Mana_gPc += _add.Mana_gPc * _count;
	Strength += mFloor(_add.Strength + _add.Strength * _add.Strength_Pc /GAMEPLAY_PERCENT_SCALE) * _count;
	Strength_gPc += _add.Strength_gPc * _count;
	Intellect += mFloor(_add.Intellect + _add.Intellect * _add.Intellect_Pc /GAMEPLAY_PERCENT_SCALE) * _count;
	Intellect_gPc += _add.Intellect_gPc * _count;
	Agility += mFloor(_add.Agility + _add.Agility * _add.Agility_Pc /GAMEPLAY_PERCENT_SCALE) * _count;
	Agility_gPc += _add.Agility_gPc * _count;
	Pneuma += mFloor(_add.Pneuma + _add.Pneuma * _add.Pneuma_Pc /GAMEPLAY_PERCENT_SCALE) * _count;
	Pneuma_gPc += _add.Pneuma_gPc * _count;
	Insight += mFloor(_add.Insight + _add.Insight * _add.Insight_Pc /GAMEPLAY_PERCENT_SCALE) * _count;
	Insight_gPc += _add.Insight_gPc * _count;
	Luck += mFloor(_add.Luck + _add.Luck * _add.Luck_Pc /GAMEPLAY_PERCENT_SCALE) * _count;
	Luck_gPc += _add.Luck_gPc * _count;

	Velocity_gPc += _add.Velocity_gPc * _count;
	MountedVelocity_gPc += _add.MountedVelocity_gPc * _count;
	AttackSpeed += _add.AttackSpeed * _count;
	AttackSpeed_gPc += _add.AttackSpeed_gPc * _count;
	SpellSpeed_gPc += _add.SpellSpeed_gPc * _count;

	CriticalRating += _add.CriticalRating * _count;
	CriticalTimes += _add.CriticalTimes * _count;
	Critical_gPc += _add.Critical_gPc * _count;
	AntiCriticalRating += _add.CriticalRating * _count;
	AntiCritical_gPc += _add.Critical_gPc * _count;
	FatalRating += _add.FatalRating * _count;
	Fatal_gPc += _add.Fatal_gPc * _count;
	DodgeRating += _add.DodgeRating * _count;
	Dodge_gPc += _add.Dodge_gPc * _count;
	HitRating += _add.HitRating * _count;
	Hit_gPc += _add.Hit_gPc * _count;
	DamageHandle_gPc += _add.DamageHandle_gPc * _count;

	PhyDamage += mFloor(_add.PhyDamage + _add.PhyDamage * _add.PhyDamage_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	PhyDamage_gPc += _add.PhyDamage_gPc * _count;
	PhyDefence += mFloor(_add.PhyDefence + _add.PhyDefence * _add.PhyDefence_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	PhyDefence_gPc += _add.PhyDefence_gPc * _count;
	MuDamage += mFloor(_add.MuDamage + _add.MuDamage * _add.MuDamage_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	MuDamage_gPc += _add.MuDamage_gPc * _count;
	MuDefence += mFloor(_add.MuDefence + _add.MuDefence * _add.MuDefence_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	MuDefence_gPc += _add.MuDefence_gPc * _count;
	HuoDamage += mFloor(_add.HuoDamage + _add.HuoDamage * _add.HuoDamage_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	HuoDamage_gPc += _add.HuoDamage_gPc * _count;
	HuoDefence += mFloor(_add.HuoDefence + _add.HuoDefence * _add.HuoDefence_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	HuoDefence_gPc += _add.HuoDefence_gPc * _count;
	TuDamage += mFloor(_add.TuDamage + _add.TuDamage * _add.TuDamage_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	TuDamage_gPc += _add.TuDamage_gPc * _count;
	TuDefence += mFloor(_add.TuDefence + _add.TuDefence * _add.TuDefence_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	TuDefence_gPc += _add.TuDefence_gPc * _count;
	JinDamage += mFloor(_add.JinDamage + _add.JinDamage * _add.JinDamage_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	JinDamage_gPc += _add.JinDamage_gPc * _count;
	JinDefence += mFloor(_add.JinDefence + _add.JinDefence * _add.JinDefence_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	JinDefence_gPc += _add.JinDefence_gPc * _count;
	ShuiDamage += mFloor(_add.ShuiDamage + _add.ShuiDamage * _add.ShuiDamage_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	ShuiDamage_gPc += _add.ShuiDamage_gPc * _count;
	ShuiDefence += mFloor(_add.ShuiDefence + _add.ShuiDefence * _add.ShuiDefence_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	ShuiDefence_gPc += _add.ShuiDefence_gPc * _count;
	PneumaDamage += mFloor(_add.PneumaDamage + _add.PneumaDamage * _add.PneumaDamage_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	PneumaDamage_gPc += _add.PneumaDamage_gPc * _count;
	PneumaDefence += mFloor(_add.PneumaDefence + _add.PneumaDefence * _add.PneumaDefence_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	PneumaDefence_gPc += _add.PneumaDefence_gPc * _count;
	Heal += mFloor(_add.Heal + _add.Heal * _add.Heal_Pc / GAMEPLAY_PERCENT_SCALE) * _count;
	Heal_gPc += _add.Heal_gPc * _count;

	PhyScale_gPc += _add.PhyScale_gPc * _count;
	SplScale_gPc += _add.SplScale_gPc * _count;
	HealScale_gPc += _add.HealScale_gPc * _count;
	HateScale_gPc += _add.HateScale_gPc * _count;

	DeriveHP_gPc += _add.DeriveHP_gPc * _count;
	DeriveMP_gPc += _add.DeriveMP_gPc * _count;
	DerivePP_gPc += _add.DerivePP_gPc * _count;

	Mu += _add.Mu * _count;
	Huo += _add.Huo * _count;
	Tu += _add.Tu * _count;
	Jin += _add.Jin * _count;
	Shui += _add.Shui * _count;

	return *this;
}
