#pragma once
#include "core/refBase.h"
#include "Gameplay/Item/Player_item.h"

// ----------------------------------------------------------------------------
// 装备镶嵌操作类
// ----------------------------------------------------------------------------
class EquipMountGem
{
public:
	EquipMountGem();
	~EquipMountGem();

	//能否从物品栏拖动到某个槽位
	static enWarnMessage checkMoveFromInventory(Player* pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex);
	static enWarnMessage checkMoveFromEquipment(Player* pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex);

	static enWarnMessage canEquipGemMount(Player* pPlayer, ItemShortcut* pEquipItem, ItemShortcut* pGemItem, S32 nColNum);
	static enWarnMessage canEquipGemUnmount(Player* pPlayer, ItemShortcut* pEquipItem, ItemShortcut* pGemItem, S32 nColNum);

	static bool IsColorMatch(Player* pPlayer, ItemShortcut *pGemItem, S32 nColIndex);

#ifdef NTJ_SERVER
	static enWarnMessage clearEquipGemMount(Player* pPlayer);
	static enWarnMessage setEquipMountGem(Player* pPlayer, ItemShortcut* pEquipItem, ItemShortcut* pGemItem, S32 nColNum);
	static enWarnMessage unsetEquipMountGem(Player* pPlayer, ItemShortcut* pEquipItem, ItemShortcut* pGemItem, S32 nColNum);
#endif
};

// ----------------------------------------------------------------------------
// 可镶嵌装备列表索引节点
// ----------------------------------------------------------------------------
class EquipMountAble_List
{
public:
	enum SELECT_TYPE
	{
		TYPE_WEAPON			= 1,		//武器类
		TYPE_EQUIPMENT		= 2,		//防具类
	};

	static const int MAX_SLOTS = 6;

public:
	EquipMountAble_List();
	~EquipMountAble_List();

	static EquipMountAble_List *getInstance();

	void setCurrentPage(S32 nCurrPage)	{ mCurrPage = nCurrPage; }
	void setSelectType(SELECT_TYPE type)	{ mSelectType = type; }
	void updateEquipableList(Player *pPlayer);
	void updateSlots(Player *pPlayer);
	ItemShortcut *getSlot(S32 nIndex);
	S32  getItemIndex(S32 nSlotIndex);
	S32  getTotalPage();

private:
	bool IsEquipMountAble(ItemShortcut *pItem);

public:
	Vector<S32>	mWeaponsList;
	Vector<S32>	mEquipmentsList;
	SELECT_TYPE					mSelectType;
	S32							mCurrPage;
	ItemShortcut**				mSlots;
	S32							mSelectedSlotIndex;
};
