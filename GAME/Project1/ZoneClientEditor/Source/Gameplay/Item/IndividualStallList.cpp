#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/item/IndividualStall.h"
#include "T3D/trigger.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/Item/ItemBaseData.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
//#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/ServerGameplayState.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

IndividualStallList::IndividualStallList()
{
	mMaxSlot = 12;
	mStallInfor = StringTable->insert("");
	mStallName = StringTable->insert("");
	//mChatInfor = StringTable->insert("");
	mSlots = new StallItem*[mMaxSlot];
	for(S32 i=0; i<mMaxSlot; ++i)
		mSlots[i] = NULL;
	mState = TRADE_NONE;
	VECTOR_SET_ASSOCIATION(mTradeInfoList);
	VECTOR_SET_ASSOCIATION(mChatInfoList);
}

IndividualStallList::~IndividualStallList()
{
	clear();
	delete [] mSlots;
	clearChatList();
	clearTradeList();
}

void IndividualStallList::clear()
{
	for(int i=0; i<getMaxSlot(); ++i)
	{
		if(mSlots[i])
		{
			if(mSlots[i]->pItem)
			{
				delete mSlots[i]->pItem;
				mSlots[i]->pItem = NULL;
			}
			delete mSlots[i];
		}
		mSlots[i] = NULL;
	}
}

void IndividualStallList::clearChatList()
{
	for(int i=0; i<mChatInfoList.size(); ++i)
	{
		if (mChatInfoList[i])
		{
			SAFE_DELETE_ARRAY(mChatInfoList[i]->chatContent)
			delete mChatInfoList[i];
			mChatInfoList[i] = NULL;
		}
	}
	mChatInfoList.clear();
}

void IndividualStallList::clearTradeList()
{
	for(int i=0; i<mTradeInfoList.size(); ++i)
	{
		if(mTradeInfoList[i])
		{
			if(mTradeInfoList[i]->type == stTradeRecord::TRADE_ITEM)
			{
				SAFE_DELETE(mTradeInfoList[i]->trade.item);
			}
			else
			{
				SAFE_DELETE(mTradeInfoList[i]->trade.pet);
			}
			delete mTradeInfoList[i];
			mTradeInfoList[i] = NULL;
		}		
	}
	mTradeInfoList.clear();
}

ItemShortcut* IndividualStallList::GetSlot(S32 index)
{
	if(index < 0 || index >= getMaxSlot())
		return NULL;
	return mSlots[index] ? mSlots[index]->pItem : NULL;
}

bool IndividualStallList::UpdateToClient(GameConnection* conn, S32 index)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	StallEvent* evt = new StallEvent(index);
	conn->postNetEvent(evt);
#endif
	return true;

}

bool IndividualStallList::UpdateTargetToClient(Player* player, Player* target, S32 index)
{
#ifdef NTJ_SERVER
	if(!player || !target)
		return false;
	GameConnection* targetConn = target->getControllingClient();
	if(targetConn)
	{
		StallTargetEvent* ev = new StallTargetEvent(player->getPlayerID(), index);
		targetConn->postNetEvent(ev);
	}
#endif
	return true;
}

void IndividualStallList::SetSlot(S32 index, ItemShortcut* pItem) 
{
	if(mSlots[index])
	{
		if(pItem)
		{
			if(mSlots[index]->pItem)
				delete mSlots[index]->pItem;
			mSlots[index]->pItem = pItem;
		}
		else
		{
			if(mSlots[index]->pItem)
				delete mSlots[index]->pItem;
			delete mSlots[index];
			mSlots[index] = NULL;
		}
		
	}
	else
	{
		StallItem* item = new StallItem;
		item->pItem = pItem;
		item->money = 0;
		mSlots[index] = item;
	}
}

void IndividualStallList::setStallMoney(S32 index, U32 money)
{
	if(mSlots[index])
		mSlots[index]->money = money;
	else
		return;
}

void IndividualStallList::setStallName(StringTableEntry name)
{
	if(name && name[0])
		mStallName = StringTable->insert(name);
	else
		mStallName = "\0";
}

U32 IndividualStallList::getStallMoney(S32 index)
{
	if(mSlots[index])
		return mSlots[index]->money;
	return 0;
}

StringTableEntry IndividualStallList::getStallName()
{
	return mStallName;
}

void IndividualStallList::setStallInfo(StringTableEntry info)
{
	if(info && info[0])
		mStallInfor = StringTable->insert(info);
	else
		mStallInfor = "\0";
}

StringTableEntry IndividualStallList::getStallInfo()
{
	return mStallInfor;
}

void IndividualStallList::SetMaxSlot(S32 maxslots)
{
	mMaxSlot = maxslots;
}

U32 IndividualStallList::getMaxSlot()
{
	return mMaxSlot;
}

void IndividualStallList::UpdateMaxSlots(GameConnection* conn)
{
#ifdef NTJ_SERVER
	if(conn)
	{
		ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_CHANGEMAXSLOTS);
		ev->SetInt32ArgValues(2, SHORTCUTTYPE_STALL, mMaxSlot);
		conn->postNetEvent(ev);

		for(S32 i = ItemManager::EXTBAG_BEGINSLOT; i < IndividualStallList::MAXSLOTS; i++)
		{
			StallEvent* ev = new StallEvent(i);
			conn->postNetEvent(ev);
		}
	}
#endif
}

enWarnMessage IndividualStallList::CanChangeMaxSlots(U32 srcBag1, U32 srcBag2, U32 tagBag1, U32 tagBag2)
{
	if(srcBag1 == tagBag1 && srcBag2 == tagBag2)
		return MSG_NONE;

	if(tagBag1 + tagBag2 > IndividualStallList::MAXSLOTS - ItemManager::EXTBAG_BANK_BEGINSLOT)
		return MSG_ITEM_BAGLIMIT;

	if(tagBag1 > 9 || tagBag2 > 9)
		return MSG_ITEM_BAGLIMIT;

	if(srcBag1 > tagBag1)
	{
		for(S32 i = ItemManager::EXTBAG_BANK_BEGINSLOT; i < ItemManager::EXTBAG_BANK_BEGINSLOT + srcBag1; i++)
			if(mSlots[i])
				return MSG_ITEM_BAGNOEMPTY1;
	}

	if(srcBag2 > tagBag2)
	{
		for(S32 i = ItemManager::EXTBAG_BANK_BEGINSLOT + srcBag1; i < ItemManager::EXTBAG_BANK_BEGINSLOT + srcBag1 + srcBag2; i++)
			if(mSlots[i])
				return MSG_ITEM_BAGNOEMPTY2;
	}
	return MSG_NONE;
}

void IndividualStallList::ChangeMaxSlots(Player*, U32 srcBag1, U32 srcBag2, U32 tagBag1, U32 tagBag2)
{
	U32 srcbags = srcBag1 + srcBag2;
	U32 tagbags = tagBag1 + tagBag2;
	U32 extBags = IndividualStallList::MAXSLOTS - ItemManager::EXTBAG_BANK_BEGINSLOT;
	StallItem** tmpSlots = new StallItem*[extBags];
	for(S32 i = 0; i < extBags; ++i)
	{
		if(i < srcbags)
			tmpSlots[i] = mSlots[ItemManager::EXTBAG_BANK_BEGINSLOT + i];
		else
			tmpSlots[i] = NULL;
	}

	//注：大包换小包必须大包为空包
	if(tagBag1 > srcBag1)
	{
		for(S32 i = ItemManager::EXTBAG_BANK_BEGINSLOT + srcBag1; i < ItemManager::EXTBAG_BANK_BEGINSLOT + srcbags; i++)
		{
			mSlots[i + (tagBag1 - srcBag1)] = tmpSlots[i - ItemManager::EXTBAG_BANK_BEGINSLOT];

			if(i < ItemManager::EXTBAG_BEGINSLOT + tagBag1)
				mSlots[i] = NULL;
		}
	}
	else if(tagBag1 < srcBag1)
	{
		for(S32 i = ItemManager::EXTBAG_BANK_BEGINSLOT + tagBag1; i < ItemManager::EXTBAG_BANK_BEGINSLOT + extBags; i++)
		{
			mSlots[i] = tmpSlots[i - ItemManager::EXTBAG_BANK_BEGINSLOT - tagBag1 + srcBag1];
			if(i >= ItemManager::EXTBAG_BANK_BEGINSLOT + srcbags)
				mSlots[i] = NULL;
		}
	}
	delete[] tmpSlots;
	SetMaxSlot(ItemManager::EXTBAG_BANK_BEGINSLOT + tagBag1 + tagBag2);	
}

StringTableEntry IndividualStallList::getStallPos()
{
	return "";
}

U32 IndividualStallList::getStallRate()
{
	return 0;
}

U32 IndividualStallList::getEmptyNum(Vector<U32>& empty)
{
	empty.clear();
	for(int i=0; i<getMaxSlot(); i++)
	{
		if(!mSlots[i] || !mSlots[i]->pItem)
		{
			empty.push_back(i);
		}
	}
	return empty.size();
}

S32 IndividualStallList::findEmptyIndex()
{
	for(int i=0; i<getMaxSlot(); ++i)
	{
		if(mSlots[i] == NULL || mSlots[i]->pItem == NULL)
			return i;
	}
	return -1;
}

S32 StallPetList::findEmptyIndex()
{
	for(int i=0; i<MAXSLOTS; ++i)
	{
		if(mSlots[i] == NULL || mSlots[i]->petSlot == NULL)
			return i;
	}
	return -1;
}

