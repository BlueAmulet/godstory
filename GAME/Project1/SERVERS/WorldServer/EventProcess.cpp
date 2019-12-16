
#include "DBLib/dbLib.h"
#include "zlib/zlib.h"
#include "Common/MemGuard.h"
#include "Common/WorkQueue.h"
#include "Common/PacketType.h"
#include "WinTCP/IPacket.h"
#include "WinTCP/dtServerSocket.h"
#include "TeamManager.h"
#include "ChatMgr.h"
#include "WorldServer.h"
#include "EventProcess.h"
#include "PlayerMgr.h"
#include "ServerMgr.h"
#include "SavePlayerJob.h"
#include "MailManager.h"
#include "Common/TeamBase.h"
#include "CopymapManager.h"
#include "ExportScript.h"
#include "GM/GMRecver.h"
#include "BanList.h"
#include "SocialManager.h"
#include "EventManager.h"
#include "TopManager.h"

#include "LockCommon.h"
#include "Base/Log.h"
#include "Common/DumpHelper.h"

IdAccountMap g_loginPending;

using namespace std;

typedef bool (*NetEventFunction)(int ,stPacketHead *,Base::BitStream *);
static NetEventFunction NetFNList[END_NET_MESSAGE];

typedef bool (*LogicEventFunction)(const char*,int);
static LogicEventFunction LogicFNList[END_LOGIC_MSG];

stAccountInfo* _GetAccount(int id)
{
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountInfo(id);

    if (0 == pAccount)
        pAccount = SERVER->GetPlayerManager()->GetAccountMap(id);

    return pAccount;
}

int SERVER_CLASS_NAME::EventProcess(LPVOID Param)
{
	WorkQueueItemStruct *pItem = (WorkQueueItemStruct*)Param;

	switch(pItem->opCode)
	{
	case WQ_CONNECT:
		break;
	case WQ_DISCONNECT:
		SERVER->GetServerManager()->ReleaseServerMap(pItem->Id);
		break;
	case WQ_LOGIC:
		{
			if(LogicFNList[pItem->Id])
				return LogicFNList[pItem->Id](pItem->Buffer,pItem->size);
		}
		break;
    case WQ_TIMER:
        {
			if( pItem->Id == 1 )
				SERVER->GetSavePlayerJob()->Update();
			else
				SERVER->GetPlayerManager()->Update();
        }
        break;
	case WQ_COMMAND:
		{
			const char* cmd = pItem->Buffer;
			// 执行命令行
			executeCmd( cmd );
		}
		break;
	case WQ_PACKET:
		{
			stPacketHead* pHead = (stPacketHead*)pItem->Buffer;
			char *pData	= (char *)(pHead) + IPacket::GetHeadSize();
			Base::BitStream RecvPacket(pData,pItem->size-IPacket::GetHeadSize());

			if(!IsValidMessage(pHead->Message))
				return false;

			if(NetFNList[pHead->Message])
				return NetFNList[pHead->Message](pItem->Id,pHead,&RecvPacket);
            else
            {
                DO_LOCK( Lock::Player );

                //!!!!!!!!!!!!!!!!所有游戏内的消息,pHead->Id都要指定为当前操作的玩家ID!!!!!!!!!!!!!!!!!!!
                stAccountInfo* pAccount = _GetAccount(pHead->Id);

                if (0 != pAccount)
                {

					try
					{
						pAccount->TriggerEvent(pHead->Message,pItem->Id,pHead,&RecvPacket);
					}
					catch(...)
					{
						g_Log.WriteError( "严重错误：未处理异常\n\r" + DumpHelper::DumpStack() );

						int error = 0;
						pAccount->Kick( NULL, error, false );
						pAccount-> m_status.SetState(stAccountInfo::STATUS_DELAY_DEL, PLAYER_TIMEOUT );
					}

                }
				else
				{
					SERVER->GetSocialManager()->HandleEvent( NULL, pHead->Message, pItem->Id, pHead, &RecvPacket );
					SERVER->GetTeamManager()->HandleEvent( NULL, pHead->Message, pItem->Id, pHead, &RecvPacket );
					SERVER->GetChatManager()->HandleEvent( NULL, pHead->Message, pItem->Id, pHead, &RecvPacket );
					SERVER->GetCopyMapManager()->HandleEvent( NULL, pHead->Message, pItem->Id, pHead, &RecvPacket );
					SERVER->GetMailManager()->HandleEvent( NULL,pHead->Message, pItem->Id, pHead, &RecvPacket);
					SERVER->GetEventManager()->HandleEvent(NULL,pHead->Message, pItem->Id, pHead, &RecvPacket );
					SERVER->GetTopManager()->HandleEvent(NULL,pHead->Message,pItem->Id, pHead, &RecvPacket );

					if( pHead->Message < MSG_TEAM_BEGIN )
					{
						//通知客户端被踢线
						CMemGuard buf( MAX_PACKET_SIZE );
						Base::BitStream SendPacket( buf.get(),MAX_PACKET_SIZE);

						stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket,WORLD_CLIENT_NOTIFY_KICKED,pHead->Id,SERVICE_CLIENT);
						SendPacket.writeString("您与服务器已断开连接，请重新登陆");

						pSendHead->PacketSize = SendPacket.getPosition() - sizeof(stPacketHead);
						SERVER->GetServerSocket()->Send(pItem->Id,SendPacket);
					}
				}
            }
		}
		break;
	}

	return false;
}


