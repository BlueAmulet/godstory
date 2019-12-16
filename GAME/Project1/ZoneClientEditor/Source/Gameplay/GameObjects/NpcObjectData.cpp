//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/gameFunctions.h"
#include "Gameplay/GameObjects/NpcObjectData.h"
#include "console/consoleTypes.h"

#include "platform/platform.h"
#include "core/bitStream.h"
#include "ts/tsShapeInstance.h"
#include "Gameplay/data/readDataFile.h"
#include "Util/ColumnData.h"

// ========================================================================================================================================
//	NpcObjectData
// ========================================================================================================================================

IMPLEMENT_CO_DATABLOCK_V1(NpcObjectData);

NpcObjectData::NpcObjectData()
{
	bInitialized = false;
	maxForwardSpeed = 1.2f;
	npcName = NULL;
	npcTitle = NULL;
	icon = NULL;
	topIcon = NULL;
	flags = 0;
	race = Race_Sheng;
	family = Family_None;
	influence = Influence_Npc;
	intensity = Intensity_Normal;
	minLevel = 0;
	maxLevel = 0;
	buffId = 0;
	defaultSkillId = 0;
	experience = 0;
	exBuffId_a = 0;
	exBuffId_b = 0;
	minScale = 1.0f;
	maxScale = 1.0f;
	visualField = 0.0f;
	activeField = 0.0f;
	chaseField = 0.0f;
	combatField = 0.0f;
	expField = 0.0f;
	scanInterval = 0;
	riseInterval = 0;
	corpseTime = 0;
	fadeTime = 0;
	dropTime = 0;
	eventID = 0;
	pathID = 0;
	packageShapeId = 0;
	attackSound_a = NULL;
	attackSound_b = NULL;
	attackSoundOdds = NULL;
	interactionSound_a = NULL;
	interactionSound_b = NULL;
	deadthSound = NULL;	
}

NpcObjectData::~NpcObjectData()
{
}

bool NpcObjectData::preload(bool server, char errorBuffer[256])
{
	if(!Parent::preload(server, errorBuffer))
		return false;

	return true;
}

bool NpcObjectData::initDataBlock()
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
//	NpcRepository
// ========================================================================================================================================

NpcRepository g_NpcRepository;

NpcRepository::NpcRepository()
{
	m_NpcDataMap.clear();
}

NpcRepository::~NpcRepository()
{
	Clear();
	if(mColumnData)
		delete mColumnData;
}

