//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/PlayerObject.h"
#include "T3D/gameConnection.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/item/Res.h"
#include "Gameplay/ClientGameplayAction.h"
#include "Gameplay/Item/Player_EquipIdentify.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ServerGameplayState.h"

#include "core/stringTable.h"


EquipIdentify gIdentify;
EquipIdentify* gIdentifyManager;

EquipIdentifyList::EquipIdentifyList():BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_IDENTIFY;
}

bool EquipIdentifyList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
#ifdef NTJ_SERVER
	if(!conn)
		return false;
	IdentifyEvent* ev = new IdentifyEvent(index, flag);
	conn->postNetEvent(ev);
#endif
	return true;
}

EquipIdentify::EquipIdentify()
{
	gIdentifyManager = this;
}

EquipIdentify::~EquipIdentify()
{

}

enWarnMessage EquipIdentify::canIdentify(Player* pPlayer, ShortcutObject* pItem1, ShortcutObject* pItem2 /* = NULL */)
{
	if(!pPlayer || !pItem1)
		return MSG_UNKOWNERROR;
	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pItem1);
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return MSG_ITEM_ITEMDATAERROR;
	//是否是装备或武器
	if(!pRes->isWeapon() && !pRes->isBody())
		return MSG_ITEM_CANNOTIDENT;
	//是否拥有鉴定属性
	if(pRes->getIdentifyType() == Res::IDENTIFYTYPE_NONE)
		return MSG_ITEM_CANNOTIDENT;
	if(pRes->IsActivatePro(EAPF_ATTACH))
		return MSG_ITEM_HADINDENTIFIED;

	/*if(pRes->getQualityLevel() > 0 && pRes->isIdentify())
		return MSG_ITEM_ITEMDATAERROR;*/

	//判断金钱
	U32 iNeedMoney = getIdentifyPric(pItem1);
	if(!pPlayer->canReduceMoney(iNeedMoney, 21))
		return MSG_PLAYER_MONEYNOTENOUGH;

	return MSG_NONE;
}
void EquipIdentify::setStatsID(Player* player, ShortcutObject* pItem1, U32 upVal, U32 lowerVal /* = 0 */)
{	
	if(!player || !pItem1)
		return ;
	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pItem1);
	Res* pRes = NULL;
	if(!pItem || !(pRes = pItem->getRes()))
		return ;
	U32	iAttributeCount = 0;	
	//有品质值，不需鉴定，直接取模板值
	if(pRes->getQualityLevel() > 0)
	{
		for(int i=0; i<ItemBaseData::MAX_APPEND_ATTRIBUTE; ++i)
		{
			U8 subID = pRes->getAppendAttribute(i);

			if(subID > 0)
			{
				U32 iStatsID = 302000000 + subID * 10000 + buildQualityLevel(pRes->getLimitLevel(), pRes->getColorLevel(), upVal, lowerVal);
				stItemInfo* stInfo = pRes->getExtData();
				stInfo->IDEProNum = iAttributeCount;
				stInfo->IDEProValue[iAttributeCount] = iStatsID;
				iAttributeCount++;
			}
		}
		pRes->setActivatePro(EAPF_ATTACH);
	}
	//没有品质
	else
	{
		stItemInfo* stInfo = pRes->getExtData();
		stInfo->Quality = buildQualityLevel(pRes->getLimitLevel(), pRes->getColorLevel(), upVal, lowerVal);
		//看是否有固定名称
		U32 ProID = pRes->getRandomName();

		if(ProID > 300 )
		{			
			U32 iStatsID = 300000000 + ProID * 10000 + stInfo->Quality;			
			stInfo->IDEProNum = 1;
			stInfo->IDEProValue[0] = iStatsID;
			stInfo->RandPropertyID = ProID;
		}	
		else if(ProID == 2)
		{
			S32 randvalue = Platform::getRandomI(1, 10000);
			Vector<U32> proList;
			U32 iPropertyNum = getStatsCount(pRes->getColorLevel());
			g_RandPropertyTable->getRandProperty(Res::PROTYPE_RANDALL,
				pRes->getSubCategory(),
				iPropertyNum,
				proList);
			stInfo->IDEProNum = proList.size();
			for(int i = 0; i < proList.size(); i++)
			{
				U32 iStatsID = 300000000 + proList[i] * 10000 + stInfo->Quality;	
				stInfo->IDEProValue[i] = iStatsID;
			}
		}
		else if(ProID == 3)
		{
			S32 randvalue = Platform::getRandomI(1, 10000);
			Vector<U32> proList;
			//名称随机
			if(randvalue > 4000)
			{
				g_RandPropertyTable->getRandProperty(Res::PROTYPE_RANDNAME,
					pRes->getSubCategory(),
					1,
					proList);
				if(proList.size() == 1)
				{
					U32 iStatsID = 300000000 + proList[0] * 10000 + stInfo->Quality;			
					stInfo->IDEProNum = 1;
					stInfo->IDEProValue[0] = iStatsID;
					stInfo->RandPropertyID = proList[0];
				}
			}
		}
		else 
		{
			S32 randvalue = Platform::getRandomI(1, 10000);
			Vector<U32> proList;
			//名称随机
			if(randvalue > 4000)
			{
				g_RandPropertyTable->getRandProperty(Res::PROTYPE_RANDNAME,
					pRes->getSubCategory(),
					1,
					proList);
				if(proList.size() == 1)
				{
					U32 iStatsID = 300000000 + proList[0] * 10000 + stInfo->Quality;			
					stInfo->IDEProNum = 1;
					stInfo->IDEProValue[0] = iStatsID;
					stInfo->RandPropertyID = proList[0];
				}
			}
			//完全随机
			else
			{
				U32 iPropertyNum = getStatsCount(pRes->getColorLevel());
				g_RandPropertyTable->getRandProperty(Res::PROTYPE_RANDALL,
					pRes->getSubCategory(),
					iPropertyNum,
					proList);
				stInfo->IDEProNum = proList.size();
				for(int i = 0; i < proList.size(); i++)
				{
					U32 iStatsID = 300000000 + proList[i] * 10000 + stInfo->Quality;	
					stInfo->IDEProValue[i] = iStatsID;
				}
			}			
		}
		pRes->setActivatePro(EAPF_ATTACH);
	}
}

