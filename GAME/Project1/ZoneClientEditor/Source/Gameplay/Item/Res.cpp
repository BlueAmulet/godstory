//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Item/Res.h"
#include "Common/PlayerStruct.h"
#include "gameplay/GameObjects/PlayerObject.h"

//物品颜色等级颜色值(RGBA)
U32 Res::sColorValue[] = 
{
	0x7700ffff,
	0X9B9B9BFF,
	0xffffffff,
	0x46ff15ff,
	0x187ddbff,
	0xca57ffff,
	0xff9600ff,
	0xff1200ff,
	0x7700ffff,
};

//宝石品质值
static U32 sGemQualityValue[10]=
{
	25,34,45,60,81,98,117,140,167,200,
};

//装备鉴定品质值
static U32 sItemQualityValue[10]=
{
	1,3,5,8,11,14,18,22,26,32,
};

static char* sCategory[][16] =
{
	{
		"法器",
		"单短",
		"双短",
		"琴",
		"弓",
		"旗杖",
		"刀斧",
	},
	{
		"头部",
		"身体",
		"背部",
		"肩部",
		"手部",
		"腰部",
		"脚部",
		"门派时装",
		"头部时装",
		"身体时装",
		"背部时装",
	},
	{
		"护身符",
		"颈部",
		"戒指",
	},
	{
		"坐骑",
		"宠物",
		"召唤",
	},
	{
		"食物",
		"药品",
		"补品",
		"绷带",
		"技能材料",
		"材料",
		"书籍",
		"技能书",
		"宝物",
		"物品",
		"宠物食物",
		"宠物丹药",
		"宠物玩具",
		"元神石",
		"元神玉",
	},
	{
		"草药",
		"矿物",
		"农作物",
		"木材",
		"鱼类",
		"兽肉",
		"奇珍",
		"皮革",
		"魔法材料",
		"纸",
	},
	{
		"采集工具",
		"挖矿工具",
		"耕种工具",
		"伐木工具",
		"钓鱼工具",
		"狩猎工具",
		"抓宠工具",
		"磨瓶",
		"铸造工具",
		"裁缝工具",
		"工艺工具",
		"符咒工具",
		"炼丹工具",
		"烹饪工具",
		"工匠工具",
		"蛊术工具",
	},
	{
		"任务",
		"任务物品"
	},
	{
		"铸造",
		"裁缝",
		"工艺",
		"符咒",
		"炼丹",
		"烹饪",
		"工匠",
		"蛊术",
		"其他",
	},
	{
		"红宝石",
		"绿宝石",
		"蓝宝石",
		"红晶石",
		"绿晶石",
		"蓝晶石",
		"五彩石",
		"青晶石",
		"紫晶石",
		"黄晶石",
		"青晶石",
		"紫晶石",
	},
	{
		"武器附魔",
		"防具附魔",
		"饰品附魔",
		"头盔附魔",
		"肩膀附魔",
		"披风附魔",
		"衣服附魔",
		"鞋子附魔",
		"护手附魔",
		"腰带附魔",
		"项链附魔",
		"戒指附魔",
		"护身附魔",
		"全部附魔",
		"法宝附魔",
	},
	{
		"物品栏永久包裹",
		"仓库栏永久包裹",
		"摆摊栏永久包裹",
		"物品栏时效包裹",
		"仓库栏时效包裹",
		"摆摊栏时效包裹",
		"摆摊栏样式包裹",
	},
	{
		"回城符",
		"记录卷轴",
		"图鉴",
		"卷轴",
		"鉴定符",
		"还童丹",
	},
	{
		"烟花",
		"嫁妆",
		"宠物装备",
		"元神装备",
		"筹码",
		"钥匙",
		"变身卡",
	},
	{
		"宠物头",
		"宠物身体",
		"宠物背",
		"宠物武器",
	},
	{
		"强化基础材料",
		"强化附加材料",
		"打孔材料",
		"摘取材料",
		"鉴定材料",
		"灵魂链接材料",
		"根骨丹",
		"法宝重铸材料",
		"法宝合成材料",
		"法宝强化材料",
	},
	{
		"法宝",
	},
	{
		"灵兽宝宝封印",
		"变异灵兽宝宝封印",
		"灵兽封印",
	    "变异灵兽封印",
	},
	{
		"骑宠封印",
	},	
};

