#include "PlayerMgr.h"
#include "NetGate\NetGate.h"
#include "base/memPool.h"
#include "zlib/zlib.h"

IMPL_SERVER_MODULE_MESSAGE( CPlayerManager );

CPlayerManager::CPlayerManager()
{
	mBackWorker = new CBackWorker( 1 );
}

CPlayerManager::~CPlayerManager()
{
	SAFE_DELETE( mBackWorker );
}

void CPlayerManager::savePlayerData( stPlayerStruct* pPlayerData )
{
	OLD_DO_LOCK( m_cs );

	int nPlayerId = pPlayerData->BaseData.PlayerId;

	stPlayerRecData& data = mPlayerDataMap[nPlayerId];

	// 修正数据复制方式
	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream s( buf.get(), MAX_PACKET_SIZE );
	pPlayerData->WriteData( &s );
	s.setPosition( 0 );
	data.mPlayerData.ReadData( &s );

	//data.mPlayerData = *pPlayerData;
	//data.mPlayerData.BaseData.SaveUID++; // update the SID
}

void CPlayerManager::removePlayerData( int PlayerId, int SID )
{
	OLD_DO_LOCK( m_cs );

	PlayerDataMap::iterator it;
	it = mPlayerDataMap.find( PlayerId );

	if( it == mPlayerDataMap.end() )
		return ;
	
	// check the SID
	if( SID >= it->second.mPlayerData.BaseData.SaveUID )
		mPlayerDataMap.erase( it );
}

void CPlayerManager::doCheckData()
{
	OLD_DO_LOCK( m_cs );

	PlayerDataMap::iterator it;

	unsigned long nowTime = ::GetTickCount();

	for( it = mPlayerDataMap.begin(); it != mPlayerDataMap.end(); it++ )
	{
		if( nowTime - it->second.mLastSendTime >= 3000 ) // 大于3秒以后重新发送数据
		{
			std::stringstream s;
			s << "重新保存玩家数据[" << it->second.mPlayerData.BaseData.PlayerId << "]";
			g_Log.WriteLog( s );
			sendPlayerData( it->second.mPlayerData.BaseData.PlayerId);
		}
	}
}

void CPlayerManager::_sendPlayerData( int nPlayerID )
{	
	OLD_DO_LOCK( m_cs );

	PlayerDataMap::iterator it;

	it = mPlayerDataMap.find( nPlayerID );
	if( it == mPlayerDataMap.end() )
		return ;

	stPlayerStruct* pPlayerData = &(it->second.mPlayerData);

	CMemGuard buf( MAX_PACKET_SIZE);
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, DATAAGENT_WORLD_NotifyPlayerData, nPlayerID, SERVICE_WORLDSERVER );

	unsigned long ZipBufSize = MAX_PACKET_SIZE;

	U8 *pZipBuf = (U8 *)MEMPOOL->GetInstance()->Alloc(ZipBufSize);
	char* pDataBuf = (char*)MEMPOOL->GetInstance()->Alloc(ZipBufSize);

	Base::BitStream DataPacket( pDataBuf, MAX_PACKET_SIZE );
	pPlayerData->WriteData( &DataPacket );

	int dataLength = DataPacket.getPosition();

	int iret = compress((Bytef *)pZipBuf,&ZipBufSize,(const Bytef *)pDataBuf,dataLength);
	if (!iret)
	{
		sendPacket.writeFlag( true );

		sendPacket.writeInt(ZipBufSize,16);
		sendPacket.writeBits(ZipBufSize*Base::Bit8,pZipBuf);

		pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	}
	else
	{
		sendPacket.writeFlag( false );
	}

	MEMPOOL->GetInstance()->Free( (MemPoolEntry)pZipBuf );
	MEMPOOL->GetInstance()->Free( (MemPoolEntry)pDataBuf );

	CDataAgent::getInstance()->getNetGateSocket()->Send( sendPacket );

	it->second.mLastSendTime = ::GetTickCount();
	it->second.mSendCount++;
}

void CPlayerManager::sendPlayerData( int playerId )
{
	mBackWorker->postWork( WorkMethod( CPlayerManager::_sendPlayerData ), this, 1, playerId );
}

ServerEventFunction( CPlayerManager, HandleServerPlayerData, SERVER_DATAAGENT_Message )
{
	// 修正数据传输方式 [10/15/2009 LivenHotch]

	CMemGuard ZipBuf( MAX_PACKET_SIZE );
	CMemGuard DataBuf( MAX_PACKET_SIZE );


	if( pPack->readFlag() )
	{

		int zipSize = pPack->readInt( 16 );
		pPack->readBits( zipSize * 8, ZipBuf.get() );


		int dataSize = MAX_PACKET_SIZE;
		int ret = uncompress((Bytef*)DataBuf.get(),(uLongf*)&dataSize,(const Bytef*)ZipBuf.get(),zipSize);

		if( !ret )
		{
			Base::BitStream DataPacket( DataBuf.get(), dataSize );
			
			stPlayerStruct PlayerData;
			PlayerData.ReadData(&DataPacket);

			std::stringstream s;
			s << "收到保存玩家数据请求[" << PlayerData.BaseData.PlayerId << "]" << " POS: " << PlayerData.DispData.LeavePos << " SIZE:" << pPack->getPosition();
			g_Log.WriteLog( s );

			pThis->savePlayerData( &PlayerData );
			pThis->sendPlayerData( PlayerData.BaseData.PlayerId );
		}
	}
	return true;
}

ServerEventFunction( CPlayerManager, HandleWorldNotifyAck, DATAAGENT_WORLD_NotifyPlayerDataAck )
{
	int pPlayerId = phead->Id;
	int SID = phead->DestZoneId;

	pThis->removePlayerData( pPlayerId, SID );
	return true;
}