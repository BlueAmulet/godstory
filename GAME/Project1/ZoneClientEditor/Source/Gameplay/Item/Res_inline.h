#include "Gameplay/item/Res.h"

// ----------------------------------------------------------------------------
// 设置物品模板数据
inline bool Res::setBaseData(U32 itemID)
{
	mBaseData = g_ItemRepository->getItemData(itemID);
	return mBaseData ? true : false;
}

// ----------------------------------------------------------------------------
// 获取物品模板数据对象
inline ItemBaseData* Res::getBaseData()
{
	return mBaseData;
}

// ----------------------------------------------------------------------------
// 判断是否同一个RES资源
inline bool Res::isSameRes(Res* res)
{
	return (isSameBaseData(res) && isSameExt(res));
}

// ----------------------------------------------------------------------------
// 判断是否同一个物品模板数据
inline bool Res::isSameBaseData(Res* res)
{
	return res ? res->getBaseData() == mBaseData : false;
}

// ----------------------------------------------------------------------------
// 判断是否同一个物品的附加属性
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
// 获取物品ID
inline U32 Res::getItemID()
{
	return mBaseData ? mBaseData->mItemID : 0;
}

// ----------------------------------------------------------------------------
// 获取物品名称（如有随机附加名称，则需要连接加上）
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
// 获取物品图标名称
inline StringTableEntry Res::getIconName()
{
	return mBaseData ? mBaseData->mIconName : "";
}

// ----------------------------------------------------------------------------
// 获取物品大类ID
inline U32 Res::getCategory()
{
	return mBaseData ? mBaseData->mCategory : 0;
}

// ----------------------------------------------------------------------------
// 获取物品子类ID
inline U32 Res::getSubCategory()
{
	return mBaseData ? mBaseData->mSubCategory : 0;
}

// ----------------------------------------------------------------------------
// 获取物品模型名
inline U16 Res::getPetInsight()
{
	return mBaseData ? mBaseData->mPetInsight : 0;
}

// ----------------------------------------------------------------------------
// 获取物品贴图名
inline U16 Res::getPetTalent()
{
	return mBaseData ? mBaseData->mPetTalent : 0;
}

// ----------------------------------------------------------------------------
// 获取物品描述内容
inline StringTableEntry Res::getDescription()
{
	return mBaseData ? GetLocalStr(mBaseData->mDescriptionID) : "";
}

// ----------------------------------------------------------------------------
// 获取物品说明内容
inline StringTableEntry Res::getHelp()
{
	return mBaseData ? GetLocalStr(mBaseData->mHelpID) : "";
}

// ----------------------------------------------------------------------------
// 获取物品用途内容
inline StringTableEntry Res::getPurpose()
{
	return mBaseData ? mBaseData->mPurposeID : "";
}

// ----------------------------------------------------------------------------
// 获取时效计算方式
inline U32 Res::getTimeMode()
{
	return mBaseData ? mBaseData->mTimeMode : 0;
}

// ----------------------------------------------------------------------------
// 获取时效持续时间
inline U32 Res::getDuration()
{
	return mBaseData ? mBaseData->mDuration : 0;
}

// ----------------------------------------------------------------------------
// 获取物品最大叠加数
inline U32 Res::getMaxOverNum()
{
	return mBaseData ? mBaseData->mMaxOverNum : 1;
}

// ----------------------------------------------------------------------------
// 获取物品卖出方式
inline S32 Res::getSaleType()
{
	return mBaseData ? mBaseData->mSaleType : 0;
}

// ----------------------------------------------------------------------------
// 获取物品卖出价格
inline S32 Res::getSalePrice()
{
	return mBaseData ? mBaseData->mSalePrice : 0;
}

// ----------------------------------------------------------------------------
// 获取物品音效ID
inline StringTableEntry Res::getSoundID()
{
	return mBaseData ? mBaseData->mSoundID : "";
}

// ----------------------------------------------------------------------------
// 获取绑定类型
inline U32 Res::getBindMode()
{
	return mBaseData ? mBaseData->mBindMode : 0;
}

