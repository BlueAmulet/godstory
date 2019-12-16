#ifndef SERVER_FRAMEWORK_H
#define SERVER_FRAMEWORK_H

#define SAFE_DELETE( p ) if( p ) {delete p;p = NULL;} 

#pragma warning( disable: 4356 )

#include "wintcp/dtServerSocket.h"
#include "WaitObject.h"
#include "Common/Script.h"

#include "Common/WorkQueue.h"
#include "base/Locker.h"
#include "base/bitStream.h"
#include "common/CommonPacket.h"
#include "common/CommonServer.h"
#include "WINTCP\dtServerSocketClient.h"

#include <string>

typedef bool (*ClassEventFunction)(void*, int ,stPacketHead *,Base::BitStream *);

#define DECLARE_SERVER_MODULE_MESSAGE() static ClassEventFunction fnList[END_NET_MESSAGE];
#define IMPL_SERVER_MODULE_MESSAGE(p) ClassEventFunction p::fnList[END_NET_MESSAGE] = { 0 };

#define ServerEventFunction(c,funct,message)  \
	bool __ef_##funct( c* pThis, int msg, stPacketHead* phead, Base::BitStream* pPack ); \
	EventFunctionBuilder< message, (ClassEventFunction)__ef_##funct, c::fnList > _ef_##funct##builder; \
	bool __ef_##funct( c* pThis, int msg, stPacketHead* phead, Base::BitStream* pPack )

struct IServerModuleBase
{
	static ClassEventFunction fnList[END_NET_MESSAGE];

	virtual bool onMessage( IServerModuleBase* pThis, int msg, stPacketHead* phead, Base::BitStream* pPack ) = 0;
};

template<int message, ClassEventFunction eventFunction, ClassEventFunction* fnList>
struct EventFunctionBuilder
{
	EventFunctionBuilder()
	{
		IServerModuleBase::fnList[message] = eventFunction;
	}
};



template< class ServerClass, int BeginMessage, int EndMessage >
struct IServerModule : IServerModuleBase
{
	IServerModule()
	{
		ServerClass::getInstance()->registerModule( this, BeginMessage, EndMessage );
	}

	bool onMessage( IServerModuleBase* pThis, int msg, stPacketHead* phead, Base::BitStream* pPack )
	{
		if( IServerModuleBase::fnList[msg] )
			return IServerModuleBase::fnList[msg]( pThis, msg, phead, pPack );

		return true;
	}
};

template<class T>
class CServerFramework : CommonPacket
{
private:

	friend class CommonServer<T>;

	struct t_server_param
	{
		int				workThreadCount;
		int				port;
		const char*		ipAddr;
		stServerParam	param;
	};


protected:
	CServerFramework()
	{
		mWaitObject			= NULL;
		mServer				= NULL;
		mWorkQueue			= NULL;

		memset( mModuleMessageMap, 0, sizeof( IServerModuleBase* ) * END_NET_MESSAGE );
	}

	virtual ~CServerFramework()
	{
		SAFE_DELETE( mWaitObject );
		SAFE_DELETE( mServer );
		SAFE_DELETE( mWorkQueue );
	}

public:
	void init( const char* szServerName, const char* exitEventName )
	{
		mWaitObject = new CWaitObject( exitEventName );

		mWorkQueue = new CWorkQueue();
		mServer = new CommonServer< T >;

		// init param
		std::string str = szServerName;
		str += ".c";
		SCRIPT->loadFile( str.c_str() );

		t_server_param param;

		_loadParam(param);

		printf("SERVER:%s - [IP:%s\tPort:%d]\n\r", szServerName, param.ipAddr, param.port );
		printf("Initialize ... \n\r");

		ISocket::InitNetLib();

		mWorkQueue->Initialize( _eventProcess, param.workThreadCount );
		mServer->Initialize( param.ipAddr, param.port, &param.param );
		mServer->Start(); // start the service

		onInit();	

		printf("Initialize completed .\n\r");
	}

