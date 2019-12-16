//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/Talent.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Data/readDataFile.h"


// ========================================================================================================================================
//  TalentData
// ========================================================================================================================================
TalentData::TalentData()
{
	dMemset(this, 0, sizeof(TalentData));
}


// ========================================================================================================================================
//  TalentSetData
// ========================================================================================================================================
TalentSetData::TalentSetData()
{
	dMemset(this, 0, sizeof(TalentSetData));
}
TalentSetData::~TalentSetData()
{
	clear();
}

void TalentSetData::clear()
{
	for (S32 i=0; i<MaxRow; ++i)
	{
		for (S32 j=0; j<MaxCol; ++j)
		{
			SAFE_DELETE(mNodes[i][j]);
		}
	}
}

TalentSetData::TalentNode * TalentSetData::getNode(S32 nIndex)
{
	S32 row = nIndex / MaxCol;
	S32 col = nIndex - row * MaxCol;
	return mNodes[row][col];
}

// ========================================================================================================================================
//  TalentRepository
// ========================================================================================================================================
TalentRepository::TalentRepository()
{
}
TalentRepository::~TalentRepository()
{
	clear();
}

void TalentRepository::clear()
{
	for (TalentDataMap::iterator itr = mTalentDataMap.begin(); itr != mTalentDataMap.end(); ++itr)
		SAFE_DELETE(itr->second);
	for (TalentSetDataMap::iterator itr = mTalentSetDataMap.begin(); itr != mTalentSetDataMap.end(); ++itr)
		SAFE_DELETE(itr->second);
	mTalentDataMap.clear();
}

void TalentRepository::read()
{
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取Talent数据
	Platform::makeFullPathName("gameres/data/repository/TalentRepository.dat", filename, sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can NOT read file : TalentRepository.dat!");
	for(S32 i = 0 ; i < file.RecordNum ; ++ i)
	{
		TalentData* pData = new TalentData;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mSeriesId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "TalentRepository.dat::Read !");
		pData->mLevel = tempData.m_U32;
		// 组成id
		pData->mTalentId = Macro_GetTalentId(pData->mSeriesId, pData->mLevel);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "TalentRepository.dat::Read !");
		pData->mName = StringTable->insert(tempData.m_string);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "TalentRepository.dat::Read !");
		pData->mText = StringTable->insert(tempData.m_string);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "TalentRepository.dat::Read !");
		pData->mIcon = StringTable->insert(tempData.m_string);
		// 天赋1
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "TalentRepository.dat::Read !");
		pData->mTalentUnits[0].mType = (TalentData::TalentType)tempData.m_Enum8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mTalentUnits[0].mOriginId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mTalentUnits[0].mGeneralId = tempData.m_U32;
		// 天赋2
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "TalentRepository.dat::Read !");
		pData->mTalentUnits[1].mType = (TalentData::TalentType)tempData.m_Enum8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mTalentUnits[1].mOriginId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mTalentUnits[1].mGeneralId = tempData.m_U32;
		// 天赋3
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "TalentRepository.dat::Read !");
		pData->mTalentUnits[2].mType = (TalentData::TalentType)tempData.m_Enum8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mTalentUnits[2].mOriginId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mTalentUnits[2].mGeneralId = tempData.m_U32;
		// 天赋4
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "TalentRepository.dat::Read !");
		pData->mTalentUnits[3].mType = (TalentData::TalentType)tempData.m_Enum8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mTalentUnits[3].mOriginId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentRepository.dat::Read !");
		pData->mTalentUnits[3].mGeneralId = tempData.m_U32;

		mTalentDataMap.insert(TalentDataMap::value_type(pData->mTalentId, pData));
	}

	// 读取TalentNode数据，组成TalentSetData(天赋树)
	Platform::makeFullPathName("gameres/data/repository/TalentSetRepository.dat", filename, sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can NOT read file : TalentSetRepository.dat!");
	for(S32 i = 0 ; i < file.RecordNum ; ++ i)
	{
		TalentSetData::TalentNode* pData = new TalentSetData::TalentNode;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentSetRepository.dat::Read !");
		U32 setId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "TalentSetRepository.dat::Read !");
		StringTableEntry background = StringTable->insert(tempData.m_string);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "TalentSetRepository.dat::Read !");
		pData->row = tempData.m_U8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "TalentSetRepository.dat::Read !");
		pData->col = tempData.m_U8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentSetRepository.dat::Read !");
		pData->preSeriesId[0] = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentSetRepository.dat::Read !");
		pData->preSeriesId[1] = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentSetRepository.dat::Read !");
		pData->preSeriesId[2] = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "TalentSetRepository.dat::Read !");
		pData->maxLevel = tempData.m_U8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "TalentSetRepository.dat::Read !");
		pData->talentSeriesId = tempData.m_U32;

		// 插入天赋树
		TalentSetDataMap::iterator it = mTalentSetDataMap.find(setId);
		if(it == mTalentSetDataMap.end() || !it->second)
		{
			TalentSetData* pSetData = new TalentSetData;
			pSetData->mId = setId;
			pSetData->mBackground = background;
			it = mTalentSetDataMap.insert(it, TalentSetDataMap::value_type(setId, pSetData));
			AssertFatal(it != mTalentSetDataMap.end() && it->second, "");
		}
		AssertFatal(!it->second->mNodes[pData->row][pData->col], "天赋重复");
		it->second->mNodes[pData->row][pData->col] = pData;
	}
}

