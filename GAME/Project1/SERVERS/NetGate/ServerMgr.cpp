
#include "DBLib/dbLib.h"
#include "Base/Log.h"
#include "wintcp/dtServerSocket.h"
#include "Common/PacketType.h"
#include "ServerMgr.h"
#include "NetGate.h"

using namespace std;

CServerManager::CServerManager()
{
	mDataAgentSocket = 0;
}

CServerManager::~CServerManager()
{
}

void CServerManager::AddServerMap(stServerInfo &ServerInfo)
{
	CLocker lock(m_cs);

	HashSocketMap::iterator sit = m_SocketMap.find(ServerInfo.SocketId);
	if(sit != m_SocketMap.end())
	{
		ReleaseServerMap( sit->second->SocketId, true );
	}

	stServerInfo *pServerInfo = new stServerInfo;
	*pServerInfo	=	ServerInfo;

	m_SocketMap[ServerInfo.SocketId]	= pServerInfo;
	m_ZoneMap[ServerInfo.ZoneId]		= pServerInfo;
}

stServerInfo* CServerManager::GetZoneInfo( int socketId )
{
	HashSocketMap::iterator sit = m_SocketMap.find(socketId);
	if(sit != m_SocketMap.end())	
		return sit->second;

	return NULL;
}

void CServerManager::ReleaseServerMap(int SocketId,bool postEvent)
{
	CLocker lock(m_cs);

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
		return;

	stServerInfo* pServerInfo = sit->second;

	m_ZoneMap.erase(pServerInfo->ZoneId);
	m_SocketMap.erase(pServerInfo->SocketId);
	delete pServerInfo;

	if(postEvent)
		SERVER->GetServerSocket()->PostEvent(dtServerSocket::OP_CODE::OP_RESTART,SocketId);
}

int CServerManager::GetZoneSocketId(int ZoneId)
{
	HashZoneMap::iterator slit = m_ZoneMap.find(ZoneId);
	if(slit == m_ZoneMap.end())
		return 0;

	stServerInfo *pServerInfo = slit->second;
	return pServerInfo->SocketId;
}

stServerInfo *CServerManager::GetServerInfo(int ZoneId)
{
	HashZoneMap::iterator slit = m_ZoneMap.find(ZoneId);
	if(slit == m_ZoneMap.end())
		return NULL;

	return slit->second;
}

bool CServerManager::Initialize()
{
	return true;
}

void CServerManager::UnpackZoneData(Base::BitStream *pPacket)
{
	CLocker lock(m_cs);

	stZoneInfo ZoneInfo;
	int Num = pPacket->readInt(Base::Bit16);
	for(int i=0;i<Num;i++)
	{
		ZoneInfo.ZoneId			= pPacket->readInt(Base::Bit32);
		ZoneInfo.MaxPlayerNum	= pPacket->readInt(Base::Bit16);
		ZoneInfo.ServicePort	= pPacket->readInt(Base::Bit16);
		m_ZoneInfoList[ZoneInfo.ZoneId] = ZoneInfo;
	}
}

bool CServerManager::GetZoneInfo(int ZoneId,stZoneInfo &Info)
{
	CLocker lock(m_cs);

	HashZoneInfo::iterator it = m_ZoneInfoList.find(ZoneId);
	if(it == m_ZoneInfoList.end())
		return false;

	Info = it->second;
	return true;
}

void CServerManager::PackZoneMgrList(Base::BitStream &Packet)
{
	CLocker lock(m_cs);

	Packet.writeInt((int)m_ZoneMap.size(),Base::Bit8);
	HashZoneMap::iterator zit = m_ZoneMap.begin();
	while(zit!=m_ZoneMap.end())
	{
		Packet.writeInt(zit->first,Base::Bit32);
		Packet.writeInt(zit->second->Ip,Base::Bit32);
		Packet.writeInt(zit->second->Port,Base::Bit16);
		Packet.writeInt(zit->second->ConnectSeq,Base::Bit32);
		zit++;
	}
}

int CServerManager::GetDataAgentSocket()
{
	return mDataAgentSocket;
}

void CServerManager::SetDataAgentSocket( int socketId )
{
	mDataAgentSocket = socketId;
}