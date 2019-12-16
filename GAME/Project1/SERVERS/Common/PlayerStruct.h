#ifndef PLAYER_STRUCT_H
#define PLAYER_STRUCT_H

#include <memory.h>
#include "base/bitStream.h"
#include "Common/PacketType.h"
#include "SocialBase.h"
#include <string>

class BitStream;
enum enFamily
{
	Family_None = 0,			// 无，玩家默认
	Family_Sheng,				// 圣，昆仑宗
	Family_Fo,					// 佛，雷音寺
	Family_Xian,				// 仙，蓬莱派
	Family_Jing,				// 精，飞花谷
	Family_Gui,					// 鬼，九幽教
	Family_Guai,				// 怪，山海宗
	Family_Yao,					// 妖，幻灵宫
	Family_Mo,					// 魔，天魔门
	MaxFamilies,
};

static char* g_strFamily[MaxFamilies] = 
{
    "无",
    "昆仑宗",
    "雷音寺",
    "蓬莱派",
    "飞花谷",
    "九幽教",
    "山海宗",
    "幻灵宫",
    "天魔门",
};

enum enInfluence
{
	Influence_None = 0,			// 无，玩家默认
	Influence_Sheng,			// 圣，昆仑宗
	Influence_Fo,				// 佛，雷音寺
	Influence_Xian,				// 仙，蓬莱派
	Influence_Jing,				// 精，飞花谷
	Influence_Gui,				// 鬼，九幽教
	Influence_Guai,				// 怪，山海宗
	Influence_Yao,				// 妖，幻灵宫
	Influence_Mo,				// 魔，天魔门
	PlayerFamilies,

	Influence_Npc = PlayerFamilies,// NPC默认
	Influence_People,			// 人
	Influence_Villain,			// 恶人
	Influence_Animal,			// 动物
	Influence_Monster,			// 怪物
	Influence_WildAnimal,		// 野兽
	Influence_Beast,			// 凶兽

	// other
	Influence_Ex1,
	Influence_Ex2,
	Influence_Ex3,

	TotalInfluence,
	MaxInfluence = 64,			// 最大64个
};

static char* g_strInfluence[MaxInfluence] = 
{
    "无",
    "昆仑宗",
    "雷音寺",
    "蓬莱派",
    "飞花谷",
    "九幽教",
    "山海宗",
    "幻灵宫",
    "天魔门",

	"神仙",
	"人",
	"恶人",
	"动物",
	"怪兽",
	"野兽",
	"凶兽",

	"未定1",
	"未定2",
	"未定3",
};

enum enRace
{
	Race_Sheng = 0,				// 圣
	Race_Fo,					// 佛
	Race_Xian,					// 仙
	Race_Jing,					// 精
	Race_Gui,					// 鬼
	Race_Guai,					// 怪
	Race_Yao,					// 妖
	Race_Mo,					// 魔

	Race_Ren,					// 人
	Race_Shen,					// 神
	Race_YeShou,				// 野兽
	Race_DiRen,					// 氐人
	Race_KuaFu,					// 夸父
	Race_LongBo,				// 龙伯
	Race_WuLing,				// 五灵
	Race_YuRen,					// 羽人
	MaxRaces,
};

static char* g_strRace[MaxRaces] = 
{
	"圣",
	"佛",
	"仙",
	"精",
	"鬼",
	"怪",
	"妖",
	"魔",

	"人",
	"神",
	"野兽",
	"氐人",
	"夸父",
	"龙伯",
	"五灵",
	"羽人",
};

// 主次职业
enum enClassesMaster
{
	_1st,
	_2nd,
	_3rd,
	_4th,
	_5th,
	MaxClassesMasters,
};

// 效果激活(APF)
enum enActivePropertyFlag
{
	EAPF_BIND			= 1 << 0,	// 绑定效果
	EAPF_ATTACH			= 1 << 1,	// 附加效果
	EAPF_WUXING			= 1 << 2,	// 五行效果
	EAPF_IMPRESS		= 1 << 3,	// 装备铭刻属性
	EAPF_EQUIPSKILLA	= 1 << 4,	// 装备后主动技能
	EAPF_EQUIPSKILLB	= 1 << 5,	// 装备后被动技能		
};

// 物品数据结构
struct stItemInfo
{
	char Producer[COMMON_STRING_LENGTH];// 制造者名称
	U64 UID;							// 物品世界唯一ID
	U32 ItemID;							// 物品模板ID
	U32 Quantity;						// 物品数量(对于装备，只能为1)
	U32 BindPlayer;						// 绑定者ID(也可用于存储灵魂链接开启后玩家ID)
	U32 BindFriend;						// 绑定好友角色ID共享使用物品

	U32 ActiveFlag;						// 物品激活标志
	U32	Quality;						// 物品品质等级
	S32 LapseTime;						// 剩余秒数
	S32 RemainUseTimes;					// 剩余使用次数
	U32 LockedLeftTime;					// 物品锁剩余时间（秒）

	S32 CurWear;						// 当前耐久度
	S32 CurMaxWear;						// 当前最大耐久度
	S32 CurAdroit;						// 当前熟练度
	U32 CurAdroitLv;					// 当前熟练度等级

	U32 ActivatePro;					// 附加属性效果激活标志

	U32 RandPropertyID;					// 随机名称属性ID
	U32 IDEProNum;						// 鉴定后附加属性个数
	U32 IDEProValue[MAX_IDEPROS];		// 鉴定后附加属性值

	U32 EquipStrengthens;				// 装备强化过总数
	U32 EquipStrengthenValue[MAX_EQUIPSTENGTHENS][2];

	U32 ImpressID;						// 铭刻ID(状态ID)
	U32 BindProID;						// 灵魂绑定附加属性(状态ID)

	U32 WuXingID;						// 五行ID
	U32 WuXingLinkID;					// 五行链接ID
	U32 WuXingPro;						// 五行效果属性

	U32 SkillAForEquip;					// 装备上获取的主动技能ID
	U32 SkillBForEquip;					// 装备上获取的被动技能ID

	U32 EmbedOpened;					// 开启镶嵌孔（每4bit位代表一个孔）
	U32	EmbedSlot[MAX_EMBEDSLOTS];		// 装备镶嵌孔的宝石ID

	stItemInfo() { InitData();}

	void InitData();

	template<class T>
	bool WriteData(T *stream)
	{
		stream->writeBits(Base::Bit64,			&UID);
		stream->writeInt(ItemID,				Base::Bit32);
		stream->writeInt(Quantity,				Base::Bit8);
		stream->writeInt(LapseTime,				Base::Bit32);
		stream->writeInt(RemainUseTimes,		10);
		stream->writeInt(LockedLeftTime,		Base::Bit32);
		stream->writeInt(BindPlayer,			Base::Bit32);
		stream->writeInt(BindFriend,			Base::Bit32);
		stream->writeString(Producer,			COMMON_STRING_LENGTH);
		stream->writeInt(ActiveFlag,			Base::Bit32);

		// 判断是否装备
		if(ActiveFlag & 0x1)
		{
			stream->writeInt(Quality,			10);
			stream->writeInt(CurWear,			Base::Bit16);
			stream->writeInt(CurMaxWear,		Base::Bit16);
			stream->writeInt(CurAdroit,			Base::Bit32);
			stream->writeInt(CurAdroitLv,		Base::Bit8);

			stream->writeInt(EmbedOpened,		Base::Bit8);
			for(int i = 0; i < MAX_EMBEDSLOTS; i++)
			{
				if((EmbedOpened >> i) & 0x1)
					stream->writeInt(EmbedSlot[i], Base::Bit32);
			}

			stream->writeInt(EquipStrengthens,		Base::Bit8);
			for(int i = 0; i < MAX_EQUIPSTENGTHENS; i++)
			{
				stream->writeInt(EquipStrengthenValue[i][0],	4);
				stream->writeInt(EquipStrengthenValue[i][1],	Base::Bit32);
			}

			stream->writeInt(ActivatePro,		Base::Bit32);
			// 判断是否激活铭刻属性
			if(ActivatePro & EAPF_IMPRESS)
			{
				stream->writeInt(ImpressID,		Base::Bit32);
			}

			// 判断是否激活绑定属性
			if(ActivatePro & EAPF_BIND)
			{
				stream->writeInt(BindProID,		Base::Bit32);
			}

			// 判断是否激活五行属性
			if(ActivatePro & EAPF_WUXING)
			{
				stream->writeInt(WuXingID,		4);
				stream->writeInt(WuXingLinkID,	4);
				stream->writeInt(WuXingPro,		Base::Bit32);
			}

			// 判断是否激活鉴定后附加属性
			if(ActivatePro & EAPF_ATTACH)
			{
				stream->writeInt(RandPropertyID,  10);
				stream->writeInt(IDEProNum,		4);
				for(int i = 0; i < IDEProNum; i++)
					stream->writeInt(IDEProValue[i],	Base::Bit32);
			}

			// 判断是否激活改变装备附加主动技能
			if(ActivatePro & EAPF_EQUIPSKILLA)
			{
				stream->writeInt(SkillAForEquip,	Base::Bit32);
			}

			// 判断是否激活改变装备附加被动技能
			if(ActivatePro & EAPF_EQUIPSKILLB)
			{
				stream->writeInt(SkillBForEquip,	Base::Bit32);
			}
		}
		return true;
	}

