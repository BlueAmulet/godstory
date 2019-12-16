#ifndef CHAT_MANAGER_H
#define CHAT_MANAGER_H

#include <deque>
#include <list>

#include "Common/TimerMgr.h"
#include "Common/PacketType.h"
#include "Base/bitStream.h"
#include "Base/Locker.h"
#include "Common/MemGuard.h"
//#include "Common/Script.h"
#include "wintcp/IPacket.h"
#include "wintcp/dtServerSocket.h"

#include "PlayerMgr.h"
#include "ServerMgr.h"
#include "Common/ChatBase.h"
#include "Channel.h"

#include "LockCommon.h"
#include "ManagerBase.h"


#define CHAT_PENDING_TIME_NORAML		3000
#define CHAT_PENDING_TIME_PRIVATE		1000
#define CHAT_PENDING_TIME_WORLDPLUS		30000
#define CHAT_PENDING_TIME_REALCITY		5000
#define CHAT_PENDING_TIME_LINE			30000

#define GM_FLAG	'$'

class CWorldServer;

struct stPlayerChatRecord
{
	stPlayerChatRecord()
	{
		memset( this, 0, sizeof( stPlayerChatRecord ) );	
	}

	UINT nLastTime;
	int nPendingTime;
};

class stAccountInfo;

class CChatManager : public ILockable,  public CManagerBase< CChatManager, MSG_CHAT_BEGIN, MSG_CHAT_END >
{
public:
    /************************************************************************/
    /* ÿ��һ����ʱ�䣬���ض���������Ϣ
    /************************************************************************/
    struct AutoMsg
    {
        AutoMsg(void) : playerId(0),channel(0),startTime(0),endTime(0),lastSendTime(0),freq(-1),times(-1) {}

        stChatMessage msg;          //��Ϣ
        int           playerId;     //����������ҵ���Ϊ��
        int           channel;      //Ƶ��
        __time64_t    startTime;    //���͵Ŀ�ʼʱ��
        __time64_t    endTime;      //��Ϣ����ʱ��
        __time64_t    lastSendTime; //�ϴη����͵�ʱ��
        int           freq;         //����s����1��
        int           times;        //�ܹ����Ͷ��ٴ�
    };

	CChatManager();
	virtual ~CChatManager();
private:
	bool CheckMsg( stChatMessage& chatMessage );

public:
	void Initialize();

    void AddAutoMsg(AutoMsg& msg) {m_autoMsgs.push_back(msg);}

    void HandleIncomingMsg( int nSocket, stPacketHead* pHead, Base::BitStream* pBitStream );
	void HandleChangeMessageType(int nSocket, stPacketHead* pHead, Base::BitStream* pBitStream );

	void HandleGMMessage( stChatMessage& chatMessage );

	void SendChatResponse( int nSocket, int nAccountId, char cType );
	
	void SendMessage( stChatMessage& chatMessage, std::list<int>& playerList );

	void SendMessageTo( stChatMessage& chatMessage, int nPlayerId );
	void SendMessageToWorld( stChatMessage& chatMessage );
	void SendMessageToGate( stChatMessage& chatMessage, int nLineId, int nGateId );
	void SendMessageToLine( stChatMessage& chatMessage, int nLineId );
	void SendMessageToZone( stChatMessage& chatMessage, int nLIneId, int nZoneId );
	void SendMessageToFriend( stChatMessage& chatMessage, int nPlayerId );
	void SendMessageToChannel( stChatMessage& chatMessage, int nChannelId );
	CChannelManager* GetChannelManager();

	void CloseMessageType( int nPlayerId, char cType );
	void OpenMessageType( int nPlayerId, char cType );
	bool IsMessageTypeOpen( int nPlayerId, char cType );

	void SetPlayerMuted( int nPlayerId );
	void SetPlayerUnmuted( int nPlayerId );

	bool IsPlayerMuted( int nPlayerId );

	void SetPlayerChatPendingTime( int nPlayerId, char cMessageType, int nTime );
	int GetPlayerChatPendingTime( int nPlayerId, char cMessageType );

	void SetPlayerChatLastTime( int nPlayerId, char cMessageType, UINT nTime );
	UINT GetPlayerChatLastTime( int nPlayerId, char cMessageType );
    
    /************************************************************************/
    /* ��������ϵͳ���ƽ��Զ���Ϣ�ķ���
    /************************************************************************/
    void    Update(void);
private:

	deque<stChatMessage*>	m_dqChatMessageList;
	CChannelManager			m_channelManager;
	//CScript*				m_pScript;

	stdext::hash_set<int>	m_playerMutedSet;
	stdext::hash_map<__int64, stPlayerChatRecord> m_playerChatMap;

    std::list<AutoMsg>      m_autoMsgs;
public:
	CMyCriticalSection m_cs;
};

#endif


