#include "math/mMath.h"
#include "core/stringTable.h"
#include "base/bitStream.h"
#include "Common/PlayerStruct.h"
#include "Gameplay/Item/TalentShortcut.h"
#include "Gameplay/GameObjects/Talent.h"
#include "Gameplay/GameObjects/SpiritTable.h"
#include "Gameplay/Data/SpiritData.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

TalentShortcut::TalentShortcut(TARGET_TYPE type/* = TARGET_SELF*/)
{
	mShortcutType = SHORTCUT_TALENT;
	mSpiritSlot = -1;
	mTalentIndex = -1;
	mTargetType = type;
}

TalentShortcut::TalentShortcut( S32 nSpiritSlot, S32 nTalentIndex, TARGET_TYPE type/* = TARGET_SELF*/ )
{
	mShortcutType = SHORTCUT_TALENT;
	mSpiritSlot = nSpiritSlot;
	mTalentIndex = nTalentIndex;
	mTargetType = type;
#ifdef NTJ_CLIENT
	update();
#endif
}

#ifdef NTJ_CLIENT
void TalentShortcut::update()
{
	mTalentNode = NULL;
	mTalentData = NULL;

	Player *pSelf = g_ClientGameplayState->GetControlPlayer();
	Player *pPlayer = (mTargetType == TARGET_SELF) ?  pSelf : dynamic_cast<Player*>(pSelf->getInteraction());
	if (!pPlayer)
		goto done;
	SpiritTable &table = (SpiritTable&)pPlayer->getSpiritTable();
	stSpiritInfo *info = table.getSpiritInfo(mSpiritSlot);
	if (!info)	
		goto done;
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(info->mDataId);
	if (!pSpiritData)
		goto done;
	TalentSetData *pTalentSetData = g_TalentRepository.getTalentSet(pSpiritData->mTalentSetId);
	if (!pTalentSetData)
		goto done;
	mTalentNode = pTalentSetData->getNode(mTalentIndex);
	if (!mTalentNode)
		goto done;
	S32 nLevel = pPlayer->getSpiritTable().getSpiritInfo(mSpiritSlot)->mTalentSkillPts[mTalentIndex];
	if (nLevel == 0)	//如果天赋等级为0，用一级的天赋代替
		nLevel = 1;
	U32 nTalentId = Macro_GetTalentId(mTalentNode->talentSeriesId, nLevel);
	mTalentData = g_TalentRepository.getTalent(nTalentId);
	return;

done:
	mTalentNode = NULL;
	mTalentData = NULL;
}
#endif

TalentShortcut* TalentShortcut::CreateEmptyTalentItem(TARGET_TYPE type/* = TARGET_SELF*/)
{
	return new TalentShortcut(type);
}

TalentShortcut* TalentShortcut::CreateTalentItem( S32 nSpiritSlot, S32 nTalentIndex, TARGET_TYPE type/* = TARGET_SELF*/ )
{
	return new TalentShortcut(nSpiritSlot, nTalentIndex, type);
}

TalentShortcut::~TalentShortcut()
{
}

StringTableEntry TalentShortcut::getIconName()
{	
#ifdef NTJ_CLIENT	
	if (mTalentData)
		return mTalentData->getIcon();
#endif
	return NULL;
}

StringTableEntry TalentShortcut::getCursorName()
{
	char szCursorName[128];
	const char* szIconName = getIconName();
	if (!szIconName)
		return NULL;
	dSprintf(szCursorName, sizeof(szCursorName), "%s%s", "Talent/", szIconName);
	char* szPos = dStrrchr(szCursorName, '.');
	if(szPos) szPos[0] = 0;
	return StringTable->insert(szCursorName);
}

void TalentShortcut::packUpdate( BitStream* stream )
{
}

void TalentShortcut::unpackUpdate( BitStream* stream )
{
}

bool TalentShortcut::isValid()
{
	return	(mSpiritSlot >=0) && (mSpiritSlot < SPIRIT_MAXSLOTS) && 
			(mTalentIndex >=0) && (mTalentIndex < MAX_TALENT) && 
			(mTalentData != NULL) && (mTalentNode != NULL);
}