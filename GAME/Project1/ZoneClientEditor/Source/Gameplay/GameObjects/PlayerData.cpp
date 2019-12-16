//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/gameFunctions.h"
#include "Gameplay/GameObjects/PlayerData.h"
//#include "DatBuilder/DatBuilder.h"
#include "Gameplay/data/readDataFile.h"

//#define PlayerDataString	"Player_"

// ========================================================================================================================================
//  PlayerData
// ========================================================================================================================================

IMPLEMENT_CO_DATABLOCK_V1(PlayerData);

PlayerData::PlayerData()
{
	bInitialized = false;
}

PlayerData::~PlayerData()
{
}

bool PlayerData::preload(bool server, char errorBuffer[256])
{
	if(!Parent::preload(server, errorBuffer))
		return false;

	return true;
}

bool PlayerData::initDataBlock()
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
//  PlayerRepository
// ========================================================================================================================================
PlayerRepository g_PlayerRepository;

PlayerRepository::PlayerRepository()
{
}

PlayerRepository::~PlayerRepository()
{
}

void PlayerRepository::Clear()
{
	PlayerDataMap::iterator pos;
	for(pos = m_PlayerDataMap.begin(); pos != m_PlayerDataMap.end(); ++pos)
	{
		delete pos->second;
	}
	m_PlayerDataMap.clear();

	m_MaleBodyMap.clear();
	m_FemaleBodyMap.clear();
	m_MaleFaceMap.clear();
	m_FemaleFaceMap.clear();
	m_MaleHairMap.clear();
	m_FemaleHairMap.clear();
}

PlayerData* PlayerRepository::GetPlayerData(U32 PlayerID)
{
	PlayerDataMap::iterator pos = m_PlayerDataMap.find(PlayerID);
	if(pos != m_PlayerDataMap.end())
		return pos->second;
	return NULL;
}

StringTableEntry PlayerRepository::GetPlayerShape(U32 PlayerShapeID, U32 type)
{
	switch (type)
	{
	case MaleBody:
		{
			PlayerShapeMap::iterator pos = m_MaleBodyMap.find(PlayerShapeID);
			if(pos != m_MaleBodyMap.end())
				return pos->second;
		}
		break;
	case FemaleBody:
		{
			PlayerShapeMap::iterator pos = m_FemaleBodyMap.find(PlayerShapeID);
			if(pos != m_FemaleBodyMap.end())
				return pos->second;
		}
		break;
	case MaleFace:
		{
			PlayerShapeMap::iterator pos = m_MaleFaceMap.find(PlayerShapeID);
			if(pos != m_MaleFaceMap.end())
				return pos->second;
		}
		break;
	case FemaleFace:
		{
			PlayerShapeMap::iterator pos = m_FemaleFaceMap.find(PlayerShapeID);
			if(pos != m_FemaleFaceMap.end())
				return pos->second;
		}
		break;
	case MaleHair:
		{
			PlayerShapeMap::iterator pos = m_MaleHairMap.find(PlayerShapeID);
			if(pos != m_MaleHairMap.end())
				return pos->second;
		}
		break;
	case FemaleHair:
		{
			PlayerShapeMap::iterator pos = m_FemaleHairMap.find(PlayerShapeID);
			if(pos != m_FemaleHairMap.end())
				return pos->second;
		}
		break;
	default:
		return NULL;
	}
	return NULL;
}

StringTableEntry PlayerRepository::GetPlayerShape(U32 PlayerShapeID)
{
	PlayerData* pData = GetPlayerData(PlayerShapeID);
	if(pData)
		return pData->shapeName;

	PlayerShapeMap::iterator pos = m_MaleFaceMap.find(PlayerShapeID);
	if(pos != m_MaleFaceMap.end())
		return pos->second;

	pos = m_FemaleFaceMap.find(PlayerShapeID);
	if(pos != m_FemaleFaceMap.end())
		return pos->second;

	pos = m_MaleHairMap.find(PlayerShapeID);
	if(pos != m_MaleHairMap.end())
		return pos->second;

	pos = m_FemaleHairMap.find(PlayerShapeID);
	if(pos != m_FemaleHairMap.end())
		return pos->second;

	return NULL;
}

