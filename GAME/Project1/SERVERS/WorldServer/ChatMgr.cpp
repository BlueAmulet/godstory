#include "TeamManager.h"
#include "ChatMgr.h"
#include "WorldServer.h"
#include "BanList.h"
#include "GM/GMRespondDef.h"

CChatManager::CChatManager()
{
    registerEvent(CLIENT_CHAT_SendMessageRequest, &CChatManager::HandleIncomingMsg);
    registerEvent(CLIENT_CHAT_ChangeMessageTypeStatus, &CChatManager::HandleChangeMessageType);

	gLock.registerLockable( this, Lock::Chat );
}

CChatManager::~CChatManager()
{
 
}

void CChatManager::Initialize()
{
	//m_pScript = CScript::GetInstance();
}

CChannelManager* CChatManager::GetChannelManager()
{
	return &m_channelManager;
}

void CChatManager::SendChatResponse( int nSocket, int nAccountId, char cType )
{
	CMemGuard buf(40);
	Base::BitStream SendPacket( buf.get(), 40 );
	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, CLIENT_CHAT_SendMessageResponse );
	pHead->DestServerType = SERVICE_CLIENT;
	pHead->Id = nAccountId;		
	pHead->SrcZoneId = cType;
	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( nSocket, SendPacket );
}

void CChatManager::HandleIncomingMsg( int nSocket, stPacketHead* pHead, Base::BitStream* pBitStream )
{
	int nPlayerId = pHead->Id; 
	HACK("��nPlayerIdҪ���а�ȫ���");

	DO_LOCK( Lock::Chat | Lock::Player | Lock::Channel );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	
	if( !pAccountInfo )
		return ;

    if( IsPlayerMuted( nPlayerId ) || CBanlist::Instance()->IsAccChatBan(pAccountInfo->AccountName) || CBanlist::Instance()->IsActorChatBan(pAccountInfo->GetCurPlayerName()))
	{
		// ����ұ�������
		SendChatResponse( nSocket, pAccountInfo->AccountId, SEND_CHAT_ACK_REFUSED );
		return ;
	}

	int nameRecver = 0;

	// ����ͨ������ֱ�ӷ�����Ϸ��Ϣ
	if( pHead->DestZoneId == 1 )
	{
		char playerName[COMMON_STRING_LENGTH];
		pBitStream->readString( playerName ,COMMON_STRING_LENGTH);
		std::string str = playerName;

		PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( str );
		
		if( pPlayerData.isNull() )
			return ;

		nameRecver = pPlayerData->BaseData.PlayerId;		
	}
	
	// ���ڷ��͸�����Ϣ
	std::list<int> nearPlayerList;

	if( pHead->DestZoneId == 2 )
	{
		int playerCount = pBitStream->readInt( 16 );
		for( int i = 0; i < playerCount; i++ )
		{
			int pid = pBitStream->readInt(32);
			nearPlayerList.push_back( pid );
		}
	}

	stChatMessage msg;
	stChatMessage::UnpackMessage( msg, *pBitStream );
	msg.nSender = nPlayerId;

	if( nameRecver != 0 )
		msg.nRecver = nameRecver;

	if( SERVER->GetPlayerManager()->IsGM( msg.nSender ) && msg.szMessage[0] == GM_FLAG )
	{
		this->HandleGMMessage( msg );
		return ;
	}

	// �����Ϣ�Ƿ�Ϸ�
	if( !CheckMsg( msg ) )
		return ;

	// ��鷢��ʱ����
	UINT nPendingTime = GetPlayerChatPendingTime( nPlayerId, msg.btMessageType );
	UINT nLastTime = GetPlayerChatLastTime( nPlayerId, msg.btMessageType );
	
	if( ::GetTickCount() - nLastTime < nPendingTime )
	{
		SendChatResponse( nSocket, pAccountInfo->AccountId, SEND_CHAT_ACK_TOOFAST );
		return ;
	}

	SetPlayerChatLastTime( nPlayerId, msg.btMessageType, ::GetTickCount() );

	// ��鷢����Ϣ����
	if( msg.btMessageType < CHAT_MSG_TYPE_RACE )
	{
		// ��Ҳ���ͨ��������Ϣ�ķ�ʽ���ʹ��ģ��Ϣ
		SendChatResponse( nSocket, pAccountInfo->AccountId, SEND_CHAT_ACK_ERROR );
		return ;
	}

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId );

	if( pPlayerData.isNull() )
		return ;

	stChatMessage sendMsg;
	std::string str = msg.szMessage;
	strcpy_s( sendMsg.szMessage, 512, str.c_str() );
	str = pPlayerData->BaseData.PlayerName;
	strcpy_s( sendMsg.szSenderName, 32, str.c_str() );
	sendMsg.btMessageType = msg.btMessageType;
	sendMsg.nSender = msg.nSender;

	CChannel* pChannel = GetChannelManager()->GetChannel( nPlayerId, msg.btMessageType );
	if( pChannel == NULL )
	{
		// �����Ⱥ��Ϣ
		if( msg.btMessageType == CHAT_MSG_TYPE_GROUP )
			pChannel = GetChannelManager()->GetChannel( msg.nRecver );
	}
	
	if( ( msg.btMessageType == CHAT_MSG_TYPE_PRIVATE || msg.btMessageType == CHAT_MSG_TYPE_P2P ) && msg.nRecver != msg.nSender ) // ���ܸ��Լ�����Ե���Ϣ
	{
		SendMessageTo( sendMsg, msg.nRecver );
	}
	else
	{
		// ����һ����Ч����Ϣ 
		if( pChannel == NULL )
		{
			if( msg.btMessageType == CHAT_MSG_TYPE_NEARBY )
			{
				SendMessage( sendMsg, nearPlayerList );
			}
			else
			{
				SendChatResponse( nSocket, pAccountInfo->AccountId, SEND_CHAT_ACK_ERROR );
			}
			return ;
		
		}
		
		SendMessageToChannel( sendMsg, pChannel->GetId() );
	}
	

}

