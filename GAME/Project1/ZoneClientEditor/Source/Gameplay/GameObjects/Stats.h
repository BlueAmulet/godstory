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
	F32			Stamina_HP;					// ����->����
	F32			Mana_MP;					// ����->����
	F32			Strength_PhyDamage;			// ����->�﹥
	F32			Intellect_SplDamage;		// ����->����
	F32			Pneuma_PnmDamage;			// Ԫ��->Ԫ��	
	F32			Agility_DodgeRating;		// ����->����
	F32			Agility_CriticalRating;		// ����->����
};

extern CalcFactor g_CalcFactor[];

// ========================================================================================================================================
//  Stats
// ========================================================================================================================================

// ͨ������
struct Stats
{
	S32			HP;					// ����
	S32			HP_Pc;				// �����ٷֱ�
	S32			HP_gPc;				// ����ȫ�ְٷֱ�
	S32			MP;					// ����
	S32			MP_Pc;				// �����ٷֱ�
	S32			MP_gPc;				// ����ȫ�ְٷֱ�
	S32			PP;					// Ԫ��
	S32			PP_Pc;				// Ԫ���ٷֱ�
	S32			PP_gPc;				// Ԫ��ȫ�ְٷֱ�

	S32			MaxHP;				// �������
	S32			MaxHP_Pc;			// ��������ٷֱ�
	S32			MaxHP_gPc;			// �������ȫ�ְٷֱ�
	S32			MaxMP;				// �������
	S32			MaxMP_Pc;			// ��������ٷֱ�
	S32			MaxMP_gPc;			// �������ȫ�ְٷֱ�
	S32			MaxPP;				// ���Ԫ��
	S32			MaxPP_Pc;			// ���Ԫ���ٷֱ�
	S32			MaxPP_gPc;			// ���Ԫ��ȫ�ְٷֱ�

	S32			Stamina;			// ����
	S32			Stamina_Pc;			// ���ʰٷֱ�
	S32			Stamina_gPc;		// ����ȫ�ְٷֱ�
	S32			Mana;				// ����
	S32			Mana_Pc;			// �����ٷֱ�
	S32			Mana_gPc;			// ����ȫ�ְٷֱ�
	S32			Strength;			// ����
	S32			Strength_Pc;		// �����ٷֱ�
	S32			Strength_gPc;		// ����ȫ�ְٷֱ�
	S32			Intellect;			// ����
	S32			Intellect_Pc;		// �����ٷֱ�
	S32			Intellect_gPc;		// ����ȫ�ְٷֱ�
	S32			Agility;			// ��
	S32			Agility_Pc;			// ���ٷֱ�
	S32			Agility_gPc;		// ��ȫ�ְٷֱ�
	S32			Pneuma;				// Ԫ��
	S32			Pneuma_Pc;			// Ԫ���ٷֱ�
	S32			Pneuma_gPc;			// Ԫ��ȫ�ְٷֱ�
	S32			Insight;			// ����
	S32			Insight_Pc;			// ���԰ٷֱ�
	S32			Insight_gPc;		// ����ȫ�ְٷֱ�
	S32			Luck;				// ��Ե
	S32			Luck_Pc;			// ��Ե�ٷֱ�
	S32			Luck_gPc;			// ��Եȫ�ְٷֱ�

	S32			Velocity_gPc;		// �ƶ��ٶ�ȫ�ְٷֱ�
	S32			MountedVelocity_gPc;// ����ٶ�ȫ�ְٷֱ�
	S32			AttackSpeed;		// �����ٶ�ʱ���������룩
	S32			AttackSpeed_gPc;	// �����ٶ�ȫ�ְٷֱ�
	S32			SpellSpeed_gPc;		// ʩ���ٶ�ȫ�ְٷֱ�

	S32			CriticalRating;		// �ػ��ȼ� Ӱ���ػ�����
	S32			CriticalTimes;		// �ػ�ȫ�ְٷֱ��˺�����
	S32			Critical_gPc;		// �ػ�ȫ�ְٷֱȼ���
	S32			AntiCriticalRating;	// ���ػ��ȼ� Ӱ���ػ�����
	S32			AntiCritical_gPc;	// ���ػ�ȫ�ְٷֱȼ���
	S32			FatalRating;		// �����ȼ� Ӱ����������
	S32			Fatal_gPc;			// ����ȫ�ְٷֱȼ���
	S32			DodgeRating;		// ���ܵȼ� Ӱ�����ܼ���
	S32			Dodge_gPc;			// ����ȫ�ְٷֱȼ���
	S32			HitRating;			// ���еȼ� Ӱ�����м���
	S32			Hit_gPc;			// ����ȫ�ְٷֱȼ���
	S32			DamageHandle_gPc;	// �˺��������ȫ�ְٷֱ�

