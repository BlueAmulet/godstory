#include "Gameplay/ClientGameplayState.h"
#include "ClientTeam.h"
#include "console\console.h"
#include "NetWork\UserPacketProcess.h"
#include "../ClientPlayerManager.h"

CClientTeam::CClientTeam()
{
	m_nCaption = 0;
}

CClientTeam* CClientTeam::GetInstance()
{
	static CClientTeam theTeam;

	return &theTeam;
}

void CClientTeam::OnCreate( std::list<int> playerList )
{
	m_playerList.clear();

	std::list<int>::iterator it;

	for( it = playerList.begin(); it != playerList.end(); it++ )
	{
		m_playerList.push_back( *it );
	}
}

void CClientTeam::OnAddPlayer( int nPlayerId )
{
	if( FindPlayer( nPlayerId ) == m_playerList.end() )
		m_playerList.push_back( nPlayerId );
}

int CClientTeam::GetCaption()
{
	if( m_playerList.size() == 0 )
		return 0;
	return *m_playerList.begin();
}

std::list<int>* CClientTeam::GetPlayerList()
{
	return &m_playerList;
}

void CClientTeam::OnRemovePlayer( int nPlayerId )
{
	m_playerList.erase( FindPlayer( nPlayerId ) );
}

// �����ɢ
void CClientTeam::OnDisband()
{
	m_playerList.clear();

	m_nTeamId = 0;
	m_nCaption = 0;
	
	mFollowEnable = false;
}

bool CClientTeam::HasTeam()
{
	return m_playerList.size() != 0;
}

void CClientTeam::SetCaption( int nPlayerId )
{
	std::list<int>::iterator it;

	it = FindPlayer( nPlayerId );

	if( it == m_playerList.end() )
		return ;

	int nCaption;

	nCaption = *m_playerList.begin();

	*m_playerList.begin() = nPlayerId;

	*it = nCaption;
}

int CClientTeam::GetTeamId()
{
	return m_nTeamId;
}

void CClientTeam::SetTeamId( int nTeamId )
{
	m_nTeamId = nTeamId;
}

int CClientTeam::GetPlayerByIndex( int nIndex, int isMyselfIncluded )
{
	std::list<int>::iterator it;

	int i = 0;
	for( it = m_playerList.begin(); it != m_playerList.end(); it++)
	{
		if( !isMyselfIncluded && *it == g_ClientGameplayState->getSelectedPlayerId() )
			continue;

		if( i == nIndex )
			return *it;
		i++;

	}

	return 0;
}

std::list<int>::iterator CClientTeam::FindPlayer( int nPlayerId )
{
	std::list<int>::iterator it;

	for( it = m_playerList.begin(); it != m_playerList.end(); it++ )
	{
		if( *it == nPlayerId )
			break;
	}

	return it;
}

bool CClientTeam::IsTeammate( int nPlayerId )
{
	if( !HasTeam() )
		return false;

	std::list<int>::iterator it;
	for( it=m_playerList.begin(); it!=m_playerList.end(); it++ )
	{
		if( *it == nPlayerId )
			return true;
	}

	return false;
}

void CClientTeam::EnableFollow( bool enabled )
{
	int playerId = g_ClientGameplayState->getSelectedPlayerId();
	mLocalFollowEnable = enabled;
	if( enabled )
	{
		if( mFollowEnable )
		{
			if( GetCaption() != playerId )
			{
				Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
				if( pPlayer )
				{
					Player* pLeader = PLAYER_MGR->GetLocalPlayer( GetCaption() );
					if( pLeader )
					{
						pPlayer->SetTarget( pLeader, true );
						return ;
					}
				}
			}
			else
			{			
				Con::printf( "ERROR: ��Ӹ����Ѿ�������" );
				return ;
			}
		}

		if( !HasTeam() )
		{
			Con::printf( "ERROR: �㲻�ڶ����У����ܿ�����Ӹ��棡" );
			return ;
		}

		if( GetCaption() != playerId )
		{
			Con::printf( "ERROR: �㲻�Ƕӳ������ܿ�����Ӹ��棡" );
			return ;
		}
		else
		{
			char buf[MAX_PACKET_SIZE];
			Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
			stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, CLIENT_TEAM_Follow, playerId, SERVICE_WORLDSERVER, enabled );

			pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

			UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
			if( pProcess )
				pProcess->Send( sendPacket );
		}
	}
	else
	{
		if( !mFollowEnable )
		{
			Con::printf( "ERROR: ��Ӹ����Ѿ��رգ�" );
			return ;
		}
		
		if( GetCaption() != playerId )
		{
			g_ClientGameplayState->GetControlPlayer()->SetTarget( NULL, false );
			Con::printf( "��Ӹ����ѹر�" );
			return ;
		}
		else
		{
			char buf[MAX_PACKET_SIZE];
			Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
			stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, CLIENT_TEAM_Follow, playerId, SERVICE_WORLDSERVER, enabled );

			pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

			UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
			if( pProcess )
				pProcess->Send( sendPacket );
		}
	}
}

