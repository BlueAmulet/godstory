//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Player_SuperMarket.h"
#include "Gameplay/Data/SuperMarketData.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/Social/ClientSocial.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameEvents/GameNetEvents.h"

SuperMarketManager g_SuperMaket;
SuperMarketManager* g_MarketManager = NULL;

SuperMarketManager::SuperMarketManager()
{
	mMarketType = MarketType_ImortalStone;
	mBigType = MARKETCATEGORY_HOTSELL;
	mSmallType = GETSUB(MARKETCATEGORY_SPECIAL);
	mCurrentPage = 0;
	mAllPage	= 0;
	g_MarketManager = this;
	mTempGoodsList.clear();
	mFindImortalStoneList.clear();
	mFindDeityStoneList.clear();
	mImortalBasketballList.clear();
	mDeityBasketballList.clear();
	mImortalStoneNum.clear();
	mDeityStoneNum.clear();
}

SuperMarketManager::~SuperMarketManager()
{
	mTempGoodsList.clear();
	mFindImortalStoneList.clear();
	mFindDeityStoneList.clear();
}

void SuperMarketManager::openSuperMarket(Player* player)
{
	if(!player)
		return;
#ifdef NTJ_CLIENT
	filterMarketGoods();
	mAllPage = getSuperMarketData(player, mCurrentPage);
	if(mAllPage < 0)
		return;
	Con::executef("openSuperMarket",Con::getIntArg(mCurrentPage), Con::getIntArg(mAllPage));
	showMarketItemInfo(player);
#endif
#ifdef NTJ_SERVER
	player->setInteraction(player, Player::INTERACTION_SUPERMARKET);
#endif
}

#ifdef NTJ_CLIENT
SuperMarketRes* SuperMarketManager::getMarketRes(U32 id)
{
	return g_MarketItemReposity->getSuperMarketRes(id);
}

void SuperMarketManager::filterMarketGoods()
{
	g_MarketItemReposity->searchGoods(mMarketType, mBigType, mSmallType, mTempGoodsList);
}

U32 SuperMarketManager::getMarketGoodID(S32 index)
{
	if(index < 0 || index >= mTempGoodsList.size())
		return 0;
	else
		return mTempGoodsList[index];
}

S32 SuperMarketManager::getSuperMarketData(Player* player, S32 page)
{
	if(!player)
		return -1;
	S32 iIndex = 0;
	player->mSuperMarketList.clear();
	for (int i=0; i<mTempGoodsList.size(); ++i)
	{
		if(i >= SuperMarketList::MAXSLOTS * page && i < (page + 1) * SuperMarketList::MAXSLOTS)
		{
			SuperMarketRes* pRes = getMarketRes(getMarketGoodID(i));
			if(pRes)
			{
				stMarketItem* pMarketItem = new stMarketItem;
				pMarketItem->marketItemID = pRes->mMarketGoodsID;
				ItemShortcut* pItem = ItemShortcut::CreateItem(pRes->mMarketItemID, 1);
				if(pItem)
				{
					pMarketItem->marketItem = pItem;
					player->mSuperMarketList.SetSlot(iIndex++, pMarketItem);
				}
				else
				{
					delete pItem;
					delete pMarketItem;
					MessageEvent::show(SHOWPOS_SCREEN, "此物品不存在");
					continue;
				}
			}
		}
	}
	U32 iAllNum = mTempGoodsList.size();
	return iAllNum % SuperMarketList::MAXSLOTS ? (iAllNum / SuperMarketList::MAXSLOTS) + 1 : iAllNum / SuperMarketList::MAXSLOTS;
}

void SuperMarketManager::findMarketItem(StringTableEntry name)
{
	U32 iType = getMaketType();
	if(iType == MarketType_ImortalStone)
	{
		g_MarketItemReposity->findMarketItem(iType, name, mFindImortalStoneList);
	}
	else
		g_MarketItemReposity->findMarketItem(iType, name, mFindDeityStoneList);
	showFindMarketItem(iType);
}

void SuperMarketManager::showFindMarketItem(U32 type)
{
	mTempGoodsList.clear();
	if(type == MarketType_ImortalStone)
	{
		for(int i=0; i<mFindImortalStoneList.size(); ++i)
			mTempGoodsList.push_back(mFindImortalStoneList[i]);
	}
	else
	{
		for(int i=0; i<mFindDeityStoneList.size(); ++i)
			mTempGoodsList.push_back(mFindDeityStoneList[i]);
	}
}