void CChatManager::HandleGMMessage( stChatMessage& chatMessage )
{
	// �����ã���ǰ����GM��������Ϣ��Ϊ������Ϣ����
	chatMessage.szMessage[0] = ' ';
	chatMessage.btMessageType = CHAT_MSG_TYPE_SYSTEM;
	SendMessageToWorld( chatMessage );
}

// û�к�ǿ��ʱЧ�ԣ�����ͻ���ͬ������
void CChatManager::HandleChangeMessageType(int nSocket,stPacketHead* pHead, Base::BitStream* pBitStream )
{
	char cType = (char)pHead->SrcZoneId;
	int nPlayerId = pHead->Id;

	bool bOpen = IsMessageTypeOpen( nPlayerId, cType );
	if( bOpen )
		CloseMessageType( nPlayerId, cType );
	else
		OpenMessageType( nPlayerId, cType );

}

// ��ȫ���緢��ȫ����Ϣ
void CChatManager::SendMessageToWorld( stChatMessage& chatMessage )
{
	DO_LOCK( Lock::Chat | Lock::Chat );

	CMemGuard Buffer(MAX_PACKET_SIZE);
	Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
	stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, SERVER_CHAT_SendMessageAll );
	pSendHead->DestServerType = SERVICE_GATESERVER;

	stChatMessage::PackMessage( chatMessage, SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	std::list<stServerInfo*> gateList;
	SERVER->GetServerManager()->GetAllGates( gateList );

	std::list<stServerInfo*>::iterator it;

	for( it = gateList.begin(); it != gateList.end(); it++ )
	{
		SERVER->GetServerSocket()->Send( (*it)->SocketId, SendPacket );
	}
}

// ��һ����Ⱥ�ڷ�����Ϣ
void CChatManager::SendMessage( stChatMessage& chatMessage, std::list<int>& playerList )
{
	CMemGuard Buffer(MAX_PACKET_SIZE);
	stdext::hash_map< int, stdext::hash_set<int> > map;
	
	SERVER->GetPlayerManager()->PutPlayersInGate( playerList, map );

	stdext::hash_map< int, stdext::hash_set<int> >::iterator mit;
	stdext::hash_set<int>::iterator it;
	CPlayerManager* pPlayerManager = SERVER->GetPlayerManager();

	int nSocket;

	DO_LOCK( Lock::Player );

	for( mit = map.begin(); mit != map.end(); mit++ )
	{	
		Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
		stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, SERVER_CHAT_SendMessage );
		pSendHead->DestServerType = SERVICE_GATESERVER;
		SendPacket.writeInt( mit->second.size(), 16 );
		for( it = mit->second.begin(); it != mit->second.end(); it++ )
		{
			SendPacket.writeInt( pPlayerManager->GetAccountInfo( *it )->AccountId, 32 ); // Ҫ��PlayerIdת����AccountId
		}
		stChatMessage::PackMessage( chatMessage, SendPacket );
		pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

		nSocket = mit->first;
		SERVER->GetServerSocket()->Send( nSocket, SendPacket );
	}

}

// ��һ��Zone�����ڷ���ȫ����Ϣ
void CChatManager::SendMessageToZone( stChatMessage& chatMessage, int nLineId, int nZoneId )
{
	
}



