//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SOUNDRESOURCE_H_
#define _SOUNDRESOURCE_H_
#include <hash_map>
#include "sfx/sfxDescription.h"
#include "sfx/sfxProfile.h"


#define MAX_SOUND_NUM   1024

enum SOUNDTYPE
{
	BACKGROUND_MUSIC		= 0,
	UI_MUSIC				= 1,
	AMBIENT_MUSIC			= 2,
	DYNAMIC_MUSIC			= 3,
};

class SoundManager
{
public:
//	typedef stdext::hash_map<std::string, SimObjectPtr<SFXProfile> >  SoundDataMap;
	typedef stdext::hash_map<std::string, SFXProfile* >  SoundDataMap;

	
	enum ISLOOP
	{
		ISLOOP_TRUE		= 0,
		ISLOOP_FALSE	= 1,
	};
	enum IS3D
	{
		IS3D_TRUE		= 0,
		IS3D_FALSE		= 1,
	};
	
	SoundManager();
	~SoundManager();

	bool Read();
	void Clear();
	SFXProfile* FindProfile(StringTableEntry id);
	bool addData(StringTableEntry id);
	bool removeData(StringTableEntry id);
	SFXDescription* getDescriptionByID(StringTableEntry id);
	bool readData(StringTableEntry dat);
	void insertData(StringTableEntry id, SFXProfile* des);
	//SOUNDTYPE getSoundType(StringTableEntry id);
	static void creat();
	static void destroy();

	void addAllData();

protected:
	SoundDataMap m_SoundMap;
	//COperateData m_ParserData;
};
extern SoundManager* g_SoundManager;

#endif