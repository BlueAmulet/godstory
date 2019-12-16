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
	Family_None = 0,			// �ޣ����Ĭ��
	Family_Sheng,				// ʥ��������
	Family_Fo,					// ��������
	Family_Xian,				// �ɣ�������
	Family_Jing,				// �����ɻ���
	Family_Gui,					// �����Ľ�
	Family_Guai,				// �֣�ɽ����
	Family_Yao,					// �������鹬
	Family_Mo,					// ħ����ħ��
	MaxFamilies,
};

static char* g_strFamily[MaxFamilies] = 
{
    "��",
    "������",
    "������",
    "������",
    "�ɻ���",
    "���Ľ�",
    "ɽ����",
    "���鹬",
    "��ħ��",
};

enum enInfluence
{
	Influence_None = 0,			// �ޣ����Ĭ��
	Influence_Sheng,			// ʥ��������
	Influence_Fo,				// ��������
	Influence_Xian,				// �ɣ�������
	Influence_Jing,				// �����ɻ���
	Influence_Gui,				// �����Ľ�
	Influence_Guai,				// �֣�ɽ����
	Influence_Yao,				// �������鹬
	Influence_Mo,				// ħ����ħ��
	PlayerFamilies,

	Influence_Npc = PlayerFamilies,// NPCĬ��
	Influence_People,			// ��
	Influence_Villain,			// ����
	Influence_Animal,			// ����
	Influence_Monster,			// ����
	Influence_WildAnimal,		// Ұ��
	Influence_Beast,			// ����

	// other
	Influence_Ex1,
	Influence_Ex2,
	Influence_Ex3,

	TotalInfluence,
	MaxInfluence = 64,			// ���64��
};

static char* g_strInfluence[MaxInfluence] = 
{
    "��",
    "������",
    "������",
    "������",
    "�ɻ���",
    "���Ľ�",
    "ɽ����",
    "���鹬",
    "��ħ��",

	"����",
	"��",
	"����",
	"����",
	"����",
	"Ұ��",
	"����",

	"δ��1",
	"δ��2",
	"δ��3",
};

enum enRace
{
	Race_Sheng = 0,				// ʥ
	Race_Fo,					// ��
	Race_Xian,					// ��
	Race_Jing,					// ��
	Race_Gui,					// ��
	Race_Guai,					// ��
	Race_Yao,					// ��
	Race_Mo,					// ħ

	Race_Ren,					// ��
	Race_Shen,					// ��
	Race_YeShou,				// Ұ��
	Race_DiRen,					// ص��
	Race_KuaFu,					// �丸
	Race_LongBo,				// ����
	Race_WuLing,				// ����
	Race_YuRen,					// ����
	MaxRaces,
};

static char* g_strRace[MaxRaces] = 
{
	"ʥ",
	"��",
	"��",
	"��",
	"��",
	"��",
	"��",
	"ħ",

	"��",
	"��",
	"Ұ��",
	"ص��",
	"�丸",
	"����",
	"����",
	"����",
};

// ����ְҵ
enum enClassesMaster
{
	_1st,
	_2nd,
	_3rd,
	_4th,
	_5th,
	MaxClassesMasters,
};

// Ч������(APF)
enum enActivePropertyFlag
{
	EAPF_BIND			= 1 << 0,	// ��Ч��
	EAPF_ATTACH			= 1 << 1,	// ����Ч��
	EAPF_WUXING			= 1 << 2,	// ����Ч��
	EAPF_IMPRESS		= 1 << 3,	// װ����������
	EAPF_EQUIPSKILLA	= 1 << 4,	// װ������������
	EAPF_EQUIPSKILLB	= 1 << 5,	// װ���󱻶�����		
};

// ��Ʒ���ݽṹ
struct stItemInfo
{
	char Producer[COMMON_STRING_LENGTH];// ����������
	U64 UID;							// ��Ʒ����ΨһID
	U32 ItemID;							// ��Ʒģ��ID
	U32 Quantity;						// ��Ʒ����(����װ����ֻ��Ϊ1)
	U32 BindPlayer;						// ����ID(Ҳ�����ڴ洢������ӿ��������ID)
	U32 BindFriend;						// �󶨺��ѽ�ɫID����ʹ����Ʒ

	U32 ActiveFlag;						// ��Ʒ�����־
	U32	Quality;						// ��ƷƷ�ʵȼ�
	S32 LapseTime;						// ʣ������
	S32 RemainUseTimes;					// ʣ��ʹ�ô���
	U32 LockedLeftTime;					// ��Ʒ��ʣ��ʱ�䣨�룩

