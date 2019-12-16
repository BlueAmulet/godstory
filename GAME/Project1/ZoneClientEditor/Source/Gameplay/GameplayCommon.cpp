//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platformStrings.h"
#include "platform/platformAssert.h"
#include "Gameplay/GameplayCommon.h"
#include "math/mMathFn.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "Gameplay/GameObjects/Stats.h"


// ========================================================================================================================================
//  全局常量 & 变量
// ========================================================================================================================================

const F32 GAMEPLAY_PERCENT_SCALE = 10000.0f;
const F32 MAX_GAMEPLAY_PERCENT_SCALE = 30000.0f;
const F32 MIN_GAMEPLAY_PERCENT_SCALE = 1000.0f;
const F32 MAX_GAMEPLAY_PERCENT = MAX_GAMEPLAY_PERCENT_SCALE/GAMEPLAY_PERCENT_SCALE;
const F32 MIN_GAMEPLAY_PERCENT = MIN_GAMEPLAY_PERCENT_SCALE/GAMEPLAY_PERCENT_SCALE;
const F32 MAX_FORWARD_SPEED = 4.5f;
const F32 MAX_WALK_SPEED = 1.5f;
const U32 MAX_ATTACK_SPEED = 800;
const U32 MIN_ATTACK_SPEED = 2500;
const U32 MAX_PET_GIFT = 1200;
const U32 MIN_PET_GIFT = 701;
const U32 MAX_PET_QUALITY = 5;
const U32 PET_STATSPOINTS_PERLEVEL = 5;

const U32 g_FamilyBuff[MaxFamilies] = 
{
	30008,	// 无
	30000,	// 圣
	30005,	// 佛
	30002,	// 仙
	30003,	// 精
	30004,	// 鬼
	30001,	// 怪
	30006,	// 妖
	30007,	// 魔
};

const S32 g_FameMax[MaxFame] = 
{
	100000,			// bottom
	100000,
	10000,
	5000,
	10000,
	100000,
	100000,
	100000,			// top
};

const S32 g_LevelExp[MaxPlayerLevel + 1] = 
{
	1,
	55,
	120,
	130,
	225,
	320,
	450,
	570,
	840,
	1150,
	1430,
	1680,
	2170,
	2720,
	3145,
	3895,
	4950,
	6000,
	7425,
	9000,
	10540,
	12800,
	15075,
	17940,
	21300,
	25530,
	30020,
	35945,
	42640,
	51000,
	58960,
	67795,
	77550,
	89240,
	103525,
	118560,
	137160,
	158480,
	182120,
	210000,
	228750,
	251250,
	275840,
	300300,
	329175,
	361080,
	392610,
	429345,
	470160,
	514500,
	566250,
	623475,
	686400,
	755250,
	835375,
	917980,
	1008085,
	1114120,
	1223200,
	1350000,
	1480470,
	1614125,
	1770020,
	1939605,
	2114315,
	2315740,
	2536255,
	2777500,
	3040150,
	3328000,
	3640805,
	3981470,
	4353020,
	4758600,
	5201475,
	5709050,
	6237600,
	6812920,
	7472610,
	8160000,
	8921675,
	9750000,
	10650075,
	11629800,
	12742730,
	13903655,
	15221150,
	16595700,
	18151200,
	19845000,
	21783000,
	23900130,
	26212680,
	28828030,
	31593250,
	34717240,
	38130720,
	41864270,
	45942950,
	50400000,
	55110450,
	60245400,
	65841930,
	71938560,
	78784545,
	86035855,
	94166340,
	103035645,
	112435710,
	122960000,
	134415970,
	146900820,
	160863280,
	175709640,
	192299235,
	209940120,
	229631920,
	251099200,
	274499100,
	300000000,
	328013060,
};

const S32 g_PetLevelExp[MAX_PET_LEVEL] = {  0, 9, 12, 17, 25, 35, 50, 68, 98, 138, 174, 209, 259, 317, 385, 476,583, 722, 887,
									1080, 1282, 1520, 1828, 2162, 2556, 3060, 3610, 4311, 5140, 6120, 7052, 8116, 9331, 
									10717, 12419, 14233, 16450, 18987, 21887, 25200, 27459, 30153, 33093, 36023, 39499,
									43289, 47078, 51554, 56430, 61740, 67989, 74841, 82352, 90582, 100215, 110142, 121013,
									133687, 146776, 162000, 177664, 193743, 212382, 232755, 253705, 277929, 304394, 333304,
									364879, 399360, 436837, 477735, 522358, 571041, 624145, 685082, 748480, 817606, 896753,
									979200, 1070554, 1169944, 1278050, 1395608, 1529165, 1668493, 1826490, 1991449, 2178186,
									2381400, 2613898, 2867948, 3145480, 3459437, 3791233, 4165985, 4575744, 5023663, 5513172,
									6048000, 6613287, 7229466, 7901000, 8632737, 9454118, 10324327, 11300031, 12364230, 13492372,
									14755200, 16129906, 17628039, 19303679, 21085250, 23075802, 25192835, 27555829, 30131948, 
									32939900, 36000000, 39361540, 43026295, 47020951, 51374503, 56118446, 61286993, 67037657,
									73179250, 80006869, 87450000, 96258207, 105929113, 116736556, 128405065, 141434649, 155503772,
									171202290, 188440865, 207677419, 228480000, 251021970, 275336118, 302381774, 332018001, 364488336,
									400600228, 439606496, 482959664, 529790233, 581808000, 640058630, 704881456, 775146871, 852261723,
									938000499, 1032153533, 1135531467, 1249021935, 1373596789, 1512000000, 1643929008 };

