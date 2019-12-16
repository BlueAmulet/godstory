#ifndef __PLAYER_ITEM_H__
#define __PLAYER_ITEM_H__
#pragma once
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/SkillShortcut.h"
#include "Gameplay/GameObjects/LivingSkillShortcut.h"
#include "Gameplay/item/ItemShortcut.h"
#include "Gameplay/Item/PetShortcut.h"
#include "UI/guiRichTextCtrl.h"

struct stPetInfo;
struct stMountInfo;

class Player;
struct NpcShopItem;

// 玩家间交易状态
enum enTradeState
{
	TRADE_NONE		= 0,					// 无交易状态
	TRADE_ASKFOR,							// 交易请求
	TRADE_ANSWER,							// 交易应答请求
	TRADE_REJECT,							// 拒绝交易请求
	TRADE_BEGIN,							// 交易开始
	TRADE_LOCK,								// 交易加锁
	TRADE_UNLOCK,							// 交易解锁
	TRADE_END,								// 交易结束
	TRADE_CANCEL,							// 交易取消

	TRADE_UPITEM,							// 交易物品上架
	TRADE_DOWNITEM,							// 交易物品下架

	TRADE_LOCKNOTIFY,						// 交易物品锁定通知对方
	TRADE_UNLOCKNOTIFY,						// 交易物品解锁通知对方
	TRADE_ENDNOTIFY,						// 确定交易通知对方

	TRADE_SETMONEY,							// 设置出售价格
	
	STALL_RENAME,							// 改名
	STALL_REINFO,							// 修改内容简介

	STALL_REMOVE_ALL,						// 所有都下架
	STALL_SEE_CLOSE,						// 关闭察看
	STALL_CHANGE,							// 加包
	STALL_ADDTOLOOKUP,						// 添加到购买列表
	STALL_BUYLOOKUP,						// 购买查找物品
	STALL_SCANSTALLNOTE,					// 打开摆摊信息
	STALL_CLEAR_MSG,						// 清楚交易和留言信息
	//以下只在客户端用
	STALL_TRADERECORD,						// 交易信息
	STALL_INFOMSG,							// 摊位信息
	STALL_CHATMSG,							// 聊天信息

};

//摆摊类型
enum enStallType
{
	stall_individual = 0,					// 个人摆摊
	stall_fixed,							// 可离线的固定摆摊
	stall_faction,							// 帮会摆摊
};

enum enNpcShopType
{
	NPCSHOP_OPEN = 0,						// 打开NPC商店
	NPCSHOP_CLOSE,							// 关闭NPC商店
	NPCSHOP_BUY,							// 从商店买物品
	NPCSHOP_SELL,							// 卖物品给商店
	NPCSHOP_RECYCLE,						// 回购物品
	NPCSHOP_PAGE,							// 商品浏览翻页
	NPCSHOP_REPAIRSIGLE,					// 修理单件装备
	NPCSHOP_REPAIRALL,						// 修理所有装备
	NPCSHOP_REPAIRFUCTION,					// 帮会修理
	NPCSHOP_FILTER,							// 筛选
	DURABLE_REPAIR,							// 经久修理
	RECOVER_REPAIR,							// 恢复最大耐久
	REPAIR_CLOSE,							// 关闭修理
	
};

enum enMarketActionType
{
	MARKET_OPEN = 0,						// 打开商城
	MARKET_BUY,								// 商城购买
	MARKET_COMPIMENT,						// 赠送
	MARKET_ASKFOR,							// 索要
};

// 栏位类型
enum enShortcutType
{
	SHORTCUTTYPE_NONE,						// 未知栏位类型
	SHORTCUTTYPE_PANEL				= 1,	// 快捷栏
	SHORTCUTTYPE_INVENTORY			= 2,	// 物品栏
	SHORTCUTTYPE_EQUIP				= 3,	// 装备栏
	SHORTCUTTYPE_BANK				= 4,	// 银行仓库栏
	SHORTCUTTYPE_DUMP				= 5,	// 商店回购栏
	SHORTCUTTYPE_TRADE				= 6,	// 玩家交易栏		
	SHORTCUTTYPE_STALL				= 7,	// 玩家摆摊栏
	SHORTCUTTYPE_NPCSHOP			= 8,	// NPC系统商店
	SHORTCUTTYPE_SKILL_COMMON		= 9,	// 普通技能栏
	SHORTCUTTYPE_PICKUP				= 10,	// 拾取物品栏
	SHORTCUTTYPE_AUTOSELL			= 11,	// 自动售卖
	SHORTCUTTYPE_IDENTIFY			= 12,	// 装备鉴定
	SHORTCUTTYPE_COMPOSE			= 13,	// 装备合成
	SHORTCUTTYPE_GEM_MOUNT			= 14,	// 装备镶嵌
	SHORTCUTTYPE_STRENGTHEN			= 15,	// 装备强化
	SHORTCUTTYPE_MISSIONITEM		= 16,	// 可选任务奖励栏
	SHORTCUTTYPE_MISSION_TRACE		= 17,	// 任务追踪
	SHORTCUTTYPE_EQUIP_PUNCHHOLE	= 18,	// 装备打孔
	SHORTCUTTYPE_EQUIP_EMBEDABLE	= 19,	// 装备可镶嵌列表
	SHORTCUTTYPE_LIVINGSKILL        = 20,   // 生活技能栏
	SHORTCUTTYPE_LIVINGCATEGORY     = 21,   // 生活技能种类
	SHORTCUTTYPE_PRESCRIPTION       = 22,   // 配方材料列表
	SHORTCUTTYPE_FIXEDMISSIONITEM	= 23,	// 固定任务奖励栏
	SHORTCUTTYPE_SKILL_STUDY        = 24,   // 技能学习栏
	SHORTCUTTYPE_TARGET_TRADE		= 25,	// 对方玩家交易栏
	SHORTCUTTYPE_PET				= 26,	// 宠物栏
	SHORTCUTTYPE_TARGET_STALL		= 27,	// 对方玩家摆摊栏
	SHORTCUTTYPE_TEMP				= 28,	// 临时栏
	SHORTCUTTYPE_STALL_PET			= 29,	// 宠物出售
	SHORTCUTTYPE_STALL_BUYLIST		= 30,	// 收购清单
	SHORTCUTTYPE_MOUNT_PET_LIST		= 31,	// 骑乘宠物栏
	SHORTCUTTYPE_TARGET_STALL_PET   = 32,   // 摆摊对方宠物清单
	SHORTCUTTYPE_TARGET_STALLBUYLIST= 33,	// 摆摊对方购物清单
	SHORTCUTTYPE_STALL_LOOKUP		= 34,	// 摆摊查找
	SHORTCUTTYPE_PET_HELP_LIST		= 35,	// 宠物帮助栏
	SHORTCUTTYPE_SKILL_HINT         = 36,   // 技能提示栏
	SHORTCUTTYPE_TRADE_STALL_HELP	= 37,	// 宠物和摆摊助手栏
	SHORTCUTTYPE_MAIL_ITEM			= 38,   // 邮件物品栏
	SHORTCUTTYPE_REPAIR				= 39,	// 经久和恢复最大耐久（公用一个界面）
	SHORTCUTTYPE_SUPERMARKET		= 40,   // 商城栏
	SHORTCUTTYPE_ITEMSPLIT          = 41,   // 物品分解栏
	SHORTCUTTYPE_ITEMCOMPOSE        = 42,   // 物品合成
	SHORTCUTTYPE_RECOMMEND			= 43,	// 推荐栏
	SHORTCUTTYPE_SHOPBASKET			= 44,   // 购物栏
	SHORTCUTTYPE_SPIRITLIST			= 45,   // 元神栏
	SHORTCUTTYPE_SPIRITSKILLLIST	= 46,   // 元神绝招栏
	SHORTCUTTYPE_SPIRITSKILLLIST_2	= 47,   // 元神绝招栏(NPC对话框)
	SHORTCUTTYPE_TALENTSKILLLIST	= 48,   // 元神天赋技能栏
	SHORTCUTTYPE_MISSIONSUBMITLIST	= 50,	// 任务递交物品栏
};

