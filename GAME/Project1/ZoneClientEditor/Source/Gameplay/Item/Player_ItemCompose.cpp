//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "util/stringUnit.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/Item/Player_ItemCompose.h"


ItemCompose::ItemCompose():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_ITEMCOMPOSE;
	mLock = false;
	clearComposeInfo();
}

enWarnMessage ItemCompose::canItemCompose(Player* pPlayer)
{
	if(!pPlayer)
		return MSG_UNKOWNERROR;

	//交互状态
	enWarnMessage msg = MSG_NONE;
	msg = pPlayer->isBusy(Player::INTERACTION_NONE);
	if(msg != MSG_NONE)
		return msg;

	//坐骑状态
	if(pPlayer->isMounted())
		return MSG_PLAYER_RIDE;

	//产物未拾取
	if(mSlots[0])
		return MSG_ITEMCOMPOSE_NOPICKUP;

	//未放入任何材料
	if(isEmptyMaterial())
		return MSG_ITEMCOMPOSE_NOTHING;

	//特殊合成材料是否满足
	if(mItemIdTag != 0)
	{
		for (U8 i=0; i<RECORD_MAXINFO; i++)
		{
			U32 iItemId = mComposeInfo[i].iItemId;
			if(iItemId == 0)
				continue;

			S32 iQuantity = pPlayer->inventoryList.GetItemCount(iItemId);
			if(iQuantity < mComposeInfo[i].iQuantity)
				return MSG_PLAYER_MATERIAL_NOTENOUGH;
		}
	}

	return MSG_NONE;
}

bool ItemCompose::startItemCompose(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	enWarnMessage msg = MSG_NONE;
	msg = canItemCompose(pPlayer);
#ifdef NTJ_CLIENT
	if(msg == MSG_NONE)
		return sendToComposeStart(pPlayer);
	else
		MessageEvent::show(SHOWTYPE_ERROR,msg);
#endif

#ifdef NTJ_SERVER
	if(msg == MSG_NONE)
	{
		std::string szIteminfo;
		char szStr[128] = {0,};

		dSprintf(szStr,sizeof(szStr),"%d",mItemIdTag);
		szIteminfo += szStr;

		Res* pRes = NULL;
		ItemShortcut* pItem = NULL;
		for (U8 i=1; i<MAXSLOTS; i++)
		{
			pItem = dynamic_cast<ItemShortcut*>(mSlots[i]);
			if(!pItem || !(pRes = pItem->getRes()))
				continue;

			dSprintf(szStr,sizeof(szStr),"|%d " "%d",pRes->getItemID(),pRes->getQuantity());
			szIteminfo += szStr;
		}

		std::string to;
		to = Con::executef("serverItemCompose",
			      Con::getIntArg(pPlayer->getId()),
				  szIteminfo.c_str());

		//合成失败
		if(dStrcmp(to.c_str(),"0") == 0)
		{
			MessageEvent::send(conn,SHOWTYPE_ERROR,MSG_ITEMCOMPOSE_ERROR);
			return false;
		}

		U32 iItemId = atoi(StringUnit::getUnit(to.c_str(),0," |\n"));
		U16 iQuantity = atoi(StringUnit::getUnit(to.c_str(),1," |\n"));
		ItemShortcut* pNewItem = ItemShortcut::CreateItem(iItemId,iQuantity);
		//物品数据出错
		if(!pNewItem)
		{
			MessageEvent::send(conn,SHOWTYPE_ERROR,MSG_ITEM_ITEMDATAERROR);
			return false;
		}

		S32 playerId = pPlayer->getPlayerID();

		//扣除玩家包裹材料
		for(U8 i=1; i<MAXSLOTS; i++)
		{
			pItem = dynamic_cast<ItemShortcut*>(mSlots[i]);
			if(!pItem || !(pRes = pItem->getRes()))
				continue;
			g_ItemManager->putItem(playerId,pRes->getItemID(),-pRes->getQuantity());
		}

		msg = g_ItemManager->batchItem(playerId);
		if(msg != MSG_NONE)
		{
			MessageEvent::send(conn,SHOWTYPE_ERROR,msg);
			return false;
		}

		//清除合成栏材料
		for(U8 i=1; i<MAXSLOTS; i++)
		{
			SAFE_DELETE(mSlots[i]);
			UpdateToClient(conn,i);
		}

		//产出合成物品
		mSlots[0] = pNewItem;
		UpdateToClient(conn,0);
		return true;
	}
	else
	{
		MessageEvent::send(conn,SHOWTYPE_ERROR,msg);
	}
#endif

	return false;
}

