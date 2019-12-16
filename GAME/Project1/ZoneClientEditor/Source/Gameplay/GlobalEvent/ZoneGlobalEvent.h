#ifndef ZONE_GLOBAL_EVENT_H
#define ZONE_GLOBAL_EVENT_H

#include "Base/Locker.h"
#include "Base/bitStream.h"

#include "Common/CommonPacket.h"


namespace CZoneGlobalEvent
{
	extern void sendGlobalEvent( int id, int type );

};

#endif