#pragma once


#include <process.h>
#include "dtServerSocket.h"
#include "dtServerSocketClient.h"

bool dtServerSocket::Initialize(const char* ip, int port,void *param)
{
	m_nPort = port;
	if(ip)
		strcpy_s(m_sIP,ip);
	else
		m_sIP[0] =0;

	if(param)
	{ 
		stServerParam *pParam = (stServerParam *)param;
		m_nMaxClients	= pParam->MaxClients;
		m_nMinClients	= pParam->MinClients;
	}

	return true;
}

// 启动服务，配置网络环境并创建用户连接
bool dtServerSocket::Start()
{
	m_bShuttingDown	= false;

	//创建监听端口
	//===============================================================
	DWORD			dwError;
	m_nSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_nSocket == INVALID_SOCKET)
	{
		dwError = WSAGetLastError();
		return false;
	}

	//重用地址
	BOOL Reuse = TRUE;
	setsockopt(m_nSocket,SOL_SOCKET,SO_REUSEADDR,(const char *)&Reuse,sizeof(BOOL));

	//绑定IP
	SOCKADDR_IN		addr;
	addr.sin_family		 = AF_INET;
	if(!strcmp(m_sIP,""))
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		addr.sin_addr.s_addr = inet_addr(m_sIP);
	addr.sin_port		 = htons((short)m_nPort);
	if (bind(m_nSocket, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(m_nSocket);
		return false;
	}

	//侦听
	setsockopt( m_nSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *) &m_nSocket, sizeof(m_nSocket) );
	if (listen(m_nSocket, m_nMaxClients+1) == SOCKET_ERROR)
	{
		closesocket(m_nSocket);
		return false;
	}

	m_CompletionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)OP_NORMAL, 0);
	if(!m_CompletionPort)
	{
		closesocket(m_nSocket);
		return false;
	}

	SYSTEM_INFO		l_si;
	::GetSystemInfo( &l_si );
	m_CompletionThreadNum	= l_si.dwNumberOfProcessors * 2 + 1;
	if(m_CompletionThreadNum>MAX_COMPLETION_THREAD_NUM)
		m_CompletionThreadNum = MAX_COMPLETION_THREAD_NUM;	

	if(!CreateIoCompletionPort((HANDLE)m_nSocket,m_CompletionPort,(ULONG_PTR)OP_NORMAL, 0))
	{
		CloseHandle(m_CompletionPort);
		closesocket(m_nSocket);
		return false;
	}

	//创建完成线程
	unsigned int threadID;
	for (int i=0; i < m_CompletionThreadNum; i++ )
	{
		m_CompletionThread[i] = (HANDLE)_beginthreadex( NULL, 0, EventRoutine, this, 0, &threadID );
	}

	//创建客户端连接
	AddClient(m_nMinClients);

	//创建辅助线程
	m_hAddClientEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hEndTimerEvent  = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hTimerThread = (HANDLE)_beginthreadex(NULL,0,TimerRoutine,this,0,&threadID);
	if(!m_hTimerThread)
	{
		closesocket(m_nSocket);
		return false;	
	}

	WSAEventSelect(m_nSocket,m_hAddClientEvent,FD_ACCEPT); 

	m_nState = SSF_ACCEPTING;

	return true;
}

