//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gui/core/guiTypes.h"
#include "util/stringUnit.h"
#include "Util/ColumnData.h"
#include "Gameplay/Data/CallboardData.h"

//------------------------------------------------------------------------------------------------------
//                                   活动时间表
//------------------------------------------------------------------------------------------------------
CallboardDailyData::CallboardDailyData() : mDegree(0),
										   mWeekDay(0),
										   mLimitLevel(0),
										   mNotifyCount(0)
										  
{
    mNotifyTime  = StringTable->insert("");
	mStartTime   = StringTable->insert("");
	mEndTime     = StringTable->insert("");
	mNpcID       = StringTable->insert("");
	mMapName     = StringTable->insert("");
	mLineName    = StringTable->insert("");
	mCaptionName = StringTable->insert("");
	dMemset(&mStartDataTime,0,sizeof(mStartDataTime));
	dMemset(&mEndDataTime,0,sizeof(mEndDataTime));
}
//----------------------------------------------------------------------------------------------------------
CallboardDailyRepository g_CallboardDailyRepository;
CallboardDailyRepository::CallboardDailyRepository()
{
	mColumnData = NULL;
}

CallboardDailyRepository::~CallboardDailyRepository()
{
	clear();
}

void CallboardDailyRepository::clear()
{
	if(mColumnData)
	{
		delete mColumnData;
		mColumnData = NULL;
	}

	Vector<CallboardDailyData*>::iterator it = mDailyVect.begin();

	for( ; it != mDailyVect.end(); ++it)
	{
		if(*it)
			delete (*it);
	}

	mDailyVect.clear();
}

bool CallboardDailyRepository::read()
{
	CDataFile file;
	RData     lineData;
	char      fileName[1024];
	file.ReadDataInit();
	Platform::makeFullPathName(GAME_CALLBOARDDAILYDATA_FILE, fileName, sizeof(fileName));

	if(!file.readDataFile(fileName))
	{
		file.ReadDataClose();
		AssertRelease(false,"can't read file : CallboardDailyData.dat");
		return false;
	}
	mColumnData = new IColumnData(file.ColumNum,"CallboardDailyData.dat");
	mColumnData->setField(CalcOffset(mIndexID,        CallboardDailyData),   DType_U16,    "序号");
	mColumnData->setField(CalcOffset(mCaptionName,    CallboardDailyData),   DType_string, "活动名称");
	mColumnData->setField(CalcOffset(mDegree,         CallboardDailyData),   DType_U8,     "推荐指数");
    mColumnData->setField(CalcOffset(mLimitLevel,     CallboardDailyData),   DType_U32,    "限制等级");
	mColumnData->setField(CalcOffset(mWeekDay,        CallboardDailyData),   DType_U8,     "星期");
	mColumnData->setField(CalcOffset(mNotifyTime,     CallboardDailyData),   DType_string, "预告时间");
	mColumnData->setField(CalcOffset(mStartTime,      CallboardDailyData),   DType_string, "开始时间");
	mColumnData->setField(CalcOffset(mEndTime,        CallboardDailyData),   DType_string, "结束时间");
	mColumnData->setField(CalcOffset(mLineName,       CallboardDailyData),   DType_string, "开放线路");
	mColumnData->setField(CalcOffset(mMapName,        CallboardDailyData),   DType_string, "地图名称");
	mColumnData->setField(CalcOffset(mNpcID,          CallboardDailyData),   DType_string, "npc编号");

	for (int i=0; i < file.RecordNum; i++)
	{
		CallboardDailyData* pData = new CallboardDailyData;
		for (int j=0; j < file.ColumNum; j++)
		{
			file.GetData(lineData);
			mColumnData->setData(pData,j,lineData);
		}

		if(!pData)
			continue;

		pData->mNotifyCount = (U8)StringUnit::getUnitCount(pData->mNotifyTime,"/\n");
		// 初始化DataTime数据结构
		U8 hour,min;
		if(dStrcmp(pData->mStartTime,"") != 0)
		{
			hour = min = 0;
			pData->mStartDataTime.yearday = StringUnit::getUnit(pData->mStartTime,0," \n");
			if(pData->mStartDataTime.yearday == "0000-00-00")
				pData->mStartDataTime.yearday = "";

			pData->mStartDataTime.longTime = StringUnit::getUnit(pData->mStartTime,1," \n");
			if(pData->mStartDataTime.longTime == "00:00")
				pData->mStartDataTime.longTime = "";

			hour = atoi(StringUnit::getUnit(pData->mStartTime,1," :\n"));
			min  = atoi(StringUnit::getUnit(pData->mStartTime,2," :\n"));
			pData->mStartDataTime.min = hour *60 + min;
		}

		if(dStrcmp(pData->mEndTime,"") != 0)
		{
			hour = min = 0;
			pData->mEndDataTime.yearday = StringUnit::getUnit(pData->mEndTime,0," \n");
			if(pData->mEndDataTime.yearday == "0000-00-00")
				pData->mEndDataTime.yearday = "";

			pData->mEndDataTime.longTime = StringUnit::getUnit(pData->mEndTime,1," \n");
			if(pData->mEndDataTime.longTime == "00:00")
				pData->mEndDataTime.longTime = "";

			hour = atoi(StringUnit::getUnit(pData->mEndTime,1," :\n"));
			min  = atoi(StringUnit::getUnit(pData->mEndTime,2," :\n"));
			pData->mEndDataTime.min = hour *60 + min;
		}

		mDailyVect.push_back(pData);
	}

	file.ReadDataClose();
	if(mDailyVect.size() > 0)
		dQsort((void*) &(mDailyVect[0]),mDailyVect.size(),sizeof(CallboardDailyData*),idCompare);
	return true;
}

