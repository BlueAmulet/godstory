//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/PetMountOperation.h"

#ifdef NTJ_CLIENT
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#endif

#ifdef NTJ_SERVER
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#endif
// ========================================================================================================================================
//	PetTable
// ========================================================================================================================================
const CalcFactor smPetCalcFactor = {13.0f, 22.8f, 1.50f, 1.26f, 2.83f};

F32 PetTable::getQualityFactor(U16 quality)
{
	switch(quality)
	{
	case 0:		return 0.9;	
	case 1:		return 0.95;
	case 2:		return 1.0;	
	case 3:		return 1.05;
	case 4:		return 1.1;	
	case 5:		return 1.15;
	default:	return 0.0;
	}
}

F32 PetTable::getChengZhangLvFactor(U32 chengZhangLv)
{
	F32 fChengZhangLv = ((F32)chengZhangLv) / 20;
	
	if (fChengZhangLv >= 0.9 && fChengZhangLv < 42)
		return 1;
	if (fChengZhangLv >= 42 && fChengZhangLv < 78)
		return 1.32;
	if (fChengZhangLv >= 78 && fChengZhangLv < 111)
		return 1.74;
	if (fChengZhangLv >= 111 && fChengZhangLv < 143)
		return 2.3;
	if (fChengZhangLv >= 143 && fChengZhangLv < 165)
		return 3.03;
	if (fChengZhangLv >= 165)
		return 4;
		
	return 0;			//不应该执行这里
}

Stats PetTable::CalAddedStats(U32 nSlot, CalcFactor &factor)
{
	Stats tmpStats;
	tmpStats.Clear();

	const stPetInfo* info = getPetInfo(nSlot);
	if (!info || info->petDataId == 0)
		return tmpStats;

	// 品质和成长率运算因子
	F32 growthFactor = 0.0f;
	F32 qualityFactor = getQualityFactor(info->quality);

	if (info->chengZhangLv == 0)
		growthFactor = getChengZhangLvFactor(qualityFactor * info->qianLi * 20);
	else
		growthFactor = getChengZhangLvFactor(info->chengZhangLv);
	
	// 最终资质
	S32 staminaGift = (info->staminaGift + info->insight * 100) * qualityFactor;
	S32 ManaGift = (info->ManaGift + info->insight * 100) * qualityFactor;
	S32 strengthGift = (info->strengthGift + info->insight * 100) * qualityFactor;
	S32 intellectGift = (info->intellectGift + info->insight * 100) * qualityFactor;
	S32 pneumaGift = (info->pneumaGift + info->insight * 100) * qualityFactor;
	S32 agilityGift = (info->agilityGift + info->insight * 100) * qualityFactor;

	//物理攻击运算因子
	factor.Strength_PhyDamage		= PetObject::smCalcFactor.Strength_PhyDamage * strengthGift / 1000 * growthFactor;
	//五行攻击运算因子
	factor.Intellect_SplDamage		= PetObject::smCalcFactor.Intellect_SplDamage * intellectGift / 1000 * growthFactor * 0.79 ;
	//生命上限运算因子
	factor.Stamina_HP				= PetObject::smCalcFactor.Stamina_HP * staminaGift / 1000 * growthFactor * 0.48;
	//精气上限运算因子
	factor.Mana_MP					= PetObject::smCalcFactor.Mana_MP * ManaGift / 1000 * growthFactor * 0.38;
	//元力攻击运算因子
	factor.Pneuma_PnmDamage			= PetObject::smCalcFactor.Pneuma_PnmDamage * pneumaGift / 1000 * growthFactor * 0.67;
	//闪避值运算因子
	factor.Agility_DodgeRating		= PetObject::smCalcFactor.Agility_DodgeRating * agilityGift / 1000 * growthFactor * 0.5;
	//暴击值运算因子
	factor.Agility_CriticalRating	= PetObject::smCalcFactor.Agility_CriticalRating * agilityGift / 1000 * growthFactor * 0.3;

	tmpStats.Stamina		= info->staminaPts;		//体质属性加点
	tmpStats.Mana			= info->ManaPts;		//法力属性加点
	tmpStats.Strength		= info->strengthPts;	//力量属性加点
	tmpStats.Intellect		= info->intellectPts;	//智力属性加点
	tmpStats.Pneuma			= info->pneumaPts;		//元力属性加点
	tmpStats.Agility		= info->agilityPts;		//身法属性加点

	return tmpStats;
}

PetTable::PetTable()
{
#ifdef NTJ_CLIENT
	m_pStats = NULL;
#endif

#ifdef NTJ_SERVER
	dMemset(mPetStartTimes, 0, sizeof(mPetStartTimes));
#endif
	dMemset(mMountStartTimes, 0, sizeof(mMountStartTimes));

#ifdef NTJ_CLIENT
	U32 nCurrTime = Platform::getRealMilliseconds();
	for (S32 i = 0; i < MOUNTPET_MAXSLOTS; i++)
	{
		mMountStartTimes[i] = nCurrTime;	
	}
#endif
}

PetTable::~PetTable()
{
#ifdef NTJ_CLIENT
	clearTempStats();
#endif
}

