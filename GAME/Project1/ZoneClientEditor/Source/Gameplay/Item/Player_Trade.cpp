//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "gameplay/GameEvents/ClientGameNetEvents.h"
#include "T3D/gameConnection.h"
#include "Gameplay/Item/Res.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Util/LocalString.h"
#endif

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#endif

TradeList::TradeList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_TRADE;
}

// ----------------------------------------------------------------------------
// ������ҽ��������ݵ��ͻ���
bool TradeList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
	if(!conn)
		return false;
	TradeEvent* ev = new TradeEvent(index, flag);
	conn->postNetEvent(ev);
	return true;
}

// ----------------------------------------------------------------------------
// ������ҽ��������ݵ��Է���ҿͻ���
bool TradeList::UpdateToTarget(Player* source, Player* target, S32 index)
{
	if(!source || !target)
		return false;
	GameConnection* targetConn = target->getControllingClient();
	if(targetConn)
	{
		TradeTargetEvent* ev = new TradeTargetEvent(source->getPlayerID(), index);
		targetConn->postNetEvent(ev);
	}
	return true;
}

// ----------------------------------------------------------------------------
//���������Ʒ����
void TradeList::Reset()
{
	for( S32 i = 0; i < mMaxSlots; i++)
	{
		if(mSlots[i])
		{
			delete mSlots[i];
			mSlots[i] = NULL;
		}
	}
	mMoney = 0;
}