void ItemCompose::finishItemCompose(Player* pPlayer)
{
	if(!pPlayer)
		return;

	if(isLock())
		setLock(false);

#ifdef NTJ_SERVER
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

	ServerGameNetEvent* pEvent =  new ServerGameNetEvent(INFO_ITEMCOMPOSE);
	pEvent->SetIntArgValues(1,ITEMCOMPOSE_END);
	conn->postNetEvent(pEvent);
#endif

#ifdef NTJ_CLIENT
	Con::executef("CloseFetchTimeProgress");
#endif
	
}

void ItemCompose::clearComposeInfo()
{
	mItemIdTag = 0;
	dMemset(&mComposeInfo,0,sizeof(mComposeInfo));
}

U8 ItemCompose::setComposeInfo(const char* string)
{
	U8 stringCount = 0;
	if(NULL == string)
		return stringCount;

	mItemIdTag = atoi(StringUnit::getUnit(string,0,"|\n"));
	if(mItemIdTag == 0)
	{
		AssertFatal(false,"ItemCompose::特殊合成产物ID非法!!");
		return stringCount;
	}

	//产物不统计在内
	stringCount = StringUnit::getUnitCount(string,"|\n");
	if(stringCount == 1)
	{
		AssertFatal(false,"ItemCompose::特殊合成无材料列表!!");
		return 0;
	}

	for (U8 i=1; i<stringCount; i++)
	{
		std::string szTemp = StringUnit::getUnit(string,i,"|\n");
		mComposeInfo[i-1].iItemId   = atoi(StringUnit::getUnit(szTemp.c_str(),0," \n")) ;
		mComposeInfo[i-1].iQuantity = atoi(StringUnit::getUnit(szTemp.c_str(),1," \n"));
	}

	return stringCount - 1;
}

bool ItemCompose::isEmptyMaterial()
{
	bool bEmpty = true;
	for (U8 i=1; i<MAXSLOTS; i++)
	{
		if(mSlots[i])
		{
			bEmpty = false;
			break;
		}
	}

	return bEmpty;
}

#ifdef NTJ_SERVER
bool ItemCompose::LoadData(stPlayerStruct* playerInfo)
{
	if(!playerInfo)
		return false;

	stShortCutInfo& pInfo = playerInfo->MainData.ComposeInfo;
	if(pInfo.ShortCutID !=0 && pInfo.ShortCutType == ShortcutObject::SHORTCUT_ITEM)
		mSlots[0] = ItemShortcut::CreateItem(pInfo.ShortCutID,pInfo.ShortCutNum);

	return true;
}

bool ItemCompose::SaveData(stPlayerStruct* playerInfo)
{
	if(!playerInfo)
		return false;
	Res* pRes = NULL;

	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(mSlots[0]);
	if(pItem && (pRes = pItem->getRes()))
	{
		playerInfo->MainData.ComposeInfo.ShortCutID = pRes->getItemID();
		playerInfo->MainData.ComposeInfo.ShortCutNum = pRes->getQuantity();
		playerInfo->MainData.ComposeInfo.ShortCutType = ShortcutObject::SHORTCUT_ITEM;
	}

	return true;
}

bool ItemCompose::sendInitialData(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	UpdateToClient(conn,0);
	return true;
}

bool ItemCompose::UpdateToClient(GameConnection* conn, S32 index)
{
	if(!conn || index<0 || index>=MAXSLOTS)
		return false;

	ItemComposeEvent* pEvent = new ItemComposeEvent(index);
	conn->postNetEvent(pEvent);

	return true;
}

void ItemCompose::cancelItemCompose(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return;

	for (U8 i=1; i<MAXSLOTS; i++)
	{
		if(!mSlots[i])
			continue;

		SAFE_DELETE(mSlots[i]);
		UpdateToClient(conn,i);
	}

	if(isLock())
		setLock(false);

	if(mItemIdTag != 0)
		clearComposeInfo();

	if(pPlayer->pScheduleEvent)
		pPlayer->pScheduleEvent->cancelEvent(pPlayer);
}

