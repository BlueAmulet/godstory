//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include <hash_map>
#include <list>
#include "T3D/gameConnection.h"
#include "GamePlay/GameObjects/GameObject.h"
#include "GamePlay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/Item/ItemPackage.h"
#include "Gameplay/Item/DropItem.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ai/HateList.h"
#include "Gameplay/Mission/PlayerMission.h"
#include "Gameplay/Item/Player_Item.h"

#ifdef NTJ_SERVER
#include "Common/LogHelper.h"
#include "Common/Log/LogTypes.h"
#include "console/sim.h"
#include "GamePlay/ServerGamePlayState.h"
#endif

#include "DropItem.h"

DropItemList::DropItemList()
{
	m_dwProtTick	= 0;
}

DropItemList::~DropItemList()
{
	Clear();
}

bool DropItemList::AddPlayer(Player *player, U32 nItemPackageId, S32 nMaxNum)
{
	bool ret = false;

#ifdef NTJ_SERVER
	F32 fProbability;

	ItemPackage *pPackage = g_ItemPackageRepository.getPackage(nItemPackageId);
	if(pPackage == NULL)
		return false;

	Vector<ItemProbobilityInfo>::iterator it = pPackage->m_itemsInfo.begin();
	S32 nCount = 0;
	while(it != pPackage->m_itemsInfo.end())
	{
		//nMaxNum小于或等于0无限制
		if (nMaxNum > 0 && nCount >= nMaxNum)
		{
			//数量大于nMaxNum，返回
			break;
		}

		fProbability = (F32)Platform::getRandomI(0, 10000) / 10000;
		if (fProbability < it->m_fProbability)
		{
			bool bSuccess = AddItemToMap(player, it->m_nId);
			if (bSuccess && !ret)
				ret = true;
			nCount++;
		}

		++it;
	}
#endif
	return ret;
}

void DropItemList::CreateFromData(U32 nItemPackageID, NpcObject *pNpc)
{
	
#ifdef NTJ_SERVER
	S32 iPlayerCount = Sim::getClientGroup()->size();
	F32 fProbability;

	for(S32 i = 0; i < iPlayerCount; i++)
	{
		GameConnection* client = dynamic_cast<GameConnection*> ((*Sim::getClientGroup())[i]);
		Player* player = NULL;
		U32 nPlayerID;
		if(client && (player = (Player *)client->getControlObject()))
		{			
			nPlayerID = player->getId();

			//U32 nItemCount = pEventData->m_packages.size();			

			//for(U32 i = 0; i < nItemCount; i++)
			//{
				ItemPackage *pPackage = g_ItemPackageRepository.getPackage(nItemPackageID);
				if(pPackage == NULL)
					continue;

				Vector<ItemProbobilityInfo>::iterator it = pPackage->m_itemsInfo.begin();
				while(it != pPackage->m_itemsInfo.end())
				{
					fProbability = Platform::getRandomI(0, 10000) / 10000;
					if (fProbability < it->m_fProbability)
					{
						AddItemToMap(player, it->m_nId);
					}

					++it;
				}
			//}
		}
	}

	/*
	F32 fProbability;
	HateList::ListHate *pHateList = &pMonster->m_pAI->getHateList().GetHateList();
	HateList::ListHate::iterator itPlayer = pHateList->begin();
	while(itPlayer != pHateList->end())
	{
		Player *player = dynamic_cast<Player *>(itPlayer->m_pObject.getObject());
		if (player == NULL)
		{
			++itPlayer;
			continue;
		}

		U32 nItemCount = pEventData->m_packages.size();			

		for(U32 i = 0; i < nItemCount; i++)
		{
			ItemPackage *pPackage = g_ItemPackageRepository.getPackage(pEventData->m_packages[i]);
			if(pPackage == NULL)
				continue;

			Vector<ItemProbobilityInfo>::iterator it = pPackage->m_itemsInfo.begin();
			while(it != pPackage->m_itemsInfo.end())
			{
				fProbability = Platform::getRandomI(0, 10000) / 10000;
				if (fProbability < it->m_fProbability)
				{
					AddItemToMap(player, it->m_nId);
				}

				++it;
			}
		}

		++itPlayer;
	}
	*/
#endif
}

