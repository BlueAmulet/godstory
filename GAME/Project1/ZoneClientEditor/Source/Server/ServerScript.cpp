
#include "T3D/gameConnection.h"
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "effects/EffectPacket.h"
#include "NetWork/ServerPacketProcess.h"

#include "Mail/ZoneMail.h"
#include <Windows.h>

//显示当前所有玩家列表
ConsoleFunction(ShowAllPlayer, void, 1, 1, "ShowAllPlayer()")
{
	S32 iCount = Sim::getClientGroup()->size();
	Con::warnf("----------- Player Info (Total: %d Players)-----------\n", iCount);
	Con::warnf("ConnID    NetID    PlayerID    ObjectName        PlayerName        PosX,PosY,PosZ\n");
	for(S32 i = 0; i < iCount; i++)
	{
		GameConnection* client = dynamic_cast<GameConnection*> ((*Sim::getClientGroup())[i]);
		GameBase*  controlobject = NULL;
		Player* player = NULL;
		if(client && (controlobject = client->getControlObject()) && (player = dynamic_cast<Player*>(controlobject)))
		{			
			Con::warnf("%d    %d    %d    %s      %s      %g,%g,%g\n",
				client->getId(), controlobject->getId(), player->getPlayerID(), player->getName(), 
				player->getPlayerName(), player->getPosition().x,player->getPosition().y, player->getPosition().z);
		}
	}
}

//向客户端指定玩家发送聊天消息
ConsoleFunction(SendOneChatMessage, void, 4, 4, "SendOneChatMessage(%channel, %text, %player)")
{
	Player* pPlayer = dynamic_cast<Player*>(Sim::findObject(dAtoi(argv[3])));
	if( pPlayer )
	{
		GameConnection* pConn = pPlayer->getControllingClient();
		if( pConn )
		{
			ZoneChatEvent* pEvent = new ZoneChatEvent( atoi( argv[1] ), "", (const char*)argv[2] );
			pConn->postNetEvent( pEvent );
		}
	}
}

//向客户端所有玩家发送聊天消息
ConsoleFunction(SendAllChatMessage, void, 3, 3, "SendAllChatMessage(%channel, %text)")
{
	S32 iCount = Sim::getClientGroup()->size();
	
	for(S32 i = 0; i < iCount; i++)
	{
		GameConnection* client = dynamic_cast<GameConnection*> ((*Sim::getClientGroup())[i]);
		GameBase*  controlobject = NULL;
		Player* player = NULL;
		if(client && (controlobject = client->getControlObject()) && (player = dynamic_cast<Player*>(controlobject)))
		{
			ZoneChatEvent* pEvent = new ZoneChatEvent( atoi( argv[1] ), "", (const char*)argv[2] );
			client->postNetEvent( pEvent );
		}
	}	
}

//向客户端指定玩家发送屏幕消息
ConsoleFunction(SendOneScreenMessage, void, 4, 4, "SendOneScreenMessage(%type, %text, %player)")
{
	Player* player = dynamic_cast<Player*>(Sim::findObject(dAtoi(argv[3])));
	if(player)
	{
		MessageEvent::send(player->getControllingClient(), dAtoi(argv[1]), argv[2], SHOWPOS_SCREEN);
	}
}

//向客户端所有玩家发送屏幕消息
ConsoleFunction(SendAllScreenMessage, void, 3, 3, "SendAllScreenMessage(%type, %text)")
{
	S32 iCount = Sim::getClientGroup()->size();

	for(S32 i = 0; i < iCount; i++)
	{
		GameConnection* client = dynamic_cast<GameConnection*> ((*Sim::getClientGroup())[i]);
		GameBase*  controlobject = NULL;
		Player* player = NULL;
		if(client && (controlobject = client->getControlObject()) && (player = dynamic_cast<Player*>(controlobject)))
		{
			MessageEvent::send(player->getControllingClient(), dAtoi(argv[1]), argv[2], SHOWPOS_SCREEN);
		}
	}
}

//获取当前地图下的玩家总数
ConsoleFunction(GetPlayerNum, S32, 1, 1, "GetPlayerNum()")
{
	S32 _inum = Sim::getClientGroup()->size();
	Con::warnf("当前地图玩家数目为：%d\n",_inum);
	return _inum;
}

//显示获取的对象类型
ConsoleFunction(GetSimObjectType, void, 2, 2, "GetSimObjectType(%obj)")
{
	SimObject* pObj = Sim::findObject(argv[1]);
	if(pObj)
		Con::printf("SimObjectType: %s",pObj->getClassName());
	else
		Con::printf("NULL Obj!");
}

