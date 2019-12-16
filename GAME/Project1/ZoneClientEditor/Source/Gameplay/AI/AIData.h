//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once
#include <hash_map>
#include "platform/platform.h"


enum AI_ObjectType
{
	AIObjectType_Common,
	AIObjectType_Player,
	AIObjectType_Npc,
	AIObjectType_Pet,
	AIObjectType_Totem,
};

enum AIDefedObject
{
	AIDefedObject_Self				= 0,	// 自身
	AIDefedObject_Target			= 1,	// 目标
	AIDefedObject_TargetTgt			= 2,	// 目标的目标
	AIDefedObject_Master			= 3,	// 主人
	AIDefedObject_MasterTgt			= 4,	// 主人的目标
	AIDefedObject_Pet				= 5,	// 宠物
	AIDefedObject_PetTgt			= 6,	// 宠物的目标
};

//-----------------------------------------------------------------------------
// AI条件数据结构
//-----------------------------------------------------------------------------

class AIBase;
//class PlayerAI;
class NpcAI;
class PetAI;

struct AICondition
{
	typedef bool (AIBase::*FunctionCommon)(S32, S32, S32, S32);
	//typedef bool (PlayerAI::*FunctionPlayer)(S32, S32, S32, S32);
	typedef bool (NpcAI::*FunctionNpc)(S32, S32, S32, S32);
	typedef bool (PetAI::*FunctionPet)(S32, S32, S32, S32);
	//typedef bool (TotemAI::*FunctionTotem)(S32, S32, S32, S32);

	// 通用函数
	enum Func
	{
		Func_GetSex,
		Func_GetRace,
		Func_GetFamily,
		Func_GetLevel,
		Func_GetExp,
		Func_IsCombative,
		Func_IsInCombat,
		Func_CheckBuff,
		Func_GetAIStateTimer,
		Func_IsPlayer,
		Func_IsNpc,
		Func_IsPet,
		Func_IsTotem,

		Func_GetHP,
		Func_GetMP,
		Func_GetPP,
		Func_GetMaxHP,
		Func_GetMaxMP,
		Func_GetMaxPP,
		Func_GetHP_Pc,
		Func_GetMP_Pc,
		Func_GetPP_Pc,

		Func_GetStamina,
		Func_GetMana,
		Func_GetStrength,
		Func_GetIntellect,
		Func_GetAgility,
		Func_GetPneuma,
		Func_GetInsight,
		Func_GetLuck,

		Func_GetVelocity,
		Func_GetAttackSpeed,
		Func_GetSpellSpeed,
		Func_GetCritical_Pc,
		Func_GetAntiCritical_Pc,
		Func_GetFatal_Pc,
		Func_GetDodge_Pc,
		Func_GetHit_Pc,
		Func_GetDamageHandle_Pc,

		Func_GetPhyDamage,
		Func_GetPhyDefence,
		Func_GetMuDamage,
		Func_GetMuDefence,
		Func_GetHuoDamage,
		Func_GetHuoDefence,
		Func_GetTuDamage,
		Func_GetTuDefence,
		Func_GetJinDamage,
		Func_GetJinDefence,
		Func_GetShuiDamage,
		Func_GetShuiDefence,
		Func_GetPneumaDamage,
		Func_GetPneumaDefence,
		Func_GetHeal,

		Func_GetPhyScale,
		Func_GetSplScale,
		Func_GetHealScale,
		Func_GetHateScale,

		Func_GetDeriveHP_Pc,
		Func_GetDeriveMP_Pc,
		Func_GetDerivePP_Pc,

		Func_GetMu,
		Func_GetHuo,
		Func_GetTu,
		Func_GetJin,
		Func_GetShui,

		Func_GetExitInterval,
		MaxFunc,
	};

	// 玩家专用函数
	enum FuncPlayer
	{
		FuncPlayer_,

		MaxFuncPlayer,
	};

	// NPC专用函数
	enum FuncNpc
	{
		FuncNpc_,

		MaxFuncNpc,
	};

	// 宠物专用函数
	enum FuncPet
	{
		FuncPet_,

		MaxFuncPet,
	};

	// 图腾专用函数
	enum FuncTotem
	{
		FuncTotem_,

		MaxFuncTotem,
	};

	enum Cmp 
	{
		Cmp_Equal,
		Cmp_NotEqual,
		Cmp_Less,
		Cmp_LessEqual,
		Cmp_Greater,
		Cmp_GreaterEqual,
	};

	U32 id;
	AI_ObjectType type;
	AIDefedObject object;
	S32 func;
	S32 param1;
	S32 param2;
	Cmp cmp;
	S32 val;

