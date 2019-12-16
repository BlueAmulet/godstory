//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/GameObjectData.h"

#include "platform/platform.h"
#include "core/bitStream.h"
#include "ts/tsShapeInstance.h"
#include "sfx/sfxSystem.h"
#include "T3D/fx/particleEmitter.h"
#include "T3D/fx/splash.h"
#include "T3D/gameConnection.h"
#include "TS/TSShapeRepository.h"
#include "T3D/fx/explosion.h"
#include "T3D/debris.h"


// 动作动画总表:
GameObjectData::ActionAnimationDef GameObjectData::ActionAnimationList[NumTableActionAnims] =
{
	// *** WARNING ***
	// 必须与GameObjectData.h一致

	// --------------------- 普通动作 ---------------------
	{ "Root_a" , 0.25f, 0, Group_Root },
	{ "Root_b" , 0.25f, 0, Group_Root },
	{ "Root_c" , 0.25f, 0, Group_Root },
	{ "Root_d" , 0.25f, 0, Group_Root },
	{ "Root_e" , 0.25f, 0, Group_Root },
	{ "Root_f" , 0.25f, 0, Group_Root },
	{ "Root_g" , 0.25f, 0, Group_Root },
	{ "Root_h" , 0.25f, 0, Group_Root },
	{ "Root_i" , 0.25f, 0, Group_Root },
	{ "Root_j" , 0.25f, 0, Group_Root },

	{ "Idle_a0" , 0.25f, 10000, Group_Idle },
	{ "Idle_a1" , 0.25f, 10000, Group_Idle },
	{ "Idle_a2" , 0.25f, 10000, Group_Idle },
	{ "Idle_b0" , 0.25f, 10000, Group_Idle },
	{ "Idle_b1" , 0.25f, 10000, Group_Idle },
	{ "Idle_b2" , 0.25f, 10000, Group_Idle },
	{ "Idle_c0" , 0.25f, 10000, Group_Idle },
	{ "Idle_c1" , 0.25f, 10000, Group_Idle },
	{ "Idle_c2" , 0.25f, 10000, Group_Idle },
	{ "Idle_d0" , 0.25f, 10000, Group_Idle },
	{ "Idle_d1" , 0.25f, 10000, Group_Idle },
	{ "Idle_d2" , 0.25f, 10000, Group_Idle },
	{ "Idle_e0" , 0.25f, 10000, Group_Idle },
	{ "Idle_e1" , 0.25f, 10000, Group_Idle },
	{ "Idle_e2" , 0.25f, 10000, Group_Idle },
	{ "Idle_f0" , 0.25f, 10000, Group_Idle },
	{ "Idle_f1" , 0.25f, 10000, Group_Idle },
	{ "Idle_f2" , 0.25f, 10000, Group_Idle },
	{ "Idle_g0" , 0.25f, 10000, Group_Idle },
	{ "Idle_g1" , 0.25f, 10000, Group_Idle },
	{ "Idle_g2" , 0.25f, 10000, Group_Idle },
	{ "Idle_h0" , 0.25f, 10000, Group_Idle },
	{ "Idle_h1" , 0.25f, 10000, Group_Idle },
	{ "Idle_h2" , 0.25f, 10000, Group_Idle },
	{ "Idle_i0" , 0.25f, 10000, Group_Idle },
	{ "Idle_i1" , 0.25f, 10000, Group_Idle },
	{ "Idle_i2" , 0.25f, 10000, Group_Idle },
	{ "Idle_j0" , 0.25f, 10000, Group_Idle },
	{ "Idle_j1" , 0.25f, 10000, Group_Idle },
	{ "Idle_j2" , 0.25f, 10000, Group_Idle },

	{ "Run_a" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_b" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_c" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_d" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_e" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_f" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_g" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_h" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_i" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },
	{ "Run_j" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },

	{ "Walk" , 0.25f, 0, Group_Run, Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },

	{ "Skip" , 0.25f, 5000, Group_Skip , Flags_ScaleVelocity , { 0.0f, 1.0f, 0.0f } },

	{ "Fall" , 0.25f, 0, Group_Jump , Flags_HideWeapon},
	{ "Jump" , 0.25f, 0, Group_Jump , Flags_HideWeapon},

	// --------------------- 战斗动作 ---------------------
	// 战斗待机动作
	{ "Ready_a" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_b" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_c" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_d" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_e" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_f" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_g" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_h" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_i" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },
	{ "Ready_j" , 0.25f, 0, Group_Ready, Flags_ScaleAttack },

	// 普通攻击套路
	{ "Attack_a0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_a1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_a2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_b0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_b1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_b2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_c0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_c1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_c2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_d0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_d1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_d2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_e0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_e1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_e2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_f0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_f1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_f2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_g0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_g1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_g2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_h0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_h1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_h2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_i0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_i1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_i2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_j0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_j1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Attack_j2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },

	// 技能攻击
	{ "SkillAttack_a0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_a1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_a2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_b0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_b1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_b2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_c0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_c1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_c2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_d0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_d1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_d2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_e0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_e1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_e2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_f0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_f1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_f2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_g0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_g1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_g2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_h0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_h1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_h2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_i0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_i1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_i2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_j0" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_j1" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "SkillAttack_j2" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },

	// 重击动作
	{ "Critical_a" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_b" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_c" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_d" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_e" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_f" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_g" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_h" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_i" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Critical_j" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },

	// 致命攻击动作
	{ "Fatal_a" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_b" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_c" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_d" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_e" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_f" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_g" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_h" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_i" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },
	{ "Fatal_j" , 0.10f, 5000, Group_Attack, Flags_Repeat | Flags_ScaleAttack },

	// 施法动作
	{ "ReadySpell_a0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_a1" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_b0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_c0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_d0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_e0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_f0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_g0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_h0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_i0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ReadySpell_j0" , 0.10f, 0, Group_ReadySpell, Flags_ScaleAttack | Flags_HideWeapon },

	{ "SpellCast_a0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_a1" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_b0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_c0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_d0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_e0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_f0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_g0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_h0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_i0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },
	{ "SpellCast_j0" , 0.10f, 5000, Group_SpellCast, Flags_Repeat | Flags_ScaleAttack | Flags_HideWeapon },

	{ "ChannelCast_a0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_a1" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_b0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_c0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_d0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_e0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_f0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_g0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_h0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_i0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },
	{ "ChannelCast_j0" , 0.10f, 0, Group_ChannelCast, Flags_ScaleAttack | Flags_HideWeapon },

	// 冲撞
	{ "ChargeCast" , 0.25f, 0, Group_Priority, Flags_WholeOnly},
	{ "ChargeLoop" , 0.25f, 0, Group_Priority, Flags_WholeOnly},
	// 被击
	{ "Wound" , 0.25f, 0, Group_Priority, Flags_WholeOnly},
	// 击退
	{ "BeatBack" , 0.25f, 0, Group_Priority, Flags_WholeOnly},
	// 击飞
	{ "BeatFly" , 0.25f, 0, Group_Priority, Flags_WholeOnly},
	// 击倒
	{ "KnockDownStart" , 0.25f, 0, Group_Priority, Flags_WholeOnly},
	{ "KnockDownLoop" , 0.25f, 0, Group_Priority, Flags_WholeOnly},
	{ "KnockDownEnd" , 0.25f, 0, Group_Priority, Flags_WholeOnly},
	// 晕眩
	{ "Stun" , 0.25f, 0, Group_Run, Flags_WholeOnly},
	// 恐惧
	{ "Fear" , 0.25f, 0, Group_Priority, Flags_WholeOnly},

	// --------------------- 死亡动作 ---------------------
	{ "Death" , 0.25f, 0, Group_Priority, Flags_WholeOnly},

	// --------------------- 生活动作 ---------------------
	// 制造
	{ "Produce" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},
	// 开采
	{ "Mine" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},
	// 砍伐
	{ "Cut" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},
	// 钓鱼
	{ "FishingCast" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},
	{ "FishingLoop" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},
	{ "FishingEnd" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},
	// 打坐
	{ "Sit" , 0.25f, 0, Group_Priority, Flags_WholeOnly | Flags_HideWeapon},

	// --------------------- 交际动作 ---------------------
	// 背&被背
	{ "Carry" , 0.25f, 0, Group_Priority, Flags_WholeOnly | Flags_HideWeapon},
	{ "Carried" , 0.25f, 0, Group_Priority, Flags_WholeOnly | Flags_HideWeapon},
	// 抱&被抱
	{ "Hold" , 0.25f, 0, Group_Priority, Flags_WholeOnly | Flags_HideWeapon},
	{ "Held" , 0.25f, 0, Group_Priority, Flags_WholeOnly | Flags_HideWeapon},

	// --------------------- 表情动作 ---------------------
	{ "Emote_00" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_01" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_02" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_03" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_04" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_05" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_06" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_07" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_08" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_09" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_10" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_11" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_12" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_13" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_14" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_15" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_16" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_17" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_18" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_19" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_20" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_21" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_22" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_23" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_24" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_25" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_26" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_27" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_28" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},
	{ "Emote_29" , 0.25f, 8000, Group_Emote, Flags_WholeOnly},

	// --------------------- 补充 ---------------------
	{ "Mount_a" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_b" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_c" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_d" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_e" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_f" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_g" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_h" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_i" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},
	{ "Mount_j" , 0.25f, 0, Group_Mount, Flags_HideWeapon | Flags_ScaleVelocity},

	{ "Mount_a0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_a1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_a2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_b0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_b1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_b2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_c0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_c1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_c2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_d0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_d1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_d2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_e0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_e1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_e2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_f0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_f1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_f2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_g0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_g1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_g2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_h0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_h1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_h2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_i0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_i1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_i2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_j0" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_j1" , 0.25f, 0, Group_Mount, Flags_HideWeapon},
	{ "Mount_j2" , 0.25f, 0, Group_Mount, Flags_HideWeapon},

	{ "Gather" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},
	{ "Hunt" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},
	{ "Plant" , 0.25f, 0, Group_Idle, Flags_WholeOnly | Flags_HideWeapon},

};

U32 GameObjectData::AnimSetMask[Group_Max] =
{
	/*Root*/	Upper_Root | Lower_Root | Upper_Ready | Lower_Ready | Upper_Run | Lower_Run | Upper_Jump | Lower_Jump | Upper_Mount | Lower_Mount /*| Whole_Combat*/,
	/*Idle*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle,
	/*Run*/		Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote | Upper_Ready | Lower_Ready | Upper_Run | Lower_Run | Upper_Jump | Lower_Jump | Lower_Move | Upper_Mount | Lower_Mount,
	/*Skip*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote | Upper_Ready | Lower_Ready | Upper_Run | Lower_Run | Upper_Jump | Lower_Jump | Upper_Run | Lower_Run,
	/*Jump*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote | Upper_Ready | Lower_Ready | Upper_Run | Lower_Run | Upper_Jump | Lower_Jump | Upper_Run | Lower_Run,
	/*Ready*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote | Upper_Ready | Lower_Ready | Upper_Run | Lower_Run | Upper_Jump | Lower_Jump | Upper_Mount | Lower_Mount,
	/*Emote*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote,
	/*Attack*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote | Upper_Ready | Lower_Ready | Upper_Run | Upper_Jump | Whole_Combat | Upper_Run | Upper_Mount,
	/*RdSpell*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote | Upper_Ready | Lower_Ready | Upper_Run | Upper_Jump | Whole_Combat | Upper_Run | Upper_Mount,
	/*Cast*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote | Upper_Ready | Lower_Ready | Upper_Run | Upper_Jump | Whole_Combat | Upper_Run | Upper_Mount,
	/*ChCast*/	Upper_Root | Lower_Root | Upper_Idle | Lower_Idle | Upper_Emote | Lower_Emote | Upper_Ready | Lower_Ready | Upper_Run | Upper_Jump | Whole_Combat | Upper_Run | Upper_Mount,
	/*Mount*/	0xFFFFFFFF & ~(Upper_Attack | Upper_ReadySpell | Upper_SpellCast | Upper_ChannelCast),
	0xFFFFFFFF,
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(GameObjectData);

GameObjectData::GameObjectData()
{
	// shapeBase的信息
	shadowEnable = true;
	shadowCanMove = true;
	shadowCanAnimate = true;
	shadowSelfShadow = false;
	shadowSize = 128;
	shadowProjectionDistance = 14.0f;

	mass = 9.0f;
	maxEnergy =  60.0f;
	drag = 0.3f;
	density = 10.0f;


	bInitialized = false;
	renderFirstPerson = true;
	pickupRadius = 0.0f;
	minLookAngle = -1.4f;
	maxLookAngle = 1.4f;
	maxFreelookAngle = 3.0f;
	maxTimeScale = 1.5f;

	runForce = 40.0f * 9.0f;
	runEnergyDrain = 0.0f;
	minRunEnergy = 0.0f;
	maxForwardSpeed = 10.0f;
	maxBackwardSpeed = 10.0f;
	maxSideSpeed = 10.0f;
	maxUnderwaterForwardSpeed = 10;
	maxUnderwaterBackwardSpeed = 10;
	maxUnderwaterSideSpeed = 10;

	maxStepHeight = 1.0f;
	runSurfaceAngle = 80.0f;

	recoverDelay = 30;
	recoverRunForceScale = 1.0f;

	jumpForce = 75.0f;
	jumpEnergyDrain = 0.0f;
	minJumpEnergy = 0.0f;
	jumpSurfaceAngle = 78.0f;
	jumpDelay = 30;
	minJumpSpeed = 500.0f;
	maxJumpSpeed = 2.0f * minJumpSpeed;

	// Jetting
	//jetJumpForce = 0;
	//jetJumpEnergyDrain = 0;
	//jetMinJumpEnergy = 0;
	//jetJumpSurfaceAngle = 78;
	//jetMinJumpSpeed = 20;
	//jetMaxJumpSpeed = 100;

	horizMaxSpeed = 80.0f;
	horizResistSpeed = 38.0f;
	horizResistFactor = 1.0f;

	upMaxSpeed = 80.0f;
	upResistSpeed = 38.0f;
	upResistFactor = 1.0f;

	minImpactSpeed = 25.0f;

	decalData      = NULL;
	decalID        = 0;
	decalOffset      = 0.0f;

	// size of bounding box
	boxSize.set(0.3f, 0.3f, 2.0f );
	// scale of obj
	objScale.set(1.0f, 1.0f, 1.0f );

	// location of head, torso, legs
	boxHeadPercentage = 0.85f;
	boxTorsoPercentage = 0.55f;

	// damage locations
	boxHeadLeftPercentage  = 0;
	boxHeadRightPercentage = 1;
	boxHeadBackPercentage  = 0;
	boxHeadFrontPercentage = 1;

	for (S32 i = 0; i < MaxSounds; i++)
		sound[i] = NULL;

	footPuffEmitter = NULL;
	footPuffID = 0;
	footPuffNumParts = 15;
	footPuffRadius = .25f;

	dustEmitter = NULL;
	dustID = 0;

	splash = NULL;
	splashId = 0;
	splashVelocity = 1.0f;
	splashAngle = 45.0f;
	splashFreqMod = 300.0f;
	splashVelEpsilon = 0.25f;
	bubbleEmitTime = 0.4f;

	medSplashSoundVel = 2.0f;
	hardSplashSoundVel = 3.0f;
	exitSplashSoundVel = 2.0f;
	footSplashHeight = 0.1f;

	dMemset( splashEmitterList, 0, sizeof( splashEmitterList ) );
	dMemset( splashEmitterIDList, 0, sizeof( splashEmitterIDList ) );

	groundImpactMinSpeed = 10.0f;
	shakeId = 0;

	// Air control
	airControl = 0.0f;

	jumpTowardsNormal = true;

	dMemset( actionList, 0, sizeof(actionList) );
	armStatus = Arm_A;

	aiChar = 0;
	aiSpell = 0;

	// 以下数据以后可以精简,或者需要加到data中去 [3/11/2009 joy]
	{
		renderFirstPerson = false;
		emap = true;
		canObserve = true;
		aiAvoidThis = false;
		mass = 90.0f;
		drag = 0.3f;
		density = 10;
		maxDamage = 100;
		maxEnergy =  60;
		repairRate = 0.33f;

		runEnergyDrain = 0;
		minRunEnergy = 0;
		maxForwardSpeed = MAX_FORWARD_SPEED;
		maxWalkSpeed = MAX_WALK_SPEED;
		maxBackwardSpeed = 1;
		maxSideSpeed = 4;
		maxUnderwaterForwardSpeed = 2.4f;
		maxUnderwaterBackwardSpeed = 0.8f;
		maxUnderwaterSideSpeed = 2.4f;
		// 保证能在一个tick内速度改变量有maxForwardSpeed的两倍，此时感觉不到加速度
		runForce = 64 * mass * maxForwardSpeed;
		jumpForce = 8.3 * mass;
		jumpEnergyDrain = 0;
		minJumpEnergy = 0;
		jumpDelay = 15;
		recoverDelay = 9;
		recoverRunForceScale = 1.2;
		boxSize.set(0.3f, 0.3f, 2.0f);
		pickupRadius = 0.75;

		boxHeadPercentage             = 0.83;
		boxTorsoPercentage            = 0.49;
		boxHeadLeftPercentage         = 0;
		boxHeadRightPercentage        = 1;
		boxHeadBackPercentage         = 0;
		boxHeadFrontPercentage        = 1;

		//decalData = PlayerFootprint;
		decalOffset = 0.25;
		//footPuffEmitter = LightPuffEmitter;
		footPuffNumParts = 10;
		footPuffRadius = 0.25;
		//dustEmitter = LiftoffDustEmitter;
		//splash = PlayerSplash;
		splashVelocity = 4.0;
		splashAngle = 67.0;
		splashFreqMod = 300.0;
		splashVelEpsilon = 0.60;
		bubbleEmitTime = 0.4;
		//splashEmitter[0] = PlayerFoamDropletsEmitter;
		//splashEmitter[1] = PlayerFoamEmitter;
		//splashEmitter[2] = PlayerBubbleEmitter;
		medSplashSoundVel = 10.0;   
		hardSplashSoundVel = 20.0;   
		exitSplashSoundVel = 5.0;

		runSurfaceAngle  = 70;
		jumpSurfaceAngle = 80;

		minJumpSpeed = 20;
		maxJumpSpeed = 30;

		horizMaxSpeed = 68;
		horizResistSpeed = 33;
		horizResistFactor = 0.35;
		upMaxSpeed = 80;
		upResistSpeed = 25;
		upResistFactor = 0.3;
		//footstepSplashHeight = 0.35;

		//FootSoftSound        = FootLightSoftSound;
		//FootHardSound        = FootLightHardSound;
		//FootMetalSound       = FootLightMetalSound;
		//FootSnowSound        = FootLightSnowSound;
		//FootShallowSound     = FootLightShallowSplashSound;
		//FootWadingSound      = FootLightWadingSound;
		//FootUnderwaterSound  = FootLightUnderwaterSound;

		groundImpactMinSpeed    = 10.0;
		shakeId = 0;
		//exitingWater         = ExitingWaterLightSound;
	}
}

bool GameObjectData::preload(bool server, char errorBuffer[256])
{
	if(!Parent::preload(server, errorBuffer))
		return false;

	// Resolve objects transmitted from server
	if (!server) {
		for (S32 i = 0; i < MaxSounds; i++)
			if (sound[i])
				Sim::findObject(SimObjectId(sound[i]),sound[i]);
	}

	//
	runSurfaceCos = mCos(mDegToRad(runSurfaceAngle));
	jumpSurfaceCos = mCos(mDegToRad(jumpSurfaceAngle));
	if (minJumpEnergy < jumpEnergyDrain)
		minJumpEnergy = jumpEnergyDrain;

	// Jetting
	if (jetMinJumpEnergy < jetJumpEnergyDrain)
		jetMinJumpEnergy = jetJumpEnergyDrain;

	// Validate some of the data
	if (recoverDelay > (1 << RecoverDelayBits) - 1) {
		recoverDelay = (1 << RecoverDelayBits) - 1;
		Con::printf("GameObjectData:: Recover delay exceeds range (0-%d)",recoverDelay);
	}
	if (jumpDelay > (1 << JumpDelayBits) - 1) {
		jumpDelay = (1 << JumpDelayBits) - 1;
		Con::printf("GameObjectData:: Jump delay exceeds range (0-%d)",jumpDelay);
	}

#ifdef NTJ_EDITOR
	//// Go ahead a pre-load the player shape
	//TSShapeInstance* si = new TSShapeInstance(shape, false);
	//TSThread* thread = si->addThread();

	//// Extract ground transform velocity from animations
	//// Get the named ones first so they can be indexed directly.
	//ActionAnimation *dp = &actionList[0];
	//for (int i = 0; i < NumTableActionAnims; i++,dp++)
	//{
	//	ActionAnimationDef *sp = &ActionAnimationList[i];
	//	dp->sequence      = shape->findSequence(sp->name);
	//}
	//actionCount = dp - actionList;
	//AssertFatal(actionCount <= NumActionAnims, "Too many action animations!");
	//delete si;

	//// Resolve spine
	//spineNode[0] = shape->findNode("Bip01 Pelvis");
	//spineNode[1] = shape->findNode("Bip01 Spine");
	//spineNode[2] = shape->findNode("Bip01 Spine1");
	//spineNode[3] = shape->findNode("Bip01 Spine2");
	//spineNode[4] = shape->findNode("Bip01 Neck");
	//spineNode[5] = shape->findNode("Bip01 Head");

	//// Recoil animations
	//recoilSequence[0] = shape->findSequence("light_recoil");
	//recoilSequence[1] = shape->findSequence("medium_recoil");
	//recoilSequence[2] = shape->findSequence("heavy_recoil");
#endif
	// Convert pickupRadius to a delta of boundingBox
	F32 dr = (boxSize.x > boxSize.y)? boxSize.x: boxSize.y;
	if (pickupRadius < dr)
		pickupRadius = dr;
	else
		if (pickupRadius > 2.0f * dr)
			pickupRadius = 2.0f * dr;
	pickupDelta = (S32)(pickupRadius - dr);

	// Validate jump speed
	if (maxJumpSpeed <= minJumpSpeed)
		maxJumpSpeed = minJumpSpeed + 0.1f;

	// Load up all the emitters
	if (!footPuffEmitter && footPuffID != 0)
		if (!Sim::findObject(footPuffID, footPuffEmitter))
			Con::errorf(ConsoleLogEntry::General, "GameObjectData::preload - Invalid packet, bad datablockId(footPuffEmitter): 0x%x", footPuffID);

	/*
	if (!decalData && decalID != 0 )
	if (!Sim::findObject(decalID, decalData))
	Con::errorf(ConsoleLogEntry::General, "GameObjectData::preload Invalid packet, bad datablockId(decalData): 0x%x", decalID);
	*/
	if (!dustEmitter && dustID != 0 )
		if (!Sim::findObject(dustID, dustEmitter))
			Con::errorf(ConsoleLogEntry::General, "GameObjectData::preload - Invalid packet, bad datablockId(dustEmitter): 0x%x", dustID);

	for (int i=0; i<NUM_SPLASH_EMITTERS; i++)
		if( !splashEmitterList[i] && splashEmitterIDList[i] != 0 )
			if( Sim::findObject( splashEmitterIDList[i], splashEmitterList[i] ) == false)
				Con::errorf(ConsoleLogEntry::General, "GameObjectData::onAdd - Invalid packet, bad datablockId(particle emitter): 0x%x", splashEmitterIDList[i]);

	return true;
}

bool GameObjectData::isTableSequence(S32 seq)
{
	// The sequences from the table must already have
	// been loaded for this to work.
	for (int i = 0; i < NumTableActionAnims; i++)
		if (actionList[i].sequence == seq)
			return true;
	return false;
}

bool GameObjectData::isJumpAction(U32 action)
{
	return (action == Jump);
}

void GameObjectData::selectAction(U32 newAct, U32 oldUpper, U32 oldLower, U32& newUpper, U32& newLower)
{
	if(oldUpper == GameObjectData::NullAnimation)
		oldUpper = 0;
	if(oldLower == GameObjectData::NullAnimation)
		oldLower = 0;
	// 保证数值合法
	//AssertFatal(newAct < NumTableActionAnims, "GameObjectData::selectAction error newAct");
	//AssertFatal(oldUpper < NumTableActionAnims, "GameObjectData::selectAction error oldUpper");
	//AssertFatal(oldLower < NumTableActionAnims, "GameObjectData::selectAction error oldLower");
	//if(newAct >= NumTableActionAnims)
	//	newAct = 0;
	//if(oldUpper >= NumTableActionAnims)
	//	oldUpper = 0;
	//if(oldLower >= NumTableActionAnims)
	//	oldLower = 0;

	if(AnimSetMask[ActionAnimationList[newAct].group] & BIT(ActionAnimationList[oldUpper].group))
		newUpper = newAct;
	else
		newUpper = oldUpper;
	if(AnimSetMask[ActionAnimationList[newAct].group] & BIT(ActionAnimationList[oldLower].group+Group_Max))
		newLower = newAct;
	else
		newLower = oldLower;
}

void GameObjectData::initPersistFields()
{
	Parent::initPersistFields();

	addField("renderFirstPerson", TypeBool, Offset(renderFirstPerson, GameObjectData));
	addField("pickupRadius", TypeF32, Offset(pickupRadius, GameObjectData));

	addField("minLookAngle", TypeF32, Offset(minLookAngle, GameObjectData));
	addField("maxLookAngle", TypeF32, Offset(maxLookAngle, GameObjectData));
	addField("maxFreelookAngle", TypeF32, Offset(maxFreelookAngle, GameObjectData));

	addField("maxTimeScale", TypeF32, Offset(maxTimeScale, GameObjectData));

	addField("maxStepHeight", TypeF32, Offset(maxStepHeight, GameObjectData));
	addField("runForce", TypeF32, Offset(runForce, GameObjectData));
	addField("runEnergyDrain", TypeF32, Offset(runEnergyDrain, GameObjectData));
	addField("minRunEnergy", TypeF32, Offset(minRunEnergy, GameObjectData));
	addField("maxForwardSpeed", TypeF32, Offset(maxForwardSpeed, GameObjectData));
	addField("maxBackwardSpeed", TypeF32, Offset(maxBackwardSpeed, GameObjectData));
	addField("maxSideSpeed", TypeF32, Offset(maxSideSpeed, GameObjectData));
	addField("maxUnderwaterForwardSpeed", TypeF32, Offset(maxUnderwaterForwardSpeed, GameObjectData));
	addField("maxUnderwaterBackwardSpeed", TypeF32, Offset(maxUnderwaterBackwardSpeed, GameObjectData));
	addField("maxUnderwaterSideSpeed", TypeF32, Offset(maxUnderwaterSideSpeed, GameObjectData));
	addField("runSurfaceAngle", TypeF32, Offset(runSurfaceAngle, GameObjectData));
	addField("minImpactSpeed", TypeF32, Offset(minImpactSpeed, GameObjectData));

	addField("recoverDelay", TypeS32, Offset(recoverDelay, GameObjectData));
	addField("recoverRunForceScale", TypeF32, Offset(recoverRunForceScale, GameObjectData));

	addField("jumpForce", TypeF32, Offset(jumpForce, GameObjectData));
	addField("jumpEnergyDrain", TypeF32, Offset(jumpEnergyDrain, GameObjectData));
	addField("minJumpEnergy", TypeF32, Offset(minJumpEnergy, GameObjectData));
	addField("minJumpSpeed", TypeF32, Offset(minJumpSpeed, GameObjectData));
	addField("maxJumpSpeed", TypeF32, Offset(maxJumpSpeed, GameObjectData));
	addField("jumpSurfaceAngle", TypeF32, Offset(jumpSurfaceAngle, GameObjectData));
	addField("jumpDelay", TypeS32, Offset(jumpDelay, GameObjectData));

	// Jetting
	addField("jetJumpForce", TypeF32, Offset(jetJumpForce, GameObjectData));
	addField("jetJumpEnergyDrain", TypeF32, Offset(jetJumpEnergyDrain, GameObjectData));
	addField("jetMinJumpEnergy", TypeF32, Offset(jetMinJumpEnergy, GameObjectData));
	addField("jetMinJumpSpeed", TypeF32, Offset(jetMinJumpSpeed, GameObjectData));
	addField("jetMaxJumpSpeed", TypeF32, Offset(jetMaxJumpSpeed, GameObjectData));
	addField("jetJumpSurfaceAngle", TypeF32, Offset(jetJumpSurfaceAngle, GameObjectData));

	addField("boundingBox", TypePoint3F, Offset(boxSize, GameObjectData));
	addField("boxHeadPercentage", TypeF32, Offset(boxHeadPercentage, GameObjectData));
	addField("boxTorsoPercentage", TypeF32, Offset(boxTorsoPercentage, GameObjectData));
	addField("boxHeadLeftPercentage", TypeS32, Offset(boxHeadLeftPercentage, GameObjectData));
	addField("boxHeadRightPercentage", TypeS32, Offset(boxHeadRightPercentage, GameObjectData));
	addField("boxHeadBackPercentage", TypeS32, Offset(boxHeadBackPercentage, GameObjectData));
	addField("boxHeadFrontPercentage", TypeS32, Offset(boxHeadFrontPercentage, GameObjectData));

	addField("horizMaxSpeed", TypeF32, Offset(horizMaxSpeed, GameObjectData));
	addField("horizResistSpeed", TypeF32, Offset(horizResistSpeed, GameObjectData));
	addField("horizResistFactor", TypeF32, Offset(horizResistFactor, GameObjectData));

	addField("upMaxSpeed", TypeF32, Offset(upMaxSpeed, GameObjectData));
	addField("upResistSpeed", TypeF32, Offset(upResistSpeed, GameObjectData));
	addField("upResistFactor", TypeF32, Offset(upResistFactor, GameObjectData));

	//   addField("decalData",         TypeDecalDataPtr, Offset(decalData, GameObjectData));
	addField("decalOffset",TypeF32, Offset(decalOffset, GameObjectData));

	addField("footPuffEmitter",   TypeParticleEmitterDataPtr,   Offset(footPuffEmitter,    GameObjectData));
	addField("footPuffNumParts",  TypeS32,                      Offset(footPuffNumParts,   GameObjectData));
	addField("footPuffRadius",    TypeF32,                      Offset(footPuffRadius,     GameObjectData));
	addField("dustEmitter",       TypeParticleEmitterDataPtr,   Offset(dustEmitter,        GameObjectData));

	addField("FootSoftSound",       TypeSFXProfilePtr, Offset(sound[FootSoft],          GameObjectData));
	addField("FootHardSound",       TypeSFXProfilePtr, Offset(sound[FootHard],          GameObjectData));
	addField("FootMetalSound",      TypeSFXProfilePtr, Offset(sound[FootMetal],         GameObjectData));
	addField("FootSnowSound",       TypeSFXProfilePtr, Offset(sound[FootSnow],          GameObjectData));
	addField("FootShallowSound",    TypeSFXProfilePtr, Offset(sound[FootShallowSplash], GameObjectData));
	addField("FootWadingSound",     TypeSFXProfilePtr, Offset(sound[FootWading],        GameObjectData));
	addField("FootUnderwaterSound", TypeSFXProfilePtr, Offset(sound[FootUnderWater],    GameObjectData));
	addField("FootBubblesSound",    TypeSFXProfilePtr, Offset(sound[FootBubbles],       GameObjectData));
	addField("movingBubblesSound",   TypeSFXProfilePtr, Offset(sound[MoveBubbles],        GameObjectData));
	addField("waterBreathSound",     TypeSFXProfilePtr, Offset(sound[WaterBreath],        GameObjectData));

	addField("impactSoftSound",   TypeSFXProfilePtr, Offset(sound[ImpactSoft],  GameObjectData));
	addField("impactHardSound",   TypeSFXProfilePtr, Offset(sound[ImpactHard],  GameObjectData));
	addField("impactMetalSound",  TypeSFXProfilePtr, Offset(sound[ImpactMetal], GameObjectData));
	addField("impactSnowSound",   TypeSFXProfilePtr, Offset(sound[ImpactSnow],  GameObjectData));

	addField("mediumSplashSoundVelocity", TypeF32,     Offset(medSplashSoundVel,  GameObjectData));
	addField("hardSplashSoundVelocity",   TypeF32,     Offset(hardSplashSoundVel,  GameObjectData));
	addField("exitSplashSoundVelocity",   TypeF32,     Offset(exitSplashSoundVel,  GameObjectData));

	addField("impactWaterEasy",   TypeSFXProfilePtr, Offset(sound[ImpactWaterEasy],   GameObjectData));
	addField("impactWaterMedium", TypeSFXProfilePtr, Offset(sound[ImpactWaterMedium], GameObjectData));
	addField("impactWaterHard",   TypeSFXProfilePtr, Offset(sound[ImpactWaterHard],   GameObjectData));
	addField("exitingWater",      TypeSFXProfilePtr, Offset(sound[ExitWater],         GameObjectData));

	addField("splash",         TypeSplashDataPtr,      Offset(splash,          GameObjectData));
	addField("splashVelocity", TypeF32,                Offset(splashVelocity,  GameObjectData));
	addField("splashAngle",    TypeF32,                Offset(splashAngle,     GameObjectData));
	addField("splashFreqMod",  TypeF32,                Offset(splashFreqMod,   GameObjectData));
	addField("splashVelEpsilon", TypeF32,              Offset(splashVelEpsilon, GameObjectData));
	addField("bubbleEmitTime", TypeF32,                Offset(bubbleEmitTime,  GameObjectData));
	addField("splashEmitter",  TypeParticleEmitterDataPtr,   Offset(splashEmitterList,   GameObjectData), NUM_SPLASH_EMITTERS);
	addField("footstepSplashHeight",      TypeF32,     Offset(footSplashHeight,  GameObjectData));

	addField("groundImpactMinSpeed",       TypeF32,       Offset(groundImpactMinSpeed,        GameObjectData));
	addField("shakeId",                    TypeS32,       Offset(shakeId,    GameObjectData));

	// Air control
	addField("airControl",                 TypeF32,       Offset(airControl,GameObjectData));
	addField("jumpTowardsNormal",          TypeBool,      Offset(jumpTowardsNormal,GameObjectData));
}

void GameObjectData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->writeFlag(renderFirstPerson);

	stream->write(minLookAngle);
	stream->write(maxLookAngle);
	stream->write(maxFreelookAngle);
	stream->write(maxTimeScale);

	stream->write(mass);
	stream->write(maxEnergy);
	stream->write(drag);
	stream->write(density);

	stream->write(maxStepHeight);

	stream->write(runForce);
	stream->write(runEnergyDrain);
	stream->write(minRunEnergy);
	stream->write(maxForwardSpeed);
	stream->write(maxBackwardSpeed);
	stream->write(maxSideSpeed);
	stream->write(maxUnderwaterForwardSpeed);
	stream->write(maxUnderwaterBackwardSpeed);
	stream->write(maxUnderwaterSideSpeed);
	stream->write(runSurfaceAngle);

	stream->write(recoverDelay);
	stream->write(recoverRunForceScale);

	stream->write(jumpForce);
	stream->write(jumpEnergyDrain);
	stream->write(minJumpEnergy);
	stream->write(minJumpSpeed);
	stream->write(maxJumpSpeed);
	stream->write(jumpSurfaceAngle);
	stream->writeInt(jumpDelay,JumpDelayBits);

	// Jetting
	stream->write(jetJumpForce);
	stream->write(jetJumpEnergyDrain);
	stream->write(jetMinJumpEnergy);
	stream->write(jetMinJumpSpeed);
	stream->write(jetMaxJumpSpeed);
	stream->write(jetJumpSurfaceAngle);

	stream->write(horizMaxSpeed);
	stream->write(horizResistSpeed);
	stream->write(horizResistFactor);

	stream->write(upMaxSpeed);
	stream->write(upResistSpeed);
	stream->write(upResistFactor);

	stream->write(splashVelocity);
	stream->write(splashAngle);
	stream->write(splashFreqMod);
	stream->write(splashVelEpsilon);
	stream->write(bubbleEmitTime);

	stream->write(medSplashSoundVel);
	stream->write(hardSplashSoundVel);
	stream->write(exitSplashSoundVel);
	stream->write(footSplashHeight);
	// Don't need damage scale on the client
	stream->write(minImpactSpeed);

	S32 i;
	for ( i = 0; i < MaxSounds; i++)
		if (stream->writeFlag(sound[i]))
			stream->writeRangedU32(packed? SimObjectId(sound[i]):
	sound[i]->getId(),DataBlockObjectIdFirst,DataBlockObjectIdLast);

	stream->write(boxSize.x);
	stream->write(boxSize.y);
	stream->write(boxSize.z);

	if( stream->writeFlag( footPuffEmitter ) )
	{
		stream->writeRangedU32( footPuffEmitter->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
	}

	stream->write( footPuffNumParts );
	stream->write( footPuffRadius );

	/*
	if( stream->writeFlag( decalData ) )
	{
	stream->writeRangedU32( decalData->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
	}
	stream->write(decalOffset);
	*/

	if( stream->writeFlag( dustEmitter ) )
	{
		stream->writeRangedU32( dustEmitter->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
	}


	if (stream->writeFlag( splash ))
	{
		stream->writeRangedU32(splash->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast);
	}

	for( i=0; i<NUM_SPLASH_EMITTERS; i++ )
	{
		if( stream->writeFlag( splashEmitterList[i] != NULL ) )
		{
			stream->writeRangedU32( splashEmitterList[i]->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
		}
	}


	stream->write(groundImpactMinSpeed);
	stream->write(shakeId);

	// Air control
	stream->write(airControl);

	// Jump off at normal
	stream->writeFlag(jumpTowardsNormal);
}

void GameObjectData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	renderFirstPerson = stream->readFlag();

	stream->read(&minLookAngle);
	stream->read(&maxLookAngle);
	stream->read(&maxFreelookAngle);
	stream->read(&maxTimeScale);

	stream->read(&mass);
	stream->read(&maxEnergy);
	stream->read(&drag);
	stream->read(&density);

	stream->read(&maxStepHeight);

	stream->read(&runForce);
	stream->read(&runEnergyDrain);
	stream->read(&minRunEnergy);
	stream->read(&maxForwardSpeed);
	stream->read(&maxBackwardSpeed);
	stream->read(&maxSideSpeed);
	stream->read(&maxUnderwaterForwardSpeed);
	stream->read(&maxUnderwaterBackwardSpeed);
	stream->read(&maxUnderwaterSideSpeed);
	stream->read(&runSurfaceAngle);

	stream->read(&recoverDelay);
	stream->read(&recoverRunForceScale);

	stream->read(&jumpForce);
	stream->read(&jumpEnergyDrain);
	stream->read(&minJumpEnergy);
	stream->read(&minJumpSpeed);
	stream->read(&maxJumpSpeed);
	stream->read(&jumpSurfaceAngle);
	jumpDelay = stream->readInt(JumpDelayBits);

	// Jetting
	stream->read(&jetJumpForce);
	stream->read(&jetJumpEnergyDrain);
	stream->read(&jetMinJumpEnergy);
	stream->read(&jetMinJumpSpeed);
	stream->read(&jetMaxJumpSpeed);
	stream->read(&jetJumpSurfaceAngle);

	stream->read(&horizMaxSpeed);
	stream->read(&horizResistSpeed);
	stream->read(&horizResistFactor);

	stream->read(&upMaxSpeed);
	stream->read(&upResistSpeed);
	stream->read(&upResistFactor);

	stream->read(&splashVelocity);
	stream->read(&splashAngle);
	stream->read(&splashFreqMod);
	stream->read(&splashVelEpsilon);
	stream->read(&bubbleEmitTime);

	stream->read(&medSplashSoundVel);
	stream->read(&hardSplashSoundVel);
	stream->read(&exitSplashSoundVel);
	stream->read(&footSplashHeight);

	stream->read(&minImpactSpeed);

	S32 i;
	for (i = 0; i < MaxSounds; i++) {
		sound[i] = NULL;
		if (stream->readFlag())
			sound[i] = (SFXProfile*)stream->readRangedU32(DataBlockObjectIdFirst,
			DataBlockObjectIdLast);
	}

	stream->read(&boxSize.x);
	stream->read(&boxSize.y);
	stream->read(&boxSize.z);

	if( stream->readFlag() )
	{
		footPuffID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
	}

	stream->read(&footPuffNumParts);
	stream->read(&footPuffRadius);

	/*
	if( stream->readFlag() )
	{
	decalID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
	}
	stream->read(&decalOffset);
	*/

	if( stream->readFlag() )
	{
		dustID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
	}


	if (stream->readFlag())
	{
		splashId = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
	}

	for( i=0; i<NUM_SPLASH_EMITTERS; i++ )
	{
		if( stream->readFlag() )
		{
			splashEmitterIDList[i] = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
		}
	}

	stream->read(&groundImpactMinSpeed);
	stream->read(&shakeId);

	// Air control
	stream->read(&airControl);

	// Jump off at normal
	jumpTowardsNormal = stream->readFlag();
}

bool GameObjectData::initDataBlock()
{
	if(bInitialized)
		return true;

	bool server = false;
#ifdef NTJ_SERVER
	server = true;
#endif

	if(!Parent::initDataBlock())
		return false;

	// Resolve objects transmitted from server
	if (!server) {
		for (S32 i = 0; i < MaxSounds; i++)
			if (sound[i])
				Sim::findObject(SimObjectId(sound[i]),sound[i]);
	}

	//
	runSurfaceCos = mCos(mDegToRad(runSurfaceAngle));
	jumpSurfaceCos = mCos(mDegToRad(jumpSurfaceAngle));
	if (minJumpEnergy < jumpEnergyDrain)
		minJumpEnergy = jumpEnergyDrain;

	// Jetting
	if (jetMinJumpEnergy < jetJumpEnergyDrain)
		jetMinJumpEnergy = jetJumpEnergyDrain;

	// Validate some of the data
	if (recoverDelay > (1 << RecoverDelayBits) - 1) {
		recoverDelay = (1 << RecoverDelayBits) - 1;
		Con::printf("GameObjectData:: Recover delay exceeds range (0-%d)",recoverDelay);
	}
	if (jumpDelay > (1 << JumpDelayBits) - 1) {
		jumpDelay = (1 << JumpDelayBits) - 1;
		Con::printf("GameObjectData:: Jump delay exceeds range (0-%d)",jumpDelay);
	}

	// Convert pickupRadius to a delta of boundingBox
	F32 dr = (boxSize.x > boxSize.y)? boxSize.x: boxSize.y;
	if (pickupRadius < dr)
		pickupRadius = dr;
	else
		if (pickupRadius > 2.0f * dr)
			pickupRadius = 2.0f * dr;
	pickupDelta = (S32)(pickupRadius - dr);

	// Validate jump speed
	if (maxJumpSpeed <= minJumpSpeed)
		maxJumpSpeed = minJumpSpeed + 0.1f;

	// Load up all the emitters
	if (!footPuffEmitter && footPuffID != 0)
		if (!Sim::findObject(footPuffID, footPuffEmitter))
			Con::errorf(ConsoleLogEntry::General, "GameObjectData::preload - Invalid packet, bad datablockId(footPuffEmitter): 0x%x", footPuffID);

	/*
	if (!decalData && decalID != 0 )
	if (!Sim::findObject(decalID, decalData))
	Con::errorf(ConsoleLogEntry::General, "GameObjectData::preload Invalid packet, bad datablockId(decalData): 0x%x", decalID);
	*/
	if (!dustEmitter && dustID != 0 )
		if (!Sim::findObject(dustID, dustEmitter))
			Con::errorf(ConsoleLogEntry::General, "GameObjectData::preload - Invalid packet, bad datablockId(dustEmitter): 0x%x", dustID);

	for (int i=0; i<NUM_SPLASH_EMITTERS; i++)
		if( !splashEmitterList[i] && splashEmitterIDList[i] != 0 )
			if( Sim::findObject( splashEmitterIDList[i], splashEmitterList[i] ) == false)
				Con::errorf(ConsoleLogEntry::General, "GameObjectData::onAdd - Invalid packet, bad datablockId(particle emitter): 0x%x", splashEmitterIDList[i]);

	bInitialized = true;
	return true;
}
