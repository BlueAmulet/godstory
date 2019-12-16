#ifndef MAIL_MANAGER_H
#define MAIL_MANAGER_H


struct stPacketHead;

#include "base/bitStream.h"
#include "common/PacketType.h"
#include "LockCommon.h"
#include "ManagerBase.h"

class stAccountInfo;

class CMailManager : public ILockable,  public CManagerBase< CMailManager, MSG_MAIL_BEGIN, MSG_MAIL_END >
{
public:
	CMailManager();
	virtual ~CMailManager();

	void HandleZoneMailRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet );
	void HandleZoneSendMailRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet );

	void DeleteAllMail(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket);
	void DeleteMail(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket);
	void MailReaded(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket);
	void GetMailItems(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket);
	void GetMailMoney(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket);
	void HandleTryGetItems(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket);
	void HandleTryGetMoney(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket);

private:

	void SendZoneMailResponse();
};


#endif