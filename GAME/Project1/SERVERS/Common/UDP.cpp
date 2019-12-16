#include "UDP.h"
#include "encrypt/AuthCrypt.h"

using namespace std;

CUDPServer::CUDPServer(const char *ip,int nPort,int side)
{
	m_sdClient				= INVALID_SOCKET;
	m_sdClientOut			= INVALID_SOCKET;
	m_MaxWorkBufferSize		= UDP_SERVER_WORK_BUFFER_SIZE;
	m_nPort					= nPort;
	strcpy_s(m_sIP, 64, ip);
	m_hThread				= NULL;
	m_hEvent				= NULL;
	m_pOutServer			= NULL;
    m_side                  = (UDP_SIDE)side;
}

CUDPServer::~CUDPServer(void)
{
	Stop();
}

BOOL CUDPServer::Send(const char *pBuffer,int nSize,ULONG ToIP,short ToPort)
{
	if (nSize == 0 || nSize > static_cast<int>(m_MaxWorkBufferSize)) {
		printf("--------------> SendError size=%d \n",nSize);
		return FALSE;
	}

	if(m_sdClient==INVALID_SOCKET)
		return FALSE;

	SOCKADDR_IN addr;
	addr.sin_family		= AF_INET;
	addr.sin_addr.s_addr	= ToIP;
	addr.sin_port			= ToPort;

#ifdef ENABLE_ENCRPY_PACKET
    if (UDP_SIDE_CLIENT == m_side)
        AuthCrypt::Instance()->EncryptSend((uint8*)const_cast<char*>(pBuffer),nSize);
#endif

	WSABUF buff;
	buff.buf = (char *)pBuffer;
	buff.len = nSize;
    
	DWORD dwBytesWritten = 0, dwFlags = 0;
	WSASendTo(m_sdClient, &buff, 1, &dwBytesWritten, dwFlags,
		(sockaddr*)&addr,sizeof(addr), NULL, NULL);

	return TRUE;
}

BOOL CUDPServer::Stop()
{
	if(m_sdClient == INVALID_SOCKET && m_sdClientOut == INVALID_SOCKET)
		return TRUE;

	if(closesocket(m_sdClient) == SOCKET_ERROR )
		SocketError("Disconnect: closesocket",WSAGetLastError());

	if(closesocket(m_sdClientOut) == SOCKET_ERROR )
		SocketError("Disconnect: closesocket",WSAGetLastError());

	m_sdClient = INVALID_SOCKET;
	m_sdClientOut = INVALID_SOCKET;

	if(m_hEvent){
		SetEvent(m_hEvent);
	}

	if(m_hThread){
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if(m_hEvent){
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	return 0;
}

void CUDPServer::SocketError(char *from,int error)
{
	stringstream LogStream;

	if(error!=0){
		LogStream << from << " --> " << error << ends;
		g_Log.WriteError(LogStream);
	}else{
		LogStream << from << ends;
		g_Log.WriteLog(LogStream);
	}
}

BOOL CUDPServer::Start()
{
	if(!Create())
		return FALSE;

	m_hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if(!m_hEvent)
		return FALSE;

	DWORD ID;
	m_hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)NetRoutine,this,0,&ID);
	if(!m_hThread)
		return FALSE;

	return TRUE;
}

BOOL CUDPServer::Create()
{
	if (m_sdClient != INVALID_SOCKET)
	{
		SocketError("Create: m_sdClient Ã»ÓÐÊÍ·Å",0);
		closesocket(m_sdClient);
		m_sdClient = INVALID_SOCKET;
	}

	m_sdClient = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_sdClient == INVALID_SOCKET)
	{
		SocketError("Create: WSASocket = INVALID_SOCKET",WSAGetLastError());
		return FALSE;
	}

	DWORD in_buf=1;
	DWORD dwBytes;
	int TimeOut = 5000;
	if(WSAIoctl(m_sdClient, SIO_ENABLE_CIRCULAR_QUEUEING, &in_buf, sizeof(in_buf), NULL, 0, &dwBytes, NULL, NULL))
	{
		SocketError("Create: WSAIoctl",WSAGetLastError());
		return FALSE;
	}

	in_buf=0;
	if(WSAIoctl(m_sdClient, FIONBIO, &in_buf, sizeof(in_buf), NULL, 0, &dwBytes, NULL, NULL))
	{
		SocketError("Create: WSAIoctl",WSAGetLastError());
		return FALSE;
	}

	SOCKADDR_IN		addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(m_sIP);
	addr.sin_port = htons(m_nPort);					//htons(Port+(short)m_SocketID);
	if (bind(m_sdClient, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		SocketError("Create: bind",WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

DWORD WINAPI CUDPServer::NetRoutine(LPVOID pParam)
{
	CUDPServer *pServer = (CUDPServer*)pParam;
	WSABUF buff;
	INT dwAddrSize=sizeof(SOCKADDR_IN);
	DWORD dwFlags=0,nByte=0;
	SOCKADDR_IN addr;

	int nSize = pServer->GetMaxWorkBufferSize();
	buff.buf = new char[nSize];
	buff.len = nSize;

	int *pIp;
	unsigned short *pPort;
	int switchIp;
	int switchPort;

	while(1)
	{
		if(WaitForSingleObject(pServer->m_hEvent,0) == WAIT_OBJECT_0)
			break;

		while(1)
		{
			if (!WSARecvFrom(pServer->m_sdClient, &buff, 1, &nByte,	&dwFlags,(sockaddr*)&addr,&dwAddrSize, NULL, NULL))
			{
#ifdef ENABLE_ENCRPY_PACKET
                if (UDP_SIDE_CLIENT == pServer->m_side)
                    AuthCrypt::Instance()->DecryptRecv((uint8*)buff.buf,nByte);
#endif
					pPort = (unsigned short *)(&buff.buf[nByte-2]);
					pIp = (int *)(&buff.buf[nByte-6]);

					switchPort = *pPort;
					switchIp = *pIp;
					*pPort = addr.sin_port;
					*pIp = addr.sin_addr.s_addr;
					addr.sin_port = switchPort;
					addr.sin_addr.s_addr = switchIp;
					pServer->m_pOutServer->Send(buff.buf,nByte,addr.sin_addr.s_addr,addr.sin_port);

			}
			else
				break;
		}
	}

	return 0;
}

