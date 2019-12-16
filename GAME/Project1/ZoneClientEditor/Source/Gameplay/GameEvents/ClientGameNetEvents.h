#ifndef __CLIENTGAMENETEVENTS_H__
#define __CLIENTGAMENETEVENTS_H__

#pragma once
#include "GamePlay/GameEvents/GameNetEvents.h"

class ClientGameNetEvent : public GameplayEvent
{
	typedef GameplayEvent Parent;
protected:
	void process(NetConnection *pCon);

public:
	ClientGameNetEvent(U32 info_type=0);
	DECLARE_CONOBJECT(ClientGameNetEvent);
};

#endif//__CLIENTGAMENETEVENTS_H__