void IndividualStallList::addPlayerList(U32 playerID)
{
	for(int i=0; i<mPlayerList.size(); ++i)
	{
		if(mPlayerList[i] == playerID)
			return;
	}
	mPlayerList.push_back(playerID);
}

U32 IndividualStallList::getPlayerListID(U32 index)
{
	if(index < 0 || index > mPlayerList.size())
		return 0;
	else
		return mPlayerList[index];
}

void IndividualStallList::delPlayerList(U32 playerID)
{
	for(int i=0; i<mPlayerList.size(); ++i)
	{
		if(mPlayerList[i] == playerID)
			mPlayerList.erase_fast(i);
	}
}
//摆摊管理

IndividualStall* g_Stall;
IndividualStall gStall;
IndividualStall::IndividualStall()
{
	mType = stall_individual;
	g_Stall = this;
	clearLookUpList();
}
IndividualStall::~IndividualStall()
{
	clearLookUpList();
}

bool IndividualStall::canOpenStall(Player* pPlayer, U32 type)
{
	if(!pPlayer)
		return false;
	U32 triggerId = pPlayer->getTriggerId();
	if(triggerId > 0)
	{
		TriggerData*  data = g_TriggerDataManager->getTriggerData(triggerId);
		if(!data)
			return false;
		if(!data->getBaseLimit(TriggerData::Limit_Stall))
			return false;
	}

	if(pPlayer->isBusy(Player::INTERACTION_STALLTRADE) != MSG_NONE)
		return false;
	return true;
}

void IndividualStall::setType(U32 type)
{
	mType = type;
}

U32 IndividualStall::getType()
{
	return mType;
}

void IndividualStall::addToLookUpList(U32 itemID)
{
	if(itemID > 0)
		mLookUpList.push_back(itemID);
}

void IndividualStall::clearLookUpList()
{
	mLookUpList.clear();
}

void IndividualStall::openStall(Player* pPlayer, U32 type, U32 playerID)
{
	if(!pPlayer)
		return;
#ifdef NTJ_SERVER	
	GameConnection* con = pPlayer->getControllingClient();
	if(!con)
		return;
	if(canOpenStall(pPlayer, type))
	{
		char stallName[32] = "\0";
		dSprintf(stallName, sizeof(stallName), pPlayer->individualStallList.getStallName());
		if(stallName[0] == '\0')
		{
			dSprintf(stallName, sizeof(stallName), "%s%s", pPlayer->getObjectName(), "的摊位");
		}
		pPlayer->individualStallList.setStallName(stallName);
		
		ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
		evt->SetInt32ArgValues(3, TRADE_BEGIN, type, playerID);
		evt->SetStringArgValues(1, stallName);
		con->postNetEvent(evt);
		pPlayer->setInteraction(pPlayer, Player::INTERACTION_STALLTRADE);
		if(pPlayer->individualStallList.getStallState() != TRADE_LOCK)
		{
			//pPlayer->individualStallList.clear();
			pPlayer->individualStallList.setStallState(TRADE_BEGIN);
		}
		ShortcutObject* pShort = pPlayer->equipList.GetSlot(Res::EQUIPPLACEFLAG_STALLPACK3);
		if(pShort)
		{
			ItemShortcut* pItem = (ItemShortcut*)pShort;
			if(pItem)
			{
				//StringTableEntry modeName = pItem->getRes()->getLinkModelName(0, pPlayer->getSex());
				/*if(modeName && modeName[0] != 0)
					pPlayer->mShapeStack.add(ShapeStack::Stack_Stall, -1, modeName, NULL, 0, 0);
				else*/
					pPlayer->mShapeStack.add(ShapeStack::Stack_Stall,  404001);
			}
			else
				pPlayer->mShapeStack.add(ShapeStack::Stack_Stall,  404001);
		}
		else
			pPlayer->mShapeStack.add(ShapeStack::Stack_Stall,  404001);
		PetOperationManager::SetPetFreeze(pPlayer, true);
	}
	else
	{
		MessageEvent::send(con, SHOWTYPE_ERROR, MSG_TRADE_STALL_FAILD, SHOWPOS_SCREEN);
		return;
	}

#endif
#ifdef NTJ_CLIENT
	U32 myID = pPlayer->getPlayerID();
	U32 zoneID = g_ClientGameplayState->getCurrentZoneId();
	U32 rate = 0;
	U32 triggerID = pPlayer->getTriggerId();
	if(triggerID > 0)
	{
		TriggerData* data = g_TriggerDataManager->getTriggerData(triggerID);
		if(data)
			rate = data->mTradeTax;
	}

	if(playerID == myID)
	{
		//pPlayer->setRotation(0.0f, true);
		S32 x = pPlayer->getPosition().x;
		S32 y = pPlayer->getPosition().y;
		Con::executef("OpenStallWnd");
		Con::executef("initStallWnd", pPlayer->individualStallList.getStallName(), pPlayer->individualStallList.getStallInfo(), Con::getIntArg(zoneID), 
			Con::getIntArg(x), Con::getIntArg(y), Con::getIntArg(rate), pPlayer->getObjectName(), Con::getIntArg(pPlayer->getLevel()));
	}
	else
	{
		Player* targetPlayer = g_ClientGameplayState->findPlayer(playerID);
		if(!targetPlayer)
			return;
		S32 x = targetPlayer->getPosition().x;
		S32 y = targetPlayer->getPosition().y;
		S32 iNum = targetPlayer->individualStallList.getMaxSlot();
		Con::executef("openSeeStall",Con::getIntArg(iNum));
		Con::executef("initSeeStallWnd", targetPlayer->individualStallList.getStallName(), targetPlayer->individualStallList.getStallInfo(),
			Con::getIntArg(zoneID), Con::getIntArg(x), Con::getIntArg(y), Con::getIntArg(rate), targetPlayer->getObjectName(),
			Con::getIntArg(targetPlayer->getLevel()));
	}
#endif	
}

void IndividualStall::closeStall(Player* pPlayer, U32 type)
{
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

#ifdef NTJ_CLIENT

	if(pPlayer->individualStallList.getStallState() != TRADE_LOCK)
	{
		ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
		event->SetInt32ArgValues(2, TRADE_CANCEL, type);
		conn->postNetEvent(event);

		pPlayer->individualStallList.clearChatList();
		pPlayer->individualStallList.clearTradeList();
		//pPlayer->stallPetList.clear();
		//pPlayer->stallBuyItemList.clear();
	}
#endif
#ifdef NTJ_SERVER
	if(pPlayer->individualStallList.getStallState() != TRADE_LOCK && pPlayer->getInteractionState() == Player::INTERACTION_STALLTRADE)
	{
		pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
		pPlayer->mShapeStack.remove(ShapeStack::Stack_Stall, (U32)404001);
		if(type == stall_individual)
		{
			pPlayer->individualStallList.clearChatList();
			pPlayer->individualStallList.clearTradeList();
			//普通物品
			for(int i=0; i<pPlayer->individualStallList.getMaxSlot(); ++i)
			{
				ItemShortcut* pItem = pPlayer->individualStallList.GetSlot(i);
				if(pItem)
				{
					U32 index = pItem->getLockedItemIndex();
					pPlayer->inventoryList.GetSlot(index)->setSlotState(ShortcutObject::SLOT_COMMON);
					pPlayer->inventoryList.UpdateToClient(conn, index, ITEM_NOSHOW);
				}
			}
			pPlayer->individualStallList.clear();
			for(int i=0; i<pPlayer->individualStallList.getMaxSlot(); ++i)
			{
				pPlayer->individualStallList.UpdateToClient(conn, i);
			}
			// 宠物
			PetOperationManager::SetPetFreeze(pPlayer, false);
			pPlayer->stallPetList.clear();
			for(int i=0; i< StallPetList::MAXSLOTS; ++i)
			{
				pPlayer->stallPetList.UpdateToClient(conn, i);
			}
			//收购
			pPlayer->stallBuyItemList.clear();
			for(int i=0; i<StallBuyItemList::MAXSLOTS; ++i)
			{
				pPlayer->stallBuyItemList.UpdateToClient(conn, i);
			}
		}
	}

#endif
	
}

