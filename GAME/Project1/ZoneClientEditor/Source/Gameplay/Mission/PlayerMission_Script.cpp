//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/ItemBaseData.h"
#include "console/console.h"
#include "Gameplay/Mission/PlayerMission.h"

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#endif

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#endif

// ----------------------------------------------------------------------------
// 获取任务旗标值
ConsoleMethod( Player, HasMissionFlag, bool, 4, 4, "%player.HasMissionFlag(%missionid, %flagname)")
{
	S32 missionid	= dAtoi(argv[2]);
	S32	flagname	= dAtoi(argv[3]);
	S32 idPos, flagPos;
	return g_MissionManager->HasMissionFlag(object, missionid, flagname, idPos, flagPos);
}

// ----------------------------------------------------------------------------
// 设置任务旗标值
ConsoleMethod( Player, SetMissionFlag, bool, 5, 6, "%player.SetMissionFlag(%missionid, %flagname, %flagvalue,%isupdate)")
{
	S32 missionid	= dAtoi(argv[2]);
	S32	flagname	= dAtoi(argv[3]);
	S32 flagvalue	= dAtoi(argv[4]);
	bool isupdate	= false;
	if(argc > 5)
		isupdate = dAtob(argv[5]);
	S32 idPos, flagPos;
	return g_MissionManager->SetMissionFlag(object, missionid, flagname, flagvalue, idPos, flagPos, isupdate);
}

// ----------------------------------------------------------------------------
// 获取某任务旗标的旗标值
ConsoleMethod( Player, GetMissionFlag, S32, 4, 4, "%player.GetMissionFlag(%missionid, %flagname)")
{
	S32 flagvalue, idpos, flagpos;
	bool ret = g_MissionManager->GetMissionFlag(object, dAtoi(argv[2]), dAtoi(argv[3]), flagvalue, idpos, flagpos);
	return ret ? flagvalue : 0;
}

