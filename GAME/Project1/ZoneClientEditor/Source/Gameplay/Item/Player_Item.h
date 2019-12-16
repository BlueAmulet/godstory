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

// ��Ҽ佻��״̬
enum enTradeState
{
	TRADE_NONE		= 0,					// �޽���״̬
	TRADE_ASKFOR,							// ��������
	TRADE_ANSWER,							// ����Ӧ������
	TRADE_REJECT,							// �ܾ���������
	TRADE_BEGIN,							// ���׿�ʼ
	TRADE_LOCK,								// ���׼���
	TRADE_UNLOCK,							// ���׽���
	TRADE_END,								// ���׽���
	TRADE_CANCEL,							// ����ȡ��

	TRADE_UPITEM,							// ������Ʒ�ϼ�
	TRADE_DOWNITEM,							// ������Ʒ�¼�

	TRADE_LOCKNOTIFY,						// ������Ʒ����֪ͨ�Է�
	TRADE_UNLOCKNOTIFY,						// ������Ʒ����֪ͨ�Է�
	TRADE_ENDNOTIFY,						// ȷ������֪ͨ�Է�

	TRADE_SETMONEY,							// ���ó��ۼ۸�
	
	STALL_RENAME,							// ����
	STALL_REINFO,							// �޸����ݼ��

	STALL_REMOVE_ALL,						// ���ж��¼�
	STALL_SEE_CLOSE,						// �رղ쿴
	STALL_CHANGE,							// �Ӱ�
	STALL_ADDTOLOOKUP,						// ��ӵ������б�
	STALL_BUYLOOKUP,						// ���������Ʒ
	STALL_SCANSTALLNOTE,					// �򿪰�̯��Ϣ
	STALL_CLEAR_MSG,						// ������׺�������Ϣ
	//����ֻ�ڿͻ�����
	STALL_TRADERECORD,						// ������Ϣ
	STALL_INFOMSG,							// ̯λ��Ϣ
	STALL_CHATMSG,							// ������Ϣ

};

//��̯����
enum enStallType
{
	stall_individual = 0,					// ���˰�̯
	stall_fixed,							// �����ߵĹ̶���̯
	stall_faction,							// ����̯
};

enum enNpcShopType
{
	NPCSHOP_OPEN = 0,						// ��NPC�̵�
	NPCSHOP_CLOSE,							// �ر�NPC�̵�
	NPCSHOP_BUY,							// ���̵�����Ʒ
	NPCSHOP_SELL,							// ����Ʒ���̵�
	NPCSHOP_RECYCLE,						// �ع���Ʒ
	NPCSHOP_PAGE,							// ��Ʒ�����ҳ
	NPCSHOP_REPAIRSIGLE,					// ������װ��
	NPCSHOP_REPAIRALL,						// ��������װ��
	NPCSHOP_REPAIRFUCTION,					// �������
	NPCSHOP_FILTER,							// ɸѡ
	DURABLE_REPAIR,							// ��������
	RECOVER_REPAIR,							// �ָ�����;�
	REPAIR_CLOSE,							// �ر�����
	
};

enum enMarketActionType
{
	MARKET_OPEN = 0,						// ���̳�
	MARKET_BUY,								// �̳ǹ���
	MARKET_COMPIMENT,						// ����
	MARKET_ASKFOR,							// ��Ҫ
};

