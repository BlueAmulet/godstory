//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef __SHORTCUTOBJECT_H__
#define __SHORTCUTOBJECT_H__
#pragma  once
#include "platform/platform.h"
#include "core/bitStream.h"

class ShortcutObject
{
public:
	enum Constants
	{
		SHORTCUT_NONE	     = 0,	//未知对象
		SHORTCUT_ITEM	     = 1,	//物品对象
		SHORTCUT_SKILL	     = 2,	//技能对象
		SHORTCUT_LIVINGSKILL = 3,   //生活技能对象
		SHORTCUT_PET		 = 4,	//宠物对象
		SHORTCUT_MOUNT_PET	 = 5,   //骑宠对象
		SHORTCUT_SPIRIT		 = 6,	//元神对象
		SHORTCUT_TALENT		 = 7,	//天赋对象
	};

	enum SlotState
	{
		SLOT_COMMON			= 0,	// 正常
		SLOT_LOCK			= 1,	// 锁定(不可移出,如:锁定快捷栏)
		SLOT_FIXED			= 2,	// 固定(不可放入,如:技能栏)
		SLOT_FREEZE			= SLOT_LOCK | SLOT_FIXED,	// 冻结(不可移出且不可放入,
														// 如:玩家交易时,物品栏上待交易的物品槽)
	};
	
	ShortcutObject	();
	virtual ~ShortcutObject(){}

	S32 getType() const { return mShortcutType; }
	bool isItemObject();
	bool isSkillObject();
	bool isPetObject();
	bool isMountObject();
	bool isLivingSkillObject();
	bool isSpiritObject();
	bool isTalentObject();

	U32 getSlotState();
	void setSlotState(SlotState state);

	bool isLocked() { return mSlotState & SLOT_LOCK;}
	bool isFixed() { return mSlotState & SLOT_FIXED;}
	bool isFreeze() { return mSlotState & SLOT_FREEZE;}
	void lock() { mSlotState |= SLOT_LOCK; }
	void unlock() { mSlotState &= ~SLOT_LOCK;}
	void fixed() { mSlotState |= SLOT_FIXED;}
	void unfixed() { mSlotState &= ~SLOT_FIXED;}
	void freeze() { mSlotState |= SLOT_FREEZE;}
	void unfreeze() { mSlotState &= ~SLOT_FREEZE;}

	S32 getLockedItemIndex();
	void setLockedItemIndex(S32 index);
	void setShow(bool bshow);
	S32 getCooldownGroup();
	virtual StringTableEntry getIconName() = 0;
	virtual StringTableEntry getCursorName() = 0;

	virtual void packUpdate( BitStream* stream);
	virtual void unpackUpdate(BitStream* stream );

protected:
	S32 mShortcutType;
	U32 mSlotState;
	S32 mLockedIndex;
	bool mIsShow;
};

#endif//__SHORTCUTOBJECT_H__