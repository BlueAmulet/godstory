#include "Gameplay/Item/Res.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ServerGameplayState.h"

PunchHoleList::PunchHoleList() : BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_EQUIP_PUNCHHOLE;
}

bool PunchHoleList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	PunchHoleEvent* ev = new PunchHoleEvent(index, flag);
	conn->postNetEvent(ev);
#endif
	return true;
}

enWarnMessage EquipPunchHole::openPunchHole(Player* pPlayer, NpcObject *pNpc)
{
	if (!pPlayer)
		return MSG_NONE;

	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return MSG_NONE;

#ifdef NTJ_SERVER
	pPlayer->setInteraction(pNpc, Player::INTERACTION_PUNCHHOLE);
	enWarnMessage MsgCode = pPlayer->isBusy(Player::INTERACTION_PUNCHHOLE);
	if(MsgCode != MSG_NONE)
	{
		return MsgCode;
	}
	pPlayer->setInteraction(pNpc, Player::INTERACTION_PUNCHHOLE);
	//发送消息到客户端，打开强化栏对话框
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_ITEM_PUNCHHOLE);
	if (ev)
	{
		ev->SetInt32ArgValues(1, 0);
		conn->postNetEvent(ev);
	}
#endif

#ifdef NTJ_CLIENT
	Con::executef("OnClient_OpenEquipPunchHoleDlg");
#endif

	return MSG_NONE;
}

enWarnMessage EquipPunchHole::closePunchHole(Player* pPlayer)
{
	if (!pPlayer)
		return MSG_NONE;

	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return MSG_NONE;

#ifdef NTJ_SERVER
	ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(0));
	//把打孔的装备槽置空，并对相应物品栏的槽解锁
	if (pEquipItem)
	{
		S32 nLockIndex = pEquipItem->getLockedItemIndex();
		if (nLockIndex != -1)
		{
			pPlayer->inventoryList.GetSlot(nLockIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
			pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
		}
		pPlayer->punchHoleList.SetSlot(0, NULL);
		pPlayer->punchHoleList.UpdateToClient(conn, 0, ITEM_NOSHOW);
	}

	//发送消息给客户端，确认关闭界面
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_ITEM_PUNCHHOLE);
	if (ev)
	{
		ev->SetInt32ArgValues(1, 1);	// 客户端关闭消息： 1
		conn->postNetEvent(ev);
	}
#endif

#ifdef NTJ_CLIENT
	Con::executef("OnClient_CloseEquipPunchHoleDlg");		//关闭装备打孔对话框
#endif

	return MSG_NONE;
}

S32	EquipPunchHole::getPunchableHoleIndex(Player *pPlayer, ItemShortcut* pEquipItem)
{
	if (!pPlayer || !pEquipItem)
		return -1;

	U32 nAllowSlot = pEquipItem->getRes()->getAllowEmbedSlot();
	U8 nOpenedSlot = pEquipItem->getRes()->getExtData()->EmbedOpened;
	for (S32 i = 0; i < 3; i++)
	{
		if ( ((nAllowSlot >> (4 * i)) & 0xF) && !((nOpenedSlot >> i) & 0x1) )
			return i;
	}

	return -1;
}

enWarnMessage EquipPunchHole::checkMoveFromInventory(Player *pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	if( pPlayer == NULL || nDestIndex != 0 )		//只有装备栏位可以拖动
		return msg;

	if (pSrcItem == NULL)
		return MSG_ITEM_ITEMDATAERROR;
	Res *pSrcRes = pSrcItem->getRes();
	if (pSrcRes == NULL)
		return MSG_ITEM_ITEMDATAERROR;

	if (!pSrcRes->isWeapon() && !pSrcRes->isEquipment() && !pSrcRes->isOrnament() && !pSrcRes->isTrump())
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	return MSG_NONE;
}

