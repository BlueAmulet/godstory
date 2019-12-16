#ifndef __PLAYER_EQUIPIDENTIFY_H__
#define __PLAYER_EQUIPIDENTIFY_H__
#include "Gameplay/item/Player_Item.h"

// ----------------------------------------------------------------------------
// 装备鉴定功能类
class EquipIdentify
{
public:
	EquipIdentify();
	~EquipIdentify();
	//能否鉴定
	enWarnMessage canIdentify(Player* pPlayer, ShortcutObject* pItem1, ShortcutObject* pItem2 = NULL);
	//换算鉴定价格
	U32 getIdentifyPric(ShortcutObject* pItem);
	void openIdentify(Player* player);
	void closeIdentify(Player* player);

	// 鉴定后附加属性方法
	void setStats(Player* player, ShortcutObject* pItem1, ShortcutObject* pItem2 = NULL);
	void setStatsID(Player* player, ShortcutObject* pItem1,  U32 upVal, U32 lowerVal = 0);

#ifdef NTJ_SERVER

	//鉴定结果
	enWarnMessage setIdentifyResult(Player* pPlayer, ShortcutObject* pItem1, ShortcutObject* pItem2 = NULL);
	enWarnMessage setEquipIdntifyResult(Player* pPlayer, ShortcutObject* pItem);
#endif

	// ================================================================
	// Help Method
	// ================================================================
	S32 getColorValue(U32 colorLevel);
	S32 getItemLayer(S32 layervalue);
	S32 getQualityValue(S32 layervalue);
	S32 buildQualityLevel(U32 levelLimit, U32 colorLevel, S32 upVal = 0, S32 lowVal = 0);
	U32	getStatsCount(U32 colorLevel);
};
extern EquipIdentify* gIdentifyManager;


#endif//__PLAYER_EQUIPIDENTIFY_H__