// ��λ����
enum enShortcutType
{
	SHORTCUTTYPE_NONE,						// δ֪��λ����
	SHORTCUTTYPE_PANEL				= 1,	// �����
	SHORTCUTTYPE_INVENTORY			= 2,	// ��Ʒ��
	SHORTCUTTYPE_EQUIP				= 3,	// װ����
	SHORTCUTTYPE_BANK				= 4,	// ���вֿ���
	SHORTCUTTYPE_DUMP				= 5,	// �̵�ع���
	SHORTCUTTYPE_TRADE				= 6,	// ��ҽ�����		
	SHORTCUTTYPE_STALL				= 7,	// ��Ұ�̯��
	SHORTCUTTYPE_NPCSHOP			= 8,	// NPCϵͳ�̵�
	SHORTCUTTYPE_SKILL_COMMON		= 9,	// ��ͨ������
	SHORTCUTTYPE_PICKUP				= 10,	// ʰȡ��Ʒ��
	SHORTCUTTYPE_AUTOSELL			= 11,	// �Զ�����
	SHORTCUTTYPE_IDENTIFY			= 12,	// װ������
	SHORTCUTTYPE_COMPOSE			= 13,	// װ���ϳ�
	SHORTCUTTYPE_GEM_MOUNT			= 14,	// װ����Ƕ
	SHORTCUTTYPE_STRENGTHEN			= 15,	// װ��ǿ��
	SHORTCUTTYPE_MISSIONITEM		= 16,	// ��ѡ��������
	SHORTCUTTYPE_MISSION_TRACE		= 17,	// ����׷��
	SHORTCUTTYPE_EQUIP_PUNCHHOLE	= 18,	// װ�����
	SHORTCUTTYPE_EQUIP_EMBEDABLE	= 19,	// װ������Ƕ�б�
	SHORTCUTTYPE_LIVINGSKILL        = 20,   // �������
	SHORTCUTTYPE_LIVINGCATEGORY     = 21,   // ���������
	SHORTCUTTYPE_PRESCRIPTION       = 22,   // �䷽�����б�
	SHORTCUTTYPE_FIXEDMISSIONITEM	= 23,	// �̶���������
	SHORTCUTTYPE_SKILL_STUDY        = 24,   // ����ѧϰ��
	SHORTCUTTYPE_TARGET_TRADE		= 25,	// �Է���ҽ�����
	SHORTCUTTYPE_PET				= 26,	// ������
	SHORTCUTTYPE_TARGET_STALL		= 27,	// �Է���Ұ�̯��
	SHORTCUTTYPE_TEMP				= 28,	// ��ʱ��
	SHORTCUTTYPE_STALL_PET			= 29,	// �������
	SHORTCUTTYPE_STALL_BUYLIST		= 30,	// �չ��嵥
	SHORTCUTTYPE_MOUNT_PET_LIST		= 31,	// ��˳�����
	SHORTCUTTYPE_TARGET_STALL_PET   = 32,   // ��̯�Է������嵥
	SHORTCUTTYPE_TARGET_STALLBUYLIST= 33,	// ��̯�Է������嵥
	SHORTCUTTYPE_STALL_LOOKUP		= 34,	// ��̯����
	SHORTCUTTYPE_PET_HELP_LIST		= 35,	// ���������
	SHORTCUTTYPE_SKILL_HINT         = 36,   // ������ʾ��
	SHORTCUTTYPE_TRADE_STALL_HELP	= 37,	// ����Ͱ�̯������
	SHORTCUTTYPE_MAIL_ITEM			= 38,   // �ʼ���Ʒ��
	SHORTCUTTYPE_REPAIR				= 39,	// ���úͻָ�����;ã�����һ�����棩
	SHORTCUTTYPE_SUPERMARKET		= 40,   // �̳���
	SHORTCUTTYPE_ITEMSPLIT          = 41,   // ��Ʒ�ֽ���
	SHORTCUTTYPE_ITEMCOMPOSE        = 42,   // ��Ʒ�ϳ�
	SHORTCUTTYPE_RECOMMEND			= 43,	// �Ƽ���
	SHORTCUTTYPE_SHOPBASKET			= 44,   // ������
	SHORTCUTTYPE_SPIRITLIST			= 45,   // Ԫ����
	SHORTCUTTYPE_SPIRITSKILLLIST	= 46,   // Ԫ�������
	SHORTCUTTYPE_SPIRITSKILLLIST_2	= 47,   // Ԫ�������(NPC�Ի���)
	SHORTCUTTYPE_TALENTSKILLLIST	= 48,   // Ԫ���츳������
	SHORTCUTTYPE_MISSIONSUBMITLIST	= 50,	// ����ݽ���Ʒ��
};

