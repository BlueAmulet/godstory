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
		TalentType_Stats		= 0,				// 直接添加属性，存储BuffId，mOriginId为TalentType_Stats
		TalentType_Skill,							// 技能，存储SkillId，mOriginId表示SkillSeriesId
		TalentType_Buff,							// 状态，存储BuffId，mOriginId表示BuffSeriesId
		TalentType_NewSkill,						// 新增技能，存储SkillId，mOriginId无效

													// 武器相关天赋，存储BuffId，mOriginId无效
		TalentType_Arm_A,							// 空手状态
		TalentType_Arm_B,							// 法器
		TalentType_Arm_C,							// 单短
		TalentType_Arm_D,							// 双短
		TalentType_Arm_E,							// 琴
		TalentType_Arm_F,							// 弓
		TalentType_Arm_G,							// 旗帜
		TalentType_Arm_H,							// 刀斧
		TalentType_Arm_I,							// unknown
		TalentType_Arm_J,							// unknown
	};

	struct TalentUnit
	{
		TalentType			mType;					// 类型
		U32					mOriginId;				// 记录针对哪个状态/技能进行增强
		U32					mGeneralId;				// 记录使用哪个状态/技能进行增强
	};

protected:
	U32						mTalentId;				// 天赋Id
	U32						mSeriesId;				// 天赋系列Id
	U32						mLevel;					// 天赋等级
	StringTableEntry		mName;					// 天赋名称
	StringTableEntry		mText;					// 说明文本
	StringTableEntry		mIcon;					// icon
	TalentUnit				mTalentUnits[MaxUnits];	// 天赋属性

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
		MaxRow			= MAX_TALENT_ROW,	// 行
		MaxCol			= MAX_TALENT_COL,	// 纵列
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
	U32 mId;							// 天赋树ID
	StringTableEntry mBackground;		// 背景图片
	TalentNode* mNodes[MaxRow][MaxCol];	// 天赋

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
	typedef Vector<U32> IdList;								// <对应各种ID>
	typedef stdext::hash_map<U32, U32> tActiveMap;			// <天赋系列ID, 等级> 已经激活的天赋
	typedef stdext::hash_map<U32, IdList*> tTable;			// <源ID,ID列表>

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


