#pragma once
#include "core/bitStream.h"
#include "platform/platform.h"
#include "Gameplay/Mission/PlayerMission.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/item/res.h"
MissionManager gMissionManager;
MissionManager* g_MissionManager = NULL;

#ifdef NTJ_CLIENT
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#endif//NTJ_CLIENT

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/Team//ZoneTeamManager.h"
#endif//NTJ_SERVER

// ----------------------------------------------------------------------------
// 构造函数
MissionManager::MissionManager()
{
	g_MissionManager = this;
}

// ----------------------------------------------------------------------------
// 设置任务完成状态位
bool MissionManager::SetState(Player* player, U32 bit)
{
	if(!player || bit < 0 || bit >= MISSION_MAX)
		return false;

	player->missionInfo.State[bit >> 3] |= U8(1 << (bit & 0x7));
	return true;
}

// ----------------------------------------------------------------------------
// 测试会务完成状态位
bool MissionManager::TestState(Player* player, U32 bit)
{
	if(!player || bit < 0 || bit >= MISSION_MAX)
		return false;

	return (player->missionInfo.State[bit >> 3] & U8(1 << (bit & 0x7))) != 0;
}

// ----------------------------------------------------------------------------
// 清除任务完成状态位
bool MissionManager::ClearState(Player* player, U32 bit)
{
	if(!player || bit < 0 || bit >= MISSION_MAX)
		return false;

	player->missionInfo.State[bit >> 3] &= U8(~(1 << (bit & 0x7)));
	return true;
}

// ----------------------------------------------------------------------------
// 获取任务相关信息
S32 MissionManager::GetInfo(Player* player, U32 type)
{
	if(!player)
		return 0;
	S32 count = 0;
	switch(type)
	{
	case Mission_FinishedCount:			// 已完成任务数量
		{
			U32* pState = (U32*)player->missionInfo.State;
			for(S32 i = 0; i < (MISSION_STATE_LENGTH / 4); ++i)
			{
				U32 n = *pState;
				while(n>0)
				{ 
					n&=(n-1);
					count++; 
				}
				pState++;
			}
			return count;
		}
	case Mission_AcceptedCount:			// 已接未完成任务的数量
		{
			for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
			{
				if(player->missionInfo.Flag[i].MissionID != -1)
					count ++;
			}
			return count;
		}
	case Mission_TimeInfoCount:			// 已接限时任务的数量
		{
			for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
			{
				if(player->missionInfo.Flag[i].MissionID != -1)
				{
					for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
					{
						if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Need)
						{
							count ++;
							break;
						}
					}
				}
			}
		}
	}
	return count;
}

// ----------------------------------------------------------------------------
// 检查是否已接某任务
bool MissionManager::IsAcceptedMission(Player* player, S32 MissionID, S32& idPos)
{	
	idPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			idPos = i;
			return true;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
// 检查是否已完成某任务
bool MissionManager::IsFinishedMission(Player* player, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;
	return TestState(player, MissionID);
}

// ----------------------------------------------------------------------------
// 检查是否存在某任务
bool MissionManager::HasMission(Player* player, S32 MissionID, S32& idPos)
{
	idPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			idPos = i;
			return true;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
// 检查是否存在某任务旗标
bool MissionManager::HasMissionFlag(Player* player, S32 MissionID, S32 FlagName, S32& idPos, S32& flagPos)
{
	idPos = flagPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] == FlagName)
				{
					idPos = i;
					flagPos = j;
					break;
				}
			}
			break;
		}
	}
	return idPos != -1 && flagPos != -1;
}

// ----------------------------------------------------------------------------
// 设置循环任务次数或完成时间
bool MissionManager::SetCycleMission(Player* player, S32 MissionID, S32 times, U32 endtime)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX || times < 0 || times > 65535)
		return false;
	S32 findPos = -1;
	for(S32 i = MISSION_CYCLE_MAX - 1; i >= 0; --i)
	{
		if(player->missionInfo.Cycle[i].MissionID == MissionID)
		{
			findPos = i;
			break;
		}
		else if(player->missionInfo.Cycle[i].MissionID == -1)
		{
			findPos = i;
		}
	}

	if(findPos != -1)
	{
		player->missionInfo.Cycle[findPos].MissionID = MissionID;
		player->missionInfo.Cycle[findPos].EndTime = endtime;
		player->missionInfo.Cycle[findPos].EndTimes = times;
#ifdef NTJ_SERVER
		return UpdateMission(player, MissionManager::UPDATEONECYCLE, findPos);
#else
		return true;
#endif//NTJ_SERVER
	}
	return false;
}

