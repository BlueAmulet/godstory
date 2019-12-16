
#pragma once
#include "base/Locker.h"
#include "base/Log.h"
#include <WinSock2.h>

#define UDP_SERVER_WORK_BUFFER_SIZE 2000

class CUDPServer
{
public:
    enum UDP_SIDE
    {
        UDP_SIDE_CLIENT,
        UDP_SIDE_ZONE,
    };

	CUDPServer(const char *ip,int nPort,int side);
	virtual ~CUDPServer(void);

	BOOL Send(const char *,int,ULONG ,short);
	void SocketError(char *from,int error);

	virtual BOOL Start();
	virtual BOOL Stop();
	BOOL Create();

public:
	void SetMaxWorkBufferSize(size_t MaxBuffSize) {m_MaxWorkBufferSize = MaxBuffSize;}
	size_t GetMaxWorkBufferSize(){ return m_MaxWorkBufferSize;}

	void SetOutServer(CUDPServer *pOut) {m_pOutServer = pOut;}

public:
	int					m_MaxWorkBufferSize;
	SOCKET				m_sdClient;
	SOCKET				m_sdClientOut;
	int					m_nPort;
	char				m_sIP[64];
    UDP_SIDE            m_side;

	HANDLE				m_hThread;
	HANDLE				m_hEvent;

	CUDPServer *		m_pOutServer;

	static DWORD WINAPI NetRoutine(LPVOID pParam);
};