#ifdef NTJ_SERVER
enWarnMessage IndividualStall::buyStallItemSucc(Player* pTarget, Player* pSelf, U32 money, U32 index, U32 num, U32 flag)
{
	enWarnMessage msg = MSG_NONE;
	if(!pTarget || !pSelf)
		return MSG_PLAYER_TARGET_LOST;
	if(flag == 1)
	{
		StallItem* pItem = pTarget->individualStallList.mSlots[index];
		if(!pItem)
			return MSG_ITEM_CANNOTDRAG;
		ItemShortcut* pItemSlot = pTarget->individualStallList.GetSlot(index);
		Res* pRes = NULL;
		if(!pItemSlot || !(pRes = pItemSlot->getRes()))
			return MSG_ITEM_CANNOTDRAG;
		U32 iSrcIndex = pItemSlot->getLockedItemIndex();
		ItemShortcut* pSrcItem = (ItemShortcut*)(pTarget->inventoryList.GetSlot(iSrcIndex));
		Res* pSrcRes = NULL;
		if(!pSrcItem || !(pSrcRes = pSrcItem->getRes()))
			return MSG_ITEM_CANNOTDRAG;

		S32 addIndex;
		msg = g_ItemManager->addItemToInventory(pSelf, pSrcItem, addIndex, num, ITEM_STALLBUY);
		if(MSG_NONE != msg)
			return msg;

		stItemInfo* pItemInfo = new stItemInfo;
		dMemcpy(pItemInfo, pSrcItem->getRes()->getExtData(), sizeof(stItemInfo));

		if(pRes->getQuantity() - num > 0)
		{
			pRes->setQuantity(pRes->getQuantity() - num);
			pSrcRes->setQuantity(pSrcRes->getQuantity() - num);
		}
		else
		{
			pTarget->individualStallList.SetSlot(index, NULL);
			pTarget->inventoryList.SetSlot(iSrcIndex, NULL);
		}

		pTarget->individualStallList.UpdateToClient(pTarget->getControllingClient(), index);
		pTarget->individualStallList.UpdateTargetToClient(pTarget, pSelf, index);
		pTarget->inventoryList.UpdateToClient(pTarget->getControllingClient(), iSrcIndex, ITEM_NOSHOW);
	
		stTradeRecord* pRecord = new stTradeRecord;
		pRecord->receiverID = pTarget->getPlayerID();
		pRecord->receiverName = pTarget->getPlayerName();
		pRecord->senderID = pSelf->getPlayerID();
		pRecord->senderName = pSelf->getPlayerName();
		pRecord->time = Platform::getTime();
		pRecord->type = stTradeRecord::TRADE_ITEM;
		pItemInfo->Quantity = num;
		pRecord->trade.item = pItemInfo;
		pRecord->money = money;
		
		pTarget->individualStallList.mTradeInfoList.push_back(pRecord);

		delete pItemInfo;
		//向摊主发送
		U32 index = pTarget->individualStallList.mTradeInfoList.size() -1;
		StallRecordEvent* ev = new StallRecordEvent(pTarget->getPlayerID(), false, index);
		pTarget->getControllingClient()->postNetEvent(ev);
		ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
		evt->SetInt32ArgValues(3, STALL_TRADERECORD,  pTarget->getPlayerID(), 1);
		pTarget->getControllingClient()->postNetEvent(evt);
		//给自己发送
		StallRecordEvent* ev1 = new StallRecordEvent(pTarget->getPlayerID(), false, index);
		pSelf->getControllingClient()->postNetEvent(ev1);
		ServerGameNetEvent* evt1 = new ServerGameNetEvent(INFO_STALL);
		evt1->SetInt32ArgValues(4, STALL_TRADERECORD, pTarget->getPlayerID(), 1, 1);
		pSelf->getControllingClient()->postNetEvent(evt1);
		
	}
	else if(flag == 2)
	{
		PetShortcut* pPet = pTarget->stallPetList.GetSlot(index);
		if(!pPet)
			return MSG_PET_NOT_EXIST;
		stPetInfo *pPetInfoSrc = (stPetInfo *)pTarget->getPetTable().getPetInfo(pPet->getSlotIndex());
		if (!pPetInfoSrc)		
			msg = MSG_PET_DATA_NOT_EXIST;
		
		pPetInfoSrc->status = PetObject::PetStatus_Idle;
		msg = PetOperationManager::AddPetInfo(pSelf, pPetInfoSrc);
		if(msg != MSG_NONE)
		{
			pPetInfoSrc->status = PetObject::PetStatus_Freeze;
			return msg;
		}

		msg = PetOperationManager::DeletePetInfo(pTarget,pPet->getSlotIndex());
		if(msg != MSG_NONE)
			return msg;
		
		pTarget->stallPetList.SetSlot(index, NULL, 0);
		pTarget->stallPetList.UpdateToClient(pTarget->getControllingClient(), index);
		pTarget->stallPetList.UpdateTargetToClient(pTarget, pSelf, index);

		stTradeRecord* pRecord = new stTradeRecord;
		pRecord->receiverID = pTarget->getPlayerID();
		pRecord->receiverName = pTarget->getPlayerName();
		pRecord->senderID = pSelf->getPlayerID();
		pRecord->senderName = pSelf->getPlayerName();
		pRecord->time = Platform::getTime();
		pRecord->type = stTradeRecord::TRADE_PET;
		pRecord->trade.pet = pPetInfoSrc;
		pRecord->money = money;

		pTarget->individualStallList.mTradeInfoList.push_back(pRecord);
		//向摊主发送
		U32 index = pTarget->individualStallList.mTradeInfoList.size() -1;
		StallRecordEvent* ev = new StallRecordEvent(pTarget->getPlayerID(), false, index);
		pTarget->getControllingClient()->postNetEvent(ev);
		ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
		evt->SetInt32ArgValues(3, STALL_TRADERECORD,  pTarget->getPlayerID(), 1);
		pTarget->getControllingClient()->postNetEvent(evt);
		//给自己发送
		StallRecordEvent* ev1 = new StallRecordEvent(pTarget->getPlayerID(), false, index);
		pSelf->getControllingClient()->postNetEvent(ev1);
		ServerGameNetEvent* evt1 = new ServerGameNetEvent(INFO_STALL);
		evt1->SetInt32ArgValues(4, STALL_TRADERECORD, pTarget->getPlayerID(), 1, 1);
		pSelf->getControllingClient()->postNetEvent(evt1);
	}
	if(msg == MSG_NONE)
	{
		pTarget->addMoney(money);
		pSelf->reduceMoney(money);
		U32 stallerHaveMoney = pTarget->getMoney(1);
		for(S32 i=0; i<StallBuyItemList::MAXSLOTS; ++i)
		{
			stStallBuyItem* pBuyItem = pTarget->stallBuyItemList.mSlots[i];
			if(pBuyItem)
			{
				if(pBuyItem->money > stallerHaveMoney)
					pTarget->stallBuyItemList.GetSlot(i)->setSlotState(ShortcutObject::SLOT_LOCK);
				else
					pTarget->stallBuyItemList.GetSlot(i)->setSlotState(ShortcutObject::SLOT_COMMON);
				pTarget->stallBuyItemList.UpdateTargetToClient(pTarget, pSelf, i);
			}
		}
	}
	return msg;
}

void IndividualStall::openStallerStall(Player* pPlayer, GameConnection* con, U32 playerID)
{
	if(!pPlayer || !con)
		return;
	Player* pTarget = g_ServerGameplayState->GetPlayer(playerID);
	if(pTarget && pTarget->getInteractionState() == Player::INTERACTION_STALLTRADE 
		&& pTarget->individualStallList.getStallState() == TRADE_LOCK)
	{
		if(!pPlayer->canInteraction(pTarget))
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_TOOFAR, SHOWPOS_SCREEN);
			return;
		}
		for(S32 i = 0; i < pTarget->individualStallList.getMaxSlot(); ++i)
			pTarget->individualStallList.UpdateTargetToClient(pTarget, pPlayer, i);

		for(S32 i=0; i< StallPetList::MAXSLOTS; ++i)
			pTarget->stallPetList.UpdateTargetToClient(pTarget, pPlayer, i);

		U32 stallerHaveMoney = pTarget->getMoney(1);
		for(S32 i=0; i<StallBuyItemList::MAXSLOTS; ++i)
		{
			stStallBuyItem* pBuyItem = pTarget->stallBuyItemList.mSlots[i];
			if(pBuyItem)
			{
				if(pBuyItem->money > stallerHaveMoney)
					pTarget->stallBuyItemList.GetSlot(i)->setSlotState(ShortcutObject::SLOT_LOCK);
			}
			pTarget->stallBuyItemList.UpdateTargetToClient(pTarget, pPlayer, i);
		}
		pTarget->individualStallList.addPlayerList(pPlayer->getPlayerID());

		pPlayer->setInteraction(pTarget, Player::INTERACTION_STALLBUY);

		ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
		evt->SetInt32ArgValues(2, TRADE_ASKFOR, playerID);
		evt->SetStringArgValues(1, pTarget->individualStallList.getStallName());
		con->postNetEvent(evt);
	}
}