	virtual void onInit() {;}

	virtual void onShutdown() {;}

	void shutdown()
	{
		onShutdown();

		ISocket::UninitNetLib();
	}

	virtual void onMainLoop()
	{

	}

	inline dtServerSocket* getServerSocket()
	{
		return mServer;
	}

	void main()
	{
		if( !mWaitObject->IsReady() )
			return ;

		while( !mWaitObject->Wait( 1000 ) )
		{
			onMainLoop();
		}
	}

	virtual void onWork()
	{

	}

	void registerModule( IServerModuleBase* pThis, int beginMsg, int endMsg )
	{
		for( int i = beginMsg; i <= endMsg; i++ )
		{
			mModuleMessageMap[i] = pThis;
		}
	}

	inline CWorkQueue* getWorkQueue()
	{
		return mWorkQueue;
	}

	inline CTCCWarper* getScript()
	{
		return SCRIPT;
	}


private:

	void _loadParam( t_server_param &param )
	{
		param.ipAddr = SCRIPT->get<const char*>("ipAddress");
		param.port = SCRIPT->get<int>( "port");
		param.param.MaxClients = SCRIPT->get<int>( "maxClientCount" );
		param.param.MinClients = SCRIPT->get<int>( "minClientCount" );
		param.workThreadCount = SCRIPT->get<int>( "workThreadCount" );
	}

	static int _eventProcess( LPVOID param )
	{
		WorkQueueItemStruct *pItem = (WorkQueueItemStruct*)param;

		switch(pItem->opCode)
		{
		case WQ_CONNECT:
			//SERVER->AddClient(pItem->Id,pItem->Buffer);
			break;
		case WQ_DISCONNECT:
			//SERVER->RemoveClient(pItem->Id);
			break;
		case WQ_PACKET:
			{
				stPacketHead* pHead = (stPacketHead*)pItem->Buffer;
				char *pData	= (char *)(pHead) + IPacket::GetHeadSize();
				Base::BitStream RecvPacket(pData,pItem->size-IPacket::GetHeadSize());

				if(!IsValidMessage(pHead->Message))
					return false;

				if( getInstance()->mModuleMessageMap[pHead->Message] )
					return getInstance()->mModuleMessageMap[pHead->Message]->onMessage( getInstance()->mModuleMessageMap[pHead->Message], pHead->Message, pHead, &RecvPacket);
			}
			break;
		}

		return false;
	}

protected:
	typedef CommonPacket Parent;

	_inline void HandleClientLogin()
	{
		char IP[COMMON_STRING_LENGTH]="";

		char *pAddress = dynamic_cast<dtServerSocketClient *>(m_pSocket)->GetConnectedIP();
		if(pAddress)
			sStrcpy(IP,COMMON_STRING_LENGTH,pAddress,COMMON_STRING_LENGTH);
		
		T::getInstance()->getWorkQueue()->PostEvent(m_pSocket->GetClientId(),IP,sStrlen(IP,COMMON_STRING_LENGTH)+1,true,WQ_CONNECT);
		
	}

	_inline void HandleClientLogout()
	{
		T::getInstance()->getWorkQueue()->PostEvent(m_pSocket->GetClientId(),NULL,0,false,WQ_DISCONNECT);
	}

	_inline bool HandleGamePacket(stPacketHead *pHead,int iSize)
	{
		T::getInstance()->getWorkQueue()->PostEvent(m_pSocket->GetClientId(),pHead,iSize,true);
		return true;
	}

private:
	CWaitObject*			mWaitObject;
	dtServerSocket*			mServer;
	CWorkQueue*				mWorkQueue;

	IServerModuleBase*		mModuleMessageMap[END_NET_MESSAGE];

public:
	static T* getInstance()
	{
		static T instance;
		return &instance;
	}
};

#endif