	template<class T>
	bool ReadData(T *stream)
	{
		stream->readBits(Base::Bit64,			&UID);
		ItemID			= stream->readInt(Base::Bit32);
		Quantity		= stream->readInt(Base::Bit8);
		LapseTime		= stream->readInt(Base::Bit32);
		RemainUseTimes	= stream->readInt(10);
		LockedLeftTime	= stream->readInt(Base::Bit32);
		BindPlayer		= stream->readInt(Base::Bit32);
		BindFriend		= stream->readInt(Base::Bit32);
		stream->readString(Producer,COMMON_STRING_LENGTH);
		ActiveFlag		= stream->readInt(Base::Bit32);

		// 判断是否装备
		if(ActiveFlag & 0x1)
		{
			Quality			= stream->readInt(10);
			CurWear			= stream->readInt(Base::Bit16);
			CurMaxWear		= stream->readInt(Base::Bit16);
			CurAdroit		= stream->readInt(Base::Bit32);
			CurAdroitLv		= stream->readInt(Base::Bit8);

			EmbedOpened		= stream->readInt(Base::Bit8);
			for(int i = 0; i < MAX_EMBEDSLOTS; i++)
			{
				if((EmbedOpened >> i) & 0x1)
					EmbedSlot[i] = stream->readInt(Base::Bit32);
			}

			EquipStrengthens	= stream->readInt(Base::Bit8);
			for(int i = 0; i < MAX_EQUIPSTENGTHENS; i++)
			{
				EquipStrengthenValue[i][0]	= stream->readInt(4);
				EquipStrengthenValue[i][1]	= stream->readInt(Base::Bit32);
			}

			ActivatePro		= stream->readInt(Base::Bit32);
			// 判断是否激活铭刻属性
			if(ActivatePro & EAPF_IMPRESS)
			{
				ImpressID	= stream->readInt(Base::Bit32);
			}

			// 判断是否激活绑定属性
			if(ActivatePro & EAPF_BIND)
			{
				BindProID	= stream->readInt(Base::Bit32);
			}

			// 判断是否激活五行属性
			if(ActivatePro & EAPF_WUXING)
			{
				WuXingID		= stream->readInt(4);
				WuXingLinkID	= stream->readInt(4);
				WuXingPro		= stream->readInt(Base::Bit32);
			}

			// 判断是否激活鉴定后附加属性
			if(ActivatePro & EAPF_ATTACH)
			{
				RandPropertyID	= stream->readInt(10);
				IDEProNum		= stream->readInt(4);
				for(U32 i = 0; i < IDEProNum; i++)
					IDEProValue[i] = stream->readInt(Base::Bit32);
			}
			
			// 判断是否激活改变装备附加主动技能
			if(ActivatePro & EAPF_EQUIPSKILLA)
			{
				SkillAForEquip		= stream->readInt(Base::Bit32);
			}

			// 判断是否激活改变装备附加被动技能
			if(ActivatePro & EAPF_EQUIPSKILLB)
			{
				SkillBForEquip		= stream->readInt(Base::Bit32);
			}
		}
		return true;
	}
};


// 玩家宠物结构
struct stPetInfo 
{
	enum
	{
		PetDataId_Mask			= 1,
		PetName_Mask			= PetDataId_Mask << 1,
		PetStatus_Mask			= PetDataId_Mask << 2,
		PetTitle_Mask			= PetDataId_Mask << 3,
		PetChengZhang_Mask		= PetDataId_Mask << 4,
		PetHappiness_Mask		= PetDataId_Mask << 5,
		PetLeftTime_Mask		= PetDataId_Mask << 6,
		PetExp_Mask				= PetDataId_Mask << 7,
		PetLevel_Mask			= PetDataId_Mask << 8,
		PetOther_Mask			= PetDataId_Mask << 9,
		
		MaxPetSkills = 12,
	};

	U32			petDataId;						//模板ID
	char		icon[16];						//图标
	U32			id;								//唯一ID
	U32			partnerId;						//配偶ID
	char		name[COMMON_STRING_LENGTH];		//名称
	U8			sex;							//性别
	U8			style;							//类型 (灵兽宝宝、变异宝宝、灵兽、变异灵兽)
	
	bool		mature;							//是否成年
	U8			status;							//状态 待机标识、出战标识、合体标识、死亡标识、打工标识、修行标识
	U8			generation;						//代数
	char		message[PET_MESSAGE_LENGTH];	//签名
	U64			title;							//称号标识
	U8			titleEnabled;					//当前称号
	U16			level;							//当前等级
	U32			lives;							//当前寿命

	U32			curHP;							//当前生命
	U32			curMP;							//当前真气
	U32			curPP;							//当前元气

	U16			staminaPts;						//体质属性点
	U16			ManaPts;						//法力属性点
	U16			strengthPts;					//力量属性点
	U16			intellectPts;					//智力属性点
	U16			agilityPts;						//身法属性点
	U16			pneumaPts;						//元力属性点
	U16			statsPoints;					//剩余属性点

	U8			insight;						//悟性
	U8			petTalent;						//根骨


	U32			chengZhangLv;					//成长率
	U16			qianLi;							//潜力
	U16			quality;						//品质
	U16			staminaGift;					//体质资质
	U16			ManaGift;						//法力资质
	U16			strengthGift;					//力量资质
	U16			intellectGift;					//智力资质
	U16			agilityGift;					//身法资质
	U16			pneumaGift;						//元力资质

	U8			happiness;						//快乐
	U32			exp;							//经验

	U16			charPoints;						//剩余性格点

	U32			Combat[MaxPetSkills];			//战斗技能
	U32			Combo[MaxPetSkills];			//合体技能
	U32			randBuffId;						//随机数值--宠物data中指定

	U32			mLeftTime;						//记录5分钟剩下的时间（用于扣除宠物快乐度的准确计时）

	stPetInfo() { clear();}
	void clear() { memset(this, 0, sizeof(stPetInfo));}

