//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifdef NTJ_SERVER
#include "GamePlay/GameObjects/PlayerObject.h"
#include "GamePlay/GameObjects/NpcObject.h"
#include "Gameplay/ai/HateList.h"
#include "Gameplay/ai/NpcHateLink.h"
#include "Gameplay/ai/NpcTeam.h"

#ifdef getCurrentTime
#undef getCurrentTime
#endif

//-------------------------------------------------------------------------------------
//	HateList
const F32 HateList::msHateOverflow = 1.10f;

HateList::HateList() :
	mObj(NULL),
	mNextUpateTick(0),
	mAttenuationTick(0),
	mTimeLimitTick(0),
	mTimeLimit(NpcTimeLimitMs),
	mTimeLimitEnabled(false)
{
}

HateList::~HateList()
{
	reset();
}

void HateList::synBeHatePtrList(GameObject* obj, bool add /* = true */)
{
	if(!obj)
		return;

	if(add)
	{
		if(obj->getGameObjectMask() & NpcObjectType)
		{
			for(BeHatePtrList::iterator it = mBeHatePtrList.begin(); it != mBeHatePtrList.end(); ++it)
				if(*it && (*it)->mObj && ((*it)->mObj->getGameObjectMask() & NpcObjectType) && gNpcTeamManager.join((NpcObject*)(*it)->mObj.getObject(), (NpcObject*)obj))
					break;
		}

		BeHated* pBeHate = new BeHated(obj);
		mBeHatePtrList.push_back(pBeHate);
		mObj->setInCombat(isInCombat());
		return;
	}

	BeHatePtrList::iterator it;
	for(it = mBeHatePtrList.begin(); it != mBeHatePtrList.end(); ++it)
	{
		if(*it && (*it)->mObj == obj)
		{
			delete (*it);
			mBeHatePtrList.erase(it);
			mObj->setInCombat(isInCombat());
			return;
		}
	}
	//AssertFatal(false, "HateList::synBeHatePtrList : on Remove");
}

HateList::HatePtrList::iterator HateList::find(GameObject* obj)
{
	if(obj)
	{
		HatePtrList::iterator it;
		for(it = mHatePtrList.begin(); it != mHatePtrList.end(); ++it)
		{
			if (*it && (*it)->mObj == obj)
				return it;
		}
	}

	return mHatePtrList.end();
}

U32	HateList::getHateValue(GameObject* obj)
{
	HatePtrList::iterator itr = find(obj);
	if (itr != mHatePtrList.end() && (*itr))
		return (*itr)->mHate;
	return 0;
}

void HateList::addHate(GameObject* obj, S32 hate, U32 flags /* = */ )
{
	// 没有目标或者仇恨为0
	if(!obj || !hate)
		return;

	// 添加仇恨
	if(hate > 0)
	{
		// =========================== 归属者 test==========================
		if(mObj->getGameObjectMask() & NpcObjectType)
		{
			NpcObject* pNpc = (NpcObject*)mObj;
			if(!pNpc->getBelongTo())
			{
				if(obj->getGameObjectMask() & PlayerObjectType)
					pNpc->setBelongTo((Player*)obj);
				else if(obj->getGameObjectMask() & PetObjectType)
					pNpc->setBelongTo(((PetObject*)obj)->getMaster());
			}
		}
		// =========================== 归属者 test==========================

		// 重新设定仇恨有效时间
		setTimeLimitTick();
		if(!(obj->getGameObjectMask() & NpcObjectType))
			obj->mHateList.setTimeLimitTick();

		HateList::HatePtrList::iterator it = find(obj);
		if (it == mHatePtrList.end())
		{
			hate = getMax(hate, S32(1));
			if(mHatePtrList.size() >= MaxCount)
			{
				Hate* pHate = mHatePtrList.back();
				if(pHate && hate > pHate->mHate)
				{
					if(pHate->mObj)
						pHate->mObj->mHateList.synBeHatePtrList(mObj, false);
					pHate->mObj = obj;
					pHate->mHate = hate;
					pHate->mTime = Platform::getVirtualMilliseconds();
					sort();
					obj->mHateList.synBeHatePtrList(mObj, true);
				}
			}
			else
			{
				Hate* pHate = new Hate(obj, hate, Platform::getVirtualMilliseconds());
				mHatePtrList.push_back(pHate);
				obj->mHateList.synBeHatePtrList(mObj, true);
			}
			// 超过最大数量，需要删除（正常情况下不应该执行这一步）
			while(mHatePtrList.size() > MaxCount)
			{
				Hate* pHate = mHatePtrList.back();
				if(pHate && pHate->mObj)
					pHate->mObj->mHateList.synBeHatePtrList(mObj, false);
				SAFE_DELETE(pHate);
				mHatePtrList.pop_back();
			}

			// team
			if((mObj->getGameObjectMask() & NpcObjectType) && (flags & Flags_NpcTeam))
			{
				NpcObject* npc = (NpcObject*)mObj;
				if(npc->mNpcTeam)
					npc->mNpcTeam->onHateAdd(obj);
			}
			mObj->setInCombat(isInCombat(), obj);
			return;
		}
		else
		{
			if(flags & Flags_NewObjOnly)
				return;
			(*it)->mTime = Platform::getVirtualMilliseconds();
			(*it)->mHate += hate;
			if ((*it)->mHate < 1)
				(*it)->mHate = 1;
		}
	}
	// 给关联者添加仇恨
	else
	{
		hate = mAbs(hate);
		for (BeHatePtrList::iterator it=mBeHatePtrList.begin(); it!=mBeHatePtrList.end(); ++it)
			if((*it) && (*it)->mObj)
				(*it)->mObj->mHateList.addHate(obj, hate);
	}
}

