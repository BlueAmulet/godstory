#include "Gameplay/item/NpcShopData.h"
#include "core/stringTable.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/item/Player_Item.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameObjects/PlayerObject.h"

ShopListRepository gShopListRepository;
ShopListRepository* g_ShopListRepository = NULL;

// ----------------------------------------------------------------------------
// 是否在上架时间与下架时间内
bool NpcShopItem::IsInTime(U32 hourmin /* = 0 */)
{
	if(hourmin == 0)
	{
		Platform::LocalTime localTime;
		Platform::getLocalTime(localTime);
		hourmin = localTime.hour * 60 + localTime.min;
	}
	return ((UpStockTime == 0 && DownStockTime ==0) || 
		(UpStockTime <= hourmin && DownStockTime>= hourmin));
}

ShopListRepository::ShopListRepository()
{
	g_ShopListRepository = this;
	mLastUpdateTime = 0;
}

ShopListRepository::~ShopListRepository()
{
	clear();
}

// ----------------------------------------------------------------------------
// 商店列表数据清除
void ShopListRepository::clear()
{
	for(NpcShopMap::iterator it = mDataMap.begin(); it != mDataMap.end(); ++it)
	{
		NpcShopData* pData = it->second;
		if(pData == NULL)
			continue;
		for(size_t i = 0; i < pData->size(); i++)
			if((*pData)[i] != NULL)
				delete (*pData)[i];
		delete pData;
	}
	mDataMap.clear();
}

typedef stdext::hash_multimap<U32, U32> RandGroupMap;

bool QFind(RandGroupMap& tempMap, U32 shopid, U32 randgroup)
{
	for(RandGroupMap::iterator itB = tempMap.lower_bound(shopid); itB != tempMap.upper_bound(shopid); ++itB)
	{
		if(itB->second == randgroup)
			return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
// 商店列表数据读取方法
void ShopListRepository::read()
{
	CDataFile op;
	RData tempdata;
	op.ReadDataInit();
	char filename[256];
	U32 GOODID = 0;
	Platform::makeFullPathName(GAME_SHOPLIST_FILE, filename, sizeof(filename));
	op.readDataFile(filename);	
	RandGroupMap tmpGroupMap;
	U32 currenttime = Platform::getTime();
	for(int i = 0; i < op.RecordNum; ++i)
	{
		NpcShopItem* pNpcShopItem = new NpcShopItem;
		//商品列表ID
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read ShopID!");
		pNpcShopItem->ShopID = tempdata.m_U32;
		//物品ID
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read ItemID!");
		pNpcShopItem->ItemID = tempdata.m_U32;		

		//随机物品组
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U8, "shoplist.dat::Read - failed to read RandGroup!");
		pNpcShopItem->RandGroup = tempdata.m_U8;
		//货币类型
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read CurrencyID!");
		pNpcShopItem->CurrencyID = tempdata.m_U32;
		//售价
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read CurrencyValue!");
		pNpcShopItem->CurrencyValue = tempdata.m_U32;
		//售率
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_F32, "shoplist.dat::Read - failed to read SellRate!");
		pNpcShopItem->SellRate = tempdata.m_F32;
		//条件ID
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read ConditionID!");
		pNpcShopItem->ConditionID = tempdata.m_U32;
		//条件值
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_string, "shoplist.dat::Read - failed to read ConditionValue!");
		pNpcShopItem->ConditionValue = StringTable->insert(tempdata.m_string);
		//库存数
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U16, "shoplist.dat::Read - failed to read StockNum!");
		pNpcShopItem->StockNum = tempdata.m_U16;
		
		//库存刷新时间
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read StockInterval!");
		pNpcShopItem->StockInterval = tempdata.m_U32 * 60;
		//库存冻结时间
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read LockTime!");
		pNpcShopItem->LockTime = tempdata.m_U32 * 60;
		//库存冻结时间表现类型
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_enum8, "shoplist.dat::Read - failed to read RandomType!");
		pNpcShopItem->ShowByLockTime = tempdata.m_Enum8;
		//上架时间
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read UpStockTime!");
		pNpcShopItem->UpStockTime = tempdata.m_U32;
		//下架时间
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "shoplist.dat::Read - failed to read DownStockTime!");
		pNpcShopItem->DownStockTime = tempdata.m_U32;

		// 若在物品仓库无法找到此物品，则忽略
		if(g_ItemRepository->getItemData(pNpcShopItem->ItemID) == NULL)
		{
			Con::warnf("shoplist.dat有物品仓库里没有的物品[id = %d]", pNpcShopItem->ItemID);
			delete pNpcShopItem;
			continue;
		}

		// 快速判定是否添加到临时随机组列表里
		if(QFind(tmpGroupMap, pNpcShopItem->ShopID, pNpcShopItem->RandGroup) == false)
			tmpGroupMap.insert(RandGroupMap::value_type(pNpcShopItem->ShopID, pNpcShopItem->RandGroup));

		// 判定当前物品是否随机物品
		if(pNpcShopItem->RandGroup == 0)
		{
			pNpcShopItem->LastUpdateTime = currenttime;
			pNpcShopItem->CurrentStockNum = pNpcShopItem->StockNum;
		}

		pNpcShopItem->GoodID = ++GOODID;
		
		NpcShopData* shoplist = g_ShopListRepository->getData(pNpcShopItem->ShopID);
		if(shoplist)
		{
			shoplist->push_back(pNpcShopItem);
		}
		else
		{
			shoplist = new NpcShopData;						
			shoplist->push_back(pNpcShopItem);
			insert(pNpcShopItem->ShopID, shoplist);
		}
	}
	op.ReadDataClose();

	// 遍历所有随机组物品进行随机设定
	for(RandGroupMap::iterator itB = tmpGroupMap.begin(); itB != tmpGroupMap.end(); ++itB)
	{
		reset(itB->first, itB->second);
	}
}

