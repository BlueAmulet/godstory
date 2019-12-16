//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/GameObjectData.h"
#include "Gameplay/GameplayCommon.h"

class IColumnData;
// ========================================================================================================================================
//	MountObjectData
//	MountObjectData 保存了Mount的模板信息
// ========================================================================================================================================
struct MountObjectData: public GameObjectData
{
	typedef GameObjectData Parent;
	enum
	{
		MaxMounted		= 6,				// 和组队的人数一样即可
	};

	struct MountedInfo
	{
		U32				linkPoint;
		U32				linkPointSelf;
		U32				action;
	};

	struct  LevelData
	{
		LevelData()			{ dMemset(this, 0, sizeof(LevelData)); }

		F32					mSpeed;
		StringTableEntry	mHead;
		U32					mEffect;
		StringTableEntry	mAudio;
	};

	StringTableEntry	mName;					// 骑宠名称
	U8					mLevel;					// 骑宠等级
	U32					mMountLevel;			// 玩家等级要求
	U32					buffId;					// 基础数值
	U8					mType;					// 骑宠类型
	U8					mRace;					// 骑宠种族
	StringTableEntry	mDescription;			// 骑宠说明
	U8					mCount;					// 可骑乘人数
	U32					mDuringTime;			// 时效
	U8					mTimeFlag;				// 计时方式
	U32					mSkillId[3];			// 技能ID结构
	MountedInfo			info[MaxMounted];		// 骑乘信息
	LevelData			mLevelData;				// 等级相关信息
	StringTableEntry	mMountShortcutIcon;		// 显示Icon

	//
	DECLARE_CONOBJECT(MountObjectData);
	MountObjectData();
	~MountObjectData();
	bool preload(bool server, char errorBuffer[256]);
	bool initDataBlock();
	U32 getCurrLevel();
};


// ========================================================================================================================================
//	MountRepository
//	MountRepository保存了所有MountObjectData的信息
// ========================================================================================================================================
class MountRepository
{
public:
	MountRepository();
	~MountRepository();
	typedef stdext::hash_map<U32, MountObjectData*> MountDataMap;
	MountObjectData*		GetMountData(U32 MountID);
	U32						GetMountIdByIndex(int index);
	S32						GetMountDataCount();

	bool					Insert(MountObjectData* pData);
	bool					Read();
	void					Clear();

	IColumnData*			mColumnData;
private:
	MountDataMap			m_MountDataMap;
};

extern MountRepository g_MountRepository;
