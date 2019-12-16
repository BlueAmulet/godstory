#include "platform/platform.h"
#include "console/console.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "GamePlay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ServerGameplayState.h"

EquipStrengthenList::EquipStrengthenList() 
						: BaseItemList(MAXSLOTS+1)
{
	mType = SHORTCUTTYPE_STRENGTHEN;
}

bool EquipStrengthenList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	EquipStrengthenEvent* ev = new EquipStrengthenEvent(index, flag);
	conn->postNetEvent(ev);
#endif
	return true;
}

EquipStrengthen::EquipStrengthen()
{

}

EquipStrengthen::~EquipStrengthen()
{

}

bool EquipStrengthen::IsFuLiaoA(ItemShortcut *pItem)	//成功符
{
	if (pItem == NULL)
		return false;

	U32 iItemId = pItem->getRes()->getItemID();
	if (iItemId >= 116020001 && iItemId <= 116020099)
		return true;

	return false;
}

bool EquipStrengthen::IsFuLiaoB(ItemShortcut *pItem)
{
	if (pItem == NULL)
		return false;

	U32 iItemId = pItem->getRes()->getItemID();
	if (iItemId >= 116020101 && iItemId <= 116020199)
		return true;

	return false;
}

bool EquipStrengthen::IsFuLiaoC(ItemShortcut *pItem)
{
	if (pItem == NULL)
		return false;

	U32 iItemId = pItem->getRes()->getItemID();
	if (iItemId >= 116020201 && iItemId <= 116020299)
		return true;

	return false;
}

enWarnMessage EquipStrengthen::checkMoveFromInventory(Player* pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;

	if( !pPlayer || !pSrcItem || !pPlayer->strengthenlist.IsVaildSlot(nDestIndex) )
		return msg;

	if (pSrcItem == NULL)
		return MSG_ITEM_ITEMDATAERROR;

	Res *pSrcRes = pSrcItem->getRes();
	if (pSrcRes == NULL)
		return MSG_ITEM_ITEMDATAERROR;

	switch (nDestIndex)
	{
	case 0:
		{	// 强化栏的第1个槽位必须属于下面几种：武器、法宝、防具、饰品
			if (!pSrcRes->isWeapon() && !pSrcRes->isTrump() && 
				!pSrcRes->isEquipment() && !pSrcRes->isOrnament() )
				return msg;

			//升级限制
			if ( !pSrcRes->canBaseLimit(Res::ITEM_UPGRADE) )
				return MSG_ITEM_UPGRADE_LIMIT;

			//是否通过鉴定
			//if ( !res->hasIdentified() )				//为测试，暂时注释
			//	return MSG_ITEM_HADNOTINDENTIFIED;

			//判断生活技能
			//to do;
			//
		}
		break;
	case 1:	//辅料A
		{
			if (!EquipStrengthen::IsFuLiaoA(pSrcItem))
				return msg;
		}
		break;
	case 2:	//辅料B
		{
			if (!EquipStrengthen::IsFuLiaoB(pSrcItem))
				return msg;
		}
		break;
	case 3:	//辅料C
		{
			if (!EquipStrengthen::IsFuLiaoC(pSrcItem))
				return msg;
		}
		break;
	case 4: //主材料	//主材料是客户端自动计算显示，不允许拖动
		{
			return msg;
		}
		break;
	}

	return MSG_NONE;
}


