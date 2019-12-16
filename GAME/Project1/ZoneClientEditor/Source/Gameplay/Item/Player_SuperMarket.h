//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef SUPERMARKET_MANAGER_H
#define SUPERMARKET_MANAGER_H

#define  SUB(a,b)  a * 100 + b
#define  GETSUB(a) a%100
//商城管理类
enum enMarketType
{
	MarketType_ImortalStone = 1,		//仙石商城
	MarketType_DeityStone,				//神石商城
};

enum enMarketResType
{
	MARKETCATEGORY_HOTSELL		= 1,				//热卖城
	MARKETCATEGORY_FASTION		= 2,				//时装店
	MARKETCATEGORY_HORSE		= 3,				//御马监
	MARKETCATEGORY_PET			= 4,				//珍兽屋
	MARKETCATEGORY_MEDICINE		= 5,				//药材铺
	MARKETCATEGORY_BOOK			= 6,				//藏经阁
	MARKETCATEGORY_GEM			= 7,				//珠宝
	MARKETCATEGORY_MATERIAL		= 8,				//材料铺
	MARKETCATEGORY_COSTFUL		= 9,				//奇珍阁

	MARKETCATEGORY_SPECIAL			= SUB(MARKETCATEGORY_HOTSELL, 1),	//特价
	MARKETCATEGORY_HOT				= SUB(MARKETCATEGORY_HOTSELL, 2),	//热卖

	MARKETCATEGORY_FAMILY			= SUB(MARKETCATEGORY_FASTION, 1),	//门宗时装
	MARKETCATEGORY_COMMON			= SUB(MARKETCATEGORY_FASTION, 2),	//普通时装
	MARKETCATEGORY_FASTIONMATERIAL	= SUB(MARKETCATEGORY_FASTION, 3),	//时装材料
	MARKETCATEGORY_FATIONBOOK		= SUB(MARKETCATEGORY_FASTION, 4),	//时装配方

	MARKETCATEGORY_HORSE_FAMILY		= SUB(MARKETCATEGORY_HORSE, 1),		//门宗坐骑
	MARKETCATEGORY_HORSE_COMMON		= SUB(MARKETCATEGORY_HORSE, 2),		//普通坐骑
	MARKETCATEGORY_HORSE_BOOK		= SUB(MARKETCATEGORY_HORSE, 3),		//坐骑技能书

	MARKETCATEGORY_PET_SEAL			= SUB(MARKETCATEGORY_PET, 1),		//珍兽封印
	MARKETCATEGORY_PET_SKILLBOOK	= SUB(MARKETCATEGORY_PET, 2),		//珍兽技能书
	MARKETCATEGORY_PET_FOOD			= SUB(MARKETCATEGORY_PET, 3),		//珍兽食物
	MARKETCATEGORY_PET_DRUG			= SUB(MARKETCATEGORY_PET, 4),		//珍兽药品
	MARKETCATEGORY_PET_REBACK		= SUB(MARKETCATEGORY_PET, 5),		//珍兽洗点

	MARKETCATEGORY_DRUG_STATE		= SUB(MARKETCATEGORY_MEDICINE, 1),	//状态药
	MARKETCATEGORY_DRUG_CAPACITY	= SUB(MARKETCATEGORY_MEDICINE, 2),	//容量药

	MARKETCATEGORY_BOOK_SKILL		= SUB(MARKETCATEGORY_BOOK, 1),		//技能书	
	MARKETCATEGORY_BOOK_FOUND		= SUB(MARKETCATEGORY_BOOK, 2),		//铸造配方
	MARKETCATEGORY_BOOK_SEWING		= SUB(MARKETCATEGORY_BOOK, 3),		//裁缝配方
	MARKETCATEGORY_BOOK_ART			= SUB(MARKETCATEGORY_BOOK, 4),		//工艺配方

	MARKETCATEGORY_GEM_STONE		= SUB(MARKETCATEGORY_GEM, 1),		//宝石

	MARKETCATEGORY_MATERIAL_IDETIFY	= SUB(MARKETCATEGORY_MATERIAL, 1),	//鉴定材料
	MARKETCATEGORY_MATERIAL_STRONG	= SUB(MARKETCATEGORY_MATERIAL, 2),	//强化材料
	MARKETCATEGORY_MATERIAL_BESET	= SUB(MARKETCATEGORY_MATERIAL, 3),	//镶嵌材料
	MARKETCATEGORY_MATERIAL_LIFE	= SUB(MARKETCATEGORY_MATERIAL, 4),	//生活材料