// ----------------------------------------------------------------------------
// 获取物品掉落几率
inline U32 Res::getDropRate()
{
	return mBaseData ? mBaseData->mDropRate : 0;
}

// ----------------------------------------------------------------------------
// 获取物品套装编号
inline U32 Res::getSuitID()
{
	return mBaseData ? mBaseData->mSuitID : 0;
}

// ----------------------------------------------------------------------------
// 获取绑定的任务编号
inline U32 Res::getMissionID()
{
	return mBaseData ? mBaseData->mMissionID : 0;
}

// ----------------------------------------------------------------------------
// 获取物品颜色等级
inline U32 Res::getFamilyCredit()
{
	return mBaseData ? mBaseData->mFamilyLimit : 0;
}

// ----------------------------------------------------------------------------
// 获取物品颜色等级
inline U32 Res::getColorLevel()
{
	return mBaseData ? mBaseData->mColorLevel : 1;
}

// ----------------------------------------------------------------------------
// 获取物品品质等级
inline U32 Res::getQualityLevel()
{
	return mExtData ? mExtData->Quality : mBaseData ? mBaseData->mQualityLevel : 0;
}

// ----------------------------------------------------------------------------
// 获取物品已开启的镶嵌孔
inline U32 Res::getOpenedEmbedSlot()
{
	return mBaseData ? mBaseData->mOpenedEmbedSlot : 0;
}

// ----------------------------------------------------------------------------
// 获取物品允许开启的镶嵌孔
inline U32 Res::getAllowEmbedSlot()
{
	return mBaseData ? mBaseData->mAllowEmbedSlot : 0;
}

// ----------------------------------------------------------------------------
// 获取物品装备特效ID
inline U32 Res::getEquipEffectID()
{
	return mBaseData ? ((mBaseData->mEquipStrengthenEffectID && getEquipStrengthens() >= mBaseData->mEquipStrengthenEffectLevel) ? mBaseData->mEquipStrengthenEffectID : mBaseData->mEquipEffectID) : 0;
}

// ----------------------------------------------------------------------------
// 获取物品强化特效ID
inline U32 Res::getEquipStrengthenEffectID()
{
	return (mBaseData && getEquipStrengthens() >= mBaseData->mEquipStrengthenEffectLevel) ? mBaseData->mEquipStrengthenEffectID : 0;
}

// ----------------------------------------------------------------------------
// 获取物品模板使用次数 
inline U32 Res::getResUsedTimes()
{
	return mBaseData ? mBaseData->mUseTimes : 0;
}

// ----------------------------------------------------------------------------
// 获取物品冷却类型
inline U32 Res::getCoolTimeType()
{
	if(mBaseData && mBaseData->mCoolTimeType)
		return mBaseData->mCoolTimeType;
	return 0xFFFFFFFF;//注：只有冷却类型在0-250之间才有效
}

// ----------------------------------------------------------------------------
// 是否有设置物品冷却
inline bool Res::isSetCoolTime()
{
	U32 uCoolTimeType = getCoolTimeType();
	return uCoolTimeType >= 0 && uCoolTimeType <= 250 && getCoolTime() != 0;
}

// 是否是技能书
inline bool Res::isSkillBook()
{
	if (!mBaseData)
		return false;
	return mBaseData->mCategory == CATEGORY_GOODS && mBaseData->mSubCategory == CATEGORY_SKILLBOOK;
}

// 是否是元神石
inline bool Res::isSpiritStone1()
{
	if (!mBaseData)
		return false;
	return mBaseData->mCategory == CATEGORY_GOODS && mBaseData->mSubCategory == CATEGORY_SPIRITSTONE1;
}

// 是否是元神玉
inline bool Res::isSpiritStone2()
{
	if (!mBaseData)
		return false;
	return mBaseData->mCategory == CATEGORY_GOODS && mBaseData->mSubCategory == CATEGORY_SPIRITSTONE2;
}

