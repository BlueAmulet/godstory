#include "Res.h"
#include "Gameplay/Item/Player_GemMount.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameplayCommon.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#ifdef NTJ_CLIENT
#include "ui/dGuiShortCut.h"
#endif

enWarnMessage EquipMountGem::checkMoveFromInventory(Player* pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Res *res = NULL;
	if( !pPlayer || !pSrcItem ||
		!(res = pSrcItem->getRes()) )
		return msg;

	if ( nDestIndex == 0 )
	{
		//装备槽中的装备必须是武器或防具
		if ( !res->isWeapon() && !res->isEquipment() && !res->isOrnament())
			return MSG_ITEM_WEAPONOREQUIPMENT;
	}
	else
	{
		ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
		//槽必须被打孔，而且并未镶嵌
		if ( !pEquipItem->getRes()->IsSlotOpened(nDestIndex) )
			return MSG_ITEM_SLOT_NOTOPENED;
		
		//pSrcItem必须为宝石，并且颜色与目标槽的颜色一致
		if (!res->isGem())
			return MSG_ITEM_GEM;
		if ( !IsColorMatch(pPlayer, pSrcItem, nDestIndex) )
			return MSG_ITEM_GEM_SLOT_COLOR;

		S32 nGemLevel = res->getLimitLevel();
		if (nGemLevel < 1 || nGemLevel > 10)
			return MSG_ITEM_GEM_LEVEL;

		// 判断宝石可镶嵌的部位
		if (!pEquipItem->getRes()->canMountGem((U32)res->getStoneLimit()))
			return MSG_ITEM_GEMPARTLIMIT;

#ifdef NTJ_CLIENT
		if ( !pEquipItem->getRes()->IsSlotEmbedable(nDestIndex))
		{
			//弹出对话框等待用户确认拖动宝石到已镶嵌的槽位
			Con::executef("EquipEmbedGemWnd_MountCheck", Con::getIntArg(nDestIndex));
			return MSG_WAITING_CONFIRM;
		}
#endif
	}

	return MSG_NONE;
}

enWarnMessage EquipMountGem::checkMoveFromEquipment(Player* pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex)
{
	return MSG_NONE;
}

enWarnMessage EquipMountGem::canEquipGemMount(Player* pPlayer, ItemShortcut* pEquipItem, ItemShortcut* pGemItem, S32 nColIndex)
{
	Res *pEquipRes = NULL;
	Res *pGemRes = NULL;

	if ( !pEquipItem || !pGemItem || 
		 !(pEquipRes = pEquipItem->getRes()) ||
		 !(pGemRes = pGemItem->getRes()) )
		return MSG_ITEM_ITEMDATAERROR;

	//pEquipItem必须是武器或防具
	if ( !pEquipRes->isWeapon() && !pEquipRes->isEquipment() )
		return MSG_ITEM_WEAPONOREQUIPMENT;
	//pGemItem必须是宝石
	if ( !pGemRes->isGem() )
		return MSG_ITEM_GEM;

	// 槽的颜色和宝石的颜色必须相同
	if ( !IsColorMatch(pPlayer, pGemItem, nColIndex) )
		return MSG_ITEM_GEM_SLOT_COLOR;

	S32 nGemLevel = pGemRes->getLimitLevel();
	if (nGemLevel < 1 || nGemLevel > 10)
		return MSG_ITEM_GEM_LEVEL;

	// 镶嵌的槽必须是可以镶嵌的(打开并且没有被镶嵌)
	if ( !pEquipItem->getRes()->IsSlotOpened(nColIndex) )
		return MSG_ITEM_SLOT_NOTOPENED;

	// 判断宝石可镶嵌的部位
	if (!pEquipRes->canMountGem((U32)pGemItem->getRes()->getStoneLimit()))
		return MSG_ITEM_GEMPARTLIMIT;

	return MSG_NONE;
}

enWarnMessage EquipMountGem::canEquipGemUnmount(Player* pPlayer, ItemShortcut* pEquipItem, ItemShortcut* pGemItem, S32 nColIndex)
{
	Res *pEquipRes = NULL;
	Res *pGemRes = NULL;

	if ( !pEquipItem || !pGemItem || 
		!(pEquipRes = pEquipItem->getRes()) ||
		!(pGemRes = pGemItem->getRes()) )
		return MSG_ITEM_ITEMDATAERROR;

	//pEquipItem必须是武器或防具
	if ( !pEquipRes->isWeapon() && !pEquipRes->isEquipment() )
		return MSG_ITEM_WEAPONOREQUIPMENT;
	//pGemItem必须是宝石
	if ( !pGemRes->isGem() )
		return MSG_ITEM_GEM;

	// 槽的颜色和宝石的颜色必须相同
	if ( !IsColorMatch(pPlayer, pGemItem, nColIndex) )
		return MSG_ITEM_GEM_SLOT_COLOR;

	// 摘取的槽必须是已经镶嵌宝石的(打开并且已经被镶嵌)
	if ( !pEquipItem->getRes()->IsSlotOpened(nColIndex) )
		return MSG_ITEM_SLOT_NOTOPENED;
	if ( !pEquipItem->getRes()->IsSlotEmbeded(nColIndex) )
		return MSG_ITEM_SLOT_NOTEMBEDED;

	//获得宝石等级，并计算所需的材料ID及数量
	S32 nGemLevel = pGemRes->getLimitLevel();
	if (nGemLevel < 1 || nGemLevel > 10)
		return MSG_ITEM_GEM_LEVEL;
	U32 nMaterialId = atoi(Con::executef("GetEquipGemUnmountConfig", Con::getIntArg(nGemLevel), "0"));
	S32 nMaterialNum = atoi(Con::executef("GetEquipGemUnmountConfig", Con::getIntArg(nGemLevel), "1"));	

#ifdef NTJ_SERVER
	if (pPlayer->inventoryList.GetItemCount(nMaterialId) < nMaterialNum)
	{
		return MSG_PLAYER_MATERIAL_NOTENOUGH;
	}
#endif

	//客户端弹出对话框
#ifdef NTJ_CLIENT
	//探出对话框等待用户确认拖动宝石到已镶嵌的槽位
	ItemBaseData *pData = g_ItemRepository->getItemData(nMaterialId);
	if (!pData)
		return MSG_ITEM_ITEMDATAERROR;
	Con::executef("EquipEmbedGemWnd_UnmountCheck", Con::getIntArg(nColIndex), pData->getItemName(), Con::getIntArg(nMaterialNum));
	return MSG_WAITING_CONFIRM;
#endif

	return MSG_NONE;
}