// 商店附加功能
enum enShopFunction
{
	SHOPFUNCTION_NONE		= 0,
	SHOPFUNCTION_FIX		= 1 << 0,		// 普通单修
	SHOPFUNCTION_FIXALL		= 1 << 1,		// 普通全修
	SHOPFUNCTION_FFIXALL	= 1 << 2,		// 帮会全修
};

// 客户端提示物品的标志
enum enItemFlag
{
	ITEM_NOSHOW = 0,						// 不作任何显示
	ITEM_LOAD,								// 初始化物品栏
	ITEM_BUY,								// 购买
	ITEM_SELL,								// 出售
	ITEM_TRADE,								// 交易
	ITEM_PICK,								// 拾取
	ITEM_DROP,								// 丢弃
	ITEM_SENDMAIL,							// 邮寄物品
	ITEM_RECEIVEMAIL,						// 收取邮件物品
	ITEM_COMPOSE,							// 合成
	ITEM_FROMBANK,							// 从银行取回了一个物品
	ITEM_SAVEBANK,							// 你向银行存放了一个物品
	ITEM_LOSE,								// 你失去了物品
	ITEM_USED,								// 你使用了物品
	ITEM_GET,								// 获取
	ITEM_REPAIRSUCC,						// 修理物品成功
	ITEM_PUTTRADE,      					// 你向交易栏放入一个物品
	ITEM_RETAKETRADE,   					// 从交易栏取回一个物品
	ITEM_SPLIT,         					// 装备分解
	ITEM_IDENTIFY,							// 装备鉴定
	ITEM_STALLBUY,							// 从摆摊栏购买物品
	ITEM_REBUY,								// 从回购栏回购物品
	ITEM_REPAIRFAIL,						// 修理物品失败
};

#ifdef NTJ_SERVER
// 栏位交换数据参数结构
struct stExChangeParam
{
	Player*					player;			// 玩家对象
	enShortcutType			SrcType;		// 源栏位类型
	enShortcutType			DestType;		// 目标栏位类型
	S32						SrcIndex;		// 源槽位索引
	S32						DestIndex;		// 目标槽位索引
	S32						Price;			// 交换金额
	S32						SkillSeriesId;	// 技能系列ID
};
#endif

enum ITEMOP_TYPE
{
	OP_NONE			= 0,					// 未处理
	OP_FINDADD		= 0x01,					// 找到叠加
	OP_FINDEMPTY	= 0x02,					// 找到空槽
	OP_FINDDEL		= 0x04,					// 找到删除
	OP_FINDDELEMPTY = 0x08,					// 删除到空槽
};

// 批量查找可放置物品的槽位数据结构
struct stItemResult
{	
	ItemShortcut*	srcEntry;				// 源物品对象
	ItemShortcut*	newEntry;				// 操作结果物品对象
	U32				flag;					// 操作结果标志
	S32				num;					// 操作结果的物品数量
	U32				PlayerID;				// 玩家角色编号
};

struct StallItem
{
	U32 money;
	ItemShortcut* pItem;
};
//宠物摆摊数据
struct stStallPet
{
	PetShortcut* petSlot;
	U32 money;

};
//摆摊收购物品结构
struct stStallBuyItem
{
	ItemShortcut* buyItem;
	U32 money;
	U32 num;
};
//商城物品
struct stMarketItem
{
	U32 marketItemID;
	ItemShortcut* marketItem;
};

// 玩家记录结构
struct stRecord
{
	U32 serialID;					// 记录流水号
	U32 time;						// 记录发生时间
	U32 senderID;					// 发送者角色ID
	U32 receiverID;					// 接收者角色ID
	StringTableEntry senderName;	// 发送者角色名
	StringTableEntry receiverName;	// 接收者角色名

	stRecord() : serialID(0), time(0), senderID(0), receiverID(0), senderName(NULL), receiverName(NULL) { }
};

// 聊天记录结构
struct stChatRecord : stRecord
{
	char* chatContent;
	U32	  contentLen;

	stChatRecord() : chatContent(NULL), contentLen(0) { }
};

// 交易记录结构
struct stTradeRecord :stRecord
{
	enum
	{
		TRADE_ITEM,
		TRADE_PET,
	};
	U32 type;  // 1-物品 2-宠物
	S32	money;	// 金钱
	union TradeContent
	{
		stItemInfo* item;	// 物品
		stPetInfo*	pet;	// 宠物
	} trade;

	stTradeRecord() { dMemset(this, 0, sizeof(stTradeRecord)); }
};

// ----------------------------------------------------------------------------
// 装备强化功能类
class EquipStrengthen
{
public:
	EquipStrengthen();
	~EquipStrengthen();

	static bool IsFuLiaoA(ItemShortcut *pItem);
	static bool IsFuLiaoB(ItemShortcut *pItem);
	static bool IsFuLiaoC(ItemShortcut *pItem);

#ifdef NTJ_CLIENT
	//根据物品类型得到相应槽位（武器、防具、饰品、法宝返回0，成功符返回1，连升符返回2，不退级符返回3），只在客户端判断
	static bool getEquipStrengthenSlotIndex(ItemShortcut *pItem, S32 &nIndex);
#endif

