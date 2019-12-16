//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "T3D/gameConnection.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/Item/Player_Bank.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/mission/PlayerMission.h"


//仓库栏
BankList::BankList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_BANK;
	mBuyBagSize = 0;
}

// ----------------------------------------------------------------------------
// 读取仓库数据
bool BankList::LoadData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	mBuyBagSize = playerInfo->MainData.BankBagSize;
	for(S32 i= 0; i < mMaxSlots; ++i)
	{
		stItemInfo& stTemp = playerInfo->MainData.BankInfo[i];
		if(stTemp.ItemID == 0)
			continue;
		mSlots[i] = ItemShortcut::CreateItem(stTemp);
	}
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 保存仓库数据
bool BankList::SaveData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	playerInfo->MainData.BankBagSize = mBuyBagSize;
	for(int i=0; i<mMaxSlots; ++i)
	{
		if(mSlots[i] == NULL)
			continue;
		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(mSlots[i]);
		stItemInfo* pTemp = NULL;
		if(pItem)
		{
			pTemp = pItem->getRes()->getExtData();
			if(pTemp)
				dMemcpy(&playerInfo->MainData.BankInfo[i], pTemp, sizeof(stItemInfo));
		}
	}
#endif
	return true;
}

void BankList::SetMaxSlot( S32 maxslots)
{
	mMaxSlots = maxslots;
}

void BankList::UpdateMaxSlots(GameConnection* conn)
{
#ifdef NTJ_SERVER
	if(conn)
	{
		ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_CHANGEMAXSLOTS);
		ev->SetInt32ArgValues(2, SHORTCUTTYPE_BANK, mMaxSlots);
		conn->postNetEvent(ev);
	}
#endif
}

enWarnMessage BankList::CanChangeMaxSlots(U32 srcBag1, U32 tagBag1)
{
	if(srcBag1 == tagBag1 )
		return MSG_NONE;

	if(tagBag1  > BankList::MAXSLOTS - mBuyBagSize)
		return MSG_ITEM_BAGLIMIT;

	if(tagBag1 > 30 )
		return MSG_ITEM_BAGLIMIT;

	if(srcBag1 > tagBag1)
	{
		for(S32 i = mBuyBagSize; i < mBuyBagSize + srcBag1; i++)
			if(mSlots[i])
				return MSG_ITEM_BAGNOEMPTY1;
	}

	return MSG_NONE;
}

void BankList::ChangeMaxSlots(Player*, U32 srcBag1, U32 tagBag1)
{
	U32 srcbags = srcBag1 + mBuyBagSize;
	U32 tagbags = tagBag1 + mBuyBagSize;
	U32 extBags = BankList::MAXSLOTS - mBuyBagSize;
	ShortcutObject** tmpSlots = new ShortcutObject*[extBags];
	for(S32 i = 0; i < extBags; ++i)
	{
		if(i < srcbags)
			tmpSlots[i] = mSlots[mBuyBagSize + i];
		else
			tmpSlots[i] = NULL;
	}

	//注：大包换小包必须大包为空包
	if(tagBag1 > srcBag1)
	{
		for(S32 i = mBuyBagSize; i < tagbags; i++)
		{
			mSlots[i] = tmpSlots[i - mBuyBagSize];

			if(i > srcbags)
				mSlots[i] = NULL;
		}
	}
	else if(tagBag1 < srcBag1)
	{
		for(S32 i = mBuyBagSize; i < srcbags; i++)
		{
			if(i < mBuyBagSize + tagBag1)
				mSlots[i] = tmpSlots[i - mBuyBagSize];
			else
				mSlots[i] = NULL;
		}
	}
	delete[] tmpSlots;
	SetMaxSlot(mBuyBagSize + tagBag1);	
}
// ----------------------------------------------------------------------------
// 更新仓库栏到客户端
bool BankList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	BankEvent* ev = new BankEvent(index, flag);
	conn->postNetEvent(ev);
#endif
	return true;
}

PlayerBank* g_PlayerBank;
PlayerBank gPlayerBank;

PlayerBank::PlayerBank()
{
	g_PlayerBank = this;
}

PlayerBank::~PlayerBank()
{
}