const S32 g_SpiritLevelExp[MAX_SPIRIT_LEVEL] = { 55, 250, 545, 1020, 1990, 3110, 4890, 7040, 10950, 16425, 23340, 33015, 46830,
									65965, 93640, 126755, 166790, 222085, 295640, 392120, 480000, 576140, 690255, 821955, 984660,
									1189725, 1441650, 1753355, 2122205, 2573200, 3094595, 3709625, 4430055, 5313755, 6368150,
									7622275, 9111620, 10910525, 13050520, 15632610, 18671675, 22279875, 26646385, 31816850, 37996200,
									45683130, 55040710, 66310490, 79994990, 96342950, 115355850, 137780490, 164820400, 197201985,
									235395710, 281316790, 336572920, 402239355, 480731120, 574499100 };

const U32 g_LivingLevelExp[MaxPlayerLevel] =
{
	1,
	10,
	100,
	200,
	300,
};

U32 g_Color[] = 
{
	0xFFFFFF,				// 白色
	0x000000,				// 黑色
	0xFF0000, 				// 红色
	0x00FF00,				// 绿色
	0x0000FF,				// 蓝色
	0xFFFF00,				// 黄色
	0xFF00FF,				// 紫色
	0x00FFFF,				// 青色
	0x808080,				// 灰色
	0x800000,				// 暗红
	0x008000,				// 暗绿
	0x000080,				// 暗蓝
	0x808000,				// 暗黄
	0x800080,				// 暗紫
	0x008080,				// 暗青
	0xFF0080,				// 品红色
	0x80FF00,				// 亮绿色
	0x0080FF,				// 湖蓝色
	0xFF8000,				// 橙黄色
	0x8000FF,				// 褐紫色
	0x00FF80,				// 新绿色
	0,
	0,
	0
};

F32 g_PetQualityFactor[] = 
{
	0.9f,
	0.95f,
	1.0f,
	1.05f,
	1.1f,
	1.15f,
};

U32 g_PetGrowthVal[] = 
{
	0,
	42,
	78,
	111,
	143,
	165,
};

F32 g_PetGrowthFactor[] = 
{
	1.0f,
	1.32f,
	1.74f,
	2.3f,
	3.03f,
	4.0f,
};

