//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "util/LocalString.h"


#ifdef NTJ_CLIENT
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ClientPlayerManager.h"
#include "Gameplay/Social/ClientSocial.h"
#endif

#ifdef NTJ_SERVER
#include "Gameplay/Social/ZoneSocial.h"
#include "Gameplay/Team/ZoneTeamManager.h"
#include "GamePlay/ServerGamePlayState.h"
#include "GamePlay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/GameObjects/NpcObject.h"
#endif

#include "Gameplay/GameObjects/MPManager.h"

stMPTable MPManager::MPManagerData[MaxMasterLevel+1] =
{
	{"",        0,   0,         0,     0,   0},
	{"小师傅",  1,   100,       50,    5,   0},
	{"师傅",    1,   500,       70,    10,  0},
	{"名师",    2,   5000,      100,   15,  0},
	{"宗师",    3,   25000,     140,   20,  0},
	{"仙师",    4,   100000,    190,   25,  0},
	{"神师",    5,   1000000,   250,   30,  0}
};

MPManager::MPManager() : mMasterLevel(0),
                         mCurrPrenticeNum(0),
                         mCurrMasterExp(0),
						 mExpPool(0),
						 mbPrentice(false),
						 mbFarPrentice(false),
						 mStudyLevel(0)

{
	mCallboardString = StringTable->insert("");
}

StringTableEntry MPManager::getMastermTitle()
{
	return MPManagerData[mMasterLevel].mTitleName;
}

void MPManager::setPrenticeNum(Player* pPlayer,S8 num)
{
	mCurrPrenticeNum = mClamp(num,0,getData(mMasterLevel).mMaxPrenticeNum);
#ifdef NTJ_SERVER
	sendEvent(pPlayer,PRENTICENUM);
#endif
}

void MPManager::setStudyLevel(Player* pPlayer,U16 level)
{
	mStudyLevel = level;

#ifdef NTJ_SERVER
	sendEvent(pPlayer,STUDYLEVEL);
#endif
}

void MPManager::setCallboardString(const char* string)
{
	mCallboardString = StringTable->insert(string);
}

void MPManager::setPrenticeState(Player* pPlayer,bool flage)
{
	mbPrentice = flage;

#ifdef NTJ_SERVER
	sendEvent(pPlayer,PRENTICEFLAGE);
#endif
}

void MPManager::setFarPrentice(Player* pPlayer,bool flage)
{
	mbFarPrentice = flage;

#ifdef NTJ_SERVER
	sendEvent(pPlayer,LEAVEMASTER);
#endif
}

void MPManager::addMasterLevel(Player* pPlayer,S32 level)
{
	S32 tempLevel = mMasterLevel;
	mMasterLevel = mClamp(mMasterLevel + level,0,MaxMasterLevel);

#ifdef NTJ_SERVER
	sendEvent(pPlayer,MASTERLEVEL);
#endif

#ifdef NTJ_CLIENT
	tempLevel = mMasterLevel - tempLevel;
	if(tempLevel > 0)
		MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_MPMANAGER_LEVEL),getData(mMasterLevel).mTitleName),SHOWPOS_CHAT);
#endif
}

void MPManager::addMasterExploit(Player* pPlayer,S32 exp)
{
	S32 tempMasterExp = mCurrMasterExp;
	mCurrMasterExp = mClamp(mCurrMasterExp + exp,0,0x7FFFFFFF);

#ifdef NTJ_SERVER
	sendEvent(pPlayer,MASTEREXPLOIT);
#endif

#ifdef NTJ_CLIENT
	tempMasterExp = mCurrMasterExp - tempMasterExp;
	if(tempMasterExp > 0)
		MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_MPMANAGER_ADD_MASTEREXP),tempMasterExp),SHOWPOS_CHAT);
	else if(tempMasterExp < 0)
		MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_MPMANAGER_DEC_MASTEREXP),-tempMasterExp),SHOWPOS_CHAT);