// �̵긽�ӹ���
enum enShopFunction
{
	SHOPFUNCTION_NONE		= 0,
	SHOPFUNCTION_FIX		= 1 << 0,		// ��ͨ����
	SHOPFUNCTION_FIXALL		= 1 << 1,		// ��ͨȫ��
	SHOPFUNCTION_FFIXALL	= 1 << 2,		// ���ȫ��
};

// �ͻ�����ʾ��Ʒ�ı�־
enum enItemFlag
{
	ITEM_NOSHOW = 0,						// �����κ���ʾ
	ITEM_LOAD,								// ��ʼ����Ʒ��
	ITEM_BUY,								// ����
	ITEM_SELL,								// ����
	ITEM_TRADE,								// ����
	ITEM_PICK,								// ʰȡ
	ITEM_DROP,								// ����
	ITEM_SENDMAIL,							// �ʼ���Ʒ
	ITEM_RECEIVEMAIL,						// ��ȡ�ʼ���Ʒ
	ITEM_COMPOSE,							// �ϳ�
	ITEM_FROMBANK,							// ������ȡ����һ����Ʒ
	ITEM_SAVEBANK,							// �������д����һ����Ʒ
	ITEM_LOSE,								// ��ʧȥ����Ʒ
	ITEM_USED,								// ��ʹ������Ʒ
	ITEM_GET,								// ��ȡ
	ITEM_REPAIRSUCC,						// ������Ʒ�ɹ�
	ITEM_PUTTRADE,      					// ������������һ����Ʒ
	ITEM_RETAKETRADE,   					// �ӽ�����ȡ��һ����Ʒ
	ITEM_SPLIT,         					// װ���ֽ�
	ITEM_IDENTIFY,							// װ������
	ITEM_STALLBUY,							// �Ӱ�̯��������Ʒ
	ITEM_REBUY,								// �ӻع����ع���Ʒ
	ITEM_REPAIRFAIL,						// ������Ʒʧ��
};

#ifdef NTJ_SERVER
// ��λ�������ݲ����ṹ
struct stExChangeParam
{
	Player*					player;			// ��Ҷ���
	enShortcutType			SrcType;		// Դ��λ����
	enShortcutType			DestType;		// Ŀ����λ����
	S32						SrcIndex;		// Դ��λ����
	S32						DestIndex;		// Ŀ���λ����
	S32						Price;			// �������
	S32						SkillSeriesId;	// ����ϵ��ID
};
#endif

enum ITEMOP_TYPE
{
	OP_NONE			= 0,					// δ����
	OP_FINDADD		= 0x01,					// �ҵ�����
	OP_FINDEMPTY	= 0x02,					// �ҵ��ղ�
	OP_FINDDEL		= 0x04,					// �ҵ�ɾ��
	OP_FINDDELEMPTY = 0x08,					// ɾ�����ղ�
};

// �������ҿɷ�����Ʒ�Ĳ�λ���ݽṹ
struct stItemResult
{	
	ItemShortcut*	srcEntry;				// Դ��Ʒ����
	ItemShortcut*	newEntry;				// ���������Ʒ����
	U32				flag;					// ���������־
	S32				num;					// �����������Ʒ����
	U32				PlayerID;				// ��ҽ�ɫ���
};

struct StallItem
{
	U32 money;
	ItemShortcut* pItem;
};
//�����̯����
struct stStallPet
{
	PetShortcut* petSlot;
	U32 money;

};
//��̯�չ���Ʒ�ṹ
struct stStallBuyItem
{
	ItemShortcut* buyItem;
	U32 money;
	U32 num;
};
//�̳���Ʒ
struct stMarketItem
{
	U32 marketItemID;
	ItemShortcut* marketItem;
};

