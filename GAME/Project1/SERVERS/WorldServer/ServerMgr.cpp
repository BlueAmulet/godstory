
#include "DBLib/dbLib.h"
#include "Base/Log.h"
#include "wintcp/dtServerSocket.h"
#include "Common/PacketType.h"
#include "ServerMgr.h"
#include "ChatMgr.h"
#include "TeamManager.h"
#include "WorldServer.h"
#include "PlayerMgr.h"
#include "Common/WorkQueue.h"
#include "Common/DumpHelper.h"

using namespace std;

CServerManager::CServerManager()
{
	m_pAccountInfo	= NULL;

	gLock.registerLockable( this, Lock::Server );
}

CServerManager::~CServerManager()
{
	if(m_pAccountInfo)
		delete m_pAccountInfo;
}

void CServerManager::AddServerMap(stServerInfo &ServerInfo)
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator sit = m_SocketMap.find(ServerInfo.SocketId);
	if(sit != m_SocketMap.end())
		delete sit->second;

	stServerInfo *pServerInfo = new stServerInfo;
	*pServerInfo	=	ServerInfo;
	m_SocketMap[ServerInfo.SocketId]	= pServerInfo;

	switch(ServerInfo.Type)
	{
	case SERVICE_ACCOUNTSERVER:
		m_pAccountInfo = pServerInfo;
		break;

	case SERVICE_GATESERVER:
		m_LineGateMap[ServerInfo.LineId][ServerInfo.GateId] = pServerInfo;
		in_addr ia;
		ia.s_addr = pServerInfo->Ip;
		printf("ADD GATE SERVER: [%d]-[%d]-[%s:%d]\n\r", pServerInfo->LineId, pServerInfo->GateId, inet_ntoa( ia ), ntohs( pServerInfo->Port ) );
		break;
	}
	
}

void CServerManager::ReleaseServerMap(int SocketId,bool postEvent)
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
		return;

	stServerInfo* pServerInfo = sit->second;

	switch(pServerInfo->Type)
	{
	case SERVICE_ACCOUNTSERVER:
		m_pAccountInfo = NULL;
		break;

	case SERVICE_GATESERVER:
		m_LineGateMap[pServerInfo->LineId].erase(pServerInfo->GateId);
		if(m_LineGateMap[pServerInfo->LineId].size() == 0)
			m_LineGateMap.erase(pServerInfo->LineId);
		
		break;
	}

	m_SocketMap.erase(pServerInfo->SocketId);
	delete pServerInfo;

	if(postEvent)
		SERVER->GetServerSocket()->PostEvent(dtServerSocket::OP_CODE::OP_RESTART,SocketId);
}

stServerInfo *CServerManager::GetServerInfo(int Type,int LineId,int GateId)
{
	switch(Type)
	{
	case SERVICE_ACCOUNTSERVER:
		return m_pAccountInfo;

	case SERVICE_GATESERVER:
		{
			HashLineGateMap::iterator slit = m_LineGateMap.find(LineId);
			if(slit == m_LineGateMap.end())
				return NULL;

			HashSocketMap::iterator sit = slit->second.find(GateId);
			if(sit == slit->second.end())
				return NULL;

			return sit->second;
		}
	}

	return NULL;
}

stServerInfo *CServerManager::GetServerInfoBySocket(int SocketId)
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
		return NULL;

	return sit->second;
}

void CServerManager::RemoveZoneFromGate(int SocketId, int ZoneId)
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
		return;

	stServerInfo* pServerInfo = sit->second;
	if(pServerInfo->Type!=SERVICE_GATESERVER)
		return;

	m_LineZoneMap[pServerInfo->LineId].erase(ZoneId);
}

int CServerManager::GetZoneSocket(int LineId, int ZoneId )
{
	DO_LOCK( Lock::Server );

	HashLineZoneMap::iterator it = m_LineZoneMap.find( LineId );
	if( it == m_LineZoneMap.end() )
		return 0;

	HashZoneMap::iterator itz = it->second.find( ZoneId );
	if( itz == it->second.end() )
		return 0;

	return itz->second.Socket;
}