//关闭服务
bool dtServerSocket::Stop()
{
	//断开连接
	{
		m_bShuttingDown = true;

		for(int i=0;i<m_CompletionThreadNum;++i)
			PostQueuedCompletionStatus(m_CompletionPort,0,OP_QUIT,0);
		if(m_CompletionThreadNum)
			WaitForMultipleObjects(m_CompletionThreadNum,m_CompletionThread,TRUE,10000);

		dtServerSocketClient *pClient;
		stdext::hash_map<int,dtServerSocketClient*>::iterator it = m_ClientList.begin();
		std::vector<dtServerSocketClient*> templist;
		for(;it!=m_ClientList.end();it++)
		{
			pClient = it->second;
			templist.push_back(pClient);
		}
		m_ClientList.clear();

		std::stringstream LogStream;
		int DisconnectCount = 0;
		for(size_t i=0;i<templist.size();i++)
		{
			pClient = templist[i];
			if(pClient->GetState() == SSF_CONNECTED)
			{
				LogStream << "断开连接 Id=" << pClient->GetClientId() << std::ends;
				g_Log.WriteLog(LogStream);
			}

			delete pClient;
			DisconnectCount++;
		}

		LogStream << "一共断开当前连接" << DisconnectCount << "个" << std::ends;
		g_Log.WriteLog(LogStream);
	}

	if(m_nSocket != INVALID_SOCKET)
	{
		shutdown(m_nSocket, SD_BOTH);
		closesocket(m_nSocket);
		m_nSocket = INVALID_SOCKET;
	}

	//退出辅助线程
	if(m_hTimerThread)
	{
		if(m_hEndTimerEvent)
		{
			SetEvent(m_hEndTimerEvent);
			WaitForSingleObject(m_hTimerThread,10000);
		}

		CloseHandle(m_hTimerThread);
		m_hTimerThread = NULL;
	}

	if(m_hAddClientEvent)
	{
		CloseHandle(m_hAddClientEvent);
		m_hAddClientEvent = NULL;
	}

	if(m_hEndTimerEvent)
	{
		CloseHandle(m_hEndTimerEvent);
		m_hEndTimerEvent = NULL;
	}

	//关闭完成端口
	if(m_CompletionPort)
	{
		CloseHandle(m_CompletionPort);
		m_CompletionPort = NULL;
	}

	for(int i=0;i<m_CompletionThreadNum;i++)
	{
		if(m_CompletionThread[i])
		{
			CloseHandle(m_CompletionThread[i]);
			m_CompletionThread[i] = NULL;
		}
	}

	m_nState = SSF_DEAD;

	return true;
}

dtServerSocket::dtServerSocket(void)
{
	m_nClientCount			= 0;
	m_nMinClients			= 1;
	m_nMaxClients			= 1;

	m_nIdSeed				= 0;

	m_nIdleTimeout			= 0;
	m_nConnectedTimeout		= 0;

	m_hAddClientEvent		= NULL;
	m_hEndTimerEvent		= NULL;
	m_hTimerThread			= NULL;

	m_bShuttingDown			= false;
	m_bDisableConnectCheck	= false;
	m_bCanAccept			= true;

	m_CompletionPort		= NULL;
	m_CompletionThreadNum	= 0;

	for(int i=0;i<MAX_COMPLETION_THREAD_NUM;i++)
		m_CompletionThread[i] = NULL;

	m_pMemPool				= CMemPool::GetInstance();
}

dtServerSocket::~dtServerSocket(void)
{
	Stop();
}

int dtServerSocket::AssignClientId()
{
	::InterlockedIncrement( (LONG volatile*)&m_nIdSeed );
	return m_nIdSeed;
}

dtServerSocketClient *dtServerSocket::GetClientById(int id)
{
	stdext::hash_map<int,dtServerSocketClient*>::iterator it = m_ClientList.find(id);
	if(it!=m_ClientList.end())
		return it->second;

	return NULL;
}

bool dtServerSocket::AddClient(int Num)
{
	if(IsShuttingDown())
		return true;

	std::stringstream LogStream;

	if(m_nClientCount+Num > m_nMaxClients)
		Num = m_nMaxClients-m_nClientCount;

	if(!Num)
	{
		g_Log.WriteError("客户端连接已经超过服务器设定上限");
		return false;
	}

	int i;
	for(i=0; i<Num; i++)
	{
		dtServerSocketClient* pClient =(dtServerSocketClient*) LoadClient();
		if (pClient)
		{
			pClient->SetIoCompRoutine(m_CompletionPort,m_CompletionThreadNum);
			pClient->SetListenSocket(m_nSocket);
			pClient->SetServer(this);
			int ClientId = AssignClientId();
			pClient->SetClientId(ClientId);
			m_ClientList[ClientId] = pClient;
			pClient->Start();
			m_nClientCount++;	
		}
		else
		{
			g_Log.WriteError("无法创建客户端连接对象");
		}
	}

	LogStream << "新增" << i << "个客户端连接对象,当前数量为" << m_nClientCount << std::ends;
	g_Log.WriteLog(LogStream);

	return true;
}

