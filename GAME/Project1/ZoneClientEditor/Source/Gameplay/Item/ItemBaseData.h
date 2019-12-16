//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//------------------------------------------------------------------------
#ifndef __ITEMBASEDATA_H__
#define __ITEMBASEDATA_H__

#pragma once
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#include <hash_map>

class Res;
class IColumnData;

class ItemBaseData
{
public:
	enum Contants
	{
		MAX_ADROIT_LEVEL				= 9,								//最大熟练度等级
		MAX_EMBED_SLOTS					= 8,								//最多镶嵌8个宝石
		MAX_EQUIPSTRENGTHEN_LEVEL		= 15,								//装备强化最多次数
		MAX_EQUIPSTRENGTHEN_ADDLEVEL	= 3,								//装备强化特定等级的个数
		MAX_LINKNODES					= 3,								//最多模型链接点数
		MAX_APPEND_ATTRIBUTE			= 12,								//鉴定后附加属性最多个数
	};

	friend class Res;
	friend class ItemRepository;

	ItemBaseData() { dMemset(this, 0, sizeof(ItemBaseData));};
	~ItemBaseData() {};

// ============================================================================
// 物品基本模板数据成员
// ============================================================================
public:
	U32					mItemID;				//物品编号
	StringTableEntry	mItemName;				//道具名称
	StringTableEntry	mIconName;				//图标名
	U32					mRandomNameID;			//随机名称属性ID
	U32					mCategory;				//物品类别
	U32					mSubCategory;			//物品子类别
	U32					mPetInsight;			//宠物悟性
	U32					mPetTalent;				//宠物根骨
	U32					mDescriptionID;			//描述字符串ID（受本地字符串数据管理LocalStringManage）
	U32					mHelpID;				//说明字符串ID（受本地字符串数据管理LocalStringManage）
	StringTableEntry	mPurposeID;				//用途字符串
	U32					mTimeMode;				//时效计算方式
	U32					mDuration;				//时效持续时间
	U32					mMaxOverNum;			//最大叠加数
	U32					mSaleType;				//物品卖出方式
	S32					mSalePrice;				//物品卖出价格
	StringTableEntry	mSoundID;				//物品音效ID
	S32					mDropRate;				//物品掉落几率
	U32					mIdentifyType;			//鉴定类型
	U32				    mSuitID;				//套装编号
	U32					mColorLevel;			//颜色等级（灰色,白色,绿色,蓝色,紫色,橙色,红色）
	U32					mQualityLevel;			//品质等级
	U32					mUseTimes;				//使用次数
	U32					mCoolTimeType;			//物品冷却类型（物品的冷却数据取自使用时的技能）
	U32					mCoolTime;				//冷却时间
	U32					mVocalTime;				//吟唱时间
	S32					mReserve;				//备用字段
	U32					mBaseLimit;				//基本限制(交易/出售npc/存仓库/修理/卸装消失等等)
	U32					mFamilyLimit;			//门宗限制
	U32					mLevelLimit;			//等级限制
	U32					mJobLimit;				//职业限制
	U32					mSexLimit;				//性别限制
	U32					mRenascences;			//转生次数限制使用
	U32					mMissionID;				//绑定任务编号
	StringTableEntry	mMissionNeed;			//需求物品的任务编号


	U32					mBaseAttribute;									//装备主属性(状态ID)
	U32					mAppendAttribute[MAX_APPEND_ATTRIBUTE];			//装备鉴定附加属性

	U32					mEquipStrengthen[MAX_EQUIPSTRENGTHEN_LEVEL];			//装备强化属性
	StringTableEntry	mEquipStrengthenSkin[MAX_EQUIPSTRENGTHEN_LEVEL];		//装备强化贴图
	U32					mEquipStrengthenLevel[MAX_EQUIPSTRENGTHEN_ADDLEVEL][2];	//装备强化特定等级及效果
	U32					mEquipEffectID;											//装备特效ID
	U32					mEquipStrengthenEffectID;								//装备强化特效ID
	U32					mEquipStrengthenEffectLevel;							//装备强化出特效等级
	U32					mEquipStrengthenEffectNode;								//装备强化出特效装配点
	StringTableEntry	mEquipStrengthenEffectModel;							//装备强化出特效替换模型
	
