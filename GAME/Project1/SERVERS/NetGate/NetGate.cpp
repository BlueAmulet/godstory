// WorldServer.cpp : 定义控制台应用程序的入口点。
//

#include <signal.h>
#include "DBLib/dbPool.h"
#include "Common/Script.h"
#include "Common/CommonServer.h"
#include "Common/CommonClient.h"
#include "Common/WorkQueue.h"
#include "Common/UDP.h"
#include "common/DataAgentHelper.h"

#include "NetGate.h"
#include "ServerPacketProcess.h"
#include "EventProcess.h"
#include "UserPacketProcess.h"
#include "UserEventProcess.h"
#include "WorldClientPacketProcess.h"

#include "Common/MemoryShare.h"

#include "PlayerMgr.h"
#include "ServerMgr.h"

#include "encrypt/AuthCrypt.h"
//#include "encrypt/BigNumber.h"

SERVER_CLASS_NAME *	SERVER_CLASS_NAME::m_pInstance		=	NULL;
HANDLE				SERVER_CLASS_NAME::m_ExitEvent		=	NULL;
CScript *			SERVER_CLASS_NAME::m_pScript		=	NULL;
CDBConnPool *		SERVER_CLASS_NAME::m_pDBConnPool	=	NULL;
CWorkQueue *		SERVER_CLASS_NAME::m_pWorkQueue		=	NULL;
CWorkQueue *		SERVER_CLASS_NAME::m_pUserWorkQueue	=	NULL;

const char GLOBAL_EXIT_EVENT[] = "flt_netgate";

SERVER_CLASS_NAME::SERVER_CLASS_NAME()
{
	m_Inited			=	false;
	m_pServerService	=	NULL;
	m_pUserService		=	NULL;
	m_pWorldClient		=	NULL;
	m_pDBConnPool		=	NULL;
	m_pScript			=	NULL;
	m_pWorkQueue		=	NULL;
	m_pUserWorkQueue	=	NULL;
	m_pPlayerManager	=	NULL;
	m_pServerManager	=	NULL;

	m_pUDPServerSide	=	NULL;
	m_pUDPClientSide	=	NULL;

	ISocket::InitNetLib();
}

SERVER_CLASS_NAME::~SERVER_CLASS_NAME()
{
	if(m_pServerService)		delete m_pServerService;
	if(m_pUserService)			delete m_pUserService;
	if(m_pWorldClient)			delete m_pWorldClient;
	if(m_pDBConnPool)			delete m_pDBConnPool;
	if(m_pScript)				delete m_pScript;
	if(m_pWorkQueue)			delete m_pWorkQueue;
	if(m_pUserWorkQueue)		delete m_pUserWorkQueue;
	if(m_pPlayerManager)		delete m_pPlayerManager;
	if(m_pServerManager)		delete m_pServerManager;

	if(m_pUDPServerSide)		delete m_pUDPServerSide;
	if(m_pUDPClientSide)		delete m_pUDPClientSide;

	m_pInstance = NULL;
	ISocket::UninitNetLib();
}

void SERVER_CLASS_NAME::OnBreak(int)
{
	if(SERVER_CLASS_NAME::m_ExitEvent)
		SetEvent(SERVER_CLASS_NAME::m_ExitEvent);
}

const char* g_genExitEventName(void)
{
    char path[256] = {0};
    GetModuleFileName(0,path,sizeof(path));

    static std::string eventName;

    eventName = "e_";

    char buf[256] = {0};
    _splitpath(path,0,0,buf,0);
    eventName += buf;

    itoa(GetCurrentProcessId(),buf,10);
    eventName += buf;

    return eventName.c_str();
}

bool SERVER_CLASS_NAME::InstallBreakHandlers()
{
    signal(SIGBREAK, OnBreak);
    signal(SIGINT, OnBreak);

    m_ExitEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,g_genExitEventName());

    if(m_ExitEvent)
    {
        return true; 
    }

    m_ExitEvent = CreateEvent(NULL,TRUE,FALSE,g_genExitEventName());
	return true;
}

