#pragma once
#include "BuildPrefix.h"
#include "platform/types.h"

class Player;
struct MountObjectData;

class PetMountOperationManager
{
public:

	//使用骑宠蛋相关函数
	static enWarnMessage	CanUseMountPetEgg					(Player *pPlayer, S32 iType, S32 iIndex);
	static bool				UseMountPetEgg						(Player *pPlayer, S32 iType, S32 iIndex);

	//骑乘宠物相关函数
	static enWarnMessage	CanSpawnMountPet					(Player *pPlayer, U32 nSlot, MountObjectData **ppData);
	static bool				SpawnMountPet						(Player *pPlayer, U32 nSlot);

	//回收骑乘宠物相关函数
	static enWarnMessage	CanDisbandMountPet					(Player *pPlayer, U32 nSlot);
	static bool				DisbandMountPet						(Player *pPlayer, U32 nSlot, bool bDelBuff = true);

	//放生骑乘宠物相关函数
	static enWarnMessage	CanReleaseMountPet					(Player *pPlayer, U32 nSlot, bool bNeedConfirm = true);
	static bool				ReleaseMountPet						(Player *pPlayer, U32 nSlot);

	//骑宠剩余时间为0时的操作
	static enWarnMessage	CanForceReleaseMountPet				(Player *pPlayer, U32 nSlot);
	static bool				ForceReleaseMountPet				(Player *pPlayer, U32 nSlot);
	static bool				Mount								(Player *pPlayer, U32 dataId);
	static void				Unmount								(Player *pPlayer, U32 dataId);

#ifdef NTJ_CLIENT
	static bool				ClientSendUseMountPetEggMsg			(Player *pPlayer, S32 iType, S32 iIndex);
	static bool				ClientSendPetMountOptMsg			(Player *pPlayer, S32 opType, U32 nSlot);
#endif

#ifdef NTJ_SERVER
	static void				InitActiveMountObject				(Player *pPlayer, U32 nSlot);

	static bool				CallBackUseMountPetEgg				(Player *pPlayer, S32 iType, S32 iIndex);
	static bool				CallBackSpawnMountPet				(Player *pPlayer, U32 nSlot);
	static void				ServerSendUpdateMountPetSlotMsg		(Player *pPlayer, U32 nSlot);
	static bool				ServerSendMountPetOptMsg			(Player *pPlayer, S32 opType, U32 nSlot, bool bOptSuccess);
#endif

	static void				ProcessingErrorMessage				(Player *pPlayer, U32 nMsgType, enWarnMessage msg);
	static bool				BeginVocalEvent						(Player *pPlayer, U32 vocalType, U32 nOptType, U32 nSlot);
};