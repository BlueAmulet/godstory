//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once
#include "Gameplay/GameObjects/ShortcutObject.h"
#include "Gameplay/GameObjects/Skill.h"

class SkillShortcut : public ShortcutObject
{
	typedef ShortcutObject Parent;
private:	
	SkillShortcut(Skill* pSkill);
public:
	SkillShortcut();
	static SkillShortcut* CreateSkill(Skill* pSkill);

	~SkillShortcut();
	StringTableEntry getIconName();
	StringTableEntry getCursorName();

	bool IsValid()  { return mSkill != NULL;}

	virtual void packUpdate(BitStream* stream);
	virtual void unpackUpdate(BitStream* stream );
	
	Skill* getSkill(){ return mSkill; }
private:
	Skill* mSkill;
};



//¼¼ÄÜÑ§Ï°SHORTCUT
class SkillStudyShortcut : public ShortcutObject
{
	typedef ShortcutObject Parent;
private:	
	SkillStudyShortcut(SkillData* pData);
public:
	SkillStudyShortcut();
	static SkillStudyShortcut* CreateSkillData(SkillData* pData);

	~SkillStudyShortcut();
	StringTableEntry getIconName();
	StringTableEntry getCursorName();

	bool IsValid()  { return mSkillData != NULL;}

	SkillData* getSkillData(){ return mSkillData; }

private:
	SkillData* mSkillData;
};