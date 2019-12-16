#pragma once

#ifndef __ISOCKET_H__
#define __ISOCKET_H__

#if  _WIN32_WINNT < 0x0500
#undef   _WIN32_WINNT
#define  _WIN32_WINNT 0x0500
#endif

#include <WinSock2.h>
#include <MSWSock.h>
#include <MSTcpIP.h>

class ISocket;

struct OVERLAPPED_PLUS : public WSAOVERLAPPED
{
	char		bCtxWrite;
	int			ClientId;
	ISocket*	pClient;
	WSABUF		WsaBuf;
};

class ISocket		//Socket�ӿ���
{
public:
	static  bool InitNetLib()
	{
		WSADATA wd = {0};
		int dwError = WSAStartup(MAKEWORD(2, 2), &wd);
		if (dwError)
			return false;

		return true;
	}

	static  bool UninitNetLib()
	{
		int dwError = WSACleanup();
		if(dwError)
			return false;

		return true;
	}

	virtual bool Initialize(const char* ip, int port,void *)		=0;		//��ʼ��
	virtual bool Start()											=0;		//��ʼ����
	virtual bool Stop ()											=0;		//ֹͣ����
	virtual bool Restart()											=0;		//��������
	virtual bool Send(const char *,int)								=0;		//��������
	virtual bool Connect()											=0;		//����
	virtual bool Disconnect(bool bForce)							=0;		//�Ͽ�����

protected:
	virtual bool HandleReceive(OVERLAPPED_PLUS *,int ByteReceived)	=0;
	virtual bool HandleConnect(OVERLAPPED_PLUS *,int ByteReceived)	=0;
	virtual bool HandleSend(OVERLAPPED_PLUS *,int ByteSended)		=0;

protected:
	virtual void OnDisconnect()										=0;		//���Ͽ�����
	virtual void OnReceive(const char *pBuf, int nBufferSize)		=0;		//�����յ�����
	virtual void OnConnect(int nErrorCode)							=0;		//���������

	virtual void OnClear()											=0;		//�����¸�λ����Ҫ��һЩ�����������

public:
	//��״̬
	enum SERVER_STATE_FLAGS
	{
		SSF_RESTARTING,
		SSF_DEAD,
		SSF_ACCEPTING,
		SSF_CONNECTING,
		SSF_CONNECTED,
		SSF_SHUTTING_DOWN,
		SSF_SHUT_DOWN,
	};
public:
	enum
	{
		CLIENT_CONNECT ,
		SERVER_CONNECT
	};

public:
	ISocket()
	{
		m_nState=SSF_DEAD;m_nSocket=INVALID_SOCKET;m_nPort=0;m_sIP[0]=0;
		m_MaxSendBufferSize= 1024;m_MaxReceiveBufferSize= 1024;
		m_nConnectType = CLIENT_CONNECT;m_ClientId=0;

		m_SendTimes=0;
		m_PendSendTimes=0;

		m_ReceiveTimes=0;
	}

	SERVER_STATE_FLAGS GetState(){	return m_nState; };

	virtual void SetMaxSendBufferSize(int MaxSendSize){m_MaxSendBufferSize = MaxSendSize;}
	int GetMaxSendBufferSize(){ return m_MaxSendBufferSize;}

	virtual void SetMaxReceiveBufferSize(int MaxReceiveSize){m_MaxReceiveBufferSize = MaxReceiveSize;}
	int GetMaxReceiveBufferSize(){ return m_MaxReceiveBufferSize;}

	//������������
	void SetConnectType(int Type){m_nConnectType = Type;}
	int GetConnectType(){return m_nConnectType;}
	virtual void SetClientId(int Id)	{	m_ClientId=Id;	}
	int GetClientId()	{	return m_ClientId;	}
	bool HasClientId()			{  return m_ClientId!=0;}

protected:
	SERVER_STATE_FLAGS	m_nState;
	SOCKET				m_nSocket;				
	int					m_nPort;
	char				m_sIP[30];

	int					m_MaxReceiveBufferSize;
	int					m_MaxSendBufferSize;

protected:
	char				m_nConnectType;				//��������
	int					m_ClientId;					//����ID������ʶ���Ӷ���

public:
	int					m_SendTimes;
	int					m_PendSendTimes;

	int					m_ReceiveTimes;
};

#endif