ConsoleFunction(getCallboardDailyData,const char*,3,3,"getCallboardDailyData(%indexID,%col)")
{
	U32 iIndexID = atoi(argv[1]);
	Vector<CallboardDailyData*>& pVectData = g_CallboardDailyRepository.getDailyData();
	
	if(pVectData.size() < iIndexID)
	{
		AssertFatal(false,"getEverydayData::参数1越界");
		return "";
	}

	CallboardDailyData* pData = pVectData[iIndexID-1];

	if(g_CallboardDailyRepository.mColumnData && pData )
	{
		std::string to;
		g_CallboardDailyRepository.mColumnData->getData(pData,atoi(argv[2]),to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;
	}

	return "";
}

//--------------------------------------------------------------------------------------------------------
//                                    日常任务活动时间表
//--------------------------------------------------------------------------------------------------------
CallboardMissionData::CallboardMissionData() : mMissionID(0),
											   mDegree(0),
											   mWeekDay(0),
											   mLimitLevel(0),
											   mLoopNum(0),
											   mbAppend(false),
											   mNpcID(0)
{
	mCaptionName = StringTable->insert("");
}
//---------------------------------------------------------------------------------------------------------
CallboardMissionRepository g_CallboardMissionRepository;
CallboardMissionRepository::CallboardMissionRepository()
{
	mColumnData = NULL;
}

CallboardMissionRepository::~CallboardMissionRepository()
{
	clear();
}

void CallboardMissionRepository::clear()
{
	if(mColumnData)
	{
		delete mColumnData;
		mColumnData = NULL;
	}

	Vector<CallboardMissionData*>::iterator it = mMissionVector.begin();
	for (; it != mMissionVector.end(); ++it)
	{
		if(*it)
			delete (*it);
	}

	mMissionVector.clear();
}

bool CallboardMissionRepository::read()
{
	CDataFile file;
	RData     lineData;
	char      fileName[1024];
	file.ReadDataInit();
	Platform::makeFullPathName(GAME_CALLBOARDMISSIONDATA_FILE, fileName, sizeof(fileName));

	if(!file.readDataFile(fileName))
	{
		file.ReadDataClose();
		AssertRelease(false,"can't read file : CallboardMissionData.dat");
		return false;
	}
	
	mColumnData = new IColumnData(file.ColumNum,"CallboardMissionData.dat");
	mColumnData->setField(CalcOffset(mIndexID,          CallboardMissionData),     DType_U16,     "序号");
	mColumnData->setField(CalcOffset(mMissionID,        CallboardMissionData),     DType_U32,     "任务编号");
	mColumnData->setField(CalcOffset(mDegree,           CallboardMissionData),     DType_U8,      "推荐指数");
	mColumnData->setField(CalcOffset(mCaptionName,      CallboardMissionData),     DType_string,  "任务名称");
	mColumnData->setField(CalcOffset(mWeekDay,          CallboardMissionData),     DType_U8,      "任务时间");
	mColumnData->setField(CalcOffset(mLimitLevel,       CallboardMissionData),     DType_U32,     "限制等级");
	mColumnData->setField(CalcOffset(mLoopNum,          CallboardMissionData),     DType_U16,     "循环次数");
	mColumnData->setField(CalcOffset(mbAppend,          CallboardMissionData),     DType_U8,      "额外奖励");
	mColumnData->setField(CalcOffset(mNpcID,            CallboardMissionData),     DType_U32,     "NPC编号");

	for (int i=0; i < file.RecordNum; i++)
	{
		CallboardMissionData* pData = new CallboardMissionData;
		for (int j=0; j < file.ColumNum; j++)
		{
			file.GetData(lineData);
			mColumnData->setData(pData,j,lineData);
		}

		mMissionVector.push_back(pData);
	}

	file.ReadDataClose();
	if(mMissionVector.size() > 0)
		dQsort((void*) &(mMissionVector[0]),mMissionVector.size(),sizeof(CallboardMissionData*),idCompare);
	return true;
}
ConsoleFunction(getCallboardMissionData,const char*,3,3,"getCallboardMissionData(%indexID,%col)")
{
	U32 iIndexID = atoi(argv[1]);
	Vector<CallboardMissionData*>& pVectData = g_CallboardMissionRepository.getMissionData();

	if(pVectData.size() < iIndexID)
	{
		AssertFatal(false,"getCallboardMissionData::参数1越界");
		return "";
	}

	CallboardMissionData* pData = pVectData[iIndexID-1];
	if(g_CallboardMissionRepository.mColumnData && pData)
	{
		std::string to;
		g_CallboardMissionRepository.mColumnData->getData(pData,atoi(argv[2]),to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;
	}

	return "";
}
//----------------------------------------------------------------------------------------------------------
//                                　　事件
//----------------------------------------------------------------------------------------------------------
CallboardEventData::CallboardEventData() : mNpcID(0)
{
	mEventName = StringTable->insert("");
	mMapName = StringTable->insert("");
}
//----------------------------------------------------------------------------------------------------------
CallboardEventRepository g_CallboardEventRepository;
CallboardEventRepository::CallboardEventRepository()
{
	mColumnData = NULL;
}

CallboardEventRepository::~CallboardEventRepository()
{
	clear();
}

void CallboardEventRepository::clear()
{
	if(mColumnData)
	{
		delete mColumnData;
		mColumnData = NULL;
	}

	Vector<CallboardEventData*>::iterator it = mEventVector.begin();
	for (; it != mEventVector.end(); ++it)
	{
		if(*it)
			delete (*it);
	}

	mEventVector.clear();
}

bool CallboardEventRepository::read()
{
	CDataFile file;
	RData     lineData;
	char      fileName[1024];
	file.ReadDataInit();
	Platform::makeFullPathName(GAME_CALLBOARDEVENTDATA_FILE, fileName, sizeof(fileName));

	if(!file.readDataFile(fileName))
	{
		file.ReadDataClose();
		AssertRelease(false,"can't read file : CallboardEventData.dat");
		return false;
	}

	mColumnData = new IColumnData(file.ColumNum,"CallboardEventData.dat");
	mColumnData->setField(CalcOffset(mIndexID,     CallboardEventData),     DType_U16,     "序号" );
	mColumnData->setField(CalcOffset(mNpcID,       CallboardEventData),     DType_U32,     "npc编号");
	mColumnData->setField(CalcOffset(mEventName,   CallboardEventData),     DType_string,  "全服事件名称");
	mColumnData->setField(CalcOffset(mMapName,     CallboardEventData),     DType_string,  "地图名称");

	for (int i=0; i < file.RecordNum; i++)
	{
		CallboardEventData* pData = new CallboardEventData;
		for (int j=0; j < file.ColumNum; j++)
		{
			file.GetData(lineData);
			mColumnData->setData(pData,j,lineData);
		}

		mEventVector.push_back(pData);
	}

	file.ReadDataClose();
	if(mEventVector.size() > 0)
		dQsort((void*) &(mEventVector[0]),mEventVector.size(),sizeof(CallboardEventData*),idCompare);
	return true;
}

ConsoleFunction(getGlobalEventData,const char*,3,3,"getGlobalEventData(%indexID,%col)")
{
	U32 iIndexID = atoi(argv[1]);
	Vector<CallboardEventData*>& pVectData = g_CallboardEventRepository.getEventData();

	if(pVectData.size() < iIndexID)
	{
		AssertFatal(false,"getGlobalEventData::参数1越界");
		return "";
	}

	CallboardEventData* pData = pVectData[iIndexID-1];
	if(g_CallboardEventRepository.mColumnData && pData)
	{
		std::string to;
		g_CallboardEventRepository.mColumnData->getData(pData,atoi(argv[2]),to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;
	}

	return "";
}