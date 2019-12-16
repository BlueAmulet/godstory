#include "Common/PlayerStruct.h"

void stPlayerDisp::InitData()
{
	memset(this, 0, sizeof(stPlayerDisp));
	Race = Race_Ren;
	Family = Family_None;
	Level = 1;
	OtherFlag = 1;
	CopyMapInstId = 1;
}

void stItemInfo::InitData()
{
	dMemset(this, 0, sizeof(stItemInfo));
}

void stPlayerMain::InitData()
{
	CurrentHP		= 0xFFFFFF;
	CurrentMP		= 0xFFFFFF;
	CurrentPP		= 0xFFFFFF;
	CurrentVigor	= 100;
	MaxVigor        = 100;
	CurrentExp		= 0; 
	CurrentLivingExp = 0;
	LivingLevel     = 0;
	Money			= 0;
	BindMoney		= 0;
	BankMoney		= 0;
	TradeLockFlag	= 0;
	SecondPassword[0] = 0;
	PrescriptionCount = 0;

	playerHot		= 0;
	SocialItemCount = 0;

	MasterLevel = 0;
	CurrMasterExp = 0;
	ExpPool = 0;
	bPrentice = 0;
	bFarPrentice = 0;
	MPMStudyLevel = 0;

	memset( &MasterText,0,sizeof(MasterText) );
	memset( &SocialItem, 0, sizeof( stSocialItem) * SOCAIL_ITEM_MAX );

	memset(PlayerDataFlag, 0, sizeof(PlayerDataFlag));
	LivingSkillStudyID = 0;
	TimerTriggerCount = 0;
	TimerPetStudyCount = 0;
	memset(PrescriptionInfo,0,sizeof(PrescriptionInfo));
	memset(TimerTriggerInfo, 0, sizeof(TimerTriggerInfo));
	memset(TimerPetStudyInfo, 0, sizeof(TimerPetStudyInfo));
	SpiritOpenedFlag = 0;
	SpiritActiveSlot = -1;
	PlayerStructVer = PLAYERSTRUCT_VERSION;
	BankBagSize		= 0;
}