// ----------------------------------------------------------------------------
// 根据商店列表ID获取数据
ShopListRepository::NpcShopData* ShopListRepository::getData(U32 shopid)
{
	NpcShopMap::iterator it = mDataMap.find(shopid);
	if(it != mDataMap.end())
		return it->second;
	else
		return NULL;
}

// ----------------------------------------------------------------------------
// 添加一个商店列表数据
bool ShopListRepository::insert(U32 shopid, NpcShopData* data)
{
	if(!data)
		return false;
	mDataMap.insert(NpcShopMap::value_type(shopid, data));
	return true;
}

// ----------------------------------------------------------------------------
// 重置随机组物品
bool ShopListRepository::reset(U32 shopid, U32 randgroupid)
{
	NpcShopData* pData = getData(shopid);
	if(pData == NULL)
		return false;
	Vector<U32> randGroup;
	for(size_t i = 0; i < pData->size(); i++)
	{
		NpcShopItem* pItem = (*pData)[i];
		if(pItem && pItem->RandGroup == randgroupid)
		{
			randGroup.push_back(i);
			pItem->IsRandHit = false;
		}
	}

	if(randGroup.empty())
		return false;

	S32 randval = Platform::getRandomI(0, randGroup.size()-1);
	NpcShopItem* pItem = (*pData)[randval];
	if(pItem == NULL)
		return false;
	pItem->IsRandHit = true;
	pItem->LastUpdateTime = Platform::getTime();
	pItem->CurrentStockNum = pItem->StockNum;
	return true;
}

