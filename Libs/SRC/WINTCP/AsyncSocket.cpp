#pragma once

#include <process.h>
#include "IPacket.h"
#include "AsyncSocket.h"

void CAsyncSocket2::SetMaxReceiveBufferSize(int MaxReceiveSize)
{
	if(m_nState != SSF_SHUT_DOWN)
		return;

	Parent::SetMaxReceiveBufferSize(MaxReceiveSize);
}

void CAsyncSocket2::SocketError(char *from,int error)
{
	std::stringstream LogStream;

	if(error!=0){
		LogStream << from << " --> " << error << std::ends;
		g_Log.WriteError(LogStream);
	}else{
		LogStream << from << std::ends;
		g_Log.WriteLog(LogStream);
	}
}

bool CAsyncSocket2::CreateSocket()
{
	if (m_nSocket != INVALID_SOCKET)
	{
		SocketError("Create: m_nSocket 没有释放",0);
		closesocket(m_nSocket);
		m_nSocket = INVALID_SOCKET;
	}

	m_nSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_nSocket == INVALID_SOCKET)
	{
		SocketError("Create: WSASocket = INVALID_SOCKET",WSAGetLastError());
		return false;
	}

	BOOL Val = 1;
	if(setsockopt(m_nSocket,SOL_SOCKET,SO_REUSEADDR,(const char *)&Val,sizeof(BOOL))==SOCKET_ERROR )
	{
		SocketError("Create: setsockopt(SO_REUSEADDR)",WSAGetLastError());
		Disconnect();
	}
	
	Val = 0;
	if(setsockopt(m_nSocket,IPPROTO_TCP,TCP_NODELAY,(const char *)&Val,sizeof(BOOL))==SOCKET_ERROR )
	{
		SocketError("Create: setsockopt(NODELAY)",WSAGetLastError());
		Disconnect();
	}

	//short Port = 10000;
	SOCKADDR_IN		addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);  //inet_addr(m_sIP);	//
	addr.sin_port = htons(0);					//htons(Port+(short)m_SocketID);
	if (bind(m_nSocket, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		SocketError("Create: bind",WSAGetLastError());
		Disconnect();
		return false;
	}

	DWORD ret;
	tcp_keepalive alive;
	alive.onoff = 1;
	alive.keepalivetime  = 5000;
	alive.keepaliveinterval = 5000;
	WSAIoctl(m_nSocket,SIO_KEEPALIVE_VALS,&alive,sizeof(tcp_keepalive),NULL,0,&ret,NULL,NULL);

	return true;
}

OVERLAPPED_PLUS* CAsyncSocket2::GetBuffer(int iSize)
{
	OVERLAPPED_PLUS *lpOverlapped = NULL;

	if(!m_pMemPool)
	{
		lpOverlapped = new OVERLAPPED_PLUS;
		memset(lpOverlapped,0,sizeof(OVERLAPPED_PLUS));
		lpOverlapped->WsaBuf.buf = new char[iSize];
	}
	else
	{
		lpOverlapped = (OVERLAPPED_PLUS*)m_pMemPool->Alloc(iSize+sizeof(OVERLAPPED_PLUS));
		memset(lpOverlapped,0,sizeof(OVERLAPPED_PLUS));
		lpOverlapped->WsaBuf.buf = (char *)lpOverlapped+sizeof(OVERLAPPED_PLUS);
	}

	lpOverlapped->pClient = this;
	lpOverlapped->WsaBuf.len = iSize;
	return lpOverlapped;
}

bool CAsyncSocket2::ReleaseBuffer(OVERLAPPED_PLUS *lpOverlapped)
{
	if(!m_pMemPool)
	{
		delete [] lpOverlapped->WsaBuf.buf;
		delete lpOverlapped;
	}
	else
	{
		m_pMemPool->Free((U8*)lpOverlapped);
	}

	return true;
}

