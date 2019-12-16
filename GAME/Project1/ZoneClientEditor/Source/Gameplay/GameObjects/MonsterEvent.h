#pragma once

#include "core/tVector.h"
#include <string>
#include <hash_map>

//�����¼��ܱ�
struct MonsterEventData
{
	MonsterEventData() { }

	U32					m_nEventId;		//�¼�ID
	Vector<U32>			m_packages;		//��Ʒ������
	StringTableEntry	m_script;		//ִ�нű�
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