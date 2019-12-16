#include "Gameplay/GameObjects/SpiritOperationManager.h"
#include "console/console.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/Data/SpiritData.h"
#include "Gameplay/GameObjects/TimerTrigger.h"

#ifdef NTJ_CLIENT
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"
#endif
#ifdef NTJ_SERVER
#include "Gameplay/GameEvents/SimCallBackEvents.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/ServerGameplayState.h"
#endif

// 开启元神栏
enWarnMessage SpiritOperationManager::CanOpenSpiritSlot( Player *pPlayer, S32 nSlot )
{
	if (!pPlayer)
		return MSG_UNKOWNERROR;

	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSlotValid(nSlot))
		return MSG_SPIRIT_SLOT_INVALID;
	if (table.isSpiritOpened(nSlot))
		return MSG_SPIRIT_SLOT_OPENED;
	return MSG_NONE;
}

bool SpiritOperationManager::OpenSpiritSlot( Player *pPlayer, S32 nSlot )
{
	enWarnMessage msg = CanOpenSpiritSlot(pPlayer, nSlot);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	return ClientSendSpiritOptMsg(pPlayer, Player::SpiritOp_OpenSpiritSlot, nSlot, -1);
#endif

#ifdef NTJ_SERVER
	SpiritTable &table = pPlayer->getSpiritTable();
	table.openSlot(nSlot);
#endif

	return true;
}

// 新增元神
enWarnMessage SpiritOperationManager::CanAddNewSpirit( Player *pPlayer, U32 nSpiritId )
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(nSpiritId);
	if (!pPlayer || !pSpiritData)
		return MSG_UNKOWNERROR;

	if (pPlayer->getSpiritTable().findEmptySlot() == -1)
		return MSG_SPIRIT_FULL_SLOT;

	return MSG_NONE;
}

bool SpiritOperationManager::AddNewSpirit( Player *pPlayer, U32 nSpiritId )
{
	enWarnMessage msg = CanAddNewSpirit(pPlayer, nSpiritId);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	ClientSendSpiritOptMsg(pPlayer, Player::SpiritOp_AddSpirit, -1, nSpiritId);
#endif

#ifdef NTJ_SERVER
	SpiritTable &table = pPlayer->getSpiritTable();
	S32 nFreeSlot = table.findEmptySlot();
	table.createSpiritInfo(pPlayer, nFreeSlot, nSpiritId);
	ServerSendUpdateSpiritSlotMsg(pPlayer, nFreeSlot);
	if (table.getSpiritCount() == 1 && table.getActiveSlot() == -1)
	{
		//激活该元神
		SpiritOperationManager::ActiviateSpirit(pPlayer, nFreeSlot);
		ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_ActivateSpirit, nFreeSlot, true);
	}
#endif

	return true;
}

// 删除元神
enWarnMessage SpiritOperationManager::CanDeleteSpirit( Player *pPlayer, S32 nSlot )
{
	if (!pPlayer)
		return MSG_UNKOWNERROR;

	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSlotValid(nSlot))
		return MSG_SPIRIT_SLOT_INVALID;
	if (!table.isSpiritOpened(nSlot))
		return MSG_SPIRIT_SLOT_NOT_OPENED;
	if (!table.isSpiritExist(nSlot))
		return MSG_SPIRIT_NOT_EXIST;
	if (table.isSpiritActive(nSlot))
		return MSG_SPIRIT_BE_ACTIVE;
	if (table.getSpiritCount() == 1)
		return MSG_SPIRIT_ONLY_ONE;

	return MSG_NONE;
}

bool SpiritOperationManager::DeleteSpirit( Player *pPlayer, S32 nSlot )
{
	enWarnMessage msg = CanDeleteSpirit(pPlayer, nSlot);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	return ClientSendSpiritOptMsg(pPlayer, Player::SpiritOp_RemoveSpirit, nSlot, -1);
#endif

#ifdef NTJ_SERVER
	pPlayer->getSpiritTable().removeSpiritInfo(nSlot);
	pPlayer->mSpiritList.SetSlot(nSlot, NULL);
	pPlayer->mSpiritList.UpdateToClient(pPlayer->getControllingClient(), nSlot, ITEM_NOSHOW);
	ServerSendUpdateSpiritSlotMsg(pPlayer, nSlot);
#endif

	return true;
}

// 元神激活
enWarnMessage SpiritOperationManager::CanActiviateSpirit( Player *pPlayer, S32 nSlot )
{
	if (!pPlayer)
		return MSG_UNKOWNERROR;

	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSlotValid(nSlot))
		return MSG_SPIRIT_SLOT_INVALID;
	if (!table.isSpiritOpened(nSlot))
		return MSG_SPIRIT_SLOT_NOT_OPENED;
	if (!table.isSpiritExist(nSlot))
		return MSG_SPIRIT_NOT_EXIST;
	if (table.isSpiritActive(nSlot))
		return MSG_SPIRIT_BE_ACTIVE;

	return MSG_NONE;
}

