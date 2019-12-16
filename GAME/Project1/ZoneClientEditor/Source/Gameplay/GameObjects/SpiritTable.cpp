#include "Gameplay/Data/SpiritData.h"
#include "Gameplay/GameObjects/SpiritTable.h"
#include "Gameplay/GameplayCommon.h"
#include "Gameplay/GameObjects/SpiritOperationManager.h"
#include "Gameplay/GameObjects/ShortcutObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/TalentShortcut.h"

#ifdef NTJ_CLIENT
#include "console/consoleTypes.h"
#include "Gameplay/Item/Player_Item.h"
#endif


SpiritTable::SpiritTable()
{
#ifdef NTJ_CLIENT
	m_ppSpiritSkillList = NULL;
#endif
	mIsHuanHua = false;
}

SpiritTable::~SpiritTable()
{
#ifdef NTJ_CLIENT
	ClearAllSpiritSlot();
#endif
}

// ----------------------------------------------------------------------------
// 数据保存
void SpiritTable::SaveData(stPlayerStruct* playerInfo)
{
	playerInfo->MainData.bSpiritHuanHua = mIsHuanHua;
	playerInfo->MainData.SpiritActiveSlot = mActiveSlot;
	for (S32 i=0; i<SPIRIT_MAXSLOTS; ++i)
	{
		if (mSpiritInfo[i].mDataId != 0)
		{
			playerInfo->MainData.SpiritInfo[i] = mSpiritInfo[i];
		}
		else
		{
			playerInfo->MainData.SpiritInfo[i].clear();
		}
	}
	playerInfo->MainData.SpiritOpenedFlag = mOpenedSlotFlag;
}

// ----------------------------------------------------------------------------
// 数据加载
void SpiritTable::LoadData(stPlayerStruct* playerInfo)
{
	mIsHuanHua = playerInfo->MainData.bSpiritHuanHua;
 	mActiveSlot = playerInfo->MainData.SpiritActiveSlot;
	for (S32 i=0; i<SPIRIT_MAXSLOTS; ++i)
	{
		if (playerInfo->MainData.SpiritInfo[i].mDataId != 0)
		{
			mSpiritInfo[i] = playerInfo->MainData.SpiritInfo[i];			
		}
	}

	if (playerInfo->MainData.SpiritOpenedFlag == 0)
		mOpenedSlotFlag = 0x3;
	else
		mOpenedSlotFlag = playerInfo->MainData.SpiritOpenedFlag;
}

// ----------------------------------------------------------------------------
// 数据网络传输打包
void SpiritTable::PackData(Player*, BitStream* stream, S32 slot /*= -1*/, U32 flag /*= 0xFFFFFFFF*/)
{
	if(stream->writeFlag(slot >=0 && slot < SPIRIT_MAXSLOTS))
	{
		stream->writeInt(slot, 4);
		mSpiritInfo[slot].WriteInfo(stream, flag);
	}
}

// ----------------------------------------------------------------------------
// 数据网络传输打包
void SpiritTable::UnpackData(Player*, BitStream* stream)
{
	if(stream->readFlag())
	{
		S32 nSlot = stream->readInt(4);
		mSpiritInfo[nSlot].ReadInfo(stream);
	}
}

stSpiritInfo * SpiritTable::getSpiritInfo( S32 nSlot )
{
	if (nSlot < 0 || nSlot >= SPIRIT_MAXSLOTS || mSpiritInfo[nSlot].mDataId == 0)
		return NULL;
	return &mSpiritInfo[nSlot];
}

bool SpiritTable::isSpiritOpened(S32 nSlot)
{
	return mOpenedSlotFlag & (BIT(nSlot));
}

bool SpiritTable::isSpiritActive(S32 nSlot)
{
	return mActiveSlot == nSlot;
}

S32 SpiritTable::getSpiritCount()
{
	S32 nCount = 0;

	for (S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		if (!isSpiritOpened(i) || mSpiritInfo[i].mDataId == 0)
			continue;
		nCount++;
	}

	return nCount;
}

