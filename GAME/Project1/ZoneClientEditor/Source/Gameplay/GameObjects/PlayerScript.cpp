//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/Item/ItemBaseData.h"
#include "console/console.h"
#include "Gameplay/Mission/PlayerMission.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/Item/PrizeBox.h"
#include "Gameplay/item/res.h"

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/Item/Prescription.h"
#endif

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ai/PlayerAISimple.h"
#endif


//-----------------------------------------------------------------------------
// 
// 客户端和服务端共有脚本
// 
//-----------------------------------------------------------------------------

// ============================================================================
// 一般性脚本方法
// ============================================================================
// ----------------------------------------------------------------------------
// 添加金钱
ConsoleFunction(addMoney, bool, 3, 4, "addMoney(%playerid, %money, %type)" )
{
#ifdef NTJ_SERVER
	U32 playerID = dAtoi(argv[1]);
	Player* player = g_ServerGameplayState->GetPlayer(playerID);
	if(player == NULL)
		return false;
	U32 type = Player::Currentcy_Money;
	if(argc > 3)
		type = dAtoi(argv[3]);
	return player->addMoney(dAtoi(argv[2]), type);
#endif
	return true;
}

ConsoleFunction(hlyAddMoney, bool, 2, 2, "hlyAddMoney(%nMoney);" )
{
#ifdef NTJ_CLIENT
	S32 nMoney = dAtoi(argv[1]);
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(player == NULL)
		return false;
	ClientGameNetEvent *ev = new ClientGameNetEvent(FRIEND_REQUEST+100);
	ev->SetInt32ArgValues(2, player->getPlayerID(), nMoney);
	GameConnection *conn = player->getControllingClient();
	return conn->postNetEvent(ev);
#endif
	return true;
}


// ----------------------------------------------------------------------------
// 添加等级
ConsoleFunction(addLevel, void, 3, 3, "addLevel(%playerid, %level)" )
{
#ifdef NTJ_SERVER
	U32 playerID = dAtoi(argv[1]);
	Player* player = g_ServerGameplayState->GetPlayer(playerID);
	if(player == NULL)
		return;
	player->addLevel(dAtoi(argv[2]));
#endif
}

