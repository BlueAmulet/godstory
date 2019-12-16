//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _LIVINGSKILLSHORTCUT_H_
#define _LIVINGSKILLSHORTCUT_H_

#include "Gameplay/GameObjects/LivingSkill.h"
class ShortcutObject;
class LivingSkillShortcut : public ShortcutObject
{
	typedef ShortcutObject Parent;
public:
	LivingSkillShortcut();
	~LivingSkillShortcut();

	StringTableEntry getIconName();
	StringTableEntry getCursorName();
	
	void packUpdate(BitStream* stream);
	void unpackUpdate(BitStream* stream);

	LivingSkill* getLivingSkill(){ return mLivingSkill; }
	static LivingSkillShortcut* Create(U32 skillID,U32 ripe);

protected:
	LivingSkill* mLivingSkill;

};
#endif