void PlayerBank::openBank(Player* player, U32 openType/* = 0*/)
{
	if(!player)
		return ;
#ifdef NTJ_SERVER
	if(openType == 0)
	{
		SceneObject* obj = player->getInteraction();
		player->setInteraction(obj, Player::INTERACTION_BANKTRADE);
	}
	else
	{
		player->setInteraction(player, Player::INTERACTION_BANKTRADE);
	}
	
	enWarnMessage msg = player->isBusy(Player::INTERACTION_BANKTRADE);
	if(msg != MSG_NONE)
	{
		MessageEvent::send(player->getControllingClient(), SHOWTYPE_NOTIFY, msg);
		return ;
	}
	for(U32 i = 0; i < player->bankList.GetMaxSlots(); i++)
	{
		BankEvent* ev = new BankEvent(i, ITEM_LOAD);
		player->getControllingClient()->postNetEvent(ev);
	}
	ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_BANK);
	ev->SetInt32ArgValues(1, PlayerBank::Bank_Open);
	player->getControllingClient()->postNetEvent(ev);
#endif
#ifdef NTJ_CLIENT
	U32 maxSlots = player->bankList.GetMaxSlots();
	//U32 iBindMoney = player->getMoney(Player::Currentcy_BankMoney);
	U32 iLevel = player->getLevel();
	Con::executef("openBank", Con::getIntArg(maxSlots)/*, Con::getIntArg(iBindMoney)*/, Con::getIntArg(iLevel));
	if(canBuySpace(player))
		Con::executef("setBuyActive", Con::getIntArg(1));
	else
		Con::executef("setBuyActive", Con::getIntArg(0));
	if(player->getBankFlag())
		Con::executef("setLockFlag", Con::getIntArg(1));
	else
		Con::executef("setLockFlag", Con::getIntArg(0));
#endif
}

void PlayerBank::closeBank(Player* player)
{
	if(!player)
		return;
#ifdef NTJ_CLIENT
	Con::executef("HideBank");
#endif
#ifdef NTJ_SERVER
#endif
}

bool PlayerBank::getLocked(Player* player)
{
	if(player)
		return player->getBankFlag();
	else
		return false;
}

void PlayerBank::setLocked(Player* player, bool locked)
{
	if(player)
		player->setBankFlag(locked);
#ifdef NTJ_SERVER
	player->sendPlayerEvent(Player::EventBankLockMask);
	ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_BANK);
	ev->SetInt32ArgValues(1, PlayerBank::Bank_Lock);
	player->getControllingClient()->postNetEvent(ev);
#endif
}

bool PlayerBank::canBuySpace(Player* player)
{
	if(!player)
		return false;
	U32 iLevel = player->getLevel();
	U32 iNum = player->bankList.GetMaxSlots();
	if(iLevel >= 1 && iLevel < 20)
	{
		if(iNum >= 18)
			return false;
		else
			return true;
	}
	else if(iLevel >= 20 && iLevel < 30)
	{
		if(iNum >= 24)
			return false;
		else
			return true;
	}
	else if(iLevel >= 30 && iLevel < 50)
	{
		if(iNum >= 30)
			return false;
		else
			return true;
	}
	else if(iLevel >= 50 && iLevel < 70)
	{
		if(iNum >= 42)
			return false;
		else 
			return true;
	}
	else if(iLevel >= 70 && iLevel < 90)
	{
		if(iNum >= 54)
			return false;
		else
			return true;
	}
	else if(iLevel >= 90 && iLevel < 110)
	{
		if(iNum >= 66)
			return false;
		else
			return  true;
	}
	else if(iLevel >= 110)
	{
		if(iNum >= 90)
			return false;
		else
			return true;
	}
	else
		return false;
}

U32 PlayerBank::NeedMoney(Player* player)
{
	if(!player)
		return -1;
	U32 iNum = player->bankList.GetMaxSlots();
	if(iNum == 12)
		return 0;

	return 0;
}

bool PlayerBank::canSaveMoney(Player* player, U32 money)
{
	if(!player)
		return false;
	if(player->getMoney() < money)
		return false;
	return true;
}

bool PlayerBank::canTakeMoney(Player* player, U32 money)
{
	if(!player)
		return false;
	if(player->getMoney(Player::Currentcy_BankMoney) < money)
		return false;
	return true;
}

#ifdef NTJ_SERVER
//包裹物品存到银行仓库
enWarnMessage PlayerBank::addItemToBank(Player* player, ItemShortcut* pItem, U32 num)

