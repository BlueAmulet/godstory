//
//
// NTianJi Game Engine V1.0
//
//
// Name:
//		ServerGameplayState.h
//
//Abstract:
#include "core/bitStream.h"
#include "GamePlay/ServerGamePlayState.h"
#include "core/crc.h"
#include "console/console.h"
#include "core/stringTable.h"
#include "core/resManager.h"
#include "ts/TSShapeRepository.h"
#include "Gameplay/GameObjects/MountObjectData.h"
#include "Gameplay/GameObjects/NpcObjectData.h"
#include "Gameplay/GameObjects/PlayerData.h"
#include "NetWork/CommonClient.h"
#include "Gameplay/Data/SoundResource.h"
#include "T3D/trigger.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "T3D/gameConnection.h"
#include "Common/DataAgentHelper.h"
#include "Common/TeamBase.h"
#include "Gameplay/Item/ItemBaseData.h"
#include "Gameplay/item/NpcShopData.h"
#include "effects/EffectPacket.h"
#include "Gameplay/Item/ItemPackage.h"
#include "Gameplay/Item/DropRule.h"
#include "Gameplay/Data/RouteRepository.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/Item/Prescription.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/Data/EventTriggerData.h"
#include "Gameplay/GameObjects/MountObjectData.h"
#include "Gameplay/Data/PetStudyData.h"
#include "util/processMgr.h"
#include "Common/LogHelper.h"
#include "Gameplay/ai/AIData.h"
#include "Gameplay/Item/PrizeBox.h"
#include "Gameplay/Data/SuperMarketData.h"
#include "Gameplay/Data/SpiritData.h"
#include "Gameplay/Mission/PlayerMission.h"
#include "Gameplay/GameObjects/Talent.h"
// ========================================================================================================================================
//	ȫ�ֶ���
// ========================================================================================================================================
ServerGameplayState* g_ServerGameplayState = NULL;
ServerGameplayState gServerGameplayState;

// ========================================================================================================================================
//	Constructor & Destructor
// ========================================================================================================================================
ServerGameplayState::ServerGameplayState()
{
	g_ServerGameplayState	= this;
	mClientMisFileCrc		= INITIAL_CRC_VALUE;
	mClientMisFileName		= NULL;
	mZoneId					= 0;
	m_pGateLink				= new TCPObject2;
	m_pDataAgentHelper		= new CDataAgentHelper;
    m_pLogHelper            = new CLogHelper;
	m_bIsRobotTest			= false;
	m_StartHandle			= NULL;
	m_StopHandle			= NULL;
}

ServerGameplayState::~ServerGameplayState()
{
	delete m_pGateLink;
	delete m_pDataAgentHelper;
    delete m_pLogHelper;
}

const char* g_genExitEventName(void)
{
    char path[256] = {0};
    GetModuleFileNameA(0,path,sizeof(path));

    static std::string eventName;

    eventName = "e_";

    char buf[256] = {0};
    _splitpath(path,0,0,buf,0);
    eventName += buf;

    itoa(GetCurrentProcessId(),buf,10);
    eventName += buf;

    return eventName.c_str();
}

