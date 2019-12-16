//------------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//------------------------------------------------------------------------------

#include "gui/controls/guiTreeViewCtrl.h"
#include "gui/controls/guiPopUpCtrl.h"
#include "T3D/gameConnection.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "terrain/terrData.h"
#include "NetWork/CommonClient.h"
#include "UI/dGuiObjectView.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientPlayerManager.h"
#include "GamePlay/Team/ClientTeam.h"
#include "T3D/fx/cameraFXMgr.h"
#include "sceneGraph/decalManager.h"
#include "Effects/ScreenFX.h"
#include "GamePlay/ClientGameplayAction.h"
#include "GamePlay/item/Res.h"
#include "Gameplay/GameObjects/Skill.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/Item/Prescription.h"
#include "Gameplay/Item/ProduceEquip.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "lightingSystem/synapseGaming/sgDynamicRangeLighting.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "Gameplay/loadmgr/ScriptLoader.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "util/powerStat.h"
#include "util/BackgroundLoadMgr.h"
#include "UI/guiListBoxAdvCtrl.h"
#include "Gameplay/Social/ClientSocial.h"
#include "T3D/fx/cameraFXMgr.h"

//获取客户端资源的CRC
ConsoleFunction(LoadClientMis,void,1,1,"LoadClientMis()")
{
	char fullpath[256];
	dSprintf(fullpath,256,"~/data/missions/%s",g_ClientGameplayState->getClientMisFileName());

	U32 MisCrc;
	char error[256];
	sprintf(error,"Error! LoadClientMis(%s) crc error",fullpath);
	AssertFatal(ResourceManager->getCrc(fullpath, MisCrc), error);
	AssertFatal(MisCrc == g_ClientGameplayState->getClientMisFileCRC(), "Error! LoadClientMis crc not match!");

	TerrainBlock::clearTerMap();
	Con::executef("exec",fullpath);
	Con::executef("loadNaviGrid");

	//GameConnection* conn = GameConnection::getConnectionToServer();
	//SimObject* grp = Sim::findObject("MissionGroup");
	//if(conn && grp)
	//	conn->addObject(grp);
}

// 设置客户端Mission文件
ConsoleFunction( setMissionFile, void, 2,2, "" )
{
	g_ClientGameplayState->setClientMisFileName( StringTable->insert( argv[1] ) );
}

//获取客户端资源的CRC
ConsoleFunction(getMissionCrc,int,1,1,"")
{
	char fullpath[256];
	dSprintf(fullpath,256,"%s",g_ClientGameplayState->getClientMisFileName());
	int len = strlen( fullpath );
	memcpy( &fullpath[len-4], ".bin", 4 );

	U32 MisCrc;
	char error[256];
	sprintf(error,"Error! LoadClientMis(%s) crc error",fullpath);
	AssertFatal(ResourceManager->getCrc(fullpath, MisCrc), error);
	Con::printf("Client Crc Value: %s--%d", fullpath, MisCrc );
	return MisCrc;
}


ConsoleFunction(LoadClientMisOpt,void,1,1,"LoadClientMis()")
{
	char filePath[256];
	dSprintf(filePath,256,"%s",g_ClientGameplayState->getClientMisFileName());
	int len = strlen( filePath );
	memcpy( &filePath[len-4], ".bin", 4 );
    
    g_ClientGameplayState->clearResource();
	TerrainBlock::clearTerMap();
	
	CTinySerializer serializer;
	CTinyStream stream( filePath, CTinyStream::Read );
	
	stream.readByte( (char*)&tagTinySerializerFileHeader::CurrentHeader, sizeof( tagTinySerializerFileHeader ) );

	AssertWarn( tagTinySerializerFileHeader::CurrentHeader.version == tagTinySerializerFileHeader::CurrentVersion, "游戏资源版本错误！" );

	serializer.unserializer( stream );
	
	Con::executef("loadNaviGrid");
	Con::executef("pathOnMissionLoadDone");

	//GameConnection* conn = GameConnection::getConnectionToServer();
	//SimObject* grp = Sim::findObject("MissionGroup");
	//if(conn && grp)
	//	conn->addObject(grp);
}

//连接到帐号服务器
ConsoleFunction(SptConnectAccountServer,void,3,3,"SptConnectAccountServer(string name,string pwd)")
{
	g_ClientGameplayState->setAccountName(argv[1]);
	g_ClientGameplayState->setAccountPWD(argv[2]);
	Con::setVariable("$pref::Player::Name",argv[1]);
	g_ClientGameplayState->ConnectServer(Con::getVariable("$JoinGameAddress"),SERVICE_ACCOUNTSERVER);
}

//连接到帐号服务器
ConsoleFunction(SptAccountServerCheck,void,3,3,"SptConnectAccountServer(string name,string pwd)")
{
	g_ClientGameplayState->setAccountName(argv[1]);
	g_ClientGameplayState->setAccountPWD(argv[2]);
	Con::setVariable("$pref::Player::Name",argv[1]);
	g_ClientGameplayState->ConnectServer(Con::getVariable("$JoinGameAddress"),SERVICE_ACCOUNTSERVER);
}

//清除帐号服务器连接检查
ConsoleFunction(SptClearAccountServerCheck, void, 1, 1,  "")
{
	g_ClientGameplayState->m_isPwdChecked = false;
}

//连接到网关服务器
ConsoleFunction(SptConnectGateServer,void,1,1,"SptConnectGateServer()")
{
	int IP = g_ClientGameplayState->getGateIP();
	char sIP[COMMON_STRING_LENGTH];
	dSprintf(sIP,COMMON_STRING_LENGTH,"%s:%d",inet_ntoa(*(in_addr*)&IP),htons(g_ClientGameplayState->getGatePort()));
	g_ClientGameplayState->ConnectServer(sIP,SERVICE_GATESERVER);
}

//连接到游戏服务器
ConsoleFunction(SptConnectGameServer,void,1,1,"SptConnectGameServer()")
{
	int AccountId	= g_ClientGameplayState->getAccountId();
	int PlayerId	= g_ClientGameplayState->getSelectedPlayerId();
	T_UID UID		= g_ClientGameplayState->getAccountUID();
	char Status		= g_ClientGameplayState->getLoginStatus();
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if(pProcess)
		pProcess->Send_Game_LoginRequest(UID,AccountId,PlayerId,Status);
}

//连接到地图服务器
ConsoleFunction(SptConnectZoneServer,void,1,1,"SptConnectZoneServer()")
{
	int IP	= g_ClientGameplayState->getZoneIP();
	int Port = g_ClientGameplayState->getZonePort();

	Con::executef("disconnect");
	g_ClientGameplayState->clearResource();

	//登录ZoneServer
	char sIP[COMMON_STRING_LENGTH];
	dSprintf(sIP,COMMON_STRING_LENGTH,"%s:%d",inet_ntoa(*(in_addr*)&IP),Port);
	char sLayer[COMMON_STRING_LENGTH];
	dSprintf( sLayer, COMMON_STRING_LENGTH, "1\0" );
	Con::printf("connect to zone : %s",sIP);
	Con::executef("connect",sIP, sLayer );
}

ConsoleFunction(SptConnectCopyMap,void,1,1,"")
{
	int IP	= g_ClientGameplayState->getCopymapZoneIp();
	int Port = g_ClientGameplayState->getCopymapZonePort();

	Con::executef("disconnect");
	g_ClientGameplayState->clearResource();

	//登录Copymap
	char sIP[COMMON_STRING_LENGTH];
	dSprintf(sIP,COMMON_STRING_LENGTH,"%s:%d",inet_ntoa(*(in_addr*)&IP),Port);
	char sLayer[COMMON_STRING_LENGTH];
	dSprintf( sLayer, COMMON_STRING_LENGTH, "%d", g_ClientGameplayState->getCopymapLayer() );
	Con::printf("connect to copymap : %s",sIP);
	Con::executef("connect",sIP, sLayer );
}