// ----------------------------------------------------------------------------
// 扣减金钱
ConsoleFunction(reduceMoney, bool, 3, 4, "reduceMoney(%playerid, %money, %type)" )
{
#ifdef NTJ_SERVER
	U32 playerID = dAtoi(argv[1]);
	Player* player = g_ServerGameplayState->GetPlayer(playerID);
	if(player == NULL)
		return false;
	U32 type = Player::Currentcy_Money;
	if(argc > 3)
		type = dAtoi(argv[3]);
	return player->reduceMoney(dAtoi(argv[2]), type);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 学习配方
ConsoleFunction(addPrescription,bool,3,3,"addPrescription(%player,%presId)")
{
#ifdef NTJ_SERVER
	Player* pPlayer = dynamic_cast<Player*>(Sim::findObject(argv[1]));
	if(NULL == pPlayer || NULL == pPlayer->pPrescription)
		return false;

	GameConnection* conn = pPlayer->getControllingClient();
	if(NULL == conn)
		return false;

	U32 presId = atoi(argv[2]);
	if(presId == 0)
		return false;

	if(pPlayer->pPrescription->addPrescription(presId))
		return pPlayer->pPrescription->updateToClient(conn,presId);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 学习生活技能
ConsoleFunction(LearnLivingSkill,bool,3,3,"LearnLivingSkill(%player,%livingSkillId)")
{
	Player* pPlayer = dynamic_cast<Player*>(Sim::findObject(argv[1]));
	if(!pPlayer || !pPlayer->getControllingClient() || !pPlayer->pLivingSkill)
		return false;

	U32 skillId = dAtoi(argv[2]);
	return pPlayer->pLivingSkill->learnLivingSkill(pPlayer,skillId);
}
//判断该生活技能是否已经学会
ConsoleFunction(isLearnLivingSkill,bool,3,3,"isLearnLivingSkill(%playerId,%livingSkillId)")
{
	Player* pPlayer = NULL;

#ifdef NTJ_SERVER
	pPlayer = g_ServerGameplayState->GetPlayer(atoi(argv[1]));
	if(NULL == pPlayer)
		return false;
#endif

#ifdef NTJ_CLIENT
	pPlayer = g_ClientGameplayState->GetControlPlayer();
#endif

	return pPlayer->pLivingSkill->isLearn(atoi(argv[2]));
}
// ----------------------------------------------------------------------------
// 增加生活技能奖励
ConsoleFunction(addLivingSkillGuerdon,void,4,4,"addLivingSkillGuerdon(%player,%livingSkillId,%extRipe)")
{
#ifdef NTJ_SERVER
	Player* pPlayer = dynamic_cast<Player*>(Sim::findObject(argv[1]));
	if(pPlayer)
		pPlayer->setLivingSkillGuerdon(dAtoi(argv[2]),dAtoi(argv[3]));
#endif
}

// ----------------------------------------------------------------------------
// 获取生活技能熟练度
ConsoleMethod(Player,getLivingSkillRipe,S32,3,3,"%player.getLivingSkillRipe(%livingSkillId)")
{
	return object->getLivingSkillRipe(atoi(argv[2]));
}
// ----------------------------------------------------------------------------
// 获取生活等级
ConsoleMethod(Player,getLivingLevel,S32,2,2,"%player.getLivingLevel()")
{
	return object->getLivingLevel();
}
// ----------------------------------------------------------------------------
// 获取生活经验
ConsoleMethod(Player,getLivingExp,S32,2,2,"%player.getLivingExp()")
{
	return object->getLivingExp();
}
// ----------------------------------------------------------------------------
// 获取当前生活级别所需的生活经验
ConsoleMethod(Player,getLivingCurrentLevelExp,S32,2,2,"%player.getLivingCurrentLevelExp()")
{
	return object->getLivingCurrentLevelExp();
}
// ----------------------------------------------------------------------------
// 添加金钱
ConsoleMethod( Player, addMoney, bool, 3, 4, "%player.addMoney(%money, %type)")
{
#ifdef NTJ_SERVER
	U32 type = Player::Currentcy_Money;
	if(argc > 3)
		type = dAtoi(argv[3]);
	return object->addMoney(dAtoi(argv[2]), type);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 添加玩家等级
ConsoleMethod( Player, addLevel, void, 3, 3, "%player.addLevel(%level)")
{
#ifdef NTJ_SERVER
	object->addLevel(dAtoi(argv[2]));
#endif
}

// ----------------------------------------------------------------------------
// 添加金钱
ConsoleMethod( Player, reduceMoney, bool, 3, 4, "%player.reduceMoney(%money, %type)")
{
#ifdef NTJ_SERVER
	U32 type = Player::Currentcy_Money;
	if(argc > 3)
		type = dAtoi(argv[3]);
	return object->reduceMoney(dAtoi(argv[2]), type);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 获取玩家身上装备ID
ConsoleMethod( Player, getEquip, bool, 3, 3, "%player.getEquip(%pos)")
{
#ifdef NTJ_SERVER
	S32 pos = dAtoi(argv[3]);
	if(pos <= 0 || pos > Res::EQUIPPLACEFLAG_FAMLIYFASHION)
		return 0;

	Player* player = (Player*)object;
	ItemShortcut* pItem = (ItemShortcut*)player->equipList.GetSlot(pos);
	return pItem ? pItem->getRes()->getItemID() : 0;
#endif
	return 0;
}

// ----------------------------------------------------------------------------
// 更换角色数据
ConsoleMethod( Player, ChangeData, void, 3, 3, "")
{
	object->onChangeDataBlock(g_PlayerRepository.GetPlayerData(dAtoi(argv[2])));
}

// ----------------------------------------------------------------------------
// 获取角色金钱
ConsoleMethod( Player, getMoney, S32, 2, 3, "obj.getMoney(%type)")
{
	U32 type = Player::Currentcy_Money;
	if(argc > 2)
		type = dAtoi(argv[2]);
	return object->getMoney(type);
}

// ----------------------------------------------------------------------------
// 获得玩家等级
ConsoleMethod(Player, getLevel, S32, 2, 2, "obj.getLevel()")
{
	return object->getLevel();
}
// ============================================================================
// 物品相关脚本方法
// ============================================================================
// ----------------------------------------------------------------------------
// 批量添加物品(预置方法)
ConsoleFunction( putItem, S32, 4, 4, "putItem(%playerid, %itemid, %itemnum)")
{
#ifdef NTJ_SERVER
	U32	playerID	= dAtoi(argv[1]);
	U32	itemID		= dAtoi(argv[2]);
	S32	itemNum		= dAtoi(argv[3]);
	return g_ItemManager->putItem(playerID, itemID, itemNum) == MSG_NONE;
#endif
	return 0;
}

// ----------------------------------------------------------------------------
// 批量添加物品(确定方法)
ConsoleFunction(addItem, bool, 2, 2, "addItem(%playerid)" )
{
#ifdef NTJ_SERVER
	U32 playerID = dAtoi(argv[1]);
	return g_ItemManager->batchItem(playerID) == MSG_NONE;
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 批量添加物品(预置方法)
ConsoleMethod( Player, putItem, S32, 4, 4, "%player.PutItem(%itemid, %itemnum)")
{
#ifdef NTJ_SERVER
	U32	itemID		= dAtoi(argv[2]);
	S32	itemNum		= dAtoi(argv[3]);
	bool isRandExt	= dAtob(argv[4]);
	return g_ItemManager->putItem(object->getPlayerID(), itemID, itemNum) == MSG_NONE;
#endif
	return 0;
}

// ----------------------------------------------------------------------------
// 批量添加物品(确定方法)
ConsoleMethod( Player, addItem, bool, 2, 2, "%player.addItem()")
{
#ifdef NTJ_SERVER
	return g_ItemManager->batchItem(object->getPlayerID()) == MSG_NONE;
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 获取玩家角色名称
ConsoleMethod( Player, getPlayerName, const char*, 2, 2, "%player.getPlayerName()")
{
	return object->getPlayerName();
}

// ----------------------------------------------------------------------------
// 获取玩家角色交互对象
ConsoleMethod( Player, getInteraction, S32, 2, 2, "%player.getInteraction()")
{
	SceneObject* obj = object->getInteraction();
	return obj ? obj->getId() : 0;
}

// ----------------------------------------------------------------------------
// 获取玩家交互状态
ConsoleMethod( Player, getInteractionState, S32, 2, 2, "%player.getInteractionState()")
{
	return object->getInteractionState();
}

// ----------------------------------------------------------------------------
// 添加单个物品
ConsoleFunction( addItemToInventory, S32, 4, 4, "addItemToInventory(%playerid, %itemid, %num)")
{
#ifdef NTJ_SERVER
	Player* pPlayer = g_ServerGameplayState->GetPlayer(dAtoi(argv[1]));
	if(!pPlayer)
		return -1;
	S32 index;
	if(g_ItemManager->addItemToInventory(pPlayer, dAtoi(argv[2]), index, dAtoi(argv[3]), ITEM_GET) == MSG_NONE)
		return index;
	else 
		return -1;
#endif
	return -1;
}

// ----------------------------------------------------------------------------
// 删除单个物品
ConsoleFunction(delItemFromInventory, bool, 4, 4, "delItemFromInventory(%playerid, %itemid, %num)")
{
#ifdef NTJ_SERVER
	Player* pPlayer = g_ServerGameplayState->GetPlayer(dAtoi(argv[1]));
	if(!pPlayer)
		return false;
	if(g_ItemManager->delItemFromInventory(pPlayer, dAtoi(argv[2]), dAtoi(argv[3]), ITEM_LOSE) == MSG_NONE)
		return true;
#endif
	return false;
}

// ----------------------------------------------------------------------------
// 删除包裹栏指定位置的某个物品
ConsoleFunction(delItemFromInventoryByIndex, bool, 4, 4, "delItemFromInventoryByIndex(%playerid, %index, %num)")
{
#ifdef NTJ_SERVER
	Player* pPlayer = g_ServerGameplayState->GetPlayer(dAtoi(argv[1]));
	if(!pPlayer)
		return false;
	if(g_ItemManager->delItemFromInventoryByIndex(pPlayer, dAtoi(argv[2]), dAtoi(argv[3])) == MSG_NONE)
		return true;
#endif
	return false;
}

// ----------------------------------------------------------------------------
// 添加单个物品
ConsoleMethod( Player, addItemToInventory, bool, 4, 4, "%player.addItemToInventory(%itemid,%num)")
{
#ifdef NTJ_SERVER
	S32 index;
	if(g_ItemManager->addItemToInventory(object, dAtoi(argv[2]), index, dAtoi(argv[3]), ITEM_GET) == MSG_NONE)
		return index;
	else
		return -1;
#endif
	return -1;
}

// ----------------------------------------------------------------------------
// 添加物品/技能/生活技能/宠物/骑宠到快捷栏
ConsoleMethod( Player, addObjectToPanel, bool, 5, 5, "%player.addObjectToPanel(%type, %id, %index)")
{
#ifdef NTJ_SERVER
	return g_ItemManager->addObjectToPanel(object, dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4])) == MSG_NONE;
#endif
	return false;
}

// ----------------------------------------------------------------------------
// 添加物品到装备栏
ConsoleMethod( Player, addItemToEquip, bool, 3, 3, "%player.addItemToEquip(%itemid)")
{
#ifdef NTJ_SERVER
	return g_ItemManager->addItemToEquip(object, dAtoi(argv[2])) == MSG_NONE;
#endif
	return false;
}

// ----------------------------------------------------------------------------
// 获取玩家物品栏某物品数量
ConsoleMethod( Player, getItemCount, S32, 3, 3, "%player.getItemCount(%itemid)")
{
	return object->inventoryList.GetItemCount(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 获取玩家角色ID
ConsoleMethod( Player, getPlayerID, S32, 2, 2, "%player.getPlayerID()")
{
	return object->getPlayerID();
}

// ----------------------------------------------------------------------------
// 获取逻辑事件类型
ConsoleMethod( Player, getLogicEventType, S32, 3, 3, "%player.getLogicEventType(%eid)")
{
	Player::stLogicEvent* ev = object->findLogicEvent(dAtoi(argv[2]));
	return ev ? ev->type : 0;
}

// ----------------------------------------------------------------------------
// 获取逻辑事件发起人名称
ConsoleMethod( Player, getLogicEventSenderName, const char*, 3, 3, "%player.getLogicEventSenderName(%eid)")
{
	Player::stLogicEvent* ev = object->findLogicEvent(dAtoi(argv[2]));
	return ev ? ev->senderName : "未知目标";
}

// ----------------------------------------------------------------------------
// 获取逻辑事件发起人ID
ConsoleMethod( Player, getLogicEventSenderId, S32, 3, 3, "%player.getLogicEventSenderId(%eid)")
{
	Player::stLogicEvent* ev = object->findLogicEvent(dAtoi(argv[2]));
	return ev ? ev->sender : 0;
}

// ----------------------------------------------------------------------------
// 获取逻辑事件总数
ConsoleMethod( Player, getLogicEventCount, S32, 2, 2, "%player.getLogicEventCount()")
{
	return object->getLogicEventCount();
}

// ----------------------------------------------------------------------------
// 获取逻辑事件类型
ConsoleMethod( Player, getLogicEventTypeByIndex, S32, 3, 3, "%player.getLogicEventTypeByIndex(%index)")
{
	Player::stLogicEvent* ev = object->getLogicEvent(dAtoi(argv[2]));
	return ev ? ev->type : 0;
}

// ----------------------------------------------------------------------------
// 以索引值获取逻辑事件ID
ConsoleMethod( Player, getLogicEventIDByIndex, S32, 3, 3, "%player.getLogicEventIDByIndex(%index)")
{
	Player::stLogicEvent* ev = object->getLogicEvent(dAtoi(argv[2]));
	return ev ? ev->id : 0;
}

// ----------------------------------------------------------------------------
// 删除指定ID的逻辑事件
ConsoleMethod( Player, removeLogicEvent, void, 3, 3, "%player.removeLogicEvent(%id)")
{
	object->removeLogicEvent(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 以索引值获取逻辑事件ID
ConsoleMethod( Player, getLogicEventRID, S32, 3, 3, "%player.getLogicEventRID(%id)")
{
	Player::stLogicEvent* ev = object->findLogicEvent(dAtoi(argv[2]));
	return ev ? ev->relationid : 0;
}


// ----------------------------------------------------------------------------
//设置PK状态
ConsoleMethod(Player, setPKState, void, 3, 3, "%player.setPKState(bSetPK)")
{
	bool bSetPK = atoi(argv[2]);
#ifdef NTJ_CLIENT	
	GameConnection *conn = dynamic_cast<GameConnection *>(GameConnection::getConnectionToServer());
	if (conn)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_SWITCH);
		pEvent->SetIntArgValues(2, Player::SWITCHSTATE_PK, bSetPK);
		conn->postNetEvent(pEvent);
	}
#endif
#ifdef NTJ_SERVER
	object->setSwitchState(Player::SWITCHSTATE_PK, bSetPK);
#endif
}

// ----------------------------------------------------------------------------
// 获得PK状态
ConsoleMethod(Player, getPKState, bool, 2, 2, "%player.getPKState()")
{
	return object->getSwitchState(Player::SWITCHSTATE_PK);
}

// ----------------------------------------------------------------------------
// 获得当前精力值
ConsoleMethod(Player,getVigor,S32,2,2,"%player.getVigor()")
{
	return object->getVigor();
}
// ----------------------------------------------------------------------------
// 获得当前精力值
ConsoleMethod(Player,getMaxVigor,S32,2,2,"%player.getMaxVigor()")
{
	return object->getMaxVigor();
}

// ----------------------------------------------------------------------------
// 设置声望
ConsoleMethod(Player, setFame, void, 5, 5, "%player.setFame(%infl, %fame, %favour)")
{
	object->setFame(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]));
}

// ----------------------------------------------------------------------------
// 获取声望
ConsoleMethod(Player, getFame, S32, 3, 3,"%player.getFame()")
{
	return object->getFame(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 增加好感度
ConsoleMethod(Player, addFavour, void, 4, 4, "%player.addFavour(%infl, %favour)")
{
	object->addFavour(dAtoi(argv[2]), dAtoi(argv[3]));
}

// ----------------------------------------------------------------------------
// 获取好感度
ConsoleMethod(Player, getFavour, S32, 3, 3,"%player.getFavour()")
{
	return object->getFavour(dAtoi(argv[2]));
}


//-----------------------------------------------------------------------------
// 
// 客户端脚本
// 
//-----------------------------------------------------------------------------

#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// 自动拾取
ConsoleFunction(SetAIState_Pickup, void, 1, 1, "SetAIState_Pickup()")
{
	Player *player = g_ClientGameplayState->GetControlPlayer();
	if (!player)
		return;

	// 首先取得最近的一个拾取物
	PrizeBox* pPrizeBox = g_PrizeBoxManager.GetNearest(player, 100.0f);
	if (!pPrizeBox)
	{
		g_PrizeBoxManager.ClearHadPicked();
	}
	else
	{
		player->SetTarget(pPrizeBox);
	}
}

// ----------------------------------------------------------------------------
// 获取目标玩家信息
ConsoleFunction(GetTargetPlayerInfo, const char*, 1,1, "GetTargetPlayerInfo()" )
{
	static char msg[100] = { 0 };
	Player* pPlayer = dynamic_cast<Player*>(g_ClientGameplayState->GetTarget());
	if( pPlayer )
	{
		stTeamInfo& mTeamInfo = pPlayer->getTeamInfo();
		dSprintf(msg, sizeof(msg), "ID:%d TID:%d BTL:%d NAME:%s TNAME:%s", pPlayer->getPlayerID(), mTeamInfo.m_nId, mTeamInfo.m_bIsCaption, pPlayer->getPlayerName(), mTeamInfo.m_szName);
	}
	return msg;
}
// ----------------------------------------------------------------------------
// 清除路径
ConsoleFunction(clearPath, void, 1, 1,"clearPath()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(pPlayer && g_ClientGameplayState->isStartNavigation())
	{
		pPlayer->m_pAI->ClearAIPath();
		g_ClientGameplayState->clearPath();
	}
}

// ----------------------------------------------------------------------------
// 取消跨服寻径
ConsoleFunction(clearStrideServerPath, void, 1, 1, "clearStrideServerPath()")
{
	 Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	 if(!pPlayer)
		 return;
	 pPlayer->m_pAI->ClearAIPath();
	 g_ClientGameplayState->getStrideServerPath().clear();
	 Con::executef("closeStrideFindPath");
	 
}

// ----------------------------------------------------------------------------
// 传送
ConsoleFunction(SitPlane,void,6,6,"SitPlane(%InOut,%MapId,%PosX,%PosY,%PosZ)")
{
	GameConnection *conn = dynamic_cast<GameConnection*>(GameConnection::getConnectionToServer());
	if (conn)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_TRANSPORT);
		pEvent->SetIntArgValues(5,dAtoi(argv[1]),dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]),dAtoi(argv[5]));
		conn->postNetEvent(pEvent);
	}
} 

// ----------------------------------------------------------------------------
// 添加物品到任务奖励栏
ConsoleMethod(Player, addItemToMission, bool, 5, 5, "%player.addItemToMission(%type, %item, %num)")
{
	return g_ItemManager->addItemToMission(object, dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]));
}

