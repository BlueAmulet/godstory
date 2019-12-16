//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once
#include <hash_set>
#include "ts/TSShapeRepository.h"
#include "NetWork/CommonClient.h"
#include "Gameplay/Data/SoundResource.h"
#include "Effects/ScreenFX.h"
#include "T3D/trigger.h"
#include "Gameplay/Item/ItemBaseData.h"
#include "effects/EffectPacket.h"
#include "Gameplay/Item/ItemPackage.h"
#include "Gameplay/Team/ClientTeam.h"
#include "sfx/sfxSource.h"
#include "Gameplay/data/RouteRepository.h"
#include "Common/version.h"
#include "GamePlay/ClientGameplayState.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/Item/Prescription.h"
#include "gameplay/Data/RouteRepository.h"
#include "Gameplay/Data/PetStudyData.h"
#include "Gameplay/GameObjects/MountObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Social/ClientSocial.h"
#include "UI/dGuiObjectView.h"
#include "UI/guiControlExt.h"
#include "Gameplay/Data/CallboardData.h"
#include "Gameplay/ai/AIData.h"
#include "Gameplay/Data/SpiritData.h"
#include "Gameplay/Data/SuperMarketData.h"
#include "Gameplay/Item/Res.h"

#include "util/processMgr.h"
#include "util/aniThread.h"
#include "Ts/TSShapeRepository.h"
#include "materials/material.h"
#include "Gameplay/GameObjects/Talent.h"

#ifdef NTJ_CLIENT
#include "ui/dGuiShortCut.h"
#include "gfx/reflectPlane.h"
#include "ClientPlayerManager.h"
#include "data/viewmapdata.h"
#endif

// ========================================================================================================================================
//	Global Variables
// ========================================================================================================================================

ClientGameplayState* g_ClientGameplayState = NULL;

#ifndef GM_CLIENT
ClientGameplayState gClientGameplayState;
#endif

// ========================================================================================================================================
//	Constructor & Destructor
// ========================================================================================================================================

bool ClientGameplayState::m_bMouseOperation = true;
bool ClientGameplayState::m_bAutoResetCamera = false;
bool ClientGameplayState::m_bCameraDragged = false;
bool ClientGameplayState::m_bAutoRun = false;
bool ClientGameplayState::m_bMouseRun = false;
bool ClientGameplayState::m_bIsPlayerQuit = false;

__time32_t ClientGameplayState::mServerInitTime = 0;
SimTime    ClientGameplayState::mClinetUpdateTime =0;

ClientGameplayState::ClientGameplayState()
{
	g_ClientGameplayState	= this;
	m_pGateLink				= new TCPObject2;

    UserPacketProcess* pPktProcess = GetPacketProcess();

    pPktProcess->SetClientGameplayState(this);

	m_LoginStatue			= SERVICE_NONE;
	m_LineSwitching			= false;
	m_DisconnectFlag		= false;
	m_bShowSearchPath       = true;
	m_bNavigation           = false;

	m_isGoingCopymap		= false;
	m_isInCopymap			= false;
	m_bDarwPath             = false;

	m_CopymapLayer			= 1;
	m_CopyMapZoneIp			= 0;
	m_CopyMapZonePort		= 0;

	mSelectShortcut.type	= -1;
	mSelectShortcut.col		= -1;

	mFindPathID				= 0;
	mbStrideServer			= false;

	isTick					= false;
	pVocalStatus = new VocalStatus;

	m_RefreshPlayerModelView = true;

	m_isPwdChecked			= false;
    m_BanNamelist.clear();

}


ClientGameplayState::~ClientGameplayState()
{
	delete m_pGateLink;
	if(pVocalStatus)
		SAFE_DELETE(pVocalStatus);
}