{
	if(!pItem || !player || num <= 0)
		return MSG_ITEM_ITEMDATAERROR;

	Res* pSrcRes = NULL;
	// 判断是否处理银行交易状态
	if (!(pSrcRes = pItem->getRes()) || !pSrcRes->canBaseLimit(Res::ITEM_SAVEBANK))
		return MSG_ITEM_BANK_SAVEERROR;
	//该物品最大叠加数
	U32 iMaxLapOverNum	= pSrcRes->getMaxOverNum();
	S32 iNeed = num;

	Vector<S32> emptyslotlist;
	U32 iEmptySlotNums = 0;
	//统计银行仓库的空槽位数量
	iEmptySlotNums = player->bankList.QueryEmptySlot(emptyslotlist);

	if(iMaxLapOverNum > 1)
	{
		Vector<S32> slotlist;
		S32 iEmpty = iEmptySlotNums;
		S32 iCount = player->bankList.QuerySameObject(pItem, slotlist, true);
		if(iCount > 0)
		{
			int iNum = 0;
			for(S32 i = 0; i < iCount; i++)
			{
				ShortcutObject* pSlot = player->bankList.GetSlot(slotlist[i]);
				ItemShortcut* pItem1 = NULL;
				if(pSlot && (pItem1 = dynamic_cast<ItemShortcut*>(pSlot)))
				{
					iNum += (iMaxLapOverNum - pItem1->getRes()->getQuantity());
				}						
			}
			if(iNeed > iNum)
				iNeed -= iNum;
			else
				iNeed = 0;
		}

		if(iNeed != 0)
		{
			iEmpty -= (iNeed / iMaxLapOverNum + 1);
			if(iEmpty < 0)
				return MSG_ITEM_BANK_NOSPACE; // 包裹满了
		}

		iNeed = num;		
		for(S32 i = 0; i < iCount; ++i)
		{
			ShortcutObject* pSlot = player->bankList.GetSlot(slotlist[i]);
			ItemShortcut* pTargetItem = NULL;
			if(pSlot && (pTargetItem = dynamic_cast<ItemShortcut*>(pSlot)))
			{
				if(pTargetItem->getRes()->getQuantity() + iNeed >= iMaxLapOverNum)
				{
					iNeed -= (iMaxLapOverNum - pTargetItem->getRes()->getQuantity());
					pTargetItem->getRes()->setQuantity(iMaxLapOverNum);
				}
				else
				{
					pTargetItem->getRes()->setQuantity(pTargetItem->getRes()->getQuantity() + iNeed);
					iNeed = 0;
				}
				player->bankList.UpdateToClient(player->getControllingClient(), slotlist[i], ITEM_SAVEBANK);

			}
		}

		if(iNeed > 0)
		{
			S32 pos = 0;
			do 
			{
				AssertFatal(iEmptySlotNums > 0, "怎么可能没有空槽位可用?");
				ItemShortcut* pItem1 = ItemShortcut::CreateItem(pItem);
				if(!pItem1)
					return MSG_ITEM_ITEMDATAERROR;

				if(iNeed > iMaxLapOverNum)
				{
					pItem1->getRes()->setQuantity(iMaxLapOverNum);
					iNeed -= iMaxLapOverNum;
				}
				else
				{
					pItem1->getRes()->setQuantity(iNeed);
					iNeed = 0;
				}

				player->bankList.SetSlot(emptyslotlist[pos], pItem1);
				player->bankList.UpdateToClient(player->getControllingClient(), emptyslotlist[pos], ITEM_SAVEBANK);
				iEmptySlotNums --;
				pos++;
			} while (iNeed > 0);
		}

		// 同步快捷栏
		g_ItemManager->syncPanel(player, pItem->getRes()->getItemID());
		
	}
	else
	{
		if(iEmptySlotNums < 0)
		{
			return MSG_ITEM_BANK_NOSPACE;		// 银行仓库已满,交易失败
		}
		else
		{
			player->bankList.SetSlot(emptyslotlist[0], pItem, false);
			player->bankList.UpdateToClient(player->getControllingClient(), emptyslotlist[0], ITEM_SAVEBANK);

		}
	}
	return MSG_NONE;
}

void PlayerBank::savingMoney(Player* player, U32 money)
{
	if(player)
	{
		player->addMoney(money, Player::Currentcy_BankMoney);
		player->reduceMoney(money);

		ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_BANK);
		ev->SetInt32ArgValues(2, PlayerBank::Bank_SaveMoney, player->getMoney(Player::Currentcy_BankMoney));
		player->getControllingClient()->postNetEvent(ev);
	}
	
}