//获得服务端的资源CRC，用来和客户端对比
ConsoleFunction(CollectionResInfo,void,1,1,"CollectionResInfo()")
{
	const char *pMissionFile = Con::getVariable("$Server::MissionFile");
	U16 len = dStrlen(pMissionFile);

	char fileName[100];
	if(!dStricmp(&pMissionFile[len-4],".mis"))
	{
		U16 pos;
		for(pos=len-4;pos>0;pos--){
			if(pMissionFile[pos]=='/')
				break;
		}

		AssertFatal(pos!=0, "Error! CollectionResInfo no found /!");

		dStrcpy(fileName, sizeof(fileName), &pMissionFile[pos+1]);
		U16 len2 = dStrlen(fileName);
		dMemcpy(&fileName[len2-4],".bin",4);
		fileName[len2] = 0;
		g_ServerGameplayState->setClientMisFileName(StringTable->insert(fileName));
	}
	else
	{
		AssertFatal(false, "Error! CollectionResInfo fatal!");
	}

	char fullpath[256];
	dMemcpy(fullpath,pMissionFile,len);
	dMemcpy(&fullpath[len-4],".bin",4);
	fullpath[len] = 0;

	U32 MisCrc;
	AssertFatal(ResourceManager->getCrc(fullpath, MisCrc), "Error! CollectionResInfo crc error!");
	Con::printf( "Crc Value: %s--%d", fullpath, MisCrc );
	g_ServerGameplayState->setClientMisFileCRC(MisCrc);
}
//刷单个怪物
ConsoleFunction(SpawnNpc,S32,2,3,"SpawnNpc(%NpcID, %layerId)")
{
	
	NpcObject* pNpc = new NpcObject;
	//
	if(!pNpc)
		return -1;
	
	pNpc->setDataID(dAtoi(argv[1]));
	pNpc->setLayerID(argc > 2 ? dAtoi(argv[2]) : 1);
	pNpc->registerObject();
	
	return pNpc->getId(); 
}


// 邮件相关-----------------------------------------------------------
 
// 发送接收邮件请求
ConsoleFunction( RecvMail, void, 4, 4, "RecvMail( recver, page, count )" )
{
	CZoneMail::RecvMail( atoi( argv[1]), atoi( argv[2]), atoi( argv[3]) );
}

ConsoleFunction( SendMail, void, 8, 8, "SendMail( sender, recver, title, msg, item, itemcount, money)" )
{	
	CZoneMail::SendMail( atoi(argv[1]), atoi( argv[2]), atoi( argv[5] ), atoi( argv[6] ), atoi( argv[7] ), (char*)argv[3], (char*)argv[4] );
}

ConsoleFunction( DeleteMail, void, 2, 2, "DeleteMail( mailId )" )
{
	CZoneMail::DeleteMail( atoi(argv[1]) );
}


ConsoleFunction( Sleep, void, 2, 2, "Sleep(milliseconds)" )
{
	U32 milliseconds = atoi(argv[1]);

	Sleep(milliseconds);
}

ConsoleFunction( MyAdd, S32, 4, 4, "MyAdd(a, b, c)")
{
	S32 a = atoi(argv[1]);
	S32 b = atoi(argv[2]);

	S32 c = a+b;
	char result[20];
	
	if (*argv[3] != 0)
	{
		_itoa(c, result, 10);
		Con::setLocalVariable(argv[3], result);
	}

	return c;
}

//打开技能系选择窗口
ConsoleFunction(OpenSkillSelect,void,6,6,"OpenSkillSelect(%player,%Series1,%Series2,%Series3,%Series4)")
{
	SimObject* obj = Sim::findObject(dAtoi(argv[1]));
	if(obj == NULL)
		return;
	Player* player = dynamic_cast<Player*>(obj);
	if(player == NULL)
		return;

    SceneObject* pObj = player->getInteraction();
	player->setInteraction(pObj,Player::INTERACTION_SELECTSKILL);

	//判断如果存在一种事件状态，就不打开技能系选择界面   
	enWarnMessage MsgCode = player->isBusy(Player::INTERACTION_SELECTSKILL);

	//发送消息通知客户端，打开技能系选择界面
	GameConnection *conn = player->getControllingClient();
    if(!conn)
		return;
	ServerGameNetEvent * ev =new ServerGameNetEvent(INFO_SKILL_OPERATE);
    ev->SetIntArgValues(5,0,dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]),dAtoi(argv[5]));
	conn->postNetEvent(ev);
}

//打开技能学习窗口
ConsoleFunction(OpenSkillStudy,void,7,7,"OpenSkillStudy(%player,%Series1,%Series2,%Series3,%Series4,%Series5)")
{
	SimObject* obj = Sim::findObject(dAtoi(argv[1]));
	if(obj == NULL)
		return;
	Player* player = dynamic_cast<Player*>(obj);
	if(player == NULL)
		return;
   
	SceneObject* pObj = player->getInteraction();
	player->setInteraction(pObj,Player::INTERACTION_LEARNSKILL);

	//判断如果存在一种事件状态，就不打开技能学习界面
	enWarnMessage MsgCode  = player->isBusy(Player::INTERACTION_LEARNSKILL);

	//发送消息通知客户端，打开技能学习界面
	GameConnection *conn = player->getControllingClient();    
   if(!conn)
		return;
	ServerGameNetEvent * ev =new ServerGameNetEvent(INFO_SKILL_OPERATE);
	ev->SetIntArgValues(6,1,dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]),dAtoi(argv[5]),dAtoi(argv[6]));
	conn->postNetEvent(ev);
}

