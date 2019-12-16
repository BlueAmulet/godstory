//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _PLAYER_ITEMSPLIT_H_
#define _PLAYER_ITEMSPLIT_H_
class Player;
class ItemSplit : public BaseItemList
{
private:
	bool   mLock;
	static const U8 MAXSLOTS = ITEMSPLIT_MAXSLOTS+1;
	static const SimTime mReadyTime = 5000;    // 吟唱时间  
public:
	ItemSplit();

	enum ItemSplitStatus
	{
		ITEMSPLIT_START = 0,        // 开始分解
		ITEMSPLIT_CANCEL,           // 取消分解
		ITEMSPLIT_END,              // 结束分解
		ITEMSPLIT_PICKUPITEM,       // 获取指定项分解物
		ITEMSPLIT_PICKUPAllITEM,    // 获取所有分解物
		ITEMSPLIT_OPENWND,          // 打开分解界面
	};

	inline SimTime   getReadyTime     (){ return mReadyTime;}  
	inline bool      isLock           (){ return mLock;}
	inline void      setLock          (bool value){ mLock = value;}
	enWarnMessage    canItemSplit     (Player* pPlayer);
	bool             startItemSplit   (Player* pPlayer);
	void             finishItemSplit  (Player* pPlayer);

#ifdef NTJ_SERVER
	bool             LoadData            (stPlayerStruct* pPlayerData);
	bool             UpdateToClient      (GameConnection* conn, S32 index);
	bool             SaveData            (stPlayerStruct* pPlayerData);
	bool             sendInitialData     (Player* pPlayer);
	void             cancelItemSplit     (Player* pPlayer);
	enWarnMessage    pickupAllItem       (Player* pPlayer); 
	enWarnMessage    pickupItemByIndex   (Player* pPlayer,U8 index);
	enWarnMessage    InventoryToItemSplit(stExChangeParam* param);
	enWarnMessage    ItemSplitToInventory(stExChangeParam* param);
#endif

#ifdef NTJ_CLIENT
	bool             sendToStartEvent(Player* pPlayer);
	bool             sendToCancelEvent(Player* pPlayer);
	bool             sendToPickupAllItem(Player* pPlayer);
	bool             sendToPickupItemByIndex(Player* pPlayer,U8 index);
#endif
	
};

#endif