Res::Res():mBaseData(NULL), mExtData(NULL)
{
}

Res::~Res()
{
	mBaseData = NULL;
	SAFE_DELETE(mExtData)
}

// ----------------------------------------------------------------------------
// 设置物品扩展数据
bool Res::setExtData(stItemInfo* info)
{
	if(mExtData == NULL && info)
	{
		mExtData = new stItemInfo;
		dMemcpy(mExtData, info, sizeof(stItemInfo));
	}
	return mExtData ? true : false;
}

// ----------------------------------------------------------------------------
// 获取物品扩展数据对象
stItemInfo* Res::getExtData()
{
	return mExtData;
}

// ----------------------------------------------------------------------------
// 获取类别名称
StringTableEntry Res::getCategoryName()
{
	return mBaseData ? sCategory[mBaseData->mSubCategory / 100 -1][mBaseData->mSubCategory % 100 -1] : "";
}

// ----------------------------------------------------------------------------
// 获取类别名称（静态方法)
StringTableEntry Res::getCategoryName(U32 cat,U32 subCat)
{
	return sCategory[subCat / 100 -1][subCat % 100 -1];
}

// ----------------------------------------------------------------------------
// 获取物品制造者
StringTableEntry Res::getProducer()
{
	return mExtData ? mExtData->Producer : "";
}

// ----------------------------------------------------------------------------
// 设置物品制造者
void Res::setProducer(StringTableEntry name)
{
	if(mExtData) dStrcpy(mExtData->Producer, COMMON_STRING_LENGTH, name);
}

// ----------------------------------------------------------------------------
// 获取物品世界唯一ID
U64 Res::getUID()
{
	return mExtData ? mExtData->UID : 0;
}

// ----------------------------------------------------------------------------
// 设置物品世界唯一ID
void Res::setUID(U64 uid)
{
	if(mExtData) mExtData->UID = uid;
}

// ----------------------------------------------------------------------------
// 获取时效剩余时间(relate: Res::getDuration)
U32 Res::getLapseTime()
{
	return mExtData ? mExtData->LapseTime : 0;
}

// ----------------------------------------------------------------------------
// 设置时效剩余时间
void Res::setLapseTime(U32 val)
{
	if(mExtData) mExtData->LapseTime = val;
}

// ----------------------------------------------------------------------------
// 获取物品最大使用次数 
U32 Res::getUsedTimes()
{
	return mExtData ? mExtData->RemainUseTimes : getResUsedTimes();
}

// ----------------------------------------------------------------------------
// 设置剩余使用次数
void Res::setUsedTimes(S32 times)
{
	if(mExtData) mExtData->RemainUseTimes = mClamp(times, 0, getResUsedTimes());
}

// ----------------------------------------------------------------------------
// 获取物品锁剩余时间
U32 Res::getLockedLeftTime()
{
	return mExtData ? mExtData->LockedLeftTime : getDuration();
}

// ----------------------------------------------------------------------------
// 设置物品锁剩余时间
void Res::setLockedLeftTime(S32 times)
{
	if(mExtData) mExtData->LockedLeftTime = mClamp(times, 0, getDuration());
}

// ----------------------------------------------------------------------------
// 获取物品灵魂绑定的角色ID
U32 Res::getBindPlayerID()
{
	return mExtData ? mExtData->BindPlayer : 0;
}

// ----------------------------------------------------------------------------
// 设置物品灵魂绑定角色ID
void Res::setBindPlayerID(U32 playerid)
{
	if(mExtData) mExtData->BindPlayer = playerid;
}

