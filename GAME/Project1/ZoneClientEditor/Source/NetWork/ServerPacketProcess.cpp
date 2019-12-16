#define BASE_BITSTREAM

#include <time.h>
#include "base/md5.h"
#include "zlib/zlib.h"
#include "core/frameAllocator.h"
#include "GamePlay/GameObjects/PlayerObject.h"
#include "Common/PacketType.h"
#include "Common/PlayerStruct.h"
#include "CommonClient.h"
#include "ServerPacketProcess.h"
#include "Gameplay/ServerGameplayState.h"
#include "GamePlay/GameEvents/GameNetEvents.h"
#include "T3D/gameConnection.h"

#include "Common/TeamBase.h"

#include "Gameplay/Team/ZoneTeamManager.h"
#include "Mail/ZoneMail.h"

#include "EventCommon.h"

#include "Gameplay/Social/ZoneSocial.h"
#include "Gameplay/Copymap/ZoneCopymap.h"


using namespace std;

void ServerPacketProcess::HandleClientLogin()
{
	m_ConnSequence++;

	int ZoneId = g_ServerGameplayState->getZoneId();
	RegisterServer(SERVICE_ZONESERVER,0,ZoneId,m_ConnSequence,
		Con::getVariable("$Pref::Net::BindAddress"),
		Con::getIntVariable("$Pref::Server::Port")); //去网关注册自己

	//ok，现在可以发送游戏信息了
}

void ServerPacketProcess::HandleClientLogout()
{
	//连接断开了，是否重新连接
	Con::printf("和网关服务器连接断开");
}

