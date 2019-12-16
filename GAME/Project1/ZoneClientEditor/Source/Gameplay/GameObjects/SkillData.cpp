//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/SkillData.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/GameplayCommon.h"
#include "gui/core/guiTypes.h"

// ========================================================================================================================================
//  SkillData
// ========================================================================================================================================

#define NAME_TEXTURE_FILE_PATH "gameres/gui/images/"
StringTableEntry SkillData::sm_SchoolName[] =
{
	"综合",
	"木",
	"火",
	"土",
	"金",
	"水",
	"变身",
};

SkillData g_SkillData;

SkillData::SkillData()
{
	Clear();
}

SkillData::~SkillData()
{
#ifdef NTJ_CLIENT
	SAFE_DELETE(m_NameTexHandle);
#endif
	Clear();
}

const GFXTexHandle* SkillData::GetNameTexHandle()
{
#ifdef NTJ_CLIENT
	return m_NameTexHandle;
#else
	return NULL;
#endif
}

void SkillData::Plus(const SkillData& _orig, const SkillData& _plus)
{
	m_CastLimit.rangeMin = getMax(0.0f, _orig.m_CastLimit.rangeMin + _plus.m_CastLimit.rangeMin);
	m_CastLimit.rangeMax = getMax(0.0f, _orig.m_CastLimit.rangeMax + _plus.m_CastLimit.rangeMax);
	if(m_CastLimit.rangeMin > m_CastLimit.rangeMax)
		_swap(m_CastLimit.rangeMin, m_CastLimit.rangeMax);
	m_EffectLimit.maxTargets = getMax((U32)1, _orig.m_EffectLimit.maxTargets + _plus.m_EffectLimit.maxTargets);
	m_EffectLimit.regionRect = _orig.m_EffectLimit.regionRect + _plus.m_EffectLimit.regionRect;
	m_Cost.HP = getMax(0, _orig.m_Cost.HP + _plus.m_Cost.HP);
	m_Cost.MP = getMax(0, _orig.m_Cost.MP + _plus.m_Cost.MP);
	m_Cost.PP = getMax(0, _orig.m_Cost.PP + _plus.m_Cost.PP);
	m_Cost.vigor = getMax(0, _orig.m_Cost.vigor + _plus.m_Cost.vigor);
	m_Cost.vigour = getMax(0, _orig.m_Cost.vigour + _plus.m_Cost.vigour);
	m_Cast.readyTime = getMax(0, (S32)_orig.m_Cast.readyTime + (S32)_plus.m_Cast.readyTime);
	m_Cast.channelTime = getMax(0, (S32)_orig.m_Cast.channelTime + (S32)_plus.m_Cast.channelTime);
	m_Cast.cooldown = getMax(0, (S32)_orig.m_Cast.cooldown + (S32)_plus.m_Cast.cooldown);

	for (S32 i=0; i<SelectableNum; ++i)
	{
		m_Trigger[i].rate = _orig.m_Trigger[i].rate + _plus.m_Trigger[i].rate;
		m_Trigger[i].buffCount_A = _orig.m_Trigger[i].buffCount_A + _plus.m_Trigger[i].buffCount_A;
		m_Trigger[i].buffCount_B = _orig.m_Trigger[i].buffCount_B + _plus.m_Trigger[i].buffCount_B;
		m_Trigger[i].buffCount_C = _orig.m_Trigger[i].buffCount_C + _plus.m_Trigger[i].buffCount_C;

		if (_orig.m_SkillId)
		{
			SkillData* pData = g_SkillRepository.GetSkill(_orig.m_SkillId);
			if(!pData)
				continue;
			if(_plus.m_Trigger[i].skillId_A)
			{
				if(_orig.m_Trigger[i].skillId_A)
				{
					AssertFatal(pData->m_Trigger[i].skillId_A == _orig.m_Trigger[i].skillId_A, "SkillData::Plus error!");
					m_Trigger[i].skillId_A = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_A), Macro_GetSkillLevel(_orig.m_Trigger[i].skillId_A));
				}
				else
					m_Trigger[i].skillId_A = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_A), _orig.m_Level);
			}
			if(_plus.m_Trigger[i].skillId_B)
			{
				if(_orig.m_Trigger[i].skillId_B)
				{
					AssertFatal(pData->m_Trigger[i].skillId_B == _orig.m_Trigger[i].skillId_B, "SkillData::Plus error!");
					m_Trigger[i].skillId_B = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_B), Macro_GetSkillLevel(_orig.m_Trigger[i].skillId_B));
				}
				else
					m_Trigger[i].skillId_B = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_B), _orig.m_Level);
			}
			if(_plus.m_Trigger[i].skillId_C)
			{
				if(_orig.m_Trigger[i].skillId_C)
				{
					AssertFatal(pData->m_Trigger[i].skillId_C == _orig.m_Trigger[i].skillId_C, "SkillData::Plus error!");
					m_Trigger[i].skillId_C = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_C), Macro_GetSkillLevel(_orig.m_Trigger[i].skillId_C));
				}
				else
					m_Trigger[i].skillId_C = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_C), _orig.m_Level);
			}
			if(_plus.m_Trigger[i].buffId_A)
			{
				if(_orig.m_Trigger[i].buffId_A)
				{
					AssertFatal(pData->m_Trigger[i].buffId_A == _orig.m_Trigger[i].buffId_A, "SkillData::Plus error!");
					m_Trigger[i].buffId_A = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_A), Macro_GetSkillLevel(_orig.m_Trigger[i].buffId_A));
				}
				else
					m_Trigger[i].buffId_A = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_A), _orig.m_Level);
			}
			if(_plus.m_Trigger[i].buffId_B)
			{
				if(_orig.m_Trigger[i].buffId_B)
				{
					AssertFatal(pData->m_Trigger[i].buffId_B == _orig.m_Trigger[i].buffId_B, "SkillData::Plus error!");
					m_Trigger[i].buffId_B = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_B), Macro_GetSkillLevel(_orig.m_Trigger[i].buffId_B));
				}
				else
					m_Trigger[i].buffId_B = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_B), _orig.m_Level);
			}
			if(_plus.m_Trigger[i].buffId_C)
			{
				if(_orig.m_Trigger[i].buffId_C)
				{
					AssertFatal(pData->m_Trigger[i].buffId_C == _orig.m_Trigger[i].buffId_C, "SkillData::Plus error!");
					m_Trigger[i].buffId_C = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_C), Macro_GetSkillLevel(_orig.m_Trigger[i].buffId_C));
				}
				else
					m_Trigger[i].buffId_C = Macro_GetSkillId( Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_C), _orig.m_Level);
			}
		}
	}
}