void CChatManager::SendMessageTo( stChatMessage& chatMessage, int nPlayerId )
{
	//// ������ں����б���
	//if( !SERVER->GetFriendManager()->IsFriend( chatMessage.nSender, nPlayerId ) )
	//{
	//	// ���������ʱ�����б���
	//	if( !SERVER->GetFriendManager()->IsFriendTemp( chatMessage.nSender, nPlayerId ) )
	//	{
	//		// ��Ϊ��ʱ����
	//		SERVER->GetFriendManager()->MakeFriendTemp( chatMessage.nSender, nPlayerId );
	//	}
	//}

	CMemGuard Buffer(MAX_PACKET_SIZE);
	Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
	stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, SERVER_CHAT_SendMessage, 0, SERVICE_CLIENT );

	stChatMessage::PackMessage( chatMessage, SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	int nGateServerSocket;

	DO_LOCK( Lock::Player );


	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	if( !pAccountInfo )
		return ;

	pSendHead->Id = pAccountInfo->AccountId;
	nGateServerSocket = pAccountInfo->socketId;
	SERVER->GetServerSocket()->Send( nGateServerSocket, SendPacket );
}

void CChatManager::SendMessageToFriend( stChatMessage& chatMessage, int nPlayerId )
{
	std::list<int> playerList;

	SendMessage( chatMessage, playerList );
}


void CChatManager::SendMessageToLine( stChatMessage& chatMessage, int nLineId )
{
	DO_LOCK( Lock::Chat );

	CMemGuard Buffer(MAX_PACKET_SIZE);
	Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
	stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, SERVER_CHAT_SendMessageAll );
	pSendHead->DestServerType = SERVICE_GATESERVER;

	stChatMessage::PackMessage( chatMessage, SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	std::list<stServerInfo*> gateList;
	SERVER->GetServerManager()->GetAllGates( gateList, nLineId );

	std::list<stServerInfo*>::iterator it;

	for( it = gateList.begin(); it != gateList.end(); it++ )
	{
		SERVER->GetServerSocket()->Send( (*it)->SocketId, SendPacket );
	}
}

void CChatManager::SendMessageToGate( stChatMessage& chatMessage, int nLineId, int nGateId )
{
	int nGateServerSocket = 0;

	//��ȡ�����ڲ����ݱ��������������д��ʱ��Ҫ��С��
	{
		DO_LOCK( Lock::Server );
		stServerInfo* pServerInfo = SERVER->GetServerManager()->GetGateServer( nLineId, nGateId );
		if(pServerInfo)
			nGateServerSocket = pServerInfo->SocketId;
	}


	if( nGateServerSocket )
	{
		CMemGuard Buffer(MAX_PACKET_SIZE);
		Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
		stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, SERVER_CHAT_SendMessageAll );
		pSendHead->DestServerType = SERVICE_GATESERVER;

		stChatMessage::PackMessage( chatMessage, SendPacket );
		pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( nGateServerSocket, SendPacket );
		
	}
}

void CChatManager::SendMessageToChannel( stChatMessage& chatMessage, int nChannelId )
{
	DO_LOCK( Lock::Channel );

	CChannel* pChannel = GetChannelManager()->GetChannel( nChannelId );
	if( !pChannel )
		return ;

	std::list<int> playerList;

	pChannel->GetPlayerList( playerList );

	SendMessage( chatMessage, playerList );
}


// ������֤��Ϣ�Ϸ���
bool CChatManager::CheckMsg( stChatMessage& chatMessage )
{
	return true;
}

// ������Ϣ
void CChatManager::CloseMessageType( int nPlayerId, char cType )
{
	// �������δ��ģ��Ϣ
	if( cType < CHAT_MSG_TYPE_RACE )
		return ;

	DO_LOCK( Lock::Channel );

	CChannel* pChannel = GetChannelManager()->GetChannel( nPlayerId, cType );
	if( !pChannel )
		return ;

	GetChannelManager()->RemovePlayer( nPlayerId, pChannel->GetId(), false );
}


// ȡ������
void CChatManager::OpenMessageType( int nPlayerId, char cType )
{
	DO_LOCK( Lock::Channel );
	
	CChannel* pChannel = GetChannelManager()->GetChannel( nPlayerId, cType );
	if( !pChannel )
		return ;

	GetChannelManager()->AddPlayer( nPlayerId, pChannel->GetId() );
}


// �ж�ĳ����Ƿ����ε�ĳ����Ϣ
// ����ֵ��Ϊû�����Σ���Ϊ����
bool CChatManager::IsMessageTypeOpen( int nPlayerId, char cType )
{
	DO_LOCK( Lock::Channel );
	
	CChannel* pChannel = GetChannelManager()->GetChannel( nPlayerId, cType );
	if( pChannel != NULL )
	{
		return pChannel->HasPlayer( nPlayerId );
	}

	return false;
}


// �������
void CChatManager::SetPlayerMuted( int nPlayerId )
{
	DO_LOCK( Lock::Chat );

	m_playerMutedSet.insert( nPlayerId );
}