bool ServerPacketProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	char *pData	= (char *)(pHead) + GetHeadSize();

	Base::BitStream RecvPacket(pData,iSize-GetHeadSize());

	char buffer[256];
	sprintf_s( buffer, sizeof( buffer ), "SERVER MSG: id:%d size:%d", pHead->Message, pHead->PacketSize );
	g_Log.WriteLog( buffer );

	switch(pHead->Message)
	{
	case COMMON_RegisterResponse:
		Handle_Gate_RegisterResponse(pHead,RecvPacket);
		return true;
	case WORLD_ZONE_PlayerData:
		Handle_World_PlayerData(pHead,RecvPacket);
		return true;
	case WORLD_COPYMAP_PlayerData:
		Handle_World_CopymapData(pHead,RecvPacket);
		return true;
	case WORLD_ZONE_SavePlayerRequest:
		HandleWorldZoneSavePlayerRequest( pHead, RecvPacket );
		return true;
	case ZONE_WORLD_SwitchMapRequest:
		Handle_World_SwitchMapRequest(pHead,RecvPacket);
		return true;
	case ZONE_WORLD_SwitchMapResponse:
		Handle_World_SwitchMapResponse(RecvPacket);
		return true;
	case CLIENT_TEAM_TeamInfo:
		Handle_World_Zone_TeamInfo( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_PlayerJoined:
		Handle_World_Zone_TeamPlayerJoin( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_PlayerLeave:
		Handle_World_Zone_TeamPlayerLeave( pHead, RecvPacket );
		return true;

	case ZONE_WORLD_PlayerSaveResponse:
		Handle_Zone_World_PlayerSaveResponse( pHead, RecvPacket );
		return true;

	case ZONE_WORLD_LotRequest:
		Handle_Zone_World_LotRequest( pHead, RecvPacket );
		return true;

	case ZONE_WORLD_LotNotify:
		Handle_Zone_World_LotNotify( pHead, RecvPacket );
		return true;

	case WORLD_ZONE_TickPlayerRequest:
		Handle_World_TickPlayerRequest( pHead, RecvPacket );
		return true;
    case WORLD_ZONE_TransportPlayerRequest:
        Handle_World_TransportPlayerRequest( pHead, RecvPacket );
        return true;
    case WORLD_ZONE_GetPlayerPosRequest:
        Handle_World_GetPlayerPosRequest( pHead, RecvPacket );
        return true;
	case ZONE_WORLD_MailListResponse:
		Handle_World_Zone_RecvMailResponse( pHead, RecvPacket );
		return true;

	case ZONE_WORLD_MailGetItems:
		Handle_World_Zone_MailGetItems( pHead, RecvPacket );
		return true;
	case ZONE_WORLD_MailGetMoney:
		Handle_World_Zone_MailGetMoney( pHead, RecvPacket );
		return true;

	case ZONE_WORLD_DeleteAllMail:
		Handle_World_Zone_DeleteAllMail( pHead, RecvPacket );
		return true;
	case CLIENT_COPYMAP_OpenRequest:
		Handle_World_CopyMap_Open( pHead, RecvPacket );
		return true;
	case CLIENT_COPYMAP_OpenResponse:
		Handle_World_CopyMap_OpenAck( pHead, RecvPacket );
		return true;
	case CLIENT_COPYMAP_LeaveResponse:
		Handle_World_CopyMap_LeaveAck( pHead, RecvPacket );
		return true;
	case CLIENT_COPYMAP_CloseRequest:
		Handle_World_CopyMap_Close( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_BaseInfo:
		ZONE_TEAM_MGR->OnTeamBaseInfoNotify( pHead, RecvPacket );
		return true;
	}

	if( pHead->Message > MSG_SOCIAL_BEGIN && pHead->Message < MSG_SOCIAL_END )
		gEventMethod[pHead->Message]( &g_zoneSocial, pHead, RecvPacket );

	return false;
}

void ServerPacketProcess::Handle_Gate_RegisterResponse(stPacketHead* pHead,Base::BitStream &RecvPacket)
{
	if( pHead->DestZoneId == -1 )
	{
		Con::printf("Zone 注册失败，请检查数据库！");
		return ;
	}

	int NetLineId = RecvPacket.readInt(Base::Bit16);
	g_ServerGameplayState->setCurrentLineId(NetLineId);
	int NetGateId = RecvPacket.readInt(Base::Bit16);
	g_ServerGameplayState->setCurrentGateId(NetGateId);

	int MaxPlayerNum = RecvPacket.readInt(Base::Bit16);	
	int ServicePort	 = RecvPacket.readInt(Base::Bit16);	

	if(!Con::getBoolVariable("$Server::Dedicated"))
	{
		g_ServerGameplayState->setMaxPlayerNum(MaxPlayerNum);		//设置本地图的最大承载人数
		Con::setIntVariable("$Pref::Server::Port",ServicePort);		//设置本地图的服务端口
		Con::executef("initDedicated");								//启动地图服务器
	}

	//char buf[256];
	//sprintf_s( buf, sizeof( buf ), "fl_zone_%d", g_ServerGameplayState->getZoneId() );
	
}

void ServerPacketProcess::Handle_World_PlayerData(stPacketHead *pHead,Base::BitStream &RecvPacket)
{
	T_UID UID = RecvPacket.readInt(UID_BITS);
	int dataSize = RecvPacket.readInt(Base::Bit32);	

	if(dataSize)
	{
		U32 waterMark = FrameAllocator::getWaterMark ();
		U32 maxSize = FrameAllocator::getHighWaterMark () - FrameAllocator::getWaterMark ();

		AssertFatal(maxSize>(dataSize+MAX_PACKET_SIZE),"Handle_World_PlayerData Buffer Full!");

		U8 *pDataBuf = (U8 *) FrameAllocator::alloc(dataSize);
		RecvPacket.readBits(dataSize*Base::Bit8,pDataBuf);

		unsigned long UnZipBufSize = MAX_PACKET_SIZE;
		U8 *pUnZipBuf = (U8 *) FrameAllocator::alloc(UnZipBufSize);

		int iret = uncompress((Bytef *)pUnZipBuf,&UnZipBufSize,(const Bytef *)pDataBuf,dataSize);
		if (!iret)
		{
			Base::BitStream dataPacket(pUnZipBuf,UnZipBufSize);
			stPlayerStruct *pPlayerData = new stPlayerStruct;

			pPlayerData->ReadData(&dataPacket);

			g_ServerGameplayState->SetPlayerSID( pPlayerData->BaseData.PlayerId, pPlayerData->BaseData.SaveUID );

			Con::printf( "RECV PLAYER DATA, POS: %s, SID: %d", pPlayerData->DispData.LeavePos, pPlayerData->BaseData.SaveUID );

			stTeamInfo teamInfo;
			teamInfo.ReadPacket( &RecvPacket );

			g_ServerGameplayState->AddPlayerLoginData(UID,&teamInfo,pPlayerData);
		}

		FrameAllocator::setWaterMark(waterMark);
	}

    //玩家可以登录游戏了
    char Buffer[30];
    Base::BitStream SendPacket(Buffer,30);
    stPacketHead *pSendHead = BuildPacketHead(SendPacket,WORLD_ZONE_PlayerDataResponse,pHead->Id,SERVICE_WORLDSERVER);
    SendPacket.writeInt(UID,UID_BITS);
    pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
    Send(SendPacket);
}

bool ServerPacketProcess::Send_World_TransportPlayerRequest(T_UID UID,stPlayerStruct *pPlayerData,int LineId,int TriggerId,
															int MapId,F32 x,F32 y,F32 z)
{
	U32 waterMark = FrameAllocator::getWaterMark ();
	U8 *pPacketBuf = (U8 *) FrameAllocator::alloc(MAX_PACKET_SIZE);

	int SrcZoneId = g_ServerGameplayState->getZoneId();

	if( LineId == 0 )
	{
		Con::printf( "玩家切换地图时，LineId错误!" );
	}

	Base::BitStream SendPacket(pPacketBuf,MAX_PACKET_SIZE);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,ZONE_WORLD_SwitchMapRequest,pPlayerData->BaseData.AccountId,SERVICE_WORLDSERVER,0,SrcZoneId);
	SendPacket.writeInt(UID,UID_BITS);
	SendPacket.writeInt(LineId,16);
	SendPacket.writeInt(TriggerId,32);
	SendPacket.writeInt(MapId,16);
	SendPacket.writeSignedInt(x*100,32);
	SendPacket.writeSignedInt(y*100,32);
	SendPacket.writeSignedInt(z*100,32); 

	U8 *pDataBuf = (U8 *) FrameAllocator::alloc(MAX_PACKET_SIZE);
	Base::BitStream DataPacket(pDataBuf,MAX_PACKET_SIZE);
	pPlayerData->WriteData(&DataPacket);
	pPlayerData->DispData.ZoneId = MapId;

	// 传送玩家队伍信息
	// [LivenHotch]: 由于WorldServer是有玩家的队伍信息的,所以这里可以不用传送
	Player* pPlayer = g_ServerGameplayState->GetPlayer( pPlayerData->BaseData.PlayerId );
	if( pPlayer )
		pPlayer->getTeamInfo().WritePacket( &DataPacket );

	int dataSize = DataPacket.getPosition();

	unsigned long ZipBufSize = MAX_PACKET_SIZE;
	U8 *pZipBuf = (U8 *) FrameAllocator::alloc(ZipBufSize);

	bool Ret = false;
	int iret = compress((Bytef *)pZipBuf,&ZipBufSize,(const Bytef *)pDataBuf,dataSize);
	if (!iret)
	{
		SendPacket.writeInt(ZipBufSize,16);
		SendPacket.writeBits(ZipBufSize*Base::Bit8,pZipBuf);
		pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
		Ret = Send(SendPacket);
	}

	FrameAllocator::setWaterMark(waterMark);
	return Ret;
}

void ServerPacketProcess::Handle_World_SwitchMapRequest(stPacketHead *pHead,Base::BitStream &RecvPacket)
{
	Base::BitStream *pPacket = &RecvPacket;

	T_UID UID		= pPacket->readInt(UID_BITS);
	int LineId		= pPacket->readInt(16);
	int TriggerId	= pPacket->readInt(32);
	int ZoneId	= pPacket->readInt(16);
	F32 x		= pPacket->readSignedInt(32);
	F32 y		= pPacket->readSignedInt(32);
	F32 z		= pPacket->readSignedInt(32);
	x/=100;
	y/=100;
	z/=100;

	int dataSize	= pPacket->readInt(16);

	U32 waterMark = FrameAllocator::getWaterMark ();

	U8 *pDataBuf = (U8 *) FrameAllocator::alloc(dataSize);
	RecvPacket.readBits(dataSize*Base::Bit8,pDataBuf);

	unsigned long UnZipBufSize = MAX_PACKET_SIZE;
	U8 *pUnZipBuf = (U8 *) FrameAllocator::alloc(UnZipBufSize);

	int iret = uncompress((Bytef *)pUnZipBuf,&UnZipBufSize,(const Bytef *)pDataBuf,dataSize);
	if (!iret)
	{
		Base::BitStream dataPacket(pUnZipBuf,UnZipBufSize);
		stPlayerStruct *pPlayerData = new stPlayerStruct;

		pPlayerData->ReadData(&dataPacket);

		g_ServerGameplayState->SetPlayerSID( pPlayerData->BaseData.PlayerId, pPlayerData->BaseData.SaveUID );

		//这里设置一些传送需要设置的数据
		if(TriggerId)
			pPlayerData->DispData.TriggerNo = TriggerId;
		pPlayerData->DispData.ZoneId	   = ZoneId;
		dSprintf(pPlayerData->DispData.LeavePos,sizeof(pPlayerData->DispData.LeavePos),"%.2f %.2f %.2f",x,y,z);

		// 接收队伍ID
		stTeamInfo teamInfo;
		teamInfo.ReadPacket( &dataPacket );

		g_ServerGameplayState->AddPlayerLoginData(UID, &teamInfo, pPlayerData);

		Send_World_TransportPlayerResponse(pHead,UID,pPlayerData->BaseData.AccountId,pPlayerData->BaseData.PlayerId,NONE_ERROR,LineId,ZoneId);
	}

	FrameAllocator::setWaterMark(waterMark);
}

void ServerPacketProcess::Send_World_TransportPlayerResponse(stPacketHead *pHead,T_UID UID,int AccountId,int PlayerId,int ErrorCode,int LineId,int ZoneId)
{
	char Buffer[128];
	Base::BitStream SendPacket(Buffer,128);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,ZONE_WORLD_SwitchMapResponse,AccountId,SERVICE_WORLDSERVER,pHead->SrcZoneId,0,pHead->LineServerId);
	SendPacket.writeInt(UID,UID_BITS);
	SendPacket.writeInt(PlayerId,32);
	SendPacket.writeInt(LineId,16);
	SendPacket.writeInt(ZoneId,16);
	SendPacket.writeInt(ErrorCode,16);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	Send(SendPacket);
}