#endif
}

void MPManager::addExpPool(Player* pPlayer,S32 exp,S32 friendValue)
{
	//好友度影响
	if(friendValue >0)
	{
		F32 friendValueScale = mClampF(friendValue/1000.0f,0.5,1.0);
		exp *= friendValueScale;
	}

	S32 tempExp = mExpPool;
	mExpPool = mClamp(mExpPool + exp,0,MAXEXP_POOL);

#ifdef NTJ_SERVER
	sendEvent(pPlayer,EXPPOOL);
#endif

#ifdef NTJ_CLIENT
	tempExp = mExpPool - tempExp;
	if(tempExp>0)
		MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_MPMANAGER_ADD_EXPPO0L),tempExp),SHOWPOS_CHAT);
	else if(tempExp<0)
		MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_MPMANAGER_DEC_EXPPO0L),-tempExp),SHOWPOS_CHAT);
#endif
}

void MPManager::tradeExp(Player* pPlayer,S32 expPool)
{
	if(NULL == pPlayer || !mMasterLevel)
		return;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

	S32 needExploit = mCeil(expPool / getData(mMasterLevel).mTradeExp);

	if(expPool > mExpPool)
	{
#ifdef NTJ_CLIENT
		MessageEvent::show(SHOWTYPE_WARN,MSG_MPMANAGER_MASTEREXP_ERROR,SHOWPOS_SCRANDCHAT);
#endif
#ifdef NTJ_SERVER
		MessageEvent::send(conn,SHOWTYPE_WARN,MSG_MPMANAGER_MASTEREXP_ERROR,SHOWPOS_SCRANDCHAT);
#endif
		return;
	}

	if(needExploit > mCurrMasterExp)
	{
#ifdef NTJ_CLIENT
		MessageEvent::show(SHOWTYPE_WARN,MSG_MPMANAGER_EXPPOOL_ERROR,SHOWPOS_SCRANDCHAT);
#endif
#ifdef NTJ_SERVER
		MessageEvent::send(conn,SHOWTYPE_WARN,MSG_MPMANAGER_EXPPOOL_ERROR,SHOWPOS_SCRANDCHAT);
#endif
		return;
	}

	addExpPool(pPlayer,-expPool);
	addMasterExploit(pPlayer,-needExploit);
	pPlayer->addExp(expPool);
}

enWarnMessage MPManager::canMaster(Player* pPlayer)
{
	if(!pPlayer)
		return MSG_UNKOWNERROR;

	if(pPlayer->getLevel() < 50)
		return MSG_PLAYER_LEVEL;

	if (mbPrentice)
		return MSG_MPMANAGER_PRENTICE_STATE;

	if(mCurrMasterExp < 100)
		return MSG_MPMANAGER_EXPPOOL_ERROR;

	return MSG_NONE;
}

#ifdef NTJ_SERVER
////////////////////////////////////////////////////////////////////////////////////////////
//                            收徒
////////////////////////////////////////////////////////////////////////////////////////////

enWarnMessage MPManager::recruitPrentice(Player* pPlayer,Player* destPlayer)
{
	if(NULL == pPlayer || NULL == destPlayer )
		return MSG_UNKOWNERROR;

	if(!mMasterLevel)
		return MSG_MPMANAGER_LEVEL_ZERO;

	if(getData(mMasterLevel).mMaxPrenticeNum <= mCurrPrenticeNum)
		return MSG_MPMANAGER_PRENTICE_FULL;

	if(pPlayer->getLevel() - destPlayer->getLevel() < 10)
		return MSG_PLAYER_LEVEL;

	stSocialItem* pSocial = pPlayer->getPlayerSocialItem(destPlayer->getPlayerID());
	if(!pSocial || (pSocial->type != SocialType::Friend && pSocial->type != SocialType::Prentice))
		return MSG_MPMANAGER_FRIEND_ERROR;

	return MSG_NONE;
}

