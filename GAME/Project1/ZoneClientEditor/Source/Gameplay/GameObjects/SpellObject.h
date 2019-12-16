//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "T3D/gameBase.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/Spell.h"

// ========================================================================================================================================
//  SpellObject
// ========================================================================================================================================

class SpellObject : public GameBase
{
	typedef GameBase Parent;

protected:
	ScheduleSpell mScheduleSpell;

public:
	SpellObject();
	SpellObject(SimTime time, S32 times, GameObject* src, bool bTgt, GameObject* tgt, Point3F& pos, SkillData* pData, Stats& stats, U32 ep, SkillData::Selectable sl);
	~SpellObject();

	DECLARE_CONOBJECT(SpellObject);

	bool onAdd();
	void onRemove();

	void processTick(const Move *move);

	U64  packUpdate  (NetConnection *conn, U64 mask, BitStream *stream);
	void unpackUpdate(NetConnection *conn,           BitStream *stream);
};