void ClientGameplayState::Initialize()
{
	// -------------------------------------- 初始化变量 --------------------------------------
	m_LineSwitching			= false;
	m_DisconnectFlag		= false;

	Con::addVariable("$Config::MouseOperation", TypeBool, &m_bMouseOperation);
	Con::addVariable("$Config::AutoResetCamera", TypeBool, &m_bAutoResetCamera);
	Con::addVariable("CameraDragged", TypeBool, &m_bCameraDragged);
	Con::addVariable("AutoRun", TypeBool, &m_bAutoRun);

	m_isRobotMode = false;
	Con::addVariable("RobotMode", TypeBool, &m_isRobotMode );

	// -------------------------------------- 初始化data --------------------------------------

#ifndef GM_CLIENT
	//声音数据
	g_SoundManager->Read();
    Con::executef("PlaySound","B_001");
	// 模型数据
	g_TSShapeRepository.Read();
	// 模型数据
	g_ShapesSetRepository.Read();
	// AI数据
	g_AIRepository.read();
	// Npc数据
	g_NpcRepository.Read();
	// 玩家数据
	g_PlayerRepository.Read();
	//采集数据
	gCollectionRepository.Read();
	//事件触发器数据
	gEventTriggerRepository.read();
	// 触发器数据
	g_TriggerDataManager->ReadTriggerData();
	//物品数据
	g_ItemRepository->read();
	g_RandPropertyTable->read();
	// 状态数据
	g_BuffRepository.Read();
	// 技能数据
	g_SkillRepository.Read();

	//特效数据
	g_EffectDataRepository.Read();

	//物品包数据
	g_ItemPackageRepository.read();

	//寻路数据
	g_RouteRepository.read();

	//生活技能数据
	g_LivingSkillRespository.read();
	//配方数据
	g_PrescriptionRepository.read();
	//跨服寻径
	g_StrideRepository.read();
	//宠物数据
	g_PetRepository.Read();
	//骑物数据
	g_MountRepository.Read();
	//宠物修行数据
	g_PetStudyRepository.Read();
	//元神数据
	g_SpiritRepository.read();
	g_TalentRepository.read();

	//仙篆录数据
	g_CallboardDailyRepository.read();
	g_CallboardMissionRepository.read();
	g_CallboardEventRepository.read();
	//仙篆录数据 end
	//地图data
	g_viewMapRepository.read();
	//商城data
	g_MarketItemReposity->read();
	g_PackGoodsResposity->read();
	CMultThreadWorkMgr::init();

	dGuiShortCut::Initialize();

    Con::executef("IniDefaultScene");
#endif
}

void ClientGameplayState::preShutdown()
{
	Cleanup();
	setPlayerQuit(true);
	clearResource();
	deleteGlobalSound();
	dGuiShortCut::Destory();

	CRichTextDrawer::cleanup();

	CMultThreadWorkMgr::shutdown();
}

void ClientGameplayState::Shutdown()
{
    if (g_SoundManager)
	    g_SoundManager->Clear();

	g_TSShapeRepository.Clear();
	g_ShapesSetRepository.Clear();
	g_AIRepository.clear();
	g_NpcRepository.Clear();
	g_PlayerRepository.Clear();

    if (g_TriggerDataManager)
	    g_TriggerDataManager->clearTriggerData();

    if (g_ItemRepository)
	    g_ItemRepository->clear();

	g_BuffRepository.Clear();
	g_SkillRepository.Clear();
	g_EffectDataRepository.Clear();
	g_LivingSkillRespository.clear();
	g_PrescriptionRepository.clear();
	g_PetRepository.Clear();
	gCollectionRepository.Clear();
	g_MountRepository.Clear();
	g_PetStudyRepository.Clear();
	g_SpiritRepository.clear();
	g_TalentRepository.clear();

    if (g_MarketItemReposity)
	    g_MarketItemReposity->clear();

    if (g_PackGoodsResposity)
	    g_PackGoodsResposity->clear();

	g_ScreenFXMgr.clear();	
	GuiControlExt::clear();
}

void ClientGameplayState::prepRenderImage(S32 StateKey)
{
	g_EffectPacketContainer.prepRenderImage(StateKey);
}