//设置是否玩家是主动退出的
ConsoleFunction(SptSetPlayerQuitTrue,void,1,1,"SptSetPlayerQuitTrue")
{
	g_ClientGameplayState->setPlayerQuit(true);
}

//玩家是否正在切换线路?
ConsoleFunction(SptIsLineSwitching,bool,1,1,"SptIsLineSwitching()")
{
	bool isSwitching = g_ClientGameplayState->isLineSwitching();
	return isSwitching;
}

//玩家是否在进入副本
ConsoleFunction(SptIsGoingCopymap,bool,1,1,"SptIsGoingCopymap()")
{
	return g_ClientGameplayState->isGoingCopyMap();
}

//关闭玩家进入副本标识
ConsoleFunction(SptDisableGoingCopymap,void,1,1,"SptDisableGoingCopymap()")
{
	g_ClientGameplayState->setGoingCopyMap( false );
	g_ClientGameplayState->setInCopymap( true );
}

//玩家解除切线标志
ConsoleFunction(SptDisableLineSwitching,void,1,1,"SptDisableLineSwitching()")
{
	g_ClientGameplayState->setTransporting(false);
	g_ClientGameplayState->setLineSwitching(false);
}

//玩家断开地图服务器的通知
ConsoleFunction(SptOnDisconnectZoneServer,void,1,1,"SptOnDisconnectZoneServer()")
{
	bool flag = g_ClientGameplayState->isLineSwitching() || g_ClientGameplayState->isTransporting() || g_ClientGameplayState->isGoingCopyMap();
	if(!flag)
	{
		//这里要判断玩家是主动退出的还是断线的
		T_UID UID		= g_ClientGameplayState->getAccountUID();
		int AccountId	= g_ClientGameplayState->getAccountId();

		int PlayerId	= g_ClientGameplayState->getSelectedPlayerId();
		UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
		if(pProcess && g_ClientGameplayState->isPlayerQuit() && !g_ClientGameplayState->isTick )
			pProcess->Send_Game_LogoutRequest(UID,AccountId,PlayerId);

		if(!g_ClientGameplayState->isPlayerQuit() && !g_ClientGameplayState->isTick)
		{
			//重新连接网关服务器
			if(pProcess)
				pProcess->Send_Gate_LoginRequest(UID,AccountId);
		}

		g_ClientGameplayState->setPlayerQuit(false);

		gClientSceneGraph->setFogDirty();

		//重置变量
		g_ClientGameplayState->setLoginStatus(SERVICE_GATESERVER);
	}
	// 清除客户端数据
	g_ClientGameplayState->onDisconnect();
}

//人物选择界面中删除角色
ConsoleFunction(SptDeletePlayer,void,1,1,"SptDeletePlayer()")
{
	int AccountId	= g_ClientGameplayState->getAccountId();
	int PlayerId	= g_ClientGameplayState->getSelectedPlayerId();
	T_UID UID		= g_ClientGameplayState->getAccountUID();
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if(pProcess)
		pProcess->Send_DeletePlayerRequest(UID,AccountId,PlayerId);
}

// 默认选择第一个玩家角色
ConsoleFunction(SptSetDefaultPlayer,void,1,1,"SptSetDefaultPlayer()")
{
	g_ClientGameplayState->setSelectedPlayerId(g_ClientGameplayState->m_PlayerList[0].BaseData.PlayerId);
	g_ClientGameplayState->setCurrentZoneId(g_ClientGameplayState->m_PlayerList[0].DispData.ZoneId);
}

//创建角色
ConsoleFunction(SptCreatePlayer,void,8,8,"SptCreatePlayer(PlayerName,Sex,Body,Face,Hair,HairCol,ItemId)")
{
	int AccountId	= g_ClientGameplayState->getAccountId();
	T_UID UID		= g_ClientGameplayState->getAccountUID();
	int Sex		=	dAtoi(argv[2]);
	int Body	=	dAtoi(argv[3]);
	int Face	=	dAtoi(argv[4]);
	int Hair	=	dAtoi(argv[5]);
	int HairCol	=	dAtoi(argv[6]);
	U32 ItemId	=	dAtol(argv[7]);
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if(pProcess)
		pProcess->Send_CreatePlayerRequest(UID,AccountId,argv[1],Sex,Body,Face,Hair,HairCol,ItemId);
}

ConsoleFunction(getNumString,void,2,2,"getNumString(num)")
{
	Con::printf("%d", dAtol(argv[1]));
}

// 获得当前线路总数量
ConsoleFunction(ServerListNum,S32,1,1,"ServerListNum()")
{
	return g_ClientGameplayState->m_ServerList.size();
}

// 获取线路ID
ConsoleFunction(GetLineId,S32,2,2,"GetLineId(index)")
{
	int Index = dAtoi(argv[1]);
	return g_ClientGameplayState->m_ServerList[Index].LineId;
}
//获取ZoneId
ConsoleFunction(GetZoneId,S32,1,1,"GetZoneId()")
{
  return g_ClientGameplayState->getCurrentZoneId();
}
// 登录时选择线路 
ConsoleFunction(SelectLine, void, 2, 2, "SelectLine( LineId )")
{
	int LineId = dAtoi(argv[1]);
	//if(g_ClientGameplayState->getCurrentLineId() == LineId)
	//	return;

	for(int i=0;i<g_ClientGameplayState->m_ServerList.size();i++)
	{
		stServerList sServerList =g_ClientGameplayState->m_ServerList[i];
		if(sServerList.LineId == LineId)
		{
			g_ClientGameplayState->setCurrentLineId(sServerList.LineId);
			int chooseGateNo = Platform::getRandomI(0,sServerList.GateList.size()-1);
			stServerInfo sServerInfo = sServerList.GateList[chooseGateNo];
			g_ClientGameplayState->setGateIP(sServerInfo.Ip);
			g_ClientGameplayState->setGatePort(sServerInfo.Port);

			int IP = g_ClientGameplayState->getGateIP();
			char sIP[COMMON_STRING_LENGTH];
			dSprintf(sIP,COMMON_STRING_LENGTH,"%s:%d",inet_ntoa(*(in_addr*)&sServerInfo.Ip),htons(sServerInfo.Port));
			g_ClientGameplayState->ConnectServer(sIP,SERVICE_GATESERVER);
			return;
		}
	}
}

// 在游戏中切换线路
ConsoleFunction(SptSelectLineInGame,void,2,2,"SptSelectLineInGame( LineId )")
{
	int LineId = dAtoi(argv[1]);

#ifndef SWITCH_LINE_DEBUG
	//if(g_ClientGameplayState->getCurrentLineId() == LineId)
	//	return;
#endif

	//发送消息给地图服务器
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn)
	{
		SwitchLineEvent *event = new SwitchLineEvent();
		event->setParam(LineId);
		conn->postNetEvent(event);
	}
}

// 获取角色数量
ConsoleFunction(PlayerListNum,S32,1,1,"PlayerListNum")
{
	return g_ClientGameplayState->m_PlayerList.size();
}