bool SpiritOperationManager::ActiviateSpirit( Player *pPlayer, S32 nSlot )
{
	enWarnMessage msg = CanActiviateSpirit(pPlayer, nSlot);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	return ClientSendSpiritOptMsg(pPlayer, Player::SpiritOp_ActivateSpirit, nSlot, -1);
#endif

#ifdef NTJ_SERVER
	SpiritTable &table = pPlayer->getSpiritTable();
	S32 nOldActiveSlot = table.getActiveSlot();
	/*if (nOldActiveSlot >= 0 && nOldActiveSlot < SPIRIT_MAXSLOTS)
		table.RemoveTalentSetFromTalentTable(pPlayer, nOldActiveSlot);*/
	table.AddTalentSetToTalentTable(pPlayer, nSlot);
	table.setActiveSlot(nSlot);
#endif
	return true;
}

// 元神幻化
enWarnMessage SpiritOperationManager::CanHuanHuaSpirit( Player *pPlayer, S32 nSlot )
{
	if (!pPlayer)
		return MSG_UNKOWNERROR;

	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSlotValid(nSlot))
		return MSG_SPIRIT_SLOT_INVALID;
	if (!table.isSpiritOpened(nSlot))
		return MSG_SPIRIT_SLOT_NOT_OPENED;
	if (!table.isSpiritExist(nSlot))
		return MSG_SPIRIT_NOT_EXIST;
	if (!table.isSpiritActive(nSlot))
		return MSG_SPIRIT_BE_INACTIVE;
	if (table.isSpiritHuanHua())
		return MSG_SPIRIT_BE_HUANHUA;
	return MSG_NONE;
}

bool SpiritOperationManager::HuanHuaSpirit( Player *pPlayer, S32 nSlot )
{
	enWarnMessage msg = CanHuanHuaSpirit(pPlayer, nSlot);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	return ClientSendSpiritOptMsg(pPlayer, Player::SpiritOp_HuanHuaSpirit, nSlot, -1);
#endif

#ifdef NTJ_SERVER
	SpiritTable &table = pPlayer->getSpiritTable();
	U32 nBuffId = table.getHuanHuaId(nSlot);
	if (nBuffId == 0)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}
	pPlayer->AddBuff(Buff::Origin_Base, nBuffId, pPlayer);
	table.setSpiritHuanHua(true);
#endif
	return true;
}

// 元神幻化
enWarnMessage SpiritOperationManager::CanCancelHuanHuaSpirit( Player *pPlayer, S32 nSlot )
{
	if (!pPlayer)
		return MSG_UNKOWNERROR;

	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSlotValid(nSlot))
		return MSG_SPIRIT_SLOT_INVALID;
	if (!table.isSpiritOpened(nSlot))
		return MSG_SPIRIT_SLOT_NOT_OPENED;
	if (!table.isSpiritExist(nSlot))
		return MSG_SPIRIT_NOT_EXIST;
	if (!table.isSpiritActive(nSlot))
		return MSG_SPIRIT_BE_INACTIVE;
	if (!table.isSpiritHuanHua())
		return MSG_SPIRIT_BE_NOTHUANHUA;

	return MSG_NONE;
}

bool SpiritOperationManager::CancelHuanHuaSpirit( Player *pPlayer, S32 nSlot )
{
	enWarnMessage msg = CanCancelHuanHuaSpirit(pPlayer, nSlot);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	return ClientSendSpiritOptMsg(pPlayer, Player::SpiritOp_CancelHuanHuaSpirit, nSlot, -1);
#endif

#ifdef NTJ_SERVER
	SpiritTable &table = pPlayer->getSpiritTable();
	U32 nBuffId = table.getHuanHuaId(nSlot);
	if (nBuffId == 0)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return false;
	}
	pPlayer->RemoveBuff(nBuffId);
	pPlayer->getSpiritTable().setSpiritHuanHua(false);
#endif
	return true;
}

// NPC对话打开操作元神绝招界面
enWarnMessage SpiritOperationManager::OpenSpiritSkillWnd(Player *pPlayer, NpcObject *pNpc)
{
#ifdef NTJ_SERVER
	S32 nCount = pPlayer->getSpiritTable().getSpiritCount();
	if (nCount == 0)
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MSG_SPIRIT_NONE);
		return MSG_SPIRIT_NONE;
	}

	if ( !pPlayer->setInteraction(pNpc, Player::INTERACTION_SPIRITSKILL) )
		return MSG_NONE;
	enWarnMessage MsgCode = pPlayer->isBusy(Player::INTERACTION_SPIRITSKILL);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return MsgCode;
	}
	pPlayer->tempList.SetOptType(pPlayer->getControllingClient(), TempList::OptType_SpiritSkill);
	if (!ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_OpenJueZhaoWnd, 0, true))
	{
		MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, MSG_UNKOWNERROR);
		return MSG_UNKOWNERROR;
	}
#endif

#ifdef NTJ_CLIENT
	Con::executef("SpiritJueZhaoWnd_OpenSpiritSkill");
#endif

	return MSG_NONE;
}
void SpiritOperationManager::CloseSpiritSkillWnd(Player *pPlayer)
{
	if(!pPlayer)
		return;

#ifdef NTJ_SERVER
	pPlayer->tempList.Clean(pPlayer);
#endif

#ifdef NTJ_CLIENT
	Con::executef("SpiritJueZhaoWnd_CloseSpiritSkill");	
#endif
}

