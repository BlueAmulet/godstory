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
		SHORTCUT_NONE	     = 0,	//δ֪����
		SHORTCUT_ITEM	     = 1,	//��Ʒ����
		SHORTCUT_SKILL	     = 2,	//���ܶ���
		SHORTCUT_LIVINGSKILL = 3,   //����ܶ���
		SHORTCUT_PET		 = 4,	//�������
		SHORTCUT_MOUNT_PET	 = 5,   //������
		SHORTCUT_SPIRIT		 = 6,	//Ԫ�����
		SHORTCUT_TALENT		 = 7,	//�츳����
	};

	enum SlotState
	{
		SLOT_COMMON			= 0,	// ����
		SLOT_LOCK			= 1,	// ����(�����Ƴ�,��:���������)
		SLOT_FIXED			= 2,	// �̶�(���ɷ���,��:������)
		SLOT_FREEZE			= SLOT_LOCK | SLOT_FIXED,	// ����(�����Ƴ��Ҳ��ɷ���,
														// ��:��ҽ���ʱ,��Ʒ���ϴ����׵���Ʒ��)
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