// ----------------------------------------------------------------------------
// 获取循环任务次数或完成时间
void MissionManager::GetCycleMission(Player* player, S32 MissionID, S32& times, U32& endtime)
{
	times	= 0;
	endtime = 0;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return;
	for(S32 i = MISSION_CYCLE_MAX - 1; i >= 0; --i)
	{
		if(player->missionInfo.Cycle[i].MissionID == MissionID)
		{
			times = player->missionInfo.Cycle[i].EndTimes;
			endtime = player->missionInfo.Cycle[i].EndTime;
			return;
		}
	}
}

// ----------------------------------------------------------------------------
// 删除循环任务次数或完成时间
bool MissionManager::DelCycleMission(Player* player, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;
	for(S32 i = MISSION_CYCLE_MAX - 1; i >= 0; --i)
	{
		if(player->missionInfo.Cycle[i].MissionID == MissionID)
		{
			player->missionInfo.Cycle[i].MissionID = -1;
			player->missionInfo.Cycle[i].EndTime = 0;
			player->missionInfo.Cycle[i].EndTimes = 0;
#ifdef NTJ_SERVER
			return UpdateMission(player, MissionManager::DELONECYCLE, MissionID);
#else
			return true;
#endif//NTJ_SERVER
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// 获取任务旗标变量值
bool MissionManager::GetMissionFlag(Player* player, S32 MissionID, S32 FlagName, S32& FlagValue, S32& idPos, S32& flagPos)
{
	idPos = flagPos = -1;
	FlagValue = 0;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;	
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] == FlagName)
				{
					idPos = i;
					flagPos = j;
					FlagValue = player->missionInfo.Flag[i].Value[j];
					break;
				}
			}
			break;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// 设置任务完成状态
bool MissionManager::SetMissionState(Player* player, S32 MissionID, bool IsComplete)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	bool ret = false;
	if(IsComplete)
		ret = SetState(player,MissionID);
	else
		ret = ClearState(player, MissionID);

	S32 idPos;
	if(HasMission(player, MissionID, idPos))
	{
		dMemset(&player->missionInfo.Flag[idPos], 0, sizeof(stMissionFlag));
		player->missionInfo.Flag[idPos].MissionID = -1;
	}
#ifdef NTJ_SERVER
	if(ret)
    {
        //TODO log
        /*lg_quest_finishInfo log;
        log.playerName = player->getPlayerName();
        //log.*/
        
		return UpdateMission(player, MissionManager::UPDATEONESTATE, MissionID, IsComplete);
    }
#endif
	return ret;
}


// ----------------------------------------------------------------------------
// 删除某任务旗标
bool MissionManager::DelMissionFlag(Player* player, S32 MissionID, S32 FlagName, S32& idPos, S32& flagPos)
{
	idPos = flagPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	bool ret = false;
	// 是否存在任务旗标
	if(HasMissionFlag(player, MissionID, FlagName, idPos, flagPos))
	{
		player->missionInfo.Flag[idPos].Name[flagPos] = 0;
		player->missionInfo.Flag[idPos].Value[flagPos] = 0;
#ifdef NTJ_SERVER
		ret = UpdateMission(player, MissionManager::DELETEONEANDONE, MissionID, flagPos);
#else
		ret = true;
#endif
	}
	return ret;
}

// ----------------------------------------------------------------------------
// 删除某任务
bool MissionManager::DelMission(Player* player, S32 MissionID, S32& idPos)
{
	idPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	bool ret = false;
	// 是否已接任务
	if(IsAcceptedMission(player, MissionID, idPos))
	{
		dMemset(&player->missionInfo.Flag[idPos], 0, sizeof(stMissionFlag));
		player->missionInfo.Flag[idPos].MissionID = -1;
#ifdef NTJ_SERVER
		ret = UpdateMission(player, MissionManager::DELETEONE, MissionID);
#else
		ret = true;
#endif
	}
	return ret;
}