enWarnMessage EquipStrengthen::canStrengthen(Player* pPlayer, S32 &nCostMoney, U32 &nCostMaterialId, S32 &nCostMaterialNum)
{
	ItemShortcut *pItem0 = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(0));	//需要强化的装备
	ItemShortcut *pItem1 = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(1));	//辅料1
	ItemShortcut *pItem2 = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(2));	//辅料2
	ItemShortcut *pItem3 = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(3));	//辅料3

	Res *res = NULL;

	if ( (pItem0 == NULL) ||
		 (res = pItem0->getRes()) == NULL)
		return MSG_ITEM_ITEMDATAERROR;

	if (!res->isWeapon() && res->isTrump() && res->isEquipment() && res->isOrnament() )
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	//升级限制
	if ( !res->canBaseLimit(Res::ITEM_UPGRADE) )
		return MSG_ITEM_UPGRADE_LIMIT;

	//是否通过鉴定
	//if ( !res->hasIdentified() )				//为测试，暂时注释
	//	return MSG_ITEM_HADNOTINDENTIFIED;

	//判断生活技能
	//to do;
	//

	//判断是否有强化属性或是否到达最大强化等级
	U32 &nCurrEquipStrengthenLevel = pItem0->getRes()->getExtData()->EquipStrengthens;
	if (nCurrEquipStrengthenLevel >= ItemBaseData::MAX_EQUIPSTRENGTHEN_LEVEL)
		return MSG_ITEM_STRENGTHENMAXLEVEL;
	if (res->getBaseData()->getEquipStrengthenNum(nCurrEquipStrengthenLevel) == 0)	//判断下一个强化等级是否有强化属性
		return MSG_ITEM_STRENGTHENPROPERR;

	//判断金钱
	//S32 nTotalMoney = pPlayer->getMoney(Player::Currentcy_Money) + pPlayer->getMoney(Player::Currentcy_BindMoney);
	nCostMoney = getStrengthenPrice(pItem0);
	//if (nTotalMoney < nCostMoney)
	if(!pPlayer->canReduceMoney(nCostMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	getStrengthenMaterial(pItem0, nCostMaterialId, nCostMaterialNum);
	ItemBaseData *pBaseData = g_ItemRepository->getItemData(nCostMaterialId);
	if (!pBaseData)
		return MSG_ITEM_ITEMDATAERROR;
	if ( pPlayer->inventoryList.GetItemCount(nCostMaterialId) < nCostMaterialNum )
		return MSG_PLAYER_MATERIAL_NOTENOUGH;

#ifdef NTJ_SERVER
	
#endif

#ifdef NTJ_CLIENT

#endif
	return MSG_NONE;
}

void EquipStrengthen::openStrengthen(Player* player, NpcObject *pNpc)
{
	if (!player)
		return;

	GameConnection *conn = player->getControllingClient();
	if (!conn)
		return;

#ifdef NTJ_SERVER
	player->setInteraction(pNpc, Player::INTERACTION_STRENGTHEN);
	enWarnMessage MsgCode = player->isBusy(Player::INTERACTION_STRENGTHEN);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(player->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return;
	}
	player->setInteraction(pNpc, Player::INTERACTION_STRENGTHEN);
	//发送消息到客户端，打开强化栏对话框
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_ITEM_STRENGTHEN);
	ev->SetInt32ArgValues(1, 0);
	conn->postNetEvent(ev);

#endif

#ifdef NTJ_CLIENT
	Con::executef("OnClient_OpenEquipStrengthenDlg");
#endif
}

void EquipStrengthen::closeStrengthen(Player* player)
{
	if (!player)
		return;

#ifdef NTJ_SERVER
	ItemShortcut *pItem = NULL;
	GameConnection *conn = player->getControllingClient();
	if (!conn)
		return;

	for (S32 i = 0; i < EquipStrengthenList::MAXSLOTS; i++)
	{
		pItem = dynamic_cast<ItemShortcut *>(player->strengthenlist.GetSlot(i));
		if (pItem != NULL)
		{
			S32 nLockedIndex = pItem->getLockedItemIndex();			

			//更新物品栏对应槽的内容
			ItemShortcut *pSrcItem = ItemShortcut::CreateItem(pItem);
			player->inventoryList.SetSlot(nLockedIndex, pSrcItem);
			player->strengthenlist.SetSlot(i, NULL);				//将强化栏中的槽置空
			player->inventoryList.UpdateToClient(conn, nLockedIndex, ITEM_NOSHOW);
			player->strengthenlist.UpdateToClient(conn, i, ITEM_NOSHOW);
		}
	}

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_ITEM_STRENGTHEN);
	ev->SetInt32ArgValues(1, 1);  //向客户端发送关闭强化栏的消息
	conn->postNetEvent(ev);
#endif

#ifdef NTJ_CLIENT
	// //disable"强化"按钮，并且关闭强化栏对话框
	Con::executef("OnClient_CloseEquipStrengthenDlg");
#endif
}

void EquipStrengthen::getStrengthenMaterial(ItemShortcut* pItem, U32 &nMatirialId, S32 &nMatirialNum)
{
	U32 nStrengthens = pItem->getRes()->getExtData()->EquipStrengthens;

	//获得主材料ID
	nMatirialId = atoi(Con::executef("GetEquipSthrengthenConfig", Con::getIntArg(nStrengthens+1), "0"));
	
	//获得主材料数量
	nMatirialNum = atoi(Con::executef("GetEquipSthrengthenConfig", Con::getIntArg(nStrengthens+1), "1"));
}

