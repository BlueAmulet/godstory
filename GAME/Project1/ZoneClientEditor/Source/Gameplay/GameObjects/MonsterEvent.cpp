#include "core/stringTable.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/GameObjects/MonsterEvent.h"

#ifdef  MAX_INTEGER_LENGTH
#undef  MAX_INTEGER_LENGTH
#endif

#define MAX_INTEGER_LENGTH 20

#define MONSTER_EVENT_DATA_FILE "gameres/data/MonsterEventRepository.dat"


MonsterEventRepository g_MonsterEventRepository;


MonsterEventRepository::MonsterEventRepository()
{
	m_Delimiter = ' ';
}

MonsterEventRepository::~MonsterEventRepository()
{
	Clear();
}

MonsterEventData* MonsterEventRepository::GetMonsterEventData(U32 nEventID) const
{
	MonsterEventMap::const_iterator it = m_MonsterEventMap.find(nEventID);

	if (it != m_MonsterEventMap.end())
	{
		return it->second;
	}

	return NULL;
}

void MonsterEventRepository::Read()
{
	CDataFile op;
	RData tempdata;
	char filename[256];
	MonsterEventData* pMonsterEventData = NULL;

	op.ReadDataInit();
	Platform::makeFullPathName(MONSTER_EVENT_DATA_FILE, filename, sizeof(filename));
	op.readDataFile(filename);

	for(int i=0; i<op.RecordNum; ++i)
	{
		pMonsterEventData = new MonsterEventData;

		// read Monster Event ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_S32, "ItemPackageRepository.dat::Read - Failed to read monster event Id", i);
		AssertErrorMsg(GetMonsterEventData(tempdata.m_S32) == NULL, "ItemPackageRepository.dat::Read - monster event Id already registered", i);
		pMonsterEventData->m_nEventId = tempdata.m_S32;

		// read item package ids string
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "ItemPackageRepository.dat::Read - Failed to read item package ids string", i);
		AssertErrorMsg(	MakeIntegers(pMonsterEventData, tempdata.m_string), 
						"ItemPackageRepository.dat::Read - Format of item package ids is not correct", i);

		// read script command
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "ItemPackageRepository.dat::Read - Failed to read item package ids string", i);
		pMonsterEventData->m_script = StringTable->insert(tempdata.m_string);

		Insert(pMonsterEventData);
	}
}

bool MonsterEventRepository::MakeIntegers(MonsterEventData* pMonsterEventData, const char *strIds)
{
	char integer[MAX_INTEGER_LENGTH + 1];
	U32	 size = 0;
	const char *pStart = NULL;
	const char *pEnd = NULL;
	U32 count = 0;  // number of integers

	pStart = strIds;
	while(*pStart != 0)
	{
		if (*pStart == m_Delimiter)
		{
			pStart++;
			continue;
		}

		pEnd = pStart;

		while(*pEnd >= '0' && *pEnd <= '9')
		{
			pEnd++;
		}

		if (*pEnd != m_Delimiter && *pEnd != 0)
		{
			return false;
		}

		size = pEnd - pStart;
		if (size > 0 && size <= MAX_INTEGER_LENGTH)
		{
			count++;
			dMemcpy(integer, pStart, size);
			integer[size] = 0;
			pMonsterEventData->m_packages.push_back(atoi(integer));
		}
		else if (size > MAX_INTEGER_LENGTH)
		{
			return false;
		}

		pStart = pEnd;
	}

	if (count == 0)
	{
		return false;
	}

	return true;
}

void MonsterEventRepository::Clear()
{
	for(MonsterEventMap::iterator it = m_MonsterEventMap.begin(); it != m_MonsterEventMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	m_MonsterEventMap.clear();
}

bool MonsterEventRepository::Insert(MonsterEventData* pMonsterEventData)
{
	if(!pMonsterEventData)
		return false;

	m_MonsterEventMap.insert( MonsterEventMap::value_type(pMonsterEventData->m_nEventId, pMonsterEventData) );
	return true;
}

void MonsterEventRepository::AssertErrorMsg(bool bAssert, const char *msg, int rowNum)
{
	char buf[256];

	if (!bAssert)
	{
		dSprintf(buf, sizeof(buf), "RowNum: %d  ", rowNum);
		dStrcat(buf, sizeof(buf), msg);
		AssertRelease(false, msg);
	}
}