// ----------------------------------------------------------------------------
// 判断玩家任务里需求某任务道具
bool MissionManager::IsNeedTaskItem(Player* player, U32 itemid, S32 MissionID /* = -1 */)
{
	if(!player)
		return false;

	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			if(MissionID != -1 && player->missionInfo.Flag[i].MissionID != MissionID)
				continue;

			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] >= Mission_ItemID_Begin &&
					player->missionInfo.Flag[i].Name[j] <= Mission_ItemID_End &&
					player->missionInfo.Flag[i].Value[j] == itemid)
				{
					return true;
				}
			}
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
// 获取玩家任务里需求某任务道具的数量
U32 MissionManager::GetNeedTaskItem(Player* player, U32 itemid, S32 MissionID /* = -1 */)
{
	if(!player)
		return 0;

	U32 neednum = 0;
	U32 flag_neednum;
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			if(MissionID != -1 && player->missionInfo.Flag[i].MissionID != MissionID)
				continue;

			flag_neednum = 0;
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] >= Mission_ItemID_Begin &&
					player->missionInfo.Flag[i].Name[j] <= Mission_ItemID_End &&
					player->missionInfo.Flag[i].Value[j] == itemid)
				{
					flag_neednum = player->missionInfo.Flag[i].Name[j] + (Mission_ItemNeedNum_Begin - Mission_ItemID_Begin);
					for(S32 k = 0; k < MISSION_FLAG_MAX; ++k)
					{
						if(player->missionInfo.Flag[i].Name[k] == flag_neednum)
						{
							neednum += player->missionInfo.Flag[i].Value[k];
							break;
						}
					}
					break;
				}
			}
		}
	}
	return neednum;
}

// ----------------------------------------------------------------------------
// 获取任务物品数量
S32 MissionManager::GetTaskItem(Player* player, U32 itemid, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return 0;

	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			U32 flag_currentnum = 0;
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] >= Mission_ItemID_Begin &&
					player->missionInfo.Flag[i].Name[j] <= Mission_ItemID_End &&
					player->missionInfo.Flag[i].Value[j] == itemid)
				{
					flag_currentnum = player->missionInfo.Flag[i].Name[j] + (Mission_ItemCurNum_Begin - Mission_ItemID_Begin);
					break;
				}
			}

			if(flag_currentnum == 0)
				continue;

			for(S32 k = 0; k < MISSION_FLAG_MAX; ++k)
			{
				if(player->missionInfo.Flag[i].Name[k] == flag_currentnum)
					return player->missionInfo.Flag[i].Value[k];
			}
			break;
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------
// 设置任务物品数量
bool MissionManager::UpdateTaskItem(Player* player, U32 itemid)
{
	if(!player || itemid == 0)
		return false;

	ItemBaseData* pItemData = g_ItemRepository->getItemData(itemid);
	if(!pItemData)
		return false;

	Vector<S32> needMissionList;
	if(!pItemData->getMissionNeed(needMissionList))
		return false;

	S32 itemnum = player->inventoryList.GetItemCount(itemid);
	U32 flag_currentnum, flag_neednum;
	S32 pos_currentnum, pos_neednum;
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == -1)
			continue;
		
		// 检查当前任务编号是否为物品所需任务编号
		bool bfind = false;
		for(S32 j = 0; j < needMissionList.size(); j++)
		{
			if(player->missionInfo.Flag[i].MissionID == needMissionList[j])
			{
				bfind = true;
				break;
			}
		}
		if(bfind == false)
			continue;
	
		// 寻找任务旗标相关物品的旗标位置
		flag_currentnum = flag_neednum = 0;
		for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
		{
			if(player->missionInfo.Flag[i].Name[j] >= Mission_ItemID_Begin &&
				player->missionInfo.Flag[i].Name[j] <= Mission_ItemID_End &&
				player->missionInfo.Flag[i].Value[j] == itemid)
			{
				flag_neednum	= player->missionInfo.Flag[i].Name[j] + (Mission_ItemNeedNum_Begin - Mission_ItemID_Begin);
				flag_currentnum = player->missionInfo.Flag[i].Name[j] + (Mission_ItemCurNum_Begin - Mission_ItemID_Begin);
				break;
			}
		}
		
		// 获取所需物品数量和当前物品数量的旗标位置
		pos_currentnum = pos_neednum = -1;
		for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
		{
			if(player->missionInfo.Flag[i].Name[j] == flag_neednum)
				pos_neednum = j;
			else if(player->missionInfo.Flag[i].Name[j] == flag_currentnum)
				pos_currentnum = j;
			else if(player->missionInfo.Flag[i].Name[j] == 0 && pos_currentnum == -1)
				pos_currentnum = j;
		}

		if(pos_neednum == -1 || pos_currentnum == -1)
			continue;

		S32 neednum = player->missionInfo.Flag[i].Value[pos_neednum];
		S32& currentnum = player->missionInfo.Flag[i].Value[pos_currentnum];
		if(neednum <= 0)
			continue;
		currentnum = neednum > itemnum ? itemnum : neednum;
