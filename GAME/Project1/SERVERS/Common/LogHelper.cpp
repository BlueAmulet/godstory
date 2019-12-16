#include "LogHelper.h"
#include <WinSock2.h>
#include <windows.h>
#include <process.h>
#include "Common/CommonClient.h"
#include "wintcp/AsyncSocket.h"
#include "Common/dbStruct.h"
#include "CommonPacket.h"
#include "base/bitStream.h"

class CLogHelperImpl
{
public:
    CLogHelperImpl();
    virtual ~CLogHelperImpl();

    void connect( const char* ipAddress, int port );
    void disconnect();
    void check();

    void writeLog( dbStruct* pStruct );

private:
    void _postPacket( Base::BitStream& packet );
    void _removePacket( int nId );

    static void WorkQuene( LPVOID param );
    static void TimeWork( LPVOID param );

    struct stLogItem
    {
        int		mTime;					// 记录发生时间
        int		mLastSendTime;			// 上一次发送时间,用于发送检查
        int		mId;					// 标识

        char*	mBuffer;				// 数据

        stLogItem()
        {
            memset( this, 0, sizeof( stLogItem ) );
            mTime = (int)time( NULL );
        }
    };

    class Process
        : public CommonPacket
    {
        typedef CommonPacket Parent;
    public:
        void OnConnect(int Error=0);
        void OnDisconnect();
    protected:
        bool HandleGamePacket(stPacketHead *pHead,int iSize);
        void HandleClientLogin();
        void HandleClientLogout();
    };

    typedef stdext::hash_map< int, stLogItem* > PendingList;

    bool				m_bIsReady;
    CAsyncSocket2*		m_pClient;

    int					mLogIdSeed;

    PendingList			mWaittingList;
    HANDLE				mIoHandle;
    HANDLE				mWorkThread;
};

void CLogHelperImpl::Process::OnConnect( int Error/*=0*/ )
{
    if( !Error)
        SendClientConnect();
}

void CLogHelperImpl::Process::OnDisconnect()
{
    Parent::OnDisconnect();
}

bool CLogHelperImpl::Process::HandleGamePacket( stPacketHead *pHead,int iSize )
{
    return true;
}

void CLogHelperImpl::Process::HandleClientLogin()
{

}

void CLogHelperImpl::Process::HandleClientLogout()
{

}

void CLogHelperImpl::WorkQuene( LPVOID param )
{
	int nRet;
	CLogHelperImpl* pThis = (CLogHelperImpl*)param;


	int dwBytes;
	int dwKey;

	stLogItem* pLogItem;

	while( 1 )
	{
		nRet = ::GetQueuedCompletionStatus( pThis->mIoHandle, (LPDWORD)&dwBytes, (PULONG_PTR)&dwKey, ( LPOVERLAPPED* )&pLogItem, -1 );

		if( pLogItem == NULL )
		{
			// TODO: write a quit log
			break;
		}

		{
			OLD_DO_LOCK( pThis );
			pThis->mWaittingList[pLogItem->mId] = pLogItem;

			// here we send the log request
			stPacketHead* pHead = (stPacketHead*)pLogItem->mBuffer;
			pLogItem->mLastSendTime = (int)time(NULL);

			pThis->m_pClient->Send( pLogItem->mBuffer, pHead->PacketSize + sizeof( stPacketHead ) );
		}
	}
}

void CLogHelperImpl::_removePacket( int nId )
{
    OLD_DO_LOCK( this );

    PendingList::iterator it;
    it = mWaittingList.find( nId );
    if( it == mWaittingList.end() )
        return ;

    stLogItem* pLogItem = it->second;

    CMemPool::GetInstance()->Free( (MemPoolEntry)pLogItem->mBuffer );
    CMemPool::GetInstance()->FreeObj<stLogItem>( pLogItem );

    mWaittingList.erase( it );
}

void CLogHelperImpl::_postPacket( Base::BitStream& packet )
{
    int nLength = packet.getPosition();
    stLogItem* pLogItem;
    CMemPool::GetInstance()->AllocObj<stLogItem>( pLogItem );
    pLogItem->mBuffer = (char*)CMemPool::GetInstance()->Alloc( nLength );
    memcpy( pLogItem->mBuffer, packet.getBuffer(), nLength );

    ::PostQueuedCompletionStatus( mIoHandle, 0, NULL, (LPOVERLAPPED)pLogItem );
}

void CLogHelperImpl::writeLog( dbStruct* pStruct )
{
    CMemGuard buf( MAX_PACKET_SIZE );
    Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
    stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ANY_LOGSERVER_Record );

    sendPacket.writeString( pStruct->mStructName.c_str() );
    pStruct->pack(sendPacket);

    pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
    _postPacket( sendPacket );
}

void CLogHelperImpl::connect( const char* ipAddress, int port )
{
    m_pClient = new CommonClient<Process>();

    if (0 != ipAddress)
    {
        m_pClient->Initialize( ipAddress, port );
        m_pClient->SetConnectType( ISocket::SERVER_CONNECT );

        m_pClient->Start();
    }
}

void CLogHelperImpl::disconnect()
{
    m_pClient->Stop();
}

void CLogHelperImpl::check()
{
    if( m_pClient && m_pClient->GetState() == ISocket::SSF_DEAD )
        m_pClient->Restart();
}

CLogHelperImpl::CLogHelperImpl()
{
    mIoHandle = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, NULL, 0 );
    mWorkThread = ( HANDLE )_beginthread( WorkQuene, 0, this );	
}

CLogHelperImpl::~CLogHelperImpl()
{
    ::PostQueuedCompletionStatus( mIoHandle, 0, NULL, NULL );
	WaitForSingleObject( mWorkThread, 3000 );
	CloseHandle(mIoHandle);
	if (m_pClient)
	{
		delete m_pClient;
		m_pClient = NULL;
	}
}

CLogHelper::CLogHelper():
    m_pImpl(new CLogHelperImpl)
{
}

CLogHelper::~CLogHelper()
{
    delete m_pImpl;
}

void CLogHelper::writeLog( dbStruct* pStruct )
{
    m_pImpl->writeLog(pStruct);
}

void CLogHelper::connect( const char* ipAddress, int port )
{
    m_pImpl->connect(ipAddress,port);
}

void CLogHelper::disconnect()
{
    m_pImpl->disconnect();
}

void CLogHelper::check()
{
    m_pImpl->check();
}
