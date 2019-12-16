#ifndef __NPCSHOPDATA_H__
#define __NPCSHOPDATA_H__

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#pragma once
#include <hash_map>

class NpcShopList;
class ItemShortcut;
// 商品列表结构
struct NpcShopItem
{
public:
	U32 GoodID;				// 商品编号
	U32 ShopID;				// 商品列表ID
	U32 ItemID;				// 物品数据模板ID
	U32 RandGroup;			// 随机物品组号
	U32 ShowByLockTime;		// 库存冻结时间表现类型(0-任何时间内都显示 1-仅库存冻结时间结束时显示)
	U32 CurrencyID;			// 货币类型(参见Res.h的SaleType枚举)
	U32 CurrencyValue;		// 货币数量
	U32 SellRate;			// 售率
	U32 ConditionID;		// 条件ID
	StringTableEntry ConditionValue;	// 条件值
	S32 StockNum;			// 库存数（0为无限)
	U32 StockInterval;		// 库存刷新间隔时间（以分钟为单位）
	U32 LockTime;			// 库存冻结时间(以分钟为单位)
	U32 UpStockTime;		// 上架时间
	U32 DownStockTime;		// 下架时间
	ItemShortcut* pItem;	// 物品实例对象

	// ------------------------------------------------------------
	// 商品列表更新
	bool IsRandHit;			// 是否随机命中(只在随机物品组号不为0时此值才有效)
	U32  LastUpdateTime;	// 上次刷新时间(只在库存刷新间隔时间不为0时此值才有效)
	U32  CurrentStockNum;	// 当前库存数(只在库存数不为0时有效，递减到0为止)

	NpcShopItem() { memset(this, 0, sizeof(NpcShopItem));}
	// 判断当前物品是否在创建物品列表时被随机命中生成
	bool IsPutInList() { return (RandGroup==0 || (RandGroup !=0 && IsRandHit));}
	// 是否库存递减到空了
	bool IsEmpty() { return (StockNum != 0 && CurrentStockNum == 0);}	
	bool IsInTime(U32 hourmin = 0);
	// 是否真实存在
	bool IsExist(U32 hourmin = 0) { return IsPutInList() && !IsEmpty() && IsInTime(hourmin);}
	// 是否需要库存刷新
	bool IsRefresh(U32 curtime = 0) { return StockInterval? LastUpdateTime + StockInterval <= curtime : false;}
	// 是否能显示给玩家看 
	bool IsShow(U32 curtime = 0) { return ShowByLockTime? LastUpdateTime + LockTime <= curtime : true;}
};

class Player;
class ShopListRepository
{
public:
	static const U32 UPDATEINTERVAL = 30;	//每隔30秒检查更新一次
	typedef Vector<NpcShopItem*> NpcShopData;
	typedef stdext::hash_map<U32, NpcShopData*> NpcShopMap;
	ShopListRepository();
	~ShopListRepository();
	void read();
	void clear();
	NpcShopData* getData(U32 shopid);
	bool insert(U32, NpcShopData* data);
	bool reset(U32 shopid, U32 randgroupid);	

#ifdef NTJ_SERVER
	bool update(U32 curTime);
	U32 getShopData(U32 shopid, NpcShopList* shoplist, S32 page);
	//根据条件筛选
	U32 getShopData(Player* player, U32 shopid, NpcShopList* shoplist, S32 page, S32 id);
#endif//NTJ_SERVER

private:
	NpcShopMap mDataMap;
	U32 mLastUpdateTime;
};

extern ShopListRepository* g_ShopListRepository;

#endif//__NPCSHOPDATA_H__