// ----------------------------------------------------------------------------
// 清除任务奖励栏
ConsoleMethod(Player, clearMissionItem, void, 2, 3, "%player.clearMissionItem(%type)")
{
	S32 type = -1;
	if(argc > 2)
		type = dAtoi(argv[2]);
	object->missionItemList.Clear(type);
}

// ----------------------------------------------------------------------------
// 获取任务奖励栏的槽位数
ConsoleMethod(Player, getMissionItemSize, S32, 2, 3, "%player.getMissionItemSize(%type)")
{
	S32 type = 0;
	if(argc > 2)
		type = dAtoi(argv[2]);
	return object->missionItemList.Size(type);
}

#endif//NTJ_CLIENT



//-----------------------------------------------------------------------------
// 
// 服务端脚本
// 
//-----------------------------------------------------------------------------

#ifdef NTJ_SERVER
// ----------------------------------------------------------------------------
// 判断某物品是否限制了使用次数
ConsoleMethod(Player,IsItemUsedTimes, bool,4,4,"IsItemUsedTimes(%ItemID, %index)")
{
	ItemShortcut* pItem = (ItemShortcut*)object->inventoryList.GetSlot(dAtoi(argv[3]));
	return (pItem && pItem->getRes()->getResUsedTimes() > 0);
}