#ifdef NTJ_SERVER
//清空镶嵌栏
enWarnMessage EquipMountGem::clearEquipGemMount(Player* pPlayer)
{
	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return MSG_NONE;
	ItemShortcut *pEquip = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
	if (!pEquip)
		return MSG_NONE;

	S32 nLockIndex = -1;
	for (S32 i = 1; i < MountGemList::MAXSLOTS; i++)
	{
		// 如果此槽为可镶嵌，则进行检查
		if (!pEquip->getRes()->IsSlotOpened(i))
			continue;

		ItemShortcut *pGemItem = NULL;
		pGemItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(i));
		if (!pGemItem)
			continue;

		nLockIndex = pGemItem->getLockedItemIndex();
		if (nLockIndex < 0)
			continue;
		pPlayer->inventoryList.GetSlot(nLockIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
		pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
		pPlayer->mountGemList.SetSlot(i, NULL);
		pPlayer->mountGemList.UpdateToClient(conn, i, ITEM_NOSHOW);
	}

	//释放物品栏对应装备槽的锁
	nLockIndex = pEquip->getLockedItemIndex();
	pPlayer->inventoryList.GetSlot(nLockIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
	pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
	pPlayer->mountGemList.SetSlot(0, NULL);
	pPlayer->mountGemList.UpdateToClient(conn, 0, ITEM_NOSHOW);

	return MSG_NONE;
}


//把物品栏的宝石镶嵌到装备上
enWarnMessage EquipMountGem::setEquipMountGem(Player* pPlayer, ItemShortcut* pEquipItem, ItemShortcut* pGemItem, S32 nColIndex)
{
	enWarnMessage msg = canEquipGemMount(pPlayer, pEquipItem, pGemItem, nColIndex);
	if (msg != MSG_NONE)
		return msg;

	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return MSG_ITEM_ITEMDATAERROR;

	//把物品栏中对应此宝石的数量减1，如果相减后数量为0，删除物品栏此槽的ItemShortcut
	S32 nLockedIndex = pGemItem->getLockedItemIndex();
	if (nLockedIndex < 0)
		return MSG_ITEM_ITEMDATAERROR;

	pGemItem->setLockedItemIndex(-1);	//重置镶嵌宝石槽对物品栏对应宝石槽的索引锁定
	ItemShortcut *pItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(nLockedIndex));
	Res *pItemRes = pItem->getRes();
	if (!pItem || !pItemRes)
		return MSG_ITEM_ITEMDATAERROR;
	
	if (pItemRes->getQuantity() == 1)
	{
		pPlayer->inventoryList.SetSlot(nLockedIndex, NULL);
	}
	else
	{
		pItemRes->setQuantity(pItemRes->getQuantity() - 1);
		pItem->setSlotState(ShortcutObject::SLOT_COMMON);
	}

	//保存Gem的物品ID到装备镶嵌列表中
	U32 *pEmbedSlot = pEquipItem->getRes()->getExtData()->EmbedSlot;
	pEmbedSlot[nColIndex - 1] = pGemItem->getRes()->getItemID();

	//更新物品栏对应的装备槽，并更新镶嵌后的装备到客户端
	S32 nLockedEquipIndex = pEquipItem->getLockedItemIndex();
	ItemShortcut *pEquipItemInventory = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(nLockedEquipIndex));
	if (!pEquipItemInventory)
		return MSG_ITEM_ITEMDATAERROR;
	pEquipItemInventory->getRes()->getExtData()->EmbedSlot[nColIndex - 1] = pEmbedSlot[nColIndex - 1];
	pPlayer->inventoryList.UpdateToClient(conn, nLockedEquipIndex, ITEM_NOSHOW);

	//更新物品栏的宝石槽以及宝石镶嵌槽到客户端
	pPlayer->inventoryList.UpdateToClient(conn, nLockedIndex, ITEM_NOSHOW);
	pPlayer->mountGemList.UpdateToClient(conn, 0, ITEM_NOSHOW);
	pPlayer->mountGemList.UpdateToClient(conn, nColIndex, ITEM_NOSHOW);

	//发送消息通知客户端镶嵌操作成功
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_ITEM_MOUNTGEM);
	ev->SetInt32ArgValues(2, 0, nColIndex);		// 0表示镶嵌操作
	conn->postNetEvent(ev);

	return MSG_NONE;
}

