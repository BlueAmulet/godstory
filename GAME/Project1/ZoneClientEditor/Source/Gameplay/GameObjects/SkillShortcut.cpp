//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/SkillShortcut.h"
#include "base/bitStream.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientGameplayState.h"

SkillShortcut::SkillShortcut()
{
	mShortcutType = SHORTCUT_SKILL;
	mSkill = NULL;
}

SkillShortcut::SkillShortcut(Skill* pSkill)
{
	if(!pSkill || !pSkill->GetData())
		return;
	mShortcutType = SHORTCUT_SKILL;
	mSkill = pSkill;	
}


SkillShortcut::~SkillShortcut()
{
	// 不用删除mSkill [6/29/2009 joy]
}

SkillShortcut* SkillShortcut::CreateSkill(Skill* pSkill)
{
	SkillShortcut* pSkillItem = new SkillShortcut(pSkill);
	if(pSkillItem->getSkill())
		return pSkillItem;
	else
	{
		delete pSkillItem;
		AssertFatal(0, avar("Error>>> Skill is null point! ID:%d", 0));
		return NULL;
	}
}


StringTableEntry SkillShortcut::getIconName()
{
	AssertFatal(mSkill && mSkill->GetData(), "Error>>> SkillShortcut::getIconName - Skill is null");
	return mSkill->GetData()->GetIcon();
}

StringTableEntry SkillShortcut::getCursorName()
{
	char szCursorName[128];
	const char* szIconName = getIconName();
	dSprintf(szCursorName, sizeof(szCursorName), "%s%s", "skill/", szIconName);
	char* szPos = dStrrchr(szCursorName, '.');
	if(szPos) szPos[0] = 0;
	return StringTable->insert(szCursorName);
}

void SkillShortcut::packUpdate(BitStream* stream)
{
	Parent::packUpdate(stream);
#ifdef NTJ_SERVER
	stream->writeInt(mSkill->GetData()->GetSeriesID(), Base::Bit16);
#endif
}

void SkillShortcut::unpackUpdate(BitStream* stream )
{
	Parent::unpackUpdate(stream);
#ifdef NTJ_CLIENT
	S32 iSkillSeriesID = stream->readInt(Base::Bit16);
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(player)
	{
		mSkill = player->GetSkillTable().GetSkill(iSkillSeriesID);
		if(!mSkill)
		{
			AssertFatal(0, avar("Error>>> Skill is null point! seriesid:%d", iSkillSeriesID));
		}
	}
#endif
}

//-----------------------------------------------------------------
//技能学习SHORTCUT
//-----------------------------------------------------------------
SkillStudyShortcut::SkillStudyShortcut()
{
	mShortcutType = SHORTCUT_SKILL;
	mSkillData = NULL;
}

SkillStudyShortcut::SkillStudyShortcut(SkillData* pData)
{
	if(!pData)
		return;
	mShortcutType = SHORTCUT_SKILL;
	mSkillData = pData;	
}


SkillStudyShortcut::~SkillStudyShortcut()
{
	
}

SkillStudyShortcut* SkillStudyShortcut::CreateSkillData(SkillData* pData)
{
	SkillStudyShortcut* pSkillItem = new SkillStudyShortcut(pData);
	if(pSkillItem->getSkillData())
		return pSkillItem;
	else
	{
		delete pSkillItem;
		AssertFatal(0, avar("Error>>> Skill is null point! ID:%d", 0));
		return NULL;
	}
}


StringTableEntry SkillStudyShortcut::getIconName()
{
	AssertFatal(mSkillData, "Error>>> SkillShortcut::getIconName - Skill is null");
	return mSkillData->GetIcon();
}

StringTableEntry SkillStudyShortcut::getCursorName()
{
	char szCursorName[128];
	const char* szIconName = getIconName();
	dSprintf(szCursorName, sizeof(szCursorName), "%s%s", "skill/", szIconName);
	char* szPos = dStrrchr(szCursorName, '.');
	szPos[0] = 0;
	return StringTable->insert(szCursorName);
}