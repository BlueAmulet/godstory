#ifndef SERVER_PACKET_PROCESS_H
#define SERVER_PACKET_PROCESS_H

#include <string>
#include "base/bitStream.h"
#include "Common/CommonPacket.h"

struct stPlayerStruct;

class ServerPacketProcess : public CommonPacket
{
	typedef CommonPacket Parent;

	int		m_ConnSequence;
public:
	ServerPacketProcess()
	{
		m_ConnSequence = 0;
	}

protected:
	void HandleClientLogin();
	void HandleClientLogout();
	bool HandleGamePacket(stPacketHead *pHead,int DataSize);

public:
	void Handle_Gate_RegisterResponse(stPacketHead* pHead, Base::BitStream &RecvPacket);
	void Handle_World_PlayerData(stPacketHead *pHead,Base::BitStream &RecvPacket);
	void Handle_World_CopymapData(stPacketHead *pHead,Base::BitStream &RecvPacket);
	
	void Handle_World_SwitchMapRequest(stPacketHead *pHead,Base::BitStream &RecvPacket);
	void Handle_World_SwitchMapResponse(Base::BitStream &RecvPacket);

	void Handle_World_TickPlayerRequest( stPacketHead* pHead, Base::BitStream &RecvPacket);
    void Handle_World_TransportPlayerRequest(stPacketHead* pHead, Base::BitStream &RecvPacket);
    void Handle_World_GetPlayerPosRequest(stPacketHead* pHead, Base::BitStream &RecvPacket);
public:
	bool Send_World_TransportPlayerRequest(T_UID,stPlayerStruct *,int LineId,int TriggerId,int MapId,F32 x,F32 y,F32 z);
	bool Send_World_SavaPlayerDataRequest(stPlayerStruct *);

	void Send_World_SavePlayerRequest( int playerId );

	void Send_World_TransportPlayerResponse(stPacketHead *pHead,T_UID UID,int AccountId,int PlayerId,int ErrorCode,int LineId,int ZondId);

public:
	void Handle_World_Zone_TeamInfo( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_World_Zone_TeamPlayerJoin( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_World_Zone_TeamPlayerLeave( stPacketHead *pHead,Base::BitStream &RecvPacket );

public:
	void Handle_World_Zone_RecvMailResponse( stPacketHead *pHead,Base::BitStream &RecvPacket );

public:
	void Handle_World_CopyMap_Open( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_World_CopyMap_OpenAck( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_World_CopyMap_LeaveAck( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_World_CopyMap_Close( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_Zone_World_PlayerSaveResponse( stPacketHead * pHead, Base::BitStream RecvPacket );
	void Send_World_PlayerStatus( int playerId );
	void HandleWorldZoneSavePlayerRequest( stPacketHead * pHead, Base::BitStream RecvPacket );
	void Handle_World_Zone_MailGetItems( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_World_Zone_MailGetMoney( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_World_Zone_DeleteAllMail( stPacketHead *pHead,Base::BitStream &RecvPacket );

	void Handle_Zone_World_LotRequest( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_Zone_World_AddPoint( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_Zone_World_LotNotify( stPacketHead *pHead,Base::BitStream &RecvPacket );
};

#endif