void PlayerBank::takeMoney(Player* player, U32 money)
{
	if(player)
	{	
		player->reduceMoney(money, Player::Currentcy_BankMoney);
		player->addMoney(money);

		ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_BANK);
		ev->SetInt32ArgValues(2, PlayerBank::Bank_GetMoney, player->getMoney(Player::Currentcy_BankMoney));
		player->getControllingClient()->postNetEvent(ev);
	}
	
}

void PlayerBank::buySpace(Player* player)
{
	if(!player)
		return;
	if(!canBuySpace(player))
		return;
	U32 iMoney = player->getMoney();
	if(iMoney < NeedMoney(player))
	{
		MessageEvent::send(player->getControllingClient(), SHOWTYPE_NOTIFY, MSG_PLAYER_MONEYNOTENOUGH);
		return;
	}
	U32 iMaxSlot = player->bankList.GetMaxSlots();
	U32 iCurrentMax = iMaxSlot + 6;
	player->bankList.mBuyBagSize += 6;
	player->bankList.SetMaxSlot(iCurrentMax);
	player->bankList.UpdateMaxSlots(player->getControllingClient());
	player->reduceMoney(iMoney);	
}

bool PlayerBank::checkPassWord(Player* player, StringTableEntry word)
{
	if(!player)
		return false;
	if(dStricmp(word, player->getSecondPassWord()) == 0)
	{
		player->setBankFlag(false);
		return true;
	}
	return false;
}

void PlayerBank::setPassWord(Player* player, StringTableEntry word)
{
	if(!player)
		return;	
}

enWarnMessage ItemManager::BankMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(!player->bankList.IsVaildSlot(param->SrcIndex) ||
		!player->inventoryList.IsVaildSlot(param->DestIndex))
		return msg;	
	
	if(player->getBankFlag())
		return MSG_BANK_LOCK;

	ShortcutObject* pSrc	= player->bankList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->inventoryList.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;
	ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrc);
	if(!pSrcItem)
		return MSG_ITEM_ITEMDATAERROR;

	U32 SrcItemID  = pSrcItem->getRes()->getItemID();
	U32 TagItemID  = 0;
	if(pDest && isSameItem(pSrc, pDest))
	{
		ItemShortcut* pDestItem = dynamic_cast<ItemShortcut*>(pDest);
		if(pDestItem)
		{
			TagItemID = pDestItem->getRes()->getItemID();
			S32 iCount = pSrcItem->getRes()->getQuantity() + pDestItem->getRes()->getQuantity();
			S32 iMaxNum = pDestItem->getRes()->getMaxOverNum();
			if(iCount <= iMaxNum)
			{
				player->inventoryList.SetSlot(param->SrcIndex, NULL, true);
				pDestItem->getRes()->setQuantity(iCount);
			}
			else
			{
				pSrcItem->getRes()->setQuantity(iCount - iMaxNum);
				pDestItem->getRes()->setQuantity(iMaxNum);
			}
		}
	}
	else
	{
		// 目标为空或有物品但非同类物品，直接交换
		player->bankList.SetSlot(param->SrcIndex, pDest, false);
		player->inventoryList.SetSlot(param->DestIndex, pSrc, false);
	}
	player->bankList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	player->inventoryList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);

	//需要更新任务物品计数
	g_MissionManager->UpdateTaskItem(player, SrcItemID);
	g_MissionManager->UpdateTaskItem(player, TagItemID);
	return MSG_NONE;
}

enWarnMessage ItemManager::BankMoveToBank(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	Player* player = param->player;
	if(param == NULL || player == NULL)
		return msg;

	if(!player->bankList.IsVaildSlot(param->SrcIndex) ||
		!player->bankList.IsVaildSlot(param->DestIndex))
		return msg;	
	if(player->getBankFlag())
		return MSG_BANK_LOCK;
	ShortcutObject* pSrc	= player->bankList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->bankList.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;
	ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrc);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;
	if(pDest && isSameItem(pSrc, pDest))
	{
		ItemShortcut* pDestItem = dynamic_cast<ItemShortcut*>(pDest);
		if(pDestItem)
		{
			S32 iCount = pSrcItem->getRes()->getQuantity() + pDestItem->getRes()->getQuantity();
			S32 iMaxNum = pDestItem->getRes()->getMaxOverNum();
			if(iCount <= iMaxNum)
			{
				player->bankList.SetSlot(param->SrcIndex, NULL, true);
				pDestItem->getRes()->setQuantity(iCount);
			}
			else
			{
				pSrcItem->getRes()->setQuantity(iCount - iMaxNum);
				pDestItem->getRes()->setQuantity(iMaxNum);
			}
		}
	}
	else
	{
		// 目标为空或有物品但非同类物品，直接交换
		player->bankList.SetSlot(param->SrcIndex, pDest, false);
		player->bankList.SetSlot(param->DestIndex, pSrc, false);
	}
	player->bankList.UpdateToClient(player->getControllingClient(), param->SrcIndex, ITEM_NOSHOW);
	player->bankList.UpdateToClient(player->getControllingClient(), param->DestIndex, ITEM_NOSHOW);

	return MSG_NONE;
}
#endif