const enFame g_FameRelationship[TotalInfluence][TotalInfluence] = 
{
	//			无，玩家默认		圣，昆仑宗			佛，雷音寺			仙，蓬莱派			精，飞花谷			鬼，九幽教			怪，山海宗			妖，幻灵宫			魔，天魔门			NPC默认				人					恶人				动物				怪物				野兽				凶兽				EX1 万灵城			EX2	月幽境			EX3 神邸
	/*无*/		{Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*圣*/		{Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*佛*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*仙*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*精*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*鬼*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*怪*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*妖*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*魔*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},

	/*NPC*/		{Fame_Amity,		Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Top/*self*/,	Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*人*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Top/*self*/,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality	},
	/*恶人*/	{Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry	},
	/*动物*/	{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality	},
	/*怪物*/	{Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry	},
	/*野兽*/	{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality	},
	/*凶兽*/	{Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry	},
	/*Ex1*/		{Fame_Amity,		Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Top/*self*/,	Fame_Amity,			Fame_Amity		},
	/*Ex2*/		{Fame_Amity,		Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Top/*self*/,	Fame_Amity		},
	/*Ex3*/		{Fame_Amity,		Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Top/*self*/},
};

// ========================================================================================================================================
//  全局函数
// ========================================================================================================================================

inline bool isNullString(StringTableEntry string)
{
	static StringTableEntry zeroString = StringTable->insert("0");
	static StringTableEntry nullString = StringTable->insert("无");
	return !(string && string[0] && string != zeroString && string != nullString);
}

inline StringTableEntry filterNullString(StringTableEntry string)
{
	static StringTableEntry zeroString = StringTable->insert("0");
	string = StringTable->insert(string);
	return (string && string[0] && string != zeroString) ? string : NULL;
}

inline F32 getDamageReducePhy(F32 val)
{
	const static F32 q = 1.036f;
	const static F32 a = 100.0f;
	return mClampF(mLog(1.0f - (val * (1 - q)/a))/mLog(q) * 0.01f, 0.0f, 0.65f);
}

inline F32 getDamageReduceSpl(F32 val)
{
	const static F32 q = 1.069f;
	const static F32 a = 80.0f;
	return mClampF(mLog(1.0f - (val * (1 - q)/a))/mLog(q) * 0.01f, 0.0f, 0.50f);
}

inline F32 getDamageReducePneuma(F32 val)
{
	return mClampF(val * 0.01f, 0.0f, 0.95f);
}

inline F32 getCriticalRate(F32 val)
{
	const static F32 q = 1.0266f;
	const static F32 a = 25.0f;
	return mLog(1.0f - (val * (1 - q)/a))/mLog(q) * 0.01f;
}

inline F32 getAntiCriticalRate(F32 val)
{
	const static F32 q = 1.0249f;
	const static F32 a = 25.0f;
	return mLog(1.0f - (val * (1 - q)/a))/mLog(q) * 0.01f;
}

inline F32 getDodgeRate(F32 val)
{
	const static F32 q = 1.083f;
	const static F32 a = 255.0f;
	return mLog(1.0f - (val * (1 - q)/a))/mLog(q) * 0.01f;
}

inline F32 getPetGrowthFactor(U32 val)
{
	for (S32 i=MAX_PET_QUALITY; i>=0; --i)
	{
		if(val > g_PetGrowthVal[i])
			return g_PetGrowthFactor[i];
	}
	return g_PetGrowthFactor[0];
}

void InitScriptVariable()
{
	char varName[20];
	for(int i = 0; i < sizeof(g_Color) / sizeof(U32); i++)
	{
		dSprintf(varName, sizeof(varName), "$Color%d", i);
		Con::addVariable(varName,	TypeS32,	&g_Color[i]);
	}	
}

#define OUTPUTSTATS(a,b,c) if(a > 0) {\
	dSprintf(szDesc, sizeof(szDesc), "<t v='1' c='%s'>%s%+d</t><b/>", c, b, a); \
	dStrcat(buff, 1024,szDesc);}

