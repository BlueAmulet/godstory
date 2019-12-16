#ifndef CLIENT_RANK_H
#define CLIENT_RANK_H

#include "Base/bitStream.h"
#include "Common/PacketType.h"
#include "Common/CommonPacket.h"

class ClientRank
{
public:
	ClientRank();
	~ClientRank();

	void RequestRankSeriesInfo(int playerId,int Series,int page);
	void RequestRevere(int playerId,int destplayerId);  //������
	void RequestSnooty(int playerId,int destplayerId);  //�������
	//void RequestLookUp(int playerId,int destplayerId);  //����鿴

	void HandleRankSeriesInfo(stPacketHead* pHead, Base::BitStream& packet);

};

extern ClientRank g_clientRank;
#endif