void HateList::removeHate(GameObject* obj, bool syn /* = true */)
{
	if (obj == mObj->GetTarget())
		mObj->SetTarget(NULL, false, GameObject::SetTargetToClient);
	HatePtrList::iterator itr = find(obj);
	if (itr != mHatePtrList.end())
	{
		if(syn && (*itr)->mObj)
			(*itr)->mObj->mHateList.synBeHatePtrList(mObj, false);
		SAFE_DELETE(*itr);
		mHatePtrList.erase(itr);
		mObj->setInCombat(isInCombat());
	}
}

void HateList::init(GameObject* obj)
{
	AssertFatal(obj, "HateList::init");
	mObj = obj;
	mObj->setInCombat(isInCombat());
	if(mObj->getGameObjectMask() & PlayerObjectType)
	{
		mTimeLimit = PlayerTimeLimitMs;
		mTimeLimitEnabled = true;
	}
}

void HateList::reset()
{
	for(HatePtrList::iterator it = mHatePtrList.begin(); it != mHatePtrList.end(); ++it)
	{
		if((*it) && (*it)->mObj)
			(*it)->mObj->mHateList.synBeHatePtrList(mObj, false);
		SAFE_DELETE(*it);
	}
	for(BeHatePtrList::iterator it = mBeHatePtrList.begin(); it != mBeHatePtrList.end(); ++it)
	{
		if((*it) && (*it)->mObj)
			(*it)->mObj->mHateList.removeHate(mObj, false);
		SAFE_DELETE(*it);
	}
	mHatePtrList.clear();
	mBeHatePtrList.clear();
	if(mObj)
		mObj->setInCombat(isInCombat());
}

void HateList::modifyHate(GameObject* obj, F32 val)
{
	HatePtrList::iterator itr = find(obj);
	if (itr != mHatePtrList.end())
		(*itr)->mHate += val * (*itr)->mHate;
}

void HateList::modifyHate(F32 val)
{
	if(!val)
		return;
	val = mClampF(val, -1.0f, 1.0f);
	// 清除其他对象对自己的仇恨
	if(val < -0.999f)
	{
		for(BeHatePtrList::iterator it = mBeHatePtrList.begin(); it != mBeHatePtrList.end(); ++it)
		{
			if((*it) && (*it)->mObj)
				(*it)->mObj->mHateList.removeHate(mObj, false);
			SAFE_DELETE(*it);
		}
		mBeHatePtrList.clear();
	}

	// 使其他对象对自己的仇恨最小化
	for(BeHatePtrList::iterator it = mBeHatePtrList.begin(); it != mBeHatePtrList.end(); ++it)
	{
		if((*it) && (*it)->mObj)
			(*it)->mObj->mHateList.modifyHate(mObj, val);
	}
}

void HateList::setTimeLimitEnabled(bool enabled)
{
	if(mTimeLimitEnabled == enabled)
		return;
	// 开始启用的时候，设置一次时间
	if(!mTimeLimitEnabled)
		setTimeLimitTick();
	mTimeLimitEnabled = enabled;
}

void HateList::setUpdateForce()
{
	mNextUpateTick = Platform::getVirtualMilliseconds() + UpdateTickMs;
}

GameObject* HateList::update(bool force /* = false */)
{
	if(mHatePtrList.empty())
		return NULL;
	if(mNextUpateTick > Platform::getVirtualMilliseconds() && !force)
		return mObj->GetTarget();
	if(mTimeLimitEnabled && mTimeLimitTick < Platform::getVirtualMilliseconds())
	{
		reset();
		return NULL;
	}

	// 嘲讽
	GameObject* target = mObj->GetBuffTable().getMocker();
	if(target)
		return target;

	target = mObj->GetTarget();
	S32 hate = target ? getHateValue(target) : 0;
	sort();
	for(HatePtrList::iterator itr = mHatePtrList.begin(); itr != mHatePtrList.end();)
	{
		if(!(*itr) || !(*itr)->mObj || (*itr)->mObj->isDisabled() || (*itr)->mHate <= 0)
		{
			if((*itr) && (*itr)->mObj)
				(*itr)->mObj->mHateList.synBeHatePtrList(mObj, false);
			SAFE_DELETE(*itr);
			mHatePtrList.erase(itr++);
			mObj->setInCombat(isInCombat());
			continue;
		}
		if(!target || hate * msHateOverflow < (*itr)->mHate)
		{
			target = (*itr)->mObj;
			hate = (*itr)->mHate;
		}
		if(mAttenuationTick >= Platform::getVirtualMilliseconds())
			(*itr)->mHate = getMax((*itr)->mHate >> 1, (S32)1);
		itr++;
	}

	mNextUpateTick = Platform::getVirtualMilliseconds() + UpdateTickMs;
	if(mAttenuationTick >= Platform::getVirtualMilliseconds())
		mAttenuationTick = Platform::getVirtualMilliseconds() + AttenuationTickMs;

	return target;
}



#endif