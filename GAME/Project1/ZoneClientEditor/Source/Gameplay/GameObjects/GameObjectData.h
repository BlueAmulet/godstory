//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#ifndef _SHAPEBASE_H_
#include "T3D/shapeBase.h"
#endif
#ifndef _BOXCONVEX_H_
#include "collision/boxConvex.h"
#endif

#include "T3D/gameProcess.h"

class ParticleEmitter;
class ParticleEmitterData;
class DecalData;
class SplashData;
class Player;

//----------------------------------------------------------------------------

struct GameObjectData: public ShapeBaseData {
	typedef ShapeBaseData Parent;
	enum Constants {
		RecoverDelayBits = 7,
		JumpDelayBits = 7,
		NumSpineNodes = 6,
		ImpactBits = 3,
		NUM_SPLASH_EMITTERS = 3,
		BUBBLE_EMITTER = 2,
	};
	bool bInitialized;         ///< 是否已经初始化
	bool renderFirstPerson;    ///< Render the player shape in first person

	F32 pickupRadius;          ///< Radius around player for items (on server)
	F32 maxTimeScale;          ///< Max timeScale for action animations

	F32 minLookAngle;          ///< Lowest angle (radians) the player can look
	F32 maxLookAngle;          ///< Highest angle (radians) the player can look
	F32 maxFreelookAngle;      ///< Max left/right angle the player can look

	/// @name Physics constants
	/// @{

	F32 runForce;                   ///< Force used to accelerate player
	F32 runEnergyDrain;             ///< Energy drain/tick
	F32 minRunEnergy;               ///< Minimum energy required to run
	F32 maxWalkSpeed;               ///< Maximum walk forward speed when running
	F32 maxForwardSpeed;            ///< Maximum forward speed when running
	F32 maxBackwardSpeed;           ///< Maximum backward speed when running
	F32 maxSideSpeed;               ///< Maximum side speed when running
	F32 maxUnderwaterForwardSpeed;  ///< Maximum underwater forward speed when running
	F32 maxUnderwaterBackwardSpeed; ///< Maximum underwater backward speed when running
	F32 maxUnderwaterSideSpeed;     ///< Maximum underwater side speed when running

	F32 maxStepHeight;         ///< Maximum height the player can step up
	F32 runSurfaceAngle;       ///< Maximum angle from vertical in degrees the player can run up

	F32 horizMaxSpeed;         ///< Max speed attainable in the horizontal
	F32 horizResistSpeed;      ///< Speed at which resistence will take place
	F32 horizResistFactor;     ///< Factor of resistence once horizResistSpeed has been reached

	F32 upMaxSpeed;            ///< Max vertical speed attainable
	F32 upResistSpeed;         ///< Speed at which resistence will take place
	F32 upResistFactor;        ///< Factor of resistence once upResistSpeed has been reached

	S32 recoverDelay;          ///< # tick
	F32 recoverRunForceScale;  ///< RunForce multiplier in recover state

	F32 jumpForce;             ///< Force exherted per jump
	F32 jumpEnergyDrain;       ///< Energy drained per jump
	F32 minJumpEnergy;         ///< Minimum energy required to jump
	F32 minJumpSpeed;          ///< Minimum speed needed to jump
	F32 maxJumpSpeed;          ///< Maximum speed before the player can no longer jump
	F32 jumpSurfaceAngle;      ///< Angle from vertical in degrees where the player can jump
	S32 jumpDelay;             ///< Delay time in ticks between jumps

	// Jetting
	F32 jetJumpForce;
	F32 jetJumpEnergyDrain;    ///< Energy per jump
	F32 jetMinJumpEnergy;
	F32 jetMinJumpSpeed;
	F32 jetMaxJumpSpeed;
	F32 jetJumpSurfaceAngle;   ///< Angle vertical degrees
	/// @}

	/// @name Hitboxes
	/// @{

	F32 boxHeadPercentage;
	F32 boxTorsoPercentage;

