#include "Gameplay/item/AutoNpcSell.h"
#include "core/stringTable.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/item/Res.h"
#include "Gameplay/item/ItemShortcut.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#endif//NTJ_CLIENT

AutoNpcSellCache gAutoNpcSellCache;
AutoNpcSellCache* g_AutoNpcSellCache = NULL;

AutoNpcSellCache::AutoNpcSellCache()
{
	g_AutoNpcSellCache = this;
	mLoadedCache = false;
	mUpdatedCache = false;
}

AutoNpcSellCache::~AutoNpcSellCache()
{
	clear();
}

// ----------------------------------------------------------------------------
// 自动售卖本地数据读取方法
void AutoNpcSellCache::loadCache(Player* player)
{
	if(!player || mLoadedCache)
		return;
	char szFilePath[MAX_PATH];
	dSprintf(szFilePath, MAX_PATH, GAME_AUTONPCSELL_FILE, player->getPlayerName());
	char filename[256];
	Platform::makeFullPathName(szFilePath, filename, sizeof(filename));
	Stream* stream = ResourceManager->openStream(filename);
	if(stream)
	{
		this->read(player, stream);
		stream->close();
	}
	mLoadedCache = true;
}

// ----------------------------------------------------------------------------
// 自动售卖本地数据存取方法
void AutoNpcSellCache::saveCache(Player* player)
{
	if(!player || !mUpdatedCache)
		return;
	char szFilePath[MAX_PATH];
	dSprintf(szFilePath, MAX_PATH, GAME_AUTONPCSELL_FILE, player->getPlayerName());
	Platform::createPath(szFilePath);
	FileStream stream;
	if(ResourceManager->openFileForWrite(stream, szFilePath)) 
	{
		this->write(player, &stream);
		stream.close();
	}
}