S32 EquipStrengthen::getStrengthenPrice(ItemShortcut* pItem)
{
	Res *res = pItem->getRes();
	U32 nColorLevel = res->getColorLevel();
	U32 nEquipPart = 0;
	if (res->isWeapon())
		nEquipPart = 1;		//武器
	else if (res->isEquipment())
		nEquipPart = 2;		//防具
	else if (res->isTrump())
		nEquipPart = 3;		//法宝
	else if (res->isOrnament())
		nEquipPart = 4;		//饰品

	U32 nLimitLevel = res->getLimitLevel();
	U32 nStrengthens = res->getExtData()->EquipStrengthens;

	const char *strPrice = Con::executef("CalcEquipStrengthenPrice", 
											Con::getIntArg(nEquipPart), 
											Con::getIntArg(nColorLevel), 
											Con::getIntArg(nLimitLevel), 
											Con::getIntArg(nStrengthens+1));
	return atoi(strPrice);
}

#ifdef NTJ_CLIENT
bool EquipStrengthen::getEquipStrengthenSlotIndex(ItemShortcut *pItem, S32 &nIndex)
{
	Res *pRes = NULL;
	if ( !pItem || !(pRes = pItem->getRes()) )
		return false;

	if (pRes->isWeapon() || pRes->isTrump() || pRes->isEquipment() || pRes->isOrnament())
	{
		nIndex = 0;
		return true;
	}
	if (EquipStrengthen::IsFuLiaoA(pItem))
	{
		nIndex = 1;
		return true;
	}
	if (EquipStrengthen::IsFuLiaoB(pItem))
	{
		nIndex = 2;
		return true;
	}
	if (EquipStrengthen::IsFuLiaoC(pItem))
	{
		nIndex = 3;
		return true;
	}
	return false;
}
#endif