// ----------------------------------------------------------------------------
// 共享物品给好友,设置好友角色ID
U32 Res::getBindFriendID()
{
	return mExtData ? mExtData->BindFriend : 0;
}

// ----------------------------------------------------------------------------
// 共享物品给好友,设置好友角色ID
void Res::setBindFriendID(U32 playerid)
{
	if(mExtData) mExtData->BindFriend = playerid;
}

// ----------------------------------------------------------------------------
// 获取激活物品标志(32bit)
U32 Res::getActiveFlag()
{
	return mExtData ? mExtData->ActiveFlag : 0;
}

// ----------------------------------------------------------------------------
// 设置激活物品标志(32bit)
void Res::setActiveFlag(U32 flag, bool isSet/*=true*/)
{
	if(mExtData)
	{
		if(isSet)
			mExtData->ActiveFlag |= flag;
		else
			mExtData->ActiveFlag &= ~flag;
	}
}

// ----------------------------------------------------------------------------
// 获取当前耐久度
S32 Res::getCurrentWear()
{
	return mExtData ? mExtData->CurWear : 0;
}

// ----------------------------------------------------------------------------
// 获取最大耐久度
S32 Res::getCurrentMaxWear()
{
	return mExtData ? mExtData->CurMaxWear : 0;
}

// ----------------------------------------------------------------------------
// 设置当前耐久度
void Res::setCurrentWear(S32 wear)
{
	if(mExtData) mExtData->CurWear = wear;
}

// ----------------------------------------------------------------------------
// 设置最大耐久度
void Res::setCurrentMaxWear(S32 max)
{
	if(mExtData) mExtData->CurMaxWear = max;
}

// ----------------------------------------------------------------------------
// 获取当前耐久度
S32 Res::getCurAdroit()
{
	return mExtData ? mExtData->CurAdroit : 0;
}

// ----------------------------------------------------------------------------
// 获取最大耐久度
S32 Res::getCurAdroitLv()
{
	return mExtData ? mExtData->CurAdroitLv : 0;
}

// ----------------------------------------------------------------------------
// 设置当前耐
void Res::setCurAdroit(S32 adroit)
{
	if(mExtData) mExtData->CurAdroit = adroit;
}

// ----------------------------------------------------------------------------
// 设置当前熟练等级
void Res::setCurAdroitLv(S32 adroitlv)
{
	if(mExtData) mExtData->CurAdroitLv = adroitlv;
}

// ----------------------------------------------------------------------------
// 获取激活附加属性效果标志
bool Res::IsActivatePro(U32 flag)
{
	return mExtData? mExtData->ActivatePro & flag : false;
}

// ----------------------------------------------------------------------------
// 设置激活附加属性效果标志
void Res::setActivatePro(U32 flag)
{
	if(mExtData) mExtData->ActivatePro |= flag;
}

// ----------------------------------------------------------------------------
// 获取鉴定附加属性个数
U32 Res::getIDEProNum()
{
	return mExtData ? mExtData->IDEProNum : 0;
}

// ----------------------------------------------------------------------------
// 获取鉴定附加属性指定索引位置的值
U32 Res::getIDEProValue(S32 index)
{
	if(index >= 0 && index < getIDEProNum() && mExtData)
		return mExtData->IDEProValue[index];
	return 0;
}

// ----------------------------------------------------------------------------
// 设置鉴定附加属性指定索引位置的值
void Res::setIDEProValue(S32 index, U32 val)
{
	if(index < 0 || index > 12)
		return;
	if(!mExtData)
		return;
	mExtData->IDEProValue[index] = val;
}

// ----------------------------------------------------------------------------
// 是否随机生成过强化附加属性
bool Res::isRandStengthens()
{
	return mExtData ? (mExtData->ActiveFlag & Res::ACTIVEF_RANDPRO) : false;
}

