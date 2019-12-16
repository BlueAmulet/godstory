#ifndef ZONE_MAIL_H
#define ZONE_MAIL_H

#include "Common/PacketType.h"
#include "Common/MailBase.h"

class CZoneMail
{
public:
	static void SendMail( int nSenderId, int nRecverId, int nItemId, int nItemCount, int nMoney, const char* title, const char* szMsg, bool isSystem = 0 );
	static void RecvMail( int nRecver, int nPage, int nCount );
	static void DeleteMail( int nMailId );
	static void DeleteAllMail( int nPlayerId );
	static void ReadMail( int nMailId );
	static void GetItems( int nMailId );
	static void GetMoney( int nMailId );
	static void TryGetItems( int nMailId, int nPlayerId );
	static void TryGetMoney( int nMailId, int nPlayerId );
};

#endif