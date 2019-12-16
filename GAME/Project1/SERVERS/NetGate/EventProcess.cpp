#include "DBLib/dbLib.h"
#include "WinTCP/IPacket.h"
#include "WinTCP/dtServerSocket.h"
#include "Common/MemGuard.h"
#include "Common/WorkQueue.h"
#include "Common/PacketType.h"
#include "Common/CommonClient.h"
#include "Common/MemoryShare.h"
#include "Common/DataCheck.h"
#include "Common/ChatBase.h"
#include "Common/Script.h"

#include "NetGate.h"
#include "EventProcess.h"
#include "ServerMgr.h"
#include "PlayerMgr.h"

using namespace std;

typedef bool (*EventFunction)(int ,stPacketHead *,Base::BitStream *);
static EventFunction fnList[END_NET_MESSAGE];

void SERVER_CLASS_NAME::SendToDataAgent( WorkQueueItemStruct * pItem, stPacketHead* pHead )
{
}

int SERVER_CLASS_NAME::EventProcess(LPVOID Param)
{
	WorkQueueItemStruct *pItem = (WorkQueueItemStruct*)Param;

	switch(pItem->opCode)
	{
	case WQ_CONNECT:
		break;
	case WQ_DISCONNECT:
		{
			CLocker locker( SERVER->GetServerManager()->m_cs );
			stServerInfo* pInfo = SERVER->GetServerManager()->GetZoneInfo( pItem->Id );
			if( pInfo )
			{
				EventFn::NotifyWorldZoneLost( pInfo->ZoneId );
			}
		}
		SERVER->GetServerManager()->ReleaseServerMap(pItem->Id, true);
		break;
	case WQ_PACKET:
		{
			stPacketHead* pHead = (stPacketHead*)pItem->Buffer;
			if(!IsValidMessage(pHead->Message))
				return false;

			Base::BitStream switchPacket(pItem->Buffer,pItem->size);
			switchPacket.setPosition(pItem->size);

			//SERVER->SendToDataAgent(pItem, pHead);
			//SERVER->GetDataAgentHelper()->Send( 0, SERVICE_GATESERVER, switchPacket );

			if(pHead->DestServerType == SERVICE_CLIENT)
				EventFn::SwitchSendToClient(pHead,switchPacket);
			else if(pHead->DestServerType == SERVICE_ZONESERVER && pHead->DestZoneId!=0)
				EventFn::SwitchSendToZone(pHead,switchPacket);
			else if(pHead->DestServerType == SERVICE_WORLDSERVER)
				EventFn::SwitchSendToWorld(pHead,switchPacket);
			else if(pHead->DestServerType == SERVICE_DATAAGENT)
				EventFn::SwitchSendToDataAgent(pHead,switchPacket);
			else
			{
				if(fnList[pHead->Message])
				{
					char *pData	= (char *)(pHead) + IPacket::GetHeadSize();
					Base::BitStream RecvPacket(pData,pItem->size-IPacket::GetHeadSize());
					return fnList[pHead->Message](pItem->Id,pHead,&RecvPacket);
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
		memset(fnList,NULL,sizeof(fnList));
		fnList[COMMON_RegisterRequest]				= EventFn::HandleRegisterRequest;
		fnList[COMMON_RegisterResponse]				= EventFn::HandleRegisterResponse;

		fnList[GATE_WORLD_ClientLoginResponse]		= EventFn::HandleClientLoginResponse;
		fnList[SERVER_CHAT_SendMessageAll]			= EventFn::HandleServerChatSendMessageAll;
		fnList[SERVER_CHAT_SendMessage]				= EventFn::HandleServerChatSendMessage;
	}

	void NotifyWorldZoneLost( int ZoneId )
	{
		CMemGuard buf( 40 );
		Base::BitStream SendPacket( buf.get(), 40 );
		stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, GATE_WORLD_ZoneLost );
		pHead->DestServerType = SERVICE_WORLDSERVER;
		pHead->Id = ZoneId;
		pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetWorldSocket()->Send( SendPacket );
	}

	void SwitchSendToClient(stPacketHead *pHead,Base::BitStream &switchPacket)
	{
		int SocketId = SERVER->GetPlayerManager()->GetSocketHandle(pHead->Id);
		if(SocketId)
		{
			SERVER->GetUserSocket()->Send(SocketId,switchPacket);
		}
		else
		{
			g_Log.WriteError( "未能发现客户端[%d]", pHead->Id );
		}
	}

	void SwitchSendToZone(stPacketHead *pHead,Base::BitStream &switchPacket)
	{
		int ZoneSocketId = SERVER->GetServerManager()->GetZoneSocketId(pHead->DestZoneId);
		if(ZoneSocketId)
		{
			SERVER->GetServerSocket()->Send(ZoneSocketId,switchPacket);
		}
		else
		{
			g_Log.WriteError( "未能发现地图[%d]", pHead->DestZoneId );
		}
	}

	void SwitchSendToWorld(stPacketHead *pHead,Base::BitStream &switchPacket)
	{
		SERVER->GetWorldSocket()->Send(switchPacket);
	}

	void SwitchSendToDataAgent( stPacketHead* pHead, Base::BitStream &switchPacket)
	{
		SERVER->GetServerSocket()->Send(SERVER->GetServerManager()->GetDataAgentSocket(), switchPacket );
	}

	bool HandleClientLoginResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		int SocketId = SERVER->GetPlayerManager()->GetSocketHandle(pHead->Id);
		
		g_Log.WriteLog( "收到世界服务器登陆网关响应%d", SocketId );

		if(SocketId)
		{
			int Error = Packet->readInt(Base::Bit16);

			CMemGuard Buffer(64);
			Base::BitStream retPacket(Buffer.get(),64);
			stPacketHead *pSendHead = IPacket::BuildPacketHead(retPacket,CLIENT_GATE_LoginResponse);
			retPacket.writeInt(Error,Base::Bit16);
			pSendHead->PacketSize = retPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetUserSocket()->Send(SocketId,retPacket);
			return true;
		}

		return false;
	}

	bool HandleRegisterRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		stServerInfo ServerInfo;
		ServerInfo.SocketId			= SocketHandle;
		int ServerType = Packet->readInt(Base::Bit8);

		if( ServerType == SERVICE_DATAAGENT )
		{
			CMemGuard Buffer(64);
			Base::BitStream retPacket(Buffer.get(),64);
			stPacketHead *pSendHead = IPacket::BuildPacketHead(retPacket,COMMON_RegisterResponse);
			pSendHead->PacketSize = retPacket.getPosition() - sizeof( stPacketHead );
			SERVER->GetServerSocket()->Send( SocketHandle, retPacket );
			SERVER->GetServerManager()->SetDataAgentSocket( SocketHandle );
			return false;
		}

		if( ServerType != SERVICE_ZONESERVER)
			return false;
		Packet->readInt(Base::Bit8);	//无用id
		ServerInfo.ZoneId			= Packet->readInt(Base::Bit32);

		stZoneInfo Info;
		CMemGuard Buffer(64);
		Base::BitStream retPacket(Buffer.get(),64);
		stPacketHead *pSendHead = IPacket::BuildPacketHead(retPacket,COMMON_RegisterResponse );
		if(SERVER->GetServerManager()->GetZoneInfo(ServerInfo.ZoneId,Info))
		{
			ServerInfo.ConnectSeq		= Packet->readInt(Base::Bit32);
			if(Packet->readFlag())
			{
				ServerInfo.Ip			= Packet->readInt(Base::Bit32);
				Packet->readInt(Base::Bit16);					//ZoneServer发上来的服务端口不使用
				ServerInfo.Port			= Info.ServicePort;		//使用数据库里设置的端口来启动服务
			}
			SERVER->GetServerManager()->AddServerMap(ServerInfo);

			//反馈给地图服务器

			retPacket.writeInt(SERVER->LineId,Base::Bit16);
			retPacket.writeInt(SERVER->GateId,Base::Bit16);

			retPacket.writeInt(Info.MaxPlayerNum,Base::Bit16);
			retPacket.writeInt(Info.ServicePort	,Base::Bit16);
			pSendHead->PacketSize = retPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,retPacket);

			//发送消息到世界服务器
			Base::BitStream wldPacket(Buffer.get(),64);
			pSendHead = IPacket::BuildPacketHead(wldPacket,GATE_WORLD_ZoneRegisterRequest);
			wldPacket.writeInt(1,Base::Bit8);
			wldPacket.writeInt(ServerInfo.ZoneId,Base::Bit32);
			wldPacket.writeInt(ServerInfo.Ip,Base::Bit32);
			wldPacket.writeInt(ServerInfo.Port,Base::Bit16);
			wldPacket.writeInt(ServerInfo.ConnectSeq,Base::Bit32);
			pSendHead->PacketSize = wldPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetWorldSocket()->Send(wldPacket);
		}
		else
		{

			pSendHead->DestZoneId = -1;
			pSendHead->PacketSize = retPacket.getPosition() - sizeof( stPacketHead );
			SERVER->GetServerSocket()->Send( SocketHandle, retPacket );
			//SERVER->GetServerSocket()->PostEvent(dtServerSocket::OP_RESTART,SocketHandle);
		}

		return true;
	}

	bool HandleRegisterResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		//来自WorldServer对自己注册的反馈
		SERVER->GetServerManager()->UnpackZoneData(Packet);
		SERVER->OnServerStart();

		//发送消息到世界服务器，同步一下已有的所有地图管理
		CMemGuard Buffer(MAX_PACKET_SIZE);
		Base::BitStream wldPacket(Buffer.get(),MAX_PACKET_SIZE);
		stPacketHead *pSendHead = IPacket::BuildPacketHead(wldPacket,GATE_WORLD_ZoneRegisterRequest);
		SERVER->GetServerManager()->PackZoneMgrList(wldPacket);
		pSendHead->PacketSize = wldPacket.getPosition()-IPacket::GetHeadSize();
		SERVER->GetWorldSocket()->Send(wldPacket);
		return true;
	}

	bool HandleServerChatSendMessageAll(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		OLD_DO_LOCK( SERVER->GetPlayerManager()->m_cs );

		stChatMessage chatMessage;
		stChatMessage::UnpackMessage( chatMessage, *Packet );

		CMemGuard buf( MAX_PACKET_SIZE );
		Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
		stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, SERVER_CHAT_SendMessage );
		pSendHead->DestServerType = SERVICE_CLIENT;
		pSendHead->Id = 0;
		stChatMessage::PackMessage( chatMessage, SendPacket );
		pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

		stdext::hash_map<int,stPlayerInfo>::iterator it;
		for( it = SERVER->GetPlayerManager()->GetAllPlayers()->begin(); it != SERVER->GetPlayerManager()->GetAllPlayers()->end(); it++ )
		{
			SERVER->GetUserSocket()->Send( it->second.socketId, SendPacket );
		}

		return false;
	}

	bool HandleServerChatSendMessage(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		OLD_DO_LOCK( SERVER->GetPlayerManager()->m_cs );
		
		std::list<int> playerList;

		unsigned short nSize = (unsigned short)Packet->readInt( Base::Bit16 );
		for( int i = 0; i < nSize; i++ )
		{
			playerList.push_back( (int)Packet->readInt( Base::Bit32 ) );
		}
		stChatMessage chatMessage;
		stChatMessage::UnpackMessage( chatMessage, *Packet );

		CMemGuard buf( MAX_PACKET_SIZE );
		Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
		stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, SERVER_CHAT_SendMessage, 0, SERVICE_CLIENT );

		stChatMessage::PackMessage( chatMessage, SendPacket );
		pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

		std::list<int>::iterator it;
		int nSocket;
		for( it = playerList.begin(); it != playerList.end(); it++ )
		{
			nSocket = SERVER->GetPlayerManager()->GetSocketHandle( *it );

			SERVER->GetUserSocket()->Send( nSocket, SendPacket );
		}

		return false;
	}
}