void ClientGameplayState::TimeLoop(S32 timeDelta)
{
	if(m_DisconnectFlag)
	{
		m_DisconnectFlag = false;

		if( isGoingCopyMap() )
		{
			Con::executef("SptConnectCopymap");
		}
		else
		{
			Con::executef("SptConnectZoneServer");
		}
	}

	CLIENT_TEAM->ProcessWork();

	g_EffectPacketContainer.advanceTime(timeDelta);
	g_ScreenFXMgr.AdvanceTime(timeDelta);

	int curTime = time(NULL);

	if( m_isRobotMode )
	{
		//stdext::hash_set<GameConnection*>::iterator it;
		//Player* pPlayer = NULL;
		//Point3F point;
		//point.z = 0;
		//for( it = GameConnection::gameConnectionSet.begin(); it != GameConnection::gameConnectionSet.end(); it++ )
		//{
		//	
		//	pPlayer = dynamic_cast<Player*>((*it)->getControlObject());
		//	if( pPlayer )
		//	{
		//		if( pPlayer->m_pAI->IsReachDest() )
		//		{
		//			point.x = float(-383 + 400 * gRandGen.randF( 0.0f, 1.0f ) );
		//			point.y = float(134 + 200 * gRandGen.randF( 0.0f, 1.0f ) );
		//			pPlayer->SetTarget( point );
		//		}
		//	}
		//}
	}
	

	if(!(curTime%5))		//5秒检查一次
	{
		if(m_LoginStatue==SERVICE_GAMESERVER && m_pGateLink && m_pGateLink->getState()==TCPObject::Disconnected)
		{
			const char *pGateAddress = 	getCurrentNetAddr();
			if(dStricmp(pGateAddress,""))
				m_pGateLink->connect(pGateAddress);
		}
	}

    g_TSShapeRepository.Update();
}

__time32_t ClientGameplayState::getSetverTime()
{
	__time32_t serverTime;
	SimTime currentTime = Platform::getVirtualMilliseconds();
	return serverTime = mServerInitTime + (currentTime - mClinetUpdateTime)/1000;

}
void ClientGameplayState::getGameFormatTime(Platform::LocalTime &lt)
{
	__time32_t serverTime = getSetverTime();

	errno_t err;
	tm systime;
	err = _localtime32_s(&systime,&serverTime);

	if(err)
		lt.year = 0;

	lt.sec      = systime.tm_sec;
	lt.min      = systime.tm_min;
	lt.hour     = systime.tm_hour;
	lt.month    = systime.tm_mon +1;
	lt.monthday = systime.tm_mday;
	lt.weekday  = systime.tm_wday;
	lt.year     = systime.tm_year +1900;
	lt.yearday  = systime.tm_yday;
	lt.isdst    = systime.tm_isdst;
}

GameBase* ClientGameplayState::GetControlObject()
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn) 
		return conn->getControlObject();
	return NULL;
}

Player* ClientGameplayState::GetControlPlayer()
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn) 
		return dynamic_cast<Player*>(conn->getControlObject());
	return NULL;
}

// ----------------------------------------------------------------------------
// 获取当前选择目标的对象
GameObject* ClientGameplayState::GetTarget()
{
	Player* player = GetControlPlayer();
	if(player)
		return player->GetTarget();
	return NULL;
}

// ----------------------------------------------------------------------------
// 获取当前交互目标的对象
SceneObject* ClientGameplayState::GetInteraction()
{
	Player* player = GetControlPlayer();
	if(player)
		return player->getInteraction();
	return NULL;
}

void ClientGameplayState::removeObjectList(U32 ID)
{
	objectListMap::iterator itr = m_ObjectList.find(ID);
	if(itr != m_ObjectList.end())
		m_ObjectList.erase(itr);
}

void ClientGameplayState::setObjectList(U32 ID, GameObject* object)
{
	if(!object)
		return;

	m_ObjectList.insert(objectListMap::value_type(ID,object));
}

// ----------------------------------------------------------------------------
// 根据角色ID，在本地视野范围对象里找到指定玩家
Player*	ClientGameplayState::findPlayer(U32 PlayerID)
{
	/*objectListMap::iterator itB = m_ObjectList.begin();
	for(; itB != m_ObjectList.end(); ++itB)
	{
		Player* player = dynamic_cast<Player*>(itB->second);
		if(player && player->getPlayerID() == PlayerID)
			return player;
	}
	return NULL;*/

	return PLAYER_MGR->GetLocalPlayer( PlayerID );
}

bool ClientGameplayState::CheckClientTerrFileCRC(U32 TerrCrc)
{
	for (int i=0; i<mClientTerrFileCrc.size(); ++i)
		if(mClientTerrFileCrc[i] == TerrCrc)
			return true;
	return false;
}

void ClientGameplayState::setClientMisFileName(StringTableEntry MisfileName)  
{
	mClientMisFileName = MisfileName;	
	Con::setVariable("$Server::MissionFile", mClientMisFileName);
}

