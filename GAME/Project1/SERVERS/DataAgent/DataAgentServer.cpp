#include "DataAgentServer.h"
#include "PlayerMgr.h"


#include "Common/CommonClient.h"
#include "UserPacketProcess.h"

#include  <time.h>

//#include "Common/Log/LogTypes.h"

void CDataAgent::onInit()
{
	mPlayerManager = new CPlayerManager();
	/*
	mLogHelper = new CLogHelper();
		mLogHelper->connect( "192.168.1.135", 50000 );*/
	
	
	m_pNetGateClient = new CommonClient< NetGateClientPacketProcess >();
	char* worldServerIp = getScript()->get<char*>("WorldServerIP");
	int worldServerPort = getScript()->get<int>("WorldServerPort");
	printf("WorldServer [%s:%d]\n\r", worldServerIp, worldServerPort );
	m_pNetGateClient->Initialize( worldServerIp, worldServerPort );
	m_pNetGateClient->SetConnectType( ISocket::SERVER_CONNECT );

	m_pNetGateClient->Start();

	m_StartEvent = CreateEventA( NULL, FALSE, FALSE, "fl_dataagent" );

}

void CDataAgent::onShutdown()
{
	CloseHandle( m_StartEvent );

	delete mPlayerManager;
}

void CDataAgent::onMainLoop()
{
	// check the socket status
	// if the socket is down , we restart it
	int t = (int)time( NULL );

	// check the connection per 2 seconds
	if( t % 2 == 0 )
	{
		if( m_pNetGateClient->GetState() == ISocket::SSF_DEAD )
			m_pNetGateClient->Restart();
	}


}

CAsyncSocket2* CDataAgent::getNetGateSocket()
{
	return m_pNetGateClient;
}