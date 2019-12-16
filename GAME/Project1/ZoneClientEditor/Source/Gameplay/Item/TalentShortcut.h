//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef __TALENTSHORTCUT_H__
#define __TALENTSHORTCUT_H__
#pragma once
#include "Gameplay/GameObjects/ShortcutObject.h"
#include "Gameplay/GameObjects/Talent.h"

struct stSpiritInfo;
struct TalentSetData::TalentNode;
class TalentData;

class TalentShortcut : public ShortcutObject
{
	friend class TalentData;
	typedef ShortcutObject Parent;

public:
	enum TARGET_TYPE
	{
		TARGET_SELF		= 1,	//指向玩家自己身上的元神天赋
		TARGET_OTHER	= 2,	//指向其他玩家身上的元神天赋
	};

private:
	TalentShortcut								(TARGET_TYPE type = TARGET_SELF);
	TalentShortcut								(S32 nSpiritSlot, S32 nTalentIndex, TARGET_TYPE type = TARGET_SELF);

public:	

#ifdef NTJ_CLIENT
	void update();
#endif

	static TalentShortcut* CreateEmptyTalentItem(TARGET_TYPE type = TARGET_SELF);
	static TalentShortcut* CreateTalentItem		(S32 nSpiritSlot, S32 nTalentIndex, TARGET_TYPE type = TARGET_SELF);

	~TalentShortcut								();
	StringTableEntry getIconName				();
	StringTableEntry getCursorName				();	

	bool isValid();
	virtual void packUpdate						(BitStream* stream);
	virtual void unpackUpdate					(BitStream* stream);

	TARGET_TYPE	 getTargetType()				{ return mTargetType; }
	void setTargetType(TARGET_TYPE type)		{ mTargetType = type; }
	TalentSetData::TalentNode* getTalentNode()	{ return mTalentNode; }
	TalentData* getTalentData()					{ return mTalentData; }

private:
	S32 mSpiritSlot;
	S32 mTalentIndex;
	TARGET_TYPE mTargetType;
	TalentSetData::TalentNode *mTalentNode;
	TalentData *mTalentData;
};
#endif