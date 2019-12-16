//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/NpcObject.h"

// ========================================================================================================================================
//	NpcTeam
// ========================================================================================================================================
class NpcTeamManager;
class NpcTeam
{
public:
	friend class NpcTeamManager;
	typedef std::list<NpcObject*> NpcTeamMember;

protected:
	static U32 msTeamId;
	U32 mTeamId;
	U32 mLayerId;
	NpcTeamMember mMember;

public:
	NpcTeam();
	~NpcTeam();

	void					clear					();
	U32						size					();
	bool					add						(NpcObject* pNpc);
	void					remove					(NpcObject* pNpc);

	void					onHateAdd				(GameObject* pObj);
};


// ========================================================================================================================================
//	NpcTeamManager
// ========================================================================================================================================

class NpcTeamManager
{
public:
	typedef stdext::hash_map<U32, NpcTeam*> NpcTeamMap;

protected:
	NpcTeamMap mNpcTeamMap;

public:
	NpcTeamManager();
	~NpcTeamManager();

	void					clear					();
	bool					join					(NpcObject* pNpcA, NpcObject* pNpcB);
	void					leave					(NpcObject* pNpc);
	void					destroy					(U32 id);
	void					merge					(U32 idA, U32 idB);

	void					onHateAdd				(GameObject* pObj);
};

extern NpcTeamManager gNpcTeamManager;