#ifdef NTJ_SERVER
enWarnMessage EquipStrengthen::setStrengthenResult(Player* pPlayer)
{
	S32 nCostMoney = 0;
	U32 nCostMaterialId = 0;
	S32 nCostMaterialNum = 0;
	enWarnMessage msg = canStrengthen(pPlayer, nCostMoney, nCostMaterialId, nCostMaterialNum);
	if ( msg != MSG_NONE )
		return msg;

	ItemShortcut *pItem0 = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(0));	//需要强化的装备
	ItemShortcut *pItem1 = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(1));	//辅料1		成功率
	ItemShortcut *pItem2 = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(2));	//辅料2		连升率
	ItemShortcut *pItem3 = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(3));	//辅料3		惩罚率

	//扣除花费的金钱和主材料
	pPlayer->reduceMoney(nCostMoney, 21);
	g_ItemManager->delItemFromInventory(pPlayer, nCostMaterialId, nCostMaterialNum);

	//随机决定是否强化成功
	U32 &nCurrEquipStrengthenLevel = pItem0->getRes()->getExtData()->EquipStrengthens;
	U32 nColorLevel = pItem0->getRes()->getColorLevel();
	if (Platform::getRandomI(0, 10000) <= getTotalSuccessRate(0, nColorLevel, nCurrEquipStrengthenLevel, 0, pItem1))  //参数暂时置0
	{
		//强化成功，计算连升率
		U32 nLianShengLv = 0;
		if ( pItem2 && (nLianShengLv = pItem2->getRes()->getBaseData()->getReserve()) && 
			Platform::getRandomI(0, 10000) <= nLianShengLv &&
			pItem0->getRes()->getBaseData()->getEquipStrengthenNum(nCurrEquipStrengthenLevel + 1) != 0)	//判断下两个强化等级是否有强化属性
		{
			//连升成功
			nCurrEquipStrengthenLevel += 2;
			msg = MSG_ITEM_STRENGTHENSUCESS2;
		}
		else					
		{
			//连升失败
			nCurrEquipStrengthenLevel += 1;
			msg = MSG_ITEM_STRENGTHENSUCESS1;
		}

		nCurrEquipStrengthenLevel = mClamp(nCurrEquipStrengthenLevel, 0, 15);
	}
	else
	{
		//强化失败，计算惩罚率
		U32 nChengFaLv = atoi( Con::executef("GetEquipSthrengthenConfig", 
								Con::getIntArg(nCurrEquipStrengthenLevel), 
								Con::getIntArg(2)) );
		U32 nBuTuiJiLv = 0;
		if (pItem3)
		{
			nBuTuiJiLv = pItem3->getRes()->getBaseData()->getReserve();
		}
		if (nCurrEquipStrengthenLevel > 0 && nChengFaLv > nBuTuiJiLv && 
			(nChengFaLv - nBuTuiJiLv) > Platform::getRandomI(0, 10000) )
		{
			//需要惩罚
			nCurrEquipStrengthenLevel -= 1;
			nCurrEquipStrengthenLevel = mClamp(nCurrEquipStrengthenLevel, 0, 15);
			msg = MSG_ITEM_STRENGTHENFAILURE2;
		}
		else
		{
			//不需要惩罚
			msg = MSG_ITEM_STRENGTHENFAILURE1;
		}
	}

	S32 i = 0;
	//更新物品栏中进行装备强化的槽
	U32 srcIndex = pItem0->getLockedItemIndex();
	pPlayer->inventoryList.SetSlot(srcIndex, pItem0, true);
	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return msg;

	pPlayer->inventoryList.UpdateToClient(conn, srcIndex, ITEM_NOSHOW);   //暂时这样处理

	//清空强化栏所有槽
	pPlayer->strengthenlist.SetSlot(0, NULL, false);
	for (i = 1; i < EquipStrengthenList::MAXSLOTS; i++)
	{
		ItemShortcut *pItemSlot = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(i));	//辅料
		if (pItemSlot)
		{
			S32 nLockIndex = pItemSlot->getLockedItemIndex();
			if (nLockIndex == -1)
				continue;
			ItemShortcut *pItemSrc = dynamic_cast<ItemShortcut *>( pPlayer->inventoryList.GetSlot(nLockIndex) );
			if (pItemSrc)
			{
				S32 nQuantity = pItemSrc->getRes()->getQuantity();
				if (nQuantity > 1)
				{
					pItemSrc->getRes()->setQuantity(nQuantity - 1);
					pItemSrc->setSlotState(ShortcutObject::SLOT_COMMON);
				}
				else
				{
					pPlayer->inventoryList.SetSlot(nLockIndex, NULL);
				}

				pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
			}
			pPlayer->strengthenlist.SetSlot(i, NULL);
			pPlayer->strengthenlist.UpdateToClient(conn, i, ITEM_NOSHOW);
		}
	}

	pPlayer->strengthenlist.UpdateToClient(conn, 0, ITEM_NOSHOW);
	return msg;
}

enWarnMessage ItemManager::InventoryMoveToStrengthen(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = NULL;

	if (param == NULL || !(pPlayer=param->player) ||
		!pPlayer->inventoryList.IsVaildSlot(param->SrcIndex))
		return msg;

	ItemShortcut *pSrcItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(param->SrcIndex));
	msg = EquipStrengthen::checkMoveFromInventory(pPlayer, pSrcItem, param->DestIndex);
	if (msg != MSG_NONE)
		return msg;

	ItemShortcut *pDestItem = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(param->DestIndex));
	pSrcItem->setLockedItemIndex(param->SrcIndex);
	pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);
	
	if (pDestItem)
	{
		// 重置前一次物品栏中所交换的ShortcutObject的状态
		S32 nLockedIndex = pDestItem->getLockedItemIndex();
		pPlayer->inventoryList.GetSlot(nLockedIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
		pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), nLockedIndex, ITEM_NOSHOW);
	}

	ItemShortcut* pTargetItem = ItemShortcut::CreateItem(pSrcItem);
	if(!pTargetItem)
		return MSG_ITEM_ITEMDATAERROR;

	pPlayer->strengthenlist.SetSlot(param->DestIndex, pTargetItem, true);
	pPlayer->strengthenlist.GetSlot(param->DestIndex)->setLockedItemIndex(param->SrcIndex);

	pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);

	GameConnection *con = pPlayer->getControllingClient();
	if (con)
	{
		pPlayer->strengthenlist.UpdateToClient(con, param->DestIndex, ITEM_NOSHOW);
		pPlayer->inventoryList.UpdateToClient(con, param->SrcIndex, ITEM_NOSHOW);
	}

	return MSG_NONE;
}