// ----------------------------------------------------------------------------
// 获取物品当前强化次数
U32 Res::getEquipStrengthens()
{
	return mExtData ? mExtData->EquipStrengthens : 0;
}

// ----------------------------------------------------------------------------
// 设置物品当前强化次数
void Res::getEquipStrengthenVal(S32 index, U32& lv, U32& val)
{
	lv = val = 0;
	if(index >= 0 && index <= getEquipStrengthens() && mExtData)
	{
		lv = mExtData->EquipStrengthenValue[index][0];
		val = mExtData->EquipStrengthenValue[index][1];
	}
}

// ----------------------------------------------------------------------------
// 设置物品当前强化次数
void Res::setEquipStrengthenVal(S32 index, U32 lv, U32 val)
{
	if(index >= 0 && index <= getEquipStrengthens() && mExtData)
	{
		mExtData->EquipStrengthenValue[index][0] = lv;
		mExtData->EquipStrengthenValue[index][1] = val;
	}
}

// ----------------------------------------------------------------------------
// 获得灵魂链接属性ID
U32 Res::getSoulLinkStatsID()
{
	return mExtData ? mExtData->BindProID : 0;
}

// ----------------------------------------------------------------------------
// 设置灵魂链接属性ID
void Res::setSoulLinkStatsID(U32 statsid)
{
	if(mExtData) mExtData->BindProID = statsid;
}

// ----------------------------------------------------------------------------
// 获取铭刻ID
U32 Res::getImpressID()
{
	return mExtData ? mExtData->ImpressID : 0;
}

// ----------------------------------------------------------------------------
// 设置铭刻ID
void Res::setImpressID(U32 statsid)
{
	if(mExtData) mExtData->ImpressID = statsid;
}

// ----------------------------------------------------------------------------
// 判断物品是否能放到对应的正确的Slot上
bool Res::canPutEquip(S32 index)
{
	if(mBaseData)
	{
		if(isTrump() && index == EQUIPPLACEFLAG_TRUMP)	// 法宝
			return true;
		else if(isWeapon() && !isTrump() && index == EQUIPPLACEFLAG_WEAPON)	// 武器
			return true;
		else if(isRide() && index == EQUIPPLACEFLAG_RIDE)	// 坐骑道具
			return true;
		else if(isPet() && index == EQUIPPLACEFLAG_PET)	// 宠物道具
			return true;
		else if(isGather() && index == EQUIPPLACEFLAG_GATHER)	// 采集工具
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_HEAD && index == EQUIPPLACEFLAG_HEAD)	// 头部
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_NECK && index == EQUIPPLACEFLAG_NECK)	// 颈部
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_BACK && index == EQUIPPLACEFLAG_BACK)// 背部
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_SHOULDER && index == EQUIPPLACEFLAG_SHOULDER) // 肩部
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_BODY && index == EQUIPPLACEFLAG_BODY)// 身体
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_HAND && index == EQUIPPLACEFLAG_HAND)	// 手部
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_WAIST && index == EQUIPPLACEFLAG_WAIST)	// 腰部
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_FOOT && index == EQUIPPLACEFLAG_FOOT)	// 脚部
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_RING && index == EQUIPPLACEFLAG_RING)	// 戒指
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_AMULET && index == 	EQUIPPLACEFLAG_AMULET)	// 护身符
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_PACK1 && (mBaseData->mSubCategory == CATEGORY_INVE_FOREVER_PACK ||
			mBaseData->mSubCategory == CATEGORY_INVE_TIME_PACK)) 	// 扩展背包1
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_PACK2 &&
			(mBaseData->mSubCategory == CATEGORY_INVE_FOREVER_PACK || mBaseData->mSubCategory == CATEGORY_INVE_TIME_PACK))	// 扩展背包2
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_BANKPACK && 
			(mBaseData->mSubCategory == CATEGORY_BANK_FOREVER_PACK || mBaseData->mSubCategory == CATEGORY_BANK_TIME_PACK))	// 银行仓库扩展格
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_STALLPACK1 &&
			(mBaseData->mSubCategory == CATEGORY_STAL_FOREVER_PACK ||
			mBaseData->mSubCategory == CATEGORY_STAL_TIME_PACK))	// 摆摊扩展格1
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_STALLPACK2 &&
			(mBaseData->mSubCategory == CATEGORY_STAL_FOREVER_PACK ||
			mBaseData->mSubCategory == CATEGORY_STAL_TIME_PACK)) // 摆摊扩展格2
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_STALLPACK3 &&
			mBaseData->mSubCategory == CATEGORY_STAL_STYLE_PACK) // 摆摊扩展格2
		return true;									

		// 摆摊样式格
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_FAMILY && index == EQUIPPLACEFLAG_FAMLIYFASHION)// 门派时装
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_HEAD && index == EQUIPPLACEFLAG_FASHIONHEAD)// 头部时装
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_BODY && index == EQUIPPLACEFLAG_FASHIONBODY)// 身体时装
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_BACK && index == EQUIPPLACEFLAG_FASHIONBACK)// 时装被
			return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
