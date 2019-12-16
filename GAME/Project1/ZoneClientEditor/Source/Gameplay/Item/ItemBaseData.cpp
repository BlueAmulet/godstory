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
// �ж��Ƿ�����ʹ����Ʒ
bool ItemBaseData::isTaskItem()
{ 
	return mBaseLimit & Res::ITEM_MISSION;
}

// ----------------------------------------------------------------------------
// �ж���Ʒ�Ƿ�ӵ��Ψһ��
bool ItemBaseData::isOnlyOne()
{ 
	return mBaseLimit & Res::ITEM_PICKUPONLY;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�����
bool ItemBaseData::isWeapon()
{
	return mCategory == Res::CATEGORY_WEAPON;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ񷨱�
bool ItemBaseData::isTrump()
{
	return mCategory == Res::CATEGORY_TRUMP;
}

// �ж��Ƿ����
bool ItemBaseData::isEquipment()
{
	return mCategory == Res::CATEGORY_EQUIPMENT;
}

// �ж��Ƿ���Ʒ
bool ItemBaseData::isOrnament()
{
	return mCategory == Res::CATEGORY_ORNAMENT;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ���������װ����Ʒ
bool ItemBaseData::isBody()
{
	return (isEquipment() || isOrnament());
}

// ----------------------------------------------------------------------------
// �ж��Ƿ񱳰���Ʒ
bool ItemBaseData::isBagItem()
{
	return mCategory == Res::CATEGORY_PACK;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�װ����Ʒ��������������Ʒ���·���ͷ����,����,�������ɼ����ߣ�
bool ItemBaseData::isEquip()
{
	return (isWeapon() || isBody() || isTrump() || isRide() || isBagItem() || isGather());
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�������Ʒ
bool ItemBaseData::isRide()
{
	return mSubCategory == Res::CATEGORY_RIDE;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�������
bool ItemBaseData::isPet()
{
	return mCategory == Res::CATEGORY_PET;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�������
bool ItemBaseData::isPetEquip()
{
	return mCategory == Res::CATEGORY_PETEQUIP;
}

// �ж��Ƿ�����ӡ
bool ItemBaseData::isPetEgg()
{
	return mCategory == Res::CATEGORY_PETEGG;
}

// �ж��Ƿ�����ӡ
bool ItemBaseData::isMountPetEgg()
{
	return mCategory == Res::CATEGORY_MOUNTPETEGG;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ɼ�����
bool ItemBaseData::isGather()
{
	return (mCategory == Res::CATEGORY_TOOLS) && (mSubCategory != Res::CATEGORY_MAGIC_BOTTLE);
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ʯ
bool ItemBaseData::isGem()
{
	return mCategory == Res::CATEGORY_GEM;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ��ܵ���
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
	mColumnData->setField(CalcOffset(mItemID,				ItemBaseData),	DType_U32,		"��Ʒ���");
	mColumnData->setField(CalcOffset(mItemName,				ItemBaseData),	DType_string,	"��������");
	mColumnData->setField(CalcOffset(mCategory,				ItemBaseData),	DType_enum8,	"��Ʒ���");
	mColumnData->setField(CalcOffset(mSubCategory,			ItemBaseData),	DType_enum16,	"��Ʒ�����");
	mColumnData->setField(CalcOffset(mDescriptionID,		ItemBaseData),	DType_U32,		"��Ʒ����");
	mColumnData->setField(CalcOffset(mHelpID,				ItemBaseData),	DType_U32,		"��Ʒ˵��");
	mColumnData->setField(CalcOffset(mPurposeID,			ItemBaseData),	DType_string,	"ʹ����;�ı�");
	mColumnData->setField(CalcOffset(mIconName,				ItemBaseData),	DType_string,	"ͼ����");
	mColumnData->setField(CalcOffset(mPetInsight,			ItemBaseData),	DType_U16,		"��������");
	mColumnData->setField(CalcOffset(mPetTalent,			ItemBaseData),	DType_U16,		"�������");
	mColumnData->setField(CalcOffset(mSoundID,				ItemBaseData),	DType_string,	"��Ч���");
	mColumnData->setField(CalcOffset(mTimeMode,				ItemBaseData),	DType_enum8,	"ʱЧ���㷽ʽ");
	mColumnData->setField(CalcOffset(mDuration,				ItemBaseData),	DType_U32,		"����ʱ��");
	mColumnData->setField(CalcOffset(mSaleType,				ItemBaseData),	DType_enum8,	"���۷�ʽ");
	mColumnData->setField(CalcOffset(mSalePrice,			ItemBaseData),	DType_U32,		"���ۼ۸�");
	mColumnData->setField(CalcOffset(mMaxOverNum,			ItemBaseData),	DType_U16,		"����������");
	mColumnData->setField(CalcOffset(mReserve,				ItemBaseData),	DType_U32,		"�����ֶ�");
	mColumnData->setField(CalcOffset(mMissionID,			ItemBaseData),	DType_U32,		"��������");
	mColumnData->setField(CalcOffset(mMissionNeed,			ItemBaseData),	DType_string,	"��Ҫ��Ʒ��������");
	mColumnData->setField(CalcOffset(mColorLevel,			ItemBaseData),	DType_enum8,	"��Ʒ��ɫ�ȼ�");
	mColumnData->setField(CalcOffset(mQualityLevel,			ItemBaseData),	DType_U16,		"Ʒ�ʵȼ�");
	mColumnData->setField(CalcOffset(mDropRate,				ItemBaseData),	DType_U32,		"���伸��");
	mColumnData->setField(CalcOffset(mBaseLimit,			ItemBaseData),	DType_U32,		"��������");
	mColumnData->setField(CalcOffset(mSexLimit,				ItemBaseData),	DType_U8,		"�Ա�����");
	mColumnData->setField(CalcOffset(mLevelLimit,			ItemBaseData),	DType_U8,		"�ȼ�����");
	mColumnData->setField(CalcOffset(mFamilyLimit,			ItemBaseData),	DType_U16,		"��������");
	mColumnData->setField(CalcOffset(mUseTimes,				ItemBaseData),	DType_U16,		"ʹ�ô�������");
	mColumnData->setField(CalcOffset(mRenascences,			ItemBaseData),	DType_U16,		"ת����������ʹ��");
	mColumnData->setField(CalcOffset(mCoolTimeType,			ItemBaseData),	DType_U16,		"��Ʒ��ȴ����");
	mColumnData->setField(CalcOffset(mCoolTime,				ItemBaseData),	DType_U32,		"��ȴʱ��");
	mColumnData->setField(CalcOffset(mVocalTime,			ItemBaseData),	DType_U32,		"����ʱ��");
	mColumnData->setField(CalcOffset(mIdentifyType,			ItemBaseData),	DType_enum8,	"��������");
	mColumnData->setField(CalcOffset(mRandomNameID,			ItemBaseData),	DType_U32,		"�����������ID");
	mColumnData->setField(CalcOffset(mSuitID,				ItemBaseData),	DType_U32,		"��װ���");
	mColumnData->setField(CalcOffset(mBaseAttribute,		ItemBaseData),	DType_U32,		"װ�������ԣ�״̬ID)");
	for(int i=0; i<ItemBaseData::MAX_APPEND_ATTRIBUTE; ++i)
	{
		mColumnData->setField(CalcOffset(mAppendAttribute[i],		ItemBaseData),	DType_U8,		"��������");
	}
	
	mColumnData->setField(CalcOffset(mSkillAForEquip,		ItemBaseData),	DType_U32,		"װ������������");
	mColumnData->setField(CalcOffset(mSkillBForEquip,		ItemBaseData),	DType_U32,		"װ���󱻶�����");
	mColumnData->setField(CalcOffset(mSkillForUse,			ItemBaseData),	DType_U32,		"ʹ����Ʒʱ���õļ���");
	mColumnData->setField(CalcOffset(mMaxWear,				ItemBaseData),	DType_U32,		"����;ö�");
	mColumnData->setField(CalcOffset(mMaxEnhanceWears,		ItemBaseData),	DType_U16,		"����;�ǿ������");
	mColumnData->setField(CalcOffset(mBindMode,				ItemBaseData),	DType_enum8,	"��Ʒ��ģʽ");
	mColumnData->setField(CalcOffset(mBindPro,				ItemBaseData),	DType_U32,		"��������");
	mColumnData->setField(CalcOffset(mAllowEmbedSlot,		ItemBaseData),	DType_U32,		"������Ƕ��");
	mColumnData->setField(CalcOffset(mOpenedEmbedSlot,		ItemBaseData),	DType_U8,		"�ѿ�������Ƕ��");

	mColumnData->setField(CalcOffset(mEquipEffectID,		ItemBaseData),	DType_U32,		"װ����ЧID");
	mColumnData->setField(CalcOffset(mEquipStrengthenEffectID,	ItemBaseData),	DType_U32,	"װ��ǿ����ЧID");
	mColumnData->setField(CalcOffset(mEquipStrengthenEffectLevel,	ItemBaseData),	DType_U8,	"װ��ǿ������Ч�ȼ�");
	mColumnData->setField(CalcOffset(mEquipStrengthenEffectNode,	ItemBaseData),	DType_U8,	"װ��ǿ������Чװ���");
	mColumnData->setField(CalcOffset(mEquipStrengthenEffectModel,ItemBaseData),	DType_string,	"װ��ǿ���滻ģ��");

	for(int i = 0; i < ItemBaseData::MAX_EQUIPSTRENGTHEN_LEVEL; i++)
	{
		mColumnData->setField(CalcOffset(mEquipStrengthen[i],ItemBaseData),	DType_U32,		"װ��ǿ������");
		mColumnData->setField(CalcOffset(mEquipStrengthenSkin[i],ItemBaseData),	DType_string,"װ��ǿ����ͼ");
	}

	for(int i = 0; i < ItemBaseData::MAX_EQUIPSTRENGTHEN_ADDLEVEL; i++)
	{
		mColumnData->setField(CalcOffset(mEquipStrengthenLevel[i][0],ItemBaseData),	DType_U8,	"װ��ǿ���ض��ȼ�");
		mColumnData->setField(CalcOffset(mEquipStrengthenLevel[i][1],ItemBaseData),	DType_U32,	"װ��ǿ���ض��ȼ�Ч��");
	}
	mColumnData->setField(CalcOffset(mMaleShapesSetId,		ItemBaseData),	DType_U32,		"ģ����Ϣ(����)");
	mColumnData->setField(CalcOffset(mFemaleShapesSetId,	ItemBaseData),	DType_U32,		"ģ����Ϣ(Ů��)");
	mColumnData->setField(CalcOffset(mStoneLimit,			ItemBaseData),  DType_U32,		"��ʯ����");
	mColumnData->setField(CalcOffset(mTrailEffect,			ItemBaseData),	DType_string,   "��β��Ч");

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
	if(type == 1)// ��ȷ����
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
	else if(type == 2)// ģ������
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
// ͨ��������ȡ��Ʒ�����ֶ�ֵ
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
// ������Ա�
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
// ����ȡ�������
// ����˵����
// type			������������������
// categoryid	�������������
// needpronum	��Ҫ���������Ը���
// ProList		������������б�
void RandPropertyTable::getRandProperty(U32 type, U32 categoryid, U32 needpronum, Vector<U32>& ProList)
{
	// ����װ������ȡװ��λ�ñ�־
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
// ��ȡ�������ǰ׺
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