	S32 boxHeadLeftPercentage;
	S32 boxHeadRightPercentage;
	S32 boxHeadBackPercentage;
	S32 boxHeadFrontPercentage;
	/// @}

	F32 minImpactSpeed;        ///< Minimum impact speed required to apply fall damage

	F32 decalOffset;

	F32 groundImpactMinSpeed;      ///< Minimum impact speed required to apply fall damage with the ground
	S32 shakeId;                   ///

	/// Zounds!
	enum Sounds {
		FootSoft,
		FootHard,
		FootMetal,
		FootSnow,
		FootShallowSplash,
		FootWading,
		FootUnderWater,
		FootBubbles,
		MoveBubbles,
		WaterBreath,
		ImpactSoft,
		ImpactHard,
		ImpactMetal,
		ImpactSnow,
		ImpactWaterEasy,
		ImpactWaterMedium,
		ImpactWaterHard,
		ExitWater,
		MaxSounds
	};
	SFXProfile* sound[MaxSounds];

	Point3F boxSize;           ///< Width, depth, height
	Point3F objScale;          ///< Width, depth, height scale

	enum AnimGroup
	{
		// 普通动作
		Group_Root = 0,
		Group_Idle,
		Group_Run,
		Group_Skip,
		Group_Jump,
		Group_Ready,
		Group_Emote,
		// 攻击动作
		Group_Attack,
		// 施法动作
		Group_ReadySpell,
		Group_SpellCast,
		Group_ChannelCast,
		// 骑乘动作
		Group_Mount,
		// 高优先级动作
		Group_Priority,

		Group_Max,
	};
	enum
	{
		Upper_Root = BIT(Group_Root),
		Upper_Idle = BIT(Group_Idle),
		Upper_Run = BIT(Group_Run),
		Upper_Skip = BIT(Group_Skip),
		Upper_Jump = BIT(Group_Jump),
		Upper_Ready = BIT(Group_Ready),
		Upper_Emote = BIT(Group_Emote),
		Upper_Attack = BIT(Group_Attack),
		Upper_ReadySpell = BIT(Group_ReadySpell),
		Upper_SpellCast = BIT(Group_SpellCast),
		Upper_ChannelCast = BIT(Group_ChannelCast),
		Upper_Mount = BIT(Group_Mount),

		Lower_Root = BIT(Group_Max + Group_Root),
		Lower_Idle = BIT(Group_Max + Group_Idle),
		Lower_Run = BIT(Group_Max + Group_Run),
		Lower_Skip = BIT(Group_Max + Group_Skip),
		Lower_Jump = BIT(Group_Max + Group_Jump),
		Lower_Ready = BIT(Group_Max + Group_Ready),
		Lower_Emote = BIT(Group_Max + Group_Emote),
		Lower_Attack = BIT(Group_Max + Group_Attack),
		Lower_ReadySpell = BIT(Group_Max + Group_ReadySpell),
		Lower_SpellCast = BIT(Group_Max + Group_SpellCast),
		Lower_ChannelCast = BIT(Group_Max + Group_ChannelCast),
		Lower_Mount = BIT(Group_Max + Group_Mount),

		Lower_Move = Upper_Run | Lower_Run | Lower_Attack | Lower_ReadySpell | Lower_SpellCast | Lower_ChannelCast,
		Whole_Combat = Upper_Attack | Upper_ReadySpell | Upper_SpellCast | Upper_ChannelCast | Lower_Attack | Lower_ReadySpell | Lower_SpellCast | Lower_ChannelCast,
	};
	enum AnimateFlags
	{
		Flags_None				= 0,			// 无标识
		Flags_ScaleVelocity		= BIT(0),		// 按移动速度缩放
		Flags_ScaleAttack		= BIT(1),		// 按攻击速度缩放
		Flags_Repeat			= BIT(2),		// 可重复播放，一定是非循环动作
		Flags_WholeOnly			= BIT(3),		// 不可分截播放
		Flags_HideWeapon		= BIT(4),		// 隐藏武器
	};
	/// Animation and other data intialized in onAdd
	struct ActionAnimationDef {
		const char* name;       ///< Sequence name
		F32 transTime;			///< 动画过渡时间
		SimTime delayTicks;		///< before picking another  0即循环
		AnimGroup group;		///< 组
		U32 flags;				///< 标识
		struct Vector {
			F32 x,y,z;
		} dir;                  ///< Default direction
	};
	struct ActionAnimation {
		S32      sequence;      ///< Sequence index
	};
	enum Animations{
		// *** WARNING ***
		// These enum values are used to index the ActionAnimationList