#ifdef NTJ_CLIENT
void PetTable::addBuffDatas(Stats &petStats, stPetInfo *pPetInfo)
{
	if(!pPetInfo)
		return;
	for (int i = 0; i < 2; i++)
	{
		BuffData *pBuffData = NULL;
		PetObjectData *pPetData = NULL;
		U32 buffId = 0;
		if (i == 0)	
			buffId = pPetInfo->randBuffId;
		else
		{
			pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
			buffId = Macro_GetBuffId(Macro_GetBuffSeriesId(pPetData->buffId), pPetInfo->level);			
		}

		if (! (pBuffData = g_BuffRepository.GetBuff(buffId)) )
			return;

		Stats buffStats = pBuffData->GetStats();
		petStats.PhyDamage += buffStats.PhyDamage;
		petStats.MuDamage += buffStats.MuDamage;
		petStats.HuoDamage += buffStats.HuoDamage;
		petStats.JinDamage += buffStats.JinDamage;
		petStats.ShuiDamage += buffStats.ShuiDamage;
		petStats.TuDamage += buffStats.TuDamage;
		petStats.PneumaDamage += buffStats.PneumaDamage;

		petStats.PhyDefence += buffStats.PhyDefence;
		petStats.MuDefence += buffStats.MuDefence;
		petStats.HuoDefence += buffStats.HuoDefence;
		petStats.JinDefence += buffStats.JinDefence;
		petStats.ShuiDefence += buffStats.ShuiDefence;
		petStats.TuDefence += buffStats.TuDefence;
		petStats.PneumaDefence += buffStats.PneumaDefence;

		petStats.HP += buffStats.HP;
		petStats.MP += buffStats.MP;
		petStats.PP += buffStats.PP;
		petStats.MaxHP += buffStats.MaxHP;
		petStats.MaxMP += buffStats.MaxMP;
		petStats.MaxPP += buffStats.MaxPP;

		petStats.Heal += buffStats.Heal;
		petStats.DodgeRating += buffStats.DodgeRating;
		petStats.CriticalRating += buffStats.CriticalRating;
		petStats.CriticalTimes += buffStats.CriticalTimes;
	}
	
}

void PetTable::initTempStats()
{
	m_pStats = new Stats * [PET_MAXSLOTS];
	for (int i = 0; i < PET_MAXSLOTS; i++)
	{
		m_pStats[i] = new Stats();
		if (mPetInfo[i].petDataId != 0)
		{
			calTempStats(i);
		}
	}
}

void PetTable::clearTempStats()
{
	if (!m_pStats)
		return;
	for (int i = 0; i < PET_MAXSLOTS; i++)
	{
		if (m_pStats[i])
		{
			delete m_pStats[i];
			m_pStats[i] = NULL;
		}
	}
	delete [] m_pStats;
}

void PetTable::cleanTempStats(U32 nSlot)
{
	if(!m_pStats || !m_pStats[nSlot])
		return;
	dMemset( (void *)m_pStats[nSlot], 0, sizeof(Stats) );
}

void PetTable::calTempStats(U32 nSlot)
{
	if (!m_pStats || !m_pStats[nSlot] || nSlot >= PET_MAXSLOTS)
		return;
	stPetInfo *pPetInfo = (stPetInfo *)getPetInfo(nSlot);
	if(!pPetInfo)
		return;
	g_Stats.Clear();
	addBuffDatas(g_Stats, pPetInfo);
	CalcFactor factor;
	g_Stats += CalAddedStats(nSlot, factor);
	m_pStats[nSlot]->Calc(g_Stats, factor);
	m_pStats[nSlot]->HP = mClamp(pPetInfo->curHP, 0, m_pStats[nSlot]->MaxHP);
	m_pStats[nSlot]->MP = mClamp(pPetInfo->curMP, 0, m_pStats[nSlot]->MaxMP);
	m_pStats[nSlot]->PP = mClamp(pPetInfo->curPP, 0, m_pStats[nSlot]->MaxPP);
}
#endif

void PetTable::init(Player* player)
{
	initPet(player);
	initMount(player);
}

void PetTable::initPet(Player* player)
{
#ifdef NTJ_SERVER
	for(S32 i=0; i<PET_MAXSLOTS; ++i)
	{
		switch(mPetInfo[i].status)
		{
		case PetObject::PetStatus_Combat:
			{
				PetOperationManager::InitActivePetObject(player, i);
			}
			break;
		case PetObject::PetStatus_Combo:
			{
			}
			break;
		}
	}
#endif
}

void PetTable::initMount(Player* player)
{
#ifdef NTJ_SERVER
	for(S32 i=0; i<MOUNTPET_MAXSLOTS; ++i)
	{
		switch(mMountInfo[i].mStatus)
		{
		case MountObject::MountStatus_Mount:
			{
				PetMountOperationManager::InitActiveMountObject(player, i);
				return;			//任何时刻只能有一个骑宠对象存在
			}
			break;
		default:
			break;
		}
	}
#endif
}

const stPetInfo* PetTable::getPetInfo(U32 slot) const
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return NULL;
	
	return &mPetInfo[slot];
}

PetObject* PetTable::getPet(U32 petIdx)
{
	if (petIdx >= PetObjectData::PetMax)
		return NULL;
	return mPetObject[petIdx];
}

void PetTable::setPet(PetObject* pet)
{
	if (pet->getPetIndex() < PetObjectData::PetMax && !mPetObject[pet->getPetIndex()])
		mPetObject[pet->getPetIndex()] = pet;
}

bool PetTable::unlinkPet(U32 petIdx)
{
	if (petIdx >= PetObjectData::PetMax)
		return false;
	
	if(mPetObject[petIdx])
	{
		mPetObject[petIdx] = NULL;
		return true;
	}
	
	return false;
}

void PetTable::savePetInfo(U32 petIdx)
{
	if (petIdx >= PetObjectData::PetMax)
		return;

	if(mPetObject[petIdx])
	{
		AssertFatal(mPetObject[petIdx]->getPetSlot() < PET_MAXSLOTS, "PetTable::savePet!");
		stPetInfo& info = mPetInfo[mPetObject[petIdx]->getPetSlot()];

		info.curHP = mPetObject[petIdx]->GetStats().HP;
		info.curMP = mPetObject[petIdx]->GetStats().MP;
		info.curPP = mPetObject[petIdx]->GetStats().PP;
	}
}

void PetTable::loadPetInfo(U32 petIdx)
{
	if (petIdx >= PetObjectData::PetMax)
		return;

	if(mPetObject[petIdx])
	{
		AssertFatal(mPetObject[petIdx]->getPetSlot() < PET_MAXSLOTS, "PetTable::savePet!");
		stPetInfo& info = mPetInfo[mPetObject[petIdx]->getPetSlot()];

		mPetObject[petIdx]->setHP(info.curHP);
		mPetObject[petIdx]->setMP(info.curMP);
		mPetObject[petIdx]->setPP(info.curPP);
	}
}

