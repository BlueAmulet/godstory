//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Data/SuperMarketData.h"
#include "Util/ColumnData.h"
#include "core/tVector.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/Item/Player_SuperMarket.h"
#include "gameplay/Item/ItemShortcut.h"
#include "Gameplay/Item/Res.h"


U32 SuperMarketRes::getMarketItemPrice()
{
	return mMarketItemPrice;
}

MarketItemReposity g_MarketResposity;
MarketItemReposity* g_MarketItemReposity = NULL;

MarketItemReposity::MarketItemReposity()
{
	mColumnData = NULL;
	g_MarketItemReposity = this;	
}

MarketItemReposity::~MarketItemReposity()
{
	clear();
	if(mColumnData)
		delete mColumnData;
}

void MarketItemReposity::clear()
{
	for(MarketDataMap::iterator it = mDataMap.begin(); it != mDataMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	mDataMap.clear();
}

void MarketItemReposity::read()
{
	CDataFile op;
	RData tempdata;
	op.ReadDataInit();
	char filename[1024];
	Platform::makeFullPathName(GAME_MARKETDATA_FILE, filename, sizeof(filename));
	AssertRelease(op.readDataFile(filename), "can't read file : SuperMarketRepository.dat!");
	mColumnData = new IColumnData(op.ColumNum, "SuperMarketRepository.dat");
	mColumnData->setField(CalcOffset(mMarketGoodsID,	SuperMarketRes), DType_U32, "商品编号");
	mColumnData->setField(CalcOffset(mMarketType,		SuperMarketRes), DType_enum8,	"商城类型");
	mColumnData->setField(CalcOffset(mMarketGoodsType,	SuperMarketRes), DType_enum8,	"商品大类");
	mColumnData->setField(CalcOffset(mMarketSubType,	SuperMarketRes), DType_enum16,	"商品小类");
	mColumnData->setField(CalcOffset(mMarketItemID,		SuperMarketRes), DType_U32, "物品ID");
	mColumnData->setField(CalcOffset(mMarketItemPrice,	SuperMarketRes), DType_U32, "商品价格");
	mColumnData->setField(CalcOffset(mRankIndex,		SuperMarketRes), DType_U32,	"商品排行榜");

	for(int i = 0; i < op.RecordNum; ++i)
	{
		SuperMarketRes* pItempdata = new SuperMarketRes;
		for(int h = 0; h < op.ColumNum; ++h)
		{
			op.GetData(tempdata);
			mColumnData->setData(pItempdata, h, tempdata);
		}
		insert(pItempdata);
	}
	op.ReadDataClose();
}

SuperMarketRes* MarketItemReposity::getSuperMarketRes(U32 goodsID)
{
	MarketDataMap::iterator it = mDataMap.find(goodsID);
	if(it != mDataMap.end())
		return it->second;
	else
		return NULL;
}

bool MarketItemReposity::insert(SuperMarketRes* data)
{
	if(!data)
		return false;
	mDataMap.insert(MarketDataMap::value_type(data->mMarketGoodsID, data));
	return true;
}

void MarketItemReposity::searchGoods(U32 markettype, U32 category, U32 sub, Vector<U32>& vec)
{
	vec.clear();
	SuperMarketRes* marketRes = NULL;
	MarketDataMap::iterator it = mDataMap.begin();
	for(; it != mDataMap.end(); ++it)
	{
		marketRes = (it->second);
		if(marketRes)
		{
			if((marketRes->mMarketType == markettype) && (marketRes->mMarketGoodsType == category) && (GETSUB(marketRes->mMarketSubType) == sub))
				vec.push_back(marketRes->mMarketGoodsID);
		}
	}
}

void MarketItemReposity::findMarketItem(U32 marketType, StringTableEntry name, Vector<U32>& vec)
{
	vec.clear();
	SuperMarketRes* marketRes = NULL;
	MarketDataMap::iterator it = mDataMap.begin();
	for(; it != mDataMap.end(); ++it)
	{
		marketRes = (it->second);
		if(marketRes)
		{
			if(marketRes->mMarketType == marketType)
			{
				ItemShortcut* pItem = ItemShortcut::CreateItem(marketRes->mMarketItemID, 1);
				if(pItem)
				{
					StringTableEntry target = dStrstr(pItem->getRes()->getItemName(), name);
					if(target != NULL)
						vec.push_back(marketRes->mMarketGoodsID);
				}
				delete pItem;
			}
		}
	}
}

//打包卖的商品

packGoodsResposity g_PackResposity;
packGoodsResposity* g_PackGoodsResposity = NULL;
packGoodsResposity::packGoodsResposity()
{
	mColumnData = NULL;
	g_PackGoodsResposity = this;
}

packGoodsResposity::~packGoodsResposity()
{
	clear();
	if(mColumnData)
		delete mColumnData;
}

void packGoodsResposity::read()
{
	CDataFile op;
	RData tempdata;
	op.ReadDataInit();
	char filename[1024];
	Platform::makeFullPathName(GAME_PACKGOODS_FILE, filename, sizeof(filename));
	AssertRelease(op.readDataFile(filename), "can't read file : PackGoods.dat!");
	mColumnData = new IColumnData(op.ColumNum, "PackGoods.dat");

	mColumnData->setField(CalcOffset(mPackID,	PackGoods), DType_U32, "商品编号");
	for(int i=0; i<PackGoods::PackMax; ++i)
	{
		mColumnData->setField(CalcOffset(mSuitItemID[i], PackGoods), DType_U32, "推荐商品编号");
	}

	for(int i = 0; i < op.RecordNum; ++i)
	{
		PackGoods* pPackGoods = new PackGoods;
		for(int h = 0; h < op.ColumNum; ++h)
		{
			op.GetData(tempdata);
			mColumnData->setData(pPackGoods, h, tempdata);
		}
		insert(pPackGoods);
	}
	op.ReadDataClose();
}

void packGoodsResposity::clear()
{
	for(PackGoodsMap::iterator it = mPackGoodsMap.begin(); it != mPackGoodsMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	mPackGoodsMap.clear();
}

PackGoods* packGoodsResposity::getPackGoods(U32 itemID)
{
	PackGoodsMap::iterator it = mPackGoodsMap.find(itemID);
	if( it != mPackGoodsMap.end())
		return it->second;
	else
		return NULL;
}

bool packGoodsResposity::insert(PackGoods* goods)
{
	if(!goods)
		return false;
	mPackGoodsMap.insert(PackGoodsMap::value_type(goods->mPackID, goods));
	return true;
}