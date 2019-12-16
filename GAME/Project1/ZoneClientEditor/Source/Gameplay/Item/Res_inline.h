#include "Gameplay/item/Res.h"

// ----------------------------------------------------------------------------
// ������Ʒģ������
inline bool Res::setBaseData(U32 itemID)
{
	mBaseData = g_ItemRepository->getItemData(itemID);
	return mBaseData ? true : false;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒģ�����ݶ���
inline ItemBaseData* Res::getBaseData()
{
	return mBaseData;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ͬһ��RES��Դ
inline bool Res::isSameRes(Res* res)
{
	return (isSameBaseData(res) && isSameExt(res));
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ͬһ����Ʒģ������
inline bool Res::isSameBaseData(Res* res)
{
	return res ? res->getBaseData() == mBaseData : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ͬһ����Ʒ�ĸ�������
inline bool Res::isSameExt(Res* res)
{
	if(!res)
		return false;
	stItemInfo* info = res->getExtData();
	if(!mExtData && !info)
		return true;
	if((!info && mExtData) || (info && !mExtData))
		return false;
	
	if(info->RemainUseTimes != mExtData->RemainUseTimes)
		return false;
	if(info->BindPlayer != mExtData->BindPlayer)
		return false;
	if(info->BindFriend != mExtData->BindFriend)
		return false;
	if(dStricmp(info->Producer, mExtData->Producer) != 0)
		return false;

	if(info->ActiveFlag == mExtData->ActiveFlag)
		return true;

	if(info->Quality != mExtData->Quality)
		return false;
	if(info->CurWear != mExtData->CurWear)
		return false;
	if(info->CurMaxWear != mExtData->CurMaxWear)
		return false;
	if(info->CurAdroit != mExtData->CurAdroit)
		return false;
	if(info->CurAdroitLv != mExtData->CurAdroitLv)
		return false;
	if(info->ActivatePro != mExtData->ActivatePro)
		return false;
	if(info->RandPropertyID != mExtData->RandPropertyID)
		return false;	
	if(info->IDEProNum != mExtData->IDEProNum)
		return false;
	for(S32 i = 0; i < info->IDEProNum; ++i)
	{
		if(info->IDEProValue[i] != mExtData->IDEProValue[i])
			return false;
	}
	if(info->EquipStrengthens != mExtData->EquipStrengthens)
		return false;
	for(S32 i = 0; i < MAX_EQUIPSTENGTHENS; ++i)
	{
		for(S32 j = 0; j < 2; ++j)
			if(info->EquipStrengthenValue[i][j] != mExtData->EquipStrengthenValue[i][j])
				return false;
	}
	if(info->ImpressID != mExtData->ImpressID)
		return false;
	if(info->BindProID != mExtData->BindProID)
		return false;
	if(info->WuXingID != mExtData->WuXingID)
		return false;
	if(info->WuXingLinkID != mExtData->WuXingLinkID)
		return false;
	if(info->WuXingPro != mExtData->WuXingPro)
		return false;
	if(info->SkillAForEquip != mExtData->SkillAForEquip)
		return false;
	if(info->SkillBForEquip != mExtData->SkillBForEquip)
		return false;
	if(info->EmbedOpened != mExtData->EmbedOpened)
		return false;
	for(S32 i = 0; i < MAX_EMBEDSLOTS; ++i)
	{
		if(info->EmbedSlot[i] != mExtData->EmbedSlot[i])
			return false;
	}
	return true;
}

// ----------------------------------------------------------------------------
// ��ȡ��ƷID
inline U32 Res::getItemID()
{
	return mBaseData ? mBaseData->mItemID : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ���ƣ���������������ƣ�����Ҫ���Ӽ��ϣ�
inline StringTableEntry Res::getItemName()
{
	if(!mBaseData)
		return "";
	if(getRandomName())
	{
		char szName[32] = {0};
		dSprintf(szName, sizeof(szName), "%s%s", getPrefixName(), mBaseData->mItemName);
		return StringTable->insert(szName);
	}
	else
		return mBaseData->mItemName;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒͼ������
inline StringTableEntry Res::getIconName()
{
	return mBaseData ? mBaseData->mIconName : "";
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ����ID
inline U32 Res::getCategory()
{
	return mBaseData ? mBaseData->mCategory : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ����ID
inline U32 Res::getSubCategory()
{
	return mBaseData ? mBaseData->mSubCategory : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒģ����
inline U16 Res::getPetInsight()
{
	return mBaseData ? mBaseData->mPetInsight : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ͼ��
inline U16 Res::getPetTalent()
{
	return mBaseData ? mBaseData->mPetTalent : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��������
inline StringTableEntry Res::getDescription()
{
	return mBaseData ? GetLocalStr(mBaseData->mDescriptionID) : "";
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ˵������
inline StringTableEntry Res::getHelp()
{
	return mBaseData ? GetLocalStr(mBaseData->mHelpID) : "";
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��;����
inline StringTableEntry Res::getPurpose()
{
	return mBaseData ? mBaseData->mPurposeID : "";
}

// ----------------------------------------------------------------------------
// ��ȡʱЧ���㷽ʽ
inline U32 Res::getTimeMode()
{
	return mBaseData ? mBaseData->mTimeMode : 0;
}

// ----------------------------------------------------------------------------
// ��ȡʱЧ����ʱ��
inline U32 Res::getDuration()
{
	return mBaseData ? mBaseData->mDuration : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��������
inline U32 Res::getMaxOverNum()
{
	return mBaseData ? mBaseData->mMaxOverNum : 1;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ������ʽ
inline S32 Res::getSaleType()
{
	return mBaseData ? mBaseData->mSaleType : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ�����۸�
inline S32 Res::getSalePrice()
{
	return mBaseData ? mBaseData->mSalePrice : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ЧID
inline StringTableEntry Res::getSoundID()
{
	return mBaseData ? mBaseData->mSoundID : "";
}

// ----------------------------------------------------------------------------
// ��ȡ������
inline U32 Res::getBindMode()
{
	return mBaseData ? mBaseData->mBindMode : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ���伸��
inline U32 Res::getDropRate()
{
	return mBaseData ? mBaseData->mDropRate : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��װ���
inline U32 Res::getSuitID()
{
	return mBaseData ? mBaseData->mSuitID : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ�󶨵�������
inline U32 Res::getMissionID()
{
	return mBaseData ? mBaseData->mMissionID : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ɫ�ȼ�
inline U32 Res::getFamilyCredit()
{
	return mBaseData ? mBaseData->mFamilyLimit : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ɫ�ȼ�
inline U32 Res::getColorLevel()
{
	return mBaseData ? mBaseData->mColorLevel : 1;
}

// ----------------------------------------------------------------------------
// ��ȡ��ƷƷ�ʵȼ�
inline U32 Res::getQualityLevel()
{
	return mExtData ? mExtData->Quality : mBaseData ? mBaseData->mQualityLevel : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ�ѿ�������Ƕ��
inline U32 Res::getOpenedEmbedSlot()
{
	return mBaseData ? mBaseData->mOpenedEmbedSlot : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ����������Ƕ��
inline U32 Res::getAllowEmbedSlot()
{
	return mBaseData ? mBaseData->mAllowEmbedSlot : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒװ����ЧID
inline U32 Res::getEquipEffectID()
{
	return mBaseData ? ((mBaseData->mEquipStrengthenEffectID && getEquipStrengthens() >= mBaseData->mEquipStrengthenEffectLevel) ? mBaseData->mEquipStrengthenEffectID : mBaseData->mEquipEffectID) : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒǿ����ЧID
inline U32 Res::getEquipStrengthenEffectID()
{
	return (mBaseData && getEquipStrengthens() >= mBaseData->mEquipStrengthenEffectLevel) ? mBaseData->mEquipStrengthenEffectID : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒģ��ʹ�ô��� 
inline U32 Res::getResUsedTimes()
{
	return mBaseData ? mBaseData->mUseTimes : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ȴ����
inline U32 Res::getCoolTimeType()
{
	if(mBaseData && mBaseData->mCoolTimeType)
		return mBaseData->mCoolTimeType;
	return 0xFFFFFFFF;//ע��ֻ����ȴ������0-250֮�����Ч
}

// ----------------------------------------------------------------------------
// �Ƿ���������Ʒ��ȴ
inline bool Res::isSetCoolTime()
{
	U32 uCoolTimeType = getCoolTimeType();
	return uCoolTimeType >= 0 && uCoolTimeType <= 250 && getCoolTime() != 0;
}

// �Ƿ��Ǽ�����
inline bool Res::isSkillBook()
{
	if (!mBaseData)
		return false;
	return mBaseData->mCategory == CATEGORY_GOODS && mBaseData->mSubCategory == CATEGORY_SKILLBOOK;
}

// �Ƿ���Ԫ��ʯ
inline bool Res::isSpiritStone1()
{
	if (!mBaseData)
		return false;
	return mBaseData->mCategory == CATEGORY_GOODS && mBaseData->mSubCategory == CATEGORY_SPIRITSTONE1;
}

// �Ƿ���Ԫ����
inline bool Res::isSpiritStone2()
{
	if (!mBaseData)
		return false;
	return mBaseData->mCategory == CATEGORY_GOODS && mBaseData->mSubCategory == CATEGORY_SPIRITSTONE2;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ȴʱ��
inline U32 Res::getCoolTime()
{
	return mBaseData ? mBaseData->mCoolTime : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒʹ������ʱ��
inline U32 Res::getVocalTime()
{
	return mBaseData ? mBaseData->mVocalTime : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ����ʹ��ת������
inline U32 Res::getRenascences()
{
	return mBaseData ? mBaseData->mRenascences : 0;
}


// ----------------------------------------------------------------------------
// �жϵ���ʹ�ñ��
inline bool Res::canBaseLimit(U32 flag)
{
	return mBaseData ? mBaseData->mBaseLimit & flag : false;
}

// ----------------------------------------------------------------------------
// �ж��������Ʊ��
inline bool Res::canFamilyLimit(U32 famliy)
{
	return mBaseData ? mBaseData->mFamilyLimit & BIT(famliy) : false ;
}

// ----------------------------------------------------------------------------
// �ж���Ʒ�ȼ�����
inline bool Res::canLevelLimit(U32 level)
{
	return mBaseData ? mBaseData->mLevelLimit <= level: false;
}

// ----------------------------------------------------------------------------
// �ж���Ʒ�Ա�����
inline bool Res::canSexLimit(U32 sex)
{
	if(mBaseData)
		return mBaseData->mSexLimit == 0 ? true : mBaseData->mSexLimit == sex;
	return false;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ����;ö�
inline U32 Res::getMaxWear()
{
	return mBaseData ? mBaseData->mMaxWear : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ����;�ǿ������
inline U32 Res::getMaxEnhanceWear()
{
	return mBaseData ? mBaseData->mMaxEnhanceWears : 0;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�����
inline bool Res::isWeapon()
{
	return mBaseData ? mBaseData->isWeapon() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ���Զ������
inline bool Res::isRangeWeapon()
{
	return isWeapon() ? (getSubCategory() == CATEGORY_BOW) : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ʯ
inline bool Res::isGem()
{
	return mBaseData ? mBaseData->isGem() : false;
}

// �ж��Ƿ����
inline bool Res::isEquipment()
{
	return mBaseData ? mBaseData->isEquipment() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ���Ʒ
inline bool Res::isOrnament()
{
	return mBaseData ? mBaseData->isOrnament() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ񷨱�
inline bool Res::isTrump()
{
	return mBaseData ? mBaseData->isTrump() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ���������װ����Ʒ
inline bool Res::isBody()
{
	return mBaseData ? mBaseData->isBody() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�������Ʒ
inline bool Res::isRide()
{
	return mBaseData ? mBaseData->isRide() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�������
inline bool Res::isPet()
{
	return mBaseData ? mBaseData->isPet() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ����װ��
inline bool Res::isPetEquip()
{
	return mBaseData ? mBaseData->isPetEquip() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�����ӡ
inline bool Res::isPetEgg()
{
	return mBaseData ? mBaseData->isPetEgg() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�����ӡ
inline bool Res::isMountPetEgg()
{
	return mBaseData ? mBaseData->isMountPetEgg() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ɼ�����
inline bool Res::isGather()
{
	return mBaseData ? mBaseData->isGather() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ɼ�����
inline bool Res::isMaterial()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_MATERIAL || mBaseData->mCategory == CATEGORY_ONADD) : false;
}

// �ж��Ƿ�ͯ��
inline bool Res::isHuanTongDan()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_SCROLL && mBaseData->mSubCategory == CATEGORY_BECOMEBABY) : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�װ����Ʒ��������������Ʒ���·���ͷ����,����ȵȣ�
inline bool Res::isEquip()
{
	return mBaseData ? mBaseData->isEquip() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ����ص�
inline bool Res::canLapOver()
{
	return mBaseData ? bool(mBaseData->mMaxOverNum > 1) : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ��ܼ���
inline bool Res::canIdentify(U32 identify)
{
	return mBaseData ? mBaseData->mIdentifyType == identify : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�ͨ��������������Ҫ���������棩
inline bool Res::hasIdentified()
{
	if ( canIdentify(Res::IDENTIFYTYPE_NONE) || IsActivatePro(EAPF_ATTACH))
		return true;
	return false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ���������Ʒ
inline bool Res::isTaskItem()
{
	return mBaseData ? mBaseData->isTaskItem() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�Ψһӵ��
inline bool Res::isOnlyOne()
{
	return mBaseData ? mBaseData->isOnlyOne() : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�Ϊ���ǵ�
inline bool Res::isGenGuDan()
{
	return (getCategory() == CATEGORY_ONADD && getSubCategory() == CATEGORY_ITEM_GENGUDAN);
}

// ----------------------------------------------------------------------------
// �ж��Ƿ񱳰���Ʒ
inline bool Res::isBagItem()
{
	return mBaseData? mBaseData->mCategory == CATEGORY_PACK : false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ����������
inline bool Res::isInventoryBag()
{
	return (getSubCategory() == CATEGORY_INVE_FOREVER_PACK || getSubCategory() == CATEGORY_INVE_TIME_PACK);
}

// ----------------------------------------------------------------------------
// �ж��Ƿ����б���
inline bool Res::isBankBag()
{
	return (getSubCategory() == CATEGORY_BANK_FOREVER_PACK || getSubCategory() == CATEGORY_BANK_TIME_PACK);
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�̯λ����
inline bool Res::isStallBag()
{
	return (getSubCategory() == CATEGORY_STAL_FOREVER_PACK || getSubCategory() == CATEGORY_STAL_TIME_PACK);
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�̯λ��ʽ����
inline bool Res::isStallStyleBag()
{
	return (getSubCategory() == CATEGORY_STAL_STYLE_PACK);
}

// ----------------------------------------------------------------------------
// ���ģ��
inline U32  Res::getShapesSetID(U32 sex)
{
	if(!mBaseData)
		return 0;
	if(sex == SEX_MALE)
		return mBaseData->mMaleShapesSetId;
	else
		return mBaseData->mFemaleShapesSetId;
}

// ----------------------------------------------------------------------------
// ���װ�������״̬
inline U32 Res::getBaseAttribute()
{
	//mExtData ? mExtData->ImpressID : 
	return mBaseData ? mBaseData->mBaseAttribute : 0;
}

// ----------------------------------------------------------------------------
// �õ����Ƶȼ�
inline U32 Res::getLimitLevel()
{
	return mBaseData ? mBaseData->mLevelLimit : 0;
}

// ----------------------------------------------------------------------------
// �Ƿ���ʹ�ýű�
inline bool Res::hasUsedScript()
{
	return mBaseData ? mBaseData->mBaseLimit & ITEM_USESCRIPT : false;
}

// ----------------------------------------------------------------------------
// �Ƿ���װ���ű�
inline bool Res::hasEqupScript()
{
	return mBaseData ? mBaseData->mBaseLimit & ITEM_MOUNTSCRIPT : false;
}

// ----------------------------------------------------------------------------
// �Ƿ���жװ�ű�
inline bool Res::hasUnequpScript()
{
	return mBaseData ? mBaseData->mBaseLimit & ITEM_UNMOUNTSCRIPT : false;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒʹ�ü��ܵ�ID
inline U32 Res::getUseStateID()
{
	return mBaseData ? mBaseData->mSkillForUse : 0;
}

// ----------------------------------------------------------------------------
// ����Ա�����
inline U32 Res::getSexLimit()
{
	return mBaseData ? mBaseData->mSexLimit : 0;
}

// ----------------------------------------------------------------------------
// ��ü�������
inline U32 Res::getIdentifyType()
{
	return mBaseData ? mBaseData->mIdentifyType : 0;
}

// ----------------------------------------------------------------------------
// ��ø�������
inline U32 Res::getAppendAttribute(U32 index)
{
	return mBaseData ? mBaseData->mAppendAttribute[index] : 0;
}

//�������
inline U32 Res::getAppendAttributeID(U32 index)
{
	U32 subId = getAppendAttribute(index);
	U32 statsId = 0;
	if(subId > 0)
		statsId = 302000000 + getAppendAttribute(index) * 10000 + getQualityLevel();
	else
		return 0;
	return statsId;
}

//���趯̬���ɸ�������
inline bool Res::isIdentify()
{
	for(int i=0; i<ItemBaseData::MAX_APPEND_ATTRIBUTE; ++i)
	{
		if(getAppendAttribute(i) > 0)
			return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
//�õ���������ID
inline U32 Res::getAppendStatsID(U32 index)
{
	U32 subIndex = getAppendAttribute(index);
	if(subIndex > 0)
		return 1;
}

// ----------------------------------------------------------------------------
//�Ƿ����������
inline U32 Res::getRandomName()
{
	return mExtData ? mExtData->RandPropertyID : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��ɫ�ȼ�Ʒ��ֵ
S32 Res::getColorValue()
{
	if(mBaseData)
	{
		switch(mBaseData->mColorLevel)
		{
		case Res::COLORLEVEL_WHITE:		return 0;
		case Res::COLORLEVEL_GREEN:		return 5;
		case Res::COLORLEVEL_BLUE :		return 11;
		case Res::COLORLEVEL_PURPLE:	return 18;
		case Res::COLORLEVEL_ORANGE:	return 36;
		}
	}	
	return 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ����ǰ׺
StringTableEntry Res::getPrefixName()
{
	return mExtData ? g_RandPropertyTable->getPrefixName(mExtData->RandPropertyID) : "";
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ���ƺ�׺
StringTableEntry Res::getPostfixName()
{
	S32 quality = 0;
	// Ʒ��ȡֵ �� ��ƷƷ�ʵȼ� - ʹ�õȼ����� - ��ɫȡֵ
	if(mExtData && mBaseData)
		quality = mExtData->Quality - mBaseData->mLevelLimit - getColorValue();
	else if(mBaseData)
		quality = mBaseData->mQualityLevel - mBaseData->mLevelLimit - getColorValue();
			
	if(quality >= 0 && quality < 4)
		return "��ͨ";
	else if(quality >= 4 && quality < 9)
		return "����";
	else if(quality >= 9 && quality < 15)
		return "����";
	else if(quality >= 15 && quality < 22)
		return "����";
	else if(quality >= 22 && quality < 30)
		return "��˵";
	else if(quality >= 30)
		return "����";
	else
		return "";
}

//��ȡ��Ʒ���ƺ�׺��ɫ�ȼ�ֵ
U32 Res::getPostfixQuality()
{
	S32 quality = 0;
	// Ʒ��ȡֵ �� ��ƷƷ�ʵȼ� - ʹ�õȼ����� - ��ɫȡֵ
	if(mExtData && mBaseData)
		quality = mExtData->Quality - mBaseData->mLevelLimit - getColorValue();
	else if(mBaseData)
		quality = mBaseData->mQualityLevel - mBaseData->mLevelLimit - getColorValue();

	if(quality >= 0 && quality < 4)
		return 1;
	else if(quality >= 4 && quality < 9)
		return 2;
	else if(quality >= 9 && quality < 15)
		return 3;
	else if(quality >= 15 && quality < 22)
		return 4;
	else if(quality >= 22 && quality < 30)
		return 5;
	else if(quality >= 30)
		return 6;

	return 0;
}

inline bool Res::getMissionNeed(Vector<S32>& NeedMissionList)
{
	return mBaseData ? mBaseData->getMissionNeed(NeedMissionList) : false;
}

// ----------------------------------------------------------------------------
// ��ȡ�����ֶε�ֵ
inline S32 Res::getReserveValue()
{
	return mBaseData ? mBaseData->mReserve : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��β��Ч�Ĳ�����ͼ�ļ�
inline StringTableEntry Res::getTrailEffectTexture()
{
	return mBaseData ? mBaseData->mTrailEffect : "";
}

// ----------------------------------------------------------------------------
// �Ƿ����ʳ��
inline bool Res::isPetFood()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_GOODS &&
			mBaseData->mSubCategory == CATEGORY_PETFOOD) : false;
}

// ----------------------------------------------------------------------------
// �Ƿ�������
inline bool Res::isPetMedical()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_GOODS &&
				mBaseData->mSubCategory == CATEGORY_PETMEDICA) : false;
}

// ----------------------------------------------------------------------------
// �Ƿ�������
inline bool Res::isPetToy()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_GOODS && 
		mBaseData->mSubCategory == CATEGORY_PETTOY) : false;
}

// ----------------------------------------------------------------------------
// ��ó���ʳ������
inline bool Res::getPetFoodType(S32 &nFoodType)
{
	if (!mBaseData || mBaseData->mCategory != CATEGORY_GOODS)
		return false;
	if (mBaseData->mSubCategory == CATEGORY_PETFOOD)
	{
		nFoodType = 1;
		return true;
	}
	else if (mBaseData->mSubCategory == CATEGORY_PETMEDICA)
	{
		nFoodType = 2;
		return true;
	}
	else if (mBaseData->mSubCategory == CATEGORY_PETTOY)
	{
		nFoodType = 3;
		return true;
	}
	else 
		return false;
}

inline char* Res::GetColorText(char *buff, U32 buffSize)
{
	if (!buff)
		return buff;

	switch (getColorLevel())
	{
	case Res::COLORLEVEL_GREY:
		{	
			dStrcpy(buff, buffSize, "0x9B9B9Bff");
		}
		break;
	case Res::COLORLEVEL_WHITE:
		{
			dStrcpy(buff, buffSize, "0xffffffff");
		}
		break;
	case Res::COLORLEVEL_GREEN:
		{
			dStrcpy(buff, buffSize, "0x00ff00ff");
		}
		break;
	case Res::COLORLEVEL_BLUE:
		{
			dStrcpy(buff, buffSize, "0x6464ffff");
		}
		break;
	case Res::COLORLEVEL_PURPLE:
		{
			dStrcpy(buff, buffSize, "0xa800ffff");
		}
		break;
	case Res::COLORLEVEL_ORANGE:
		{
			dStrcpy(buff, buffSize, "0xff9000ff");
		}
		break;
	case Res::COLORLEVEL_RED:
		{
			dStrcpy(buff, buffSize, "0xff0000ff");
		}
		break;
	default:
		{
			dStrcpy(buff, buffSize, "0x7700ffff");
		}
		break;
	}

	return buff;
}