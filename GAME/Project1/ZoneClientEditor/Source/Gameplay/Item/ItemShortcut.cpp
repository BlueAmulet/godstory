//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/item/Res.h"
#include <windows.h>
#include "base/bitStream.h"
#include "Common/PlayerStruct.h"
#include "Gameplay/Item/ItemShortcut.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/Item/Player_EquipIdentify.h"
#include <time.h>

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#endif

#define BuildFirstTime //20100301 

// ----------------------------------------------------------------------------
// 生成物品的世界唯一ID
unsigned __int64 GeneralItemUID()
{
	U32 zoneid = 0;
#ifdef NTJ_SERVER
	zoneid = g_ServerGameplayState->getZoneId();
#else
	zoneid = 999999;
#endif

	struct tm tmCurtime = {0,0,0,1,2,110,1,59,0};//2010-03-01 00:00:00
	time_t curtime, start;
	time( &curtime );
	start = mktime( &tmCurtime);

	static int times = 0;
	unsigned __int64 var = 0;											// total: 64bit
	var |= ((unsigned __int64)(0xFFFFFFF & (curtime - start))) << 36;	// 28bit--距离2010030
	var |= ((unsigned __int64)(0xFFFFF & zoneid)) << 16;				// 20bit--MapID
	var |= ((unsigned __int64)(0xFFFF &times));							// 16bit--序列id
	if(++times > 65535) times = 0;
	//note: 1秒内应该不会有65535个物品同时产生吧，神啊!
	return var;
}

ItemShortcut::ItemShortcut()
{
	mRes = NULL;
	mShortcutType = SHORTCUT_ITEM;
}

// ----------------------------------------------------------------------------
// 创造物品对象1
ItemShortcut::ItemShortcut(S32 itemID, S32 quantity, bool isIgnore /* = false */)
{
	mShortcutType = SHORTCUT_ITEM;
	mRes = new Res;
	if(mRes->setBaseData(itemID) && mRes->setExtData())
	{
		mRes->setQuantity(quantity, isIgnore);
		mRes->setUID(GeneralItemUID());
	}
	else
	{
		delete mRes;
		mRes = NULL;
	}
}

// ----------------------------------------------------------------------------
// 创造物品对象2
ItemShortcut::ItemShortcut(ItemShortcut* pItem)
{
	if(!pItem || !pItem->getRes())
		return;

	mRes = new Res;
	mShortcutType = SHORTCUT_ITEM;
	if(pItem && mRes->setBaseData(pItem->getRes()->getItemID()) &&
		mRes->setExtData(pItem->getRes()->getExtData()))
	{
		mRes->setUID(GeneralItemUID());
	}
	else
	{
		delete mRes;
		mRes = NULL;
	}
}

ItemShortcut* ItemShortcut::CreateItem(S32 itemID, S32 quantity, bool isIgnore /* = false */)
{
	ItemShortcut* pItem = new ItemShortcut(itemID, quantity, isIgnore);
	if(pItem->IsValid())
		return pItem;
	else
	{
		delete pItem;
		AssertFatal(0, avar("物品没有数据! ID:%d", itemID));
		return NULL;
	}
}

ItemShortcut* ItemShortcut::CreateItem(ItemShortcut* pSrcItem)
{
	ItemShortcut* pItem = new ItemShortcut(pSrcItem);
	if(pItem->IsValid())
		return pItem;
	else
	{
		delete pItem;
		AssertFatal(0, avar("物品没有数据! ID:%d", 0));
		return NULL;
	}
}

ItemShortcut* ItemShortcut::CreateEmptyItem()
{
	return new ItemShortcut();
}



// ----------------------------------------------------------------------------
// 创造物品对象3
ItemShortcut::ItemShortcut(stItemInfo& itemInfo)
{
	mShortcutType = SHORTCUT_ITEM;
	mRes = new Res;
	if(mRes->setBaseData(itemInfo.ItemID) && mRes->setExtData(&itemInfo))
	{
		mRes->setUID(GeneralItemUID());
	}
	else
	{
		delete mRes;
		mRes = NULL;
	}
}

ItemShortcut* ItemShortcut::CreateItem(stItemInfo& itemInfo)
{
	ItemShortcut* pItem = new ItemShortcut(itemInfo);
	if(pItem->IsValid())
		return pItem;
	else
	{
		delete pItem;
		AssertFatal(0, avar("物品没有数据! ID:%d", itemInfo.ItemID));
		return NULL;
	}
}

ItemShortcut::~ItemShortcut()
{
	if(mRes)
		delete mRes;
	mRes = NULL;
}

#define POWER_ENGINE_USER

void ItemShortcut::packUpdate(BitStream* stream)
{
	Parent::packUpdate(stream);
#ifdef NTJ_SERVER
	if(stream->writeFlag(IsValid()))
	{
		stItemInfo* extData = mRes->getExtData();
		extData->WriteData<BitStream>(stream);
		//WriteData(mRes->getExtData(), stream);
	}
	else
	{
		AssertFatal(0, "物品数据有错!");
	}
#endif
}