// 读取游戏启动配置文件Client.ini
bool ClientGameplayState::LoadClientIni()
{
	GetPrivateProfileStringA("Crash",	"ReportFTP",		"",	m_CrashReportFtp,		COMMON_STRING_LENGTH, GAME_CLIENTINI_FILE);
	GetPrivateProfileStringA("Crash",	"ReportMail",		"",	m_CrashReportEmail,		COMMON_STRING_LENGTH, GAME_CLIENTINI_FILE);
	GetPrivateProfileStringA("Version",	"GameVersion",		"",	m_GameVersion,			COMMON_STRING_LENGTH, GAME_CLIENTINI_FILE);
	return true;
}

void ClientGameplayState::clearResource()
{
#ifdef USE_MULTITHREAD_ANIMATE
	g_MultThreadWorkMgr->endProcess();
	g_MultThreadWorkMgr->enable(false);
#endif

	SimObject* grp = Sim::findObject("MissionGroup");
	if(grp)
		grp->deleteObject();

    ReflectPlane_SingleZ::freeResource();
    
    //释放所有的模型数据
    g_TSShapeRepository.ClearRes();

    //清除所有材质里的纹理数据
    SimSet* pMatSet = Material::getMaterialSet();

    for(SimSet::iterator i = pMatSet->begin(); i != pMatSet->end(); ++i)
    {
        Material* pMat = (Material *)(*i);

        for (int iStage = 0; iStage < Material::MAX_STAGES; ++iStage)
        {
            for (int iFethure = 0; iFethure < GFXShaderFeatureData::NumFeatures; ++iFethure)
            {
                pMat->stages[iStage].tex[iFethure] = 0;
            }
        }
    }
}

void ClientGameplayState::ConnectServer(const char *addr,char Status)
{
	clearObjectList();
	m_LoginStatue = Status;
	m_pGateLink->disconnect();
	setCurrentNetAddr(addr);
	m_pGateLink->connect(addr);
}

void ClientGameplayState::DisconnectServer()
{
	m_pGateLink->disconnect();
	setCurrentNetAddr("");
}

void ClientGameplayState::onDisconnect()
{
	// 与zone断开连接时都会调用，包括切线、传送等
	g_EffectPacketContainer.removeFromScene();

	Con::executef( "CloseAllGui" );
#ifdef USE_MULTITHREAD_ANIMATE
	g_MultThreadWorkMgr->endProcess();
	g_MultThreadWorkMgr->enable(false);
#endif
}

UserPacketProcess* ClientGameplayState::GetPacketProcess()								
{
	return (UserPacketProcess *)m_pGateLink->GetPacket();
}

void ClientGameplayState::setLineSwitching(bool flag)
{
	m_LineSwitching = flag;
}

// ----------------------------------------------------------------------------
// 设置屏幕输出显示字符串内容
void ClientGameplayState::OutputScreenMessage(stMsgParam& param)
{
	Con::executef("OutputScrMsg",
		param.message,
		Con::getIntArg(param.posx),
		Con::getIntArg(param.posy),
		Con::getIntArg(param.mode),
		Con::getIntArg(param.limit),
		Con::getIntArg(param.time),
		Con::getIntArg(param.fonttype),
		Con::getIntArg(param.fontcolor),
		Con::getIntArg(param.fontsize));
}

void ClientGameplayState::RandomGate(int LineId)  
{
	if(m_ServerList.size())
	{
		int i;
		for(i=0;i<m_ServerList.size();i++)
		{
			if(m_ServerList[i].LineId == LineId)
				break;
		}

		if(i == m_ServerList.size())
			return;

		int chooseLineNo = i;
		int chooseLineId = LineId;
		int chooseGateNo;
		stServerInfo serverInfo;

		if(m_ServerList[chooseLineNo].GateList.size())
		{
			chooseGateNo = Platform::getRandomI(0,m_ServerList[chooseLineNo].GateList.size()-1);
			serverInfo  = m_ServerList[chooseLineNo].GateList[chooseGateNo];
			chooseLineId =m_ServerList[chooseLineNo].LineId;

			setGateIP(serverInfo.Ip);					
			setGatePort(serverInfo.Port);	
			setCurrentLineId(chooseLineId);
			Con::setIntVariable("$Pref::Net::Line",chooseLineId);
		}
	}
}