bool SpiritTable::isJueZhaoOpened(S32 nSpiritSlot, S32 nSlot)
{
	return mSpiritInfo[nSpiritSlot].mOpenedSkillFlag & BIT(nSlot);
}

void SpiritTable::OpenJueZhao(S32 nSpiritSlot, S32 nSlot)
{
	mSpiritInfo[nSpiritSlot].mOpenedSkillFlag |= BIT(nSlot);
}

bool SpiritTable::isJueZhaoExist(S32 nSpiritSlot, S32 nSlot)
{
	return (mSpiritInfo[nSpiritSlot].mSkillId[nSlot] != 0);
}

void SpiritTable::AddJueZhao(Player *pPlayer, S32 nSpiritSlot, S32 nSlot, U32 nSkillId)
{
	mSpiritInfo[nSpiritSlot].mSkillId[nSlot] = nSkillId;

#ifdef NTJ_SERVER
	pPlayer->AddSkill(nSkillId, true);	
#endif

#ifdef NTJ_CLIENT
	m_ppSpiritSkillList[nSpiritSlot]->AddSkillShortcut(pPlayer, Macro_GetSkillSeriesId(nSkillId), nSlot);
#endif
}

void SpiritTable::RemoveJueZhao(Player *pPlayer, S32 nSpiritSlot, S32 nSlot)
{
	U32 nSkillId = mSpiritInfo[nSpiritSlot].mSkillId[nSlot];
	mSpiritInfo[nSpiritSlot].mSkillId[nSlot] = 0;
#ifdef NTJ_SERVER
	pPlayer->RemoveSkill(nSkillId);	
#endif

#ifdef NTJ_CLIENT
	m_ppSpiritSkillList[nSpiritSlot]->AddSkillShortcut(pPlayer, Macro_GetSkillSeriesId(nSkillId), nSlot);
#endif
}


#ifdef NTJ_SERVER
void SpiritTable::sendInitialData(Player *pPlayer)
{
	//加载元神绝招到玩家对象的mSkillTable
	for (S32 i =0; i < SPIRIT_MAXSLOTS; i ++)
	{
		if (mSpiritInfo[i].mDataId == 0)
			continue;
		for (S32 j = 0; j < MAX_SPIRIT_SKILL; j++)
		{
			if (mSpiritInfo[i].mSkillId[j] > 0)
				pPlayer->AddSkill(mSpiritInfo[i].mSkillId[j], true);
		}
	}

	SpiritOperationManager::ServerSendInitSpiritMsg(pPlayer);
	for (int i = 0; i < SPIRIT_MAXSLOTS; i++)
	{
		SpiritOperationManager::ServerSendUpdateSpiritSlotMsg(pPlayer, i);
	}

	if (mActiveSlot >= 0 && mActiveSlot < SPIRIT_MAXSLOTS)
	{
		AddTalentSetToTalentTable(pPlayer, mActiveSlot);
	}
}

void SpiritTable::AddTalentToTalentTable(Player* pPlayer, S32 nSlot, S32 nIndex)
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData( mSpiritInfo[nSlot].mDataId );
	TalentSetData* pTalentSetData = g_TalentRepository.getTalentSet( pSpiritData->mTalentSetId );
	TalentSetData::TalentNode *pNode = pTalentSetData->getNode(nIndex);
	S32 nLevel = mSpiritInfo[nSlot].mTalentSkillPts[nIndex];
	if (nLevel == 0)
		return;
	U32 nTalentId = Macro_GetTalentId(pNode->talentSeriesId, nLevel);
	pPlayer->mTalentTable.addTalent(nTalentId);
}

void SpiritTable::RemoveTalentFromTalentTable(Player* pPlayer, S32 nSlot, S32 nIndex)
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData( mSpiritInfo[nSlot].mDataId );
	TalentSetData* pTalentSetData = g_TalentRepository.getTalentSet( pSpiritData->mTalentSetId );
	TalentSetData::TalentNode *pNode = pTalentSetData->getNode(nIndex);
	S32 nLevel = mSpiritInfo[nSlot].mTalentSkillPts[nIndex];
	if (nLevel == 0)
		return;
	U32 nTalentId = Macro_GetTalentId(pNode->talentSeriesId, nLevel);
	pPlayer->mTalentTable.removeTalent(nTalentId);
}

