//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.

// ����˵��:����data������ϸ�����˳���
//          �¼�dataԤ�����ڿտ�����գ�����Ų���Ϊ��
//-----------------------------------------------------------------------------
#ifndef _CALLBOARDDATA_H_
#define _CALLBOARDDATA_H_

#include "console/sim.h"

class CallboardData
{
protected:
	U16                          mIndexID;                     // ���
public:
	CallboardData() : mIndexID(0){}
	virtual inline U16               getIndexID      ()         { return mIndexID;}
};

class IColumnData;
//------------------------------------------------------------------------------------------------------
//                                   �ʱ���
//------------------------------------------------------------------------------------------------------
class CallboardDailyData : public CallboardData
{
	friend class CallboardDailyRepository;

	struct dataTime 
	{
		U16              min;              // һ�����
		std::string      yearday;          // ��-��-��
		std::string      longTime;         // ʱ:��
	};

protected:
	U8                           mDegree;                      // �Ƽ�ָ��                      
	U8                           mWeekDay;                     // ����(0~7)
	U32                          mLimitLevel;                  // ���Ƶȼ�
	StringTableEntry             mNotifyTime;                  // Ԥ��ʱ��
	StringTableEntry             mStartTime;                   // ��ʼʱ��
	StringTableEntry             mEndTime;                     // ����ʱ��
	StringTableEntry             mCaptionName;                 // �����
	StringTableEntry             mLineName;                    // ������·
	StringTableEntry             mMapName;                     // ��ͼ����
	StringTableEntry             mNpcID;                       // npc���    ע:���ܻ��ж��Npc,�м���","�ָ�,������Ѱ��
	U8                           mNotifyCount;
	dataTime                     mStartDataTime;
	dataTime                     mEndDataTime;
	
public:
	CallboardDailyData();

	inline U8                    getDegree           ()         { return mDegree;}
	inline U8                    getWeekDay          ()         { return mWeekDay;}
	inline U32                   getLimitLevel       ()         { return mLimitLevel;}
	inline StringTableEntry      getStartTime        ()         { return mStartTime;}
	inline StringTableEntry      getEndTime          ()         { return mEndTime;}
	inline StringTableEntry      getNotifyTime       ()         { return mNotifyTime;}
	inline StringTableEntry      getCaptionName      ()         { return mCaptionName;}
	inline StringTableEntry      getLineName         ()         { return mLineName;}
	inline StringTableEntry      getMapName          ()         { return mMapName;}
	inline StringTableEntry      getNpcID            ()         { return mNpcID;}
	inline U8                    getNotifyCount      ()         { return mNotifyCount;}
	inline dataTime&             getStartDataTime    ()         { return mStartDataTime;}
	inline dataTime&             getEndDataTime      ()         { return mEndDataTime;}

};

class CallboardDailyRepository
{
private:
	Vector<CallboardDailyData*>       mDailyVect;

public:
	IColumnData* mColumnData;

public:
	CallboardDailyRepository();
	~CallboardDailyRepository();

	void      clear();
	bool      read();
	inline Vector<CallboardDailyData*>&   getDailyData()    { return mDailyVect;}
};
extern CallboardDailyRepository g_CallboardDailyRepository;
//--------------------------------------------------------------------------------------------------------
//                                    �ճ�����ʱ���
//--------------------------------------------------------------------------------------------------------
class CallboardMissionData : public CallboardData
{
	friend class CallboardMissionRepository;
protected:
	U32                         mMissionID;                   // ������
	U8                          mDegree;                      // �Ƽ�ָ��
	U8                          mWeekDay;                     // ����ʱ������(0~7)
	U32                         mLimitLevel;                  // ���Ƶȼ�
	U16                         mLoopNum;                     // ѭ������
	bool                        mbAppend;                     // ���⽱��
	StringTableEntry            mCaptionName;                 // ��������
	U32                         mNpcID;                       // Ѱ��NPC���
public:
	CallboardMissionData();

	inline S32                getMissionID          ()          { return mMissionID;}
	inline U8                 getDegree             ()          { return mDegree;}
	inline U8                 getWeekDay            ()          { return mWeekDay;}
	inline U32                getLimitLevel         ()          { return mLimitLevel;}
	inline U16                getLoopNum            ()          { return mLoopNum;}
	inline bool               isAppend              ()          { return mbAppend;}
	inline StringTableEntry   getCaptionName        ()          { return mCaptionName;}
	inline U32                getNpcID              ()          { return mNpcID;}
};

class CallboardMissionRepository
{
private:
	Vector<CallboardMissionData*> mMissionVector;
public:
	IColumnData* mColumnData;

public:
	CallboardMissionRepository();
	~CallboardMissionRepository();

	void    clear();
	bool    read();
	inline Vector<CallboardMissionData*>& getMissionData()  { return mMissionVector;}
};
extern CallboardMissionRepository g_CallboardMissionRepository;
//----------------------------------------------------------------------------------------------------------
//                                �����¼�
//----------------------------------------------------------------------------------------------------------
class CallboardEventData : public CallboardData
{
	friend class CallboardEventRepository;
protected:

	U32                         mNpcID;                       // npc���,������Ѱ��
	StringTableEntry            mEventName;                   // ȫ���¼�����
	StringTableEntry            mMapName;                     // ��ͼ����
public:
	CallboardEventData();

	inline U32                  getNpcID          ()      { return mNpcID;}
	inline StringTableEntry     getMapName        ()      { return mMapName;}
	inline StringTableEntry     getEventName      ()      { return mEventName;}
};

class CallboardEventRepository
{
private:
	Vector<CallboardEventData*> mEventVector;
public:
	IColumnData* mColumnData;

public:
	CallboardEventRepository();
	~CallboardEventRepository();

	void      clear();
	bool      read();
	inline Vector<CallboardEventData*>& getEventData() { return mEventVector;}
};
extern CallboardEventRepository g_CallboardEventRepository;

static S32 QSORT_CALLBACK idCompare(const void *a,const void *b)
{
	CallboardData* pA = *(CallboardData**)(a);
	CallboardData* pB = *(CallboardData**)(b);
	S32 result = pA->getIndexID() - pB->getIndexID();
	return result;
}

#endif