void ServerGameplayState::Initialize()
{
	mZoneId = Con::getIntVariable("ZoneID");//��õ�ͼ�������ı��
	Con::setIntVariable("ZoneID", mZoneId);

    m_StopHandle = CreateEventA( NULL, FALSE, FALSE, g_genExitEventName());

	//���õ�ͼ����������̨���ڱ���
	{
		char szTitle[128];
		Platform::LocalTime lt;
		Platform::getLocalTime(lt);
		dSprintf(szTitle, sizeof(szTitle),"MapID[%d]  Port[%d] Startup[%4d-%02d-%02d %02d:%02d:%02d]", 
						mZoneId, 
						0,
						lt.year + 1900,lt.month + 1,lt.monthday,lt.hour,lt.min,lt.sec);
		SetConsoleTitleA(szTitle);
	}

	char GateIpPort[COMMON_STRING_LENGTH];
	dStrcpy(GateIpPort, sizeof(GateIpPort), Con::getVariable("$Pref::Net::Gate"));

	char *portString = dStrchr(GateIpPort, ':');
	U16 port = 0;
	if(portString)
	{
		*portString++ = 0;
		port = htons(dAtoi(portString));
	}

	setGateIP(inet_addr(GateIpPort));
	setGatePort(port);

	Con::executef( "enableWinConsole", "true" );

	m_pGateLink->connect(Con::getVariable("$Pref::Net::Gate"));

	m_pDataAgentHelper->Init( Con::getVariable("$Pref::Net::DataAgentIp"), atoi( Con::getVariable("$Pref::Net::DataAgentPort") ) );
    m_pLogHelper->connect( Con::getVariable("$Pref::Net::LogIp"), atoi( Con::getVariable("$Pref::Net::LogPort") ) );
	// -------------------------------------- ��ʼ��data --------------------------------------

	//��������
	g_SoundManager->Read();
	// ģ������
	g_TSShapeRepository.Read();
	// ģ������
	g_ShapesSetRepository.Read();
	// AI����
	g_AIRepository.read();
	// Npc����
	g_NpcRepository.Read();
	// �������
	g_PlayerRepository.Read();
	//�ɼ���������
	gCollectionRepository.Read();
	//�¼�����������
	gEventTriggerRepository.read();
	// ����������
	g_TriggerDataManager->ReadTriggerData();
	//��Ʒ����
	g_ItemRepository->read();
	g_RandPropertyTable->read();
	// ״̬����
	g_BuffRepository.Read();
	// ��������
	g_SkillRepository.Read();
	// ����NPC�̵��б�����
	g_ShopListRepository->read();

	//��Ч����
	g_EffectDataRepository.Read();

	//��Ʒ������
	g_ItemPackageRepository.read();

	//�����������
	g_DropRuleRepository.read();

	//Ѱ������
	g_RouteRepository.read();

	//���������
	g_LivingSkillRespository.read();
	//�䷽����
	g_PrescriptionRepository.read();
	//��������
	g_PetRepository.Read();
	//��������
	g_MountRepository.Read();
	//������������
	g_PetStudyRepository.Read();
	//Ԫ������
	g_SpiritRepository.read();
	g_TalentRepository.read();
	//�̳�
	g_MarketItemReposity->read();
	g_PackGoodsResposity->read();
}

void ServerGameplayState::preShutdown()
{
	g_PrizeBoxManager.Clear();
}

void ServerGameplayState::Shutdown()
{
	g_SoundManager->Clear();
	g_TSShapeRepository.Clear();
	g_ShapesSetRepository.Clear();
	g_AIRepository.clear();
	g_NpcRepository.Clear();
	g_PlayerRepository.Clear();
	g_TriggerDataManager->clearTriggerData();
	g_ItemRepository->clear();
	g_BuffRepository.Clear();
	g_SkillRepository.Clear();
	g_ShopListRepository->clear();
	g_EffectDataRepository.Clear();
	g_LivingSkillRespository.clear();
	g_PrescriptionRepository.clear();
	g_PetRepository.Clear();
	g_MountRepository.Clear();
	g_PetStudyRepository.Clear();
	g_MarketItemReposity->clear();
	g_PackGoodsResposity->clear();
	g_SpiritRepository.clear();
	g_TalentRepository.clear();
}

void ServerGameplayState::TimeLoop(S32 elapsedTime)
{
	U32 curTime = Platform::getTime();

	//����Ч�������ƽ�
	g_EffectPacketContainer.advanceTime(elapsedTime);
	//��Ʒ�б�ʱˢ��
	g_ShopListRepository->update(curTime);
	
	//��ͼ��������ʱ�¼�(0:00:00ʱ��ʱ����)
	//ע:Ŀǰ��ʱ���ִ��������ԡ���ӷ�������ʱ���¼�����ʽ����
	Platform::LocalTime lt;
	Platform::getDateTime(curTime, lt);
	static bool bNoUpdated = true;
	if(lt.hour == 0 && lt.min == 0 && bNoUpdated)
	{
		bNoUpdated = false;
		Con::executef("CommonTimerEvent");
		HashPlayer::iterator itB = m_PlayerManager.begin();
		for(; itB != m_PlayerManager.end(); ++itB)
		{
			Player* player = itB->second;
			//�������ѭ���������ʱ��
			g_MissionManager->ClearCycleMissionTime(player);
			Con::executef(player, "CommonPlayerTimerEvent");
		}
	}

	if(lt.hour == 0 && lt.min == 1)
		bNoUpdated = true;	
	
	if(!(curTime%5))		//5����һ��
	{
		if(m_pGateLink && m_pGateLink->getState()==TCPObject::Disconnected)
		{
			const char *pGateAddress = Con::getVariable("$Pref::Net::Gate");
			m_pGateLink->connect(pGateAddress);
		}

		// �z��DataAgent�B�� 
		m_pDataAgentHelper->Check();
        m_pLogHelper->check();
	}

	//if(!(curTime%60))		//2���Ӽ��һ��
	//{
	//	FilterPlayerLoginData(curTime);
	//}
}