void SkillData::Minus(const SkillData& _orig, const SkillData& _plus)
{
	m_CastLimit.rangeMin = getMax(0.0f, _orig.m_CastLimit.rangeMin - _plus.m_CastLimit.rangeMin);
	m_CastLimit.rangeMax = getMax(0.0f, _orig.m_CastLimit.rangeMax - _plus.m_CastLimit.rangeMax);
	if(m_CastLimit.rangeMin > m_CastLimit.rangeMax)
		_swap(m_CastLimit.rangeMin, m_CastLimit.rangeMax);
	m_EffectLimit.maxTargets = getMax((U32)1, _orig.m_EffectLimit.maxTargets - _plus.m_EffectLimit.maxTargets);
	m_EffectLimit.regionRect = _orig.m_EffectLimit.regionRect - _plus.m_EffectLimit.regionRect;
	m_Cost.HP = getMax(0, _orig.m_Cost.HP - _plus.m_Cost.HP);
	m_Cost.MP = getMax(0, _orig.m_Cost.MP - _plus.m_Cost.MP);
	m_Cost.PP = getMax(0, _orig.m_Cost.PP - _plus.m_Cost.PP);
	m_Cost.vigor = getMax(0, _orig.m_Cost.vigor - _plus.m_Cost.vigor);
	m_Cost.vigour = getMax(0, _orig.m_Cost.vigour - _plus.m_Cost.vigour);
	m_Cast.readyTime = getMax(0, (S32)_orig.m_Cast.readyTime - (S32)_plus.m_Cast.readyTime);
	m_Cast.channelTime = getMax(0, (S32)_orig.m_Cast.channelTime - (S32)_plus.m_Cast.channelTime);
	m_Cast.cooldown = getMax(0, (S32)_orig.m_Cast.cooldown - (S32)_plus.m_Cast.cooldown);

	for (S32 i=0; i<SelectableNum; ++i)
	{
		m_Trigger[i].rate = _orig.m_Trigger[i].rate - _plus.m_Trigger[i].rate;
		m_Trigger[i].buffCount_A = _orig.m_Trigger[i].buffCount_A - _plus.m_Trigger[i].buffCount_A;
		m_Trigger[i].buffCount_B = _orig.m_Trigger[i].buffCount_B - _plus.m_Trigger[i].buffCount_B;
		m_Trigger[i].buffCount_C = _orig.m_Trigger[i].buffCount_C - _plus.m_Trigger[i].buffCount_C;

		if (_orig.m_SkillId)
		{
			SkillData* pData = g_SkillRepository.GetSkill(_orig.m_SkillId);
			if(!pData)
				continue;
			if(_plus.m_Trigger[i].skillId_A)
			{
				AssertFatal(Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_A) == Macro_GetSkillSeriesId(_orig.m_Trigger[i].skillId_A), "SkillData::Plus error!");
				m_Trigger[i].skillId_A = pData->m_Trigger[i].skillId_A;
			}
			if(_plus.m_Trigger[i].skillId_B)
			{
				AssertFatal(Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_B) == Macro_GetSkillSeriesId(_orig.m_Trigger[i].skillId_B), "SkillData::Plus error!");
				m_Trigger[i].skillId_B = pData->m_Trigger[i].skillId_B;
			}
			if(_plus.m_Trigger[i].skillId_C)
			{
				AssertFatal(Macro_GetSkillSeriesId(_plus.m_Trigger[i].skillId_C) == Macro_GetSkillSeriesId(_orig.m_Trigger[i].skillId_C), "SkillData::Plus error!");
				m_Trigger[i].skillId_C = pData->m_Trigger[i].skillId_C;
			}
			if(_plus.m_Trigger[i].buffId_A)
			{
				AssertFatal(Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_A) == Macro_GetSkillSeriesId(_orig.m_Trigger[i].buffId_A), "SkillData::Plus error!");
				m_Trigger[i].buffId_A = pData->m_Trigger[i].buffId_A;
			}
			if(_plus.m_Trigger[i].buffId_B)
			{
				AssertFatal(Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_B) == Macro_GetSkillSeriesId(_orig.m_Trigger[i].buffId_B), "SkillData::Plus error!");
				m_Trigger[i].buffId_B = pData->m_Trigger[i].buffId_B;
			}
			if(_plus.m_Trigger[i].buffId_C)
			{
				AssertFatal(Macro_GetSkillSeriesId(_plus.m_Trigger[i].buffId_C) == Macro_GetSkillSeriesId(_orig.m_Trigger[i].buffId_C), "SkillData::Plus error!");
				m_Trigger[i].buffId_C = pData->m_Trigger[i].buffId_C;
			}
		}
	}
}

