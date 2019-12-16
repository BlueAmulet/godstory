#ifndef EVENT_COMMON_H
#define EVENT_COMMON_H

#include "Base/bitStream.h"
#include "Common/PacketType.h"
#include "Common/CommonPacket.h"

template< typename _Ty >
struct EventFunction
{
	typedef void (_Ty::*Type)( stPacketHead *pHead, Base::BitStream &RecvPacket );
};

struct EventMethod
{
	EventMethod()
	{
		pMethod = NULL;
	}

	template< typename _Ty >
	void operator = ( _Ty eventFunction )
	{
		memcpy( &pMethod, &eventFunction, sizeof( _Ty ));
	}

	template< typename _Ty >
	void operator () ( _Ty* pThis, stPacketHead *pHead, Base::BitStream &RecvPacket )
	{
		static EventFunction<_Ty>::Type method;
		
		memcpy( &method, &pMethod, sizeof( EventFunction<_Ty>::Type ) );

		if( pMethod != NULL )
			( pThis->*((EventFunction<_Ty>::Type)method) )( pHead, RecvPacket );
	}

private:
	void* pMethod;
};

extern EventMethod gEventMethod[END_NET_MESSAGE];

#endif