// ��Ҽ�¼�ṹ
struct stRecord
{
	U32 serialID;					// ��¼��ˮ��
	U32 time;						// ��¼����ʱ��
	U32 senderID;					// �����߽�ɫID
	U32 receiverID;					// �����߽�ɫID
	StringTableEntry senderName;	// �����߽�ɫ��
	StringTableEntry receiverName;	// �����߽�ɫ��

	stRecord() : serialID(0), time(0), senderID(0), receiverID(0), senderName(NULL), receiverName(NULL) { }
};

// �����¼�ṹ
struct stChatRecord : stRecord
{
	char* chatContent;
	U32	  contentLen;

	stChatRecord() : chatContent(NULL), contentLen(0) { }
};

// ���׼�¼�ṹ
struct stTradeRecord :stRecord
{
	enum
	{
		TRADE_ITEM,
		TRADE_PET,
	};
	U32 type;  // 1-��Ʒ 2-����
	S32	money;	// ��Ǯ
	union TradeContent
	{
		stItemInfo* item;	// ��Ʒ
		stPetInfo*	pet;	// ����
	} trade;

	stTradeRecord() { dMemset(this, 0, sizeof(stTradeRecord)); }
};

// ----------------------------------------------------------------------------
// װ��ǿ��������
class EquipStrengthen
{
public:
	EquipStrengthen();
	~EquipStrengthen();

	static bool IsFuLiaoA(ItemShortcut *pItem);
	static bool IsFuLiaoB(ItemShortcut *pItem);
	static bool IsFuLiaoC(ItemShortcut *pItem);

#ifdef NTJ_CLIENT
	//������Ʒ���͵õ���Ӧ��λ�����������ߡ���Ʒ����������0���ɹ�������1������������2�����˼�������3����ֻ�ڿͻ����ж�
	static bool getEquipStrengthenSlotIndex(ItemShortcut *pItem, S32 &nIndex);
#endif

	//�ܷ����Ʒ���϶���ĳ����λ
	static enWarnMessage checkMoveFromInventory(Player* pPlayer, ItemShortcut* pSrcItem, S32 nDestIndex);
	//�ܷ�ǿ��
	static enWarnMessage canStrengthen(Player* pPlayer, S32 &nCostMoney, U32 &nMaterialId, S32 &nMaterialNum);
	/////////////////////////////////////////
	static S32 getStrengthenPrice(ItemShortcut* pItem);
	static void getStrengthenMaterial(ItemShortcut* pItem, U32 &nMatirialId, S32 &nMatirialNum);
	static U32 getTotalSuccessRate(U32 nItemLevel, U32 nQualityLevel, U32 nEquipStrengthenLevel, U32 nYangSheng, ItemShortcut *pItemFuZhu_A);
	/////////////////////////////////////////

	static void openStrengthen(Player* player, NpcObject *pNpc);
	static void closeStrengthen(Player* player);

#ifdef NTJ_SERVER
	//ǿ�����
	static enWarnMessage setStrengthenResult(Player* pPlayer);
#endif
};

// ----------------------------------------------------------------------------
// װ���������
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
// װ������
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
// װ����������
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
// װ�����
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
// ��Ʒ����������
class ItemManager
{
public:
	ItemManager();
	enum Constants
	{
		TRADE_LEVEL_LIMIT		= 7,		// ���׹��ܵȼ�����
		TRADE_MAXWAITTIME		= 60,		// ��������ȴ����ʱ�� 
		MAX_ADDITEMS			= 10,		// ��һ�������Ʒ����ҵ��������
		EXTBAG_BEGINSLOT		= 40,		// ��չ��λ����Ʒ������ʼλ��
		EXTBAG_BANK_BEGINSLOT	= 12,		// ��չ��λ�����вֿ�������ʼλ��
		REPAIRCOST_ITEM			= 105090005,// ����װ����������������Ʒ(�칤����)
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
	// ����
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
// ��λ����
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
	S32 mMaxSlots;				// ʵ������λ��
	S32 mType;					// ��λ����
	ShortcutObject** mSlots;	// ��λ��������
};

// ----------------------------------------------------------------------------
// ��Ʒ����
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
// װ������
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
// ���вֿ�����
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
// ��һع�����
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
// �����
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
	bool mIsLocked;		//�Ƿ���ס���������ϷŲ���
#ifdef NTJ_CLIENT
	U32 mUI[MAXSLOTS];	//��������ʹ�ù���Ʒ���ܵ�ID�����ڰ�͸����ʾ
#endif//NTJ_CLIENT
};