	//能否从物品栏拖动到某个槽位
	static enWarnMessage checkMoveFromInventory(Player* pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex);
	//能否强化
	static enWarnMessage canStrengthen(Player* pPlayer, S32 &nCostMoney, U32 &nMaterialId, S32 &nMaterialNum);
	/////////////////////////////////////////
	static S32 getStrengthenPrice(ItemShortcut* pItem);
	static void getStrengthenMaterial(ItemShortcut* pItem, U32 &nMatirialId, S32 &nMatirialNum);
	static U32 getTotalSuccessRate(U32 nItemLevel, U32 nQualityLevel, U32 nEquipStrengthenLevel, U32 nYangSheng, ItemShortcut *pItemFuZhu_A);
	/////////////////////////////////////////

	static void openStrengthen(Player* player, NpcObject *pNpc);
	static void closeStrengthen(Player* player);

#ifdef NTJ_SERVER
	//强化结果
	static enWarnMessage setStrengthenResult(Player* pPlayer);
#endif
};

// ----------------------------------------------------------------------------
// 装备灵魂连接
class SoulLink
{
public:
	SoulLink(){}
	~SoulLink(){}
	static enWarnMessage canSoulLink(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2);
#ifdef NTJ_SERVER
	static enWarnMessage setSoulLink(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2);
#endif
};

// ----------------------------------------------------------------------------
// 装备铭刻
class EquipImpress
{
public:
	EquipImpress(){}
	~EquipImpress(){}
	static enWarnMessage canImpress(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2);
#ifdef NTJ_SERVER
	static enWarnMessage setImpress(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2);
#endif
};

// ----------------------------------------------------------------------------
// 装备五行连接
class WuXingLink
{
public:
	WuXingLink();
	~WuXingLink();
	static enWarnMessage canWuxingLink(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2);
#ifdef NTJ_SERVER
	static enWarnMessage setWuxingLink(Player* player, ShortcutObject* pShortCut1, ShortcutObject* pShortCut2);
#endif
};

// ----------------------------------------------------------------------------
// 装备打孔
class EquipPunchHole
{
public:
	EquipPunchHole();
	~EquipPunchHole();

	static S32				getOpenedHoles(Player *pPlayer, ItemShortcut* pEquipItem);
	static S32				getAllowedHoles(Player *pPlayer, ItemShortcut* pEquipItem);
	static S32				getLeftHoles(Player *pPlayer, ItemShortcut* pEquipItem);
	static S32				getPunchableHoleIndex(Player *pPlayer, ItemShortcut* pEquipItem);

	static enWarnMessage	checkMoveFromInventory(Player *pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex);
	static enWarnMessage	openPunchHole(Player* pPlayer, NpcObject *pNpc);
	static enWarnMessage	closePunchHole(Player* pPlayer);
	static enWarnMessage	canPunchHole(Player* pPlayer, ItemShortcut* pEquipItem, S32 &money, S32 &nHoleIndex);
	static S32				CalculatePunchMoney(Player* pPlayer, ItemShortcut* pEquipItem);

#ifdef NTJ_SERVER
	static enWarnMessage	setEquipPunchHoleResult(Player* pPlayer, ItemShortcut* pEquipItem);
#endif
};

// ----------------------------------------------------------------------------
// 物品操作管理类
class ItemManager
{
public:
	ItemManager();
	enum Constants
	{
		TRADE_LEVEL_LIMIT		= 7,		// 交易功能等级限制
		TRADE_MAXWAITTIME		= 60,		// 交易邀请等待最大时间 
		MAX_ADDITEMS			= 10,		// 能一次添加物品给玩家的最大数量
		EXTBAG_BEGINSLOT		= 40,		// 扩展槽位在物品栏的起始位置
		EXTBAG_BANK_BEGINSLOT	= 12,		// 扩展槽位在银行仓库栏的起始位置
		REPAIRCOST_ITEM			= 105090005,// 经久装备修理所需特殊物品(天工开物)
	};

	ItemShortcut* cloneItem(ShortcutObject* pShortObject);
	bool isSameItem(ShortcutObject* pItem1, ShortcutObject* pItem2, bool ignoreExtPro = false);
	enWarnMessage canUseItem(Player* player, ItemShortcut* pItem, GameObject* pObj);
	enWarnMessage canDragToEquipment(Player* player, ItemShortcut* pItem, S32 SlotIndex = -1);
	bool checkDragToEquipment(Player* player, ItemShortcut* pItem, S32 index);
	bool canDragItem(Player* player, ItemShortcut* pItem = NULL);
	void getSameObject(Player* player, S32 slottype, ShortcutObject* pSlot, S32 &index);
	void delSameItem(Player* player, S32 slottype, ItemShortcut* pItem);	

	ShortcutObject* getShortcutSlot(Player* player, U32 slottype, S32 index);
	ItemShortcut* getItemByUID(Player* player, U64 UID, S32& idx);

	bool openNpcShop(Player* player, U32 shopid, U32 shoptype = SHOPFUNCTION_NONE);
	void closeNpcShop(Player* player);
	NpcShopItem* getNpcShopSlot(Player* player, S32 index);
	// 修理
	bool openRepair(Player* pPlayer, U32 type);	
	void closeRepair(Player* pPlayer);