void SuperMarketManager::addItemToBasketballList(U32 goodsId, U32 num)
{
	U32 iType = g_MarketManager->getMaketType();
	
	bool isPush = true;
	if(iType == MarketType_ImortalStone)
	{
		for(int i=0; i<mImortalBasketballList.size(); ++i)
		{
			if(goodsId == mImortalBasketballList[i])
			{
				isPush = false;
				if(mImortalStoneNum[i] != num)
					mImortalStoneNum[i] = num;
			}
		}
		if(isPush)
		{
			mImortalBasketballList.push_back(goodsId);
			mImortalStoneNum.push_back(num);
		}
		
	}
	else if(iType == MarketType_DeityStone)
	{
		for(int i=0; i<mDeityBasketballList.size(); ++i)
		{
			if(goodsId == mDeityBasketballList[i])
			{
				isPush = false;
				if(mDeityStoneNum[i] != num)
					mDeityStoneNum[i] = num;
			}
		}
		if(isPush)
		{
			mDeityBasketballList.push_back(goodsId);
			mDeityStoneNum.push_back(num);
		}
		
	}
}

#endif

#ifdef NTJ_CLIENT
SuperMarketList::SuperMarketList()
{
	for(int i=0; i<MAXSLOTS; ++i)
		mSlots[i] = NULL;
}

SuperMarketList::~SuperMarketList()
{
	clear();
}

ItemShortcut* SuperMarketList::GetSlot(S32 index)
{
	if(index < 0 || index >= MAXSLOTS)
		return NULL;
	if(mSlots[index])
		return mSlots[index]->marketItem;
	return NULL;
}

U32 SuperMarketList::getMarketItemID(S32 index)
{
	if(index < 0 || index >= MAXSLOTS)
		return 0;
	if(mSlots[index])
		return mSlots[index]->marketItemID;
	return 0;
}

void SuperMarketList::SetSlot(S32 index, stMarketItem* pItem)
{
	if(index < 0 || index >= MAXSLOTS)
		return;
	if(mSlots[index])
		delete mSlots[index];	
	mSlots[index] = pItem;	
}

void SuperMarketList::clear()
{
	for(int i=0; i<MAXSLOTS; ++i)
	{
		if(mSlots[i])
		{
			if(mSlots[i]->marketItem)
			{
				delete mSlots[i]->marketItem;
				mSlots[i]->marketItem = NULL;
			}
			delete mSlots[i];
		}
		mSlots[i] = NULL;
	}
}

#endif

#ifdef NTJ_CLIENT
ConsoleFunction(setMarketType, void, 2, 2, "setMarketType(%type)")
{
	U32 iType = dAtoi(argv[1]);
	if(iType < MarketType_ImortalStone || iType > MarketType_DeityStone)
		return;
	g_MarketManager->setMarkettype(iType);
}
ConsoleFunction(filterSuperMarket, void, 3, 3, "filterSuperMarket(%big, %small)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	pPlayer->mSuperMarketList.clear();
	g_MarketManager->setMarketBigType(dAtoi(argv[1]));
	g_MarketManager->setMarketSmallType(dAtoi(argv[2]));
	g_MarketManager->filterMarketGoods();
	S32 allPage = g_MarketManager->getSuperMarketData(pPlayer, g_MarketManager->getCurrentPage());
	if(allPage < 0)
		return;
	g_MarketManager->setAllPage(allPage);
	Con::executef("showMarketItemPage", Con::getIntArg(g_MarketManager->getCurrentPage()), Con::getIntArg(allPage));
	g_MarketManager->showMarketItemInfo(pPlayer);

}
//换页显示
ConsoleFunction(changeMarketItemPage, void, 2, 2, "changeMarketItemPage(%page)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 iCurrPage = dAtoi(argv[1]);
	if(iCurrPage < 0 || iCurrPage > g_MarketManager->getAllPage())
		return;
	g_MarketManager->getSuperMarketData(pPlayer, iCurrPage);
	Con::executef("showMarketItemPage", argv[1], Con::getIntArg(g_MarketManager->getAllPage()));
	g_MarketManager->showMarketItemInfo(pPlayer);

}