// ----------------------------------------------------------------------------
// 自动售卖数据写方法
bool AutoNpcSellCache::write(Player* player, Stream* stream)
{
	if(!player || !stream)
		return false;
	stream->writeString(player->getPlayerName());
	stream->write(player->autoSellList.mAutoSellItem.size());
	for(S32 i = 0; i < player->autoSellList.mAutoSellItem.size(); i++)
	{
		ItemShortcut* pItem = player->autoSellList.mAutoSellItem[i];
		bool hasItem = pItem ? true : false;
		stream->write(hasItem);
		if(hasItem)
		{
			stream->write(pItem->getRes()->getItemID());
			stItemInfo* info = pItem->getRes()->getExtData();
			bool hasInfo = info ? true : false;
			stream->write(hasInfo);
			if(hasInfo)
			{
				stream->write(sizeof(stItemInfo), info);
			}
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// 自动售卖数据读方法
bool AutoNpcSellCache::read(Player* player, Stream* stream)
{
	if(!player || !stream)
		return false;
	char szPlayerName[32];
	U32 count, itemID;
	bool hasItem,hasInfo;
	stream->readString(szPlayerName,32);
	stream->read(&count);
	for(S32 i = 0; i < count; i++)
	{
		stream->read(&hasItem);
		if(hasItem)
		{
			stream->read(&itemID);
			stream->read(&hasInfo);
			ItemShortcut* pItem = NULL;
			if(hasInfo)
			{
				stItemInfo info;
				stream->read(sizeof(stItemInfo), &info);
				pItem = ItemShortcut::CreateItem(info);
			}
			else
			{
				pItem = ItemShortcut::CreateItem(itemID, 1);
			}
			if(!pItem)
				return false;
			player->autoSellList.mAutoSellItem.push_back(pItem);
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// 清除自动售卖数据
void AutoNpcSellCache::clear()
{
	for(S32 i = 0; i < mData.size(); i++)
	{
		ItemShortcut* pItem = mData[i];
		if(pItem)
		{
			delete pItem;
		}
	}
	mData.clear();
}

#ifdef NTJ_CLIENT

//=============================================================================
// 自动售卖列表
//
//=============================================================================
//自动售卖
AutoSellList::AutoSellList()
{
	mShowType = 1;
	isAutoSell = false;
	VECTOR_SET_ASSOCIATION(mBestItem);
	VECTOR_SET_ASSOCIATION(mGeneralItem);
	VECTOR_SET_ASSOCIATION(mAutoSellItem);
}

AutoSellList::~AutoSellList()
{
	clear();
}

// ----------------------------------------------------------------------------
// 从物品栏筛选物品分别加入普通物品栏和优秀物品栏
void AutoSellList::assortItem()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	for(S32 i = 0; i < pPlayer->inventoryList.GetMaxSlots(); ++i)
	{
		ItemShortcut* pItem = (ItemShortcut*)pPlayer->inventoryList.GetSlot(i);
		if(!pItem || pItem->getSlotState() == ShortcutObject::SLOT_LOCK)
			continue;

		if(!pItem->getRes()->canBaseLimit(Res::ITEM_SELLNPC))
			continue;

		ItemShortcut* newItem = ItemShortcut::CreateItem(pItem);
		if(!newItem)
			break;

		bool bAdd = true;
		if(pItem->getRes()->getColorLevel() < Res::COLORLEVEL_GREEN)
		{
			U32 iSize = pPlayer->autoSellList.mGeneralItem.size();					
			for (S32 j= 0; j < iSize; ++j)
			{
				ItemShortcut* pAutoObj = pPlayer->autoSellList.mGeneralItem[j];
				if(!pAutoObj)
					continue;
				if(g_ItemManager->isSameItem(pAutoObj, pItem,true))
				{
					bAdd = false;
					break;
				}							
			}

			if(bAdd)
				pPlayer->autoSellList.mGeneralItem.push_back(newItem);
			else
				delete newItem;
		}
		else
		{
			U32 iSize = pPlayer->autoSellList.mBestItem.size();
			for (S32 j = 0; j < iSize; ++j)
			{
				ItemShortcut* pAutoObj = pPlayer->autoSellList.mBestItem[j];
				if(!pAutoObj)
					continue;
				if(g_ItemManager->isSameItem(pAutoObj, pItem, false))
				{
					bAdd = false;
					break;
				}
			}

			if(bAdd)
				pPlayer->autoSellList.mBestItem.push_back(newItem);
			else
				delete newItem;
		}
	}
}

// ----------------------------------------------------------------------------
// 自动售卖
void AutoSellList::autoSell()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	if(isAutoSell)
		return;
	S32 iCount = pPlayer->autoSellList.mAutoSellItem.size();
	for(S32 i = 0; i< iCount; ++i)
	{
		ItemShortcut* pObj = pPlayer->autoSellList.mAutoSellItem[i];
		if(pObj)
		{
			Vector<S32> samelist;
			S32 iNum = pPlayer->inventoryList.QuerySameObject(pObj, samelist, false);
			if(iNum > 0)
			{
				for(S32 j = 0; j < iNum; ++j)
				{
					ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
					event->SetIntArgValues(2, NPCSHOP_SELL, samelist[j]);
					pPlayer->getControllingClient()->postNetEvent(event);
				}
			}		
		}
	}
	isAutoSell = true;
	assortItem();
}

void AutoSellList::autoSellByIndex(Player* pPlayer, U32 index)
{
	if(!pPlayer)
		return;
	U32 iCount = pPlayer->autoSellList.getItemCount();
	if(index < 0 || index > iCount)
		return;
	ItemShortcut* pItem = pPlayer->autoSellList.GetSlot(index);
	if(!pItem)
		return;
	Vector<S32> samelist;
	S32 iNum = pPlayer->inventoryList.QuerySameObject(pItem, samelist, false);
	if(iNum > 0)
	{
		for(int i=0; i<iNum; ++i)
		{
			ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
			event->SetIntArgValues(2, NPCSHOP_SELL, samelist[i]);
			pPlayer->getControllingClient()->postNetEvent(event);
		}
	}

	pPlayer->autoSellList.delFromAutoSellList(pPlayer, index);
}
ItemShortcut* AutoSellList::GetSlot(U32 index)
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return NULL;
	U32 iCount = 0;
	switch (mShowType)
	{
	case 1://普通
		{
			iCount = pPlayer->autoSellList.mGeneralItem.size();
			if(index < 0 || index >= iCount || iCount == 0)
				return NULL;
			else
			{
				//Con::executef("showGeneralListInfor", Con::getIntArg(mShowType), Con::getIntArg(index));
				return pPlayer->autoSellList.mGeneralItem[index];
			}
		}
		break;
	case 2://优秀
		{
			iCount = pPlayer->autoSellList.mBestItem.size();
			if(index < 0 || index >= iCount || iCount == 0)
				return NULL;
			else
			{
				//Con::executef("showGeneralListInfor", Con::getIntArg(mShowType), Con::getIntArg(index));
				return pPlayer->autoSellList.mBestItem[index];
			}
		}
		break;
	case 3://自动售卖
		{
			iCount = pPlayer->autoSellList.mAutoSellItem.size();
			if(index < 0 || index >= iCount || iCount == 0)
				return NULL;
			else
			{
				//Con::executef("showGeneralListInfor", Con::getIntArg(mShowType), Con::getIntArg(index));
				return pPlayer->autoSellList.mAutoSellItem[index];
			}
		}
		break;
	default:
		break;
	}
	return NULL;
}

void AutoSellList::setShowType(U32 type)
{
	mShowType = type;
}

U32 AutoSellList::getShowType()
{
	return mShowType;
}
U32 AutoSellList::getItemCount()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return 0;
	U32 iCount = 0;
	switch (mShowType)
	{
	case 1://普通
		{
			iCount = pPlayer->autoSellList.mGeneralItem.size();
		}
		break;
	case 2://优秀
		{
			iCount = pPlayer->autoSellList.mBestItem.size();
		}
		break;
	case 3://自动售卖
		{
			iCount = pPlayer->autoSellList.mAutoSellItem.size();
		}
		break;
	default:
		break;
	}
	return iCount;
}
void AutoSellList::showAutoSellItem()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	pPlayer->autoSellList.setShowType(1);
	U32 iCount = getItemCount();
	g_AutoNpcSellCache->loadCache(pPlayer);
	Con::executef("showAutoSellWnd", Con::getIntArg(iCount));
}


void AutoSellList::clear()
{
	clearGeneralItemList();
	clearBestItemList();
	for(U32 i = 0; i < mAutoSellItem.size(); i++)
	{
		if(mAutoSellItem[i])
			delete mAutoSellItem[i];
		mAutoSellItem[i] = NULL;
	}
	mAutoSellItem.clear();
}

void AutoSellList::clearGeneralItemList()
{
	for(U32 i = 0; i < mGeneralItem.size(); i++)
	{
		if(mGeneralItem[i])
			delete mGeneralItem[i];
		mGeneralItem[i] = NULL;
	}
	mGeneralItem.clear();
}

void AutoSellList::clearBestItemList()
{
	for(U32 i = 0; i < mBestItem.size(); i++)
	{
		if(mBestItem[i])
			delete mBestItem[i];
		mBestItem[i] = NULL;
	}
	mBestItem.clear();
}

void AutoSellList::addToAutoSellList(Player* pPlayer, S32 index)
{
	if(!pPlayer)
		return ;
	ItemShortcut* pItem = GetSlot(index);
	if(!pItem)
		return;
	ItemShortcut* item = ItemShortcut::CreateItem(pItem);
	if(!item)
		return;
	U32 iSize = pPlayer->autoSellList.mAutoSellItem.size();
	bool bAdd = true;
	for(int i=0; i<iSize; ++i)
	{
		ItemShortcut* pAutoObj = pPlayer->autoSellList.mAutoSellItem[i];
		if(!pAutoObj)
			continue;
		if(g_ItemManager->isSameItem(pAutoObj, item, false))
		{
			bAdd = false;
			break;
		}
	}
	if(bAdd)
	{
		pPlayer->autoSellList.mAutoSellItem.push_back(item);
		g_AutoNpcSellCache->setUpdate();
	}
	else
		delete item;
}

void AutoSellList::delFromAutoSellList(Player* pPlayer, S32 index)
{
	if(!pPlayer)
		return;
	if(index < 0 || index > pPlayer->autoSellList.getItemCount())
		return;
	U32 iType = pPlayer->autoSellList.getShowType();
	switch (iType)
	{
	case 1:
		delete pPlayer->autoSellList.mGeneralItem[index];
		pPlayer->autoSellList.mGeneralItem.erase_fast(index);
		break;
	case 2:
		delete pPlayer->autoSellList.mBestItem[index];
		pPlayer->autoSellList.mBestItem.erase_fast(index);
		break;
	case 3:
		delete pPlayer->autoSellList.mAutoSellItem[index];
		pPlayer->autoSellList.mAutoSellItem.erase_fast(index);
		Con::executef("refreshAutoSellList");
		g_AutoNpcSellCache->setUpdate();
		break;
	}
	U32 iCount = getItemCount();
}

void AutoSellList::clearAll(Player* pPlayer)
{
	if(!pPlayer)
		return;
	for(U32 i = 0; i < mAutoSellItem.size(); i++)
	{
		if(mAutoSellItem[i])
			delete mAutoSellItem[i];
		mAutoSellItem[i] = NULL;
	}
	mAutoSellItem.clear();
	g_AutoNpcSellCache->setUpdate();
	Con::executef("refreshAutoSellList");

}

ConsoleFunction(openAutoSell, void, 1, 1, "openAutoSell()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	player->autoSellList.clearGeneralItemList();
	player->autoSellList.clearBestItemList();
	player->autoSellList.assortItem();
	player->autoSellList.showAutoSellItem();
	Con::executef("showGeneralListInfor", Con::getIntArg(1));
}

ConsoleFunction(refreshAutoSellWnd, void, 2, 2, "refreshAutoSellWnd(%iType)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	U32 iType = dAtoi(argv[1]);
	player->autoSellList.setShowType(iType);
	player->autoSellList.clearGeneralItemList();
	player->autoSellList.clearBestItemList();
	player->autoSellList.assortItem();

	Con::executef("RefreshAutoSellSlots", Con::getIntArg(player->autoSellList.getItemCount()));
	Con::executef("showGeneralListInfor", Con::getIntArg(iType));
}

ConsoleFunction(refreshAutoSellList, void, 1, 1, "refreshAutoSellList()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	player->autoSellList.setShowType(3);
	Con::executef("RefreshAutoSellSlots", Con::getIntArg(player->autoSellList.getItemCount()));	
	Con::executef("showGeneralListInfor", Con::getIntArg(3));
}

//添加自动出售栏
ConsoleFunction(addToAutoSellList, void, 2, 2, "addToAutoSellList(%index)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	U32 iIndex = dAtoi(argv[1]);
	player->autoSellList.addToAutoSellList(player, iIndex);
}
// 删除
ConsoleFunction(delFromAutoSellList, void, 2, 2, "delFromAutoSellList(%index)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	U32 iIndex = dAtoi(argv[1]);
	player->autoSellList.delFromAutoSellList(player, iIndex);
}

ConsoleFunction(autoListClearAll, void, 1, 1, "autoListClearAll()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	player->autoSellList.clearAll(player);
}

// 卖出
ConsoleFunction(AutoSellByIndex, void, 2, 2, "AutoSellByIndex(%index)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	U32 iIndex = dAtoi(argv[1]);
	player->autoSellList.autoSellByIndex(player, iIndex);
}

ConsoleFunction(saveAutoSellList, void, 1, 1, "saveAutoSellList")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	g_AutoNpcSellCache->saveCache(player);
}

ConsoleFunction(showGeneralListInfor, void, 2, 2, "showGeneralListInfor(%type)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 iType = dAtoi(argv[1]);
	ItemShortcut* pItem = NULL;
	U32 iCount = pPlayer->autoSellList.getItemCount();
	for(S32 i = 0; i < iCount; ++i)
	{
		switch(iType)
		{
		case 1:
			pItem = pPlayer->autoSellList.mGeneralItem[i];
			break;
		case 2:
			pItem = pPlayer->autoSellList.mBestItem[i];
			break;
		case 3:
			pItem = pPlayer->autoSellList.mAutoSellItem[i];
			break;
		}
		if(!pItem)
			return;
		char itemName[32] = {0};
		dSprintf(itemName, sizeof(itemName), "%s", pItem->getRes()->getItemName());
		U32 iMoney = pItem->getRes()->getSalePrice();
		Con::executef("setEveryItemInfo", Con::getIntArg(i), itemName, Con::getIntArg(iMoney));
	}
}

#endif//NTJ_CLIENT