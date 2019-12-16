//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/gameFunctions.h"
#include "Gameplay/GameObjects/PetObjectData.h"
#include "Gameplay/GameObjects/Skill.h"
#include "console/consoleTypes.h"

#include "platform/platform.h"
#include "core/bitStream.h"
#include "ts/tsShapeInstance.h"
#include "Gameplay/data/readDataFile.h"
#include "Util/ColumnData.h"

// ========================================================================================================================================
//	PetObjectData
// ========================================================================================================================================

IMPLEMENT_CO_DATABLOCK_V1(PetObjectData);

PetObjectData::PetObjectData()
{
	bInitialized = false;
	maxForwardSpeed = 5.0f;
	flags = 0;
	ability = 0;
	petName = NULL;
	petIcon = NULL;
	race = Race_YeShou;
	lives = 0;
	buffId = 0;
	defaultSkillId = 0;
	combatPetIdx = 0;

	defaultSkillId = DefaultSkill_NPC;
}

PetObjectData::~PetObjectData()
{
}

bool PetObjectData::preload(bool server, char errorBuffer[256])
{
	if(!Parent::preload(server, errorBuffer))
		return false;

	return true;
}

U32 PetObjectData::getRandBuffId()
{
	U32 result = 0;
	S32 nCount = 0;
	S32 values[28];

	for (S32 i = 0; i < 28; i++)
	{
		if (initialTemplate & BIT(i))
		{
			values[nCount++] = i;
		}
	}
	if (nCount == 0)
	{
		return result;
	}

	S32 index = Platform::getRandomI(0, nCount-1);
	if (values[index] <= 3)
	{
		result = values[index] + 1;
	}
	else if (values[index] <= 11)
	{
		result = values[index] - 4 + 11;
	}
	else if (values[index] <= 19)
	{
		result = values[index] - 12 + 21;
	}
	else if (values[index] <= 27)
	{
		result = values[index] - 20 + 31;
	}

	return (result + 30022 * 10000);
}

bool PetObjectData::initDataBlock()
{
	if(bInitialized)
		return true;

	bool server = false;
#ifdef NTJ_SERVER
	server = true;
#endif

	if(!Parent::initDataBlock())
		return false;

	bInitialized = true;
	return true;
}

// ========================================================================================================================================
//	PetRepository
// ========================================================================================================================================

PetRepository g_PetRepository;

PetRepository::PetRepository()
{
	m_PetDataMap.clear();
}

PetRepository::~PetRepository()
{
	Clear();
	if(mColumnData)
		delete mColumnData;
}

