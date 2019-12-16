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
		PROTECT_TIME_TICK		= 60000,		//����ʱ��
		DROP_PROBABILITY_RANGE	= 10000-1,		//���伸�ʷ�Χ 1/10000
	};

public:
	bool	AddPlayer(Player *player, U32 nItemPackageId, S32 nMaxNum = 0);			//����й���Ȩ�����ID
	void	CreateFromData(U32 nItemPackageID, NpcObject *pNpc);
	void	Clear();
	bool	IsEmpty(U32 nPlayerID);				//�Ƿ����
	bool	IsHavingNothing();
	void	DeletePlayer(U32 nPlayerID);

	//��ѯ
	bool	IsViewItems	(Player* pPlayer);		//�Ƿ��ĳ�����ʾ��Ʒ����
	//bool	HasMissionItems(Player* pPlayer);	//�Ƿ���������Ʒ
	S32		GetMissionItemCount(Player* player);//��ѯָ����ҵ������������
	S32		GetItemCount(Player* player);

	//ʰȡ����
#ifdef NTJ_SERVER
	bool	OpenPickup	(Player* pPlayer);		//��ʼʰȡ
	bool	ClosePickup	(Player* pPlayer);		//����ʰȡ
	bool	PickupItemAll(Player* pPlayer, S32 &count, U32 &mask);
	bool	PickupItem	(Player* pPlayer, U16 nIndex, enWarnMessage &msg);		//ʰȡ��Ʒ
	bool	ClearItem	(Player* pPlayer, U16 nIndex);		//ɾ��һ����Ʒ
#endif

	//�����Ʒ,�ڸú����м������
	bool	AddItem		(ItemShortcut* pItem);				//����ʬ��ĵ�����Ʒ��ӵ��б�
	void	SetProtected(U32 dwProtectTime = PROTECT_TIME_TICK);

protected:
	bool	IsInProctTime	() { return (Sim::getCurrentTime() < m_dwProtTick);} //�Ƿ��ڱ���ʱ����
	void	UnProtected  () { m_dwProtTick = 0; }	//�������ʱ��
	bool	AddItemToMap(Player *player, U32 nItemId);
	bool	CanPickupItem(Player *player, ItemBaseData *pBaseData);

protected:
	U32		m_dwProtTick;				//����ʱ��
	DropItemsMap	m_MissionItemsMap;	//������Ʒ�б�

	std::list<U32>	m_MasterList;		//����Ȩ�б�,����ʱ����ֻ���б��е������Ȩ��ȡ, ע����PlayerId

public:
	DropItemsMap	m_ItemsMap;			//��Ʒ�б�
};