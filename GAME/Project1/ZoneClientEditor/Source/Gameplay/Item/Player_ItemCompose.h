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
		U32 iItemId;        // �������ID
		U16 iQuantity;      // ��������
	};

private:
	bool        mLock;                   // �ϳɿ�ʼ��
	U32         mItemIdTag;              // �ϳɷ�ʽ���,��ͨ�ϳɻ�����ϳ�.(��ͨΪ0,����ΪID)
	stRecord    mComposeInfo[8];

	static const U8      MAXSLOTS = 9;         // ��λ����
	static const SimTime mReadyTime = 5000;    // ����ʱ�� 

public:
	ItemCompose();

	enum Constants
	{
		RECORD_MAXINFO = 8,             // ������������
	};

	enum itemComposeStatus
	{
		ITEMCOMPOSE_START = 0,          // ��ʼ�ϳ�
		ITEMCOMPOSE_CANCEL,             // ȡ���ϳ�
		ITEMCOMPOSE_END,                // ��ɺϳ�
		ITEMCOMPOSE_PICKUP,             // ȡ�ز��ϻ�ϳ���
		ITEMCOMPOSE_OPENWND,            // �򿪺ϳɽ���
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