void *Create_UserPacketProcess()
{
	return new UserPacketProcess;
}

void *Create_ServerPacketProcess()
{
	return new ServerPacketProcess;
}

void SERVER_CLASS_NAME::ShowMessage()
{
	printf("**********************************************************\n");
	printf("*\tUserCount\t\t\t%d\n", SCRIPT->get<int>("UserCount"));
	printf("*\tMaxUserCount\t\t\t%d\n", SCRIPT->get<int>("MaxUserCount"));
	printf("*\tUserNetIP\t\t\t%s:%d\n", SCRIPT->get<CStr>("UserNetIP"), SCRIPT->get<int>("UserNetPort"));
	printf("*\tWorldServerIP\t\t\t%s:%d\n", SCRIPT->get<CStr>("WorldServerIP"), SCRIPT->get<int>("WorldServerPort"));
	printf("*\tNetLineId\t\t\t%d\n", SCRIPT->get<int>("NetLineId"));
	printf("*\tNetGateId\t\t\t%d\n", SCRIPT->get<int>("NetGateId"));
	printf("**********************************************************\n");
}

void SERVER_CLASS_NAME::OnServerStarted()
{
	printf("*****************游戏网关服务已启动****************************\n");
}


bool SERVER_CLASS_NAME::InitializeDatabase()
{
	return true;

	if(!m_pScript)
		return false;

	m_pDBConnPool = new CDBConnPool;

	int nCount = SCRIPT->get<int>("MaxDBConnection");
	const char* szServer = SCRIPT->get<CStr>("DB_Server");
	const char* szUserId = SCRIPT->get<CStr>("DB_UserId");
	const char* szPassword = SCRIPT->get<CStr>("DB_Password");
	const char* szDBName = SCRIPT->get<CStr>("DB_Name");

	return m_pDBConnPool->Open(nCount
		,szServer
		,szUserId
		,szPassword
		,szDBName

		);
}