CAsyncSocket2::CAsyncSocket2()
{
	m_pMemPool				= CMemPool::GetInstance();

	m_nState				= SSF_SHUT_DOWN;
	m_nSocket				= INVALID_SOCKET;

	m_nPort					= 0;
	m_sIP[0]				= 0;

	m_hThread				= NULL;

	m_hKillEvent			= NULL;
	m_hConnectEvent			= NULL;
	m_hSendEvent			= NULL;

	m_bPendingRead			= false;
}

CAsyncSocket2::~CAsyncSocket2(void)
{
	if(m_nSocket!=INVALID_SOCKET)
		Stop();
}

bool CAsyncSocket2::Initialize(const char *ip,int nPort,void *)
{
	m_nPort = nPort;
	strcpy_s(m_sIP,ip);

	return true;
}

bool CAsyncSocket2::Start()
{
	if(!CreateSocket())
		return false;

	m_hKillEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if(!m_hKillEvent)
		return false;

	m_hConnectEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(!m_hConnectEvent)
		return false;

	m_hSendEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(!m_hSendEvent)
		return false;

	unsigned int ID;
	m_hThread = (HANDLE)_beginthreadex(NULL,0,NetRoutine,this,0,&ID);
	if(!m_hThread)
		return false;

	SetEvent(m_hConnectEvent);

	return true;
}

