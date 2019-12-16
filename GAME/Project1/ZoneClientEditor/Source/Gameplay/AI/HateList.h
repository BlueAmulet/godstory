//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifdef NTJ_SERVER
#pragma once

#include <list>
#include "console/simObject.h"

class GameObject;

//-----------------------------------------------------------------------------
// 仇恨列表类

class HateList
{
public:
	enum 
	{
		MaxCount					= 16,						// 最大列表项
		UpdateTickMs				= 2000,						// 更新时间
		AttenuationTickMs			= UpdateTickMs << 3,		// 下一次衰减时间，因为每次更新才做同步，所以时间上不一定精确
		PlayerTimeLimitMs			= 6000,						// 玩家仇恨的最大保留时间
		NpcTimeLimitMs				= 12000,					// NPC仇恨的最大保留时间
	};

	enum Flags
	{
		Flags_NewObjOnly			= BIT(0),					// 只添加新的仇恨对象，如果对象在仇恨列表中，无效
		Flags_NpcTeam				= BIT(1),					// 队伍仇恨共享
	};

	struct Hate
	{
		SimObjectPtr<GameObject>	mObj;
		S32							mHate;						// 仇恨值
		U32							mTime;						// 时间值

		inline bool	operator < (const Hate& t) { return mHate < t.mHate;}
		Hate(GameObject* obj, S32 hate, U32 time) { mObj = obj; mHate = hate; mTime = time;}
	};

	struct BeHated
	{
		SimObjectPtr<GameObject>	mObj;

		BeHated(GameObject *obj) { mObj = obj;}
	};

	typedef std::list<Hate*> HatePtrList;
	typedef std::list<BeHated*> BeHatePtrList;
	typedef Hate* HatePtr;

private:
	const static F32				msHateOverflow;				// OT伐值

	GameObject*						mObj;						// 本体对象
	HatePtrList						mHatePtrList;				// 本体对象对其他对象的仇恨列表
	BeHatePtrList					mBeHatePtrList;				// 其他对象对的本体对象仇恨列表
	SimTime							mNextUpateTick;				// 下一次更新仇恨时间
	SimTime							mAttenuationTick;			// 下一次衰减的时间
	SimTime							mTimeLimitTick;				// 仇恨存在的时间
	U32								mTimeLimit;					// 仇恨存在的时间限制
	bool							mTimeLimitEnabled;			// 激活时间限制

protected:
	static inline bool				greatHate					(const HatePtr& h1, const HatePtr& h2) { return h1->mHate > h2->mHate;}
	inline void						sort						() { mHatePtrList.sort(greatHate);}
	void							synBeHatePtrList			(GameObject* obj, bool add = true);
	void							setTimeLimitTick			() { mTimeLimitTick = Platform::getVirtualMilliseconds() + mTimeLimit;}
	void							modifyHate					(GameObject* obj, F32 val);

public:

	HateList();
	~HateList();

	const inline HatePtrList&		getHatePtrList				() const { return mHatePtrList; }
	const inline BeHatePtrList&		getBeHatePtrList			() const { return mBeHatePtrList; }
	inline bool						isInCombat					() { return (!mHatePtrList.empty() || !mBeHatePtrList.empty());}
	inline bool						isEmpty						() { return mHatePtrList.empty();}
	inline S32						getCount					() { return mHatePtrList.size();}
	HatePtrList::iterator			find						(GameObject* obj);
	U32								getHateValue				(GameObject* obj);

	void							addHate						(GameObject* obj, S32 hate, U32 flags = Flags_NpcTeam);
	void							removeHate					(GameObject* obj, bool syn = true);

	void							init						(GameObject* obj);
	void							reset						();
	void							modifyHate					(F32 val);
	void							setTimeLimitEnabled			(bool enabled);
	void							setUpdateForce				();
	GameObject*						update						(bool force = false);
};

#endif