	template<class T>
	void WriteData(T *stream)
	{
		stream->writeInt(petDataId,			Base::Bit32);
		stream->writeInt(chengZhangLv,		Base::Bit32);
		stream->writeInt(qianLi,			Base::Bit32);
		stream->writeString(icon,			16);
		stream->writeInt(id,				Base::Bit32);
		stream->writeInt(partnerId,			Base::Bit32);
		stream->writeString(name,			COMMON_STRING_LENGTH);
		stream->writeInt(sex,				4);
		stream->writeInt(style,				Base::Bit8);
		stream->writeFlag(mature);
		stream->writeInt(status,			Base::Bit8);
		stream->writeInt(generation,		Base::Bit8);
		stream->writeString(message,		PET_MESSAGE_LENGTH);
		stream->writeBits(Base::Bit64,		&title);
		stream->writeInt(titleEnabled,		Base::Bit8);
		stream->writeInt(level,				10);
		stream->writeInt(lives,				24);
		stream->writeInt(curHP,				24);
		stream->writeInt(curMP,				24);
		stream->writeInt(curPP,				24);
		stream->writeInt(staminaPts,		12);
		stream->writeInt(ManaPts,			12);
		stream->writeInt(strengthPts,		12);
		stream->writeInt(intellectPts,		12);
		stream->writeInt(agilityPts,		12);
		stream->writeInt(pneumaPts,			12);
		stream->writeInt(statsPoints,		12);
		stream->writeInt(insight,			Base::Bit8);
		stream->writeInt(petTalent,			Base::Bit8);
		stream->writeInt(quality,			12);
		stream->writeInt(staminaGift,		12);
		stream->writeInt(ManaGift,			12);
		stream->writeInt(strengthGift,		12);
		stream->writeInt(intellectGift,		12);
		stream->writeInt(agilityGift,		12);
		stream->writeInt(pneumaGift,		12);
		stream->writeInt(happiness,			Base::Bit8);
		stream->writeInt(exp,				Base::Bit32);
		stream->writeInt(charPoints,		Base::Bit16);
		for (S32 i=0; i<MaxPetSkills; ++i)
		{
			stream->writeInt(Combat[i],		Base::Bit32);
			stream->writeInt(Combo[i],		Base::Bit32);
		}
		stream->writeInt(randBuffId,		Base::Bit32);
	}

	template<class T>
	void WriteInfo(T *stream, U32 flag)
	{
		stream->writeInt(flag,					Base::Bit32);
		if (flag & PetDataId_Mask)
		{
			stream->writeInt(petDataId,			Base::Bit32);
			stream->writeString(icon,			16);
			stream->writeInt(id,				Base::Bit32);
			stream->writeInt(partnerId,			Base::Bit32);
			stream->writeInt(sex,				4);
			stream->writeInt(style,				Base::Bit8);
			stream->writeFlag(mature);
			stream->writeInt(generation,		Base::Bit8);
			stream->writeString(message,		PET_MESSAGE_LENGTH);
		}
		if(flag & PetName_Mask)
		{
			stream->writeString(name,			COMMON_STRING_LENGTH);
		}
		if(flag & PetStatus_Mask)
		{
			stream->writeInt(status,			Base::Bit8);
		}
		if(flag & PetTitle_Mask)
		{
			stream->writeBits(Base::Bit64,		&title);
			stream->writeInt(titleEnabled,		Base::Bit8);
		}
		if (flag & PetChengZhang_Mask)
		{
			stream->writeInt(chengZhangLv,		Base::Bit32);
		}
		if (flag & PetHappiness_Mask)
		{
			stream->writeInt(happiness,			Base::Bit32);
		}
		if (flag & PetLeftTime_Mask)
		{
			stream->writeInt(mLeftTime,			Base::Bit32);
		}
		if (flag & PetExp_Mask)
		{
			stream->writeInt(exp,				Base::Bit32);
		}
		if (flag & PetLevel_Mask)
		{
			stream->writeInt(level,				10);
		}

		if(flag & PetOther_Mask)
		{			
			stream->writeInt(qianLi,			32);
			stream->writeInt(lives,				24);
			stream->writeInt(curHP,				24);
			stream->writeInt(curMP,				24);
			stream->writeInt(curPP,				24);
			stream->writeInt(staminaPts,		12);
			stream->writeInt(ManaPts,			12);
			stream->writeInt(strengthPts,		12);
			stream->writeInt(intellectPts,		12);
			stream->writeInt(agilityPts,		12);
			stream->writeInt(pneumaPts,			12);
			stream->writeInt(statsPoints,		12);
			stream->writeInt(insight,			Base::Bit8);
			stream->writeInt(petTalent,			Base::Bit8);
			stream->writeInt(quality,			12);
			stream->writeInt(staminaGift,		12);
			stream->writeInt(ManaGift,			12);
			stream->writeInt(strengthGift,		12);
			stream->writeInt(intellectGift,		12);
			stream->writeInt(agilityGift,		12);
			stream->writeInt(pneumaGift,		12);
			stream->writeInt(happiness,			Base::Bit8);			
			stream->writeInt(charPoints,		Base::Bit16);
			for (S32 i=0; i<MaxPetSkills; ++i)
			{
				stream->writeInt(Combat[i],		Base::Bit32);
				stream->writeInt(Combo[i],		Base::Bit32);
			}
			stream->writeInt(randBuffId,		Base::Bit32);
		}
	}


	template<class T>
	U32 ReadInfo(T *stream)
	{
		U32 flag = stream->readInt(Base::Bit32);
		if (flag & PetDataId_Mask)
		{
			petDataId = stream->readInt(Base::Bit32);
			stream->readString(icon,Base::Bit16);
			id = stream->readInt(Base::Bit32);
			partnerId = stream->readInt(Base::Bit32);
			sex = stream->readInt(4);
			style = stream->readInt(Base::Bit8);
			mature = stream->readFlag();
			generation = stream->readInt(Base::Bit8);
			stream->readString(message,PET_MESSAGE_LENGTH);
		}
		if(flag & PetName_Mask)
		{
			stream->readString(name,COMMON_STRING_LENGTH);
		}
		if(flag & PetStatus_Mask)
		{
			status = stream->readInt(Base::Bit8);
		}
		if(flag & PetTitle_Mask)
		{
			stream->readBits(Base::Bit64, &title);
			titleEnabled = stream->readInt(Base::Bit8);
		}
		if (flag & PetChengZhang_Mask)
		{
			chengZhangLv = stream->readInt(Base::Bit32);
		}
		if (flag & PetHappiness_Mask)
		{
			happiness = stream->readInt(Base::Bit32);
		}
		if (flag & PetLeftTime_Mask)
		{
			mLeftTime = stream->readInt(Base::Bit32);
		}
		if (flag & PetExp_Mask)
		{
			exp = stream->readInt(Base::Bit32);
		}
		if (flag & PetLevel_Mask)
		{
			level = stream->readInt(10);
		}
		if(flag & PetOther_Mask)
		{			
			qianLi = stream->readInt(32);
			lives = stream->readInt(24);
			curHP = stream->readInt(24);
			curMP = stream->readInt(24);
			curPP = stream->readInt(24);
			staminaPts = stream->readInt(12);
			ManaPts = stream->readInt(12);
			strengthPts = stream->readInt(12);
			intellectPts = stream->readInt(12);
			agilityPts = stream->readInt(12);
			pneumaPts = stream->readInt(12);
			statsPoints = stream->readInt(12);
			insight = stream->readInt(Base::Bit8);
			petTalent = stream->readInt(Base::Bit8);
			quality = stream->readInt(12);
			staminaGift = stream->readInt(12);
			ManaGift = stream->readInt(12);
			strengthGift = stream->readInt(12);
			intellectGift = stream->readInt(12);
			agilityGift = stream->readInt(12);
			pneumaGift = stream->readInt(12);
			happiness = stream->readInt(Base::Bit8);			
			charPoints = stream->readInt(Base::Bit16);
			for (S32 i=0; i<MaxPetSkills; ++i)
			{
				Combat[i] = stream->readInt(Base::Bit32);
				Combo[i] = stream->readInt(Base::Bit32);
			}
			randBuffId = stream->readInt(Base::Bit32);
		}
		return flag;
	}