ConsoleFunction(openSuperMarketWnd, void, 1, 1, "openSuperMarketWnd()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	g_MarketManager->openSuperMarket(pPlayer);
}
//显示购买信息
ConsoleFunction(showBuyMarketItemInfo, void, 2, 2, "showBuyMarketItemInfo(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	S32 index = dAtoi(argv[1]);
	if(index < 0 && index >= SuperMarketList::MAXSLOTS)
		return;
	pPlayer->mRecommendList.clear();
	stMarketItem* pMarketItem = pPlayer->mSuperMarketList.mSlots[index];
	if(!pMarketItem)
		return;
	ItemShortcut* pItem = pPlayer->mSuperMarketList.GetSlot(index);
	Res* pRes = pItem->getRes();
	if(!pItem || !pRes)
		return;
	char szItemName[128] = {0};
	U32 iMoney = 0;
	pRes->getItemName(szItemName, 128);
	SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
	if(!pMarketRes)
		return;
	iMoney = pMarketRes->mMarketItemPrice;

	stMarketItem* pNewMarketItem = new stMarketItem;
	pNewMarketItem->marketItemID = pMarketRes->mMarketGoodsID;
	ItemShortcut* pNewItem = ItemShortcut::CreateItem(pMarketRes->mMarketItemID, 1);
	if(!pNewItem)
		return;
	pNewMarketItem->marketItem = pNewItem;
	pPlayer->mRecommendList.SetSlot(5, pNewMarketItem);
	PackGoods* pPackGoods = g_PackGoodsResposity->getPackGoods(pMarketItem->marketItemID);
	if(pPackGoods)
	{
		for(int i=0; i<5; ++i)
		{
			U32 iGoodID = pPackGoods->mSuitItemID[i];
			if( iGoodID> 0)
			{
				stMarketItem* pRecommendItem = new stMarketItem;
				SuperMarketRes* pRecommendRes = g_MarketItemReposity->getSuperMarketRes(iGoodID);
				if(!pRecommendRes)
					delete pRecommendItem;
				ItemShortcut* pItem = ItemShortcut::CreateItem(pRecommendRes->mMarketItemID, 1);
				if(!pItem)
				{
					delete pRecommendItem;
					delete pItem;
				}

				pRecommendItem->marketItem = pItem;
				pRecommendItem->marketItemID = iGoodID;
				pPlayer->mRecommendList.SetSlot(i, pRecommendItem);
			}
		}
	}
	U32 iType = g_MarketManager->getMaketType();
	Con::executef("ShowAndOpenMarketBuyWnd", szItemName, Con::getIntArg(iMoney), Con::getIntArg(iType));
}
//显示赠取信息
ConsoleFunction(showComplementMarketItemInfo, void, 2, 2, "showComplementMarketItemInfo(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	pPlayer->mRecommendList.clear();

	S32 index = dAtoi(argv[1]);
	if(index < 0 && index >= SuperMarketList::MAXSLOTS)
		return;
	stMarketItem* pMarketItem = pPlayer->mSuperMarketList.mSlots[index];
	if(!pMarketItem)
		return;
	ItemShortcut* pItem = pPlayer->mSuperMarketList.GetSlot(index);
	Res* pRes = pItem->getRes();
	if(!pItem || !pRes)
		return;
	char szItemName[128] = {0};
	U32 iMoney = 0;
	pRes->getItemName(szItemName, 128);
	SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
	if(!pMarketRes)
		return;
	iMoney = pMarketRes->mMarketItemPrice;

	stMarketItem* pNewMarketItem = new stMarketItem;
	pNewMarketItem->marketItemID = pMarketRes->mMarketGoodsID;
	ItemShortcut* pNewItem = ItemShortcut::CreateItem(pMarketRes->mMarketItemID, 1);
	if(!pNewItem)
		return;
	pNewMarketItem->marketItem = pNewItem;
	pPlayer->mRecommendList.SetSlot(6, pNewMarketItem);
	Con::executef("ShowAndOpenComplimentAwayWnd", szItemName, Con::getIntArg(iMoney));
	Con::executef("showFriendList");
}
// 显示索求信息
ConsoleFunction(showAskForInfor, void, 4, 4, "showAskForInfor(%name, %goodsID, %num)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	
	char szItemName[128] = {0};
	U32 iMoney = 0;
	U32 itemId = dAtoi(argv[2]);
	
	pPlayer->mRecommendList.clear();
	SuperMarketRes* pMarketItemRes = g_MarketItemReposity->getSuperMarketRes(itemId);
	if(!pMarketItemRes)
		return;
	stMarketItem* pMarketItem = new stMarketItem;
	pMarketItem->marketItemID = pMarketItemRes->mMarketGoodsID;
	ItemShortcut* pItem = ItemShortcut::CreateItem(pMarketItemRes->mMarketItemID, 1);
	if(!pItem)
		return;
	pMarketItem->marketItem = pItem;
	pItem->getRes()->getItemName(szItemName, 128);
	iMoney = pMarketItemRes->mMarketItemPrice;
	pPlayer->mRecommendList.SetSlot(7, pMarketItem);

	Con::executef("showAskForWnd", (char*)argv[1], szItemName, Con::getIntArg(iMoney), argv[3]);
}
//索取或赠送的好友列表
ConsoleFunction(showFriendList, void, 1, 1, "showFriendList()")
{
	Con::executef("ClearFriendComplementList");

	CClientSocial::SocialMap& map = g_clientSocial.getClientSocialInfo();

	CClientSocial::SocialMap::iterator it = map.begin();
	for( ; it != map.end(); it++ )
	{
		if(  it->second.friendValue > 0 && it->second.status > 0)
			Con::executef("AddFriendToComplementList", it->second.name, Con::getIntArg(it->second.id));
	}
}
//确定赠送
ConsoleFunction(EnsureComplement, void, 3, 3, "EnsureComplement(%num, %name)")
{
	U32 iTargetPlayerID = 0;
	U32 iMoney = 0;
	U32 iNum = dAtoi(argv[1]);
	if(iNum < 1)
		return;
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	stMarketItem* pMarketItem = pPlayer->mRecommendList.mSlots[6];
	if(!pMarketItem)
		return;

	SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
	if(!pMarketRes)
		return;
	
	iMoney = iNum * pMarketRes->mMarketItemPrice;
	CClientSocial::SocialMap& map = g_clientSocial.getClientSocialInfo();

	CClientSocial::SocialMap::iterator it = map.begin();
	for( ; it != map.end(); it++ )
	{
		if(  it->second.friendValue > 0 )
		{
			if(dStrcmp(it->second.name, argv[2]) == 0 && it->second.status > 0)
				iTargetPlayerID = it->second.id;
		}
	}
	if(iTargetPlayerID > 0)
	{
		Con::executef("SptMail_Send", Con::getIntArg(pMarketItem->marketItemID), argv[1],  "赠送", "", Con::getIntArg(iMoney), Con::getIntArg(iTargetPlayerID));
	}
}
//确定索取
ConsoleFunction(EnsureAskFor, void, 3, 3, "EnsureAskFor(%num, %name)")
{
	U32 iTargetPlayerID = 0;
	U32 iNum = dAtoi(argv[1]);
	if(iNum < 1)
		return;
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	
	stMarketItem* pMarketItem = pPlayer->mRecommendList.mSlots[6];
	if(!pMarketItem)
		return;

	SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
	if(!pMarketRes)
		return;
		
	CClientSocial::SocialMap& map = g_clientSocial.getClientSocialInfo();

	CClientSocial::SocialMap::iterator it = map.begin();
	for( ; it != map.end(); it++ )
	{
		if(  it->second.friendValue > 0 )
		{
			if(dStrcmp(it->second.name, argv[2]) == 0 && it->second.status > 0)
				iTargetPlayerID = it->second.id;
		}
	}
	if(iTargetPlayerID > 0)
		Con::executef("SptItemRequest", Con::getIntArg(iTargetPlayerID), Con::getIntArg(pMarketItem->marketItemID), Con::getIntArg(iNum));
}
//显示商品信息
void SuperMarketManager::showMarketItemInfo(Player* pPlayer)
{
	if(!pPlayer)
		return;
	char szItemName[128] = {0};
	U32 iMoney = 0;
	U32 iType = getMaketType();
	for(int i=0; i<SuperMarketList::MAXSLOTS; ++i)
	{
		stMarketItem* pMarketItem = pPlayer->mSuperMarketList.mSlots[i];
		if(pMarketItem)
		{
			ItemShortcut* pItem = pPlayer->mSuperMarketList.GetSlot(i);
			Res* pRes = pItem->getRes();
			if(pItem && pRes)
			{
				pRes->getItemName(szItemName, 128);
				SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
				if(pMarketRes)
					iMoney = pMarketRes->mMarketItemPrice;
				Con::executef("showMarketItemInfo", Con::getIntArg(i), szItemName, Con::getIntArg(iMoney), Con::getIntArg(pRes->getDuration()/60), Con::getIntArg(iType));
			}
		}
		else
			Con::executef("hideMarketItem", Con::getIntArg(i));
	}
}

