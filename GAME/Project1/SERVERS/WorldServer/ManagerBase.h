#ifndef MANAGER_BASE_H
#define MANAGER_BASE_H

#include "Common/PacketType.h"
#include "wintcp/IPacket.h"

class stAccountInfo;

template< typename _Ty, int msgBegin, int msgEnd >
class CManagerBase
{

public:
   CManagerBase()
   {
	   memset( m_eventHandlers, 0, sizeof( m_eventHandlers ) );
   }

protected:
	typedef void (_Ty::*EventFunction)(int ,stPacketHead *,Base::BitStream *);

	void registerEvent( int event, EventFunction funct )
	{
		m_eventHandlers[event] = funct;
	}

public:
	void HandleEvent(stAccountInfo* pAccount,int opcode,int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{ 
		//if (0 == pAccount)
		//	return;

		if (opcode < msgBegin || opcode > msgEnd)
			return;

		if (0 != m_eventHandlers[opcode])
			(((_Ty*)this)->*m_eventHandlers[opcode])(SocketHandle,pHead,Packet);
	}

private:
	EventFunction m_eventHandlers[END_NET_MESSAGE];
};

#ifndef DECLARE_EVENT_FUNCTION
#define DECLARE_EVENT_FUNCTION(p) void p(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
#endif

#ifndef EVENT_FUNCTION
#define EVENT_FUNCTION(p) p(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
#endif


#endif