#ifdef NTJ_SERVER
// ----------------------------------------------------------------------------
// �ƶ���Ʒ�����󵽽�����λ��
enWarnMessage ItemManager::InventoryMoveToTrade(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(player->isBusy(Player::INTERACTION_PLAYERTRADE))
		return MSG_TRADE_SELF_BUSY;

	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
		!player->tradeList.IsVaildSlot(param->DestIndex))
		return msg;

	ShortcutObject* pSrc	= player->inventoryList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->tradeList.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;

	ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrc);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;

	SceneObject* pTargetObj = player->getInteraction();
	Player* pTarget = NULL;
	if(!pTargetObj || !(pTarget = dynamic_cast<Player*>(pTargetObj)))
		return MSG_TRADE_LOSTTARGET;

	// ע�����κ�һ���С�ȷ�������׺󣬼Ӹ�������ʩ�����ܲ�����Ʒ
	if(player->tradeList.GetTradeState() == TRADE_END || pTarget->tradeList.GetTradeState() == TRADE_END)
		return MSG_NONE;

	//�ж���Ʒ�Ƿ�ɽ���
	if(!pSrcItem->getRes()->canTrade() || pSrcItem->getSlotState() == ShortcutObject::SLOT_LOCK)
		return MSG_ITEM_CANNOTTRADE;

	// �ж��Ƿ����������ף������Ƚ������
	msg = unLockPlayerTrade(player);
	if(msg != MSG_NONE)
		return msg;
	
	if(pDest)
	{
		ItemShortcut* pDestItem = dynamic_cast<ItemShortcut*>(pDest);
		if(!pDestItem)
			return MSG_ITEM_ITEMDATAERROR;
		U32 index = pDestItem->getLockedItemIndex();
		ShortcutObject* pDestFromItem = player->inventoryList.GetSlot(index);
		if(!pDestFromItem || pDestFromItem->getSlotState() == ShortcutObject::SLOT_COMMON)
			return MSG_ITEM_ITEMDATAERROR;			
		pDestFromItem->setSlotState(ShortcutObject::SLOT_COMMON);
		player->inventoryList.UpdateToClient(player->getControllingClient(), index, ITEM_RETAKETRADE);
	}

	ItemShortcut* pItem = ItemShortcut::CreateItem(pSrcItem);
	if(!pItem)
		return MSG_ITEM_ITEMDATAERROR;
	pItem->setLockedItemIndex(param->SrcIndex);
	pSrcItem->setSlotState(ShortcutObject::SLOT_LOCK);
	player->tradeList.SetSlot(param->DestIndex, pItem);
	player->tradeList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_PUTTRADE);
	player->tradeList.UpdateToTarget(player, pTarget, param->DestIndex);
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �ƶ�������������Ʒ��λ��
enWarnMessage ItemManager::TradeMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(player->isBusy(Player::INTERACTION_PLAYERTRADE))
		return MSG_TRADE_SELF_BUSY;

	if(!player->tradeList.IsVaildSlot(param->SrcIndex) ||
		!player->inventoryList.IsVaildSlot(param->DestIndex))
		return msg;

	ShortcutObject* pSrc	= player->tradeList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->inventoryList.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;

	ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrc);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;

	SceneObject* pTargetObj = player->getInteraction();
	Player* pTarget = NULL;
	if(!pTargetObj || !(pTarget = dynamic_cast<Player*>(pTargetObj)))
		return MSG_TRADE_LOSTTARGET;

	// ע�����κ�һ���С�ȷ�������׺󣬼Ӹ�������ʩ�����ܲ�����Ʒ
	if(player->tradeList.GetTradeState() == TRADE_END || pTarget->tradeList.GetTradeState() == TRADE_END)
		return MSG_NONE;

	// �ж��Ƿ����������ף������Ƚ������
	msg = unLockPlayerTrade(player);
	if(msg != MSG_NONE)
		return msg;

	U32 mSrcIndex = pSrcItem->getLockedItemIndex();
	if(pDest)
	{
		// �ж��Ƿ��Ƶ��Լ���������Ʒ��λ��
		if(mSrcIndex == param->DestIndex)
		{
			pDest->setSlotState(ShortcutObject::SLOT_COMMON);
			player->tradeList.SetSlot(param->SrcIndex, NULL);
		}
		else
		{			
			ShortcutObject* pDestSlot = player->inventoryList.GetSlot(mSrcIndex);
			if(!pDestSlot)
				return MSG_ITEM_ITEMDATAERROR;
			pDestSlot->setSlotState(ShortcutObject::SLOT_COMMON);
			pDest->setSlotState(ShortcutObject::SLOT_LOCK);

			ItemShortcut* pItem = ItemShortcut::CreateItem((ItemShortcut*)pDest);
			if(!pItem)
				return MSG_ITEM_ITEMDATAERROR;
			pItem->setLockedItemIndex(param->DestIndex);
			player->tradeList.SetSlot(param->SrcIndex, pItem);
		}
		player->inventoryList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);
	}
	else
	{
		ShortcutObject* pDestSlot = player->inventoryList.GetSlot(mSrcIndex);
		if(!pDestSlot)
			return MSG_ITEM_ITEMDATAERROR;
		pDestSlot->setSlotState(ShortcutObject::SLOT_COMMON);
		player->tradeList.SetSlot(param->SrcIndex, NULL);		
	}
	player->tradeList.UpdateToClient(player->getControllingClient(), param->SrcIndex, TRADE_DOWNITEM);
	player->tradeList.UpdateToTarget(player, pTarget, param->SrcIndex);
	player->inventoryList.UpdateToClient(player->getControllingClient(), mSrcIndex, ITEM_NOSHOW);	
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �����Ʒ����Ʒ��������
enWarnMessage ItemManager::addItemToTrade(Player* player, S32 slotIndex)
{
	if(player->isBusy(Player::INTERACTION_PLAYERTRADE))
		return MSG_TRADE_SELF_BUSY;

	Player* pTarget = dynamic_cast<Player*>(player->getInteraction());
	enWarnMessage MsgCode = g_ItemManager->canTrade(player, pTarget);
	if(MsgCode != MSG_NONE)
		return MsgCode;

	// �ж���λ״̬�Ƿ�Ϊ����
	ShortcutObject* pSrcSlot = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_INVENTORY, slotIndex);
	if (!pSrcSlot || pSrcSlot->getSlotState() == ShortcutObject::SLOT_LOCK)
		return MSG_ITEM_ITEMBEUSED;		// ��Ʒ��ʹ���У��޷�����

	// �жϽ���״̬�Ƿ�Trade_Begin
	if(player->tradeList.GetTradeState() != TRADE_BEGIN)
		return MSG_TRADE_SELF_STATELOCK;

	U32 index = player->tradeList.FindEmptySlot();
	if (index == -1)
		return MSG_TRADE_NOSPACE;

	pSrcSlot->setSlotState(ShortcutObject::SLOT_LOCK);
	player->tradeList.SetSlot(index, g_ItemManager->cloneItem(pSrcSlot));
	player->tradeList.GetSlot(index)->setLockedItemIndex(slotIndex);
	player->tradeList.UpdateToClient(player->getControllingClient(), index, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(player->getControllingClient(), slotIndex, ITEM_NOSHOW);
	player->tradeList.UpdateToTarget(player, pTarget, index);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// �ӽ�����ɾ����Ʒ��������Ʒ��
enWarnMessage ItemManager::delItemFromTrade(Player* player, S32 slotIndex)
{
	if(player->isBusy(Player::INTERACTION_PLAYERTRADE))
		return MSG_TRADE_SELF_BUSY;

	Player* pTarget = dynamic_cast<Player*>(player->getInteraction());
	enWarnMessage MsgCode = g_ItemManager->canTrade(player, pTarget);
	if(MsgCode != MSG_NONE)
		return MsgCode;

	// �жϽ���״̬�Ƿ�Trade_Begin
	if(player->tradeList.GetTradeState() != TRADE_BEGIN)
		return MSG_TRADE_SELF_STATELOCK;

	// �ж���λ״̬�Ƿ�Ϊ����
	ShortcutObject* pSlot = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_TRADE, slotIndex);
	if (!pSlot)
		return MSG_TRADE_TARGETITEMNOEXIST;

	U32 iSrcIndex = pSlot->getLockedItemIndex();
	ShortcutObject* pSrcSlot = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_INVENTORY, iSrcIndex);
	if (!pSrcSlot || pSrcSlot->getSlotState() != ShortcutObject::SLOT_LOCK)
		return MSG_ITEM_ITEMDATAERROR;
	
	pSrcSlot->setSlotState(ShortcutObject::SLOT_COMMON);
	player->tradeList.SetSlot(slotIndex, NULL, true);
	player->inventoryList.UpdateToClient(player->getControllingClient(), iSrcIndex, ITEM_NOSHOW);
	player->tradeList.UpdateToClient(player->getControllingClient(), slotIndex, ITEM_NOSHOW);
	player->tradeList.UpdateToTarget(player, pTarget, slotIndex);
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ��ҽ�����Ʒ
enWarnMessage ItemManager::exchangeItemForTrade(Player* player, Player* target)
{
	Vector<stItemResult*> player_ItemList,target_ItemList,playerList,targetList;
	enWarnMessage msgCode = MSG_TRADE_FAILED;
	if(!player || !target || player->getInteraction() != target || target->getInteraction() != player)
		goto ClearAll;
	
	// �Լ��Ľ�����Ʒ������Ʒ�б�
	for(S32 i = 0; i < player->tradeList.GetMaxSlots(); i++)
	{
		ItemShortcut* pItem = (ItemShortcut*)player->tradeList.GetSlot(i);
		if(!pItem)
			continue;
		//ע:�Լ�����Ʒ�ǿۼ����Է�����Ʒ�����
		stItemResult* pSrc	= new stItemResult;
		pSrc->srcEntry		= pItem;
		pSrc->newEntry		= NULL;
		pSrc->PlayerID		= player->getPlayerID();
		pSrc->flag			= OP_NONE;
		pSrc->num			= 0-pItem->getRes()->getQuantity();
		player_ItemList.push_back(pSrc);
		stItemResult* pTag	= new stItemResult;
		pTag->srcEntry		= pItem;
		pTag->newEntry		= NULL;
		pTag->PlayerID		= player->getPlayerID();
		pTag->flag			= OP_NONE;
		pTag->num			= pItem->getRes()->getQuantity();
		target_ItemList.push_back(pTag);
	}

	// �Է��Ľ�����Ʒ������Ʒ�б�
	for(S32 i = 0; i < target->tradeList.GetMaxSlots(); i++)
	{
		ItemShortcut* pItem = (ItemShortcut*)target->tradeList.GetSlot(i);
		if(!pItem)
			continue;
		//ע:�Լ�����Ʒ����ӣ��Է���Ʒ�ǿۼ�
		stItemResult* pSrc	= new stItemResult;
		pSrc->srcEntry		= pItem;
		pSrc->newEntry		= NULL;
		pSrc->PlayerID		= target->getPlayerID();
		pSrc->flag			= OP_NONE;
		pSrc->num			= pItem->getRes()->getQuantity();
		player_ItemList.push_back(pSrc);
		stItemResult* pTag	= new stItemResult;
		pTag->srcEntry		= pItem;
		pTag->newEntry		= NULL;
		pTag->PlayerID		= target->getPlayerID();
		pTag->flag			= OP_NONE;
		pTag->num			= 0-pItem->getRes()->getQuantity();
		target_ItemList.push_back(pTag);
	}

	// ����Ƿ���Ϊ�Լ���������������Ʒ
	if(detectPutItem(player, player_ItemList, playerList, true) != MSG_NONE)
	{
		msgCode = MSG_TRADE_SELF_BAGNOSPACE;
		goto ClearAll;
	}

	// ����Ƿ���Ϊ�Է�����Լ��Ľ�����Ʒ
	if(detectPutItem(target, target_ItemList, targetList, true) != MSG_NONE)
	{
		msgCode = MSG_TRADE_TARGET_BAGNOSPACE;
		goto ClearAll;
	}

	S32 player_money, target_money;
	player_money = player->tradeList.GetTradeMoney();
	target_money = target->tradeList.GetTradeMoney();
	//�ж��Ƿ�����Է����׽�Ǯ
	if(player->canTradeMoney(target, player_money, target_money))
	{
		if(player_money > 0)
		{
			player->reduceMoney(player_money, Player::Currentcy_Money);
			target->addMoney(player_money, Player::Currentcy_Money);
		}
		if(target_money > 0)
		{
			target->reduceMoney(target_money, Player::Currentcy_Money);
			player->addMoney(target_money, Player::Currentcy_Money);
		}
	}
	else
	{
		if(player_money > 0 || target_money > 0)
		{
			msgCode = MSG_PLAYER_MONEYNOTENOUGH;
			goto ClearAll;
		}
	}

	//���׶Է���Ʒ���Լ�
	donePutItem(player, playerList);
	//�����Լ���Ʒ���Է�
	donePutItem(target, targetList);

	// ����Լ���Ʒ��������Ʒ������
	for(S32 i = 0; i < player->tradeList.GetMaxSlots(); ++i)
	{
		ItemShortcut* pItem = (ItemShortcut*)player->tradeList.GetSlot(i);
		S32 iSrcIndex  = -1;
		if(pItem && (iSrcIndex = pItem->getLockedItemIndex()) != -1)
		{
			ItemShortcut* pSrc = (ItemShortcut*)player->inventoryList.GetSlot(iSrcIndex);
			if(pSrc && pSrc->getSlotState() == ShortcutObject::SLOT_LOCK)
			{
				pSrc->setSlotState(ShortcutObject::SLOT_COMMON);
				player->inventoryList.UpdateToClient(player->getControllingClient(), iSrcIndex, ITEM_NOSHOW);
			}			
		}
	}

	// ����Է���Ʒ��������Ʒ������
	for(S32 i = 0; i < target->tradeList.GetMaxSlots(); ++i)
	{
		ItemShortcut* pItem = (ItemShortcut*)target->tradeList.GetSlot(i);
		S32 iSrcIndex = -1;
		if(pItem && (iSrcIndex = pItem->getLockedItemIndex()) != -1)
		{
			ItemShortcut* pSrc = (ItemShortcut*)target->inventoryList.GetSlot(iSrcIndex);
			if(pSrc && pSrc->getSlotState() == ShortcutObject::SLOT_LOCK)
			{
				pSrc->setSlotState(ShortcutObject::SLOT_COMMON);
				target->inventoryList.UpdateToClient(target->getControllingClient(), iSrcIndex, ITEM_NOSHOW);
			}			
		}
	}
	msgCode = MSG_NONE;
ClearAll:
	for(S32 i = 0; i < player_ItemList.size(); i++)
	{
		if(player_ItemList[i])
			delete player_ItemList[i];
	}

	for(S32 i = 0; i < target_ItemList.size(); i++)
	{
		if(target_ItemList[i])
			delete target_ItemList[i];
	}
	player_ItemList.clear();
	target_ItemList.clear();
	// �����ҽ���״̬
	player->setInteraction(NULL, Player::INTERACTION_NONE);
	return msgCode;
}

