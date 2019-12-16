//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/ai/NpcTeam.h"

// ========================================================================================================================================
//	NpcTeam
// ========================================================================================================================================
U32 NpcTeam::msTeamId = 0;

NpcTeam::NpcTeam()
{
	clear();
	mTeamId = ++msTeamId;
}

NpcTeam::~NpcTeam()
{
	clear();
}

void NpcTeam::clear()
{
	for (NpcTeamMember::iterator it = mMember.begin(); it != mMember.end(); ++it)
		if(*it)
			(*it)->mNpcTeam = NULL;

	mTeamId = 0;
	mLayerId = 0;
	mMember.clear();
}

U32 NpcTeam::size()
{
	return /*(U32)*/mMember.size();
}

bool NpcTeam::add(NpcObject* pNpc)
{
	if(!pNpc || pNpc->mNpcTeam)
	{
		AssertFatal(false, "NpcTeam::add : Npc is Null or on another team!");
		return false;
	}

	// 空的队伍
	if(mMember.empty())
		mLayerId = pNpc->getLayerID();

	if(mLayerId != pNpc->getLayerID())
	{
		AssertFatal(false, "NpcTeam::add : LayerId error!");
		return false;
	}

	mMember.push_back(pNpc);
	pNpc->mNpcTeam = this;
	return true;
}

void NpcTeam::remove(NpcObject* pNpc)
{
	if(!pNpc || mLayerId != pNpc->getLayerID())
		return;

	for (NpcTeamMember::iterator it = mMember.begin(); it != mMember.end(); ++it)
	{
		if((*it) == pNpc)
		{
			mMember.erase(it);
			pNpc->mNpcTeam = NULL;
			return;
		}
	}
	AssertFatal(false, "NpcTeam::remove : can NOT find Npc!");
}

void NpcTeam::onHateAdd(GameObject* pObj)
{
	if(!pObj || mLayerId != pObj->getLayerID())
		return;

	for (NpcTeamMember::iterator it = mMember.begin(); it != mMember.end(); ++it)
		if(*it)
			(*it)->mHateList.addHate(pObj, 1, HateList::Flags_NewObjOnly);
}

// ========================================================================================================================================
//	NpcTeamManager
// ========================================================================================================================================

NpcTeamManager gNpcTeamManager;

NpcTeamManager::NpcTeamManager()
{
	clear();
}

NpcTeamManager::~NpcTeamManager()
{
	clear();
}

void NpcTeamManager::clear()
{
	for (NpcTeamMap::iterator it = mNpcTeamMap.begin(); it != mNpcTeamMap.end(); ++it)
		if(it->second)
			delete (it->second);
	mNpcTeamMap.clear();
}

bool NpcTeamManager::join(NpcObject* pNpcA, NpcObject* pNpcB)
{
	if(!pNpcA || !pNpcB || pNpcA->getLayerID() != pNpcB->getLayerID())
		return false;
	if(!pNpcA->getTeamMatchId() || pNpcA->getTeamMatchId() != pNpcB->getTeamMatchId())
		return false;

	NpcTeam* pTeamA = pNpcA->mNpcTeam;
	NpcTeam* pTeamB = pNpcB->mNpcTeam;
	if(pTeamA == pTeamB && !pTeamA)
		return false;

	// 创建新队伍
	if(!pTeamA && !pTeamB)
	{
		pTeamA = new NpcTeam;
		mNpcTeamMap.insert(NpcTeamMap::value_type(pTeamA->mTeamId, pTeamA));
		pTeamA->add(pNpcA);
		pTeamA->add(pNpcB);
	}
	// 合并队伍
	else if(pTeamA && pTeamB)
	{
		for (NpcTeam::NpcTeamMember::iterator it = pTeamB->mMember.begin(); it != pTeamB->mMember.end(); ++it)
		{
			if(*it)
			{
				(*it)->mNpcTeam = NULL;
				pTeamA->add(*it);
				(*it) = NULL;
			}
		}
		pTeamB->mMember.clear();
		destroy(pTeamB->mTeamId);
	}
	// 加入队伍
	else
	{
		if(pTeamA)
			pTeamA->add(pNpcB);
		else
			pTeamB->add(pNpcA);
	}
	return true;
}

void NpcTeamManager::leave(NpcObject* pNpc)
{
	if(!pNpc || !pNpc->mNpcTeam)
		return;

	if(pNpc->mNpcTeam->size() <= 2)
		destroy(pNpc->mNpcTeam->mTeamId);
	else
		pNpc->mNpcTeam->remove(pNpc);
}

void NpcTeamManager::destroy(U32 id)
{
	static NpcTeamMap::iterator it;
	if(!id || (it = mNpcTeamMap.find(id)) == mNpcTeamMap.end())
		return;

	delete (it->second);
	mNpcTeamMap.erase(it);
}
