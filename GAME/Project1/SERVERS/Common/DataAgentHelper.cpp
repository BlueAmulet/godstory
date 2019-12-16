#include "DataAgentHelper.h"
#include "zlib/zlib.h"

CDataAgentHelper* g_DataAgentHelper = NULL;

CDataAgentHelper::CDataAgentHelper()
{
	m_bIsReady		= false;
	m_pClient		= NULL;
}


CDataAgentHelper::~CDataAgentHelper()
{
	Close();
}

void CDataAgentHelper::Init( const char* szIpAddress, int nPort )
{
	m_pClient = new CommonClient<DataAgentProcess>;
	m_pClient->Initialize( szIpAddress, nPort );
	m_pClient->SetConnectType( ISocket::SERVER_CONNECT );

	m_pClient->Start();
	
	g_DataAgentHelper = this;
}

void CDataAgentHelper::Close()
{
	if( m_pClient )
	{
		delete m_pClient;
		m_pClient = NULL;
	}

	g_DataAgentHelper = NULL;
}

void CDataAgentHelper::Send( int nId, int nType, Base::BitStream& stream )
{
	//if( !m_bIsReady )
	//	return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	CMemGuard b( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, SERVER_DATAAGENT_Message, nId, SERVICE_DATAAGENT, 0, nType );

	// copy stream to SendPacket
	int nLength = ( stream.getPosition() - sizeof( stPacketHead ) ) * 8;
	int nPos = stream.getPosition();

	if( nLength < 0 )
		nLength = 0;

	stream.setPosition( sizeof( stPacketHead ) );
	stream.readBits( nLength, b.get() );
	stream.setPosition( nPos );

	nLength /= 8;

	CMemGuard zipBuf( MAX_PACKET_SIZE );
	int ZipBufSize  = MAX_PACKET_SIZE;
	int ret = compress( (Bytef*)zipBuf.get(),(uLongf*)&ZipBufSize,(const Bytef *)b.get(),nLength);

	if( !ret )
	{
		SendPacket.writeFlag( true );
		SendPacket.writeInt( ZipBufSize, 16 );
		
		SendPacket.writeBits( ZipBufSize * 8, zipBuf.get() );
	
	}
	else
	{
		SendPacket.writeFlag( false );
	}

	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
	m_pClient->Send( SendPacket );
}

void CDataAgentHelper::Check()
{
	if( m_pClient )
	{
		if( m_pClient->GetState() == ISocket::SSF_DEAD )
			m_pClient->Restart();
	}
}

void CDataAgentHelper::DataAgentProcess::OnConnect(int Error/* =0 */)
{
	if( !Error)
		SendClientConnect();
}

void CDataAgentHelper::DataAgentProcess::OnDisconnect()
{
	if( g_DataAgentHelper )
		g_DataAgentHelper->m_bIsReady = false;

	Parent::OnDisconnect();

	printf("\n与DataAgent断开连接\n");
}

void CDataAgentHelper::DataAgentProcess::HandleClientLogin()
{
	if( g_DataAgentHelper )
		g_DataAgentHelper->m_bIsReady = true;

	printf("\n与DataAgent建立连接\n");
}

void CDataAgentHelper::DataAgentProcess::HandleClientLogout()
{

}

bool CDataAgentHelper::DataAgentProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	return true;
}

