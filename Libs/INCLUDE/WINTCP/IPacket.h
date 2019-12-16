#pragma once

#ifndef _IPACKET_H_
#define _IPACKET_H_

#include "Base/Locker.h"
#include "Base/Log.h"
#include "Base/bitStream.h"

#define STX                             0x27        //包文起始标识

struct stPacketHead
{
	char Stx;					//包标识
	unsigned short  Message;	//网络消息
	char LineServerId;			//线路编号
	char DestServerType;		//目标服务器
	unsigned long SrcZoneId;			//地图服务器编号
	unsigned long DestZoneId;			//目标
	int	 Id;					//玩家标识
	UINT PacketSize;			//包内容大小
};

class ISocket;
class IPacket
{
	CMyCriticalSection m_cs;

	//加密处理
	virtual bool EncodePacket(stPacketHead *,char *&);
	virtual bool DecodePacket(stPacketHead *,char *&);

protected:
	//Socket
	ISocket *m_pSocket;
public:
	static int	GetHeadSize();
	static int	GetTotalSize(const char *pInBuffer);
	static const char *SeekToHead(const char *pInBuffer,int &nBufferSize);
	static stPacketHead *BuildPacketHead(Base::BitStream &Packeter,unsigned short Msg,int Id = 0,char DestServerType=0,unsigned long DestZoneId=0,unsigned long SrcZoneId=0,char LineServerId=0);
	virtual bool HandlePacket(const char *pInData,int InSize) = 0;
public:
	IPacket();
	virtual bool Send(char *,int);

	virtual void Initialize();
	void BindSocket(ISocket *pSocket);
	ISocket *GetSocket(){return m_pSocket;};
};


#endif