bool CAsyncSocket2::Stop()
{
	Disconnect(true);

	if(m_hKillEvent){
		SetEvent(m_hKillEvent);
	}

	if(m_hThread){
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if(m_hKillEvent){
		CloseHandle(m_hKillEvent);
		m_hKillEvent = NULL;
	}

	if(m_hConnectEvent){
		CloseHandle(m_hConnectEvent);
		m_hConnectEvent = NULL;
	}

	if(m_hSendEvent){
		CloseHandle(m_hSendEvent);
		m_hSendEvent = NULL;
	}

#if defined(_DEBUG)
	CAsyncSocket2 *pClient = this;
	std::stringstream LogStream;
	LogStream << "Client   Sended=" << pClient->m_SendTimes <<" PendingSended=" << pClient->m_PendSendTimes
		<< "         Received=" << pClient->m_ReceiveTimes 
		<< std::ends;
	g_Log.WriteLog(LogStream);
#endif

	return true;
}

bool CAsyncSocket2::Restart()
{
	Disconnect(true);

	if(!CreateSocket())
		return false;

	SetEvent(m_hConnectEvent);

	return true;
}

bool CAsyncSocket2::Send(Base::BitStream &SendPacket)
{
	return Send(reinterpret_cast<char*>(SendPacket.getBuffer()),SendPacket.getPosition());
}

bool CAsyncSocket2::Send(const char *Buf,int nSize)
{
	if (nSize == 0 || nSize>m_MaxSendBufferSize) {
		SocketError(" SendError size",nSize);
		// [LivenHotch]: 有时出现size 为 zero ,可能会有些问题.
		AssertFatal( nSize, "SendError size is zero" );
		return false;
	}
	
	CLocker aLock(m_cs);

	OVERLAPPED_PLUS* lpOverlapped = GetBuffer(nSize);
	memcpy(lpOverlapped->WsaBuf.buf, Buf, nSize);
	lpOverlapped->WsaBuf.len = nSize;
	lpOverlapped->bCtxWrite = 1;
	m_OutBuffer.push_back(lpOverlapped);

	SetEvent(m_hSendEvent);

	return true;
}

bool CAsyncSocket2::Connect()
{
	if(m_nState==ISocket::SSF_CONNECTING)
		return false;

	m_nState = ISocket::SSF_CONNECTING;

	u_long param = 0;
	if(ioctlsocket(m_nSocket,FIONBIO,&param)==SOCKET_ERROR)
	{
		SocketError("无法使用阻塞方式",WSAGetLastError());
		m_nState = ISocket::SSF_DEAD;
		return false;
	}

	OVERLAPPED_PLUS *lpOverlapped = GetBuffer(m_MaxReceiveBufferSize);
	lpOverlapped->bCtxWrite = 0;

	SOCKADDR_IN	addr;
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(m_sIP);
	addr.sin_port			= htons((short)m_nPort);

	int dwError = 0;
	if(WSAConnect(m_nSocket, (SOCKADDR*)&addr,	sizeof(addr),NULL,&lpOverlapped->WsaBuf,NULL,NULL) == SOCKET_ERROR)
	{
		//要用完成端口需要用ConnectEx
		m_nState = ISocket::SSF_DEAD;
		dwError = WSAGetLastError();
		OnConnect(dwError);
		ReleaseBuffer(lpOverlapped);
		return false;
	}

	if(!lpOverlapped->WsaBuf.len)
	{
		ReleaseBuffer(lpOverlapped);
		lpOverlapped = GetBuffer(m_MaxReceiveBufferSize);
		m_bPendingRead = false;
	}
	else
	{
		m_bPendingRead = true;
	}

	param = 1;
	if(ioctlsocket(m_nSocket,FIONBIO,&param)==SOCKET_ERROR)
	{
		SocketError("无法恢复异步端口",WSAGetLastError());
		return false;
	}

	m_nState = SSF_CONNECTED;

	while(HandleReceive(lpOverlapped,lpOverlapped->WsaBuf.len));

	HandleConnect(lpOverlapped,dwError);
	
	return true;
}

bool CAsyncSocket2::Disconnect(bool bForce)
{
	CLocker aLock(m_cs);	//Lock the class, we do not want to do stuff

	if(m_nSocket == INVALID_SOCKET)
		return true;

	struct linger li = { 0, 0 }; // default: SO_DONTLINGER
	li.l_onoff = !bForce;
	li.l_linger = 5;
	setsockopt( m_nSocket, SOL_SOCKET, SO_LINGER, (char *) &li, sizeof li );

	if(closesocket(m_nSocket) == SOCKET_ERROR )
		SocketError("Disconnect: closesocket",WSAGetLastError());

	m_nState = SSF_DEAD;
	m_nSocket = INVALID_SOCKET;
	OnDisconnect();
	OnClear();

	m_PendSendTimes = 0;
	m_SendTimes		= 0;
	m_ReceiveTimes	= 0;

	return true;
}

bool CAsyncSocket2::HandleConnect(OVERLAPPED_PLUS *lpOverlapped,int nError)
{
	if(!nError)
	{
		m_nState = SSF_CONNECTED;
		OnConnect(nError);

		return true;
	}
	else
	{
		m_nState = SSF_DEAD;
		OnClear();
		return false;
	}
}

bool CAsyncSocket2::HandleSend(OVERLAPPED_PLUS *lpOverlapped,int ByteSended)
{
	if(lpOverlapped)
	{
		ReleaseBuffer(lpOverlapped);
	}

	if (m_nState != SSF_CONNECTED)
	{
		return false;
	}

	CLocker aLock(m_cs);
	if(m_OutBuffer.size() && !m_PendSendTimes)
	{
		lpOverlapped = m_OutBuffer.front();
		m_OutBuffer.pop_front();

		m_SendTimes++;

		int nLastError = 0;
		DWORD dwBytesWritten = 0,dwFlags = 0;
		if (WSASend(m_nSocket, &lpOverlapped->WsaBuf, 1, &dwBytesWritten, dwFlags, lpOverlapped, ComplateRoutine))
		{
			nLastError = WSAGetLastError();
			if (WSA_IO_PENDING != nLastError)
			{
				ReleaseBuffer(lpOverlapped);
				SocketError("HandleSend: WSASend:",nLastError);
				Disconnect();
				return false;
			}
		}

		m_PendSendTimes++;
	}
	
	return true;
}

bool CAsyncSocket2::HandleReceive(OVERLAPPED_PLUS *lpOverlapped,int ByteReceived)
{
	if (m_nState == SSF_DEAD || m_nState == SSF_SHUTTING_DOWN)
	{
		ReleaseBuffer(lpOverlapped);
		SocketError("HandleReceive: m_nState=",m_nState);
		Disconnect();
		return false;
	}

	if (m_nState != SSF_CONNECTED)
	{
		ReleaseBuffer(lpOverlapped);
		return false;
	}

	DWORD dwLastError=0;
	if(!m_bPendingRead)
	{
		lpOverlapped->WsaBuf.len = m_MaxReceiveBufferSize-1;
		lpOverlapped->bCtxWrite = 0;

		DWORD dwBytesRecvd = 0,dwFlags=0;
		if (WSARecv(m_nSocket, &lpOverlapped->WsaBuf, 1, (LPDWORD)&dwBytesRecvd,	&dwFlags, lpOverlapped, ComplateRoutine) )
		{
			dwLastError = WSAGetLastError();
			if (WSA_IO_PENDING != dwLastError)
			{
				ReleaseBuffer(lpOverlapped);
				SocketError("HandleReceive: WSARecv",dwLastError);
				Disconnect();
				return false;
			}

		}
		m_bPendingRead = true;
		return false;
	}

	lpOverlapped->WsaBuf.buf[ByteReceived] = 0;
	OnReceive(lpOverlapped->WsaBuf.buf, ByteReceived);  //处理接收到的数据
	m_bPendingRead = false;

	return true;
}

unsigned int CAsyncSocket2::NetRoutine(LPVOID pParam)
{
	CAsyncSocket2 *pSocket = (CAsyncSocket2 *)pParam;
	HANDLE hHandle[3];

	hHandle[0] = pSocket->m_hKillEvent;
	hHandle[1] = pSocket->m_hConnectEvent;
	hHandle[2] = pSocket->m_hSendEvent;


	DWORD dwRet;
	while(1)
	{
		dwRet = WSAWaitForMultipleEvents(3,hHandle,FALSE,INFINITE,TRUE);
		switch(dwRet){
		case WSA_WAIT_EVENT_0:
			ExitThread(0);
			return 0;
		case WSA_WAIT_EVENT_0+1:
			pSocket->Connect();
			break;
		case WSA_WAIT_EVENT_0+2:
			pSocket->HandleSend(NULL,0);
			break;
		case WAIT_IO_COMPLETION:
			break;
		case WAIT_FAILED:
			pSocket->SocketError("网络线程等待出错NetRoutine",GetLastError());
			return -1;
		}
	}
}

void CALLBACK CAsyncSocket2::ComplateRoutine(DWORD dwErrorCode,DWORD dwByteCount,LPWSAOVERLAPPED lpOverlapped,DWORD InFlag)
{
	OVERLAPPED_PLUS *ov;
	CAsyncSocket2 *pClient;

	ov = (OVERLAPPED_PLUS *)lpOverlapped;
	if (ov != NULL)
		pClient = dynamic_cast<CAsyncSocket2 *>(ov->pClient);
	else
		return;

	CLocker aLock(pClient->m_cs);

	if (dwErrorCode == 0)
	{
		if (dwByteCount != 0)
		{
			if (ov->bCtxWrite)
			{
				pClient->m_PendSendTimes--;
				pClient->HandleSend(ov,dwByteCount);					//发送失败会Disconnect
			}
			else
			{
				while (pClient->HandleReceive(ov,dwByteCount));	//接收失败会Disconnect
			}
		}
		else
		{
			pClient->ReleaseBuffer(ov);

			if (pClient->GetState() == CAsyncSocket2::SSF_CONNECTING)
				return;

			pClient->Disconnect(true);	//正常断开
		}
	}
	else
	{
		pClient->ReleaseBuffer(ov);

		if (pClient->GetState() == CAsyncSocket2::SSF_CONNECTING || pClient->GetState() == CAsyncSocket2::SSF_DEAD)
			return;

		pClient->SocketError("CAsyncSocket2: dwErrorCode:",dwErrorCode);
		pClient->Disconnect(true);	//错误断开
	}
}