//摘取镶嵌槽中的宝石到物品栏
enWarnMessage EquipMountGem::unsetEquipMountGem(Player* pPlayer, ItemShortcut* pEquipItem, ItemShortcut* pGemItem, S32 nColIndex)
{
	enWarnMessage msg = canEquipGemUnmount(pPlayer, pEquipItem, pGemItem, nColIndex);
	if (msg != MSG_NONE)
		return msg;

	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return MSG_ITEM_ITEMDATAERROR;

	//寻找物品栏中是否有这种宝石
	//S32 nSlotIndex = pPlayer->inventoryList.FindSameObjectByID(pGemItem->getRes()->getItemID(), true);
	//if (nSlotIndex == -1)
	//{
	//	//物品栏中没有这种宝石, 寻找一个空槽放入
	//	nSlotIndex = pPlayer->inventoryList.FindEmptySlot();
	//	if (nSlotIndex == -1)	//物品栏已满
	//		return MSG_ITEM_ITEMNOSPACE;
	//	pPlayer->inventoryList.SetSlot(nSlotIndex, pGemItem);
	//	//删除镶嵌槽中的宝石
	//	pPlayer->mountGemList.SetSlot(nColIndex, NULL, false);	//不释放ItemShortcut
	//}
	//else
	//{
	//	//物品栏中有这种宝石，数量加1
	//	ItemShortcut *pInventoryItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(nSlotIndex));
	//	pInventoryItem->getRes()->setQuantity(pInventoryItem->getRes()->getQuantity() + 1);
	//	//删除镶嵌槽中的宝石
	//	pPlayer->mountGemList.SetSlot(nColIndex, NULL, true);	//释放ItemShortcut
	//}

	S32 nGemLevel = pGemItem->getRes()->getLimitLevel();
	if (nGemLevel < 1 || nGemLevel > 10)
		return MSG_ITEM_GEM_LEVEL;
	U32 nMaterialId = atoi(Con::executef("GetEquipGemUnmountConfig", Con::getIntArg(nGemLevel), "0"));
	S32 nMaterialNum = atoi(Con::executef("GetEquipGemUnmountConfig", Con::getIntArg(nGemLevel), "1"));

	S32 SlotIndex = 0;
	//从物品栏中扣除摘取此种宝石所需的材料
	msg = g_ItemManager->delItemFromInventory(pPlayer, nMaterialId, nMaterialNum);
	if (msg != MSG_NONE)
		return msg;

	//摘取宝石到物品栏
	msg = g_ItemManager->addItemToInventory(pPlayer, pGemItem->getRes()->getItemID(), SlotIndex, 1,ITEM_GET);
	if (msg != MSG_NONE)
		return msg;

	pPlayer->mountGemList.SetSlot(nColIndex, NULL, true);	//释放ItemShortcut

	//在装备镶嵌列表去掉Gem的ID
	U32 *pEmbedSlot = pEquipItem->getRes()->getExtData()->EmbedSlot;
	pEmbedSlot[nColIndex - 1] = 0;

	//更新镶嵌后的装备到客户端
	S32 nLockedEquipIndex = pEquipItem->getLockedItemIndex();
	ItemShortcut *pEquipItemInventory = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(nLockedEquipIndex));
	if (!pEquipItemInventory)
		return MSG_ITEM_ITEMDATAERROR;
	pEquipItemInventory->getRes()->getExtData()->EmbedSlot[nColIndex - 1] = 0;
	pPlayer->inventoryList.UpdateToClient(conn, nLockedEquipIndex, ITEM_NOSHOW);

	//更新物品栏的宝石槽以及宝石镶嵌槽到客户端
	pPlayer->mountGemList.UpdateToClient(conn, 0, ITEM_NOSHOW);
	pPlayer->mountGemList.UpdateToClient(conn, nColIndex, ITEM_NOSHOW);

	//发送消息通知客户端摘取操作成功
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_ITEM_MOUNTGEM);
	ev->SetInt32ArgValues(2, 0, nColIndex);		// 1表示摘取操作
	conn->postNetEvent(ev);

	return MSG_NONE;
}
#endif

MountGemList::MountGemList() : BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_GEM_MOUNT;
#ifdef NTJ_CLIENT
	for (S32 i = 0; i < MAXSLOTS - 1; i++)
	{
		mOverlap[i] = false;
	}
#endif
}

bool MountGemList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	MountGemEvent* ev = new MountGemEvent(index, flag);
	conn->postNetEvent(ev);
#endif
	return true;
}

bool MountGemList::InitEmbeding(ItemShortcut *pEquipItem)
{
	Res *res = NULL;

	// 被镶嵌的装备必须是武器或防具
	if (!pEquipItem || !(res = pEquipItem->getRes()) || !(res->getExtData()) ||
		 (!res->isWeapon() && !res->isEquipment()) )
	{
		return false;
	}

	// 设置被镶嵌的装备
	SetSlot(0, pEquipItem);
	// 获取镶嵌宝石
	InitEmbedSlots(pEquipItem);
	return true;
}

void MountGemList::InitEmbedSlots(ItemShortcut *pEquip)
{
	U32 *pEmbedSlots = pEquip->getRes()->getExtData()->EmbedSlot;
	for(S32 i = 1; i < MAXSLOTS; i++)
	{
#ifdef NTJ_CLIENT
		mOverlap[i - 1] = false;
#endif
		ItemShortcut *pItem = NULL;
		if ( pEquip->getRes()->IsSlotOpened(i) && pEmbedSlots[i - 1] != 0)	//插孔打开，并有宝石镶嵌
		{
			pItem = ItemShortcut::CreateItem(pEmbedSlots[i - 1], 1);
		}
		SetSlot(i, pItem);
	}
}