	bool closePlayerTrade(Player* player);
	S32 getItemEquipPos(Player* player, ItemShortcut* pItem, bool ignoreExist = false);
#ifdef NTJ_CLIENT
	bool insertText(Player* pPlayer, Res* pRes, char* buff,S32 mSlotType,S32 mSlotIndex);
	bool insertText(Player* pPlayer, stPetInfo* pPetInfo, char* buff,S32 mSlotType,S32 mSlotIndex);
	bool insertText(Player* pPlayer, stMountInfo* pMountInfo, char* buff,S32 mSlotType,S32 mSlotIndex);
	bool insertText(Player* pPlayer, stSpiritInfo* pSpiritInfo, char* buff, S32 mSlotType, S32 mSlotIndex);
	bool insertText(Player* pPlayer, TalentData* pTalentData, char* buff, S32 mSlotType, S32 mSlotIndex);
	bool insertText(Player* pPlayer, SkillData* data, char* buff, S32 mSlotType, S32 mSlotIndex);
	bool insertText(Player* pPlayer, LivingSkillData* pData,char* buff,S32 mSlotType,S32 mSlotIndex);
	bool insertText(BuffData* pBuffData, char* buff);
#endif
	

#ifdef NTJ_SERVER	
	enWarnMessage ExchangeShortcut(stExChangeParam* param);
	enWarnMessage PanelMoveToPanel(stExChangeParam* param);
	enWarnMessage InventoryMoveToPanel(stExChangeParam* param);
	enWarnMessage InventoryMoveToInventory(stExChangeParam* param);
	enWarnMessage InventoryMoveToEquip(stExChangeParam* param);
	enWarnMessage InventoryMoveToBank(stExChangeParam* param);
	enWarnMessage BankMoveToInventory(stExChangeParam* param);
	enWarnMessage BankMoveToBank(stExChangeParam* parm);
	enWarnMessage EquipMoveToInventory(stExChangeParam* param);
	enWarnMessage InventoryMoveToTrade(stExChangeParam* param);
	enWarnMessage TradeMoveToInventory(stExChangeParam* param);
	enWarnMessage SkillCommonMoveToPanel(stExChangeParam* param);
	enWarnMessage SkillHintMoveToPanel(stExChangeParam* param);
	enWarnMessage InventoryMoveToIdentify(stExChangeParam* param);
	enWarnMessage IdentifyMoveToInventory(stExChangeParam* param);
	enWarnMessage InventoryMoveToStrengthen(stExChangeParam* param);
	enWarnMessage StrengthenMoveToInventory(stExChangeParam* param);
	enWarnMessage StrengthenMoveToStrengthen(stExChangeParam* param);
	enWarnMessage InventoryMoveToMountGem(stExChangeParam* param);
	enWarnMessage EquipMoveToMountGem(stExChangeParam* param);
	enWarnMessage MountGemMoveToInventory(stExChangeParam* param);
	enWarnMessage InventoryMoveToPunchHole(stExChangeParam* param);
	enWarnMessage LivingSkillMoveToPanel(stExChangeParam* param);
	enWarnMessage PunchHoleMoveToInventory(stExChangeParam* param);
	enWarnMessage InventoryMoveToStall(stExChangeParam* param);
	enWarnMessage StallMoveToInventory(stExChangeParam* param);
	enWarnMessage PetListMoveToPanel(stExChangeParam *param);
	enWarnMessage PetListMoveToPetList(stExChangeParam *param);
	enWarnMessage PetListMoveToTempList(stExChangeParam *param);
	enWarnMessage PetListMoveToStallPetList(stExChangeParam* param);
	enWarnMessage MountListMoveToPanel(stExChangeParam *param);
	enWarnMessage MountListMoveToMountList(stExChangeParam *param);
	enWarnMessage InventoryMoveToRepairList(stExChangeParam* param);
	enWarnMessage RepairListMoveToInventory(stExChangeParam* param);
	enWarnMessage InventoryMoveToMissionSubmit(stExChangeParam* param);
	enWarnMessage PetListMoveToMissionSubmit(stExChangeParam* param);
	enWarnMessage PetListMoveToTrade(stExChangeParam* param);

	enWarnMessage delFromRepairList(Player* pPlayer);

	void		  mountEquipAttribute(Player* player, ItemShortcut* pItem);
	void		  unmountEquipAttribute(Player* player, ItemShortcut* pItem);
	void		  mountEquipModel(Player* player, ItemShortcut* pItem);
	void		  unmountEquipModel(Player* player, ItemShortcut* pItem);
	void		  mountAllEquipAttribute(Player* player);
	void		  mountAllEquipModel(Player* player);

	bool setPutItemProperty(Player* player, U32 index, U32 type, S32 argc, ...);

	S32	getItemUsedTimes(Player* player, S32 index, U32 itemid);
	enWarnMessage setItemUsedTimes(Player* player, S32 index, U32 itemid, S32 num);

	enWarnMessage addItemToInventory(Player* player, S32 itemID, S32& index, S32 num = 1, U32 flag = ITEM_LOAD);
	enWarnMessage addItemToInventory(Player* player, ItemShortcut* pItem, S32& index, S32 num = 1, U32 flag = ITEM_LOAD);
	enWarnMessage delItemFromInventory(Player* player, S32 itemID, S32 num = 1, U32 flag = ITEM_LOSE);
	enWarnMessage delItemFromInventory(Player* player, ItemShortcut* pItem, S32 num = 1, U32 flag = ITEM_LOSE, bool ignoreExpro = false);
	enWarnMessage delItemFromInventoryByIndex(Player* player, S32 index, S32 num = 1);
	enWarnMessage addItemToTrade(Player* player, S32 slotIndex);
	enWarnMessage delItemFromTrade(Player* player, S32 slotIndex);
	enWarnMessage addItemToEquip(Player* player, U32 id);
	enWarnMessage addItemToEquip(Player* player, ItemShortcut* pItem);
	enWarnMessage delItemFromEquip(Player* player, U32 ItemID);
	enWarnMessage delItemFromEquip(Player* player, ItemShortcut* pItem);
	enWarnMessage addObjectToPanel(Player* player, U32 type, U32 id, S32 index);
	enWarnMessage addObjectToPanel(Player* player, ShortcutObject* pItem, S32 index);

	enWarnMessage addItemToLookUp(Player* player, stStallBuyItem* item);
	enWarnMessage delItemFromStallLookUp(Player* player, S32 index);
	enWarnMessage delPetFromStallPetList(Player* player, S32 index);

	enWarnMessage addItemToDumpList(Player* player, ItemShortcut* item, S32 num = 1);
	enWarnMessage addItemToStallList(Player* player, S32 index);
	enWarnMessage delItemFromStallList(Player* player, S32 index);

	enWarnMessage useItem(Player* player, S32 type, S32 col, GameObject* pTarget);
	enWarnMessage buyItem(Player* player,S32 index, S32 goodid, S32 num);
	enWarnMessage sellItem(Player* player, S32 index);
	enWarnMessage reBuyItem(Player* player, S32 index);
	enWarnMessage setNpcShopPage(Player* player, S32 page);

	enWarnMessage exchangeItemForTrade(Player* player, Player* target);
	enWarnMessage RegisterTimeItem(Player* player, ItemShortcut* pItem);

	void syncPanel(Player* player, U32 itemid);

	void syncPetPanel(Player* player, U32 nOldPetSlot, U32 nNewPetSlot);
	void syncMountPanel(Player* player, U32 nOldPetSlot, U32 nNewPetSlot);

	enWarnMessage detectPutItem(Player* player, Vector<stItemResult*> itemList, Vector<stItemResult*>& resultList, bool ignoreLock/* = false*/);
	void donePutItem(Player* player, Vector<stItemResult*>& resultList);
	U32 putItem(U32 PlayerID, U32 ItemID, S32 num);
	enWarnMessage batchItem(U32 PlayerID);

