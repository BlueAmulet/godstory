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
		MAX_ADROIT_LEVEL				= 9,								//��������ȵȼ�
		MAX_EMBED_SLOTS					= 8,								//�����Ƕ8����ʯ
		MAX_EQUIPSTRENGTHEN_LEVEL		= 15,								//װ��ǿ��������
		MAX_EQUIPSTRENGTHEN_ADDLEVEL	= 3,								//װ��ǿ���ض��ȼ��ĸ���
		MAX_LINKNODES					= 3,								//���ģ�����ӵ���
		MAX_APPEND_ATTRIBUTE			= 12,								//�����󸽼�����������
	};

	friend class Res;
	friend class ItemRepository;

	ItemBaseData() { dMemset(this, 0, sizeof(ItemBaseData));};
	~ItemBaseData() {};

// ============================================================================
// ��Ʒ����ģ�����ݳ�Ա
// ============================================================================
public:
	U32					mItemID;				//��Ʒ���
	StringTableEntry	mItemName;				//��������
	StringTableEntry	mIconName;				//ͼ����
	U32					mRandomNameID;			//�����������ID
	U32					mCategory;				//��Ʒ���
	U32					mSubCategory;			//��Ʒ�����
	U32					mPetInsight;			//��������
	U32					mPetTalent;				//�������
	U32					mDescriptionID;			//�����ַ���ID���ܱ����ַ������ݹ���LocalStringManage��
	U32					mHelpID;				//˵���ַ���ID���ܱ����ַ������ݹ���LocalStringManage��
	StringTableEntry	mPurposeID;				//��;�ַ���
	U32					mTimeMode;				//ʱЧ���㷽ʽ
	U32					mDuration;				//ʱЧ����ʱ��
	U32					mMaxOverNum;			//��������
	U32					mSaleType;				//��Ʒ������ʽ
	S32					mSalePrice;				//��Ʒ�����۸�
	StringTableEntry	mSoundID;				//��Ʒ��ЧID
	S32					mDropRate;				//��Ʒ���伸��
	U32					mIdentifyType;			//��������
	U32				    mSuitID;				//��װ���
	U32					mColorLevel;			//��ɫ�ȼ�����ɫ,��ɫ,��ɫ,��ɫ,��ɫ,��ɫ,��ɫ��
	U32					mQualityLevel;			//Ʒ�ʵȼ�
	U32					mUseTimes;				//ʹ�ô���
	U32					mCoolTimeType;			//��Ʒ��ȴ���ͣ���Ʒ����ȴ����ȡ��ʹ��ʱ�ļ��ܣ�
	U32					mCoolTime;				//��ȴʱ��
	U32					mVocalTime;				//����ʱ��
	S32					mReserve;				//�����ֶ�
	U32					mBaseLimit;				//��������(����/����npc/��ֿ�/����/жװ��ʧ�ȵ�)
	U32					mFamilyLimit;			//��������
	U32					mLevelLimit;			//�ȼ�����
	U32					mJobLimit;				//ְҵ����
	U32					mSexLimit;				//�Ա�����
	U32					mRenascences;			//ת����������ʹ��
	U32					mMissionID;				//��������
	StringTableEntry	mMissionNeed;			//������Ʒ��������


	U32					mBaseAttribute;									//װ��������(״̬ID)
	U32					mAppendAttribute[MAX_APPEND_ATTRIBUTE];			//װ��������������

	U32					mEquipStrengthen[MAX_EQUIPSTRENGTHEN_LEVEL];			//װ��ǿ������
	StringTableEntry	mEquipStrengthenSkin[MAX_EQUIPSTRENGTHEN_LEVEL];		//װ��ǿ����ͼ
	U32					mEquipStrengthenLevel[MAX_EQUIPSTRENGTHEN_ADDLEVEL][2];	//װ��ǿ���ض��ȼ���Ч��
	U32					mEquipEffectID;											//װ����ЧID
	U32					mEquipStrengthenEffectID;								//װ��ǿ����ЧID
	U32					mEquipStrengthenEffectLevel;							//װ��ǿ������Ч�ȼ�
	U32					mEquipStrengthenEffectNode;								//װ��ǿ������Чװ���
	StringTableEntry	mEquipStrengthenEffectModel;							//װ��ǿ������Ч�滻ģ��
	
	U32					mSkillAForEquip;		//װ������������
	U32					mSkillBForEquip;		//װ���󱻶�����
	U32					mSkillForUse;			//ʹ��ʱ���õļ���ID

	U32					mMaxWear;				//����;ö�
	U32					mMaxEnhanceWears;		//����;�ǿ������

	U32					mBindMode;				//��Ʒ��ģʽ
	U32					mBindPro;				//��������	

	U32					mAllowEmbedSlot;				//������Ƕ��(0xFFFFFFFF,ÿ4��bit����һ���ף����8��)
	U32					mOpenedEmbedSlot;				//�ѿ�������Ƕ��(������11111111,ÿ1��bit����һ���ף����8��)

	U32					mMaleShapesSetId;					//ģ����Ϣ(����)
	U32					mFemaleShapesSetId;					//ģ����Ϣ(Ů��)

	U32					mStoneLimit;						//��ʯ����
	StringTableEntry	mTrailEffect;						//��β��Ч
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
// ��Ʒ�����������������
struct stPropertyEntry
{
	U32 type;					// ����������(��Res::PropertyType)
	U32 proID;					// �����������ID
	U32 flag;					// װ��λ�ñ�־
	StringTableEntry preName;	// ����ǰ׺�ַ���
};

// ----------------------------------------------------------------------------
// ��Ʒ����������Ա�
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