	template<class T>
	void ReadData(T *stream)
	{
		petDataId		= stream->readInt(Base::Bit32);
		chengZhangLv	= stream->readInt(Base::Bit32);
		qianLi			= stream->readInt(Base::Bit32);
		stream->readString(icon,16);
		id				= stream->readInt(Base::Bit32);
		partnerId		= stream->readInt(Base::Bit32);
		stream->readString(name,COMMON_STRING_LENGTH);
		sex				= stream->readInt(4);
		style			= stream->readInt(Base::Bit8);
		mature			= stream->readFlag();
		status			= stream->readInt(Base::Bit8);
		generation		= stream->readInt(Base::Bit8);
		stream->readString(message,PET_MESSAGE_LENGTH);
		stream->readBits(Base::Bit64,		&title);
		titleEnabled	= stream->readInt(Base::Bit8);
		level			= stream->readInt(10);
		lives			= stream->readInt(24);
		curHP			= stream->readInt(24);
		curMP			= stream->readInt(24);
		curPP			= stream->readInt(24);
		staminaPts		= stream->readInt(12);
		ManaPts			= stream->readInt(12);
		strengthPts		= stream->readInt(12);
		intellectPts	= stream->readInt(12);
		agilityPts		= stream->readInt(12);
		pneumaPts		= stream->readInt(12);
		statsPoints		= stream->readInt(12);
		insight			= stream->readInt(Base::Bit8);
		petTalent		= stream->readInt(Base::Bit8);
		quality			= stream->readInt(12);
		staminaGift		= stream->readInt(12);
		ManaGift		= stream->readInt(12);
		strengthGift	= stream->readInt(12);
		intellectGift	= stream->readInt(12);
		agilityGift		= stream->readInt(12);
		pneumaGift		= stream->readInt(12);
		happiness		= stream->readInt(Base::Bit8);
		exp				= stream->readInt(Base::Bit32);
		charPoints		= stream->readInt(Base::Bit16);
		for (S32 i=0; i<MaxPetSkills; ++i)
		{
			Combat[i]	= stream->readInt(Base::Bit32);
			Combo[i]	= stream->readInt(Base::Bit32);
		}
		randBuffId = stream->readInt(Base::Bit32);
	}
};

// 玩家骑宠结构
struct stMountInfo
{
	U32		mDataId;
	U8		mLevel;
	U32		mLeftTime;
	U32		mSkillId[3];
	U8		mStatus;

	stMountInfo() { clear();}
	void clear() { memset(this, 0, sizeof(stMountInfo));}

	template<class T>
	void WriteData(T *stream)
	{
		stream->writeInt(mDataId, Base::Bit32);
		stream->writeInt(mLevel, Base::Bit8);
		stream->writeInt(mLeftTime, Base::Bit32);
		for (int i = 0; i < 3; i++)
			stream->writeInt(mSkillId[i], Base::Bit32);
		stream->writeInt(mStatus, Base::Bit8);
	}

	template<class T>
	void ReadData(T *stream)
	{
		mDataId		= stream->readInt(Base::Bit32);
		mLevel		= stream->readInt(Base::Bit8);
		mLeftTime	= stream->readInt(Base::Bit32);
		for (int i = 0; i < 3; i++)
			mSkillId[i]	= stream->readInt(Base::Bit32);
		mStatus		= stream->readInt(Base::Bit8);
	}
};

//玩家元神结构
struct stSpiritInfo
{
	enum
	{
		DataId_Mask				= 1,
		Level_Mask				= DataId_Mask << 1,
		JingJie_Mask			= DataId_Mask << 2,
		TalentPts_Mask			= DataId_Mask << 3,
		Skill_Mask				= DataId_Mask << 4,
		EquipId_Mask			= DataId_Mask << 5,
		CleanCount_Mask			= DataId_Mask << 6,
		CostBuff_Mask			= DataId_Mask << 7,
		Exp_Mask				= DataId_Mask << 8,

		MaxEquipId				= 3,
	};

	U32		mDataId;									//元神Data编号
	U16		mLevel;										//元神当前等级
	U8		mJingJieLevel;								//元神境界等级
	
	U16		mTalentExtPts;								//天赋扩展点数
	U16		mTalentLeftPts;								//天赋剩余点数
	U8		mTalentSkillPts[MAX_TALENT];				//天赋技能已配的点数

	U8		mOpenedSkillFlag;							//已开启的绝招栏
	U32		mSkillId[MAX_SPIRIT_SKILL];					//绝招ID
	U32		mEquipId[MaxEquipId];						//装备ID
	U32		mCleanPtsCount;								//当前洗点次数

	U32		mValueBuffId;								//元神数值状态(包含变身和基础数值模板)
	U32		mCostBuffId;								//元神消耗状态
	U32		mCurrExp;									//元神当前经验

	stSpiritInfo() { clear();}
	void clear() { memset(this, 0, sizeof(stSpiritInfo));}
	
	template<class T>
	void WriteInfo(T *stream, U32 flag)
	{
		stream->writeInt(flag,				Base::Bit32);
		if (flag & DataId_Mask)
		{
			stream->writeInt(mDataId,		Base::Bit32);
		}
		if (flag & Level_Mask)
		{
			stream->writeInt(mLevel,		Base::Bit16);
			stream->writeInt(mValueBuffId,	Base::Bit32);
		}
		if (flag & JingJie_Mask)
		{
			stream->writeInt(mJingJieLevel,	Base::Bit32);
		}
		if (flag & TalentPts_Mask)
		{
			stream->writeInt(mTalentExtPts,		Base::Bit16);
			stream->writeInt(mTalentLeftPts,	Base::Bit16);
			for (int i = 0; i < MAX_TALENT; i++)
			{
				stream->writeInt(mTalentSkillPts[i], Base::Bit8);
			}
		}	
		if (flag & Skill_Mask)
		{
			stream->writeInt(mOpenedSkillFlag,	Base::Bit8);
			for (int i = 0; i < MAX_SPIRIT_SKILL; i++)
			{
				stream->writeInt(mSkillId[i],	Base::Bit32);
			}
		}
		if (flag & EquipId_Mask)
		{
			for (int i = 0; i < MaxEquipId; i++)
			{
				stream->writeInt(mEquipId[i],	Base::Bit32);
			}
		}
		if (flag & CleanCount_Mask)
		{
			stream->writeInt(mCleanPtsCount,	Base::Bit32);
		}
		if (flag & CostBuff_Mask)
		{
			stream->writeInt(mCostBuffId,		Base::Bit32);
		}
		if (flag & Exp_Mask)
		{
			stream->writeInt(mCurrExp,			Base::Bit32);
		}
	}

	template<class T>
	U32 ReadInfo(T *stream)
	{
		U32 flag = stream->readInt(Base::Bit32);

		if (flag & DataId_Mask)
		{
			mDataId = stream->readInt(Base::Bit32);
		}
		if (flag & Level_Mask)
		{
			mLevel = stream->readInt(Base::Bit16);
			mValueBuffId = stream->readInt(Base::Bit32);
		}
		if (flag & JingJie_Mask)
		{
			mJingJieLevel = stream->readInt(Base::Bit32);
		}
		if (flag & TalentPts_Mask)
		{
			mTalentExtPts = stream->readInt(Base::Bit16);
			mTalentLeftPts = stream->readInt(Base::Bit16);
			for (int i = 0; i < MAX_TALENT; i++)
			{
				mTalentSkillPts[i] = stream->readInt(Base::Bit8);
			}
		}	
		if (flag & Skill_Mask)
		{
			mOpenedSkillFlag = stream->readInt(Base::Bit8);
			for (int i = 0; i < MAX_SPIRIT_SKILL; i++)
			{
				mSkillId[i] = stream->readInt(Base::Bit32);
			}
		}
		if (flag & EquipId_Mask)
		{
			for (int i = 0; i < MaxEquipId; i++)
			{
				mEquipId[i] = stream->readInt(Base::Bit32);
			}
		}
		if (flag & CleanCount_Mask)
		{
			mCleanPtsCount = stream->readInt(Base::Bit32);
		}
		if (flag & CostBuff_Mask)
		{
			mCostBuffId = stream->readInt(Base::Bit32);
		}
		if (flag & Exp_Mask)
		{
			mCurrExp = stream->readInt(Base::Bit32);
		}
		return flag;
	}
};

