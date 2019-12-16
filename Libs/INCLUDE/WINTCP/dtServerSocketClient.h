#pragma once

#ifndef __DTSERVERSOCKETCLIENT_H__
#define __DTSERVERSOCKETCLIENT_H__

#include <list>

#include "ISocket.h"
#include "dtServerSocket.h"
#include "Base/Locker.h"
#include "Base/Log.h"
#include "Base/memPool.h"



class dtServerSocketClient : public CTcpSocket
{
	typedef CTcpSocket Parent;
public:
	virtual bool Initialize(const char* ip, int port,void *)	{ return true ;}
	virtual bool Start();
	virtual bool Stop ()										{ return false;}				//不用实现
	virtual bool Restart();
	virtual bool Send(const char *,int);
	virtual bool Connect()										{ return false;}				//不用实现
	virtual bool Disconnect(bool bForce);

    sockaddr_in  GetRemoteAddr(void)							{return m_RemoteAddr;}
protected:
	virtual bool HandleReceive(OVERLAPPED_PLUS *,int ByteReceived);
	virtual bool HandleConnect(OVERLAPPED_PLUS *,int ByteReceived);
	virtual bool HandleSend(OVERLAPPED_PLUS *,int ByteSended);

protected:
	virtual void OnDisconnect();
	//virtual void OnReceive(const char *pBuf, int nBufferSize)	{ return ;		};
	virtual void OnConnect(int nErrorCode);

public:
	friend class dtServerSocket;


protected:
	SOCKET				m_nListenSocket;			// 监听端口

	sockaddr_in			m_LocalAddr;				// 本地地址
	sockaddr_in			m_RemoteAddr;				// 远程地址

	dtServerSocket*		m_pServer;					/// Pointer to the server that this client belongs to.			
	HANDLE				m_CompletionPort;
	int					m_CompletionThreadNum;

	bool				m_bPendingRead;				/// a read is pending
	int					m_nLastTransTickCount;

	std::list<OVERLAPPED_PLUS*>	m_SendingList;

protected:
	CMemPool*			m_pMemPool;

	//内存管理
	OVERLAPPED_PLUS*	GetBuffer			(int iSize);

	bool				BeginSend			(OVERLAPPED_PLUS *lpOverlapped);
	void				SetListenSocket		(SOCKET sdListen);
	void				SetIoCompRoutine	(HANDLE CompletionPort,int ThreadNum);
	void				SetServer			(dtServerSocket *pServer);

	void				ResetVar			();

public:
						dtServerSocketClient(void);
	virtual				~dtServerSocketClient(void);

public:
	int					GetClientId			()														{return m_ClientId;}

	BOOL ReleaseBuffer(OVERLAPPED_PLUS*);
	void PostEvent(int Msg,void *pData=NULL);

	SERVER_STATE_FLAGS GetState()
	{ 
		return m_nState;	
	};

	int GetConnectTime();
	int GetIdleTime();
	char *GetConnectedIP()
	{
		return inet_ntoa(m_RemoteAddr.sin_addr);
	}

};

#endif