S32 MountGemList::GetAllowEmbedSlotsCount()
{
	S32 nCount = 0;
	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(GetSlot(0));
	if (!pEquipItem)
		return 0;

	U32 nAllowEmbedFlag = pEquipItem->getRes()->getAllowEmbedSlot();
	if (!nAllowEmbedFlag)
		return 0;

	for (S32 i = 1; i < MAXSLOTS; i++)
	{
		U32 flag = nAllowEmbedFlag >> (4 * (i - 1));
		if (flag & 0xF)
		{
			nCount++;
		}
	}

	return nCount;
}

S32 MountGemList::GetOpenedEmbedSlotsCount()
{
	S32 nCount = 0;
	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(GetSlot(0));
	if (!pEquipItem)
		return 0;
	U8 nOpenedEmbededFlag = pEquipItem->getRes()->getExtData()->EmbedOpened;
	if ( !nOpenedEmbededFlag )
		return 0;

	for (S32 i = 1; i < MAXSLOTS; i++)
	{
		U32 flag = nOpenedEmbededFlag >> (i - 1);
		if (flag & 0x1)
		{
			nCount++;
		}
	}

	return nCount;
}

#ifdef NTJ_CLIENT
S32 MountGemList::GetSlotType(S32 nIndex)
{
	//  -1: 错误
	//	0:	不允许打孔
	//	1:  未打孔
	//	2:  可镶嵌（打孔但未镶嵌），槽中没有宝石
	//  3:  可镶嵌（打孔但未镶嵌）,槽中有未镶嵌的宝石
	//	4:	已镶嵌（打孔并镶嵌），不属于替换操作
	//	5:	已镶嵌（打孔并镶嵌），属于替换操作

	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(GetSlot(0));
	if (!pEquipItem || nIndex <= 0 || nIndex > 3)		//只能取1 2 3槽
		return -1;

	if (!pEquipItem->getRes()->IsSlotAllowEmbed(nIndex))
		return 0;				//不允许打孔

	if (!pEquipItem->getRes()->IsSlotOpened(nIndex))
		return 1;				//未打孔

	if (pEquipItem->getRes()->IsSlotEmbeded(nIndex))
	{
		if ( !mOverlap[nIndex - 1] )
			return 4;	//不属于替换操作
		else
			return 5;	//属于替换操作
	}
	else
	{
		ItemShortcut *pGem = dynamic_cast<ItemShortcut *>(GetSlot(nIndex));
		if (pGem)
			return 3;
		else
			return 2;
	}
}
#endif

bool EquipMountGem::IsColorMatch(Player* pPlayer, ItemShortcut *pGemItem, S32 nColIndex)
{
	U32 nGemSubCategory = pGemItem->getRes()->getSubCategory();
	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
	if (!pEquipItem)
		return false;

	U32 nAllowFlag = pEquipItem->getRes()->getAllowEmbedSlot();
	U8 nColor = nAllowFlag >> (4 * (nColIndex - 1) ) & 0xF;

	S32 nColorMatch = atoi( Con::executef("GemIsColorMatch", Con::getIntArg(nColor), Con::getIntArg(nGemSubCategory)) );

	return (nColorMatch == 1);
}

#ifdef NTJ_SERVER
enWarnMessage ItemManager::InventoryMoveToMountGem(stExChangeParam* param)
{
	GameConnection *conn = NULL;
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = NULL;
	
	if (param == NULL)
		return msg;

	pPlayer = param->player;
	ItemShortcut *pSrcItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(param->SrcIndex));
	msg = EquipMountGem::checkMoveFromInventory(pPlayer, pSrcItem, param->DestIndex);
	if (msg != MSG_NONE)
		return msg;

	ItemShortcut *pDestItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(param->DestIndex));
	conn = pPlayer->getControllingClient();
	if (!conn)
		return msg;

	if ( param->DestIndex == 0 )	//移动到装备槽
	{
		if (pDestItem)
		{
			S32 nLockIndex = 0;
			//若槽上存在未镶嵌的宝石，释放对其源（物品栏中的宝石）的锁操作
			for (S32 i = 1; i < MountGemList::MAXSLOTS; i++)
			{
				// 如果此槽为可镶嵌，则进行检查
				if (!pDestItem->getRes()->IsSlotOpened(i))
					continue;
				
				ItemShortcut *pGemItem = NULL;
				if (!pDestItem->getRes()->IsSlotOpened(i))	//可镶嵌槽
					continue;

				pGemItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(i));
				if (!pGemItem)
					continue;
				
				nLockIndex = pGemItem->getLockedItemIndex();
				if (nLockIndex < 0)
					continue;
				pPlayer->inventoryList.GetSlot(nLockIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
				pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
			}

			//释放物品栏对应装备槽的锁
			nLockIndex = pDestItem->getLockedItemIndex();
			pPlayer->inventoryList.GetSlot(nLockIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
			pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
		}

		ItemShortcut *pNewItem = ItemShortcut::CreateItem(pSrcItem);
		if (!pNewItem)
			return MSG_ITEM_CANNOTDRAGTOOBJECT;

		pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);
		pPlayer->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
		pNewItem->setLockedItemIndex(param->SrcIndex);
		pPlayer->mountGemList.InitEmbeding(pNewItem);		
		//更新所有镶嵌槽到客户端
		for(S32 i = 1; i < MountGemList::MAXSLOTS; i++)
		{
			pPlayer->mountGemList.UpdateToClient(conn, i, ITEM_NOSHOW);
		}
		pPlayer->mountGemList.UpdateToClient(conn, 0, ITEM_NOSHOW);	//最后更新装备槽到客户端
	}
	else
	{
		ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
		if (!pEquipItem)
			return msg;
		if (pDestItem)
		{
			//槽中放有未镶嵌的宝石,释放对其源（物品栏中的宝石）的锁操作
			ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
			if ( pEquipItem && pEquipItem->getRes()->IsSlotOpened(param->DestIndex) )
			{
				S32 nLockedIndex = pDestItem->getLockedItemIndex();
				if (nLockedIndex >= 0)
				{
					pPlayer->inventoryList.GetSlot(nLockedIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
					pPlayer->inventoryList.UpdateToClient(conn, nLockedIndex, ITEM_NOSHOW);
				}
			}		
		}

		pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);
		pDestItem = ItemShortcut::CreateItem(pSrcItem->getRes()->getItemID(), 1);
		pDestItem->setLockedItemIndex(param->SrcIndex);
		pPlayer->mountGemList.SetSlot(param->DestIndex, pDestItem);

		//只需要更新客户端镶嵌槽的内容
		pPlayer->mountGemList.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
		pPlayer->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
	}

	return MSG_NONE;
}