// ----------------------------------------------------------------------------
// ��ҽ�������
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
	enTradeState	mState;			// ��ǰ����״̬
	U32				mMoney;			// ��ǰ���׽�Ǯ
	ShortcutObject* mPetSlots[MAXPETSLOTS];	// ���ｻ����
};

// ----------------------------------------------------------------------------
// ��ҵ�NPCϵͳ�̵��б�
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
	S32 mShopType;			// ����̵긽�ӹ�������
	S32 mCurrentNpcShopID;	// ��ҵ�ǰ�̵��б�ID
	S32 mCurrentPage;		// ����̵��б�ǰҳ��
	S32 mTotalPage;			// ����̵��б���ҳ��
	S32 mLastQueryTime;		// �ϴοͻ�����������ѯ�б��ʱ��
	S32 mFilterCase;		// ɸѡ����
};

// ��Ҹ��˰�̯��
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
	StringTableEntry getStallName();	//̯λ��
	StringTableEntry getStallPos();		//�ص�
	StringTableEntry getStallInfo();
	U32 getStallRate();					//˰��
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
	//��̯��������б�
	Vector<U32> mPlayerList;
	
};

// ��̯������
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

// ��̯�չ���
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

// ��̯��ѯ��
#ifdef NTJ_CLIENT
class StallLookUpList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 3;
	StallLookUpList();
};
#endif

// ----------------------------------------------------------------------------
// ���ʰȡ��Ʒ�б�
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
	S32 mCurrentPage;		// ���ʰȡ��ǰҳ��
};


class Skill;
//������ʾ��
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
// ͨ�ü�����
class SkillList_Common: public BaseItemList
{
public:
	static const U32 MAXSLOTS = 64;
	SkillList_Common();
	bool AddSkillShortcut(Player* player, U32 seriesId);
private:
	bool IsAdd(Skill* pSkill);
};

// ����ѧϰ��
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

// Ԫ�������
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
// �Զ�������
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
	void clearAll(Player* pPlayer);//ȫ�����

	//��������
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
//��̯�ͽ�������
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
// װ������
// ----------------------------------------------------------------------------
class EquipIdentifyList: public BaseItemList
{
public:
	static const U32 MAXSLOTS = 2;
	EquipIdentifyList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
};

// ----------------------------------------------------------------------------
// װ��ǿ��
// ----------------------------------------------------------------------------
class EquipStrengthenList: public BaseItemList
{
public:
	static const U32 MAXSLOTS = 4;
	EquipStrengthenList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
};

// ----------------------------------------------------------------------------
// װ����Ƕ��
// ----------------------------------------------------------------------------
class MountGemList : public BaseItemList
{
public:
	enum OPT_TYPE
	{
		TYPE_GEM_NONE,				//�޲���
		TYPE_GEM_MOUNT,				//��Ƕ
		TYPE_GEM_UNMOUNT,			//ժȡ
	};

