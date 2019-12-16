//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/NpcObjectData.h"
#include "Gameplay/Item/DropItem.h"

#ifdef NTJ_SERVER
#include "Gameplay/ai/NpcAI.h"
#include "Gameplay/ai/PetAI.h"
#endif

#define NPC_EXP_SCLAE 100.0f

// ========================================================================================================================================
//	NpcObject
// ========================================================================================================================================
class NpcTeam;

class NpcObject: public GameObject
{
	typedef GameObject Parent;
	//----------------------------------------------------------------------------
	// ��Ա����
	//----------------------------------------------------------------------------
	NpcObjectData*			mDataBlock;
	SimObjectPtr<Player>	m_BelongTo;						// ����Ȩ�������������Ʒ��Ǯ����������ĸ����
	U32						mExclusivePlayerId;				// ��ռ����

protected:
	static const SimTime	sForceUpdateTicks;				// ǿ�Ƹ���ʱ����
	static const SimTime	sDefaultRiseInterval;
	static const SimTime	sDefaultCorpseTime;
	static const SimTime	sDefaultFadeTime;

	SimTime					mForceUpdate;					// ǿ�Ƹ��£���NPC������Ұ
	SimTime					mRiseTime;						// ����ʱ��
	bool					mCanTame;						// �ɲ����
	U32						mHateLinkId;					// �������

	U32						mMissionEffectId;				// ������Ч
#ifdef NTJ_CLIENT
	GFXTexHandle            mTextureTopIcon;                // NPCͷ������
#endif

public:
	//----------------------------------------------------------------------------
	// AI
	bool					m_bAI;							// �Ƿ�ʹ��AI
	Point3F					m_InitialPosition;				// ��ʼλ��
	Point3F					m_InitialRotation;				// ��ʼ����
	Point3F					m_MoveDestination;				// �յ�λ��

	U32						m_MovementAreaRadius_FreeMove;	// �����ƶ��ķ�Χ
	U32						m_MovementAreaRadius_Trace;		// ���׷�پ���
	U32						m_PlayerScopeStamp;				// ������һ��ʱ���
	//·��
	U32						m_WayPointID;                   // ·����,����gServerPathManager���е�·�� 
	U32						m_nSeqIndex;                    // Path�е� Sequence index;

#ifdef NTJ_SERVER
	NpcTeam*				mNpcTeam;						// NPC����
#endif

	//----------------------------------------------------------------------------
	// ��Ա����
	//----------------------------------------------------------------------------
protected:

public:
	NpcObject();
	~NpcObject();

	DECLARE_CONOBJECT(NpcObject);

	bool					onAdd					();
	void					onRemove				();
	bool					onNewDataBlock			(GameBaseData* dptr);
	void					processTick				(const Move *move);
	U64						packUpdate				(NetConnection *conn, U64 mask, BitStream *stream);
	void					unpackUpdate			(NetConnection *conn,           BitStream *stream);
	virtual bool			collideBox				(const Point3F &start, const Point3F &end, RayInfo* info);

	void					onEnabled				();
	void					onDisabled				();
	inline void				setBelongTo				(Player* player) { m_BelongTo = player;}
	inline Player*			getBelongTo				() { return m_BelongTo.getObject();}

	static void				initPersistFields		();
	virtual bool			CanAttackTarget			(GameObject* obj);
	virtual bool			canTame					() const { return mCanTame;}
	virtual bool			isDamageable			() const;

	NpcObjectData::enIntensity getIntensity			() const { return mDataBlock ? mDataBlock->intensity : NpcObjectData::Intensity_Normal;}
	F32						getExpScale				() const { return mDataBlock ? (mDataBlock->experience / NPC_EXP_SCLAE) : 1.0f;}
	U32						getHateLinkId			() const { return mHateLinkId;}
	U32						getTeamMatchId			() const { return mDataBlock ? mDataBlock->teamMatchId : 0;}

	virtual void			onCombatBegin			(GameObject* pObj);
	virtual void			onCombatEnd				(GameObject* pObj);

	void					setMissionEffect		(U32 effectId);
	void					clearMissionEffect		();
	inline void				setExclusivePlayerId	(U32 id) { mExclusivePlayerId = id; setMaskBits(GameObject::BaseInfoMask);}
	inline U32				getExclusivePlayerId	() { return mExclusivePlayerId;}
	bool					checkExclusive			(GameObject* pObj);

	StringTableEntry		getObjectIcon			() { return mDataBlock ? mDataBlock->icon : ""; }
#ifdef NTJ_SERVER
	void					LanchEvent				();
	void					LanchEvent				(U32 nEventID);

	void					DropPrize				();
	void					DropPrizeBox			();
	void					DropPrizeItems			(DropItemList *pList);

	void					SetRiseTime				();
	bool					CheckRiseTime			();

	void					onSpawn					();
	void					onReset					();
	inline bool				canWalk					() { return m_MovementAreaRadius_FreeMove > 0.01f;}
	void					addLevel				(S32 level);
	void					setLevel				(S32 level);
#endif

#ifdef NTJ_CLIENT
	RectF texRect;
	bool					updateData				(GuiControlProfile** pStyleList,RectF *rect);
	void					drawObjectInfo			(GameObject *,void *,void *);
	void                    drawObjectWord          (void *);

#endif

};