////////////////////////////////////////////////////////////////////////////////////////////
//                            拜师
////////////////////////////////////////////////////////////////////////////////////////////
enWarnMessage MPManager::followMaster(Player* pPlayer,Player* destPlayer)
{
	MPManager* pInfo = NULL;
	if(NULL == pPlayer || NULL == destPlayer || !(pInfo = destPlayer->getMPInfo()))
		return MSG_UNKOWNERROR;

	if(mbFarPrentice)
		return MSG_MPMANAGER_LEAVEMASTER;

	if(pInfo->getMasterLevel() == 0)
		return MSG_MPMANAGER_DEST_LEVELZERO;

	if(getData(pInfo->getMasterLevel()).mMaxPrenticeNum <= pInfo->getCurrPrenticeNum())
		return MSG_MPMANAGER_PRENTICE_FULL;

	if( destPlayer->getLevel() - pPlayer->getLevel()< 10)
		return MSG_PLAYER_LEVEL;

	stSocialItem* pSocial = pPlayer->getPlayerSocialItem(destPlayer->getPlayerID());
	if(!pSocial || (pSocial->type != SocialType::Friend && pSocial->type != SocialType::Master))
		return MSG_MPMANAGER_FRIEND_ERROR;

	return MSG_NONE;
}

void MPManager::createRelation(Player* pPlayer,SocialType::Type type)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return;

	S32 playerId = pPlayer->getPlayerID();
	CZoneTeam* pZoneTeam = g_ZoneTeamManager.GetTeamByPlayerId(playerId);
	if(!pZoneTeam)
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_MPMANAGER_NOTEAM,SHOWPOS_SCREEN);
		return;
	}
	
	Player* destPlayer = NULL;
	U8 teammateCount = pZoneTeam->GetTeammateCount();
	//当前队伍必需是2个人
	if( teammateCount != 2)
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_MPMANAGER_ERRORTEAM,SHOWPOS_SCREEN);
		return;
	}

	for(int i=0; i<teammateCount; i++)
	{
		S32 Id = pZoneTeam->GetPlayerId(i);
		if(Id == playerId)
			continue;

		destPlayer = g_ServerGameplayState->GetPlayer(Id);
		if(NULL == destPlayer)
			return MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_MPMANAGER_DESTPLAEYR_ERROR,SHOWPOS_SCREEN);
		else
			break;
	}

	SceneObject* pObject = pPlayer->getInteraction();
	if(!destPlayer->canInteraction(pObject))
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_PLAYER_TOOFAR,SHOWPOS_SCREEN);
		MessageEvent::send(destPlayer->getControllingClient(),SHOWTYPE_NOTIFY,MSG_PLAYER_TOOFAR,SHOWPOS_SCREEN);
		return;
	}

	enWarnMessage msg = MSG_NONE;
	if(type == SocialType::Prentice)
	{
		msg = recruitPrentice(pPlayer,destPlayer);
		if(msg == MSG_NONE)
		{
			sendClientRequest(pPlayer,destPlayer,SocialType::Prentice);
			return;
		}
	}

	if(type == SocialType::Master)
	{
		msg = followMaster(pPlayer,destPlayer);
		if(msg == MSG_NONE)
		{
			sendClientRequest(pPlayer,destPlayer,SocialType::Master);
			return;
		}
	}

	if(msg != MSG_NONE)
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,msg,SHOWPOS_SCREEN);
}

