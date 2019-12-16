#ifndef __PLAYERMISSION_H__
#define __PLAYERMISSION_H__

// ============================================================================
// 任务系统简要说明
// 1.玩家任务存储数据在PlayerStruct中
// 2.玩家任务操作数据在PlayerObject中
// 3.玩家任务的网络数据传输主要有以下二个:
//		a.MissionEvent(GameNetEvent事件类型)
//		b.标志为Info_ShareMission的GamePlayerEvent事件
// 4.NPC对话任务交互(可见Conversation.h)
// 5.
//
// ============================================================================

#include "Common\PlayerStruct.h"

class Player;
// ----------------------------------------------------------------------------
//	玩家任务数据
//
enum enMission
{
	Mission_FinishedCount		= 0,			// 已完成任务数量
	Mission_AcceptedCount		= 1,			// 已接任务数量
	Mission_TimeInfoCount		= 2,			// 已接限时任务数量

	Mission_Step				= 1,			// 若任务旗标名1,则表示旗标值为Step的值
	Mission_Share				= 2,			// 若任务旗标名2,则表示旗标值为任务共享方式
	Mission_Freeze				= 3,			// 若任务旗标名3,旗标值为宠物ID，表示宠物代做任务

	Mission_LimitTime_Need		= 10,			// 限时任务限时秒数
	Mission_LimitTime_Lapse		= 11,			// 限时任务流逝时间秒数
	Mission_LimitTime_Begin		= 12,			// 限时任务开始的时间

	Mission_NpcID_Begin			= 1000,			// 若任务旗标名为1000-1999,则表示旗标值为NPC编号
	Mission_NpcID_End			= 1999,			// 共计999项

	Mission_ItemID_Begin		= 2100,			// 若任务旗标名为2100-2199,则表示旗标值为物品编号
	Mission_ItemID_End			= 2199,			// 共计99项
	Mission_ItemNeedNum_Begin	= 2200,			// 若任务旗标名为2200-2299,则表示旗标值为需求物品数量
	Mission_ItemNeedum_End		= 2299,			// 共计99项
	Mission_ItemCurNum_Begin	= 2300,			// 若任务旗标名为2200-2299,则表示旗标值为当前物品数量
	Mission_ItemCurNum_End		= 2399,			// 共计99项

	Mission_MonsterID_Begin		= 3100,			// 若任务旗标名为3100-3199,则表示旗标值为怪物编号
	Mission_MonsterID_End		= 3199,			// 共计99项
	Mission_MonsterNeedNum_Begin= 3200,			// 若任务旗标名为3200-3299,则表示旗标值为需求怪物数量
	Mission_MonsterNeedNum_End	= 3299,			// 共计99项
	Mission_MonsterCurNum_Begin	= 3300,			// 若任务旗标名为3300-3399,则表示旗标值为当前怪物数量
	Mission_MonsterCurNum_End	= 3399,			// 共计99项	

	Mission_DialogID_Begin		= 4000,			// 若任务旗标名为4000-4999,则表示旗标值为对话编号
	Mission_DialogID_End		= 4999,			// 共计999项

	Mission_SkillID_Begin		= 5000,			// 若任务旗标名为5000-5999,则表示旗标值为技能编号
	Mission_SkillID_End			= 5999,			// 共计999项

	Mission_StatusID_Begin		= 6000,			// 若任务旗标名为6000-6999,则表示旗标值为状态编号
	Mission_StatusD_End			= 6999,			// 共计999项

	Mission_TimeFM_Begin		= 7000,			// 若任务旗标名为7000-7999,则表示旗标值为时间格式
	Mission_TimeFM_End			= 7999,			// 共计999项

	Mission_Number_Begin		= 8000,			// 若任务旗标名为8000-8999,则表示旗标值为相应数字
	Mission_Number_End			= 8999			// 共计999项
};