#ifdef NTJ_SERVER
// ----------------------------------------------------------------------------
// 刷新商品列表
bool ShopListRepository::update(U32 curTime)
{
	if(curTime <= mLastUpdateTime + UPDATEINTERVAL)
		return false;

	for(NpcShopMap::iterator it = mDataMap.begin(); it != mDataMap.end(); ++it)
	{
		NpcShopData* pData = it->second;
		if(pData == NULL)
			continue;
		for(size_t i = 0; i < pData->size(); i++)
		{
			NpcShopItem* pItem = (*pData)[i];
			if(pItem == NULL)
				continue;
			// 忽略没有随机命中的物品
			if(pItem->RandGroup != 0 && !pItem->IsRandHit)
				continue;

			// 判断库存刷新时间是否到
			if(pItem->IsRefresh(curTime))
			{
				// 当随机命中的物品库存递减到空时，需要重新随机
				if(pItem->RandGroup != 0 && pItem->IsEmpty())
					reset(pItem->ShopID, pItem->RandGroup);
				pItem->CurrentStockNum = pItem->StockNum;
				pItem->LastUpdateTime = curTime;
			}
		}
	}
	mLastUpdateTime = curTime;
	return true;
}

// ----------------------------------------------------------------------------
// 获取玩家真实可见的商品列表数据
U32 ShopListRepository::getShopData(U32 shopid, NpcShopList* shoplist, S32 page)
{
	NpcShopData* pData = getData(shopid);
	if(pData == NULL)
		return 0;
	Platform::LocalTime localTime;
	Platform::getLocalTime(localTime);
	U32 curLocalTime = localTime.hour * 60 + localTime.min;
	U32 Pos = 0, SumPos = 0;
	for(size_t i = 0; i < pData->size(); i++)
	{
		NpcShopItem* pShopItem = (*pData)[i];
		// 判断物品对于玩家是否真实被放入商品列表里
		if(pShopItem && pShopItem->IsExist(curLocalTime))
		{
			if(SumPos < ((page + 1) * NpcShopList::MAXSLOTS) && SumPos >= page * NpcShopList::MAXSLOTS)
			{
				pShopItem->pItem = ItemShortcut::CreateItem(pShopItem->ItemID, 1);
                shoplist->mSlots[Pos++] = pShopItem;
			}
			SumPos ++;
		}
	}
	return (SumPos % NpcShopList::MAXSLOTS) ? (SumPos / NpcShopList::MAXSLOTS) + 1 : SumPos / NpcShopList::MAXSLOTS ;
}

// ----------------------------------------------------------------------------
// 按条件筛选过滤玩家真实可见的商品列表数据
U32 ShopListRepository::getShopData(Player* player, U32 shopid, NpcShopList* shoplist, S32 page, S32 id)
{
	if(!player)
		return 0;
	NpcShopData* pData = getData(shopid);
	if(pData == NULL)
		return 0;
	Platform::LocalTime localTime;
	Platform::getLocalTime(localTime);
	U32 curLocalTime = localTime.hour * 60 + localTime.min;
	U32 Pos = 0, SumPos = 0;

	for(size_t i = 0; i < pData->size(); i++)
	{
		NpcShopItem* pShopItem = (*pData)[i];
		// 判断物品对于玩家是否真实被放入商品列表里		
		if(pShopItem && pShopItem->IsExist(curLocalTime))
		{
			if(id == 2)	//如果条件为"不可用"
			{
				ItemBaseData* pBaseData = g_ItemRepository->getItemData(pShopItem->ItemID);
				if(!pBaseData)
					continue;
				if(!pBaseData->canLevelLimit(player->getLevel()))
					continue;
				//职业判断
				if(!pBaseData->canFamilyLimit(player->getFamily()))
					continue;
				//性别判断
				if(!pBaseData->canSexLimit(player->getSex()))
					continue;
			}

			if(SumPos < ((page + 1) * NpcShopList::MAXSLOTS) && SumPos >= page * NpcShopList::MAXSLOTS)
			{
				pShopItem->pItem = ItemShortcut::CreateItem(pShopItem->ItemID, 1);
				shoplist->mSlots[Pos++] = pShopItem;
			}
			SumPos ++;
		}
	}

	return (SumPos % NpcShopList::MAXSLOTS) ? (SumPos / NpcShopList::MAXSLOTS) + 1 : SumPos / NpcShopList::MAXSLOTS ;
}

#endif//NTJ_SERVER