void ServerGameplayState::DeletePlayerLoginData(T_UID UID,int PlayerId)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(PlayerId);

	if(it == m_PlayerLoginData.end())
		return;

	if( it->second.UID == UID )
	{
		if( it->second.pPlayerData )
			delete it->second.pPlayerData;

		m_PlayerLoginData.erase(it);
	}
}

void ServerGameplayState::AddPlayerLoginData(T_UID UID, stTeamInfo* pTeamInfo, stPlayerStruct *pPlayerData)
{
	PlayerLoginBlock block;
	block.UID			= UID;
	block.pPlayerData	= pPlayerData;
	block.Time			= time(NULL);
	memcpy( &(block.teamInfo), pTeamInfo, sizeof( stTeamInfo ) );

	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(pPlayerData->BaseData.PlayerId);
	if(it != m_PlayerLoginData.end())
	{
		delete it->second.pPlayerData;
	}

	// [LivenHotch]: ����������䷽ʽ
	memcpy( &(m_PlayerLoginData[pPlayerData->BaseData.PlayerId]), &block, sizeof( PlayerLoginBlock ) );
}

stPlayerStruct *ServerGameplayState::GetPlayerLoginData(T_UID UID,int PlayerId)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(PlayerId);
	if(it == m_PlayerLoginData.end())
		return NULL;

	PlayerLoginBlock& block = it->second;

	if(block.UID == UID)
		return block.pPlayerData;

	return NULL;
}

stPlayerStruct* ServerGameplayState::GetPlayerLoginData( int playerId )
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(playerId);
	if(it == m_PlayerLoginData.end())
		return NULL;

	PlayerLoginBlock& block = it->second;

	return block.pPlayerData;
}

void ServerGameplayState::FilterPlayerLoginData(int curTime)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.begin();
	while(it!=m_PlayerLoginData.end())
	{
		PlayerLoginBlock& block = it->second;
		if(curTime - block.Time> 5*60 ) //5����
		{
			delete block.pPlayerData;
			m_PlayerLoginData.erase(it++);
		}
		else
			it++;
	}

}

ServerPacketProcess* ServerGameplayState::GetPacketProcess()								
{
	return (ServerPacketProcess *)m_pGateLink->GetPacket(); 
}

CLogHelper* ServerGameplayState::GetLog(void)                                                        
{
    if (0 != m_pLogHelper)
        return m_pLogHelper;

    static CLogHelper dummy;
    return &dummy;
}

void ServerGameplayState::AddInPlayerManager(Player *pPlayer)
{
	if( !pPlayer )
		return ;


	HashPlayer::iterator itp = m_PlayerManager.find(pPlayer->getPlayerID());
	if(itp != m_PlayerManager.end())
	{
		Player *pOld = itp->second;
		GameConnection *pCon = pOld->getControllingClient();
		if(pCon)
		{
			if( !m_bIsRobotTest )
			{
				T_UID uid  = pPlayer->getUID();
				stPlayerStruct* pPlayerData = GetPlayerLoginData( pPlayer->getUID(), pPlayer->getPlayerID() );
				PlayerLoginBlock pb = FakePlayerLoginData( pPlayer->getPlayerID() );
				stPlayerStruct* pNewPlayerData = new stPlayerStruct();

				memcpy( pNewPlayerData, pPlayerData, sizeof( stPlayerStruct ) );
				stTeamInfo info;
				memcpy( &info, &pb.teamInfo, sizeof( stTeamInfo ) );
				
				pCon->deleteObject();		// �ɵ�������
				
				AddPlayerLoginData( uid, &info, pNewPlayerData );
			}
		}
	}

	m_PlayerManager[pPlayer->getPlayerID()] = pPlayer;

	m_AccountMap[pPlayer->getPlayerID()] = pPlayer->getAccountID();

	// ֪ͨWORLDSERVER�������ȷ���ѵ�½��ZONE����������
	char buf[100];
	Base::BitStream sendPacket( buf, 100 );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_PlayerLogin, pPlayer->getPlayerID(), SERVICE_WORLDSERVER );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	GetPacketProcess()->Send( sendPacket );

	// ֪ͨ������ҽ���
	if( pPlayer->getPlayerID() > 1 )
	{
		char CopyMapId[8];
		sprintf_s( CopyMapId, 8, "%d", getZoneId() );
		CopyMapId[4] = 0;

		Con::executef("OnCopyMapPlayerEnter", Con::getIntArg( pPlayer->getPlayerID() ), CopyMapId );
	}
}