void MPManager::destroyRelation(Player* pPlayer,S32 destPlayerId,SocialType::Type type)
{
	if(!pPlayer || type != SocialType::Master || type != SocialType::Prentice)
		return;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

	///未知目标
	if(destPlayerId<=0)
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_COMBAT_UNKNOWN_TARGET,SHOWPOS_SCREEN);
		return;
	}

	//修改师徒关系为好友关系(对方不在线的情况放在ZoneSocial处理)
	if(pPlayer->getMoney()>=4000) 
	{
		if (pPlayer->reduceMoney(4000))
		{
			//自已是师傅,否则对方是师傅
			if(type == SocialType::Master) 
			{
				g_zoneSocial.chanageLink(pPlayer->getPlayerID(),destPlayerId,SocialType::Master,SocialType::Friend);
				g_zoneSocial.chanageLink(destPlayerId,pPlayer->getPlayerID(),SocialType::Prentice,SocialType::Friend);
			}
			else
			{
				g_zoneSocial.chanageLink(pPlayer->getPlayerID(),destPlayerId,SocialType::Prentice,SocialType::Friend);
				g_zoneSocial.chanageLink(destPlayerId,pPlayer->getPlayerID(),SocialType::Master,SocialType::Friend);
			}
		}
	}
	else
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_PLAYER_MONEYNOTENOUGH,SHOWPOS_SCREEN);
	}
}

bool MPManager::sendClientRequest(Player* pPlayer,Player* destPlayer,SocialType::Type type)
{
	if(NULL == pPlayer || NULL == destPlayer)
		return false;

	GameConnection* conn = destPlayer->getControllingClient();
	if(!conn)
		return false;

	ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_MPMANAGER);
	pEvent->SetInt32ArgValues(1,pPlayer->getPlayerID());
	pEvent->SetIntArgValues(1,type);

	return conn->postNetEvent(pEvent);
}

void MPManager::initialize(stPlayerStruct *pPlayerData)
{
	if(NULL == pPlayerData)
	{
		AssertFatal(false,"MPManager::initializeData error!");
		return;
	}

	mMasterLevel = pPlayerData->MainData.MasterLevel;
	bool bPrentice = false;
	for (int i=0; i<pPlayerData->MainData.SocialItemCount; i++)
	{
		if(mMasterLevel >0 && pPlayerData->MainData.SocialItem[i].type == SocialType::Prentice)
			mCurrPrenticeNum++;

		if(pPlayerData->MainData.SocialItem[i].type == SocialType::Master)
		{
			bPrentice = true;
			break;
		}
	}

	mCurrMasterExp   = pPlayerData->MainData.CurrMasterExp;
	mExpPool         = pPlayerData->MainData.ExpPool;
	mbPrentice       = bPrentice;
	mbFarPrentice    = pPlayerData->MainData.bFarPrentice;
	mStudyLevel      = pPlayerData->MainData.MPMStudyLevel;
	if(pPlayerData->MainData.MasterText[0] != '\0')
		mCallboardString = StringTable->insert(pPlayerData->MainData.MasterText);
}

bool MPManager::saveData(stPlayerStruct *pPlayerData)
{
	if(NULL == pPlayerData)
	{
		AssertFatal(false,"MPManager::saveData error!");
		return false;
	}
	pPlayerData->MainData.MasterLevel     = mMasterLevel;
	pPlayerData->MainData.CurrMasterExp   = mCurrMasterExp;
	pPlayerData->MainData.ExpPool         = mExpPool;
	pPlayerData->MainData.bPrentice       = mbPrentice;
	pPlayerData->MainData.bFarPrentice    = mbFarPrentice;
	pPlayerData->MainData.MPMStudyLevel   = mStudyLevel;
	if(mMasterLevel>0 && dStrcmp(mCallboardString,"") != 0)
		dStrcpy(pPlayerData->MainData.MasterText,MPM_CALLBOARD_MAXSTRING,mCallboardString);

	return true;
}

bool MPManager::sendInitialData(Player* pPlayer)
{
	if(!pPlayer)
		return false;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return false;

	MPManagerEvent* pEvent = new MPManagerEvent();
	return conn->postNetEvent(pEvent);
}

bool MPManager::sendEvent(Player* pPlayer,U16 mask, bool isFirstUpdate /* = false */)
{
	if(!pPlayer)
		return false;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return false;

	MPManagerEvent* pEvent = new MPManagerEvent(mask,isFirstUpdate);
	return conn->postNetEvent(pEvent);
}

