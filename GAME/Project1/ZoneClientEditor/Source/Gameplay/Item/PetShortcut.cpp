#include "math/mMath.h"
#include "core/stringTable.h"
#include "base/bitStream.h"
#include "Common/PlayerStruct.h"
#include "Gameplay/Item/PetShortcut.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/GameObjectData.h"
#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

PetShortcut::PetShortcut(TARGET_TYPE type /*= TARGET_SELF*/)
{
	mShortcutType = SHORTCUT_PET;
	mSlotIndex = -1;
	mTargetType = type;
}

PetShortcut::PetShortcut(S32 nSlotIndex, TARGET_TYPE type /*= TARGET_SELF*/)
{
	mShortcutType = SHORTCUT_PET;
	mSlotIndex = nSlotIndex;
	mTargetType = type;
}

PetShortcut* PetShortcut::CreateEmptyPetItem(TARGET_TYPE type/* = TARGET_SELF*/)
{
	return new PetShortcut(type);
}

PetShortcut* PetShortcut::CreatePetItem( S32 nSlotIndex, TARGET_TYPE type /*= TARGET_SELF*/ )
{
	return new PetShortcut(nSlotIndex, type);
}

PetShortcut::~PetShortcut()
{
}

StringTableEntry PetShortcut::getIconName()
{
	if (mSlotIndex == -1)
		return NULL;

#ifdef NTJ_CLIENT
	Player *pSelf = g_ClientGameplayState->GetControlPlayer();
	Player *pPlayer = (mTargetType == TARGET_SELF) ?  pSelf : dynamic_cast<Player*>(pSelf->getInteraction());
	if (!pPlayer)
		return NULL;
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(mSlotIndex);
	if (!pPetInfo || pPetInfo->petDataId == 0)
		return NULL;
	PetObjectData *pData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pData)
		return NULL;

	return pData->petShortcutIcon;
#endif

	return NULL;
	//return mPetInfo ? mPetInfo->icon : NULL;
}

StringTableEntry PetShortcut::getCursorName()
{
	char szCursorName[128];
	const char* szIconName = getIconName();
	if (!szIconName)
		return NULL;
	dSprintf(szCursorName, sizeof(szCursorName), "%s%s", "pet/", szIconName);
	char* szPos = dStrrchr(szCursorName, '.');
	if(szPos) szPos[0] = 0;
	return StringTable->insert(szCursorName);
}

void PetShortcut::packUpdate( BitStream* stream )
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
		//AssertFatal(0, "宠物数据错误!");
	}
#endif
}

void PetShortcut::unpackUpdate(BitStream* stream)
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


////////////////////////////////////////////////////////////////////////////
MountShortcut::MountShortcut()
{
	mShortcutType = SHORTCUT_MOUNT_PET;
	mSlotIndex = -1;
}

MountShortcut::MountShortcut(S32 nSlotIndex)
{
	mShortcutType = SHORTCUT_MOUNT_PET;
	mSlotIndex = nSlotIndex;
}

MountShortcut* MountShortcut::CreateEmptyMountItem()
{
	return new MountShortcut();
}

MountShortcut* MountShortcut::CreateMountItem( S32 nSlotIndex )
{
	return new MountShortcut(nSlotIndex);
}

MountShortcut::~MountShortcut()
{
}

StringTableEntry MountShortcut::getIconName()
{
	if (mSlotIndex == -1)
		return NULL;

#ifdef NTJ_CLIENT
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	stMountInfo *pMountInfo = (stMountInfo *)pPlayer->getPetTable().getMountInfo(mSlotIndex);
	if (!pMountInfo || pMountInfo->mDataId == 0)
		return NULL;
	MountObjectData *pData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pData)
		return NULL;

	return pData->mMountShortcutIcon;
#endif

	return NULL;
	//return mPetInfo ? mPetInfo->icon : NULL;
}

StringTableEntry MountShortcut::getCursorName()
{
	char szCursorName[128];
	const char* szIconName = getIconName();
	if (!szIconName)
		return NULL;
	dSprintf(szCursorName, sizeof(szCursorName), "%s%s", "mount/", szIconName);
	char* szPos = dStrrchr(szCursorName, '.');
	szPos[0] = 0;
	return StringTable->insert(szCursorName);
}

void MountShortcut::packUpdate( BitStream* stream )
{
	Parent::packUpdate(stream);
#ifdef NTJ_SERVER
	if(stream->writeFlag(IsValid()))
	{
		stream->writeInt(mSlotIndex, 8);
	}
	else
	{
		//AssertFatal(0, "宠物数据错误!");
	}
#endif
}

void MountShortcut::unpackUpdate(BitStream* stream)
{
	Parent::unpackUpdate(stream);
#ifdef NTJ_CLIENT
	if(stream->readFlag())
	{
		mSlotIndex = stream->readInt(8);
	}
	else
	{
		mSlotIndex = -1;
	}	
#endif
}