	enWarnMessage unLockPlayerTrade(Player* player);
	enWarnMessage setChangeSize(Player* player, U32 index, ItemShortcut* pItem);

	void changeToNormalFashionEquip(Player* player, S32 destIndex, ItemShortcut* pNewEquip);
	void changeFashionEquip(Player* player, S32 destIndex, ItemShortcut* pOldEquip, ItemShortcut* pNewEquip);

	enWarnMessage repairAllEquip(Player* player);
	enWarnMessage repairOneEquip(Player* player, U32 type, U32 slotType, S32 index = 0);
private:
	Vector<stItemResult*> mTempItemList;
	void clearTempItemList();	
#endif //NTJ_SERVER

public:
	enWarnMessage canTrade(Player* player, Player* target);
	bool		  hasItem(Player* player, U32 itemID);

#ifdef NTJ_CLIENT
	bool		  addItemToMission(Player* player, U32 type, U32 itemID, U32 num);
#endif //NTJ_CLIENT
};

// ----------------------------------------------------------------------------
// 栏位基类
class BaseItemList
{
public:
	BaseItemList(S32 maxSlots);	
	virtual ~BaseItemList();
	virtual bool LoadData(stPlayerStruct* playerInfo) { return true; };
	virtual void SendInitialData(GameConnection* conn) {};
	virtual bool UpdateToClient(GameConnection* conn, S32 index, U32 flag) { return true;};
	virtual bool SaveData(stPlayerStruct* playerInfo) { return true; };

	ShortcutObject* GetSlot(S32 index);
	ShortcutObject* GetSlotByID(S32 id);
	bool SetSlot(S32 index, ShortcutObject* pSlot, bool IsClear = true);
	bool DelSlot(S32 index, U32 itemNum);
	S32 FindEmptySlot();
	S32 findSlot(U64 uId);
	S32 QueryEmptySlot(Vector<S32>& slotlist);
	S32 GetEmptyCount();
	S32 FindSameObject(ShortcutObject* obj, bool checkSlotState, bool ignoreExtPro = true);
	S32 FindSameObjectByID(S32 id, bool checkSlotState = false);
	S32 QuerySameObject(ShortcutObject* obj, Vector<S32>& slotlist, bool checkSlotState, bool ignoreExtPro = true);
	S32 QuerySameObjectByID(S32 id, Vector<S32>& slotlist, bool checkSlotState = false);
	bool IsVaildSlot(S32 index);
	bool CompactSlots();
	S32 GetMaxSlots();

	virtual void Clear();
	virtual void Reset();
	
protected:
	S32 mMaxSlots;				// 实际最大槽位数
	S32 mType;					// 栏位类型
	ShortcutObject** mSlots;	// 栏位对象数组
};

// ----------------------------------------------------------------------------
// 物品栏类
class InventoryList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = INVENTROY_MAXSLOTS;
	InventoryList();
	InventoryList(U32 maxSlots);
	bool LoadData(stPlayerStruct* playerInfo);
	void SendInitialData(GameConnection* conn);
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	void Unlock(GameConnection* conn);
	bool SaveData(stPlayerStruct* playerInfo);
	S32 GetItemCount(ShortcutObject* obj, bool checkSlotState = true, bool ignoreExtPro = true);
	S32 GetItemCount(S32 id, bool checkSlotState = true, bool ignoreExtPro = true);
	void SetMaxSlot(S32 maxslots);
	void UpdateMaxSlots(GameConnection* conn);
	enWarnMessage CanChangeMaxSlots(U32 srcBag1, U32 srcBag2, U32 tagBag1, U32 tagBag2);
	void ChangeMaxSlots(Player*, U32 srcBag1, U32 srcBag2, U32 tagBag1, U32 tagBag2);
	InventoryList* CloneList();
	ItemShortcut* FindSlot(U64 uid);
	
};

// ----------------------------------------------------------------------------
// 装备栏类
class EquipList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = EQUIP_MAXSLOTS;
	EquipList();
	bool LoadData(stPlayerStruct* playerInfo);
	void SendInitialData(GameConnection* conn);
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	bool SaveData(stPlayerStruct* playerInfo);

	S32  GetRepairCost();

	S32  GetBagSize(S32 index);
	ItemShortcut* GetBagItem(S32 index);
};

// ----------------------------------------------------------------------------
// 银行仓库栏类
class BankList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = BANK_MAXSLOTS;
	BankList();
	bool LoadData(stPlayerStruct* playerInfo);
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	bool SaveData(stPlayerStruct* playerInfo);
	void SetMaxSlot(S32 maxslots);
	void UpdateMaxSlots(GameConnection* conn);
	enWarnMessage CanChangeMaxSlots(U32 srcBag1, U32 tagBag1);
	void ChangeMaxSlots(Player*, U32 srcBag1, U32 tagBag1);

	U32	 mBuyBagSize;

};

// ----------------------------------------------------------------------------
// 玩家回购栏类
class DumpList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 5;
	DumpList();
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
public:
	S32 mCurrentIndex;
};

// ----------------------------------------------------------------------------
// 快捷栏
class PanelList: public BaseItemList
{
public:
	static const U32 MAXSLOTS = PANEL_MAXSLOTS;
	PanelList();
	bool LoadData(stPlayerStruct* playerInfo) { return true;};
	bool LoadData(stPlayerStruct* playerInfo, Player* player);

	void SendInitialData(GameConnection* conn);
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	bool SaveData(stPlayerStruct* playerInfo);

	inline bool IsLocked() { return mIsLocked;}
	void SetLocked(bool IsLock = true) { mIsLocked = IsLock;}

#ifdef NTJ_CLIENT
	void LoadCacheUI(Player* player);
	void SaveCacheUI(Player* player);
#endif//NTJ_CLIENT
private:
	bool mIsLocked;		//是否锁住，不允许拖放操作
#ifdef NTJ_CLIENT
	U32 mUI[MAXSLOTS];	//保留曾经使用过物品或技能的ID，便于半透明显示
#endif//NTJ_CLIENT
};

// ----------------------------------------------------------------------------
// 玩家交易栏类
class TradeList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 5;
	static const U32 MAXPETSLOTS = 4;
	TradeList();
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	bool UpdateToTarget(Player* source, Player* target, S32 index);
	void SetTradeState(enTradeState state) { mState = state;}
	void SetTradeMoney(S32 money) { mMoney = money;}
	enTradeState GetTradeState() { return mState;}
	S32 GetTradeMoney()	{ return mMoney;}
	void Reset();