// 获取角色名称
ConsoleFunction(GetPlayerName,const char*,2,2,"GetPlayerName( index )")
{
	stPlayerSelect sPlayerData = g_ClientGameplayState->m_PlayerList[dAtoi(argv[1])];
	char* upNum = Con::getReturnBuffer(32);
	dSprintf(upNum,32,"%s",sPlayerData.BaseData.PlayerName);
	return upNum;
}
//获取角色等级
ConsoleFunction(SptGetPlayerLevel,S32,2,2,"SptGetPlayerLevel( index )")
{
	stPlayerSelect sPlayerData = g_ClientGameplayState->m_PlayerList[dAtoi(argv[1])];
	return sPlayerData.DispData.Level;
}

// 获取角色门宗
ConsoleFunction(SptGetPlayerFamily,const char*,2,2,"GetPlayerFamily( index )")
{
	stPlayerSelect sPlayerData = g_ClientGameplayState->m_PlayerList[dAtoi(argv[1])];
	char* upNum = Con::getReturnBuffer(32);
	dSprintf(upNum,32,"%d",sPlayerData.DispData.Family);
	return upNum;
}

// 选择角色
ConsoleFunction(SelectPlayer, void, 2, 2, "SelectPlayer( index )")
{
	stPlayerSelect& sPlayerData = g_ClientGameplayState->m_PlayerList[dAtoi(argv[1])];
	g_ClientGameplayState->setSelectedPlayerId(sPlayerData.BaseData.PlayerId);
	g_ClientGameplayState->setCurrentZoneId(sPlayerData.DispData.ZoneId);

    //记录玩家的参考点
    Point3F pt;
    sscanf_s(sPlayerData.DispData.LeavePos,"%f %f %f",&pt.x,&pt.y,&pt.z);
    CBackgroundLoadMgr::Instance()->SetRefPoint(pt);

	// 显示角色模型
	dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("SelectObjectView"));
	if (!pObjView)
		return;
	PlayerData* playerData = g_PlayerRepository.GetPlayerData(sPlayerData.DispData.Body);
	if(!playerData)
		return;

	pObjView->clear();
	// 场景
	pObjView->setScene("Scene_3");

	ShapeStack shapeStack;
	// 初始模型
	shapeStack.add(ShapeStack::Stack_Base, playerData->shapesSetId);
	shapeStack.add(ShapeStack::Stack_Base, dAtoi(Con::executef("getCreateFace",Con::getIntArg(sPlayerData.DispData.Sex),Con::getIntArg(sPlayerData.DispData.Face))));
	shapeStack.add(ShapeStack::Stack_Base, dAtoi(Con::executef("getCreateHair",Con::getIntArg(sPlayerData.DispData.Sex),Con::getIntArg(sPlayerData.DispData.Hair))));

	Res* pRes = new Res;
	// 普通装备
	for (S32 i=0; i<Res::EQUIPPLACEFLAG_TRUMP; ++i)
	{
		pRes->setBaseData(sPlayerData.DispData.EquipInfo[i].ItemID);
		shapeStack.add(ShapeStack::Stack_Equipment, pRes->getShapesSetID(sPlayerData.DispData.Sex), pRes->getEquipEffectID());
	}
	// 普通时装
	for (S32 i=Res::EQUIPPLACEFLAG_FASHIONHEAD; i<Res::EQUIPPLACEFLAG_FASHIONBACK; ++i)
	{
		pRes->setBaseData(sPlayerData.DispData.EquipInfo[i].ItemID);
		shapeStack.add(ShapeStack::Stack_Fashion, pRes->getShapesSetID(sPlayerData.DispData.Sex), pRes->getEquipEffectID());
	}
	// 门宗时装
	pRes->setBaseData(sPlayerData.DispData.EquipInfo[Res::EQUIPPLACEFLAG_FAMLIYFASHION].ItemID);
	shapeStack.add(ShapeStack::Stack_FamilyFashion, pRes->getShapesSetID(sPlayerData.DispData.Sex), pRes->getEquipEffectID());

	bool fashion = (sPlayerData.DispData.OtherFlag & Player::OTHERFLAG_USEFASHION) && !(sPlayerData.DispData.OtherFlag & Player::OTHERFLAG_FAMILYFASHION);
	bool familyFashion = (sPlayerData.DispData.OtherFlag & Player::OTHERFLAG_USEFASHION) && (sPlayerData.DispData.OtherFlag & Player::OTHERFLAG_FAMILYFASHION);
	shapeStack.setFlags(ShapeStack::Stack_Fashion, ShapeStack::Flags_Enabled, fashion);
	shapeStack.setFlags(ShapeStack::Stack_FamilyFashion, ShapeStack::Flags_Enabled, familyFashion);
	shapeStack.refresh();

	// 武器
	pRes->setBaseData(sPlayerData.DispData.EquipInfo[Res::EQUIPPLACEFLAG_WEAPON].ItemID);
	U32 armStatus = GETSUB(pRes->getSubCategory());

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

	delete pRes;
}

// 角色时装预览
ConsoleFunction(FamilyFashionPreview, void, 3, 3, "FamilyFashionPreview(%sex, %item)")
{
	// 显示角色模型
	dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("CreateObjectView"));
	if (!pObjView)
		return;

	// 检查性别
	U32 sex = dAtoi(argv[1]);
	if(sex != SEX_MALE && sex != SEX_FEMALE)
		return;
	// 门宗时装
	Res* pRes = new Res;
	pRes->setBaseData(dAtoi(argv[2]));
	if(!pRes->getBaseData())
		return;

	// 清除Player
	pObjView->setPlayer(NULL);

	ShapeStack shapeStack;
	shapeStack.add(ShapeStack::Stack_FamilyFashion, pRes->getShapesSetID(sex), pRes->getEquipEffectID());

	shapeStack.setFlags(ShapeStack::Stack_FamilyFashion, ShapeStack::Flags_Enabled, true);
	shapeStack.refresh();

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
	pObjView->setPlayerAnimation(GameObjectData::Root_a);

	delete pRes;
}

// 获取游戏主版本号
ConsoleFunction(GetGameVersion,const char*,1,1,"GetGameVersion()")
{
	return g_ClientGameplayState->GetGameVersion();
}

// 获取当前选择的角色id
ConsoleFunction(GetSelectedPlayerId,int,1,1,"GetSelectedPlayerId()")
{
	return g_ClientGameplayState->getSelectedPlayerId();
}

// 获取当前玩家的SecationId
ConsoleFunction(GetSelectedPlayerUID,int,1,1,"GetSelectedPlayerUID()")
{
	return g_ClientGameplayState->getAccountUID();
}

// 获取目标玩家的TeamId
ConsoleFunction( GetTargetPlayerTeamId, int, 1, 1, "GetTargetPlayerTeamId" )
{
	GameObject* pObj = g_ClientGameplayState->GetTarget();
	Player* pPlayer = dynamic_cast<Player*>( pObj );

	if( pPlayer != NULL )
	{
		Con::printf( "目标玩家的队伍ID为:%d", pPlayer->getTeamInfo().m_nId );
		return pPlayer->getTeamInfo().m_nId;
	}	
	return 0;
}

// 判断目标玩家是否是队长
ConsoleFunction( IsTargetPlayerCaption, int, 1, 1, "" )
{
	GameObject* pObj = g_ClientGameplayState->GetTarget();
	Player* pPlayer = dynamic_cast<Player*>( pObj );

	if( pPlayer != NULL )
	{
		return pPlayer->getTeamInfo().m_bIsCaption;
	}	
	return 0;
}
// 获得目标玩家的playerId
ConsoleFunction(GetTargetPlayer, int , 1, 1, "" )
{
	GameObject* pObj = g_ClientGameplayState->GetTarget();
	Player* pPlayer = dynamic_cast<Player*>( pObj );
	if( pPlayer )
		return pPlayer->getId();
	return 0;
}