void MPManager::calcMasterExp(Player* pPlayer,NpcObject* npc,F32 expDist)
{
	if(!pPlayer || !npc)
		return;

	if(!pPlayer->getTeamInfo().m_bIsCaption)  // 队长
		return;

	S32 level = pPlayer->getLevel();
	if(level < 20)
		return;

	int myselfId = pPlayer->getPlayerID();
	CZoneTeam* pZoneTeam = g_ZoneTeamManager.GetTeamByPlayerId(myselfId);
	if(!pZoneTeam)
		return;

	Player* pTeammate = NULL;
	bool    bAddExp = false;
	bool    bPrentice = false;
	//获取徒弟列表
	Vector<S32>  prenticeList; 
	getPrenticeList(pPlayer,prenticeList);

	for (S32 i=0; i<pZoneTeam->GetTeammateCount();i++)
	{
		// 查找playerId是否合法或是自己
		S32 playerId = pZoneTeam->GetPlayerId(i);
		if(playerId == 0 || playerId == myselfId ) 
			continue;

		// 得到队友等级
		pTeammate = g_ServerGameplayState->GetPlayer(playerId);
		S32 TeammateLevel = pTeammate->getLevel();   

		// 受益范围队友给自己加师德条件
		if(TeammateLevel >=10 && level - TeammateLevel >=8 && pTeammate->getDistance(npc)<= expDist)
		{
			bAddExp = true;
			// 受益范围内是否存在徒弟
			if(prenticeList.size()>0 && bPrentice == false)
			{
				for(S32 j =0; j<prenticeList.size(); j++)
				{
					if(prenticeList[j] == pTeammate->getPlayerID())
					{
						bPrentice = true;
						break;
					}
				}

				if(bPrentice)
					break;
			}
		}
	}

	if(!bAddExp)
		return;

	//队存在徒弟加3点师德,不存在加1点师德(暂定)
	if(bPrentice)  
		addMasterExploit(pPlayer,3);
	else
		addMasterExploit(pPlayer,1);

}

void MPManager::calcAttachPrize(Player* pPlayer)
{
	if(!pPlayer)
		return;

	//获得自己的师傅（如师傅不在线应放到数据库，现暂不处理）
	Player* pMasterPlayer = getMasterPlayer(pPlayer); 
	if(!pMasterPlayer)
		return;

	//自己社会关系
	stSocialItem* pSocialItem = pPlayer->getPlayerSocialItem(pMasterPlayer->getPlayerID());
	if(!pSocialItem)
		return;

	//好友度
	S32 friendValue = pSocialItem->type;

	//升级给师傅加额外奖历
	switch (pPlayer->getLevel())
	{
	case 30:
		{
			//if(pPlayer->getFactionLevel())
			//	addExpPool(pMasterPlayer,500000,true);
			//else
				addExpPool(pMasterPlayer,250000,friendValue);
		}
		break;
	case 50:
		{
			//if(pPlayer->getFactionLevel())
			//	addExpPool(pMasterPlayer,1000000,true);
			//else
				addExpPool(pMasterPlayer,500000,friendValue);
		}
		break;
	case 60:
		{
			addMasterExploit(pMasterPlayer,5400);
			addExpPool(pMasterPlayer,1800000,friendValue);
		}
		break;
	case 70:
		{
			addMasterExploit(pMasterPlayer,9600);
			addExpPool(pMasterPlayer,3200000,friendValue);
		}
		break;
	case 80:
		{
			addMasterExploit(pMasterPlayer,20000);
			addExpPool(pMasterPlayer,6800000,friendValue);
		}
		break;
	case 90:
		{
			addMasterExploit(pMasterPlayer,60000);
			addExpPool(pMasterPlayer,60000000,friendValue);
		}
		break;
	default:
		return;
	}
}

