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
// ����б���

class HateList
{
public:
	enum 
	{
		MaxCount					= 16,						// ����б���
		UpdateTickMs				= 2000,						// ����ʱ��
		AttenuationTickMs			= UpdateTickMs << 3,		// ��һ��˥��ʱ�䣬��Ϊÿ�θ��²���ͬ��������ʱ���ϲ�һ����ȷ
		PlayerTimeLimitMs			= 6000,						// ��ҳ�޵������ʱ��
		NpcTimeLimitMs				= 12000,					// NPC��޵������ʱ��
	};

	enum Flags
	{
		Flags_NewObjOnly			= BIT(0),					// ֻ����µĳ�޶�����������ڳ���б��У���Ч
		Flags_NpcTeam				= BIT(1),					// �����޹���
	};

	struct Hate
	{
		SimObjectPtr<GameObject>	mObj;
		S32							mHate;						// ���ֵ
		U32							mTime;						// ʱ��ֵ

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
	const static F32				msHateOverflow;				// OT��ֵ

	GameObject*						mObj;						// �������
	HatePtrList						mHatePtrList;				// ����������������ĳ���б�
	BeHatePtrList					mBeHatePtrList;				// ��������Եı���������б�
	SimTime							mNextUpateTick;				// ��һ�θ��³��ʱ��
	SimTime							mAttenuationTick;			// ��һ��˥����ʱ��
	SimTime							mTimeLimitTick;				// ��޴��ڵ�ʱ��
	U32								mTimeLimit;					// ��޴��ڵ�ʱ������
	bool							mTimeLimitEnabled;			// ����ʱ������

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