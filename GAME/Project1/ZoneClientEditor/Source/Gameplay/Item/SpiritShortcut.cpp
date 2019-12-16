#include "math/mMath.h"
#include "core/stringTable.h"
#include "base/bitStream.h"
#include "Common/PlayerStruct.h"
#include "Gameplay/Item/SpiritShortcut.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/GameObjectData.h"
#include "Gameplay/Data/SpiritData.h"
#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

SpiritShortcut::SpiritShortcut( TARGET_TYPE type /*= TARGET_SELF*/ )
{
	mShortcutType = SHORTCUT_SPIRIT;
	mSlotIndex = -1;
	mTargetType = type;
}

SpiritShortcut::SpiritShortcut( S32 nSlotIndex, TARGET_TYPE type /*= TARGET_SELF*/ )
{
	mShortcutType = SHORTCUT_SPIRIT;
	mSlotIndex = nSlotIndex;
	mTargetType = type;
}

SpiritShortcut* SpiritShortcut::CreateEmptySpiritItem( TARGET_TYPE type /*= TARGET_SELF*/ )
{
	return new SpiritShortcut(type);
}

SpiritShortcut* SpiritShortcut::CreateSpiritItem( S32 nSlotIndex, TARGET_TYPE type /*= TARGET_SELF*/ )
{
	return new SpiritShortcut(nSlotIndex, type);
}

SpiritShortcut::~SpiritShortcut()
{

}

StringTableEntry SpiritShortcut::getIconName()
{
	if (mSlotIndex == -1)
		return NULL;

#ifdef NTJ_CLIENT
	Player *pSelf = g_ClientGameplayState->GetControlPlayer();
	Player *pPlayer = (mTargetType == TARGET_SELF) ?  pSelf : dynamic_cast<Player*>(pSelf->getInteraction());
	if (!pPlayer)
		return NULL;
	stSpiritInfo *pSpiritInfo = (stSpiritInfo *)pPlayer->getSpiritTable().getSpiritInfo(mSlotIndex);
	if (!pSpiritInfo || pSpiritInfo->mDataId == 0)
		return NULL;
	SpiritData *pData = g_SpiritRepository.getSpiritData(pSpiritInfo->mDataId);
	if (!pData)
		return NULL;
	return pData->mIcon;
#endif

	return NULL;
}

StringTableEntry SpiritShortcut::getCursorName()
{
	char szCursorName[128];
	const char* szIconName = getIconName();
	if (!szIconName)
		return NULL;
	dSprintf(szCursorName, sizeof(szCursorName), "%s%s", "Spirit/", szIconName);
	char* szPos = dStrrchr(szCursorName, '.');
	if(szPos) szPos[0] = 0;
	return StringTable->insert(szCursorName);
}

void SpiritShortcut::packUpdate( BitStream* stream )
{
	Parent::packUpdate(stream);
#ifdef NTJ_SERVER
	if(stream->writeFlag(IsValid()))
	{
		stream->writeInt(mSlotIndex, 8);
		stream->writeInt(mTargetType, 8);
	}
	else
	{
		//AssertFatal(0, "元神数据错误!");
	}
#endif
}

void SpiritShortcut::unpackUpdate( BitStream* stream )
{
	Parent::unpackUpdate(stream);
#ifdef NTJ_CLIENT
	if(stream->readFlag())
	{
		mSlotIndex = stream->readInt(8);
		mTargetType = (TARGET_TYPE)stream->readInt(8);
	}
	else
	{
		mSlotIndex = -1;
	}	
#endif
}