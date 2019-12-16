//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/Item/ItemBaseData.h"
#include "Gameplay/Item/Res.h"
#include "core/stringTable.h"
#include "Gameplay/Data/readDataFile.h"
#include "core/tVector.h"
#include "Util/ColumnData.h"
#include "Gameplay/Item/IndividualStall.h"

ItemRepository gItemRepository;
ItemRepository* g_ItemRepository = NULL;

// ----------------------------------------------------------------------------
// 判定是否任务使用物品
bool ItemBaseData::isTaskItem()
{ 
	return mBaseLimit & Res::ITEM_MISSION;
}

// ----------------------------------------------------------------------------
// 判断物品是否拥有唯一性
bool ItemBaseData::isOnlyOne()
{ 
	return mBaseLimit & Res::ITEM_PICKUPONLY;
}

// ----------------------------------------------------------------------------
// 判断是否武器
bool ItemBaseData::isWeapon()
{
	return mCategory == Res::CATEGORY_WEAPON;
}

// ----------------------------------------------------------------------------
// 判断是否法宝
bool ItemBaseData::isTrump()
{
	return mCategory == Res::CATEGORY_TRUMP;
}

// 判断是否防具
bool ItemBaseData::isEquipment()
{
	return mCategory == Res::CATEGORY_EQUIPMENT;
}

// 判断是否饰品
bool ItemBaseData::isOrnament()
{
	return mCategory == Res::CATEGORY_ORNAMENT;
}

// ----------------------------------------------------------------------------
// 判断是否除武器外的装备物品
bool ItemBaseData::isBody()
{
	return (isEquipment() || isOrnament());
}

// ----------------------------------------------------------------------------
// 判断是否背包物品
bool ItemBaseData::isBagItem()
{
	return mCategory == Res::CATEGORY_PACK;
}

// ----------------------------------------------------------------------------
// 判断是否装备物品（包括武器，饰品，衣服，头，手,坐骑,背包，采集工具）
bool ItemBaseData::isEquip()
{
	return (isWeapon() || isBody() || isTrump() || isRide() || isBagItem() || isGather());
}

// ----------------------------------------------------------------------------
// 判断是否坐骑物品
bool ItemBaseData::isRide()
{
	return mSubCategory == Res::CATEGORY_RIDE;
}

// ----------------------------------------------------------------------------
// 判断是否宠物道具
bool ItemBaseData::isPet()
{
	return mCategory == Res::CATEGORY_PET;
}

// ----------------------------------------------------------------------------
// 判断是否宠物道具
bool ItemBaseData::isPetEquip()
{
	return mCategory == Res::CATEGORY_PETEQUIP;
}

// 判断是否宠物封印
bool ItemBaseData::isPetEgg()
{
	return mCategory == Res::CATEGORY_PETEGG;
}

// 判断是否骑宠封印
bool ItemBaseData::isMountPetEgg()
{
	return mCategory == Res::CATEGORY_MOUNTPETEGG;
}

// ----------------------------------------------------------------------------
// 判断是否采集工具
bool ItemBaseData::isGather()
{
	return (mCategory == Res::CATEGORY_TOOLS) && (mSubCategory != Res::CATEGORY_MAGIC_BOTTLE);
}

// ----------------------------------------------------------------------------
// 判断是否宝石
bool ItemBaseData::isGem()
{
	return mCategory == Res::CATEGORY_GEM;
}

// ----------------------------------------------------------------------------
// 判断是否能叠加
bool ItemBaseData::canLapOver()
{
	return bool(mMaxOverNum > 1);
}

S32 ItemBaseData::getMaxOverNum()
{
	return mMaxOverNum;
}

S32 ItemBaseData::getReserve()
{
	return mReserve;
}

S32 ItemBaseData::getEquipStrengthenNum( S32 tempNum )
{
	return mEquipStrengthen[tempNum];
}

S32 ItemBaseData::getmEquipStrengthenLevel( S32 levelNum )
{
	return mEquipStrengthenLevel[levelNum][0];
}

bool ItemBaseData::canSexLimit(U32 sex)
{
	if(mSexLimit == 0 || mSexLimit == sex)
		return true;
	return false;
}