void SkillData::PlusDone()
{
}


// ========================================================================================================================================
//  SkillRepository
// ========================================================================================================================================

SkillRepository g_SkillRepository;

SkillRepository::SkillRepository()
{
}

SkillRepository::~SkillRepository()
{
	Clear();
}

bool SkillRepository::Read()
{
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取BUFF数据
	Platform::makeFullPathName(GAME_SKILLDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can NOT read file : SkillRepository.dat!");

	//std::bitset<BUFF_GROUP_BITS> tempBit;
	for(int i = 0 ; i < file.RecordNum ; ++ i)
	{
		SkillData* pData = new SkillData;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_SeriesId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_Level = tempData.m_U16;

		pData->m_SkillId = Macro_GetSkillId(pData->m_SeriesId, pData->m_Level);

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
		//file.GetData(tempData);
		//AssertRelease(tempData.m_Type == DType_string, "SkillRepository.dat::Read !");
		//pData->m_NameTexture = StringTable->insert(tempData.m_string);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "SkillRepository.dat::Read !");
		pData->m_School = (SkillData::School)tempData.m_Enum8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Flags = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Effect = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_LearnLimit.prepSkill = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "SkillRepository.dat::Read !");
		pData->m_LearnLimit.level = tempData.m_U8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_LearnLimit.money = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_LearnLimit.exp = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_LearnLimit.item = tempData.m_U32;
		//主系门宗限制 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_LearnLimit.family[0] = tempData.m_U16;
		//辅系门宗限制
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_LearnLimit.family[1] = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "SkillRepository.dat::Read !");
		pData->m_LearnLimit.scriptFunction = StringTable->insert(tempData.m_string);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "SkillRepository.dat::Read !");
		pData->m_CastLimit.object = (SkillData::Object)tempData.m_Enum8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_CastLimit.targetSl[0] = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_CastLimit.targetSl[1] = tempData.m_U32;

		pData->m_CastLimit.target = pData->m_CastLimit.targetSl[0] | pData->m_CastLimit.targetSl[1];
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "SkillRepository.dat::Read !");
		pData->m_CastLimit.rangeMin = tempData.m_F32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "SkillRepository.dat::Read !");
		pData->m_CastLimit.rangeMax = tempData.m_F32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.armStatus = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.srcPreBuff_A = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.srcPreBuffCount_A = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.srcPreBuff_B = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.srcPreBuffCount_B = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "SkillRepository.dat::Read !");
		pData->m_CastLimit.srcPreBuffOp = tempData.m_U8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.tgtPreBuff_A = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.tgtPreBuffCount_A = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.tgtPreBuff_B = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_CastLimit.tgtPreBuffCount_B = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "SkillRepository.dat::Read !");
		pData->m_CastLimit.tgtPreBuffOp = tempData.m_U8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_EffectLimit.effectTarget[0] = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_EffectLimit.effectTarget[1] = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "SkillRepository.dat::Read !");
		pData->m_EffectLimit.maxTargets = tempData.m_U8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "SkillRepository.dat::Read !");
		pData->m_EffectLimit.region = (SkillData::Region)tempData.m_Enum8;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "SkillRepository.dat::Read !");
		pData->m_EffectLimit.regionRect.x = tempData.m_F32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "SkillRepository.dat::Read !");
		pData->m_EffectLimit.regionRect.y = tempData.m_F32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_Cost.HP = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_Cost.MP = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_Cost.PP = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_Cost.vigor = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_Cost.vigour = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Cost.item = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_Cast.readyTime = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "SkillRepository.dat::Read !");
		pData->m_Cast.channelTime = tempData.m_U16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Cast.cooldown = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S16, "SkillRepository.dat::Read !");
		pData->m_Cast.cdGroup = tempData.m_S16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum16, "SkillRepository.dat::Read !");
		pData->m_Cast.readySpell = (GameObjectData::Animations)tempData.m_Enum16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum16, "SkillRepository.dat::Read !");
		pData->m_Cast.spellCast = (GameObjectData::Animations)tempData.m_Enum16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum16, "SkillRepository.dat::Read !");
		pData->m_Cast.spellCastCritical = (GameObjectData::Animations)tempData.m_Enum16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum16, "SkillRepository.dat::Read !");
		pData->m_Cast.channelCast = (GameObjectData::Animations)tempData.m_Enum16;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Cast.sourceEP = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Cast.targetEP = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Cast.sourceDelayEP = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Cast.targetDelayEP = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Cast.environmentDelayEP = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "SkillRepository.dat::Read !");
		pData->m_Cast.projectileVelocity = getMax(4.0f, tempData.m_F32);
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "SkillRepository.dat::Read !");
		pData->m_Cast.attenuation = tempData.m_F32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "SkillRepository.dat::Read !");
		pData->m_Cast.hate = tempData.m_S32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "SkillRepository.dat::Read !");
		pData->m_Cast.modifyHate = tempData.m_F32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Operation[0].buffId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Operation[0].channelSrcBuff = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Operation[0].channelTgtBuff = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Operation[1].buffId = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Operation[1].channelSrcBuff = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Operation[1].channelTgtBuff = tempData.m_U32;
		// 
		//file.GetData(tempData);
		//AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		//pData->m_Trigger[0].rate = tempData.m_U32;
		pData->m_Trigger[0].rate = 5000;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].skillId_A = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].skillId_B = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].skillId_C = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].buffId_A = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].buffCount_A = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].buffId_B = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].buffCount_B = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].buffId_C = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].buffCount_C = 0; //tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "SkillRepository.dat::Read !");
		pData->m_Trigger[0].scriptFun = StringTable->insert(tempData.m_string);
		// 
		//file.GetData(tempData);
		//AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		//pData->m_Trigger[1].rate = tempData.m_U32;
		pData->m_Trigger[1].rate = 5000;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].skillId_A = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].skillId_B = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].skillId_C = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].buffId_A = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].buffCount_A = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].buffId_B = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].buffCount_B = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].buffId_C = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].buffCount_C = tempData.m_U32;
		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "SkillRepository.dat::Read !");
		pData->m_Trigger[1].scriptFun = StringTable->insert(tempData.m_string);

