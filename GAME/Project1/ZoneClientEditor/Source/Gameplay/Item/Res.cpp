//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Item/Res.h"
#include "Common/PlayerStruct.h"
#include "gameplay/GameObjects/PlayerObject.h"

//��Ʒ��ɫ�ȼ���ɫֵ(RGBA)
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

//��ʯƷ��ֵ
static U32 sGemQualityValue[10]=
{
	25,34,45,60,81,98,117,140,167,200,
};

//װ������Ʒ��ֵ
static U32 sItemQualityValue[10]=
{
	1,3,5,8,11,14,18,22,26,32,
};

static char* sCategory[][16] =
{
	{
		"����",
		"����",
		"˫��",
		"��",
		"��",
		"����",
		"����",
	},
	{
		"ͷ��",
		"����",
		"����",
		"�粿",
		"�ֲ�",
		"����",
		"�Ų�",
		"����ʱװ",
		"ͷ��ʱװ",
		"����ʱװ",
		"����ʱװ",
	},
	{
		"�����",
		"����",
		"��ָ",
	},
	{
		"����",
		"����",
		"�ٻ�",
	},
	{
		"ʳ��",
		"ҩƷ",
		"��Ʒ",
		"����",
		"���ܲ���",
		"����",
		"�鼮",
		"������",
		"����",
		"��Ʒ",
		"����ʳ��",
		"���ﵤҩ",
		"�������",
		"Ԫ��ʯ",
		"Ԫ����",
	},
	{
		"��ҩ",
		"����",
		"ũ����",
		"ľ��",
		"����",
		"����",
		"����",
		"Ƥ��",
		"ħ������",
		"ֽ",
	},
	{
		"�ɼ�����",
		"�ڿ󹤾�",
		"���ֹ���",
		"��ľ����",
		"���㹤��",
		"���Թ���",
		"ץ�蹤��",
		"ĥƿ",
		"���칤��",
		"�÷칤��",
		"���չ���",
		"���乤��",
		"��������",
		"��⿹���",
		"��������",
		"��������",
	},
	{
		"����",
		"������Ʒ"
	},
	{
		"����",
		"�÷�",
		"����",
		"����",
		"����",
		"���",
		"����",
		"����",
		"����",
	},
	{
		"�챦ʯ",
		"�̱�ʯ",
		"����ʯ",
		"�쾧ʯ",
		"�̾�ʯ",
		"����ʯ",
		"���ʯ",
		"�ྦྷʯ",
		"�Ͼ�ʯ",
		"�ƾ�ʯ",
		"�ྦྷʯ",
		"�Ͼ�ʯ",
	},
	{
		"������ħ",
		"���߸�ħ",
		"��Ʒ��ħ",
		"ͷ����ħ",
		"���ħ",
		"���總ħ",
		"�·���ħ",
		"Ь�Ӹ�ħ",
		"���ָ�ħ",
		"������ħ",
		"������ħ",
		"��ָ��ħ",
		"����ħ",
		"ȫ����ħ",
		"������ħ",
	},
	{
		"��Ʒ�����ð���",
		"�ֿ������ð���",
		"��̯�����ð���",
		"��Ʒ��ʱЧ����",
		"�ֿ���ʱЧ����",
		"��̯��ʱЧ����",
		"��̯����ʽ����",
	},
	{
		"�سǷ�",
		"��¼����",
		"ͼ��",
		"����",
		"������",
		"��ͯ��",
	},
	{
		"�̻�",
		"��ױ",
		"����װ��",
		"Ԫ��װ��",
		"����",
		"Կ��",
		"����",
	},
	{
		"����ͷ",
		"��������",
		"���ﱳ",
		"��������",
	},
	{
		"ǿ����������",
		"ǿ�����Ӳ���",
		"��ײ���",
		"ժȡ����",
		"��������",
		"������Ӳ���",
		"���ǵ�",
		"������������",
		"�����ϳɲ���",
		"����ǿ������",
	},
	{
		"����",
	},
	{
		"���ޱ�����ӡ",
		"�������ޱ�����ӡ",
		"���޷�ӡ",
	    "�������޷�ӡ",
	},
	{
		"����ӡ",
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
// ������Ʒ��չ����
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
// ��ȡ��Ʒ��չ���ݶ���
stItemInfo* Res::getExtData()
{
	return mExtData;
}

// ----------------------------------------------------------------------------
// ��ȡ�������
StringTableEntry Res::getCategoryName()
{
	return mBaseData ? sCategory[mBaseData->mSubCategory / 100 -1][mBaseData->mSubCategory % 100 -1] : "";
}

// ----------------------------------------------------------------------------
// ��ȡ������ƣ���̬����)
StringTableEntry Res::getCategoryName(U32 cat,U32 subCat)
{
	return sCategory[subCat / 100 -1][subCat % 100 -1];
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ������
StringTableEntry Res::getProducer()
{
	return mExtData ? mExtData->Producer : "";
}

// ----------------------------------------------------------------------------
// ������Ʒ������
void Res::setProducer(StringTableEntry name)
{
	if(mExtData) dStrcpy(mExtData->Producer, COMMON_STRING_LENGTH, name);
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ����ΨһID
U64 Res::getUID()
{
	return mExtData ? mExtData->UID : 0;
}

// ----------------------------------------------------------------------------
// ������Ʒ����ΨһID
void Res::setUID(U64 uid)
{
	if(mExtData) mExtData->UID = uid;
}

// ----------------------------------------------------------------------------
// ��ȡʱЧʣ��ʱ��(relate: Res::getDuration)
U32 Res::getLapseTime()
{
	return mExtData ? mExtData->LapseTime : 0;
}

// ----------------------------------------------------------------------------
// ����ʱЧʣ��ʱ��
void Res::setLapseTime(U32 val)
{
	if(mExtData) mExtData->LapseTime = val;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ���ʹ�ô��� 
U32 Res::getUsedTimes()
{
	return mExtData ? mExtData->RemainUseTimes : getResUsedTimes();
}

// ----------------------------------------------------------------------------
// ����ʣ��ʹ�ô���
void Res::setUsedTimes(S32 times)
{
	if(mExtData) mExtData->RemainUseTimes = mClamp(times, 0, getResUsedTimes());
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ʣ��ʱ��
U32 Res::getLockedLeftTime()
{
	return mExtData ? mExtData->LockedLeftTime : getDuration();
}

// ----------------------------------------------------------------------------
// ������Ʒ��ʣ��ʱ��
void Res::setLockedLeftTime(S32 times)
{
	if(mExtData) mExtData->LockedLeftTime = mClamp(times, 0, getDuration());
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ���󶨵Ľ�ɫID
U32 Res::getBindPlayerID()
{
	return mExtData ? mExtData->BindPlayer : 0;
}

// ----------------------------------------------------------------------------
// ������Ʒ���󶨽�ɫID
void Res::setBindPlayerID(U32 playerid)
{
	if(mExtData) mExtData->BindPlayer = playerid;
}

// ----------------------------------------------------------------------------
// ������Ʒ������,���ú��ѽ�ɫID
U32 Res::getBindFriendID()
{
	return mExtData ? mExtData->BindFriend : 0;
}

// ----------------------------------------------------------------------------
// ������Ʒ������,���ú��ѽ�ɫID
void Res::setBindFriendID(U32 playerid)
{
	if(mExtData) mExtData->BindFriend = playerid;
}

// ----------------------------------------------------------------------------
// ��ȡ������Ʒ��־(32bit)
U32 Res::getActiveFlag()
{
	return mExtData ? mExtData->ActiveFlag : 0;
}

// ----------------------------------------------------------------------------
// ���ü�����Ʒ��־(32bit)
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
// ��ȡ��ǰ�;ö�
S32 Res::getCurrentWear()
{
	return mExtData ? mExtData->CurWear : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ����;ö�
S32 Res::getCurrentMaxWear()
{
	return mExtData ? mExtData->CurMaxWear : 0;
}

// ----------------------------------------------------------------------------
// ���õ�ǰ�;ö�
void Res::setCurrentWear(S32 wear)
{
	if(mExtData) mExtData->CurWear = wear;
}

// ----------------------------------------------------------------------------
// ��������;ö�
void Res::setCurrentMaxWear(S32 max)
{
	if(mExtData) mExtData->CurMaxWear = max;
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�;ö�
S32 Res::getCurAdroit()
{
	return mExtData ? mExtData->CurAdroit : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ����;ö�
S32 Res::getCurAdroitLv()
{
	return mExtData ? mExtData->CurAdroitLv : 0;
}

// ----------------------------------------------------------------------------
// ���õ�ǰ��
void Res::setCurAdroit(S32 adroit)
{
	if(mExtData) mExtData->CurAdroit = adroit;
}

// ----------------------------------------------------------------------------
// ���õ�ǰ�����ȼ�
void Res::setCurAdroitLv(S32 adroitlv)
{
	if(mExtData) mExtData->CurAdroitLv = adroitlv;
}

// ----------------------------------------------------------------------------
// ��ȡ���������Ч����־
bool Res::IsActivatePro(U32 flag)
{
	return mExtData? mExtData->ActivatePro & flag : false;
}

// ----------------------------------------------------------------------------
// ���ü��������Ч����־
void Res::setActivatePro(U32 flag)
{
	if(mExtData) mExtData->ActivatePro |= flag;
}

// ----------------------------------------------------------------------------
// ��ȡ�����������Ը���
U32 Res::getIDEProNum()
{
	return mExtData ? mExtData->IDEProNum : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ������������ָ������λ�õ�ֵ
U32 Res::getIDEProValue(S32 index)
{
	if(index >= 0 && index < getIDEProNum() && mExtData)
		return mExtData->IDEProValue[index];
	return 0;
}

// ----------------------------------------------------------------------------
// ���ü�����������ָ������λ�õ�ֵ
void Res::setIDEProValue(S32 index, U32 val)
{
	if(index < 0 || index > 12)
		return;
	if(!mExtData)
		return;
	mExtData->IDEProValue[index] = val;
}

// ----------------------------------------------------------------------------
// �Ƿ�������ɹ�ǿ����������
bool Res::isRandStengthens()
{
	return mExtData ? (mExtData->ActiveFlag & Res::ACTIVEF_RANDPRO) : false;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ǰǿ������
U32 Res::getEquipStrengthens()
{
	return mExtData ? mExtData->EquipStrengthens : 0;
}

// ----------------------------------------------------------------------------
// ������Ʒ��ǰǿ������
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
// ������Ʒ��ǰǿ������
void Res::setEquipStrengthenVal(S32 index, U32 lv, U32 val)
{
	if(index >= 0 && index <= getEquipStrengthens() && mExtData)
	{
		mExtData->EquipStrengthenValue[index][0] = lv;
		mExtData->EquipStrengthenValue[index][1] = val;
	}
}

// ----------------------------------------------------------------------------
// ��������������ID
U32 Res::getSoulLinkStatsID()
{
	return mExtData ? mExtData->BindProID : 0;
}

// ----------------------------------------------------------------------------
// ���������������ID
void Res::setSoulLinkStatsID(U32 statsid)
{
	if(mExtData) mExtData->BindProID = statsid;
}

// ----------------------------------------------------------------------------
// ��ȡ����ID
U32 Res::getImpressID()
{
	return mExtData ? mExtData->ImpressID : 0;
}

// ----------------------------------------------------------------------------
// ��������ID
void Res::setImpressID(U32 statsid)
{
	if(mExtData) mExtData->ImpressID = statsid;
}

// ----------------------------------------------------------------------------
// �ж���Ʒ�Ƿ��ܷŵ���Ӧ����ȷ��Slot��
bool Res::canPutEquip(S32 index)
{
	if(mBaseData)
	{
		if(isTrump() && index == EQUIPPLACEFLAG_TRUMP)	// ����
			return true;
		else if(isWeapon() && !isTrump() && index == EQUIPPLACEFLAG_WEAPON)	// ����
			return true;
		else if(isRide() && index == EQUIPPLACEFLAG_RIDE)	// �������
			return true;
		else if(isPet() && index == EQUIPPLACEFLAG_PET)	// �������
			return true;
		else if(isGather() && index == EQUIPPLACEFLAG_GATHER)	// �ɼ�����
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_HEAD && index == EQUIPPLACEFLAG_HEAD)	// ͷ��
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_NECK && index == EQUIPPLACEFLAG_NECK)	// ����
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_BACK && index == EQUIPPLACEFLAG_BACK)// ����
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_SHOULDER && index == EQUIPPLACEFLAG_SHOULDER) // �粿
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_BODY && index == EQUIPPLACEFLAG_BODY)// ����
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_HAND && index == EQUIPPLACEFLAG_HAND)	// �ֲ�
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_WAIST && index == EQUIPPLACEFLAG_WAIST)	// ����
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_FOOT && index == EQUIPPLACEFLAG_FOOT)	// �Ų�
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_RING && index == EQUIPPLACEFLAG_RING)	// ��ָ
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_AMULET && index == 	EQUIPPLACEFLAG_AMULET)	// �����
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_PACK1 && (mBaseData->mSubCategory == CATEGORY_INVE_FOREVER_PACK ||
			mBaseData->mSubCategory == CATEGORY_INVE_TIME_PACK)) 	// ��չ����1
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_PACK2 &&
			(mBaseData->mSubCategory == CATEGORY_INVE_FOREVER_PACK || mBaseData->mSubCategory == CATEGORY_INVE_TIME_PACK))	// ��չ����2
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_BANKPACK && 
			(mBaseData->mSubCategory == CATEGORY_BANK_FOREVER_PACK || mBaseData->mSubCategory == CATEGORY_BANK_TIME_PACK))	// ���вֿ���չ��
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_STALLPACK1 &&
			(mBaseData->mSubCategory == CATEGORY_STAL_FOREVER_PACK ||
			mBaseData->mSubCategory == CATEGORY_STAL_TIME_PACK))	// ��̯��չ��1
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_STALLPACK2 &&
			(mBaseData->mSubCategory == CATEGORY_STAL_FOREVER_PACK ||
			mBaseData->mSubCategory == CATEGORY_STAL_TIME_PACK)) // ��̯��չ��2
			return true;
		else if(isBagItem() && index == EQUIPPLACEFLAG_STALLPACK3 &&
			mBaseData->mSubCategory == CATEGORY_STAL_STYLE_PACK) // ��̯��չ��2
		return true;									

		// ��̯��ʽ��
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_FAMILY && index == EQUIPPLACEFLAG_FAMLIYFASHION)// ����ʱװ
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_HEAD && index == EQUIPPLACEFLAG_FASHIONHEAD)// ͷ��ʱװ
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_BODY && index == EQUIPPLACEFLAG_FASHIONBODY)// ����ʱװ
			return true;
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_BACK && index == EQUIPPLACEFLAG_FASHIONBACK)// ʱװ��
			return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��װ���Ĳ�λλ��
S32 Res::getEquipPos()
{
	if(mBaseData)
	{
		if(isTrump())
			return EQUIPPLACEFLAG_TRUMP;		// ����
		else if(isWeapon() && !isTrump())
			return EQUIPPLACEFLAG_WEAPON;		// ����
		//else if(isRide())
		//	return EQUIPPLACEFLAG_RIDE;			// �������
		//else if(isPet())
		//	return EQUIPPLACEFLAG_PET;			// �������
		else if(isGather())
			return EQUIPPLACEFLAG_GATHER;		// �ɼ�����
		else if(mBaseData->mSubCategory == CATEGORY_HEAD)
			return EQUIPPLACEFLAG_HEAD;			// ͷ��
		else if(mBaseData->mSubCategory == CATEGORY_NECK)
			return EQUIPPLACEFLAG_NECK;			// ����
		else if(mBaseData->mSubCategory == CATEGORY_BACK)
			return EQUIPPLACEFLAG_BACK;			// ����
		else if(mBaseData->mSubCategory == CATEGORY_SHOULDER)
			return EQUIPPLACEFLAG_SHOULDER;		// �粿
		else if(mBaseData->mSubCategory == CATEGORY_BODY)
			return EQUIPPLACEFLAG_BODY;			// ����
		else if(mBaseData->mSubCategory == CATEGORY_HAND)
			return EQUIPPLACEFLAG_HAND;			// �ֲ�
		else if(mBaseData->mSubCategory == CATEGORY_WAIST)
			return EQUIPPLACEFLAG_WAIST;		// ����
		else if(mBaseData->mSubCategory == CATEGORY_FOOT)
			return EQUIPPLACEFLAG_FOOT;			// �Ų�
		else if(mBaseData->mSubCategory == CATEGORY_RING)
			return EQUIPPLACEFLAG_RING;			// ��ָ
		else if(mBaseData->mSubCategory == CATEGORY_AMULET)
			return EQUIPPLACEFLAG_AMULET;		// �����
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_FAMILY)
			return EQUIPPLACEFLAG_FAMLIYFASHION;	// ����ʱװ
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_HEAD)
			return EQUIPPLACEFLAG_FASHIONHEAD;	// ͷ��ʱװ
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_BODY)
			return EQUIPPLACEFLAG_FASHIONBODY;	// ����ʱװ
		else if(mBaseData->mSubCategory == CATEGORY_FASHION_BACK)
			return EQUIPPLACEFLAG_FASHIONBACK;	// ����ʱװ

	}
	return -1;	
}