private:
	enTradeState	mState;			// 当前交易状态
	U32				mMoney;			// 当前交易金钱
	ShortcutObject* mPetSlots[MAXPETSLOTS];	// 宠物交易栏
};

// ----------------------------------------------------------------------------
// 玩家的NPC系统商店列表
class NpcShopList
{
public:
	static const U32 MAXSLOTS = 10;	
	NpcShopList();
	~NpcShopList();
	void Clear();
	void SetNpcShopID(S32 shopid) { mCurrentNpcShopID = shopid;}
	S32 GetNpcShopID() { return mCurrentNpcShopID;}
	void SetNpcShopPage(S32 page, S32 total) { mCurrentPage = page; mTotalPage = total;}
	S32 GetNpcShopPage() { return mCurrentPage;}
	S32 GetNpcTotalPage() { return mTotalPage;}
	void SetQueryTime(U32 t) { mLastQueryTime = t;}
	bool CanQueryTime(U32 t) { return (mLastQueryTime + 5 < t);}
	void SetShopType(U32 t) { mShopType = t;}
	U32 GetShopType()	{ return mShopType;}
	ShortcutObject* GetSlot(S32 index);
	ShortcutObject* GetSlotTrue(S32 index);
	void UpdateData();
	void setFilterCase(S32 id) { mFilterCase = id; }
	S32  getFilterCase() { return mFilterCase; }	

public:
	NpcShopItem* mSlots[MAXSLOTS];
private:
	S32 mShopType;			// 玩家商店附加功能类型
	S32 mCurrentNpcShopID;	// 玩家当前商店列表ID
	S32 mCurrentPage;		// 玩家商店列表当前页数
	S32 mTotalPage;			// 玩家商店列表总页数
	S32 mLastQueryTime;		// 上次客户端玩家请求查询列表的时间
	S32 mFilterCase;		// 筛选条件
};

// 玩家个人摆摊栏
class IndividualStallList 
{
public:
	
	static const U32 MAXSLOTS = 30;	
	IndividualStallList();
	~IndividualStallList();
	void clear();
	void UpdateMaxSlots(GameConnection* conn);
	bool UpdateToClient(GameConnection* conn, S32 index);
	bool UpdateTargetToClient(Player* player, Player* target, S32 index);

	ItemShortcut* GetSlot(S32 index);
	U32	getMaxSlot();
	U32 getStallMoney(S32 index);
	StringTableEntry getStallName();	//摊位名
	StringTableEntry getStallPos();		//地点
	StringTableEntry getStallInfo();
	U32 getStallRate();					//税率
	U32 getStallState() {return mState; }


	void SetSlot(S32 index, ItemShortcut* pItem);
	void setStallMoney(S32 index, U32 money);
	void SetMaxSlot(S32 maxslots);
	void setStallName(StringTableEntry name);
	void setStallInfo(StringTableEntry info);
	void setStallState(U32 state) { mState = state; }

	U32 getEmptyNum(Vector<U32>& empty);
	S32 findEmptyIndex();

	void addPlayerList(U32 playerID);
	void delPlayerList(U32 playerID);
	Vector<U32>& getPlayerList() { return mPlayerList; }

	U32 getPlayerListID(U32 index);

	enWarnMessage CanChangeMaxSlots(U32 srcBag1, U32 srcBag2, U32 tagBag1, U32 tagBag2);
	void ChangeMaxSlots(Player*, U32 srcBag1, U32 srcBag2, U32 tagBag1, U32 tagBag2);
	void	clearChatList();
	void	clearTradeList();

public:
	StallItem** mSlots;
	Vector<stChatRecord*>		mChatInfoList;
	Vector<stTradeRecord*>	mTradeInfoList;
private:
	StringTableEntry	mStallName;
	StringTableEntry	mStallInfor;
	
	U32	mMaxSlot;
	U32 mState;
	//摆摊交易玩家列表
	Vector<U32> mPlayerList;
	
};

// 摆摊宠物栏
class StallPetList 
{
public:
	static const U32 MAXSLOTS = 6;
	StallPetList();
	~StallPetList();
	void clear();
	PetShortcut* GetSlot(S32 index);
	bool UpdateToClient(GameConnection* conn, S32 index);
	bool UpdateTargetToClient(Player* player, Player* target, S32 index);
	void SetSlot(S32 index, PetShortcut* pItem, U32 money);
	S32 findEmptyIndex();
public:
	stStallPet* mSlots[MAXSLOTS];
};

// 摆摊收购栏
class StallBuyItemList
{
public:
	static const U32 MAXSLOTS = 6;
	StallBuyItemList();
	~StallBuyItemList();
	void clear();
	ItemShortcut* GetSlot(S32 index);
	bool UpdateToClient(GameConnection* conn, S32 index);
	bool UpdateTargetToClient(Player* player, Player* target, S32 index);
	void SetSlot(S32 index, ItemShortcut* pItem, U32 money, U32 num);
	S32  findEmptySlot();
public:
	stStallBuyItem* mSlots[MAXSLOTS];
};

// 摆摊查询栏
#ifdef NTJ_CLIENT
class StallLookUpList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 3;
	StallLookUpList();
};
#endif

// ----------------------------------------------------------------------------
// 玩家拾取物品列表
class PickupList
{
	enum
	{
		NUM_ITEMS_PER_PAGE = 4
	};
public:
	PickupList() : mCurrentPage(0) {}
	void	Clear();
	S32		FindItemByID(U32 nItemID);
	void	ClearList(S32 count, S32 mask);
	S32		GetCurrentPage() { return mCurrentPage;}
	S32		GetTotalPage(); 
	bool	EraseSlot(U32 nIndex);
	ShortcutObject* GetSlot(S32 nIndex);
	bool	Empty() { return mSlots.empty(); }
	U32		Size() { return mSlots.size(); }
	bool	AddItem(Player *pPlayer, U32 nItemId);
	void	AddItem(ItemShortcut* pItem) { mSlots.push_back(pItem); }
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	bool	IncCurrentPage();
	bool	DecCurrentPage();
	bool	SetCurrentPage(S32 pageIndex);
	ItemShortcut* GetSlotForGuiControl(S32 nIndex);
	StringTableEntry GetSlotItemName(S32 nIndex);
	void	DeleteSlot(U32 nIndex);
	void	UpdateList();
	bool	CanGetSlotInCurrentPage();
private:
	Vector<ItemShortcut*> mSlots;
	S32 mCurrentPage;		// 玩家拾取当前页数
};