void IndividualStall::upItemToStall(Player* pPlayer, GameConnection* con, S32 index, U32 type)
{
	if(!pPlayer || !con)
		return;
	enWarnMessage msg = MSG_NONE;
	if(type == 1)
	{
		msg = g_ItemManager->addItemToStallList(pPlayer, index);
		if(msg != MSG_NONE)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
			return;
		}
	}
	else if(type == 2)
	{
		if ( !pPlayer->petList.IsVaildSlot(index) || 
			index < 0 || index > 5 )	//宠物摆摊栏位置校验
			return ;

		stPetInfo *pPetInfoSrc = (stPetInfo *)pPlayer->getPetTable().getPetInfo(index);
		if (!pPetInfoSrc)		//此时不进行交换操作,也不需要提示错误
			return ;
		if (pPetInfoSrc->status != PetObject::PetStatus_Idle)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PET_STATUS_NOT_IDLE, SHOWPOS_SCREEN);
			return ;
		}
		//pPetInfoSrc->status = PetObject::PetStatus_Stall;
		//PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, index);

		S32 iPos = pPlayer->stallPetList.findEmptyIndex();
		if(iPos < 0)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PET_STATUS_NOT_IDLE, SHOWPOS_SCREEN);
			return ;
		}

		PetShortcut *pNewShortcut = PetShortcut::CreatePetItem(index);
		pPlayer->stallPetList.SetSlot(iPos, pNewShortcut, 0);
		pPlayer->stallPetList.UpdateToClient(con, iPos);
	}
}
//下架
void IndividualStall::downItemFromStall(Player* pPlayer, GameConnection* con, S32 index, U32 type)
{
	if(!pPlayer || !con)
		return;
	if(type == 1)
	{
		enWarnMessage msg = g_ItemManager->delItemFromStallList(pPlayer, index);
		if(msg != MSG_NONE)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
			return;
		}
	}
	else if(type == 2)
	{
		enWarnMessage msg = g_ItemManager->delPetFromStallPetList(pPlayer, index);
		if(msg != MSG_NONE)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
			return;
		}
	}

	else if(type == 3)
	{
		pPlayer->stallBuyItemList.SetSlot(index, NULL, 0, 0);
		pPlayer->stallBuyItemList.UpdateToClient(con, index);
	}

	ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
	evt->SetInt32ArgValues(3, TRADE_DOWNITEM, index, type);
	con->postNetEvent(evt);
}
//改价
void IndividualStall::setStallItemPrice(Player* pPlayer, GameConnection* con, S32 index, U32 money, U32 type)
{
	if(!pPlayer || !con)
		return;
	if(pPlayer->individualStallList.getStallState() != TRADE_LOCK)
	{
		if(type ==1)
		{
			pPlayer->individualStallList.setStallMoney(index, money);
			pPlayer->individualStallList.UpdateToClient(con, index);
		}
		else
		{
			pPlayer->stallPetList.mSlots[index]->money = money;
			pPlayer->stallPetList.UpdateToClient(con, index);
		}
		ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
		evt->SetInt32ArgValues(4, TRADE_SETMONEY, index, money, type);
		con->postNetEvent(evt);
	}
	else
	{
		MessageEvent::send(con, SHOWTYPE_ERROR, MSG_TRADE_STALL_NOCHANGEPRICE, SHOWPOS_SCREEN);
		return;
	}
}
//全部下架
void IndividualStall::downAllStallItem(Player* pPlayer, GameConnection* con)
{
	if(!pPlayer || !con)
		return;
	if(pPlayer->individualStallList.getStallState() == TRADE_LOCK)
	{
		MessageEvent::send(con, SHOWTYPE_ERROR, MSG_TRADE_STALL_NORENAME, SHOWPOS_SCREEN);
		return;
	}
	for(int i=0; i<pPlayer->individualStallList.getMaxSlot(); i++)
	{
		ItemShortcut* pItem = pPlayer->individualStallList.GetSlot(i);
		if(pItem)
		{
			enWarnMessage msg = g_ItemManager->delItemFromStallList(pPlayer, i);
			if(msg != MSG_NONE)
			{
				MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
				return;
			}
		}
	}

	for(int i=0; i<StallPetList::MAXSLOTS; ++i)
	{
		PetShortcut* pPet = pPlayer->stallPetList.GetSlot(i);
		if(pPet)
		{
			enWarnMessage msg = g_ItemManager->delPetFromStallPetList(pPlayer, i);
			if(msg != MSG_NONE)
			{
				MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
				return;
			}
		}
	}

	ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
	evt->SetInt32ArgValues(1, STALL_REMOVE_ALL);
	con->postNetEvent(evt);
}

void IndividualStall::stallTrade(Player* pPlayer, GameConnection* con, S32 index, U32 num, U32 playerID, U32 flag)
{
	if(!pPlayer || !con)
		return;
	Player* pTarget = g_ServerGameplayState->GetPlayer(playerID);
	if(!pTarget || pTarget->getInteractionState() != Player::INTERACTION_STALLTRADE 
		|| pTarget->individualStallList.getStallState() != TRADE_LOCK)
		return;
	enWarnMessage msg = MSG_UNKOWNERROR;
	if(flag == 1)
	{
		StallItem* pItem = pTarget->individualStallList.mSlots[index];
		if(!pItem)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_ITEM_ITEMDATAERROR, SHOWPOS_SCREEN);
			return;
		}
		ItemShortcut* pItemSlot = pTarget->individualStallList.GetSlot(index);
		if(!pItemSlot)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_ITEM_ITEMDATAERROR, SHOWPOS_SCREEN);
			return;
		}
		if(pPlayer->getMoney() < (pItem->money * num))
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH, SHOWPOS_SCREEN);
			return;
		}

		if(pItemSlot->getRes()->getQuantity() < num)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH, SHOWPOS_SCREEN);
			return;
		}
		msg = g_Stall->buyStallItemSucc(pTarget, pPlayer, pItem->money * num, index, num, 1);

	}
	else if(flag == 2)
	{
		stStallPet* pPet = pTarget->stallPetList.mSlots[index];
		if(!pPet)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PET_DATA_NOT_EXIST, SHOWPOS_SCREEN);
			return;
		}
		PetShortcut* pPetSlot = pTarget->stallPetList.GetSlot(index);
		if(!pPetSlot)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PET_DATA_NOT_EXIST, SHOWPOS_SCREEN);
			return;
		}
		if(pPlayer->getMoney() < pPet->money)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH, SHOWPOS_SCREEN);
			return;
		}
		if(num  > 1)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH, SHOWPOS_SCREEN);
			return;
		}
		msg = g_Stall->buyStallItemSucc(pTarget, pPlayer, pPet->money, index, 1, 2);

	}

	else if(flag == 3)
	{
		stStallBuyItem* pBuyItem = pTarget->stallBuyItemList.mSlots[index];
		if(!pBuyItem)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_ITEM_ITEMDATAERROR, SHOWPOS_SCREEN);
			return;
		}
		if(pBuyItem->num < num)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_ITEM_ITEMDATAERROR, SHOWPOS_SCREEN);
			return;
		}

		if(pTarget->getMoney() < pBuyItem->money * num)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH, SHOWPOS_SCREEN);
			return;
		}

		if(pPlayer->inventoryList.GetItemCount(pBuyItem->buyItem, false, true) < num)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH, SHOWPOS_SCREEN);
			return;
		}

		msg = g_ItemManager->delItemFromInventory(pPlayer, pBuyItem->buyItem, num, ITEM_SELL);
		if(msg != MSG_NONE)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
			return;
		}

		S32 iIndex;
		msg = g_ItemManager->addItemToInventory(pTarget, pBuyItem->buyItem, iIndex, num, ITEM_LOAD);
		if(msg != MSG_NONE)
		{
			MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
			return;
		}

		S32 money = pBuyItem->money * num;

		stItemInfo* pItemInfo = new stItemInfo;
		dMemcpy(pItemInfo, pBuyItem->buyItem->getRes()->getExtData(), sizeof(stItemInfo));
		pItemInfo->Quantity = num;

		stTradeRecord* pRecord = new stTradeRecord;
		pRecord->receiverID = pTarget->getPlayerID();
		pRecord->receiverName = pTarget->getPlayerName();
		pRecord->senderID = pPlayer->getPlayerID();
		pRecord->senderName = pPlayer->getPlayerName();
		pRecord->time = Platform::getTime();
		pRecord->type = stTradeRecord::TRADE_ITEM;
		pRecord->trade.item = pItemInfo;
		pRecord->money = -money;

		pTarget->individualStallList.mTradeInfoList.push_back(pRecord);

		if(pBuyItem->num - num > 0)
			pTarget->stallBuyItemList.mSlots[index]->num = pBuyItem->num - num;
		else
			pTarget->stallBuyItemList.SetSlot(index, NULL, 0, 0);
		pTarget->stallBuyItemList.UpdateToClient(pTarget->getControllingClient(), index);
		pTarget->stallBuyItemList.UpdateTargetToClient(pTarget, pPlayer, index);

		pTarget->reduceMoney(money);
		pPlayer->addMoney(money);


		//更新收购栏状态
		U32 stallerHaveMoney = pTarget->getMoney(1);
		for(S32 i=0; i<StallBuyItemList::MAXSLOTS; ++i)
		{
			stStallBuyItem* pBuyItem = pTarget->stallBuyItemList.mSlots[i];
			if(pBuyItem)
			{
				if(pBuyItem->money > stallerHaveMoney)
					pTarget->stallBuyItemList.GetSlot(i)->setSlotState(ShortcutObject::SLOT_LOCK);
				else
					pTarget->stallBuyItemList.GetSlot(i)->setSlotState(ShortcutObject::SLOT_COMMON);
				pTarget->stallBuyItemList.UpdateTargetToClient(pTarget, pPlayer, i);
			}
		}


		msg = MSG_NONE;
	}

	if(msg != MSG_NONE)
	{
		MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
		return;
	}
	else
	{
		MessageEvent::send(con, SHOWTYPE_ERROR, MSG_TRADE_SUCCEED, SHOWPOS_SCREEN);
		return;
	}
}

enWarnMessage ItemManager::InventoryMoveToStall(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(player->isBusy(Player::INTERACTION_STALLTRADE))
		return MSG_TRADE_SELF_BUSY;

	if(player->individualStallList.getStallState() == TRADE_LOCK)
		return MSG_TRADE_SELF_STATELOCK;

	ShortcutObject* pSrc	= player->inventoryList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->individualStallList.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;

	ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrc);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;

	//判定物品是否可交易
	if(!pSrcItem->getRes()->canTrade() || pSrcItem->getSlotState() == ShortcutObject::SLOT_LOCK)
		return MSG_ITEM_CANNOTTRADE;

	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;

	if(pDest)
	{
		// 如果在摆摊栏已经存在物品，则先删除
		U32 iSrcIndex = pDest->getLockedItemIndex();
		ItemShortcut* pDestItem = ItemShortcut::CreateItem(pSrcItem);
		if(!pDestItem)
			return MSG_ITEM_ITEMDATAERROR;

		player->individualStallList.SetSlot(param->DestIndex, pDestItem);
		player->individualStallList.setStallMoney(param->DestIndex, param->Price);
		player->individualStallList.GetSlot(param->DestIndex)->setLockedItemIndex(param->SrcIndex);

		player->individualStallList.UpdateToClient(conn, param->DestIndex);

		player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);		
		pSrc->setSlotState(ShortcutObject::SLOT_LOCK);

		player->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}
	else
	{
		ItemShortcut* pTargetItem = ItemShortcut::CreateItem(pSrcItem);
		if(!pTargetItem)
			return MSG_ITEM_ITEMDATAERROR;

		player->individualStallList.SetSlot(param->DestIndex, pTargetItem);
		player->individualStallList.setStallMoney(param->DestIndex, param->Price);
		player->individualStallList.GetSlot(param->DestIndex)->setLockedItemIndex(param->SrcIndex);
		player->individualStallList.UpdateToClient(conn, param->DestIndex);

		pSrc->setSlotState(ShortcutObject::SLOT_LOCK);
		player->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
	}
	
	ServerGameNetEvent* event = new ServerGameNetEvent(INFO_STALL);
	event->SetInt32ArgValues(4, TRADE_SETMONEY, param->DestIndex,param->Price, 1);
	conn->postNetEvent(event);

	return MSG_NONE;
}