// ----------------------------------------------------------------------------
// 获取状态描述文本
void GetStatsDesc(Stats* stats, char* color, char buff[1024])
{
	if(!stats || !buff || !color || *color == 0)
		return;
	char szDesc[128] = {0};
	OUTPUTSTATS(stats->HP,					"生命",						color)
	//OUTPUTSTATS(stats->HP_Pc,				"生命百分比",				color)
	OUTPUTSTATS(stats->MP,					"精气",						color)
	//OUTPUTSTATS(stats->MP_Pc,				"真气百分比",				color)
	OUTPUTSTATS(stats->PP,					"元气",						color)
	//OUTPUTSTATS(stats->PP_Pc,				"元气百分比",				color)
	OUTPUTSTATS(stats->MaxHP,				"最大生命",					color)
	//OUTPUTSTATS(stats->MaxHP_Pc,			"最大生命百分比",			color)
	OUTPUTSTATS(stats->MaxMP,				"最大精气",					color)
	//OUTPUTSTATS(stats->MaxMP_Pc,			"最大真气百分比",			color)
	OUTPUTSTATS(stats->MaxPP,				"最大元气",					color)
	//OUTPUTSTATS(stats->MaxPP_Pc,			"最大元气百分比",			color)
	OUTPUTSTATS(stats->Stamina,				"体魄",						color)
	//OUTPUTSTATS(stats->Stamina_Pc,			"体魄百分比",				color)
	OUTPUTSTATS(stats->Mana,				"精力",						color)
	//OUTPUTSTATS(stats->Mana_Pc,				"精力百分比",				color)
	OUTPUTSTATS(stats->Strength,			"力道",						color)
	//OUTPUTSTATS(stats->Strength_Pc,			"力道百分比",				color)
	OUTPUTSTATS(stats->Intellect,			"灵力",						color)
	//OUTPUTSTATS(stats->Intellect_Pc,		"灵力百分比",				color)
	OUTPUTSTATS(stats->Agility,				"敏捷",						color)
	//OUTPUTSTATS(stats->Agility_Pc,			"敏捷百分比",				color)
	OUTPUTSTATS(stats->Pneuma,				"元力",						color)
	//OUTPUTSTATS(stats->Pneuma_Pc,			"元力百分比",				color)
	OUTPUTSTATS(stats->Insight,				"悟性",						color)
	//OUTPUTSTATS(stats->Insight_Pc,			"悟性百分比",				color)
	OUTPUTSTATS(stats->Luck,				"福缘",						color)
	//OUTPUTSTATS(stats->Luck_Pc,				"福缘百分比",				color)
	OUTPUTSTATS(stats->Velocity_gPc,		"移动速度",					color)
	//OUTPUTSTATS(stats->MountedVelocity_gPc,	"骑乘速度全局百分比",		color)
	//OUTPUTSTATS(stats->AttackSpeed,			"攻击速度时间间隔",			color)
	OUTPUTSTATS(stats->AttackSpeed_gPc,		"攻击速度",					color)
	//OUTPUTSTATS(stats->SpellSpeed_gPc,		"施放速度全局百分比",		color)
	OUTPUTSTATS(stats->CriticalRating,		"暴击",						color)
	OUTPUTSTATS(stats->CriticalTimes,		"暴击倍数",					color)
	//OUTPUTSTATS(stats->Critical_gPc,		"重击全局百分比几率",		color)
	OUTPUTSTATS(stats->AntiCriticalRating,	"坚韧",						color)
	OUTPUTSTATS(stats->FatalRating,			"致命等级",					color)
	//OUTPUTSTATS(stats->Fatal_gPc,			"致命全局百分比几率",		color)
	OUTPUTSTATS(stats->DodgeRating,			"闪避",						color)
	//OUTPUTSTATS(stats->Dodge_gPc,			"闪避全局百分比几率",		color)
	OUTPUTSTATS(stats->HitRating,			"命中",						color)
	//OUTPUTSTATS(stats->Hit_gPc,				"命中全局百分比几率",		color)
	OUTPUTSTATS(stats->PhyDamage,			"物理攻击",					color)
	//OUTPUTSTATS(stats->PhyDamage_Pc,		"物理伤害百分比",			color)
	OUTPUTSTATS(stats->PhyDefence,			"物理防御",					color)
	//OUTPUTSTATS(stats->PhyDefence_Pc,		"物理防御百分比",			color)
	OUTPUTSTATS(stats->MuDamage,			"木灵伤害",					color)
	//OUTPUTSTATS(stats->MuDamage_Pc,			"木伤害百分比",				color)
	OUTPUTSTATS(stats->MuDefence,			"木灵抗性",					color)
	//OUTPUTSTATS(stats->MuDefence_Pc,		"木防御百分比",				color)
	OUTPUTSTATS(stats->HuoDamage,			"火灵伤害",					color)
	//OUTPUTSTATS(stats->HuoDamage_Pc,		"火伤害百分比",				color)
	OUTPUTSTATS(stats->HuoDefence,			"火灵抗性",					color)
	//OUTPUTSTATS(stats->HuoDefence_Pc,		"火防御百分比",				color)
	OUTPUTSTATS(stats->TuDamage,			"土灵伤害",					color)
	//OUTPUTSTATS(stats->TuDamage_Pc,			"土伤害百分比",				color)
	OUTPUTSTATS(stats->TuDefence,			"土灵抗性",					color)
	//OUTPUTSTATS(stats->TuDefence_Pc,		"土防御百分比",				color)
	OUTPUTSTATS(stats->JinDamage,			"金灵伤害",					color)
	//OUTPUTSTATS(stats->JinDamage_Pc,		"金伤害百分比",				color)
	OUTPUTSTATS(stats->JinDefence,			"金灵抗性",					color)
	//OUTPUTSTATS(stats->JinDefence_Pc,		"金防御百分比",				color)
	OUTPUTSTATS(stats->ShuiDamage,			"水灵伤害",					color)
	//OUTPUTSTATS(stats->ShuiDamage_Pc,		"水伤害百分比",				color)
	OUTPUTSTATS(stats->ShuiDefence,			"水灵抗性",					color)
	//OUTPUTSTATS(stats->ShuiDefence_Pc,		"水防御百分比",				color)
	OUTPUTSTATS(stats->PneumaDamage,		"元力伤害",					color);
	OUTPUTSTATS(stats->PneumaDefence,		"元力防御",					color);
	OUTPUTSTATS(stats->Heal,				"治疗",						color);
}

void ParsePlayerScript(char buff[1024], StringTableEntry str, U32 id)
{
	if(!str || !str[0])
	{
		if(buff)
			buff[0] = 0;
		return;
	}
	char szId[16];
	dSprintf(szId, sizeof(szId), "%d", id);
	std::string strTemp = str;
	//_strlwr_s((char*)strTemp.c_str(), strTemp.length());
	size_t nIdx;
	while((nIdx = strTemp.find("%player"))!= -1)
		strTemp.replace(nIdx, 7, szId);
	if(((char*)strTemp.c_str())[getMax((S32)strTemp.length()-1, (S32)0)] != ';')
		strTemp += ";";
	dSprintf(buff, strTemp.length()+1, "%s", strTemp.c_str());
}