void DropItemList::Clear()
{
	m_dwProtTick = 0;

#ifdef NTJ_SERVER
	m_MasterList.clear();

	DropItemsMap::iterator it = m_ItemsMap.begin();
	while(it != m_ItemsMap.end())
	{
		PickupList *pItemList = it->second;
		if (pItemList == NULL)
		{
			++it;
			continue;
		}

		pItemList->Clear();
		SAFE_DELETE(pItemList);
		++it;
	}
	m_ItemsMap.clear();

#endif
}

S32	DropItemList::GetItemCount(Player* player)
{
	DropItemsMap::iterator it = m_ItemsMap.find(player->getPlayerID());
	if (it == m_ItemsMap.end())
		return 0;
	if (!it->second)
		AssertFatal(false, "GetItemCount   it->second == NULL");
	return it->second->Size();
}

bool DropItemList::IsViewItems(Player* pPlayer)
{
#ifdef NTJ_SERVER
	if (pPlayer && !IsEmpty(pPlayer->getPlayerID()))
	{
		//if (!IsInProctTime() ||	IsInMasterList(pPlayer))
		//{
			return true;			
		//}
	}
#endif
	return false;
}

bool DropItemList::IsEmpty(U32 nPlayerID)
{
	DropItemsMap::iterator it1 = m_ItemsMap.find(nPlayerID);

	return (it1 == m_ItemsMap.end() || !it1->second || it1->second->Empty());
}

bool DropItemList::IsHavingNothing()
{
	return m_ItemsMap.empty();
}

void DropItemList::DeletePlayer(U32 nPlayerID)
{
	DropItemsMap::iterator itr = m_ItemsMap.find(nPlayerID);
	if (itr != m_ItemsMap.end())
	{
		delete itr->second;
		m_ItemsMap.erase(itr);
	}
}

void DropItemList::SetProtected(U32 dwProtectTime)
{
	m_dwProtTick = Sim::getCurrentTime() + dwProtectTime;
}

bool DropItemList::AddItem(ItemShortcut* pItem)
{	
	return true;
}

#ifdef NTJ_SERVER

bool DropItemList::OpenPickup(Player* pPlayer)
{
	//检查是否可以拾取
	if (!IsViewItems(pPlayer))
		return false;

	return true;
}
bool DropItemList::ClosePickup(Player* pPlayer)
{
	UnProtected();
	return true;
}

bool DropItemList::PickupItemAll(Player* pPlayer, S32 &count, U32 &mask)
{
	PickupList *pItemList = NULL;
	DropItemsMap::iterator it = m_ItemsMap.find(pPlayer->getPlayerID());

	if (it == m_ItemsMap.end())
	{
		return false;
	}

	pItemList = it->second;
	U16 size = pItemList->Size();
	mask = 0;
	count = 0;

	for(U16 i = 0; i < size; i++)
	{
		ItemShortcut *pItem = dynamic_cast<ItemShortcut *>(pItemList->GetSlot(i));
		enWarnMessage msg = MSG_NONE;

		if (pItem != NULL && pItem->getRes()->getItemID() != 100000001 && pItem->getRes()->getItemID() != 100000004)
		{
			S32 index;
			msg = g_ItemManager->addItemToInventory(pPlayer, pItem, index, pItem->getRes()->getQuantity(), ITEM_PICK);
			if (msg == MSG_NONE)
			{
				//pItemList->EraseSlot(i);
				pItemList->DeleteSlot(i);
				mask = mask | ( 1 << i );
				count++;

                //lg it 下面代码编不过
                //lg_pickup_item lg;
                //lg.playerName  = pPlayer->getPlayerName();
                //lg.monsterName = "";
                //lg.itemUID     = pItem->getRes()->getUID();
                //lg.itemName    = pItem->getRes()->getItemName();
                //lg.itemCount   = pItem->getRes()->getQuantity();
                //lg.map         = g_ServerGameplayState->getZoneId();
                //lg.posX        = pPlayer->getPosition().x;
                //lg.posY        = pPlayer->getPosition().y;

                //g_ServerGameplayState->GetLog()->writeLog(&lg);
			}
		}
	}
	pItemList->UpdateList();
	return true;
}