enWarnMessage ItemManager::StallMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;

	if(param == NULL)
		return msg;
	Player* player = param->player;
	if(player == NULL)
		return msg;

	ShortcutObject* pSrc	= player->individualStallList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->inventoryList.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;
	U32 iSrcIndex = pSrc->getLockedItemIndex();
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;
	if(pDest)
	{
		ItemShortcut* pDestItem = (ItemShortcut*)pDest;
		if(!pDestItem)
			return MSG_ITEM_ITEMDATAERROR;
		ItemShortcut* tempItem = ItemShortcut::CreateItem(pDestItem);
		if(!tempItem)
			return MSG_ITEM_ITEMDATAERROR;
		player->individualStallList.SetSlot(param->SrcIndex, tempItem);

		player->individualStallList.GetSlot(param->SrcIndex)->setLockedItemIndex(param->DestIndex);
		player->individualStallList.UpdateToClient(conn, param->SrcIndex);

		player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
		player->inventoryList.GetSlot(param->DestIndex)->setSlotState(ShortcutObject::SLOT_LOCK);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
		player->inventoryList.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
	}
	else
	{
		player->individualStallList.SetSlot(param->SrcIndex, NULL);
		player->individualStallList.UpdateToClient(conn, param->SrcIndex);

		player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}
	return MSG_NONE;
}

enWarnMessage ItemManager::addItemToStallList(Player* player, S32 index)
{
	if(player->isBusy(Player::INTERACTION_STALLTRADE))
		return MSG_TRADE_SELF_BUSY;
	
	ShortcutObject* pSrcSlot = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_INVENTORY, index);
	if (!pSrcSlot || pSrcSlot->getSlotState() == ShortcutObject::SLOT_LOCK)
		return MSG_ITEM_ITEMBEUSED;		// 物品正使用中，无法操作

	ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;

	// 判断物品是否可交易
	if(!pSrcItem->getRes()->canTrade())
		return MSG_ITEM_CANNOTTRADE;

	// 判断状态是否Trade_Begin
	if(player->individualStallList.getStallState() == TRADE_LOCK)
		return MSG_TRADE_SELF_STATELOCK;

	U32 emptyIndex = player->individualStallList.findEmptyIndex();
	if (emptyIndex != -1)
	{
		ItemShortcut* tempItem = ItemShortcut::CreateItem(pSrcItem);
		if(!tempItem)
			return MSG_ITEM_ITEMDATAERROR;
		pSrcSlot->setSlotState(ShortcutObject::SLOT_LOCK);
		player->individualStallList.SetSlot(emptyIndex, tempItem);
		player->individualStallList.GetSlot(emptyIndex)->setLockedItemIndex(index);
		player->individualStallList.UpdateToClient(player->getControllingClient(), emptyIndex);
		player->inventoryList.UpdateToClient(player->getControllingClient(), index, ITEM_NOSHOW);
		return MSG_NONE;
	}
	else
	{
		return MSG_TRADE_NOSPACE;
	}
}

enWarnMessage ItemManager::delItemFromStallList(Player* player, S32 index)
{
	if(player->isBusy(Player::INTERACTION_STALLTRADE))
		return MSG_TRADE_SELF_BUSY;

	// 判断交易状态是否Trade_Begin
	if(player->individualStallList.getStallState() == TRADE_LOCK)
		return MSG_TRADE_SELF_STATELOCK;

	// 判断栏位状态是否为自由
	ShortcutObject* pSlot = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_STALL, index);
	if (!pSlot)
		return MSG_NONE;

	U32 iSrcIndex = pSlot->getLockedItemIndex();
	ShortcutObject* pSrcSlot = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_INVENTORY, iSrcIndex);
	if (!pSrcSlot || pSrcSlot->getSlotState() != ShortcutObject::SLOT_LOCK)
		return MSG_ITEM_ITEMDATAERROR;

	pSrcSlot->setSlotState(ShortcutObject::SLOT_COMMON);
	player->individualStallList.SetSlot(index, NULL);
	player->inventoryList.UpdateToClient(player->getControllingClient(), iSrcIndex, ITEM_NOSHOW);
	player->individualStallList.UpdateToClient(player->getControllingClient(), index);

	return MSG_NONE;
}
#endif

#ifdef NTJ_CLIENT
//修改钱
ConsoleFunction(setStallMoney, void, 4, 4, "setStallMoney(%index, %money, %iType)")
{
	S32 index = dAtoi(argv[1]);
	if(index < 0)
		return;
	U32 money = dAtoi(argv[2]);
	if(money == 0)
		return;
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;

	GameConnection* conn = player->getControllingClient();
	if(conn)
	{
		ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
		event->SetInt32ArgValues(4, TRADE_SETMONEY, index,money,dAtoi(argv[3]));
		conn->postNetEvent(event);
	}
}
//修改名字
ConsoleFunction(setStallName, void, 2, 2, "setStallName(%name)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(conn)
	{
		ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
		event->SetInt32ArgValues(1, STALL_RENAME);
		event->SetStringArgValues(1, argv[1]);
		conn->postNetEvent(event);
	}
}
//修改摆摊信息
ConsoleFunction(setStallInfo, void, 2, 2, "setStallInfo(%info)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(conn)
	{
		ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
		event->SetInt32ArgValues(1, STALL_REINFO);
		event->SetStringArgValues(1, argv[1]);
		conn->postNetEvent(event);
	}
}

// 清除
ConsoleFunction(clearStall, void, 2, 2, "clearStall(%type)")
{
	U32 type = dAtoi(argv[1]);
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	g_Stall->closeStall(player, type);
}
//下架
ConsoleFunction(removeStall, void, 3, 3, "removeStall(%index, %type)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	S32 index = dAtoi(argv[1]);
	U32 iType = dAtoi(argv[2]);
	if(index < 0)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_NOSELECT);
		return;
	}
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
	event->SetInt32ArgValues(3, TRADE_DOWNITEM, index, iType);
	conn->postNetEvent(event);

}
//全部下架
ConsoleFunction(removeAllStall, void, 1, 1, "removeAllStall()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	
	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
	evt->SetInt32ArgValues(1, STALL_REMOVE_ALL);
	conn->postNetEvent(evt);
	
}
//显示价格
ConsoleFunction(showStallItemMoney, void, 3, 3, "showStallItemMoney(%index, %type)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	S32 index = dAtoi(argv[1]);
	if(index < 0)
		Con::executef("showStallMoney", Con::getIntArg(0), Con::getIntArg(0));
	U32 iType = dAtoi(argv[2]);
	if(iType == 1)
	{
		ItemShortcut* pItem = player->individualStallList.GetSlot(index);
		if(pItem)
		{
			U32 money = player->individualStallList.getStallMoney(index);
			U32 num = pItem->getRes()->getQuantity();
			Con::executef("showStallMoney", Con::getIntArg(money), Con::getIntArg(num));
		}
		else
			Con::executef("showStallMoney", Con::getIntArg(0), Con::getIntArg(0));
	}
	else if(iType == 2)
	{
		PetShortcut* petSolt = player->stallPetList.GetSlot(index);
		if(petSolt)
		{
			U32 money = player->stallPetList.mSlots[index]->money;
			Con::executef("showStallMoney", Con::getIntArg(money), Con::getIntArg(1));
		}
		else
			Con::executef("showStallMoney", Con::getIntArg(0), Con::getIntArg(0));
	}

	else if(iType == 3)
	{
		ItemShortcut* pItem = player->stallBuyItemList.GetSlot(index);
		if(!pItem)
			Con::executef("showStallMoney", Con::getIntArg(0), Con::getIntArg(0));
		else
		{
			U32 money = player->stallBuyItemList.mSlots[index]->money;
			U32 iNum = player->stallBuyItemList.mSlots[index]->num;
			Con::executef("showStallMoney", Con::getIntArg(money), Con::getIntArg(iNum));
		}
	}
}
//计算价格
ConsoleFunction(calculatePrice, S32, 4, 4, "calculatePrice(%gold, %siller, %copper)")
{
	S32 money = 0;
	S32 gold = dAtoi(argv[1]);
	S32 siller = dAtoi(argv[2]);
	S32 copper = dAtoi(argv[3]);

	money = gold * 10000 + siller * 100 + copper;
	return money;
}
//出摊
ConsoleFunction(outStall, void, 1, 1, "outStall()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;

	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
	evt->SetInt32ArgValues(1, TRADE_LOCK);
	conn->postNetEvent(evt);
}
//收摊
ConsoleFunction(overStall, void, 1, 1, "overStall()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;

	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
	evt->SetInt32ArgValues(1, TRADE_UNLOCK);
	conn->postNetEvent(evt);
}

