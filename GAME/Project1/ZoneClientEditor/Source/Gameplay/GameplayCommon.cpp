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
//  ȫ�ֳ��� & ����
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
	30008,	// ��
	30000,	// ʥ
	30005,	// ��
	30002,	// ��
	30003,	// ��
	30004,	// ��
	30001,	// ��
	30006,	// ��
	30007,	// ħ
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
	0xFFFFFF,				// ��ɫ
	0x000000,				// ��ɫ
	0xFF0000, 				// ��ɫ
	0x00FF00,				// ��ɫ
	0x0000FF,				// ��ɫ
	0xFFFF00,				// ��ɫ
	0xFF00FF,				// ��ɫ
	0x00FFFF,				// ��ɫ
	0x808080,				// ��ɫ
	0x800000,				// ����
	0x008000,				// ����
	0x000080,				// ����
	0x808000,				// ����
	0x800080,				// ����
	0x008080,				// ����
	0xFF0080,				// Ʒ��ɫ
	0x80FF00,				// ����ɫ
	0x0080FF,				// ����ɫ
	0xFF8000,				// �Ȼ�ɫ
	0x8000FF,				// ����ɫ
	0x00FF80,				// ����ɫ
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
	//			�ޣ����Ĭ��		ʥ��������			��������			�ɣ�������			�����ɻ���			�����Ľ�			�֣�ɽ����			�������鹬			ħ����ħ��			NPCĬ��				��					����				����				����				Ұ��				����				EX1 �����			EX2	���ľ�			EX3 ��ۡ
	/*��*/		{Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*ʥ*/		{Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*��*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*��*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*��*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*��*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*��*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*��*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*ħ*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Top/*self*/,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Amity		},

	/*NPC*/		{Fame_Amity,		Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Top/*self*/,	Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity		},
	/*��*/		{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Top/*self*/,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality	},
	/*����*/	{Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry	},
	/*����*/	{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality	},
	/*����*/	{Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry	},
	/*Ұ��*/	{Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality,	Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Neutrality,	Fame_Neutrality,	Fame_Neutrality	},
	/*����*/	{Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Amity,			Fame_Rivalry,		Fame_Top/*self*/,	Fame_Rivalry,		Fame_Rivalry,		Fame_Rivalry	},
	/*Ex1*/		{Fame_Amity,		Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Top/*self*/,	Fame_Amity,			Fame_Amity		},
	/*Ex2*/		{Fame_Amity,		Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Top/*self*/,	Fame_Amity		},
	/*Ex3*/		{Fame_Amity,		Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Amity,			Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Neutrality,	Fame_Rivalry,		Fame_Amity,			Fame_Amity,			Fame_Top/*self*/},
};

// ========================================================================================================================================
//  ȫ�ֺ���
// ========================================================================================================================================