// 玩家技能结构
struct stSkillInfo 
{
	U32 SkillId;

	stSkillInfo():SkillId(0) {}

	template<class T>
	void WriteData(T *stream)
	{
		stream->writeInt(SkillId,		Base::Bit32);
	}

	template<class T>
	void ReadData(T *stream)
	{
		SkillId		= stream->readInt(Base::Bit32);
	}
};

// 玩家状态结构
struct stBuffInfo 
{
	U32 buffId;
	U32 time;
	U16 count;
	bool self;

	stBuffInfo():buffId(0),time(0),count(0),self(false)  {}

	template<class T>
	void WriteData(T *stream)
	{
		if(stream->writeFlag(buffId))
		{
			stream->writeInt(buffId,		Base::Bit32);
			stream->writeInt(time,			Base::Bit32);
			stream->writeInt(count,			Base::Bit16);
			stream->writeFlag(self);
		}
	}

	template<class T>
	void ReadData(T *stream)
	{
		if(stream->readFlag())
		{
			buffId		= stream->readInt(Base::Bit32);
			time		= stream->readInt(Base::Bit32);
			count		= stream->readInt(Base::Bit16);
			self		= stream->readFlag();
		}
	}
};

// 冷却时间结构
struct stCooldownInfo 
{
	enum
	{
		CooldownSecBits = 18,	// 秒，最大三天
	};

	U32 Cooldown;

	stCooldownInfo():Cooldown(0) {}

	template<class T>
	void WriteData(T *stream)
	{
		stream->writeInt(Cooldown,		CooldownSecBits);
	}

	template<class T>
	void ReadData(T *stream)
	{
		Cooldown	= stream->readInt(CooldownSecBits);
	}
};
//生活技能
struct stLivingSkillInfo
{
	U32 SkillID;
	U32 Ripe;
	stLivingSkillInfo():SkillID(0),Ripe(0){}

	template<class T>
	void WriteData(T *stream)
	{
		stream->writeInt(SkillID, Base::Bit32);
		stream->writeInt(Ripe,    Base::Bit32);
	}

	template<class T>
	void ReadData(T *stream)
	{
		stream->readInt(SkillID, Base::Bit32);
		stream->readInt(Ripe,    Base::Bit32);
	}
};

//这个结构有可能是玩家创建结构(仅用于显示用的微小结构)
//比如玩家外貌特征，装备武器等
struct stPlayerDisp
{
	//玩家压缩的基本数据结构
	int PlayerOnlineTime;						//角色在线累计时间
	int	 Level;									//角色等级
	int  ZoneId;								//下线地图编号(登录服务器用)
	int  TriggerNo;								//触发区域编号(显示所在区域用)
	int  fId;									//副本编号
	int  fTime;									//副本开启时间，这个是为了防止编号重复
	int  fZoneId;								//下线副本地图编号(登录服务器用)
	int  fTriggerNo;							//副本触发区域编号(显示所在区域用)
	int  FactionId;								//帮会编号
	int  FactionExp;							//帮会贡献
	char LeavePos[COMMON_STRING_LENGTH];		//离开地图时的位置
	char fLeavePos[COMMON_STRING_LENGTH];		//离开副本时的位置
	char Sex;									//性别
	char Race;									//种族
	char Family;								//门宗
	int	 honour;								//人气
	char Classes[MaxClassesMasters];			//职业
	U32  OtherFlag;								//其它标志位(例如:是否显示时装,是否新手)
	U32  CopyMapInstId;							//副本实例ID
	U32  flyPath;								//飞行路径
	U32  flyPos;								//飞行位置
	U32  flyMount;								//飞行骑乘

	//////////////////////////////////////////////////////////////////////////
	// 以下为客户端缓存的一些临时数据，不参与数据传输与数据库存储
#ifdef NTJ_CLIENT
	U32	 TeamId;
#endif

	//显示用特有数据结构
	U16 Body;		//身体
	U16 Face;		//脸型
	U16 Hair;		//发型
	char HairCol;	//发色

	stItemInfo	EquipInfo[EQUIP_MAXSLOTS];		// 装备栏物品数据

	stPlayerDisp() {InitData();}
	void InitData();	

	template<class T>
	bool WriteData(T* pPacket)
	{
		pPacket->writeInt(PlayerOnlineTime,		Base::Bit32);
		pPacket->writeInt(Sex,					Base::Bit8);
		pPacket->writeInt(Race,					Base::Bit8);
		pPacket->writeInt(Family,				Base::Bit8);
		pPacket->writeInt(Classes[_1st],		Base::Bit8);
		pPacket->writeInt(Classes[_2nd],		Base::Bit8);
		pPacket->writeInt(Classes[_3rd],		Base::Bit8);
		pPacket->writeInt(Classes[_4th],		Base::Bit8);
		pPacket->writeInt(Classes[_5th],		Base::Bit8);
		pPacket->writeInt(Level,				Base::Bit32);
		pPacket->writeInt(ZoneId,				Base::Bit32);
		pPacket->writeInt(fZoneId,				Base::Bit32);
		pPacket->writeInt(TriggerNo,			Base::Bit32);
		pPacket->writeInt(OtherFlag,			Base::Bit32);
		pPacket->writeSignedInt(honour,			Base::Bit32);
		pPacket->writeInt(CopyMapInstId,		Base::Bit32);
		pPacket->writeInt(flyPath,				Base::Bit32);
		pPacket->writeInt(flyPos,				Base::Bit32);
		pPacket->writeInt(flyMount,				Base::Bit32);
		pPacket->writeString(LeavePos,COMMON_STRING_LENGTH);
		pPacket->writeString(fLeavePos,COMMON_STRING_LENGTH);
		pPacket->writeInt(Body, Base::Bit16);
		pPacket->writeInt(Face, Base::Bit16);
		pPacket->writeInt(Hair, Base::Bit16);
		pPacket->writeInt(HairCol, Base::Bit8);


		for(int i = 0; i < EQUIP_MAXSLOTS; i++)
			EquipInfo[i].WriteData<T>(pPacket);
		return true;
	}

	template<class T>
	bool ReadData(T* pPacket)
	{
		PlayerOnlineTime	= pPacket->readInt(Base::Bit32);
		Sex					= pPacket->readInt(Base::Bit8);
		Race				= pPacket->readInt(Base::Bit8);
		Family				= pPacket->readInt(Base::Bit8);
		Classes[_1st]		= pPacket->readInt(Base::Bit8);
		Classes[_2nd]		= pPacket->readInt(Base::Bit8);
		Classes[_3rd]		= pPacket->readInt(Base::Bit8);
		Classes[_4th]		= pPacket->readInt(Base::Bit8);
		Classes[_5th]		= pPacket->readInt(Base::Bit8);
		Level				= pPacket->readInt(Base::Bit32);
		ZoneId				= pPacket->readInt(Base::Bit32);
		fZoneId				= pPacket->readInt(Base::Bit32);
		TriggerNo			= pPacket->readInt(Base::Bit32);
		OtherFlag			= pPacket->readInt(Base::Bit32);
		honour				= pPacket->readSignedInt(Base::Bit32);
		CopyMapInstId		= pPacket->readInt(Base::Bit32);
		flyPath				= pPacket->readInt(Base::Bit32);
		flyPos				= pPacket->readInt(Base::Bit32);
		flyMount			= pPacket->readInt(Base::Bit32);

		pPacket->readString(LeavePos,COMMON_STRING_LENGTH);
		pPacket->readString(fLeavePos,COMMON_STRING_LENGTH);
		Body = pPacket->readInt(Base::Bit16);
		Face = pPacket->readInt(Base::Bit16);
		Hair = pPacket->readInt(Base::Bit16);
		HairCol = pPacket->readInt(Base::Bit8);

		for(int i = 0; i < EQUIP_MAXSLOTS; i++)
			EquipInfo[i].ReadData<T>(pPacket);

		return true;
	}
};