//设置钱
ConsoleFunction(setStallPrice, void, 4, 4, "SetStallPrice(%index, %money, %type)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	U32 iType = dAtoi(argv[3]);
	U32 iMoney = dAtoi(argv[2]);
	if(iMoney == 0)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, "请输入物品单价");
		//g_ClientGameplayState->cancelCurrentAction();
		return;
	}
	ClientGameplayParam* param1 = new ClientGameplayParam;
	if(iType == 1)
		param1->setIntArgValues(2, SHORTCUTTYPE_STALL, dAtoi(argv[1]));
	else if(iType == 2)
		param1->setIntArgValues(2, SHORTCUTTYPE_STALL_PET, dAtoi(argv[1]));
	param1->setInt32ArgValues(1, iMoney);
	g_ClientGameplayState->setCurrentActionParam(param1);
}
//察看物品价格
ConsoleFunction(setSeeItemPrice, void, 3, 3, "setSeeItemPrice(%index, %type)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	S32 index = dAtoi(argv[1]);
	U32 iType = dAtoi(argv[2]);
	SceneObject* pTargetObject = player->getInteraction();
	Player* pTarget = NULL;
	if(pTargetObject && (pTarget = dynamic_cast<Player*>(pTargetObject)))
	{
		if(iType == 1)
		{
			StallItem* pItem = pTarget->individualStallList.mSlots[index];
			if(pItem && pItem->pItem)
			{
				U32 money = pItem->money;
				U32 num = pItem->pItem->getRes()->getQuantity();
				U32 iMaxNum = pItem->pItem->getRes()->getMaxOverNum();
				Con::executef("showSeeStallMoney", Con::getIntArg(money), Con::getIntArg(num), Con::getIntArg(index), Con::getIntArg(iMaxNum));
			}
			else
				Con::executef("showSeeStallMoney", Con::getIntArg(0), Con::getIntArg(0), Con::getIntArg(-1), "0");
		}
		else if(iType == 2)
		{
			stStallPet* pPet = pTarget->stallPetList.mSlots[index];
			if(pPet && pPet->petSlot)
			{
				U32 money = pPet->money;
				Con::executef("showSeeStallMoney", Con::getIntArg(money), Con::getIntArg(1), Con::getIntArg(index), "1");
			}
			else
				Con::executef("showSeeStallMoney", Con::getIntArg(0), Con::getIntArg(0), Con::getIntArg(-1), "0");
		}
		else if(iType == 3)
		{
			stStallBuyItem* pBuyItem = pTarget->stallBuyItemList.mSlots[index];
			if(pBuyItem && pBuyItem->buyItem)
			{
				U32 money = pBuyItem->money;
				U32 iNum = pBuyItem->num;
				U32 iMaxNum = pBuyItem->buyItem->getRes()->getMaxOverNum();
				Con::executef("showSeeStallMoney", Con::getIntArg(money), Con::getIntArg(iNum), Con::getIntArg(index), Con::getIntArg(iMaxNum));
			}
			else
				Con::executef("showSeeStallMoney", Con::getIntArg(0), Con::getIntArg(0), Con::getIntArg(-1), "0");
		}
	}
}

//确认购买
ConsoleFunction(makeSureToBuyStallItem, void, 4, 4, "makeSureToBuyStallItem(%index, %num, %flag)")
{
	U32 index = dAtoi(argv[1]);
	U32 num = dAtoi(argv[2]);
	U32 iFlag = dAtoi(argv[3]);
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

	SceneObject* pTargetObject = pPlayer->getInteraction();
	Player* pTarget = NULL;
	if(pTargetObject && (pTarget = dynamic_cast<Player*>(pTargetObject)))
	{
		if(iFlag == 1)
		{
			StallItem* pItem = pTarget->individualStallList.mSlots[index];
			if(pItem)
			{
				U32 needMoney = pItem->money * num;
				U32 haveMoney = pPlayer->getMoney();
				if(haveMoney < needMoney)
				{
					MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
					return;
				}
				if(pItem->pItem->getRes()->getQuantity() < num)
				{
					MessageEvent::show(SHOWTYPE_ERROR, MSG_TRADE_NOHAVE_ITEM);
					return;
				}
				ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
				evt->SetInt32ArgValues(5, TRADE_ENDNOTIFY, index, num, pTarget->getPlayerID(), 1);
				conn->postNetEvent(evt);
			}
		}
		else if(iFlag == 2)
		{
			stStallPet* pPet = pTarget->stallPetList.mSlots[index];
			if(pPet)
			{
				if(num > 1)
				{
					MessageEvent::show(SHOWTYPE_ERROR, MSG_TRADE_NOHAVE_ITEM);
					return;
				}
				U32 needMoney = pPet->money * 1;
				U32 haveMoney = pPlayer->getMoney();
				if(haveMoney < needMoney)
				{
					MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
					return;
				}

				ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
				evt->SetInt32ArgValues(5, TRADE_ENDNOTIFY, index, num, pTarget->getPlayerID(), 2);
				conn->postNetEvent(evt);
			}
		}
		else if(iFlag == 3)
		{
			stStallBuyItem* pBuyItem = pTarget->stallBuyItemList.mSlots[index];
			if(pBuyItem)
			{
				if(pPlayer->inventoryList.GetItemCount(pBuyItem->buyItem,false,true) < num)
				{
					MessageEvent::show(SHOWTYPE_ERROR, MSG_ITEM_NOENOUGHNUM);
					return;
				}
				
				if(pBuyItem->num < num)
				{
					MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
					return;
				}

				ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
				evt->SetInt32ArgValues(5, TRADE_ENDNOTIFY, index, num, pTarget->getPlayerID(), 3);
				conn->postNetEvent(evt);
			}
		}
	}
}


// 关闭查看
ConsoleFunction(closeSeeStall, void, 1, 1, "closeSeeStall")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
	evt->SetInt32ArgValues(1, STALL_SEE_CLOSE);
	conn->postNetEvent(evt);
}

ConsoleFunction(setAddEvent, void, 3, 3, "setAddEvent(%index,%type)")
{
	S32 index = dAtoi(argv[1]);
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;

	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
	event->SetInt32ArgValues(3, TRADE_UPITEM, index, dAtoi(argv[2]));
	player->getControllingClient()->postNetEvent(event);
}

//显示摆摊信息
ConsoleFunction(showStallInfo, void, 1, 1, "showStallInfo()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
	
	//Con::executef("clearTradeAndChatMessage");
	if(pPlayer->getInteractionState() == Player::INTERACTION_STALLBUY)
	{
		SceneObject* pTargetObject = pPlayer->getInteraction();
		Player* pTarget = NULL;
		if(pTargetObject && (pTarget = dynamic_cast<Player*>(pTargetObject)))
		{
			/*for(int i=0; i<pTarget->individualStallList.mChatInfoList.size(); ++i)
			{
				stChatRecord* pRecord = pTarget->individualStallList.mChatInfoList[i];
				if(pRecord)
					delete pRecord;
				pTarget->individualStallList.mChatInfoList.erase_fast(i--);
			}
			for(int i=0; i<pTarget->individualStallList.mTradeInfoList.size(); ++i)
			{
				stTradeRecord* pRecord = pTarget->individualStallList.mTradeInfoList[i];
				if(pRecord)
					delete pRecord;
				pTarget->individualStallList.mTradeInfoList.erase_fast(i--);
			}*/

			evt->SetInt32ArgValues(2, STALL_SCANSTALLNOTE, pTarget->getPlayerID());
		}
		else
			return;
	}	
	else
		evt->SetInt32ArgValues(2, STALL_SCANSTALLNOTE, pPlayer->getPlayerID());
	conn->postNetEvent(evt);
}

//显示聊天信息
ConsoleFunction(addStallChatMsg, void, 2, 2, "addStallChatMsg(%info)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	SceneObject* pTargetObject = pPlayer->getInteraction();
	Player* pTarget = NULL;
	if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
		return;
	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
	evt->SetInt32ArgValues(3, STALL_CHATMSG, pPlayer->getPlayerID(), pTarget->getPlayerID());
	evt->SetStringArgValues(1, argv[1]);
	conn->postNetEvent(evt);

}

ConsoleFunction(clearStallMessage, void, 1, 1, "clearStallMessage()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	if(pPlayer->getInteractionState() != Player::INTERACTION_STALLTRADE)
		return;
	ClientGameNetEvent* evt = new ClientGameNetEvent(INFO_STALL);
	evt->SetInt32ArgValues(1, STALL_CLEAR_MSG);
	conn->postNetEvent(evt);

}
//是否显示摆摊改价界面
ConsoleFunction(ShowChangePriceWnd, bool, 3, 3, "ShowChangePriceWnd(%index, %type)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return false;
	U32 iType = dAtoi(argv[2]);
	U32 iIndex = dAtoi(argv[1]);
	if(iType == 1)
	{
		ItemShortcut* pItem = pPlayer->individualStallList.GetSlot(iIndex);
		if(!pItem)
			return false;
	}
	else if(iType == 2)
	{
		PetShortcut* pPet = pPlayer->stallPetList.GetSlot(iIndex);
		if(!pPet)
			return false;
		stPetInfo *pPetInfoSrc = (stPetInfo *)pPlayer->getPetTable().getPetInfo(pPet->getSlotIndex());
		if (!pPetInfoSrc)		
			return false;
	}

	return true;
}
#endif



//-------------------------------------------------------------------------------------------------------------
//摆摊收购栏
//-------------------------------------------------------------------------------------------------------------
StallBuyItemList::StallBuyItemList()
{
	for(int i=0; i<MAXSLOTS; ++i)
		mSlots[i] = NULL;
}

StallBuyItemList::~StallBuyItemList()
{
	clear();
}