void ServerPacketProcess::Handle_World_SwitchMapResponse(Base::BitStream &RecvPacket)
{
	Base::BitStream *pPacket = &RecvPacket;
	int Ip=0,Port=0;

	T_UID UID = pPacket->readInt(UID_BITS);
	int PlayerId = pPacket->readInt(32);
	int LineId	 = pPacket->readInt(16);
	int ZoneId	 = pPacket->readInt(16);
	int ErrorCode= pPacket->readInt(16);
	if(ErrorCode == NONE_ERROR)
	{
		if(pPacket->readFlag())
		{
			Ip = pPacket->readInt(32);
			Port = pPacket->readInt(16);
		}
	}

	Player *pPlayer = g_ServerGameplayState->GetPlayer(PlayerId);
	if(!pPlayer)
		return;

	if(ErrorCode == NONE_ERROR)
		pPlayer->doTransportObject(ErrorCode,UID,PlayerId,LineId,ZoneId,Ip,Port);
	else
	{
		pPlayer->setTransporting( false );   // 修正因为玩家传送不成功,而导致的玩家数据无法保存问题.
		
		pPlayer->mCopymapInstId = pPlayer->getLayerID();

		GameConnection *pCon = pPlayer->getControllingClient();
		if(pCon)
		{
			MessageEvent::send(pCon, SHOWTYPE_ERROR, 1001);
		}
	}
}