bool TalentRepository::insert(TalentData* pData)
{
	if(!pData)
		return false;
	mTalentDataMap.insert(TalentDataMap::value_type(pData->mTalentId, pData));
	return true;
}

TalentData* TalentRepository::getTalent(U32 TalentId)
{
	TalentDataMap::iterator itr = mTalentDataMap.find(TalentId);
	if(itr != mTalentDataMap.end())
		return itr->second;
	return NULL;
}

TalentSetData* TalentRepository::getTalentSet(U32 TalentSetId)
{
	TalentSetDataMap::iterator itr = mTalentSetDataMap.find(TalentSetId);
	if(itr != mTalentSetDataMap.end())
		return itr->second;
	return NULL;
}

// ========================================================================================================================================
//  TalentTable
// ========================================================================================================================================
TalentRepository g_TalentRepository;

TalentTable::TalentTable()
{
	mPlayer = NULL;
	clear();
}

TalentTable::~TalentTable()
{
	clear();
}

void TalentTable::init(Player* player)
{
	mPlayer = player;
	clear();
	AssertFatal(mPlayer, "TalentTable::init");
}

void TalentTable::clear()
{
	tActiveMap::iterator Active_itr = mActiveMap.begin();
	while (Active_itr != mActiveMap.end())
	{
		U32 talentId = Macro_GetTalentId(Active_itr->first, Active_itr->second);
		removeTalent(talentId);
		Active_itr = mActiveMap.begin();
	}

	mActiveMap.clear();
	tTable::iterator itr = mTable.begin();
	while (itr != mTable.end())
	{
		SAFE_DELETE(itr->second);
		++itr;
	}
	mTable.clear();
}

TalentTable::IdList* TalentTable::getIdList(U32 originId)
{
	tTable::iterator itr = mTable.find(originId);
	if(itr != mTable.end())
		return itr->second;
	return NULL;
}

bool TalentTable::addTalent(U32 talentId)
{
	TalentData* pData = g_TalentRepository.getTalent(talentId);
	if(!pData || !mPlayer)
		return false;

	// 已经有该系的天赋
	tActiveMap::iterator Active_itr = mActiveMap.find(pData->mSeriesId);
	if(Active_itr != mActiveMap.end())
	{
		// 相同等级，直接返回
		if(pData->mLevel == Active_itr->second)
			return true;
		// 不同等级，先移除之
		removeTalent(talentId);
	}

	mActiveMap.insert(tActiveMap::value_type(pData->mSeriesId, pData->mLevel));
	for (S32 i=0; i<TalentData::MaxUnits; ++i)
	{
		if(pData->mTalentUnits[i].mGeneralId)
		{
			U32 key = 0;
			switch (pData->mTalentUnits[i].mType)
			{
			case TalentData::TalentType_Stats:
				{
					key = pData->mTalentUnits[i].mType;
					#ifdef NTJ_SERVER
					// 直接增加属性
					mPlayer->AddBuff(Buff::Origin_Base, pData->mTalentUnits[i].mGeneralId, mPlayer);
					#endif
				}
				break;
			case TalentData::TalentType_Skill:
				{
					key = pData->mTalentUnits[i].mOriginId;
					Skill* pSkill = mPlayer->GetSkillTable().GetSkill(key);
					if(pSkill)
						pSkill->AddPlus(pData->mTalentUnits[i].mGeneralId);
				}
				break;
			case TalentData::TalentType_Buff:
				{
					key = pData->mTalentUnits[i].mOriginId;
				}
				break;
			case TalentData::TalentType_NewSkill:
				{
					key = pData->mTalentUnits[i].mType;
					#ifdef NTJ_SERVER
					mPlayer->AddSkill(pData->mTalentUnits[i].mGeneralId, mPlayer->hasInitializEvent());
					#endif
				}
				break;
			case TalentData::TalentType_Arm_A:
			case TalentData::TalentType_Arm_B:
			case TalentData::TalentType_Arm_C:
			case TalentData::TalentType_Arm_D:
			case TalentData::TalentType_Arm_E:
			case TalentData::TalentType_Arm_F:
			case TalentData::TalentType_Arm_G:
			case TalentData::TalentType_Arm_H:
			case TalentData::TalentType_Arm_I:
			case TalentData::TalentType_Arm_J:
				{
					key = pData->mTalentUnits[i].mType;
					#ifdef NTJ_SERVER
					if(key == (mPlayer->getArmStatus() + TalentData::TalentType_Arm_A))
						mPlayer->AddBuff(Buff::Origin_Buff, pData->mTalentUnits[i].mGeneralId, mPlayer);
					#endif
				}
				break;
			default:
				continue;
			}
			IdList* pList = getIdList(key);
			if(!pList)
			{
				pList = new IdList;
				mTable.insert(tTable::value_type(key, pList));
			}
			pList->push_back(pData->mTalentUnits[i].mGeneralId);
		}
	}
#ifdef NTJ_SERVER
	// 发送消息到客户端
	if(mPlayer->getControllingClient() && mPlayer->hasInitializEvent())
	{
		ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_TALENT);
		pEvent->SetIntArgValues(1, 1);
		pEvent->SetInt32ArgValues(1, talentId);
		mPlayer->getControllingClient()->postNetEvent(pEvent);
	}