// ----------------------------------------------------------------------------
// 设置物品栏指定位置物品的使用次数
ConsoleMethod(Player,setItemUsedTimes, bool,5,5,"setItemUsedTimes(%ItemID, %index, %times)")
{
	enWarnMessage msgCode = g_ItemManager->setItemUsedTimes(object, dAtoi(argv[3]), dAtoi(argv[2]), dAtoi(argv[4]));
	return msgCode == MSG_NONE;
}

// ----------------------------------------------------------------------------
// 获取物品栏指定位置物品的使用次数
ConsoleMethod(Player,getItemUsedTimes, S32,4,4,"setItemUsedTimes(%ItemID, %index)")
{
	return g_ItemManager->getItemUsedTimes(object, dAtoi(argv[3]), dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 预设置PutItem物品的属性
ConsoleMethod(Player,setPutItemProperty, bool, 5, 9,"setPutItemProperty(%index, %type, %value1, %value2, %value3, %value4, %value5)")
{
	if(argc == 5)
		return g_ItemManager->setPutItemProperty(object, dAtoi(argv[2]), dAtoi(argv[3]), 1, argv[4]);
	else if(argc == 6)
		return g_ItemManager->setPutItemProperty(object, dAtoi(argv[2]), dAtoi(argv[3]), 2, argv[4], argv[5]);
	else if(argc == 7)
		return g_ItemManager->setPutItemProperty(object, dAtoi(argv[2]), dAtoi(argv[3]), 3, argv[4], argv[5], argv[6]);
	else if(argc == 8)
		return g_ItemManager->setPutItemProperty(object, dAtoi(argv[2]), dAtoi(argv[3]), 4, argv[4], argv[5], argv[6], argv[7]);
	else
		return g_ItemManager->setPutItemProperty(object, dAtoi(argv[2]), dAtoi(argv[3]), 5, argv[4], argv[5], argv[6], argv[7], argv[8]);
}

// ----------------------------------------------------------------------------
// 连接创建玩家放在了这里
ConsoleMethod(GameConnection, CreatePlayer, bool, 4, 4, "(%UID,%PlayerId) ZoneServer createPlayer")
{
	T_UID UID		= dAtoi(argv[2]);
	U32 PlayerId	= dAtoi(argv[3]);

	stPlayerStruct *pPlayerData = g_ServerGameplayState->GetPlayerLoginData(UID,PlayerId);
	if (!pPlayerData)
		return false;

	// Create the player object
	Player *pPlayer = new Player();
	pPlayer->initializeData(object,UID,pPlayerData);
	if(!pPlayer->registerObject())
	{
		delete pPlayer;
		return false;
	}

	// Give the client control of the player
	object->setModDynamicFields(true);
	char buff[10];
	dSprintf(buff,sizeof(buff),"%d",pPlayer->getId());
	object->setDataField("Player","", buff);

	g_ServerGameplayState->AddInPlayerManager(pPlayer);
	
	// 增加玩家登陆到游戏后,自动保存信息功能
	pPlayer->saveToWorld();

	return true;
}

// ---------------------------------------------------------------------------
// 服务端获得玩家对象
ConsoleFunction( SptGetPlayer, S32, 2, 2, "" )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1]) );
	if( pPlayer )
		return pPlayer->getId();

	return 0;
}