//确认购买
ConsoleFunction(doBuyMarketItem, void, 2, 2, "doBuyMarketItem(%num)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	U32 iNum = dAtoi(argv[1]);
	stMarketItem* pMarketItem = pPlayer->mRecommendList.mSlots[5];
	if(!pMarketItem)
		return;
	SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
	if(!pMarketRes)
		return;
	U32 iType = g_MarketManager->getMaketType();
	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_SUPERMARKET);
	evt->SetInt32ArgValues(4, MARKET_BUY, pMarketItem->marketItemID, iNum, iType);
	conn->postNetEvent(evt);
}
//添加到购物篮
ConsoleFunction(addBasketBallList, void, 2, 2, "addBasketBallList(%num)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 iNum = dAtoi(argv[1]);
	stMarketItem* pMarketItem = pPlayer->mRecommendList.mSlots[5];
	if(!pMarketItem)
		return;
	SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
	if(!pMarketRes)
		return;
	g_MarketManager->addItemToBasketballList(pMarketRes->mMarketGoodsID, iNum);
	g_MarketManager->setBasketListSlot(pPlayer);
	g_MarketManager->showBasketItemInfo(pPlayer);

}
//搜索
ConsoleFunction(doFindMarketItem, void, 2, 2, "doFindMarketItem(%name)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	StringTableEntry name = argv[1];
	if(!name || dStrlen(name) < 2)
		return;
	g_MarketManager->findMarketItem(name);

	S32 allPage = g_MarketManager->getSuperMarketData(pPlayer, g_MarketManager->getCurrentPage());
	if(allPage < 0)
		return;
	g_MarketManager->setAllPage(allPage);
	Con::executef("showMarketItemPage", Con::getIntArg(g_MarketManager->getCurrentPage()), Con::getIntArg(allPage));
	g_MarketManager->showMarketItemInfo(pPlayer);
}
//点击搜索
ConsoleFunction(clickOnFind, void, 1, 1, "clickOnFind()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 iType = g_MarketManager->getMaketType();
	g_MarketManager->showFindMarketItem(iType);

	S32 allPage = g_MarketManager->getSuperMarketData(pPlayer, g_MarketManager->getCurrentPage());
	if(allPage < 0)
		return;
	g_MarketManager->setAllPage(allPage);
	Con::executef("showMarketItemPage", Con::getIntArg(g_MarketManager->getCurrentPage()), Con::getIntArg(allPage));
	g_MarketManager->showMarketItemInfo(pPlayer);
}
#endif
//推荐栏
#ifdef NTJ_CLIENT
RecommendItemList::RecommendItemList()
{
	for(int i=0; i<MAXSLOTS; ++i)
		mSlots[i] = NULL;
}