// ----------------------------------------------------------------------------
// �ɷ���
bool Res::canTrade()
{
	// �Ƿ��ױ���/����װ����/��
	if(isEquip() && mExtData && mExtData->BindPlayer)
		return false;

	//�Ƿ�ɽ���
	return canBaseLimit(ITEM_TRADE);
}


// ============================================================================
// ��չ���ݷ���
// ============================================================================
// ----------------------------------------------------------------------------
// ������Ʒ����չ����
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

			// �����������ԣ���ʹ���������Ҳ���ܴ��ڼ����������ԣ�
			for(S32 i = 0; i < MAX_IDEPROS; i++)
			{
				if(mBaseData->mAppendAttribute[i] != 0)
				{
					mExtData->IDEProNum++;
					mExtData->IDEProValue[i] = 302000000 + mBaseData->mAppendAttribute[i] * 10000 +
						mBaseData->mQualityLevel;
				}
			}

			// װ��ǿ���ȼ���Ч������
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
// ��ȡ��Ʒ������
S32 Res::getQuantity()
{
	return mExtData ? mExtData->Quantity : 0;
}

// ----------------------------------------------------------------------------
// ������Ʒ������
void Res::setQuantity(S32 num, bool ignorecheck/*=false*/)
{
	if(!mExtData || !mBaseData || num <0)
		return;
	if(ignorecheck || (!ignorecheck && num <= mBaseData->mMaxOverNum))
		mExtData->Quantity = num;
}



