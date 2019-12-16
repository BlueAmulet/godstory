//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/MountObjectData.h"


// ========================================================================================================================================
//	MountObject
// ========================================================================================================================================

class MountObject: public GameObject
{
	typedef GameObject Parent;

	//----------------------------------------------------------------------------
	// ö��&�ṹ
	//----------------------------------------------------------------------------
public:
	enum enMountStatus
	{
		MountStatus_Idle		= 0,
		MountStatus_Mount		= 1,
		MountStatus_Max,
	};
	//----------------------------------------------------------------------------
	// ��Ա����
	//----------------------------------------------------------------------------
protected:
	MountObjectData*		mDataBlock;

	//----------------------------------------------------------------------------
	// ��Ա����
	//----------------------------------------------------------------------------
protected:

public:
	MountObject();
	~MountObject();

	DECLARE_CONOBJECT(MountObject);

	bool					onAdd					();
	bool					onNewDataBlock			(GameBaseData* dptr);
	void					processTick				(const Move *move);
	U64						packUpdate				(NetConnection *conn, U64 mask, BitStream *stream);
	void					unpackUpdate			(NetConnection *conn,           BitStream *stream);

	static void				initPersistFields		();

	bool					collideBox				(const Point3F &start, const Point3F &end, RayInfo* info);

	virtual bool			CanAttackTarget			(GameObject* obj);
};