class Skill;
//技能提示栏
class SkillList_Hint:public BaseItemList
{
public:
	static const U32 MAXSlOTS = 8;
	SkillList_Hint();
	bool AddSkillHintShortcut(Player* player,U32 seriesId);
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	void CheckCurrentSlot();
private:
	bool IsAdd(Skill* pSkill);
	U32  mStartIndex; 
};


#ifdef NTJ_CLIENT
// 通用技能栏
class SkillList_Common: public BaseItemList
{
public:
	static const U32 MAXSLOTS = 64;
	SkillList_Common();
	bool AddSkillShortcut(Player* player, U32 seriesId);
private:
	bool IsAdd(Skill* pSkill);
};

// 技能学习栏
class SkillList_Study: public BaseItemList
{
public:
	static const U32 MAXSLOTS = 8;
	SkillList_Study();
	bool AddSkillStudyShortcut(Player* player, U32 seriesId);
	bool UpdataSkillStudyUI(U32 skillId);
private:
	bool IsAdd(SkillData* pData);
};

// 元神绝招栏
class SkillList_Spirit : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 6;
	SkillList_Spirit();
	bool AddSkillShortcut(Player *pPlayer, U32 nSeiresId, S32 nSlot);
private:
	bool IsAdd(Skill *pSkill);
};

#endif//NTJ_CLIENT

// ----------------------------------------------------------------------------
// 自动售物栏
// ----------------------------------------------------------------------------
#ifdef NTJ_CLIENT
class AutoSellList
{
public:
	AutoSellList();
	~AutoSellList();
	ItemShortcut* GetSlot(U32 index);
	void setShowType(U32 type);
	U32 getShowType();
	void showAutoSellItem();
	void clearGeneralItemList();
	void clearBestItemList();
	void clear();
	U32 getItemCount();
	void assortItem();
	void autoSell();
	void addToAutoSellList(Player* pPlayer, S32 index);
	void delFromAutoSellList(Player* pPlayer, S32 index);
	void autoSellByIndex(Player* pPlayer, U32 index);
	void clearAll(Player* pPlayer);//全部清除

	//出售助手
	Vector<ItemShortcut*> mGeneralItem;
	Vector<ItemShortcut*> mBestItem;
	Vector<ItemShortcut*> mAutoSellItem;
	U32 mShowType;
	bool isAutoSell;

};

struct TradeHelpShortCut 
{
	ShortcutObject* shortcut;
	U32 mIndex;
};
//摆摊和交易助手
class TradeAndStallHelpList
{
public:
	enum Sel_type
	{
		sel_common		= 0,
		sel_excellence,
		sel_seldom,
		sel_pet,
	};
public:
	TradeAndStallHelpList();
	~TradeAndStallHelpList();
	void Initialize(Player* pPlayer, Sel_type nOptType);
	void Clear();
	ShortcutObject* GetSlot(S32 index);
	TradeHelpShortCut* getShortcut(S32 index);
private:
	Vector<TradeHelpShortCut*>	mSlots;
	
};
#endif//NTJ_CLIENT

// ----------------------------------------------------------------------------
// 装备鉴定
// ----------------------------------------------------------------------------
class EquipIdentifyList: public BaseItemList
{
public:
	static const U32 MAXSLOTS = 2;
	EquipIdentifyList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
};

// ----------------------------------------------------------------------------
// 装备强化
// ----------------------------------------------------------------------------
class EquipStrengthenList: public BaseItemList
{
public:
	static const U32 MAXSLOTS = 4;
	EquipStrengthenList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
};

// ----------------------------------------------------------------------------
// 装备镶嵌栏
// ----------------------------------------------------------------------------
class MountGemList : public BaseItemList
{
public:
	enum OPT_TYPE
	{
		TYPE_GEM_NONE,				//无操作
		TYPE_GEM_MOUNT,				//镶嵌
		TYPE_GEM_UNMOUNT,			//摘取
	};

	static const U32 MAXSLOTS = 4;	//目前允许最多三个孔，另加一个装备槽
	MountGemList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	U32		getFlag() { return mFlag; }

	bool	InitEmbeding(ItemShortcut *pEquipItem);
	S32		GetAllowEmbedSlotsCount();
	S32		GetOpenedEmbedSlotsCount();

private:
	void	InitEmbedSlots(ItemShortcut *pEquipItem);

private:
	U32		mFlag;

#ifdef NTJ_CLIENT
public:
	S32		GetSlotType(S32 nIndex);
	bool	mOverlap[MAXSLOTS - 1];
#endif
};

// ----------------------------------------------------------------------------
// 装备打孔栏
// ----------------------------------------------------------------------------
class PunchHoleList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 4;	//目前允许最多三个孔，另加一个装备槽
	PunchHoleList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	U32		getFlag() { return mFlag; }
private:
	U32		mFlag;
};

// ----------------------------------------------------------------------------
// 任务奖励物品栏
// ----------------------------------------------------------------------------
#ifdef NTJ_CLIENT
class MissionItemList
{
public:
	MissionItemList();
	void Clear(S32 type = -1);
	ShortcutObject* GetSlot(U32 type, S32 index);
	S32 Size(U32 type);
	Vector<ItemShortcut*>	mOptionalItemList;			//可选奖励栏
	Vector<ItemShortcut*>	mFixedItemList;				//固定奖励栏
};
#endif//NTJ_CLIENT

// ----------------------------------------------------------------------------
// 生活技能栏
// ----------------------------------------------------------------------------
#ifdef NTJ_CLIENT
class LivingSkillData;
class LivingSkillList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 10;
	LivingSkillList();
	bool AddLivingSkillShortcut(U32 skillID,U32 ripe);
private:
	void setState(U32 slotIndex,LivingSkillData* pData); //设置shortcut状态
};
#endif

// ----------------------------------------------------------------------------
// 配方材料栏
// ----------------------------------------------------------------------------
#ifdef NTJ_CLIENT
class PrescriptionList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 7;
	PrescriptionList();
	bool	initPrescriptionList	(U32 SerialID);
};
#endif

class PetTable;
class TempList : public BaseItemList
{
public:
	enum OptType
	{
		OptType_None					= 0,		//无操作空闲
		OptType_PetIdentify				= 1,		//宠物鉴定
		OptType_PetInsight				= 2,		//宠物慧根
		OptType_PetLianHua				= 3,		//宠物炼化
		OptType_PetHuanTong				= 4,		//宠物还童
		OptType_EquipPunchHole			= 5,		//装备打孔
		OptType_EquipGemEmbed			= 6,		//装备镶嵌
		OptType_EquipStrengthen			= 7,		//装备强化
		OptType_PickUpItem				= 8,		//拾取物品
		OptType_SpiritSkill				= 9,		//元神绝招
	};