	S32 CurWear;						// ��ǰ�;ö�
	S32 CurMaxWear;						// ��ǰ����;ö�
	S32 CurAdroit;						// ��ǰ������
	U32 CurAdroitLv;					// ��ǰ�����ȵȼ�

	U32 ActivatePro;					// ��������Ч�������־

	U32 RandPropertyID;					// �����������ID
	U32 IDEProNum;						// �����󸽼����Ը���
	U32 IDEProValue[MAX_IDEPROS];		// �����󸽼�����ֵ

	U32 EquipStrengthens;				// װ��ǿ��������
	U32 EquipStrengthenValue[MAX_EQUIPSTENGTHENS][2];

	U32 ImpressID;						// ����ID(״̬ID)
	U32 BindProID;						// ���󶨸�������(״̬ID)

	U32 WuXingID;						// ����ID
	U32 WuXingLinkID;					// ��������ID
	U32 WuXingPro;						// ����Ч������

	U32 SkillAForEquip;					// װ���ϻ�ȡ����������ID
	U32 SkillBForEquip;					// װ���ϻ�ȡ�ı�������ID

	U32 EmbedOpened;					// ������Ƕ�ף�ÿ4bitλ����һ���ף�
	U32	EmbedSlot[MAX_EMBEDSLOTS];		// װ����Ƕ�׵ı�ʯID

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

		// �ж��Ƿ�װ��
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
			// �ж��Ƿ񼤻���������
			if(ActivatePro & EAPF_IMPRESS)
			{
				stream->writeInt(ImpressID,		Base::Bit32);
			}

			// �ж��Ƿ񼤻������
			if(ActivatePro & EAPF_BIND)
			{
				stream->writeInt(BindProID,		Base::Bit32);
			}

			// �ж��Ƿ񼤻���������
			if(ActivatePro & EAPF_WUXING)
			{
				stream->writeInt(WuXingID,		4);
				stream->writeInt(WuXingLinkID,	4);
				stream->writeInt(WuXingPro,		Base::Bit32);
			}

			// �ж��Ƿ񼤻�����󸽼�����
			if(ActivatePro & EAPF_ATTACH)
			{
				stream->writeInt(RandPropertyID,  10);
				stream->writeInt(IDEProNum,		4);
				for(int i = 0; i < IDEProNum; i++)
					stream->writeInt(IDEProValue[i],	Base::Bit32);
			}

			// �ж��Ƿ񼤻�ı�װ��������������
			if(ActivatePro & EAPF_EQUIPSKILLA)
			{
				stream->writeInt(SkillAForEquip,	Base::Bit32);
			}

			// �ж��Ƿ񼤻�ı�װ�����ӱ�������
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

		// �ж��Ƿ�װ��
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
			// �ж��Ƿ񼤻���������
			if(ActivatePro & EAPF_IMPRESS)
			{
				ImpressID	= stream->readInt(Base::Bit32);
			}

			// �ж��Ƿ񼤻������
			if(ActivatePro & EAPF_BIND)
			{
				BindProID	= stream->readInt(Base::Bit32);
			}

			// �ж��Ƿ񼤻���������
			if(ActivatePro & EAPF_WUXING)
			{
				WuXingID		= stream->readInt(4);
				WuXingLinkID	= stream->readInt(4);
				WuXingPro		= stream->readInt(Base::Bit32);
			}

			// �ж��Ƿ񼤻�����󸽼�����
			if(ActivatePro & EAPF_ATTACH)
			{
				RandPropertyID	= stream->readInt(10);
				IDEProNum		= stream->readInt(4);
				for(U32 i = 0; i < IDEProNum; i++)
					IDEProValue[i] = stream->readInt(Base::Bit32);
			}
			
			// �ж��Ƿ񼤻�ı�װ��������������
			if(ActivatePro & EAPF_EQUIPSKILLA)
			{
				SkillAForEquip		= stream->readInt(Base::Bit32);
			}

			// �ж��Ƿ񼤻�ı�װ�����ӱ�������
			if(ActivatePro & EAPF_EQUIPSKILLB)
			{
				SkillBForEquip		= stream->readInt(Base::Bit32);
			}
		}
		return true;
	}
};


