#include "CommonPacket.h"

#ifdef TCP_SERVER
#include "Common/CommonServerClient.h"
#else
#include "wintcp/ISocket.h"
#endif

void CommonPacket::OnDisconnect()
{
	HandleClientLogout();
}

bool CommonPacket::Send(Base::BitStream &SendPacket)
{
	return Parent::Send(reinterpret_cast<char*>(SendPacket.getBuffer()),SendPacket.getPosition());
}

bool CommonPacket::SendClientConnect()
{
	CMemGuard Buffer(64);
	Base::BitStream SendPacket(Buffer.get(),64);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,MSG_CONNECT);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

bool CommonPacket::RegisterServer(char ServerType,char ServerLineId,int ServerId,int ConnectTimes,const char*ServiceIP,short Port)
{
	CMemGuard Buffer(64);
	Base::BitStream SendPacket(Buffer.get(),64);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,COMMON_RegisterRequest);
	SendPacket.writeInt(ServerType	,Base::Bit8);
	SendPacket.writeInt(ServerLineId,Base::Bit8);
	SendPacket.writeInt(ServerId	,Base::Bit32);
	SendPacket.writeInt(ConnectTimes,Base::Bit32);

	if(SendPacket.writeFlag(ServiceIP!=NULL))
	{
		unsigned long ip = inet_addr(ServiceIP);
		SendPacket.writeInt(ip		,Base::Bit32);
		SendPacket.writeInt(htons(Port)	,Base::Bit16);
	}

	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

bool CommonPacket::HandlePacket(const char *pInData,int InSize)
{
	const stPacketHead *pHead = reinterpret_cast<const stPacketHead *>(pInData);
	const char *pData	= pInData + GetHeadSize();

	switch(pHead->Message)
	{
	case MSG_CONNECT:
		{
			CMemGuard Buffer(64);
			Base::BitStream SendPacket(Buffer.get(),64);
			stPacketHead *pSendHead = BuildPacketHead(SendPacket,MSG_CONNECTED);

			SendPacket.writeInt(m_pSocket->GetClientId(),Base::Bit32);
			pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
			Send(SendPacket);
		}
		return true;
	case MSG_AUTH:
		{
			Base::BitStream RecvPacket(const_cast<char *>(pData),InSize-GetHeadSize());
			int sequence;
			sequence = RecvPacket.readInt(Base::Bit32);
#ifdef TCP_SERVER
			if(sequence!=m_pSocket->GetClientId())
			{
				dynamic_cast<dtServerSocketClient *>(m_pSocket)->PostEvent(dtServerSocket::OP_RESTART);
				return true;
			}
#endif
			if(RecvPacket.readFlag())
				m_pSocket->SetConnectType(ISocket::SERVER_CONNECT);
			else
				m_pSocket->SetConnectType(ISocket::CLIENT_CONNECT);
			
			HandleClientLogin();

			CMemGuard Buffer(64);
			Base::BitStream SendPacket(Buffer.get(),64);
			stPacketHead *pSendHead = BuildPacketHead(SendPacket,MSG_AUTHED);
			pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
			Send(SendPacket);
		}
		return true;
	case MSG_CONNECTED:
		{
			Base::BitStream RecvPacket(const_cast<char *>(pData),InSize-GetHeadSize());
			m_pSocket->SetClientId(RecvPacket.readInt(Base::Bit32));

			CMemGuard Buffer(64);
			Base::BitStream SendPacket(Buffer.get(),64);
			stPacketHead *pSendHead = BuildPacketHead(SendPacket,MSG_AUTH);
			SendPacket.writeInt(m_pSocket->GetClientId(),Base::Bit32);
			SendPacket.writeFlag(m_pSocket->GetConnectType()==ISocket::SERVER_CONNECT);
			pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
			Send(SendPacket);
		}
		return true;
	case MSG_AUTHED:
		{
			HandleClientLogin();
		}
		return true;
	default:
#ifdef TCP_SERVER
		if(!m_pSocket->HasClientId())
		{
			dtServerSocketClient *pSocket = dynamic_cast<dtServerSocketClient *>(m_pSocket);
			if(pSocket)
				pSocket->PostEvent(dtServerSocket::OP_RESTART);

			return true;
		}
#endif
		return HandleGamePacket(const_cast<stPacketHead*>(pHead),InSize);
	}

	return false;
}