// ----------------------------------------------------------------------------
// 网络传输数据打包
void PetTable::writePetInfo(Player* player, BitStream* stream, S32 slot/* = -1*/, U32 flag/* = 0xFFFFFFFF*/)
{
#ifdef NTJ_SERVER
	if(stream->writeFlag(slot >=0 && slot < PET_MAXSLOTS))
	{
		stream->writeInt(slot, 4);
		mPetInfo[slot].WriteInfo(stream, flag);
	}
#endif//NTJ_SERVER
}

// ----------------------------------------------------------------------------
// 网络传输数据解包
void PetTable::readPetInfo(Player* player, BitStream* stream)
{
#ifdef NTJ_CLIENT
	if(stream->readFlag())
	{
		S32 slot = stream->readInt(4);		
		U32 flag = mPetInfo[slot].ReadInfo(stream);

		if ( flag != -1 && (flag & stSpiritInfo::Level_Mask) )
		{
			Con::executef("ShowPetUpgradeHint");
		}

		calTempStats(slot);
		Con::executef("UpdatePetInfo", Con::getIntArg(slot));
	}
#endif//NTJ_CLIENT
}

PetObject* PetTable::getCombatPetBySlot(U32 slot)
{
	if (slot >= PET_MAXSLOTS)
		return NULL;

	for (S32 i=0; i<PetObjectData::CombatPetMax; ++i)
	{
		if(mPetObject[i] && mPetObject[i]->getDataID() == mPetInfo[slot].petDataId)
		{
			// 在战斗宠中，同种宠物只会存在一个
			return mPetObject[i];
		}
	}
	return NULL;
}

bool PetTable::randHuanTongProps(U32 slot, S32 nHuanTongDanLevel)
{
	stPetInfo *pPetInfo = (stPetInfo *)getPetInfo(slot);
	if (!pPetInfo)
		return false;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return false;

	switch(nHuanTongDanLevel)
	{
	case 1:		//初级还童
		{
			//重新随机六项属性及潜在实力
			pPetInfo->staminaGift		= (pPetData->minTiPo < pPetData->maxTiPo) ? gRandGen.randI(pPetData->minTiPo, pPetData->maxTiPo) : pPetData->minTiPo;
			pPetInfo->ManaGift			= (pPetData->minJingLi < pPetData->maxJingLi) ? gRandGen.randI(pPetData->minJingLi, pPetData->maxJingLi) : pPetData->minJingLi;
			pPetInfo->strengthGift		= (pPetData->minStrength < pPetData->maxStrength) ? gRandGen.randI(pPetData->minStrength, pPetData->maxStrength) : pPetData->minStrength;
			pPetInfo->intellectGift		= (pPetData->minLingLi < pPetData->maxLingLi) ? gRandGen.randI(pPetData->minLingLi, pPetData->maxLingLi) : pPetData->minLingLi;
			pPetInfo->agilityGift		= (pPetData->minMinJie < pPetData->maxMinJie) ? gRandGen.randI(pPetData->minMinJie, pPetData->maxMinJie) : pPetData->minMinJie;
			pPetInfo->pneumaGift		= (pPetData->minYuanLi < pPetData->maxYuanLi) ? gRandGen.randI(pPetData->minYuanLi, pPetData->maxYuanLi) : pPetData->minYuanLi;

			pPetInfo->qianLi			= gRandGen.randI(pPetData->minQianLi, pPetData->maxQianLi);
		}
		break;
	case 2:		//中级还童
		{
			//先随机选出六项属性的三项属性，然后把随机出的这三项属性下限加100，最后再随机
			S32 count = 3;
			S32 addMin = 100;
			S32 nPos = gRandGen.randI(0, 5);
			for (S32 i = 0; i < count; i++)
			{
				switch(nPos)
				{
				case 0:
					pPetInfo->staminaGift		= ((pPetData->minTiPo + addMin) < pPetData->maxTiPo) ? 
													gRandGen.randI(pPetData->minTiPo + addMin, pPetData->maxTiPo) : (pPetData->minTiPo + addMin);
					break;
				case 1:
					pPetInfo->ManaGift			= ((pPetData->minJingLi + addMin) < pPetData->maxJingLi) ? 
													gRandGen.randI(pPetData->minJingLi + addMin, pPetData->maxJingLi) : (pPetData->minJingLi + addMin);
					break;
				case 2:
					pPetInfo->strengthGift		= ((pPetData->minStrength + addMin) < pPetData->maxStrength) ? 
													gRandGen.randI(pPetData->minStrength + addMin, pPetData->maxStrength) : (pPetData->minStrength + addMin);
					break;
				case 3:
					pPetInfo->intellectGift		= ((pPetData->minLingLi + addMin) < pPetData->maxLingLi) ? 
													gRandGen.randI(pPetData->minLingLi + addMin, pPetData->maxLingLi) : (pPetData->minLingLi + addMin);
					break;
				case 4:
					pPetInfo->agilityGift		= ((pPetData->minMinJie + addMin) < pPetData->maxMinJie) ? 
													gRandGen.randI(pPetData->minMinJie + addMin, pPetData->maxMinJie) : (pPetData->minMinJie + addMin);
					break;
				case 5:
					pPetInfo->pneumaGift		= ((pPetData->minYuanLi + addMin) < pPetData->maxYuanLi) ? 
													gRandGen.randI(pPetData->minYuanLi + addMin, pPetData->maxYuanLi) : (pPetData->minYuanLi + addMin);
					break;
				default:
					break;
				}
				nPos = (nPos + 1) % 6;
			}
			pPetInfo->qianLi	= gRandGen.randI(pPetData->minQianLi, pPetData->maxQianLi);
		}
		break;
	case 3:		//高级还童
		{
			//(1)先随机选出六项属性的四项属性，然后把随机出的这四项属性下限加150，最后再随机
			//(2)潜在实力的下限加10
			S32 count = 4;
			S32 addMin = 150;
			S32 nPos = gRandGen.randI(0, 5);
			for (S32 i = 0; i < count; i++)
			{
				switch(nPos)
				{
				case 0:
					pPetInfo->staminaGift		= ((pPetData->minTiPo + addMin) < pPetData->maxTiPo) ? 
													gRandGen.randI(pPetData->minTiPo + addMin, pPetData->maxTiPo) : (pPetData->minTiPo + addMin);
					break;
				case 1:
					pPetInfo->ManaGift			= ((pPetData->minJingLi + addMin) < pPetData->maxJingLi) ? 
													gRandGen.randI(pPetData->minJingLi + addMin, pPetData->maxJingLi) : (pPetData->minJingLi + addMin);
					break;
				case 2:
					pPetInfo->strengthGift		= ((pPetData->minStrength + addMin) < pPetData->maxStrength) ? 
													gRandGen.randI(pPetData->minStrength + addMin, pPetData->maxStrength) : (pPetData->minStrength + addMin);
					break;
				case 3:
					pPetInfo->intellectGift		= ((pPetData->minLingLi + addMin) < pPetData->maxLingLi) ? 
													gRandGen.randI(pPetData->minLingLi + addMin, pPetData->maxLingLi) : (pPetData->minLingLi + addMin);
					break;
				case 4:
					pPetInfo->agilityGift		= ((pPetData->minMinJie + addMin) < pPetData->maxMinJie) ? 
													gRandGen.randI(pPetData->minMinJie + addMin, pPetData->maxMinJie) : (pPetData->minMinJie + addMin);
					break;
				case 5:
					pPetInfo->pneumaGift		= ((pPetData->minYuanLi + addMin) < pPetData->maxYuanLi) ? 
													gRandGen.randI(pPetData->minYuanLi + addMin, pPetData->maxYuanLi) : (pPetData->minYuanLi + addMin);
					break;
				}
				nPos = (nPos + 1) % 6;
			}
			pPetInfo->qianLi	= ((pPetData->minQianLi + 10) < pPetData->maxQianLi) ? 
									gRandGen.randI(pPetData->minQianLi + 10, pPetData->maxQianLi) : (pPetData->minQianLi + 10);
		}
		break;
	default:
		return false;
	}

	pPetInfo->level				= 1;
	pPetInfo->statsPoints		= 0;
	pPetInfo->staminaPts		= 0;
	pPetInfo->ManaPts			= 0;
	pPetInfo->strengthPts		= 0;
	pPetInfo->intellectPts		= 0;
	pPetInfo->agilityPts		= 0;
	pPetInfo->pneumaPts			= 0;

	return true;
}

