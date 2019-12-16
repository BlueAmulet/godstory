
#include "TcpSocket.h"
#include "IPacket.h"


CTcpSocket::CTcpSocket()
{
	m_pPacket			= NULL;
	m_pPacketHeadSize	= 0;

	m_pInBuffer			= NULL;
	m_bHalf				= FALSE;
	m_iHalfSize			= 0;			//初始化变量
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
	m_pPacketHeadSize = pPacket->GetHeadSize();							//包括STX
}

void CTcpSocket::SetMaxReceiveBufferSize(int MaxReceiveSize)
{
	Parent::SetMaxReceiveBufferSize(MaxReceiveSize);

	m_pInBuffer = new char [MaxReceiveSize];
}

void CTcpSocket::OnClear()
{
	m_bHalf				= FALSE;
	m_iHalfSize			= 0;			//初始化变量

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
		return;				//有错误
	}

	if(m_bHalf)
	{			//有没有上次没有检查完成的包？
		if(m_iHalfSize+nBufferSize < m_pPacketHeadSize)
		{	//包头还是没有收全
			memcpy(&m_pInBuffer[m_iHalfSize],lpVt,nBufferSize);
			m_iHalfSize += nBufferSize;
		}
		else
		{
			if(m_iHalfSize < m_pPacketHeadSize)
			{			//前面有没收齐包头
				int iSize = m_pPacketHeadSize-m_iHalfSize;

				memcpy(&m_pInBuffer[m_iHalfSize],lpVt,iSize);
				m_iHalfSize += iSize;

				lpVt+=iSize;
				nBufferSize-=iSize;
			}

			iPacketSize = m_pPacket->GetTotalSize(m_pInBuffer);		//m_pInBuffer包含STX

			if( iPacketSize < 0 )
				return ;

			if( m_iHalfSize+nBufferSize < iPacketSize )
			{				//没收完整继续
				if(m_iHalfSize+nBufferSize <= m_MaxReceiveBufferSize)
				{
					memcpy(&m_pInBuffer[m_iHalfSize],lpVt,nBufferSize);
					m_iHalfSize += nBufferSize;
				}
				else
				{
					assert( false );

					m_iHalfSize = 0;		//越界
					m_bHalf = FALSE;
					g_Log.WriteError("(A)致命的越界包,已经被忽略");
				}
			}else if( m_iHalfSize+nBufferSize == iPacketSize )
			{
				if(m_iHalfSize+nBufferSize <= m_MaxReceiveBufferSize)
				{
					memcpy(&m_pInBuffer[m_iHalfSize],lpVt,nBufferSize);
					OnHandleOnePacket(&m_pInBuffer[0]);	//包括STX
				}
				else
				{
					assert( false );
					
					m_iHalfSize = 0;
					m_bHalf = FALSE;	//越界
					g_Log.WriteError("(B)致命的越界包,已经被忽略");
					return;		//越界
				}
			}
			else if(m_iHalfSize+nBufferSize > iPacketSize)
			{
				int iSize = iPacketSize-m_iHalfSize;

				if(m_iHalfSize+iSize <= m_MaxReceiveBufferSize)
				{		//如果尺寸正常就处理,否则抛弃这个包
					memcpy(&m_pInBuffer[m_iHalfSize],lpVt, iSize);
					OnHandleOnePacket(&m_pInBuffer[0]);	//包括STX
				}
				else
				{
					assert( false );

					m_iHalfSize = 0;
					m_bHalf = FALSE;	//越界
					g_Log.WriteError("(C)致命的越界包,已经被忽略");
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
		{	//包头还没有收全
			memcpy(&m_pInBuffer[0],lpVt,nBufferSize); 
			m_bHalf = TRUE;
			m_iHalfSize += nBufferSize;
		}
		else
		{
			iPacketSize = m_pPacket->GetTotalSize(lpVt);

			if(nBufferSize == m_pPacketHeadSize)
			{		//如果只有包头没有内容的消息也可以通过
				if( iPacketSize-m_pPacketHeadSize != 0)
				{
					memcpy(&m_pInBuffer[0],lpVt,nBufferSize); 
					m_bHalf = TRUE;
					m_iHalfSize += nBufferSize;
					return;
				}
			}

			if(iPacketSize >= m_MaxReceiveBufferSize)
			{		//一个不正常的尺寸，解决错误3的部分情况
				lpVt++;			//跳过这个STX内容
				nBufferSize--;	
				assert( false );
				goto CheckSTX; 	//查找下一个STX
				g_Log.WriteError("(D)致命的越界包,已经被忽略");
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
				OnHandleOnePacket(&m_pInBuffer[0]);	//包括STX
			}
			else if(nBufferSize > iPacketSize)
			{
				memcpy(m_pInBuffer,lpVt,iPacketSize); 
				OnHandleOnePacket(&m_pInBuffer[0]);	//包括STX

				lpVt += iPacketSize;
				OnReceive(lpVt,nBufferSize-iPacketSize);
			}
		}
	}

	return;
}




