//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/GameObjects/MountObject.h"
#include "Common/PlayerStruct.h"

#define PET_HAPPINESS_TIME		3600000			//1Сʱ,������ֶ�ʱ��ۼ�
#define MOUNT_REFRESH_TIME		1000			//1���ӣ����ʣ��ʱ��ˢ��

// ========================================================================================================================================
//	PetTable
// ========================================================================================================================================

class Player;
class BitStream;

class PetTable
{
	friend class Player;
	//----------------------------------------------------------------------------
	// ��Ա����
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
	Stats						**m_pStats;								// �ͻ��˴��������ʾ�����������
#endif
	stPetInfo					mPetInfo[PET_MAXSLOTS];					// ������Ϣ�б�
	stMountInfo					mMountInfo[MOUNTPET_MAXSLOTS];			// �����Ϣ�б�
	SimObjectPtr<PetObject>		mPetObject[PetObjectData::PetMax];		// �������

	//----------------------------------------------------------------------------
	// ��Ա����
	//----------------------------------------------------------------------------
protected:
	void					init					(Player* player);
	void					initPet					(Player* player);
	void					initMount				(Player* player);

	bool					canComboPet				(U32 slot);
public:
	PetTable();
	~PetTable();

	//������غ���
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

	//�����غ���
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


	//���ݼ��ؼ�����
	void					saveData				(stPlayerStruct* playerInfo);
	void					loadData				(stPlayerStruct* playerInfo);

	//�������Ե���غ���
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

//////////////////////////////// ���������غ��� ///////////////////////////////////////////////
#ifdef NTJ_CLIENT
	bool					isPetExist				(U32 slot);

	StringTableEntry		getPetName				(U32 slot);
	S32						getPetLevel				(U32 slot);
	S32						getPetSpawnLevel		(U32 slot);
	S32						getPetHuiGen			(U32 slot);
	S32						getPetGrowth			(U32 slot);
	S32						getPetGenGu				(U32 slot);

	//�����˺�
	S32						getPetWuAttack			(U32 slot);
	S32						getPetYuanAttack		(U32 slot);
	S32						getPetMuAttack			(U32 slot);
	S32						getPetHuoAttack			(U32 slot);
	S32						getPetTuAttack			(U32 slot);
	S32						getPetJinAttack			(U32 slot);
	S32						getPetShuiAttack		(U32 slot);

	//�������
	S32						getPetWuDefence			(U32 slot);
	S32						getPetYuanDefence		(U32 slot);
	S32						getPetMuDefence			(U32 slot);
	S32						getPetHuoDefence		(U32 slot);
	S32						getPetTuDefence			(U32 slot);
	S32						getPetJinDefence		(U32 slot);
	S32						getPetShuiDefence		(U32 slot);

	//��������
	S32						getPetStaminaGift		(U32 slot);
	S32						getPetManaGift			(U32 slot);
	S32						getPetStrengthGift		(U32 slot);
	S32						getPetIntellectGift		(U32 slot);
	S32						getPetAgilityGift		(U32 slot);
	S32						getPetPneumaGift		(U32 slot);

	//����
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

//////////////////////////////// ��������غ��� ///////////////////////////////////////////////
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


	void					ProcessTick				(Player *pPlayer);		//���ڳ��������ʱ
//////////////////////////////// ��������ڳ����ʱ����غ��������ݽṹ//////////////////////////////////
#ifdef NTJ_SERVER
public:
	void					setPetStartTime			(U32 slot, U32 nTime)	{ mPetStartTimes[slot] = nTime; }	
	U32						getPetStartTime			(U32 slot)				{ return mPetStartTimes[slot]; }
	
private:
	U32						mPetStartTimes[PET_MAXSLOTS];	
#endif
//////////////////////////////// ����˺Ϳͻ�����������ʱ����غ��������ݽṹ//////////////////////////////////
public:
	void					setMountStartTime		(U32 slot, U32 nTime)	{ mMountStartTimes[slot] = nTime; }
	U32						getMountStartTime		(U32 slot)				{ return mMountStartTimes[slot]; }
private:
	U32						mMountStartTimes[MOUNTPET_MAXSLOTS];
};