bool PetTable::addExp(U32 slot, S32 nExp, Player *pPlayer)
{
	stPetInfo *pPetInfo = (stPetInfo*)getPetInfo(slot);
	if (!pPetInfo || pPetInfo->level == MAX_PET_LEVEL)
		return false;
	pPetInfo->exp += nExp;

#ifdef NTJ_SERVER
	PetOperationManager::ServerSendPetAddExpMsg(pPlayer, slot, nExp);  //先发送增加宠物经验的消息
#endif

	//检查是否需要升级
	g_PetLevelExp[pPetInfo->level];
	S32 i = pPetInfo->level;
	S32 nAddLevel = 0;
	while(i < MAX_PET_LEVEL)
	{
		if (g_PetLevelExp[i] <= pPetInfo->exp)
		{
			pPetInfo->exp -= g_PetLevelExp[i];
			nAddLevel++;
			i++;
		}
		else
		{
			break;
		}
	}

	if (nAddLevel > 0)
	{
		return addLevel(slot, nAddLevel, pPlayer, pPetInfo->exp);
	}
	else if (nExp > 0)
	{
#ifdef NTJ_SERVER
		PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, slot, stPetInfo::PetExp_Mask);
#endif
		return true;
	}

	return false;
}

bool PetTable::addLevel(U32 slot, S32 nLevel, Player *pPlayer, S32 nExp /* = 0*/)
{
	stPetInfo *pPetInfo = (stPetInfo*)getPetInfo(slot);
	if (!pPetInfo || pPetInfo->level == MAX_PET_LEVEL)
		return false;

	S32 nAddLevel = nLevel;
	if (pPetInfo->level + nLevel > MAX_PET_LEVEL)
		nAddLevel = MAX_PET_LEVEL - pPetInfo->level;

	if (nAddLevel == 0)
		return false;

	pPetInfo->level += nAddLevel;
	pPetInfo->statsPoints += 5 * nAddLevel;
	pPetInfo->exp = nExp;
	pPetInfo->curHP = 0xFFFFFFFF;
	pPetInfo->curMP = 0xFFFFFFFF;
	pPetInfo->curPP = 0xFFFFFFFF;


	PetObject *pPetObject = ((PetTable&)pPlayer->getPetTable()).getSpawnedPet(slot);
	if (pPetObject)
	{
		pPetObject->setLevel(pPetInfo->level);
	}

#ifdef NTJ_SERVER
	PetOperationManager::ServerSendPetAddLevelMsg(pPlayer, slot, nLevel);  //发送增加宠物等级的消息
	PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, slot, stPetInfo::PetLevel_Mask | stPetInfo::PetOther_Mask);
#endif
	return true;
}

stPetInfo* PetTable::getEmptyPetSlot(U32& slot)
{
	for (S32 i=0; i<PET_MAXSLOTS; ++i)
	{
		if(!mPetInfo[i].petDataId)
		{
			slot = i;
			return &(mPetInfo[i]);
		}
	}
	return NULL;
}

bool PetTable::getPetStatus(U32 slot, PetObject::enPetStatus &status) const
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return false;

	status = (PetObject::enPetStatus)mPetInfo[slot].status;
	return true;
}

