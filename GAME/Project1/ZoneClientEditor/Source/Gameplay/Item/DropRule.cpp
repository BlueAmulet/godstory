#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/Item/DropRule.h"

DropRuleRepository g_DropRuleRepository;

DropRuleRepository::DropRuleRepository()
{
}

DropRuleRepository::~DropRuleRepository()
{
	clear();
}

void DropRuleRepository::clear()
{
	for(DropRuleMap::iterator it = m_dropRuleMap.begin(); it != m_dropRuleMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	m_dropRuleMap.clear();
}

void DropRuleRepository::read()
{
	CDataFile op;
	RData tempdata;
	char filename[256];
	DropRuleData* pDropRuleData = NULL;
	RuleData *pRuleData = NULL;
	U8 nRuleType;
	U32 nEventID;

	Platform::makeFullPathName(GAME_DROPRULE_FILE, filename, sizeof(filename));
	if(!op.readDataFile(filename))
		return;

	for(int i=0; i<op.RecordNum; ++i)
	{
		pRuleData = new RuleData;

		// read Drop Event ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "DropRuleRepository.dat::Read - failed to read m_nDropEventId!", i);
		nEventID = tempdata.m_U32;

		// read Drop Event ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_enum8, "DropRuleRepository.dat::Read - failed to read m_nRuleType!", i);
		nRuleType = tempdata.m_Enum8;

		if ( (pDropRuleData = getDropRuleData(nEventID)) == NULL)
		{
			pDropRuleData = new DropRuleData;
			pDropRuleData->m_nDropEventId = nEventID;
			pDropRuleData->m_nRuleType = nRuleType;
			insert(pDropRuleData);
		}
		else
		{
			if (pDropRuleData->m_nRuleType != nRuleType)
			{
				AssertErrorMsg(tempdata.m_Type == pDropRuleData->m_nRuleType != nRuleType, "DropRuleRepository.dat::Read - duplicate RuleType for one EventID!", i);
			}
		}
		// read Low Level
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_S16, "DropRuleRepository.dat::Read - failed to read m_nLowLevel!", i);
		AssertErrorMsg(tempdata.m_S16 >= -150 && tempdata.m_S16 <= 150, "DropRuleRepository.dat::Read - m_nLowLevel is out of range!", i);
		pRuleData->m_nLowLevel = tempdata.m_S16;

		// read High Level
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_S16, "DropRuleRepository.dat::Read - failed to read m_nHighLevel!", i);
		AssertErrorMsg(tempdata.m_S16 >= -150 && tempdata.m_S16 <= 150, "DropRuleRepository.dat::Read - m_nLowLevel is out of range!", i);
		pRuleData->m_nHighLevel = tempdata.m_S16;

		// read ItemPackage Id
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "DropRuleRepository.dat::Read - failed to read m_nItemPackageId!", i);
		pRuleData->m_nItemPackageId = tempdata.m_U32;

		// read Max Drop Number
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_S32, "DropRuleRepository.dat::Read - failed to read m_nMaxNum!", i);
		pRuleData->m_nMaxNum = tempdata.m_S32;

		pDropRuleData->m_vtRuleData.push_back(pRuleData);
	}
}

bool DropRuleRepository::insert( DropRuleData *pDropRuleData )
{
	if(!pDropRuleData)
		return false;

	m_dropRuleMap.insert( DropRuleMap::value_type(pDropRuleData->m_nDropEventId, pDropRuleData) );
	return true;
}

DropRuleData* DropRuleRepository::getDropRuleData( U32 nDropEventId )
{
	DropRuleMap::iterator it = m_dropRuleMap.find(nDropEventId);

	if (it != m_dropRuleMap.end())
		return it->second;

	return NULL;
}

void DropRuleRepository::AssertErrorMsg(bool bAssert, const char *msg, int rowNum)
{
	char buf[256];

	if (!bAssert)
	{
		dSprintf(buf, sizeof(buf), "RowNum: %d  ", rowNum);
		dStrcat(buf, sizeof(buf), msg);
		AssertRelease(false, buf);
	}
}

bool DropRuleData::GetItemPackageID(U32 nDropEventId, U32 nPlayerLevel, U32 nNpcLevel, U32 &nItemPackageId, S32 &nMaxNum, bool bUsePlayerLevelOnly/* = true*/)
{
	S32 nLevel = 0;

	if (this->m_nDropEventId != nDropEventId)
		return false;

	switch(this->m_nRuleType)
	{
	case 0:		// PLAYER_LEVEL	
		{
			nLevel = (S32)nPlayerLevel;
			break;
		}
	case 1:		// NPC_LEVEL
		{
			if (bUsePlayerLevelOnly)
				return false;

			nLevel = (S32)nNpcLevel;
			break;
		}
	case 2:		// PLAYER_LEVEL_DIFF
		{
			if (bUsePlayerLevelOnly)
				return false;

			nLevel = (S32)nPlayerLevel - (S32)nNpcLevel;
			break;
		}
	default:
		return false;
	}

	Vector<RuleData *>::iterator it = m_vtRuleData.begin();
	while(it != m_vtRuleData.end())
	{
		if ( nLevel >= (*it)->m_nLowLevel && nLevel <= (*it)->m_nHighLevel )
		{
			nItemPackageId = (*it)->m_nItemPackageId;
			nMaxNum = (*it)->m_nMaxNum;
			return true;
		}

		++it;
	}

	return false;
}