stServerZone* CServerManager::GetZoneInfo(int SocketId, int ZoneId)
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
		return NULL;

	stServerInfo* pServerInfo = sit->second;
	if(pServerInfo->Type!=SERVICE_GATESERVER)
		return NULL;

	HashZoneMap::iterator itz  = m_LineZoneMap[pServerInfo->LineId].find(ZoneId);
	if( itz != m_LineZoneMap[pServerInfo->LineId].end() )
		return &(itz->second );

	return NULL;
}

void CServerManager::AddZoneToGate(int SocketId,int ZoneId,int Ip,int Port,int ConnectSeq)
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
		return;

	stServerInfo* pServerInfo = sit->second;
	if(pServerInfo->Type!=SERVICE_GATESERVER)
		return;

	stServerZone ServerZoneInfo;
	ServerZoneInfo.pGateInfo = pServerInfo;
	ServerZoneInfo.ZoneIp = Ip;
	ServerZoneInfo.ZonePort = Port;
	ServerZoneInfo.ConnectSeq = ConnectSeq;
	ServerZoneInfo.Socket = SocketId;

	sockaddr_in addr;
	addr.sin_addr.s_addr = Ip;
	in_addr ia;
	ia.s_addr = Ip;
	printf("ADD ZONE [%d]-[%d]-[%s:%d]\n\r",pServerInfo->LineId, ZoneId, inet_ntoa( ia ), Port );

	HashZoneMap::iterator itz  = m_LineZoneMap[pServerInfo->LineId].find(ZoneId);
	if(itz!=m_LineZoneMap[pServerInfo->LineId].end())
	{
		if(itz->second.ConnectSeq > ConnectSeq)
		{
			//这里表示地图服务器宕机重启了，这里可能需要清理玩家
			//使用事件方式驱动，防止锁混乱
			int Param[2];
			Param[0] = pServerInfo->LineId;
			Param[1] = ZoneId;
			SERVER->GetWorkQueue()->PostEvent(ON_ZONE_RESTART, Param, sizeof(Param), true, WQ_LOGIC );
		}
	}

	m_LineZoneMap[pServerInfo->LineId][ZoneId] = ServerZoneInfo;
}

int CServerManager::GetLineId(int SocketId)
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
	{
		AssertFatal( true, "this can not happen !" );
		return 0;
	}

	int lineId = sit->second->LineId;
	g_Log.WriteLog( "读取LineId[%d]:%d", SocketId, lineId );
	return lineId;
}

int CServerManager::GetSocketId(int SocketId,int ZoneId)
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
		return 0;

	int LineId = sit->second->LineId;
	HashLineZoneMap::iterator lzit = m_LineZoneMap.find(LineId);
	if(lzit==m_LineZoneMap.end())
		return 0;

	HashZoneMap::iterator zsit = lzit->second.find(ZoneId);
	if(zsit==lzit->second.end())
		return 0;

	stServerInfo* pServerInfo = zsit->second.pGateInfo;
	return pServerInfo->SocketId;
}

bool CServerManager::GetZoneIpPort(int LineId,int ZoneId,int &Ip,int &Port)
{
	DO_LOCK( Lock::Server );

	HashLineZoneMap::iterator lzit = m_LineZoneMap.find(LineId);
	if(lzit==m_LineZoneMap.end())
		return false;

	HashZoneMap::iterator zsit = lzit->second.find(ZoneId);
	if(zsit==lzit->second.end())
		return false;

	stServerZone& ServerZone= zsit->second;
	Ip = ServerZone.ZoneIp;
	Port = ServerZone.ZonePort;
	return true;
}

bool CServerManager::Initialize()
{
	DO_LOCK( Lock::Server );

	CDBOperator dbHandle(DATABASE);
	dbHandle->SQL("select MapId,MaxPlayerNum,ServicePort from ServerMapInfo order by MapId");
	try
	{
		stZoneInfo Info;
		while(dbHandle->More())
		{
			Info.ZoneId			= dbHandle->GetInt();
			Info.MaxPlayerNum	= dbHandle->GetInt();
			Info.ServicePort	= dbHandle->GetInt();

			m_ZoneInfoList[Info.ZoneId] = Info;
		}

		return true;
	}
	DB_CATCH_LOG(CServerManager::Initialize());

	return false;
}