void SpiritTable::AddTalentSetToTalentTable(Player* pPlayer, S32 nSlot)
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData( mSpiritInfo[nSlot].mDataId );
	TalentSetData* pTalentSetData = g_TalentRepository.getTalentSet( pSpiritData->mTalentSetId );
	
	for(S32 nIndex = 0; nIndex < MAX_TALENT; nIndex++)
	{
		TalentSetData::TalentNode *pNode = pTalentSetData->getNode(nIndex);
		if (!pNode) 
			continue;
		S32 nLevel = mSpiritInfo[nSlot].mTalentSkillPts[nIndex];
		if (nLevel == 0)
			continue;
		U32 nTalentId = Macro_GetTalentId(pNode->talentSeriesId, nLevel);
		pPlayer->mTalentTable.addTalent(nTalentId);
	}
}

void SpiritTable::RemoveTalentSetFromTalentTable(Player* pPlayer, S32 nSlot)
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData( mSpiritInfo[nSlot].mDataId );
	TalentSetData* pTalentSetData = g_TalentRepository.getTalentSet( pSpiritData->mTalentSetId );

	for(S32 nIndex = 0; nIndex < MAX_TALENT; nIndex++)
	{
		TalentSetData::TalentNode *pNode = pTalentSetData->getNode(nIndex);
		if (!pNode) 
			continue;
		S32 nLevel = mSpiritInfo[nSlot].mTalentSkillPts[nIndex];
		if (nLevel == 0)
			continue;
		U32 nTalentId = Macro_GetTalentId(pNode->talentSeriesId, nLevel);
		pPlayer->mTalentTable.removeTalent(nTalentId);
	}
}
#endif

#ifdef NTJ_CLIENT
void SpiritTable::initialize(S32 nActiveSlot, U32 nOpenedSlotFlag)
{
	mActiveSlot = nActiveSlot;
	mOpenedSlotFlag = nOpenedSlotFlag;
}
#endif

S32 SpiritTable::findEmptySlot()
{
	for (S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		if (!isSpiritOpened(i))
			continue;

		if (mSpiritInfo[i].mDataId == 0)
		{
			return i;
		}
	}
	return -1;
}

S32 SpiritTable::findFirstLockedSlot()
{
	for (S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		if (isSpiritOpened(i))
			continue;
		return i;
	}
	return -1;
}

void SpiritTable::createSpiritInfo(Player *pPlayer, S32 nSlot, U32 nSpiritId)
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(nSpiritId);

	dMemset(&mSpiritInfo[nSlot], 0, sizeof(stSpiritInfo));

	mSpiritInfo[nSlot].mDataId = nSpiritId;
	mSpiritInfo[nSlot].mLevel = 1;
	mSpiritInfo[nSlot].mCurrExp = 0;
	mSpiritInfo[nSlot].mOpenedSkillFlag = pSpiritData->mOpenedSkillFlag;

	for (int i = 0; i < MAX_SPIRIT_SKILL; i++)
	{
		bool bOpen = pSpiritData->mOpenedSkillFlag & BIT(i);
		if (!bOpen || pSpiritData->mSkillIDs[i] == 0)
			continue;

		mSpiritInfo[nSlot].mSkillId[i] = pSpiritData->mSkillIDs[i];
		pPlayer->AddSkill(pSpiritData->mSkillIDs[i], true);
	}
}

void SpiritTable::removeSpiritInfo(S32 nSlot)
{
	dMemset(&mSpiritInfo[nSlot], 0, sizeof(stSpiritInfo));
#ifdef NTJ_CLIENT
	clearSpiritSkillSlot(nSlot);		//清空此元神的绝招栏
#endif
}