#ifdef NTJ_SERVER
		UpdateMission(player, MissionManager::UPDATEONEANDONE, i, pos_currentnum);
#endif
	}
	return true;
}

// ----------------------------------------------------------------------------
// 判断玩家任务里是否需求杀某怪物
bool MissionManager::IsNeedKillMonster(Player* player, U32 monsterid, S32 MissionID /* = -1 */)
{
	if(!player)
		return false;
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			if(MissionID != -1 && player->missionInfo.Flag[i].MissionID != MissionID)
				continue;

			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] >= Mission_MonsterID_Begin &&
					player->missionInfo.Flag[i].Name[j] <= Mission_MonsterID_End &&
					player->missionInfo.Flag[i].Value[j] == monsterid)
				{
					return true;
				}
			}
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
// 获取玩家任务里需求杀某怪物的数量
S32 MissionManager::GetNeedKillMonster(Player* player, U32 monsterid, S32 MissionID /* = -1 */)
{
	if(!player)
		return false;
	S32 neednum = 0;
	U32 flag_neednum;
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			if(MissionID != -1 && player->missionInfo.Flag[i].MissionID != MissionID)
				continue;
			flag_neednum = 0;
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] >= Mission_MonsterID_Begin &&
					player->missionInfo.Flag[i].Name[j] <= Mission_MonsterID_End &&
					player->missionInfo.Flag[i].Value[j] == monsterid)
				{
					flag_neednum = player->missionInfo.Flag[i].Name[j] + (Mission_MonsterNeedNum_Begin - Mission_MonsterID_Begin);
					for(S32 k = 0; k < MISSION_FLAG_MAX; ++k)
					{
						if(player->missionInfo.Flag[i].Name[k] == flag_neednum)
						{
							neednum += player->missionInfo.Flag[i].Value[k];
							break;
						}
					}
					break;
				}
			}
		}
	}
	return neednum;
}

// ----------------------------------------------------------------------------
// 获取玩家任务里杀某怪物的数量
S32 MissionManager::GetKillMonster(Player* player, U32 monsterid, S32 MissionID /* = -1 */)
{
	if(!player)
		return false;
	S32 currentnum = 0;
	U32 flag_currentnum;
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			if(MissionID != -1 && player->missionInfo.Flag[i].MissionID != MissionID)
				continue;

			flag_currentnum = 0;
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] >= Mission_MonsterID_Begin &&
					player->missionInfo.Flag[i].Name[j] <= Mission_MonsterID_End &&
					player->missionInfo.Flag[i].Value[j] == monsterid)
				{
					flag_currentnum = player->missionInfo.Flag[i].Name[j] + (Mission_MonsterCurNum_Begin - Mission_MonsterID_Begin);
					for(S32 k = 0; k < MISSION_FLAG_MAX; ++k)
					{
						if(player->missionInfo.Flag[i].Name[k] == flag_currentnum)
						{
							currentnum += player->missionInfo.Flag[i].Value[k];
							break;
						}
					}
					break;
				}
			}
		}
	}
	return currentnum;
}

