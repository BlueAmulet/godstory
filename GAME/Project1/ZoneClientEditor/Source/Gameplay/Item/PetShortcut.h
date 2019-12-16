//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef __PETSHORTCUT_H__
#define __PETSHORTCUT_H__
#pragma once
#include "Gameplay/GameObjects/ShortcutObject.h"

struct stPetInfo;

class PetShortcut : public ShortcutObject
{
	typedef ShortcutObject Parent;
public:
	enum TARGET_TYPE
	{
		TARGET_SELF		= 1,	//指向玩家自己身上的宠物
		TARGET_OTHER	= 2,	//指向其他玩家身上的宠物
	};

private:
	PetShortcut(TARGET_TYPE type = TARGET_SELF);
	PetShortcut(S32 nSlotIndex, TARGET_TYPE type = TARGET_SELF);
public:
	static PetShortcut* CreateEmptyPetItem(TARGET_TYPE type = TARGET_SELF);
	static PetShortcut* CreatePetItem(S32 nSlotIndex, TARGET_TYPE type = TARGET_SELF);

	~PetShortcut();
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


class MountShortcut : public ShortcutObject
{
	typedef ShortcutObject Parent;

private:
	MountShortcut();
	MountShortcut(S32 nSlotIndex);
public:
	static MountShortcut* CreateEmptyMountItem();
	static MountShortcut* CreateMountItem(S32 nSlotIndex);

	~MountShortcut();
	StringTableEntry getIconName();
	StringTableEntry getCursorName();
	S32 getSlotIndex() { return mSlotIndex; }
	void setSlotIndex(S32 nSlotIndex) { mSlotIndex = nSlotIndex; }
	bool IsValid()  { return mSlotIndex != -1;}

	virtual void packUpdate(BitStream* stream);
	virtual void unpackUpdate(BitStream* stream);

private:
	S32 mSlotIndex;			// 物品资源数据对象
};

#endif//__ITEMSHORTCUT_H__