ConsoleFunction(GetLocalPlayer, int, 2, 2, "" )
{
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer( atoi( argv[1] ) );
	if( pPlayer )
		return pPlayer->getId();

	return 0;
}

// 获取目标玩家的PlayerId
ConsoleFunction(GetTargetPlayerId,int,1,1,"GetTargetPlayerId()")
{
	GameObject* pObj = g_ClientGameplayState->GetTarget();
	Player* pPlayer = dynamic_cast<Player*>( pObj );

	if( pPlayer != NULL )
	{
		Con::printf( "目标玩家的ID为:%d", pPlayer->getPlayerID() );
		return pPlayer->getPlayerID();
	}	
	return 0;
}
// 发送一个聊天消息
ConsoleFunction(SptSendChatMessage,void,3,3,"SptSendChatMessage( type, msg )" )
{
	// 附近频道发送到Zone
	int nChannel = atoi( argv[1] );

	if( dStrlen( argv[2] ) == 0 )
		return ;

	if( nChannel == 10 || nChannel == 0 )
	{
		Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
		if( !pPlayer )
			return ;

		GameConnection* pConn = pPlayer->getControllingClient();
		if( !pConn )
			return ;

		ClientGameNetEvent* pEvent = new ClientGameNetEvent( ZONE_CHAT );
		pEvent->SetStringArgValues( 1, argv[2] );
		pConn->postNetEvent( pEvent );
	}
	else
	{
		UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
		if(pProcess)
			pProcess->Send_Chat_Message( g_ClientGameplayState->getSelectedPlayerId(), dAtoi( argv[1] ), argv[2] );
	}
}

// 发送一个目标聊天消息
ConsoleFunction(SptSendChatMessageTo,void,4,4,"SptSendChatMessageTo( type, msg, recver )" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if(pProcess)
		pProcess->Send_Chat_Message( g_ClientGameplayState->getSelectedPlayerId(), dAtoi( argv[1] ), argv[2], dAtoi( argv[3] ) );
}

// 改变聊天频道状态
ConsoleFunction( SptChangeChatChannelType,void,2,2,"SptChangeChatChannelType( type )")
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if(pProcess)
		pProcess->Send_Chat_ChangeType( g_ClientGameplayState->getSelectedPlayerId(), dAtoi( argv[1] ) );
}

// 自己创建一个队伍
ConsoleFunction( SptTeam_BuildAlong, void, 1, 1, "SptTeam_BuildAlong()" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_BuildAlongRequest( g_ClientGameplayState->getSelectedPlayerId() );
}

