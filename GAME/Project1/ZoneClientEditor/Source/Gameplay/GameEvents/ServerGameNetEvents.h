#ifndef __SERVERGAMENETEVENT_H__
#define __SERVERGAMENETEVENT_H__

#pragma  once
#include "GamePlay/GameEvents/GameNetEvents.h"

class ServerGameNetEvent : public GameplayEvent
{
	typedef GameplayEvent Parent;
protected:
	void process(NetConnection *pCon);
public:
	ServerGameNetEvent(U32 info_type=0);
	DECLARE_CONOBJECT(ServerGameNetEvent);
};

#endif//__SERVERGAMENETEVENT_H__