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
	static const SimTime mReadyTime = 5000;    // ����ʱ��  
public:
	ItemSplit();

	enum ItemSplitStatus
	{
		ITEMSPLIT_START = 0,        // ��ʼ�ֽ�
		ITEMSPLIT_CANCEL,           // ȡ���ֽ�
		ITEMSPLIT_END,              // �����ֽ�
		ITEMSPLIT_PICKUPITEM,       // ��ȡָ����ֽ���
		ITEMSPLIT_PICKUPAllITEM,    // ��ȡ���зֽ���
		ITEMSPLIT_OPENWND,          // �򿪷ֽ����
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