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

	// ��ȡģ������
	Platform::makeFullPathName(GAME_NPCDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can't read file : NpcRepository.dat!");

	mColumnData = new IColumnData(file.ColumNum, "NpcRepository.dat");
	mColumnData->setField(CalcOffset(dataBlockId,			NpcObjectData),	DType_U32,		"NPC���");
	mColumnData->setField(CalcOffset(npcName,				NpcObjectData),	DType_string,	"NPC����");
	mColumnData->setField(CalcOffset(npcTitle,				NpcObjectData),	DType_string,	"NPC�ƺ�");
	mColumnData->setField(CalcOffset(icon,					NpcObjectData),	DType_string,	"NPC icon");
	mColumnData->setField(CalcOffset(topIcon,				NpcObjectData),	DType_string,	"NPCͷ��ͼƬ");
	mColumnData->setField(CalcOffset(shapesSetId,			NpcObjectData),	DType_U32,		"NPCģ������");
	mColumnData->setField(CalcOffset(effectPacketId,		NpcObjectData),	DType_U32,		"NPC��Ч���");
	mColumnData->setField(CalcOffset(flags,					NpcObjectData),	DType_U32,		"NPC���Ա�־");
	mColumnData->setField(CalcOffset(aiChar,				NpcObjectData),	DType_U32,		"AI�Ը�");
	//mColumnData->setField(CalcOffset(aiSpell,				NpcObjectData),	DType_U32,		"AI����");
	mColumnData->setField(CalcOffset(race,					NpcObjectData),	DType_enum8,	"NPC����");
	mColumnData->setField(CalcOffset(family,				NpcObjectData),	DType_enum8,	"NPC����");
	mColumnData->setField(CalcOffset(intensity,				NpcObjectData),	DType_enum8,	"NPCǿ��");
	mColumnData->setField(CalcOffset(minLevel,				NpcObjectData),	DType_U8,		"NPC��С�ȼ�");
	mColumnData->setField(CalcOffset(maxLevel,				NpcObjectData),	DType_U8,		"NPC���ȼ�");
	mColumnData->setField(CalcOffset(buffId,				NpcObjectData),	DType_U32,		"��ֵBuffID");
	mColumnData->setField(CalcOffset(defaultSkillId,		NpcObjectData),	DType_U32,		"��ͨ��������ID");
	mColumnData->setField(CalcOffset(experience,			NpcObjectData),	DType_U16,		"NPC����ֵ");
	mColumnData->setField(CalcOffset(exBuffId_a,			NpcObjectData),	DType_U32,		"����״̬1");
	mColumnData->setField(CalcOffset(exBuffId_b,			NpcObjectData),	DType_U32,		"����״̬2");
	mColumnData->setField(CalcOffset(maxWalkSpeed,			NpcObjectData),	DType_F32,		"NPC�����ٶ�");
	mColumnData->setField(CalcOffset(maxForwardSpeed,		NpcObjectData),	DType_F32,		"NPC�ܲ��ٶ�");
	mColumnData->setField(CalcOffset(visualField,			NpcObjectData),	DType_F32,		"NPC��Ұ��Χ");
	mColumnData->setField(CalcOffset(activeField,			NpcObjectData),	DType_F32,		"NPC���Χ");
	mColumnData->setField(CalcOffset(chaseField,			NpcObjectData),	DType_F32,		"NPC׷����Χ");
	mColumnData->setField(CalcOffset(combatField,			NpcObjectData),	DType_F32,		"NPCս����Χ");
	mColumnData->setField(CalcOffset(expField,				NpcObjectData),	DType_F32,		"������淶Χ");
	mColumnData->setField(CalcOffset(scanInterval,			NpcObjectData),	DType_U16,		"�ѵм��");
	mColumnData->setField(CalcOffset(riseInterval,			NpcObjectData),	DType_U32,		"�������");
	mColumnData->setField(CalcOffset(corpseTime,			NpcObjectData),	DType_U32,		"ʬ��ɼ�ʱ��");
	mColumnData->setField(CalcOffset(fadeTime,				NpcObjectData),	DType_U16,		"���뵭��ʱ��");
	mColumnData->setField(CalcOffset(dropTime,				NpcObjectData),	DType_U32,		"���������ʱ��");
	mColumnData->setField(CalcOffset(eventID,				NpcObjectData),	DType_U32,		"�¼�ID");
	mColumnData->setField(CalcOffset(pathID,				NpcObjectData),	DType_U32,		"·��ID");
	mColumnData->setField(CalcOffset(packageShapeId,		NpcObjectData),	DType_U32,		"�����ģ�ͱ��");
	mColumnData->setField(CalcOffset(attackSound_a,			NpcObjectData),	DType_string,	"������Ч1");
	mColumnData->setField(CalcOffset(attackSound_b,			NpcObjectData),	DType_string,	"������Ч2");
	mColumnData->setField(CalcOffset(attackSoundOdds,		NpcObjectData),	DType_U32,		"������Ч���ż���");
	mColumnData->setField(CalcOffset(interactionSound_a,	NpcObjectData),	DType_string,	"������Ч1");
	mColumnData->setField(CalcOffset(interactionSound_b,	NpcObjectData),	DType_string,	"������Ч2");
	mColumnData->setField(CalcOffset(deadthSound,			NpcObjectData),	DType_string,	"������Ч");
	//mColumnData->setField(CalcOffset(objScale.x,			NpcObjectData),	DType_F32,		"x������");
	//mColumnData->setField(CalcOffset(objScale.y,			NpcObjectData),	DType_F32,		"y������");
	//mColumnData->setField(CalcOffset(objScale.z,			NpcObjectData),	DType_F32,		"z������");
	mColumnData->setField(CalcOffset(minScale,				NpcObjectData),	DType_F32,		"��С����");
	mColumnData->setField(CalcOffset(maxScale,				NpcObjectData),	DType_F32,		"�������");
	
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
// ͨ��������ȡNPC�����ֶ�ֵ
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