inline bool isNullString(StringTableEntry string)
{
	static StringTableEntry zeroString = StringTable->insert("0");
	static StringTableEntry nullString = StringTable->insert("��");
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
// ��ȡ״̬�����ı�
void GetStatsDesc(Stats* stats, char* color, char buff[1024])
{
	if(!stats || !buff || !color || *color == 0)
		return;
	char szDesc[128] = {0};
	OUTPUTSTATS(stats->HP,					"����",						color)
	//OUTPUTSTATS(stats->HP_Pc,				"�����ٷֱ�",				color)
	OUTPUTSTATS(stats->MP,					"����",						color)
	//OUTPUTSTATS(stats->MP_Pc,				"�����ٷֱ�",				color)
	OUTPUTSTATS(stats->PP,					"Ԫ��",						color)
	//OUTPUTSTATS(stats->PP_Pc,				"Ԫ���ٷֱ�",				color)
	OUTPUTSTATS(stats->MaxHP,				"�������",					color)
	//OUTPUTSTATS(stats->MaxHP_Pc,			"��������ٷֱ�",			color)
	OUTPUTSTATS(stats->MaxMP,				"�����",					color)
	//OUTPUTSTATS(stats->MaxMP_Pc,			"��������ٷֱ�",			color)
	OUTPUTSTATS(stats->MaxPP,				"���Ԫ��",					color)
	//OUTPUTSTATS(stats->MaxPP_Pc,			"���Ԫ���ٷֱ�",			color)
	OUTPUTSTATS(stats->Stamina,				"����",						color)
	//OUTPUTSTATS(stats->Stamina_Pc,			"���ǰٷֱ�",				color)
	OUTPUTSTATS(stats->Mana,				"����",						color)
	//OUTPUTSTATS(stats->Mana_Pc,				"�����ٷֱ�",				color)
	OUTPUTSTATS(stats->Strength,			"����",						color)
	//OUTPUTSTATS(stats->Strength_Pc,			"�����ٷֱ�",				color)
	OUTPUTSTATS(stats->Intellect,			"����",						color)
	//OUTPUTSTATS(stats->Intellect_Pc,		"�����ٷֱ�",				color)
	OUTPUTSTATS(stats->Agility,				"����",						color)
	//OUTPUTSTATS(stats->Agility_Pc,			"���ݰٷֱ�",				color)
	OUTPUTSTATS(stats->Pneuma,				"Ԫ��",						color)
	//OUTPUTSTATS(stats->Pneuma_Pc,			"Ԫ���ٷֱ�",				color)
	OUTPUTSTATS(stats->Insight,				"����",						color)
	//OUTPUTSTATS(stats->Insight_Pc,			"���԰ٷֱ�",				color)
	OUTPUTSTATS(stats->Luck,				"��Ե",						color)
	//OUTPUTSTATS(stats->Luck_Pc,				"��Ե�ٷֱ�",				color)
	OUTPUTSTATS(stats->Velocity_gPc,		"�ƶ��ٶ�",					color)
	//OUTPUTSTATS(stats->MountedVelocity_gPc,	"����ٶ�ȫ�ְٷֱ�",		color)
	//OUTPUTSTATS(stats->AttackSpeed,			"�����ٶ�ʱ����",			color)
	OUTPUTSTATS(stats->AttackSpeed_gPc,		"�����ٶ�",					color)
	//OUTPUTSTATS(stats->SpellSpeed_gPc,		"ʩ���ٶ�ȫ�ְٷֱ�",		color)
	OUTPUTSTATS(stats->CriticalRating,		"����",						color)
	OUTPUTSTATS(stats->CriticalTimes,		"��������",					color)
	//OUTPUTSTATS(stats->Critical_gPc,		"�ػ�ȫ�ְٷֱȼ���",		color)
	OUTPUTSTATS(stats->AntiCriticalRating,	"����",						color)
	OUTPUTSTATS(stats->FatalRating,			"�����ȼ�",					color)
	//OUTPUTSTATS(stats->Fatal_gPc,			"����ȫ�ְٷֱȼ���",		color)
	OUTPUTSTATS(stats->DodgeRating,			"����",						color)
	//OUTPUTSTATS(stats->Dodge_gPc,			"����ȫ�ְٷֱȼ���",		color)
	OUTPUTSTATS(stats->HitRating,			"����",						color)
	//OUTPUTSTATS(stats->Hit_gPc,				"����ȫ�ְٷֱȼ���",		color)
	OUTPUTSTATS(stats->PhyDamage,			"������",					color)
	//OUTPUTSTATS(stats->PhyDamage_Pc,		"�����˺��ٷֱ�",			color)
	OUTPUTSTATS(stats->PhyDefence,			"�������",					color)
	//OUTPUTSTATS(stats->PhyDefence_Pc,		"��������ٷֱ�",			color)
	OUTPUTSTATS(stats->MuDamage,			"ľ���˺�",					color)
	//OUTPUTSTATS(stats->MuDamage_Pc,			"ľ�˺��ٷֱ�",				color)
	OUTPUTSTATS(stats->MuDefence,			"ľ�鿹��",					color)
	//OUTPUTSTATS(stats->MuDefence_Pc,		"ľ�����ٷֱ�",				color)
	OUTPUTSTATS(stats->HuoDamage,			"�����˺�",					color)
	//OUTPUTSTATS(stats->HuoDamage_Pc,		"���˺��ٷֱ�",				color)
	OUTPUTSTATS(stats->HuoDefence,			"���鿹��",					color)
	//OUTPUTSTATS(stats->HuoDefence_Pc,		"������ٷֱ�",				color)
	OUTPUTSTATS(stats->TuDamage,			"�����˺�",					color)
	//OUTPUTSTATS(stats->TuDamage_Pc,			"���˺��ٷֱ�",				color)
	OUTPUTSTATS(stats->TuDefence,			"���鿹��",					color)
	//OUTPUTSTATS(stats->TuDefence_Pc,		"�������ٷֱ�",				color)
	OUTPUTSTATS(stats->JinDamage,			"�����˺�",					color)
	//OUTPUTSTATS(stats->JinDamage_Pc,		"���˺��ٷֱ�",				color)
	OUTPUTSTATS(stats->JinDefence,			"���鿹��",					color)
	//OUTPUTSTATS(stats->JinDefence_Pc,		"������ٷֱ�",				color)
	OUTPUTSTATS(stats->ShuiDamage,			"ˮ���˺�",					color)
	//OUTPUTSTATS(stats->ShuiDamage_Pc,		"ˮ�˺��ٷֱ�",				color)
	OUTPUTSTATS(stats->ShuiDefence,			"ˮ�鿹��",					color)
	//OUTPUTSTATS(stats->ShuiDefence_Pc,		"ˮ�����ٷֱ�",				color)
	OUTPUTSTATS(stats->PneumaDamage,		"Ԫ���˺�",					color);
	OUTPUTSTATS(stats->PneumaDefence,		"Ԫ������",					color);
	OUTPUTSTATS(stats->Heal,				"����",						color);
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