		// a\b等表示按武器分类
		// --------------------- 普通动作 ---------------------
		// 待机
		Root_a = 0,
		Root_b,
		Root_c,
		Root_d,
		Root_e,
		Root_f,
		Root_g,
		Root_h,
		Root_i,
		Root_j,

		// 特殊待机
		Idle_a0,
		Idle_a1,
		Idle_a2,
		Idle_b0,
		Idle_b1,
		Idle_b2,
		Idle_c0,
		Idle_c1,
		Idle_c2,
		Idle_d0,
		Idle_d1,
		Idle_d2,
		Idle_e0,
		Idle_e1,
		Idle_e2,
		Idle_f0,
		Idle_f1,
		Idle_f2,
		Idle_g0,
		Idle_g1,
		Idle_g2,
		Idle_h0,
		Idle_h1,
		Idle_h2,
		Idle_i0,
		Idle_i1,
		Idle_i2,
		Idle_j0,
		Idle_j1,
		Idle_j2,

		Run_a,
		Run_b,
		Run_c,
		Run_d,
		Run_e,
		Run_f,
		Run_g,
		Run_h,
		Run_i,
		Run_j,

		// 步行
		Walk,

		// 小跳
		Skip,

		// 跳跃
		Jump,
		Fall,

		// --------------------- 战斗动作 ---------------------
		// 战斗待机动作
		Ready_a,
		Ready_b,
		Ready_c,
		Ready_d,
		Ready_e,
		Ready_f,
		Ready_g,
		Ready_h,
		Ready_i,
		Ready_j,

		// 普通攻击套路
		Attack_a0,		// 空手
		Attack_a1,
		Attack_a2,
		Attack_b0,		// 武器1
		Attack_b1,
		Attack_b2,
		Attack_c0,		// 武器2
		Attack_c1,
		Attack_c2,
		Attack_d0,		// 武器3
		Attack_d1,
		Attack_d2,
		Attack_e0,		// 武器4
		Attack_e1,
		Attack_e2,
		Attack_f0,		// 武器5
		Attack_f1,
		Attack_f2,
		Attack_g0,		// 武器6
		Attack_g1,
		Attack_g2,
		Attack_h0,		// 武器7
		Attack_h1,
		Attack_h2,
		Attack_i0,		// 武器8
		Attack_i1,
		Attack_i2,
		Attack_j0,		// 武器9
		Attack_j1,
		Attack_j2,

		// 技能攻击 空手和持有武器
		SkillAttack_a0,
		SkillAttack_a1,
		SkillAttack_a2,
		SkillAttack_b0,
		SkillAttack_b1,
		SkillAttack_b2,
		SkillAttack_c0,
		SkillAttack_c1,
		SkillAttack_c2,
		SkillAttack_d0,
		SkillAttack_d1,
		SkillAttack_d2,
		SkillAttack_e0,
		SkillAttack_e1,
		SkillAttack_e2,
		SkillAttack_f0,
		SkillAttack_f1,
		SkillAttack_f2,
		SkillAttack_g0,
		SkillAttack_g1,
		SkillAttack_g2,
		SkillAttack_h0,
		SkillAttack_h1,
		SkillAttack_h2,
		SkillAttack_i0,
		SkillAttack_i1,
		SkillAttack_i2,
		SkillAttack_j0,
		SkillAttack_j1,
		SkillAttack_j2,