U32 PlayerRepository::GetMaleBodyID(U32 index)
{
	index = mClamp(index, 0, (S32)m_MaleBodyMap.size() - 1);
	PlayerShapeMap::iterator pos = m_MaleBodyMap.begin();
	while (pos != m_MaleBodyMap.end() && index-- > 0)
		pos++;

	return pos->first;
}

U32 PlayerRepository::GetFemaleBodyID(U32 index)
{
	index = mClamp(index, 0, (S32)m_FemaleBodyMap.size() - 1);
	PlayerShapeMap::iterator pos = m_FemaleBodyMap.begin();
	while (pos != m_FemaleBodyMap.end() && index-- > 0)
		pos++;

	return pos->first;
}

U32 PlayerRepository::GetMaleFaceID(U32 index)
{
	index = mClamp(index, 0, (S32)m_MaleFaceMap.size() - 1);
	PlayerShapeMap::iterator pos = m_MaleFaceMap.begin();
	while (pos != m_MaleFaceMap.end() && index-- > 0)
		pos++;

	return pos->first;
}

U32 PlayerRepository::GetFemaleFaceID(U32 index)
{
	index = mClamp(index, 0, (S32)m_FemaleFaceMap.size() - 1);
	PlayerShapeMap::iterator pos = m_FemaleFaceMap.begin();
	while (pos != m_FemaleFaceMap.end() && index-- > 0)
		pos++;

	return pos->first;
}

U32 PlayerRepository::GetMaleHairID(U32 index)
{
	index = mClamp(index, 0, (S32)m_MaleHairMap.size() - 1);
	PlayerShapeMap::iterator pos = m_MaleHairMap.begin();
	while (pos != m_MaleHairMap.end() && index-- > 0)
		pos++;

	return pos->first;
}

U32 PlayerRepository::GetFemaleHairID(U32 index)
{
	index = mClamp(index, 0, (S32)m_FemaleHairMap.size() - 1);
	PlayerShapeMap::iterator pos = m_FemaleHairMap.begin();
	while (pos != m_FemaleHairMap.end() && index-- > 0)
		pos++;

	return pos->first;
}

bool PlayerRepository::Read()
{
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取模型数据
	Platform::makeFullPathName(GAME_PLAYDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can NOT read file : PlayerRepository.dat!");
	//
	for(int i = 0 ; i < file.RecordNum ; ++ i)
	{
		//玩家ID
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "PlayerRepository.dat::Read - failed to read m_PlayerID!");
		U32 id = tempData.m_U32;
		//玩家模型ID
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "PlayerRepository.dat::Read - failed to read shapesSetId!");
		U32 ssId = tempData.m_U32;
		//类型
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "PlayerRepository.dat::Read - failed to read type!");
		U32 type = tempData.m_Enum8;

		if(type == PlayerDataBlock)
		{
			PlayerData * pPlayerData = new PlayerData;
			pPlayerData->dataBlockId = id;
			pPlayerData->shapesSetId = ssId;

			m_PlayerDataMap.insert(PlayerDataMap::value_type(pPlayerData->dataBlockId, pPlayerData));

			addToUserDataBlockGroup(pPlayerData);
		}
	}
	//
	return true;
}

