#include "CommonClient.h"

IMPLEMENT_CONOBJECT(TCPObject2);

CommonClient::CommonClient()
{
	IPacket *pPacket = NULL;
#ifdef NTJ_SERVER
	pPacket = dynamic_cast<IPacket *>(new ServerPacketProcess);
#else
	pPacket = dynamic_cast<IPacket *>(new UserPacketProcess);
#endif

	pPacket->BindSocket(this);
	BindPacket(pPacket);

	SetMaxReceiveBufferSize(1024*10);
}

void CommonClient::onConnected()
{
	(dynamic_cast<CommonPacket *>(m_pPacket))->SendClientConnect();

	Parent::onConnected();
}

void CommonClient::onDisconnect()
{
	(dynamic_cast<CommonPacket *>(m_pPacket))->OnDisconnect();
	Parent::onDisconnect();
}

U32 CommonClient::onReceive(U8 *buffer, U32 bufferLen)
{
	CTcpSocket::OnReceive((const char *)buffer,(int)bufferLen);
	return bufferLen;
}

//CTcpSocket改变接口
bool CommonClient::Send(const char *buffer,int bufferLen)
{
	TCPObject::send((U8 *)buffer,(U32)bufferLen);
	return true;
}

bool CommonClient::Disconnect(bool)
{
	TCPObject::disconnect();
	return true;
}

ConsoleMethod( TCPObject2, connect, void, 3, 3, "connect(IP:PORT)")
{
	return object->connect(argv[2]);
}

