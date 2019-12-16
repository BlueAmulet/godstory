#pragma once

#include "core/tVector.h"
#include <string>
#include <hash_map>

//怪物事件总表
struct MonsterEventData
{
	MonsterEventData() { }

	U32					m_nEventId;		//事件ID
	Vector<U32>			m_packages;		//物品包数组
	StringTableEntry	m_script;		//执行脚本
};

class MonsterEventRepository
{
public:	
	typedef stdext::hash_map<U32, MonsterEventData*> MonsterEventMap;

	MonsterEventRepository();
	~MonsterEventRepository();

	MonsterEventData*	GetMonsterEventData(U32 nEventID) const;
	void	Read();

	void	Clear();

private:
	bool	Insert(MonsterEventData* pMonsterEventData);
	bool	MakeIntegers(MonsterEventData* pMonsterEventData, const char *strIds);
	void	AssertErrorMsg(bool bAssert, const char *msg, int rowNum);
	MonsterEventMap			m_MonsterEventMap;

	char	m_Delimiter;
};

extern MonsterEventRepository g_MonsterEventRepository;