// ----------------------------------------------------------------------------
// 设置玩家任务里杀某怪物的数量
bool MissionManager::SetKillMonster(Player* player, U32 monsterid)
{
	if(!player)
		return false;

	U32 flag_currentnum;
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			flag_currentnum = 0;
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] >= Mission_MonsterID_Begin &&
					player->missionInfo.Flag[i].Name[j] <= Mission_MonsterID_End &&
					player->missionInfo.Flag[i].Value[j] == monsterid)
				{
					flag_currentnum = player->missionInfo.Flag[i].Name[j] + (Mission_MonsterCurNum_Begin - Mission_MonsterID_Begin);
					for(S32 k = 0; k < MISSION_FLAG_MAX; ++k)
					{
						if(player->missionInfo.Flag[i].Name[k] == flag_currentnum)
						{
							player->missionInfo.Flag[i].Value[k]++;
#ifdef NTJ_SERVER
							UpdateMission(player, MissionManager::UPDATEONE, player->missionInfo.Flag[i].MissionID);
#endif
							break;
						}
					}
					break;
				}
			}
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// 设置任务旗标变量值
bool MissionManager::SetMissionFlag(Player* player, S32 MissionID,	S32 FlagName, S32 FlagValue, S32& idPos, S32& flagPos, bool IsUpdate)
{
	idPos = flagPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	// 如果任务已经完成，直接返回
	if(TestState(player,MissionID))
		return true;

	bool IsAddMission = false;
	for(S32 i = MISSION_ACCEPT_MAX - 1; i >=0; --i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			IsAddMission = false;
			idPos = i;
			flagPos = -1;
			for(S32 j = MISSION_FLAG_MAX - 1; j >= 0; --j)
			{
				if(player->missionInfo.Flag[i].Name[j] == FlagName)
				{
					flagPos = j;
					break;
				}
				else if(player->missionInfo.Flag[i].Name[j] == 0)
				{
					flagPos = j;
				}
			}
			break;
		}
		else if(player->missionInfo.Flag[i].MissionID == -1)
		{
			IsAddMission = true;
			idPos = i;
			flagPos = 0;
		}
	}

	if(idPos != -1 && flagPos != -1)
	{
		player->missionInfo.Flag[idPos].MissionID = MissionID;
		player->missionInfo.Flag[idPos].Name[flagPos] = FlagName;
		player->missionInfo.Flag[idPos].Value[flagPos] = FlagValue;
		// 判断是否限时任务，因为限时任务需要程序自动额外增加二个旗标变量
		if(FlagName == Mission_LimitTime_Need)
		{
			S32 idPos1, idPos2, flagPos1, flagPos2;
			bool Succeed1 = SetMissionFlag(player, MissionID, Mission_LimitTime_Lapse, 0, idPos1, flagPos1, false);
			bool Succeed2 = SetMissionFlag(player, MissionID, Mission_LimitTime_Begin, Platform::getTime(), idPos2, flagPos2, false);
			if(!Succeed1 || !Succeed2)
			{
				player->missionInfo.Flag[idPos].MissionID = -1;
				player->missionInfo.Flag[idPos].Name[flagPos] = 0;
				player->missionInfo.Flag[idPos].Value[flagPos] = 0;
				if(Succeed1)
				{
					player->missionInfo.Flag[idPos1].Name[flagPos1] = 0;
					player->missionInfo.Flag[idPos1].Value[flagPos1] = 0;
				}
				if(Succeed2)
				{
					player->missionInfo.Flag[idPos2].Name[flagPos2] = 0;
					player->missionInfo.Flag[idPos2].Value[flagPos2] = 0;
				}
				return false;
			}
		}
#ifdef NTJ_SERVER
		if(IsUpdate)
			return IsAddMission ? UpdateMission(player, MissionManager::UPDATEONE, player->missionInfo.Flag[idPos].MissionID) :
				UpdateMission(player, MissionManager::UPDATEONEANDONE, idPos, flagPos);
#endif
		return true;
	}
	return false;
}

#ifdef NTJ_SERVER

