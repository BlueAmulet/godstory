//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/SpellObject.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Effects/EffectPacket.h"


// ========================================================================================================================================
//  SpellObject
// ========================================================================================================================================
IMPLEMENT_CO_NETOBJECT_V1(SpellObject);

SpellObject::SpellObject()
{
#ifdef NTJ_SERVER
	GameBase::mDataBlock = &msDataBlock;
	mNetFlags.set(Ghostable);
#endif
}

SpellObject::SpellObject(SimTime time, S32 times, GameObject* src, bool bTgt, GameObject* tgt, Point3F& pos, SkillData* pData, Stats& stats, U32 ep, SkillData::Selectable sl):
	mScheduleSpell(time, times, src, bTgt, tgt, pos, pData, stats, ep, sl)
{
#ifdef NTJ_SERVER
	GameBase::mDataBlock = &msDataBlock;
	mNetFlags.set(Ghostable);
#endif
}

SpellObject::~SpellObject()
{
}

bool SpellObject::onAdd()
{
	if(!Parent::onAdd())
		return false;

	addToScene();

#ifdef NTJ_CLIENT
	// 客户端添加特效
	AssertFatal(mScheduleSpell.mSkillData, "SpellObject::onAdd()");
	mScheduleSpell.mEffectPacket = g_EffectPacketContainer.addEffectPacket(mScheduleSpell.mSkillData->m_Cast.environmentDelayEP, this, getRenderTransform(), NULL, getPosition());
#endif
	return true;
}

void SpellObject::onRemove()
{
	Parent::onRemove();
#ifdef NTJ_CLIENT
	if(mScheduleSpell.mEffectPacket)
		g_EffectPacketContainer.removeEffectPacket(mScheduleSpell.mEffectPacket, true, 1000);
#endif
	removeFromScene();
	//plUnlink();
	//Parent::Parent::onRemove();
}

void SpellObject::processTick(const Move *move)
{
	Parent::processTick(move);

#ifdef NTJ_SERVER
	if(!mScheduleSpell.Advance(TickMs))
		deleteObject();
#endif
}

U64 SpellObject::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 ret = Parent::packUpdate(conn, mask, stream);

	if(stream->writeFlag(mask & InitialUpdateMask))
	{
		stream->writeInt(mScheduleSpell.mSkillData->GetID(), Base::Bit32);
		stream->writeCompressedPoint(getPosition());
	}

	return ret;
}

void SpellObject::unpackUpdate(NetConnection *conn, BitStream *stream)
{
	Parent::unpackUpdate(conn, stream);

	if(stream->readFlag())
	{
		Point3F pos;
		U32 id = stream->readInt(Base::Bit32);
		mScheduleSpell.mSkillData = g_SkillRepository.GetSkill(id);
		stream->readCompressedPoint(&pos);
		setPosition(pos);
	}
}