bool CClientTeam::AddInvation( int nPlayerId, int nType )
{
	if( Con::getBoolVariable("$IsTeamAutoRefuseApply",false) && nType == 1 )
	{
		UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
		if( pProcess )
			pProcess->Send_Client_Team_Refused( nPlayerId, nType );

		return false;
	}

	if( Con::getBoolVariable("$IsTeamAutoRefuseInvation",false) && nType != 1 )
	{
		UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
		if( pProcess )
			pProcess->Send_Client_Team_Refused( nPlayerId, nType );

		return false;
	}

	stClientTeamInvation *invation = new stClientTeamInvation();
	invation->mTime = (int)time(NULL);
	invation->mPlayerId = nPlayerId;
	invation->mType = nType;

	mInvationSet.push_back( invation );

	std::string playerName = PLAYER_MGR->getPlayerName( nPlayerId );

	// ���������߼��ӳ��¼�
	Player::stLogicEvent* logicEvent = new Player::stLogicEvent;
	logicEvent->type		= Player::LOGICEVENT_TEAMINVITE;
	if( nType == 1 )
		logicEvent->type	= Player::LOGICEVENT_TEAMREQUEST; 
	logicEvent->sender		= nPlayerId;
	logicEvent->senderName	= StringTable->insert( playerName.c_str() );
	logicEvent->data		= NULL;
	logicEvent->datalen		= 0;
	logicEvent->relationid	= nType;
	
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if( pPlayer )
	{	
		U32 eid = pPlayer->insertLogicEvent(logicEvent);

		// ����ͨ�������ʱ�Ի���
		Con::executef("CommonInviteDialog", Con::getIntArg(eid));
	}

	return true;
}

//void CClientTeam::

void CClientTeam::ProcessWork()
{
	std::list< stClientTeamInvation* >::iterator it;

	int curTime = (int)time(NULL);
	stClientTeamInvation* invation = NULL;
	
	for( it = mInvationSet.begin(); it != mInvationSet.end(); it++ )
	{
		invation = *it;
		if( curTime - invation->mTime > 30 ) 
		{
			RemoveInvation( (*it)->mPlayerId );
			return ;
		}
	}
}

void CClientTeam::RemoveInvation( int nPlayerId )
{
	std::list< stClientTeamInvation* >::iterator it;

	int id[2] = { 0 };
	char szId[32];
	char szType[32];
	
	for( it = mInvationSet.begin(); it != mInvationSet.end(); it++ )
	{
		if( (*it)->mPlayerId == nPlayerId )
		{
			g_ClientGameplayState->GetPacketProcess()->Send_Client_Team_Refused( (*it)->mPlayerId, (*it)->mType);
			dSprintf( szId, sizeof(szId), "%d", (*it)->mType == 1 ? id[1] : id[0] );
			dSprintf( szType, sizeof(szType), "%d", (*it)->mType == 1 );
			Con::executef( "TeamAutoRefused", szId, szType );
			delete (*it);
			mInvationSet.erase( it );
			return ;
		}

		if( (*it)->mType != 1)
			id[0] ++;
		else
			id[1] ++;
	}
}

