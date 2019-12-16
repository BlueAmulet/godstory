//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/GameObjects/MountObject.h"
#include "Common/PlayerStruct.h"

#define PET_HAPPINESS_TIME		3600000			//1小时,宠物快乐度时间扣减
#define MOUNT_REFRESH_TIME		1000			//1秒钟，骑宠剩余时间刷新

// ========================================================================================================================================
//	PetTable
// ========================================================================================================================================

class Player;
class BitStream;

class PetTable
{
	friend class Player;
	//----------------------------------------------------------------------------
	// 成员变量
	//----------------------------------------------------------------------------
public:
	static F32					getQualityFactor(U16 quality);
	static F32					getChengZhangLvFactor(U32 chengZhangLv);

	Stats						CalAddedStats(U32 nSlot, CalcFactor &factor);

#ifdef NTJ_CLIENT
	void						addBuffDatas(Stats &petStats, stPetInfo *pPetInfo);
	void						initTempStats();
	void						clearTempStats();
	void						cleanTempStats(U32 nSlot);
	void						calTempStats(U32 nSlot);
	void						swapTempStats(U32 nSlot1, U32 nSlot2);
#endif

protected:
#ifdef NTJ_CLIENT
	Stats						**m_pStats;								// 客户端存放用于显示宠物各种属性
#endif
	stPetInfo					mPetInfo[PET_MAXSLOTS];					// 宠物信息列表
	stMountInfo					mMountInfo[MOUNTPET_MAXSLOTS];			// 骑宠信息列表
	SimObjectPtr<PetObject>		mPetObject[PetObjectData::PetMax];		// 宠物对象

	//----------------------------------------------------------------------------
	// 成员函数
	//----------------------------------------------------------------------------
protected:
	void					init					(Player* player);
	void					initPet					(Player* player);
	void					initMount				(Player* player);

	bool					canComboPet				(U32 slot);
public:
	PetTable();
	~PetTable();

	//宠物相关函数
	bool					randHuanTongProps		(U32 slot, S32 nHuanTongDanLevel);
	bool					addExp					(U32 slot, S32 nExp, Player *pPlayer);
	bool					addLevel				(U32 slot, S32 nLevel, Player *pPlayer, S32 nExp = 0);

	stPetInfo*				getEmptyPetSlot			(U32& slot);
	PetObject*				getPet					(U32 petIdx);
	void					setPet					(PetObject* pet);
	bool					unlinkPet				(U32 petIdx);
	void					savePetInfo				(U32 petIdx);
	void					loadPetInfo				(U32 petIdx);
	void					writePetInfo			(Player*, BitStream*, S32 slot = -1, U32 flag = 0xFFFFFFFF);
	void					readPetInfo				(Player*, BitStream*);
	void					setPetStatus			(U32 slot, U32 status);
	PetObject*				getCombatPetBySlot		(U32 slot);	
	void					packPetObject			(NetConnection* conn, BitStream* stream);
	void					unpackPetObject			(NetConnection* conn, BitStream* stream);
	const stPetInfo*		getPetInfo				(U32 slot) const;
	bool 					getPetStatus			(U32 slot, PetObject::enPetStatus &status) const;
	void					swapPetInfo				(U32 slot1, U32 slot2);
	bool					RefreshPetInfos			(U32 nBeginSlot, U32 &nEndSlot);
	U32						getPetInfoCount			() const;
	S32						getSpawnPetSlot			();
	U32						getSpawnPetID			();
	bool					fillPetInfo				(Player* player, ItemShortcut* pItem);
	void					sendInitialData			(Player* player);
	void					disbandAllPet			(Player* player);

	//骑宠相关函数
	bool					getMountStatus			(U32 slot, MountObject::enMountStatus &status);
	stMountInfo*			getEmptyMountSlot		(U32& slot);
	void					saveMountInfo			();
	void					loadMountInfo			();
	void					writeMountInfo			(Player*, BitStream*, S32 slot = -1);
	void					readMountInfo			(Player*, BitStream*);
	void					setMountStatus			(U32 slot, U32 status);
	const stMountInfo*		getMountInfo			(U32 slot) const;
	void					swapMountInfo			(U32 slot1, U32 slot2);
	S32						getMountedSlot			() const;
	U32						getMountInfoCount		() const;


	//数据加载及保存
	void					saveData				(stPlayerStruct* playerInfo);
	void					loadData				(stPlayerStruct* playerInfo);

