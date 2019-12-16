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
//	MountObjectData ������Mount��ģ����Ϣ
// ========================================================================================================================================
struct MountObjectData: public GameObjectData
{
	typedef GameObjectData Parent;
	enum
	{
		MaxMounted		= 6,				// ����ӵ�����һ������
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

	StringTableEntry	mName;					// �������
	U8					mLevel;					// ���ȼ�
	U32					mMountLevel;			// ��ҵȼ�Ҫ��
	U32					buffId;					// ������ֵ
	U8					mType;					// �������
	U8					mRace;					// �������
	StringTableEntry	mDescription;			// ���˵��
	U8					mCount;					// ���������
	U32					mDuringTime;			// ʱЧ
	U8					mTimeFlag;				// ��ʱ��ʽ
	U32					mSkillId[3];			// ����ID�ṹ
	MountedInfo			info[MaxMounted];		// �����Ϣ
	LevelData			mLevelData;				// �ȼ������Ϣ
	StringTableEntry	mMountShortcutIcon;		// ��ʾIcon

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
//	MountRepository����������MountObjectData����Ϣ
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
