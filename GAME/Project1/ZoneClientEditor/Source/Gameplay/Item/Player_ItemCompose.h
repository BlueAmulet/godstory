//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _PLAYER_ITEMCOMPOSE_H_
#define _PLAYER_ITEMCOMPOSE_H_

class Player;
class ItemCompose : public BaseItemList
{
public:
	struct stRecord
	{
		U32 iItemId;        // 所需材料ID
		U16 iQuantity;      // 所需数量
	};

private:
	bool        mLock;                   // 合成开始锁
	U32         mItemIdTag;              // 合成方式标记,普通合成或特殊合成.(普通为0,特殊为ID)
	stRecord    mComposeInfo[8];

	static const U8      MAXSLOTS = 9;         // 槽位个数
	static const SimTime mReadyTime = 5000;    // 吟唱时间 

public:
	ItemCompose();

	enum Constants
	{
		RECORD_MAXINFO = 8,             // 所需材料最大数
	};

	enum itemComposeStatus
	{
		ITEMCOMPOSE_START = 0,          // 开始合成
		ITEMCOMPOSE_CANCEL,             // 取消合成
		ITEMCOMPOSE_END,                // 完成合成
		ITEMCOMPOSE_PICKUP,             // 取回材料或合成物
		ITEMCOMPOSE_OPENWND,            // 打开合成界面
	};

	inline   bool       isLock        ()           { return mLock;}
	inline   void       setLock       (bool value) { mLock = value;}
	inline   SimTime    getReadyTime  ()           { return mReadyTime;}
	inline   U32        getItemIdTag  ()           { return mItemIdTag;}   
	inline   stRecord*  getComposeInfo()           { return mComposeInfo;}

	enWarnMessage    canItemCompose          (Player* pPlayer);
	bool             startItemCompose        (Player* pPlayer);
	void             finishItemCompose       (Player* pPlayer);
	U8               setComposeInfo          (const char* string);
	void             clearComposeInfo        ();
	bool             isEmptyMaterial         ();

#ifdef NTJ_SERVER
	bool             LoadData                (stPlayerStruct* playerInfo);
	bool             SaveData                (stPlayerStruct* playerInfo);
	bool             sendInitialData         (Player* pPlayer);
	bool             UpdateToClient          (GameConnection* conn, S32 index);
	void             cancelItemCompose       (Player* pPlayer);
	enWarnMessage    pickupItemByIndex       (Player* pPlayer,S32 iIndex);
	enWarnMessage    InventoryToItemCompose  (stExChangeParam* param);
	enWarnMessage    ItemComposeToInventory  (stExChangeParam* param);
#endif

#ifdef NTJ_CLIENT    
	bool              sendToComposeStart      (Player* pPlayer);
	bool              sendToComposeCancel     (Player* pPlayer); 
	bool              sendToPickupItemByIndex (Player* pPlayer,U8 iIndex);
#endif
};
#endif