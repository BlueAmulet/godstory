//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/NpcObject.h"


// ========================================================================================================================================
//  NpcHateLink
// ========================================================================================================================================

class NpcHateLink
{
public:
	typedef stdext::hash_multimap<U32, NpcObject*> HateLinkMap;
	typedef stdext::hash_map<U32, HateLinkMap*> HateLinkTable;

protected:
	HateLinkTable mTable;

public:
	NpcHateLink();
	~NpcHateLink();

	void					clear					();
	void					add						(NpcObject* pNpc);
	void					remove					(NpcObject* pNpc);

	//void					onCombatBegin			();
	void					onHateLink				(NpcObject* pNpc, GameObject* pObj);
};

extern NpcHateLink gNpcHateLink;