void ServerPacketProcess::Handle_World_Zone_TeamInfo( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	ZONE_TEAM_MGR->OnTeamInfoNotify( pHead, RecvPacket );
}

void ServerPacketProcess::Handle_World_Zone_TeamPlayerJoin( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nTeam = pHead->Id;
	int nPlayerId = pHead->SrcZoneId;
	ZONE_TEAM_MGR->OnTeamPlayerJoin( nPlayerId, nTeam );
}

void ServerPacketProcess::Handle_World_Zone_TeamPlayerLeave( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nTeam = pHead->Id;
	int nPlayerId = pHead->SrcZoneId;
	ZONE_TEAM_MGR->OnTeamPlayerLeave( nPlayerId, nTeam );
}

void ServerPacketProcess::Handle_World_TickPlayerRequest(stPacketHead* pHead, Base::BitStream &RecvPacket)
{
	int nPlayerId = pHead->Id;
	int nAccountId = pHead->SrcZoneId;

	bool bTickNow = RecvPacket.readFlag();

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, WORLD_ZONE_TickPlayerResponse, nPlayerId, SERVICE_WORLDSERVER, nAccountId );

	Player* pPlayer = g_ServerGameplayState->GetPlayer( nPlayerId );

	if( !pPlayer )
		pPlayer = g_ServerGameplayState->GetPlayerByAccount( nAccountId );

	if( SendPacket.writeFlag( pPlayer != NULL ) )
	{

		stPlayerStruct* pPlayerStruct = pPlayer->buildPlayerData();

		pPlayerStruct->WriteData( &SendPacket );
	}

	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	if( bTickNow && pPlayer )
	{

		if( pPlayer->getControllingClient() )
			pPlayer->getControllingClient()->deleteObject();

	}

	Send( SendPacket );
}

