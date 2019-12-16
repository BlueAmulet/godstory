//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include <iostream>
#include <fstream>
using namespace std;
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/Item/ItemPackage.h"



ItemPackageRepository g_ItemPackageRepository;

ItemPackageRepository::ItemPackageRepository()
{
	VECTOR_SET_ASSOCIATION(m_tempVector);
}
ItemPackageRepository::~ItemPackageRepository()
{
	clear();
}

void ItemPackageRepository::clear()
{
	for(ItemPackageMap::iterator it = mItemPackageMap.begin(); it != mItemPackageMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	mItemPackageMap.clear();
}

void ItemPackageRepository::read()
{
	CDataFile op;
	RData tempdata;
	char filename[256];
	ItemPackage* pPackageData = NULL;
	TempInfo *pTempInfo;
	U32 nSubType;
	U32 nID;

	Platform::makeFullPathName(GAME_ITEMPACKAGE_FILE, filename, sizeof(filename));
	op.readDataFile(filename);

	for(int i=0; i<op.RecordNum; ++i)
	{
		// read package ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "ItemPackageRepository.dat::Read - Failed to read packageID", i);

		pTempInfo = getTempInfo(tempdata.m_U32);

		if (pTempInfo == NULL)
		{
			pTempInfo = new TempInfo(tempdata.m_U32);
			m_tempVector.push_back(pTempInfo);
		}

		// read sub type
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U8, "ItemPackageRepository.dat::Read - Failed to read sub type", i);
		AssertErrorMsg( (tempdata.m_U8 == TYPE_ID_PACKAGE || tempdata.m_U8 ==TYPE_ID_ITEM), 
			"ItemPackageRepository.dat::Read - Failed to read sub type(type is neither PACKAGE OR ITEM)", i);
		nSubType = tempdata.m_U8;

		// read sub ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "ItemPackageRepository.dat::Read - Failed to read sub ID", i);
		nID = tempdata.m_U32;

		// read probability for sub-item or sub-package
		if (nSubType == TYPE_ID_PACKAGE)
		{
			AssertErrorMsg(getTempInfo(tempdata.m_U32) != NULL, "ItemPackageRepository.dat::Read - Failed to read sub package(package not registered)", i);
			// read sub-packages probability
			op.GetData(tempdata);
			AssertErrorMsg(tempdata.m_Type == DType_F32, "ItemPackageRepository.dat::Read - Failed to read sub package probability", i);
			pTempInfo->m_tmpPackages.push_back(ItemProbobilityInfo(nID, tempdata.m_F32 / 10000));
		}
		else  // nSubType == TYPE_ID_ITEM
		{
			op.GetData(tempdata);
			AssertErrorMsg(tempdata.m_Type == DType_F32, "ItemPackageRepository.dat::Read - Failed to read sub item probability", i);
			pTempInfo->m_tmpItems.push_back(ItemProbobilityInfo(nID, tempdata.m_F32 / 10000));
		}
	}

	onReadFinshed();
	
}

void ItemPackageRepository::onReadFinshed()
{
	TempInfoVec::iterator it = m_tempVector.begin();

	while(it != m_tempVector.end())
	{
		processTempInfo(*it);
		++it;
	}

	// clear temp map
    for (int i = 0; i < m_tempVector.size(); ++i)
    {
        delete m_tempVector[i];
    }

	m_tempVector.clear();
}

void ItemPackageRepository::processTempInfo(TempInfo *pTempInfo)
{
	if (pTempInfo == NULL)
		return;

	Vector<ItemProbobilityInfo>::iterator itOfTempInfo;
	ItemPackage *pItemPackage = new ItemPackage;
	pItemPackage->m_ItemPackageID = pTempInfo->m_tmpPackageID;
	F32 fProbablity;

	// process items
	if(!pTempInfo->m_tmpItems.empty())
	{
		itOfTempInfo = pTempInfo->m_tmpItems.begin();

		while(itOfTempInfo != pTempInfo->m_tmpItems.end())
		{
			pItemPackage->m_itemsInfo.push_back(*itOfTempInfo);
			++itOfTempInfo;
		}
	}

	// process packages
	if (!pTempInfo->m_tmpPackages.empty())
	{
		itOfTempInfo = pTempInfo->m_tmpPackages.begin();
		
		while(itOfTempInfo != pTempInfo->m_tmpPackages.end())
		{
			ItemPackage *pPackageInMap = getPackage(itOfTempInfo->m_nId); // mItemPackageMap.find(itOfTempInfo->m_nId);
			Vector<ItemProbobilityInfo>::iterator it = pPackageInMap->m_itemsInfo.begin();
			while(it != pPackageInMap->m_itemsInfo.end())
			{
				fProbablity = itOfTempInfo->m_fProbability * it->m_fProbability;
				pItemPackage->m_itemsInfo.push_back(ItemProbobilityInfo(it->m_nId, fProbablity));
				++it;
			}

			++itOfTempInfo;
		}
	}

	insert(pItemPackage);
}

void ItemPackageRepository::AssertErrorMsg(bool bAssert, const char *msg, int rowNum)
{
	char buf[256];

	if (!bAssert)
	{
		dSprintf(buf, sizeof(buf), "RowNum: %d  ", rowNum);
		dStrcat(buf, sizeof(buf), msg);
		AssertRelease(false, buf);
	}
}

TempInfo* ItemPackageRepository::getTempInfo(U32 nItemPackageID)
{
	TempInfoVec::iterator it = m_tempVector.begin();
	while(it != m_tempVector.end())
	{
		if ((*it)->m_tmpPackageID == nItemPackageID)
		{
			return *it;
		}

		++it;
	}

	return NULL;
}

ItemPackage* ItemPackageRepository::getPackage(U32 nItemPackageID)
{
	ItemPackageMap::iterator it = mItemPackageMap.find(nItemPackageID);
	if(it != mItemPackageMap.end())
		return it->second;
	else
		return NULL;
}

bool ItemPackageRepository::insert(ItemPackage* package)
{
	if(!package)
		return false;

	mItemPackageMap.insert(ItemPackageMap::value_type(package->m_ItemPackageID, package));
	return true;
}