// 获取物品可装备的槽位位置
S32 Res::getEquipPos()
{
	if(mBaseData)
	{
		if(isTrump())
			return EQUIPPLACEFLAG_TRUMP;		// 法宝
		else if(isWeapon() && !isTrump())
			return EQUIPPLACEFLAG_WEAPON;		// 武器
		//else if(isRide())
		//	return EQUIPPLACEFLAG_RIDE;			// 坐骑道具
		//else if(isPet())
		//	return EQUIPPLACEFLAG_PET;			// 宠物道具
		else if(isGather())
			return EQUIPPLACEFLAG_GATHER;		// 采集工具
		else if(mBaseData->mSubCategory == CATEGORY_HEAD)
			return EQUIPPLACEFLAG_HEAD;			// 头部
		else if(mBaseData->mSubCategory == CATEGORY_NECK)
			return EQUIPPLACEFLAG_NECK;			// 颈部
		else if(mBaseData->mSubCategory == CATEGORY_BACK)
			return EQUIPPLACEFLAG_BACK;			// 背部
		else if(mBaseData->mSubCategory == CATEGORY_SHOULDER)
			return EQUIPPLACEFLAG_SHOULDER;		// 肩部
		else if(mBaseData->mSubCategory == CATEGORY_BODY)
			return EQUIPPLACEFLAG_BODY;			// 身体
		else if(mBaseData->mSubCategory == CATEGORY_HAND)
			return EQUIPPLACEFLAG_HAND;			// 手部
		else if(mBaseData->mSubCategory == CATEGORY_WAIST)
			return EQUIPPLACEFLAG_WAIST;		// 腰部
		else if(mBaseData->mSubCategory == CATEGORY_FOOT)
			return EQUIPPLACEFLAG_FOOT;			// 脚部
		else if(mBaseData->mSubCategory == CATEGORY_RING)
			return EQUIPPLACEFLAG_RING;			// 戒指
		else if(mBaseData->mSubCategory == CATEGORY_AMULET)
			return EQUIPPLACEFLAG_AMULET;		// 护身符
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_FAMILY)
			return EQUIPPLACEFLAG_FAMLIYFASHION;	// 门派时装
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_HEAD)
			return EQUIPPLACEFLAG_FASHIONHEAD;	// 头部时装
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_BODY)
			return EQUIPPLACEFLAG_FASHIONBODY;	// 身体时装
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_BACK)
			return EQUIPPLACEFLAG_FASHIONBACK;	// 背部时装

	}
	return -1;	
}

// ----------------------------------------------------------------------------
// 可否交易
bool Res::canTrade()
{
	// 是否交易保护/开启装备锁/绑定
	if(isEquip() && mExtData && mExtData->BindPlayer)
		return false;

	//是否可交易
	return canBaseLimit(ITEM_TRADE);
}


