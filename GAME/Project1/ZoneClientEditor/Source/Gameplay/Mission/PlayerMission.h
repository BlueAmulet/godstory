#ifndef __PLAYERMISSION_H__
#define __PLAYERMISSION_H__

// ============================================================================
// ����ϵͳ��Ҫ˵��
// 1.�������洢������PlayerStruct��
// 2.����������������PlayerObject��
// 3.���������������ݴ�����Ҫ�����¶���:
//		a.MissionEvent(GameNetEvent�¼�����)
//		b.��־ΪInfo_ShareMission��GamePlayerEvent�¼�
// 4.NPC�Ի����񽻻�(�ɼ�Conversation.h)
// 5.
//
// ============================================================================

#include "Common\PlayerStruct.h"

class Player;
// ----------------------------------------------------------------------------
//	�����������
//
enum enMission
{
	Mission_FinishedCount		= 0,			// �������������
	Mission_AcceptedCount		= 1,			// �ѽ���������
	Mission_TimeInfoCount		= 2,			// �ѽ���ʱ��������

	Mission_Step				= 1,			// �����������1,���ʾ���ֵΪStep��ֵ
	Mission_Share				= 2,			// �����������2,���ʾ���ֵΪ������ʽ
	Mission_Freeze				= 3,			// �����������3,���ֵΪ����ID����ʾ�����������

	Mission_LimitTime_Need		= 10,			// ��ʱ������ʱ����
	Mission_LimitTime_Lapse		= 11,			// ��ʱ��������ʱ������
	Mission_LimitTime_Begin		= 12,			// ��ʱ����ʼ��ʱ��

	Mission_NpcID_Begin			= 1000,			// �����������Ϊ1000-1999,���ʾ���ֵΪNPC���
	Mission_NpcID_End			= 1999,			// ����999��

	Mission_ItemID_Begin		= 2100,			// �����������Ϊ2100-2199,���ʾ���ֵΪ��Ʒ���
	Mission_ItemID_End			= 2199,			// ����99��
	Mission_ItemNeedNum_Begin	= 2200,			// �����������Ϊ2200-2299,���ʾ���ֵΪ������Ʒ����
	Mission_ItemNeedum_End		= 2299,			// ����99��
	Mission_ItemCurNum_Begin	= 2300,			// �����������Ϊ2200-2299,���ʾ���ֵΪ��ǰ��Ʒ����
	Mission_ItemCurNum_End		= 2399,			// ����99��

	Mission_MonsterID_Begin		= 3100,			// �����������Ϊ3100-3199,���ʾ���ֵΪ������
	Mission_MonsterID_End		= 3199,			// ����99��
	Mission_MonsterNeedNum_Begin= 3200,			// �����������Ϊ3200-3299,���ʾ���ֵΪ�����������
	Mission_MonsterNeedNum_End	= 3299,			// ����99��
	Mission_MonsterCurNum_Begin	= 3300,			// �����������Ϊ3300-3399,���ʾ���ֵΪ��ǰ��������
	Mission_MonsterCurNum_End	= 3399,			// ����99��	

	Mission_DialogID_Begin		= 4000,			// �����������Ϊ4000-4999,���ʾ���ֵΪ�Ի����
	Mission_DialogID_End		= 4999,			// ����999��

	Mission_SkillID_Begin		= 5000,			// �����������Ϊ5000-5999,���ʾ���ֵΪ���ܱ��
	Mission_SkillID_End			= 5999,			// ����999��

	Mission_StatusID_Begin		= 6000,			// �����������Ϊ6000-6999,���ʾ���ֵΪ״̬���
	Mission_StatusD_End			= 6999,			// ����999��

	Mission_TimeFM_Begin		= 7000,			// �����������Ϊ7000-7999,���ʾ���ֵΪʱ���ʽ
	Mission_TimeFM_End			= 7999,			// ����999��

	Mission_Number_Begin		= 8000,			// �����������Ϊ8000-8999,���ʾ���ֵΪ��Ӧ����
	Mission_Number_End			= 8999			// ����999��
};

// �������ݲ�����
class MissionManager
{
public:
	enum Constants
	{
		DONOTHING,								// ʲô������
		ADDONE,									// ���һ������
		//ADDONEANDONE,							// ���һ�������ĳ���������
		UPDATEONE,								// ����һ������
		UPDATEONEANDONE,						// ����һ�������ĳ���������
		DELETEONE,								// ɾ��һ������
		DELETEONEANDONE,						// ɾ��һ�������ĳ���������
		UPDATEONESTATE,							// ����һ���������״̬
		UPDATEALLSTATE,							// ���������������״̬
		UPDATEONECYCLE,							// ����һ��ѭ������
		UPDATEALLCYCLE,							// ��������ѭ������
		DELONECYCLE,							// ɾ��һ��ѭ������
		//UPDATEONESHARETO,						// ����һ�����������
		//UPDATEALLSHARETO,						// �����������������
		UPDATEONESHAREFROM,						// ����һ����������
		UPDATEALLSHAREFROM,						// �������й�������
		STARTTIME,								// ������ʱ����
		ENDTIME,								// ������ʱ����
		CONTINUETIME,							// ������ʱ����
		UPDATENEWBIEMISSION,					// ��������
		UPDATENEWBIEHELP,						// ���ְ���

		MISSIONREQUEST_ACCEPT	= 0,			// �ͻ��������ȡ����
		MISSIONREQUEST_DELETE	= 1,			// �ͻ��������������
		MISSIONREQUEST_SHARE	= 2 ,			// �ͻ������������������
		MISSIONREQUEST_SHAREGET = 3,			// �ͻ��������ȡ���鹲������
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
	// �����Ժ���
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