enWarnMessage ItemCompose::pickupItemByIndex(Player* pPlayer,S32 iIndex)
{
	GameConnection* conn = NULL;
	if(!pPlayer|| !(conn = pPlayer->getControllingClient()))
		return MSG_UNKOWNERROR;

	//位置合法性检查
	if(iIndex<0 && iIndex>=MAXSLOTS)
		return MSG_ITEMSPLIT_INDEXERROR;

	//无效目标槽位
	ShortcutObject* pShortcut = mSlots[iIndex];
	if(!pShortcut)
		return MSG_ITEMSPLIT_NOSHORTCUT;

	//合成产物
	if(iIndex == 0)
	{
		Res* pRes = NULL;
		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
		if(!pItem || !(pRes = pItem->getRes()))
			return MSG_ITEM_ITEMDATAERROR;

		enWarnMessage msg = MSG_NONE;
		S32 temIndex = 0;
		msg = g_ItemManager->addItemToInventory(pPlayer,pRes->getItemID(),temIndex,pRes->getQuantity(),ITEM_COMPOSE);
		if(msg == MSG_NONE)
		{
			SAFE_DELETE(mSlots[iIndex]);
			UpdateToClient(conn,iIndex);
		}
		else
			return msg;
	}
	else  //合成材料
	{
		SAFE_DELETE(mSlots[iIndex]);
		UpdateToClient(conn,iIndex);
	}
	return MSG_NONE;
}

enWarnMessage ItemCompose::InventoryToItemCompose(stExChangeParam* param)
{
	if(isLock())
		return MSG_ITEMCOMPOSE_LOCK;

	//参数有效性检查
	if(!param)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	GameConnection* conn = NULL;
	Player* pPlayer = param->player;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	//位置检查
	if(param->DestIndex == 0 || !pPlayer->inventoryList.IsVaildSlot(param->SrcIndex))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	ShortcutObject* pShortcutSrc = pPlayer->inventoryList.GetSlot(param->SrcIndex);
	
	Res* pRes = NULL;
	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcutSrc);
	if(!pItem || !(pRes = pItem->getRes()))
		return MSG_ITEM_ITEMDATAERROR;


	ItemShortcut* pNewItem = ItemShortcut::CreateItem(pRes->getItemID(),pRes->getQuantity());
	if(!pNewItem)
		return MSG_ITEM_ITEMDATAERROR;
	
	//放到目标位置
	pPlayer->mItemComposeList.SetSlot(param->DestIndex,pNewItem);
	UpdateToClient(conn,param->DestIndex);

	return MSG_NONE;
}

enWarnMessage ItemCompose::ItemComposeToInventory(stExChangeParam* param)
{
	if(isLock())
		return MSG_ITEMCOMPOSE_LOCK;

	//参数有效性检查
	if(!param)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	GameConnection* conn = NULL;
	Player* pPlayer = param->player;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	//位置检查
	if(!pPlayer->mItemComposeList.IsVaildSlot(param->SrcIndex)|| !pPlayer->inventoryList.IsVaildSlot(param->DestIndex))
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	//取回合成材料
	if(param->SrcIndex>=1 && param->SrcIndex<MAXSLOTS)
	{
		SAFE_DELETE(mSlots[param->SrcIndex]);
		UpdateToClient(conn,param->SrcIndex);
	}
	else   //拾取合成产物
	{
		ShortcutObject* pShortcutSrc = mSlots[param->SrcIndex];
		ShortcutObject* pShortcutDest = pPlayer->inventoryList.GetSlot(param->DestIndex);

		Res* pRes = NULL;
		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcutSrc);
		if(!pItem || !(pRes = pItem->getRes()))
			return MSG_ITEM_ITEMDATAERROR;

		//目标已有物品
		if(pShortcutDest)
			return MSG_ITEM_OBJECTHASITEM;
		
		ItemShortcut* pNewItem = ItemShortcut::CreateItem(pRes->getItemID(),pRes->getQuantity());
		if(!pNewItem)
			return MSG_ITEM_ITEMDATAERROR;

		SAFE_DELETE(mSlots[param->SrcIndex]);
		UpdateToClient(conn,param->SrcIndex);

		pPlayer->inventoryList.SetSlot(param->DestIndex,pNewItem);
		pPlayer->inventoryList.UpdateToClient(conn,param->DestIndex,ITEM_COMPOSE);
	}

	return MSG_NONE;
}