// ----------------------------------------------------------------------------
// 获取物品冷却时间
inline U32 Res::getCoolTime()
{
	return mBaseData ? mBaseData->mCoolTime : 0;
}

// ----------------------------------------------------------------------------
// 获取物品使用吟唱时间
inline U32 Res::getVocalTime()
{
	return mBaseData ? mBaseData->mVocalTime : 0;
}

// ----------------------------------------------------------------------------
// 获取限制使用转生次数
inline U32 Res::getRenascences()
{
	return mBaseData ? mBaseData->mRenascences : 0;
}


// ----------------------------------------------------------------------------
// 判断道具使用标记
inline bool Res::canBaseLimit(U32 flag)
{
	return mBaseData ? mBaseData->mBaseLimit & flag : false;
}

// ----------------------------------------------------------------------------
// 判定门宗限制标记
inline bool Res::canFamilyLimit(U32 famliy)
{
	return mBaseData ? mBaseData->mFamilyLimit & BIT(famliy) : false ;
}

// ----------------------------------------------------------------------------
// 判断物品等级限制
inline bool Res::canLevelLimit(U32 level)
{
	return mBaseData ? mBaseData->mLevelLimit <= level: false;
}

// ----------------------------------------------------------------------------
// 判定物品性别限制
inline bool Res::canSexLimit(U32 sex)
{
	if(mBaseData)
		return mBaseData->mSexLimit == 0 ? true : mBaseData->mSexLimit == sex;
	return false;
}

// ----------------------------------------------------------------------------
// 获取物品最大耐久度
inline U32 Res::getMaxWear()
{
	return mBaseData ? mBaseData->mMaxWear : 0;
}

// ----------------------------------------------------------------------------
// 获取物品最大耐久强化次数
inline U32 Res::getMaxEnhanceWear()
{
	return mBaseData ? mBaseData->mMaxEnhanceWears : 0;
}

// ----------------------------------------------------------------------------
// 判断是否武器
inline bool Res::isWeapon()
{
	return mBaseData ? mBaseData->isWeapon() : false;
}

// ----------------------------------------------------------------------------
// 判断是否是远程武器
inline bool Res::isRangeWeapon()
{
	return isWeapon() ? (getSubCategory() == CATEGORY_BOW) : false;
}

// ----------------------------------------------------------------------------
// 判断是否宝石
inline bool Res::isGem()
{
	return mBaseData ? mBaseData->isGem() : false;
}

// 判断是否防具
inline bool Res::isEquipment()
{
	return mBaseData ? mBaseData->isEquipment() : false;
}

// ----------------------------------------------------------------------------
// 判断是否饰品
inline bool Res::isOrnament()
{
	return mBaseData ? mBaseData->isOrnament() : false;
}

// ----------------------------------------------------------------------------
// 判断是否法宝
inline bool Res::isTrump()
{
	return mBaseData ? mBaseData->isTrump() : false;
}

// ----------------------------------------------------------------------------
// 判断是否除武器外的装备物品
inline bool Res::isBody()
{
	return mBaseData ? mBaseData->isBody() : false;
}

// ----------------------------------------------------------------------------
// 判断是否坐骑物品
inline bool Res::isRide()
{
	return mBaseData ? mBaseData->isRide() : false;
}

// ----------------------------------------------------------------------------
// 判断是否宠物道具
inline bool Res::isPet()
{
	return mBaseData ? mBaseData->isPet() : false;
}

// ----------------------------------------------------------------------------
// 判断是否宠物装备
inline bool Res::isPetEquip()
{
	return mBaseData ? mBaseData->isPetEquip() : false;
}

// ----------------------------------------------------------------------------
// 判断是否宠物封印
inline bool Res::isPetEgg()
{
	return mBaseData ? mBaseData->isPetEgg() : false;
}

// ----------------------------------------------------------------------------
// 判断是否骑宠封印
inline bool Res::isMountPetEgg()
{
	return mBaseData ? mBaseData->isMountPetEgg() : false;
}

// ----------------------------------------------------------------------------
// 判断是否采集工具
inline bool Res::isGather()
{
	return mBaseData ? mBaseData->isGather() : false;
}

