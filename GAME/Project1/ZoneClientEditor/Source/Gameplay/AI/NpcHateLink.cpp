//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/ai/NpcHateLink.h"


// ========================================================================================================================================
//  NpcHateLink
// ========================================================================================================================================
NpcHateLink gNpcHateLink;

NpcHateLink::NpcHateLink()
{
	clear();
}

NpcHateLink::~NpcHateLink()
{
	clear();
}

void NpcHateLink::clear()
{
	HateLinkTable::iterator it = mTable.begin();
	for (; it != mTable.end(); ++it)
		delete it->second;

	mTable.clear();
}

void NpcHateLink::add(NpcObject* pNpc)
{
	if(!pNpc || !pNpc->getHateLinkId())
		return;
	HateLinkTable::iterator it = mTable.find(pNpc->getLayerID());
	HateLinkMap* pMap = NULL;
	if(it == mTable.end() || !it->second)
	{
		pMap = new HateLinkMap;
		mTable[pNpc->getLayerID()] = pMap;
	}

#ifdef DEBUG
	HateLinkMap::iterator itr = pMap->lower_bound(pNpc->getHateLinkId());
	HateLinkMap::iterator end = pMap->upper_bound(pNpc->getHateLinkId());
	for(;itr != end; ++itr)
		AssertFatal(itr->second && itr->second != pNpc, "debug : NpcHateLink::add!");
#endif
	pMap->insert(HateLinkMap::value_type(pNpc->getHateLinkId(), pNpc));
}

void NpcHateLink::remove(NpcObject* pNpc)
{
	if(!pNpc || !pNpc->getHateLinkId())
		return;

	HateLinkTable::iterator it = mTable.find(pNpc->getLayerID());
	if(it == mTable.end() || !it->second)
	{
		AssertFatal(false, "debug : NpcHateLink::remove!");
		return;
	}

	HateLinkMap::iterator itr = it->second->lower_bound(pNpc->getHateLinkId());
	HateLinkMap::iterator end = it->second->upper_bound(pNpc->getHateLinkId());
	for(;itr != end; ++itr)
	{
		if(itr->second == pNpc)
			it->second->erase(itr);
	}
}

void NpcHateLink::onHateLink(NpcObject* pNpc, GameObject* pObj)
{
	if(!pNpc || !pNpc->getHateLinkId() || !pObj)
		return;

	HateLinkTable::iterator it = mTable.find(pNpc->getLayerID());
	if(it == mTable.end() || !it->second)
		return;

	HateLinkMap* pMap = it->second;
	HateLinkMap::iterator itr = pMap->lower_bound(pNpc->getHateLinkId());
	HateLinkMap::iterator end = pMap->upper_bound(pNpc->getHateLinkId());
#ifdef DEBUG
	for(;itr != end; ++itr)
		AssertFatal(itr->second && (itr->second == pNpc || !itr->second->isInCombat()), "debug : NpcHateLink::onHateLink!");
	itr = pMap->lower_bound(pNpc->getHateLinkId());
#endif
	for(;itr != end; ++itr)
	{
		if(itr->second && itr->second != pNpc)
			itr->second->mHateList.addHate(pObj, 1, HateList::Flags_NewObjOnly);
	}
}