// ----------------------------------------------------------------------------
// 获取当前触发区域ID
ConsoleMethod(Player,getTriggerID, S32,2,2,"getTriggerID()")
{
	return object->getTriggerId();
}

// ----------------------------------------------------------------------------
// 脚本方法传送玩家
ConsoleMethod(Player,TransportObject,void,7,7,"TransportObject(%TriggerId,%MAP,%x,%y,%z)")
{
	object->preTransportObject(g_ServerGameplayState->getCurrentLineId(),dAtoi(argv[2]),dAtoi(argv[3]),dAtof(argv[4]),dAtof(argv[5]),dAtof(argv[6]));
}

// ----------------------------------------------------------------------------
// 测试脚本传送
ConsoleMethod(Player,TTO,void,2,2,"TTO()")
{
	object->testTransportObject();
}

ConsoleMethod(Player,SendToWorld,void,2,2,"SendToWorld();")
{
	object->saveToWorld();
}

// ----------------------------------------------------------------------------
// 召唤宠物
ConsoleMethod(Player, spawnPet, void, 3, 3, "spawnPet(slot);")
{
#ifdef NTJ_CLIENT
	PetOperationManager::SpawnPet(object, dAtoi(argv[2]));
#endif
}

// ----------------------------------------------------------------------------
// 回收宠物
ConsoleMethod(Player, disbandPet, void, 3, 3, "disbandPet(slot);")
{
#ifdef NTJ_CLIENT
	PetOperationManager::DisbandPet(object, dAtoi(argv[2]));
#endif
}