enWarnMessage ItemManager::StrengthenMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = param->player;

	if( param == NULL || (pPlayer == NULL) ||
		!pPlayer->strengthenlist.IsVaildSlot(param->SrcIndex) ||
		!pPlayer->inventoryList.IsVaildSlot(param->DestIndex) )
		return msg;

	S32 nLockIndex = pPlayer->strengthenlist.GetSlot(param->SrcIndex)->getLockedItemIndex();
	pPlayer->strengthenlist.SetSlot(param->SrcIndex, NULL);
	pPlayer->inventoryList.GetSlot(nLockIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
	GameConnection *con = pPlayer->getControllingClient();
	if (con)
	{
		pPlayer->strengthenlist.UpdateToClient(con, param->SrcIndex, ITEM_NOSHOW);
		pPlayer->inventoryList.UpdateToClient(con, nLockIndex, ITEM_NOSHOW);
	}

	return MSG_NONE;
}

enWarnMessage ItemManager::StrengthenMoveToStrengthen(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	/*Player *pPlayer = param->player;

	if( param == NULL || (pPlayer == NULL) || (param->SrcIndex == 0) || (param->DestIndex == 0) ||
		!pPlayer->strengthenlist.IsVaildSlot(param->SrcIndex) ||
		!pPlayer->strengthenlist.IsVaildSlot(param->DestIndex) )
		return msg;

	ItemShortcut *pItemSrc = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(param->SrcIndex));
	ItemShortcut *pItemDest = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(param->DestIndex));
	
	pPlayer->strengthenlist.SetSlot(param->DestIndex, pItemSrc, false);
	pPlayer->strengthenlist.SetSlot(param->SrcIndex, pItemDest, false);

	GameConnection *con = pPlayer->getControllingClient();
	if (con)
	{
		pPlayer->strengthenlist.UpdateToClient(con, param->SrcIndex, ITEM_NOSHOW);
		pPlayer->strengthenlist.UpdateToClient(con, param->DestIndex, ITEM_NOSHOW);
	}
	*/

	return MSG_NONE;
}

ConsoleMethod(Player, OpenEquipStrengthenDialog, void, 3, 3, "%player.OpenEquipStrengthenDialog(%npc);")
{
	NpcObject *pNpc = dynamic_cast<NpcObject *>( Sim::findObject(atoi(argv[2])) );

	if (!pNpc)
		return;

	EquipStrengthen::openStrengthen(object, pNpc);
}
#endif

#ifdef NTJ_CLIENT
ConsoleFunction (CloseEquipStrengthenDialog, void, 1, 1, " CloseEquipStrengthenDialog()")
{
	GameConnection *con = GameConnection::getConnectionToServer();
	if (!con)
		return;

	ClientGameNetEvent *event = new ClientGameNetEvent(INFO_ITEM_STRENGTHEN);
	event->SetInt32ArgValues(1, 1);  //向服务端发送结束强化操作
	con->postNetEvent(event);
}

ConsoleFunction (EquipStrengthen_doStrengthen, void, 1, 1, " EquipStrengthen_doStrengthen()")
{
	GameConnection *con = GameConnection::getConnectionToServer();
	if (!con)
		return;

	ClientGameNetEvent *event = new ClientGameNetEvent(INFO_ITEM_STRENGTHEN);
	event->SetInt32ArgValues(1, 2);  //向服务端发送执行强化操作
	con->postNetEvent(event);
}

ConsoleFunction(EquipStrengthen_GetCostMoney, S32 , 1, 1, "EquipStrengthen_GetCostMoney();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();

	if (!pPlayer)
		return 0;

	ItemShortcut *pEquip = dynamic_cast<ItemShortcut *>( pPlayer->strengthenlist.GetSlot(0) );
	if (!pEquip)
		return 0;

	return EquipStrengthen::getStrengthenPrice(pEquip);
}

ConsoleFunction(EquipStrengthen_HaveEquipItem, bool, 1, 1, "EquipStrengthen_HaveEquipItem();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();

	if (!pPlayer)
		return false;
	ItemShortcut *pEquip = dynamic_cast<ItemShortcut *>( pPlayer->strengthenlist.GetSlot(0) );
	if (!pEquip)
		return false;

	return true;
}

