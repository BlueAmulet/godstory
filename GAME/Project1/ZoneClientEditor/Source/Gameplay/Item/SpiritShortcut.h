//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef __SPIRITSHORTCUT_H__
#define __SPIRITSHORTCUT_H__
#pragma once
#include "Gameplay/GameObjects/ShortcutObject.h"

struct stSpiritInfo;

class SpiritShortcut : public ShortcutObject
{
	typedef ShortcutObject Parent;
public:
	enum TARGET_TYPE
	{
		TARGET_SELF		= 1,	//指向玩家自己身上的元神
		TARGET_OTHER	= 2,	//指向其他玩家身上的元神
	};

private:
	SpiritShortcut(TARGET_TYPE type = TARGET_SELF);
	SpiritShortcut(S32 nSlotIndex, TARGET_TYPE type = TARGET_SELF);
public:
	static SpiritShortcut* CreateEmptySpiritItem(TARGET_TYPE type = TARGET_SELF);
	static SpiritShortcut* CreateSpiritItem(S32 nSlotIndex, TARGET_TYPE type = TARGET_SELF);

	~SpiritShortcut();
	StringTableEntry getIconName();
	StringTableEntry getCursorName();
	S32 getSlotIndex() { return mSlotIndex; }
	void setSlotIndex(S32 nSlotIndex) { mSlotIndex = nSlotIndex; }
	bool IsValid()  { return mSlotIndex != -1;}
	TARGET_TYPE	 getTargetType() { return mTargetType; }
	void SetTargetType(TARGET_TYPE type)	{ mTargetType = type; }

	virtual void packUpdate(BitStream* stream);
	virtual void unpackUpdate(BitStream* stream);

private:
	S32 mSlotIndex;			// 物品资源数据对象
	TARGET_TYPE mTargetType;
};

#endif