void ServerPacketProcess::Handle_World_TransportPlayerRequest(stPacketHead* pHead, Base::BitStream &RecvPacket)
{
    int nPlayerId  = pHead->Id;
    int nAccountId = pHead->SrcZoneId;

    Player* pPlayer = g_ServerGameplayState->GetPlayer( nPlayerId );

    if (0 == pPlayer)
        return;

    int mapId;
    float x,y,z;

    mapId = RecvPacket.readInt(Base::Bit32);
    RecvPacket.readBits(Base::Bit32,&x);
    RecvPacket.readBits(Base::Bit32,&y);
    RecvPacket.readBits(Base::Bit32,&z);

    pPlayer->transportObject(g_ServerGameplayState->getCurrentLineId(),0,mapId,x,y,z);
}

void ServerPacketProcess::Handle_World_GetPlayerPosRequest(stPacketHead* pHead, Base::BitStream &RecvPacket)
{
    int nPlayerId  = pHead->Id;
    int nAccountId = pHead->SrcZoneId;

    Player* pPlayer = g_ServerGameplayState->GetPlayer( nPlayerId );

    if (0 == pPlayer)
        return;

    Point3F pos = pPlayer->getPosition();

    CMemGuard buf(50);
    Base::BitStream SendPacket( buf.get(), 50 );

    stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, WORLD_ZONE_GetPlayerPosResponse, nPlayerId, SERVICE_WORLDSERVER, nAccountId );

    SendPacket.writeBits(Base::Bit32,&pos.x);
    SendPacket.writeBits(Base::Bit32,&pos.y);
    SendPacket.writeBits(Base::Bit32,&pos.z);
    
    pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
    Send( SendPacket );
}
void ServerPacketProcess::Handle_World_Zone_MailGetItems( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nMailId = pHead->Id;
	int nPlayerId = pHead->SrcZoneId;

	int nItemId = RecvPacket.readInt( 32 );
	int nItemCount = RecvPacket.readInt( 16 );

	// 通过邮件系统给玩家加物品
	Player* pPlayer = g_ServerGameplayState->GetPlayer( nPlayerId );
	if( !pPlayer )
		return ;

	int index;
	int error = g_ItemManager->addItemToInventory( pPlayer,nItemId,index,nItemCount );
	if( !error )
	{
		// 给玩家添加物品失败
		return ;
	}

	// 同步玩家背包
	g_ItemManager->syncPanel( pPlayer, nItemId );

	// 通知WorldServer玩家已经获得物品
	CZoneMail::GetItems( nMailId );
}