RecommendItemList::~RecommendItemList()
{
	clear();
}

void RecommendItemList::clear()
{
	for(int i=0; i<MAXSLOTS; ++i)
	{
		if(mSlots[i])
		{
			if(mSlots[i]->marketItem)
			{
				delete mSlots[i]->marketItem;
				mSlots[i]->marketItem = NULL;
			}
			delete mSlots[i];
		}
		mSlots[i] = NULL;
	}
}

ItemShortcut* RecommendItemList::GetSlot(S32 index)
{
	if(index < 0 || index >= MAXSLOTS)
		return NULL;
	if(mSlots[index])
		return mSlots[index]->marketItem;
	return NULL;
}

void RecommendItemList::SetSlot(S32 index, stMarketItem* pItem)
{
	if(index < 0 || index >= MAXSLOTS)
		return;
	if(mSlots[index])
		delete mSlots[index];	
	mSlots[index] = pItem;	
}

#endif
//购物篮
#ifdef NTJ_CLIENT
ShopBaksetList::ShopBaksetList()
{

}

ShopBaksetList::~ShopBaksetList()
{
	clear();
}

ItemShortcut* ShopBaksetList::GetSlot(S32 index)
{
	if(index < 0)
		return NULL;
	if(mSlots[index])
		return mSlots[index]->marketItem;
	return NULL;
}

void ShopBaksetList::SetSlot(S32 index, stMarketItem* pItem)
{
	if(index < 0)
		return;
	mSlots.push_back(pItem);
}