// ============================================================================
// 扩展数据方法
// ============================================================================
// ----------------------------------------------------------------------------
// 设置物品的扩展数据
bool Res::setExtData()
{
	if(mBaseData && !mExtData)
	{
		mExtData = new stItemInfo;
		dMemset(mExtData, 0, sizeof(stItemInfo));
		mExtData->Producer[0]	= '\0';
		mExtData->UID			= 0;
		mExtData->ItemID		= mBaseData->mItemID;
		mExtData->Quantity		= 1;
		mExtData->LapseTime		= mBaseData->mDuration;
		mExtData->RemainUseTimes= mBaseData->mUseTimes;
		mExtData->BindPlayer	= 0;
		mExtData->BindPlayer	= 0;
		mExtData->ActiveFlag	|= mBaseData->isEquip() ? Res::ACTIVEF_ISEQUIP : 0;
		mExtData->Quality		= mBaseData->mQualityLevel;

		if(mBaseData->isEquip())
		{			
			mExtData->CurWear		= mBaseData->mMaxWear;
			mExtData->CurMaxWear	= mBaseData->mMaxWear;
			mExtData->CurAdroit		= 0;
			mExtData->CurAdroitLv	= 0;
			mExtData->ActivatePro	= 0;			
			mExtData->RandPropertyID= mBaseData->mRandomNameID;

			// 鉴定附加属性（即使无需鉴定，也可能存在鉴定附加属性）
			for(S32 i = 0; i < MAX_IDEPROS; i++)
			{
				if(mBaseData->mAppendAttribute[i] != 0)
				{
					mExtData->IDEProNum++;
					mExtData->IDEProValue[i] = 302000000 + mBaseData->mAppendAttribute[i] * 10000 +
						mBaseData->mQualityLevel;
				}
			}

			// 装备强化等级及效果属性
			mExtData->EquipStrengthens	= 0;
			dMemcpy(mExtData->EquipStrengthenValue,  mBaseData->mEquipStrengthenLevel,
				sizeof(mExtData->EquipStrengthenValue));

			mExtData->ImpressID			= 0;
			mExtData->BindProID			= 0;
			mExtData->WuXingID			= 0;
			mExtData->WuXingLinkID		= 0;
			mExtData->WuXingPro			= 0;
			mExtData->SkillAForEquip	= mBaseData->mSkillAForEquip;
			mExtData->SkillBForEquip	= mBaseData->mSkillBForEquip;
			mExtData->EmbedOpened		= mBaseData->mOpenedEmbedSlot;	
			for(S32 i=0; i<MAX_EMBEDSLOTS; ++i)
				mExtData->EmbedSlot[i] = 0;
		}
	}
	return mExtData ? true : false;
}



// ----------------------------------------------------------------------------
// 获取物品的数量
S32 Res::getQuantity()
{
	return mExtData ? mExtData->Quantity : 0;
}

// ----------------------------------------------------------------------------
// 设置物品的数量
void Res::setQuantity(S32 num, bool ignorecheck/*=false*/)
{
	if(!mExtData || !mBaseData || num <0)
		return;
	if(ignorecheck || (!ignorecheck && num <= mBaseData->mMaxOverNum))
		mExtData->Quantity = num;
}



// ----------------------------------------------------------------------------
// 获取物品的实际价格
S32 Res::getActualPrice()
{
	//根据耐久度计算价值的损耗
	return 0;
}


// ----------------------------------------------------------------------------
// 获得物品分类子类ID
U32 Res::getSubPropertyID(U32 statsid)
{
	return (statsid % 10000000) / 1000;
}

// ----------------------------------------------------------------------------
// 获得宝石限制条件
U32 Res::getStoneLimit()
{
	return mBaseData ? mBaseData->mStoneLimit : (U32)Res::Stone_None;
}

