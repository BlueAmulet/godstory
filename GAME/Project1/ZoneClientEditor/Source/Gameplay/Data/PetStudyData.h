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
//	PetStudyData 保存了宠物修行的模板信息
// ========================================================================================================================================
struct PetStudyData
{
	U32					mId;				// 修行编号
	StringTableEntry	mName;				// 修行名称
	U8					mType;				// 修行类型
	U32					mPetLevel;			// 宠物等级
	U32					mStudyTime;			// 修行时间
	U8					mTimeFlag;			// 计时方式
	U32					mCostMoney;			// 消耗游戏币
	U32					mCostHappiness;		// 消耗快乐度
	U32					mRewardMoney;		// 奖励游戏币
	U32					mRewardPlayerExp;	// 奖励人物经验
	U32					mRewardPetExp;		// 奖励宠物经验

	PetStudyData()	{ }
	~PetStudyData()	{ }
};


// ========================================================================================================================================
//	PetRepository
//	PetRepository保存了所有PetObjectData的信息
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
