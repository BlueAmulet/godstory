#ifndef WORLD_CLIENT_PACKET_PROCESS_H
#define WORLD_CLIENT_PACKET_PROCESS_H

#include "Common/CommonPacket.h"

using namespace Base;

class WorldClientPacketProcess : public CommonPacket
{
	typedef CommonPacket Parent;

	int		m_ConnectTimes;
public:
	WorldClientPacketProcess()
	{
		m_ConnectTimes = 0;
	}

	void OnConnect(int Error=0);
	void OnDisconnect();
protected:
	bool HandleGamePacket(stPacketHead *pHead,int iSize);
	void HandleClientLogin();
	void HandleClientLogout();

//消息处理

//消息发送

//数据库操作
};

_inline void WorldClientPacketProcess::OnConnect(int Error)
{
	if(!Error)
		SendClientConnect();
}

_inline void WorldClientPacketProcess::OnDisconnect()
{
	Parent::OnDisconnect();
	g_Log.WriteError("与世界服务器断开");
}

_inline void WorldClientPacketProcess::HandleClientLogin()
{
	m_ConnectTimes++;

	int NetLineId = SCRIPT->get<int>("NetLineId");
	int NetGateId = SCRIPT->get<int>("NetGateId");
	const char* szIp = SCRIPT->get<CStr>("UserNetIP");
	int port = SCRIPT->get<int>("UserNetPort");
	RegisterServer(SERVICE_GATESERVER,NetLineId,NetGateId,m_ConnectTimes, szIp, port );

	if(m_ConnectTimes>1)
	{
		g_Log.WriteWarn("重新连接上世界服务器");
	}
}

_inline void WorldClientPacketProcess::HandleClientLogout()
{
}

_inline bool WorldClientPacketProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	SERVER->GetWorkQueue()->PostEvent(0,pHead,iSize,true);
	return true;
}

#endif

