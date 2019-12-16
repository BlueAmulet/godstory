#ifndef COMMON_PACKET_H
#define COMMON_PACKET_H

#include <cstring>
#include "base/SafeString.h"
#include "WINTCP/IPacket.h"
#include "Common/MemGuard.h"
#include "Common/Packettype.h"

enum EM_MSG
{
	MSG_CONNECT = END_NET_MESSAGE,
	MSG_CONNECTED,
	MSG_AUTH,
	MSG_AUTHED,
	MSG_USER,

	MSG_END,
};

class CommonPacket : public IPacket
{
	typedef IPacket Parent;

public:
	CommonPacket()
	{
	}
	
	virtual bool HandleGamePacket(stPacketHead *pHead,int DataSize) = 0;
	virtual void HandleClientLogin() = 0;
	virtual void HandleClientLogout() = 0;

	virtual void OnConnect(int Error=0){};
	virtual void OnDisconnect();
	virtual bool HandlePacket(const char *pInData,int InSize);

	bool SendClientConnect();
	bool RegisterServer(char ServerType,char ServerLineId,int ServerId,int ConnectTimes,const char*ServiceIP=NULL,short port=0);
	bool Send(Base::BitStream &SendPacket);
};

#endif