bool NpcRepository::Read()
{
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取模型数据
	Platform::makeFullPathName(GAME_NPCDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can't read file : NpcRepository.dat!");

	mColumnData = new IColumnData(file.ColumNum, "NpcRepository.dat");
	mColumnData->setField(CalcOffset(dataBlockId,			NpcObjectData),	DType_U32,		"NPC编号");
	mColumnData->setField(CalcOffset(npcName,				NpcObjectData),	DType_string,	"NPC名称");
	mColumnData->setField(CalcOffset(npcTitle,				NpcObjectData),	DType_string,	"NPC称号");
	mColumnData->setField(CalcOffset(icon,					NpcObjectData),	DType_string,	"NPC icon");
	mColumnData->setField(CalcOffset(topIcon,				NpcObjectData),	DType_string,	"NPC头顶图片");
	mColumnData->setField(CalcOffset(shapesSetId,			NpcObjectData),	DType_U32,		"NPC模型组编号");
	mColumnData->setField(CalcOffset(effectPacketId,		NpcObjectData),	DType_U32,		"NPC特效编号");
	mColumnData->setField(CalcOffset(flags,					NpcObjectData),	DType_U32,		"NPC属性标志");
	mColumnData->setField(CalcOffset(aiChar,				NpcObjectData),	DType_U32,		"AI性格");
	//mColumnData->setField(CalcOffset(aiSpell,				NpcObjectData),	DType_U32,		"AI技能");
	mColumnData->setField(CalcOffset(race,					NpcObjectData),	DType_enum8,	"NPC种族");
	mColumnData->setField(CalcOffset(family,				NpcObjectData),	DType_enum8,	"NPC门宗");
	mColumnData->setField(CalcOffset(intensity,				NpcObjectData),	DType_enum8,	"NPC强度");
	mColumnData->setField(CalcOffset(minLevel,				NpcObjectData),	DType_U8,		"NPC最小等级");
	mColumnData->setField(CalcOffset(maxLevel,				NpcObjectData),	DType_U8,		"NPC最大等级");
	mColumnData->setField(CalcOffset(buffId,				NpcObjectData),	DType_U32,		"数值BuffID");
	mColumnData->setField(CalcOffset(defaultSkillId,		NpcObjectData),	DType_U32,		"普通攻击技能ID");
	mColumnData->setField(CalcOffset(experience,			NpcObjectData),	DType_U16,		"NPC经验值");
	mColumnData->setField(CalcOffset(exBuffId_a,			NpcObjectData),	DType_U32,		"附加状态1");
	mColumnData->setField(CalcOffset(exBuffId_b,			NpcObjectData),	DType_U32,		"附加状态2");
	mColumnData->setField(CalcOffset(maxWalkSpeed,			NpcObjectData),	DType_F32,		"NPC步行速度");
	mColumnData->setField(CalcOffset(maxForwardSpeed,		NpcObjectData),	DType_F32,		"NPC跑步速度");
	mColumnData->setField(CalcOffset(visualField,			NpcObjectData),	DType_F32,		"NPC视野范围");
	mColumnData->setField(CalcOffset(activeField,			NpcObjectData),	DType_F32,		"NPC活动范围");
	mColumnData->setField(CalcOffset(chaseField,			NpcObjectData),	DType_F32,		"NPC追击范围");
	mColumnData->setField(CalcOffset(combatField,			NpcObjectData),	DType_F32,		"NPC战斗范围");
	mColumnData->setField(CalcOffset(expField,				NpcObjectData),	DType_F32,		"玩家受益范围");
	mColumnData->setField(CalcOffset(scanInterval,			NpcObjectData),	DType_U16,		"搜敌间隔");
	mColumnData->setField(CalcOffset(riseInterval,			NpcObjectData),	DType_U32,		"重生间隔");
	mColumnData->setField(CalcOffset(corpseTime,			NpcObjectData),	DType_U32,		"尸体可见时间");
	mColumnData->setField(CalcOffset(fadeTime,				NpcObjectData),	DType_U16,		"淡入淡出时间");
	mColumnData->setField(CalcOffset(dropTime,				NpcObjectData),	DType_U32,		"掉物包存在时间");
	mColumnData->setField(CalcOffset(eventID,				NpcObjectData),	DType_U32,		"事件ID");
	mColumnData->setField(CalcOffset(pathID,				NpcObjectData),	DType_U32,		"路径ID");
	mColumnData->setField(CalcOffset(packageShapeId,		NpcObjectData),	DType_U32,		"掉落包模型编号");
	mColumnData->setField(CalcOffset(attackSound_a,			NpcObjectData),	DType_string,	"攻击音效1");
	mColumnData->setField(CalcOffset(attackSound_b,			NpcObjectData),	DType_string,	"攻击音效2");
	mColumnData->setField(CalcOffset(attackSoundOdds,		NpcObjectData),	DType_U32,		"攻击音效播放几率");
	mColumnData->setField(CalcOffset(interactionSound_a,	NpcObjectData),	DType_string,	"交互音效1");
	mColumnData->setField(CalcOffset(interactionSound_b,	NpcObjectData),	DType_string,	"交互音效2");
	mColumnData->setField(CalcOffset(deadthSound,			NpcObjectData),	DType_string,	"死亡音效");
	//mColumnData->setField(CalcOffset(objScale.x,			NpcObjectData),	DType_F32,		"x轴缩放");
	//mColumnData->setField(CalcOffset(objScale.y,			NpcObjectData),	DType_F32,		"y轴缩放");
	//mColumnData->setField(CalcOffset(objScale.z,			NpcObjectData),	DType_F32,		"z轴缩放");
	mColumnData->setField(CalcOffset(minScale,				NpcObjectData),	DType_F32,		"最小缩放");
	mColumnData->setField(CalcOffset(maxScale,				NpcObjectData),	DType_F32,		"最大缩放");
	
	for(int k = 0; k < file.RecordNum; ++k)
	{
		NpcObjectData* pData = new NpcObjectData;
		for(int h = 0; h < file.ColumNum; ++h)
		{
			file.GetData(tempData);
			mColumnData->setData(pData, h, tempData);
		}
		Insert(pData);

		addToUserDataBlockGroup(pData);
	}
	file.ReadDataClose();
	//SAFE_DELETE(mColumnData);
	return true;
}

bool NpcRepository::Insert(NpcObjectData* pData)
{
	if(!pData)
		return false;

#ifdef DEBUG
	NpcDataMap::iterator it = m_NpcDataMap.find(pData->dataBlockId);
	if(it != m_NpcDataMap.end())
	{
		Con::errorf("NpcRepository::Insert ! id = %d", pData->dataBlockId);
		it->second->deleteObject();
		m_NpcDataMap.erase(it);
	}
#endif
	return m_NpcDataMap.insert(NpcDataMap::value_type(pData->dataBlockId, pData)).second;
}

void NpcRepository::Clear()
{
	NpcDataMap::const_iterator it = m_NpcDataMap.begin();
	for(; it != m_NpcDataMap.end(); ++it )
	{
		if(it->second)
			delete  it->second;
	}
	m_NpcDataMap.clear();
}

NpcObjectData*  NpcRepository::GetNpcData(U32 NpcID)
{
	NpcDataMap::const_iterator it = m_NpcDataMap.find(NpcID);
	if(it == m_NpcDataMap.end())
		return NULL;
	return it->second;
}

S32 NpcRepository::GetNpcDataCount()
{
	return (S32)m_NpcDataMap.size();
}

U32 NpcRepository::GetNpcIdByIndex(S32 index)
{
	AssertRelease( (index >= 0) &&(index < m_NpcDataMap.size()) , "MonsterData.dat::invalid index");
	NpcDataMap::const_iterator it = m_NpcDataMap.begin();
	int temp = 0;
	for(;it != m_NpcDataMap.end(); ++ it, ++ temp )
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
ConsoleFunction( getNpcData, const char*, 3, 3, "getNpcData(%npcid, %col)")
{
	NpcObjectData* data = g_NpcRepository.GetNpcData(dAtol(argv[1]));
	if(data)
	{
		std::string to;
		g_NpcRepository.mColumnData->getData(data, dAtoi(argv[2]), to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;		
	}
	return false;
}