void dtServerSocket::Send(int id,const char *pData,int nSize)
{
	if (nSize == 0 || nSize > m_MaxSendBufferSize) 
		return;

	OVERLAPPED_PLUS *lpOverlapped = (OVERLAPPED_PLUS*)m_pMemPool->Alloc(nSize+sizeof(OVERLAPPED_PLUS));
	memset(lpOverlapped,0,sizeof(OVERLAPPED_PLUS));
	lpOverlapped->WsaBuf.buf = (char *)lpOverlapped+sizeof(OVERLAPPED_PLUS);
	memcpy(lpOverlapped->WsaBuf.buf, pData, nSize);
	lpOverlapped->WsaBuf.len = nSize;
	lpOverlapped->bCtxWrite = 1;
	lpOverlapped->ClientId = id;

	PostEvent( dtServerSocket::OP_SEND ,lpOverlapped );
}

void dtServerSocket::Send(int id,Base::BitStream &SendPacket)
{
	Send(id,reinterpret_cast<char*>(SendPacket.getBuffer()),SendPacket.getPosition());
}

void dtServerSocket::PostEvent(int Msg,void *pData)
{
	PostQueuedCompletionStatus(m_CompletionPort,0,Msg,(LPOVERLAPPED)pData);
}

void dtServerSocket::PostEvent(int Msg,int id)
{
	OVERLAPPED_PLUS *lpOverlapped = (OVERLAPPED_PLUS*)m_pMemPool->Alloc(sizeof(OVERLAPPED_PLUS));
	memset(lpOverlapped,0,sizeof(OVERLAPPED_PLUS));
	lpOverlapped->WsaBuf.buf = NULL;
	lpOverlapped->WsaBuf.len = 0;
	lpOverlapped->ClientId = id;

	PostQueuedCompletionStatus(m_CompletionPort,0,Msg,(LPOVERLAPPED)lpOverlapped);
}

unsigned int dtServerSocket::TimerRoutine(LPVOID Param)
{
	dtServerSocket *lpServer = (dtServerSocket*)Param;

	HANDLE HandleList[2];

	HandleList[0] = lpServer->m_hAddClientEvent;	
	HandleList[1] = lpServer->m_hEndTimerEvent;	

	DWORD dwRet;
	while(true)
    {
		dwRet = WaitForMultipleObjects(2,HandleList,FALSE,5000);
		switch(dwRet){
		case WAIT_OBJECT_0:
			lpServer->PostEvent(OP_ADDCLIENT);
			break; 
		case WAIT_OBJECT_0+1:
			return 0;
		case WAIT_TIMEOUT:
			lpServer->PostEvent(OP_MAINTENANCE);
			break;
		}
	}

	return 0;
}

