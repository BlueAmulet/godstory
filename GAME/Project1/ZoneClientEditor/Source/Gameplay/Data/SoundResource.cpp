//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/gameFunctions.h"
#include "SoundResource.h"
#include "core/stringTable.h"
//#include "DatBuilder/DatBuilder.h"
#include "Gameplay/data/readDataFile.h"


SoundManager* g_SoundManager = NULL;


SoundManager::SoundManager()
{
	
}

SoundManager::~SoundManager()
{
}

void SoundManager::creat()
{
	AssertFatal (g_SoundManager == NULL,
		"ResourceManager::create: manager already exists.");
	g_SoundManager = new SoundManager;
	
}

void SoundManager::destroy()
{
	
	AssertFatal (g_SoundManager != NULL,
		"ResourceManager::destroy: manager does not exist.");
	delete g_SoundManager;
	g_SoundManager = NULL;
}

bool SoundManager::Read()
{
	CDataFile  op;
	RData tempdata;
	op.ReadDataInit();
	char filename[1024];
	Platform::makeFullPathName(GAME_SOUNDDATA_FILE, filename, sizeof(filename));
    op.readDataFile(filename);
	for(int i =0; i<op.RecordNum; ++i)
	{
		SFXDescription* description = new SFXDescription;
		SFXProfile* profile = new SFXProfile();
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_string, "SoundRepository.dat::Read - failed to read mSoundID!");
		description->mSoundID = StringTable->insert(tempdata.m_string);
		profile->mSoundID = StringTable->insert(tempdata.m_string);
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_string, "SoundRepository.dat::Read - failed to read mFileName!");
		char path[256];
		dSprintf(path, sizeof(path), "%s%s", "gameres/data/sounds/", StringTable->insert(tempdata.m_string));
		//dSprintf(description->mFileName, sizeof(description->mFileName), "%s", path);
		description->mFileName = StringTable->insert(path);
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_enum8, "SoundRepository.dat::Read - failed to read mSoundChannel!");
		description->mSoundChannel = tempdata.m_Enum8;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_F32, "SoundRepository.dat::Read - failed to read mVolume!");
		description->mVolume = tempdata.m_F32;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_S8, "SoundRepository.dat::Read - failed to read mLoopcount!");
		description->mLoopcount = tempdata.m_S8;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U8, "SoundRepository.dat::Read - failed to read mPriority!");
		description->mPriority = tempdata.m_U8;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_enum8, "SoundRepository.dat::Read - failed to read mIs3D!");
		description->mIs3D = tempdata.m_Enum8;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_F32, "SoundRepository.dat::Read - failed to read mReferenceDistance!");
		description->mReferenceDistance = tempdata.m_F32;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_F32, "SoundRepository.dat::Read - failed to read mMaxDistance!");
		description->mMaxDistance = tempdata.m_F32;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U16, "SoundRepository.dat::Read - failed to read mConeInsideAngle!");
		description->mConeInsideAngle = tempdata.m_U16;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U16, "SoundRepository.dat::Read - failed to read mConeOutsideAngle!");
		description->mConeOutsideAngle = tempdata.m_U16;
		//
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_F32, "SoundRepository.dat::Read - failed to read mConeOutsideVolume!");
		description->mConeOutsideVolume = tempdata.m_F32;
		profile->Insert(description);
		insertData(description->mSoundID, profile);

		addToUserDataBlockGroup(description);
		addToUserDataBlockGroup(profile);
	}
	//addAllData();
	op.ReadDataClose();
	return true;
}

void SoundManager::Clear()
{
	SoundDataMap::iterator it = m_SoundMap.begin();
	for(; it != m_SoundMap.end(); ++it)
	{
		//SimObjectPtr<SFXProfile> profile = (SFXProfile*)it->second;		
		//if(!profile.isNull())
		//	delete profile;

		delete it->second;
	}
	m_SoundMap.clear();
}

SFXProfile* SoundManager::FindProfile(StringTableEntry id)
{
	SoundDataMap::iterator it;
	it = m_SoundMap.find(id);
	if(it != m_SoundMap.end())
		return it->second;
	else
	    return NULL;
}

