#pragma once
#include "BuildPrefix.h"
#include "platform/types.h"

class Player;
class NpcObject;
class Res;
class ItemShortcut;

class PetOperationManager
{
public:
	enum FeedType
	{
		FeedTypeFood	= 1,		//喂食
		FeedTypeMedical	= 2,		//补气
		FeedTypeToy		= 3,		//驯服
	};

public:
	//宠物死亡
	static bool				PetDead							(Player *pPlayer, PetObject *pPetObject);

	//宠物改名相关函数
	static bool				PetChangeName					(Player *pPlayer, U32 nSlot, StringTableEntry sNewName);

	//宠物修改属性点函数
	static bool				PetChangeProperties				(Player *pPlayer, U32 nSlot, S32 nTiPo, S32 nJingLi, S32 nLiDao, S32 nLingLi, S32 nMinJie, S32 nYuanLi);

	//宠物灵慧相关函数
	static enWarnMessage	OpenPetInsight					(Player *pPlayer, NpcObject *pNpc);
	static void				ClosePetInsight					(Player *pPlayer);
	static enWarnMessage	CanIncreasePetInsight			(Player *pPlayer, U32 nSlot, S32 &nItemIndex, U32 &nMoney);
	static bool				IncreasePetInsight				(Player *pPlayer, U32 nSlot);

	//宠物炼化相关函数
	static enWarnMessage	OpenPetLianHua					(Player *pPlayer, NpcObject *pNpc);
	static void				ClosePetLianHua					(Player *pPlayer);
	static enWarnMessage	CanLianHuaPet					(Player *pPlayer, U32 nSlot, U32 &nMoney, bool bNeedConfirm = true);
	static bool				LianHuaPet						(Player *pPlayer, U32 nSlot);

	//宠物成长率鉴定相关函数
	static enWarnMessage	OpenPetIdentify					(Player *pPlayer, NpcObject *pNpc);
	static void				ClosePetIdentify				(Player *pPlayer);
	static enWarnMessage	CanPetIdentify					(Player *pPlayer, U32 nSlot, U32 &nMoney);
	static bool				PetIdentify						(Player *pPlayer, U32 nSlot);

	//宠物还童相关函数
	static enWarnMessage	OpenPetHuanTong					(Player *pPlayer, NpcObject *pNpc);
	static void				ClosePetHuanTong				(Player *pPlayer);
	static enWarnMessage	CanPetHuanTong					(Player *pPlayer, U32 nSlot, U32 &nMoney, bool bNeedConfirm = true);
	static bool				PetHuanTong						(Player *pPlayer, U32 nSlot);

	//宠物放生相关函数
	static enWarnMessage	CanReleasePet					(Player *pPlayer, U32 nSlot, bool bNeedConfirm = true);
	static bool				ReleasePet						(Player *pPlayer, U32 nSlot);

	//宠物召唤相关函数
	static enWarnMessage	CanSpawnPet						(Player *pPlayer, U32 nSlot, PetObjectData **ppData);
	static bool				SpawnPet						(Player *pPlayer, U32 nSlot);

	//宠物回收相关函数
	static enWarnMessage	CanDisbandPet					(Player *pPlayer, U32 nSlot);
	static bool				DisbandPet						(Player *pPlayer, U32 nSlot);

	//宠物捕捉相关函数
	static enWarnMessage	CanTameNpc						(Player *pPlayer, NpcObject *pNpc);
	static bool				TameNpc							(Player *pPlayer, NpcObject *pNpc);

	//使用宠物蛋相关函数
	static enWarnMessage	CanUsePetEgg					(Player *pPlayer, S32 iType, S32 iIndex);
	static bool				UsePetEgg						(Player *pPlayer, S32 iType, S32 iIndex);

	//宠物喂食相关函数
	static enWarnMessage	CanPetWeiShi					(Player *pPlayer, U32 nSlot, FeedType nFeedType, S32 &nItemIndex);
	static enWarnMessage	CanPetWeiShi					(Player *pPlayer, U32 nSlot, FeedType nFeedType, S32 nItemIndex, ItemShortcut *pItem);
	static bool				PetWeiShi_1						(Player *pPlayer, U32 nSlot, FeedType nFeedType);
	static bool				PetWeiShi_2						(Player *pPlayer, U32 nSlot, S32 nItemIndex);

	//宠物修行相关函数
	static enWarnMessage	CanPetXiuXing					(Player *pPlayer, U32 nSlot, U32 nPetStudyDataId);
	static bool				PetXiuXing						(Player *pPlayer, U32 nSlot, U32 nPetStudyDataId);
	
	//宠物交易相关函数
	static enWarnMessage	CanPetTrade						(Player* pPlayer, Player* pTargetPlayer, U32 nSlot);

	static bool				BeginVocalEvent					(Player *pPlayer, U32 vocalType, U32 nOptType, U32 nSlot);

#ifdef NTJ_CLIENT
	static bool				ClientSendUsePetEggMsg			(Player *pPlayer, S32 iType, S32 iIndex);
	static bool				ClientSendPetXiuXingMsg			(Player *pPlayer, U32 nSlot, U32 nPetStudyDataId);
	static bool				ClientSendPetOptMsg				(Player *pPlayer, S32 opType, U32 nSlot);
	static bool				ClientSendPetWeiShiMsg			(Player *pPlayer, U32 nSlot, S32 iType, S32 iIndex);
	static bool				ClientSendPetSlotExchangeMsg	(Player *pPlayer, S32 nSrcIndex, S32 nDestType, S32 nDestIndex);
#endif

#ifdef NTJ_SERVER
	static void				InitActivePetObject				(Player *pPlayer, U32 nSlot);
	static void				RefreshPetTable					(Player *pPlayer, U32 nBeginSlot);

	static bool				CallBackSpawnPet				(Player *pPlayer, U32 nSlot);
	static bool				CallBackUsePetEgg				(Player *pPlayer, S32 iType, S32 iIndex);

	static void				ServerSendUpdatePetSlotMsg		(Player *pPlayer, U32 nSlot, U32 flag = 0xFFFFFFFF);
	static bool				ServerSendPetOptMsg				(Player *pPlayer, S32 opType, U32 nSlot, bool bOptSuccess);
	static bool				ServerSendPetOpenMsg			(Player *pPlayer, S32 opType);

	static bool				UsePetFood						(Player *pPlayer, U32 nSlot, FeedType nFeedType, S32 iIndex);

	//宠物交易相关函数
	static bool				ResetPetIdleStatus				(Player *pPlayer, U32 nSlot);	
	static void				SetPetFreeze					(Player* pPlayer, bool bEnableFreeze = true);

	//服务端宠物添加及删除操作函数
	static enWarnMessage	AddPetInfo						(Player *pPlayer, stPetInfo *pPetInfo);
	static enWarnMessage	DeletePetInfo					(Player *pPlayer, U32 nSlot);

	//服务端通知宠物升级与获得经验的消息
	static bool				ServerSendPetAddExpMsg			(Player *pPlayer, U32 nSlot, U32 nAddedExp);
	static bool				ServerSendPetAddLevelMsg		(Player *pPlayer, U32 nSlot, U32 nAddedLevel);

#endif

	static void				ProcessingErrorMessage			(Player *pPlayer, U32 nMsgType, enWarnMessage msg);
};