unsigned int dtServerSocket::EventRoutine(LPVOID lParam)
{
	dtServerSocket *pServer = (dtServerSocket*)lParam;
	OVERLAPPED_PLUS *ov=NULL;
	dtServerSocketClient *pClient=NULL;
	DWORD dwByteCount=0,dwErrorCode=0;
	dtServerSocket::OP_CODE opCode = dtServerSocket::OP_NORMAL;
	ISocket::SERVER_STATE_FLAGS State = ISocket::SSF_DEAD;
	char WriteFlag = 0;
	DWORD rf = 0;

	CMemPool *pMemPool		= CMemPool::GetInstance();

	while(1)
	{
		::GetQueuedCompletionStatus(pServer->m_CompletionPort, &dwByteCount, (ULONG_PTR *)&opCode,(LPOVERLAPPED*)&ov, INFINITE );

		if(opCode == dtServerSocket::OP_QUIT)
			return 0;

		if(opCode != dtServerSocket::OP_ADDCLIENT && opCode != dtServerSocket::OP_MAINTENANCE)
		{
			if (ov != NULL)
			{
				pClient = pServer->GetClientById(ov->ClientId);
				if(!pClient)
				{
#ifdef _DEBUG
					std::stringstream strError;
					strError<<"EventRoutine(), pClient==NULL id="<<ov->ClientId<<std::endl;
					g_Log.WriteError( strError );
#endif
					pMemPool->Free((MemPoolEntry)ov);
					continue;
				}
			}
			else
				continue;
		}

		switch(opCode)
		{
		case dtServerSocket::OP_ADDCLIENT:
			pServer->AddClient(ADD_CLIENT_NUM);
			break;
		case dtServerSocket::OP_MAINTENANCE:
			pServer->Maintenance();
			break;
		case dtServerSocket::OP_SEND:
			pClient->BeginSend((OVERLAPPED_PLUS *)ov);
			break;
		case dtServerSocket::OP_DISCONNECT:
			break;
		case dtServerSocket::OP_RESTART:
			pMemPool->Free((MemPoolEntry)ov);
			pClient->Restart();
			break;
		case dtServerSocket::OP_NORMAL:
			{
				dwErrorCode = 0;
				if(!WSAGetOverlappedResult(pClient->m_nSocket,ov,&dwByteCount,TRUE,&rf))
					dwErrorCode = WSAGetLastError();

				WriteFlag = ov->bCtxWrite;

				std::stringstream LogStream;
				if (!dwErrorCode)
				{
					State = pClient->GetState();

					if (dwByteCount)
					{
						if (WriteFlag)
						{
							pClient->HandleSend(ov,dwByteCount);
						}
						else
						{
							bool Success = true;
							if (State == ISocket::SSF_ACCEPTING)
							{
								Success = pClient->HandleConnect(ov,dwByteCount);
							}

							if(Success)
								while (pClient->HandleReceive(ov,dwByteCount));
						}
					}
					else
					{
						if(pClient->m_nState == ISocket::SSF_CONNECTED)
						{
							LogStream << "<ip=" << pClient->GetConnectedIP() << ">" << "的用户断开连接" << std::ends;
							g_Log.WriteWarn(LogStream.str());

							pClient->PostEvent(dtServerSocket::OP_RESTART,ov);
						}
					}
				}
				else
				{
					if(dwErrorCode == ERROR_OPERATION_ABORTED)
						continue;

					if(dwErrorCode == WSAECONNRESET)
					{
						LogStream << "<ip=" << pClient->GetConnectedIP() << ">" << "的用户断开连接" << std::ends;
						g_Log.WriteWarn(LogStream.str());
					}
					else
					{
						LogStream << "EventRoutine() (" << (int)pClient->m_nSocket << 
							")<Error=" << (int)dwErrorCode << "><ip=" << pClient->GetConnectedIP() << ">" << 
							"<Byte=" << dwByteCount << ">" << "<Writeing=" << (int)ov->bCtxWrite << ">" << std::ends;
						g_Log.WriteError(LogStream.str());
					}

					if(pClient->m_nState == ISocket::SSF_CONNECTED)
					{
						pClient->PostEvent(dtServerSocket::OP_RESTART,ov);
					}
				}
			}
			break;
		}
	}

	return 0;
}

int dtServerSocket::Maintenance()
{
    if (IsShuttingDown()) 
        return 0;

    std::stringstream LogStream;

    int Count=0,nConnectTime=0;
	dtServerSocketClient *pClient = NULL;
	dtServerSocketClient::SERVER_STATE_FLAGS SocketStatus;

	stdext::hash_map<int,dtServerSocketClient*>::iterator it = m_ClientList.begin();
    for (; it != m_ClientList.end(); it++)  
    {
		pClient = it->second;

        SocketStatus = pClient->GetState();

		switch(SocketStatus)
		{
		case dtServerSocketClient::SSF_ACCEPTING:
			nConnectTime = pClient->GetConnectTime();
			if (!m_bDisableConnectCheck && nConnectTime != 0xFFFFFFFF && nConnectTime > 20)
			{
				pClient->PostEvent(dtServerSocket::OP_RESTART);
				Count++;
			}
			break;

		case dtServerSocketClient::SSF_CONNECTED:
			nConnectTime = pClient->GetConnectTime();
			if(m_nConnectedTimeout && pClient->GetConnectType()!=SERVER_CONNECT && nConnectTime>=m_nConnectedTimeout)
			{
				pClient->PostEvent(dtServerSocket::OP_RESTART);
				Count++;
			}
			break;

		case dtServerSocketClient::SSF_DEAD:
			if(m_bCanAccept)
			{
				if(SocketStatus == dtServerSocketClient::SSF_DEAD)
				{
					pClient->PostEvent(dtServerSocket::OP_RESTART);
					Count++;
				}
			}
			break;
		}
    }

    return Count;
}

ISocket *dtServerSocket::LoadClient()
{
	ISocket *pSocketClient = new dtServerSocketClient;

	pSocketClient->SetMaxSendBufferSize(m_MaxSendBufferSize);
	pSocketClient->SetMaxReceiveBufferSize(m_MaxReceiveBufferSize);

	return pSocketClient;
}