// ----------------------------------------------------------------------------
// ��ȡ��Ʒ��ʵ�ʼ۸�
S32 Res::getActualPrice()
{
	//�����;öȼ����ֵ�����
	return 0;
}


// ----------------------------------------------------------------------------
// �����Ʒ��������ID
U32 Res::getSubPropertyID(U32 statsid)
{
	return (statsid % 10000000) / 1000;
}

// ----------------------------------------------------------------------------
// ��ñ�ʯ��������
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

	if (isWeapon())			//������
	{
		limit = (U32)Stone_Faqi << (GETSUB(getSubCategory()) - 1);
	}
	else if (isEquipment())	//������
	{
		limit = (U32)Stone_Head << (GETSUB(getSubCategory()) - 1);
	}
	else if (isOrnament())	//��Ʒ��
	{
		limit = (U32)Stone_Amulet << (GETSUB(getSubCategory()) - 1);
	}
	else if (isTrump())		//������
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
	//������࿪������
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
	//������࿪������
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

	//�жϵ�ǰλ���Ƿ񱻴��
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

	//�жϵ�ǰλ���Ƿ������Ƕ��ʯ(��׵�δ��Ƕ)
	stItemInfo *pItemInfo = getExtData();
	if (!pItemInfo)
		return false;

	return ( pItemInfo->EmbedSlot[nIndex - 1] == 0 );
}