bool DropItemList::PickupItem(Player* pPlayer, U16 nIndex, enWarnMessage &msg)
{
	PickupList *pItemList;

	S32 size = 0;

	pItemList = m_ItemsMap[pPlayer->getPlayerID()];
	if (pItemList == NULL || pItemList->Empty())
	{
		return false;
	}
	
	size = pItemList->Size();
	for(U16 i = 0; i < size; ++i)
	{
		ItemShortcut *pItem = dynamic_cast<ItemShortcut *>(pItemList->GetSlot(i));
		if (nIndex == i &&  pItem != NULL)
		{
			S32 index;
			msg = g_ItemManager->addItemToInventory(pPlayer, pItem, index, pItem->getRes()->getQuantity(), ITEM_PICK);
			if ( msg == MSG_NONE )
			{
				pItemList->EraseSlot(nIndex);

                //lg it 下面代码编不过
                //lg_pickup_item lg;
                //lg.playerName  = pPlayer->getPlayerName();
                //lg.monsterName = "";
                //lg.itemUID     = pItem->getRes()->getUID();
                //lg.itemName    = pItem->getRes()->getItemName();
                //lg.itemCount   = pItem->getRes()->getQuantity();
                //lg.map         = g_ServerGameplayState->getZoneId();
                //lg.posX        = pPlayer->getPosition().x;
                //lg.posY        = pPlayer->getPosition().y;

                //g_ServerGameplayState->GetLog()->writeLog(&lg);
    
				return true;
			}
			else
			{
				//WarnMessageEvent::AddScreenMessage(player, msg);
				return false;
			}
		}
	}

	return false;
}

bool DropItemList::ClearItem(Player* pPlayer, U16 nIndex)
{
	U32 nPlayerID = pPlayer->getPlayerID();
	PickupList *pItemList = NULL;

	pItemList = m_ItemsMap[nPlayerID];
	pItemList->Clear();

	return true;
}
#endif

///////////////////// private member functions /////////////////////////////////

bool DropItemList::AddItemToMap(Player *player, U32 nItemId)
{
	DropItemsMap* pMap = NULL;
	ItemBaseData* pBaseData = g_ItemRepository->getItemData(nItemId);
	PickupList *pItemList = NULL;

	if (pBaseData == NULL || !CanPickupItem(player, pBaseData)) // 玩家筛选
	{
		return false;
	}

	pMap = &m_ItemsMap;

	U32 nPlayerID = player->getPlayerID();

	DropItemsMap::iterator it = pMap->find(nPlayerID);
	if (it == pMap->end())
	{
		pItemList = new PickupList;
		pMap->insert(DropItemsMap::value_type(nPlayerID, pItemList));
	}
	else
	{
		pItemList = it->second;
	}

	return pItemList->AddItem(player, nItemId);
}

bool DropItemList::CanPickupItem(Player *player, ItemBaseData *pBaseData)
{
	//唯一性检查
	if (pBaseData->isOnlyOne())
	{
		if ( player->inventoryList.FindSameObjectByID(pBaseData->getItemID()) != -1)
		{
			return false;
		}

		if( player->pickupList.FindItemByID(pBaseData->getItemID()) != -1)
		{
			return false;
		}		
	}

	//能够触发任务的物品检查
	U32 nBindMissionId = pBaseData->getMissionID();
	if (nBindMissionId != 0 && g_MissionManager->IsFinishedMission(player, nBindMissionId))
	{
		return false;
	}

	//任务检查
	if (pBaseData->isTaskItem())
	{
		S32 nNeedCount = g_MissionManager->GetNeedTaskItem(player, pBaseData->getItemID());
		if (nNeedCount != 0)
		{
			S32 nHadCount = player->inventoryList.GetItemCount(pBaseData->getItemID());
			if (nNeedCount <= nHadCount)
				return false;
		}
		else
			return false;
	}
	return true;
}