// ----------------------------------------------------------------------------
// 判断是否采集工具
inline bool Res::isMaterial()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_MATERIAL || mBaseData->mCategory == CATEGORY_ONADD) : false;
}

// 判断是否还童丹
inline bool Res::isHuanTongDan()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_SCROLL && mBaseData->mSubCategory == CATEGORY_BECOMEBABY) : false;
}

// ----------------------------------------------------------------------------
// 判断是否装备物品（包括武器，饰品，衣服，头，手,坐骑等等）
inline bool Res::isEquip()
{
	return mBaseData ? mBaseData->isEquip() : false;
}

// ----------------------------------------------------------------------------
// 判断是否能重叠
inline bool Res::canLapOver()
{
	return mBaseData ? bool(mBaseData->mMaxOverNum > 1) : false;
}

// ----------------------------------------------------------------------------
// 判断是否能鉴定
inline bool Res::canIdentify(U32 identify)
{
	return mBaseData ? mBaseData->mIdentifyType == identify : false;
}

// ----------------------------------------------------------------------------
// 判断是否通过鉴定（若不需要鉴定返回真）
inline bool Res::hasIdentified()
{
	if ( canIdentify(Res::IDENTIFYTYPE_NONE) || IsActivatePro(EAPF_ATTACH))
		return true;
	return false;
}

// ----------------------------------------------------------------------------
// 判断是否是任务物品
inline bool Res::isTaskItem()
{
	return mBaseData ? mBaseData->isTaskItem() : false;
}

// ----------------------------------------------------------------------------
// 判断是否唯一拥有
inline bool Res::isOnlyOne()
{
	return mBaseData ? mBaseData->isOnlyOne() : false;
}

// ----------------------------------------------------------------------------
// 判断是否为根骨丹
inline bool Res::isGenGuDan()
{
	return (getCategory() == CATEGORY_ONADD && getSubCategory() == CATEGORY_ITEM_GENGUDAN);
}

// ----------------------------------------------------------------------------
// 判断是否背包物品
inline bool Res::isBagItem()
{
	return mBaseData? mBaseData->mCategory == CATEGORY_PACK : false;
}

// ----------------------------------------------------------------------------
// 判断是否包裹栏背包
inline bool Res::isInventoryBag()
{
	return (getSubCategory() == CATEGORY_INVE_FOREVER_PACK || getSubCategory() == CATEGORY_INVE_TIME_PACK);
}

// ----------------------------------------------------------------------------
// 判断是否银行背包
inline bool Res::isBankBag()
{
	return (getSubCategory() == CATEGORY_BANK_FOREVER_PACK || getSubCategory() == CATEGORY_BANK_TIME_PACK);
}

// ----------------------------------------------------------------------------
// 判断是否摊位背包
inline bool Res::isStallBag()
{
	return (getSubCategory() == CATEGORY_STAL_FOREVER_PACK || getSubCategory() == CATEGORY_STAL_TIME_PACK);
}

// ----------------------------------------------------------------------------
// 判断是否摊位样式背包
inline bool Res::isStallStyleBag()
{
	return (getSubCategory() == CATEGORY_STAL_STYLE_PACK);
}

// ----------------------------------------------------------------------------
// 获得模型
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
// 获得装备后的主状态
inline U32 Res::getBaseAttribute()
{
	//mExtData ? mExtData->ImpressID : 
	return mBaseData ? mBaseData->mBaseAttribute : 0;
}

// ----------------------------------------------------------------------------
// 得到限制等级
inline U32 Res::getLimitLevel()
{
	return mBaseData ? mBaseData->mLevelLimit : 0;
}

// ----------------------------------------------------------------------------
// 是否有使用脚本
inline bool Res::hasUsedScript()
{
	return mBaseData ? mBaseData->mBaseLimit & ITEM_USESCRIPT : false;
}

// ----------------------------------------------------------------------------
// 是否有装备脚本
inline bool Res::hasEqupScript()
{
	return mBaseData ? mBaseData->mBaseLimit & ITEM_MOUNTSCRIPT : false;
}