bool Res::IsSlotEmbeded(S32 nIndex)
{
	if ( !IsSlotOpened(nIndex)  || !mExtData)
		return false;

	//�жϵ�ǰλ���Ƿ��Ѿ���Ƕ��ʯ
	stItemInfo *pItemInfo = getExtData();

	return ( pItemInfo->EmbedSlot[nIndex - 1] != 0 );
}

// ----------------------------------------------------------------------------
// ��Ʒ����ɫ��ȡ��Ʒ����
void Res::getItemName(char* colorName, U32 size)
{
	dSprintf(colorName, size, "<t m='0' c='0x%x'>%s</t>", 
		sColorValue[getColorLevel()], getItemName());
}

// ----------------------------------------------------------------------------
// ����װ���;�
bool Res::setRepairResult(U32 type)
{
	if(getCurrentMaxWear() == -1)		//����ǿ��װ������
	{
		setCurrentWear(getMaxWear());
	}
	else 
	{
		if(type == REPAIR_RECOVER)		//�ָ�����
		{
			//���ָ�����ָ���ģ������;ö�
			setCurrentMaxWear(getMaxWear());
			setCurrentWear(getCurrentMaxWear());
		}
		else if(type == REPAIR_DURABLE)	//��������
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
			// �;����������
			S32 hitRate = baseRate +  (getCurrentMaxWear()- getCurrentWear())  / 

				getCurrentMaxWear() * 50;
			if(randNum < hitRate)
			{
				//δ����ǿ����װ��������ǰ����;�=��ǰ����;� - (����;�/4)
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
// ������ͨ�����Ǯ
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
// ���㾭�������Ǯ
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