	//宠物属性点相关函数
	S32						getPetTiPo				(U32 slot);
	S32						getPetJingLi			(U32 slot);
	S32						getPetLiDao				(U32 slot);
	S32						getPetLingLi			(U32 slot);
	S32						getPetMingJie			(U32 slot);
	S32						getPetYuanLi			(U32 slot);
	S32						getPetQianLi			(U32 slot);
	S32						getPetChengZhangLv		(U32 slot);

	bool					isPetSpawned			(U32 slot);
	PetObject*				getSpawnedPet			(U32 slot);

//////////////////////////////// 宠物界面相关函数 ///////////////////////////////////////////////
#ifdef NTJ_CLIENT
	bool					isPetExist				(U32 slot);

	StringTableEntry		getPetName				(U32 slot);
	S32						getPetLevel				(U32 slot);
	S32						getPetSpawnLevel		(U32 slot);
	S32						getPetHuiGen			(U32 slot);
	S32						getPetGrowth			(U32 slot);
	S32						getPetGenGu				(U32 slot);

	//宠物伤害
	S32						getPetWuAttack			(U32 slot);
	S32						getPetYuanAttack		(U32 slot);
	S32						getPetMuAttack			(U32 slot);
	S32						getPetHuoAttack			(U32 slot);
	S32						getPetTuAttack			(U32 slot);
	S32						getPetJinAttack			(U32 slot);
	S32						getPetShuiAttack		(U32 slot);

	//宠物防御
	S32						getPetWuDefence			(U32 slot);
	S32						getPetYuanDefence		(U32 slot);
	S32						getPetMuDefence			(U32 slot);
	S32						getPetHuoDefence		(U32 slot);
	S32						getPetTuDefence			(U32 slot);
	S32						getPetJinDefence		(U32 slot);
	S32						getPetShuiDefence		(U32 slot);

	//宠物资质
	S32						getPetStaminaGift		(U32 slot);
	S32						getPetManaGift			(U32 slot);
	S32						getPetStrengthGift		(U32 slot);
	S32						getPetIntellectGift		(U32 slot);
	S32						getPetAgilityGift		(U32 slot);
	S32						getPetPneumaGift		(U32 slot);

	//其他
	S32						getPetZhiLiao			(U32 slot);
	S32						getPetShanBi			(U32 slot);
	S32						getPetBaoJi				(U32 slot);
	S32						getPetBaoJiLv			(U32 slot);

	S32						getPetHappiness			(U32 slot);
	S32						getPetGeneration		(U32 slot);
	S32						getPetRace				(U32 slot);
	S32						getPetSex				(U32 slot);
	S32						getPetStyle				(U32 slot);

	S32						GetPetMp				(U32 slot);
	S32						GetPetMaxMp				(U32 slot);
	S32						GetPetHp				(U32 slot);
	S32						GetPetMaxHp				(U32 slot);
	S32						GetPetExp				(U32 slot);
	S32						GetPetMaxExp			(U32 slot);

//////////////////////////////// 骑宠界面相关函数 ///////////////////////////////////////////////
	bool					isMountExist			(U32 slot);
	StringTableEntry		getMountName			(U32 slot);
	StringTableEntry		getMountInfoDesc		(U32 slot);
	S32						getMountLevel			(U32 slot);
	S32						getMountPlayerLevel		(U32 slot);
	S32						getMountSpeedupPercent	(U32 slot);
	S32						getMountLeftTime		(U32 slot);
	U8						getMountType			(U32 slot);
	S32						getMountPlayerCount		(U32 slot);
	S32						getMountRace			(U32 slot);
#endif


	void					ProcessTick				(Player *pPlayer);		//用于宠物和骑宠计时
//////////////////////////////// 服务端用于宠物计时的相关函数及数据结构//////////////////////////////////
#ifdef NTJ_SERVER
public:
	void					setPetStartTime			(U32 slot, U32 nTime)	{ mPetStartTimes[slot] = nTime; }	
	U32						getPetStartTime			(U32 slot)				{ return mPetStartTimes[slot]; }
	
private:
	U32						mPetStartTimes[PET_MAXSLOTS];	
#endif
//////////////////////////////// 服务端和客户端用于骑宠计时的相关函数及数据结构//////////////////////////////////
public:
	void					setMountStartTime		(U32 slot, U32 nTime)	{ mMountStartTimes[slot] = nTime; }
	U32						getMountStartTime		(U32 slot)				{ return mMountStartTimes[slot]; }
private:
	U32						mMountStartTimes[MOUNTPET_MAXSLOTS];
};