void CChatManager::SetPlayerUnmuted( int nPlayerId )
{
	DO_LOCK( Lock::Chat );;

	m_playerMutedSet.erase( nPlayerId );
}

bool CChatManager::IsPlayerMuted( int nPlayerId )
{
	DO_LOCK( Lock::Chat );;

	return m_playerMutedSet.find( nPlayerId ) != m_playerMutedSet.end();
}

// ���ͼ�����
void CChatManager::SetPlayerChatPendingTime( int nPlayerId, char cMessageType, int nTime )
{
	__int64 v;
	v = nPlayerId;
	v = ( v << 8 ) | cMessageType;

	DO_LOCK( Lock::Chat );

	m_playerChatMap[v].nPendingTime = nTime;
}

int CChatManager::GetPlayerChatPendingTime( int nPlayerId, char cMessageType )
{
	__int64 v;
	v = nPlayerId;
	v = ( v << 8 ) | cMessageType;

	DO_LOCK( Lock::Chat );

	stPlayerChatRecord& rec = m_playerChatMap[v];
	if( rec.nPendingTime == 0 )
	{
		switch( cMessageType )
		{
		case CHAT_MSG_TYPE_PRIVATE:
			rec.nPendingTime = CHAT_PENDING_TIME_PRIVATE;
			break;
		case CHAT_MSG_TYPE_WORLDPLUS:
			rec.nPendingTime = CHAT_PENDING_TIME_WORLDPLUS;
			break;
		case CHAT_MSG_TYPE_REALCITY:
			rec.nPendingTime = CHAT_PENDING_TIME_REALCITY;
			break;
		default:
			rec.nPendingTime = CHAT_PENDING_TIME_NORAML;
			break;
		}
	}

	return rec.nPendingTime;
}

void CChatManager::SetPlayerChatLastTime( int nPlayerId, char cMessageType, UINT nTime )
{
	__int64 v;
	v = nPlayerId;
	v = ( v << 8 ) | cMessageType;

	DO_LOCK( Lock::Chat );

	m_playerChatMap[v].nLastTime = nTime;
}

UINT CChatManager::GetPlayerChatLastTime( int nPlayerId, char cMessageType )
{
	__int64 v;
	v = nPlayerId;
	v = ( v << 8 ) | cMessageType;

	DO_LOCK( Lock::Chat );;

	return m_playerChatMap[v].nLastTime;
}

void CChatManager::Update(void)
{
    DO_LOCK( Lock::Chat );

    __time64_t curTime = time(0);

    for (std::list<AutoMsg>::iterator iter = m_autoMsgs.begin(); iter != m_autoMsgs.end();)
    {
        AutoMsg& autoMsg = *iter;

        if (curTime < autoMsg.startTime)
        {
            ++iter;
            continue;
        }
        
        bool isEnded = false;

        if (autoMsg.freq <= 0 && autoMsg.times <= 0)
        {
            isEnded = true;
        }

        //�ж�ʱ���Ƿ����
        if (autoMsg.startTime != autoMsg.endTime)
        {
            if (curTime >= autoMsg.endTime)
            {
                isEnded = true;
            }
        }
        else
        {
            if (autoMsg.freq <= 0 || autoMsg.times <= 0)
                isEnded = true;
        }

        //�жϷ��ʹ����Ƿ����
        if (0 == autoMsg.times)
        {
            isEnded = true;
        }
        
        if (isEnded)
        {
            iter = m_autoMsgs.erase(iter);
            continue;
        }

        //�ƽ�ʱ��
        bool isSend = false;

        if(curTime - autoMsg.lastSendTime >= autoMsg.freq)
        {
            isSend = true;
        }

        if (isSend)
        {
            if (autoMsg.times > 0 )
                autoMsg.times--;

            autoMsg.lastSendTime = curTime;

            //������Ϣ
            if(0 == autoMsg.playerId)
            {
                SERVER->GetChatManager()->SendMessageToWorld(autoMsg.msg);
            }
            else
            {
                switch(autoMsg.channel)
                {
                case CHAT_TARGET_PERSON:
                    {
                        autoMsg.msg.btMessageType = CHAT_MSG_TYPE_PRIVATE;
                        SERVER->GetChatManager()->SendMessageTo(autoMsg.msg,autoMsg.playerId);
                    }
                    break;
                case CHAT_TARGET_TEAM:
                    {
                        //��ȡ������ڵ�team
                        CTeam* pTeam = SERVER->GetTeamManager()->GetTeamByPlayer(autoMsg.playerId);

                        if (0 == pTeam)
                            return;
                        
                        autoMsg.msg.btMessageType = CHAT_MSG_TYPE_TEAM;
                        SERVER->GetChatManager()->SendMessageToChannel(autoMsg.msg,pTeam->GetChatChannelId());
                    }   
                    break;
                }
            }
        }

        ++iter;
    }
}