// ----------------------------------------------------------------------------
// ��������������
enWarnMessage ItemManager::unLockPlayerTrade(Player* player)
{
	if(!player)
		return MSG_TRADE_LOSTTARGET;
	if(player->tradeList.GetTradeState() == TRADE_END)
		return MSG_TRADE_SELF_TRADING;	// �Լ��Ѿ�ȷ�����ף��޷��������

	SceneObject* pTargetObject = player->getInteraction();
	Player* pTarget = NULL;
	if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
		return MSG_TRADE_LOSTTARGET;

	if(pTarget->tradeList.GetTradeState() == TRADE_END)
		return MSG_TRADE_TARGET_TRADING; // �Է�ȷ�����ף��޷��������

	if(player->tradeList.GetTradeState() == TRADE_LOCK)
	{
		player->tradeList.SetTradeState(TRADE_BEGIN);
		if(pTarget->getControllingClient())
		{
			ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
			e->SetInt32ArgValues(1,	TRADE_UNLOCKNOTIFY);
			pTarget->getControllingClient()->postNetEvent(e);
		}

		if(player->getControllingClient())
		{
			ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
			e->SetInt32ArgValues(1,	TRADE_UNLOCK);
			player->getControllingClient()->postNetEvent(e);
		}		
	}
	return MSG_NONE;
}

