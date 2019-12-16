
#include "IPacket.h"
#include "ISocket.h"

IPacket::IPacket()
{
	m_pSocket = NULL;
}

int IPacket::GetHeadSize()
{
	return sizeof(stPacketHead);	//����STX
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
			//���������i=0
			//���stxλ���ı�
			//1.û���ҵ�STX����������
			//2.�����ҵ���һ����ͷ��STX������������������һ����
			//3.Ҳ�����ҵ����������������STXֵͬ��������Ϊ��ʼ�㣬�����˴���

			break;
		}
	}

	if(i == nBufferSize)
		return NULL;		//û���ҵ�STXͷ������ȫ�������1���

	nBufferSize -= i;

	return (pInBuffer += i);		//������һ�����Ŀ�ʼ������ȫ�������2�����
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