// ----------------------------------------------------------------------------
// 
bool Res::canMountGem(U32 nGemLimit)
{
	U32 limit=0;

	if (nGemLimit == 0)
		return true;

	if (isWeapon())			//武器类
	{
		limit = (U32)Stone_Faqi << (GETSUB(getSubCategory()) - 1);
	}
	else if (isEquipment())	//防具类
	{
		limit = (U32)Stone_Head << (GETSUB(getSubCategory()) - 1);
	}
	else if (isOrnament())	//饰品类
	{
		limit = (U32)Stone_Amulet << (GETSUB(getSubCategory()) - 1);
	}
	else if (isTrump())		//法宝类
	{
		limit = (U32)Stone_Trump;
	}
	if (nGemLimit & limit)
		return true;
	return false;
}

// ----------------------------------------------------------------------------
// 
S32 Res::getOpenedHoles()
{
	if(!mExtData)
		return 0;
	//允许最多开三个孔
	S32 nOpenedNum = 0;
	U32 nAllowSlot = getAllowEmbedSlot();
	U8 nOpenedSlot = getExtData()->EmbedOpened;
	for (S32 i = 0; i < 3; i++)
	{
		if ( ((nAllowSlot >> (4 * i)) & 0xF) == 0 )
			continue;

		if ( (nOpenedSlot >> (i)) & 0x1 )
			nOpenedNum++;
	}

	return nOpenedNum;
}

// ----------------------------------------------------------------------------
// 
S32 Res::getAllowedHoles()
{
	//允许最多开三个孔
	S32 nAllowedNum = 0;
	U32 nAllowSlot = getAllowEmbedSlot();
	for (S32 i = 0; i < 3; i++)
	{
		if ( (nAllowSlot >> (4 * i)) & 0xF )
			nAllowedNum++;
	}

	return nAllowedNum;
}

// ----------------------------------------------------------------------------
// 
S32 Res::getLeftHoles()
{
	S32 nAllowNum = getAllowedHoles();
	S32 nOpenedNum = getOpenedHoles();
	S32 nLeftNum = nAllowNum - nOpenedNum;

	return (nLeftNum >= 0) ? nLeftNum : 0;
}

// ----------------------------------------------------------------------------
// 
S32	Res::GetSlotColor(S32 nIndex)
{
	U32 nAllowEmbedFlag = getAllowEmbedSlot();
	if ( nIndex < 1 || nIndex > 3 || !IsSlotOpened(nIndex) || !nAllowEmbedFlag)
		return 0;

	return (nAllowEmbedFlag >> (4 * (nIndex - 1)))  & 0xF;
}

// ----------------------------------------------------------------------------
// 
bool Res::IsSlotOpened(S32 nIndex)
{
	if ( !IsSlotAllowEmbed(nIndex) || !getExtData())
		return false;	

	//判断当前位置是否被打孔
	U8 nOpenedEmbededFlag = getExtData()->EmbedOpened;

	return (nOpenedEmbededFlag >> (nIndex - 1)) & 0x1;
}

// ----------------------------------------------------------------------------
// 
bool Res::IsSlotAllowEmbed(S32 nIndex)
{
	if ( nIndex < 1 || nIndex > 3 )
		return false;

	U32 nAllowOpenSlotFlag = getAllowEmbedSlot();
	if (!nAllowOpenSlotFlag)
		return false;

	return (nAllowOpenSlotFlag >> ((nIndex - 1) * 4)) & 0xF;
}

// ----------------------------------------------------------------------------
// 
bool Res::IsSlotEmbedable(S32 nIndex)
{
	if ( !IsSlotOpened(nIndex) || !mExtData)
		return false;

	//判断当前位置是否可以镶嵌宝石(打孔但未镶嵌)
	stItemInfo *pItemInfo = getExtData();
	if (!pItemInfo)
		return false;

	return ( pItemInfo->EmbedSlot[nIndex - 1] == 0 );
}

bool Res::IsSlotEmbeded(S32 nIndex)
{
	if ( !IsSlotOpened(nIndex)  || !mExtData)
		return false;

	//判断当前位置是否已经镶嵌宝石
	stItemInfo *pItemInfo = getExtData();

	return ( pItemInfo->EmbedSlot[nIndex - 1] != 0 );
}