void CClientTeam::AcceptInvationPlayerByIndex( int index, int type )
{
	std::list< stClientTeamInvation* >::iterator it;

	if( type > 1 )
		return ;

	int id[2] = { 0 };

	char szId[32];

	for( it = mInvationSet.begin(); it != mInvationSet.end(); it++ )
	{
		if( index == id[type] )
		{
			//g_ClientGameplayState->GetPacketProcess()->Send_Client_Team_Refused( (*it)->mPlayerId, (*it)->mType);
			dSprintf( szId, sizeof(szId), "%d", (*it)->mPlayerId );
			switch( (*it)->mType )
			{
			case 0:
				Con::executef( "SptTeam_AcceptBuild", szId );
				break;
			case 1:
				Con::executef( "SptTeam_AcceptJoin", szId );
				break;
			case 2:
				Con::executef( "SptTeam_AcceptAdd", szId );
				break;
			}
			//Con::executef( "SptTeam_SendRefuset", szId, szType );
			//Con::executef( "TeamAutoRefused", szId, szType );
			delete *it;
			mInvationSet.erase( it );
			return ;
		}
		if( (*it)->mType != 1 )
			id[0]++;
		else
			id[1]++;
	}

	return ;
}

int CClientTeam::GetInvationIndexById( int playerId, int type )
{
	std::list< stClientTeamInvation* >::iterator it;

	int id[2] = { 0 };

	int index = 0;
	for( it = mInvationSet.begin(); it != mInvationSet.end(); it++ )
	{
		if( (*it)->mPlayerId == playerId )
			return id[type];

		if( (*it)->mType != 1 )
			id[0]++;
		else
			id[1]++;
	}

	return -1;
}

// �ܾ��б��е����
void CClientTeam::RefuseInvationPlayerByIndex( int index, int type )
{
	std::list< stClientTeamInvation* >::iterator it;

	if( type > 1 )
		return ;

	int id[2] = { 0 };

	char szId[32];
	char szType[32];

	for( it = mInvationSet.begin(); it != mInvationSet.end(); it++ )
	{
		if( index == id[type] )
		{
			g_ClientGameplayState->GetPacketProcess()->Send_Client_Team_Refused( (*it)->mPlayerId, (*it)->mType);
			dSprintf( szId, sizeof(szId), "%d", (*it)->mPlayerId );
			dSprintf( szType, sizeof(szType), "%d", (*it)->mType );
			Con::executef( "SptTeam_SendRefused", szId, szType );
			//Con::executef( "TeamAutoRefused", szId, szType );
			delete *it;
			mInvationSet.erase( it );
			return ;
		}
		if( (*it)->mType != 1 )
			id[0]++;
		else
			id[1]++;
	}

	return ;
}

// ͨ��IndexȡPlayerId
int CClientTeam::GetInvationPlayerByIndex(int index, int type)
{
	std::list< stClientTeamInvation* >::iterator it;

	if( type > 1 )
		return 0;

	int id[2] = { 0 };


	for( it = mInvationSet.begin(); it != mInvationSet.end(); it++ )
	{
		if( index == id[type] )
		{
			return (*it)->mPlayerId;
		}
		if( (*it)->mType != 1 )
			id[0]++;
		else
			id[1]++;
	}

	return 0;
}

void CClientTeam::OnEnableFollow( bool enabled )
{
	mFollowEnable = enabled;
}

// ������ر���Ӹ���
ConsoleFunction( EnableTeamFollow, void, 2, 2, "" )
{
	CLIENT_TEAM->EnableFollow( atoi(argv[1] ) );
}


// ����Ż�ȡ����ID
ConsoleFunction(GetTeammate, int, 3, 3, "" )
{
	return CLIENT_TEAM->GetPlayerByIndex( atoi( argv[1] ), atoi( argv[2] ) );
}

// �ж��Լ��Ƿ��Ƕӳ�
ConsoleFunction(IsMyselfCaption, int, 1, 1, "" )
{
	if( CLIENT_TEAM->m_playerList.empty())
		return false;
	return g_ClientGameplayState->getSelectedPlayerId() == *(CLIENT_TEAM->m_playerList.begin());
}

// �ж��Լ��Ƿ��ж���
ConsoleFunction( IsMyselfHaveTeam, int, 1, 1, "" )
{
	return CLIENT_TEAM->HasTeam();
}