void ServerPacketProcess::Handle_World_Zone_RecvMailResponse( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	stMailHeader mailHeader;
 
	stMailItem mailItem;

	std::list<stMailItem> mailList;

	mailHeader.UnpackData( RecvPacket );

	MailEvent* pEvent = new MailEvent( mailHeader.nPage, mailHeader.nCount, mailHeader.nRecver );
	pEvent->setMailHeader( mailHeader );
	int count = 0;

	while( RecvPacket.readFlag() )
	{
		mailItem.UnpackData( RecvPacket );

		pEvent->addMailItem( mailItem );
		count++;
	}

	Player* pPlayer = g_ServerGameplayState->GetPlayer( mailHeader.nRecver );
	if( pPlayer )
	{
		GameConnection* pConnection = pPlayer->getControllingClient();
		if( pConnection )
		{
			pConnection->postNetEvent( pEvent );
		}
	}

	Con::printf( "MAIL RECV: %d", count );
}

void ServerPacketProcess::Handle_World_CopyMap_Open( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	// 建立副本管理
	ZONE_COPYMAP->getCopymap( pHead->Id );

	// SvrCopymapOpen
	

	int srcZoneId = RecvPacket.readInt( 32 );
	int srcLineId = RecvPacket.readInt( 32 );

	int playerId = pHead->SrcZoneId;

	char buf[128];
	Base::BitStream sendPacket( buf, 128 );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_COPYMAP_OpenResponse, pHead->Id, SERVICE_WORLDSERVER );

	sendPacket.writeInt( srcLineId, 32 );
	sendPacket.writeInt( srcZoneId, 32 );
	sendPacket.writeInt( playerId, 32 );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	Send( sendPacket );
}

void ServerPacketProcess::Handle_World_CopyMap_Close( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	ZONE_COPYMAP->closeCopymap( pHead->Id );
}

#include "common/DataAgentHelper.h"

void ServerPacketProcess::Handle_Zone_World_PlayerSaveResponse( stPacketHead * pHead, Base::BitStream RecvPacket )
{
	int SID = pHead->SrcZoneId;

	g_ServerGameplayState->SetPlayerSID( pHead->Id, pHead->SrcZoneId );

	stPlayerStruct* pPlayer = g_ServerGameplayState->GetPlayerSaveData( pHead->Id );

	if( pPlayer )
	{
		Con::printf("保存玩家数据【%d】- SID: %d", pHead->Id, SID );
	}
	else
	{
		Player* player = g_ServerGameplayState->GetPlayer( pHead->Id );
		if( player )
		{
			pPlayer = player->buildPlayerData();

			Con::printf("保存玩家数据【%d】- SID: %d", pHead->Id, SID );
		}
		else
		{
			Con::printf( "没有找到要保存的玩家数据[%d]", pHead->Id );
			return ;
		}
	}

	pPlayer->BaseData.SaveUID = SID;

	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );

	// here we build a fake packet head
	IPacket::BuildPacketHead( sendPacket, 0 );

	int nPos = sendPacket.getPosition();

	pPlayer->WriteData( &sendPacket );

	g_ServerGameplayState->GetDataAgentHelper()->Send( g_ServerGameplayState->getZoneId(), SERVICE_ZONESERVER, sendPacket );

	g_ServerGameplayState->RemovePlayerSaveData( pHead->Id );
}