enWarnMessage ItemManager::EquipMoveToMountGem(stExChangeParam* param)
{
	return MSG_NONE;
}

enWarnMessage ItemManager::MountGemMoveToInventory(stExChangeParam* param)
{
	return MSG_NONE;
}
#endif

EquipMountAble_List g_EquipMountAble_List;

EquipMountAble_List * EquipMountAble_List::getInstance()
{
	return &g_EquipMountAble_List;
}

EquipMountAble_List::EquipMountAble_List()
{
	mSelectedSlotIndex = -1;
	mCurrPage = 0;
	mSelectType = TYPE_WEAPON;
	mSlots = new ItemShortcut * [MAX_SLOTS];
	for (S32 i = 0; i < MAX_SLOTS; i++)
	{
		mSlots[i] = NULL;
	}
}

EquipMountAble_List::~EquipMountAble_List()
{
	delete [] mSlots;
}

bool EquipMountAble_List::IsEquipMountAble(ItemShortcut *pItem)
{
	if (!pItem)
		return false;

	Res *res = pItem->getRes();
	if (!res)
		return false;

	//U32 nAllowFlag = res->getAllowEmbedSlot();
	U32 nOpenedFlag = res->getExtData()->EmbedOpened;
	return (nOpenedFlag != 0);
}

void EquipMountAble_List::updateEquipableList(Player *pPlayer)
{
	ItemShortcut *pItem = NULL;
	Res *pItemRes = NULL;
	S32 nIndex = 0;

	//首先释放原来的内容
	mWeaponsList.clear();
	mEquipmentsList.clear();

	// 遍历物品栏
	for (nIndex = 0; nIndex < InventoryList::MAXSLOTS; nIndex++)
	{
		pItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(nIndex));
		
		if ( !pItem || !(pItemRes = pItem->getRes()) || pItemRes->getAllowEmbedSlot() == 0 )
			continue;

		//判断是否可镶嵌
		//if ( !IsEquipMountAble(pItem) )
		//	continue;

		if (pItemRes->isWeapon())
		{
			mWeaponsList.push_back(nIndex);
		}
		else if (pItemRes->isEquipment())
		{
			mEquipmentsList.push_back(nIndex);
		}

	}

	S32 nTotalPage = getTotalPage();
	if (mCurrPage >= nTotalPage)
	{
		mCurrPage = nTotalPage - 1;
	}
	if (mCurrPage < 0)
	{
		mCurrPage = 0;
	}
}

void EquipMountAble_List::updateSlots(Player *pPlayer)
{
	if (!pPlayer)
		return;

	mSelectedSlotIndex = -1;
	S32 i = 0;
	//首先释放原来的槽
	for (i = 0; i < MAX_SLOTS; i++)
	{
		if (mSlots[i] != NULL)
		{
			delete mSlots[i];
			mSlots[i] = NULL;
		}
	}

	Vector<S32> *pEquipList = NULL;
	if (mSelectType == EquipMountAble_List::TYPE_WEAPON)
	{
		pEquipList = &mWeaponsList;
	}
	else if (mSelectType == EquipMountAble_List::TYPE_EQUIPMENT)
	{
		pEquipList = &mEquipmentsList;
	}
	else	//选择武器或防具标签出错
		return;

	S32 nTotalPage = getTotalPage();
	if (mCurrPage < 0 || mCurrPage >= nTotalPage)	//选择页出错
		return;

	for (i = 0; i < MAX_SLOTS; i++)
	{
		//检查是否超出索引范围
		S32 nIndex = MAX_SLOTS * mCurrPage + i;
		if ( nIndex >= pEquipList->size() )
			break;

		S32 nItemIndex = pEquipList->operator [](nIndex);
		ItemShortcut *pItem = NULL;
		pItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(nItemIndex));
		
		if (pItem)
		{
			mSlots[i] = ItemShortcut::CreateItem(pItem);
		}
	}
}