// ----------------------------------------------------------------------------
// 是否有卸装脚本
inline bool Res::hasUnequpScript()
{
	return mBaseData ? mBaseData->mBaseLimit & ITEM_UNMOUNTSCRIPT : false;
}

// ----------------------------------------------------------------------------
// 获取物品使用技能的ID
inline U32 Res::getUseStateID()
{
	return mBaseData ? mBaseData->mSkillForUse : 0;
}

// ----------------------------------------------------------------------------
// 获得性别限制
inline U32 Res::getSexLimit()
{
	return mBaseData ? mBaseData->mSexLimit : 0;
}

// ----------------------------------------------------------------------------
// 获得鉴定类型
inline U32 Res::getIdentifyType()
{
	return mBaseData ? mBaseData->mIdentifyType : 0;
}

// ----------------------------------------------------------------------------
// 获得附加属性
inline U32 Res::getAppendAttribute(U32 index)
{
	return mBaseData ? mBaseData->mAppendAttribute[index] : 0;
}

//无需鉴定
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

//无需动态生成附加属性
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
//得到附加属性ID
inline U32 Res::getAppendStatsID(U32 index)
{
	U32 subIndex = getAppendAttribute(index);
	if(subIndex > 0)
		return 1;
}

// ----------------------------------------------------------------------------
//是否有随机名称
inline U32 Res::getRandomName()
{
	return mExtData ? mExtData->RandPropertyID : 0;
}

// ----------------------------------------------------------------------------
// 获取颜色等级品质值
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
// 获取物品名称前缀
StringTableEntry Res::getPrefixName()
{
	return mExtData ? g_RandPropertyTable->getPrefixName(mExtData->RandPropertyID) : "";
}

// ----------------------------------------------------------------------------
// 获取物品名称后缀
StringTableEntry Res::getPostfixName()
{
	S32 quality = 0;
	// 品质取值 ＝ 物品品质等级 - 使用等级限制 - 颜色取值
	if(mExtData && mBaseData)
		quality = mExtData->Quality - mBaseData->mLevelLimit - getColorValue();
	else if(mBaseData)
		quality = mBaseData->mQualityLevel - mBaseData->mLevelLimit - getColorValue();
			
	if(quality >= 0 && quality < 4)
		return "普通";
	else if(quality >= 4 && quality < 9)
		return "精良";
	else if(quality >= 9 && quality < 15)
		return "优秀";
	else if(quality >= 15 && quality < 22)
		return "完美";
	else if(quality >= 22 && quality < 30)
		return "传说";
	else if(quality >= 30)
		return "逆天";
	else
		return "";
}

//获取物品名称后缀颜色等级值
U32 Res::getPostfixQuality()
{
	S32 quality = 0;
	// 品质取值 ＝ 物品品质等级 - 使用等级限制 - 颜色取值
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
// 获取保留字段的值
inline S32 Res::getReserveValue()
{
	return mBaseData ? mBaseData->mReserve : 0;
}

// ----------------------------------------------------------------------------
// 获取拖尾特效的材质贴图文件
inline StringTableEntry Res::getTrailEffectTexture()
{
	return mBaseData ? mBaseData->mTrailEffect : "";
}

// ----------------------------------------------------------------------------
// 是否宠物食物
inline bool Res::isPetFood()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_GOODS &&
			mBaseData->mSubCategory == CATEGORY_PETFOOD) : false;
}

// ----------------------------------------------------------------------------
// 是否宠物材料
inline bool Res::isPetMedical()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_GOODS &&
				mBaseData->mSubCategory == CATEGORY_PETMEDICA) : false;
}

// ----------------------------------------------------------------------------
// 是否宠物玩具
inline bool Res::isPetToy()
{
	return mBaseData ? (mBaseData->mCategory == CATEGORY_GOODS && 
		mBaseData->mSubCategory == CATEGORY_PETTOY) : false;
}

// ----------------------------------------------------------------------------
// 获得宠物食物类型
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