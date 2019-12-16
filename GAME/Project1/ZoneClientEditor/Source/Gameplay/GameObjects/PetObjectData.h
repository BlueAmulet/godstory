//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/GameObjectData.h"
#include "Gameplay/GameplayCommon.h"

class IColumnData;
// ========================================================================================================================================
//	PetObjectData
//	PetObjectData 保存了Pet的模板信息
// ========================================================================================================================================
struct PetObjectData: public GameObjectData
{
	typedef GameObjectData Parent;

	enum Flags
	{
		Flags_Mutant			= BIT(0),	// 变异
	};

	enum Ability
	{
		Ability_Combative		= BIT(0),	// 战斗
		Ability_Combinative		= BIT(1),	// 合体
		Ability_Generative		= BIT(2),	// 繁殖
	};

	enum PetIndex
	{
		Combat_A				= 0,		// data中只填战斗类型
		Combat_B,
		Combat_C,
		Mount,
		PetMax,
		CombatPetMax			= Combat_C,
	};

	enum InitTemplateRandom	//初始摸板随机区间
	{
		PingYong_1		= 1,						//宠物摸板-平庸1
		PingYong_2		= PingYong_1	<< 1,		//宠物摸板-平庸50
		PingYong_3		= PingYong_1	<< 2,		//宠物摸板-平庸85
		PingYong_4		= PingYong_1	<< 3,		//宠物摸板-平庸95

		PingFaGong_1	= PingYong_4	<< 1,		//宠物摸板-法功1
		PingFaGong_2	= PingYong_4	<< 2,		//宠物摸板-法功50
		PingFaGong_3	= PingYong_4	<< 3,		//宠物摸板-法功50
		PingFaGong_4	= PingYong_4	<< 4,		//宠物摸板-法功50
		PingFaGong_5	= PingYong_4	<< 5,		//宠物摸板-法功85
		PingFaGong_6	= PingYong_4	<< 6,		//宠物摸板-法功85
		PingFaGong_7	= PingYong_4	<< 7,		//宠物摸板-法功85
		PingFaGong_8	= PingYong_4	<< 8,		//宠物摸板-法功95

		PingWuGong_1	= PingFaGong_8	<< 1,		//宠物摸板-物功1
		PingWuGong_2	= PingFaGong_8	<< 2,		//宠物摸板-物功50
		PingWuGong_3	= PingFaGong_8	<< 3,		//宠物摸板-物功50
		PingWuGong_4	= PingFaGong_8	<< 4,		//宠物摸板-物功50
		PingWuGong_5	= PingFaGong_8	<< 5,		//宠物摸板-物功85
		PingWuGong_6	= PingFaGong_8	<< 6,		//宠物摸板-物功85
		PingWuGong_7	= PingFaGong_8	<< 7,		//宠物摸板-物功85
		PingWuGong_8	= PingFaGong_8	<< 8,		//宠物摸板-物功95

		PingFangChong_1	= PingWuGong_8	<< 1,		//宠物摸板-防宠1
		PingFangChong_2	= PingWuGong_8	<< 2,		//宠物摸板-防宠50
		PingFangChong_3	= PingWuGong_8	<< 3,		//宠物摸板-防宠50
		PingFangChong_4	= PingWuGong_8	<< 4,		//宠物摸板-防宠50
		PingFangChong_5	= PingWuGong_8	<< 5,		//宠物摸板-防宠85
		PingFangChong_6	= PingWuGong_8	<< 6,		//宠物摸板-防宠85
		PingFangChong_7	= PingWuGong_8	<< 7,		//宠物摸板-防宠85
		PingFangChong_8	= PingWuGong_8	<< 8,		//宠物摸板-防宠95
	};

	StringTableEntry	petName;			// 宠物名字
	StringTableEntry	petShortcutIcon;	// 槽中显示的图标
	StringTableEntry	petIcon;			// 宠物头像
	enRace				race;				// 种族
	U32					flags;				// Pet属性标志
	U32					ability;			// 能力
	U32					lives;				// 寿命（最大）
	U32					combatPetIdx;		// 战斗宠类型
	U32					buffId;				// 基础数值
	U32					defaultSkillId;		// 基本攻击
	U32					spawnLevel;			// 携带等级
	U32					TameLevel;			// 捕捉等级
	U32					insight;			// 灵慧
	U32					petTalent;			// 根骨
	
	U32					initialTemplate;	// 初始摸板
	U16					minStrength;		// 力量资质最小值
	U16					maxStrength;		// 力量资质最大值
	U16					minLingLi;			// 灵力资质最小值
	U16					maxLingLi;			// 灵力资质最大值
	U16					minTiPo;			// 体魄资质最小值
	U16					maxTiPo;			// 体魄资质最大值
	U16					minJingLi;			// 精力资质最小值
	U16					maxJingLi;			// 精力资质最大值
	U16					minYuanLi;			// 元力资质最小值
	U16					maxYuanLi;			// 元力资质最大值
	U16					minMinJie;			// 敏捷资质最小值
	U16					maxMinJie;			// 敏捷资质最大值
	U16					minQianLi;			// 潜力资质最小值
	U16					maxQianLi;			// 潜力资质最大值
	//
	DECLARE_CONOBJECT(PetObjectData);
	PetObjectData();
	~PetObjectData();
	bool preload(bool server, char errorBuffer[256]);
	bool initDataBlock();

	U32	getRandBuffId();
};


// ========================================================================================================================================
//	PetRepository
//	PetRepository保存了所有PetObjectData的信息
// ========================================================================================================================================
class PetRepository
{
public:
	PetRepository();
	~PetRepository();
	typedef stdext::hash_map<U32, PetObjectData*> PetDataMap;
	PetObjectData*			GetPetData(U32 PetID);
	U32						GetPetIdByIndex(int index);
	S32						GetPetDataCount();

	bool					Insert(PetObjectData* pData);
	bool					Read();
	void					Clear();

	IColumnData*			mColumnData;
private:
	PetDataMap				m_PetDataMap;
};

extern PetRepository g_PetRepository;