ConsoleFunction(EquipStrengthen_CanStrengthen, bool, 1, 1, "EquipStrengthen_CanStrengthen();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;

	S32 nCostMoney = 0;
	U32 nMaterialId = 0;
	S32 nMaterialNum = 0;

	enWarnMessage msg = EquipStrengthen::canStrengthen(pPlayer, nCostMoney, nMaterialId, nMaterialNum);
	return (msg == MSG_NONE);
}

ConsoleFunction(EquipStrengthen_GetEquipName, StringTableEntry, 1, 1, "EquipStrengthen_GetEquipName();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;

	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(0));
	if (!pEquipItem)
		return NULL;

	StringTableEntry pItemName = pEquipItem->getRes()->getItemName();
	S32 nStrengthenNum = pEquipItem->getRes()->getExtData()->EquipStrengthens;
	S32 size = dStrlen(pItemName) + 15;
	char *buffer = Con::getArgBuffer(size);
	dSprintf(buffer, size, "%s 强化+%d", pItemName, nStrengthenNum);

	return buffer;
}

ConsoleFunction(EquipStrengthen_GetMaterialText, StringTableEntry, 1, 1, "EquipStrengthen_GetMaterialText();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	
	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(0));
	if (!pEquipItem)
		return NULL;

	//获得主材料ID及数量
	U32 nMaterialId = 0;
	S32 nMaterialNum = 0;
	EquipStrengthen::getStrengthenMaterial(pEquipItem, nMaterialId, nMaterialNum);
	ItemBaseData *pMaterialData = g_ItemRepository->getItemData(nMaterialId);
	if (!pMaterialData)
		return NULL;

	S32 nTotalMaterialNum = pPlayer->inventoryList.GetItemCount(nMaterialId);
	char *buffer = Con::getArgBuffer(15);
	dSprintf(buffer, 15, "%d/%d", nTotalMaterialNum, nMaterialNum);

	return buffer;
}

ConsoleFunction(EquipStrengthen_UpdateMaterialUI, void , 1, 1, "EquipStrengthen_UpdateMaterialUI();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
	{
		pPlayer->strengthenlist.SetSlot(4, NULL);
		return;
	}

	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->strengthenlist.GetSlot(0));
	if (!pEquipItem)
	{
		pPlayer->strengthenlist.SetSlot(4, NULL);
		return;
	}
	//获得主材料ID及数量
	U32 nMaterialId = 0;
	S32 nMaterialNum = 0;
	EquipStrengthen::getStrengthenMaterial(pEquipItem, nMaterialId, nMaterialNum);
	ItemBaseData *pMaterialData = g_ItemRepository->getItemData(nMaterialId);
	if (!pMaterialData)
	{
		pPlayer->strengthenlist.SetSlot(4, NULL);
		return;
	}
	S32 nTotalNum = pPlayer->inventoryList.GetItemCount(nMaterialId);
	ItemShortcut *pMaterial = ItemShortcut::CreateItem(nMaterialId, 0);		//主材料槽不显示数量
	pPlayer->strengthenlist.SetSlot(4, pMaterial);
}
#endif

U32 EquipStrengthen::getTotalSuccessRate(U32 nItemLevel, U32 nQualityLevel, U32 nEquipStrengthenLevel, U32 nYangSheng, ItemShortcut *pItemFuZhu_A)
{
	//获得由A类辅助材料决定的成功率
	U32 nFuZhuRate = pItemFuZhu_A ? pItemFuZhu_A->getRes()->getReserveValue() : 0;

	//获得由强化次数及装备颜色等级决定的成功率
	U32 nEquipRate = 0;
	S32 type = 0;
	switch(nQualityLevel)
	{
		case Res::COLORLEVEL_GREY:
			break;
		case Res::COLORLEVEL_WHITE:
			type = 3;
			break;
		case Res::COLORLEVEL_GREEN:
			type = 4;
			break;
		case Res::COLORLEVEL_BLUE:
			type = 5;
			break;
		case Res::COLORLEVEL_PURPLE:
			type = 6;
			break;
		case Res::COLORLEVEL_ORANGE:
			type = 7;
			break;
		case Res::COLORLEVEL_RED:
			break;
		default:
			break;
	}

	if (type > 0)
	{
		nEquipRate = atoi( Con::executef("GetEquipSthrengthenConfig", 
							Con::getIntArg(nEquipStrengthenLevel+1), 
							Con::getIntArg(type)) );
	}

	return (nFuZhuRate + nEquipRate);
}