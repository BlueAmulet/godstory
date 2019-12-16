//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gui/core/guiTypes.h"
#include "base/bitStream.h"
#include "Common/PacketType.h"
#include "Gameplay/GameObjects/ShortcutObject.h"
#include "Gameplay/GameObjects/LivingSkillShortcut.h"

LivingSkillShortcut::LivingSkillShortcut()
{
	mShortcutType = SHORTCUT_LIVINGSKILL;
	mLivingSkill = new LivingSkill();
}
LivingSkillShortcut::~LivingSkillShortcut()
{
	if(mLivingSkill)
	{
		delete mLivingSkill;
		mLivingSkill = NULL;
	}
}
LivingSkillShortcut* LivingSkillShortcut::Create(U32 skillID,U32 ripe)
{
	LivingSkillShortcut* pShortcut = new LivingSkillShortcut();
	if (pShortcut && pShortcut->mLivingSkill)
	{
		pShortcut->mLivingSkill->setData(g_LivingSkillRespository.getLivingSkillData(skillID));
		if(pShortcut->mLivingSkill->getData())
		{
			pShortcut->mLivingSkill->setRipe(ripe);
			return pShortcut;
		}
	}

	if(pShortcut)
		delete pShortcut;
	AssertFatal(0, avar("Error>>> LivingSkill is null Data! ID:%d", skillID));
	return NULL;
}

StringTableEntry LivingSkillShortcut::getIconName()
{
	AssertFatal(mLivingSkill && mLivingSkill->getData(), "Error>>> LivingSkillShortcut::getIconName - LivingSkill is null");
	return mLivingSkill->getData()->getIconName();
}

StringTableEntry LivingSkillShortcut::getCursorName()
{
	char szCursorName[128];
	const char* szIconName = getIconName();
	dSprintf(szCursorName, sizeof(szCursorName), "%s%s", "skill/", szIconName);
	char* szPos = dStrrchr(szCursorName, '.');
	if(szPos) szPos[0] = 0;
	return StringTable->insert(szCursorName);
}

void LivingSkillShortcut::packUpdate(BitStream* stream)
{
	Parent::packUpdate(stream);
#ifdef NTJ_SERVER
	if (mLivingSkill && mLivingSkill->getData())
	{
		stream->writeInt(mLivingSkill->getData()->getID(), Base::Bit32);
		stream->writeInt(mLivingSkill->getRipe(),Base::Bit32);
	}
	else
		AssertFatal(0, "LivingSkill Error!");
#endif
}

void LivingSkillShortcut::unpackUpdate(BitStream* stream)
{
	Parent::unpackUpdate(stream);
#ifdef NTJ_CLIENT
	U32 iSkillID = stream->readInt(Base::Bit32);
	U32 iRipe = stream->readInt(Base::Bit32);
	mLivingSkill->setData(g_LivingSkillRespository.getLivingSkillData(iSkillID));
	mLivingSkill->setRipe(iRipe);

	if(!mLivingSkill)
		AssertFatal(0, avar("Error>>> LivingSkill is null Data! ID:%d", iSkillID));
#endif
}