void ItemShortcut::unpackUpdate(BitStream* stream)
{
	Parent::unpackUpdate(stream);
#ifdef NTJ_CLIENT
	if(stream->readFlag())
	{
		stItemInfo info;
		info.ReadData<BitStream>(stream);
		//ReadData(&info, stream);
		if(mRes) delete mRes;
		mRes = new Res;
		if(!mRes->setBaseData(info.ItemID) || !mRes->setExtData(&info))
		{
			delete mRes;
			AssertFatal(0, avar("物品数据创建不成功！id = %d", info.ItemID));
			mRes = NULL;
		}
	}
	else
	{
		if(mRes)
			delete mRes;
		mRes = NULL;
	}	
#endif
}

StringTableEntry ItemShortcut::getIconName()
{
	return mRes->getIconName();
}

StringTableEntry ItemShortcut::getCursorName()
{
	char szCursorName[128];
	const char* szIconName = getIconName();
	dSprintf(szCursorName, sizeof(szCursorName), "%s%s", "item/", szIconName);
	char* szPos = dStrrchr(szCursorName, '.');
	if(szPos) szPos[0] = 0;
	return StringTable->insert(szCursorName);
}

void ItemShortcut::RandStrengthenExtData()
{
	//检查是否可以升级
	if (!getRes()->canBaseLimit(Res::ITEM_UPGRADE) )
		return;
	//检查物品类型：武器、防具、饰品、法宝
	if ( !getRes()->isEquip())
		return;
	//注:检查最初数据源来自物品模板数据时有否有填写强化附加效果等级和强化附加效果，
	//有填强化附加效果等级而没有填写强化附加效果，则进行随机取值强化附加效果。
	int i = 0;
	int j = 0;
	stItemInfo *pItemInfo = getRes()->getExtData();
	if (!pItemInfo)
		return;
	for (i = 0; i < MAX_EQUIPSTENGTHENS; i++)
	{
		bool bDoRandom = true;

		if (pItemInfo->EquipStrengthenValue[i][0] == 0 || pItemInfo->EquipStrengthenValue[i][1] > 0)
			continue;
		// 过滤重复项
		for (j = 0; j < i; j++)
		{
			if (pItemInfo->EquipStrengthenValue[i][0] == pItemInfo->EquipStrengthenValue[j][0])
			{
				pItemInfo->EquipStrengthenValue[i][0] = 0;	//此项重复，置0
				pItemInfo->EquipStrengthenValue[i][1] = 0;
				bDoRandom = false;
				break;
			}
		}
		if (!bDoRandom)
			continue;

		Vector<U32> proList;
		g_RandPropertyTable->getRandProperty(Res::PROTYPE_STRENGTHEN, getRes()->getSubCategory(), 1, proList);
		if (proList.empty())
			continue;

		S32 nPropId = proList[0];
		S32 nSerialId = getRes()->getLimitLevel() + pItemInfo->EquipStrengthenValue[i][0] + 
						gIdentifyManager->getColorValue(getRes()->getColorLevel());
		pItemInfo->EquipStrengthenValue[i][1] = (3 * 10000 + nPropId) * 10000 + nSerialId;
		//pItemInfo->EquipStrengthenValue[i][1] = 303360187;		//test		304520014
	}

	//从小到大排序
	for(i = 0; i < MAX_EQUIPSTENGTHENS; i++)
	{
		for (j = i+1; j < MAX_EQUIPSTENGTHENS; j++)
		{
			if (pItemInfo->EquipStrengthenValue[i][0] > pItemInfo->EquipStrengthenValue[j][0])
			{
				U32 temp = pItemInfo->EquipStrengthenValue[i][0];
				pItemInfo->EquipStrengthenValue[i][0] = pItemInfo->EquipStrengthenValue[j][0];
				pItemInfo->EquipStrengthenValue[j][0] = temp;
				temp = pItemInfo->EquipStrengthenValue[i][1];
				pItemInfo->EquipStrengthenValue[i][1] = pItemInfo->EquipStrengthenValue[j][1];
				pItemInfo->EquipStrengthenValue[j][1] = temp;
			}
		}
	}

	// 将空项置后
	for(i = 0,j = 0; i < MAX_EQUIPSTENGTHENS; i++)
	{
		if (pItemInfo->EquipStrengthenValue[i][0] != 0)
		{
			pItemInfo->EquipStrengthenValue[j][0] = pItemInfo->EquipStrengthenValue[i][0];
			pItemInfo->EquipStrengthenValue[j][1] = pItemInfo->EquipStrengthenValue[i][1];
			if(i != j)
			{
				pItemInfo->EquipStrengthenValue[i][0] = 0;
				pItemInfo->EquipStrengthenValue[i][1] = 0;
			}			
			j++;
		}
	}

	// 设置随机强化等级附加属性的标志
	getRes()->setActiveFlag(Res::ACTIVEF_RANDPRO);
}