void ClientGameplayState::setSelectShortcutSlot(S32 type, S32 col, MouseEventType event, S32 num /* = 0 */, S32 price /* = 0 */)
{
	mSelectShortcut.type	= type;
	mSelectShortcut.col		= col;
	mSelectShortcut.event	= event;
	mSelectShortcut.nums	= num;
	mSelectShortcut.price	= price;

	Player* pPlayer = GetControlPlayer();
	ShortcutObject* pObj = NULL;
	if(pPlayer)
	{
		pObj = g_ItemManager->getShortcutSlot(pPlayer,type, col);
		if(pObj && event != SLOT_SHIFT_LEFTMOUSEDOWN)
		{
			StringTableEntry iconName = pObj->getIconName();
			char icon[16] = "\0";
			char name[32] = "\0";
			S32 pos =(int)(dStrchr( iconName, '.' ) - iconName ) ;
			dStrncpy(icon, sizeof(icon), iconName, pos);
			if(pObj->isItemObject())
				dSprintf(name, sizeof(name), "%s%s", "item/", icon);
			else
				dSprintf(name, sizeof(name), "%s%s", "skill/", icon);
			setCurrentCursor(StringTable->insert(name));
		}

	}
		

}

bool ClientGameplayState::isSelectShortcutSlot(S32 type,  S32 col)
{
	return (mSelectShortcut.type == type) &&  (mSelectShortcut.col == col);
}

void ClientGameplayState::clearSelectShortcutSlot()
{
	mSelectShortcut.type	= -1;
	mSelectShortcut.col		= -1;
	mSelectShortcut.event	= SLOT_NONE;

	GameConnection* con = GameConnection::getConnectionToServer();
	if(!con)
		return;
	GameBase* pObj = con->getControlObject();
	if(!pObj)
		return;
	Player* pPlayer = dynamic_cast<Player*>(pObj);
	if(!pPlayer)
		return;
	//以后添加代码
	popCursor();

}

bool ClientGameplayState::isShortcutSlotBeSelect()
{
	return mSelectShortcut.type != -1;
}

S32 ClientGameplayState::getSelectShortcutType()
{
	return mSelectShortcut.type;
}



S32 ClientGameplayState::getSelectShortcutCol()
{
	return mSelectShortcut.col;
}

ClientGameplayState::MouseEventType ClientGameplayState::getSelectShortcutMouseEvent()
{
	return mSelectShortcut.event;
}

S32 ClientGameplayState::getSelectShortcutNum()
{
	return mSelectShortcut.nums;
}

S32 ClientGameplayState::getSelectShortcutPrice()
{
	return mSelectShortcut.price;
}

// ----------------------------------------------------------------------------
// 设置当前操作行为
void ClientGameplayState::setCurrentAction(ClientGameplayAction* action)
{
	if(mCurrentAction)
		cancelCurrentAction();
	if(action)
		mCurrentAction = action;

	mCurrentAction->onActivated();
	if(mCurrentAction->getParamCount() == 0)
	{
		mCurrentAction->sendGameplayEvent();
		doneCurrentAction();
	}
}

// ----------------------------------------------------------------------------
// 获取当前操作行为
ClientGameplayAction* ClientGameplayState::getCurrentAction() const
{
	return mCurrentAction;
}

// ----------------------------------------------------------------------------
// 设置当前操作参数
void ClientGameplayState::setCurrentActionParam(ClientGameplayParam* param)
{
	if(getCurrentAction() == NULL)
		return;
	if(!getCurrentAction()->setParam(param))
	{
		cancelCurrentAction();
		return;
	}
	if(mCurrentAction->getCurrentParamCount() >= mCurrentAction->getParamCount())
	{
		mCurrentAction->sendGameplayEvent();
		doneCurrentAction();
	}
}

// ----------------------------------------------------------------------------
// 终止当前操作行为
void ClientGameplayState::cancelCurrentAction()
{
	if(mCurrentAction)
	{
		mCurrentAction->onCancle();
		delete mCurrentAction;
	}
	mCurrentAction = NULL;
}

// ----------------------------------------------------------------------------
// 结束完成当前操作
void ClientGameplayState::doneCurrentAction()
{
	if(mCurrentAction)
	{
		mCurrentAction->onDone();
		delete mCurrentAction;
	}
	mCurrentAction = NULL;
}

void ClientGameplayState::setTempAction(ClientGameplayAction* action)
{
	if(mTempAction)
		clearTempAction();
	if(action)
		mTempAction = action;
}