	MARKETCATEGORY_COSTFUL_EXP		= SUB(MARKETCATEGORY_COSTFUL, 1),	//经验丹
	MARKETCATEGORY_COSTFUL_BODY		= SUB(MARKETCATEGORY_COSTFUL, 2),	//变身卡
	MARKETCATEGORY_COSTFUL_FIRE		= SUB(MARKETCATEGORY_COSTFUL, 3),	//烟花
	MARKETCATEGORY_COSTFUL_FLOWER	= SUB(MARKETCATEGORY_COSTFUL, 4),	//鲜花
	MARKETCATEGORY_COSTFUL_OTHER	= SUB(MARKETCATEGORY_COSTFUL, 5),	//其他

};

//活动促销数据结构
struct stPromotion
{
	U32 id;				//流水号
	U32 begindate;		//开始日期时间
	U32 enddate;		//结束日期时间
	U32 mode;			//消费模式(0-无 1-仙石 2-神石)
	U32 money;			//消费数额
	U32 condition[2][2];//判定条件
	U32 reward[4][2];	//赠送物品
};

//折扣数据结构
struct stDiscount
{
	U32 id;				//流水号
	U32 goodid;			//商品编号
	U32 type;			//0-折扣 1-特价
	U32 value;			//折扣率或特价定值
	U32 begindate;		//开始日期时间
	U32 enddate;		//结束日期时间
	S32	isoffsell;		//是否结束后下架
	S32 goodcount;		//上架数量(-1为无限制)
	U32 reward[2][2];	//赠送物品
};

//交易记录数据结构
struct stMarketTrade
{
	U32 id;				//流水号
	U32 tradeid;		//交易批次号
	U32 accountid;		//帐号ID
	U32 playerid;		//角色ID
	U8	playername[32];//角色名称
	U32 mode;			//消费模式(0-空 1-仙石 2-神石)
	U32 money;			//仙石数值或神石数值
	U32 goodid;			//商品编号
	U32 goodnum;		//商品数量
	U32 promotionid;	//活动促销数据流水号
	U32 discountid;		//折扣数据流水号
	U32 tradedate;		//交易发生时间
};

class SuperMarketRes;
class Player;
class SuperMarketManager
{
public:
	SuperMarketManager();
	~SuperMarketManager();
	//打开商城界面
	void openSuperMarket(Player* player);
	
#ifdef NTJ_CLIENT
	void filterMarketGoods();
	U32 getTempListSize() { return mTempGoodsList.size(); }
	U32 getMarketGoodID(S32 index);
	SuperMarketRes* getMarketRes(U32 id);
	S32 getSuperMarketData(Player* player, S32 page);
	void setMarkettype (U32 type) { mMarketType = type; }
	void showMarketItemInfo(Player* pPlayer);
	void setMarketBigType(U32 type) { mBigType = type; }
	void setMarketSmallType(U32 type) { mSmallType = type; }
	U32  getCurrentPage() { return mCurrentPage; }
	void setCurrentPage(U32 page) { mCurrentPage = page; }
	U32  getAllPage() { return mAllPage; }
	void setAllPage(U32 page) { mAllPage = page; }
	U32 getMaketType()	{ return mMarketType; }
	void findMarketItem(StringTableEntry name);
	void showFindMarketItem(U32 type);
	void addItemToBasketballList(U32 goodsId, U32 num);
	void showBasketballListInfo();	
	void setBasketListSlot(Player* pPlayer);
	void showBasketItemInfo(Player* pPlayer);
#endif

private:
	U32 mMarketType;		// 商城类型
	U32	mBigType;			// 商品大类
	U32 mSmallType;			// 商品小类
	U32 mCurrentPage;
	S32 mAllPage;
public:
	Vector<U32> mTempGoodsList;			//用于显示物品
	Vector<U32> mFindImortalStoneList;	//仙石搜索
	Vector<U32> mFindDeityStoneList;	//神石搜索
	Vector<U32> mImortalBasketballList;	//仙石购物篮
	Vector<U32> mDeityBasketballList;	//神石购物篮
	Vector<U32>	mImortalStoneNum;		//购买仙石数量
	Vector<U32> mDeityStoneNum;		//购买神石数量
};

extern SuperMarketManager* g_MarketManager;
#endif