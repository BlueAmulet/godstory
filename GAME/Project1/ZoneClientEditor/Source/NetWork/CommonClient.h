#ifndef COMMON_CLIENT_H
#define COMMON_CLIENT_H

#include "app/net/tcpObject.h"
#include "WinTCP/TcpSocket.h"

#ifdef NTJ_SERVER
#include "ServerPacketProcess.h"
#else
#include "UserPacketProcess.h"
#endif

class CommonClient : public TCPObject,public CTcpSocket
{
	typedef TCPObject Parent;

public:
	CommonClient();


	//CTcpSocket 废弃接口=============================
	bool Initialize(const char *ip,int nPort,void *Param=NULL){return false;};
	bool Start()	{return false;};
	bool Stop ()	{return false;};
	bool Restart()	{return false;};
	bool Connect()	{return false;};
	
	bool HandleReceive(OVERLAPPED_PLUS *,int ByteReceived)	{return false;};
	bool HandleConnect(OVERLAPPED_PLUS *,int ErrorCode)		{return false;};
	bool HandleSend(OVERLAPPED_PLUS *,int ByteSended)		{return false;};

protected:
	void OnDisconnect()	{return;};
	void OnConnect(int)	{return;};
	//===============================================

public:	
	//TCPObject的改变接口
	void onConnected();
	U32 onReceive(U8 *buffer, U32 bufferLen);
	void onDisconnect();

	//CTcpSocket改变接口
	bool Send(const char *buffer,int bufferLen);
	bool Disconnect(bool bForce=false);
};

class TCPObject2 : public CommonClient
{
public:
	typedef CommonClient Parent;
	DECLARE_CONOBJECT(TCPObject2);
};
#endif