void ServerPacketProcess::Send_World_SavePlayerRequest( int playerId )
{
	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, ZONE_WORLD_PlayerSaveRequest, playerId, SERVICE_WORLDSERVER, 0, g_ServerGameplayState->getZoneId() );

	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	Send( SendPacket );
}

void ServerPacketProcess::Send_World_PlayerStatus( int playerId )
{
	CMemGuard buf(MAX_PACKET_SIZE);
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_PlayerDispChanged, playerId, SERVICE_WORLDSERVER );
	Player* pPlayer = g_ServerGameplayState->GetPlayer( playerId );

	if( pPlayer )
	{
		stPlayerStruct* pPlayerStruct = pPlayer->buildPlayerData();
		pPlayerStruct->DispData.WriteData( &sendPacket );
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		Send( sendPacket );
	}
}

void ServerPacketProcess::HandleWorldZoneSavePlayerRequest( stPacketHead * pHead, Base::BitStream RecvPacket )
{
	int playerId = pHead->Id;

	CMemGuard buf(MAX_PACKET_SIZE);
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_ZONE_SavePlayerResponse, playerId, SERVICE_WORLDSERVER );
	Player* pPlayer = g_ServerGameplayState->GetPlayer( playerId );

	if( pPlayer )
	{
		stPlayerStruct* pPlayerStruct = pPlayer->buildPlayerData();
		pPlayerStruct->WriteData( &sendPacket );
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		Send( sendPacket );
	}
}

void ServerPacketProcess::Handle_World_CopymapData( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	T_UID UID = RecvPacket.readInt(UID_BITS);
	int dataSize = RecvPacket.readInt(Base::Bit32);	

	if(dataSize)
	{
		U32 waterMark = FrameAllocator::getWaterMark ();
		U32 maxSize = FrameAllocator::getHighWaterMark () - FrameAllocator::getWaterMark ();

		AssertFatal(maxSize>(dataSize+MAX_PACKET_SIZE),"Handle_Copymap_PlayerData Buffer Full!");

		U8 *pDataBuf = (U8 *) FrameAllocator::alloc(dataSize);
		RecvPacket.readBits(dataSize*Base::Bit8,pDataBuf);

		unsigned long UnZipBufSize = MAX_PACKET_SIZE;
		U8 *pUnZipBuf = (U8 *) FrameAllocator::alloc(UnZipBufSize);

		int iret = uncompress((Bytef *)pUnZipBuf,&UnZipBufSize,(const Bytef *)pDataBuf,dataSize);
		if (!iret)
		{
			Base::BitStream dataPacket(pUnZipBuf,UnZipBufSize);
			stPlayerStruct *pPlayerData = new stPlayerStruct;

			pPlayerData->ReadData(&dataPacket);

			Con::printf( "RECV PLAYER DATA, POS: %s, SID: %d", pPlayerData->DispData.LeavePos, pPlayerData->BaseData.SaveUID );

			stTeamInfo teamInfo;
			teamInfo.ReadPacket( &RecvPacket );

			g_ServerGameplayState->AddPlayerLoginData(UID,&teamInfo,pPlayerData);

			g_ServerGameplayState->SetPlayerSID( pPlayerData->BaseData.PlayerId, pPlayerData->BaseData.SaveUID );
		}

		FrameAllocator::setWaterMark(waterMark);
	}

	//玩家可以登录副本了
	char Buffer[30];
	Base::BitStream SendPacket(Buffer,30);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,WORLD_COPYMAP_PlayerDataResponse,pHead->Id,SERVICE_WORLDSERVER, pHead->SrcZoneId );
	SendPacket.writeInt(UID,UID_BITS);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	Send(SendPacket);
}