// ----------------------------------------------------------------------------
// 解散宠物
ConsoleMethod(Player, releasePet, void, 3, 3, "releasePet(slot);")
{
#ifdef NTJ_CLIENT
	PetOperationManager::ReleasePet(object, dAtoi(argv[2]));
#endif
}

// ----------------------------------------------------------------------------
// 获取玩家日期数据旗标方法
ConsoleMethod(Player, getFlagsDate, const char*, 3, 3, "getFlagsDate(%index);")
{
	return object->getFlagsDate(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 设置玩家日期数据旗标方法
ConsoleMethod(Player, setFlagsDate, bool, 9, 9, "setFlagsDate(%index, %year, %month, %day, %hour, %minute, %second);")
{
	return object->setFlagsDate(dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]),dAtoi(argv[5]),dAtoi(argv[6]),dAtoi(argv[7]),dAtoi(argv[8]));
}

// ----------------------------------------------------------------------------
// 获取某索引位的旗标值
ConsoleMethod(Player, getFlagsByte, S32, 3, 3, "getFlagsByte(%index);")
{
	return object->getFlagsByte(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 设置某索引位的旗标值
ConsoleMethod(Player, setFlagsByte, bool, 4, 4, "setFlagsByte(%index, %byte);")
{
	return object->setFlagsByte(dAtoi(argv[2]), dAtoi(argv[3]));
}

// ----------------------------------------------------------------------------
// 判定某索引位的bit的状态
ConsoleMethod(Player, getFlagsBit, bool, 4, 4, "getFlagsBit(%index, %byte);")
{
	return object->getFlagsBit(dAtoi(argv[2]), dAtoi(argv[3]));
}

// ----------------------------------------------------------------------------
// 设置某索引位的bit的状态
ConsoleMethod(Player, setFlagsBit, bool, 5, 5, "setFlagsByte(%index, %byte, %flag);")
{
	return object->setFlagsBit(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]));
}

