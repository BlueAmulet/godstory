
#include "IPacket.h"
#include "ISocket.h"

IPacket::IPacket()
{
	m_pSocket = NULL;
}

int IPacket::GetHeadSize()
{
	return sizeof(stPacketHead);	//包括STX
}

int	IPacket::GetTotalSize(const char *pInBuffer)
{
	stPacketHead *pHead = (stPacketHead *)pInBuffer;

	return sizeof(stPacketHead)+pHead->PacketSize;
}

const char *IPacket::SeekToHead(const char *pInBuffer,int &nBufferSize)
{
    int i=0;

	for(i=0;i<nBufferSize;i++)
	{
		if(pInBuffer[i] == STX)
		{
			//正常情况下i=0
			//如果stx位被改变
			//1.没有找到STX，丢弃本包
			//2.可能找到下一个包头的STX，丢弃本包，处理下一个包
			//3.也可能找到本包数据内容里和STX同值的数据做为起始点，隐含了错误

			break;
		}
	}

	if(i == nBufferSize)
		return NULL;		//没有找到STX头部，完全解决错误1情况

	nBufferSize -= i;

	return (pInBuffer += i);		//跳到下一个包的开始处，完全解决错误2的情况
}

stPacketHead *IPacket::BuildPacketHead(Base::BitStream &Packeter,unsigned short Msg,int Id,char DestServerType,unsigned long DestZoneId,unsigned long SrcZoneId,char LineServerId)
{
	stPacketHead *pHead		= reinterpret_cast<stPacketHead *>(Packeter.getBuffer());
	stPacketHead content;
	content.Stx				= STX;
	content.DestServerType	= DestServerType;
	content.Message			= Msg;
	content.LineServerId	= LineServerId;
	content.SrcZoneId		= SrcZoneId;
	content.DestZoneId		= DestZoneId;
	content.PacketSize		= 0;
	content.Id				= Id;

	Packeter.writeBits(GetHeadSize() * Base::Bit8,&content);
	return pHead;
}

void IPacket::Initialize()
{
}

void IPacket::BindSocket(ISocket *pSocket)
{
	m_pSocket = pSocket;
}

bool IPacket::Send(char *pOutData,int iSize)
{
	if(!m_pSocket)
		return false;

	return m_pSocket->Send(pOutData,iSize);
}

bool IPacket::EncodePacket(stPacketHead *,char *&)
{
	return false;
}

bool IPacket::DecodePacket(stPacketHead *,char *&)
{
	return false;
}