	S32			PhyDamage;			// �����˺�
	S32			PhyDamage_Pc;		// �����˺��ٷֱ�
	S32			PhyDamage_gPc;		// �����˺�ȫ�ְٷֱ�
	S32			PhyDefence;			// �������
	S32			PhyDefence_Pc;		// ��������ٷֱ�
	S32			PhyDefence_gPc;		// �������ȫ�ְٷֱ�
	S32			MuDamage;			// ľ�˺�
	S32			MuDamage_Pc;		// ľ�˺��ٷֱ�
	S32			MuDamage_gPc;		// ľ�˺�ȫ�ְٷֱ�
	S32			MuDefence;			// ľ����
	S32			MuDefence_Pc;		// ľ�����ٷֱ�
	S32			MuDefence_gPc;		// ľ����ȫ�ְٷֱ�
	S32			HuoDamage;			// ���˺�
	S32			HuoDamage_Pc;		// ���˺��ٷֱ�
	S32			HuoDamage_gPc;		// ���˺�ȫ�ְٷֱ�
	S32			HuoDefence;			// �����
	S32			HuoDefence_Pc;		// ������ٷֱ�
	S32			HuoDefence_gPc;		// �����ȫ�ְٷֱ�
	S32			TuDamage;			// ���˺�
	S32			TuDamage_Pc;		// ���˺��ٷֱ�
	S32			TuDamage_gPc;		// ���˺�ȫ�ְٷֱ�
	S32			TuDefence;			// ������
	S32			TuDefence_Pc;		// �������ٷֱ�
	S32			TuDefence_gPc;		// ������ȫ�ְٷֱ�
	S32			JinDamage;			// ���˺�
	S32			JinDamage_Pc;		// ���˺��ٷֱ�
	S32			JinDamage_gPc;		// ���˺�ȫ�ְٷֱ�
	S32			JinDefence;			// �����
	S32			JinDefence_Pc;		// ������ٷֱ�
	S32			JinDefence_gPc;		// �����ȫ�ְٷֱ�
	S32			ShuiDamage;			// ˮ�˺�
	S32			ShuiDamage_Pc;		// ˮ�˺��ٷֱ�
	S32			ShuiDamage_gPc;		// ˮ�˺�ȫ�ְٷֱ�
	S32			ShuiDefence;		// ˮ����
	S32			ShuiDefence_Pc;		// ˮ�����ٷֱ�
	S32			ShuiDefence_gPc;	// ˮ����ȫ�ְٷֱ�
	S32			PneumaDamage;		// Ԫ���˺�
	S32			PneumaDamage_Pc;	// Ԫ���˺��ٷֱ�
	S32			PneumaDamage_gPc;	// Ԫ���˺�ȫ�ְٷֱ�
	S32			PneumaDefence;		// Ԫ������
	S32			PneumaDefence_Pc;	// Ԫ�������ٷֱ�
	S32			PneumaDefence_gPc;	// Ԫ������ȫ�ְٷֱ�
	S32			Heal;				// ����
	S32			Heal_Pc;			// ���ưٷֱ�
	S32			Heal_gPc;			// ����ȫ�ְٷֱ�

	S32			PhyScale_gPc;		// �����˺�����ȫ�ְٷֱ�(����)
	S32			SplScale_gPc;		// �����˺�����ȫ�ְٷֱ�
	S32			HealScale_gPc;		// ���Ƶ���ȫ�ְٷֱ�
	S32			HateScale_gPc;		// ��޵���ȫ�ְٷֱ�

	S32			DeriveHP_gPc;		// ��ȡ����ȫ�ְٷֱ�
	S32			DeriveMP_gPc;		// ��ȡ����ȫ�ְٷֱ�
	S32			DerivePP_gPc;		// ��ȡԪ��ȫ�ְٷֱ�

	S32			Mu;					// ľ
	S32			Huo;				// ��
	S32			Tu;					// ��
	S32			Jin;				// ��
	S32			Shui;				// ˮ

	Stats();
	void Clear();

	Stats& operator+=(const Stats&);
	Stats& operator-=(const Stats&);
	Stats& operator*=(const F32);

	// ���ռ���
	void Calc(const Stats& _add, const CalcFactor& factor);
	// ��ֵ��ǿ������״̬�����ܵ���ǿ��
	void Plus(const Stats& _orig, const Stats& _plus);
	// ��ǿ���ֵ��+=��-=ʵ�����Ǽ�����ǿ���ֵ
	void PlusDone();
	// ����������Դ�����ԣ���Щֵ���裨���ܣ�����
	void PlusSrc(const Stats& _orig, S32 rate);
	// ���Ե��ӣ���+=��������˵�����
	Stats& Add(const Stats& _add, S32 _count = 1);
};

extern Stats g_Stats;