//打开生活技能学习界面
ConsoleFunction(OpenLivingSkillStudy,void, 3, 3,"OpenLivingSkillStudy(%player,%Series)")
{
	SimObject* obj = Sim::findObject(dAtoi(argv[1]));
	if(obj == NULL)
		return;
	Player* player = dynamic_cast<Player*>(obj);
	if(player == NULL)
		return;

	SceneObject* pObj = player->getInteraction();
	player->setInteraction(pObj,Player::INTERACTION_STUDYLIVESKILL);

	//判断如果存在一种事件状态，就不打开生活技能学习   
	enWarnMessage MsgCode = player->isBusy(Player::INTERACTION_STUDYLIVESKILL);

	//发送消息通知客户端，打开生活技能学习界面 
	GameConnection *conn = player->getControllingClient();
	if(!conn)
		return;
	ServerGameNetEvent * ev =new ServerGameNetEvent(INFO_LIVINGSKILL_LEARN);
	ev->SetIntArgValues(1,atoi(argv[2]));
	conn->postNetEvent(ev);
}

//特效测试用
ConsoleFunction(test1,void, 2, 2,"test1(%player)")
{
	SimObject* obj = Sim::findObject(dAtoi(argv[1]));
	if(obj == NULL)
		return;
	Player* player = dynamic_cast<Player*>(obj);
	if(player == NULL)
		return;

	g_EffectPacketContainer.addEffectPacket(620001, player, player->getRenderTransform(), NULL,player->getPosition());
}

//更新排行榜
ConsoleFunction(UpdateTop, void, 2, 2, "" )
{
	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
	{
		char buf[1024];
		Base::BitStream sendPacket( buf, 1024 );
		stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_UpdateTop, atoi(argv[1]), SERVICE_WORLDSERVER );
		pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		pProcess->Send( sendPacket );
	}
}

//命运有缘人请求
ConsoleFunction(LotRequest, void, 2, 2, "" )
{
	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
	{
		char buf[1024];
		Base::BitStream sendPacket( buf, 1024 );
		stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_LotRequest, atoi(argv[1]), SERVICE_WORLDSERVER );
		pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		pProcess->Send( sendPacket );
	}
}

//命运有缘人请求
ConsoleFunction(AddLotRequest, void, 2, 2, "" )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1] ) );
	if( !pPlayer )
		return ;

	int sex = pPlayer->getSex();

	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
	{
		char buf[1024];
		Base::BitStream sendPacket( buf, 1024 );
		stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_AddToLotRequest, atoi(argv[1]), SERVICE_WORLDSERVER, sex );
		pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		pProcess->Send( sendPacket );
	}
}

//命运有缘人请求
ConsoleFunction(RemoveLotRequest, void, 2, 2, "" )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1] ) );
	if( !pPlayer )
		return ;

	int sex = pPlayer->getSex();

	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
	{
		char buf[1024];
		Base::BitStream sendPacket( buf, 1024 );
		stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_RemoveFromLotRequest, atoi(argv[1]), SERVICE_WORLDSERVER, sex );
		pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		pProcess->Send( sendPacket );
	}
}

// 取得ZoneId
ConsoleFunction( SptParseZoneId, int, 2,2,"")
{
	std::string str = argv[1];
	int index = str.find_last_of( '/' );

	str = str.substr( index + 1, str.size() - index - 1 - 4 );

	return atoi( str.c_str() );
}

//建立社会关系
ConsoleFunction(createRelation,void,3,3,"createRelation(%playerId,%type)")
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer(atoi(argv[1]));
	if(!pPlayer)
		return;

	SocialType::Type type = (SocialType::Type)atoi(argv[2]);

	//师徒关系
	if((SocialType::Master || SocialType::Prentice) && pPlayer->getMPInfo())
		pPlayer->getMPInfo()->createRelation(pPlayer,type);
}

//请求帮助指引
ConsoleFunction(HelpDirectByIndex, void, 3, 3, "HelpDirectByIndex(%playerId, %index)")
{   
	Player* pPlayer = g_ServerGameplayState->GetPlayer(atoi(argv[1]));
	if(!pPlayer)
		return;
	GameConnection *conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	ServerGameNetEvent * ev =new ServerGameNetEvent(INFO_HELPDIRECT);
	ev->SetIntArgValues(1,atoi(argv[2]));
	conn->postNetEvent(ev);
}