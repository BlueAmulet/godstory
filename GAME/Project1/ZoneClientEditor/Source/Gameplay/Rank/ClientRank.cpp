#include "Gameplay/ClientGameplayState.h"
#include "Network/UserPacketProcess.h"
#include "ClientRank.h"
#include "Network/EventCommon.h"
#include "../GameEvents/ClientGameNetEvents.h"

ClientRank g_clientRank;

ClientRank::ClientRank()
{
	gEventMethod[Client_WORLD_RequestRankSeriesInfo] = &ClientRank::HandleRankSeriesInfo;
}

ClientRank::~ClientRank()
{

}

void ClientRank::RequestRankSeriesInfo(int playerId, int Series, int page)
{
	char Buf[MAX_PACKET_SIZE];
	Base::BitStream packet(Buf,MAX_PACKET_SIZE);
	stPacketHead* pHead = IPacket::BuildPacketHead(packet,Client_WORLD_RequestRankSeriesInfo,playerId,SERVICE_WORLDSERVER,Series,page);
	pHead->PacketSize = packet.getPosition() - sizeof(stPacketHead);

	g_ClientGameplayState->GetPacketProcess()->Send(packet);
}

void ClientRank::RequestRevere(int playerId, int destplayerId)
{
/*
	char Buf[MAX_PACKET_SIZE];
	Base::BitStream packet(Buf,MAX_PACKET_SIZE);
	stPacketHead* pHead = IPacket::BuildPacketHead(packet,Client_WORLD_RequestRevere,playerId,SERVICE_WORLDSERVER,destplayerId);
	pHead->PacketSize = packet->getPosition() - sizeof(stPacketHead);

	g_ClientGameplayState->GetPacketProcess()->Send(packet);*/

}


void ClientRank::RequestSnooty(int playerId, int destplayerId)
{
/*
	char Buf[MAX_PACKET_SIZE];
	Base::BitStream packet(Buf,MAX_PACKET_SIZE);
	stPacketHead* pHead = IPacket::BuildPacketHead(packet,Client_WORLD_RequestSnooty,playerId,SERVICE_WORLDSERVER,destplayerId);
	pHead->PacketSize = packet->getPosition() - sizeof(stPacketHead);

	g_ClientGameplayState->GetPacketProcess()->Send(packet);*/

}

void ClientRank::HandleRankSeriesInfo(stPacketHead *pHead, Base::BitStream &packet)
{
	int nPage = pHead->DestZoneId;
	int nType = pHead->SrcZoneId;

	//packet.writeInt( players[i].playerId, 32 );
	//packet.writeInt( players[i].value, 32 );
	//packet.writeInt( players[i].honour, 32 );
	//packet.writeInt( players[i].indexValue, 16 );
	//packet.writeString( players[i].name.c_str() );
	//pPlayer = SERVER->GetPlayerManager()->GetPlayerData( players[i].playerId );
	//packet.writeFlag( pPlayer != NULL ); // 玩家是否在线

	char buf[256];
	int nCount = packet.readInt( 16 );
	for( int i = 0; i < nCount; i++ )
	{
		int playerId = packet.readInt( 32 );
		int value = packet.readInt( 32 );
		int honour = packet.readInt( 32 );
		int indexValue = packet.readSignedInt( 16 );
		packet.readString( buf ,256);
		bool isOnline = packet.readFlag();

		// here we insert the data into the UI table
		Con::evaluatef("RecvRankSeriesListInfo(%d,%d,\"%s\",%d,%d,%d,%d);",i,indexValue,StringTable->insert( buf ),value,isOnline,honour,playerId);
	}

	int myIndex = packet.readSignedInt( 16 );

	Con::evaluatef("GetMyselfSeatInRank(%d);",myIndex);
}

ConsoleFunction(SptRequestRevere,void, 2, 2, "SptRequestRevere(%playerId)")
{
	g_clientRank.RequestRevere(g_ClientGameplayState->getSelectedPlayerId(),atoi(argv[1]));
}

ConsoleFunction(SptRequestSnooty,void, 2, 2, "SptRequestSnooty(%playerId)")
{
	g_clientRank.RequestSnooty(g_ClientGameplayState->getSelectedPlayerId(),atoi(argv[1]));
}

ConsoleFunction(SptRequestRankSeriesInfo,void, 3, 3, "SptRequestRankSeriesInfo()")
{
	g_clientRank.RequestRankSeriesInfo(g_ClientGameplayState->getSelectedPlayerId(),atoi(argv[1]),atoi(argv[2]));
}