#ifndef USER_PACKET_PROCESS_H
#define USER_PACKET_PROCESS_H

#include "Common/CommonPacket.h"

using namespace Base;

class NetGateClientPacketProcess : public CommonPacket
{
	typedef CommonPacket Parent;

	int		m_ConnectTimes;
public:
	NetGateClientPacketProcess()
	{
		m_ConnectTimes = 0;
	}

	void OnConnect(int Error=0);
	void OnDisconnect();
protected:
	bool HandleGamePacket(stPacketHead *pHead,int iSize);
	void HandleClientLogin();
	void HandleClientLogout();

	//��Ϣ����

	//��Ϣ����

	//���ݿ����
};

_inline void NetGateClientPacketProcess::OnConnect(int Error)
{
	if(!Error)
		SendClientConnect();
}

_inline void NetGateClientPacketProcess::OnDisconnect()
{
	Parent::OnDisconnect();
	g_Log.WriteError("������������Ͽ�");
}

_inline void NetGateClientPacketProcess::HandleClientLogin()
{
	m_ConnectTimes++;

	if(m_ConnectTimes>1)
	{
		g_Log.WriteWarn("�������������������");
	}

	RegisterServer(SERVICE_DATAAGENT,1,1,m_ConnectTimes);
}

_inline void NetGateClientPacketProcess::HandleClientLogout()
{
}

_inline bool NetGateClientPacketProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	CDataAgent::getInstance()->getWorkQueue()->PostEvent(0,pHead,iSize,true);
	return true;
}


#endif