void PetTable::swapPetInfo(U32 slot1, U32 slot2)
{
	stPetInfo temp = mPetInfo[slot1];
	mPetInfo[slot1] = mPetInfo[slot2];
	mPetInfo[slot2] = temp;
}

bool PetTable::RefreshPetInfos(U32 nBeginSlot, U32 &nEndSlot)
{
	if (nBeginSlot >= PET_MAXSLOTS - 1)
		return false;

	bool bSendPetIndex = false;
	bool bMeetEnd = false;
	S32 i = 0;
	for (i = nBeginSlot+1; i < PET_MAXSLOTS; i++)
	{
		if (mPetInfo[i].petDataId == 0)
		{
			bMeetEnd = true;
			break;
		}

		mPetInfo[i-1] = mPetInfo[i];
		mPetInfo[i].clear();
		for (S32 j = 0; j < PetObjectData::PetMax; j++)
		{
			if (!mPetObject[j])
				continue;
			if (mPetObject[j]->getPetSlot() == (i) )
			{
				bSendPetIndex = true;
				mPetObject[j]->setPetSlot(i-1);
			}
		}
	}

	if (bMeetEnd)
		nEndSlot = i - 1;
	else 
		nEndSlot = i;
	return bSendPetIndex;
}


bool PetTable::fillPetInfo(Player* player, ItemShortcut* pItem)
{
	if(!player || !pItem)
		return false;

	U32 slot = -1;
	stPetInfo* info = getEmptyPetSlot(slot);
	if(!info)
		return false;

	PetObjectData* pPetData = g_PetRepository.GetPetData(pItem->getRes()->getBaseAttribute());
	if(!pPetData)
		return false;

	info->petDataId = pItem->getRes()->getBaseAttribute();
	info->quality = mClamp(pItem->getRes()->getQualityLevel(), 0, MAX_PET_QUALITY);
	info->mature = (pItem->getRes()->getSubCategory() == GETSUB(Res::CATEGORY_PETEGG_GROWN_NORMAL) ||
					pItem->getRes()->getSubCategory() == GETSUB(Res::CATEGORY_PETEGG_GROWN_VARIATION));
	info->level = mClamp(pItem->getRes()->getReserveValue(), 1, EnablePlayerLevel);
	info->generation = 0;
	info->statsPoints = (info->level-1) * PET_STATSPOINTS_PERLEVEL;
	info->insight = pPetData->insight;
	info->petTalent = pPetData->petTalent;
	dStrcpy(info->name, 32, pPetData->petName);

	info->curHP = 0xFFFFFF;
	info->curMP = 0xFFFFFF;
	info->curPP = 0;

	info->staminaGift		= gRandGen.randI(MIN_PET_GIFT, MAX_PET_GIFT);
	info->ManaGift			= gRandGen.randI(MIN_PET_GIFT, MAX_PET_GIFT);
	info->strengthGift		= gRandGen.randI(MIN_PET_GIFT, MAX_PET_GIFT);
	info->intellectGift		= gRandGen.randI(MIN_PET_GIFT, MAX_PET_GIFT);
	info->agilityGift		= gRandGen.randI(MIN_PET_GIFT, MAX_PET_GIFT);
	info->pneumaGift		= gRandGen.randI(MIN_PET_GIFT, MAX_PET_GIFT);

	if(player->getControllingClient())
	{
		PetEvent* event = new PetEvent(slot);
		player->getControllingClient()->postNetEvent(event);
	}
	return true;
}

// ----------------------------------------------------------------------------
// 发送宠物或骑宠的初始化数据到客户端
void PetTable::sendInitialData(Player* player)
{
	GameConnection* conn = NULL;
	if(!player || !(conn = player->getControllingClient()))
		return;
	
	init(player);

	for (S32 i = 0; i < PET_MAXSLOTS; ++i)
	{
		PetEvent* ev = new PetEvent(i);
		conn->postNetEvent(ev);
	}

	for (S32 i = 0; i < MOUNTPET_MAXSLOTS; ++i)
	{
		MountPetEvent* ev = new MountPetEvent(i);
		conn->postNetEvent(ev);
	}
}

void PetTable::disbandAllPet(Player* player)
{
	for (S32 i = 0; i < PET_MAXSLOTS; ++i)
	{
		const stPetInfo *info = getPetInfo(i);
		if (!info || info->status != PetObject::PetStatus_Combat)
			continue;
		PetOperationManager::DisbandPet(player, i);
	}
}

U32 PetTable::getPetInfoCount() const
{
	U32 count = 0;
	for (S32 i=0; i<PET_MAXSLOTS; ++i)
	{
		if(mPetInfo[i].petDataId)
			++count;
	}
	return count;
}

S32 PetTable::getSpawnPetSlot()
{
	S32 nPetSlot = -1;

	for (S32 i = 0; i < PetObjectData::PetMax; i++)
	{
		PetObject *pPet = mPetObject[i];
		if (pPet)
		{
			nPetSlot = pPet->getPetSlot();
			break;
		}
	}

	return nPetSlot;
}

U32 PetTable::getSpawnPetID()
{
	U32 nPetId = 0;

	for (S32 i = 0; i < PetObjectData::PetMax; i++)
	{
		PetObject *pPet = mPetObject[i];
		if (pPet)
		{
			nPetId = pPet->getId();
			break;
		}
	}

	return nPetId;
}


void PetTable::setPetStatus(U32 slot, U32 status)
{
	if (slot < PET_MAXSLOTS && status < PetObject::PetStatus_Max)
		mPetInfo[slot].status = status;
}

bool PetTable::canComboPet(U32 slot)
{
	if (slot >= PET_MAXSLOTS)
		return false;
	return true;
}

