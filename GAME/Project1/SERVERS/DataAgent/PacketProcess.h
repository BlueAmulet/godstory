#ifndef PACKET_PROCESS_H
#define PACKET_PROCESS_H

#include "Common/CommonPacket.h"
#include "DataAgentServer.h"

class CPacketProcess 
	: public CommonPacket
{
	typedef CommonPacket Parent;

protected:
	void HandleClientLogin();
	void HandleClientLogout();
	bool HandleGamePacket(stPacketHead *pHead,int DataSize);
};

_inline void CPacketProcess::HandleClientLogin()
{
	char IP[COMMON_STRING_LENGTH]="";
	char *pAddress = dynamic_cast<dtServerSocketClient *>(m_pSocket)->GetConnectedIP();
	if(pAddress)
		sStrcpy(IP,COMMON_STRING_LENGTH,pAddress,COMMON_STRING_LENGTH);

	SERVER->GetWorkQueue()->PostEvent(m_pSocket->GetClientId(),IP,sStrlen(IP,COMMON_STRING_LENGTH)+1,true,WQ_CONNECT);
}

_inline void CPacketProcess::HandleClientLogout()
{
	SERVER->GetWorkQueue()->PostEvent(m_pSocket->GetClientId(),NULL,0,false,WQ_DISCONNECT);
}

_inline bool CPacketProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	SERVER->GetWorkQueue()->PostEvent(m_pSocket->GetClientId(),pHead,iSize,true);
	return true;
}



#endif