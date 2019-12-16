#pragma once

#ifndef __ASYNSOCKET_H__
#define __ASYNSOCKET_H__

#include <list>
#include "Base/Locker.h"
#include "Base/Log.h"
#include "Base/memPool.h"
#include "base/bitStream.h"
#include "TcpSocket.h"

//�ص�IO��
class CAsyncSocket2 : public CTcpSocket
{
public:
	typedef CTcpSocket Parent;

public:
	bool Initialize(const char* ip, int port,void *pParam = NULL);
	bool Start();
	bool Stop ();
	bool Restart();
	bool Send(const char *,int);
	bool Connect();
	bool Disconnect(bool bForce=false);

protected:
	bool HandleReceive(OVERLAPPED_PLUS *,int ByteReceived);
	bool HandleConnect(OVERLAPPED_PLUS *,int ErrorCode);
	bool HandleSend(OVERLAPPED_PLUS *,int ByteSended);

protected:
	void OnDisconnect()										{};
	//void OnReceive(const char* pBuf, int nBufferSize)		{};
	void OnConnect(int nErrorCode=0)						{};

public:
	CAsyncSocket2();
	virtual ~CAsyncSocket2(void);

	bool Send(Base::BitStream &SendPacket);

protected:
	void SocketError(char *from,int error);
	bool CreateSocket();

	void SetMaxReceiveBufferSize(int MaxReceiveSize);

	std::list<OVERLAPPED_PLUS* > m_OutBuffer;
	OVERLAPPED_PLUS* GetBuffer(int iSize);
	bool ReleaseBuffer(OVERLAPPED_PLUS*);
	
	static unsigned int WINAPI NetRoutine(LPVOID Param);
	static void CALLBACK ComplateRoutine(DWORD Error,DWORD Bytes,LPWSAOVERLAPPED Overlapped,DWORD InFlag);

public:
	CMyCriticalSection	m_cs;					/// �������

protected:
	sockaddr_in			m_LocalAddr;			/// ������ַ
	sockaddr_in			m_RemoteAddr;			/// Զ�̵�ַ

	CMyCriticalSection	m_CritWrite;			/// outbuffer���ٽ���
	CMyCriticalSection	m_CritFree;				/// �����ٽ���

	CMemPool*			m_pMemPool;

	bool				m_bPendingRead;			// a read is pending

	HANDLE				m_hSendEvent;
	HANDLE				m_hConnectEvent;
	HANDLE				m_hKillEvent;
	HANDLE				m_hThread;
};

#endif