	static const U32 MAXSLOTS = 4;	//Ŀǰ������������ף����һ��װ����
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
// װ�������
// ----------------------------------------------------------------------------
class PunchHoleList : public BaseItemList
{
public:
	static const U32 MAXSLOTS = 4;	//Ŀǰ������������ף����һ��װ����
	PunchHoleList();
	bool	UpdateToClient(GameConnection* conn, S32 index, U32 flag);
	U32		getFlag() { return mFlag; }
private:
	U32		mFlag;
};

// ----------------------------------------------------------------------------
// ��������Ʒ��
// ----------------------------------------------------------------------------
#ifdef NTJ_CLIENT
class MissionItemList
{
public:
	MissionItemList();
	void Clear(S32 type = -1);
	ShortcutObject* GetSlot(U32 type, S32 index);
	S32 Size(U32 type);
	Vector<ItemShortcut*>	mOptionalItemList;			//��ѡ������
	Vector<ItemShortcut*>	mFixedItemList;				//�̶�������
};
#endif//NTJ_CLIENT

// ----------------------------------------------------------------------------
// �������
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
	void setState(U32 slotIndex,LivingSkillData* pData); //����shortcut״̬
};
#endif

// ----------------------------------------------------------------------------
// �䷽������
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
		OptType_None					= 0,		//�޲�������
		OptType_PetIdentify				= 1,		//�������
		OptType_PetInsight				= 2,		//����۸�
		OptType_PetLianHua				= 3,		//��������
		OptType_PetHuanTong				= 4,		//���ﻹͯ
		OptType_EquipPunchHole			= 5,		//װ�����
		OptType_EquipGemEmbed			= 6,		//װ����Ƕ
		OptType_EquipStrengthen			= 7,		//װ��ǿ��
		OptType_PickUpItem				= 8,		//ʰȡ��Ʒ
		OptType_SpiritSkill				= 9,		//Ԫ�����
	};

	enum TempSlotSize
	{
		SlotSize_None					= 0,
		SlotSize_PetIdentify			= 1,		//�����������
		SlotSize_PetInsight				= 2,		//����۸�����
		SlotSize_PetLianhua				= 2,		//������������
		SlotSize_PetHuanTong			= 1,		//���ﻹͯ����
		SlotSize_EquipPunchHole			= 4,		//װ����ײ���
		SlotSize_EquipGemEmbed			= 4,		//װ����Ƕ����
		SlotSize_EquipStrengthen		= 5,		//װ��ǿ������
		SlotSize_PickUpItem				= 6,		//ʰȡ��Ʒ����
		SlotSize_SpiritSkill			= 1,		//Ԫ�������Ʒ����
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

	//�۸�������Ʒ�����ཻ��
	static enWarnMessage	PetInsighMoveToInventory			(stExChangeParam* param);
	static enWarnMessage	InventoryMoveToPetInsight			(stExChangeParam* param);

	//��ͯ������Ʒ�����ཻ��
	static enWarnMessage	PetHuanTongMoveToInventory			(stExChangeParam* param);
	static enWarnMessage	InventoryMoveToPetHuanTong			(stExChangeParam* param);

	//Ԫ�������Ʒ������Ʒ�����ཻ��
	static enWarnMessage	SpiritSkillMoveToInventory			(stExChangeParam* param);
	static enWarnMessage	InventoryMoveToSpiritSkill			(stExChangeParam* param);
	
	//����۷ֱ𽻻�����������������������������������
	static enWarnMessage	PetListMoveToPetIdentify			(stExChangeParam* param);
	static enWarnMessage	PetListMoveToPetInsight				(stExChangeParam* param);
	static enWarnMessage	PetListMoveToPetLianHua				(stExChangeParam* param);
	static enWarnMessage	PetListMoveToPetHuanTong			(stExChangeParam* param);

	/*
	//��Ʒ�����������������������������
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
//�Ƽ���
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
//������
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

//����ݽ���Ʒ��
class MissionSubmitList: public BaseItemList
{
public:
	static const U32 MAXSLOT = 12;
	MissionSubmitList();
	bool UpdateToClient(GameConnection* conn, S32 index, U32 flag);
};

extern ItemManager* g_ItemManager;

#endif//__PLAYER_ITEM_H__