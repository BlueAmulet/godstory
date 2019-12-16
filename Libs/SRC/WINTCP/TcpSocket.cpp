
#include "TcpSocket.h"
#include "IPacket.h"


CTcpSocket::CTcpSocket()
{
	m_pPacket			= NULL;
	m_pPacketHeadSize	= 0;

	m_pInBuffer			= NULL;
	m_bHalf				= FALSE;
	m_iHalfSize			= 0;			//��ʼ������
}

CTcpSocket::~CTcpSocket()
{
	if(m_pInBuffer){
		delete [] m_pInBuffer;
		m_pInBuffer = NULL;
	}

	if(m_pPacket)
		delete m_pPacket;
}

void CTcpSocket::BindPacket(IPacket* pPacket)
{
	m_pPacket = pPacket;
	m_pPacketHeadSize = pPacket->GetHeadSize();							//����STX
}

void CTcpSocket::SetMaxReceiveBufferSize(int MaxReceiveSize)
{
	Parent::SetMaxReceiveBufferSize(MaxReceiveSize);

	m_pInBuffer = new char [MaxReceiveSize];
}

void CTcpSocket::OnClear()
{
	m_bHalf				= FALSE;
	m_iHalfSize			= 0;			//��ʼ������

	if(m_pPacket)
		m_pPacket->Initialize();
}

void CTcpSocket::OnHandleOnePacket(char* pInData)
{
	m_iHalfSize = 0;
	m_bHalf = FALSE;

	int iPacketSize = m_pPacket->GetTotalSize(pInData);
	m_pPacket->HandlePacket(pInData,iPacketSize);
}

#include <assert.h>

void CTcpSocket::OnReceive(const char *pInData, int nBufferSize)
{
	const char *lpVt = pInData;
	int iPacketSize;

	if(!m_pPacket || !pInData || nBufferSize<=0)
	{
		g_Log.WriteError("CServerClient::OnReceive(), parse packet error!");
		return;				//�д���
	}

	if(m_bHalf)
	{			//��û���ϴ�û�м����ɵİ���
		if(m_iHalfSize+nBufferSize < m_pPacketHeadSize)
		{	//��ͷ����û����ȫ
			memcpy(&m_pInBuffer[m_iHalfSize],lpVt,nBufferSize);
			m_iHalfSize += nBufferSize;
		}
		else
		{
			if(m_iHalfSize < m_pPacketHeadSize)
			{			//ǰ����û�����ͷ
				int iSize = m_pPacketHeadSize-m_iHalfSize;

				memcpy(&m_pInBuffer[m_iHalfSize],lpVt,iSize);
				m_iHalfSize += iSize;

				lpVt+=iSize;
				nBufferSize-=iSize;
			}

			iPacketSize = m_pPacket->GetTotalSize(m_pInBuffer);		//m_pInBuffer����STX

			if( iPacketSize < 0 )
				return ;

			if( m_iHalfSize+nBufferSize < iPacketSize )
			{				//û����������
				if(m_iHalfSize+nBufferSize <= m_MaxReceiveBufferSize)
				{
					memcpy(&m_pInBuffer[m_iHalfSize],lpVt,nBufferSize);
					m_iHalfSize += nBufferSize;
				}
				else
				{
					assert( false );

					m_iHalfSize = 0;		//Խ��
					m_bHalf = FALSE;
					g_Log.WriteError("(A)������Խ���,�Ѿ�������");
				}
			}else if( m_iHalfSize+nBufferSize == iPacketSize )
			{
				if(m_iHalfSize+nBufferSize <= m_MaxReceiveBufferSize)
				{
					memcpy(&m_pInBuffer[m_iHalfSize],lpVt,nBufferSize);
					OnHandleOnePacket(&m_pInBuffer[0]);	//����STX
				}
				else
				{
					assert( false );
					
					m_iHalfSize = 0;
					m_bHalf = FALSE;	//Խ��
					g_Log.WriteError("(B)������Խ���,�Ѿ�������");
					return;		//Խ��
				}
			}
			else if(m_iHalfSize+nBufferSize > iPacketSize)
			{
				int iSize = iPacketSize-m_iHalfSize;

				if(m_iHalfSize+iSize <= m_MaxReceiveBufferSize)
				{		//����ߴ������ʹ���,�������������
					memcpy(&m_pInBuffer[m_iHalfSize],lpVt, iSize);
					OnHandleOnePacket(&m_pInBuffer[0]);	//����STX
				}
				else
				{
					assert( false );

					m_iHalfSize = 0;
					m_bHalf = FALSE;	//Խ��
					g_Log.WriteError("(C)������Խ���,�Ѿ�������");
				}

				lpVt += iSize;
				OnReceive(lpVt,nBufferSize-iSize);
			}
		}
	}
	else
	{

CheckSTX:

		lpVt = m_pPacket->SeekToHead(lpVt,nBufferSize);
		if(!lpVt)
			return;

		if(nBufferSize < m_pPacketHeadSize)
		{	//��ͷ��û����ȫ
			memcpy(&m_pInBuffer[0],lpVt,nBufferSize); 
			m_bHalf = TRUE;
			m_iHalfSize += nBufferSize;
		}
		else
		{
			iPacketSize = m_pPacket->GetTotalSize(lpVt);

			if(nBufferSize == m_pPacketHeadSize)
			{		//���ֻ�а�ͷû�����ݵ���ϢҲ����ͨ��
				if( iPacketSize-m_pPacketHeadSize != 0)
				{
					memcpy(&m_pInBuffer[0],lpVt,nBufferSize); 
					m_bHalf = TRUE;
					m_iHalfSize += nBufferSize;
					return;
				}
			}

			if(iPacketSize >= m_MaxReceiveBufferSize)
			{		//һ���������ĳߴ磬�������3�Ĳ������
				lpVt++;			//�������STX����
				nBufferSize--;	
				assert( false );
				goto CheckSTX; 	//������һ��STX
				g_Log.WriteError("(D)������Խ���,�Ѿ�������");
			}

			if(nBufferSize < iPacketSize)
			{
				memcpy(&m_pInBuffer[m_iHalfSize],lpVt,nBufferSize);
				m_bHalf = TRUE;
				m_iHalfSize += nBufferSize;
			}
			else if(nBufferSize == iPacketSize)
			{
				memcpy(m_pInBuffer,lpVt,nBufferSize); 
				OnHandleOnePacket(&m_pInBuffer[0]);	//����STX
			}
			else if(nBufferSize > iPacketSize)
			{
				memcpy(m_pInBuffer,lpVt,iPacketSize); 
				OnHandleOnePacket(&m_pInBuffer[0]);	//����STX

				lpVt += iPacketSize;
				OnReceive(lpVt,nBufferSize-iPacketSize);
			}
		}
	}

	return;
}