bool SoundManager::addData(StringTableEntry id)
{
	if(m_SoundMap.size() > MAX_SOUND_NUM)
		return false;
	SoundDataMap::iterator it;
	it = m_SoundMap.find(id);
	if(it == m_SoundMap.end())
	{
		SFXProfile* profile = new SFXProfile();
		profile->Insert(getDescriptionByID(id));
		m_SoundMap[id] = profile;
	}
		
	return false;
}

bool SoundManager::removeData(StringTableEntry id)
{
	SoundDataMap::iterator it;
	it = m_SoundMap.find(id);
	if(it != m_SoundMap.end())
	{
		delete m_SoundMap[id];
		m_SoundMap.erase(it);
		return true;
	}
	return false;
}

SFXDescription* SoundManager::getDescriptionByID(StringTableEntry id)
{
	//COperateData op;
	/*RData temp[15];
	RData temp2;
	for (int i=0; i<op.RecordNum; ++i)
	{
		for(int j=0; j<op.ColumNum; ++j)
		{
			op.GetData(temp2);
			temp[j] = temp2;
		}
		if(StringTable->insert(temp[0].m_string) == id)
		{
			SFXDescription* description = new SFXDescription;
			description->mSoundID = StringTable->insert(temp[0].m_string);
			description->mFileName = StringTable->insert(temp[1].m_string);
			description->mSoundChannel = temp[2].m_S32;
			description->mVolume = temp[3].m_F32;
			description->mPriority = temp[5].m_S32;
			description->mLoopcount = temp[4].m_S32;
			description->mIs3D = temp[6].m_S32;
			description->mReferenceDistance = temp[7].m_F32;
			description->mMaxDistance = temp[8].m_F32;
			description->mConeInsideAngle = temp[9].m_S32;
			description->mConeOutsideAngle = temp[10].m_S32;
			description->mConeOutsideVolume = temp[11].m_F32;
			
			
			op.ReadDataClose();

			return description;
		}
	}
	op.ReadDataClose();*/
	SoundDataMap::iterator it;
	it = m_SoundMap.find(id);
	if(it != m_SoundMap.end())
		return it->second->getDescription();
		
	return NULL;
}

void SoundManager::insertData(StringTableEntry id, SFXProfile* des)
{
	if(des)
	{
		SoundDataMap::iterator it = m_SoundMap.find(id);
		if(it == m_SoundMap.end())
			m_SoundMap[id] = des;

	}
}

//SOUNDTYPE SoundManager::getSoundType(StringTableEntry id)
//{
//	SoundDataMap::iterator it = m_SoundMap.find(id);
//	if(it != m_SoundMap.end())
//	{
//		
//	}
//	return BACKGROUND_MUSIC;
//}

void SoundManager::addAllData()
{
//	COperateData op;
	//RData temp[15];
	//RData temp2;
	//for (int i=0; i<op.RecordNum; ++i)
	//{
	//	/*for(int j=0; j<op.ColumNum; ++j)
	//	{
	//		op.GetData(temp2);
	//		temp[j] = temp2;
	//	}
	//	
	//	SFXDescription* description = new SFXDescription;
	//	description->mSoundID = StringTable->insert(temp[0].m_string);
	//	description->mFileName = StringTable->insert(temp[1].m_string);
	//	description->mSoundChannel = temp[2].m_S32;
	//	description->mVolume = temp[3].m_F32;
	//	description->mPriority = temp[5].m_S32;
	//	description->mLoopcount = temp[4].m_S32;
	//	description->mIs3D = temp[6].m_S32;
	//	description->mReferenceDistance = temp[7].m_F32;
	//	description->mMaxDistance = temp[8].m_F32;
	//	description->mConeInsideAngle = temp[9].m_S32;
	//	description->mConeOutsideAngle = temp[10].m_S32;
	//	description->mConeOutsideVolume = temp[11].m_F32;
	//	SFXProfile* profile = new SFXProfile();
	//	profile->mSoundID = StringTable->insert(temp[0].m_string);
	//	profile->Insert(description);*/
	//	

	//		
	//	
	//}
	

}