bool SERVER_CLASS_NAME::Initialize()
{
	if(m_Inited)
		return true;
	
	if(	!InstallBreakHandlers())
		return false;
   
	//设置日志文件名
	g_Log.SetLogName("NetGateLog");

	//执行初始化脚本
	m_pScript = CScript::GetInstance();
	m_pScript->Initialize();
	SCRIPT->loadFile(".\\NetGate.c");

	LineId = SCRIPT->get<int>("NetLineId");
	GateId = SCRIPT->get<int>("NetGateId");

	ShowMessage();
	//初始化数据库连接
	if(!InitializeDatabase())
		return false;

#ifdef ENABLE_ENCRPY_PACKET
    //初始化数据包的发送key
    //char keyarray[] = {"sxz12345"};
    //BigNumber key;
    //key.SetBinary((uint8*)keyarray,sizeof(keyarray));
    AuthCrypt::Instance()->SetKey(0);
#endif

	//初始化工作队列
	m_pWorkQueue = new CWorkQueue;
	m_pWorkQueue->Initialize(EventProcess,10);
	m_pUserWorkQueue = new CWorkQueue;
	m_pUserWorkQueue->Initialize(UserEventProcess,10);

	m_pServerManager = new CServerManager;
	m_pServerManager->Initialize();

	m_pPlayerManager = new CPlayerManager;

	m_StartEvent = INVALID_HANDLE_VALUE;

	//消息处理绑定
	EventFn::Initialize();
	UserEventFn::Initialize();

	stServerParam Param;
	//开放给地图服务器的服务
	m_pServerService	=	new CommonServer<ServerPacketProcess>;  //CommonServer<>(Create_UserPacketProcess);
	Param.MinClients	=	SCRIPT->get<int>("RemoteCount");
	Param.MaxClients	=	SCRIPT->get<int>("MaxRemoteCount");
	m_pServerService->Initialize(SCRIPT->get<CStr>("RemoteNetIP"),SCRIPT->get<int>("RemoteNetPort"),&Param);

	//开放给玩家的服务
	m_pUserService		=	new CommonServer<UserPacketProcess>;  //CommonServer<>(Create_UserPacketProcess);
	// 暂时去掉，目前Client与NetGate之间几乎没有什么通讯
	//m_pUserService->SetConnectionTimeout(60*5);						//5分钟连接超时
	Param.MinClients	=	SCRIPT->get<int>("UserCount");
	Param.MaxClients	=	SCRIPT->get<int>("MaxUserCount");
	m_pUserService->Initialize(SCRIPT->get<CStr>("UserNetIP"),SCRIPT->get<int>("UserNetPort"),&Param);

	//连接世界服务器
	m_pWorldClient		=	new CommonClient<WorldClientPacketProcess>;	//CommonClient<>(Create_WorldClientPacketProcess)
	m_pWorldClient->Initialize(SCRIPT->get<CStr>("WorldServerIP"), SCRIPT->get<int>("WorldServerPort"));
	m_pWorldClient->SetConnectType(ISocket::SERVER_CONNECT);

	//UDP转发
    m_pUDPClientSide	=	new CUDPServer(SCRIPT->get<CStr>("UserNetIP"),SCRIPT->get<int>("UserNetPort"),CUDPServer::UDP_SIDE_CLIENT);
	m_pUDPServerSide	=	new CUDPServer(SCRIPT->get<CStr>("RemoteNetIP"),SCRIPT->get<int>("RemoteNetPort"),CUDPServer::UDP_SIDE_ZONE);
	m_pUDPClientSide->SetOutServer(m_pUDPServerSide);
	m_pUDPServerSide->SetOutServer(m_pUDPClientSide);

	//连接DataAgent
	const char* strIp = SCRIPT->get<CStr>("DataAgentIP");
	const int nPort = SCRIPT->get<int>("DataAgentPort");
	m_dataAgentHelper.Init( strIp, nPort );

	m_Inited	= true;
	return m_Inited;
}

void SERVER_CLASS_NAME::StartService()
{
	if(m_pWorldClient)		m_pWorldClient->Start();
}

void SERVER_CLASS_NAME::OnServerStart()
{
	if(m_pServerService)
	{
		if(m_pServerService->GetState() == ISocket::SSF_DEAD)
			m_pServerService->Start();
	}

	if(m_pUserService)
	{
		if(m_pUserService->GetState() == ISocket::SSF_DEAD)
		{
			m_pUserService->Start();
			OnServerStarted();

			m_StartEvent = CreateEventA( NULL, FALSE, FALSE, "fl_netgate" );

			if(m_pUDPClientSide)	m_pUDPClientSide->Start();
			if(m_pUDPServerSide)	m_pUDPServerSide->Start();
		}
	}
}

void SERVER_CLASS_NAME::Maintenance()
{
	while(1)
	{
		if(WaitForSingleObject(m_ExitEvent,2000) == WAIT_OBJECT_0)
			break;

		if(m_pWorldClient)
		{
			if(m_pWorldClient->GetState() == ISocket::SSF_DEAD)
			{
				m_pWorldClient->Restart();
			}
		}

		// 时不时的要检查与DataAgent的连接
		//m_dataAgentHelper.Check();
	}
}

void SERVER_CLASS_NAME::DenyService()
{

}

void SERVER_CLASS_NAME::StopService()
{
	if(m_pServerService)	m_pServerService->Stop();
	if(m_pWorldClient)		m_pWorldClient->Stop();
	if(m_pWorkQueue)		m_pWorkQueue->Stop();

	if(m_pUDPClientSide)	m_pUDPClientSide->Stop();
	if(m_pUDPServerSide)	m_pUDPServerSide->Stop();

	if( m_StartEvent != INVALID_HANDLE_VALUE )
		CloseHandle( m_StartEvent );

	ResetEvent(m_ExitEvent);
}








