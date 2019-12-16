//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gameplay/GameObjects/PlayerObject.h"

#ifdef NTJ_CLIENT

MissionItemList::MissionItemList()
{
}

// ----------------------------------------------------------------------------
// 添加物品到任务奖励栏
bool ItemManager::addItemToMission(Player* player, U32 type, U32 itemID, U32 num)
{
	if(!player || itemID == 0 || num == 0)
		return false;
	Vector<ItemShortcut*>* pItemList = NULL;
	if(type == 0)
		pItemList = &player->missionItemList.mOptionalItemList;
	else
		pItemList = &player->missionItemList.mFixedItemList;
	if(!pItemList)
		return false;

	ItemShortcut* pItem = ItemShortcut::CreateItem(itemID, num);
	if(!pItem)
		return false;
	pItemList->push_back(pItem);	
	return true;
}

// ----------------------------------------------------------------------------
// 清除物品栏
void MissionItemList::Clear(S32 type /* = -1 */)
{
	if(type == -1 || type == 1)
	{
		for(S32 i = 0; i < mFixedItemList.size(); ++i)
		{
			if(mFixedItemList[i])
				delete mFixedItemList[i];
			mFixedItemList[i] = NULL;
		}
		mFixedItemList.clear();
	}
	
	if(type == -1 || type == 0)
	{
		for(S32 i = 0; i < mOptionalItemList.size(); ++i)
		{
			if(mOptionalItemList[i])
				delete mOptionalItemList[i];
			mOptionalItemList[i] = NULL;
		}
		mOptionalItemList.clear();
	}	
}

S32 MissionItemList::Size(U32 type)
{
	return type == 0 ? mOptionalItemList.size() : mFixedItemList.size();
}

// ----------------------------------------------------------------------------
// 根据索引位置获取槽位对象
ShortcutObject* MissionItemList::GetSlot(U32 type, S32 index)
{
	Vector<ItemShortcut*>* pItemList = NULL;
	if(type == 0)
		pItemList = &mOptionalItemList;
	else
		pItemList = &mFixedItemList;
	if(!pItemList)
		return false;
	if(index >= 0 && index < pItemList->size())
		return (*pItemList)[index];
	return NULL;
}
#endif//NTJ_CLIENT