// ----------------------------------------------------------------------------
// 删除任务旗标值
ConsoleMethod( Player, DelMissionFlag, bool, 4, 4, "%player.DelMissionFlag(%missionid, %flagname)")
{
#ifdef NTJ_SERVER
	S32 missionid	= dAtoi(argv[2]);
	S32	flagname	= dAtoi(argv[3]);
	S32 idPos, flagPos;
	return g_MissionManager->DelMissionFlag(object, missionid, flagname, idPos, flagPos);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 当前已接任务的数量
ConsoleMethod( Player, GetAcceptedMission, S32, 2, 2, "%player.GetAcceptedMission()")
{
	return g_MissionManager->GetInfo(object, Mission_AcceptedCount);
}

// ----------------------------------------------------------------------------
// 当前已完成任务的数量
ConsoleMethod( Player, GetFinishedMission, S32, 2, 2, "%player.GetFinishedMission()")
{
	return g_MissionManager->GetInfo(object, Mission_FinishedCount);
}

// ----------------------------------------------------------------------------
// 更新任务到客户端
ConsoleMethod( Player, UpdateMission, bool, 3, 3, "%player.UpdateMission(%missionid)")
{
#ifdef NTJ_SERVER
	return g_MissionManager->UpdateMission(object, MissionManager::UPDATEONE, dAtoi(argv[2]));
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 检查是否已接某任务
ConsoleMethod( Player, IsAcceptedMission, bool, 3, 3, "%player.IsAcceptedMission(%missionid)")
{
	S32 idpos;
	return g_MissionManager->IsAcceptedMission(object, dAtoi(argv[2]), idpos);
}

// ----------------------------------------------------------------------------
// 检查是否已完成某任务
ConsoleMethod( Player, IsFinishedMission, bool, 3, 3, "%player.IsFinishedMission(%missionid)")
{
	return g_MissionManager->IsFinishedMission(object, dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 设置任务完成情况标志
ConsoleMethod( Player, SetMission, bool, 4, 4, "%player.SetMission(%missionid, %IsComplete)")
{
#ifdef NTJ_SERVER
	return g_MissionManager->SetMissionState(object, dAtoi(argv[2]), dAtob(argv[3]));
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 删除任务
ConsoleMethod( Player, DelMission, bool, 3, 3, "%player.DelMission(%missionid)")
{
#ifdef NTJ_SERVER
	S32 idpos;
	return g_MissionManager->DelMission(object, dAtoi(argv[2]), idpos);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 设置循环任务
ConsoleMethod( Player, SetCycleMission, bool, 4, 4, "%player.SetCycleMission(%missionid, %times)")
{
#ifdef NTJ_SERVER
	U32 curtime = Platform::getTime();
	return g_MissionManager->SetCycleMission(object, dAtoi(argv[2]), dAtoi(argv[3]), curtime);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 获取某循环任务的循环次数
ConsoleMethod( Player, GetCycleMissionTimes, S32, 3, 3, "%player.GetCycleMissionTimes(%missionid)")
{
#ifdef NTJ_SERVER
	S32 times;
	U32 endtime;
	g_MissionManager->GetCycleMission(object, dAtoi(argv[2]), times, endtime);
	return times;
#endif
	return 0;
}

// ----------------------------------------------------------------------------
// 获取某循环任务的完成时间
ConsoleMethod( Player, GetCycleMissionEndTime, const char*, 3, 3, "%player.GetCycleMissionEndTime(%missionid)")
{
#ifdef NTJ_SERVER
	S32 times;
	U32 endtime;
	g_MissionManager->GetCycleMission(object, dAtoi(argv[2]), times, endtime);
	time_t ltime = endtime;
	struct tm *systime = localtime( &ltime );
	char* ret = Con::getArgBuffer(20);
	dSprintf(ret, 20, "%4d%02d%02d%02d%02d%02d",
			systime->tm_year + 1900,
			systime->tm_mon + 1,
			systime->tm_mday,
			systime->tm_hour,
			systime->tm_min,
			systime->tm_sec
				);
	return ret;
#endif
	return 0;
}

// ----------------------------------------------------------------------------
// 删除循环任务
ConsoleMethod( Player, DelCycleMission, bool, 3, 3, "%player.DelCycleMission(%missionid)")
{
#ifdef NTJ_SERVER
	return g_MissionManager->DelCycleMission(object, dAtoi(argv[2]));
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 判定是否需求某任务物品
ConsoleMethod( Player, IsNeedTaskItem, bool, 3, 4, "%player.IsNeedTaskItem(%itemid,%missionid)")
{
	S32 iMissionID = -1;
	if(argc > 3)
		iMissionID = dAtoi(argv[3]);
	return g_MissionManager->IsNeedTaskItem(object, dAtoi(argv[2]), iMissionID);
}

// ----------------------------------------------------------------------------
// 获取需求某任务物品的数量
ConsoleMethod( Player, GetNeedTaskItem, S32, 3, 4, "%player.GetNeedTaskItem(%itemid,%missionid)")
{
	S32 iMissionID = -1;
	if(argc > 3)
		iMissionID = dAtoi(argv[3]);
	return g_MissionManager->GetNeedTaskItem(object, dAtoi(argv[2]), iMissionID);
}

// ----------------------------------------------------------------------------
// 判定是否需求某任务物品
ConsoleMethod( Player, IsNeedKillMonster, bool, 3, 4, "%player.IsNeedKillMonster(%monsterid,%missionid)")
{
	S32 iMissionID = -1;
	if(argc > 3)
		iMissionID = dAtoi(argv[3]);
	return g_MissionManager->IsNeedKillMonster(object, dAtoi(argv[2]), iMissionID);
}

// ----------------------------------------------------------------------------
// 获取需求某任务物品的数量
ConsoleMethod( Player, GetNeedKillMonster, S32, 3, 4, "%player.GetNeedKillMonster(%monsterid,%missionid)")
{
	S32 iMissionID = -1;
	if(argc > 3)
		iMissionID = dAtoi(argv[3]);
	return g_MissionManager->GetNeedKillMonster(object, dAtoi(argv[2]), iMissionID);
}

// ----------------------------------------------------------------------------
// 获取需求某任务物品的数量
ConsoleMethod( Player, GetKillMonster, S32, 3, 4, "%player.GetKillMonster(%monsterid,%missionid)")
{
	S32 iMissionID = -1;
	if(argc > 3)
		iMissionID = dAtoi(argv[3]);
	return g_MissionManager->GetKillMonster(object, dAtoi(argv[2]), iMissionID);
}

// ----------------------------------------------------------------------------
// 启动限时任务计时
ConsoleMethod( Player, StartTimeMission, bool, 3, 3, "%player.StartTimeMission(%missionid)")
{
#ifdef NTJ_SERVER
	return g_MissionManager->StartTime(object, dAtoi(argv[2]));
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 结束限时任务计时
ConsoleMethod( Player, EndTimeMission, bool, 3, 3, "%player.EndTimeMission(%missionid)")
{
#ifdef NTJ_SERVER
	return g_MissionManager->EndTime(object, dAtoi(argv[2]));
#endif
	return true;
}

// ----------------------------------------------------------------------------
// 判断是否限时任务
ConsoleMethod( Player, IsTimeInfo, bool, 3, 3, "%player.IsTimeInfo(%missionid)")
{
	return g_MissionManager->IsTimeInfo(object, dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 检查某限时任务是否超时
ConsoleMethod( Player, CheckTimeMission, bool, 3, 3, "%player.CheckTimeMission(%missionid)")
{
	return g_MissionManager->CheckTime(object, dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 结束限时任务计时
ConsoleMethod( Player, ContinueTimeMission, void, 2, 2, "%player.ContinueTimeMission()")
{
#ifdef NTJ_SERVER
	g_MissionManager->ContinueTime(object);
#endif
}

// ----------------------------------------------------------------------------
// 获取玩家接到小队的共享任务总数
ConsoleMethod( Player, GetShareMissionCount, S32, 2, 2, "%player.GetShareMissionCount()")
{
	return object->missionShareList.size();
}

// ----------------------------------------------------------------------------
// 指定索引编号获取共享任务的任务编号
ConsoleMethod( Player, GetShareMission, S32, 3, 3, "%player.GetShareMission(%index)")
{
	S32 index = dAtoi(argv[2]);
	return (index >= 0 && index < object->missionShareList.size()) ? object->missionShareList[index] : -1;
}

// ----------------------------------------------------------------------------
// 添加任务编号到玩家共享任务列表
ConsoleMethod( Player, AddShareMission, bool, 3, 3, "%player.AddShareMission(%missionid)")
{
	S32 iMissionID = dAtoi(argv[2]);
	if(iMissionID < 0 || iMissionID >= MISSION_MAX)
		return false;
	if(g_MissionManager->IsFinishedMission(object, iMissionID))
		return false;
	S32 idPos;
	if(g_MissionManager->IsAcceptedMission(object,iMissionID, idPos))
		return false;
	object->missionShareList.push_back(iMissionID);
	return true;
}

// ----------------------------------------------------------------------------
// 删除玩家共享任务列表内某个任务编号
ConsoleMethod( Player, DelShareMission, bool, 3, 3, "%player.DelShareMission(%missionid)")
{
	S32 iMissionID = dAtoi(argv[2]);
	if(iMissionID < 0 || iMissionID >= MISSION_MAX)
		return false;
	for(S32 i = 0; i < object->missionShareList.size(); i++)
	{
		if(object->missionShareList[i] == iMissionID)
		{
			object->missionShareList.erase_fast(i);
			return true;
		}
	}
	return false;
}

#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// 获取对应索引编号的任务编号
ConsoleMethod( Player, GetMission, S32, 3, 3, "%player.GetMission(%index)")
{
	return g_MissionManager->GetMission(object, dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 发送客户端任务请求
ConsoleMethod( Player, SendMissionRequest, bool, 4, 4, "%player.SendMissionRequest(%type, %missionid)")
{
	return g_MissionManager->SendMissionRequest(object, dAtoi(argv[2]), dAtoi(argv[3]));
}

#endif//NTJ_CLIENT