void CServerManager::PackZoneData(Base::BitStream &Packet)
{
	DO_LOCK( Lock::Server );

	Packet.writeInt((int)m_ZoneInfoList.size(),Base::Bit16);
	HashZoneInfo::iterator it = m_ZoneInfoList.begin();
	while(it!=m_ZoneInfoList.end())
	{
		Packet.writeInt(it->second.ZoneId		,Base::Bit32);
		Packet.writeInt(it->second.MaxPlayerNum	,Base::Bit16);
		Packet.writeInt(it->second.ServicePort	,Base::Bit16);
		it++;
	}
}

void CServerManager::PackGateData(Base::BitStream &Packet)
{
	DO_LOCK( Lock::Server );

	Packet.writeInt((int)m_LineGateMap.size(),Base::Bit8);
	HashSocketMap::iterator itg;
	HashLineGateMap::iterator itl = m_LineGateMap.begin();
	while(itl != m_LineGateMap.end())
	{
		Packet.writeInt(itl->first			,Base::Bit8);
		Packet.writeInt((int)itl->second.size()	,Base::Bit8);
		itg = itl->second.begin();
		while(itg!=itl->second.end())
		{
			Packet.writeInt(itg->second->GateId	,Base::Bit16);
			Packet.writeInt(itg->second->Ip		,Base::Bit32);
			Packet.writeInt(itg->second->Port	,Base::Bit16);
			itg++;
		}
		itl++;
	}
}

// 得到所有的GateServer
void CServerManager::GetAllGates( list<stServerInfo*>& serverInfoList )
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator itg;
	HashLineGateMap::iterator itl = m_LineGateMap.begin();
	while(itl != m_LineGateMap.end())
	{
		itg = itl->second.begin();
		while(itg!=itl->second.end())
		{
			serverInfoList.push_back( itg->second );
			itg++;
		}
		itl++;
	}
}

void CServerManager::GetAllGates( std::list<stServerInfo*>& serverInfoList, int nLineId )
{
	DO_LOCK( Lock::Server );

	HashSocketMap::iterator itg;
	HashLineGateMap::iterator itl = m_LineGateMap.find( nLineId );
	if( itl != m_LineGateMap.end())
	{
		itg = itl->second.begin();
		while(itg!=itl->second.end())
		{
			serverInfoList.push_back( itg->second );
			itg++;
		}
	}
}


// 通过LineId,ZoneId得到相应的GateServer
//因为获取的数据是内部的，必须外部加锁，内部锁无效！！
//每个Line会有自己对应的一份Zone，必须指定Line才能找到Zone
stServerInfo* CServerManager::GetGateServerByZoneId(int nLineId, int nZoneId)
{
	char szBuf[512];
	sprintf_s(szBuf, 512, "地图[%d:%d]不存在", nLineId, nZoneId );
	HashLineZoneMap::iterator lit = m_LineZoneMap.find(nLineId);
	if(lit == m_LineZoneMap.end())
	{
		g_Log.WriteError( szBuf );
		g_Log.WriteError( DumpHelper::DumpStack() );
		return NULL;
	}

	HashZoneMap::iterator zit = lit->second.find( nZoneId );
	if( zit != lit->second.end() )
		return zit->second.pGateInfo;
	
	g_Log.WriteError( "地图错误：%s", szBuf );
	g_Log.WriteError( DumpHelper::DumpStack() );
	return NULL;
}

stServerInfo* CServerManager::GetGateServerByPlayerId( int nPlayerId )
{
	DO_LOCK( Lock::Player );

	stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	if( pAccount == NULL )
		return NULL;

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId );
	if( pPlayerData.isNull() )
		return NULL;

	return GetGateServerByZoneId(pAccount->LineId,pPlayerData->DispData.ZoneId );
}

stServerInfo* CServerManager::GetGateServer( int nLineId, int nGateId )
{
	return this->GetServerInfo( SERVICE_GATESERVER, nLineId, nGateId );
}