ConsoleFunction(openItemComposeWnd,bool,2,3,"openItemComposeWnd(%playerId,%descInfo)")
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer(dAtoi(argv[1]));
	if(!pPlayer)
		return false;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return false;

	//清除原来合成信息
	pPlayer->mItemComposeList.clearComposeInfo();

	ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_ITEMCOMPOSE);
	pEvent->SetIntArgValues(1,ItemCompose::ITEMCOMPOSE_OPENWND);

	if(argc == 2)
	{
		pEvent->SetStringArgValues(1,"");
	}
	else
	{
		pEvent->SetStringArgValues(1,argv[2]);
		pPlayer->mItemComposeList.setComposeInfo(argv[2]);
	}

	return conn->postNetEvent(pEvent);
}
#endif

#ifdef NTJ_CLIENT
bool ItemCompose::sendToComposeStart(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_ITEMCOMPOSE);
	pEvent->SetIntArgValues(1,ITEMCOMPOSE_START);
	if(conn->postNetEvent(pEvent))
	{
		setLock(true);
		Con::executef("InitFetchTimeProgress",
			Con::getIntArg(mReadyTime),
			Con::getIntArg((S32)VocalStatus::VOCALSTATUS_ITEMCOMPOSE),
			"");
		return true;
	}
	return false;
}

bool ItemCompose::sendToComposeCancel(Player* pPlayer)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	if(isLock())
		setLock(false);

	if(mItemIdTag != 0)
		clearComposeInfo();

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_ITEMCOMPOSE);
	pEvent->SetIntArgValues(1,ITEMCOMPOSE_CANCEL);
	return conn->postNetEvent(pEvent);
}

bool ItemCompose::sendToPickupItemByIndex(Player* pPlayer,U8 iIndex)
{
	GameConnection* conn = NULL;
	if(!pPlayer || !(conn = pPlayer->getControllingClient()))
		return false;

	//位置合法性检查
	if(iIndex<0 && iIndex>=MAXSLOTS)
		return MSG_ITEMSPLIT_INDEXERROR;

	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_ITEMCOMPOSE);
	pEvent->SetIntArgValues(2,ITEMCOMPOSE_PICKUP,iIndex);
	return conn->postNetEvent(pEvent);
}

ConsoleFunction(uiItemComposeStart,void,1,1,"uiItemComposeStart()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	VocalStatus* pStatus = g_ClientGameplayState->pVocalStatus;
	if(!pStatus)
		return;

	// 玩家正忙
	if(pStatus->getStatus() && pStatus->getStatusType() != VocalStatus::VOCALSTATUS_NONE)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY,MSG_PLAYER_BUSY);
		return;
	}

	pPlayer->mItemComposeList.startItemCompose(pPlayer);
}

ConsoleFunction(uiItemComposeCancel,void,1,1,"uiItemComposeCancel()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	VocalStatus* pStatus = g_ClientGameplayState->pVocalStatus;
	if(pStatus && pStatus->getStatus() && pStatus->getStatusType() == VocalStatus::VOCALSTATUS_ITEMCOMPOSE)
		Con::executef("cancleFetchTimeProgress");

	pPlayer->mItemComposeList.sendToComposeCancel(pPlayer);
}
#endif

//字符串比较函数，根据过滤符,按单元字符查找比较
ConsoleFunction(strcmpBySet,bool,4,4,"strcmpBySet(%str1,%str2,%set)")
{
	char set[128] ={0,};
	dStrcpy(set,sizeof(set),argv[3]);

	U16 wordCount1 = StringUnit::getUnitCount(argv[1],set);
	U16 wordCount2 = StringUnit::getUnitCount(argv[2],set);

	if(wordCount1 != wordCount2)
		return false;

	Vector<const char*> strbuff2;
	for (int i =0;i<wordCount1; i++)
	{
		const char* pBuff = StringUnit::getUnit(argv[2],i,set);
		strbuff2.push_back(pBuff);
	}

	if(strbuff2.size() != wordCount1)
		return false;

	for (int i=0 ;i<wordCount1; i++)
	{
		const char* pStr = StringUnit::getUnit(argv[1],i,set);
		bool bEqual = false;
		for (int i =0; i<strbuff2.size();i++)
		{
			if(dStrcmp(pStr,strbuff2[i]) == 0)
			{
				bEqual = true;
				strbuff2.erase_fast(i);
				break;
			}
		}

		if(!bEqual)
			return false;
	}

	return true;
}