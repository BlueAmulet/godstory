#pragma once
#include "BuildPrefix.h"
#include "platform/types.h"

class Player;
struct MountObjectData;

class PetMountOperationManager
{
public:

	//ʹ����走��غ���
	static enWarnMessage	CanUseMountPetEgg					(Player *pPlayer, S32 iType, S32 iIndex);
	static bool				UseMountPetEgg						(Player *pPlayer, S32 iType, S32 iIndex);

	//��˳�����غ���
	static enWarnMessage	CanSpawnMountPet					(Player *pPlayer, U32 nSlot, MountObjectData **ppData);
	static bool				SpawnMountPet						(Player *pPlayer, U32 nSlot);

	//������˳�����غ���
	static enWarnMessage	CanDisbandMountPet					(Player *pPlayer, U32 nSlot);
	static bool				DisbandMountPet						(Player *pPlayer, U32 nSlot, bool bDelBuff = true);

	//������˳�����غ���
	static enWarnMessage	CanReleaseMountPet					(Player *pPlayer, U32 nSlot, bool bNeedConfirm = true);
	static bool				ReleaseMountPet						(Player *pPlayer, U32 nSlot);

	//���ʣ��ʱ��Ϊ0ʱ�Ĳ���
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