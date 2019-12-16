//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "windows.h"
#include "platform/platform.h"
#include "console/simBase.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "T3D/gameConnection.h"
#include "Gameplay/Item/ProduceEquip.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/PetMountOperation.h"

#include "Gameplay/GameEvents/SimCallBackEvents.h"


simCallBackEvents::simCallBackEvents(U32 info_type /* = 0 */):m_InfoType(info_type),
m_IntArgCount(0),m_Int32ArgCount(0),m_StringArgCount(0)
{
	ZeroMemory(m_IntArgValue,sizeof(m_IntArgValue));

	ZeroMemory(m_Int32ArgValue,sizeof(m_Int32ArgValue));

	ZeroMemory(m_StringArgValue,sizeof(m_StringArgValue));	

}
//设置16位的整数参数
void simCallBackEvents::SetIntArgValues(S32 argc, ...)
{
	AssertRelease(argc<= MAX_INT_ARGS,"错误! 16位整数参数太多，超过64个!" );

	va_list args;

	va_start(args,argc);

	for(S32 i =0  ; i < argc ; ++ i )
	{
		m_IntArgValue[i] = 	va_arg(args, S32);
	}

	va_end(args);

	m_IntArgCount = argc;
}
// 设置32位的整数参数
void simCallBackEvents::SetInt32ArgValues(S32 argc, ...)
{
	AssertRelease(argc<= MAX_INT32_ARGS,"错误! 32位整数参数太多，超过64个!" );

	va_list args;

	va_start(args,argc);

	for(S32 i =0 ; i < argc ; ++ i )
	{
		m_Int32ArgValue[i] = 	va_arg(args, S32);
	}

	va_end(args);

	m_Int32ArgCount = argc;
}
// 设置字符串参数
void simCallBackEvents::SetStringArgValues(S32 argc, ...)
{
	AssertRelease(argc<= MAX_STRING_ARGS,"错误! 错误! 字符串参数太多，超过8个!" );

	va_list args;
	va_start(args, argc);

	for(S32 i = 0; i < argc; i++)
	{
		ZeroMemory(m_StringArgValue[i],sizeof(m_StringArgValue[i]));
		//sprintf(m_StringArgValue[i], va_arg(args, char*), StringArgValueBits);
	}

	va_end(args);

	m_StringArgCount = argc;

}

void simCallBackEvents::process(SimObject *object)
{
#ifdef NTJ_SERVER
	Player* pPlayer = dynamic_cast<Player*>(object);
	if(!pPlayer || !pPlayer->pScheduleEvent)
		return;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

	switch(m_InfoType)
	{
	case INFO_PRESCRIPTION_PRODUCE:
		{
			U16 iCount    = GetIntArg(0);
			U32 iSerialId = GetInt32Arg(0);

			if(g_ProduceEquip.Produce(pPlayer,iSerialId,iCount--) && (iCount)>0)
				g_ProduceEquip.sendCurrentEvent(pPlayer,iSerialId,iCount);
			else if(iCount>0)
				g_ProduceEquip.cancelProduce(pPlayer,iSerialId,iCount);
			else
				g_ProduceEquip.finishProduce(pPlayer,iSerialId,0);
		}
		break;

	case INFO_COLLECTION_ITEM:
		{
			U32 nCollectionObjId = GetInt32Arg(0);
			U32 nEventType = GetInt32Arg(1);
			U32 nEventId = GetInt32Arg(2);
			CollectionObject *pCollectionObject = dynamic_cast<CollectionObject *>( Sim::findObject(nCollectionObjId) );
			if (!pCollectionObject)
				return;
			
			if ( pCollectionObject->IsTriggerCountExceed() )
			{
				//enWarnMessage msg = MSG_COLLECTION_NOT_ENOUGTH;		//可采集次数为0
				//发送失败消息
				//MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_ERROR, MSG_COLLECTION_NOT_ENOUGTH, SHOWPOS_SCREEN);
			}
			else
			{
				pCollectionObject->ExecPredefinedEvent(pPlayer, nEventType, nEventId);
			}
		}
		break;

	case INFO_PET_OPERATE:
		{
			switch(GetIntArg(0))
			{				
			case Player::PetOp_Spawn:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetOperationManager::CallBackSpawnPet(pPlayer, nSlot);
					PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_Spawn, nSlot, bSuccess);
				}
				break;
			case Player::PetOp_UseEgg:
				{
					S32 iType = GetIntArg(1);
					S32 iIndex = GetIntArg(2);
					bool bSuccess = PetOperationManager::CallBackUsePetEgg(pPlayer, iType, iIndex);
					PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_UseEgg, 0, bSuccess);
				}
			default:
				break;
			}
		}
		break;
	case INFO_MOUNT_PET_OPERATE:
		{
			switch(GetIntArg(0))
			{
			case Player::PetOp_UseMountPetEgg:
				{
					S32 iType = GetIntArg(1);
					S32 iIndex = GetIntArg(2);
					bool bSuccess = PetMountOperationManager::CallBackUseMountPetEgg(pPlayer, iType, iIndex);
					PetMountOperationManager::ServerSendMountPetOptMsg(pPlayer, Player::PetOp_UseMountPetEgg, 0, bSuccess);
				}
				break;
			case Player::PetOp_SpawnMountPet:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetMountOperationManager::CallBackSpawnMountPet(pPlayer, nSlot);
					PetMountOperationManager::ServerSendMountPetOptMsg(pPlayer, Player::PetOp_SpawnMountPet, nSlot, bSuccess);
				}
				break;
			}
		}
		break;
	case INFO_ITEMSPLIT:
		{
			if(!pPlayer->mItemSplitList.startItemSplit(pPlayer))
				pPlayer->mItemSplitList.cancelItemSplit(pPlayer);
			else
				pPlayer->mItemSplitList.finishItemSplit(pPlayer);
		}
		break; 
	case INFO_ITEMCOMPOSE:
		{
			if(!pPlayer->mItemComposeList.startItemCompose(pPlayer))
				pPlayer->mItemComposeList.cancelItemCompose(pPlayer);
			else
				pPlayer->mItemComposeList.finishItemCompose(pPlayer);
		}
		break;
	default:	
		break;
	}

	pPlayer->pScheduleEvent->clear();
#endif
}