void PetTable::saveData(stPlayerStruct* playerInfo)
{
	U32 nCurrTime = Platform::getRealMilliseconds();

	for (S32 i=0; i<PET_MAXSLOTS; ++i)
	{
		if (mPetInfo[i].petDataId != 0)
		{
#ifdef NTJ_SERVER
			U32 nElapseTime = nCurrTime - mPetStartTimes[i];
			mPetInfo[i].mLeftTime = (PET_HAPPINESS_TIME >= nElapseTime) ? (PET_HAPPINESS_TIME - nElapseTime) : 0;
#endif
			if (mPetInfo[i].status == PetObject::PetStatus_Freeze)
				mPetInfo[i].status = PetObject::PetStatus_Idle;
			playerInfo->MainData.PetInfo[i] = mPetInfo[i];
		}
		else
		{
			playerInfo->MainData.PetInfo[i].clear();
		}
	}

	for (S32 i=0; i<MOUNTPET_MAXSLOTS; ++i)
	{
		if (mMountInfo[i].mDataId != 0)
		{
#ifdef NTJ_SERVER
			U32 nElapseTime = nCurrTime - mMountStartTimes[i];
			mMountInfo[i].mLeftTime = (mMountInfo[i].mLeftTime >= nElapseTime) ? (mMountInfo[i].mLeftTime - nElapseTime) : 0;
#endif
			playerInfo->MainData.MountInfo[i] = mMountInfo[i];
		}
		else
		{
			playerInfo->MainData.MountInfo[i].clear();
		}
	}
}

void PetTable::loadData(stPlayerStruct* playerInfo)
{
	U32 nCurrTime = Platform::getRealMilliseconds();

	for (S32 i=0; i<PET_MAXSLOTS; ++i)
	{
#ifdef NTJ_SERVER
		mPetStartTimes[i] = nCurrTime;		
#endif
		mPetInfo[i] = playerInfo->MainData.PetInfo[i];
	}
	for (S32 i=0; i<MOUNTPET_MAXSLOTS; ++i)
	{
		mMountStartTimes[i] = nCurrTime;
		mMountInfo[i] = playerInfo->MainData.MountInfo[i];		
	}
}

void PetTable::packPetObject(NetConnection* conn, BitStream* stream)
{
	for (S32 i=0; i<PetObjectData::PetMax; ++i)
	{
		if(stream->writeFlag(mPetObject[i]))
		{
			stream->writeInt(conn->getGhostIndex(mPetObject[i]), NetConnection::GhostIdBitSize);
		}
	}
}

void PetTable::unpackPetObject(NetConnection* conn, BitStream* stream)
{
	for (S32 i=0; i<PetObjectData::PetMax; ++i)
	{
		if(stream->readFlag())
		{
			PetObject* pet = dynamic_cast<PetObject*>(conn->resolveGhost(stream->readInt(NetConnection::GhostIdBitSize)));
			if(pet)
				setPet(pet);
		}
	}
}

//骑宠相关函数
bool PetTable::getMountStatus(U32 slot, MountObject::enMountStatus &status)
{
	if (slot > MOUNTPET_MAXSLOTS || mMountInfo[slot].mDataId == 0)
		return false;
	status = (MountObject::enMountStatus)mMountInfo[slot].mStatus;
	return true;
}

stMountInfo* PetTable::getEmptyMountSlot	(U32& slot)
{
	for (S32 i=0; i<MOUNTPET_MAXSLOTS; ++i)
	{
		if(!mMountInfo[i].mDataId)
		{
			slot = i;
			return &(mMountInfo[i]);
		}
	}
	return NULL;
}

void PetTable::saveMountInfo()
{
}
void PetTable::loadMountInfo()
{
}

// ----------------------------------------------------------------------------
// 骑宠数据网络传输打包
void PetTable::writeMountInfo(Player* player, BitStream* stream, S32 slot/* = -1*/)
{
#ifdef NTJ_SERVER
	if(stream->writeFlag(slot >=0 && slot < MOUNTPET_MAXSLOTS))
	{
		stream->writeInt(slot, 4);
		mMountInfo[slot].WriteData(stream);
	}
#endif//NTJ_SERVER
}

// ----------------------------------------------------------------------------
// 骑宠数据网络传输解包
void PetTable::readMountInfo(Player* player, BitStream* stream)
{
#ifdef NTJ_CLIENT
	if(stream->readFlag())
	{
		S32 slot = stream->readInt(4);
		mMountInfo[slot].ReadData(stream);
		setMountStartTime(slot, Platform::getRealMilliseconds());
	}
#endif//NTJ_CLIENT
}

void PetTable::setMountStatus(U32 slot, U32 status)
{
	if (slot < MOUNTPET_MAXSLOTS && status < MountObject::MountStatus_Max)
		mMountInfo[slot].mStatus = status;
}
const stMountInfo*  PetTable::getMountInfo(U32 slot) const
{
	if (slot >= MOUNTPET_MAXSLOTS || mMountInfo[slot].mDataId == 0)
		return NULL;

	return &mMountInfo[slot];
}

void PetTable::swapMountInfo(U32 slot1, U32 slot2)
{
	stMountInfo temp = mMountInfo[slot1];
	mMountInfo[slot1] = mMountInfo[slot2];
	mMountInfo[slot2] = temp;
}

S32 PetTable::getMountedSlot() const
{
	for (S32 i=0; i<MOUNTPET_MAXSLOTS; ++i)
	{
		if(mMountInfo[i].mDataId && mMountInfo[i].mStatus == MountObject::MountStatus_Mount)
			return i;
	}
	return -1;
}

U32 PetTable::getMountInfoCount() const
{
	U32 count = 0;
	for (S32 i=0; i<MOUNTPET_MAXSLOTS; ++i)
	{
		if(mMountInfo[i].mDataId)
			++count;
	}
	return count;
}


////////////////////////////////////宠物属性点相关函数///////////////////////////////////////
S32 PetTable::getPetTiPo(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	Stats *pStats = g_BuffRepository.GetBuffStats(pPetInfo->randBuffId);
	if (!pStats)
		return 0;
	return pPetInfo->staminaPts + pStats->Stamina;
}

S32 PetTable::getPetJingLi(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	Stats *pStats = g_BuffRepository.GetBuffStats(pPetInfo->randBuffId);
	if (!pStats)
		return 0;
	return pPetInfo->ManaPts + pStats->Mana;
}

