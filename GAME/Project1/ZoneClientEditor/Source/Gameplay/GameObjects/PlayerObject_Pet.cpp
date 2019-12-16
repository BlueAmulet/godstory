//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "console/console.h"
#include "Gameplay/GameObjects/PetOperationManager.h"

/*
S32 Player::spawnPet(U32 slot)
{
	if (!PetOperationManager::SpawnPet(this, slot))
		return 0;
	return 1;
	if(!mPetTable.canSpawnPet(slot))
		return 0;

#ifdef NTJ_CLIENT
	// �����ٻ�����
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn && conn->getControlObject() == this)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_PET_OPERATE);
		pEvent->SetIntArgValues(2, Player::PetOp_Spawn, slot);
		conn->postNetEvent(pEvent);
	}
	return 1;
#endif

#ifdef NTJ_SERVER
	// ���ȼ���Ƿ���Ҫ�ջص�ǰ����
	const stPetInfo *pPetInfo = mPetTable.getPetInfo(slot);
	if (!pPetInfo)
		return 0;

	PetObjectData* pData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if(!pData)
		return 0;
	if(mPetTable.getPet(pData->combatPetIdx))
		AssertFatal(disbandPet(mPetTable.getPet(pData->combatPetIdx)->getPetSlot()), "can NOT disband pet!");

	PetObject* pPet = new PetObject;
	if(!pPet)
		return 0;

	pPet->setDataID(pPetInfo->petDataId);
	pPet->setPetSlot(slot);
	pPet->mMaster = this;
	if(!pPet->registerObject())
	{
		pPet->deleteObject();
		return 0;
	}

	pPet->mMaster = NULL;
	pPet->setMaster(this);
	pPet->setTransform(this->getTransform());

	mPetTable.loadPetInfo(pPet->getPetIndex());
	mPetTable.setPetStatus(slot, PetObject::PetStatus_Combat);

	setMaskBits(PetMask);
	return pPet->getId();
#endif
	return 0;
}

bool Player::disbandPet(U32 slot)
{
	if (!PetOperationManager::DisbandPet(this, slot))
		return 0;
	return 1;

	if(!mPetTable.canDispatchPet(slot))
		return false;

#ifdef NTJ_CLIENT
	// �����ٻ�����
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn && conn->getControlObject() == this)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_PET_OPERATE);
		pEvent->SetIntArgValues(2, Player::PetOp_Disband, slot);
		conn->postNetEvent(pEvent);
	}
	return true;
#endif

#ifdef NTJ_SERVER
	PetObject* pet = mPetTable.getCombatPetBySlot(slot);
	if(!pet)
		return false;

	// �ȱ������
	mPetTable.savePetInfo(pet->getPetIndex());		
	pet->unregisterObject();
	mPetTable.setPetStatus(slot, PetObject::PetStatus_Idle);
	setMaskBits(PetMask);
	return true;
#endif
	return true;
}
*/