void ShopBaksetList::clear()
{
	for(int i=0; i<mSlots.size(); ++i)
	{
		if(mSlots[i])
		{
			if(mSlots[i]->marketItem)
			{
				delete mSlots[i]->marketItem;
				mSlots[i]->marketItem = NULL;
			}
			delete mSlots[i];
		}
		mSlots[i] = NULL;
	}
	mSlots.clear();
}

void SuperMarketManager::setBasketListSlot(Player* pPlayer)
{
	if(!pPlayer)
		return;

	U32 iType = getMaketType();
	S32 iIndex = 0;
	pPlayer->mShopBaksetList.clear();
	Con::executef("clearAllBasketList");
	
	U32 iCount = 0;
	if(iType == MarketType_ImortalStone)
	{
		iCount = mImortalBasketballList.size();
		for(int i=0; i<mImortalBasketballList.size(); ++i)
		{
			SuperMarketRes* pRes = getMarketRes(mImortalBasketballList[i]);
			if(pRes)
			{
				stMarketItem* pMarketItem = new stMarketItem;
				pMarketItem->marketItemID = pRes->mMarketGoodsID;
				ItemShortcut* pItem = ItemShortcut::CreateItem(pRes->mMarketItemID, 1);
				if(pItem)
				{	
					pItem->getRes()->setQuantity(mImortalStoneNum[i],true);
					pMarketItem->marketItem = pItem;
					pPlayer->mShopBaksetList.SetSlot(iIndex++, pMarketItem);
				}
				else
				{
					delete pItem;
					delete pMarketItem;
					MessageEvent::show(SHOWPOS_SCREEN, "此物品不存在");
					continue;
				}
			}
		}
	}
	else if(iType == MarketType_DeityStone)
	{
		iCount = mDeityBasketballList.size();
		for(int i=0; i<mDeityBasketballList.size(); ++i)
		{
			SuperMarketRes* pRes = getMarketRes(mDeityBasketballList[i]);
			if(pRes)
			{
				stMarketItem* pMarketItem = new stMarketItem;
				pMarketItem->marketItemID = pRes->mMarketGoodsID;
				ItemShortcut* pItem = ItemShortcut::CreateItem(pRes->mMarketItemID, 1);
				if(pItem)
				{
					pItem->getRes()->setQuantity(mDeityStoneNum[i], true);
					pMarketItem->marketItem = pItem;
					pPlayer->mShopBaksetList.SetSlot(iIndex++, pMarketItem);
				}
				else
				{
					delete pItem;
					delete pMarketItem;
					MessageEvent::show(SHOWPOS_SCREEN, "此物品不存在");
					continue;
				}
			}
		}
	}
	Con::executef("showBasketballInfo", Con::getIntArg(iCount));

}

void SuperMarketManager::showBasketItemInfo(Player* pPlayer)
{
	if(!pPlayer)
		return;
	stMarketItem* pMarketItem = NULL;
	char szItemName[128] = {0};
	U32 iMoney = 0;
	Res* pRes = NULL;
	
	U32 iType = getMaketType();
	U32 iNum = 0;
	U32 iAllMoney = 0;
	SuperMarketRes* pMarketRes = NULL;
	for(int i=0; i<pPlayer->mShopBaksetList.mSlots.size(); ++i)
	{
		pMarketItem = pPlayer->mShopBaksetList.mSlots[i];
		if(pMarketItem)
		{
			if(pMarketItem->marketItem)
			{
				pRes = pMarketItem->marketItem->getRes();
				pRes->getItemName(szItemName, 128);
				iNum = pRes->getQuantity();
				pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
				if(!pMarketRes)
					continue;
				iMoney = pMarketRes->mMarketItemPrice;
				iAllMoney += (pMarketRes->mMarketItemPrice * iNum);
				Con::executef("showBasketItemListInfo", Con::getIntArg(i), szItemName, Con::getIntArg(iMoney), Con::getIntArg(iNum), Con::getIntArg(iType));
			}
		}
	}
	Con::executef("showAllBasketNeedMoney", Con::getIntArg(iAllMoney), Con::getIntArg(iType));
}

