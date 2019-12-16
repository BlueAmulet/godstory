#pragma once

#ifndef __DTSERVERSOCKET_H__
#define __DTSERVERSOCKET_H__

#include <vector>
#include <hash_map>

#include "TcpSocket.h"
#include "Base/Locker.h"
#include "Base/Log.h"
#include "Base/memPool.h"
#include "base/bitStream.h"

struct stServerParam
{
	int MinClients;
	int MaxClients;
};

class dtServerSocket;
class dtServerSocketClient;
typedef void*(*NET_CALLBACK_FN)(dtServerSocket *pServer,dtServerSocketClient *pClient,int Msg,void *pData);

class dtServerSocket : public ISocket
{
public:
	typedef CTcpSocket Parent;

	enum
	{
		MAX_COMPLETION_THREAD_NUM	= 1,		//单线程
		ADD_CLIENT_NUM				= 20,
		MAX_PENDING_SEND			= 2,
	};

	enum OP_CODE
	{
		OP_NORMAL,
		OP_QUIT,
		OP_RESTART,
		OP_DISCONNECT,
		OP_SEND,
		OP_ADDCLIENT,
		OP_MAINTENANCE,
		
		OP_ON_CONNECT,
		OP_ON_DISCONNECT,
		OP_ON_PACKET,
	};

public:
	virtual bool Initialize(const char* ip, int port,void *param=NULL);
	virtual bool Start();
	virtual bool Stop ();
	virtual bool Restart()										{ return false;}				//不用实现
	virtual bool Send(const char *,int)							{ return false;}				//不用实现
	virtual bool Connect()										{ return false;}				//不用实现
	virtual bool Disconnect(bool bForce)						{ return false;}				//不用实现

protected:
	virtual bool HandleReceive(OVERLAPPED_PLUS *,int ByteReceived)		{ return false;}		//不用实现
	virtual bool HandleConnect(OVERLAPPED_PLUS *,int ByteReceived)		{ return false;}		//不用实现
	virtual bool HandleSend(OVERLAPPED_PLUS *,int ByteSended)			{ return false;}		//不用实现

protected:
	virtual void OnDisconnect()									{}
	virtual void OnReceive(const char *pBuf, int nBufferSize)	{ return;		}				//不用实现
	virtual void OnConnect(int nErrorCode)						{}
	virtual void OnClear()										{}

private:
	static unsigned int WINAPI EventRoutine(LPVOID Param);
	static unsigned int WINAPI TimerRoutine(LPVOID Param);

public:
	friend class dtServerSocketClient;

	dtServerSocket(void);
	virtual ~dtServerSocket(void);

public:
	//超时检测
	void DisableTimeoutCheck()							{m_bDisableConnectCheck=true;};
	void SetIdleTimeout(int idleTimeout)				{m_nIdleTimeout = idleTimeout;};
	void SetConnectionTimeout(int ConnectionTimeout)	{m_nConnectedTimeout=ConnectionTimeout;};

	//状态检查
	bool IsShuttingDown()								{return m_bShuttingDown;};
	bool CanAccept()									{return m_bCanAccept;};
	int GetEnableClientNum()							{return m_nMaxClients-m_nClientCount;};

	void PostEvent(int Msg,void *pData=NULL);
	void PostEvent(int Msg,int id);
	void Send(int id,const char *,int);
	void Send(int id,Base::BitStream &SendPacket);

protected:
	virtual ISocket *LoadClient	();
	bool AddClient				(int Num);
	int  AssignClientId			();
	dtServerSocketClient *GetClientById(int id);
	virtual int Maintenance		();

protected:
	CMemPool*			m_pMemPool;

	int			m_nClientCount;			//当前已经启用的客户端数量
	int			m_nMaxClients;			//最大可操作的客户端数量
	int			m_nMinClients;			//最小可操作的客户端数量
	volatile int	m_nIdSeed;

	int			m_nIdleTimeout;			//空闲时间
	int			m_nConnectedTimeout;	//整个连接的时间

	bool		m_bShuttingDown;		//是否准备关闭标识
	bool		m_bCanAccept;			//是否允许接入连接
	bool		m_bDisableConnectCheck;	//不允许连接检查

protected:
	stdext::hash_map<int,dtServerSocketClient*>		m_ClientList;	//连接处理队列

protected:
	//维护线程
	HANDLE		m_hAddClientEvent;
	HANDLE		m_hEndTimerEvent;
	HANDLE		m_hTimerThread;

	//完成端口和线程
	HANDLE		m_CompletionPort;
	HANDLE		m_CompletionThread[MAX_COMPLETION_THREAD_NUM];		//线程池
	int			m_CompletionThreadNum;
};

#endif