	enum TempSlotSize
	{
		SlotSize_None					= 0,
		SlotSize_PetIdentify			= 1,		//宠物鉴定槽数
		SlotSize_PetInsight				= 2,		//宠物慧根槽数
		SlotSize_PetLianhua				= 2,		//宠物炼化槽数
		SlotSize_PetHuanTong			= 1,		//宠物还童槽数
		SlotSize_EquipPunchHole			= 4,		//装备打孔槽数
		SlotSize_EquipGemEmbed			= 4,		//装备镶嵌槽数
		SlotSize_EquipStrengthen		= 5,		//装备强化槽数
		SlotSize_PickUpItem				= 6,		//拾取物品槽数
		SlotSize_SpiritSkill			= 1,		//元神绝招物品槽数
	};

	static const U32 MAXSLOTS = 10;
	TempList();
	bool					UpdateToClient						(GameConnection* conn, S32 index, U32 flag);
	
	void					Clean								(Player* pPlayer);
	bool					SetOptType							(GameConnection* conn, OptType typ);
	OptType					GetOptType	()						{ return m_optType; }

#ifdef NTJ_SERVER
	static enWarnMessage	TempListMoveToTempList				(stExChangeParam* param);
	static enWarnMessage	TempListMoveToInventory				(stExChangeParam* param);
	static enWarnMessage	InventoryMoveToTempList				(stExChangeParam* param);
	static enWarnMessage	EquipMoveToTempList					(stExChangeParam* param);
	static enWarnMessage	TempListMoveToEquip					(stExChangeParam* param);

	//慧根槽与物品栏槽相交换
	static enWarnMessage	PetInsighMoveToInventory			(stExChangeParam* param);
	static enWarnMessage	InventoryMoveToPetInsight			(stExChangeParam* param);

	//还童槽与物品栏槽相交换
	static enWarnMessage	PetHuanTongMoveToInventory			(stExChangeParam* param);
	static enWarnMessage	InventoryMoveToPetHuanTong			(stExChangeParam* param);

	//元神绝招物品槽与物品栏槽相交换
	static enWarnMessage	SpiritSkillMoveToInventory			(stExChangeParam* param);
	static enWarnMessage	InventoryMoveToSpiritSkill			(stExChangeParam* param);
	
	//宠物槽分别交换到宠物鉴定栏、宠物灵慧栏、宠物炼化栏
	static enWarnMessage	PetListMoveToPetIdentify			(stExChangeParam* param);
	static enWarnMessage	PetListMoveToPetInsight				(stExChangeParam* param);
	static enWarnMessage	PetListMoveToPetLianHua				(stExChangeParam* param);
	static enWarnMessage	PetListMoveToPetHuanTong			(stExChangeParam* param);

	/*
	//物品栏交换到宠物灵慧栏、宠物炼化栏
	static enWarnMessage	InventoryMoveToPetInsight			(stExChangeParam* param);
	static enWarnMessage	InventoryMoveToPetLianHua			(stExChangeParam* param);
	*/
#endif

#ifdef NTJ_CLIENT
	static enWarnMessage	CheckInventoryMoveToTempList		(Player *pPlayer, ItemShortcut *pSrcItem, S32 nDestIndex);
#endif
	static enWarnMessage	CheckInventoryMoveToPetInsight		(const PetTable &table, U32 nSlot, ItemShortcut *pGenGuItem);
	static enWarnMessage	CheckInventoryMoveToPetHuanTong		(const PetTable &table, U32 nSlot, ItemShortcut *pHuanTongDan);
	static enWarnMessage	CheckInventoryMoveToSpiritSkill		(Player *pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex);
private:
	OptType m_optType;
};

class PetList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 8;	
	PetList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	U32		getFlag() { return mFlag; }
	void	Initialize();
	
private:
	U32		mFlag;
};

class MountPetList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 8;
	MountPetList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	U32		getFlag() { return mFlag; }
	void	Initialize();
private:
	U32		mFlag;
};

class SpiritList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 5;
	SpiritList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	U32		getFlag() { return mFlag; }
	void	Initialize();
private:
	U32		mFlag;
};

#ifdef NTJ_CLIENT
class TalentSkillList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 36;
	TalentSkillList();
	U32		getFlag() { return mFlag;}
	void	Initialize(Player *pPlayer, S32 nSpiritSlot, bool bIsSelf = true);
private:
	U32		mFlag;
};
#endif

#ifdef NTJ_CLIENT
class PetHelpList
{
public:
	enum Opt_Type
	{
		Opt_None		= 0,
		Opt_JianDing	= 1,
		Opt_HuiGeng		= 2,
		Opt_LianHua		= 3,
		Opt_HuanTong	= 4,
		Opt_Max
	};

public:
	PetHelpList();
	~PetHelpList();
	void Initialize(Opt_Type nOptType);
	void Clear();
	PetShortcut *GetSlot(S32 index);
	void AppendSlot(S32 nSlotIndex);
	void AppendSlot(PetShortcut *pSlot);
private:
	Vector<PetShortcut *> mPetHelpList;
	Player *mPlayer;
};
#endif

#ifdef NTJ_CLIENT
class SuperMarketList 
{
public:
	static const U32 MAXSLOTS = 12;
	SuperMarketList();
	~SuperMarketList();
	ItemShortcut* GetSlot(S32 index);
	U32 getMarketItemID(S32 index);
	void SetSlot(S32 index, stMarketItem* pItem);
	void clear();
public:
	stMarketItem* mSlots[MAXSLOTS];
};
//推荐栏
class RecommendItemList
{
public:
	static const U32 MAXSLOTS = 8;
	RecommendItemList();
	~RecommendItemList();
	ItemShortcut* GetSlot(S32 index);
	void SetSlot(S32 index, stMarketItem* pItem);
	void clear();
public :
	stMarketItem* mSlots[MAXSLOTS];
};
//购物篮
class ShopBaksetList
{
public:
	ShopBaksetList();
	~ShopBaksetList();
	ItemShortcut* GetSlot(S32 index);
	void SetSlot(S32 index, stMarketItem* pItem);
	void clear();
public:
	Vector<stMarketItem*> mSlots;
};
#endif

class RepairList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 2;
	RepairList();
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
};

//任务递交物品栏
class MissionSubmitList: public BaseItemList
{
public:
	static const U32 MAXSLOT = 12;
	MissionSubmitList();
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
};

extern ItemManager* g_ItemManager;

#endif//__PLAYER_ITEM_H__