ClientGameplayAction* ClientGameplayState::getTempAction()
{
	return mTempAction;
}

bool ClientGameplayState::setTempAcrionParam(ClientGameplayParam* param)
{
	if (!getTempAction())
		return  false;
	if(!getTempAction()->setParam(param))
		return false;
	return true;
}

void ClientGameplayState::clearTempAction()
{
	if(mTempAction)
		delete mTempAction;
	mTempAction = NULL;
}

void ClientGameplayState::setTempToCurrentAction()
{
	if(mTempAction)
	{
		setCurrentAction(mTempAction);
		if(mCurrentAction->getCurrentParamCount() >= mCurrentAction->getParamCount())
		{
			mCurrentAction->sendGameplayEvent();
			doneCurrentAction();
		}
		mTempAction = NULL;
	}
}

void ClientGameplayState::clearObjectList()
{
	getObjectList().clear();
}

void ClientGameplayState::clearPath()
{
	 m_SearchPath.clear();
	 if (isStartNavigation())
	 {
		 setStartNavigation(false);
		 GuiControl* ctrl = dynamic_cast<GuiControl*>(Sim::findObject("SeekLoadDialogWnd"));
		 if(ctrl && ctrl->isVisible())
			 Con::executef("CloseFindPathDialog");
	 }
	
}
	 
void ClientGameplayState::setCurrentCursor(StringTableEntry iconName)
{
	PlatformWindow* window = WindowManager->getFirstWindow();
	if(!window || !iconName)
		return;
	if(_stricmp((window->getCursorController()->getCursorVector().last().mCursorName), iconName) != 0)
	{
		window->getCursorController()->popCursor();
		window->getCursorController()->pushCursor(iconName);
	}
}

void ClientGameplayState::popCursor()
{
	PlatformWindow* window = WindowManager->getFirstWindow();
	if(!window)
		return;
	window->getCursorController()->popCursor();
}

void ClientGameplayState::setGlobalSound(SFXSource* source)
{
	mGlobalSound = source;
}

void ClientGameplayState::deleteGlobalSound()
{
	if(mGlobalSound)
	{
		mGlobalSound->deleteObject();
	}
	mGlobalSound = NULL;
}

ClientGameplayState* ClientGameplayState::Instance(void)
{
    if (0 != g_ClientGameplayState)
    {
        return g_ClientGameplayState;
    }

    static ClientGameplayState local;
    g_ClientGameplayState = &local; 
    return g_ClientGameplayState;
}

bool ClientGameplayState::SendGMCommand(int sessionId,Base::BitStream* pack,const char* cmdName,const char* fmt,...)
{
    if (0 == cmdName || 0 == fmt)
        return false;

    static char buffer[4096];
    std::string cmd = cmdName;

    va_list args;
    va_start(args,fmt);
    _vsnprintf_s(buffer,sizeof(buffer),sizeof(buffer),fmt,(char*)args);

    cmd += "(";
    cmd += buffer;
    cmd += ");";

    return SendGMCommand(sessionId,pack,cmd.c_str());
}

bool ClientGameplayState::SendGMCommand(int sessionId,Base::BitStream* pack,const char* fun)
{
    if(0 == fun || 0 == GetPacketProcess())
        return false;

    return GetPacketProcess()->SendGMCommand(sessionId,pack,getAccountId(),fun);
}

bool ClientGameplayState::HandleGMCommandResp(int sessionId,Base::BitStream &RecvPacket)
{
    static char cmdName[256];
    RecvPacket.readString(cmdName,256);

    int error = RecvPacket.readInt(Base::Bit32);
    
    //查找处理器
    GM_RESP_HANDLERS::iterator iter = m_gmRespHandlers.find(cmdName);

    if (iter == m_gmRespHandlers.end())
        return false;

    iter->second->OnResp(this,cmdName,error,sessionId,RecvPacket);

    return true;
}

void ClientGameplayState::ClearGMRespHandlers(void)
{
    m_gmRespHandlers.clear();
}