// 任务数据操作类
class MissionManager
{
public:
	enum Constants
	{
		DONOTHING,								// 什么都不做
		ADDONE,									// 添加一个任务
		//ADDONEANDONE,							// 添加一个任务的某个任务旗标
		UPDATEONE,								// 更新一个任务
		UPDATEONEANDONE,						// 更新一个任务的某个任务旗标
		DELETEONE,								// 删除一个任务
		DELETEONEANDONE,						// 删除一个任务的某个任务旗标
		UPDATEONESTATE,							// 更新一个任务完成状态
		UPDATEALLSTATE,							// 更新所有任务完成状态
		UPDATEONECYCLE,							// 更新一个循环任务
		UPDATEALLCYCLE,							// 更新所有循环任务
		DELONECYCLE,							// 删除一个循环任务
		//UPDATEONESHARETO,						// 共享一个任务给队友
		//UPDATEALLSHARETO,						// 共享所有任务给队友
		UPDATEONESHAREFROM,						// 更新一个共享任务
		UPDATEALLSHAREFROM,						// 更新所有共享任务
		STARTTIME,								// 启动限时任务
		ENDTIME,								// 结束限时任务
		CONTINUETIME,							// 继续限时任务
		UPDATENEWBIEMISSION,					// 新手任务
		UPDATENEWBIEHELP,						// 新手帮助

		MISSIONREQUEST_ACCEPT	= 0,			// 客户端请求接取任务
		MISSIONREQUEST_DELETE	= 1,			// 客户端请求放弃任务
		MISSIONREQUEST_SHARE	= 2 ,			// 客户端请求共享任务给队友
		MISSIONREQUEST_SHAREGET = 3,			// 客户端请求获取队伍共享任务
	};
	MissionManager();
	S32  GetInfo(Player* player, U32 type);
	bool IsAcceptedMission(Player* player, S32 MissionID, S32& idPos);
	bool IsFinishedMission(Player* player, S32 MissionID);
	bool HasMission(Player* player, S32 MissionID, S32& idPos);
	bool HasMissionFlag(Player* player, S32 MissionID, S32 FlagName, S32& idPos, S32& flagPos);
	bool GetMissionFlag(Player* player, S32 MissionID, S32 FlagName, S32& FlagValue, S32& idPos, S32& flagPos);
	bool SetMissionState(Player* player, S32 MissionID, bool IsComplete);
	bool DelMissionFlag(Player* player, S32 MissionID, S32 FlagName, S32& idPos, S32& flagPos);
	bool DelMission(Player* player, S32 MissionID, S32& idPos);

	bool SetCycleMission(Player* player, S32 MissionID, S32 times, U32 endtime);
	void GetCycleMission(Player* player, S32 MissionID, S32& times, U32& endtime);
	bool DelCycleMission(Player* player, S32 MissionID);	

	bool SetMissionFlag(Player* player, S32 MissionID,	S32 FlagName, S32 FlagValue, S32& idPos, S32& flagPos, bool IsUpdate = false);
#ifdef NTJ_SERVER
	bool SendInitialize(Player* player);
	bool UpdateMission(Player* player, U32 opt, S32 arg1 = -1, S32 arg2 = -1, bool IsNotify = true);
	bool ShareMission(Player* player, S32 MissionID);
	bool ShareMissionResponse(Player* player);
	bool ClearCycleMissionTime(Player* player);
#endif
	// ========================================================================
	// 功能性函数
	// ========================================================================
	bool IsNeedTaskItem(Player* player, U32 itemid, S32 MissionID = -1);
	U32  GetNeedTaskItem(Player* player, U32 itemid, S32 MissionID = -1);
	S32  GetTaskItem(Player* player, U32 itemid, S32 MissionID);
	bool UpdateTaskItem(Player* player, U32 itemid);
	bool IsNeedKillMonster(Player* player, U32 monsterid, S32 MissionID = -1);
	S32  GetNeedKillMonster(Player* player, U32 monsterid, S32 MissionID = -1);
	S32  GetKillMonster(Player* player, U32 monsterid, S32 MissionID = -1);
	bool SetKillMonster(Player* player, U32 monsterid);

	bool IsTimeInfo(Player* player, S32 MissionID);
	void PauseTime(Player* player);
	void ContinueTime(Player* player);
	bool CheckTime(Player* player, S32 MissionID);
	bool StartTime(Player* player, S32 MissionID);
	bool EndTime(Player* player, S32 MissionID);

#ifdef NTJ_CLIENT
	S32  GetMission(Player* player, U32 index);
	bool SendMissionRequest(Player* player, U32 type, S32 missionid);
#endif//NTJ_CLIENT

private:
	bool SetState(Player* player, U32 bit);
	bool ClearState(Player* player, U32 bit);
	bool TestState(Player* player, U32 bit);
};
extern MissionManager* g_MissionManager;
#endif