#ifdef NTJ_CLIENT
		// 载入所以名称纹理
		// 这种方法不是很好，期待某个勤快的人帮我改好
		if(!isNullString(pData->GetNameTexture()))
		{
			GFXTexHandle* pHandle = new GFXTexHandle(avar(NAME_TEXTURE_FILE_PATH "%s", pData->GetNameTexture()), &GFXDefaultGUIProfile);
			if(pHandle->isValid())
			{
				pData->m_NameTexHandle = pHandle;
			}
			else
			{
				delete pHandle;
				pData->m_NameTexHandle = NULL;
			}
		}
#endif

		Insert(*pData);
	}
	return true;
}

void SkillRepository::Clear()
{
	SkillDataMap::iterator pos;
	for(pos = m_SkillDataMap.begin(); pos != m_SkillDataMap.end(); ++pos)
		delete pos->second;
	m_SkillDataMap.clear();
}

SkillData* SkillRepository::GetSkill(U32 SkillId)
{
	SkillDataMap::iterator itr = m_SkillDataMap.find(SkillId);
	if(itr == m_SkillDataMap.end())
		return NULL;
	return itr->second;
}

SkillData* SkillRepository::GetSkill(U32 seriesId, U32 level)
{
	SkillDataMap::iterator itr = m_SkillDataMap.find(Macro_GetSkillId(seriesId,level));
	if(itr == m_SkillDataMap.end())
		return NULL;
	return itr->second;
}

bool SkillRepository::Insert(SkillData& _Skill)
{
	if(GetSkill(_Skill.m_SkillId))
		return false;
	m_SkillDataMap.insert(SkillDataMap::value_type(_Skill.m_SkillId, &_Skill));
	// Flags_Learnable
	//if(_Skill.IsFlags(SkillData::Flags_Learnable))
		m_SkillDataSeriesMap[_Skill.GetSchool()].insert( _Skill.GetSeriesID() );
	return true;
}

SkillRepository::SkillDataSeriesMap* SkillRepository::GetSeriesMap()
{
	return &m_SkillDataSeriesMap;
}



