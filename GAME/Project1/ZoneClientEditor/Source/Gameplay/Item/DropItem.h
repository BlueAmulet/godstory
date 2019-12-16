//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "platform/platform.h"
#include "console/sim.h"
#include "core/tVector.h"
#include <list>
#include <hash_map>

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

class NpcObject;
class Player;
class CGameObject;
class PickupList;
class ItemBaseData;
class ItemShortcut;

class DropItemList
{
	friend class DropItemEvent;
	friend class NpcObject;	

public:
	typedef stdext::hash_map< U32, PickupList* > DropItemsMap;

	DropItemList();
	~DropItemList();
	enum
	{
		PROTECT_TIME_TICK		= 60000,		//保护时间
		DROP_PROBABILITY_RANGE	= 10000-1,		//掉落几率范围 1/10000
	};

public:
	bool	AddPlayer(Player *player, U32 nItemPackageId, S32 nMaxNum = 0);			//添加有归属权的玩家ID
	void	CreateFromData(U32 nItemPackageID, NpcObject *pNpc);
	void	Clear();
	bool	IsEmpty(U32 nPlayerID);				//是否被清空
	bool	IsHavingNothing();
	void	DeletePlayer(U32 nPlayerID);

	//查询
	bool	IsViewItems	(Player* pPlayer);		//是否对某玩家显示物品闪光
	//bool	HasMissionItems(Player* pPlayer);	//是否有任务物品
	S32		GetMissionItemCount(Player* player);//查询指定玩家的任务道具数量
	S32		GetItemCount(Player* player);

	//拾取操作
#ifdef NTJ_SERVER
	bool	OpenPickup	(Player* pPlayer);		//开始拾取
	bool	ClosePickup	(Player* pPlayer);		//结束拾取
	bool	PickupItemAll(Player* pPlayer, S32 &count, U32 &mask);
	bool	PickupItem	(Player* pPlayer, U16 nIndex, enWarnMessage &msg);		//拾取物品
	bool	ClearItem	(Player* pPlayer, U16 nIndex);		//删除一件物品
#endif

	//添加物品,在该函数中计算概率
	bool	AddItem		(ItemShortcut* pItem);				//人物尸体的掉落物品添加到列表
	void	SetProtected(U32 dwProtectTime = PROTECT_TIME_TICK);

protected:
	bool	IsInProctTime	() { return (Sim::getCurrentTime() < m_dwProtTick);} //是否在保护时间内
	void	UnProtected  () { m_dwProtTick = 0; }	//解除保护时间
	bool	AddItemToMap(Player *player, U32 nItemId);
	bool	CanPickupItem(Player *player, ItemBaseData *pBaseData);

protected:
	U32		m_dwProtTick;				//保护时间
	DropItemsMap	m_MissionItemsMap;	//任务物品列表

	std::list<U32>	m_MasterList;		//归属权列表,保护时间内只有列表中的玩家有权捡取, 注意是PlayerId

public:
	DropItemsMap	m_ItemsMap;			//物品列表
};