S32 PetTable::getPetLiDao(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	Stats *pStats = g_BuffRepository.GetBuffStats(pPetInfo->randBuffId);
	if (!pStats)
		return 0;
	return pPetInfo->strengthPts + pStats->Strength;
}

S32 PetTable::getPetLingLi(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	Stats *pStats = g_BuffRepository.GetBuffStats(pPetInfo->randBuffId);
	if (!pStats)
		return 0;
	return pPetInfo->intellectPts + pStats->Intellect;
}

S32 PetTable::getPetMingJie(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	Stats *pStats = g_BuffRepository.GetBuffStats(pPetInfo->randBuffId);
	if (!pStats)
		return 0;
	return pPetInfo->agilityPts + pStats->Agility;
}

S32 PetTable::getPetYuanLi(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	Stats *pStats = g_BuffRepository.GetBuffStats(pPetInfo->randBuffId);
	if (!pStats)
		return 0;
	return pPetInfo->pneumaPts + pStats->Pneuma;
}

S32 PetTable::getPetQianLi(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	return pPetInfo->statsPoints;
}

S32 PetTable::getPetChengZhangLv(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	return pPetInfo->chengZhangLv;
}

bool PetTable::isPetSpawned(U32 slot)
{
	if (slot >= PET_MAXSLOTS)
		return false;
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo)
		return false;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return false;
	PetObject *pPetObject = getPet(pPetData->combatPetIdx);
	if (!pPetObject)
		return false;
	return ( slot == pPetObject->getPetSlot() );
}

PetObject *PetTable::getSpawnedPet(U32 slot)
{
	if (slot >= PET_MAXSLOTS)
		return false;
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo)
		return false;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return false;
	PetObject *pPetObject = getPet(pPetData->combatPetIdx);
	return pPetObject;
}

////////////////////////////////////宠物界面显示相关函数///////////////////////////////////////////
#ifdef NTJ_CLIENT
bool PetTable::isPetExist(U32 slot)
{
	if (slot >= PET_MAXSLOTS)
		return false;
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo)
		return false;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return false;
	return true;
}

StringTableEntry PetTable::getPetName(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return NULL;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return NULL;
	return pPetInfo->name;
}

S32 PetTable::getPetLevel(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	return pPetInfo->level;
}

S32 PetTable::getPetSpawnLevel(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	return pPetData->spawnLevel;
}

S32 PetTable::getPetHuiGen(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	return pPetInfo->insight;
}

S32 PetTable::getPetGenGu(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	return pPetInfo->petTalent;
}

S32 PetTable::getPetGrowth(U32 slot)
{
	const stPetInfo *pPetInfo = getPetInfo(slot);
	if (!pPetInfo || !pPetInfo->petDataId)
		return 0;
	PetObjectData *pPetData = g_PetRepository.GetPetData(pPetInfo->petDataId);
	if (!pPetData)
		return 0;
	//return pPetInfo->g;
	return 0;
}

S32 PetTable::getPetWuAttack(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;

	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().PhyDamage;
	}
	return m_pStats[slot]->PhyDamage;
}

S32 PetTable::getPetYuanAttack(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().PneumaDamage;
	}
	return m_pStats[slot]->PneumaDamage;
}

S32 PetTable::getPetMuAttack(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().MuDamage;
	}
	return m_pStats[slot]->MuDamage;
}

S32 PetTable::getPetHuoAttack(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().HuoDamage;
	}
	return m_pStats[slot]->HuoDamage;
}

S32 PetTable::getPetTuAttack(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().TuDamage;
	}
	return m_pStats[slot]->TuDamage;
}

S32 PetTable::getPetJinAttack(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().JinDamage;
	}
	return m_pStats[slot]->JinDamage;
}

S32 PetTable::getPetShuiAttack(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().ShuiDamage;
	}
	return m_pStats[slot]->ShuiDamage;
}

S32 PetTable::getPetWuDefence(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().PhyDefence;
	}
	return m_pStats[slot]->PhyDefence;
}
S32 PetTable::getPetYuanDefence(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().PneumaDefence;
	}
	return m_pStats[slot]->PneumaDefence;
}
S32 PetTable::getPetMuDefence(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().MuDefence;
	}
	return m_pStats[slot]->MuDefence;
}
S32 PetTable::getPetHuoDefence(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().HuoDefence;
	}
	return m_pStats[slot]->HuoDefence;
}
S32 PetTable::getPetTuDefence(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0|| !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().TuDefence;
	}
	return m_pStats[slot]->TuDefence;
}
S32 PetTable::getPetJinDefence(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().JinDefence;
	}
	return m_pStats[slot]->JinDefence;
}
S32 PetTable::getPetShuiDefence(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().ShuiDefence;
	}
	return m_pStats[slot]->ShuiDefence;
}

//宠物资质
S32 PetTable::getPetStaminaGift(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].staminaGift;
}
S32 PetTable::getPetManaGift(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].ManaGift;
}
S32 PetTable::getPetStrengthGift(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].strengthGift;
}
S32 PetTable::getPetIntellectGift(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].intellectGift;
}
S32 PetTable::getPetAgilityGift(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].agilityGift;
}
S32 PetTable::getPetPneumaGift(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].pneumaGift;
}

//其他
S32 PetTable::getPetZhiLiao(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().Heal;
	}
	return m_pStats[slot]->Heal;
}

S32 PetTable::getPetShanBi(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().DodgeRating;
	}
	return m_pStats[slot]->DodgeRating;
}

S32 PetTable::getPetBaoJi(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().CriticalRating;
	}
	return m_pStats[slot]->CriticalRating;
}

S32 PetTable::getPetBaoJiLv(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0 || !m_pStats[slot])
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().CriticalTimes;
	}
	return m_pStats[slot]->CriticalTimes;
}

S32 PetTable::getPetHappiness(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].happiness;
}

S32 PetTable::getPetGeneration(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].generation;
}