		// 重击动作
		Critical_a,
		Critical_b,
		Critical_c,
		Critical_d,
		Critical_e,
		Critical_f,
		Critical_g,
		Critical_h,
		Critical_i,
		Critical_j,

		// 致命攻击动作
		Fatal_a,
		Fatal_b,
		Fatal_c,
		Fatal_d,
		Fatal_e,
		Fatal_f,
		Fatal_g,
		Fatal_h,
		Fatal_i,
		Fatal_j,

		// 施法动作
		// 物理系空手施法
		ReadySpell_a0,			// 施法准备
		// 法术系空手施法
		ReadySpell_a1,
		// 其他持器施法
		ReadySpell_b0,
		ReadySpell_c0,
		ReadySpell_d0,
		ReadySpell_e0,
		ReadySpell_f0,
		ReadySpell_g0,
		ReadySpell_h0,
		ReadySpell_i0,
		ReadySpell_j0,

		SpellCast_a0,			// 施法释放
		SpellCast_a1,
		SpellCast_b0,
		SpellCast_c0,
		SpellCast_d0,
		SpellCast_e0,
		SpellCast_f0,
		SpellCast_g0,
		SpellCast_h0,
		SpellCast_i0,
		SpellCast_j0,

		ChannelCast_a0,			// 施法持续
		ChannelCast_a1,
		ChannelCast_b0,
		ChannelCast_c0,
		ChannelCast_d0,
		ChannelCast_e0,
		ChannelCast_f0,
		ChannelCast_g0,
		ChannelCast_h0,
		ChannelCast_i0,
		ChannelCast_j0,

		// 冲撞
		ChargeCast,
		ChargeLoop,
		// 被击
		Wound,
		// 击退
		BeatBack,
		// 击飞
		BeatFly,
		// 击倒
		KnockDownStart,
		KnockDownLoop,
		KnockDownEnd,
		// 晕眩
		Stun,
		// 恐惧
		Fear,

		// --------------------- 死亡动作 ---------------------
		Death,

		// --------------------- 生活动作 ---------------------
		// 制造
		Produce,
		// 开采
		Mine,
		// 砍伐
		Cut,
		// 钓鱼
		FishingCast,
		FishingLoop,
		FishingEnd,
		// 打坐
		Sit,

		// --------------------- 交际动作 ---------------------
		// 背&被背
		Carry,
		Carried,
		// 抱&被抱
		Hold,
		Held,

		// --------------------- 表情动作 ---------------------
		Emote_00,
		Emote_01,
		Emote_02,
		Emote_03,
		Emote_04,
		Emote_05,
		Emote_06,
		Emote_07,
		Emote_08,
		Emote_09,
		Emote_10,
		Emote_11,
		Emote_12,
		Emote_13,
		Emote_14,
		Emote_15,
		Emote_16,
		Emote_17,
		Emote_18,
		Emote_19,
		Emote_20,
		Emote_21,
		Emote_22,
		Emote_23,
		Emote_24,
		Emote_25,
		Emote_26,
		Emote_27,
		Emote_28,
		Emote_29,

		// --------------------- 补充 ---------------------
		Mount_a,
		Mount_b,
		Mount_c,
		Mount_d,
		Mount_e,
		Mount_f,
		Mount_g,
		Mount_h,
		Mount_i,
		Mount_j,

		MountIdle_a0,
		MountIdle_a1,
		MountIdle_a2,
		MountIdle_b0,
		MountIdle_b1,
		MountIdle_b2,
		MountIdle_c0,
		MountIdle_c1,
		MountIdle_c2,
		MountIdle_d0,
		MountIdle_d1,
		MountIdle_d2,
		MountIdle_e0,
		MountIdle_e1,
		MountIdle_e2,
		MountIdle_f0,
		MountIdle_f1,
		MountIdle_f2,
		MountIdle_g0,
		MountIdle_g1,
		MountIdle_g2,
		MountIdle_h0,
		MountIdle_h1,
		MountIdle_h2,
		MountIdle_i0,
		MountIdle_i1,
		MountIdle_i2,
		MountIdle_j0,
		MountIdle_j1,
		MountIdle_j2,