S32 EquipMountAble_List::getTotalPage()
{
	Vector<S32> *pEquipList = NULL;

	if (mSelectType == EquipMountAble_List::TYPE_WEAPON)
	{
		pEquipList = &mWeaponsList;
	}
	else if (mSelectType == EquipMountAble_List::TYPE_EQUIPMENT)
	{
		pEquipList = &mEquipmentsList;
	}
	else	//选择武器或防具标签出错
		return 0;

	S32 nTotalPage = ( (pEquipList->size() % MAX_SLOTS == 0)) ?
						(pEquipList->size() / MAX_SLOTS) :
						(pEquipList->size() / MAX_SLOTS + 1);

	return nTotalPage;
}

ItemShortcut* EquipMountAble_List::getSlot(S32 nIndex)
{
	if (nIndex < 0 || nIndex >= MAX_SLOTS)
		return NULL;

	return mSlots[nIndex];
}

S32 EquipMountAble_List::getItemIndex(S32 nSlotIndex)
{
	if (mSelectType == TYPE_WEAPON)
	{
		if (nSlotIndex >= 0 && nSlotIndex < mWeaponsList.size() )
			return mWeaponsList[nSlotIndex];
	}
	else if (mSelectType == TYPE_EQUIPMENT)
	{
		if (nSlotIndex >= 0 && nSlotIndex < mEquipmentsList.size() )
			return mEquipmentsList[nSlotIndex];
	}

	return -1;
}


#ifdef NTJ_CLIENT
ConsoleFunction (EquipMountAble_GetSelectType, S32, 1, 1, "EquipMountAble_GetSelectType();")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	if (!pInstance)
		return 0;

	return pInstance->mSelectType;
}

ConsoleFunction (EquipMountAble_SetSelectType, void, 2, 2, "EquipMountAble_SetSelectType(%nSelectType);")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	if (!pInstance)
		return;

	S32 nSelectType = atoi(argv[1]);
	if (nSelectType == 1 || nSelectType == 2)
	{
		pInstance->mSelectType = (EquipMountAble_List::SELECT_TYPE)nSelectType;
		pInstance->mCurrPage = 0;
	}
}

ConsoleFunction (EquipMountAble_RightPage, bool, 1, 1, "EquipMountAble_RightPage();")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pInstance || !pPlayer)
		return false;

	S32 nTotalPage = pInstance->getTotalPage();
	if (pInstance->mCurrPage < nTotalPage - 1)	//没有到末页
	{
		pInstance->mCurrPage += 1;
		//更新槽内容
		pInstance->updateSlots(pPlayer);
		return true;
	}

	return false;
}

ConsoleFunction (EquipMountAble_GetTotalPage, S32, 1, 1, "EquipMountAble_GetTotalPage();")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pInstance || !pPlayer)
		return 0;
	
	return pInstance->getTotalPage();

}

ConsoleFunction (EquipMountAble_GetCurrentPage, S32, 1, 1, "EquipMountAble_GetCurrentPage();")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pInstance || !pPlayer)
		return 0;

	return pInstance->mCurrPage;
}

ConsoleFunction (EquipMountAble_LeftPage, bool, 1, 1, "EquipMountAble_LeftPage();")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pInstance || !pPlayer)
		return false;

	S32 nTotalPage = pInstance->getTotalPage();
	if (pInstance->mCurrPage > 0)	//没有到末页
	{
		pInstance->mCurrPage -= 1;
		//更新槽内容
		pInstance->updateSlots(pPlayer);
		return true;
	}

	return false;
}

ConsoleFunction (EquipMountAble_Refresh, void, 1, 1, "EquipMountAble_Refresh();")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pInstance || !pPlayer)
		return;

	pInstance->updateEquipableList(pPlayer);
	//更新槽内容
	pInstance->updateSlots(pPlayer);

}

ConsoleFunction (EquipMountAble_ShowSlots, void, 1, 1, "EquipMountAble_ShowSlots();")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pInstance || !pPlayer)
		return;

	//更新槽内容
	pInstance->updateSlots(pPlayer);
}

ConsoleFunction (EquipMountAble_GetSlotItemName, StringTableEntry, 2, 2, "EquipMountAble_GetSlotItemName(%nSlotIndex);")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	S32 nSlotIndex = atoi(argv[1]);

	if (!pInstance || nSlotIndex < 0 || nSlotIndex >= EquipMountAble_List::MAX_SLOTS)
		return NULL;

	//更新槽内容
	ItemShortcut *pItem = pInstance->getSlot(nSlotIndex);
	if (!pItem)
		return NULL;
	Res *pRes = pItem->getRes();
	if (!pRes)
		return NULL;

	return pRes->getItemName();
}

ConsoleFunction (EquipMountAble_GetSlotItemColor, StringTableEntry, 2, 2, "EquipMountAble_GetSlotItemColor(%nSlotIndex);")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	S32 nSlotIndex = atoi(argv[1]);

	if (!pInstance || nSlotIndex < 0 || nSlotIndex >= EquipMountAble_List::MAX_SLOTS)
		return NULL;

	//更新槽内容
	ItemShortcut *pItem = pInstance->getSlot(nSlotIndex);
	if (!pItem)
		return NULL;
	Res *pRes = pItem->getRes();
	if (!pRes)
		return NULL;

	char *colorText = Con::getArgBuffer(20);
	
	return pRes->GetColorText(colorText, 20);
}

ConsoleFunction (EquipMountAble_SetSelectSlotIndex, void, 2, 2, "EquipMountAble_SetSelectSlotIndex(%nSlotIndex);")
{
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();
	S32 nSlotIndex = atoi(argv[1]);

	if (!pInstance || nSlotIndex < 0 || nSlotIndex >= EquipMountAble_List::MAX_SLOTS)
		return;
	
	pInstance->mSelectedSlotIndex = nSlotIndex;
}

