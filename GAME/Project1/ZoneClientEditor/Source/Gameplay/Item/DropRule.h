#pragma once

#include "platform/types.h"
#include "core/tVector.h"
#include <hash_map>

struct RuleData
{
	S16	m_nLowLevel;
	S16	m_nHighLevel;
	U32	m_nItemPackageId;
	S32 m_nMaxNum;

	~RuleData() { }
};

struct DropRuleData
{
	U32 m_nDropEventId;
	U8	m_nRuleType;
	Vector<RuleData *> m_vtRuleData;

	~DropRuleData() 
	{
		Vector<RuleData *>::iterator it = m_vtRuleData.begin();
		while(it != m_vtRuleData.end())
		{
			delete (*it);
			m_vtRuleData.erase(it);
		}
	}

	bool GetItemPackageID(U32 nDropEventId, U32 nPlayerLevel, U32 nNpcLevel, U32 &nItemPackageId, S32 &nMaxNum, bool bUsePlayerLevelOnly = false);
};

class DropRuleRepository
{
	
public:
	typedef stdext::hash_map<U32, DropRuleData *> DropRuleMap;

	DropRuleRepository();
	~DropRuleRepository();

	void read();
	void clear();
	bool insert(DropRuleData* pDropRuleData);
	DropRuleData* getDropRuleData(U32 nDropEventId);

private:
	void AssertErrorMsg(bool bAssert, const char *msg, int rowNum);

private:
	DropRuleMap m_dropRuleMap;
};

extern DropRuleRepository g_DropRuleRepository;