namespace EventFn
{
	void Initialize()
	{
		//处理网络消息
		memset(NetFNList,NULL,sizeof(NetFNList));

		NetFNList[COMMON_RegisterRequest]				= EventFn::HandleRegisterRequest;
		NetFNList[GATE_WORLD_ZoneRegisterRequest]		= EventFn::HandleZoneRegisterRequest;
		NetFNList[ACCOUNT_WORLD_ClientLoginRequest]		= EventFn::HandleClientLoginAccountRequest;

		NetFNList[GATE_WORLD_ZoneLost]					= EventFn::HandleGateZoneLost;

		NetFNList[CLIENT_WORLD_PlayerDataTransResponse] = EventFn::HandleClientPlayerDataTransRequest;
		NetFNList[DATAAGENT_WORLD_NotifyPlayerData]		= EventFn::HandleDataAgentPlayerDataNotify;

		NetFNList[ZONE_WORLD_PlayerSaveRequest]			= EventFn::HandleClientWorldSavePlayerRequest;
        NetFNList[GATE_WORLD_ClientLost]                = EventFn::HandleGateClientLost;
        NetFNList[WORLD_ZONE_TickPlayerResponse]        = EventFn::HandleTickPlayerFromZone;
        NetFNList[WORLD_ZONE_GetPlayerPosResponse]      = EventFn::HandleGetPlayerPos;
		NetFNList[ZONE_WORLD_PlayerDispChanged]			= EventFn::HandleZonePlayerDispChanged;
		NetFNList[WORLD_ZONE_SavePlayerResponse]		= EventFn::HandleWorldZoneSavePlayerResponse;

		NetFNList[CLIENT_WORLD_ItemRequest]				= EventFn::HandleClientWorldItemRequest;
		NetFNList[ZONE_WORLD_AddPoint]					= EventFn::HandleZoneAddPoint;

		NetFNList[GM_MESSAGE]                           = EventFn::HandleGMMessage;


		//处理游戏逻辑
		memset(LogicFNList,NULL,sizeof(LogicFNList));
		LogicFNList[ON_ZONE_RESTART]					= EventFn::OnZoneServerRestart;
	}

	bool HandleRegisterRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		stServerInfo ServerInfo;
		ServerInfo.SocketId			= SocketHandle;
		ServerInfo.Type				= Packet->readInt(Base::Bit8);
		ServerInfo.LineId			= Packet->readInt(Base::Bit8);
		ServerInfo.GateId			= Packet->readInt(Base::Bit32);
		int ConnectTimes			= Packet->readInt(Base::Bit32);

/*
		std::stringstream strs;
		char str[256];
		sprintf(str,"%d:%d registered!",ServerInfo.LineId, ServerInfo.GateId);
		strs<<str<<endl;
		g_Log.WriteLog( strs );*/


		int IP = 0,Port = 0;
		if(Packet->readFlag())
		{
			ServerInfo.Ip			= Packet->readInt(Base::Bit32);
			ServerInfo.Port			= Packet->readInt(Base::Bit16);

			// fix: get the ip from the remote ip address
			//sockaddr_in sock;
			//int l = sizeof( sockaddr_in );
			//getpeername( SocketHandle, (sockaddr*)&sock, &l );
			//ServerInfo.Ip = sock.sin_addr.s_addr;
			//ServerInfo.Port = sock.sin_port;
		}
		SERVER->GetServerManager()->AddServerMap(ServerInfo);

