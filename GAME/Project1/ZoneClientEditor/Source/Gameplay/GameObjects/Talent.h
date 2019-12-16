//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/SkillData.h"
#include "Gameplay/GameObjects/BuffData.h"


// ========================================================================================================================================
//  TalentData
// ========================================================================================================================================
#define TalentLevelDecimalBits 100
#define Macro_GetTalentId(s,l) ((s)*TalentLevelDecimalBits + (l))
#define Macro_GetTalentSeriesId(i) ((i)/TalentLevelDecimalBits)
#define Macro_GetTalentLevel(i) ((i)%TalentLevelDecimalBits)

class TalentData
{
	friend class TalentRepository;
	friend class TalentTable;

public:
	enum
	{
		MaxUnits			= 4,
	};

	enum TalentType
	{
		TalentType_Stats		= 0,				// ֱ��������ԣ��洢BuffId��mOriginIdΪTalentType_Stats
		TalentType_Skill,							// ���ܣ��洢SkillId��mOriginId��ʾSkillSeriesId
		TalentType_Buff,							// ״̬���洢BuffId��mOriginId��ʾBuffSeriesId
		TalentType_NewSkill,						// �������ܣ��洢SkillId��mOriginId��Ч

													// ��������츳���洢BuffId��mOriginId��Ч
		TalentType_Arm_A,							// ����״̬
		TalentType_Arm_B,							// ����
		TalentType_Arm_C,							// ����
		TalentType_Arm_D,							// ˫��
		TalentType_Arm_E,							// ��
		TalentType_Arm_F,							// ��
		TalentType_Arm_G,							// ����
		TalentType_Arm_H,							// ����
		TalentType_Arm_I,							// unknown
		TalentType_Arm_J,							// unknown
	};

	struct TalentUnit
	{
		TalentType			mType;					// ����
		U32					mOriginId;				// ��¼����ĸ�״̬/���ܽ�����ǿ
		U32					mGeneralId;				// ��¼ʹ���ĸ�״̬/���ܽ�����ǿ
	};

protected:
	U32						mTalentId;				// �츳Id
	U32						mSeriesId;				// �츳ϵ��Id
	U32						mLevel;					// �츳�ȼ�
	StringTableEntry		mName;					// �츳����
	StringTableEntry		mText;					// ˵���ı�
	StringTableEntry		mIcon;					// icon
	TalentUnit				mTalentUnits[MaxUnits];	// �츳����

public:
	TalentData();

	U32						getTalentId()			{ return mTalentId; }
	U32						getSeriesId()			{ return mSeriesId; }
	U32						getLevel()				{ return mLevel; }
	StringTableEntry		getName()				{ return mName; }
	StringTableEntry		getText()				{ return mText; }
	StringTableEntry		getIcon()				{ return mIcon; }	
};


// ========================================================================================================================================
//  TalentSetData
// ========================================================================================================================================

class TalentSetData
{
	friend class TalentRepository;
	friend class TalentTable;

public:
	enum
	{
		MaxRow			= MAX_TALENT_ROW,	// ��
		MaxCol			= MAX_TALENT_COL,	// ����
		MaxPreTalent	= 3,
	};

	struct TalentNode
	{
		U32 row;
		U32 col;
		U32 preSeriesId[MaxPreTalent];
		U32 maxLevel;
		U32 talentSeriesId;
	};

protected:
	U32 mId;							// �츳��ID
	StringTableEntry mBackground;		// ����ͼƬ
	TalentNode* mNodes[MaxRow][MaxCol];	// �츳

public:
	TalentSetData();
	~TalentSetData();
	void clear();
	TalentNode* getNode(S32 nIndex);
	StringTableEntry getBackground() { return mBackground; }
};


// ========================================================================================================================================
//  TalentRepository
// ========================================================================================================================================

class TalentRepository
{
public:
	typedef stdext::hash_map<U32, TalentData*> TalentDataMap;
	typedef stdext::hash_map<U32, TalentSetData*> TalentSetDataMap;

protected:
	TalentDataMap			mTalentDataMap;
	TalentSetDataMap		mTalentSetDataMap;

public:
	TalentRepository();
	~TalentRepository();

	void					clear					();
	void					read					();
	bool					insert					(TalentData* pData);
	TalentData*				getTalent				(U32 TalentId);
	TalentSetData*			getTalentSet			(U32 TalentSetId);
};
extern TalentRepository g_TalentRepository;


// ========================================================================================================================================
//  TalentTable
// ========================================================================================================================================

class TalentTable
{
public:
	typedef Vector<U32> IdList;								// <��Ӧ����ID>
	typedef stdext::hash_map<U32, U32> tActiveMap;			// <�츳ϵ��ID, �ȼ�> �Ѿ�������츳
	typedef stdext::hash_map<U32, IdList*> tTable;			// <ԴID,ID�б�>

protected:
	Player*					mPlayer;
	tActiveMap				mActiveMap;
	tTable					mTable;

public:
	TalentTable();
	~TalentTable();

	void					init					(Player* player);
	void					clear					();
	IdList*					getIdList				(U32 originId);
	bool					addTalent				(U32 talentId);
	void					removeTalent			(U32 talentId);

	void					onArm					(U32 arm);
	void					onUnarm					(U32 arm);
};


