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
	void RequestRevere(int playerId,int destplayerId);  //请求敬仰
	void RequestSnooty(int playerId,int destplayerId);  //请求鄙视
	//void RequestLookUp(int playerId,int destplayerId);  //请求查看

	void HandleRankSeriesInfo(stPacketHead* pHead, Base::BitStream& packet);

};

extern ClientRank g_clientRank;
#endif