#endif
	return true;
}

void TalentTable::removeTalent(U32 talentId)
{
	TalentData* pData = g_TalentRepository.getTalent(talentId);
	if(!pData || !mPlayer)
	{
		AssertFatal(false, "TalentTable::removeTalent");
		return;
	}

	tActiveMap::iterator Active_itr = mActiveMap.find(pData->mSeriesId);
	if(Active_itr == mActiveMap.end())
		return;

	mActiveMap.erase(pData->mSeriesId);
	for (S32 i=0; i<TalentData::MaxUnits; ++i)
	{
		if(pData->mTalentUnits[i].mGeneralId)
		{
			U32 key = 0;
			switch (pData->mTalentUnits[i].mType)
			{
			case TalentData::TalentType_Stats:
				{
					key = pData->mTalentUnits[i].mType;
					#ifdef NTJ_SERVER
					// 直接删除属性
					mPlayer->RemoveBuff(pData->mTalentUnits[i].mGeneralId);
					#endif
				}
				break;
			case TalentData::TalentType_Skill:
				{
					key = pData->mTalentUnits[i].mOriginId;
					Skill* pSkill = mPlayer->GetSkillTable().GetSkill(key);
					if(pSkill)
						pSkill->RemovePlus(pData->mTalentUnits[i].mGeneralId);
				}
				break;
			case TalentData::TalentType_Buff:
				{
					key = pData->mTalentUnits[i].mOriginId;
				}
				break;
			case TalentData::TalentType_NewSkill:
				{
					key = pData->mTalentUnits[i].mType;
					#ifdef NTJ_SERVER
					mPlayer->RemoveSkill(pData->mTalentUnits[i].mGeneralId);
					#endif
				}
				break;
			case TalentData::TalentType_Arm_A:
			case TalentData::TalentType_Arm_B:
			case TalentData::TalentType_Arm_C:
			case TalentData::TalentType_Arm_D:
			case TalentData::TalentType_Arm_E:
			case TalentData::TalentType_Arm_F:
			case TalentData::TalentType_Arm_G:
			case TalentData::TalentType_Arm_H:
			case TalentData::TalentType_Arm_I:
			case TalentData::TalentType_Arm_J:
				{
					key = pData->mTalentUnits[i].mType;
					#ifdef NTJ_SERVER
					if(key == (mPlayer->getArmStatus() + TalentData::TalentType_Arm_A))
						mPlayer->RemoveBuffBySrc(pData->mTalentUnits[i].mGeneralId, mPlayer);
					#endif
				}
				break;
			default:
				continue;
			}
			IdList* pList = getIdList(key);
			if(pList)
			{
				for (S32 j=pList->size()-1; j>=0; --j)
					if((*pList)[j] == pData->mTalentUnits[i].mGeneralId)
					{
						pList->erase(j);
						break;
					}
				if(pList->empty())
				{
					delete pList;
					mTable.erase(key);
				}
			}
			else
				AssertFatal(false, "TalentTable::removeTalent");
		}
	}
#ifdef NTJ_SERVER
	// 发送消息到客户端
	if(mPlayer->getControllingClient() && mPlayer->hasInitializEvent())
	{
		ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_TALENT);
		pEvent->SetIntArgValues(1, 0);
		pEvent->SetInt32ArgValues(1, talentId);
		mPlayer->getControllingClient()->postNetEvent(pEvent);
	}
#endif
}

void TalentTable::onArm(U32 arm)
{
#ifdef NTJ_SERVER
	TalentTable::IdList* list = getIdList(TalentData::TalentType_Arm_A + arm);
	if (!list)
		return;
	for (S32 i=list->size()-1; i>=0; --i)
		mPlayer->AddBuff(Buff::Origin_Buff, (*list)[i], mPlayer);
#endif
}

void TalentTable::onUnarm(U32 arm)
{
#ifdef NTJ_SERVER
	TalentTable::IdList* list = getIdList(TalentData::TalentType_Arm_A + arm);
	if (!list)
		return;
	for (S32 i=list->size()-1; i>=0; --i)
		mPlayer->RemoveBuffBySrc((*list)[i], mPlayer);
#endif
}