bool MPManager::autoLeaveMaster(Player *pPlayer)
{
	if(mbPrentice && pPlayer)
	{
		S32 masterID = getMasterPlayerId(pPlayer);
		Player* pMasterPlayer = g_ServerGameplayState->GetPlayer(masterID);
		if(pMasterPlayer)
		{
			g_zoneSocial.chanageLink(pPlayer->getPlayerID(),masterID,SocialType::Master,SocialType::Friend);
			g_zoneSocial.chanageLink(masterID,pPlayer->getPlayerID(),SocialType::Prentice,SocialType::Friend);
		}
		else
		{
			g_zoneSocial.chanageLink(pPlayer->getPlayerID(),masterID,SocialType::Master,SocialType::Friend);
			//给师傅发送系统消息,徒弟pPlayer已经出师
		}

		// 发送playerID记录到师傅的已出师表中,师傅当前徒弟－１
		// 启动出师任务
		return true;
	}

	return false;
}

Player* MPManager::getMasterPlayer(Player* pPlayer)
{
	if(!pPlayer)
		return NULL;

	Player* pMasterPlayer = NULL;
	stSocialItem* pSocial = pPlayer->getSocialItem();
	S32 count = pPlayer->getSocialCount();

	for (int i=0; i<count; i++)
	{
		if(pSocial[i].type == SocialType::Master)
			pMasterPlayer = g_ServerGameplayState->GetPlayer(pSocial[i].playerId);
	}

	return pMasterPlayer;
}

U32 MPManager::getMasterPlayerId(Player* pPlayer)
{
	if(!pPlayer)
		return 0;

	stSocialItem* pSocial = pPlayer->getSocialItem();
	S32 count = pPlayer->getSocialCount();

	for (int i=0; i<count; i++)
	{
		if(pSocial[i].type == SocialType::Master)
			return pSocial[i].playerId;
	}

	return 0;
}

void MPManager::getPrenticeList(Player* pPlayer,Vector<int>& listId)
{
	if(!pPlayer)
		return;

	stSocialItem* pSocial = pPlayer->getSocialItem();
	S32 count = pPlayer->getSocialCount();

	for (int i=0; i<count; i++)
	{
		if(pSocial[i].type == SocialType::Prentice)
			listId.push_back(pSocial[i].playerId);
	}
}

//增加资格(师傅等级)
ConsoleFunction(addMasterLevel,void,3,3,"addMasterLevel(%playerId,%level)")
{
	S8 iLevel = dAtoi(argv[2]);
	GameConnection* conn = NULL;
	Player* pPlayer = g_ServerGameplayState->GetPlayer(dAtoi(argv[1]));
	if(NULL == pPlayer || NULL == pPlayer->getMPInfo() || !(conn = pPlayer->getControllingClient()))
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_UNKOWNERROR);
		return;
	}

	enWarnMessage msg = pPlayer->getMPInfo()->canMaster(pPlayer);

	if(msg != MSG_NONE)
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,msg);
		return;
	}

	U8 curLevel = pPlayer->getMPInfo()->getMasterLevel();
	S32 curExp = pPlayer->getMPInfo()->getCurrMasterExp();

	S32 needExp = MPManager::getData(curLevel+iLevel).mNeedExploit;
	if(needExp > curExp)
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_MPMANAGER_EXPPOOL_ERROR);
		return;
	}

	pPlayer->getMPInfo()->addMasterExploit(pPlayer,-needExp);
	pPlayer->getMPInfo()->addMasterLevel(pPlayer,iLevel);
}

