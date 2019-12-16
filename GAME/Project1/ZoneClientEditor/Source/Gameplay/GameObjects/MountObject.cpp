//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/MountObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"

// ========================================================================================================================================
//	MountObject
// ========================================================================================================================================
IMPLEMENT_CO_NETOBJECT_V1(MountObject);

MountObject::MountObject()
{
	mGameObjectMask |= MountObjectType;
}

MountObject::~MountObject()
{
}

bool MountObject::onAdd()
{
	// 直接从MountRepository中读取dataBlock
	if (!GameBase::mDataBlock)
	{
		GameBase::mDataBlock = dynamic_cast<GameBaseData*>(g_MountRepository.GetMountData(mDataBlockId));
	}

	if(!Parent::onAdd())
		return false;

	return true;
}

bool MountObject::onNewDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<MountObjectData*>(dptr);
	if (!mDataBlock || !Parent::onNewDataBlock(mDataBlock))
		return false;

	// 初始化
	m_BuffTable.AddBuff(Buff::Origin_Base ,Macro_GetBuffId(Macro_GetBuffSeriesId(mDataBlock->buffId), m_Level), this);
	CalcStats();
	setHP(0xFFFFFF);
	setMP(0xFFFFFF);
	setPP(0xFFFFFF);

	return true;
}

void MountObject::processTick(const Move *move)
{
	Parent::processTick(move);

#ifdef NTJ_SERVER
	// 空的骑乘
	if(!getMountList() && isServerObject())
		safeDeleteObject();
#endif
}

U64 MountObject::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);

	return retMask;
}

void MountObject::unpackUpdate(NetConnection *conn, BitStream *stream)
{
	Parent::unpackUpdate(conn,stream);
}

void MountObject::initPersistFields()
{
	Parent::initPersistFields();
}

bool MountObject::collideBox(const Point3F &start, const Point3F &end, RayInfo* info)
{
	return false;
}

bool MountObject::CanAttackTarget(GameObject* obj)
{
	return false;
}