void EquipIdentify::setStats(Player* player, ShortcutObject* pItem1, ShortcutObject* pItem2 /* = NULL */)
{
	if(!player || !pItem1)
		return;
	ItemShortcut* pItemcut = dynamic_cast<ItemShortcut*>(pItem2);
	if(pItemcut)
	{
		if(pItemcut->getRes()->getSubCategory() == Res::CATEGORY_ITEM_IDENTIFY)
		{
			U32 iOffset = pItemcut->getRes()->getReserveValue();
			setStatsID(player, pItem1, iOffset);
		}
	}
	else
		setStatsID(player, pItem1, 0);
}

U32 EquipIdentify::getIdentifyPric(ShortcutObject* pSlot)
{
	ItemShortcut* pItem = (ItemShortcut*)pSlot;
	if(!pItem)
		return 0;
    char* result = Con::getReturnBuffer(32);
	result = (char*)Con::executef("CalcEquipIdentifyPrice", Con::getIntArg(pItem->getRes()->getCategory()),
		Con::getIntArg(pItem->getRes()->getColorLevel()),
		Con::getIntArg(pItem->getRes()->getLimitLevel()));
	return dAtol(result);
}

void EquipIdentify::openIdentify(Player* player)
{
	if(!player)
		return;
#ifdef NTJ_SERVER
	SceneObject* obj = player->getInteraction();
	player->setInteraction(obj, Player::INTERACTION_IDENTIFY);
	enWarnMessage MsgCode = player->isBusy(Player::INTERACTION_IDENTIFY);
	if(MsgCode != MSG_NONE)
	{
		MessageEvent::send(player->getControllingClient(), SHOWTYPE_NOTIFY, MsgCode);
		return;
	}
	ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_ITEM_IDENTIFY);
	player->getControllingClient()->postNetEvent(ev);

#endif
#ifdef NTJ_CLIENT
	Con::executef("openIdentifyWnd");
#endif
}

void EquipIdentify::closeIdentify(Player* player)
{
	if(!player)
		return;
#ifdef NTJ_CLIENT
	Con::executef("closeIdentifyWnd");
#endif
	for(int i=0; i<2; ++i)
	{
		ShortcutObject* pObj = player->identifylist.GetSlot(i);
		if(pObj)
		{
			U32 iSrcIndex = pObj->getLockedItemIndex();
			player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);			
			player->identifylist.SetSlot(i, NULL, true);
		}
	}
}