// 打开元神绝招栏
enWarnMessage SpiritOperationManager::CanOpenSpiritJueZhao(Player *pPlayer, S32 nSlot, S32 nIndex)
{
	if (!pPlayer || nSlot < 0 || nSlot >= SPIRIT_MAXSLOTS || nIndex < 0 || nIndex >= MAX_SPIRIT_SKILL)
		return MSG_UNKOWNERROR;
	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSpiritOpened(nSlot))
		return MSG_SPIRIT_SLOT_NOT_OPENED;
	if (!table.isSpiritExist(nSlot))
		return MSG_SPIRIT_NOT_EXIST;
	if (table.isJueZhaoOpened(nSlot, nIndex))
		return MSG_SPIRIT_SKILL_OPENED;
	if (pPlayer->tempList.GetOptType() != TempList::OptType_SpiritSkill)
		return MSG_UNKOWNERROR;
	
	ItemShortcut *pItem = dynamic_cast<ItemShortcut*>( pPlayer->tempList.GetSlot(0) );
	if (!pItem || !pItem->getRes()->isSpiritStone1())
		return MSG_SPIRIT_NO_STONE1;

	// 检查金钱
	U32 nCostMoney = 1;
	if (!pPlayer->canReduceMoney(nCostMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	return MSG_NONE;
}

bool SpiritOperationManager::OpenSpiritJueZhao(Player *pPlayer, S32 nSlot, S32 nIndex)
{
	enWarnMessage msg = CanOpenSpiritJueZhao(pPlayer, nSlot, nIndex);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	return ClientSendSpiritSkillOptMsg(pPlayer, Player::SpiritOp_OpenJueZhaoSlot, nSlot, nIndex);
#endif

#ifdef NTJ_SERVER
	ServerDeleteSpiritSkillItem(pPlayer);
	SpiritTable &table = pPlayer->getSpiritTable();
	table.OpenJueZhao(nSlot, nIndex);
	ServerSendUpdateSpiritSlotMsg(pPlayer, nSlot, stSpiritInfo::Skill_Mask);
#endif

	return true;
}

// 新增元神绝招
enWarnMessage SpiritOperationManager::CanAddNewSpiritJueZhao(Player *pPlayer, S32 nSlot, S32 nIndex)
{
	if (!pPlayer || nSlot < 0 || nSlot >= SPIRIT_MAXSLOTS || nIndex < 0 || nIndex >= MAX_SPIRIT_SKILL)
		return MSG_UNKOWNERROR;
	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSpiritOpened(nSlot))
		return MSG_SPIRIT_SLOT_NOT_OPENED;
	if (!table.isSpiritExist(nSlot))
		return MSG_SPIRIT_NOT_EXIST;
	if (!table.isJueZhaoOpened(nSlot, nIndex))
		return MSG_SPIRIT_SKILL_NOT_OPENED;
	if (table.isJueZhaoExist(nSlot, nIndex))
		return MSG_SPIRIT_SKILL_ALREADY_EXIST;
	if (pPlayer->tempList.GetOptType() != TempList::OptType_SpiritSkill)
		return MSG_UNKOWNERROR;

	ItemShortcut *pItem = dynamic_cast<ItemShortcut*>( pPlayer->tempList.GetSlot(0) );
	if (!pItem || !pItem->getRes()->isSkillBook())
		return MSG_SPIRIT_NO_SKILLBOOK;
	U32 nSkillId = pItem->getRes()->getReserveValue();
	if (nSkillId == 0)
		return MSG_ITEM_ITEMDATAERROR;

	// 检查金钱
	U32 nCostMoney = 1;
	if (!pPlayer->canReduceMoney(nCostMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	return MSG_NONE;
}

bool SpiritOperationManager::AddNewSpiritJueZhao(Player *pPlayer, S32 nSlot, S32 nIndex)
{
	enWarnMessage msg = CanAddNewSpiritJueZhao(pPlayer, nSlot, nIndex);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}	

#ifdef NTJ_CLIENT
	return ClientSendSpiritSkillOptMsg(pPlayer, Player::SpiritOp_AddJueZhao, nSlot, nIndex);
#endif

#ifdef NTJ_SERVER
	ItemShortcut *pSkillBook = dynamic_cast<ItemShortcut*>( pPlayer->tempList.GetSlot(0) );
	U32 nSkillId = pSkillBook->getRes()->getReserveValue();
	ServerDeleteSpiritSkillItem(pPlayer);
	SpiritTable &table = pPlayer->getSpiritTable();
	table.AddJueZhao(pPlayer, nSlot, nIndex, nSkillId);
	ServerSendUpdateSpiritSlotMsg(pPlayer, nSlot, stSpiritInfo::Skill_Mask);
#endif

	return true;
}

// 删除元神绝招
enWarnMessage SpiritOperationManager::CanRemoveSpiritJueZhao(Player *pPlayer, S32 nSlot, S32 nIndex)
{
	if (!pPlayer || nSlot < 0 || nSlot >= SPIRIT_MAXSLOTS || nIndex < 0 || nIndex >= MAX_SPIRIT_SKILL)
		return MSG_UNKOWNERROR;
	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSpiritOpened(nSlot))
		return MSG_SPIRIT_SLOT_NOT_OPENED;
	if (!table.isSpiritExist(nSlot))
		return MSG_SPIRIT_NOT_EXIST;
	if (!table.isJueZhaoOpened(nSlot, nIndex))
		return MSG_SPIRIT_SKILL_NOT_OPENED;
	if (!table.isJueZhaoExist(nSlot, nIndex))
		return MSG_SPIRIT_SKILL_NOT_EXIST;
	if (pPlayer->tempList.GetOptType() != TempList::OptType_SpiritSkill)
		return MSG_UNKOWNERROR;

	ItemShortcut *pItem = dynamic_cast<ItemShortcut*>( pPlayer->tempList.GetSlot(0) );
	if (!pItem || !pItem->getRes()->isSpiritStone2())
		return MSG_SPIRIT_NO_STONE2;

	// 检查金钱
	U32 nCostMoney = 1;
	if (!pPlayer->canReduceMoney(nCostMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	return MSG_NONE;
}

bool SpiritOperationManager::RemoveSpiritJueZhao(Player *pPlayer, S32 nSlot, S32 nIndex)
{
	enWarnMessage msg = CanRemoveSpiritJueZhao(pPlayer, nSlot, nIndex);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	return ClientSendSpiritSkillOptMsg(pPlayer, Player::SpiritOp_RemoveJueZhao, nSlot, nIndex);
#endif

#ifdef NTJ_SERVER
	ServerDeleteSpiritSkillItem(pPlayer);
	SpiritTable &table = pPlayer->getSpiritTable();
	table.RemoveJueZhao(pPlayer, nSlot, nIndex);
	ServerSendUpdateSpiritSlotMsg(pPlayer, nSlot, stSpiritInfo::Skill_Mask);
#endif

	return true;
}

// 增加元神天赋等级
enWarnMessage SpiritOperationManager::CanAddTalentLevel(Player *pPlayer, S32 nSpiritSlot, S32 nTalentIndex, S32 nAddLevel)
{
	if (nSpiritSlot < 0 || nSpiritSlot >= MAX_SPIRIT_SKILL)
		return MSG_SPIRIT_SLOT_INVALID;
	if (nTalentIndex < 0 || nTalentIndex > MAX_TALENT)
		return MSG_SPIRIT_TALENTSLOT_INVALID;
	SpiritTable &table = pPlayer->getSpiritTable();
	if (!table.isSpiritOpened(nSpiritSlot))
		return MSG_SPIRIT_SLOT_NOT_OPENED;
	if (!table.isSpiritExist(nSpiritSlot))
		return MSG_SPIRIT_NOT_EXIST;
	stSpiritInfo *pSpiritInfo = table.getSpiritInfo(nSpiritSlot);
	S32 nTalentMaxLevel = table.getTalentMaxLevel(nSpiritSlot, nTalentIndex);
	S32 nTalentCurLevel = table.getTalentLevel(nSpiritSlot, nTalentIndex);
	if (nTalentCurLevel >= nTalentMaxLevel)
		return MSG_SPIRIT_TALENTLEVEL_LIMIT;
	U16 nTalentLeftPts = table.getTalentLeftPts(nSpiritSlot);
	if (nTalentLeftPts == 0)
		return MSG_SPIRIT_TALENT_LEFTPTS_ZERO;

	return MSG_NONE;
}
bool SpiritOperationManager::AddTalentLevel(Player *pPlayer, S32 nSpiritSlot, S32 nTalentIndex, S32 nAddLevel)
{
	enWarnMessage msg = CanAddTalentLevel(pPlayer, nSpiritSlot, nTalentIndex, nAddLevel);
	if (msg != MSG_NONE)
	{
		ProcessingErrorMessage(pPlayer, SHOWTYPE_NOTIFY, msg);
		return false;
	}

#ifdef NTJ_CLIENT
	return ClientSendAddTalentLevelMsg(pPlayer, nSpiritSlot, nTalentIndex, nAddLevel);
#endif

#ifdef NTJ_SERVER
	SpiritTable& table = pPlayer->getSpiritTable();
	stSpiritInfo *pSpiritInfo = table.getSpiritInfo(nSpiritSlot);

	S32 nTalentMaxLevel = table.getTalentMaxLevel(nSpiritSlot, nTalentIndex);
	S32 nTalentCurLevel = table.getTalentLevel(nSpiritSlot, nTalentIndex);
	S32 nDiffLevel = nTalentMaxLevel >= (nAddLevel+nTalentCurLevel) ? nAddLevel : (nTalentMaxLevel - nAddLevel);

	S32 nCurrAcitveSlot = table.getActiveSlot();
	
	if (nCurrAcitveSlot == nSpiritSlot)
		table.RemoveTalentFromTalentTable(pPlayer, nSpiritSlot, nTalentIndex);

	pSpiritInfo->mTalentSkillPts[nTalentIndex] += nDiffLevel;
	pSpiritInfo->mTalentLeftPts -= nDiffLevel;

	if (nCurrAcitveSlot == nSpiritSlot)
		table.AddTalentToTalentTable(pPlayer, nSpiritSlot, nTalentIndex);

	SpiritOperationManager::ServerSendUpdateSpiritSlotMsg(pPlayer, nSpiritSlot, stSpiritInfo::TalentPts_Mask);
#endif

	return true;
}

void SpiritOperationManager::ProcessingErrorMessage( Player *pPlayer, U32 nMsgType, enWarnMessage msg )
{
#ifdef NTJ_CLIENT
	MessageEvent::show(nMsgType, msg);
#endif

#ifdef NTJ_SERVER
	MessageEvent::send(pPlayer->getControllingClient(), nMsgType, msg);
#endif
}

#ifdef NTJ_CLIENT
bool SpiritOperationManager::ClientSendSpiritOptMsg(Player *pPlayer, S32 opType, S32 nSlot, U32 nSpiritId)
{
	//元神操作消息
	if (!pPlayer || nSlot >= SPIRIT_MAXSLOTS)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_SPIRIT_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(2, opType, nSlot);
		ev->SetInt32ArgValues(1, nSpiritId);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool SpiritOperationManager::ClientSendSpiritSkillOptMsg(Player *pPlayer, S32 opType, S32 nSlot, S32 nIndex)
{
	//元神绝招操作消息
	if (!pPlayer || nSlot >= SPIRIT_MAXSLOTS)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_SPIRIT_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(3, opType, nSlot, nIndex);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool SpiritOperationManager::ClientSendAddTalentLevelMsg(Player *pPlayer, S32 nSpiritSlot, S32 nTalentIndex, S32 nAddLevel)
{
	//增加元神天赋等级
	if (!pPlayer)
		return false;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_SPIRIT_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(4, Player::SpiritOp_AddTalentLevel, nSpiritSlot, nTalentIndex, nAddLevel);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}
#endif

#ifdef NTJ_SERVER
//服务端通知客户端元神天赋提升等级的消息
bool SpiritOperationManager::ServerSendAddTalentLevelMsg(Player *pPlayer, S32 nSlot, S32 nIndex, bool bOptSuccess)
{
	GameConnection *conn = pPlayer->getControllingClient();
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_SPIRIT_OPERATE);
	if (!ev || !conn)
		return false;
	ev->SetIntArgValues(4, Player::SpiritOp_AddTalentLevel,  nSlot, nIndex, bOptSuccess);
	return conn->postNetEvent(ev);
}
//服务端通知客户端元神升级与获得经验的消息
bool SpiritOperationManager::ServerSendSpiritAddExpMsg(Player *pPlayer, S32 nSlot, S32 nAddedExp)
{
	GameConnection *conn = pPlayer->getControllingClient();
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_SPIRIT_OPERATE);
	if (!ev || !conn)
		return false;
	ev->SetIntArgValues(3, Player::SpiritOp_AddSpiritExp,  nSlot, nAddedExp);
	return conn->postNetEvent(ev);
}
bool SpiritOperationManager::ServerSendSpiritAddLevelMsg(Player *pPlayer, S32 nSlot, S32 nAddedLevel)
{
	GameConnection *conn = pPlayer->getControllingClient();
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_SPIRIT_OPERATE);
	if (!ev || !conn)
		return false;
	ev->SetIntArgValues(3, Player::SpiritOp_AddSpiritLevel, nSlot, nAddedLevel);
	return conn->postNetEvent(ev);
}
bool SpiritOperationManager::ServerSendSpiritOpenJueZhao(Player *pPlayer, S32 nSlot, S32 nIndex, bool bOptSuccess)
{
	if (!pPlayer || nSlot >= SPIRIT_MAXSLOTS)
		return false;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_SPIRIT_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetIntArgValues(3, Player::SpiritOp_OpenJueZhaoSlot, nSlot, nIndex, bOptSuccess);
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

void SpiritOperationManager::ServerSendUpdateSpiritSlotMsg(Player *pPlayer, S32 nSlot, U32 flag /*= 0xFFFFFFFF*/)
{
	if (!pPlayer || nSlot < 0 || nSlot >= SPIRIT_MAXSLOTS)
		return;

	GameConnection *conn = pPlayer->getControllingClient();
	SpiritEvent *ev = new SpiritEvent(nSlot, flag);
	if (conn && ev)
		conn->postNetEvent(ev);
}

bool SpiritOperationManager::ServerSendSpiritOptMsg(Player *pPlayer, S32 opType, S32 nSlot, bool bOptSuccess)
{
	if (!pPlayer || nSlot >= SPIRIT_MAXSLOTS)
		return false;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_SPIRIT_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	if (conn && ev)
	{
		switch(opType)
		{
		case Player::SpiritOp_OpenJueZhaoWnd:
			{
				ev->SetIntArgValues(1, opType);
			}
			break;
		default:
			{
				ev->SetIntArgValues(3, opType, nSlot, bOptSuccess);				
			}
			break;
		}
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool SpiritOperationManager::ServerSendInitSpiritMsg( Player *pPlayer )
{
	if (!pPlayer)
		return false;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_SPIRIT_OPERATE);
	GameConnection *conn = pPlayer->getControllingClient();
	SpiritTable &table = pPlayer->getSpiritTable();
	if (conn && ev)
	{
		ev->SetIntArgValues(3, Player::SpiritOp_Initialize, table.getActiveSlot(), table.getOpenedSlotFlag());
		conn->postNetEvent(ev);
		return true;
	}

	return false;
}

bool SpiritOperationManager::ServerDeleteSpiritSkillItem( Player *pPlayer )
{
	if (!pPlayer)
		return false;
	GameConnection *conn = pPlayer->getControllingClient();
	ShortcutObject *pShortcut = pPlayer->tempList.GetSlot(0);
	S32 nLockIndex = pShortcut->getLockedItemIndex();
	ItemShortcut *pSrcItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(nLockIndex));
	S32 nQuantity = pSrcItem->getRes()->getQuantity();
	if (nQuantity > 1)
	{
		pSrcItem->getRes()->setQuantity(nQuantity - 1);
		pSrcItem->setSlotState(ShortcutObject::SLOT_COMMON);
	}
	else
	{
		pPlayer->inventoryList.SetSlot(nLockIndex, 0);
	}
	
	pPlayer->tempList.SetSlot(0, NULL);
	pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_LOSE);
	pPlayer->tempList.UpdateToClient(conn, 0, ITEM_NOSHOW);
	return true;
}
#endif

ConsoleMethod(Player, CanAddNewSpirit, bool, 3, 3, "%player.CanAddNewSpirit(%nSpiritId);")
{
	S32 nSpiritId = atoi(argv[2]);
	return (SpiritOperationManager::CanAddNewSpirit(object, nSpiritId) == MSG_NONE);
}

ConsoleMethod(Player, AddNewSpirit, void, 3, 3, "%player.AddNewSpirit(%nSpiritId);")
{
	S32 nSpiritId = atoi(argv[2]);
	SpiritTable &table = object->getSpiritTable();
	S32 nFreeSlot = table.findEmptySlot();
	if (nFreeSlot == -1)
		return;

	bool bSuccess = SpiritOperationManager::AddNewSpirit(object, nSpiritId);
#ifdef NTJ_SERVER
	SpiritOperationManager::ServerSendSpiritOptMsg(object, Player::SpiritOp_AddSpirit, nFreeSlot, bSuccess);
#endif
}

#ifdef NTJ_CLIENT

/////////////////////////////////////客户端操作//////////////////////////////////////////////////
// 启用元神栏
ConsoleFunction(SpiritOperationManager_CanOpenSpiritSlot, bool, 2, 2, "SpiritOperationManager_CanOpenSpiritSlot(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	return (SpiritOperationManager::CanOpenSpiritSlot(pPlayer, nSlot) == MSG_NONE);
}
//打开元神栏
ConsoleFunction(SpiritOperationManager_OpenSpiritSlot, void, 2, 2, "SpiritOperationManager_OpenSpiritSlot(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	SpiritOperationManager::OpenSpiritSlot(pPlayer, nSlot);
}

// 删除元神
ConsoleFunction(SpiritOperationManager_CanRemoveSpirit, bool, 2, 2, "SpiritOperationManager_CanRemoveSpirit(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	return (SpiritOperationManager::CanDeleteSpirit(pPlayer, nSlot) == MSG_NONE);
}
ConsoleFunction(SpiritOperationManager_DeleteSpirit, void, 2, 2, "SpiritOperationManager_OpenSpiritSlot(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	SpiritOperationManager::DeleteSpirit(pPlayer, nSlot);
}

// 激活元神
ConsoleFunction(SpiritOperationManager_CanActiviateSpirit, bool, 2, 2, "SpiritOperationManager_CanActiviateSpirit(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	return (SpiritOperationManager::CanActiviateSpirit(pPlayer, nSlot) == MSG_NONE);
}
ConsoleFunction(SpiritOperationManager_ActiviateSpirit, void, 2, 2, "SpiritOperationManager_ActiviateSpirit(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	SpiritOperationManager::ActiviateSpirit(pPlayer, nSlot);
}

// 元神是否幻化
ConsoleFunction(SpiritOperationManager_IsSpiritHuanHua, bool, 2, 2, "SpiritOperationManager_IsSpiritHuanHua(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	return (SpiritOperationManager::CanCancelHuanHuaSpirit(pPlayer, nSlot) == MSG_NONE);
}

// 幻化元神
ConsoleFunction(SpiritOperationManager_CanHuanHuaSpirit, bool, 2, 2, "SpiritOperationManager_CanHuanHuaSpirit(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	return (SpiritOperationManager::CanHuanHuaSpirit(pPlayer, nSlot) == MSG_NONE);
}
ConsoleFunction(SpiritOperationManager_HuanHuaSpirit, void, 2, 2, "SpiritOperationManager_ActiviateSpirit(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	SpiritOperationManager::HuanHuaSpirit(pPlayer, nSlot);
}

// 取消幻化元神
ConsoleFunction(SpiritOperationManager_CanCancelHuanHuaSpirit, bool, 2, 2, "SpiritOperationManager_CanCancelHuanHuaSpirit(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	return (SpiritOperationManager::CanCancelHuanHuaSpirit(pPlayer, nSlot) == MSG_NONE);
}
ConsoleFunction(SpiritOperationManager_CancelHuanHuaSpirit, void, 2, 2, "SpiritOperationManager_CancelHuanHuaSpirit(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	SpiritOperationManager::CancelHuanHuaSpirit(pPlayer, nSlot);
}

// 绝招栏解封
ConsoleFunction(SpiritOperationManager_OpenJueZhao, void, 3, 3, "SpiritOperationManager_OpenJueZhao(%nSpiritSlot, %nSlot);")
{
	S32 nSpiritSlot = atoi(argv[1]);
	S32 nSlot = atoi(argv[2]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	SpiritOperationManager::OpenSpiritJueZhao(pPlayer, nSpiritSlot, nSlot);
}

// 增加元神绝招
ConsoleFunction(SpiritOperationManager_AddJueZhao, void, 3, 3, "SpiritOperationManager_AddJueZhao(%nSpiritSlot, %nSlot);")
{
	S32 nSpiritSlot = atoi(argv[1]);
	S32 nSlot = atoi(argv[2]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	SpiritOperationManager::AddNewSpiritJueZhao(pPlayer, nSpiritSlot, nSlot);
}

// 删除元神绝招
ConsoleFunction(SpiritOperationManager_RemoveJueZhao, void, 3, 3, "SpiritOperationManager_RemoveJueZhao(%nSpiritSlot, %nSlot);")
{
	S32 nSpiritSlot = atoi(argv[1]);
	S32 nSlot = atoi(argv[2]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	SpiritOperationManager::RemoveSpiritJueZhao(pPlayer, nSpiritSlot, nSlot);
}

/////////////////////////////////////客户端属性显示//////////////////////////////////////////////////
ConsoleFunction(SpiritOperationManager_AddTalentLevel, bool, 4, 4, "SpiritOperationManager_AddTalentLevel(%nSpiritSlot, %nTalentIndex, %nAddLevel);")
{
	S32 nSpiritSlot		= atoi(argv[1]);
	S32 nTalentIndex	= atoi(argv[2]);
	S32 nAddLevel		= atoi(argv[3]);

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	return SpiritOperationManager::AddTalentLevel(pPlayer, nSpiritSlot, nTalentIndex, nAddLevel);
}
ConsoleFunction(SpiritOperationManager_GetTalentBackground, StringTableEntry, 2, 2, "SpiritOperationManager_GetTalentBackground(%nSpiritSlot);")
{
	S32 nSpiritSlot = atoi(argv[1]);

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	return pPlayer->getSpiritTable().getTalentBackImage(nSpiritSlot);
}

ConsoleFunction(SpiritOperationManager_IsTalentExist, bool, 3, 3, "SpiritOperationManager_IsTalentExist(%nSpiritSlot, %nIndex);")
{
	S32 nSpiritSlot = atoi(argv[1]);
	S32 nTalentIndex = atoi(argv[2]);

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return pPlayer->getSpiritTable().isTalentExist(nSpiritSlot, nTalentIndex);
}

ConsoleFunction(SpiritOperationManager_GetTalentLevel, S32, 3, 3, "SpiritOperationManager_GetTalentLevel(%nSpiritSlot, %nIndex);")
{
	S32 nSpiritSlot = atoi(argv[1]);
	S32 nTalentIndex = atoi(argv[2]);

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return -1;
	return pPlayer->getSpiritTable().getTalentLevel(nSpiritSlot, nTalentIndex);
}

ConsoleFunction(SpiritOperationManager_GetTalentMaxLevel, S32, 3, 3, "SpiritOperationManager_GetTalentMaxLevel(%nSpiritSlot, %nIndex);")
{
	S32 nSpiritSlot = atoi(argv[1]);
	S32 nTalentIndex = atoi(argv[2]);

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return -1;
	return pPlayer->getSpiritTable().getTalentMaxLevel(nSpiritSlot, nTalentIndex);
}

ConsoleFunction(SpiritOperationManager_IsJueZhaoOpened, bool, 3, 3, "SpiritOperationManager_IsJueZhaoOpened(%nSpiritSlot, %nSlot);")
{
	S32 nSpiritSlot = atoi(argv[1]);
	S32 nJueZhaoSlot = atoi(argv[2]);

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return pPlayer->getSpiritTable().isJueZhaoOpened(nSpiritSlot, nJueZhaoSlot);
}

ConsoleFunction(SpiritOperationManager_IsSpiritExist, bool, 2, 2, "SpiritOperationManager_IsSpiritExist(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return pPlayer->getSpiritTable().isSpiritExist(nSlot);
}

ConsoleFunction(SpiritOperationManager_IsSpiritOpened, bool, 2, 2, "SpiritOperationManager_IsSpiritOpened(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return pPlayer->getSpiritTable().isSpiritOpened(nSlot);
}

ConsoleFunction(SpiritOperationManager_IsSpiritActive, bool, 2, 2, "SpiritOperationManager_IsSpiritActive(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;
	return pPlayer->getSpiritTable().isSpiritActive(nSlot);
}

ConsoleFunction(SpiritOperationManager_GetSpiritType, S32, 2, 2, "SpiritOperationManager_GetSpiritType(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return -1;
	return pPlayer->getSpiritTable().getSpiritType(nSlot);
}

ConsoleFunction(SpiritOperationManager_GetSpiritName, StringTableEntry, 2, 2, "SpiritOperationManager_GetSpiritName(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	return pPlayer->getSpiritTable().getSpiritName(nSlot);
}

ConsoleFunction(SpiritOperationManager_GetSpiritLevel, S32, 2, 2, "SpiritOperationManager_GetSpiritLevel(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return pPlayer->getSpiritTable().getCurrLevel(nSlot);
}

ConsoleFunction(SpiritOperationManager_GetSpiritMaxLevel, S32, 2, 2, "SpiritOperationManager_GetSpiritMaxLevel(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return pPlayer->getSpiritTable().getMaxLevel(nSlot);
}

ConsoleFunction(SpiritOperationManager_GetSpiritCurrExp, S32, 2, 2, "SpiritOperationManager_GetSpiritCurrExp(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return pPlayer->getSpiritTable().getCurrExp(nSlot);
}

ConsoleFunction(SpiritOperationManager_GetSpiritMaxExp, S32, 2, 2, "SpiritOperationManager_GetSpiritMaxExp(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return pPlayer->getSpiritTable().getMaxExp(nSlot);
}

ConsoleFunction(SpiritOperationManager_GetTalentExtPts, S32, 2, 2, "SpiritOperationManager_GetTalentExtPts(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return pPlayer->getSpiritTable().getTalentExtPts(nSlot);
}

ConsoleFunction(SpiritOperationManager_GetTalentLeftPts, S32, 2, 2, "SpiritOperationManager_GetTalentLeftPts(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return pPlayer->getSpiritTable().getTalentLeftPts(nSlot);
}
ConsoleFunction(SpiritOperationManager_GetTalentPts, S32, 2, 2, "SpiritOperationManager_GetTalentPts(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return pPlayer->getSpiritTable().getTalentPts(nSlot);
}
ConsoleFunction(SpiritOperationManager_GetTalentSetId, S32, 2, 2, "SpiritOperationManager_GetTalentSetId(%nSlot);")
{
	S32 nSlot = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
	return pPlayer->getSpiritTable().getTalentSetId(nSlot);
}

ConsoleFunction(closeSpiritSkillWnd, void, 1, 1, "%closeSpiritSkillWnd();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return;
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_SPIRIT_OPERATE);
	if (ev)
	{
		ev->SetIntArgValues(1, Player::SpiritOp_CloseJueZhaoWnd);
		conn->postNetEvent(ev);
	}
}


#endif



/////////////////服务端脚本/////////////////
#ifdef NTJ_SERVER
ConsoleMethod(Player, openSpiritSkillWnd, void, 3, 3, "%player.openSpiritSkillWnd(%npc);")
{
	U32 nNpcId = atoi(argv[2]);
	NpcObject *pNpc = dynamic_cast<NpcObject *>(Sim::findObject(nNpcId));
	if (!pNpc)
		return;
	SpiritOperationManager::OpenSpiritSkillWnd(object, pNpc);
}

ConsoleMethod(Player, getActiveSpiritSlot, S32, 2, 2, "%player.getActiveSpiritSlot();")
{
	SpiritTable &table = (SpiritTable &)object->getSpiritTable();
	return table.getActiveSlot();
}

ConsoleMethod(Player, addSpiritExp, bool, 4, 4, "%player.addSpiritExp(%nSlot, %nExp);")
{
	S32 nSlot = atoi(argv[2]);
	U32 nExp = atoi(argv[3]);
	SpiritTable &table = (SpiritTable &)object->getSpiritTable();
	return table.addExp(nSlot, nExp, object);
}

ConsoleMethod(Player, addSpiritLevel, bool, 4, 4, "%player.addSpiritLevel(%nSlot, %nLevel);")
{
	S32 nSlot = atoi(argv[2]);
	U32 nLevel = atoi(argv[3]);
	SpiritTable &table = (SpiritTable &)object->getSpiritTable();
	return table.addLevel(nSlot, nLevel, object);
}

ConsoleMethod(Player, AddSpiritJingJie, bool, 3, 4, "%player.addSpiritJingJie(%nSlot, %nJingJie);")
{
	S32 nSlot = atoi(argv[2]);
	U32 nJingJie = atoi(argv[3]);
	SpiritTable &table = (SpiritTable &)object->getSpiritTable();
	return table.addJingJie(nSlot, nJingJie, object);
}

//服务端打开元神栏
ConsoleMethod(Player, OpenSpiritSlot, bool, 2, 2, "%player.OpenSpiritSlot();")
{
	SpiritTable& table = (SpiritTable &)object->getSpiritTable();
	S32 nLockSlot = table.findFirstLockedSlot();
	bool bSuccess = SpiritOperationManager::OpenSpiritSlot(object, nLockSlot);
	return SpiritOperationManager::ServerSendSpiritOptMsg(object, Player::SpiritOp_OpenSpiritSlot, nLockSlot, bSuccess);
}

#endif