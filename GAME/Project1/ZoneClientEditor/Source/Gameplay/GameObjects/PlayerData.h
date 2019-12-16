//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/GameObjectData.h"
#include <string>
#include <hash_map>


// ========================================================================================================================================
//  PlayerData
// ========================================================================================================================================

struct PlayerData: public GameObjectData 
{
	typedef GameObjectData Parent;

	DECLARE_CONOBJECT(PlayerData);

	bool bInitialized;

public:
	PlayerData();
	~PlayerData();

	bool preload(bool server, char errorBuffer[256]);
	bool initDataBlock();
};

// ========================================================================================================================================
//  PlayerRepository
// ========================================================================================================================================

class PlayerRepository
{
public:
	enum DataType
	{
		PlayerDataBlock		= 0,
		MaleBody,
		FemaleBody,
		MaleFace,
		FemaleFace,
		MaleHair,
		FemaleHair,
	};

	PlayerRepository();
	~PlayerRepository();

	PlayerData*				GetPlayerData(U32 PlayerID);
	StringTableEntry		GetPlayerShape(U32 PlayerShapeID, U32 type);
	StringTableEntry		GetPlayerShape(U32 PlayerShapeID);
	void					Clear();
	bool					Read();
	bool					Insert(U32 id, StringTableEntry shapeName, U32 type);

	U32						GetMaleBodySize()		{ return (U32)m_MaleBodyMap.size();}
	U32						GetFemaleBodySize()		{ return (U32)m_FemaleBodyMap.size();}
	U32						GetMaleFaceSize()		{ return (U32)m_MaleFaceMap.size();}
	U32						GetFemaleFaceSize()		{ return (U32)m_FemaleFaceMap.size();}
	U32						GetMaleHairSize()		{ return (U32)m_MaleHairMap.size();}
	U32						GetFemaleHairSize()		{ return (U32)m_FemaleHairMap.size();}

	U32						GetMaleBodyID(U32 index);
	U32						GetFemaleBodyID(U32 index);
	U32						GetMaleFaceID(U32 index);
	U32						GetFemaleFaceID(U32 index);
	U32						GetMaleHairID(U32 index);
	U32						GetFemaleHairID(U32 index);
public:
	typedef stdext::hash_map<U32, PlayerData*> PlayerDataMap;
	typedef stdext::hash_map<U32, StringTableEntry> PlayerShapeMap;

	PlayerDataMap		m_PlayerDataMap;

	PlayerShapeMap		m_MaleBodyMap;
	PlayerShapeMap		m_FemaleBodyMap;

	PlayerShapeMap		m_MaleFaceMap;
	PlayerShapeMap		m_FemaleFaceMap;

	PlayerShapeMap		m_MaleHairMap;
	PlayerShapeMap		m_FemaleHairMap;
};

extern PlayerRepository g_PlayerRepository;