bool PlayerRepository::Insert(U32 id, StringTableEntry shapeName, U32 type)
{
	switch (type)
	{
	case PlayerDataBlock:
		{
			PlayerData * pPlayerData = new PlayerData;
			pPlayerData->dataBlockId = id;
			pPlayerData->shapeName = StringTable->insert(shapeName);

			m_PlayerDataMap.insert(PlayerDataMap::value_type(pPlayerData->dataBlockId, pPlayerData));

			addToUserDataBlockGroup(pPlayerData);
		}
		break;
	case MaleBody:
		{
			m_MaleBodyMap.insert(PlayerShapeMap::value_type(id, shapeName));
		}
		break;
	case FemaleBody:
		{
			m_FemaleBodyMap.insert(PlayerShapeMap::value_type(id, shapeName));
		}
		break;
	case MaleFace:
		{
			m_MaleFaceMap.insert(PlayerShapeMap::value_type(id, shapeName));
		}
		break;
	case FemaleFace:
		{
			m_FemaleFaceMap.insert(PlayerShapeMap::value_type(id, shapeName));
		}
		break;
	case MaleHair:
		{
			m_MaleHairMap.insert(PlayerShapeMap::value_type(id, shapeName));
		}
		break;
	case FemaleHair:
		{
			m_FemaleHairMap.insert(PlayerShapeMap::value_type(id, shapeName));
		}
		break;
	default:
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------------------------------------------------------------------
//  导出脚本
//

ConsoleFunction(GetMaleBodySize, S32, 1, 1, "GetMaleBodySize()")
{
	return g_PlayerRepository.GetMaleBodySize();
}

ConsoleFunction(GetFemaleBodySize, S32, 1, 1, "GetFemaleBodySize()")
{
	return g_PlayerRepository.GetFemaleBodySize();
}

ConsoleFunction(GetMaleFaceSize, S32, 1, 1, "GetMaleFaceSize()")
{
	return g_PlayerRepository.GetMaleFaceSize();
}

ConsoleFunction(GetFemaleFaceSize, S32, 1, 1, "GetFemaleFaceSize()")
{
	return g_PlayerRepository.GetFemaleFaceSize();
}

ConsoleFunction(GetMaleHairSize, S32, 1, 1, "GetMaleHairSize()")
{
	return g_PlayerRepository.GetMaleHairSize();
}

ConsoleFunction(GetFemaleHairSize, S32, 1, 1, "GetFemaleHairSize()")
{
	return g_PlayerRepository.GetFemaleHairSize();
}

ConsoleFunction(GetMaleBodyID, S32, 2, 2, "GetMaleBodyID(index)")
{
	return g_PlayerRepository.GetMaleBodyID(dAtoi(argv[1]));
}

ConsoleFunction(GetFemaleBodyID, S32, 2, 2, "GetFemaleBodyID(index)")
{
	return g_PlayerRepository.GetFemaleBodyID(dAtoi(argv[1]));
}

ConsoleFunction(GetMaleFaceID, S32, 2, 2, "GetMaleFaceID(index)")
{
	return g_PlayerRepository.GetMaleFaceID(dAtoi(argv[1]));
}

ConsoleFunction(GetFemaleFaceID, S32, 2, 2, "GetFemaleFaceID(index)")
{
	return g_PlayerRepository.GetFemaleFaceID(dAtoi(argv[1]));
}

ConsoleFunction(GetMaleHairID, S32, 2, 2, "GetMaleHairID(index)")
{
	return g_PlayerRepository.GetMaleHairID(dAtoi(argv[1]));
}

ConsoleFunction(GetFemaleHairID, S32, 2, 2, "GetFemaleHairID(index)")
{
	return g_PlayerRepository.GetFemaleHairID(dAtoi(argv[1]));
}

ConsoleFunction(GetMaleBodyShape, const char*, 2, 2, "GetMaleBodyShape(id)")
{
	return g_PlayerRepository.GetPlayerShape(dAtoi(argv[1]), PlayerRepository::MaleBody);
}

ConsoleFunction(GetFemaleBodyShape, const char*, 2, 2, "GetFemaleBodyShape(id)")
{
	return g_PlayerRepository.GetPlayerShape(dAtoi(argv[1]), PlayerRepository::FemaleBody);
}

ConsoleFunction(GetMaleFaceShape, const char*, 2, 2, "GetMaleFaceShape(id)")
{
	return g_PlayerRepository.GetPlayerShape(dAtoi(argv[1]), PlayerRepository::MaleFace);
}

ConsoleFunction(GetFemaleFaceShape, const char*, 2, 2, "GetFemaleFaceShape(id)")
{
	return g_PlayerRepository.GetPlayerShape(dAtoi(argv[1]), PlayerRepository::FemaleFace);
}

ConsoleFunction(GetMaleHairShape, const char*, 2, 2, "GetMaleHairShape(id)")
{
	return g_PlayerRepository.GetPlayerShape(dAtoi(argv[1]), PlayerRepository::MaleHair);
}

ConsoleFunction(GetFemaleHairShape, const char*, 2, 2, "GetFemaleHairShape(id)")
{
	return g_PlayerRepository.GetPlayerShape(dAtoi(argv[1]), PlayerRepository::FemaleHair);
}