		CMemGuard Buffer(1024);
		Base::BitStream SendPacket(Buffer.get(),1024);
		stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,COMMON_RegisterResponse);
		switch(ServerInfo.Type)
		{
		case SERVICE_ACCOUNTSERVER:
			pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);
			break;
		case SERVICE_GATESERVER:
			SERVER->GetServerManager()->PackZoneData(SendPacket);
			pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);
			break;
		}

		return true;
	}

	bool HandleZoneRegisterRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		int Num = Packet->readInt(Base::Bit8);
		int ZoneId,Ip,Port,ConnectSeq;
		for(int i=0;i<Num;i++)
		{
			ZoneId = Packet->readInt(Base::Bit32);
			Ip = Packet->readInt(Base::Bit32);
			Port = Packet->readInt(Base::Bit16);
			ConnectSeq = Packet->readInt(Base::Bit32);
			SERVER->GetServerManager()->AddZoneToGate(SocketHandle,ZoneId,Ip,Port,ConnectSeq);
		}
		return true;
	}

	bool HandleClientLoginAccountRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		DO_LOCK( Lock::Player | Lock::Server );

		stAccountInfo AccountInfo;
		AccountInfo.UID				= Packet->readInt(UID_BITS);
		AccountInfo.AccountId		= Packet->readInt(Base::Bit32);
		AccountInfo.isGM			= Packet->readInt(Base::Bit32);
        AccountInfo.status          = Packet->readInt(Base::Bit32);

		AccountInfo.isAdult			= Packet->readFlag();

		AccountInfo.dTotalOnlineTime = Packet->readInt(32);
		AccountInfo.dTotalOfflineTime = Packet->readInt(32);
		AccountInfo.loginTime = Packet->readInt(32);
		AccountInfo.logoutTime = Packet->readInt(32);
		AccountInfo.PointNum = Packet->readInt(32);

		// 计算离线时间
		AccountInfo.dTotalOfflineTime += _time32( 0 ) - AccountInfo.logoutTime;

		// 离线时间满5小时
		if( AccountInfo.dTotalOfflineTime >= 5 * 60 * 60 )
		{
			// 清除累计在线时间
			AccountInfo.dTotalOnlineTime = 0;

			// 清除累计离线时间
			AccountInfo.dTotalOfflineTime = 0;
		}
        
        char ip[256];
        Packet->readString(ip,256);
        AccountInfo.loginIP         = ip;
        Packet->readString(AccountInfo.AccountName,COMMON_STRING_LENGTH);

		AccountInfo.accountSocketId = SocketHandle;
		// 此时还没有线程概念
		//AccountInfo.LineId = SERVER->GetServerManager()->GetLineId( SocketHandle );

		Packet->readBits(MD5_STRING_LENGTH*Base::Bit8,&AccountInfo.Password1);
		Packet->readBits(MD5_STRING_LENGTH*Base::Bit8,&AccountInfo.Password2);
		Packet->readBits(MD5_STRING_LENGTH*Base::Bit8,&AccountInfo.Password3);

		int error = SERVER->GetPlayerManager()->AddAccount(AccountInfo);

        if (NONE_ERROR == error)
        {
            stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountMap(AccountInfo.AccountId);

            if (0 != pAccount)
            {
                pAccount->TriggerEvent(ACCOUNT_WORLD_ClientLoginRequest,SocketHandle,pHead,Packet);
            }
        }
		else
		{
			CMemGuard Buffer(MAX_PACKET_SIZE);
			Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
			stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,ACCOUNT_WORLD_ClientLoginResponse);

			SendPacket.writeInt(AccountInfo.UID,UID_BITS);
			SendPacket.writeInt(AccountInfo.AccountId,Base::Bit32);
			SendPacket.writeInt(error,Base::Bit16);

			pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);
		}

		return true;
	}

	bool OnZoneServerRestart(const char *buffer, int size)
	{
		const int *pIntValue = (const int *)buffer;
		SERVER->GetPlayerManager()->OnZoneServerRestart(pIntValue[0],pIntValue[1]);
		return true;
	}

	bool HandleZonePlayerDispChanged(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket)
	{
		int playerId = pHead->Id;

		DO_LOCK( Lock::Player );

		PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( playerId );

		pPlayerData->DispData.ReadData( pPacket );

		char buf[256];
		sprintf_s( buf, 256, "玩家显示数据更新[%d]", playerId );
		g_Log.WriteLog( buf );
		return true;
	}

	bool HandleGateZoneLost(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket)
	{
		int nZoneId = pHead->Id;

		if( nZoneId == 0 )		
			return true;

		char str[100];
		sprintf_s(str,sizeof(str),"地图服务器[%d]已离线", nZoneId);
		g_Log.WriteError(str);

		SERVER->GetServerManager()->RemoveZoneFromGate( SocketHandle, nZoneId );

		return true;
	}

	bool HandleClientPlayerDataTransRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
	{
		int nPlayerId = pHead->DestZoneId;
		int nPlayerIdSrc = pHead->Id;

		DO_LOCK( Lock::Player );

		stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerIdSrc );
		
		if( !pAccountInfo )
			return true;

		if( !SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId ) )
		{
			SERVER->GetPlayerManager()->SendSimplePacket( nPlayerIdSrc, CLIENT_WORLD_PlayerDataTransResponse, nPlayerId, 0 );
			return true;
		}

		CMemGuard buf( MAX_PACKET_SIZE );
		Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
		stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_WORLD_PlayerDataTransResponse, SERVICE_CLIENT, pAccountInfo->AccountId, pHead->SrcZoneId );
		
		int nType = pHead->SrcZoneId;

		pSendHead->DestZoneId = nPlayerId;
		pSendHead->SrcZoneId = nType;

		if( nType & ( 1 << PLAYER_TRANS_TYPE_BASE ) )
			SERVER->GetPlayerManager()->PackPlayerTransDataBase( nPlayerId, SendPacket );

		if( nType & ( 1 << PLAYER_TRANS_TYPE_NORMAL) )
			SERVER->GetPlayerManager()->PackPlayerTransDataNormal( nPlayerId, SendPacket );

		pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
		
		SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );

		return true;
	}

	bool HandleDataAgentPlayerDataNotify( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
	{
		unsigned long UID = pHead->Id;

		if( Packet->readFlag() )
		{
			int DataSize = Packet->readInt( 16 );
	
			char* pBufBuf = (char*)MEMPOOL->GetInstance()->Alloc( MAX_PACKET_SIZE );
			char* pDataBuf = (char*)MEMPOOL->GetInstance()->Alloc( MAX_PACKET_SIZE );

			Packet->readBits( DataSize * 8, pBufBuf );

			int dataSize = MAX_PACKET_SIZE;
			int ret = uncompress((Bytef*)pDataBuf,(uLongf*)&dataSize,(const Bytef*)pBufBuf,DataSize);

			if( !ret )
			{
				Base::BitStream DataPacket(pDataBuf, dataSize );

				stPlayerStruct player;
				player.ReadData( &DataPacket );

				//printf("Incoming player data from DataAgent ... ACCOUNT[%d]-PLAYER[%d]\n\r", player.BaseData.AccountId, player.BaseData.PlayerId );
				//printf("%s\r\n", player.DispData.LeavePos );
				SERVER->GetSavePlayerJob()->SetDataAgentSocketHandle( player.BaseData.PlayerId, SocketHandle );
				SERVER->GetSavePlayerJob()->PostPlayerData( UID, &player );
			}
			else
			{
				char buf[256];
				sprintf_s( buf, 256, "DATAAGENT： 解压缩玩家数据错误！！" );
				g_Log.WriteError(buf);
			}

			MEMPOOL->GetInstance()->Free( (MemPoolEntry)pBufBuf );
			MEMPOOL->GetInstance()->Free( (MemPoolEntry)pDataBuf );
		}

		return true;
	}

    bool HandleGateClientLost( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
    {
		DO_LOCK( Lock::Player );

		SendSavePlayerRequest( pHead->Id );

        stAccountInfo* pAccount = _GetAccount(pHead->Id);

        if (0 == pAccount)
            return true;

        T_UID UID = Packet->readInt(UID_BITS);

        if (pAccount->UID != UID)
            return true;

        pAccount->TriggerEvent(GATE_WORLD_ClientLost,SocketHandle,pHead,Packet);

        return true;
    }
    
    bool HandleTickPlayerFromZone( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
    {
		DO_LOCK( Lock::Player );

        stPlayerStruct player;

        if(Packet->readFlag())
        {
            player.ReadData(Packet);
			char buf[256];
			sprintf_s( buf, 256, "踢人数据返回[%d]", player.BaseData.PlayerId );
			g_Log.WriteLog( buf );
            SERVER->GetPlayerManager()->SavePlayerData(&player);
        }

        stAccountInfo* pAccount = _GetAccount(pHead->Id);

        if (0 == pAccount)
            return true;

        pAccount->TriggerEvent(WORLD_ZONE_TickPlayerResponse,SocketHandle,pHead,Packet);
        return true;
    }

	bool HandleWorldZoneSavePlayerResponse( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
	{

		stPlayerStruct playerStruct;

		playerStruct.ReadData( Packet );

		SERVER->GetPlayerManager()->SavePlayerData( &playerStruct );

		return true;
	}

    bool HandleGetPlayerPos( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
    {
        DO_LOCK( Lock::Player );

        PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData(pHead->Id);

        if (pPlayerData.isNull())
            return true;
        
        return true;
    }

    bool HandleGMMessage(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket)
    {   
        int accountId = pHead->Id;

		DO_LOCK( Lock::Player );

        stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountMap(accountId);

        if(!pAccountInfo)
            return false;

        //检测是否是gm
        if (!pAccountInfo->isGM)
            return false;

        //读取GM命令
        static char buffer[4096];
        pPacket->readLongString(sizeof(buffer),buffer);

        bool hasPack = pPacket->readFlag();
    
        return CGMRecver::Instance()->HandleCommand(pHead->DestZoneId,SocketHandle,accountId,pAccountInfo->isGM,buffer,hasPack ? pPacket : 0);
    }

	bool HandleClientWorldSavePlayerRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
	{
		int playerId = pHead->Id;

		int SID = SERVER->GetPlayerManager()->updatePlayerSID( playerId );

		CMemGuard buf( MAX_PACKET_SIZE );
		Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_PlayerSaveResponse, playerId, SERVICE_ZONESERVER, pHead->SrcZoneId, SID );
		
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
		return true;
	}

	void SendSavePlayerRequest( int accountId )
	{
		DO_LOCK( Lock::Player );

		stAccountInfo* pInfo = SERVER->GetPlayerManager()->GetAccountMap( accountId );
		if( !pInfo )
			return ;

		int playerId = SERVER->GetPlayerManager()->GetSelectedPlayer( accountId );

		PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData(playerId);
		if( pPlayer.isNull() )
			return ;

		CMemGuard buf( MAX_PACKET_SIZE );
		Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_ZONE_SavePlayerRequest, playerId, SERVICE_ZONESERVER, pPlayer->DispData.ZoneId );

		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( pInfo->socketId, sendPacket );
	}

	// 处理玩家向另一个玩家索取物品
	bool HandleClientWorldItemRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
	{
		PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( pHead->Id );
		if( pPlayer.isNull() )
			return true;

		PlayerDataRef pDestPlayer = SERVER->GetPlayerManager()->GetPlayerData( pHead->DestZoneId );
		if( pDestPlayer.isNull() )
			return true;

		int itemId = Packet->readInt( 32 );
		int itemCount = Packet->readInt( 32 );

		char name[COMMON_STRING_LENGTH];

		Packet->readString( name ,COMMON_STRING_LENGTH);

		CMemGuard buf( MAX_PACKET_SIZE );
		Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );

		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_ItemRequest, pDestPlayer->BaseData.AccountId, SERVICE_CLIENT, 0, pHead->Id );

		sendPacket.writeInt( itemId, 32 );
		sendPacket.writeInt( itemCount, 32 );

		sendPacket.writeString( name );

		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

		SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );

		return true;
	}

	bool HandleZoneAddPoint( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
	{
		DO_LOCK( Lock::Player );

		int srcZoneId = pHead->SrcZoneId;

		int num = pHead->DestZoneId;

		int playerId = pHead->Id;

		PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( playerId );
		if( pPlayer.isNull() )
			return true;

		CDBOperator db(DATABASE_ACCOUNT);

		db->SQL( "EXECUTE AddPoint %d, %d", num, pPlayer->BaseData.AccountId );
		if( db->More() )
		{
			int ret = db->GetInt();

			int num = 0;
			
			if( ret == 0 )
				num = db->GetInt();
			
			CMemGuard buf( MAX_PACKET_SIZE );
			Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );

			stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_AddPoint, ret, SERVICE_CLIENT, srcZoneId );
			sendPacket.writeInt( playerId, 32 );
			sendPacket.writeInt( num, 32 );

			SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
		}

		return true;
	}
}





