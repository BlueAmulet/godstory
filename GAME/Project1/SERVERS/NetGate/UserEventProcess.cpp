#include "DBLib/dbLib.h"
#include "WinTCP/IPacket.h"
#include "WinTCP/dtServerSocket.h"
#include "Common/MemGuard.h"
#include "Common/WorkQueue.h"
#include "Common/PacketType.h"
#include "Common/CommonClient.h"

#include "NetGate.h"
#include "UserEventProcess.h"
#include "PlayerMgr.h"

using namespace std;

typedef bool (*EventFunction)(int ,stPacketHead *,Base::BitStream *);
static EventFunction fnList[END_NET_MESSAGE];

int SERVER_CLASS_NAME::UserEventProcess(LPVOID Param)
{
	WorkQueueItemStruct *pItem = (WorkQueueItemStruct*)Param;

	int nId;

	switch(pItem->opCode)
	{
	case WQ_CONNECT:
		//SERVER->GetPlayerManager()->AddSocketMap(pItem->Id,pItem->Buffer);
		break;
	case WQ_DISCONNECT:
        {
            // 此处通知World该客户端已断开
            T_UID uid = 0;
            nId = SERVER->GetPlayerManager()->GetPlayerIdBySocket( pItem->Id,uid);

            if( nId )
            {
                UserEventFn::NotifyWorldClientLost( nId,uid);
            }

            // 相关清理
            SERVER->GetPlayerManager()->ReleaseSocketMap(pItem->Id);
        }
		break;
	case WQ_PACKET:
		{
			stPacketHead* pHead = (stPacketHead*)pItem->Buffer;
			if(!IsValidMessage(pHead->Message))
				return false;

			Base::BitStream switchPacket(pItem->Buffer,pItem->size);
			switchPacket.setPosition(pItem->size);

			if(pHead->DestServerType == SERVICE_WORLDSERVER)
				UserEventFn::SwitchSendToWorld(pHead,switchPacket);
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

#include <assert.h>

namespace UserEventFn
{
	void Initialize()
	{
		memset(fnList,NULL,sizeof(fnList));

		fnList[CLIENT_GATE_LoginRequest]			= UserEventFn::HandleClientLoginRequest;
	}

	void SwitchSendToWorld(stPacketHead *pHead,Base::BitStream &switchPacket)
	{
		SERVER->GetWorldSocket()->Send(switchPacket);
	}

	void NotifyWorldClientLost( int nPlayerId,T_UID uid)
	{
		CMemGuard buf( 40 );
		Base::BitStream SendPacket( buf.get(), 40 );
		stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, GATE_WORLD_ClientLost );
		pHead->DestServerType = SERVICE_WORLDSERVER;
		pHead->Id = nPlayerId;
        SendPacket.writeInt(uid,Base::Bit32);
		pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetWorldSocket()->Send( SendPacket );
	}

	bool HandleClientLoginRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		short Error = NONE_ERROR;

		int AccountId	= pHead->Id;
		int NetVersion = Packet->readInt(Base::Bit32);
		T_UID UID		= Packet->readInt(UID_BITS);

		if(NetVersion != NETWORK_PROTOCOL_VERSION)
			Error = VERSION_ERROR;

		if(Error == NONE_ERROR)
		{
			Error = SERVER->GetPlayerManager()->AddPlayerMap(SocketHandle,UID,AccountId);
			if(Error == NONE_ERROR)
			{
				CMemGuard Buffer(64);
				Base::BitStream SendPacket(Buffer.get(),64);
				stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,GATE_WORLD_ClientLoginRequest,AccountId);
				SendPacket.writeInt(UID,UID_BITS);
				pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
				SERVER->GetWorldSocket()->Send(SendPacket);
				return true;
			}
		}
		
		//assert(0);
		
		CMemGuard Buffer(64);
		Base::BitStream SendPacket(Buffer.get(),64);
		stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_GATE_LoginResponse);
		SendPacket.writeInt(Error,Base::Bit16);
		pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
		SERVER->GetUserSocket()->Send(SocketHandle,SendPacket);
		return true;
	}
}




