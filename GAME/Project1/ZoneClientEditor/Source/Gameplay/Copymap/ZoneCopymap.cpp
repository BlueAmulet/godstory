#include "ZoneCopymap.h"
#include "NetWork\ServerPacketProcess.h"
#include "..\ServerGameplayState.h"
#include "zlib/zlib.h"

CZoneCopymapMgr g_ZoneCopymapMgr;

CZoneCopymap::CZoneCopymap()
{
	
}

CZoneCopymap::~CZoneCopymap()
{

}

//---------------------------------------------------------------
// 给特定副本制定一个ID
//---------------------------------------------------------------
void CZoneCopymap::assignId( int copymapInstId )
{
	nCopymapInstId = copymapInstId;
}

//---------------------------------------------------------------
// 取得 ID
//---------------------------------------------------------------
int CZoneCopymap::getId()
{
	return nCopymapInstId;
}

//---------------------------------------------------------------
// 添加玩家
//---------------------------------------------------------------
void CZoneCopymap::addPlayer( Player* pPlayer )
{
	if( !pPlayer )
		return ;

	mPlayerMap[pPlayer->getPlayerID()] = pPlayer;
}

//---------------------------------------------------------------
// 移除玩家
//---------------------------------------------------------------
void CZoneCopymap::removePlayer( Player* pPlayer )
{
	if( !pPlayer )
		return ;

	mPlayerMap.erase( pPlayer->getPlayerID() );
}

//---------------------------------------------------------------
// 开始副本
//---------------------------------------------------------------
void CZoneCopymap::start()
{

}

//---------------------------------------------------------------
// 结束副本
//---------------------------------------------------------------
void CZoneCopymap::close()
{

}

CZoneCopymapMgr::CZoneCopymapMgr()
{

}

CZoneCopymapMgr::~CZoneCopymapMgr()
{

}

//---------------------------------------------------------------
// 获得一个制定ID的副本对象
//---------------------------------------------------------------
CZoneCopymap* CZoneCopymapMgr::getCopymap( int nCopymapInstId )
{
	CopymapMap::iterator it;
	
	it = mCopymapMap.find( nCopymapInstId );
	if( it == mCopymapMap.end() )
	{
		CZoneCopymap* pCopymap = new CZoneCopymap();
		pCopymap->assignId( nCopymapInstId );

		Con::executef( "SvrCopymapOpen", Con::getIntArg( nCopymapInstId ), Con::getIntArg( g_ServerGameplayState->getZoneId() ) );

		mCopymapMap[nCopymapInstId] = pCopymap;
		return pCopymap;
	}
	else
		return it->second;
}

//---------------------------------------------------------------
// 关闭一个副本实例
//---------------------------------------------------------------
void CZoneCopymapMgr::closeCopymap( int nCopymapInstId )
{
	CopymapMap::iterator it;

	it = mCopymapMap.find( nCopymapInstId );
	if( it == mCopymapMap.end() )
		return ;

	delete it->second;

	mCopymapMap.erase( it );
}

void CZoneCopymapMgr::onPlayerLeave( Player* pPlayer )
{
	if( !pPlayer )
		return ;

	int nCopyMapInstId = mPlayerCopymapMap[pPlayer->getPlayerID()];

	getCopymap( nCopyMapInstId )->removePlayer( pPlayer );

	mPlayerCopymapMap.erase( pPlayer->getPlayerID() );
}

void CZoneCopymapMgr::onPlayerEnter( Player* pPlayer )
{
	if( !pPlayer )
		return ;

	CZoneCopymap* pCopyMap = getCopymap( pPlayer->getLayerID() );

	mPlayerCopymapMap[pPlayer->getPlayerID()] = pCopyMap->getId();
}
// 开启一个副本
ConsoleFunction( SptCopyMap_Open, void, 3, 3, "" )
{
	int copyMapId = atoi( argv[1] );
	int playerId = atoi( argv[2] );


	char buf[128];
	Base::BitStream sendPacket( buf, 128 );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_COPYMAP_OpenRequest, playerId, SERVICE_WORLDSERVER, copyMapId, g_ServerGameplayState->getZoneId() );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}