bool PetRepository::Read()
{
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取模型数据
	Platform::makeFullPathName("gameres/data/Repository/PetRepository.dat",filename,sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can't read file : PetRepository.dat!");

	mColumnData = new IColumnData(file.ColumNum, "PetRepository.dat");
	mColumnData->setField(CalcOffset(dataBlockId,			PetObjectData),	DType_U32,		"Pet编号");
	mColumnData->setField(CalcOffset(petName,				PetObjectData),	DType_string,	"默认名称");
	mColumnData->setField(CalcOffset(petShortcutIcon,		PetObjectData),	DType_string,	"Pet图标");
	mColumnData->setField(CalcOffset(shapesSetId,			PetObjectData),	DType_U32,		"模型组编号");
	mColumnData->setField(CalcOffset(petIcon,				PetObjectData),	DType_string,	"头像");
	mColumnData->setField(CalcOffset(race,					PetObjectData),	DType_enum8,	"种族");
	mColumnData->setField(CalcOffset(flags,					PetObjectData),	DType_U32,		"标志");
	mColumnData->setField(CalcOffset(ability,				PetObjectData),	DType_U32,		"能力");
	mColumnData->setField(CalcOffset(lives,					PetObjectData),	DType_U32,		"寿命");
	mColumnData->setField(CalcOffset(combatPetIdx,			PetObjectData),	DType_U8,		"战斗宠类型");
	mColumnData->setField(CalcOffset(initialTemplate,		PetObjectData),	DType_U32,		"战斗宠类型");
	mColumnData->setField(CalcOffset(buffId,				PetObjectData),	DType_U32,		"数值ID");
	mColumnData->setField(CalcOffset(defaultSkillId,		PetObjectData),	DType_U32,		"普通攻击技能ID");
	mColumnData->setField(CalcOffset(maxForwardSpeed,		PetObjectData),	DType_F32,		"攻击时的追击速度");
	mColumnData->setField(CalcOffset(spawnLevel,			PetObjectData),	DType_U8,		"携带等级");
	mColumnData->setField(CalcOffset(TameLevel,				PetObjectData),	DType_U8,		"捕捉等级");
	mColumnData->setField(CalcOffset(insight,				PetObjectData),	DType_U8,		"灵慧");
	mColumnData->setField(CalcOffset(petTalent,				PetObjectData),	DType_U8,		"根骨");
	mColumnData->setField(CalcOffset(minStrength,			PetObjectData),	DType_U16,		"力量资质最小值");
	mColumnData->setField(CalcOffset(maxStrength,			PetObjectData),	DType_U16,		"力量资质最大值");
	mColumnData->setField(CalcOffset(minLingLi,				PetObjectData),	DType_U16,		"灵力资质最小值");
	mColumnData->setField(CalcOffset(maxLingLi,				PetObjectData),	DType_U16,		"灵力资质最大值");
	mColumnData->setField(CalcOffset(minTiPo,				PetObjectData),	DType_U16,		"体魄资质最小值");
	mColumnData->setField(CalcOffset(maxTiPo,				PetObjectData),	DType_U16,		"体魄资质最大值");
	mColumnData->setField(CalcOffset(minJingLi,				PetObjectData),	DType_U16,		"精力资质最小值");
	mColumnData->setField(CalcOffset(maxJingLi,				PetObjectData),	DType_U16,		"精力资质最大值");
	mColumnData->setField(CalcOffset(minYuanLi,				PetObjectData),	DType_U16,		"元力资质最小值");
	mColumnData->setField(CalcOffset(maxYuanLi,				PetObjectData),	DType_U16,		"元力资质最大值");
	mColumnData->setField(CalcOffset(minMinJie,				PetObjectData),	DType_U16,		"敏捷资质最小值");
	mColumnData->setField(CalcOffset(maxMinJie,				PetObjectData),	DType_U16,		"敏捷资质最大值");
	mColumnData->setField(CalcOffset(minQianLi,				PetObjectData),	DType_U16,		"潜力资质最小值");
	mColumnData->setField(CalcOffset(maxQianLi,				PetObjectData),	DType_U16,		"潜力资质最大值");

	mColumnData->setField(CalcOffset(objScale.x,			PetObjectData),	DType_F32,		"x轴缩放");
	mColumnData->setField(CalcOffset(objScale.y,			PetObjectData),	DType_F32,		"y轴缩放");
	mColumnData->setField(CalcOffset(objScale.z,			PetObjectData),	DType_F32,		"z轴缩放");

	for(int k = 0; k < file.RecordNum; ++k)
	{
		PetObjectData* pData = new PetObjectData;
		for(int h = 0; h < file.ColumNum; ++h)
		{
			file.GetData(tempData);
			mColumnData->setData(pData, h, tempData);
		}
		Insert(pData);

		addToUserDataBlockGroup(pData);
	}
	file.ReadDataClose();
	return true;
}

bool PetRepository::Insert(PetObjectData* pData)
{
	if(!pData)
		return false;

	return m_PetDataMap.insert(PetDataMap::value_type(pData->dataBlockId, pData)).second;
}

void PetRepository::Clear()
{
	PetDataMap::const_iterator it = m_PetDataMap.begin();
	for(; it != m_PetDataMap.end(); ++it )
	{
		if(it->second)
			delete  it->second;
	}
	m_PetDataMap.clear();
}

PetObjectData*  PetRepository::GetPetData(U32 PetID)
{
	PetDataMap::const_iterator it = m_PetDataMap.find(PetID);
	if(it == m_PetDataMap.end())
		return NULL;
	return it->second;
}

S32 PetRepository::GetPetDataCount()
{
	return (S32)m_PetDataMap.size();
}

U32 PetRepository::GetPetIdByIndex(S32 index)
{
	AssertRelease( (index >= 0) &&(index < m_PetDataMap.size()) , "PetData.dat::invalid index");
	PetDataMap::const_iterator it = m_PetDataMap.begin();
	int temp = 0;
	for(;it != m_PetDataMap.end(); ++ it, ++ temp )
	{
		if(temp == index)
		{
			return it->first;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// 通过列数获取NPC数据字段值
ConsoleFunction( getPetData, const char*, 3, 3, "getPetData(%npcid, %col)")
{
	PetObjectData* data = g_PetRepository.GetPetData(dAtol(argv[1]));
	if(data)
	{
		std::string to;
		g_PetRepository.mColumnData->getData(data, dAtoi(argv[2]), to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;		
	}
	return false;
}