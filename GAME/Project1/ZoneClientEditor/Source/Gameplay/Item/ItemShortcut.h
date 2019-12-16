//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef __ITEMSHORTCUT_H__
#define __ITEMSHORTCUT_H__
#pragma once
#include "Gameplay/GameObjects/ShortcutObject.h"

class Res;
struct stItemInfo;

class ItemShortcut : public ShortcutObject
{
	typedef ShortcutObject Parent;
private:
	ItemShortcut();
	ItemShortcut(S32 itemID, S32 quantity, bool isIgnore = false);
	ItemShortcut(ItemShortcut* pItem);
	ItemShortcut(stItemInfo& itemInfo);
public:
	static ItemShortcut* CreateEmptyItem();
	static ItemShortcut* CreateItem(S32 itemID, S32 quantity, bool isIgnore = false);
	static ItemShortcut* CreateItem(ItemShortcut* pItem);
	static ItemShortcut* CreateItem(stItemInfo& itemInfo);
	
	~ItemShortcut();
	StringTableEntry getIconName();
	StringTableEntry getCursorName();
	Res* const getRes() { return mRes; }
	bool IsValid()  { return mRes != NULL;}

	virtual void packUpdate(BitStream* stream);
	virtual void unpackUpdate(BitStream* stream);

	void	RandStrengthenExtData();

private:
	Res* mRes;			// 物品资源数据对象
};

#endif//__ITEMSHORTCUT_H__