bool ItemBaseData::canFamilyLimit(U32 famliy)
{
	return  mFamilyLimit ? mFamilyLimit & (BIT(famliy)) : false;
}

bool ItemBaseData::canLevelLimit(U32 level)
{
	if(mLevelLimit > level)
		return false;
	return true;
}

bool ItemBaseData::getMissionNeed(Vector<S32>& NeedMissionList)
{	
	if(!mMissionNeed || mMissionNeed[0] == 0)
		return false;
	char list[256];
	dStrcpy(list, sizeof(list), mMissionNeed);
	char * context = NULL;
	char *value = dStrtok(list, "/", &context);
	while (value)
	{
		NeedMissionList.push_back(dAtol(value));
		value = dStrtok(NULL, "/", &context);
	}
	return true;
}

S32 ItemBaseData::getmEquipStrengthenLevelId( S32 levelNum )
{
	return mEquipStrengthenLevel[levelNum][1];
}

bool ItemBaseData::canBaseLimit(U32 flag)
{
	return mBaseLimit & flag;
}

ItemRepository::ItemRepository()
{
	g_ItemRepository = this;
}
ItemRepository::~ItemRepository()
{
	clear();
	if(mColumnData)
		delete mColumnData;
}

void ItemRepository::clear()
{
	for(ItemDataMap::iterator it = mDataMap.begin(); it != mDataMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	mDataMap.clear();
}


void ItemRepository::read()
{
	CDataFile op;
	RData tempdata;
	op.ReadDataInit();
	char filename[1024];
	Platform::makeFullPathName(GAME_ITEMDATA_FILE, filename, sizeof(filename));
	AssertRelease(op.readDataFile(filename), "can't read file : ItemRepository.dat!");
	mColumnData = new IColumnData(op.ColumNum, "ItemRepository.dat");
	mColumnData->setField(CalcOffset(mItemID,				ItemBaseData),	DType_U32,		"物品编号");
	mColumnData->setField(CalcOffset(mItemName,				ItemBaseData),	DType_string,	"道具名称");
	mColumnData->setField(CalcOffset(mCategory,				ItemBaseData),	DType_enum8,	"物品类别");
	mColumnData->setField(CalcOffset(mSubCategory,			ItemBaseData),	DType_enum16,	"物品子类别");
	mColumnData->setField(CalcOffset(mDescriptionID,		ItemBaseData),	DType_U32,		"物品描述");
	mColumnData->setField(CalcOffset(mHelpID,				ItemBaseData),	DType_U32,		"物品说明");
	mColumnData->setField(CalcOffset(mPurposeID,			ItemBaseData),	DType_string,	"使用用途文本");
	mColumnData->setField(CalcOffset(mIconName,				ItemBaseData),	DType_string,	"图标名");
	mColumnData->setField(CalcOffset(mPetInsight,			ItemBaseData),	DType_U16,		"宠物悟性");
	mColumnData->setField(CalcOffset(mPetTalent,			ItemBaseData),	DType_U16,		"宠物根骨");
	mColumnData->setField(CalcOffset(mSoundID,				ItemBaseData),	DType_string,	"音效编号");
	mColumnData->setField(CalcOffset(mTimeMode,				ItemBaseData),	DType_enum8,	"时效计算方式");
	mColumnData->setField(CalcOffset(mDuration,				ItemBaseData),	DType_U32,		"持续时间");
	mColumnData->setField(CalcOffset(mSaleType,				ItemBaseData),	DType_enum8,	"出售方式");
	mColumnData->setField(CalcOffset(mSalePrice,			ItemBaseData),	DType_U32,		"出售价格");
	mColumnData->setField(CalcOffset(mMaxOverNum,			ItemBaseData),	DType_U16,		"最大叠加数量");
	mColumnData->setField(CalcOffset(mReserve,				ItemBaseData),	DType_U32,		"备用字段");
	mColumnData->setField(CalcOffset(mMissionID,			ItemBaseData),	DType_U32,		"绑定任务编号");
	mColumnData->setField(CalcOffset(mMissionNeed,			ItemBaseData),	DType_string,	"需要物品的任务编号");
	mColumnData->setField(CalcOffset(mColorLevel,			ItemBaseData),	DType_enum8,	"物品颜色等级");
	mColumnData->setField(CalcOffset(mQualityLevel,			ItemBaseData),	DType_U16,		"品质等级");
	mColumnData->setField(CalcOffset(mDropRate,				ItemBaseData),	DType_U32,		"掉落几率");
	mColumnData->setField(CalcOffset(mBaseLimit,			ItemBaseData),	DType_U32,		"基本限制");
	mColumnData->setField(CalcOffset(mSexLimit,				ItemBaseData),	DType_U8,		"性别限制");
	mColumnData->setField(CalcOffset(mLevelLimit,			ItemBaseData),	DType_U8,		"等级限制");
	mColumnData->setField(CalcOffset(mFamilyLimit,			ItemBaseData),	DType_U16,		"门宗限制");
	mColumnData->setField(CalcOffset(mUseTimes,				ItemBaseData),	DType_U16,		"使用次数限制");
	mColumnData->setField(CalcOffset(mRenascences,			ItemBaseData),	DType_U16,		"转生次数限制使用");
	mColumnData->setField(CalcOffset(mCoolTimeType,			ItemBaseData),	DType_U16,		"物品冷却类型");
	mColumnData->setField(CalcOffset(mCoolTime,				ItemBaseData),	DType_U32,		"冷却时间");
	mColumnData->setField(CalcOffset(mVocalTime,			ItemBaseData),	DType_U32,		"吟唱时间");
	mColumnData->setField(CalcOffset(mIdentifyType,			ItemBaseData),	DType_enum8,	"鉴定类型");
	mColumnData->setField(CalcOffset(mRandomNameID,			ItemBaseData),	DType_U32,		"随机名称属性ID");
	mColumnData->setField(CalcOffset(mSuitID,				ItemBaseData),	DType_U32,		"套装编号");
	mColumnData->setField(CalcOffset(mBaseAttribute,		ItemBaseData),	DType_U32,		"装备主属性（状态ID)");
	for(int i=0; i<ItemBaseData::MAX_APPEND_ATTRIBUTE; ++i)
	{
		mColumnData->setField(CalcOffset(mAppendAttribute[i],		ItemBaseData),	DType_U8,		"附加属性");
	}
	
	mColumnData->setField(CalcOffset(mSkillAForEquip,		ItemBaseData),	DType_U32,		"装备后主动技能");
	mColumnData->setField(CalcOffset(mSkillBForEquip,		ItemBaseData),	DType_U32,		"装备后被动技能");
	mColumnData->setField(CalcOffset(mSkillForUse,			ItemBaseData),	DType_U32,		"使用物品时调用的技能");
	mColumnData->setField(CalcOffset(mMaxWear,				ItemBaseData),	DType_U32,		"最大耐久度");
	mColumnData->setField(CalcOffset(mMaxEnhanceWears,		ItemBaseData),	DType_U16,		"最大耐久强化次数");
	mColumnData->setField(CalcOffset(mBindMode,				ItemBaseData),	DType_enum8,	"物品绑定模式");
	mColumnData->setField(CalcOffset(mBindPro,				ItemBaseData),	DType_U32,		"灵魂绑定属性");
	mColumnData->setField(CalcOffset(mAllowEmbedSlot,		ItemBaseData),	DType_U32,		"允许镶嵌孔");
	mColumnData->setField(CalcOffset(mOpenedEmbedSlot,		ItemBaseData),	DType_U8,		"已开启的镶嵌孔");

	mColumnData->setField(CalcOffset(mEquipEffectID,		ItemBaseData),	DType_U32,		"装备特效ID");
	mColumnData->setField(CalcOffset(mEquipStrengthenEffectID,	ItemBaseData),	DType_U32,	"装备强化特效ID");
	mColumnData->setField(CalcOffset(mEquipStrengthenEffectLevel,	ItemBaseData),	DType_U8,	"装备强化出特效等级");
	mColumnData->setField(CalcOffset(mEquipStrengthenEffectNode,	ItemBaseData),	DType_U8,	"装备强化出特效装配点");
	mColumnData->setField(CalcOffset(mEquipStrengthenEffectModel,ItemBaseData),	DType_string,	"装备强化替换模型");

	for(int i = 0; i < ItemBaseData::MAX_EQUIPSTRENGTHEN_LEVEL; i++)
	{
		mColumnData->setField(CalcOffset(mEquipStrengthen[i],ItemBaseData),	DType_U32,		"装备强化属性");
		mColumnData->setField(CalcOffset(mEquipStrengthenSkin[i],ItemBaseData),	DType_string,"装备强化贴图");
	}

	for(int i = 0; i < ItemBaseData::MAX_EQUIPSTRENGTHEN_ADDLEVEL; i++)
	{
		mColumnData->setField(CalcOffset(mEquipStrengthenLevel[i][0],ItemBaseData),	DType_U8,	"装备强化特定等级");
		mColumnData->setField(CalcOffset(mEquipStrengthenLevel[i][1],ItemBaseData),	DType_U32,	"装备强化特定等级效果");
	}
	mColumnData->setField(CalcOffset(mMaleShapesSetId,		ItemBaseData),	DType_U32,		"模型信息(男性)");
	mColumnData->setField(CalcOffset(mFemaleShapesSetId,	ItemBaseData),	DType_U32,		"模型信息(女性)");
	mColumnData->setField(CalcOffset(mStoneLimit,			ItemBaseData),  DType_U32,		"宝石限制");
	mColumnData->setField(CalcOffset(mTrailEffect,			ItemBaseData),	DType_string,   "拖尾特效");

	for(int k = 0; k < op.RecordNum; ++k)
	{
		ItemBaseData* pItempdata = new ItemBaseData;
		for(int h = 0; h < op.ColumNum; ++h)
		{
			op.GetData(tempdata);
			mColumnData->setData(pItempdata, h, tempdata);
		}
		if (!insert(pItempdata) )
		{
			delete pItempdata;
			pItempdata = NULL;
		}
	}
	op.ReadDataClose();
}

ItemBaseData* ItemRepository::getItemData(U32 ItemID)
{
	ItemDataMap::iterator it = mDataMap.find(ItemID);
	if(it != mDataMap.end())
		return it->second;
	else
		return NULL;
}

U32 ItemBaseData::getBaseAttribute()
{
	return mBaseAttribute;
}

bool ItemRepository::insert(ItemBaseData* data)
{
	if(!data)
		return false;
	mDataMap.insert(ItemDataMap::value_type(data->mItemID, data));
	return true;
}

void ItemRepository::searchItemByName(StringTableEntry name, U32 type)
{
	ItemDataMap::iterator it = mDataMap.begin();	
	ItemBaseData* data = NULL;
	if(type == 1)// 精确查找
	{
		for(; it != mDataMap.end(); ++it)
		{
			data = it->second;
			if(data)
			{
				if(dStricmp(data->getItemName(), name) == 0)
				{
					if(data->canBaseLimit(Res::ITEM_TRADE) && data->canBaseLimit(Res::ITEM_SEARCH))
					{
						g_Stall->addToLookUpList(it->first);
						break;
					}
				}
			}
		}
	}
	else if(type == 2)// 模糊查找
	{
		for(; it != mDataMap.end(); ++it)
		{
			data = it->second;
			if(data)
			{
				StringTableEntry target = dStrstr(data->getItemName(), name);
				if(target != NULL)
				{
					if(data->canBaseLimit(Res::ITEM_TRADE) && data->canBaseLimit(Res::ITEM_SEARCH))
						g_Stall->addToLookUpList(it->first);
					else
						continue;
				}
			}
		}
	}
	
}

// ----------------------------------------------------------------------------
// 通过列数获取物品数据字段值
ConsoleFunction( getItemData, const char*, 3, 3, "GetItemData(%itemid, %col)")
{
	ItemBaseData* data = g_ItemRepository->getItemData(dAtol(argv[1]));
	if(data)
	{
		std::string to;
		g_ItemRepository->mColumnData->getData(data, dAtoi(argv[2]), to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;		
	}
	return false;
}


// ============================================================================
// 随机属性表
// ============================================================================

RandPropertyTable* g_RandPropertyTable;
RandPropertyTable gRandPropertyTable;

RandPropertyTable::RandPropertyTable()
{
	g_RandPropertyTable = this;
}

RandPropertyTable::~RandPropertyTable()
{
	PROPERTYMAP::iterator itb = mMap.begin();
	for(; itb != mMap.end(); ++itb)
	{
		if(itb->second)
			delete itb->second;
	}
	mMap.clear();
}

// ----------------------------------------------------------------------------
// 查表获取随机属性
// 参数说明：
// type			道具随机附加属性类别
// categoryid	道具子类类别编号
// needpronum	需要获得随机属性个数
// ProList		随机附加属性列表
void RandPropertyTable::getRandProperty(U32 type, U32 categoryid, U32 needpronum, Vector<U32>& ProList)
{
	// 根据装备类别获取装备位置标志
	U32 srcFlag = 0;
	{
		if(categoryid >= Res::CATEGORY_FAQI && categoryid <= Res::CATEGORY_ARMS)
			srcFlag = 1 << (categoryid - Res::CATEGORY_FAQI);
		else if(categoryid >= Res::CATEGORY_HEAD && categoryid <= Res::CATEGORY_FOOT)
			srcFlag	= 1 << (categoryid - Res::CATEGORY_HEAD + 7);
		else if(categoryid >= Res::CATEGORY_AMULET && categoryid <= Res::CATEGORY_RING)
			srcFlag	= 1 << (categoryid - Res::CATEGORY_AMULET + 14);
	}

	Vector<U32> tmpList;
	PROPERTYMAP::iterator itb = mMap.begin();
	for(; itb != mMap.end(); ++itb)
	{
		stPropertyEntry* pData = itb->second;
		if(pData && pData->type == type && pData->flag & srcFlag)
		{
			tmpList.push_back(pData->proID);
		}
	}

	if(tmpList.size() < needpronum)
		needpronum = tmpList.size();
	while(needpronum > 0)
	{
		U32 randValue = Platform::getRandomI(1, tmpList.size());
		ProList.push_back(tmpList[randValue-1]);
		tmpList.erase_fast(randValue-1);
		needpronum--;
	}
}

// ----------------------------------------------------------------------------
// 获取随机名称前缀
StringTableEntry RandPropertyTable::getPrefixName(U32 preID)
{
	PROPERTYMAP::iterator it = mMap.find(preID);
	if(it != mMap.end())
	{
		stPropertyEntry* pData = it->second;
		if(pData)
			return pData->preName;
	}
	return "";
}

stPropertyEntry* RandPropertyTable::GetData(U32 ProID)
{
	PROPERTYMAP::iterator it = mMap.find(ProID);
	if(it != mMap.end())
		return it->second;
	else
		return NULL;
}

bool RandPropertyTable::insert(stPropertyEntry* data)
{
	if(!data)
		return false;
	mMap.insert(PROPERTYMAP::value_type(data->proID, data));
	return true;
}

void RandPropertyTable::read()
{
	CDataFile op;
	RData tempdata;
	op.ReadDataInit();
	char filename[1024];
	Platform::makeFullPathName(GAME_RANDPROPERTY_FILE, filename, sizeof(filename));
	AssertRelease(op.readDataFile(filename), "can't read file : RandProperty.dat!");
	//U32 mainID = 0;
	for(int k = 0; k < op.RecordNum; ++k)
	{
		stPropertyEntry* pData = new stPropertyEntry;		
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U8, "RandProperty.dat read error: type");
		pData->type = tempdata.m_U8;
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U16, "RandProperty.dat read error: proID");
		pData->proID = tempdata.m_U16;
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "RandProperty.dat read error: flag");
		pData->flag = tempdata.m_U32;
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_string, "RandProperty.dat read error: preName");
		pData->preName = StringTable->insert(tempdata.m_string);

		insert(pData);
	}
	op.ReadDataClose();
}

void RandPropertyTable::clear()
{
	PROPERTYMAP::iterator itb = mMap.begin();
	for(; itb != mMap.end(); ++itb)
	{
		if(itb->second)
			delete itb->second;
	}
	mMap.clear();
}
