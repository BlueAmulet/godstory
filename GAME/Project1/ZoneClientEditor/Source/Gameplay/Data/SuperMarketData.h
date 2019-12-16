//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef SUPERMARKET_DATA_H
#define SUPERMARKET_DATA_H

#pragma once
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#include <hash_map>


class IColumnData;
class SuperMarketRes
{
public:
	
	friend class MarketItemReposity;

	SuperMarketRes(){ dMemset(this, 0, sizeof(SuperMarketRes));};
	~SuperMarketRes(){};

public:
	U32 mMarketGoodsID;				//商品编号
	U32	mMarketType;				//商城类型
	U32 mMarketGoodsType;			//商品大类
	U32	mMarketSubType;				//商品小类
	U32 mMarketItemID;				//物品ID
	U32 mMarketItemPrice;			//商品价格
	U32 mRankIndex;					//排行

public:
	U32 getMarketItemPrice();
};

class MarketItemReposity
{
public:
	typedef stdext::hash_map<U32, SuperMarketRes*>	MarketDataMap;

	MarketItemReposity();
	~MarketItemReposity();

	void read();
	void clear();
	SuperMarketRes* getSuperMarketRes(U32 goodsID);
	bool insert(SuperMarketRes* data);
	
	void searchGoods(U32 markettype, U32 category, U32 sub, Vector<U32>& vec);
	void findMarketItem(U32 marketType, StringTableEntry name, Vector<U32>& vec);

public:
	IColumnData* mColumnData;
	MarketDataMap mDataMap;
};

extern MarketItemReposity* g_MarketItemReposity;

//打包一起买的物品
struct  PackGoods
{
	static const U32 PackMax = 5;
	U32 mPackID;
	U32 mSuitItemID[PackMax];
	//U32 mAbateRate;
};

class packGoodsResposity
{
public:
	typedef stdext::hash_map<U32, PackGoods*> PackGoodsMap;
 	packGoodsResposity();
	~packGoodsResposity();
	
	void read();
	void clear();

	PackGoods* getPackGoods(U32 itemID);
	bool insert(PackGoods* goods);
public:
	IColumnData* mColumnData;
	PackGoodsMap mPackGoodsMap;

};
extern packGoodsResposity* g_PackGoodsResposity;
#endif