		Gather,
		Hunt,
		Plant,

		// ----------------------------------------------------
		AnimsEnd,

		LowerActionAnimsBegin = Run_a,
		LowerActionAnimsEnd = Fall + 1,
		NumMoveActionAnims = Skip + 1,
		NumTableActionAnims = AnimsEnd,
		NumExtraActionAnims = 512 - NumTableActionAnims,
		NumActionAnims = NumTableActionAnims + NumExtraActionAnims,
		ActionAnimBits = 9,
		NullAnimation = (1 << ActionAnimBits) - 1
	};

	static ActionAnimationDef ActionAnimationList[NumTableActionAnims];
	static U32 AnimSetMask[Group_Max];
	ActionAnimation actionList[NumActionAnims];
	U32 actionCount;
	S32 spineNode[NumSpineNodes];
	S32 pickupDelta;           ///< Base off of pcikupRadius
	F32 runSurfaceCos;         ///< Angle from vertical in cos(runSurfaceAngle)
	F32 jumpSurfaceCos;        ///< Angle from vertical in cos(jumpSurfaceAngle)

	enum ArmStatus
	{
		Arm_A = 0,		// 空手状态
		Arm_B,			// 法器
		Arm_C,			// 单短
		Arm_D,			// 双短
		Arm_E,			// 琴
		Arm_F,			// 弓
		Arm_G,			// 旗帜
		Arm_H,			// 刀斧
		Arm_I,			// unknown
		Arm_J,			// unknown
	};
	ArmStatus armStatus;

	enum Impacts {
		ImpactNone,
		ImpactNormal,
	};

	enum Recoil {
		LightRecoil,
		MediumRecoil,
		HeavyRecoil,
		NumRecoilSequences
	};
	S32 recoilSequence[NumRecoilSequences];

	/// @name Particles
	/// All of the data relating to environmental effects
	/// @{

	ParticleEmitterData * footPuffEmitter;
	S32 footPuffID;
	S32 footPuffNumParts;
	F32 footPuffRadius;

	DecalData* decalData;
	S32 decalID;

	ParticleEmitterData * dustEmitter;
	S32 dustID;

	SplashData* splash;
	S32 splashId;
	F32 splashVelocity;
	F32 splashAngle;
	F32 splashFreqMod;
	F32 splashVelEpsilon;
	F32 bubbleEmitTime;

	F32 medSplashSoundVel;
	F32 hardSplashSoundVel;
	F32 exitSplashSoundVel;
	F32 footSplashHeight;

	// Air control
	F32 airControl;

	// Jump off surfaces at their normal rather than straight up
	bool jumpTowardsNormal;

	ParticleEmitterData* splashEmitterList[NUM_SPLASH_EMITTERS];
	S32 splashEmitterIDList[NUM_SPLASH_EMITTERS];
	/// @}

	// AI
	U32 aiChar;
	U32 aiSpell;

	//
	DECLARE_CONOBJECT(GameObjectData);
	GameObjectData();
	bool preload(bool server, char errorBuffer[256]);
	bool isTableSequence(S32 seq);
	bool isJumpAction(U32 action);
	static void selectAction(U32 newAct, U32 oldUpper, U32 oldLower, U32& newUpper, U32& newLower);
	static inline bool getActionFlags(U32 action, U32 flags) { return ActionAnimationList[action].flags & flags;}

	static void initPersistFields();
	virtual void packData(BitStream* stream);
	virtual void unpackData(BitStream* stream);
	virtual bool initDataBlock();
};

