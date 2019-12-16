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
//	PetObjectData ������Pet��ģ����Ϣ
// ========================================================================================================================================
struct PetObjectData: public GameObjectData
{
	typedef GameObjectData Parent;

	enum Flags
	{
		Flags_Mutant			= BIT(0),	// ����
	};

	enum Ability
	{
		Ability_Combative		= BIT(0),	// ս��
		Ability_Combinative		= BIT(1),	// ����
		Ability_Generative		= BIT(2),	// ��ֳ
	};

	enum PetIndex
	{
		Combat_A				= 0,		// data��ֻ��ս������
		Combat_B,
		Combat_C,
		Mount,
		PetMax,
		CombatPetMax			= Combat_C,
	};

	enum InitTemplateRandom	//��ʼ�����������
	{
		PingYong_1		= 1,						//��������-ƽӹ1
		PingYong_2		= PingYong_1	<< 1,		//��������-ƽӹ50
		PingYong_3		= PingYong_1	<< 2,		//��������-ƽӹ85
		PingYong_4		= PingYong_1	<< 3,		//��������-ƽӹ95

		PingFaGong_1	= PingYong_4	<< 1,		//��������-����1
		PingFaGong_2	= PingYong_4	<< 2,		//��������-����50
		PingFaGong_3	= PingYong_4	<< 3,		//��������-����50
		PingFaGong_4	= PingYong_4	<< 4,		//��������-����50
		PingFaGong_5	= PingYong_4	<< 5,		//��������-����85
		PingFaGong_6	= PingYong_4	<< 6,		//��������-����85
		PingFaGong_7	= PingYong_4	<< 7,		//��������-����85
		PingFaGong_8	= PingYong_4	<< 8,		//��������-����95

		PingWuGong_1	= PingFaGong_8	<< 1,		//��������-�﹦1
		PingWuGong_2	= PingFaGong_8	<< 2,		//��������-�﹦50
		PingWuGong_3	= PingFaGong_8	<< 3,		//��������-�﹦50
		PingWuGong_4	= PingFaGong_8	<< 4,		//��������-�﹦50
		PingWuGong_5	= PingFaGong_8	<< 5,		//��������-�﹦85
		PingWuGong_6	= PingFaGong_8	<< 6,		//��������-�﹦85
		PingWuGong_7	= PingFaGong_8	<< 7,		//��������-�﹦85
		PingWuGong_8	= PingFaGong_8	<< 8,		//��������-�﹦95

		PingFangChong_1	= PingWuGong_8	<< 1,		//��������-����1
		PingFangChong_2	= PingWuGong_8	<< 2,		//��������-����50
		PingFangChong_3	= PingWuGong_8	<< 3,		//��������-����50
		PingFangChong_4	= PingWuGong_8	<< 4,		//��������-����50
		PingFangChong_5	= PingWuGong_8	<< 5,		//��������-����85
		PingFangChong_6	= PingWuGong_8	<< 6,		//��������-����85
		PingFangChong_7	= PingWuGong_8	<< 7,		//��������-����85
		PingFangChong_8	= PingWuGong_8	<< 8,		//��������-����95
	};

	StringTableEntry	petName;			// ��������
	StringTableEntry	petShortcutIcon;	// ������ʾ��ͼ��
	StringTableEntry	petIcon;			// ����ͷ��
	enRace				race;				// ����
	U32					flags;				// Pet���Ա�־
	U32					ability;			// ����
	U32					lives;				// ���������
	U32					combatPetIdx;		// ս��������
	U32					buffId;				// ������ֵ
	U32					defaultSkillId;		// ��������
	U32					spawnLevel;			// Я���ȼ�
	U32					TameLevel;			// ��׽�ȼ�
	U32					insight;			// ���
	U32					petTalent;			// ����
	
	U32					initialTemplate;	// ��ʼ����
	U16					minStrength;		// ����������Сֵ
	U16					maxStrength;		// �����������ֵ
	U16					minLingLi;			// ����������Сֵ
	U16					maxLingLi;			// �����������ֵ
	U16					minTiPo;			// ����������Сֵ
	U16					maxTiPo;			// �����������ֵ
	U16					minJingLi;			// ����������Сֵ
	U16					maxJingLi;			// �����������ֵ
	U16					minYuanLi;			// Ԫ��������Сֵ
	U16					maxYuanLi;			// Ԫ���������ֵ
	U16					minMinJie;			// ����������Сֵ
	U16					maxMinJie;			// �����������ֵ
	U16					minQianLi;			// Ǳ��������Сֵ
	U16					maxQianLi;			// Ǳ���������ֵ
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
//	PetRepository����������PetObjectData����Ϣ
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