void StallBuyItemList::clear()
{
	for(int i=0; i<MAXSLOTS; ++i)
	{
		if(mSlots[i])
		{
			if(mSlots[i]->buyItem)
			{
				delete mSlots[i]->buyItem;
				mSlots[i]->buyItem = NULL;
			}
			delete mSlots[i];
		}
		mSlots[i] = NULL;
	}
}

ItemShortcut* StallBuyItemList::GetSlot(S32 index)
{
	if(index < 0 || index >MAXSLOTS)
		return NULL;
	return mSlots[index] ? mSlots[index]->buyItem : NULL;
}

void StallBuyItemList::SetSlot(S32 index, ItemShortcut* pItem, U32 money, U32 num)
{
	if(mSlots[index])
	{
		if(pItem)
		{
			if(mSlots[index]->buyItem)
				delete mSlots[index]->buyItem;
			mSlots[index]->buyItem = pItem;
			mSlots[index]->money = money;
			mSlots[index]->num = num;
		}
		else
		{
			if(mSlots[index]->buyItem)
				delete mSlots[index]->buyItem;
			delete mSlots[index];
			mSlots[index] = NULL;
		}

	}
	else
	{
		stStallBuyItem* item = new stStallBuyItem;
		item->buyItem = pItem;
		item->money = money;
		item->num = num;
		mSlots[index] = item;
	}
}

bool StallBuyItemList::UpdateToClient(GameConnection* conn, S32 index)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	StallLookUpEvent* evt = new StallLookUpEvent(index);
	conn->postNetEvent(evt);
#endif
	return true;
}

bool StallBuyItemList::UpdateTargetToClient(Player* player, Player* target, S32 index)
{
#ifdef NTJ_SERVER
	if(!player || !target)
		return false;
	GameConnection* targetConn = target->getControllingClient();
	if(targetConn)
	{
		StallLookUpTargetEvent* ev = new StallLookUpTargetEvent(player->getPlayerID(), index);
		targetConn->postNetEvent(ev);
	}
#endif
	return true;
}

S32 StallBuyItemList::findEmptySlot()
{
	for(int i=0; i<MAXSLOTS; ++i)
	{
		if(mSlots[i] == NULL || mSlots[i]->buyItem == NULL)
			return i;
	}
	return -1;
}

#ifdef NTJ_CLIENT
StallLookUpList::StallLookUpList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_STALL_LOOKUP;

	for(int i = 0; i< StallLookUpList::MAXSLOTS; ++i)
		mSlots[i] = NULL;
}
#endif


void IndividualStall::showLookUpItem(S32 page /* = 1 */)
{
#ifdef NTJ_CLIENT
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	S32 cout = mLookUpList.size();
	if(cout == 0)
	{
		Con::executef("HintInfor", "1");
		return;
	}
	else
		Con::executef("HintInfor", "0");
	S32 totalPage = cout / 3 + 1;

	Con::executef("showResultPage", Con::getIntArg(page), Con::getIntArg(totalPage));

	U32 Pos = 0;

	for(int i=0; i<cout; ++i)
	{
		if(i < (page * StallLookUpList::MAXSLOTS) && i >= (page-1) * StallLookUpList::MAXSLOTS)
		{
			if(mLookUpList[i] > 0)
			{
				ItemShortcut* pItem = ItemShortcut::CreateItem(mLookUpList[i], 1);
				if(pItem)
				{
					pPlayer->lookUpList.SetSlot(Pos, pItem);
					Con::executef("showResultItemInfo", Con::getIntArg(Pos+1), pItem->getRes()->getItemName());
					if(++Pos >= StallLookUpList::MAXSLOTS)
						Pos = 0;
				}
			}
		}
	}
#endif
}

#ifdef NTJ_CLIENT
ConsoleFunction(LookUpItem, void, 3, 3, "LookUpItem(%name， %type)")
{
	StringTableEntry name = argv[1];
	U32 iType = dAtoi(argv[2]);
	if(!name || dStrlen(name) < 2)
		return;
	Con::executef("clearLookUpList");
	g_ItemRepository->searchItemByName(name, iType);
	g_Stall->showLookUpItem(1);
}

ConsoleFunction(showLookUpItem, void, 2, 2, "showLookUpItem(%page)")
{
	S32 page = dAtoi(argv[1]);
	g_Stall->showLookUpItem(page);
}

ConsoleFunction(clearLookUpList, void, 1, 1, "clearLookUpList()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	for(int i=0; i<StallLookUpList::MAXSLOTS; ++i)
	{
		pPlayer->lookUpList.SetSlot(i, NULL);
	}
	g_Stall->clearLookUpList();
}

ConsoleFunction(AddNeedBuyItem, void, 4, 4, "AddNeedBuyItem(%index, %money, %num)")
{
	S32 iIndex = dAtoi(argv[1]);
	U32 iMoney = dAtoi(argv[2]);
	U32 iNum = dAtoi(argv[3]);

	if(iMoney < 1)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, "请输入收购物品单价");
		return;
	}

	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	ShortcutObject* pObj = pPlayer->lookUpList.GetSlot(iIndex);
	ItemShortcut* pItem = NULL;
	if(!pObj || !(pItem = (ItemShortcut*)pObj))
		return;
	U32 itemId = pItem->getRes()->getItemID();

	ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_STALL);
	ev->SetInt32ArgValues(5, STALL_ADDTOLOOKUP, itemId, iMoney, iNum, -1);
	conn->postNetEvent(ev);

	//pPlayer->lookUpList.Clear();
	Con::executef("CloseLookupWnd");
}

//确认加入购买清单
ConsoleFunction(makeSureAddToBuyList, void, 4, 4, "makeSureAddToBuyList(%index, %num, %money)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if(!currentAction)
		return;
	ClientGameplayParam* param = currentAction->getParam(0);
	if(!param)
		return;
	ShortcutObject* pObj = g_ItemManager->getShortcutSlot(pPlayer, param->getIntArg(0), param->getIntArg(1));
	if(!pObj)
		return;
	ItemShortcut* pItem = (ItemShortcut*)pObj;
	if(!pItem)
		return;
	U32 itemId = pItem->getRes()->getItemID();

	S32 iIndex = dAtoi(argv[1]);
	if(iIndex < 0 || iIndex >= InventoryList::MAXSLOTS)
		return;
	U32 iNum = dAtoi(argv[2]);
	U32 iMoney = dAtoi(argv[3]);
	if(iMoney == 0)
		return;
	g_ClientGameplayState->cancelCurrentAction();
	ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_STALL);
	ev->SetInt32ArgValues(5, STALL_ADDTOLOOKUP, itemId, iMoney, iNum, iIndex);
	conn->postNetEvent(ev);

	Con::executef("CloseWillBuyItemWnd");
}
#endif

//-------------------------------------------------------------------------------------------------------------
//摆摊宠物栏
//-------------------------------------------------------------------------------------------------------------
StallPetList::StallPetList()
{
	for (int i=0; i<MAXSLOTS; ++i)
		mSlots[i] = NULL;
}

StallPetList::~StallPetList()
{
	clear();
}

void StallPetList::clear()
{
	for(int i=0; i<MAXSLOTS; ++i)
	{
		if(mSlots[i] )
		{
			if(mSlots[i]->petSlot)
			{
				delete mSlots[i]->petSlot;
				mSlots[i]->petSlot = NULL;
			}
			delete mSlots[i];
		}
		mSlots[i] = NULL;
	}
}

void StallPetList::SetSlot(S32 index, PetShortcut* pItem, U32 money)
{
	if(mSlots[index])
	{
		if(pItem)
		{
			if(mSlots[index]->petSlot)
				delete mSlots[index]->petSlot;
			mSlots[index]->petSlot = pItem;
		}
		else
		{
			if(mSlots[index]->petSlot)
				delete mSlots[index]->petSlot;
			delete mSlots[index];
			mSlots[index] = NULL;
		}

	}
	else
	{
		stStallPet* item = new stStallPet;
		item->petSlot = pItem;
		item->money = money;
		mSlots[index] = item;
	}
}

PetShortcut* StallPetList::GetSlot(S32 index)
{
	if(index < 0 || index >= MAXSLOTS)
		return NULL;
	return mSlots[index] ? mSlots[index]->petSlot : NULL;
}

bool StallPetList::UpdateToClient(GameConnection* conn, S32 index)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	StallPetEvent* ev = new StallPetEvent(index);
	conn->postNetEvent(ev);
#endif
	return true;
}

bool StallPetList::UpdateTargetToClient(Player* player, Player* target, S32 index)
{
#ifdef NTJ_SERVER
	if(!player || !target)
		return false;
	GameConnection* targetConn = target->getControllingClient();
	if(targetConn)
	{
		StallPetTargetEvent* ev = new StallPetTargetEvent(player->getPlayerID(), index);
		targetConn->postNetEvent(ev);
	}
#endif
	return true;
}

