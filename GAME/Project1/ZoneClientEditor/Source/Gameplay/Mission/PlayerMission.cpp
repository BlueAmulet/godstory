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
// ���캯��
MissionManager::MissionManager()
{
	g_MissionManager = this;
}

// ----------------------------------------------------------------------------
// �����������״̬λ
bool MissionManager::SetState(Player* player, U32 bit)
{
	if(!player || bit < 0 || bit >= MISSION_MAX)
		return false;

	player->missionInfo.State[bit >> 3] |= U8(1 << (bit & 0x7));
	return true;
}

// ----------------------------------------------------------------------------
// ���Ի������״̬λ
bool MissionManager::TestState(Player* player, U32 bit)
{
	if(!player || bit < 0 || bit >= MISSION_MAX)
		return false;

	return (player->missionInfo.State[bit >> 3] & U8(1 << (bit & 0x7))) != 0;
}

// ----------------------------------------------------------------------------
// ����������״̬λ
bool MissionManager::ClearState(Player* player, U32 bit)
{
	if(!player || bit < 0 || bit >= MISSION_MAX)
		return false;

	player->missionInfo.State[bit >> 3] &= U8(~(1 << (bit & 0x7)));
	return true;
}

// ----------------------------------------------------------------------------
// ��ȡ���������Ϣ
S32 MissionManager::GetInfo(Player* player, U32 type)
{
	if(!player)
		return 0;
	S32 count = 0;
	switch(type)
	{
	case Mission_FinishedCount:			// �������������
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
	case Mission_AcceptedCount:			// �ѽ�δ������������
		{
			for(S32 i = 0; i < MISSION_ACCEPT_MAX; ++i)
			{
				if(player->missionInfo.Flag[i].MissionID != -1)
					count ++;
			}
			return count;
		}
	case Mission_TimeInfoCount:			// �ѽ���ʱ���������
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
// ����Ƿ��ѽ�ĳ����
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
// ����Ƿ������ĳ����
bool MissionManager::IsFinishedMission(Player* player, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;
	return TestState(player, MissionID);
}

// ----------------------------------------------------------------------------
// ����Ƿ����ĳ����
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
// ����Ƿ����ĳ�������
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
// ����ѭ��������������ʱ��
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
// ��ȡѭ��������������ʱ��
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
// ɾ��ѭ��������������ʱ��
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
// ��ȡ����������ֵ
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
// �����������״̬
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
// ɾ��ĳ�������
bool MissionManager::DelMissionFlag(Player* player, S32 MissionID, S32 FlagName, S32& idPos, S32& flagPos)
{
	idPos = flagPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	bool ret = false;
	// �Ƿ�����������
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
// ɾ��ĳ����
bool MissionManager::DelMission(Player* player, S32 MissionID, S32& idPos)
{
	idPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	bool ret = false;
	// �Ƿ��ѽ�����
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
// �ж��������������ĳ�������
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
// ��ȡ�������������ĳ������ߵ�����
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
// ��ȡ������Ʒ����
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
// ����������Ʒ����
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
		
		// ��鵱ǰ�������Ƿ�Ϊ��Ʒ����������
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
	
		// Ѱ��������������Ʒ�����λ��
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
		
		// ��ȡ������Ʒ�����͵�ǰ��Ʒ���������λ��
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
// �ж�����������Ƿ�����ɱĳ����
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
// ��ȡ�������������ɱĳ���������
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
// ��ȡ���������ɱĳ���������
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
// �������������ɱĳ���������
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
// ��������������ֵ
bool MissionManager::SetMissionFlag(Player* player, S32 MissionID,	S32 FlagName, S32 FlagValue, S32& idPos, S32& flagPos, bool IsUpdate)
{
	idPos = flagPos = -1;
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;

	// ��������Ѿ���ɣ�ֱ�ӷ���
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
		// �ж��Ƿ���ʱ������Ϊ��ʱ������Ҫ�����Զ��������Ӷ���������
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
// ���ͳ�ʼ������
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
// ����������ͻ���
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
// �������������
bool MissionManager::ShareMission(Player* player, S32 MissionID)
{
	if(!player || MissionID < 0 || MissionID >= MISSION_MAX)
		return false;
	//�ж��Ƿ�����ɴ�����
	if(IsFinishedMission(player, MissionID))
		return false;
	S32 idPos;
	//�ж��Ƿ�δ�ӵ�������
	if(!IsAcceptedMission(player, MissionID, idPos))
		return false;

	stTeamInfo& teamInfo = player->getTeamInfo();
	// �ж��Ƿ��ڶ���
	if(!teamInfo.m_bInTeam)
		return false;

	// ���Ҷ��ѵ���������
	CZoneTeam* pTeam = g_ZoneTeamManager.GetTeam(teamInfo.m_nId);
	if(!pTeam)
		return false;

	Player* teamates = NULL;
	for(S32 i = 0; i < pTeam->GetTeammateCount(); i++)
	{
		teamates = g_ServerGameplayState->GetPlayer( pTeam->GetPlayerId( i ) );
		if( !teamates || teamates == player)
			continue;
		//�ж��Ƿ�����ɴ�����
		if(IsFinishedMission(teamates, MissionID))
			continue;
		S32 idPos;
		//�ж��Ƿ�δ�ӵ�������
		if(IsAcceptedMission(teamates, MissionID, idPos))
			continue;
		//����˽ű��������Ƿ��ܽӴ�����
		const char* ret = Con::executef("CanDoThisMission",
								Con::getIntArg(teamates->getId()),
								Con::getIntArg(MissionID),
								Con::getIntArg(1),
								Con::getIntArg(0),
								Con::getIntArg(0));
		if(!ret || ret[0])
			continue;

		//����ѷ��͹��������ȡ����
		Con::executef("Mission_Share",
								Con::getIntArg(teamates->getId()),
								Con::getIntArg(MissionID),
								Con::getIntArg(0));
	}
	return true;
}

// ----------------------------------------------------------------------------
// ������¹�������
bool MissionManager::ShareMissionResponse(Player* player)
{
	if(!player)
		return false;
	stTeamInfo& teamInfo = player->getTeamInfo();
	// �ж��Ƿ��ڶ���
	if(!teamInfo.m_bInTeam)
	{
		player->missionShareList.clear();
		return false;
	}

	// ���Ҷ��ѵ���������
	CZoneTeam* pTeam = g_ZoneTeamManager.GetTeam(teamInfo.m_nId);
	if(!pTeam)
	{
		player->missionShareList.clear();
		return false;
	}

	//��ȡ���������й�������
	bool ret = pTeam->GetShareMission(player->getPlayerID());
	//����˽ű�����ĳЩ�ض�����Ĺ���
	Con::executef("FilterShareMission", Con::getIntArg(player->getId()));
	//���͹����������ݵ��ͻ���
	UpdateMission(player, UPDATEALLSHAREFROM);
	return ret;
}

// ----------------------------------------------------------------------------
// ���ѭ���������ʱ��
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
// ����Ƿ���ʱ����
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
// ���ĳ��ʱ����
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
// ����ĳ����ʱ����
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
// ��ͣ������ʱ����
// Note::������������������ʱ���л���ͼǰ����
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
// ����������ʱ����
// Note::�����������������߻��л���Ŀ���ͼ�����
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
// ֪ͨ�ͻ��˽�����ʱ����
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
// ����Ż�ȡ�ѽ������������
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
// ���Ϳͻ�������������Ϣ����ȡ��������������ѻ��ȡ���鹲��
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