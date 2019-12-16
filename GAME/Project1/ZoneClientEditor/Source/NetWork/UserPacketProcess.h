#ifndef USER_PACKET_PROCESS_H
#define USER_PACKET_PROCESS_H

#include <string>
#include "Common/CommonPacket.h"

class ClientGameplayState;

class UserPacketProcess : public CommonPacket
{
	typedef CommonPacket Parent;

public:
	UserPacketProcess()
	{
	}
    
    void SetClientGameplayState(ClientGameplayState* pClientGameplayState) {m_pClientGameplayState = pClientGameplayState;}
protected:
	void HandleClientLogin();
	void HandleClientLogout();
	bool HandleGamePacket(stPacketHead *pHead,int DataSize);

public:
	void SendSimplePacket( int nPlayerId, char cMessageType, int nDestZoneId = 0, int nSrcZoneId = 0 );

	void Handle_Account_LoginResponse(Base::BitStream &RecvPacket);
	void Handle_Account_CheckResponse(Base::BitStream &RecvPacket);

	void Handle_Gate_LoginResponse(Base::BitStream &RecvPacket);
	void Handle_Game_LoginResponse(Base::BitStream &RecvPacket);
	void Handle_Game_LogoutResponse(stPacketHead *pHead,Base::BitStream &RecvPacket);

	void Handle_World_SelectPlayerResponse(stPacketHead *pHead,Base::BitStream &RecvPacket);
	void Handle_World_DeletePlayerResponse(stPacketHead *pHead,Base::BitStream &RecvPacket);
	void Handle_World_CreatePlayerResponse(stPacketHead *pHead,Base::BitStream &RecvPacket);

	void Handle_World_Client_PlayerDataTransResopnse(stPacketHead *pHead,Base::BitStream &RecvPacket);

	void Handle_Client_ItemRequest( stPacketHead *pHead,Base::BitStream &RecvPacket );


public:
	bool Send_Account_CheckRequest(const char *AccountName,const unsigned char *Password);
	bool Send_Account_LoginRequest(const char *AccountName,const unsigned char *Password);
	bool Send_Gate_LoginRequest(T_UID UID,int AccountId);
	bool Send_Game_LoginRequest(T_UID UID,int AccountId,int PlayerId,char Status);
	bool Send_Game_LogoutRequest(T_UID UID,int AccountId,int PlayerId);

	bool Send_SelectPlayerRequest(T_UID UID,int AccountId);
	bool Send_DeletePlayerRequest(T_UID UID,int AccountId,int PlayerId);
	bool Send_CreatePlayerRequest(T_UID UID,int AccountId,const char *PlayerName,int Sex,int Body,int Face,int Hair,int HairCol,U32 ItemId);

	// 消息相关
public:
	void Handle_Server_SendMessage( stPacketHead *pHead,Base::BitStream &RecvPacket );
	bool Send_Chat_TestMessage( int AccountId, const char* pMessage );
	bool Send_Chat_Message( int nPlayerId, char cMessageType, const char* pMessage, int nRecver = 0 );
	bool Send_Chat_ChangeType( int nPlayerId, int nType );
	void Handle_Chat_SendMessageResponse(  stPacketHead *pHead,Base::BitStream &RecvPacket );

	// 组队相关
public:
	void Send_Client_Team_BuildAlongRequest( int nPlayerId );
	void Send_Client_Team_BuildReuest( int nPlayerId, int nPlayerIdTo );
	void Send_Client_Team_AcceptBuildRequest( int nPlayerId, int nPlayerIdTo );
	// 其他玩家请求加入队伍
	void Send_Client_Team_JoinRequest( int nPlayerId, int nPlayerIdTo );
	void Send_Client_Team_AcceptJoinRequest( int nPlayerId, int nPlayerIdTo );
	// 队长加人
	void Send_Client_Team_AddRequest( int nPlayerId, int nPlayerIdTo );
	void Send_Client_Team_AcceptAddRequest( int nPlayerId, int nPlayerIdTo );
	void Send_Client_Team_DropPlayer( int nPlayerId, int nPlayerIdTo );
	
	void Send_Client_Team_Leave( int nPlayerId );
	void Send_Client_Team_Disband( int nPlayerId );
	void Send_Client_Team_Refused( int nPlayerId, int nType );
	void Send_Client_Team_ChangeName( int nPlayerId, char* szName );
	void Send_Client_Team_ChangeLeader( int nPlayerId, int nNewLeaderId );

	// 处理服务端通知
	bool Handle_Client_Team_PlayerJoined( stPacketHead *pHead,Base::BitStream &RecvPacket );
	bool Handle_Client_Team_PlayerLeave( stPacketHead *pHead,Base::BitStream &RecvPacket );
	bool Handle_Client_Team_Info( stPacketHead *pHead,Base::BitStream &RecvPacket );
	bool Handle_Client_Team_Error( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_Client_Team_Refused( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_Client_Team_Cancel( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Handle_Client_Team_ShowInfo( stPacketHead *pHead,Base::BitStream &RecvPacket );
	// 处理其他玩家请求
	bool Handle_Client_Team_BuildRequest( stPacketHead *pHead,Base::BitStream &RecvPacket );
	bool Handle_Client_Team_JoinRequest( stPacketHead *pHead,Base::BitStream &RecvPacket );
	bool Handle_Client_Team_AddRequest( stPacketHead *pHead,Base::BitStream &RecvPacket );

	void Handle_Client_Team_BaseInfo( stPacketHead* pHead, Base::BitStream& RecvPacket );

	bool Handle_Client_Team_MapMark( stPacketHead *pHead,Base::BitStream &RecvPacket );
	bool Handle_Client_Team_TargetMark( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void Send_Client_Team_MapMark( int id, int x, int y );
	void Send_Client_Team_TargetMark( int id, int targetId );

	// copy map related

    //发送GM命令
    bool SendGMCommand(int sessionId,Base::BitStream* pack,int AccountId,const char* fun);
    bool HandleGMCommandResp( stPacketHead *pHead,Base::BitStream &RecvPacket );
public:
	bool Handle_Client_CopyMap_EnterResponse( stPacketHead *pHead,Base::BitStream &RecvPacket );
	bool Handle_Client_CopyMap_OpenResponse( stPacketHead *pHead,Base::BitStream &RecvPacket  );
	bool Handle_Client_CopyMap_LeaveResponse( stPacketHead *pHead,Base::BitStream &RecvPacket );
	void HandleTick(stPacketHead *pHead,Base::BitStream &RecvPacket);
	void Hanlde_Client_Team_Follow( stPacketHead * pHead, Base::BitStream RecvPacket );
	bool Send_Chat_MessageByName( const char* playerName, const char* pMessage );
	bool Send_Chat_MessageNearby(const char* pMessage);
	bool Handle_Client_Team_InvationCopymap( stPacketHead *pHead,Base::BitStream &RecvPacket );

	void Hanlde_World_Client_MailNotify( stPacketHead *pHead,Base::BitStream &RecvPacket );
private:
    ClientGameplayState* m_pClientGameplayState;
};

#endif