//////////////////////////////// 元神相关属性 ///////////////////////////////////////////////
S32 SpiritTable::getSpiritType(S32 nSlot)
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(mSpiritInfo[nSlot].mDataId);
	if (!pSpiritData)
		return -1;
	return pSpiritData->mRace;
}

bool SpiritTable::isSlotValid(S32 nSlot)
{
	return (nSlot >= 0 && nSlot < SPIRIT_MAXSLOTS);
}

bool SpiritTable::isSpiritExist( S32 nSlot )
{
	if (mSpiritInfo[nSlot].mDataId == 0)
		return false;
	return true;
}

StringTableEntry SpiritTable::getSpiritName( S32 nSlot )
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(mSpiritInfo[nSlot].mDataId);
	if (!pSpiritData)
		return NULL;
	return pSpiritData->mName;
}

U32 SpiritTable::getTalentSetId( S32 nSlot )
{
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(mSpiritInfo[nSlot].mDataId);
	if (!pSpiritData)
		return 0;
	return pSpiritData->mTalentSetId;
}

U16 SpiritTable::getTalentExtPts(S32 nSlot)
{
	return mSpiritInfo[nSlot].mTalentExtPts;
}

U16 SpiritTable::getTalentLeftPts(S32 nSlot)
{
	return mSpiritInfo[nSlot].mTalentLeftPts;
}
U16 SpiritTable::getTalentPts(S32 nSlot)
{
	S32 count = 0;
	for (S32 i = 0; i < MAX_TALENT; i++)
	{
		count += mSpiritInfo[nSlot].mTalentSkillPts[i];
	}
	return count;
}

U32 SpiritTable::getCleanPtsCount( S32 nSlot )
{
	return mSpiritInfo[nSlot].mCleanPtsCount;
}

U32 SpiritTable::getValueBuffId( S32 nSlot )
{
	return mSpiritInfo[nSlot].mValueBuffId;
}

U32 SpiritTable::getCostBuffId( S32 nSlot )
{
	return mSpiritInfo[nSlot].mCostBuffId;
}

U32 SpiritTable::getCurrExp(S32 nSlot)
{
	return mSpiritInfo[nSlot].mCurrExp;
}

U32 SpiritTable::getMaxExp(S32 nSlot)
{
	if (mSpiritInfo[nSlot].mLevel == 0 || mSpiritInfo[nSlot].mLevel > 160)
		return 0;

	return g_SpiritLevelExp[mSpiritInfo[nSlot].mLevel - 1];
}

S32 SpiritTable::getCurrLevel(S32 nSlot)
{
	return mSpiritInfo[nSlot].mLevel;
}

S32 SpiritTable::getMaxLevel(S32 nSlot)
{
	return 160;
}

bool SpiritTable::isTalentExist(S32 nSlot, S32 nTalentIndex)
{
	if (!isSpiritExist(nSlot))
		return false;

#ifdef NTJ_CLIENT
	TalentShortcut *pTalentShortcut = dynamic_cast<TalentShortcut *>(m_ppSpiritSkillList[nSlot]->GetSlot(nTalentIndex));
	if (!pTalentShortcut)
		return false;
	return pTalentShortcut->isValid();
#endif
	return true;
}

S32 SpiritTable::getTalentLevel(S32 nSlot, S32 nTalentIndex)
{
	if (!isSpiritExist(nSlot) || nTalentIndex < 0 || nTalentIndex >= MAX_TALENT)
		return -1;
	return mSpiritInfo[nSlot].mTalentSkillPts[nTalentIndex];
}

S32 SpiritTable::getTalentMaxLevel(S32 nSlot, S32 nTalentIndex)
{
	if (!isSpiritExist(nSlot) || nTalentIndex < 0 || nTalentIndex >= MAX_TALENT)
		return 0;
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(mSpiritInfo[nSlot].mDataId);
	if (!pSpiritData)
		return 0;
	TalentSetData *pTalentSet = g_TalentRepository.getTalentSet(pSpiritData->mTalentSetId);
	if (!pTalentSet)
		return 0;
	TalentSetData::TalentNode *node = pTalentSet->getNode(nTalentIndex);
	if (!node)
		return 0;
	return node->maxLevel;
}