//清除某一个物品
ConsoleFunction(clearOneFromBasket, void, 2, 2, "clearOneFromBasket(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	S32 iIndex = dAtoi(argv[1]);
	if(iIndex < 0)
		return;
	U32 iType = g_MarketManager->getMaketType();
	if(iType == MarketType_ImortalStone)
	{
		if(g_MarketManager->mImortalBasketballList[iIndex])
		{
			g_MarketManager->mImortalBasketballList.erase_fast(iIndex);
			g_MarketManager->mImortalStoneNum.erase_fast(iIndex);
		}
		else
			return;
	}
	else if(iType == MarketType_DeityStone)
	{
		if(g_MarketManager->mDeityBasketballList[iIndex])
		{
			g_MarketManager->mDeityBasketballList.erase_fast(iIndex);
			g_MarketManager->mDeityStoneNum.erase_fast(iIndex);
		}
		else
			return;
	}
	g_MarketManager->setBasketListSlot(pPlayer);
	g_MarketManager->showBasketItemInfo(pPlayer);
}
//清除所有
ConsoleFunction(clearAllBasket, void, 1, 1, "clearAllBasket()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 iType = g_MarketManager->getMaketType();
	if(iType == MarketType_ImortalStone)
	{
		g_MarketManager->mImortalBasketballList.clear();
		g_MarketManager->mImortalStoneNum.clear();
	}
	else
	{
		g_MarketManager->mDeityBasketballList.clear();
		g_MarketManager->mDeityStoneNum.clear();
	}
	g_MarketManager->setBasketListSlot(pPlayer);
	g_MarketManager->showBasketItemInfo(pPlayer);
}
//确定购买（购物篮）
ConsoleFunction(BuyBasketItemOk, void, 1, 1, "BuyBasketItemOk()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 iType = g_MarketManager->getMaketType();
	U32 iNeedMoney = 0;
	Res* pRes = NULL;
	stMarketItem* pMarketItem = NULL;
	SuperMarketRes* pMarketRes = NULL;
	
	for(int i=0; i<pPlayer->mShopBaksetList.mSlots.size(); ++i)
	{
		pMarketItem = pPlayer->mShopBaksetList.mSlots[i];
		if(pMarketItem)
		{
			pRes = pMarketItem->marketItem->getRes();
			if(!pRes)
				continue;
			pMarketRes = g_MarketItemReposity->getSuperMarketRes(pMarketItem->marketItemID);
			if(!pMarketRes)
				continue;
			iNeedMoney += (pMarketRes->mMarketItemPrice) * (pRes->getQuantity());
		}
	}
	
	pMarketItem = NULL;
	pRes = NULL;
	if(iType == MarketType_ImortalStone)
	{
		if(!pPlayer->canReduceMoney(iNeedMoney, 1))
		{
			MessageEvent::show(SHOWTYPE_NOTIFY, MSG_PLAYER_GOLDNOTENOUGH);
			return;
		}
		
	}
	BasketItemEvent* evt = new BasketItemEvent(iType);

	for(int i=0; i<pPlayer->mShopBaksetList.mSlots.size(); ++i )
	{
		pMarketItem = pPlayer->mShopBaksetList.mSlots[i];
		if(!pMarketItem)
			continue;
		pRes = pMarketItem->marketItem->getRes();
		if(!pRes)
			continue;

		evt->addBasketItem(pMarketItem->marketItemID, pRes->getQuantity());
	}
	pPlayer->getControllingClient()->postNetEvent(evt);
}

ConsoleFunction(selectedRecommendIndex, void, 2, 2, "selectedRecommendIndex(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	S32 iIndex = dAtoi(argv[1]);
	stMarketItem* pMarketItem = pPlayer->mRecommendList.mSlots[iIndex];
	if(!pMarketItem)
		return;
	g_MarketManager->mTempGoodsList.clear();
	g_MarketManager->mTempGoodsList.push_back(pMarketItem->marketItemID);


	S32 allPage = g_MarketManager->getSuperMarketData(pPlayer, g_MarketManager->getCurrentPage());
	if(allPage < 0)
		return;
	g_MarketManager->setAllPage(allPage);
	Con::executef("showMarketItemPage", Con::getIntArg(g_MarketManager->getCurrentPage()), Con::getIntArg(allPage));
	g_MarketManager->showMarketItemInfo(pPlayer);
}
//试穿
ConsoleFunction(ClickOnSuperItem, void, 2, 2, "ClickOnSuperItem(%index)")
{
	S32 iIndex = dAtoi(argv[1]);
	g_ClientGameplayState->setTryEquipOk(iIndex);
}

#endif