// ----------------------------------------------------------------------------
// 比较日期旗标数据
ConsoleMethod(Player, compareFlagsDate, S32, 9, 9, "setFlagsByte(%index, %year, %month, %day, %hour, %minute, %second);")
{
	return object->compareFlagsDate(dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]),dAtoi(argv[5]),dAtoi(argv[6]),dAtoi(argv[7]),dAtoi(argv[8]));
}

// ----------------------------------------------------------------------------
// 保存当前日期时间旗标数据
ConsoleMethod(Player, saveFlagsDate, bool, 3, 3, "saveFlagsDate(%index);")
{
	return object->saveFlagsDate(dAtoi(argv[2]));
}
//------------------------------------------------------------------------------
//增加活力值
ConsoleMethod(Player,addVigor,void,3,3,"%player.addVigor(%vigor)")
{
	object->addVigor(atoi(argv[2]));
}

//得到物品的限制等级
ConsoleMethod(Player, getItemLimitLevel, S32, 3, 3, "getItemLimitLevel(%index)")
{
	if(!object)
		return -1;
	ShortcutObject* pObj = object->inventoryList.GetSlot(dAtoi(argv[2]));
	if(!pObj)
		return -1;
	ItemShortcut* pItem = (ItemShortcut*)pObj;
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return -1;
	return pRes->getLimitLevel();
}