#ifdef NTJ_SERVER
enWarnMessage ItemManager::PetListMoveToStallPetList(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAG;

	Player *pPlayer = param->player;
	if (!pPlayer || !pPlayer->petList.IsVaildSlot(param->SrcIndex) || 
		param->DestIndex < 0 || param->DestIndex > 5 )	//宠物摆摊栏位置校验
		return msg;
	PetShortcut *pSrcPetShortcut = dynamic_cast<PetShortcut*>(pPlayer->petList.GetSlot(param->SrcIndex));
	if (!pSrcPetShortcut)
		return msg;
	stPetInfo *pPetInfoSrc = (stPetInfo *)pPlayer->getPetTable().getPetInfo(pSrcPetShortcut->getSlotIndex());
	if (!pPetInfoSrc)		//此时不进行交换操作,也不需要提示错误
		return MSG_NONE;
	if (pPetInfoSrc->status != PetObject::PetStatus_Freeze)
		return MSG_PET_STATUS_NOT_IDLE;
	
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;

	PetShortcut *pNewShortcut = PetShortcut::CreatePetItem(param->SrcIndex);
	pPlayer->stallPetList.SetSlot(param->DestIndex, pNewShortcut, param->Price);
	pPlayer->stallPetList.UpdateToClient(conn, param->DestIndex);
	
	ServerGameNetEvent* event = new ServerGameNetEvent(INFO_STALL);
	event->SetInt32ArgValues(4, TRADE_SETMONEY, param->DestIndex,param->Price, 2);
	conn->postNetEvent(event);
	return MSG_NONE;
}


enWarnMessage ItemManager::addItemToLookUp(Player* player, stStallBuyItem* item)
{
	enWarnMessage msg = MSG_NONE;
	if(!item || !player)
		msg  = MSG_ITEM_ITEMDATAERROR;
	S32 iIndex = player->stallBuyItemList.findEmptySlot();
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;

	if(iIndex >= 0)
	{
		player->stallBuyItemList.mSlots[iIndex] = item;
		player->stallBuyItemList.UpdateToClient(conn, iIndex);
		ServerGameNetEvent* event = new ServerGameNetEvent(INFO_STALL);
		event->SetInt32ArgValues(4, TRADE_SETMONEY, iIndex,item->money, 3);
		conn->postNetEvent(event);
	}
	
	else
	{
		delete item;
		msg = MSG_STALL_LOOKUP_NOSPACE;
	}
	
	return msg;
}

enWarnMessage ItemManager::delPetFromStallPetList(Player* player, S32 index)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAG;
	if (!player || 	index < 0 || index > 5 )	//宠物摆摊栏位置校验
		return msg;
	PetShortcut* pPet = player->stallPetList.GetSlot(index);
	if(!pPet)
		return MSG_NONE;
	stPetInfo *pPetInfoSrc = (stPetInfo *)player->getPetTable().getPetInfo(pPet->getSlotIndex());
	if (!pPetInfoSrc)		//此时不进行交换操作,也不需要提示错误
		return MSG_NONE;

	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;

	pPetInfoSrc->status = PetObject::PetStatus_Idle;
	PetOperationManager::ServerSendUpdatePetSlotMsg(player, pPet->getSlotIndex(), stPetInfo::PetStatus_Mask);

	
	player->stallPetList.SetSlot(index, NULL, 0);
	player->stallPetList.UpdateToClient(conn, index);
	
	msg = MSG_NONE;

	return msg;

}
#endif

//-----------------------------------------------------------------------------------------------------------
//摆摊助手
#ifdef NTJ_CLIENT
TradeAndStallHelpList::TradeAndStallHelpList()
{
	VECTOR_SET_ASSOCIATION(mSlots);
}

TradeAndStallHelpList::~TradeAndStallHelpList()
{
	Clear();
}

void TradeAndStallHelpList::Clear()
{
	for(int i=0; i<mSlots.size(); ++i)
	{
		if(mSlots[i]->shortcut)
		{
			delete mSlots[i]->shortcut;
			mSlots[i]->shortcut = NULL;
		}
		mSlots[i] = NULL;
	}
	mSlots.clear();
}

void TradeAndStallHelpList::Initialize(Player* pPlayer, Sel_type nOptType)
{	
	if(!pPlayer)
		return;
	if(nOptType < sel_common || nOptType > sel_pet)
		return;
	g_Stall->setType(nOptType);
	Clear();
	if(nOptType != sel_pet)
	{
		for(int i=0; i<pPlayer->inventoryList.GetMaxSlots(); ++i)
		{
			ShortcutObject* pObj = pPlayer->inventoryList.GetSlot(i);
			if(pObj)
			{
				ItemShortcut* pItem = (ItemShortcut*)pObj;
				if(!pItem->getRes()->canBaseLimit(Res::ITEM_TRADE))
					continue;
				Res* pRes = NULL;
				if(pItem && (pRes = pItem->getRes()))
				{
					TradeHelpShortCut* pTrade = new TradeHelpShortCut;
					ItemShortcut* newItem = ItemShortcut::CreateItem(pItem);
					if(!newItem)
						continue;
					pTrade->shortcut = newItem;
					pTrade->mIndex = i;
					switch (nOptType)
					{
					case sel_common:
						{
							if(pRes->getColorLevel() < Res::COLORLEVEL_GREEN)
								mSlots.push_back(pTrade);
						}
						break;
					case sel_excellence:
						{
							if(pRes->getColorLevel() >= Res::COLORLEVEL_GREEN && pRes->getColorLevel() < Res::COLORLEVEL_PURPLE)
								mSlots.push_back(pTrade);
						}
						break;
					case sel_seldom:
						{
							if(pRes->getColorLevel() >= Res::COLORLEVEL_PURPLE)
								mSlots.push_back(pTrade);
						}
						break;
					}
				}
			}
		}
	}
	else
	{
		for (S32 i = 0; i < PET_MAXSLOTS; i++)
		{
			ShortcutObject* pObj = pPlayer->petList.GetSlot(i);
			PetShortcut* pPet = NULL;
			if(pObj && (pPet = (PetShortcut*)pObj))
			{
				stPetInfo *pPetInfoSrc = (stPetInfo *)pPlayer->getPetTable().getPetInfo(pPet->getSlotIndex());
				if (!pPetInfoSrc || pPetInfoSrc->status != PetObject::PetStatus_Idle)
					continue;

				TradeHelpShortCut* pTrade = new TradeHelpShortCut;
				PetShortcut* newPet = PetShortcut::CreatePetItem(pPet->getSlotIndex());
				pTrade->shortcut = newPet;
				pTrade->mIndex = i;
				mSlots.push_back(pTrade);
			}
		}
	}
	S32 iCount = mSlots.size();
	Con::executef("RefreshTradeHelpShow", Con::getIntArg(iCount));
}

ShortcutObject* TradeAndStallHelpList::GetSlot(S32 index)
{
	Player *pPlayer = NULL;
	if(index < 0 || index >= mSlots.size() || !(pPlayer = g_ClientGameplayState->GetControlPlayer()))
		return NULL;
	if(mSlots[index]->shortcut)
	{
		if(mSlots[index]->shortcut->isItemObject())
		{
			ItemShortcut* pItem = (ItemShortcut*)(mSlots[index]->shortcut);
			if(pItem)
				Con::executef("showTradeHelpItemName", Con::getIntArg(index), pItem->getRes()->getItemName());
		}
		else
		{
			PetShortcut* pPetCut = (PetShortcut*)(mSlots[index]->shortcut);
			if(pPetCut && pPetCut->getSlotIndex() != -1)
			{
				stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(pPetCut->getSlotIndex());
				if (pPetInfo)
				{
					Con::executef("showTradeHelpItemName", Con::getIntArg(index), pPetInfo->name);
				}
			}
		}
		return mSlots[index]->shortcut;
	}
	else
		return NULL;
}

TradeHelpShortCut* TradeAndStallHelpList::getShortcut(S32 index)
{
	if(index < 0 || index > mSlots.size())
		return NULL;
	else
		return mSlots[index];
}

ConsoleFunction(refreshTradeHelp, void, 2, 2, "refreshTradeHelp(%type)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	U32 iType = dAtoi(argv[1]);
	pPlayer->mTradeStallHelpList.Initialize(pPlayer, (TradeAndStallHelpList::Sel_type)iType);
}

ConsoleFunction(AddHelpItemOrPet, void, 2, 2, "AddHelpItemOrPet(%index)")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	S32 index = dAtoi(argv[1]);
	if(index < 0 )
		return;
	TradeHelpShortCut* pHelpShortcut = pPlayer->mTradeStallHelpList.getShortcut(index);
	if(!pHelpShortcut)
		return;
	if(g_Stall->getType() < 3)
	{
		ShortcutObject* pObject = g_ItemManager->getShortcutSlot(pPlayer, SHORTCUTTYPE_INVENTORY, pHelpShortcut->mIndex);
		if(!pObject || pObject->getSlotState() == ShortcutObject::SLOT_LOCK)
		{
			MessageEvent::show(SHOWTYPE_WARN, "此物品已经在摆摊栏中");
			return;
		}
		S32 index = pPlayer->individualStallList.findEmptyIndex();
		if(index < 0)
		{
			MessageEvent::show(SHOWTYPE_WARN, "摆摊物品栏已满");
			return;
		}
		else
		{
			Con::executef("oPenSetPrice", Con::getIntArg(index), Con::getIntArg(pHelpShortcut->mIndex));
		}
	}
	else
	{
		for(int i=0; i<StallPetList::MAXSLOTS; ++i)
		{
			PetShortcut* pet = pPlayer->stallPetList.GetSlot(i);
			if(pet)
			{
				if(pet->getSlotIndex() == pHelpShortcut->mIndex)
				{
					MessageEvent::show(SHOWTYPE_WARN, "此宠物已经在摆摊栏中");
					return;
				}
			}
		}

		S32 index = pPlayer->stallPetList.findEmptyIndex();
		if(index < 0)
		{
			MessageEvent::show(SHOWTYPE_WARN, "摆摊宠物栏已满");
			g_ClientGameplayState->cancelCurrentAction();
			return;
		}
		else
			Con::executef("openSetPetPrice", Con::getIntArg(index), Con::getIntArg(pHelpShortcut->mIndex));
	}
}
#endif