//增加师德
ConsoleFunction(addMasterExp,void,3,3,"addMasterExp(%playerId,%exp)")
{
	S8 iExp = dAtoi(argv[2]);
	GameConnection* conn = NULL;
	Player* pPlayer = g_ServerGameplayState->GetPlayer(dAtoi(argv[1]));
	if(NULL == pPlayer || NULL == pPlayer->getMPInfo() || !(conn = pPlayer->getControllingClient()))
	{
		MessageEvent::send(conn,SHOWTYPE_NOTIFY,MSG_UNKOWNERROR);
		return;
	}

	pPlayer->getMPInfo()->addMasterExploit(pPlayer,iExp);
}
#endif

#ifdef NTJ_CLIENT
bool MPManager::sendZoneResonse(Player* pPlayer,Player* srcPlayer,SocialType::Type type)
{
	if(!pPlayer || !srcPlayer)
		return false;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return false;

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_MPMANAGER);
	pEvent->SetInt32ArgValues(1,srcPlayer->getPlayerID());
	pEvent->SetIntArgValues(1,type);

	return conn->postNetEvent(pEvent);
}

bool MPManager::sendZoneChangeText(Player* pPlayer,const char* str)
{
	if(NULL ==  pPlayer)
		return false;

	GameConnection* conn =  pPlayer->getControllingClient();
	if(NULL == conn)
		return false;

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_MPMCHANGETEXT);
	pEvent->SetStringArgValues(1,mCallboardString);

	return conn->postNetEvent(pEvent);
}

//同意收徒
ConsoleFunction(agreePrentice,void,2,2,"agreePrentice(%playerId)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	Player* srcPlayer = PLAYER_MGR->GetLocalPlayer(dAtoi(argv[1]));

	if(!pPlayer ||!pPlayer->getMPInfo() || !srcPlayer)
		return;

	pPlayer->getMPInfo()->sendZoneResonse(pPlayer,srcPlayer,SocialType::Prentice);
}
//同意拜师
ConsoleFunction(agreeMaster,void,2,2,"agreeMaster.(%playerId)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	Player* srcPlayer = PLAYER_MGR->GetLocalPlayer(dAtoi(argv[1]));

	if(!pPlayer || !pPlayer->getMPInfo() || !srcPlayer)
		return;

	pPlayer->getMPInfo()->sendZoneResonse(pPlayer,srcPlayer,SocialType::Master);
}

//拒绝对方
ConsoleFunction(rejectRelation,void,2,2,"rejectRelation(%playerId)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	Player* srcPlayer = PLAYER_MGR->GetLocalPlayer(dAtoi(argv[1]));

	if(!pPlayer ||!pPlayer->getMPInfo() || !srcPlayer)
		return;

	pPlayer->getMPInfo()->sendZoneResonse(pPlayer,srcPlayer,SocialType::Count);
}
//获取自己师徒信息
ConsoleFunction(getMyselfMPInfo,void,1,1,"getMyselfMPInfo()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer || NULL == pPlayer->getMPInfo())
		return;

	U8   prenNum = 0;
	U8   maxPrenNum = 0;
	U32  masterExp = 0;
	U32  maxExpPool = 0;
	U32  expPool = 0;
	U8   level = 0;
	char titleName[COMMON_STRING_LENGTH] = {0,};
	CClientSocial::SocialMap&  pSocialMap = g_clientSocial.getClientSocialInfo();
	CClientSocial::SocialMap::iterator it = pSocialMap.begin();
    level = pPlayer->getMPInfo()->getMasterLevel();

	if(level)
	{
		dSprintf(titleName,sizeof(titleName),"%s",pPlayer->getMPInfo()->getMastermTitle());
		for(; it != pSocialMap.end(); it++)
		{
			stSocialInfo stInfo = it->second;
			if(stInfo.type == SocialType::Prentice)
				prenNum++;
		}

		maxPrenNum = MPManager::getData(level).mMaxPrenticeNum;
		masterExp = pPlayer->getMPInfo()->getCurrMasterExp();

		expPool = pPlayer->getMPInfo()->getExpPool();
		maxExpPool = MAXEXP_POOL;

		Con::executef("UpdataMpInfo",titleName,
			Con::getIntArg(prenNum),
			Con::getIntArg(maxPrenNum),
			Con::getIntArg(masterExp),
			Con::getIntArg(expPool),
			Con::getIntArg(maxExpPool));
	}
}