#endif//NTJ_SERVER

// ----------------------------------------------------------------------------
// �ر���ҽ���
bool ItemManager::closePlayerTrade(Player* player)
{
	if(!player)
		return false;
	player->tradeList.Reset();
	player->tradeList.SetTradeState(TRADE_NONE);
#ifdef NTJ_SERVER
	player->inventoryList.Unlock(player->getControllingClient());
#endif
#ifdef NTJ_CLIENT
	Con::executef("CloseTradeWnd");
#endif//NTJ_CLIENT
	return true;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ��ܽ���
enWarnMessage ItemManager::canTrade(Player* player, Player* target)
{
	if(player == NULL || target == NULL)
		return MSG_INVALIDPARAM;				// ��ǰ�����޷�����

	// �ж�˫����������
	if(!player->canInteraction(target))
		return MSG_PLAYER_TOOFAR;				// ��������̫Զ

	// �ж�����Լ����׿����Ƿ��
	if(player->getSwitchState(Player::SWITCHSTATE_TRADE))
		return MSG_TRADE_SELF_SWITCHCLOSE;		// ����Լ����׿���δ��

	// �ж�����Լ��ĵȼ�����
	if(player->getLevel() <= ItemManager::TRADE_LEVEL_LIMIT)
		return MSG_TRADE_SELF_LEVEL;			// ����Լ��ȼ�����

	// �ж�����Լ��Ƿ�Ʋ�����
	if(player->getTradeFlag() & Player::TradeLock_Trade)
		return MSG_TRADE_SELF_TRADELOCK;		// ����Լ��Ʋ�����

	// �ж�����Լ��Ľ��׶����Ƿ�Է�
	if((player->getInteractionState() == Player::INTERACTION_PLAYERTRADE) &&
		(player->getInteraction() != target))
		return MSG_TRADE_SELF_BUSY;				// ����Լ���æ���޷�����

	// �ж�����Լ��Ƿ�ǰ��æ
	if (player->isBusy(Player::INTERACTION_PLAYERTRADE) != MSG_NONE)
		return MSG_TRADE_SELF_BUSY;				// ����Լ���æ���޷�����

	// �ж϶Է��Ľ��׿����Ƿ��
	if(target->getSwitchState(Player::SWITCHSTATE_TRADE))
		return MSG_TRADE_TARGET_SWITCHCLOSE;	// �Է����׿���δ��

	// �ж϶Է��ȼ�����
	if(target->getLevel() <= ItemManager::TRADE_LEVEL_LIMIT)
		return MSG_TRADE_TARGET_LEVEL;			// �Է���ҵȼ�����

	// �ж϶Է��Ƿ�Ʋ�����
	if(target->getTradeFlag() & Player::TradeLock_Trade)
		return MSG_TRADE_TARGET_TRADELOCK;		// �Է���ҲƲ�����

	// �ж϶Է��Ľ��׶����Ƿ��Լ�
	if((target->getInteractionState() == Player::INTERACTION_PLAYERTRADE) &&
		(target->getInteraction() != player))
		return MSG_TRADE_TARGET_BUSY;			// �Է������æ���޷�����

	// �ж϶Է��Ƿ�ǰ��æ
	if (target->isBusy(Player::INTERACTION_PLAYERTRADE) != MSG_NONE)
		return MSG_TRADE_TARGET_BUSY;			// �Է������æ���޷�����
	return MSG_NONE;
}

#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// �������뷽--���뽻��
ConsoleFunction(applyTrade, void, 1, 1 ,"applyTrade()")
{
	GameObject* targetObj = g_ClientGameplayState->GetTarget();
	Player* target = NULL;
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (targetObj && (target = dynamic_cast<Player*>(targetObj)))
	{
		enWarnMessage msgCode = g_ItemManager->canTrade(player, target);
		if(msgCode != MSG_NONE)
		{
			MessageEvent::show(SHOWTYPE_ERROR, msgCode);
			return;
		}

		//�������뽻����Ϣ
		GameConnection *conn = dynamic_cast<GameConnection *>(GameConnection::getConnectionToServer());
		if (conn)
		{
			ClientGameNetEvent* e = new ClientGameNetEvent(INFO_TRADE);
			e->SetInt32ArgValues(2, TRADE_ASKFOR, target->getPlayerID());
			conn->postNetEvent(e);
		}
	}
}

// ----------------------------------------------------------------------------
// ���ױ����뷽--ͬ�⽻��
ConsoleFunction(agreeTrade, void, 2, 2, "agreeTrade(%eid)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	Player::stLogicEvent* ev = player->findLogicEvent(dAtoi(argv[1]));
	if(ev == NULL || Platform::getVirtualMilliseconds() - ev->start > Player::MAX_LOGIC_REPLYTIME)
	{
		MessageEvent::show(SHOWTYPE_ERROR, MSG_TRADE_INVITE_TIMEOUT);
		return;
	}

	//����ͬ�⽻����Ϣ
	GameConnection *conn = dynamic_cast<GameConnection *>(GameConnection::getConnectionToServer());
	if (conn)
	{		
		ClientGameNetEvent* e = new ClientGameNetEvent(INFO_TRADE);
		e->SetInt32ArgValues(3, TRADE_ANSWER, ev->sender, ev->relationid);
		conn->postNetEvent(e);
	}
}

// ----------------------------------------------------------------------------
// ���ױ����뷽--�ܾ�����
ConsoleFunction(rejectTrade,void,2,2,"rejectTrade(%eid)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player)
		return;
	Player::stLogicEvent* ev = player->findLogicEvent(dAtoi(argv[1]));
	if(ev == NULL || Platform::getVirtualMilliseconds() - ev->start > Player::MAX_LOGIC_REPLYTIME)
		return;

	//����ͬ�⽻����Ϣ
	GameConnection *conn = dynamic_cast<GameConnection *>(GameConnection::getConnectionToServer());
	if (conn)
	{		
		ClientGameNetEvent* e = new ClientGameNetEvent(INFO_TRADE);
		e->SetInt32ArgValues(3, TRADE_REJECT, ev->sender, ev->relationid);
		conn->postNetEvent(e);
	}
}

// ----------------------------------------------------------------------------
// ��������
ConsoleFunction(lockTrade,bool, 2,2,"LockTrade(%trademoney)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(player && player->tradeList.GetTradeState() == TRADE_BEGIN)
	{
		S32 iTradeMoney = dAtoi(argv[1]);
		if(iTradeMoney > player->getMoney(Player::Currentcy_Money))
		{
			MessageEvent::show(SHOWTYPE_ERROR, MSG_TRADE_NOENOUGHMONEY);
			return false;
		}
		else
		{
			GameConnection *conn = GameConnection::getConnectionToServer();
			if (conn)
			{
				ClientGameNetEvent* e = new ClientGameNetEvent(INFO_TRADE);
				e->SetInt32ArgValues(2, TRADE_LOCK, iTradeMoney);
				conn->postNetEvent(e);
				return true;
			}
		}		
	}
	return false;
}

// ----------------------------------------------------------------------------
// ��ɽ���
ConsoleFunction(endTrade, void,1,1,"endTrade()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (player)
	{
		if(player->tradeList.GetTradeState() != TRADE_LOCK)
		{
			Con::warnf("��δ�������ף�����ȷ�Ͻ���!");
			return;
		}

		GameConnection *conn = GameConnection::getConnectionToServer();
		if (conn)
		{
			ClientGameNetEvent* e = new ClientGameNetEvent(INFO_TRADE);
			e->SetInt32ArgValues(1, TRADE_END);
			conn->postNetEvent(e);
			return;
		}
	}
}

// ----------------------------------------------------------------------------
// ȡ������
ConsoleFunction(cancelTrade,void,1,1,"cancelTrade()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (player)
	{
		GameConnection *conn = GameConnection::getConnectionToServer();
		if (conn)
		{
			ClientGameNetEvent* e = new ClientGameNetEvent(INFO_TRADE);
			e->SetInt32ArgValues(1, TRADE_CANCEL);
			conn->postNetEvent(e);
			return;
		}
	}
}

// ----------------------------------------------------------------------------
// ȡ����������
ConsoleFunction(cancelLockTrade,void,1,1,"cancelLockTrade()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (player)
	{
		GameConnection *conn = GameConnection::getConnectionToServer();
		if (conn)
		{
			ClientGameNetEvent* e = new ClientGameNetEvent(INFO_TRADE);
			e->SetInt32ArgValues(1, TRADE_UNLOCK);
			conn->postNetEvent(e);
			return;
		}
	}
}
#endif//NTJ_CLIENT

// ----------------------------------------------------------------------------
// ���ý��׿���
ConsoleMethod(Player, setTradeSwitch, void, 3, 3, "%player.setTradeSwitch(%IsOpen)")
{
	bool IsOpen = atoi(argv[2]);
#ifdef NTJ_CLIENT	
	GameConnection *conn = dynamic_cast<GameConnection *>(GameConnection::getConnectionToServer());
	if (conn)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_SWITCH);
		pEvent->SetIntArgValues(2, Player::SWITCHSTATE_TRADE, IsOpen);
		conn->postNetEvent(pEvent);
	}
#endif
#ifdef NTJ_SERVER
	object->setSwitchState(Player::SWITCHSTATE_TRADE, IsOpen);
#endif
}