void ClientGameplayState::endFindPath()
{
	U32 iPathID = g_ClientGameplayState->findFindPathID();
	if(iPathID)
	{
		/*if(!mbStrideServer)
		{
			mStrideServerPath.clear();
			Con::executef("closeStrideFindPath");
			return;
		}*/
		Con::executef("closeStrideFindPath");
		RouteData *pRouteData = g_RouteRepository.getRouteData(iPathID);
		if(pRouteData)
		{
			U32 iType = pRouteData->mTargetType;
			U32 iTargetID = pRouteData->mTargetID;
			//if(iType == 1) //怪物
			//	Con::executef("closeStrideFindPath");
			if(iType == 2)
			{
				typedef stdext::hash_map<U32,GameObject*> ObjMap;
				ObjMap::iterator itr = g_ClientGameplayState->getObjectList().begin();
				for(; itr != g_ClientGameplayState->getObjectList().end(); ++itr)
				{
					GameObject* pObj = (itr->second);
					if(pObj && (pObj->getGameObjectMask() & NpcObjectType))
					{
						NpcObject* Npc = (NpcObject*)(pObj);
						if (Npc && !Npc->isCombative())
						{
							if(Npc->getDataID() == iTargetID)
							{
								mStrideServerPath.clear();
								//Con::executef("closeStrideFindPath");
								mbStrideServer = false;
								GameConnection* conn = GameConnection::getConnectionToServer();
								if(conn)
								{
									// 向服务端发送与NPC交互的消息
									ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_QUEST_TRIGGER_NPC);
									ev->SetInt32ArgValues(3, Npc->getServerId(), 0, -1);
									conn->postNetEvent(ev);
								}		
							}

						}
					}

				}

			}
		}

		g_ClientGameplayState->setFindPathID(0);
	}
	else if(iPathID == 0 && mbStrideServer)
	{
		mStrideServerPath.clear();
		mbStrideServer = false;
	}
	if(mbStrideServer)
	{	
		if(mStrideServerPath.size() > 0 && (mStrideServerPath[mStrideServerPath.size()-1] == iPathID))
		{
			mStrideServerPath.clear();
			mbStrideServer = false;
			Con::executef("closeStrideFindPath");
		}
			
	}
}

void ClientGameplayState::Cleanup()
{
	// 清除本地队伍信息
	CLIENT_TEAM->OnDisband();

	// 清除本地社会关系
	CLIENT_SOCIAL->cleanup();

	// 清除重置变量
	ShapeBase::sLastCtrlSimId = 0;
	m_RefreshPlayerModelView = true;
}

// ----------------------------------------------------------------------------
// 模型浏览
void ClientGameplayState::refreshPlayerModelView()
{
	if(!m_RefreshPlayerModelView)
		return;

	dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("PlayerModelView"));
	if ((!pObjView || !pObjView->isVisible()))
		return;

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player || !player->getTSSahpeInfo())
		return;

	// 将人物当前的模型显示到该控件
	pObjView->setPlayer(player->getTSSahpeInfo()->m_TSShapeId);
	for (S32 i=0; i<ShapeBase::MaxMountedImages; ++i)
	{
		if(player->getMountedImage(i))
		{
			StringTableEntry skinTagStr = player->getImageStruct(i)->skinNameHandle.getString();
			pObjView->setMounted(i, player->getMountedImage(i), player->getImageStruct(i)->mountPoint, player->getImageStruct(i)->mountPointSelf, (skinTagStr && skinTagStr[0]) ? skinTagStr[0] : 0);
		}
		else
		{
			pObjView->getMounted(i).clear();
		}
	}
	pObjView->setPlayerAnimation(GameObjectData::Root_a + player->getArmStatus());


	m_RefreshPlayerModelView = false;
}