#ifdef NTJ_SERVER
ConsoleMethod(Player, openBank, void, 3, 3, "openBank(%openType)")
{
	g_PlayerBank->openBank(object, dAtol(argv[2]));
}
#endif
#ifdef NTJ_CLIENT
ConsoleFunction(closeBank, void, 1, 1, "closeBank()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(1, PlayerBank::Bank_Close);
	conn->postNetEvent(event);
}
//检验密码
ConsoleFunction(checkPassWord, void, 2, 2, "checkPassWord(%password)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(1, PlayerBank::Bank_CheckPassWord);
	
	event->SetStringArgValues(1, argv[1]);
	conn->postNetEvent(event);
}
//设置密码
ConsoleFunction(setPassWord, void, 2, 2, "setPassWord(%password)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(1, PlayerBank::Bank_SetPassWord);
	event->SetStringArgValues(1, argv[1]);
	conn->postNetEvent(event);
}
//锁定
ConsoleFunction(setBankLock, void, 1, 1, "setBankLock()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(1, PlayerBank::Bank_Lock);
	conn->postNetEvent(event);
}
//存钱
ConsoleFunction(saveMoney, void, 2, 2, "saveMoney(%money)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	if(player->getBankFlag())
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_BANK_LOCK);
		return;
	}
	U32 iMoney = dAtoi(argv[1]);
	if(!g_PlayerBank->canSaveMoney(player, iMoney))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, "输入金钱有误");
		return;
	}
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(2, PlayerBank::Bank_SaveMoney, iMoney);
	conn->postNetEvent(event);
}
//全存
ConsoleFunction(saveAllMoney, void, 1, 1, "saveAllMoney()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	if(player->getBankFlag())
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_BANK_LOCK);
		return;
	}
	U32 iMoney = player->getMoney(Player::Currentcy_Money);
	if(!g_PlayerBank->canSaveMoney(player, iMoney))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, "输入金钱有误");
		return;
	}
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(2, PlayerBank::Bank_SaveMoney, iMoney);
	conn->postNetEvent(event);
}
//取钱
ConsoleFunction(getMoney, void, 2, 2, "getMoney(%money)")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	if(player->getBankFlag())
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_BANK_LOCK);
		return;
	}
	U32 iMoney = dAtoi(argv[1]);
	if(!g_PlayerBank->canTakeMoney(player, iMoney))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, "输入金钱有误");
		return;
	}
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(2, PlayerBank::Bank_GetMoney, iMoney);
	conn->postNetEvent(event);
}
//全取
ConsoleFunction(takeAllMoney, void, 1, 1, "takeAllMoney()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	if(player->getBankFlag())
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_BANK_LOCK);
		return;
	}
	U32 iMoney = player->getMoney(Player::Currentcy_BankMoney);
	if(!g_PlayerBank->canTakeMoney(player, iMoney))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, "输入金钱有误");
		return;
	}
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(2, PlayerBank::Bank_GetMoney, iMoney);
	conn->postNetEvent(event);
}
//整理
ConsoleFunction(bankCleanUp, void, 1, 1, "bankCleanUp()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(1, PlayerBank::Bank_CleanUp);
	conn->postNetEvent(event);
}
//购买空间
ConsoleFunction(buySpace, void, 1, 1, "buySpace()")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return ;
	if(!g_PlayerBank->canBuySpace(player))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, "不能买更多的空间");
		return;
	}
	if(player->getMoney() < g_PlayerBank->NeedMoney(player))
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, MSG_PLAYER_MONEYNOTENOUGH);
		return;
	}
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
	event->SetInt32ArgValues(1, PlayerBank::Bank_BuySpace);
	conn->postNetEvent(event);
}
#endif