// 任务的旗标数据结构(244)
struct stMissionFlag
{
	S16			MissionID;					//任务编号
	S16			Name[MISSION_FLAG_MAX];		//任务旗标名
	S32			Value[MISSION_FLAG_MAX];	//任务旗标值
};

// 任务的循环任务结构(8)
struct stMissionCycle
{
	S16			MissionID;					//任务编号
	U16			EndTimes;					//完成次数
	U32			EndTime;					//完成时间
};

//　玩家任务结构(8976)
struct stMission
{
	U8	State[MISSION_STATE_LENGTH];			//任务完成状态
	stMissionCycle	Cycle[MISSION_CYCLE_MAX];	//循环任务信息
	stMissionFlag	Flag[MISSION_ACCEPT_MAX];	//已接任务旗标
	stMission() { InitData();}
	void InitData()
	{
		memset(this, 0, sizeof(stMission));
		for(int i = 0; i < MISSION_CYCLE_MAX; ++i)
			Cycle[i].MissionID = -1;
		for(int i = 0; i < MISSION_ACCEPT_MAX; ++i)
			Flag[i].MissionID = -1;
	}
};

//时间触发器结构
struct stTimerTriggerInfo
{
	U32					m_nLeftTime;		//持续时间
	U32					m_nIntervalTime;	//刷新时间
	bool				m_bCanCancel;		//能否取消
	U32					m_nState;			//当前状态
	U32					m_nTimeFlag;		//计时行为
	U32					m_Id;				//计时器ID
	U32					m_nItemId[4];		//奖励的物品ID
};

//时间触发器宠物修行结构
struct stTimerTriggerPetStudyInfo
{
	U32					m_nLeftTime;		//剩余持续时间
	U32					m_nTotalTime;		//总时间
	U32					m_nStudyDataId;		//修行data ID
	U32					m_nIntervalTime;	//刷新时间
	U32					m_nTimeFlag;		//计时行为
	U32					m_Id;				//计时器ID
};

// 快捷栏数据结构
struct stShortCutInfo
{
	U8	ShortCutType;		//快捷栏类型(0-物品, 1-技能 ,2-生活技能)
	U16	ShortCutNum;		//快捷栏数量
	U32	ShortCutID;			//快捷栏编号(物品编号或技能编号)
	stShortCutInfo():ShortCutType(0),ShortCutNum(0),ShortCutID(0) {}
};

// 玩家社会关系结构
struct stSocialItem
{
	int	playerId;
	SocialType::Type type;				// SocialType
	int friendValue;					// 好友度
};

// 势力属性
struct stFame
{
	S8 fame;							// 声望
	S32 favour;							// 好感度

	stFame() {dMemset(this, 0, sizeof(stFame));}

	template<class T>
	void WriteData(T *stream)
	{
		stream->writeInt(favour,	8);
		stream->writeInt(fame,		18);
	}

	template<class T>
	void ReadData(T *stream)
	{
		favour	= stream->readInt(8);
		fame	= stream->readInt(18);
	}
};

//这个结构有可能是玩家主要数据结构
//比如玩家任务，背包，技能等
struct stPlayerMain
{
	int SocialItemCount;								// 玩家社会关系数量
	stSocialItem SocialItem[SOCAIL_ITEM_MAX];			// 玩家社会关系
	U8   MasterLevel;                                   // 师傅资格等级
	U32  CurrMasterExp;                                 // 当前师德
	U32  ExpPool;                                       // 经验池
	bool bPrentice;                                     // 是否徒状态
	bool bFarPrentice;                                  // 出师标记
	U16  MPMStudyLevel;                                 // 拜师等级
	char MasterText[MPM_CALLBOARD_MAXSTRING];           // 师傅公告信息 

	int playerHot;										// 仙缘值

	U32 CurrentHP;										// 当前血量值
	U32 CurrentMP;										// 当前法力值
	U32 CurrentPP;										// 当前元气值
	U32 CurrentVigor;									// 当前活力值
	U32 MaxVigor;                                       // 最大活力值
	U32 CurrentExp;										// 当前经验值
	U32 CurrentLivingExp;                               // 当前生活经验值
	U32 LivingLevel;                                    // 生活等级
	U32 Money;											// 金钱
	U32 BindMoney;										// 绑定金钱
	U32 BankMoney;										// 银行仓库金币
	U32 TradeLockFlag;									// 第二密码交易锁功能标志
	char SecondPassword[COMMON_STRING_LENGTH];			// 第二密码
	U32 PrescriptionCount;                              // 拥有配方数
	U32 PrescriptionInfo[PRESCRIPTION_MAX];             // 配方数据
	U32 BankBagSize;									// 玩家购买的银行包裹
	U32 LivingSkillStudyID;                             // 修练专精生活技能ID

	S32			TimerTriggerCount;						//剩余时间触发器个数
	stTimerTriggerInfo	TimerTriggerInfo[MAX_TIMER_SIZE]; //时间触发器数据
	S32			TimerPetStudyCount;						//宠物修行时间触发器个数
	stTimerTriggerPetStudyInfo TimerPetStudyInfo[PET_MAXSLOTS];	//宠物修行时间触发器数据
	
	stMission	MissionInfo;
	stItemInfo	InventoryInfo[INVENTROY_MAXSLOTS];		// 物品栏数据
	stItemInfo	BankInfo[BANK_MAXSLOTS];				// 银行仓库物品数据
	stShortCutInfo PanelInfo[PANEL_MAXSLOTS];			// 快捷栏物品或技能数据
	stShortCutInfo SplitInfo[ITEMSPLIT_MAXSLOTS];       // 分解产物数据 
	stShortCutInfo ComposeInfo;                         // 合成产物数据
	stSkillInfo	SkillInfo[SKILL_MAXSLOTS];				// 技能数据
	stCooldownInfo CooldownInfo[COOLDOWN_MAX];			// 冷却时间数据
	stLivingSkillInfo LivingSkillInfo[LIVINGSKILL_MAXSLOTS]; //生活技能数据
	stPetInfo PetInfo[PET_MAXSLOTS];					// 宠物数据
	stMountInfo MountInfo[MOUNTPET_MAXSLOTS];			// 骑宠数据
	stFame Fame[MaxInfluence];							// 声望数据

	U8	SpiritOpenedFlag;								// 已开启元神栏(bit位表示)
	S8	SpiritActiveSlot;								// 当前激活的元神
	bool bSpiritHuanHua;								// 当前激活的元神是否进行了幻化
	stSpiritInfo SpiritInfo[SPIRIT_MAXSLOTS];			// 元神数据

	stBuffInfo	BuffInfo[BUFF_MAXSLOTS];				// buff数据
	stBuffInfo	DebuffInfo[BUFF_MAXSLOTS];				// debuff数据
	stBuffInfo	SysBuffInfo[BUFF_MAXSLOTS];				// sysbuff数据

	U8	 PlayerDataFlag[MAX_PLAYERDATAFLAGS];			// 策划用玩家旗标数据	
	U32	 PlayerStructVer;								// <<人物数据结构版本值,避免外部工具版本不同步引起误操作>>

	stPlayerMain() {InitData();}
	void InitData();
	