void ClientGameplayState::setTryEquipOk(S32 index)
{
	Player* pPlayer = GetControlPlayer();
	if(!pPlayer)
		return;

	dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("PlayerMarketModeView"));
	if ((!pObjView || !pObjView->isVisible()))
		return;

	pObjView->clear();
	ShapeStack shapeStack;
	// 初始模型
	shapeStack.add(ShapeStack::Stack_Base, ((ShapeBaseData*)pPlayer->getDataBlock())->shapesSetId);
	shapeStack.add(ShapeStack::Stack_Base, dAtoi(Con::executef("getCreateFace",Con::getIntArg(pPlayer->getSex()),Con::getIntArg(pPlayer->mFace))));
	shapeStack.add(ShapeStack::Stack_Base, dAtoi(Con::executef("getCreateHair",Con::getIntArg(pPlayer->getSex()),Con::getIntArg(pPlayer->mHair))));

	bool isRefresh = true;
	if(index < 0 || index >= SuperMarketList::MAXSLOTS)
		isRefresh = false;
	stMarketItem* pMarketItem = pPlayer->mSuperMarketList.mSlots[index];
	if(!pMarketItem)
		isRefresh = false;
	Res* pRes = pMarketItem->marketItem->getRes();
	if(!pRes)
		isRefresh = false;
	
	if(pRes->getCategory() > Res::CATEGORY_EQUIPMENT)
		isRefresh = false;

	//职业判断
	if(!pRes->canFamilyLimit(pPlayer->getFamily()))
		isRefresh = false;
	//性别判断
	if(!pRes->canSexLimit(pPlayer->getSex()))
		isRefresh = false;
	if(isRefresh)
		shapeStack.add(ShapeStack::Stack_Equipment, pRes->getShapesSetID(pPlayer->getSex()), pRes->getEquipEffectID());

	shapeStack.refresh();
	U32 armStatus = 0;
	if(pRes->getCategory() == Res::CATEGORY_WEAPON)
	{
		armStatus= GETSUB(pRes->getSubCategory());
	}

	const ShapesSet& refSS = shapeStack.getShapes();
	pObjView->setPlayer(refSS.shapeName, refSS.skinName ? refSS.skinName[0] : 0);
	pObjView->addEffectMounted(refSS.effectId);
	for (S32 i=0; i<GameObject::MaxMountedImages; ++i)
	{
		if(!refSS.subImages[i].shapeName)
			continue;
		pObjView->setMounted(i, refSS.subImages[i].shapeName,refSS.subImages[i].mountPoint,refSS.subImages[i].mountPointSelf, refSS.subImages[i].skinName ? refSS.subImages[i].skinName[0] : 0);
		pObjView->addEffectMounted(refSS.subImages[i].effectId);
	}
	pObjView->setPlayerAnimation(GameObjectData::Root_a + armStatus);

}

//取消试穿
ConsoleFunction(ClickOnCancleSuperItem, void, 1, 1, "ClickOnCancleSuperItem()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	
	dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("PlayerMarketModeView"));
	if ((!pObjView || !pObjView->isVisible()))
		return;

	ShapeStack shapeStack;
	// 初始模型
	shapeStack.add(ShapeStack::Stack_Base, ((ShapeBaseData*)pPlayer->getDataBlock())->shapesSetId);
	shapeStack.add(ShapeStack::Stack_Base, dAtoi(Con::executef("getCreateFace",Con::getIntArg(pPlayer->getSex()),Con::getIntArg(pPlayer->mFace))));
	shapeStack.add(ShapeStack::Stack_Base, dAtoi(Con::executef("getCreateHair",Con::getIntArg(pPlayer->getSex()),Con::getIntArg(pPlayer->mHair))));

	shapeStack.refresh();

	const ShapesSet& refSS = shapeStack.getShapes();
	pObjView->clear();
	pObjView->setPlayer(refSS.shapeName, refSS.skinName ? refSS.skinName[0] : 0);
	pObjView->addEffectMounted(refSS.effectId);
	for (S32 i=0; i<GameObject::MaxMountedImages; ++i)
	{
		if(!refSS.subImages[i].shapeName)
			continue;
		pObjView->setMounted(i, refSS.subImages[i].shapeName,refSS.subImages[i].mountPoint,refSS.subImages[i].mountPointSelf, refSS.subImages[i].skinName ? refSS.subImages[i].skinName[0] : 0);
		pObjView->addEffectMounted(refSS.subImages[i].effectId);
	}
	pObjView->setPlayerAnimation(GameObjectData::Root_a + 0);
}



void ClientGameplayState::refreshPlayerModelAction()
{
	dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("PlayerModelView"));
	if ((!pObjView || !pObjView->isVisible()))
		return;

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player)
		return;

	pObjView->setPlayerAnimation(GameObjectData::Root_a + player->getArmStatus());
}

// ----------------------------------------------------------------------------
// 获取当前玩家对象
ConsoleFunction(GetPlayer, S32, 1, 1, "GetPlayer()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	return player ? player->getId() : 0;
}

//清除AI
ConsoleFunction(clearAIPath, void, 1, 1, "clearAIPath()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	Point3F pos = pPlayer->getPosition();
	pPlayer->m_pAI->SetMove(pos);
}