	U32					mSkillAForEquip;		//装备后主动技能
	U32					mSkillBForEquip;		//装备后被动技能
	U32					mSkillForUse;			//使用时调用的技能ID

	U32					mMaxWear;				//最大耐久度
	U32					mMaxEnhanceWears;		//最大耐久强化次数

	U32					mBindMode;				//物品绑定模式
	U32					mBindPro;				//灵魂绑定属性	

	U32					mAllowEmbedSlot;				//允许镶嵌孔(0xFFFFFFFF,每4个bit代表一个孔，最多8孔)
	U32					mOpenedEmbedSlot;				//已开启的镶嵌孔(二进制11111111,每1个bit代表一个孔，最多8孔)

	U32					mMaleShapesSetId;					//模型信息(男性)
	U32					mFemaleShapesSetId;					//模型信息(女性)

	U32					mStoneLimit;						//宝石限制
	StringTableEntry	mTrailEffect;						//拖尾特效
public:
	inline U32 getItemID()	{ return mItemID;}
    inline U32 getCategory() const { return mCategory;}
    inline U32 getSubCategory() const { return mSubCategory;}
	inline StringTableEntry getItemName() { return mItemName;}
	inline U32 getMissionID() { return mMissionID;}
	inline StringTableEntry getIconName() { return mIconName ? mIconName : "";}
	inline U8 getColorLevel(){return mColorLevel;}
	bool isWeapon();
	bool isTrump();
	bool isEquipment();
	bool isOrnament();
	bool isBody();
	bool isEquip();
	bool isBagItem();
	bool isRide();
	bool isPet();
	bool isPetEquip();
	bool isPetEgg();
	bool isMountPetEgg();
	bool isGather();	
	bool isGem();
	bool isTaskItem();
	bool isOnlyOne();
	bool canLapOver();
	S32 getLevelLimit() { return mLevelLimit;}
	S32 getSex() { return mSexLimit;}
	S32 getFamily() { return mFamilyLimit;}
	S32	getMaxOverNum();
	S32 getReserve();
	S32 getEquipStrengthenNum(S32 tempNum);
	S32 getmEquipStrengthenLevel(S32 levelNum);
	S32 getmEquipStrengthenLevelId(S32 levelNum);
	bool canSexLimit(U32 sex);
	bool canFamilyLimit(U32 famliy);
	bool canLevelLimit(U32 level);
	U32  getBaseAttribute();
	bool getMissionNeed(Vector<S32>& NeedMissionList);
	bool canBaseLimit(U32 flag);
};

class ItemRepository
{
public:	
	typedef stdext::hash_map<U32, ItemBaseData*> ItemDataMap;
	
	ItemRepository();
	~ItemRepository();
	void read();
	void clear();
	ItemBaseData* getItemData(U32 ItemID);
	bool insert(ItemBaseData* data);
	void searchItemByName(StringTableEntry name, U32 type);

	IColumnData* mColumnData;
public:
	ItemDataMap mDataMap;
};

extern ItemRepository* g_ItemRepository;


// ----------------------------------------------------------------------------
// 物品随机附加属性数据项
struct stPropertyEntry
{
	U32 type;					// 随机属性类别(见Res::PropertyType)
	U32 proID;					// 随机附加属性ID
	U32 flag;					// 装备位置标志
	StringTableEntry preName;	// 名称前缀字符串
};

// ----------------------------------------------------------------------------
// 物品随机附加属性表
class RandPropertyTable
{
public:
	typedef stdext::hash_map<U32, stPropertyEntry*> PROPERTYMAP;
	RandPropertyTable();
	~RandPropertyTable();

	void getRandProperty(U32 type, U32 categoryid, U32 needpronum, Vector<U32>& ProList);
	stPropertyEntry* GetData(U32 ProID);
	StringTableEntry getPrefixName(U32 preID);

	void read();
	bool insert(stPropertyEntry* data);
	void clear();
private:
	PROPERTYMAP mMap;
};

extern RandPropertyTable* g_RandPropertyTable;

#include "Gameplay/Item/ItemBaseData_inline.h"

#endif//__ITEMBASEDATA_H__