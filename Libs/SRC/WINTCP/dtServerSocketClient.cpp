#pragma once

#include "dtServerSocketClient.h"
#include "IPacket.h"

void dtServerSocketClient::SetIoCompRoutine(HANDLE CompletionPort,int ThreadNum)
{
	m_CompletionThreadNum = ThreadNum;
	m_CompletionPort = CompletionPort;
}

void dtServerSocketClient::SetListenSocket(SOCKET sdListen) 
{ 
	m_nListenSocket = sdListen; 
}

void dtServerSocketClient::SetServer(dtServerSocket *pServer)
{
	m_pServer = pServer;
}

dtServerSocketClient::dtServerSocketClient(void)
{
	m_pMemPool				= CMemPool::GetInstance();

	m_nListenSocket			= INVALID_SOCKET;
	m_pServer				= NULL;
	m_CompletionPort		= NULL;
	m_CompletionThreadNum	= 0;
	m_ClientId				= 0;

	ResetVar();
}

dtServerSocketClient::~dtServerSocketClient(void)
{
	Disconnect(true);
	m_pServer = 0;
}

void dtServerSocketClient::ResetVar()
{
	m_nState				= SSF_SHUT_DOWN;
	m_bPendingRead			= false;
	m_nLastTransTickCount	= GetTickCount();
	m_nConnectType			= CLIENT_CONNECT;
	m_nSocket				= INVALID_SOCKET;
}

bool dtServerSocketClient::Start()
{
	//如果正在关闭
	if (m_nState != SSF_SHUT_DOWN && m_nState != SSF_RESTARTING)
		return false;

	//初始化有问题
	if (!m_pServer || m_nListenSocket == INVALID_SOCKET)
		return false;

	//创建端口
	if (m_nSocket != INVALID_SOCKET)
	{
		CancelIo((HANDLE)m_nSocket);
		closesocket(m_nSocket);
	}

	//变量初始化
	ResetVar();

	m_nSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if(m_nSocket == INVALID_SOCKET)
		return false;

	//__int32 iRcvBufSize = 307200;
	//::setsockopt(m_sdClient, SOL_SOCKET, SO_RCVBUF, (char *)&iRcvBufSize, sizeof(__int32));

	DWORD ret;
	tcp_keepalive alive;
	alive.onoff = 1;
	alive.keepalivetime  = 5000;
	alive.keepaliveinterval = 5000;
	WSAIoctl(m_nSocket,SIO_KEEPALIVE_VALS,&alive,sizeof(tcp_keepalive),NULL,0,&ret,NULL,NULL);

	BOOL Val = 1;
	if(setsockopt(m_nSocket,SOL_SOCKET,SO_REUSEADDR,(const char *)&Val,sizeof(BOOL))==SOCKET_ERROR )
		return false;

	Val = 0;
	if(setsockopt(m_nSocket,IPPROTO_TCP,TCP_NODELAY,(const char *)&Val,sizeof(BOOL))==SOCKET_ERROR )
		return false;

	OVERLAPPED_PLUS *lpOverlapped;
	while(m_SendingList.size())
	{
		lpOverlapped = *m_SendingList.begin();
		m_SendingList.pop_front();
		m_pMemPool->Free((MemPoolEntry)lpOverlapped);
	}

	lpOverlapped = (OVERLAPPED_PLUS *)GetBuffer(m_MaxReceiveBufferSize);

	//发起接收
	DWORD dwBytesRecvd;
	if (!AcceptEx(m_nListenSocket,
		m_nSocket,
		lpOverlapped->WsaBuf.buf,
		m_MaxReceiveBufferSize-(sizeof(sockaddr_in)+16)*2 -1,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		&dwBytesRecvd,
		lpOverlapped))
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_IO_PENDING)
		{
			PostEvent(dtServerSocket::OP_RESTART,lpOverlapped);

			std::stringstream LogStream;
			LogStream << "AcceptEx(Restart), Error:"<<dwError  << std::ends;
			g_Log.WriteError(LogStream.str());

			return false;
		}
	}

	m_nState = SSF_ACCEPTING;
	return true;
}

bool dtServerSocketClient::Restart()
{
	Disconnect(false);

	if (m_pServer->IsShuttingDown()) 
		return true;

	m_ClientId = m_pServer->AssignClientId();
	m_pServer->m_ClientList[m_ClientId] = this;

	return Start();
}

