//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/PetObjectData.h"

class GuiControlProfile;
// ========================================================================================================================================
//	PetObject
// ========================================================================================================================================
class PetObject: public GameObject
{
	typedef GameObject Parent;

	friend class Player;
	//----------------------------------------------------------------------------
	// 枚举&结构
	//----------------------------------------------------------------------------
public:
	enum enPetStatus
	{
		PetStatus_Idle		= 0,
		PetStatus_Combat,
		PetStatus_Combo,
		PetStatus_Mount,
		PetStatus_Learn,
		PetStatus_Dispatch,
		PetStatus_Freeze,				// 冻结状态（不允许任何操作）

		PetStatus_Max,
	};

	//----------------------------------------------------------------------------
	// 成员变量
	//----------------------------------------------------------------------------
public:
	static const CalcFactor smCalcFactor;
	static const SimTime	sDefaultCorpseTime;
	static const SimTime	sDefaultFadeTime;
protected:
	const static U64 MasterMask		= Parent::NextFreeMask << 0;
	const static U64 PetNameMask	= Parent::NextFreeMask << 1;
	const static U64 NextFreeMask	= Parent::NextFreeMask << 2;

	PetObjectData*			mDataBlock;

	U32						mPetSlot;				// 宠物槽位
	//----------------------------------------------------------------------------
	// 成员函数
	//----------------------------------------------------------------------------
protected:

public:
	PetObject();
	~PetObject();

	DECLARE_CONOBJECT(PetObject);

	bool					onAdd					();
	bool					onNewDataBlock			(GameBaseData* dptr);
	void					processTick				(const Move *move);
	U64						packUpdate				(NetConnection *conn, U64 mask, BitStream *stream);
	void					unpackUpdate			(NetConnection *conn,           BitStream *stream);

	virtual void			OnStatsChanged();

	void					onEnabled				();
	void					onDisabled				();

	static void				initPersistFields		();
	virtual void			CalcStats				();
	virtual bool			CanAttackTarget			(GameObject* obj);
	void					setMaster				(Player* player);
	inline Player*			getMaster				() { return mMaster;}
	void					changeName				() { setMaskBits(PetNameMask); }

	inline S32				getPetIndex				() { return mDataBlock ? mDataBlock->combatPetIdx : -1;}
	inline S32				getPetSlot				() { return mPetSlot;}
	inline void				setPetSlot				(U32 slot) { mPetSlot = slot;}
	void					addExp					(S32 exp);
	void					setLevel				(S32 level);

	SimObjectPtr<Player>	mMaster;				// 宠物的主人

#ifdef NTJ_CLIENT
	bool					mIsFirstUnpack;
	char					mPetName[COMMON_STRING_LENGTH];
	S32                     mMasterId;
	RectF                   texRect;
	bool					updateData				(GuiControlProfile** pStyleList,RectF *rect);
	void                    drawObjectInfo(GameObject *,void *,void *);
	void                    drawObjectWord(void *);
#endif
};