StringTableEntry SpiritTable::getTalentBackImage(S32 nSlot)
{
	if (!isSpiritExist(nSlot) )
		return NULL;
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(mSpiritInfo[nSlot].mDataId);
	if (!pSpiritData)
		return NULL;
	TalentSetData *pTalentSet = g_TalentRepository.getTalentSet(pSpiritData->mTalentSetId);
	if (!pTalentSet)
		return NULL;
	return pTalentSet->getBackground();
}

U32 SpiritTable::getHuanHuaId(S32 nSlot)
{
	S32 race = getSpiritType(nSlot);
	
	switch(race)
	{
	case 0:			//圣
		return Macro_GetBuffId(36001, 1);
	case 1:			//佛
		return Macro_GetBuffId(36006, 1);
	case 2:			//仙
		return Macro_GetBuffId(36003, 1);
	case 3:			//精
		return Macro_GetBuffId(36004, 1);
	case 4:			//鬼
		return Macro_GetBuffId(36005, 1);
	case 5:			//怪
		return Macro_GetBuffId(36002, 1);
	case 6:			//妖
		return Macro_GetBuffId(36007, 1);
	case 7:			//魔
		return Macro_GetBuffId(36008, 1);
	case 8:			//神
		//return Macro_GetBuffId(36001, 1);
		return 0;	//暂时没有设置
	default:
		break;
	}
	return 0;
}

