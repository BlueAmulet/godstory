//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "core/tVector.h"
#include <hash_map>

#define IDS_SEPERATOR_CHARACTER (' ')

enum TYPE_ID
{
	TYPE_ID_ITEM = 0,
	TYPE_ID_PACKAGE = 1
};

struct ItemProbobilityInfo
{
	ItemProbobilityInfo () {}
	ItemProbobilityInfo(U32 nID, F32 fProbability) : m_nId(nID), m_fProbability(fProbability) {}

	U32 m_nId;
	F32 m_fProbability;
};

struct ItemPackage
{
	Vector<ItemProbobilityInfo> m_itemsInfo;
	U32 m_ItemPackageID;
};

struct TempInfo
{
	TempInfo() : m_tmpPackageID(0) { }
	TempInfo(U32 nPackageID) : m_tmpPackageID(nPackageID) { }

	Vector<ItemProbobilityInfo> m_tmpItems;
	Vector<ItemProbobilityInfo> m_tmpPackages;
	U32 m_tmpPackageID;
};

class ItemPackageRepository
{
	typedef stdext::hash_map<U32, ItemPackage*> ItemPackageMap;
	typedef Vector<TempInfo*> TempInfoVec;

public:
	ItemPackageRepository();
	~ItemPackageRepository();
	void read();
	void clear();
	ItemPackage* getPackage(U32 nItemPackageID);
	TempInfo* getTempInfo(U32 nItemPackageID);
	bool insert(ItemPackage* package);
	
private:
	void onReadFinshed();
	void processTempInfo(TempInfo *pTempInfo);
	void AssertErrorMsg(bool bAssert, const char *msg, int rowNum);
private:
	TempInfoVec m_tempVector;
	ItemPackageMap mItemPackageMap;
};

extern ItemPackageRepository g_ItemPackageRepository;