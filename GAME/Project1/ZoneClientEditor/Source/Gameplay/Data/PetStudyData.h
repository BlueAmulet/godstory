//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameplayCommon.h"
#include <hash_map>

class IColumnData;
// ========================================================================================================================================
//	PetStudyData
//	PetStudyData �����˳������е�ģ����Ϣ
// ========================================================================================================================================
struct PetStudyData
{
	U32					mId;				// ���б��
	StringTableEntry	mName;				// ��������
	U8					mType;				// ��������
	U32					mPetLevel;			// ����ȼ�
	U32					mStudyTime;			// ����ʱ��
	U8					mTimeFlag;			// ��ʱ��ʽ
	U32					mCostMoney;			// ������Ϸ��
	U32					mCostHappiness;		// ���Ŀ��ֶ�
	U32					mRewardMoney;		// ������Ϸ��
	U32					mRewardPlayerExp;	// �������ﾭ��
	U32					mRewardPetExp;		// �������ﾭ��

	PetStudyData()	{ }
	~PetStudyData()	{ }
};


// ========================================================================================================================================
//	PetRepository
//	PetRepository����������PetObjectData����Ϣ
// ========================================================================================================================================
class PetStudyRepository
{
public:
	PetStudyRepository();
	~PetStudyRepository();
	typedef stdext::hash_map<U32, PetStudyData*> PetStudyDataMap;
	PetStudyData*			GetPetStudyData(U32 PetID);
	S32						GetPetStudyDataCount();

	bool					Insert(PetStudyData* pData);
	bool					Read();
	void					Clear();
	PetStudyDataMap*		GetPetStudyDataMap()	{ return &m_PetStudyDataMap; }

	IColumnData*			mColumnData;
private:
	PetStudyDataMap			m_PetStudyDataMap;
};

extern PetStudyRepository g_PetStudyRepository;