bool SpiritTable::addExp( S32 slot, S32 nExp, Player *pPlayer )
{
	stSpiritInfo *pSpiritInfo = (stSpiritInfo*)getSpiritInfo(slot);
	if (!pSpiritInfo || pSpiritInfo->mLevel == MAX_SPIRIT_LEVEL)		//条件需要修改
		return false;
	pSpiritInfo->mCurrExp += nExp;
	//检查是否需要升级
#ifdef NTJ_SERVER
	SpiritOperationManager::ServerSendSpiritAddExpMsg(pPlayer, slot, nExp);  //先发送增加宠物经验的消息
#endif

	S32 i = pSpiritInfo->mLevel - 1;
	S32 nAddLevel = 0;
	while(i < MAX_SPIRIT_LEVEL)
	{
		if (g_SpiritLevelExp[i] <= pSpiritInfo->mCurrExp)
		{
			pSpiritInfo->mCurrExp -= g_SpiritLevelExp[i];
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
		return addLevel(slot, nAddLevel, pPlayer, pSpiritInfo->mCurrExp);
	}
	else if (nExp > 0)
	{
#ifdef NTJ_SERVER
		SpiritOperationManager::ServerSendUpdateSpiritSlotMsg(pPlayer, slot, stSpiritInfo::Exp_Mask);
#endif
		return true;
	}

	return false;
}

bool SpiritTable::addLevel( S32 slot, S32 nLevel, Player *pPlayer, S32 nExp /*= 0*/ )
{
	stSpiritInfo *pSpiritInfo = (stSpiritInfo*)getSpiritInfo(slot);
	if (!pSpiritInfo || pSpiritInfo->mLevel == MAX_SPIRIT_LEVEL)
		return false;

	S32 nAddLevel = nLevel;

	if (pSpiritInfo->mLevel + nLevel >= MAX_SPIRIT_LEVEL)
		nAddLevel = MAX_SPIRIT_LEVEL - pSpiritInfo->mLevel;
		
	if (nAddLevel == 0)
		return false;

	pSpiritInfo->mLevel += nAddLevel;
	pSpiritInfo->mCurrExp = nExp;
	pSpiritInfo->mTalentLeftPts += nAddLevel;
#ifdef NTJ_SERVER
	SpiritOperationManager::ServerSendUpdateSpiritSlotMsg(pPlayer, slot, 
														stSpiritInfo::Level_Mask | 
														stSpiritInfo::Exp_Mask | 
														stSpiritInfo::TalentPts_Mask);
	SpiritOperationManager::ServerSendSpiritAddLevelMsg(pPlayer, slot, nAddLevel);
#endif
	return true;
}

bool SpiritTable::addJingJie(S32 slot, S32 nJingJie, Player *pPlayer)
{
	if (nJingJie <= 0 || !isSpiritExist(slot) || !isSpiritOpened(slot) || mSpiritInfo->mJingJieLevel == 10)
		return false;
	
	S32 nAddJingJie = nJingJie;
	if (mSpiritInfo->mJingJieLevel + nJingJie > MAX_SPIRIT_JINGJIE)
		nAddJingJie = MAX_SPIRIT_JINGJIE - mSpiritInfo->mJingJieLevel;
	if (nAddJingJie <= 0)
		return false;

	mSpiritInfo->mJingJieLevel += nAddJingJie;
#ifdef NTJ_SERVER
	SpiritOperationManager::ServerSendUpdateSpiritSlotMsg(pPlayer, slot, stSpiritInfo::JingJie_Mask);
#endif
	return true;
}

#ifdef NTJ_CLIENT
void SpiritTable::InitAllSpiritSlot(Player *pPlayer)
{
	ClearAllSpiritSlot();
	InitAllSpiritSkillSlot(pPlayer);
	initAllTalentSkillSlot(pPlayer);
}

void SpiritTable::ClearAllSpiritSlot()
{
	clearAllSpiritSkillSlot();
	clearAllTalentSkillSlot();
}

//////////////////// 元神绝招栏相关函数 /////////////////////////////
void SpiritTable::InitAllSpiritSkillSlot(Player *pPlayer)
{
	m_ppSpiritSkillList = new SkillList_Spirit * [SPIRIT_MAXSLOTS];
	for (int i = 0; i < SPIRIT_MAXSLOTS; ++i)
	{
		m_ppSpiritSkillList[i] = NULL;
		if (!isSpiritOpened(i) || !isSpiritExist(i))
			continue;
		m_ppSpiritSkillList[i] = new SkillList_Spirit();
		AddSpiritSkillSlot(pPlayer, i);
	}
}

ShortcutObject* SpiritTable::getCurrentSpiritSkillSlot(S32 nSkillSlot)
{
	if (nSkillSlot < 0 || nSkillSlot >= MAX_SPIRIT_SKILL || !m_ppSpiritSkillList)
		return NULL;
	S32 nSpiritSlot = Con::getIntVariable("$SoulGeniusWnd_SpiritSlot");
	if (nSpiritSlot < 0 || nSpiritSlot >= SPIRIT_MAXSLOTS || !m_ppSpiritSkillList[nSpiritSlot])
		return NULL;
	return m_ppSpiritSkillList[nSpiritSlot]->GetSlot(nSkillSlot);
}

ShortcutObject* SpiritTable::getCurrentSpiritSkillSlot_2(S32 nSkillSlot)
{
	if (nSkillSlot < 0 || nSkillSlot >= MAX_SPIRIT_SKILL || !m_ppSpiritSkillList)
		return NULL;
	S32 nSpiritSlot = Con::getIntVariable("$SpiritJueZhaoWnd_SpiritSlot");
	if (nSpiritSlot < 0 || nSpiritSlot >= SPIRIT_MAXSLOTS || !m_ppSpiritSkillList[nSpiritSlot])
		return NULL;
	return m_ppSpiritSkillList[nSpiritSlot]->GetSlot(nSkillSlot);
}

void SpiritTable::clearAllSpiritSkillSlot()
{
	if (m_ppSpiritSkillList)
	{
		for (int i = 0; i < SPIRIT_MAXSLOTS; i++)
		{
			SAFE_DELETE(m_ppSpiritSkillList[i]);
		}
		delete [] m_ppSpiritSkillList;
		m_ppSpiritSkillList = NULL;
	}
}

void SpiritTable::clearSpiritSkillSlot(S32 nSpiritSlot)
{
	if (nSpiritSlot >=0 && nSpiritSlot < SPIRIT_MAXSLOTS)
		SAFE_DELETE(m_ppSpiritSkillList[nSpiritSlot]);
}

void SpiritTable::removeSpiritSkillSlot(S32 nSpiritSlot, S32 nIndex)
{
	if (!m_ppSpiritSkillList[nSpiritSlot])
		return;
	m_ppSpiritSkillList[nSpiritSlot]->SetSlot(nIndex, NULL);
}

void SpiritTable::AddSpiritSkillSlot(Player *pPlayer, S32 nSpiritSlot)
{
	if (!m_ppSpiritSkillList[nSpiritSlot])
		m_ppSpiritSkillList[nSpiritSlot] = new SkillList_Spirit();

	for(int j = 0; j < MAX_SPIRIT_SKILL; ++j)
	{
		if (mSpiritInfo[nSpiritSlot].mSkillId[j] == 0)
			continue;
		m_ppSpiritSkillList[nSpiritSlot]->AddSkillShortcut(pPlayer, Macro_GetSkillSeriesId(mSpiritInfo[nSpiritSlot].mSkillId[j]), j);
	}
}

//////////////////// 元神天赋技能栏相关函数 /////////////////////////////
void SpiritTable::initAllTalentSkillSlot(Player *pPlayer)
{
	m_ppTalentSkillList = new TalentSkillList * [SPIRIT_MAXSLOTS];
	for (int i = 0; i < SPIRIT_MAXSLOTS; ++i)
	{
		m_ppTalentSkillList[i] = NULL;
		if (!isSpiritOpened(i) || !isSpiritExist(i))
			continue;
		m_ppTalentSkillList[i] = new TalentSkillList();
		m_ppTalentSkillList[i]->Initialize(pPlayer, i);
	}
}

ShortcutObject* SpiritTable::getCurrentTalentSkillSlot( S32 nTalentSkillSlot )
{
	if (nTalentSkillSlot < 0 || nTalentSkillSlot >= MAX_TALENT || !m_ppSpiritSkillList)
		return NULL;
	S32 nSpiritSlot = Con::getIntVariable("$SoulGeniusWnd_SpiritSlot");
	if (nSpiritSlot < 0 || nSpiritSlot >= SPIRIT_MAXSLOTS || !m_ppTalentSkillList[nSpiritSlot])
		return NULL;
	return m_ppTalentSkillList[nSpiritSlot]->GetSlot(nTalentSkillSlot);
}

void SpiritTable::clearAllTalentSkillSlot()
{
	if (m_ppTalentSkillList)
	{
		for (int i = 0; i < SPIRIT_MAXSLOTS; i++)
		{
			SAFE_DELETE(m_ppTalentSkillList[i]);
		}
		delete [] m_ppTalentSkillList;
		m_ppTalentSkillList = NULL;
	}
}

void SpiritTable::clearTalentSkillSlot(S32 nSpiritSlot)
{
	if (nSpiritSlot >=0 && nSpiritSlot < SPIRIT_MAXSLOTS)
		SAFE_DELETE(m_ppTalentSkillList[nSpiritSlot]);
}

void SpiritTable::AddTalentSkillSlot(Player *pPlayer, S32 nSpiritSlot)
{
	SAFE_DELETE(m_ppTalentSkillList[nSpiritSlot]);
	if (!isSpiritOpened(nSpiritSlot) || !isSpiritExist(nSpiritSlot))
		return;
	m_ppTalentSkillList[nSpiritSlot] = new TalentSkillList();
	m_ppTalentSkillList[nSpiritSlot]->Initialize(pPlayer, nSpiritSlot);
}

void SpiritTable::updateTalentSkillSlot(Player *pPlayer, S32 nSpiritSlot, S32 nTalentIndex)
{
	if (!m_ppTalentSkillList || !m_ppTalentSkillList[nSpiritSlot])
		return;
	TalentShortcut *pTalentShortcut = dynamic_cast<TalentShortcut*>( m_ppTalentSkillList[nSpiritSlot]->GetSlot(nTalentIndex) );
	if (!pTalentShortcut)
		return;
	pTalentShortcut->update();
}
#endif