#ifdef NTJ_SERVER

enWarnMessage EquipIdentify::setIdentifyResult(Player* pPlayer, ShortcutObject* pItem1, ShortcutObject* pItem2 /* = NULL */)
{
	if(!pPlayer || !pItem1)
		return MSG_UNKOWNERROR;
	
	setStats(pPlayer, pItem1, pItem2);

	U32 iSrcindex1= pPlayer->identifylist.GetSlot(0)->getLockedItemIndex();
	pPlayer->inventoryList.SetSlot(iSrcindex1, pItem1, false);


	if(iSrcindex1 >= 0)
	{
		pPlayer->inventoryList.GetSlot(iSrcindex1)->setSlotState(ShortcutObject::SLOT_COMMON);
	}
	pPlayer->identifylist.SetSlot(0, NULL, true);

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;
	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pItem1);
	Res* pRes = NULL;
	if(pItem && (pRes = pItem->getRes()))
	{
		char succ[64] = {0};

		dSprintf(succ, sizeof(succ), "经鉴定，此物品的品质为——%s",  pRes->getPostfixName());
		MessageEvent::send(conn, SHOWTYPE_NOTIFY, succ);
	}
	pPlayer->inventoryList.UpdateToClient(conn, iSrcindex1, ITEM_IDENTIFY);
	
	pPlayer->identifylist.UpdateToClient(conn, 0, ITEM_IDENTIFY);
	
	if(pItem2)
	{
		ItemShortcut* pAppend = dynamic_cast<ItemShortcut*>(pPlayer->identifylist.GetSlot(1));
		if(!pAppend)
			return MSG_ITEM_ITEMDATAERROR;
		U32 iSrcIndex2 = pAppend->getLockedItemIndex();
		pPlayer->inventoryList.GetSlot(iSrcIndex2)->setSlotState(ShortcutObject::SLOT_COMMON);
		g_ItemManager->delItemFromInventoryByIndex(pPlayer, iSrcIndex2);
		
		pPlayer->identifylist.SetSlot(1, NULL, true);
		pPlayer->identifylist.UpdateToClient(conn, 1, ITEM_IDENTIFY);

	}
	pPlayer->reduceMoney(getIdentifyPric(pItem1), 21);
	//关闭鉴定栏
	//pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);

	return MSG_NONE;
	
}

enWarnMessage EquipIdentify::setEquipIdntifyResult(Player* pPlayer, ShortcutObject* pItem)
{
	if(!pPlayer || !pItem)
		return MSG_UNKOWNERROR;
	setStats(pPlayer, pItem);
	ItemShortcut* pIemIdntify = (ItemShortcut*)pItem;
	if(!pIemIdntify)
		return MSG_UNKOWNERROR;
	U32 iIndex = pIemIdntify->getRes()->getEquipPos();
	if(iIndex < 0)
		return MSG_UNKOWNERROR;
	pPlayer->equipList.SetSlot(iIndex, pItem, false);
	pPlayer->equipList.UpdateToClient(pPlayer->getControllingClient(), iIndex, ITEM_IDENTIFY);
	
	return MSG_NONE;	
}
enWarnMessage ItemManager::InventoryMoveToIdentify(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;

	if(param == NULL)
		return msg;
	Player* player = param->player;
	if(player == NULL)
		return msg;

	if(!player->inventoryList.IsVaildSlot(param->SrcIndex) ||
		!player->identifylist.IsVaildSlot(param->DestIndex))
		return msg;	

	ShortcutObject* pSrc	= player->inventoryList.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->identifylist.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;

	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSrc);
	if(!pItem)
		return MSG_ITEM_ITEMDATAERROR;

	/*if(gIdentifyManager->canIdentify(player, pSrc, NULL) != MSG_NONE)
		return gIdentifyManager->canIdentify(player, pSrc, NULL);*/

	// 判断是否装备
	if(!pItem->getRes()->isBody() && !pItem->getRes()->isWeapon() && pItem->getRes()->getSubCategory() != Res::CATEGORY_ITEM_IDENTIFY)
		return MSG_ITEM_CANNOTDRAGTOOBJECT;

	if(pItem->getRes()->IsActivatePro(EAPF_ATTACH))
		return MSG_ITEM_HADINDENTIFIED;

	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;

	if(pDest)
	{
		// 如果在鉴定栏已经存在物品，则先删除
		U32 iSrcIndex = pDest->getLockedItemIndex();
		ItemShortcut* pDestItem = ItemShortcut::CreateItem(pItem);
		if(!pDestItem)
			return MSG_ITEM_ITEMDATAERROR;

		player->identifylist.SetSlot(param->DestIndex, pDestItem, true);
		player->identifylist.GetSlot(param->DestIndex)->setLockedItemIndex(param->SrcIndex);
		player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);		
		pSrc->setSlotState(ShortcutObject::SLOT_LOCK);
		
		player->identifylist.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
		player->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
		player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	}
	else
	{
		
		ItemShortcut* pTargetItem = ItemShortcut::CreateItem(pItem);
		if(!pTargetItem)
			return MSG_ITEM_ITEMDATAERROR;

		player->identifylist.SetSlot(param->DestIndex, pTargetItem, true);
		player->identifylist.GetSlot(param->DestIndex)->setLockedItemIndex(param->SrcIndex);

		pSrc->setSlotState(ShortcutObject::SLOT_LOCK);

		player->identifylist.UpdateToClient(conn, param->DestIndex, ITEM_NOSHOW);
		player->inventoryList.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
	}
	return MSG_NONE;
}

