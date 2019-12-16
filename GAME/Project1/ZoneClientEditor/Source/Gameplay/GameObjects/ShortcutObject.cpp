//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/ShortcutObject.h"
#include "Gameplay/GameObjects/SkillShortcut.h"
#include "Gameplay/item/ItemShortcut.h"
#include "Gameplay/item/Res.h"

ShortcutObject::ShortcutObject()
{
	mShortcutType = SHORTCUT_NONE;
	mIsShow	= true;
	mSlotState = SLOT_COMMON;
	mLockedIndex = -1;
}

bool ShortcutObject::isItemObject()
{
	return mShortcutType == SHORTCUT_ITEM;
}

bool ShortcutObject::isSkillObject()
{
	return mShortcutType == SHORTCUT_SKILL;
}

bool ShortcutObject::isPetObject()
{
	return mShortcutType == SHORTCUT_PET;
}

bool ShortcutObject::isMountObject()
{
	return mShortcutType == SHORTCUT_MOUNT_PET;
}

bool ShortcutObject::isSpiritObject()
{
	return mShortcutType == SHORTCUT_SPIRIT;
}

bool ShortcutObject::isTalentObject()
{
	return mShortcutType == SHORTCUT_TALENT;
}

bool ShortcutObject::isLivingSkillObject()
{
	return mShortcutType == SHORTCUT_LIVINGSKILL;
}

U32 ShortcutObject::getSlotState()
{
	return mSlotState;
}

void ShortcutObject::setSlotState(SlotState state)
{
	mSlotState = state;
}

S32 ShortcutObject::getLockedItemIndex()
{
	return mLockedIndex;
}

void ShortcutObject::setLockedItemIndex(S32 index)
{
	mLockedIndex = index;
}

void ShortcutObject::setShow(bool bshow)
{
	mIsShow = bshow;
}

S32 ShortcutObject::getCooldownGroup()
{
	if(isSkillObject())
	{
		SkillShortcut* pSkillShortcut = dynamic_cast<SkillShortcut*>(this);
		if(pSkillShortcut && pSkillShortcut->getSkill() && pSkillShortcut->getSkill()->GetData())
			return pSkillShortcut->getSkill()->GetData()->GetCDGroup();
	}
	else if(isItemObject())
	{
		ItemShortcut* pItemShortcut = static_cast<ItemShortcut*>(this);
		if(pItemShortcut && pItemShortcut->getRes())
			return pItemShortcut->getRes()->getCoolTimeType();
	}
	return -1;
}

void ShortcutObject::packUpdate( BitStream* stream)
{
#ifdef NTJ_SERVER
	if(stream->writeFlag(mIsShow))
	{
		stream->writeInt(mSlotState, 8);
		stream->writeInt(mLockedIndex, 8);
	}
#endif
}

void ShortcutObject::unpackUpdate(BitStream* stream )
{
#ifdef NTJ_CLIENT
	if(mIsShow = stream->readFlag())
	{
		mSlotState = static_cast<SlotState>(stream->readInt(8));
		mLockedIndex = stream->readInt(8);
	}
#endif
}