void ServerGameplayState::DeleteFromPlayerManager(Player *pPlayer)
{
	if( !pPlayer )
		return ;

	// ֪ͨ��������뿪
	if( pPlayer->getLayerID() > 1 )
	{
		char CopyMapId[8];
		sprintf_s( CopyMapId, 8, "%d", getZoneId() );
		CopyMapId[4] = 0;

		Con::executef("OnCopyMapPlayerLeave", Con::getIntArg( pPlayer->getPlayerID() ), CopyMapId );
	}

	if( GetPlayer( pPlayer->getPlayerID()) == NULL )
		return ;

	if( !pPlayer->isTransporting() )
		pPlayer->saveToWorld();

	m_PlayerManager.erase(pPlayer->getPlayerID());

	m_AccountMap.erase( pPlayer->getAccountID() );

	g_ServerGameplayState->DeletePlayerLoginData( pPlayer->getUID(), pPlayer->getPlayerID() );
/*
	GameConnection *pCon = pPlayer->getControllingClient();
	if(pCon)
	{
		pCon->deleteObject();		// �ɵ�������
	}*/

}

Player *ServerGameplayState::GetPlayer(int PlayerId)
{
	HashPlayer::iterator itp = m_PlayerManager.find(PlayerId);
	if(itp != m_PlayerManager.end())
	{
		return itp->second;
	}

	return NULL;
}

stTeamInfo* ServerGameplayState::GetPlayerTeam(T_UID UID,int PlayerId)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(PlayerId);
	if(it == m_PlayerLoginData.end())
		return NULL;

	PlayerLoginBlock& block = it->second;

	if(block.UID == UID)
		return &(block.teamInfo);

	return NULL;
}

// copy map related functions

void ServerGameplayState::openCopyMap(int nCopyMapInstId)
{
	m_copyMapSet.insert( nCopyMapInstId );
}

void ServerGameplayState::closeCopyMap(int nCopyMapInstId)
{
	m_copyMapSet.erase( nCopyMapInstId );
}

bool ServerGameplayState::isCopyMapOpen(int nCopyMapInstId)
{
	return m_copyMapSet.find( nCopyMapInstId ) != m_copyMapSet.end();
}

stPlayerStruct* ServerGameplayState::GetPlayerSaveData( U32 playerId )
{
	HashPlayerSaveMap::iterator it;
	it = m_PlayerSaveMap.find( playerId );
	if( it == m_PlayerSaveMap.end() )
		return NULL;

	return it->second;
}

void ServerGameplayState::AddPlayerSaveData( stPlayerStruct* pPlayer )
{
	HashPlayerSaveMap::iterator it;
	it = m_PlayerSaveMap.find( pPlayer->BaseData.PlayerId );
	if( it != m_PlayerSaveMap.end() )
	{
		delete it->second;
		it->second = pPlayer;
	}
	else
	{
		m_PlayerSaveMap[pPlayer->BaseData.PlayerId] = pPlayer;
	}
}

void ServerGameplayState::RemovePlayerSaveData( U32 playerId )
{
	HashPlayerSaveMap::iterator it;
	it = m_PlayerSaveMap.find( playerId );
	if( it == m_PlayerSaveMap.end() )
		return ;

	delete it->second;

	m_PlayerSaveMap.erase( it );
}

Player * ServerGameplayState::GetPlayerByAccount( int accountId )
{
	HashAccount::iterator it;
	it = m_AccountMap.find( accountId );
	if( it == m_AccountMap.end() )
		return NULL;

	return GetPlayer( it->second );
}

PlayerLoginBlock ServerGameplayState::FakePlayerLoginData( int PlayerId )
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(PlayerId);
	if(it == m_PlayerLoginData.end())
		return PlayerLoginBlock();

	PlayerLoginBlock playerObj = it->second;

	return playerObj;
}

ConsoleFunction( SetRobotMode, void, 2, 2, "" )
{
	g_ServerGameplayState->m_bIsRobotTest = atoi( argv[1] );
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ��ͼ���
ConsoleFunction( GetZoneID, S32, 1, 1, "GetZoneID()" )
{
	return g_ServerGameplayState->getZoneId();
}