enWarnMessage ItemManager::IdentifyMoveToInventory(stExChangeParam* param)
{
	enWarnMessage msg = MSG_ITEM_CANNOTDRAGTOOBJECT;
	
	if(param == NULL)
		return msg;
	Player* player = param->player;
	if(player == NULL)
		return msg;

	if(!player->identifylist.IsVaildSlot(param->SrcIndex) ||
		!player->inventoryList.IsVaildSlot(param->DestIndex))
		return msg;	

	ShortcutObject* pSrc	= player->identifylist.GetSlot(param->SrcIndex);
	ShortcutObject* pDest	= player->inventoryList.GetSlot(param->DestIndex);
	if(!pSrc)
		return MSG_ITEM_ITEMDATAERROR;
	U32 iSrcIndex = pSrc->getLockedItemIndex();
	GameConnection* conn = player->getControllingClient();
	if(!conn)
		return MSG_UNKOWNERROR;
	player->identifylist.SetSlot(param->SrcIndex, NULL, true);
	player->identifylist.UpdateToClient(conn, param->SrcIndex, ITEM_NOSHOW);
	player->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
	player->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
	return MSG_NONE;
}

ConsoleFunction(openIdentify, void, 2, 2, "openIdentify(%player)")
{
	SimObject* obj = Sim::findObject(dAtoi(argv[1]));
	if(obj == NULL)
		return ;
	Player* player = dynamic_cast<Player*>(obj);
	if(player == NULL)
		return ;
	gIdentifyManager->openIdentify(player);

}

//手动鉴定
ConsoleMethod(Player, setIdentify, void, 7, 7, "setIdentify(%index, %upValue, %lowValue, %index1, %type)")
{
	S32 iIndex1 = dAtoi(argv[2]);//待鉴定物品Index
	S32 iIndex2 = dAtoi(argv[5]);//鉴定符Index
	U32 iType = dAtoi(argv[6]);
	if(iIndex1 < 0 || iIndex1 >= InventoryList::MAXSLOTS || iIndex2 < 0 || iIndex2 >= InventoryList::MAXSLOTS)
		return;
	if(!object)
		return;
	GameConnection* conn = object->getControllingClient();
	if(!conn)
		return;

	ShortcutObject* pShortCut = g_ItemManager->getShortcutSlot(object, iType, iIndex1);
	if(!pShortCut)
		return;
	ShortcutObject* pIdentify = g_ItemManager->getShortcutSlot(object, SHORTCUTTYPE_INVENTORY, iIndex2);
	if(!pIdentify)
		return;
	enWarnMessage msg = gIdentifyManager->canIdentify(object, pShortCut);
	if( msg != MSG_NONE)
	{
		MessageEvent::send(conn, SHOWTYPE_NOTIFY, msg);
		return;
	}
	if(iType != SHORTCUTTYPE_INVENTORY)
	{
		MessageEvent::send(conn, SHOWTYPE_NOTIFY, MSG_ITEM_SELECTTARGET_ERROR);
		return;
	}
	gIdentifyManager->setStatsID(object, pShortCut, dAtoi(argv[3]), dAtoi(argv[4]));
	
	object->inventoryList.GetSlot(iIndex1)->setSlotState(ShortcutObject::SLOT_COMMON);
	object->inventoryList.UpdateToClient(conn, iIndex1, ITEM_NOSHOW);
	/*else if(iType == SHORTCUTTYPE_EQUIP)
	{
		object->equipList.GetSlot(iIndex1)->setSlotState(ShortcutObject::SLOT_COMMON);
		object->equipList.UpdateToClient(conn, iIndex1, ITEM_NOSHOW);
	}*/

	g_ItemManager->delItemFromInventoryByIndex(object, iIndex2, 1);
}
#endif

