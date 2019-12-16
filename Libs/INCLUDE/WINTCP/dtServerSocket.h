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
		MAX_COMPLETION_THREAD_NUM	= 1,		//���߳�
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
	virtual bool Restart()										{ return false;}				//����ʵ��
	virtual bool Send(const char *,int)							{ return false;}				//����ʵ��
	virtual bool Connect()										{ return false;}				//����ʵ��
	virtual bool Disconnect(bool bForce)						{ return false;}				//����ʵ��

protected:
	virtual bool HandleReceive(OVERLAPPED_PLUS *,int ByteReceived)		{ return false;}		//����ʵ��
	virtual bool HandleConnect(OVERLAPPED_PLUS *,int ByteReceived)		{ return false;}		//����ʵ��
	virtual bool HandleSend(OVERLAPPED_PLUS *,int ByteSended)			{ return false;}		//����ʵ��

protected:
	virtual void OnDisconnect()									{}
	virtual void OnReceive(const char *pBuf, int nBufferSize)	{ return;		}				//����ʵ��
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
	//��ʱ���
	void DisableTimeoutCheck()							{m_bDisableConnectCheck=true;};
	void SetIdleTimeout(int idleTimeout)				{m_nIdleTimeout = idleTimeout;};
	void SetConnectionTimeout(int ConnectionTimeout)	{m_nConnectedTimeout=ConnectionTimeout;};

	//״̬���
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

	int			m_nClientCount;			//��ǰ�Ѿ����õĿͻ�������
	int			m_nMaxClients;			//���ɲ����Ŀͻ�������
	int			m_nMinClients;			//��С�ɲ����Ŀͻ�������
	volatile int	m_nIdSeed;

	int			m_nIdleTimeout;			//����ʱ��
	int			m_nConnectedTimeout;	//�������ӵ�ʱ��

	bool		m_bShuttingDown;		//�Ƿ�׼���رձ�ʶ
	bool		m_bCanAccept;			//�Ƿ������������
	bool		m_bDisableConnectCheck;	//���������Ӽ��

protected:
	stdext::hash_map<int,dtServerSocketClient*>		m_ClientList;	//���Ӵ������

protected:
	//ά���߳�
	HANDLE		m_hAddClientEvent;
	HANDLE		m_hEndTimerEvent;
	HANDLE		m_hTimerThread;

	//��ɶ˿ں��߳�
	HANDLE		m_CompletionPort;
	HANDLE		m_CompletionThread[MAX_COMPLETION_THREAD_NUM];		//�̳߳�
	int			m_CompletionThreadNum;
};

#endif