enWarnMessage EquipPunchHole::canPunchHole(Player* pPlayer, ItemShortcut* pEquipItem, S32 &money, S32 &nHoleIndex)
{
	if (!pEquipItem)
		return MSG_ITEM_ITEMDATAERROR;
	Res *pEquipRes = pEquipItem->getRes();
	if (!pEquipRes)
		return MSG_ITEM_ITEMDATAERROR;;
	stItemInfo *pEquipInfo = pEquipRes->getExtData();
	if (!pEquipInfo)
		return MSG_ITEM_ITEMDATAERROR;

	//判断是否允许有孔可打，并且是否有剩余的孔未打
	nHoleIndex = getPunchableHoleIndex(pPlayer, pEquipItem);
	if (nHoleIndex == -1)
		return MSG_ITEM_PUNCHHOLE_FULL; //此装备上的孔已满，不能再打孔

	//金钱是否足够
	money = CalculatePunchMoney(pPlayer, pEquipItem);
	/*S32 nTotalMoney = pPlayer->getMoney(Player::Currentcy_Money) +  pPlayer->getMoney(Player::Currentcy_BindMoney);
	if (money > nTotalMoney)*/
	if(!pPlayer->canReduceMoney(money, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	return MSG_NONE;
}

S32 EquipPunchHole::CalculatePunchMoney(Player* pPlayer, ItemShortcut* pEquipItem)
{
	if ( !pPlayer || !pEquipItem)
		return 0;

	S32 nLevel = pEquipItem->getRes()->getLimitLevel();
	S32 nAllowNum = pEquipItem->getRes()->getAllowedHoles();
	S32 nHoleNum = pEquipItem->getRes()->getOpenedHoles();
	S32 nCostMoney = atoi(Con::executef("GetEquipPunchHoleCostMoney", 
											Con::getIntArg(nLevel), 
											Con::getIntArg(nHoleNum),
											Con::getIntArg(nAllowNum)));
	return nCostMoney;
}

#ifdef NTJ_SERVER
enWarnMessage EquipPunchHole::setEquipPunchHoleResult(Player* pPlayer, ItemShortcut* pEquipItem)
{
	S32 nCostMoney = 0;
	S32 nHoleIndex = 0;
	enWarnMessage msg = canPunchHole(pPlayer, pEquipItem, nCostMoney, nHoleIndex);
	
	if ( msg != MSG_NONE )
		return msg;

	S32 nHolesNum = pEquipItem->getRes()->getOpenedHoles();

	//扣除金钱
	pPlayer->reduceMoney(nCostMoney, 21);

	//计算成功率
	S32 nSuccessRate = atoi( Con::executef("GetEquipPunchHoleSuceessRate", Con::getIntArg(nHolesNum)) );
	if (Platform::getRandomI(1, 10000) < nSuccessRate)
	{
		if (nHolesNum == 2)
		{
			int test = 2;
			test = 3;
		}
		//打孔成功
		//打孔成功,设置装备EmbedOpened标志
		stItemInfo *pEquipInfo = pEquipItem->getRes()->getExtData();
		pEquipInfo->EmbedOpened |= BIT(nHoleIndex);

		//更新装备槽及物品栏对应槽到客户端
		S32 nlockedIndex = pEquipItem->getLockedItemIndex();
		if (nlockedIndex == -1)
			return MSG_NONE;

		GameConnection *conn = pPlayer->getControllingClient();
		ItemShortcut *pSrcEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(nlockedIndex));
		if (!conn || !pSrcEquipItem)
			return MSG_NONE;

		pSrcEquipItem->getRes()->getExtData()->EmbedOpened = pEquipInfo->EmbedOpened;
		pPlayer->inventoryList.UpdateToClient(conn, nlockedIndex, ITEM_NOSHOW);
		pPlayer->punchHoleList.UpdateToClient(conn, 0, ITEM_NOSHOW);

		//发送消息通知客户端成功	
		ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_ITEM_PUNCHHOLE);
		if (ev)
		{
			ev->SetInt32ArgValues(2, 2, nHoleIndex);
			conn->postNetEvent(ev);
		}
	}
	else
	{
		//发送消息通知客户端失败
		GameConnection *conn = pPlayer->getControllingClient();
		ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_ITEM_PUNCHHOLE);
		if (conn && ev)
		{
			ev->SetInt32ArgValues(2, 3, nHoleIndex);
			conn->postNetEvent(ev);
		}
		//打孔失败
		return MSG_ITEM_PUNCHHOLE_FAIL;
	}

	return MSG_NONE;
}

enWarnMessage ItemManager::PunchHoleMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = NULL;
	if( param == NULL || (pPlayer = param->player) == NULL ||
		param->SrcIndex != 0)		//只有装备栏位可以拖动
		return msg;

	ItemShortcut *pSrcItem = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(param->SrcIndex));
	if (pSrcItem == NULL)
		return MSG_ITEM_ITEMDATAERROR;

	S32 nLockIndex = pSrcItem->getLockedItemIndex();
	if (nLockIndex >= 0)
	{
		GameConnection *conn = pPlayer->getControllingClient();
		if (conn)
		{
			ShortcutObject *pShortcut = pPlayer->inventoryList.GetSlot(nLockIndex);
			if (pShortcut)
			{
				pShortcut->setSlotState(ShortcutObject::SLOT_COMMON);
				pPlayer->inventoryList.UpdateToClient(conn, nLockIndex, ITEM_NOSHOW);
			}
			pPlayer->punchHoleList.SetSlot(param->SrcIndex, NULL);
			pPlayer->punchHoleList.UpdateToClient(conn, 0, ITEM_NOSHOW);
		}	
	}

	return MSG_NONE;
}