#ifdef NTJ_CLIENT
ConsoleFunction(DoIdentify, void, 1, 1, "DoIdentify()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	ShortcutObject* pObj1 = pPlayer->identifylist.GetSlot(0);
	ShortcutObject* pObj2 = pPlayer->identifylist.GetSlot(1);
	if(!pObj1)
		return;
	enWarnMessage msg = gIdentifyManager->canIdentify(pPlayer, pObj1, pObj2);
	if(msg != MSG_NONE)
	{
		MessageEvent::show(SHOWTYPE_NOTIFY, msg);
		return;
	}

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;

	S32 iSrcIndex1 = -1, iSrcIndex2 = -1;
	
	iSrcIndex1 = pObj1->getLockedItemIndex();
	if(pObj2)
		iSrcIndex2 = pObj2->getLockedItemIndex();

	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_ITEM_IDENTIFY);
	event->SetIntArgValues(5, 0, SHORTCUTTYPE_INVENTORY, iSrcIndex1, SHORTCUTTYPE_INVENTORY, iSrcIndex2);
	//event->SetIntArgValues(5, 0, SHORTCUTTYPE_IDENTIFY, 0, SHORTCUTTYPE_IDENTIFY, 1);
	conn->postNetEvent(event);

}

ConsoleFunction(closeIdentify, void, 1, 1, "closeIdentify()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
	ClientGameNetEvent* event = new ClientGameNetEvent(INFO_ITEM_IDENTIFY);
	event->SetIntArgValues(5, 1, SHORTCUTTYPE_IDENTIFY, 0, SHORTCUTTYPE_IDENTIFY, 1);
	conn->postNetEvent(event);
}

ConsoleFunction(showIdentifyState, void, 1, 1, "showIdentifyState()")
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;
	ShortcutObject* pObj = pPlayer->identifylist.GetSlot(0);
	if(pObj)
	{
		U32 iMoney = gIdentifyManager->getIdentifyPric(pObj);
		if(pPlayer->canReduceMoney(iMoney, 21))
			Con::executef("activeOnIdentify", Con::getIntArg(1), Con::getIntArg(iMoney));
		else
			Con::executef("activeOnIdentify", Con::getIntArg(0), Con::getIntArg(iMoney));
	}
	else
		Con::executef("activeOnIdentify", Con::getIntArg(0), Con::getIntArg(0));
}
#endif

// ================================================================
// Help Method
// ================================================================
// ----------------------------------------------------------------------------
// 获取物品的颜色取值
S32 EquipIdentify::getColorValue(U32 colorLevel)
{
	switch(colorLevel)
	{
	case Res::COLORLEVEL_WHITE:		return 0;
	case Res::COLORLEVEL_GREEN:		return 5;
	case Res::COLORLEVEL_BLUE :		return 11;
	case Res::COLORLEVEL_PURPLE:	return 18;
	case Res::COLORLEVEL_ORANGE:	return 36;
	}
	return 0;
}

// ----------------------------------------------------------------------------
// 获取物品品质取值
S32 EquipIdentify::getQualityValue(S32 layervalue)
{
	/*if(layervalue > 0 && layervalue <= 60000)
		return 1;
	else if(layervalue > 60000 && layervalue <= 90000)
		return 2;
	else if(layervalue > 90000 && layervalue <= 96000)
		return 3;
	else if(layervalue > 96000 && layervalue <= 99000)
		return 4;
	else if(layervalue > 99000 && layervalue <= 99600)
		return 5;
	else if(layervalue > 99600 && layervalue <= 99800)
		return 6;
	else if(layervalue > 99800 && layervalue <= 99900)
		return 7;
	else if(layervalue > 99900 && layervalue <= 99960)
		return 8;
	else if(layervalue > 99960 && layervalue <= 99980)
		return 9;
	else if(layervalue > 99980 && layervalue <= 99990)
		return 10;
	else if(layervalue > 99990 && layervalue <= 99996)
		return 11;
	else if(layervalue > 99996 && layervalue <= 99999)
		return 12;
	else if(layervalue > 99999)
		return 20;*/
	if(layervalue > 0 && layervalue <= 90000)
		return 0;
	else if(layervalue > 90000 && layervalue <= 99000)
		return 4;
	else if(layervalue > 99000 && layervalue <= 99900)
		return 9;
	else if(layervalue > 99900 && layervalue <= 99990)
		return 15;
	else if(layervalue > 99990 && layervalue <= 99999)
		return 22;
	else if(layervalue > 99999)
		return 30;
	else
		return 0;
}