void ServerPacketProcess::Handle_World_Zone_MailGetMoney( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nMailId = pHead->Id;
	int nPlayerId = pHead->SrcZoneId;
	int money = pHead->LineServerId;

	// 通过邮件系统给玩家加金币
	Player* pPlayer = g_ServerGameplayState->GetPlayer( pHead->Id );
	if( !pPlayer )
		return ;

	pPlayer->addMoney( money );

	// 通知WorldServer玩家确实都到金币
	CZoneMail::GetMoney( nMailId );
}

void ServerPacketProcess::Handle_World_Zone_DeleteAllMail( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nPlayerId = pHead->Id;

	ServerGameNetEvent* pEvent = new ServerGameNetEvent( MAIL_DELETEALL );
	Player* pPlayer = g_ServerGameplayState->GetPlayer( nPlayerId );
	if( pPlayer )
	{
		GameConnection* pConn = pPlayer->getControllingClient();
		if( pConn )
		{
			pConn->postNetEvent( pEvent );
		}
	}
}

void ServerPacketProcess::Handle_World_CopyMap_OpenAck( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nCopyMapInstId = pHead->Id;
	int nPlayerId = pHead->SrcZoneId;

	int nErr = pHead->LineServerId;

	if( nErr == 0 )
	{
		// 执行副本创建响应脚本
		Con::executef("SptCopyMap_OpenAck", Con::getIntArg( nCopyMapInstId ), Con::getIntArg( nPlayerId ) );
	}
	else
	{
		Con::executef("SptCopyMap_OpenAckFail", Con::getIntArg( nPlayerId ) );
	}
}

void ServerPacketProcess::Handle_World_CopyMap_LeaveAck( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int playerId = pHead->Id;
	Player* pPlayer = g_ServerGameplayState->GetPlayer( playerId );
	if( !pPlayer )
		return ;

	stPlayerStruct* pPlayerData = g_ServerGameplayState->GetPlayerLoginData( playerId );
	if( !pPlayerData )
		return ;

	ZONE_COPYMAP->onPlayerLeave( pPlayer );

	// 重新拉回1层
	pPlayer->mCopymapInstId = 1;

	float x, y, z;
	sscanf_s( pPlayerData->DispData.LeavePos, "%g %g %g", &x, &y, &z );
	pPlayer->transportObject( pHead->SrcZoneId, 0, pPlayerData->DispData.ZoneId, x, y, z );
}

void ServerPacketProcess::Handle_Zone_World_LotRequest( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nPlayerId = pHead->Id;

	int nDestPlayerId = pHead->DestZoneId;

	Con::executef( "HandleLotReuqest", Con::getIntArg( nPlayerId ), Con::getIntArg( nDestPlayerId ) );
}

void ServerPacketProcess::Handle_Zone_World_LotNotify( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nPlayerId = pHead->Id;

	int nDestPlayerId = pHead->DestZoneId;

	Con::executef( "HandleLotAck", Con::getIntArg( nPlayerId ), Con::getIntArg( nDestPlayerId ) );
}

void ServerPacketProcess::Handle_Zone_World_AddPoint( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int error = pHead->Id;

	int playerId = RecvPacket.readInt( 32 );
	int num = RecvPacket.readInt( 32 );

	// 成功
	if( error == 0 )
	{
		Player* pPlayer = g_ServerGameplayState->GetPlayer( playerId );

		if( pPlayer )
		{
			pPlayer->mPointNum = num;
		}
	}
}