// ----------------------------------------------------------------------------
// 发送初始化数据
bool MissionManager::SendInitialize(Player* player)
{
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
			UpdateMission(player, UPDATEONE, player->missionInfo.Flag[i].MissionID, -1, false);
	}

	for(int i = 0; i < MissionEvent::STATE_BLOCK; ++i)
		UpdateMission(player, UPDATEALLSTATE, i, -1, true);
	return true;
}

// ----------------------------------------------------------------------------
// 更新任务给客户端
bool MissionManager::UpdateMission(Player* player, U32 opt, S32 arg1, S32 arg2, bool IsNotify)
{
	if(!player)
		return false;
	GameConnection* conn = player->getControllingClient();
	if(conn)
	{
		MissionEvent* ev = new MissionEvent();
		ev->setParam(opt, arg1, arg2, IsNotify);
		conn->postNetEvent(ev);
		return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
// 分享任务给队友
bool MissionManager::ShareMission(Player* player, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;
	//判断是否已完成此任务
	if(IsFinishedMission(player, MissionID))
		return false;
	S32 idPos;
	//判断是否未接到此任务
	if(!IsAcceptedMission(player, MissionID, idPos))
		return false;

	stTeamInfo& teamInfo = player->getTeamInfo();
	// 判断是否在队中
	if(!teamInfo.m_bInTeam)
		return false;

	// 查找队友的所有任务
	CZoneTeam* pTeam = g_ZoneTeamManager.GetTeam(teamInfo.m_nId);
	if(!pTeam)
		return false;

	Player* teamates = NULL;
	for(S32 i = 0; i < pTeam->GetTeammateCount(); i++)
	{
		teamates = g_ServerGameplayState->GetPlayer( pTeam->GetPlayerId( i ) );
		if( !teamates || teamates == player)
			continue;
		//判断是否已完成此任务
		if(IsFinishedMission(teamates, MissionID))
			continue;
		S32 idPos;
		//判断是否未接到此任务
		if(IsAcceptedMission(teamates, MissionID, idPos))
			continue;
		//服务端脚本检查玩家是否能接此任务
		const char* ret = Con::executef("CanDoThisMission",
								Con::getIntArg(teamates->getId()),
								Con::getIntArg(MissionID),
								Con::getIntArg(1),
								Con::getIntArg(0),
								Con::getIntArg(0));
		if(!ret || ret[0])
			continue;

		//向队友发送共享任务接取流程
		Con::executef("Mission_Share",
								Con::getIntArg(teamates->getId()),
								Con::getIntArg(MissionID),
								Con::getIntArg(0));
	}
	return true;
}

// ----------------------------------------------------------------------------
// 请求更新共享任务
bool MissionManager::ShareMissionResponse(Player* player)
{
	if(!player)
		return false;
	stTeamInfo& teamInfo = player->getTeamInfo();
	// 判断是否在队中
	if(!teamInfo.m_bInTeam)
	{
		player->missionShareList.clear();
		return false;
	}

	// 查找队友的所有任务
	CZoneTeam* pTeam = g_ZoneTeamManager.GetTeam(teamInfo.m_nId);
	if(!pTeam)
	{
		player->missionShareList.clear();
		return false;
	}

	//获取队伍内所有共享任务
	bool ret = pTeam->GetShareMission(player->getPlayerID());
	//服务端脚本过滤某些特定任务的共享
	Con::executef("FilterShareMission", Con::getIntArg(player->getId()));
	//发送共享任务数据到客户端
	UpdateMission(player, UPDATEALLSHAREFROM);
	return ret;
}

// ----------------------------------------------------------------------------
// 清除循环任务完成时间
bool MissionManager::ClearCycleMissionTime(Player* player)
{
	if(!player)
		return false;
	for(S32 i = MISSION_CYCLE_MAX - 1; i >= 0; --i)
	{
		if(player->missionInfo.Cycle[i].MissionID != -1)
		{
			player->missionInfo.Cycle[i].EndTime = 0;
			return UpdateMission(player, MissionManager::UPDATEONECYCLE, i);
		}
	}
	return true;
}

#endif//NTJ_SERVER
// ----------------------------------------------------------------------------
// 检查是否限时任务
bool MissionManager::IsTimeInfo(Player* player, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Need) 
					return true;
			}
			break;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
// 检查某限时任务
bool MissionManager::CheckTime(Player* player, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			S32 *need, *start;
			need = start = NULL;
			U32 curtime = Platform::getTime();
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Need) 
				{
					need = &player->missionInfo.Flag[i].Value[j];
				}

				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Begin)
				{
					start = &player->missionInfo.Flag[i].Value[j];
				}
			}
			if(need && start)
				return curtime - *start <= *need;
			break;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