	template<class T>
	bool WriteData(T *stream)
	{
		stream->writeInt(playerHot,				Base::Bit16);
		stream->writeInt(SocialItemCount,		Base::Bit16 );
		for( int i = 0; i < SocialItemCount; i++ )
		{
			stream->writeInt( SocialItem[i].playerId,		Base::Bit32 );
			stream->writeInt( SocialItem[i].type, 4 );
			stream->writeInt( SocialItem[i].friendValue,	Base::Bit32 );
		}

		stream->writeInt(MasterLevel,         Base::Bit8);
		stream->writeInt(CurrMasterExp,       Base::Bit32);
		stream->writeInt(ExpPool,             Base::Bit32);
		stream->writeInt(bPrentice,           Base::Bit8);
		stream->writeInt(bFarPrentice,        Base::Bit8);
		stream->writeInt(MPMStudyLevel,       Base::Bit16);
		stream->writeString(MasterText,MPM_CALLBOARD_MAXSTRING);

		stream->writeInt(CurrentHP,			  Base::Bit32);
		stream->writeInt(CurrentMP,			  Base::Bit32);
		stream->writeInt(CurrentPP,			  Base::Bit32);
		stream->writeInt(CurrentVigor,		  Base::Bit32);
		stream->writeInt(MaxVigor,            Base::Bit32);
		stream->writeInt(CurrentExp,		  Base::Bit32);
		stream->writeInt(CurrentLivingExp,    Base::Bit32);
		stream->writeInt(LivingLevel,         Base::Bit32);
		stream->writeInt(Money,				  Base::Bit32);
		stream->writeInt(BindMoney,			  Base::Bit32);
		stream->writeInt(BankMoney,			  Base::Bit32);
		stream->writeInt(TradeLockFlag,		  Base::Bit32);
		stream->writeInt(LivingSkillStudyID,  Base::Bit32);

		stream->writeInt(TimerTriggerCount,   Base::Bit8);
		for (int i = 0; i < TimerTriggerCount; i++)
		{
			stream->writeInt(TimerTriggerInfo[i].m_nLeftTime, Base::Bit32);
			stream->writeInt(TimerTriggerInfo[i].m_nIntervalTime, Base::Bit32);
			stream->writeFlag(TimerTriggerInfo[i].m_bCanCancel);
			stream->writeInt(TimerTriggerInfo[i].m_nState, Base::Bit8);
			stream->writeInt(TimerTriggerInfo[i].m_nTimeFlag, Base::Bit8);
			stream->writeInt(TimerTriggerInfo[i].m_Id, Base::Bit32);
			for (int j = 0; j < 4; j++)
			{
				stream->writeInt(TimerTriggerInfo[i].m_nItemId[j], Base::Bit32);
			}
		}

		stream->writeInt(TimerPetStudyCount,   Base::Bit8);
		for (int i = 0; i < TimerPetStudyCount; i++)
		{
			stream->writeInt(TimerPetStudyInfo[i].m_nLeftTime, Base::Bit32);
			stream->writeInt(TimerPetStudyInfo[i].m_nTotalTime, Base::Bit32);
			stream->writeInt(TimerPetStudyInfo[i].m_nStudyDataId, Base::Bit32);
			stream->writeInt(TimerPetStudyInfo[i].m_nIntervalTime, Base::Bit32);
			stream->writeInt(TimerPetStudyInfo[i].m_nTimeFlag, Base::Bit8);
			stream->writeInt(TimerPetStudyInfo[i].m_Id, Base::Bit32);
		}

		stream->writeInt(BankBagSize,		  Base::Bit8);
		stream->writeString(SecondPassword,COMMON_STRING_LENGTH);

		stream->writeInt(PrescriptionCount,   Base::Bit16);
		stream->writeBits(sizeof(U32) * 8 * PrescriptionCount, &PrescriptionInfo);

		stream->writeBits(sizeof(stMission) * 8,	&MissionInfo);

		for(int i = 0; i < INVENTROY_MAXSLOTS; i++)
			InventoryInfo[i].WriteData<T>(stream);

		for(int i = 0; i < BANK_MAXSLOTS; i++)
			BankInfo[i].WriteData<T>(stream);

		stream->writeBits(sizeof(stShortCutInfo) * 8 * PANEL_MAXSLOTS,	&PanelInfo);
		stream->writeBits(sizeof(stShortCutInfo) * 8 * ITEMSPLIT_MAXSLOTS,&SplitInfo);
		stream->writeBits(sizeof(stShortCutInfo) * 8 ,&ComposeInfo);
		stream->writeBits(sizeof(stSkillInfo) * 8 * SKILL_MAXSLOTS,	&SkillInfo);
		stream->writeBits(sizeof(stLivingSkillInfo) * 8 * LIVINGSKILL_MAXSLOTS,&LivingSkillInfo);

		for(int i = 0; i < COOLDOWN_MAX; ++i)
			CooldownInfo[i].WriteData<T>(stream);
		for(int i = 0; i < PET_MAXSLOTS; ++i)
			PetInfo[i].WriteData<T>(stream);
		for(int i = 0; i < MOUNTPET_MAXSLOTS; ++i)
			MountInfo[i].WriteData<T>(stream);
		for(int i = 0; i < MaxInfluence; ++i)
			Fame[i].WriteData<T>(stream);

		stream->writeInt(SpiritOpenedFlag, Base::Bit8);
		stream->writeInt(SpiritActiveSlot, Base::Bit8);
		for (int i = 0; i < SPIRIT_MAXSLOTS; ++i)
		{
			SpiritInfo[i].WriteInfo<T>(stream, -1);
		}

		for(int i = 0; i < BUFF_MAXSLOTS; ++i)
			BuffInfo[i].WriteData<T>(stream);
		for(int i = 0; i < BUFF_MAXSLOTS; ++i)
			DebuffInfo[i].WriteData<T>(stream);
		for(int i = 0; i < BUFF_MAXSLOTS; ++i)
			SysBuffInfo[i].WriteData<T>(stream);
		
		stream->writeBits(sizeof(PlayerDataFlag) * 8, PlayerDataFlag);
		stream->writeInt(PlayerStructVer,		Base::Bit32);
		return true;
	}