ConsoleFunction (EquipMountAble_MoveToEquipMountGem, bool, 1, 1, "EquipMountAble_MoveToEquipMountGem();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	EquipMountAble_List	*pInstance = EquipMountAble_List::getInstance();

	if ( !pPlayer || !pInstance )
		return false;

	S32 nSlotIndex = pInstance->mSelectedSlotIndex;
	if (nSlotIndex < 0 || nSlotIndex >= EquipMountAble_List::MAX_SLOTS)
		 return false;

	//获得物品(指向物品栏或装备栏)
	nSlotIndex = pInstance->mCurrPage * EquipMountAble_List::MAX_SLOTS + nSlotIndex;
	//发送消息给ZoneServer
	GameConnection *con = GameConnection::getConnectionToServer();
	if (!con)
		return false;

	S32 nIndex = pInstance->getItemIndex(nSlotIndex);
	if (nIndex == -1)
		return false;

	ClientGameNetEvent *event = new ClientGameNetEvent(INFO_SHORTCUT_EXCHANGE);
	if (!event)
		return false;

	event->SetIntArgValues(4, SHORTCUTTYPE_INVENTORY, nIndex, SHORTCUTTYPE_GEM_MOUNT, 0);
	event->SetInt32ArgValues(1, 0);
	con->postNetEvent(event);

	return true;
}

ConsoleFunction (MountGem_DoMount, void, 2, 2, " MountGem_DoMount(%slotCol)")
{
	S32 nSlotCol = atoi(argv[1]);
	if (nSlotCol <= 0 || nSlotCol >= MountGemList::MAXSLOTS)
		return;
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	GameConnection *conn = NULL;

	if (!pPlayer || !(conn = pPlayer->getControllingClient()) )
		return;

	ClientGameNetEvent *event = new ClientGameNetEvent(INFO_ITEM_MOUNTGEM);
	event->SetInt32ArgValues(1, nSlotCol);
	conn->postNetEvent(event);
	pPlayer->mountGemList.mOverlap[nSlotCol - 1] = false;
}

ConsoleFunction (MountGem_DoUnmount, void, 2, 2, " MountGem_DoUnmount(%slotCol)")
{
	S32 nSlotCol = atoi(argv[1]);
	if (nSlotCol <= 0 || nSlotCol >= MountGemList::MAXSLOTS)
		return;
	
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
	ItemShortcut *pGemItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(nSlotCol));

	enWarnMessage msg = EquipMountGem::canEquipGemUnmount(pPlayer, pEquipItem, pGemItem, nSlotCol);
	if (msg != MSG_NONE && msg != MSG_WAITING_CONFIRM)
	{		
		MessageEvent::show(SHOWTYPE_NOTIFY, msg);	//客户端输出消息
	}
}

ConsoleFunction (MountGem_IsSlotOpened, bool, 3, 3, " MountGem_IsSlotOpened(%nPlayerId, %slotCol)")
{
	Player *pPlayer = dynamic_cast<Player *>(Sim::findObject(argv[1]));
	
	if (pPlayer)
	{
		ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
		if (pEquipItem)
			return pEquipItem->getRes()->IsSlotOpened(atoi(argv[2]));
	}

	return false;
}

ConsoleFunction (MountGem_GetAllowSlotsCount, S32, 2, 2, " MountGem_GetAllowSlotsCount(%nPlayerId)")
{
	Player *pPlayer = dynamic_cast<Player *>(Sim::findObject(argv[0]));

	if (pPlayer)
	{
		return pPlayer->mountGemList.GetAllowEmbedSlotsCount();
	}
	return 0;
}

ConsoleFunction (MountGem_GetOpenedSlotsCount, S32, 2, 2, " MountGem_GetOpenedSlotsCount(%nPlayerId)")
{
	Player *pPlayer = dynamic_cast<Player *>(Sim::findObject(argv[0]));

	if (pPlayer)
	{
		return pPlayer->mountGemList.GetOpenedEmbedSlotsCount();
	}
	return 0;
}

ConsoleFunction (MountGem_GetSlotType, S32, 2, 2, " MountGem_GetSlotType(%slotCol)")
{
	//  -1: 错误
	//	0:	不可镶嵌（未打孔）
	//	1:  可镶嵌（打孔但未镶嵌）
	//	2:	已镶嵌（打孔并镶嵌），不属于替换操作
	//	3:	已镶嵌（打孔并镶嵌），属于替换操作
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	S32 nSlotCol = atoi(argv[1]);

	if (!pPlayer)
		return -1;

	return pPlayer->mountGemList.GetSlotType(nSlotCol);
}

ConsoleFunction (MountGem_GetEquipName, StringTableEntry, 1, 1, " MountGem_GetEquipName()")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	
	ItemShortcut *pEquip = dynamic_cast<ItemShortcut*>(pPlayer->mountGemList.GetSlot(0));
	if (!pEquip)
		return NULL;

	char *buf = Con::getArgBuffer(100);
	char colorText[20];
	dSprintf(buf, 1024, "<t h='1' v='1' c='%s'>%s</t><b/>", pEquip->getRes()->GetColorText(colorText, 20), pEquip->getRes()->getItemName());
	return buf;
}