// 启动某个限时任务
bool MissionManager::StartTime(Player* player, S32 MissionID)
{
	if(!player ||  MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	bool ret = false;
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID == MissionID)
		{
			S32 *need, *lapse, *start;
			need = lapse = start = NULL;
			U32 curtime = Platform::getTime();
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Need) 
				{
					need = &player->missionInfo.Flag[i].Value[j];
				}

				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Lapse)
				{
					lapse = &player->missionInfo.Flag[i].Value[j];
				}

				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Begin)
				{
					start = &player->missionInfo.Flag[i].Value[j];
				}
			}
			if(need && lapse && start)
			{
				*start = curtime;
#ifdef NTJ_SERVER
				ret = UpdateMission(player, STARTTIME, MissionID);
#else
				ret = true;
#endif
			}
			break;
		}
	}
	return ret;
}

// ----------------------------------------------------------------------------
// 暂停所有限时任务
// Note::这个方法用于玩家下线时或切换地图前调用
void MissionManager::PauseTime(Player* player)
{
	if(!player)
		return;

	S32 *need, *lapse, *start;
	U32 currenttime = Platform::getTime();
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			need = lapse = start = NULL;
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Need) 
				{
					need = &player->missionInfo.Flag[i].Value[j];
				}

				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Lapse)
				{
					lapse = &player->missionInfo.Flag[i].Value[j];
				}

				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Begin)
				{
					start = &player->missionInfo.Flag[i].Value[j];
				}
			}

			if(need && lapse && start)
				*lapse = (currenttime - *start);			
		}
	}
}

// ----------------------------------------------------------------------------
// 继续所有限时任务
// Note::这个方法用于玩家上线或切换到目标地图后调用
void MissionManager::ContinueTime(Player* player)
{
	if(!player)
		return;

	S32 *need, *lapse, *start;
	U32 currenttime = Platform::getTime();
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			need = lapse = start = NULL;
			for(S32 j = 0; j < MISSION_FLAG_MAX; ++j)
			{
				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Need) 
				{
					need = &player->missionInfo.Flag[i].Value[j];
				}

				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Lapse)
				{
					lapse = &player->missionInfo.Flag[i].Value[j];
				}

				if(player->missionInfo.Flag[i].Name[j] == Mission_LimitTime_Begin)
				{
					start = &player->missionInfo.Flag[i].Value[j];
				}
			}

			if(need && lapse && start)
			{
				*start = (currenttime - *lapse);
			}
		}
	}
#ifdef NTJ_SERVER
	UpdateMission(player, CONTINUETIME);
#endif
}

// ----------------------------------------------------------------------------
// 通知客户端结束限时任务
bool MissionManager::EndTime(Player* player, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	if(IsTimeInfo(player, MissionID))
	{
#ifdef NTJ_SERVER
		return UpdateMission(player, ENDTIME, MissionID);
#else
		return true;
#endif
	}
	return false;
}

#ifdef NTJ_CLIENT

// ----------------------------------------------------------------------------
// 按序号获取已接任务的任务编号
S32 MissionManager::GetMission(Player* player, U32 index)
{
	if(!player || index >= MISSION_ACCEPT_MAX)
		return 0;

	U32 No = 0;
	for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
	{
		if(player->missionInfo.Flag[i].MissionID != -1)
		{
			if(No == index)
				return player->missionInfo.Flag[i].MissionID;
			No ++;
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------
// 发送客户端请求任务消息（接取、放弃、共享队友或获取队伍共享）
bool MissionManager::SendMissionRequest(Player* player, U32 type, S32 missionid)
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!player || !conn)
		return false;

	ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_MISSION);
	ev->SetIntArgValues(2, type, missionid);
	conn->postNetEvent(ev);
	return true;
}
#endif//NTJ_CLIENT