// ��ҳ���ṹ
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

	U32			petDataId;						//ģ��ID
	char		icon[16];						//ͼ��
	U32			id;								//ΨһID
	U32			partnerId;						//��żID
	char		name[COMMON_STRING_LENGTH];		//����
	U8			sex;							//�Ա�
	U8			style;							//���� (���ޱ��������챦�������ޡ���������)
	
	bool		mature;							//�Ƿ����
	U8			status;							//״̬ ������ʶ����ս��ʶ�������ʶ��������ʶ���򹤱�ʶ�����б�ʶ
	U8			generation;						//����
	char		message[PET_MESSAGE_LENGTH];	//ǩ��
	U64			title;							//�ƺű�ʶ
	U8			titleEnabled;					//��ǰ�ƺ�
	U16			level;							//��ǰ�ȼ�
	U32			lives;							//��ǰ����

	U32			curHP;							//��ǰ����
	U32			curMP;							//��ǰ����
	U32			curPP;							//��ǰԪ��

	U16			staminaPts;						//�������Ե�
	U16			ManaPts;						//�������Ե�
	U16			strengthPts;					//�������Ե�
	U16			intellectPts;					//�������Ե�
	U16			agilityPts;						//�����Ե�
	U16			pneumaPts;						//Ԫ�����Ե�
	U16			statsPoints;					//ʣ�����Ե�

	U8			insight;						//����
	U8			petTalent;						//����


	U32			chengZhangLv;					//�ɳ���
	U16			qianLi;							//Ǳ��
	U16			quality;						//Ʒ��
	U16			staminaGift;					//��������
	U16			ManaGift;						//��������
	U16			strengthGift;					//��������
	U16			intellectGift;					//��������
	U16			agilityGift;					//������
	U16			pneumaGift;						//Ԫ������

	U8			happiness;						//����
	U32			exp;							//����

	U16			charPoints;						//ʣ���Ը��

	U32			Combat[MaxPetSkills];			//ս������
	U32			Combo[MaxPetSkills];			//���弼��
	U32			randBuffId;						//�����ֵ--����data��ָ��

	U32			mLeftTime;						//��¼5����ʣ�µ�ʱ�䣨���ڿ۳�������ֶȵ�׼ȷ��ʱ��

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

// ������ṹ
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

//���Ԫ��ṹ
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

	U32		mDataId;									//Ԫ��Data���
	U16		mLevel;										//Ԫ��ǰ�ȼ�
	U8		mJingJieLevel;								//Ԫ�񾳽�ȼ�
	
	U16		mTalentExtPts;								//�츳��չ����
	U16		mTalentLeftPts;								//�츳ʣ�����
	U8		mTalentSkillPts[MAX_TALENT];				//�츳��������ĵ���

	U8		mOpenedSkillFlag;							//�ѿ����ľ�����
	U32		mSkillId[MAX_SPIRIT_SKILL];					//����ID
	U32		mEquipId[MaxEquipId];						//װ��ID
	U32		mCleanPtsCount;								//��ǰϴ�����

	U32		mValueBuffId;								//Ԫ����ֵ״̬(��������ͻ�����ֵģ��)
	U32		mCostBuffId;								//Ԫ������״̬
	U32		mCurrExp;									//Ԫ��ǰ����

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

// ��Ҽ��ܽṹ
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

// ���״̬�ṹ
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

// ��ȴʱ��ṹ
struct stCooldownInfo 
{
	enum
	{
		CooldownSecBits = 18,	// �룬�������
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
//�����
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

//����ṹ�п�������Ҵ����ṹ(��������ʾ�õ�΢С�ṹ)
//���������ò������װ��������
struct stPlayerDisp
{
	//���ѹ���Ļ������ݽṹ
	int PlayerOnlineTime;						//��ɫ�����ۼ�ʱ��
	int	 Level;									//��ɫ�ȼ�
	int  ZoneId;								//���ߵ�ͼ���(��¼��������)
	int  TriggerNo;								//����������(��ʾ����������)
	int  fId;									//�������
	int  fTime;									//��������ʱ�䣬�����Ϊ�˷�ֹ����ظ�
	int  fZoneId;								//���߸�����ͼ���(��¼��������)
	int  fTriggerNo;							//��������������(��ʾ����������)
	int  FactionId;								//�����
	int  FactionExp;							//��ṱ��
	char LeavePos[COMMON_STRING_LENGTH];		//�뿪��ͼʱ��λ��
	char fLeavePos[COMMON_STRING_LENGTH];		//�뿪����ʱ��λ��
	char Sex;									//�Ա�
	char Race;									//����
	char Family;								//����
	int	 honour;								//����
	char Classes[MaxClassesMasters];			//ְҵ
	U32  OtherFlag;								//������־λ(����:�Ƿ���ʾʱװ,�Ƿ�����)
	U32  CopyMapInstId;							//����ʵ��ID
	U32  flyPath;								//����·��
	U32  flyPos;								//����λ��
	U32  flyMount;								//�������

	//////////////////////////////////////////////////////////////////////////
	// ����Ϊ�ͻ��˻����һЩ��ʱ���ݣ����������ݴ��������ݿ�洢
#ifdef NTJ_CLIENT
	U32	 TeamId;
#endif

	//��ʾ���������ݽṹ
	U16 Body;		//����
	U16 Face;		//����
	U16 Hair;		//����
	char HairCol;	//��ɫ

	stItemInfo	EquipInfo[EQUIP_MAXSLOTS];		// װ������Ʒ����

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

// �����������ݽṹ(244)
struct stMissionFlag
{
	S16			MissionID;					//������
	S16			Name[MISSION_FLAG_MAX];		//���������
	S32			Value[MISSION_FLAG_MAX];	//�������ֵ
};

// �����ѭ������ṹ(8)
struct stMissionCycle
{
	S16			MissionID;					//������
	U16			EndTimes;					//��ɴ���
	U32			EndTime;					//���ʱ��
};

//���������ṹ(8976)
struct stMission
{
	U8	State[MISSION_STATE_LENGTH];			//�������״̬
	stMissionCycle	Cycle[MISSION_CYCLE_MAX];	//ѭ��������Ϣ
	stMissionFlag	Flag[MISSION_ACCEPT_MAX];	//�ѽ��������
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

//ʱ�䴥�����ṹ
struct stTimerTriggerInfo
{
	U32					m_nLeftTime;		//����ʱ��
	U32					m_nIntervalTime;	//ˢ��ʱ��
	bool				m_bCanCancel;		//�ܷ�ȡ��
	U32					m_nState;			//��ǰ״̬
	U32					m_nTimeFlag;		//��ʱ��Ϊ
	U32					m_Id;				//��ʱ��ID
	U32					m_nItemId[4];		//��������ƷID
};

//ʱ�䴥�����������нṹ
struct stTimerTriggerPetStudyInfo
{
	U32					m_nLeftTime;		//ʣ�����ʱ��
	U32					m_nTotalTime;		//��ʱ��
	U32					m_nStudyDataId;		//����data ID
	U32					m_nIntervalTime;	//ˢ��ʱ��
	U32					m_nTimeFlag;		//��ʱ��Ϊ
	U32					m_Id;				//��ʱ��ID
};

// ��������ݽṹ
struct stShortCutInfo
{
	U8	ShortCutType;		//���������(0-��Ʒ, 1-���� ,2-�����)
	U16	ShortCutNum;		//���������
	U32	ShortCutID;			//��������(��Ʒ��Ż��ܱ��)
	stShortCutInfo():ShortCutType(0),ShortCutNum(0),ShortCutID(0) {}
};

// �������ϵ�ṹ
struct stSocialItem
{
	int	playerId;
	SocialType::Type type;				// SocialType
	int friendValue;					// ���Ѷ�
};

// ��������
struct stFame
{
	S8 fame;							// ����
	S32 favour;							// �øж�

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

//����ṹ�п����������Ҫ���ݽṹ
//����������񣬱��������ܵ�
struct stPlayerMain
{
	int SocialItemCount;								// �������ϵ����
	stSocialItem SocialItem[SOCAIL_ITEM_MAX];			// �������ϵ
	U8   MasterLevel;                                   // ʦ���ʸ�ȼ�
	U32  CurrMasterExp;                                 // ��ǰʦ��
	U32  ExpPool;                                       // �����
	bool bPrentice;                                     // �Ƿ�ͽ״̬
	bool bFarPrentice;                                  // ��ʦ���
	U16  MPMStudyLevel;                                 // ��ʦ�ȼ�
	char MasterText[MPM_CALLBOARD_MAXSTRING];           // ʦ��������Ϣ 

	int playerHot;										// ��Եֵ

	U32 CurrentHP;										// ��ǰѪ��ֵ
	U32 CurrentMP;										// ��ǰ����ֵ
	U32 CurrentPP;										// ��ǰԪ��ֵ
	U32 CurrentVigor;									// ��ǰ����ֵ
	U32 MaxVigor;                                       // ������ֵ
	U32 CurrentExp;										// ��ǰ����ֵ
	U32 CurrentLivingExp;                               // ��ǰ�����ֵ
	U32 LivingLevel;                                    // ����ȼ�
	U32 Money;											// ��Ǯ
	U32 BindMoney;										// �󶨽�Ǯ
	U32 BankMoney;										// ���вֿ���
	U32 TradeLockFlag;									// �ڶ����뽻�������ܱ�־
	char SecondPassword[COMMON_STRING_LENGTH];			// �ڶ�����
	U32 PrescriptionCount;                              // ӵ���䷽��
	U32 PrescriptionInfo[PRESCRIPTION_MAX];             // �䷽����
	U32 BankBagSize;									// ��ҹ�������а���
	U32 LivingSkillStudyID;                             // ����ר�������ID