// ----------------------------------------------------------------------------
// 按品质颜色获取物品名称
void Res::getItemName(char* colorName, U32 size)
{
	dSprintf(colorName, size, "<t m='0' c='0x%x'>%s</t>", 
		sColorValue[getColorLevel()], getItemName());
}

// ----------------------------------------------------------------------------
// 修理装备耐久
bool Res::setRepairResult(U32 type)
{
	if(getCurrentMaxWear() == -1)		//经久强化装备修理
	{
		setCurrentWear(getMaxWear());
	}
	else 
	{
		if(type == REPAIR_RECOVER)		//恢复修理
		{
			//若恢复修理恢复到模板最大耐久度
			setCurrentMaxWear(getMaxWear());
			setCurrentWear(getCurrentMaxWear());
		}
		else if(type == REPAIR_DURABLE)	//经久修理
		{
			setCurrentMaxWear(-1);
			setCurrentWear(getMaxWear());
		}
		else
		{
			S32 randNum = Platform::getRandomI(1, 100);
			char* result = Con::getReturnBuffer(32);
			result = (char*)Con::executef("getRepairFailRate", Con::getIntArg

				(mBaseData->mLevelLimit));
			S32 baseRate = dAtoi(result);
			// 耐久损耗命中率
			S32 hitRate = baseRate +  (getCurrentMaxWear()- getCurrentWear())  / 

				getCurrentMaxWear() * 50;
			if(randNum < hitRate)
			{
				//未经久强化的装备修理，当前最大耐久=当前最大耐久 - (最大耐久/4)
				S32 currentMax = getCurrentMaxWear() - getMaxWear() / 4;
				if(currentMax < 1)
					currentMax = 1;
				setCurrentMaxWear(currentMax);
			}
			setCurrentWear(getCurrentMaxWear());
		}	
	}

	return true;
}

// ----------------------------------------------------------------------------
// 计算普通修理扣钱
U32 Res::getRepairNeedMoney(U8 level /* = 1 */)
{
	if(!mBaseData || !mExtData)
		return 0;
	char* result = Con::getReturnBuffer(32);
	result = (char*)Con::executef("getRepairPrice", Con::getIntArg(mBaseData->mMaxWear), 

		Con::getIntArg(mExtData->CurWear),
		Con::getIntArg(mExtData->CurMaxWear), Con::getIntArg(level));
	U32 iCost = dAtof(result);
	return iCost;
}

// ----------------------------------------------------------------------------
// 计算经久修理扣钱
U32 Res::getRepairNeedMoney1()
{
	if(!mBaseData || !mExtData)
		return 0;
	char* result = Con::getReturnBuffer(32);
	result = (char*)Con::executef("getMaxRepairPrice", Con::getIntArg(mBaseData->mMaxWear),	

		Con::getIntArg(mExtData->CurMaxWear));
	U32 iCost = dAtof(result);
	return iCost;
}

U32 Res::getGemQualityValue()
{ 
	if(!mExtData)
		return 0;
	U32 tempGemValue = 0;
	for (int i = 0; i<getOpenedHoles(); i++ )
	{
		if (IsSlotEmbeded(i+1))
		{
			U32 nGemId = getExtData()->EmbedSlot[i];
			ItemBaseData* pBase = g_ItemRepository->getItemData(nGemId);
			U32 nGemLevel = pBase ? pBase->mLevelLimit : 0;
			if (nGemLevel)
				tempGemValue += sGemQualityValue[nGemLevel-1];
		}
	}
	return tempGemValue;
}

U32 Res::getItemQualityValue()
{
	U32 Strengthens = getEquipStrengthens();
	if (Strengthens > 0)
		return  sItemQualityValue[Strengthens-1] + getQualityLevel();
	return getQualityLevel();
}