	static FunctionCommon funcCom[MaxFunc];
	//static FunctionPlayer funcPlayer[MaxFuncPlayer];
	static FunctionNpc funcNpc[MaxFuncNpc];
	static FunctionPet funcPet[MaxFuncPet];
	//static FunctionTotem funcTotem[MaxFuncTotem];

	AICondition() { dMemset(this, 0, sizeof(AICondition));}
	void static initFunc();
};

//-----------------------------------------------------------------------------
// AI操作数据结构
//-----------------------------------------------------------------------------

struct AIOperation 
{
	typedef void (AIBase::*FunctionCommon)(S32, S32);
	//typedef void (PlayerAI::*FunctionPlayer)(S32, S32);
	typedef void (NpcAI::*FunctionNpc)(S32, S32);
	typedef void (PetAI::*FunctionPet)(S32, S32);
	//typedef void (TotemAI::*FunctionTotem)(S32, S32);

	// 通用函数
	enum Func
	{
		Func_AddBuff,

		MaxFunc,
	};

	// 玩家专用函数
	enum FuncPlayer
	{
		FuncPlayer_,

		MaxFuncPlayer,
	};

	// NPC专用函数
	enum FuncNpc
	{
		FuncNpc_,

		MaxFuncNpc,
	};

	// 宠物专用函数
	enum FuncPet
	{
		FuncPet_,

		MaxFuncPet,
	};

	// 图腾专用函数
	enum FuncTotem
	{
		FuncTotem_,

		MaxFuncTotem,
	};

	U32 id;
	AI_ObjectType type;
	AIDefedObject object;
	S32 func;
	S32 param1;
	S32 param2;

	static FunctionCommon funcCom[MaxFunc];
	//static FunctionPlayer funcPlayer[MaxFuncPlayer];
	static FunctionNpc funcNpc[MaxFuncNpc];
	static FunctionPet funcPet[MaxFuncPet];
	//static FunctionTotem funcTotem[MaxFuncTotem];

	AIOperation() { dMemset(this, 0, sizeof(AIOperation));}
	void static initFunc();
};

//-----------------------------------------------------------------------------
// AI跳转数据结构
//-----------------------------------------------------------------------------

struct AIGoto
{
	enum
	{
		MaxCondition = 8,
		MaxOperation = 8,
	};

	//U32 charId;
	//StringTableEntry charName;
	//AI_ObjectType type;
	U32 srcState;
	U32 tgtState;
	U32 flagNOT;
	U32 flagOR;
	const AICondition* condition[MaxCondition];
	const AIOperation* operation[MaxOperation];

	AIGoto() { dMemset(this, 0, sizeof(AIGoto));}
};


//-----------------------------------------------------------------------------
// AISpellEntry
//-----------------------------------------------------------------------------

struct AISpellEntry
{
	enum
	{
		MaxCondition = 8,
	};

	U32 id;
	U32 skillId;
	U32 rate;
	U32 flagNOT;
	U32 flagOR;
	const AICondition* condition[MaxCondition];

	AISpellEntry() { dMemset(this, 0, sizeof(AISpellEntry));}
};


//-----------------------------------------------------------------------------
// AISpell
//-----------------------------------------------------------------------------

struct AISpell
{
	enum
	{
		MaxSpells = 12,
	};

	U32 id;
	const AISpellEntry* entry[MaxSpells];

	AISpell() { dMemset(this, 0, sizeof(AISpell));}
	~AISpell();
};

//-----------------------------------------------------------------------------
// AI性格数据结构
//-----------------------------------------------------------------------------

struct AICharacter
{
	enum
	{
		MaxAIStates = 16,
	};

	U32 charId;
	StringTableEntry charName;
	AI_ObjectType type;

	const AIGoto* aiGoto[MaxAIStates][MaxAIStates];

	AICharacter() { dMemset(this, 0, sizeof(AICharacter));}
	~AICharacter();
};

//-----------------------------------------------------------------------------
// AI仓库
//-----------------------------------------------------------------------------

class AIRepository
{
public:
	typedef stdext::hash_map<U32, const AICondition*> AICdMap;
	typedef stdext::hash_map<U32, const AIOperation*> AIOpMap;
	typedef stdext::hash_map<U32, const AICharacter*> AICharMap;
	typedef stdext::hash_map<U32, const AISpell*> AISpellMap;

protected:
	AICdMap mAICdMap;
	AIOpMap mAIOpMap;
	AICharMap mAICharMap;
	AISpellMap mAISpellMap;

public:
	AIRepository();
	~AIRepository();

	void read();
	void clear();

	const AICharacter* getAIChar(U32 id);
	const AISpell* getAISpell(U32 id);
};

extern AIRepository g_AIRepository;