	template<class T>
	bool ReadData(T *stream)
	{
		playerHot = stream->readInt( Base::Bit16 );
		SocialItemCount		= stream->readInt(Base::Bit16);
		for( int i = 0; i < SocialItemCount; i++ )
		{
			SocialItem[i].playerId = stream->readInt( Base::Bit32 );
			SocialItem[i].type = (SocialType::Type)stream->readInt( 4 );
			SocialItem[i].friendValue = stream->readInt( Base::Bit32 );
		}

		MasterLevel   = stream->readInt(Base::Bit8);
		CurrMasterExp = stream->readInt(Base::Bit32);
		ExpPool       = stream->readInt(Base::Bit32);
		bPrentice     = (bool)(stream->readInt(Base::Bit8));
		bFarPrentice  = stream->readInt(Base::Bit8);
		MPMStudyLevel = stream->readInt(Base::Bit16);
		stream->readString(MasterText,MPM_CALLBOARD_MAXSTRING);

		CurrentHP	= stream->readInt(Base::Bit32);
		CurrentMP	= stream->readInt(Base::Bit32);
		CurrentPP	= stream->readInt(Base::Bit32);
		CurrentVigor= stream->readInt(Base::Bit32);
		MaxVigor    = stream->readInt(Base::Bit32);
		CurrentExp	= stream->readInt(Base::Bit32);
		CurrentLivingExp = stream->readInt(Base::Bit32);
		LivingLevel = stream->readInt(Base::Bit32);
		Money		= stream->readInt(Base::Bit32);
		BindMoney	= stream->readInt(Base::Bit32);
		BankMoney	= stream->readInt(Base::Bit32);
		TradeLockFlag = stream->readInt(Base::Bit32);
		LivingSkillStudyID = stream->readInt(Base::Bit32);

		TimerTriggerCount = stream->readInt(Base::Bit8);
		for (int i = 0; i < TimerTriggerCount; i++)
		{
			TimerTriggerInfo[i].m_nLeftTime = stream->readInt(Base::Bit32);
			TimerTriggerInfo[i].m_nIntervalTime = stream->readInt(Base::Bit32);
			TimerTriggerInfo[i].m_bCanCancel = stream->readFlag();
			TimerTriggerInfo[i].m_nState = stream->readInt(Base::Bit8);
			TimerTriggerInfo[i].m_nTimeFlag = stream->readInt(Base::Bit8);
			TimerTriggerInfo[i].m_Id = stream->readInt(Base::Bit32);
			for (int j = 0; j < 4; j++)
			{
				TimerTriggerInfo[i].m_nItemId[j] = stream->readInt(Base::Bit32);
			}
		}

		TimerPetStudyCount = stream->readInt(Base::Bit8);
		for (int i = 0; i < TimerPetStudyCount; i++)
		{
			TimerPetStudyInfo[i].m_nLeftTime = stream->readInt(Base::Bit32);
			TimerPetStudyInfo[i].m_nTotalTime = stream->readInt(Base::Bit32);
			TimerPetStudyInfo[i].m_nStudyDataId = stream->readInt(Base::Bit32);
			TimerPetStudyInfo[i].m_nIntervalTime = stream->readInt(Base::Bit32);
			TimerPetStudyInfo[i].m_nTimeFlag = stream->readInt(Base::Bit8);
			TimerPetStudyInfo[i].m_Id = stream->readInt(Base::Bit32);
		}

		BankBagSize = stream->readInt(Base::Bit8);
		stream->readString(SecondPassword,COMMON_STRING_LENGTH);

		PrescriptionCount = stream->readInt(Base::Bit16);
		stream->readBits(sizeof(U32) * 8 * PrescriptionCount, &PrescriptionInfo);

		stream->readBits(sizeof(stMission) * 8,	&MissionInfo);

		for(int i = 0; i < INVENTROY_MAXSLOTS; i++)
			InventoryInfo[i].ReadData<T>(stream);

		for(int i = 0; i < BANK_MAXSLOTS; i++)
			BankInfo[i].ReadData<T>(stream);

		stream->readBits(sizeof(stShortCutInfo) * 8 * PANEL_MAXSLOTS,	&PanelInfo);
		stream->readBits(sizeof(stShortCutInfo) * 8 * ITEMSPLIT_MAXSLOTS,&SplitInfo);
		stream->readBits(sizeof(stShortCutInfo) * 8,&ComposeInfo);
		stream->readBits(sizeof(stSkillInfo) * 8 * SKILL_MAXSLOTS,	&SkillInfo);
		stream->readBits(sizeof(stLivingSkillInfo) * 8 * LIVINGSKILL_MAXSLOTS,&LivingSkillInfo);

		for(int i = 0; i < COOLDOWN_MAX; ++i)
			CooldownInfo[i].ReadData<T>(stream);
		for(int i = 0; i < PET_MAXSLOTS; ++i)
			PetInfo[i].ReadData<T>(stream);
		for(int i = 0; i < MOUNTPET_MAXSLOTS; ++i)
			MountInfo[i].ReadData<T>(stream);
		for(int i = 0; i < MaxInfluence; ++i)
			Fame[i].ReadData<T>(stream);

		SpiritOpenedFlag = stream->readInt(Base::Bit8);
		SpiritActiveSlot = stream->readInt(Base::Bit8);
		for (int i = 0; i < SPIRIT_MAXSLOTS; ++i)
		{
			SpiritInfo[i].ReadInfo<T>(stream);
		}

		for(int i = 0; i < BUFF_MAXSLOTS; ++i)
			BuffInfo[i].ReadData<T>(stream);
		for(int i = 0; i < BUFF_MAXSLOTS; ++i)
			DebuffInfo[i].ReadData<T>(stream);
		for(int i = 0; i < BUFF_MAXSLOTS; ++i)
			SysBuffInfo[i].ReadData<T>(stream);

		stream->readBits(sizeof(PlayerDataFlag) * 8,  PlayerDataFlag);
		PlayerStructVer = stream->readInt(Base::Bit32);
		return true;
	}
};

//包括一些离线易变信息，和需要数据库统计的信息
//比如帮会，结婚，师徒等社会关系，或者等级经验等需要排行的信息
struct stPlayerBase
{
	int AccountId;								//帐号ID
	int PlayerId;								//角色ID
    int PlayerFunction;							//角色功能标志
	char PlayerName[COMMON_STRING_LENGTH];		//角色名称
	int PointNum;								//点数

#pragma region 防沉迷系统相关
	bool isAdult;								//是否是成人
	int	 dTotalOnlineTime;						//累计在线时间
#pragma endregion 防沉迷系统相关
	
	int SaveUID;
    int createTime;                             //创建时间
    int lastLoginTime;                          //最近登录时间
    int lastLogoutTime;                         //最后登出时间
	int currentTime;							//本次在线时间
	int dbVersion;								//玩家数据版本

	stPlayerBase() {InitData();}
	void InitData() { memset(this, 0, sizeof(stPlayerBase));}

	template<class T>
	bool WriteData(T *pPacket)
	{
		pPacket->writeInt(AccountId,			Base::Bit32);
		pPacket->writeInt(PlayerId,				Base::Bit32);
		pPacket->writeString(PlayerName,COMMON_STRING_LENGTH);
		pPacket->writeInt(PlayerFunction,		Base::Bit32);
		pPacket->writeInt(SaveUID,				Base::Bit32);
		pPacket->writeInt(lastLogoutTime,		Base::Bit32);
		pPacket->writeInt(lastLoginTime,		Base::Bit32);
		pPacket->writeFlag(isAdult);
		pPacket->writeInt(dTotalOnlineTime,		Base::Bit32 );
		pPacket->writeInt(PointNum,				Base::Bit32 );
		return true;
	}

	template<class T>
	bool ReadData(T *pPacket)
	{
		AccountId			= pPacket->readInt(Base::Bit32);
		PlayerId			= pPacket->readInt(Base::Bit32);
		pPacket->readString(PlayerName,COMMON_STRING_LENGTH);
		PlayerFunction		= pPacket->readInt(Base::Bit32);
		SaveUID				= pPacket->readInt(Base::Bit32);
		lastLogoutTime		= pPacket->readInt(Base::Bit32);
		lastLoginTime		= pPacket->readInt(Base::Bit32);
		isAdult				= pPacket->readFlag();
		dTotalOnlineTime	= pPacket->readInt(Base::Bit32);
		PointNum			= pPacket->readInt(Base::Bit32);		
		return true;
	}
};

//玩家保存总结构
struct stPlayerStruct 
{
	stPlayerBase		BaseData;
	stPlayerDisp		DispData;
	stPlayerMain		MainData;

	stPlayerStruct() {}

	void InitData()
	{
		BaseData.InitData();
		DispData.InitData();
		MainData.InitData();
	}

	template<class T>
	bool WriteData(T* pStream)
	{
		return BaseData.WriteData<T>((T*)pStream) && DispData.WriteData<T>((T*)pStream) && MainData.WriteData<T>((T*)pStream);
	}

	template<class T>
	bool ReadData(T* pStream)
	{
		return BaseData.ReadData<T>((T*)pStream) && DispData.ReadData<T>((T*)pStream) && MainData.ReadData<T>((T*)pStream);
	}
};

//玩家查询临时结构
struct stPlayerSelect
{
	stPlayerBase		BaseData;
	stPlayerDisp		DispData;
	stPlayerSelect() { InitData();}

	void InitData()
	{
		BaseData.InitData();
		DispData.InitData();
	}

	template<class T>
	bool WriteData(T* pStream)
	{
		return BaseData.WriteData<T>(pStream) && DispData.WriteData<T>(pStream);
	}

	template<class T>
	bool ReadData(T* pStream)
	{
		return BaseData.ReadData<T>(pStream) && DispData.ReadData<T>(pStream);
	}
};

// 玩家传送信息
struct stPlayerTransferBase
{
	int nPlayerId;		// 玩家ID
	int nPlayerHp;		// HP
	int nPlayerMp;		// MP
	int nStatus;		// 当前状态
};

struct PlayerStatusType
{
	enum Type
	{
		Offline,		// 离线
		Idle,			// 空闲
		Copymap,		// 副本
		Team,			// 组队
		Fight,			// 战斗
		Business,		// 摆摊
		TempOffline,	// 暂离

		Count
	};
};

struct stPlayerTransferNormal
{
	char szPlayerName[COMMON_STRING_LENGTH];
};


const int PLAYER_STRUCT_SIZE		=	sizeof(stPlayerStruct);
const int PLAYER_DISP_SIZE			=	sizeof(stPlayerDisp);
const int PLAYER_MAIN_SIZE			=	sizeof(stPlayerMain);

#endif