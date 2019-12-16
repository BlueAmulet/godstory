#ifndef __NPCSHOPDATA_H__
#define __NPCSHOPDATA_H__

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#pragma once
#include <hash_map>

class NpcShopList;
class ItemShortcut;
// ��Ʒ�б�ṹ
struct NpcShopItem
{
public:
	U32 GoodID;				// ��Ʒ���
	U32 ShopID;				// ��Ʒ�б�ID
	U32 ItemID;				// ��Ʒ����ģ��ID
	U32 RandGroup;			// �����Ʒ���
	U32 ShowByLockTime;		// ��涳��ʱ���������(0-�κ�ʱ���ڶ���ʾ 1-����涳��ʱ�����ʱ��ʾ)
	U32 CurrencyID;			// ��������(�μ�Res.h��SaleTypeö��)
	U32 CurrencyValue;		// ��������
	U32 SellRate;			// ����
	U32 ConditionID;		// ����ID
	StringTableEntry ConditionValue;	// ����ֵ
	S32 StockNum;			// �������0Ϊ����)
	U32 StockInterval;		// ���ˢ�¼��ʱ�䣨�Է���Ϊ��λ��
	U32 LockTime;			// ��涳��ʱ��(�Է���Ϊ��λ)
	U32 UpStockTime;		// �ϼ�ʱ��
	U32 DownStockTime;		// �¼�ʱ��
	ItemShortcut* pItem;	// ��Ʒʵ������

	// ------------------------------------------------------------
	// ��Ʒ�б����
	bool IsRandHit;			// �Ƿ��������(ֻ�������Ʒ��Ų�Ϊ0ʱ��ֵ����Ч)
	U32  LastUpdateTime;	// �ϴ�ˢ��ʱ��(ֻ�ڿ��ˢ�¼��ʱ�䲻Ϊ0ʱ��ֵ����Ч)
	U32  CurrentStockNum;	// ��ǰ�����(ֻ�ڿ������Ϊ0ʱ��Ч���ݼ���0Ϊֹ)

	NpcShopItem() { memset(this, 0, sizeof(NpcShopItem));}
	// �жϵ�ǰ��Ʒ�Ƿ��ڴ�����Ʒ�б�ʱ�������������
	bool IsPutInList() { return (RandGroup==0 || (RandGroup !=0 && IsRandHit));}
	// �Ƿ���ݼ�������
	bool IsEmpty() { return (StockNum != 0 && CurrentStockNum == 0);}	
	bool IsInTime(U32 hourmin = 0);
	// �Ƿ���ʵ����
	bool IsExist(U32 hourmin = 0) { return IsPutInList() && !IsEmpty() && IsInTime(hourmin);}
	// �Ƿ���Ҫ���ˢ��
	bool IsRefresh(U32 curtime = 0) { return StockInterval? LastUpdateTime + StockInterval <= curtime : false;}
	// �Ƿ�����ʾ����ҿ� 
	bool IsShow(U32 curtime = 0) { return ShowByLockTime? LastUpdateTime + LockTime <= curtime : true;}
};

class Player;
class ShopListRepository
{
public:
	static const U32 UPDATEINTERVAL = 30;	//ÿ��30�������һ��
	typedef Vector<NpcShopItem*> NpcShopData;
	typedef stdext::hash_map<U32, NpcShopData*> NpcShopMap;
	ShopListRepository();
	~ShopListRepository();
	void read();
	void clear();
	NpcShopData* getData(U32 shopid);
	bool insert(U32, NpcShopData* data);
	bool reset(U32 shopid, U32 randgroupid);	

#ifdef NTJ_SERVER
	bool update(U32 curTime);
	U32 getShopData(U32 shopid, NpcShopList* shoplist, S32 page);
	//��������ɸѡ
	U32 getShopData(Player* player, U32 shopid, NpcShopList* shoplist, S32 page, S32 id);
#endif//NTJ_SERVER

private:
	NpcShopMap mDataMap;
	U32 mLastUpdateTime;
};

extern ShopListRepository* g_ShopListRepository;

#endif//__NPCSHOPDATA_H__