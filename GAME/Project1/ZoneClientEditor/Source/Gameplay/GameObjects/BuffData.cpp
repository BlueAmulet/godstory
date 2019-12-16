//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/Buff.h"
#include "Gameplay/data/readDataFile.h"


// ========================================================================================================================================
//  BuffData
// ========================================================================================================================================
BuffData g_BuffData;

BuffData::BuffData()
{
	Clear();
}

void BuffData::Plus(const BuffData& _orig, const BuffData& _plus)
{
	m_Stats.Plus(_orig.m_Stats, _plus.m_Stats);
	m_LimitTime = _orig.m_LimitTime + _plus.m_LimitTime;
	m_LimitTimes = _orig.m_LimitTimes + _plus.m_LimitTimes;
	m_LimitCount = _orig.m_LimitCount + _plus.m_LimitCount;
}

void BuffData::PlusDone()
{
	m_Stats.PlusDone();
}


// ========================================================================================================================================
//  BuffRepository
// ========================================================================================================================================

BuffRepository g_BuffRepository;

BuffRepository::BuffRepository()
{
}

BuffRepository::~BuffRepository()
{
	Clear();
}

bool BuffRepository::Read()
{
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取BUFF数据
	Platform::makeFullPathName(GAME_BUFFDATA_FILE,filename,sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can NOT read file : BuffRepository.dat!");

	StringTableEntry ZeroString = StringTable->insert("0");
	StringTableEntry NullString = StringTable->insert("");
	std::bitset<BUFF_GROUP_BITS> tempBit;
	for(int i = 0 ; i < file.RecordNum ; ++ i)
	{
		BuffData* pData = new BuffData;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "BuffRepository.dat::Read !");
		pData->m_SeriesId = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "BuffRepository.dat::Read !");
		pData->m_Level = tempData.m_U8;

		pData->m_BuffId = Macro_GetBuffId(pData->m_SeriesId, pData->m_Level);

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "SkillRepository.dat::Read !");
		pData->m_Name = StringTable->insert(tempData.m_string);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "SkillRepository.dat::Read !");
		pData->m_Icon = StringTable->insert(tempData.m_string);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "SkillRepository.dat::Read !");
		pData->m_Text = StringTable->insert(tempData.m_string);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_PlusId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_BuffEffect = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_Flags = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "BuffRepository.dat::Read !");
		pData->m_DamageType = tempData.m_U8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "BuffRepository.dat::Read !");
		pData->m_PlusSrcRate = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_CountMask = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_RemoveMask = tempData.m_U32;
		pData->m_TotalMask = pData->m_CountMask | pData->m_RemoveMask;
		// 触发
		for(S32 i=0; i<BuffData::TriggerMax; ++i)
		{
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].mask = tempData.m_U32;
			pData->m_TotalMask |= pData->m_Trigger[i].mask;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].rate = tempData.m_U32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_enum8, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].skillTT_A = (BuffData::TriggerTarget)tempData.m_Enum8;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].skillId_A = tempData.m_U32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_enum8, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].skillTT_B = (BuffData::TriggerTarget)tempData.m_Enum8;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].skillId_B = tempData.m_U32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_enum8, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].skillTT_C = (BuffData::TriggerTarget)tempData.m_Enum8;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].skillId_C = tempData.m_U32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_enum8, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffTT_A = (BuffData::TriggerTarget)tempData.m_Enum8;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffId_A = tempData.m_U32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_S32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffCount_A = tempData.m_S32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_enum8, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffTT_B = (BuffData::TriggerTarget)tempData.m_Enum8;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffId_B = tempData.m_U32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_S32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffCount_B = tempData.m_S32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_enum8, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffTT_C = (BuffData::TriggerTarget)tempData.m_Enum8;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffId_C = tempData.m_U32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_S32, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].buffCount_C = tempData.m_S32;
			// 
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_string, "BuffRepository.dat::Read !");
			pData->m_Trigger[i].scriptFun = StringTable->insert(tempData.m_string);
		}
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "BuffRepository.dat::Read !");
		pData->m_ShieldInfo.absorbRate = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "BuffRepository.dat::Read !");
		pData->m_ShieldInfo.triggerRate = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_LimitCount = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "BuffRepository.dat::Read !");
		pData->m_LimitTimes = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_LimitTime = tempData.m_U32;

		// m_Group
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_Group |= tempData.m_U32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 32;
		pData->m_Group |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 64;
		pData->m_Group |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 96;
		pData->m_Group |= tempBit;

		// m_RemoveGroupA
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_RemoveGroupA |= tempData.m_U32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 32;
		pData->m_RemoveGroupA |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 64;
		pData->m_RemoveGroupA |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 96;
		pData->m_RemoveGroupA |= tempBit;

		// m_RemoveGroupB
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_RemoveGroupB |= tempData.m_U32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 32;
		pData->m_RemoveGroupB |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 64;
		pData->m_RemoveGroupB |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 96;
		pData->m_RemoveGroupB |= tempBit;

		// m_RemoveGroupC
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_RemoveGroupC |= tempData.m_U32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 32;
		pData->m_RemoveGroupC |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 64;
		pData->m_RemoveGroupC |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 96;
		pData->m_RemoveGroupC |= tempBit;

		// m_ImmunityGroup
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_ImmunityGroup |= tempData.m_U32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 32;
		pData->m_ImmunityGroup |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 64;
		pData->m_ImmunityGroup |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 96;
		pData->m_ImmunityGroup |= tempBit;

		// m_RestrainGroup
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_RestrainGroup |= tempData.m_U32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 32;
		pData->m_RestrainGroup |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 64;
		pData->m_RestrainGroup |= tempBit;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		tempBit.reset();
		tempBit |= tempData.m_U32;
		tempBit <<= 96;
		pData->m_RestrainGroup |= tempBit;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_BuffEP = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "BuffRepository.dat::Read !");
		pData->m_ShapesSetId = tempData.m_U32;
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "BuffRepository.dat::Read !");
		pData->m_OnAddScript = StringTable->insert(tempData.m_string);
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "BuffRepository.dat::Read !");
		pData->m_OnRemoveScript = StringTable->insert(tempData.m_string);
		if(pData->m_OnAddScript == ZeroString)
			pData->m_OnAddScript = NullString;
		if(pData->m_OnRemoveScript == ZeroString)
			pData->m_OnRemoveScript = NullString;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "BuffRepository.dat::Read !");
		pData->m_Stats.HP = tempData.m_S32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HP_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HP_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "BuffRepository.dat::Read !");
		pData->m_Stats.MP = tempData.m_S32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MP_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MP_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PP = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PP_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PP_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxHP = tempData.m_S32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxHP_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxHP_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxMP = tempData.m_S32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxMP_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxMP_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxPP = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxPP_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MaxPP_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Stamina = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Stamina_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Stamina_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Mana = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Mana_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Mana_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Strength = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Strength_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Strength_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Intellect = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Intellect_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Intellect_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Agility = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Agility_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Agility_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Pneuma = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Pneuma_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Pneuma_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Insight = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Insight_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Insight_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Luck = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Luck_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Luck_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Velocity_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MountedVelocity_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.AttackSpeed = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.AttackSpeed_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.SpellSpeed_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.CriticalRating = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.CriticalTimes = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Critical_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.AntiCriticalRating = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.AntiCritical_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.FatalRating = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Fatal_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.DodgeRating = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Dodge_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HitRating = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Hit_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.DamageHandle_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PhyDamage = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PhyDamage_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PhyDamage_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PhyDefence = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PhyDefence_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PhyDefence_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MuDamage = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MuDamage_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MuDamage_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MuDefence = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MuDefence_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.MuDefence_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HuoDamage = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HuoDamage_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HuoDamage_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HuoDefence = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HuoDefence_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HuoDefence_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.TuDamage = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.TuDamage_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.TuDamage_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.TuDefence = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.TuDefence_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.TuDefence_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.JinDamage = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.JinDamage_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.JinDamage_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.JinDefence = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.JinDefence_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.JinDefence_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.ShuiDamage = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.ShuiDamage_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.ShuiDamage_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.ShuiDefence = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.ShuiDefence_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.ShuiDefence_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PneumaDamage = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PneumaDamage_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PneumaDamage_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PneumaDefence = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PneumaDefence_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PneumaDefence_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Heal = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Heal_Pc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.Heal_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.PhyScale_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.SplScale_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HealScale_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.HateScale_gPc = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.DeriveHP_gPc= tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.DeriveMP_gPc= tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "BuffRepository.dat::Read !");
		pData->m_Stats.DerivePP_gPc= tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S8, "BuffRepository.dat::Read !");
		pData->m_Stats.Mu = tempData.m_S8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S8, "BuffRepository.dat::Read !");
		pData->m_Stats.Huo = tempData.m_S8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S8, "BuffRepository.dat::Read !");
		pData->m_Stats.Tu = tempData.m_S8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S8, "BuffRepository.dat::Read !");
		pData->m_Stats.Jin = tempData.m_S8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S8, "BuffRepository.dat::Read !");
		pData->m_Stats.Shui = tempData.m_S8;

		if ( !Insert(*pData) )
		{
			delete pData;
		}
	}

	return true;
}

void BuffRepository::Clear()
{
	BuffDataMap::iterator pos;
	for(pos = m_BuffDataMap.begin(); pos != m_BuffDataMap.end(); ++pos)
		delete pos->second;
	m_BuffDataMap.clear();
}

BuffData* BuffRepository::GetBuff(U32 BuffId)
{
	BuffDataMap::iterator itr = m_BuffDataMap.find(BuffId);
	if(itr == m_BuffDataMap.end())
		return NULL;
	return itr->second;
}

Stats* BuffRepository::GetBuffStats(U32 BuffId)
{
	BuffDataMap::iterator itr = m_BuffDataMap.find(BuffId);
	if(itr == m_BuffDataMap.end())
		return NULL;
	AssertFatal(itr->second, "BuffRepository::GetBuffStats !");
	return &(itr->second->m_Stats);
}

bool BuffRepository::Insert(BuffData& _buff)
{
	if(GetBuff(_buff.m_BuffId))
		return false;
	m_BuffDataMap.insert(BuffDataMap::value_type(_buff.m_BuffId, &_buff));
	return true;
}