#include "NetWork\ServerPacketProcess.h"

ConsoleMethod( Player, AddPoint, void, 3, 3, "obj.addPoint( %num )" )
{
	int accountId = object->getPlayerID();

	int num = atoi( argv[2] );

	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_AddPoint, accountId, SERVICE_WORLDSERVER, num, g_ServerGameplayState->getZoneId() );

	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}

#ifdef DEBUG
// 添加天赋
ConsoleMethod(Player, addTalent, void, 3, 3, "%player.addTalent(%id)")
{
	object->mTalentTable.addTalent(dAtoi(argv[2]));
}
// 移除天赋
ConsoleMethod(Player, removeTalent, void, 3, 3, "%player.removeTalent(%id)")
{
	object->mTalentTable.removeTalent(dAtoi(argv[2]));
}
// 清空天赋
ConsoleMethod(Player, clearTalent, void, 2, 2, "%player.clearTalent(%id)")
{
	object->mTalentTable.clear();
}

//是否有定位值
ConsoleMethod(Player, isHavePoint, S32, 2, 2, "%player.isHavePoint(%index)")
{
	if(!object)
		return 0;
	ShortcutObject* pObj = object->inventoryList.GetSlot(dAtoi(argv[2]));
	if(!pObj)
		return 0;
	ItemShortcut* pItem = (ItemShortcut*)pObj;
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return 0;
	if(!pRes->getExtData())
		return 0;
	if(pRes->getIDEProNum() == 0 || pRes->getIDEProNum() > 4)
		return 0;
	return 1;
}
//设置定位点
ConsoleMethod(Player, setPositionPoint, void, 2, 2, "%player.setPositionPoint(%index)")
{
	if(!object)
		return ;
	ShortcutObject* pObj = object->inventoryList.GetSlot(dAtoi(argv[2]));
	if(!pObj)
		return ;
	ItemShortcut* pItem = (ItemShortcut*)pObj;
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return ;

	Point3F pos = object->getPosition();
	U32 uZoneId = object->getZoneID();
	U32 xPos = pos.x * 100;
	U32 yPos = pos.y * 100;
	U32	zPos = pos.z * 100;

	pRes->setIDEProValue(0, uZoneId);
	pRes->setIDEProValue(1, xPos);
	pRes->setIDEProValue(2, yPos);
	pRes->setIDEProValue(3, zPos);
}

ConsoleMethod(Player, getPositionPoint, S32, 3, 3, "%player.getPositionPoint(%index, %index)")
{
	if(!object)
		return -1;
	ShortcutObject* pObj = object->inventoryList.GetSlot(dAtoi(argv[2]));
	if(!pObj)
		return -1;
	ItemShortcut* pItem = (ItemShortcut*)pObj;
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return -1;
	if(!pRes->getExtData())
		return -1;
	return pRes->getIDEProValue(dAtoi(argv[3]));
}
#endif

#endif//NTJ_SERVER