bool dtServerSocketClient::Disconnect(bool bForce)
{
	if(m_nState == SSF_SHUTTING_DOWN || m_nState == SSF_SHUT_DOWN)
		return false;

	m_pServer->m_ClientList.erase(m_ClientId);
	m_nState = SSF_SHUTTING_DOWN;

	//	shutdown(m_sdClient, SD_BOTH); //SD_SEND		//服务端被动关闭一个正常连接

	CancelIo((HANDLE)m_nSocket);

	struct linger li = { 0, 0 }; // default: SO_DONTLINGER
	li.l_onoff = !bForce;
	li.l_linger = 5;
	setsockopt( m_nSocket, SOL_SOCKET, SO_LINGER, (char *) &li, sizeof li );

	closesocket( m_nSocket );
	Sleep(5);

	OnDisconnect();
	OnClear();

	OVERLAPPED_PLUS *lpOverlapped;
	while(m_SendingList.size())
	{
		lpOverlapped = *m_SendingList.begin();
		m_SendingList.pop_front();
		m_pMemPool->Free((MemPoolEntry)lpOverlapped);
	}

	//变量初始化
	ResetVar();

	m_nState = SSF_SHUT_DOWN;

	return true;
}

void dtServerSocketClient::OnDisconnect()									
{ 
}

void dtServerSocketClient::OnConnect(int nErrorCode)
{
    m_PendSendTimes = 0;
    m_SendingList.clear();
}

bool dtServerSocketClient::BeginSend(OVERLAPPED_PLUS *lpOverlapped)
{
	if (m_pServer->IsShuttingDown() || m_nState!=SSF_CONNECTED) 
		return false;

	lpOverlapped->bCtxWrite = 1;
	m_SendingList.push_back(lpOverlapped);

	if(m_PendSendTimes > dtServerSocket::MAX_PENDING_SEND)
		return true;

	lpOverlapped = *m_SendingList.begin();
	m_SendingList.pop_front();
	DWORD dwBytesWritten = 0, dwFlags = 0;
	if(WSASend(m_nSocket, &lpOverlapped->WsaBuf, 1, &dwBytesWritten, dwFlags, lpOverlapped, NULL))
	{
		int nLastError = WSAGetLastError();
		if (WSA_IO_PENDING != nLastError)
		{
			std::stringstream LogStream;
			LogStream << "WSASendError Socket:" << (int)m_nSocket << " ErrorCode=" << nLastError << std::ends;
			g_Log.WriteError(LogStream);

			PostEvent(dtServerSocket::OP_RESTART,lpOverlapped);

			return false;
		}
	}

	m_SendTimes++;
	m_PendSendTimes++;
	m_nLastTransTickCount = GetTickCount();
	return true;
}

bool dtServerSocketClient::Send(const char* data, int nSize)
{
	m_pServer->Send(GetClientId(),data,nSize);
	return true;
}

bool dtServerSocketClient::HandleReceive(OVERLAPPED_PLUS *lpOverlapped,int ByteReceived)
{
	if (m_pServer->IsShuttingDown() || m_nState != SSF_CONNECTED)
	{
		ReleaseBuffer(lpOverlapped);
		return false;
	}

	if (!m_bPendingRead)
	{
		DWORD dwLastError = 0, dwBytesRecvd = 0;
		
		lpOverlapped->WsaBuf.len = m_MaxReceiveBufferSize-1;
		DWORD dwFlags=0;
		if ( WSARecv(m_nSocket, &lpOverlapped->WsaBuf, 1, &dwBytesRecvd,	&dwFlags, lpOverlapped, NULL) )
		{
			dwLastError = WSAGetLastError();
			if (WSA_IO_PENDING != dwLastError)
			{
				std::stringstream LogStream;
				LogStream << "WSARecvError Socket:" << (int)m_nSocket << " ErrorCode=" << dwLastError << std::ends;
				g_Log.WriteError(LogStream);

				PostEvent(dtServerSocket::OP_RESTART,lpOverlapped);
				return false;
			}
		}

		m_bPendingRead = true;
		return false;
	}


	m_nLastTransTickCount = GetTickCount();

	lpOverlapped->WsaBuf.buf[ByteReceived] = 0;
	OnReceive(lpOverlapped->WsaBuf.buf, ByteReceived);
	m_bPendingRead = false;
	m_ReceiveTimes++;

	return true;
}

