//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/gameFunctions.h"
#include "Gameplay/GameObjects/MountObjectData.h"
#include "Gameplay/GameObjects/Skill.h"
#include "console/consoleTypes.h"

#include "platform/platform.h"
#include "core/bitStream.h"
#include "ts/tsShapeInstance.h"
#include "Gameplay/data/readDataFile.h"
#include "Util/ColumnData.h"

// ========================================================================================================================================
//	MountObjectData
// ========================================================================================================================================

IMPLEMENT_CO_DATABLOCK_V1(MountObjectData);

MountObjectData::MountObjectData()
{
	bInitialized = false;
	buffId = 0;
	dMemset(info, 0, sizeof(MountedInfo)*MaxMounted);
}

MountObjectData::~MountObjectData()
{
}

bool MountObjectData::preload(bool server, char errorBuffer[256])
{
	if(!Parent::preload(server, errorBuffer))
		return false;

	return true;
}

bool MountObjectData::initDataBlock()
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

U32 MountObjectData::getCurrLevel()
{
	//得到
	char buf[10];		//dataBlockId必须为9位数
	_itoa(dataBlockId, buf, 10);
	U32 nLevel = (buf[8] - '0') + (buf[7] - '0') * 10;
	return nLevel;
}
// ========================================================================================================================================
//	MountRepository
// ========================================================================================================================================

MountRepository g_MountRepository;

MountRepository::MountRepository()
{
	m_MountDataMap.clear();
}

MountRepository::~MountRepository()
{
	Clear();
	if(mColumnData)
		delete mColumnData;
}

bool MountRepository::Read()
{
	CDataFile op;
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取模型数据
	Platform::makeFullPathName("gameres/data/repository/MountRepository.dat",filename,sizeof(filename));

	file.readDataFile(filename);

	for (S32 i = 0; i < file.RecordNum; i++)
	{
		MountObjectData *pMountObjectData = new MountObjectData;
		// 编号
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_U32, "MountRepository.dat:: Filed to read dataBlockId");
		pMountObjectData->dataBlockId = tempData.m_U32;
		
		// Mount名称
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_string, "MountRepository.dat:: Filed to read mName");
		pMountObjectData->mName = StringTable->insert(tempData.m_string);

		// 玩家等级要求
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_U32, "MountRepository.dat:: Filed to read mMountLevel");
		pMountObjectData->mMountLevel = tempData.m_U32;

		// 数值ID
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_U32, "MountRepository.dat:: Filed to read buffId");
		pMountObjectData->buffId = tempData.m_U32;
		// 骑宠类型
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_enum8, "MountRepository.dat:: Filed to read mType");
		pMountObjectData->mType = tempData.m_Enum8;
		// 骑宠种族
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_enum8, "MountRepository.dat:: Filed to read mRace");
		pMountObjectData->mRace = tempData.m_Enum8;
		// 骑宠说明
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_string, "MountRepository.dat:: Filed to read mDescription");
		pMountObjectData->mDescription = StringTable->insert(tempData.m_string);
		// 可骑乘人数
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_U8, "MountRepository.dat:: Filed to read mCount");
		pMountObjectData->mCount = tempData.m_U8;
		// 时效
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_U32, "MountRepository.dat:: Filed to read mCount");
		pMountObjectData->mDuringTime = tempData.m_U32;
		// 计时方式
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_enum8, "MountRepository.dat:: Filed to read mTimeFlag");
		pMountObjectData->mTimeFlag = tempData.m_Enum8;
		//技能结构ID
		for (S32 j = 0; j < 3; j++)
		{
			file.GetData(tempData);
			AssertFatal(tempData.m_Type == DType_U32, "MountRepository.dat:: Filed to read mSkillId");
			pMountObjectData->mSkillId[j] = tempData.m_U32;
		}

		for (S32 j = 0; j < MountObjectData::MaxMounted; j++)
		{
			file.GetData(tempData);
			AssertFatal(tempData.m_Type == DType_U8, "MountRepository.dat:: Filed to read linkPoint");
			pMountObjectData->info[j].linkPoint = tempData.m_U8;
			file.GetData(tempData);
			AssertFatal(tempData.m_Type == DType_U8, "MountRepository.dat:: Filed to read linkPointSelf");
			pMountObjectData->info[j].linkPointSelf = tempData.m_U8;
			file.GetData(tempData);
			AssertFatal(tempData.m_Type == DType_enum8, "MountRepository.dat:: Filed to read action");
			pMountObjectData->info[j].action = tempData.m_Enum8;
		}

		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_F32, "MountRepository.dat:: Filed to read mSpeed");
		pMountObjectData->mLevelData.mSpeed = tempData.m_F32;
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_string, "MountRepository.dat:: Filed to read mHead");
		pMountObjectData->mLevelData.mHead = StringTable->insert(tempData.m_string);
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_U32, "MountRepository.dat:: Filed to read shapesSetId");
		pMountObjectData->shapesSetId = tempData.m_U32;
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_U32, "MountRepository.dat:: Filed to read mEffect");
		pMountObjectData->mLevelData.mEffect = tempData.m_U32;
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_string, "MountRepository.dat:: Filed to read mAudio");
		pMountObjectData->mLevelData.mAudio = StringTable->insert(tempData.m_string);

		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_string, "MountRepository.dat:: Filed to read mMountShortcutIcon");
		pMountObjectData->mMountShortcutIcon = StringTable->insert(tempData.m_string);

		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_F32, "MountRepository.dat:: Filed to read mEffect");
		pMountObjectData->objScale.x = tempData.m_F32;
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_F32, "MountRepository.dat:: Filed to read mEffect");
		pMountObjectData->objScale.y = tempData.m_F32;
		file.GetData(tempData);
		AssertFatal(tempData.m_Type == DType_F32, "MountRepository.dat:: Filed to read mEffect");
		pMountObjectData->objScale.z = tempData.m_F32;

		Insert(pMountObjectData);

		addToUserDataBlockGroup(pMountObjectData);
	}
	
	file.ReadDataClose();
	return true;
}

bool MountRepository::Insert(MountObjectData* pData)
{
	if(!pData)
		return false;

	return m_MountDataMap.insert(MountDataMap::value_type(pData->dataBlockId, pData)).second;
}

void MountRepository::Clear()
{
	MountDataMap::const_iterator it = m_MountDataMap.begin();
	for(; it != m_MountDataMap.end(); ++it )
	{
		if(it->second)
			delete  it->second;
	}
	m_MountDataMap.clear();
}

MountObjectData*  MountRepository::GetMountData(U32 MountID)
{
	MountDataMap::const_iterator it = m_MountDataMap.find(MountID);
	if(it == m_MountDataMap.end())
		return NULL;
	return it->second;
}

S32 MountRepository::GetMountDataCount()
{
	return (S32)m_MountDataMap.size();
}

U32 MountRepository::GetMountIdByIndex(S32 index)
{
	AssertRelease( (index >= 0) &&(index < m_MountDataMap.size()) , "MountData.dat::invalid index");
	MountDataMap::const_iterator it = m_MountDataMap.begin();
	int temp = 0;
	for(;it != m_MountDataMap.end(); ++ it, ++ temp )
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
ConsoleFunction( getMountData, const char*, 3, 3, "getMountData(%npcid, %col)")
{
	MountObjectData* data = g_MountRepository.GetMountData(dAtol(argv[1]));
	if(data)
	{
		std::string to;
		g_MountRepository.mColumnData->getData(data, dAtoi(argv[2]), to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;		
	}
	return false;
}