// 请求组队
ConsoleFunction( SptTeam_Build, void, 2, 2, "SptTeam_Build( dest_playerId )" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_BuildReuest( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 接受组队
ConsoleFunction( SptTeam_AcceptBuild, void, 2, 2, "SptTeam_AcceptBuild( dest_playerId )" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_AcceptBuildRequest( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 请求加入
ConsoleFunction( SptTeam_JoinRequest, void, 2, 2, "SptTeam_JoinRequest( dest_playerId )")
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_JoinRequest( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 同意加入
ConsoleFunction( SptTeam_AcceptJoin, void, 2, 2, "SptTeam_AcceptJoin( dest_playerId )" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_AcceptJoinRequest( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 队长添加
ConsoleFunction( SptTeam_AddRequest, void, 2, 2, "SptTeam_AddRequest( dest_playerId )" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_AddRequest( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 同意被加入
ConsoleFunction( SptTeam_AcceptAdd, void, 2, 2, "SptTeam_AcceptAdd( dest_playerId )" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_AcceptAddRequest( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 离开队伍
ConsoleFunction( SptTeam_LeaveRequest, void, 1, 1, "SptTeam_LeaveRequest()" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_Leave( g_ClientGameplayState->getSelectedPlayerId() );
}

// 队长踢人
ConsoleFunction( SptTeam_DropRequest, void, 2, 2, "" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_DropPlayer( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

ConsoleFunction( SptTeam_MapMark, void, 4, 4, "" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_MapMark( atoi( argv[1] ), atoi( argv[2] ), atoi( argv[3] ) );
}

ConsoleFunction( SptTeam_TargetMark, void, 3, 3, "" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_TargetMark( atoi( argv[1] ), atoi( argv[2] ) );
}

ConsoleFunction( SptTeam_GetMapMarkX, int, 2, 2, "" )
{
	if( CLIENT_TEAM->mapMarks[atoi( argv[1] )].sMapId != g_ClientGameplayState->getCurrentZoneId() )
		return 0;

	return CLIENT_TEAM->mapMarks[atoi( argv[1] )].sX;
}

ConsoleFunction( SptTeam_GetMapMarkY, int, 2, 2, "" )
{
	if( CLIENT_TEAM->mapMarks[atoi( argv[1] )].sMapId != g_ClientGameplayState->getCurrentZoneId() )
		return 0;

	return CLIENT_TEAM->mapMarks[atoi( argv[1] )].sY;
}

//获取技能指定内容
ConsoleFunction( GetSkillData, const char*, 3, 3, "" )
{
	SkillData* pData = g_SkillRepository.GetSkill( atoi( argv[1] ) );
	if( !pData )
		return "";

	std::string& str = RTTI::getStringValue( pData, argv[2] );

	char* pBuf = Con::getReturnBuffer( str.c_str() );
	return pBuf;
}

//根据类别显示技能界面已学的技能
ConsoleFunction(DisplaySkillBySchool,void, 2, 2, "DisplaySkillBySchool(%School)")
{

	Player* player = g_ClientGameplayState->GetControlPlayer();
	player->skillList_Common.Reset();

	U32 index = 0;
	Vector<U32> SkillSeriesView;

	for each( U32 SeriesID in g_SkillRepository.GetSeriesMap()->operator [](atoi(argv[1]) ) )
	{
		SkillSeriesView.push_back(SeriesID);
	}
	std::sort(SkillSeriesView.begin(),SkillSeriesView.end());

	for ( U32 i = 0;i<SkillSeriesView.size();i++ )
	{

		Skill* pSkill = player->GetSkillTable().GetSkill(SkillSeriesView[i]); 

		if (pSkill)
		{
			player->skillList_Common.AddSkillShortcut(player,SkillSeriesView[i]);

			//Todo::Update the UI
			Con::executef("ShowSkillNameAndLevel",Con::getIntArg(index),pSkill->GetData()->GetName(),Con::getIntArg(pSkill->GetData()->GetLevel()));
			index ++;
		}
	}
}

//获取某系技能
ConsoleFunction(GetSkillSeriesInfo,void, 3, 3, "GetSkillSeriesInfo(%School,%IsMain)")
{
	Player* player =g_ClientGameplayState->GetControlPlayer();
	player->skillList_Study.Reset();

	Vector<U32> SkillSeriesView;
	SkillData* mData = NULL;
	for each( U32 SeriesID in g_SkillRepository.GetSeriesMap()->operator [](atoi(argv[1]) ) )
	{
		mData = g_SkillRepository.GetSkill(SeriesID,1);
		if(player && mData )
		{
			if( mData->IsFlags(SkillData::Flags_Learnable) && mData->GetLearnLimit().family[atoi(argv[2])] & BIT(player->getFamily())
				/*&& mData->GetLearnLimit().classes & BIT(atoi(argv[2]))*/)
				SkillSeriesView.push_back(SeriesID);
		}	
	}
	std::sort(SkillSeriesView.begin(),SkillSeriesView.end());

	for ( U32 i= 0; i < SkillSeriesView.size();i++ )
	{		
		if(player)
			player->skillList_Study.AddSkillStudyShortcut(player,SkillSeriesView[i]);
	}
}

//获取指定技能信息
ConsoleFunction(GetStudySkillInfo,void, 2, 2, "GetStudySkillInfo(%SkillId)")
{
   SkillData* mData = g_SkillRepository.GetSkill(atol(argv[1]));
   if (mData)
   {
	   Con::executef("RecvStudySkillInfo",mData->GetIcon(),mData->GetName(),Con::getIntArg(mData->GetLevel()),
		              Con::getIntArg(mData->GetLearnLimit().level),Con::getIntArg(mData->GetLearnLimit().exp),
					  Con::getIntArg(mData->GetLearnLimit().money),mData->GetText());
   }
}

//学习技能
ConsoleFunction(StudySkill,void, 2, 2,"StudySkill(%SkillID)")
{
	GameConnection* conn = dynamic_cast<GameConnection*>(GameConnection::getConnectionToServer());
	if (conn)
	{
		ClientGameNetEvent* mEvent= new ClientGameNetEvent(INFO_SKILL_OPERATE);
		mEvent->SetInt32ArgValues(3,1,dAtoi(argv[1]),0);
		conn->postNetEvent(mEvent);
	}
}
/////////////////////////////////////////测试用/////////////////////////////////////////
/////////////////////////////////////////测试用/////////////////////////////////////////
/////////////////////////////////////////测试用/////////////////////////////////////////

// 发送一个测试聊天消息
ConsoleFunction(SptSendChatTestMessage,void,2,2,"SptSendChatTestMessage( string msg )" )
{
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if(pProcess)
		pProcess->Send_Chat_TestMessage( g_ClientGameplayState->getSelectedPlayerId(), argv[1] );
}


// 测试event
ConsoleFunction(EventsTest, void , 1,1, "EventsTest(void);")
{

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_QUEST_TRIGGER_NPC);
	GameConnection* conn = GameConnection::getConnectionToServer();
	pEvent->SetInt32ArgValues(3,1,2,-1);
	if(conn)
		conn->postNetEvent(pEvent);

}

ConsoleFunction(addcamerafx, void , 9, 9, "addcamerafx(a1,a2,a3,a4,a5,a6,a7,a8);")
{
	F32 Duration = dAtof(argv[1]);                                 //特效总维持时间
	VectorF mFreq;
	mFreq.x = dAtof(argv[2]);                                      //频率
	mFreq.y = dAtof(argv[3]);
	mFreq.z = dAtof(argv[4]);
	VectorF StartAmp;
	StartAmp.x = dAtof(argv[5]);                                   //幅度
	StartAmp.y = dAtof(argv[6]);
	StartAmp.z = dAtof(argv[7]);
	F32 Falloff = dAtof(argv[8]);                                  //衰减值

	return gCamFXMgr.addFX(Duration, mFreq, StartAmp, Falloff);
}

ConsoleFunction(AddRobot,void, 2,2, "" )
{
	int port = atoi( argv[1] );

	Con::evaluatef( "connectFoo(\"192.168.1.135:31001\",robot_%d,%d);", port, port );

}

ConsoleFunction(addScreenFx,void,5,5, "addScreenFx(a1,a2,a3,a4);")
{
	CScreenFX* temp = new CScreenFX(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]));
	g_ScreenFXMgr.Add(temp);
}

ConsoleFunction(setScreenMask,void,2,2, "setScreenMask(a1);")
{
	g_ScreenFXMgr.setScreenMask(argv[1]);
}
 
ConsoleFunction(RiseRequest, void , 2, 2, "" )
{
	GameConnection* conn = GameConnection::getConnectionToServer();
   if(conn)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_RISE_REQUEST);
		pEvent->SetIntArgValues(1,dAtoi(argv[1]));
	    conn->postNetEvent(pEvent);
	}	
}

ConsoleFunction( SptForceQuit, void, 1,1, "" )
{
	T_UID UID		= g_ClientGameplayState->getAccountUID();
	int AccountId	= g_ClientGameplayState->getAccountId();
	UserPacketProcess *pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Game_LogoutRequest(UID,AccountId,g_ClientGameplayState->getSelectedPlayerId());
}

//宠物系统相关脚本函数:
//SpawnPetRequest:		宠物召唤
//DisbandPetRequest:	宠物收回
//ReleasePetRequest:	宠物解散
ConsoleFunction(SpawnPetRequest, void , 2, 2, "" )
{
	PetOperationManager::SpawnPet(g_ClientGameplayState->GetControlPlayer(), dAtoi(argv[1]));
}

ConsoleFunction(DisbandPetRequest, void , 2, 2, "" )
{
	PetOperationManager::DisbandPet(g_ClientGameplayState->GetControlPlayer(), dAtoi(argv[1]));
}

ConsoleFunction(ReleasePetRequest, void , 2, 2, "" )
{
	PetOperationManager::ReleasePet(g_ClientGameplayState->GetControlPlayer(), dAtoi(argv[1]));
}

 //按类显示生活技能
ConsoleFunction(DisplayLivingSkill,void,2,2,"DisplayLivingSkill(%tab)")
{
	if((atoi(argv[1])<0 && atoi(argv[1])> LivingSkillTable::MAXTAB))
	{
		AssertWarn(false,"DisplayLivingSkill(%tab)::非法参数");
		return;
	}

	Player* myself = g_ClientGameplayState->GetControlPlayer();
	if(!myself || !myself->pLivingSkill)
		return;
	
	LivingSkillData* pData = NULL;
	LivingSkill* pSkill = NULL;
	U8 iTab = atoi(argv[1]);
	Vector<U32>& vecTabMap = myself->pLivingSkill->getLivingSkillView(iTab);
	Vector<U32>::iterator it   = vecTabMap.begin();

	for (; it != vecTabMap.end(); it++)
	{
		U32 skillId= *it;
		if( skillId < LIVINGSKILL_ID_LIMIT)
		{
			myself->mLivingSkillList->AddLivingSkillShortcut(skillId,0);
			continue;
		}

		LivingSkillTable::LivingSkillTableMap::iterator it = myself->pLivingSkill->getLivingSkillTable().find(GETSUBCATEGORY(skillId));
		if(it != myself->pLivingSkill->getLivingSkillTable().end())
		{
			pSkill = it->second;
			if(!pSkill)
				continue;
			myself->mLivingSkillList->AddLivingSkillShortcut(skillId,pSkill->getRipe());
		}
			
		pData = g_LivingSkillRespository.getLivingSkillData(skillId);
		if(!pData)
			continue;
		myself->mLivingSkillList->AddLivingSkillShortcut(skillId,pData->getRipe());

	}
}

//生活技能学习列表
ConsoleFunction(GetLivingSkillSeries,void, 2, 2, "GetLivingSkillSeries(%Series)")
{  
  Player* myself = g_ClientGameplayState->GetControlPlayer();
  if(!myself || !myself->pLivingSkill)
	  return;
  
  LivingSkillData* pData = NULL;
  LivingSkill* pSkill = NULL;
  U8 iTab = atoi(argv[1]);

  if (iTab>8)
  {
	  iTab =0;
  }
  Vector<U32>& vecTabMap = myself->pLivingSkill->getLivingSkillView(iTab);
  Vector<U32>::iterator it   = vecTabMap.begin();
  
  for (; it != vecTabMap.end(); it++)
  {
	  U32 skillId= *it;
	  if( skillId < LIVINGSKILL_ID_LIMIT)
	  {		 
		  continue;
	  }
	  pData = g_LivingSkillRespository.getLivingSkillData(skillId);

	  //这里特殊处理采集大类的6个子类
	  if(iTab == 0 )
	  {
		  if (pData->getSubCategory() ==  SUB(1,atoi(argv[1])-8))
		  {
			  LivingSkillTable::LivingSkillTableMap::iterator itr = myself->pLivingSkill->getLivingSkillTable().find(GETSUBCATEGORY(skillId));

			  if(itr != myself->pLivingSkill->getLivingSkillTable().end())
			  {
				  LivingSkillData* tData = g_LivingSkillRespository.getLivingSkillData(skillId+1);
				  if(tData)
					  pData = tData;
				  else
					  pData = NULL;
			  }			 
		  }
		  else
			  pData = NULL;
	  }
	  else //这里是处理其它8个大类的生活技能
	  {
		  LivingSkillTable::LivingSkillTableMap::iterator itr = myself->pLivingSkill->getLivingSkillTable().find(GETSUBCATEGORY(skillId));
		  if(itr != myself->pLivingSkill->getLivingSkillTable().end())
		  {
			  LivingSkillData* tData = g_LivingSkillRespository.getLivingSkillData(skillId+1);
			  if(tData)
				  pData = tData;
			  else
				  pData = NULL;
		  }
	  }
	  if (pData)
	  {
		  Con::executef("AddLivingSkillStudyList",Con::getIntArg(pData->getID()),pData->getName(),Con::getIntArg(pData->getRipe()),
			  Con::getIntArg(pData->getLevel()),Con::getIntArg(pData->getLearnLimit().exp),Con::getIntArg(pData->getLearnLimit().level),
			  Con::getIntArg(pData->getLearnLimit().money),Con::getIntArg(pData->getLearnLimit().ripe),pData->getDesc());
	  }
  }
}


//按类查询配方
ConsoleFunction(QueryPrescription,void,2,4,"QueryPrescription(%tab,%text,%colorLevel)")
{
	if((atoi(argv[1])<1 && atoi(argv[1])> LivingSkillTable::MAXTAB))
	{
		AssertWarn(false,"QueryPrescription(%tab)::非法参数");
		return;
	}
	
	Player* myself = g_ClientGameplayState->GetControlPlayer();
	if(!myself || !myself->pPrescription)
		return;

	static GuiTreeViewCtrl* ctrlTree = dynamic_cast<GuiTreeViewCtrl*>(Sim::findObject("Prescription_TreeCtl"));
	static GuiPopUpMenuCtrl* ctrlPopUpMenu = dynamic_cast<GuiPopUpMenuCtrl*>(Sim::findObject("Prescription_PopUpMenuCtl"));
	if(!ctrlTree || !ctrlPopUpMenu)
		return;

	//清除数据
	ctrlTree->clearItem();
	
	//初始化
	if(argc == 2)
	{
		ctrlPopUpMenu->closePopUp();
		ctrlPopUpMenu->clear();
		//ctrlPopUpMenu->addEntry("全部",0);
		//ctrlPopUpMenu->setSelected(0,false);
	}

	char szNodeID[8] ={0,};//节点标记
	U8 iCategory = atoi(argv[1]);
	Prescription::PrescriptionMap& preTabMap = myself->pPrescription->getPrescriptionTab();
	Prescription::PrescriptionMap::iterator it = preTabMap.begin();

	for (int iMenu = 1; it != preTabMap.end(); it++)
	{
		PrescriptionData* pData = it->second;
		if(!pData)
			continue;

		U32 itemID = pData->getBaseItemID();
		ItemBaseData* pItemData = g_ItemRepository->getItemData(itemID);
		if(!pItemData)
			continue;

		U8 colorLevel = pItemData->getColorLevel();
		U8 icolorLevel = atoi(argv[3]);

		if(pData->getCategory() == iCategory && (icolorLevel == 0 || icolorLevel == colorLevel))
		{
			dSprintf(szNodeID,sizeof(szNodeID),"%d",pData->getSubCategory());
			S32 subId = ctrlTree->findMissionTreeNode(szNodeID);
			//增加配方类
			if(szNodeID[0] == '\0'|| subId == 0)
			{
				if(argc > 2 && ctrlPopUpMenu->getSelected()>0 && dStrcmp(argv[2],pData->getSubCategoryName()) != 0)
					continue;

				char szName[64]={0,};
				dSprintf(szName,sizeof(szName),"<t c='0xff9000ff'>%s</t>",pData->getSubCategoryName());
				ctrlTree->insertItem(0,szName,"","",0,1,1,szNodeID);

				if(argc == 2)
				{
					ctrlPopUpMenu->addEntry(pData->getSubCategoryName(),iMenu);
					iMenu++;
				}
			}
	
			//配方类下不同等级配方
			subId = ctrlTree->findMissionTreeNode(szNodeID);
			if(subId && (ctrlPopUpMenu->getSelected() == 0 || dStrcmp(argv[2],pData->getSubCategoryName()) == 0))
			{
				char szText[128]={0,};
				char szCommand[64]={0,}; 
				dSprintf(szCommand,sizeof(szCommand),"QueryPrescriptionItemInfo(%d);",pData->getSerialID());

				switch(colorLevel)
				{
				case Res::COLORLEVEL_GREY:
					dSprintf(szText,sizeof(szText),"<t c='0x9B9B9Bff'>[%d]%s</t>",pData->getLevel(),pData->getName() ? pData->getName() : pItemData->getItemName());
					break;
				case Res::COLORLEVEL_WHITE:
					dSprintf(szText,sizeof(szText),"<t c='0xffffffff'>[%d]%s</t>",pData->getLevel(),pData->getName() ? pData->getName() : pItemData->getItemName());
					break;
				case Res::COLORLEVEL_GREEN:
					dSprintf(szText,sizeof(szText),"<t c='0x00ff00ff'>[%d]%s</t>",pData->getLevel(),pData->getName() ? pData->getName() : pItemData->getItemName());
					break;
				case Res::COLORLEVEL_BLUE:
					dSprintf(szText,sizeof(szText),"<t c='0x6464ffff'>[%d]%s</t>",pData->getLevel(),pData->getName() ? pData->getName() : pItemData->getItemName());
					break;
				case Res::COLORLEVEL_PURPLE:
					dSprintf(szText,sizeof(szText),"<t c='0xa800ffff'>[%d]%s</t>",pData->getLevel(),pData->getName() ? pData->getName() : pItemData->getItemName());
					break;
				case Res::COLORLEVEL_ORANGE:
					dSprintf(szText,sizeof(szText),"<t c='0xff9000ff'>[%d]%s</t>",pData->getLevel(),pData->getName() ? pData->getName() : pItemData->getItemName());
					break;
				case Res::COLORLEVEL_RED:
					dSprintf(szText,sizeof(szText),"<t c='0x7700ffff'>[%d]%s</t>",pData->getLevel(),pData->getName() ? pData->getName() : pItemData->getItemName());
					break;
				default:break;
				}
				ctrlTree->insertItem(subId,szText,szCommand,"",0,1,1,avar("%d",pData->getLevel()));
				ctrlTree->setItemExpanded(subId,true);  //展开树结构
			}
		}
	}

	if(argc == 2)
	{
		ctrlPopUpMenu->sort();
		ctrlPopUpMenu->addEntry("全部",0);
		ctrlPopUpMenu->setSelected(0,false);
	}
}

ConsoleFunction(QueryPrescriptionItemInfo,void,2,2,"QueryPrescriptionItemInfo(%id)")
{
	Player* myself = g_ClientGameplayState->GetControlPlayer();
	if(!myself || !myself->pPrescription)
		return;

	U32 SerialID = atoi(argv[1]);
	PrescriptionData* pData = g_PrescriptionRepository.getPrescriptionData(SerialID);
	if(!pData)
		return;

	PrescriptionData::stMaterial* pMat = pData->getMaterial();
	if(!pMat)
		return;

	ItemBaseData* pItemData = g_ItemRepository->getItemData(pData->getBaseItemID());
	if(!pItemData)
		return;

	U8 colCount = myself->pPrescription->getColResCount(pData);
	Con::executef("clearMaterialItem");
	for(int i=0; i<colCount; i++)
	{
		U32 iCurrCount  = myself->inventoryList.GetItemCount(pMat[i].resID);
		Con::executef("displayMaterialInfo",
			Con::getIntArg(i),
			Con::getIntArg(pMat[i].resNum),
			Con::getIntArg(iCurrCount));
	}

	U32 count = myself->pPrescription->canCompose(myself,pData->getSerialID());
	count =  getMin(count,myself->getVigor() / pData->getVigour());

	Con::executef("PrescriptionInfo",
		Con::getIntArg(pData->getInsight()),
		Con::getIntArg(pData->getVigour()),
		Con::getIntArg(pData->getMoney()),
		pItemData->getItemName(),pData->getDesc(),
		Con::getIntArg(count),
		Con::getIntArg(pData->getSerialID()));

	g_ProduceEquip.refreshUI(myself,pData);

	myself->mPrescriptionList->initPrescriptionList(SerialID);
}

ConsoleFunction(startProduce,void,3,3,"startProduce(%Id,%num)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	if(!g_ClientGameplayState || !g_ClientGameplayState->pVocalStatus)
		return;

	if(g_ClientGameplayState->pVocalStatus->getStatus())
		return;

	U32 SerialId = atoi(argv[1]);
	S32 iNum = atoi(argv[2]);
	g_ProduceEquip.Produce(pPlayer,SerialId,iNum);
}

//清除吟唱状态
ConsoleFunction(clearVocalStatus,void,1,1,"clearVocalStatus()")
{
	if(g_ClientGameplayState && g_ClientGameplayState->pVocalStatus)
		g_ClientGameplayState->pVocalStatus->clear();
}
//获取吟唱状态
ConsoleFunction(getVocalStatus,bool,1,1,"getVocalStatus()")
{
	if(g_ClientGameplayState && g_ClientGameplayState->pVocalStatus)
		return g_ClientGameplayState->pVocalStatus->getStatus();

	return false;
}

//获取吟唱类型
ConsoleFunction(getVocalStatusType,S32,1,1,"getVocalStatusType()")
{
	if(g_ClientGameplayState && g_ClientGameplayState->pVocalStatus)
		return (int)g_ClientGameplayState->pVocalStatus->getStatusType();

	return 0;
}

//获取吟唱名
ConsoleFunction(getVocalStatusName,const char*,1,1,"getVocalStatusName()")
{
	if(g_ClientGameplayState && g_ClientGameplayState->pVocalStatus)
	{
		char* retBuffer =Con::getReturnBuffer(64);
		dSprintf(retBuffer,64,"%s",g_ClientGameplayState->pVocalStatus->getStateName());
		return retBuffer;
	}

	return "";
}

//设置吟唱
ConsoleFunction(setVocalStatus,bool,3,4,"setVocalStatus(%status,%types,%action)")
{
	if(g_ClientGameplayState && g_ClientGameplayState->pVocalStatus)
	{
		bool status = atoi(argv[1]);
		VocalStatus::VocalStatusType type = (VocalStatus::VocalStatusType)atoi(argv[2]);
		return g_ClientGameplayState->pVocalStatus->setStatus(type,status, argc > 3 ? dAtoi(argv[3]) : 0);
	}

	return false;
}

ConsoleFunction(setcamerashake, void , 5, 5, "setcamerashake(a1,a2,a3,a4);")
{
	F32 a1 = dAtof(argv[1]);                                        //镜头晃动速度
	F32 a2 = dAtof(argv[2]);                                        //镜头晃动范围
	F32 a3 = dAtof(argv[3]);                                        //镜头晃动范围调剂       
	F32 a4 = dAtof(argv[4]);                                        //晃动时间
	return gCameraShakeEX.SetConstants(a1,a2,a3,a4);
}

// 返回true表示原本就没有tempAction，返回false表示有tempAction
ConsoleFunction( ClearTempAction, bool, 1, 1, "" )
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return false;

	bool ret = true;

	if(player->GetSpell().GetPreSpell().pSkillData)
	{
		player->GetSpell().ResetPreSpell();
		ret = false;
	}

	return ret;
}

ConsoleFunction( getSkillCount, S32 ,1,1, "getSkillCount()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return 0;

	return getMax((S32)0, player->skillList_Common.FindEmptySlot());
}


#include "Common/version.h"

ConsoleFunction( GetCustomGameVersion, const char*, 1, 1, "" )
{
	return FULL_VERSION;
}

//----------------------------测试用------------------------------------
ConsoleFunction(setglow, void , 6, 6, "setglow(a1,a2,a3,a4,a5);")
{
	F32 a1 = dAtof(argv[1]);                                        //泛光颜色强度（0-10）默认为1
	F32 a2 = dAtof(argv[2]);                                        //泛光颜色强度(2者稍有区别，微调)（0-10）默认为1.5
	F32 a3 = dAtof(argv[3]);                                        //原景颜色比例（0-1）默认为1
	F32 a4 = dAtof(argv[4]);                                        //泛光图颜色比例（0-1）默认为1
	F32 a5 = dAtof(argv[5]);                                        //泛光晕开的范围，此值越小范围越大（0-1）默认为0.25
	return sgDRLSurfaceChain::SetGlowConstants(a1, a2, a3, a4, a5);
}

ConsoleFunction(showResource, void , 1, 1, "showResource();")
{
	CStat::Instance()->Print(0);
	CommonFontManager::PrintAllFont();
}

//----------------------------GUI人物模型显示------------------------------------
ConsoleFunction(refreshPlayerModelView, void , 1, 1, "refreshPlayerModelView();")
{
    g_ClientGameplayState->refreshPlayerModelView();
}

ConsoleFunction(refreshPetModelView, void , 2, 2, "refreshPetModelView(%slot);")
{
	dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("PetModelView"));
	if (!pObjView || !pObjView->isVisible())
		return;
	pObjView->getPlayer().clear();

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player)
		return;

	const stPetInfo* pPetInfo = player->getPetTable().getPetInfo(dAtoi(argv[1]));
	if(!pPetInfo)
		return;

	PetObjectData* pData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if(!pData || !pData->shapesSetId)
		return;

	const ShapesSet* pSet = g_ShapesSetRepository.GetSet(pData->shapesSetId);
	if(!pSet || !pSet->shapeName || !pSet->shapeName[0])
		return;

	pObjView->setPlayer(pSet->shapeName);
	pObjView->setPlayerAnimation(GameObjectData::Root_a);
}

ConsoleFunction(refreshMountModelView, void , 2, 2, "refreshMountModelView(%slot);")
{
	dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("MountModelView"));
	if (!pObjView || !pObjView->isVisible())
		return;
	pObjView->getPlayer().clear();

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player)
		return;

	const stMountInfo* pMountInfo = player->getPetTable().getMountInfo(dAtoi(argv[1]));
	if(!pMountInfo)
		return;

	MountObjectData* pData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if(!pData || !pData->shapesSetId)
		return;

	const ShapesSet* pSet = g_ShapesSetRepository.GetSet(pData->shapesSetId);
	if(!pSet || !pSet->shapeName || !pSet->shapeName[0])
		return;

	pObjView->setPlayer(pSet->shapeName);
	pObjView->setPlayerAnimation(GameObjectData::Root_a);
}
//-------------------------------------------------------------------
// 查看邮件
//-------------------------------------------------------------------
ConsoleFunction(SptMail_Request, void, 3, 3, "SptMail_Reuqest(%page,%pagecount)" )
{
	ClientGameNetEvent* pEvent = new ClientGameNetEvent(MAIL_REQUEST);
	pEvent->SetIntArgValues( 2, atoi( argv[1] ), atoi( argv[2] )  );
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection* pConn = pPlayer->getControllingClient();
	if( pConn )
		pConn->postNetEvent( pEvent );
}

//-------------------------------------------------------------------
// 写邮件
//-------------------------------------------------------------------
ConsoleFunction(SptMail_Send, void, 7, 7, "SptMail_Send(%itemId, %itemCount, %title, %content, %money, %playerId)" )
{
	ClientGameNetEvent* pEvent = new ClientGameNetEvent(MAIL_SEND);
	pEvent->SetInt32ArgValues( 4, atoi( argv[1] ), atoi( argv[2] ), atoi( argv[5] ), atoi( argv[6] ) );
	pEvent->SetStringArgValues( 2, argv[3], argv[4] );
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection* pConn = pPlayer->getControllingClient();
	if( pConn )
		pConn->postNetEvent( pEvent );			
}

//------------------------------------------------------------------
// 收取Money
//------------------------------------------------------------------
ConsoleFunction(SptMail_GetMoney, void, 2, 2, "" )
{
	ClientGameNetEvent* pEvent = new ClientGameNetEvent(MAIL_GETMONEY);
	pEvent->SetInt32ArgValues( 1, atoi( argv[1] ) );
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection* pConn = pPlayer->getControllingClient();
	if( pConn )
		pConn->postNetEvent( pEvent );			
}
//------------------------------------------------------------------
// 收取Item
//------------------------------------------------------------------
ConsoleFunction(SptMail_GetItem, void, 2, 2, "" )
{
	ClientGameNetEvent* pEvent = new ClientGameNetEvent(MAIL_GETITEMS);
	pEvent->SetInt32ArgValues( 1, atoi( argv[1] ) );
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection* pConn = pPlayer->getControllingClient();
	if( pConn )
		pConn->postNetEvent( pEvent );			
}
//------------------------------------------------------------------
// 更新邮件界面按钮状态
//------------------------------------------------------------------
ConsoleFunction(GuiMail_ClearButtonStatus, void, 2, 2, "" )
{
	GuiListBoxAdvCtrl* pCtrl = (GuiListBoxAdvCtrl*)Sim::findObject( argv[1] );
	if( !pCtrl )
		return ;

	GuiListBoxAdvCtrl::ItemMap& map = pCtrl->getItemMap();

	GuiListBoxAdvCtrl::ItemMap::iterator it;

	char buf[1024];
	for( it = map.begin(); it != map.end(); it++ )
	{
		sprintf_s( buf, sizeof( buf ), "%sbutton", it->first.c_str() );

		GuiButtonCtrl* pButton = (GuiButtonCtrl*)Sim::findObject( buf );

		if( pButton )
		{
			pButton->setStateOn( 0 );
		}
	}
}

//-----------------------------------------------------------------
// 更新收件人列表
//-----------------------------------------------------------------
ConsoleFunction( GuiMail_UpdateRecver, void, 1, 1, "" )
{
	Con::executef( "clearMailRecvers" );

	// 将所有的仙友加到发件人列表中
	CClientSocial::SocialMap& map = g_clientSocial.getClientSocialInfo();

	CClientSocial::SocialMap::iterator it = map.begin();
	for( ; it != map.end(); it++ )
	{
		if(  it->second.friendValue > 0 )
			Con::evaluatef( "addMailRecver(\"%s\", %d );", it->second.name, it->second.id );
	}
}

//---------------------------------------------------------------------
// 删除邮件
//---------------------------------------------------------------------
ConsoleFunction(SptMail_Delete, void, 2, 2, "" )
{
	ClientGameNetEvent* pEvent = new ClientGameNetEvent(MAIL_DELETE);
	pEvent->SetInt32ArgValues( 1, atoi( argv[1] ) );
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection* pConn = pPlayer->getControllingClient();
	if( pConn )
		pConn->postNetEvent( pEvent );			
}

//---------------------------------------------------------------
// 清空邮件
//---------------------------------------------------------------
ConsoleFunction( SptDeleteAllMail, void, 1, 1, "" )
{
	ClientGameNetEvent* pEvent = new ClientGameNetEvent(MAIL_DELETEALL);
	pEvent->SetInt32ArgValues( 1, atoi( argv[1] ) );
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection* pConn = pPlayer->getControllingClient();
	if( pConn )
		pConn->postNetEvent( pEvent );	
}

//---------------------------------------------------------------
// 向其他玩家索取物品
//---------------------------------------------------------------
ConsoleFunction( SptItemRequest, void, 4, 4, "SptItemRequest( %destPlayerId, %itemId, %itemCount );" )
{
	if( g_ClientGameplayState->GetControlPlayer() == 0 )
		return ;

	int nDestPlayerId = atoi( argv[1] );
	int itemId = atoi( argv[2] );
	int itemCount = atoi( argv[3] );

	char buf[128];
	Base::BitStream sendPacket( buf, 128 );
	stPacketHead* pHead =IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_ItemRequest, g_ClientGameplayState->getSelectedPlayerId(), SERVICE_WORLDSERVER, nDestPlayerId );

	sendPacket.writeInt( itemId, 32 );
	sendPacket.writeInt( itemCount, 32 );

	sendPacket.writeString( g_ClientGameplayState->GetControlPlayer()->getPlayerName() );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}
//得到物品的限制等级
ConsoleFunction(getItemLimitLevel, S32, 2, 2, "getItemLimitLevel(%index)")
{
    Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return -1;
	ShortcutObject* pObj = pPlayer->inventoryList.GetSlot(dAtoi(argv[1]));
	if(!pObj)
		return -1;
	ItemShortcut* pItem = (ItemShortcut*)pObj;
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return -1;
	return pRes->getLimitLevel();
}

ConsoleFunction(followTarget, void , 1, 1, "followTarget();")
{
    Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
    if(!pPlayer)
        return;
    if(!pPlayer->CanAttackTarget(pPlayer->GetTarget()))
    {
        pPlayer->m_pAI->SetFollowObject(pPlayer->GetTarget(), INTERACTIONDISTANCE);
    }
    else
    {
        MessageEvent::show(SHOWTYPE_WARN, "敌对目标无法跟随！");
    }
}

ConsoleFunction(CopyTargetName, void , 1, 1, "CopyTargetName();")
{
    Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
    if(!pPlayer)
        return;
    Player* pTargetPlayer = (Player*)(pPlayer->GetTarget());
    std::string Targetname = const_cast<char*>(pTargetPlayer->getPlayerName());
    g_ClientGameplayState->setClipboard(Targetname);
}

ConsoleFunction(AddTargetInBanList, void , 1, 1, "AddTargetInBanList();")
{
    Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
    if(!pPlayer)
        return;
    Player* pTargetPlayer = (Player*)(pPlayer->GetTarget());
    std::string Targetname = const_cast<char*>(pTargetPlayer->getPlayerName());
    g_ClientGameplayState->m_BanNamelist.push_back(Targetname);
}

ConsoleFunction(CheckTargetInfo, void , 1, 1, "CheckTargetInfo();")
{
    Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
    if(!pPlayer)
        return;
    Player* pTargetPlayer = (Player*)(pPlayer->GetTarget());
    std::string Targetname = const_cast<char*>(pTargetPlayer->getPlayerName());
    g_ClientGameplayState->m_BanNamelist.push_back(Targetname);
}