ConsoleFunction(getPrenticeListInfo,void,1,1,"getPrenticeListInfo()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer || NULL == pPlayer->getMPInfo())
		return;

	S32  masterPlayerId  = 0;
	CClientSocial::SocialMap&  pSocialMap = g_clientSocial.getClientSocialInfo();
	CClientSocial::SocialMap::iterator it = pSocialMap.begin();

	for (; it != pSocialMap.end(); it++)
	{
		stSocialInfo stInfo = it->second;
		if(stInfo.type == SocialType::Master )
		{
			masterPlayerId = stInfo.id;
			break;
		}

		if(stInfo.type == SocialType::Prentice)
		{
			Con::executef("uiAddPrenticeList",
				Con::getIntArg(stInfo.id),
				stInfo.name,
				Con::getIntArg(stInfo.level),
				Con::getIntArg(stInfo.family),
				Con::getIntArg(stInfo.status));
		}
	}

	if (masterPlayerId > 0 ) //找到师傅 
	{
		if(g_clientSocial.getQueryPrenticeNum()>0)
			g_clientSocial.clearQueryPrenticeNum();
		//向worldServer请求数据后返回处理
		g_clientSocial.sendPlayerInfoRequest(masterPlayerId);
		return;
	}
	else                   //自己是师傅
	{
		U8 maxPrenNum = MPManager::getData(pPlayer->getMPInfo()->getMasterLevel()).mMaxPrenticeNum;
		Con::executef("uiUpdateMasterInfo",
			pPlayer->getPlayerName(),
			Con::getIntArg(pPlayer->getMPInfo()->getCurrPrenticeNum()),
			Con::getIntArg(maxPrenNum),
			pPlayer->getMPInfo()->getCallboardString());
	}
}

ConsoleFunction(setMPMCallboardText,void,2,2,"setMPMCallboardText(%str)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer || NULL == pPlayer->getMPInfo() || pPlayer->getMPInfo()->getMasterLevel() == 0)
		return;

	char temText[121] = {0,}; //60个汉字
    dStrcpy	(temText,sizeof(temText),argv[1]);
	pPlayer->getMPInfo()->setCallboardString(temText);
	pPlayer->getMPInfo()->sendZoneChangeText(pPlayer,temText);
}

ConsoleFunction(getMPMCallboardText,const char*,1,1,"getMPMCallboardText()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer || NULL == pPlayer->getMPInfo())
		return "";

	return pPlayer->getMPInfo()->getCallboardString();
}

ConsoleFunction(getSelectMPMInfoById,void,2,2,"getSelectMPMInfoById(%playerId)")
{
	S32 playerId =  dAtoi(argv[1]);
	Player* myself = g_ClientGameplayState->GetControlPlayer();
	if(NULL == myself || playerId<=0)
		return;

	//查询另外玩家
	if(playerId != myself->getPlayerID() || myself->getMPInfo()->getMasterLevel()>0)
	{
		//向worldServer请求数据后返回处理
		g_clientSocial.sendPlayerInfoRequest(playerId);
		return;
	}

	//查询自己,并且是徒弟
	CClientSocial::SocialMap&  pSocialMap = g_clientSocial.getClientSocialInfo();
	CClientSocial::SocialMap::iterator it = pSocialMap.begin();
	for(; it != pSocialMap.end(); it++)
	{
		stSocialInfo stInfo = it->second;
		if((stInfo.type == SocialType::Master))
		{
			Con::executef("onlinePrenticeInfo",
				myself->getPlayerName(),
				Con::getIntArg(myself->getMPInfo()->getStudyLevel()),
				Con::getIntArg(stInfo.friendValue),
				"0/0");

			return;
		}
	}
}
#endif