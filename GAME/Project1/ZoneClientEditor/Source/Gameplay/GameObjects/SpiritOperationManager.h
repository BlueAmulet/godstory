#pragma once
#include "BuildPrefix.h"
#include "platform/types.h"

class Player;
class NpcObject;
class Res;
class ItemShortcut;

class SpiritOperationManager
{
public:
	// 开启元神栏
	static enWarnMessage	CanOpenSpiritSlot				(Player *pPlayer, S32 nSlot);
	static bool				OpenSpiritSlot					(Player *pPlayer, S32 nSlot);

	// 新增元神
	static enWarnMessage	CanAddNewSpirit					(Player *pPlayer, U32 nSpiritId);
	static bool				AddNewSpirit					(Player *pPlayer, U32 nSpiritId);

	// 删除元神
	static enWarnMessage	CanDeleteSpirit					(Player *pPlayer, S32 nSlot);
	static bool				DeleteSpirit					(Player *pPlayer, S32 nSlot);	

	// 元神激活
	static enWarnMessage	CanActiviateSpirit				(Player *pPlayer, S32 nSlot);
	static bool				ActiviateSpirit					(Player *pPlayer, S32 nSlot);

	// 元神幻化
	static enWarnMessage	CanHuanHuaSpirit				(Player *pPlayer, S32 nSlot);
	static bool				HuanHuaSpirit					(Player *pPlayer, S32 nSlot);

	// 取消元神幻化
	static enWarnMessage	CanCancelHuanHuaSpirit			(Player *pPlayer, S32 nSlot);
	static bool				CancelHuanHuaSpirit				(Player *pPlayer, S32 nSlot);

	// NPC对话打开操作元神绝招界面
	static enWarnMessage	OpenSpiritSkillWnd				(Player *pPlayer, NpcObject *pNpc);
	static void				CloseSpiritSkillWnd				(Player *pPlayer);

	// 打开元神绝招栏
	static enWarnMessage	CanOpenSpiritJueZhao			(Player *pPlayer, S32 nSlot, S32 nIndex);
	static bool				OpenSpiritJueZhao				(Player *pPlayer, S32 nSlot, S32 nIndex);

	// 新增元神绝招
	static enWarnMessage	CanAddNewSpiritJueZhao			(Player *pPlayer, S32 nSlot, S32 nIndex);
	static bool				AddNewSpiritJueZhao				(Player *pPlayer, S32 nSlot, S32 nIndex);

	// 删除元神绝招
	static enWarnMessage	CanRemoveSpiritJueZhao			(Player *pPlayer, S32 nSlot, S32 nIndex);
	static bool				RemoveSpiritJueZhao				(Player *pPlayer, S32 nSlot, S32 nIndex);

	// 增加元神天赋等级
	static enWarnMessage	CanAddTalentLevel				(Player *pPlayer, S32 nSpiritSlot, S32 nTalentIndex, S32 nAddLevel);
	static bool				AddTalentLevel					(Player *pPlayer, S32 nSpiritSlot, S32 nTalentIndex, S32 nAddLevel);

	static void				ProcessingErrorMessage			(Player *pPlayer, U32 nMsgType, enWarnMessage msg);

#ifdef NTJ_CLIENT
	static bool				ClientSendSpiritOptMsg			(Player *pPlayer, S32 opType, S32 nSlot, U32 nSpiritId);
	static bool				ClientSendSpiritSkillOptMsg		(Player *pPlayer, S32 opType, S32 nSlot, S32 nIndex);
	static bool				ClientSendAddTalentLevelMsg		(Player *pPlayer, S32 nSpiritSlot, S32 nTalentIndex, S32 nAddLevel);
#endif

#ifdef NTJ_SERVER
	static bool				ServerSendAddTalentLevelMsg		(Player *pPlayer, S32 nSlot, S32 nIndex, bool bOptSuccess);
	static bool				ServerSendSpiritAddLevelMsg		(Player *pPlayer, S32 nSlot, S32 nAddedLevel);
	static bool				ServerSendSpiritAddExpMsg		(Player *pPlayer, S32 nSlot, S32 nAddedExp);
	static bool				ServerSendSpiritOpenJueZhao		(Player *pPlayer, S32 nSlot, S32 nIndex, bool bOptSuccess);
	static void				ServerSendUpdateSpiritSlotMsg	(Player *pPlayer, S32 nSlot, U32 flag = 0xFFFFFFFF);
	static bool				ServerSendSpiritOptMsg			(Player *pPlayer, S32 opType, S32 nSlot, bool bOptSuccess);
	static bool				ServerSendInitSpiritMsg			(Player *pPlayer);
#endif

#ifdef NTJ_SERVER
private:
	static bool				ServerDeleteSpiritSkillItem		(Player *pPlayer);
#endif
};