enWarnMessage ItemManager::InventoryMoveToPunchHole(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player *pPlayer = NULL;
	if( param == NULL || (pPlayer = param->player) == NULL ||
		param->DestIndex != 0 ||		//只有装备栏位可以拖动
		!pPlayer->inventoryList.IsVaildSlot(param->SrcIndex) ||
		!pPlayer->punchHoleList.IsVaildSlot(param->DestIndex) )
		return msg;

	ItemShortcut *pSrcItem = dynamic_cast<ItemShortcut *>(pPlayer->inventoryList.GetSlot(param->SrcIndex));
	if (pSrcItem == NULL)
		return MSG_ITEM_ITEMDATAERROR;
	Res *pSrcRes = pSrcItem->getRes();
	if (pSrcRes == NULL)
		return MSG_ITEM_ITEMDATAERROR;

	msg = EquipPunchHole::checkMoveFromInventory(pPlayer, pSrcItem, param->DestIndex);
	if (msg != MSG_NONE)
		return msg;

	ItemShortcut *pDestItem = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(param->DestIndex));
	ItemShortcut *pNewEquipItem = ItemShortcut::CreateItem(pSrcItem);
	if (!pNewEquipItem)
		return MSG_ITEM_ITEMDATAERROR;
	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return MSG_ITEM_ITEMDATAERROR;

	if (pDestItem)
	{
		//解除原物品栏对应槽的锁定
		S32 nLockedIndex = pDestItem->getLockedItemIndex();
		if (nLockedIndex != -1)
		{
			pPlayer->inventoryList.GetSlot(nLockedIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
			pPlayer->inventoryList.UpdateToClient(conn, nLockedIndex, ITEM_NOSHOW);
		}
	}
	pNewEquipItem->setLockedItemIndex(param->SrcIndex);
	pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);
	pPlayer->punchHoleList.SetSlot(0, pNewEquipItem);
	pPlayer->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
	pPlayer->punchHoleList.UpdateToClient(conn, 0, ITEM_NOSHOW);

	return MSG_NONE;
}
#endif

#ifdef NTJ_SERVER
ConsoleMethod(Player, OpenEquipPunchHoleDialog, void, 3, 3, "%player.OpenEquipPunchHoleDialog(%npc);")
{
	NpcObject *pNpc = dynamic_cast<NpcObject *>( Sim::findObject(atoi(argv[2])) );

	if (!pNpc)
		return;

	EquipPunchHole::openPunchHole(object, pNpc);
}
#endif

#ifdef NTJ_CLIENT
ConsoleFunction (EquipPunchHole_SendCloseMsg, void, 1, 1, "EquipPunchHole_SendCloseMsg();")
{
	GameConnection *conn = GameConnection::getConnectionToServer();
	if (!conn)
		return;

	//发送消息到服务端关闭打孔界面
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_ITEM_PUNCHHOLE);
	ev->SetInt32ArgValues(1, 1);	// 1：关闭
	if (ev)
	{
		conn->postNetEvent(ev);
	}
}

ConsoleFunction (EquipPunchHole_SendDoPunchHoleMsg, void, 1, 1, "EquipPunchHole_SendDoPunchHoleMsg();")
{
	GameConnection *conn = GameConnection::getConnectionToServer();
	if (!conn)
		return;

	//发送消息到服务端执行打孔操作
	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_ITEM_PUNCHHOLE);
	ev->SetInt32ArgValues(1, 2);	// 2：打孔
	if (ev)
	{
		conn->postNetEvent(ev);
	}
}

ConsoleFunction (EquipPunchHole_CanPunchHole, bool, 1, 1, "EquipPunchHole_CanPunchHole();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;

	ItemShortcut *pEquip = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(0));
	if (!pEquip)
		return 0;

	S32 nMoney = 0;
	S32 nHoleIndex = 0;

	enWarnMessage msg = EquipPunchHole::canPunchHole(pPlayer, pEquip, nMoney, nHoleIndex);
	if ( msg == MSG_NONE)
		return true;

	return false;
}

ConsoleFunction (EquipPunchHole_GetLeftNum, S32, 1, 1, "EquipPunchHole_GetLeftNum();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return 0;
		
	ItemShortcut *pEquip = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(0));
	if (!pEquip)
		return 0;

	return pEquip->getRes()->getLeftHoles();
}

ConsoleFunction (EquipPunchHole_HaveEquipItem, bool, 1, 1, "EquipPunchHole_HaveEquipItem();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;

	ItemShortcut *pEquip = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(0));

	return (pEquip != NULL);
}

ConsoleFunction (EquipPunchHole_GetCostMoney, S32, 1, 1, "EquipPunchHole_GetCostMoney();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;

	ItemShortcut *pEquip = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(0));

	return EquipPunchHole::CalculatePunchMoney(pPlayer, pEquip);
}

ConsoleFunction (EquipPunchHole_GetColorType, S32, 2, 2, "EquipPunchHole_GetColorType(%nHoleIndex);")
{
	S32 nHoleIndex = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer || nHoleIndex < 0 || nHoleIndex >= 3)
		return -1;

	ItemShortcut *pEquip = dynamic_cast<ItemShortcut *>( pPlayer->punchHoleList.GetSlot(0) );
	if (!pEquip)
		return -1;

	U32 nAllowFlag = pEquip->getRes()->getAllowEmbedSlot();
	U8 color = (nAllowFlag >> (4 * nHoleIndex)) & 0xF;

	return color;
}
#endif