// 将玩家传送到副本内
ConsoleFunction( SptCopyMap_TransportPlayer, void, 5, 5, "" )
{
	int playerId = atoi( argv[1] );
	int nCopyMapInstId = atoi( argv[2] );

	Player* pPlayer = g_ServerGameplayState->GetPlayer( playerId );
	if( !pPlayer )
		return ;

	float x, y, z;
	sscanf( argv[3], "%g %g %g", &x, &y, &z );

	char buf[COMMON_STRING_LENGTH];
	sprintf_s( buf, sizeof( buf ), "%.2f %.2f %.2f", x, y, z );
	pPlayer->mCopyMapPosition = buf;

	char buf1[MAX_PACKET_SIZE];
	Base::BitStream SendPacket( buf1, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_COPYMAP_EnterRequest, playerId, SERVICE_WORLDSERVER, nCopyMapInstId );
	
	SendPacket.writeInt(pPlayer->getUID(),UID_BITS);
	SendPacket.writeInt(g_ServerGameplayState->getCurrentLineId(),16);
	SendPacket.writeInt(atoi( argv[4] ),32);
	SendPacket.writeInt( g_ServerGameplayState->getZoneId(),16);
	SendPacket.writeSignedInt(x*100,32);
	SendPacket.writeSignedInt(y*100,32);
	SendPacket.writeSignedInt(z*100,32); 

	U8 *pDataBuf = (U8 *) FrameAllocator::alloc(MAX_PACKET_SIZE);
	Base::BitStream DataPacket(pDataBuf,MAX_PACKET_SIZE);

	pPlayer->mCopymapInstId = nCopyMapInstId;
	stPlayerStruct* pPlayerData = pPlayer->buildPlayerData();

	pPlayerData->WriteData(&DataPacket);
	pPlayerData->DispData.ZoneId = g_ServerGameplayState->getZoneId();

	// 传送玩家队伍信息
	// [LivenHotch]: 由于WorldServer是有玩家的队伍信息的,所以这里可以不用传送
	if( pPlayer )
		pPlayer->getTeamInfo().WritePacket( &DataPacket );

	int dataSize = DataPacket.getPosition();

	unsigned long ZipBufSize = MAX_PACKET_SIZE;
	FrameAllocatorMarker maker;
	U8 *pZipBuf = (U8 *) maker.alloc(ZipBufSize);

	bool Ret = false;
	int iret = compress((Bytef *)pZipBuf,&ZipBufSize,(const Bytef *)pDataBuf,dataSize);
	if (!iret)
	{
		SendPacket.writeInt(ZipBufSize,16);
		SendPacket.writeBits(ZipBufSize*Base::Bit8,pZipBuf);

		pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

		ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
		if( pProcess )
			pProcess->Send( SendPacket );
	}

}

// 将玩家移除副本地图
ConsoleFunction( SptCopyMap_RemovePlayer, void, 2, 2, "" )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1] ) );
	if( !pPlayer )
		return ;

	char buf[128];
	Base::BitStream sendPacket( buf, 128 );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_COPYMAP_LeaveRequest, pPlayer->getPlayerID(), SERVICE_WORLDSERVER, 0, g_ServerGameplayState->getZoneId() );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}

// 关闭副本
ConsoleFunction( SptCopymap_Close, void, 2, 2, "" )
{
	int copymapInstId = atoi( argv[1] );

	char buf[128];
	Base::BitStream sendPacket( buf, 128 );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_COPYMAP_CloseRequest, copymapInstId, SERVICE_WORLDSERVER, 0, g_ServerGameplayState->getZoneId() );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}

// 邀请队伍玩家进入副本
ConsoleFunction( SptCopyMap_InvationTeammate, void, 4, 4, "SptCopyMap_InvationTeammate( %playerId, %copymapId, %copymapInstId )" )
{
	int playerId = atoi( argv[1] );
	int copymapId = atoi( argv[2] );
	int copymapInstId = atoi( argv[3] );

	char buf[128];
	Base::BitStream sendPacket( buf, 128 );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_COPYMAP_LeaveRequest, playerId, SERVICE_WORLDSERVER, 0, g_ServerGameplayState->getZoneId() );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}