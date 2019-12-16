//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.

// 描述说明:所有data的序号严格按照序顺填号
//          事件data预留项内空可以填空，但序号不能为空
//-----------------------------------------------------------------------------
#ifndef _CALLBOARDDATA_H_
#define _CALLBOARDDATA_H_

#include "console/sim.h"

class CallboardData
{
protected:
	U16                          mIndexID;                     // 序号
public:
	CallboardData() : mIndexID(0){}
	virtual inline U16               getIndexID      ()         { return mIndexID;}
};

class IColumnData;
//------------------------------------------------------------------------------------------------------
//                                   活动时间表
//------------------------------------------------------------------------------------------------------
class CallboardDailyData : public CallboardData
{
	friend class CallboardDailyRepository;

	struct dataTime 
	{
		U16              min;              // 一天分钟
		std::string      yearday;          // 年-月-日
		std::string      longTime;         // 时:分
	};

protected:
	U8                           mDegree;                      // 推荐指数                      
	U8                           mWeekDay;                     // 星期(0~7)
	U32                          mLimitLevel;                  // 限制等级
	StringTableEntry             mNotifyTime;                  // 预告时间
	StringTableEntry             mStartTime;                   // 开始时间
	StringTableEntry             mEndTime;                     // 结束时间
	StringTableEntry             mCaptionName;                 // 活动名称
	StringTableEntry             mLineName;                    // 开放线路
	StringTableEntry             mMapName;                     // 地图名称
	StringTableEntry             mNpcID;                       // npc编号    注:可能会有多个Npc,中间用","分隔,可用于寻径
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
//                                    日常任务活动时间表
//--------------------------------------------------------------------------------------------------------
class CallboardMissionData : public CallboardData
{
	friend class CallboardMissionRepository;
protected:
	U32                         mMissionID;                   // 任务编号
	U8                          mDegree;                      // 推荐指数
	U8                          mWeekDay;                     // 任务时间星期(0~7)
	U32                         mLimitLevel;                  // 限制等级
	U16                         mLoopNum;                     // 循环次数
	bool                        mbAppend;                     // 额外奖励
	StringTableEntry            mCaptionName;                 // 任务名称
	U32                         mNpcID;                       // 寻径NPC编号
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
//                                　　事件
//----------------------------------------------------------------------------------------------------------
class CallboardEventData : public CallboardData
{
	friend class CallboardEventRepository;
protected:

	U32                         mNpcID;                       // npc编号,可用于寻径
	StringTableEntry            mEventName;                   // 全服事件名称
	StringTableEntry            mMapName;                     // 地图名称
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