S32 PetTable::getPetRace(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	
	PetObjectData *pPetData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
	return pPetData->race;
}

S32 PetTable::getPetSex(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].sex;
}

S32 PetTable::getPetStyle(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].style;
}

S32 PetTable::GetPetMp(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().MP;
	}
	return m_pStats[slot]->MP;
}
S32 PetTable::GetPetMaxMp(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().MaxMP;
	}
	return m_pStats[slot]->MaxMP;
}
S32 PetTable::GetPetHp(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().HP;
	}
	return m_pStats[slot]->HP;
}
S32 PetTable::GetPetMaxHp(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	
	if (mPetInfo[slot].status == PetObject::PetStatus_Combat)
	{
		//获得宠物对象的stats的对应值
		PetObjectData *pData = g_PetRepository.GetPetData(mPetInfo[slot].petDataId);
		if (!pData)
			return 0;
		PetObject *pPetObject = mPetObject[pData->combatPetIdx];
		if (!pPetObject)
			return 0;
		return pPetObject->GetStats().MaxHP;
	}
	return m_pStats[slot]->MaxHP;
}
S32 PetTable::GetPetExp(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return mPetInfo[slot].exp;
}
S32 PetTable::GetPetMaxExp(U32 slot)
{
	if (slot >= PET_MAXSLOTS || mPetInfo[slot].petDataId == 0)
		return 0;
	return g_PetLevelExp[mPetInfo[slot].level];
}

////////////////////////////////////骑宠界面显示相关函数///////////////////////////////////////////
bool PetTable::isMountExist(U32 slot)
{
	if (slot >= MOUNTPET_MAXSLOTS)
		return false;
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo || !pMountInfo->mDataId)
		return false;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return false;
	return true;
}

StringTableEntry PetTable::getMountName(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return NULL;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return NULL;
	return pMountData->mName;
}

StringTableEntry PetTable::getMountInfoDesc(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return NULL;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return NULL;
	return pMountData->mDescription;
}

S32 PetTable::getMountLevel(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return 0;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return 0;
	return pMountData->getCurrLevel();
}

S32 PetTable::getMountPlayerLevel(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return 0;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return 0;
	return pMountData->mMountLevel;
}

S32 PetTable::getMountSpeedupPercent(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return 0;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return 0;
	Stats *pStats = g_BuffRepository.GetBuffStats(pMountData->buffId);
	if (!pStats)
		return 0;
	return pStats->MountedVelocity_gPc;
}

S32 PetTable::getMountLeftTime(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return 0;
	return mRound((F32)pMountInfo->mLeftTime / 1000);
}

U8 PetTable::getMountType(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return 0;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return 0;
	return pMountData->mType;
}

S32 PetTable::getMountPlayerCount(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return 0;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return 0;
	return pMountData->mCount;
}

S32 PetTable::getMountRace(U32 slot)
{
	const stMountInfo *pMountInfo = getMountInfo(slot);
	if (!pMountInfo)
		return 0;
	MountObjectData *pMountData = g_MountRepository.GetMountData(pMountInfo->mDataId);
	if (!pMountData)
		return 0;
	return pMountData->mRace;
}
#endif


void PetTable::ProcessTick(Player *pPlayer)
{
	U32 nCurrTime = Platform::getRealMilliseconds();

#ifdef NTJ_SERVER
	//宠物快乐度每一个小时计算扣除
	for (S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		stPetInfo *pPetInfo = (stPetInfo *)getPetInfo(i);
		if(!pPetInfo)
			continue;		

		U8 nReduceHappiness = 0;
		switch(pPetInfo->status)
		{
			case PetObject::PetStatus_Combat:	nReduceHappiness = 1; break;
			case PetObject::PetStatus_Combo:	nReduceHappiness = 1; break;
			default:	
				break;
		}

		if (nReduceHappiness == 0)
			continue;
		
		if ( (nCurrTime - mPetStartTimes[i]) < pPetInfo->mLeftTime )
			continue;

		pPetInfo->mLeftTime = PET_HAPPINESS_TIME;
		mPetStartTimes[i] = nCurrTime;		//重设快乐度计时的开始时间
		pPetInfo->happiness = (pPetInfo->happiness >= nReduceHappiness) ? (pPetInfo->happiness - nReduceHappiness) : 0;
		PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, i, stPetInfo::PetHappiness_Mask);
		if ( isPetSpawned(i) )
		{
			if (pPetInfo->happiness == 0)
			{
				PetOperationManager::DisbandPet(pPlayer, i);
			}
			else if (pPetInfo->happiness < 60)
			{
				char sMsg[256];
				dSprintf(sMsg, sizeof(sMsg), "%s的快乐度不足", pPetInfo->name);
				MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_NOTIFY, sMsg);
			}			
		}
	}
#endif

	//骑宠剩余时间计算
	for (S32 i = 0; i < MOUNTPET_MAXSLOTS; i++)
	{
		stMountInfo *pMountInfo = (stMountInfo *)getMountInfo(i);
		if(!pMountInfo)
			continue;

		if (pMountInfo->mLeftTime == 0)
		{
#ifdef NTJ_SERVER
			//删除当前骑宠
			bool bSuccess = PetMountOperationManager::ForceReleaseMountPet(pPlayer, i);
			PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_Release, i, bSuccess);
#endif
			continue;
		}

		U32 nElapsedTime = nCurrTime - mMountStartTimes[i];
#ifdef NTJ_CLIENT
		if (nElapsedTime < MOUNT_REFRESH_TIME)
			continue;		
#endif
		mMountStartTimes[i] = nCurrTime;
		pMountInfo->mLeftTime = (pMountInfo->mLeftTime >= nElapsedTime) ? (pMountInfo->mLeftTime - nElapsedTime) : 0;
#ifdef NTJ_CLIENT
		if (pMountInfo->mLeftTime < 3600000)
		{
			Con::executef("Mount_RefreshLeftTime", Con::getIntArg(i));
		}
#endif
	}
}