	S32			TimerTriggerCount;						//ʣ��ʱ�䴥��������
	stTimerTriggerInfo	TimerTriggerInfo[MAX_TIMER_SIZE]; //ʱ�䴥��������
	S32			TimerPetStudyCount;						//��������ʱ�䴥��������
	stTimerTriggerPetStudyInfo TimerPetStudyInfo[PET_MAXSLOTS];	//��������ʱ�䴥��������
	
	stMission	MissionInfo;
	stItemInfo	InventoryInfo[INVENTROY_MAXSLOTS];		// ��Ʒ������
	stItemInfo	BankInfo[BANK_MAXSLOTS];				// ���вֿ���Ʒ����
	stShortCutInfo PanelInfo[PANEL_MAXSLOTS];			// �������Ʒ��������
	stShortCutInfo SplitInfo[ITEMSPLIT_MAXSLOTS];       // �ֽ�������� 
	stShortCutInfo ComposeInfo;                         // �ϳɲ�������
	stSkillInfo	SkillInfo[SKILL_MAXSLOTS];				// ��������
	stCooldownInfo CooldownInfo[COOLDOWN_MAX];			// ��ȴʱ������
	stLivingSkillInfo LivingSkillInfo[LIVINGSKILL_MAXSLOTS]; //���������
	stPetInfo PetInfo[PET_MAXSLOTS];					// ��������
	stMountInfo MountInfo[MOUNTPET_MAXSLOTS];			// �������
	stFame Fame[MaxInfluence];							// ��������

	U8	SpiritOpenedFlag;								// �ѿ���Ԫ����(bitλ��ʾ)
	S8	SpiritActiveSlot;								// ��ǰ�����Ԫ��
	bool bSpiritHuanHua;								// ��ǰ�����Ԫ���Ƿ�����˻û�
	stSpiritInfo SpiritInfo[SPIRIT_MAXSLOTS];			// Ԫ������

	stBuffInfo	BuffInfo[BUFF_MAXSLOTS];				// buff����
	stBuffInfo	DebuffInfo[BUFF_MAXSLOTS];				// debuff����
	stBuffInfo	SysBuffInfo[BUFF_MAXSLOTS];				// sysbuff����

	U8	 PlayerDataFlag[MAX_PLAYERDATAFLAGS];			// �߻�������������	
	U32	 PlayerStructVer;								// <<�������ݽṹ�汾ֵ,�����ⲿ���߰汾��ͬ�����������>>

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

//����һЩ�����ױ���Ϣ������Ҫ���ݿ�ͳ�Ƶ���Ϣ
//�����ᣬ��飬ʦͽ������ϵ�����ߵȼ��������Ҫ���е���Ϣ
struct stPlayerBase
{
	int AccountId;								//�ʺ�ID
	int PlayerId;								//��ɫID
    int PlayerFunction;							//��ɫ���ܱ�־
	char PlayerName[COMMON_STRING_LENGTH];		//��ɫ����
	int PointNum;								//����

#pragma region ������ϵͳ���
	bool isAdult;								//�Ƿ��ǳ���
	int	 dTotalOnlineTime;						//�ۼ�����ʱ��
#pragma endregion ������ϵͳ���
	
	int SaveUID;
    int createTime;                             //����ʱ��
    int lastLoginTime;                          //�����¼ʱ��
    int lastLogoutTime;                         //���ǳ�ʱ��
	int currentTime;							//��������ʱ��
	int dbVersion;								//������ݰ汾

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

//��ұ����ܽṹ
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

//��Ҳ�ѯ��ʱ�ṹ
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

// ��Ҵ�����Ϣ
struct stPlayerTransferBase
{
	int nPlayerId;		// ���ID
	int nPlayerHp;		// HP
	int nPlayerMp;		// MP
	int nStatus;		// ��ǰ״̬
};

struct PlayerStatusType
{
	enum Type
	{
		Offline,		// ����
		Idle,			// ����
		Copymap,		// ����
		Team,			// ���
		Fight,			// ս��
		Business,		// ��̯
		TempOffline,	// ����

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