// ���;ܾ�
ConsoleFunction( SptTeam_SendRefused, void, 3, 3, "" )
{
	UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_Refused( atoi( argv[1] ), atoi( argv[2] ) );
}

// ���ͽ�ɢ
ConsoleFunction( SptTeam_Disband, void, 1, 1, "" )
{
	UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_Disband( g_ClientGameplayState->getSelectedPlayerId() );
}

// �޸Ķ�������
ConsoleFunction( SptTeam_ChangeName, void, 2, 2, "SptTeam_ChangeName( team_name )" )
{
	UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_ChangeName( g_ClientGameplayState->getSelectedPlayerId(), (char*)argv[1] );
}

// ���ӳ�
ConsoleFunction( SptTeam_ChangeLeader, void, 2, 2, "SptTeam_ChangeLeader( leader_id )" )
{
	UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send_Client_Team_ChangeLeader( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// ���������Ϣ
 ConsoleFunction( SptTeam_TeamInfoRequest, void, 2, 2, "SptTeam_TeamInfoRequest( teamid )" )
 {
	 UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	 if( pProcess )
		 pProcess->SendSimplePacket( g_ClientGameplayState->getSelectedPlayerId(), CLIENT_TEAM_ShowTeamInfo, atoi( argv[1] ) );
 }
 
 // ȡ���ض���ҵĶ���ID
 ConsoleFunction( SptTeam_GetPlayerTeamId, int, 2, 2, "SptTeam_GetPlayerTeamId( playerid )" )
 {
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer( atoi( argv[1] ) );
	if( !pPlayer )
		return 0;

	return pPlayer->getTeamInfo().m_nId;
 }

 // �Ƴ��Զ��ܾ�
 ConsoleFunction( SptTeam_RemoveRefuse, void, 2, 2, "SptTeam_RemoveRefuse" )
 {
	 CLIENT_TEAM->RemoveInvation( atoi( argv[1]) );
 }

 //// �Ƴ��Զ��ܾ�
 //ConsoleFunction( SptTeam_RemoveRefuseByIndex, void, 2, 2, "SptTeam_RemoveRefuseByIndex" )
 //{
	// CLIENT_TEAM->RemoveInvationByIndex( atoi( argv[1]) );
 //}

 //// �ܾ��Զ��б��еĶ���
 //ConsoleFunction( SptTeam_RefuseInvation, void, 3, 3, "" )
 //{
	// CLIENT_TEAM->RefuseInvation( atoi( argv[1]), atoi( argv[2] ) );
 //}

 // �����Ӹ���״̬
 ConsoleFunction( SptTeam_IsFollow, bool, 1,1, "" )
 {
	 return CLIENT_TEAM->mLocalFollowEnable;
 }

 // ���ӳ��Ƿ�����Ӹ���
 ConsoleFunction( SptTeam_IsFollowOn, bool, 1,1, "" )
 {
	 return CLIENT_TEAM->mFollowEnable;
 }

 ConsoleFunction( SptTeam_GetInvationPlayerByIndex, int, 3, 3, "" )
 {
	 return CLIENT_TEAM->GetInvationPlayerByIndex( atoi( argv[1] ), atoi( argv[2]) );
 }

 // ͨ��Indexͬ������
 ConsoleFunction( SptTeam_AcceptInvationPlayerByIndex, void, 3, 3, "" )
 {
	 CLIENT_TEAM->AcceptInvationPlayerByIndex( atoi( argv[1] ), atoi( argv[2] ) );
 }

// ͨ��Index�ܾ�����
 ConsoleFunction( SptTeam_RefuseInvationPlayerByIndex, void, 3, 3, "" )
 {
	 CLIENT_TEAM->RefuseInvationPlayerByIndex( atoi( argv[1] ), atoi( argv[2] ) );
 }

 // ͨ��PlayerId���Index
 ConsoleFunction( SptTeam_GetInvationIndex, int, 3, 3, "" )
 {
	return CLIENT_TEAM->GetInvationIndexById( atoi( argv[1] ), atoi( argv[2]) );
 }