ConsoleFunction (MountGem_GetSlotGemName, StringTableEntry, 2, 2, " MountGem_GetSlotGemName(%slotCol)")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	S32 nSlotCol = atoi(argv[1]);

	if (!pPlayer || nSlotCol <= 0 || nSlotCol > 3)		//只能取1 2 3槽
		return NULL;

	ItemShortcut *pGem = dynamic_cast<ItemShortcut*>(pPlayer->mountGemList.GetSlot(nSlotCol));
	if (!pGem)
		return NULL;

	char *buf = Con::getArgBuffer(100);
	char colorText[20];
	dSprintf(buf, 1024, "<t h='1' v='1' c='%s'>%s</t><b/>", pGem->getRes()->GetColorText(colorText, 20), pGem->getRes()->getItemName());

	return buf;
}

ConsoleFunction (MountGem_GetSlotGemDesc, StringTableEntry, 2, 2, " MountGem_GetSlotGemDesc(%slotCol)")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	S32 nSlotCol = atoi(argv[1]);

	if (!pPlayer || nSlotCol <= 0 || nSlotCol > 3)		//只能取1 2 3槽
		return NULL;

	ItemShortcut *pGem = dynamic_cast<ItemShortcut*>(pPlayer->mountGemList.GetSlot(nSlotCol));
	if (!pGem)
		return NULL;

	U32 nBaseAttr = pGem->getRes()->getBaseAttribute();
	Stats *pStats = g_BuffRepository.GetBuffStats(nBaseAttr);
	if (pStats)
	{
		char *buff = Con::getReturnBuffer(1024);
		dMemset(buff, 0, 1024);
		char colorText[20];
		GetStatsDesc(pStats, pGem->getRes()->GetColorText(colorText, 20), buff);
		return buff;
	}

	return NULL;
}

ConsoleFunction (MountGem_GetSlotGemColor, S32, 2, 2, " MountGem_GetSlotGemColor(%slotCol)")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	S32 nSlotCol = atoi(argv[1]);

	if (!pPlayer || nSlotCol <= 0 || nSlotCol > 3)		//只能取1 2 3槽
		return 0;

	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
	if (!pEquipItem)
		return 0;
	return pEquipItem->getRes()->GetSlotColor(nSlotCol);
}

ConsoleFunction (MountGem_ConfirmMount, void, 2, 2, " MountGem_ConfirmMountCheck(%nDestSlotIndex)")
{
	ClientGameplayAction *action =  g_ClientGameplayState->getCurrentAction();
	if (!action || action->getActionType() != INFO_SHORTCUT_EXCHANGE || action->getCurrentParamCount() != 1)
		return;
	
	S32 nDestSlotIndex = atoi(argv[1]);
	if (nDestSlotIndex <= 0 || nDestSlotIndex >= MountGemList::MAXSLOTS)
		return;

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;

	//设置客户端重叠标志，用于以后判断能否进行摘取操作
	pPlayer->mountGemList.mOverlap[nDestSlotIndex - 1] = true;

	//发送消息到服务端进行交换
	g_ClientGameplayState->popCursor();
	action->setCursorName(NULL);
	ClientGameplayParam *param = new ClientGameplayParam;
	param->setIntArgValues(2, SHORTCUTTYPE_GEM_MOUNT, nDestSlotIndex);
	g_ClientGameplayState->setCurrentActionParam(param);
}

ConsoleFunction (MountGem_CancelMount, void, 1, 1, " MountGem_CancelMount()")
{
	ClientGameplayAction *action =  g_ClientGameplayState->getCurrentAction();
	if (!action || action->getActionType() != INFO_SHORTCUT_EXCHANGE || action->getCurrentParamCount() != 1)
		return;
	
	g_ClientGameplayState->cancelCurrentAction();
}

ConsoleFunction (MountGem_ConfirmUnmount, void, 2, 2, " MountGem_ConfirmUnmount(%nSlotIndex)")
{
	S32 nSlotCol = atoi(argv[1]);
	GameConnection *conn = GameConnection::getConnectionToServer();
	if (!conn || nSlotCol <= 0 || nSlotCol >= MountGemList::MAXSLOTS)
		return;

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;

	//获得宝石等级，并计算所需的材料ID及数量
	ItemShortcut *pGemItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(nSlotCol));
	if (!pGemItem)
		return;

	S32 nGemLevel = pGemItem->getRes()->getLimitLevel();
	if (nGemLevel < 1 || nGemLevel > 10)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_GEM_LEVEL);
		return;
	}
	U32 nMaterialId = atoi(Con::executef("GetEquipGemUnmountConfig", Con::getIntArg(nGemLevel), "0"));
	S32 nMaterialNum = atoi(Con::executef("GetEquipGemUnmountConfig", Con::getIntArg(nGemLevel), "1"));	
	if (pPlayer->inventoryList.GetItemCount(nMaterialId) < nMaterialNum)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_PLAYER_MATERIAL_NOTENOUGH);
		return;
	}

	ClientGameNetEvent *event = new ClientGameNetEvent(INFO_ITEM_UNMOUNTGEM);
	event->SetInt32ArgValues(1, nSlotCol);
	conn->postNetEvent(event);
	pPlayer->mountGemList.mOverlap[nSlotCol - 1] = false;
}

ConsoleFunction (MountGem_CancelUnmount, void, 1, 1, " MountGem_CancelUnmount()")
{
	
}

ConsoleFunction (EquipEmbedGem_Close, void, 1, 1, " EquipEmbedGem_Close()")	
{
	GameConnection *conn = GameConnection::getConnectionToServer();
	if (!conn)
		return;

	ClientGameNetEvent *event = new ClientGameNetEvent(INFO_ITEM_CLOSE_MOUNTGEM);
	conn->postNetEvent(event);
}
#endif