// ----------------------------------------------------------------------------
// 生成鉴定后物品品质等级
S32 EquipIdentify::buildQualityLevel(U32 levelLimit, U32 colorLevel, S32 upVal /* = 0 */, S32 lowVal /* = 0 */)
{
	S32 layervalue = 1;
		
	switch(colorLevel)
	{
	case Res::COLORLEVEL_GREEN:
		{
			if(upVal + 1 > lowVal + 99999)
				upVal = lowVal + 99999 -1;
			layervalue = Platform::getRandomI(upVal + 1, lowVal + 99999);
		}
		
		break;
	case Res::COLORLEVEL_BLUE:
		{
			if(upVal + 90001 > lowVal + 99999)
				upVal = lowVal + 99999 -90001;
			layervalue = Platform::getRandomI(upVal + 90001, lowVal + 99999);

		}
		break;
	case Res::COLORLEVEL_PURPLE:
		{
			if(upVal + 99001 > lowVal + 100000)
				upVal = lowVal + 100000 -99001;
			layervalue = Platform::getRandomI(upVal + 99001, lowVal + 100000);
		}
		break;
	case Res::COLORLEVEL_ORANGE:
		{
			if(upVal + 100000 > lowVal + 100000)
				upVal = lowVal + 100000 -99001;
			layervalue = Platform::getRandomI(upVal + 99001, lowVal + 100000);
		}
		break;
	}
	// 物品品质等级 = 使用等级限制 + 品质取值 + 颜色取值
	return levelLimit + getQualityValue(layervalue) + getColorValue(colorLevel);
}

// ----------------------------------------------------------------------------
//获得随机后的附加属性的个数
U32 EquipIdentify::getStatsCount(U32 colorLevel)
{
	S32 layervalue = Platform::getRandomI(1, 10000);
	switch (colorLevel)
	{
	case Res::COLORLEVEL_GREEN:
		{
			if(layervalue > 0 && layervalue <= 2400)
				return 1;
			else if(layervalue > 2400 && layervalue <= 8400)
				return 2;
			else if(layervalue > 8400 && layervalue <= 10000)
				return 3;
		}
	case Res::COLORLEVEL_BLUE:
		{
			if(layervalue > 0 && layervalue <= 2400)
				return 2;
			else if(layervalue > 2400 && layervalue <= 4800)
				return 3;
			else if(layervalue > 4800 && layervalue <= 8400)
				return 4;
			else if(layervalue > 8400 && layervalue <= 10000)
				return 5;
		}
	case Res::COLORLEVEL_PURPLE:
		{
			if(layervalue > 0 && layervalue <= 1200)
				return 4;
			else if(layervalue > 1200 && layervalue <= 4000)
				return 5;
			else if(layervalue > 4000 && layervalue <= 6000)
				return 6;
			else if(layervalue > 6000 && layervalue <= 8800)
				return 7;
			else if(layervalue > 8800 && layervalue <= 10000)
				return 8;
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------
// 获取物品档次
S32 EquipIdentify::getItemLayer(S32 layervalue)
{
	if(layervalue >= 1 && layervalue <= 90000)
		return 1;		// 普通的
	else if(layervalue > 90000 && layervalue <= 99000)
		return 2;		// 精良的
	else if(layervalue > 99000 && layervalue <= 99900)
		return 3;		// 优秀的
	else if(layervalue > 99900 && layervalue <= 99990)
		return 4;		// 完美的
	else if(layervalue > 99990 && layervalue <= 99999)
		return 5;		// 传说的
	else if(layervalue > 99999)
		return 6;		// 逆天的
	else
		return 0;
}