bool dtServerSocketClient::HandleConnect(OVERLAPPED_PLUS *lpOverlapped,int ByteReceived)
{
	if (m_pServer->IsShuttingDown() || m_nState != SSF_ACCEPTING)
	{
		ReleaseBuffer(lpOverlapped);
		return false;
	}

	setsockopt( m_nSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *) &m_nListenSocket, sizeof(m_nListenSocket) );

	int locallen, remotelen;
	sockaddr_in *plocal = 0, *premote = 0;
	GetAcceptExSockaddrs(lpOverlapped->WsaBuf.buf,
		m_MaxReceiveBufferSize-(sizeof(sockaddr_in)+16)*2 - 1,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		(sockaddr **)&plocal,
		&locallen,
		(sockaddr **)&premote,
		&remotelen);

	memcpy(&m_LocalAddr, plocal, sizeof(sockaddr_in));
	memcpy(&m_RemoteAddr, premote, sizeof(sockaddr_in));

	//BOOL Value = TRUE;
	//int Result = setsockopt(m_sdClient,SOL_SOCKET,SO_KEEPALIVE,(char *)&Value,sizeof(Value));

	if(!CreateIoCompletionPort((HANDLE)m_nSocket,m_CompletionPort,(ULONG_PTR)dtServerSocket::OP_NORMAL, 0))
	{
		PostEvent(dtServerSocket::OP_RESTART,lpOverlapped);
		return false;
	}

	m_nState = SSF_CONNECTED;

	lpOverlapped->WsaBuf.buf[ByteReceived] = 0;
	OnConnect(0);	//无法把连接和接收第一个数据包分开,这里才真正确保完成连接

	OnReceive(lpOverlapped->WsaBuf.buf, ByteReceived);
	m_ReceiveTimes++;

	m_bPendingRead = false;
	return true;
}

bool dtServerSocketClient::HandleSend(OVERLAPPED_PLUS *lpOverlapped,int ByteReceived)
{
	ReleaseBuffer(lpOverlapped);

	if (m_pServer->IsShuttingDown() || m_nState != SSF_CONNECTED)
		return false;

	m_PendSendTimes--;
	if(m_PendSendTimes > dtServerSocket::MAX_PENDING_SEND)
		return true;

	if(!m_SendingList.size())
		return true;

	lpOverlapped = *m_SendingList.begin();
	m_SendingList.pop_front();
	DWORD dwBytesWritten = 0, dwFlags = 0;
	if(WSASend(m_nSocket, &lpOverlapped->WsaBuf, 1, &dwBytesWritten, dwFlags, lpOverlapped, NULL))
	{
		int nLastError = WSAGetLastError();
		if (WSA_IO_PENDING != nLastError)
		{
			std::stringstream LogStream;
			LogStream << "WSASendError Socket:" << (int)m_nSocket << " ErrorCode=" << nLastError << std::ends;
			g_Log.WriteError(LogStream);

			PostEvent(dtServerSocket::OP_RESTART,lpOverlapped);

			return false;
		}
	}

	m_SendTimes++;
	m_PendSendTimes++;
	m_nLastTransTickCount = GetTickCount();
	return true;
}



int dtServerSocketClient::GetConnectTime()
{
	INT nSecs;
	INT nBytes = sizeof(nSecs);
	int nErr = getsockopt( m_nSocket, SOL_SOCKET, SO_CONNECT_TIME,	(char *)&nSecs, (PINT)&nBytes );
	if ( nErr != NO_ERROR ) 
		return -1;

	return nSecs;
}

int dtServerSocketClient::GetIdleTime()
{
	if (!m_nLastTransTickCount) 
		return 0;

	return (GetTickCount() - m_nLastTransTickCount) / 1000;
}

OVERLAPPED_PLUS* dtServerSocketClient::GetBuffer(int nSize)
{
	OVERLAPPED_PLUS *lpOverlapped = (OVERLAPPED_PLUS*)m_pMemPool->Alloc(nSize+sizeof(OVERLAPPED_PLUS));
	memset(lpOverlapped,0,sizeof(OVERLAPPED_PLUS));

	if(nSize)
		lpOverlapped->WsaBuf.buf = (char *)lpOverlapped+sizeof(OVERLAPPED_PLUS);
	else
		lpOverlapped->WsaBuf.buf = NULL;

	lpOverlapped->pClient		= NULL;
	lpOverlapped->ClientId		= m_ClientId;
	lpOverlapped->WsaBuf.len	= nSize;
	lpOverlapped->bCtxWrite		= 0;

	return lpOverlapped;
}

BOOL dtServerSocketClient::ReleaseBuffer(OVERLAPPED_PLUS *pBuf)
{
	m_pMemPool->Free((U8 *)pBuf);
	return TRUE;
}

void dtServerSocketClient::PostEvent(int Msg,void *pData)
{
	if(Msg == dtServerSocket::OP_RESTART)
		m_nState = SSF_RESTARTING;

	if(!pData)
		pData = GetBuffer(0);

	::PostQueuedCompletionStatus(m_CompletionPort,0,Msg,(LPOVERLAPPED)pData);
}

