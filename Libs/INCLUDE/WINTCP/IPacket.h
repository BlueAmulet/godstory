#pragma once

#ifndef _IPACKET_H_
#define _IPACKET_H_

#include "Base/Locker.h"
#include "Base/Log.h"
#include "Base/bitStream.h"

#define STX                             0x27        //������ʼ��ʶ

struct stPacketHead
{
	char Stx;					//����ʶ
	unsigned short  Message;	//������Ϣ
	char LineServerId;			//��·���
	char DestServerType;		//Ŀ�������
	unsigned long SrcZoneId;			//��ͼ���������
	unsigned long DestZoneId;			//Ŀ��
	int	 Id;					//��ұ�ʶ
	UINT PacketSize;			//�����ݴ�С
};

class ISocket;
class IPacket
{
	CMyCriticalSection m_cs;

	//���ܴ���
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