#include "math/mMathFn.h"
#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "Gameplay/Data/PetStudyData.h"
#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/GameObjects/PetObjectData.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#endif

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/Item/ItemShortcut.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#endif

#include "Gameplay/GameplayCommon.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/TimerTrigger.h"

////////////////////////////   TimerTriggerBase   ///////////////////////////////////////
TimerTriggerBase::TimerTriggerBase(U32 nId, U32 nLeftTime, U32 nTimeFlag, U32 nInterval, TimerTriggerType nType) 
	: m_Id(nId), m_nLeftTime(nLeftTime), m_nTimeFlag(nTimeFlag), m_nIntervalTime(nInterval), m_nType(nType)
{
	m_TimerTriggerMgr = NULL;
	m_nStartTime = 0;
}

TimerTriggerBase::~TimerTriggerBase()
{
}

bool TimerTriggerBase::ProcessTick()	//Called by TimerTriggerManager::ProcessTick()
{
	U32 nCurrTime = Platform::getRealMilliseconds();
#ifdef NTJ_SERVER
	if ( (nCurrTime - m_nStartTime) >= m_nLeftTime)
	{
		ExecuteEvent();
		End();
	}
#endif

#ifdef NTJ_CLIENT
	//超过间隔时间
	if (m_nLeftTime == 0)
		return true;

	if ((nCurrTime - m_nStartTime) >= m_nIntervalTime)
	{
		if (nCurrTime - m_nStartTime >= m_nLeftTime)
		{
			m_nLeftTime = 0;
		}
		else
		{			
			m_nLeftTime -= (nCurrTime - m_nStartTime);
			m_nStartTime = nCurrTime;
		}
		RefreshWindow();
	}
#endif

	return true;
}

bool TimerTriggerBase::Start()
{
	m_nStartTime = Platform::getRealMilliseconds();
	return true;
}

void TimerTriggerBase::SetManager(TimerTriggerManager *mgr)
{
	m_TimerTriggerMgr = mgr;
}

TimerTriggerManager *TimerTriggerBase::GetManager()
{
	return m_TimerTriggerMgr;
}

////////////////////////////   TimerTrigger   ///////////////////////////////////////
TimerTrigger::TimerTrigger(U32 nId, U32 nLeftTime, U32 nIntervalTime, bool bCanCancel, bool bCanSave, U32 nTimeFlag)
							: TimerTriggerBase(nId, nLeftTime, nTimeFlag, nIntervalTime), m_bCanCancel(bCanCancel),
							m_bCanSave(bCanSave)
{
	m_nState = STATE_NONE;
#ifdef NTJ_CLIENT
	m_ctrlIndex = 0;
#endif
}

TimerTrigger::~TimerTrigger()
{
#ifdef NTJ_CLIENT
	m_arrRewardIds.clear();
#endif
}

void TimerTrigger::SaveData(stPlayerStruct *pPlayerData, S32 nIndex)
{
	stTimerTriggerInfo *pInfo = pPlayerData->MainData.TimerTriggerInfo;

	if (m_bCanSave)
	{
		//存储此计时器数据
		pInfo[nIndex].m_Id = m_Id;
		pInfo[nIndex].m_bCanCancel = m_bCanCancel;
		U32 nElapseTime = Platform::getRealMilliseconds() - m_nStartTime;
		pInfo[nIndex].m_nLeftTime = m_nLeftTime > nElapseTime ? (m_nLeftTime - nElapseTime) : 0;
		pInfo[nIndex].m_nIntervalTime = m_nIntervalTime;
		pInfo[nIndex].m_nTimeFlag = m_nTimeFlag;
		pInfo[nIndex].m_nState = m_nState;
		S32 nItemCount = m_arrRewardIds.size();
		for (int j = 0; j < 4; j++)
		{
			if (j < nItemCount)
				pInfo[nIndex].m_nItemId[j] = m_arrRewardIds[j];
			else
				pInfo[nIndex].m_nItemId[j] = 0;
		}
	}
}

void TimerTrigger::InitData(stPlayerStruct *pPlayerData, S32 nIndex)
{

}

bool TimerTrigger::SendInitialData(GameConnection *conn)
{
	if (m_nLeftTime == 0)
	{
		ExecuteEvent();
		return false;
	}
	else
	{
		//首先发送生成计时触发器消息			
		ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER);			
		if (ev && conn)
		{
			ev->SetInt32ArgValues( 5, 1, m_Id, m_nLeftTime, m_nIntervalTime, m_nTimeFlag);  //1表示生成计时器
			ev->SetIntArgValues(2, m_bCanCancel, m_bCanSave);	
			conn->postNetEvent(ev);
		}

		//发送开始消息，同时发送物品列表
		ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER);
		if (ev && conn)
		{
			// 发送物品列表
			S32 count = m_arrRewardIds.size();
			ev->SetInt32ArgValues(3, 2, m_Id, count);		//发送消息通知客户端启动计时器
			for (S32 i = 0; i < count; i++)
			{
				U32 nItemId = m_arrRewardIds[i];
				ev->AddMemoryBlock((char *)&nItemId, sizeof(U32));
			}
			conn->postNetEvent(ev);
		}
		//++it;
		return true;
	}
}

void TimerTrigger::OnClear()
{
#ifdef NTJ_CLIENT
	Con::executef("SetTimerCtrlVisible", Con::getIntArg(m_ctrlIndex), "0");
#endif
}

bool TimerTrigger::ProcessTick()
{
	if (m_nState == STATE_NONE)
		return true;
	else if ( m_nState == STATE_END || m_nState == STATE_CANCEL)
		return false;

	return Parent::ProcessTick();
}

void TimerTrigger::RefreshWindow()
{
#ifdef NTJ_CLIENT
	U32 nLeftTime = GetLeftTime();
	Con::executef("UpdateTimerCtrlLeftTime", Con::getIntArg(m_ctrlIndex), Con::getIntArg(nLeftTime));
#endif
}

void TimerTrigger::ExecuteEvent()
{
#ifdef NTJ_SERVER
	Con::executef("echo", "TimerTrigger::ExecuteEvent on ZoneServer!");		//测试使用
	Con::executef("OnTimerControlTriggered", 
					Con::getIntArg(GetManager()->GetPlayer()->getId()),
					Con::getIntArg(GetId()));		//测试使用
#endif
}

#ifdef NTJ_SERVER
void TimerTrigger::ServerSendEndMsg()
{
	Player *pPlayer = NULL;
	if (!m_TimerTriggerMgr || !(pPlayer = m_TimerTriggerMgr->GetPlayer()) )
		return;

	GameConnection *conn = pPlayer->getControllingClient();
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER);
	if (conn && ev)
	{
		ev->SetInt32ArgValues(2, 3, m_Id);
		conn->postNetEvent(ev);
	}
}

void TimerTrigger::ServerSendCancelMsg()
{
	Player *pPlayer = NULL;
	if (!m_TimerTriggerMgr || !(pPlayer = m_TimerTriggerMgr->GetPlayer()) )
		return;

	GameConnection *conn = pPlayer->getControllingClient();
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER);
	if (conn && ev)
	{
		ev->SetInt32ArgValues(2, 4, m_Id);
		conn->postNetEvent(ev);
	}
}
#endif

bool TimerTrigger::Start()
{
	if (!Parent::Start())
		return false;
	if (m_nState != STATE_NONE)
		return false;

	m_nState = STATE_START;
#ifdef NTJ_CLIENT
	SetVisible(true);
#endif
	return true;
}

bool TimerTrigger::Cancel()
{
	if (m_nState == STATE_START && m_bCanCancel)
	{
		m_nState = STATE_CANCEL;
#ifdef NTJ_CLIENT
		SetVisible(false);
#endif
#ifdef NTJ_SERVER
		ServerSendCancelMsg();
#endif
		return true;
	}

	return false;
}

void TimerTrigger::End()
{
	m_nState = STATE_END;
#ifdef NTJ_SERVER
	ServerSendEndMsg();
#endif
}

void TimerTrigger::FillIdArrayFromString(StringTableEntry sIds, Vector<U32> &arrIds)
{
	if (!sIds || !sIds[0])
		return;

	char intBuf[16];
	StringTableEntry pStart = sIds;
	StringTableEntry pEnd = sIds;
	while(*pEnd != 0)
	{
		while(*pEnd != 0 && *pEnd != ':')
		{
			AssertFatal((*pEnd >= '0' || *pEnd <= '9'), "TimerTrigger::FillRewardIds: invald id string");
			pEnd++;
		}
		dMemcpy(intBuf, pStart, pEnd - pStart);
		intBuf[pEnd - pStart] = 0;
		arrIds.push_back(atoi(intBuf));
		if (*pEnd != 0)
		{
			pEnd++;
			pStart = pEnd;
		}
	}
}

#ifdef NTJ_CLIENT
StringTableEntry TimerTrigger::GetRewardDesc()
{
	return Con::executef("GetTimerInfo", Con::getIntArg(m_Id), "1");
}

void TimerTrigger::InitialControlIcons()
{
	char buf[16];
	S32 nCount = m_arrRewardIds.size();
	for (S32 i = 1; i <= 4; i++)
	{
		S32 iType = 0;
		U32 nId = 0;
		if (i <= nCount)
		{
			nId = m_arrRewardIds[i - 1];
			// 1: 道具 2:技能 3:状态
			_itoa(nId, buf, 10);
			if (buf[0] == '1')			iType = 1;
			else if (buf[0] == '2')		iType = 2;
			else if (buf[0] == '3')		iType = 3;
		}
		Con::executef("SetTimerCtrlIcon", Con::getIntArg(m_ctrlIndex), Con::getIntArg(i), Con::getIntArg(iType), Con::getIntArg(nId));
	}
}

void TimerTrigger::SetVisible(bool bVisible)
{
	if (bVisible)
	{
		//寻找空闲的TimerTrigger控件
		S32 nCtrlIndex = g_ClientGameplayState->GetControlPlayer()->mTimerTriggerMgr->GetFreeCtrlIndex(m_Id);
		if (nCtrlIndex == 0)
			return;				//这里不应该被执行到

		m_ctrlIndex = nCtrlIndex;
		StringTableEntry desc = GetRewardDesc();															//获得奖励说明
		char *sDesc = Con::getArgBuffer(dStrlen(desc) + 1);
		if (sDesc)
		{
			dStrcpy(sDesc, 256, desc);
			Con::executef("SetTimerCtrlDesc", Con::getIntArg(m_ctrlIndex), sDesc);							//设置奖励说明
		}

		InitialControlIcons();																				//初始化奖励物品显示Icon
		Con::executef("UpdateTimerCtrlLeftTime", Con::getIntArg(m_ctrlIndex), Con::getIntArg(m_nLeftTime));	//初始化计时
		Con::executef("SetTimerCtrlCancel", Con::getIntArg(m_ctrlIndex), Con::getIntArg(m_bCanCancel));		//是否设置取消按钮
		Con::executef("SetTimerCtrlVisible", Con::getIntArg(m_ctrlIndex), "1");								//显示控件界面
	}
	else
	{
		g_ClientGameplayState->GetControlPlayer()->mTimerTriggerMgr->DeleteFromList(m_Id);					//删除列表
		Con::executef("SetTimerCtrlVisible", Con::getIntArg(m_ctrlIndex), "0");								//隐藏控件界面
	}
}
#endif

////////////////////////////   TimerTrigger2   ////////////////////////////////////
TimerTriggerPetStudy::TimerTriggerPetStudy(U32 nId, U32 nLeftTime, U32 nTotalTime, U32 nIntervalTime, U32 nTimeFlag)
: TimerTriggerBase(nId, nLeftTime, nTimeFlag, 1000, TimerTriggerBase::TypePetStudy) , m_nTotalTime(nTotalTime)
{
	Start();
}

TimerTriggerPetStudy::~TimerTriggerPetStudy()
{
}

bool TimerTriggerPetStudy::ProcessTick()
{
	if (m_nLeftTime == 0)
		return false;

	return Parent::ProcessTick();
}

void TimerTriggerPetStudy::ExecuteEvent()
{
#ifdef NTJ_SERVER
	Player *pPlayer = NULL;
	stPetInfo *pPetInfo = NULL;
	if (!m_TimerTriggerMgr || !(pPlayer = m_TimerTriggerMgr->GetPlayer()) || 
		!(pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(m_Id-1)))
		return;
	PetStudyData *pData = g_PetStudyRepository.GetPetStudyData(m_nStudyDataId);
	if (!pData)
		return;

	PetTable& petTable = (PetTable&)pPlayer->getPetTable();
	
	if (pData->mType == 1)
	{		
		petTable.addExp(m_Id-1, pData->mRewardPetExp, pPlayer);	// 自身修行
	}
	else if (pData->mType == 2)
	{
		pPlayer->addExp(pData->mRewardPlayerExp);	//代主修行
	}
	else if (pData->mType == 3)
	{
		pPlayer->addMoney(pData->mRewardMoney);		//代主打工
	}
	else
		return;


	pPetInfo->status = PetObject::PetStatus_Idle;
	petTable.setPetStartTime(m_Id-1, Platform::getRealMilliseconds());		//重设宠物快乐度计时开始时间

	Con::executef("echo", "TimerTriggerPetStudy::ExecuteEvent");
	PetOperationManager::ServerSendUpdatePetSlotMsg(pPlayer, m_Id-1);
#endif
}

bool TimerTriggerPetStudy::Start()
{
	if (!Parent::Start())
		return false;
	return true;
}

void TimerTriggerPetStudy::End()
{
	m_nLeftTime = 0;
#ifdef NTJ_SERVER
	ServerSendEndMsg();
#endif
}

void TimerTriggerPetStudy::SaveData(stPlayerStruct *pPlayerData, S32 nIndex)
{
	stTimerTriggerPetStudyInfo *pInfo = pPlayerData->MainData.TimerPetStudyInfo;
	pInfo[nIndex].m_Id = m_Id;
	U32 nElapseTime = Platform::getRealMilliseconds() - m_nStartTime;
	pInfo[nIndex].m_nLeftTime = m_nLeftTime > nElapseTime ? (m_nLeftTime - nElapseTime) : 0;
	pInfo[nIndex].m_nTotalTime = m_nTotalTime;
	pInfo[nIndex].m_nTimeFlag = m_nTimeFlag;
	pInfo[nIndex].m_nStudyDataId = m_nStudyDataId;
}

void TimerTriggerPetStudy::InitData(stPlayerStruct *pPlayerData, S32 nIndex)
{

}

bool TimerTriggerPetStudy::SendInitialData(GameConnection *conn)
{
	if (!conn)
		return false;
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER_PET_STUDY);
	if (!ev)
		return false;
	ev->SetInt32ArgValues(6, 1, m_Id, m_nLeftTime, m_nIntervalTime, m_nTimeFlag, m_nStudyDataId);
	return conn->postNetEvent(ev);
}

void TimerTriggerPetStudy::OnClear()
{

}

void TimerTriggerPetStudy::RefreshWindow()
{
	
}

#ifdef NTJ_SERVER
bool TimerTriggerPetStudy::ServerSendStartMsg()
{

	Player *pPlayer = NULL;
	if (!m_TimerTriggerMgr || !(pPlayer = m_TimerTriggerMgr->GetPlayer()) )
		return false;
	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return false;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER_PET_STUDY);
	if (!ev)
		return false;
	ev->SetInt32ArgValues(7, 1, m_Id, m_nLeftTime, m_nTotalTime, m_nIntervalTime, m_nTimeFlag, m_nStudyDataId);
	return conn->postNetEvent(ev);
}

void TimerTriggerPetStudy::ServerSendEndMsg()
{
	Player *pPlayer = NULL;
	if (!m_TimerTriggerMgr || !(pPlayer = m_TimerTriggerMgr->GetPlayer()) )
		return;
	GameConnection *conn = pPlayer->getControllingClient();
	if (!conn)
		return;
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER_PET_STUDY);
	if (!ev)
		return;
	ev->SetInt32ArgValues(2, 2, m_Id);
	conn->postNetEvent(ev);
}
#endif

////////////////////////////   TimerTriggerManager   ////////////////////////////////////

TimerTriggerManager::TimerTriggerManager()
{
	m_pPlayer = NULL;
}

TimerTriggerManager::~TimerTriggerManager()
{
	ClearList();
}

void TimerTriggerManager::ProcessTick()
{
	std::list<TimerTriggerBase *>::iterator it = m_pList.begin();
	while(it != m_pList.end())
	{
		if (*it)
		{
			if (!(*it)->ProcessTick())
			{
				SAFE_DELETE(*it);
				m_pList.erase(it++);
			}
			else
				++it;
		}
		else
		{
			m_pList.erase(it++);
		}
	}
}

void TimerTriggerManager::SendInitialData()
{
	std::list<TimerTriggerBase *>::iterator it = m_pList.begin();
	TimerTrigger *pTimerTrigger = NULL;

	
	while(it != m_pList.end())
	{
		if( !(*it)->SendInitialData(m_pPlayer->getControllingClient()))
		{
			delete (*it);
			m_pList.erase(it++);
		}
		else
			it++;
	}	
}

bool TimerTriggerManager::CreateTimerPetStudy(U32 nId, U32 nLeftTime, U32 nTotalTime, U32 nIntervalTime, U32 nTimeFlag, U32 nPetStudyDataId)
{
	PetStudyData *pData = g_PetStudyRepository.GetPetStudyData(nPetStudyDataId);
	if(!pData)
		return false;
	//nLeftTime是以分钟为单位，乘以60000转换为毫秒   
	//nTotalTime是以分钟为单位，不用转换为毫秒
	TimerTriggerPetStudy *pTimerTriggerPetStudy = new TimerTriggerPetStudy(nId, nLeftTime * 60000, nTotalTime, nIntervalTime, nTimeFlag);
	if (!pTimerTriggerPetStudy)
		return false;
	pTimerTriggerPetStudy->SetStudyDataId(nPetStudyDataId);
	if (!AddToList(pTimerTriggerPetStudy))
	{
		delete pTimerTriggerPetStudy;
		return false;
	}
#ifdef NTJ_SERVER
	if (!pTimerTriggerPetStudy->ServerSendStartMsg())
	{
		DeleteFromList(nId);
		return false;
	}
#endif
	return true;
}

void TimerTriggerManager::SwapTimerPetStudyId(U32 nId1, U32 nId2)
{
#ifdef NTJ_SERVER
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER_PET_STUDY);
	GameConnection *conn = m_pPlayer->getControllingClient();
	if (conn && ev)
	{
		ev->SetInt32ArgValues(3, 3, nId1, nId2);
		conn->postNetEvent(ev);
	}
#endif

	TimerTriggerPetStudy *pTimer1 = dynamic_cast<TimerTriggerPetStudy *>(GetTimerTrigger(nId1));
	TimerTriggerPetStudy *pTimer2 = dynamic_cast<TimerTriggerPetStudy *>(GetTimerTrigger(nId2));
	if (pTimer1)
		pTimer1->SetId(nId2);
	if (pTimer2)
		pTimer2->SetId(nId1);
}

void TimerTriggerManager::DeleteFromList(U32 nId)
{
	std::list<TimerTriggerBase *>::iterator it = m_pList.begin();
	while(it != m_pList.end())
	{
		if (*it && (*it)->GetId() == nId)
		{
			delete (*it);
			m_pList.erase(it);
			break;
		}
		++it;
	}
}

bool TimerTriggerManager::AddToList(TimerTriggerBase *pTimerTriggerBase)
{
	if (!pTimerTriggerBase || m_pList.size() == MAX_SIZE)
		return false;

	if (GetTimerTrigger(pTimerTriggerBase->GetId()))
		return false;

	pTimerTriggerBase->SetManager(this);
	m_pList.push_back(pTimerTriggerBase);
	return true;
}

void TimerTriggerManager::ClearList()
{
	std::list<TimerTriggerBase *>::iterator it = m_pList.begin();
	while(it != m_pList.end())
	{
		if (*it)
		{
			(*it)->OnClear();
			delete (*it);
		}
		++it;
	}
	m_pList.clear();
}

bool TimerTriggerManager::Start(U32 nId)
{
	TimerTriggerBase *pTimerTriggerBase = GetTimerTrigger(nId);
	if (!pTimerTriggerBase)
		return false;

	return pTimerTriggerBase->Start();
}

bool TimerTriggerManager::Cancel(U32 nId)
{
	TimerTriggerBase *pTimerTriggerBase = GetTimerTrigger(nId);
	if (!pTimerTriggerBase || pTimerTriggerBase->GetType() != TimerTriggerBase::TypeReward)
		return false;
	
	TimerTrigger *pTimerTrigger = dynamic_cast<TimerTrigger *>(pTimerTriggerBase);
	return pTimerTrigger->Cancel();
}

TimerTriggerBase* TimerTriggerManager::GetTimerTrigger(U32 nId)
{
	std::list<TimerTriggerBase *>::iterator it = m_pList.begin();
	while(it != m_pList.end())
	{
		if (*it && (*it)->GetId() == nId)
		{
			return *it;
		}
		++it;
	}

	return NULL;
}

void TimerTriggerManager::Initialize(stPlayerStruct *pPlayerData)
{
	//根据玩家数据库中的时间触发器初始化列表
	S32 nTriggerCount = pPlayerData->MainData.TimerTriggerCount;
	stTimerTriggerInfo *pTimerTriggerInfo = pPlayerData->MainData.TimerTriggerInfo;
	for (S32 i = 0; i < nTriggerCount; i++)
	{
		TimerTrigger *pTimerTrigger = NULL;
		if (pTimerTriggerInfo[i].m_nState == TimerTrigger::STATE_NONE)
		{
			pTimerTrigger = new TimerTrigger(pTimerTriggerInfo[i].m_Id, pTimerTriggerInfo[i].m_nLeftTime, 
												pTimerTriggerInfo[i].m_nIntervalTime, pTimerTriggerInfo[i].m_bCanCancel,
												true, pTimerTriggerInfo[i].m_nTimeFlag);
		}
		else if (pTimerTriggerInfo[i].m_nState == TimerTrigger::STATE_START)
		{
			U32 nLeftTime = 0;
			if (pTimerTriggerInfo[i].m_nTimeFlag == TimerTrigger::Timer_OnLine)
			{
				nLeftTime = pTimerTriggerInfo[i].m_nLeftTime;
			}
			else if (pTimerTriggerInfo[i].m_nTimeFlag == TimerTrigger::Timer_All)
			{
				//U32 nElapseTime = Platform::getTime() - pPlayerData->BaseData.lastLogoutTime;				
				U32 nElapseTime = 10;		//测试用10秒
				nLeftTime = pTimerTriggerInfo[i].m_nLeftTime / 1000;
				nLeftTime = nElapseTime >= nLeftTime ? 0 : (nLeftTime - nElapseTime) * 1000;
			}

			pTimerTrigger = new TimerTrigger(pTimerTriggerInfo[i].m_Id, nLeftTime, 
				pTimerTriggerInfo[i].m_nIntervalTime, pTimerTriggerInfo[i].m_bCanCancel,
				true, pTimerTriggerInfo[i].m_nTimeFlag);
			pTimerTrigger->SetState((U32)TimerTrigger::STATE_START);
		}

		if(pTimerTrigger)
		{
			for (int j = 0; j < 4; j++)
			{
				U32 nItemId = pTimerTriggerInfo[i].m_nItemId[j];
				if (nItemId)
					pTimerTrigger->m_arrRewardIds.push_back(nItemId);
			}
			pTimerTrigger->SetStartTime(Platform::getRealMilliseconds());
			AddToList(pTimerTrigger);
		}
	}
}


void TimerTriggerManager::InitializePetStudy(stPlayerStruct *pPlayerData)
{
	//根据玩家数据库中的时间触发器初始化列表
	S32 nTriggerCount = pPlayerData->MainData.TimerPetStudyCount;
	stTimerTriggerPetStudyInfo *pTimerTriggerPetStudyInfo = pPlayerData->MainData.TimerPetStudyInfo;
	for (S32 i = 0; i < nTriggerCount; i++)
	{		
		if (pTimerTriggerPetStudyInfo[i].m_Id == 0)
			continue;

		TimerTriggerPetStudy *pTimerTriggerPetStudy = NULL;		
		U32 nLeftTime = 0;
		if (pTimerTriggerPetStudyInfo[i].m_nTimeFlag == TimerTrigger::Timer_OnLine)
		{
			nLeftTime = pTimerTriggerPetStudyInfo[i].m_nLeftTime;
		}
		else if (pTimerTriggerPetStudyInfo[i].m_nTimeFlag == TimerTrigger::Timer_All)
		{
			//U32 nElapseTime = Platform::getTime() - pPlayerData->BaseData.lastLogoutTime;				
			U32 nElapseTime = 10;		//测试用10秒
			nLeftTime = pTimerTriggerPetStudyInfo[i].m_nLeftTime / 1000;
			nLeftTime = nElapseTime >= nLeftTime ? 0 : (nLeftTime - nElapseTime) * 1000;
		}

		pTimerTriggerPetStudy = new TimerTriggerPetStudy(
										pTimerTriggerPetStudyInfo[i].m_Id, nLeftTime,
										pTimerTriggerPetStudyInfo[i].m_nTotalTime,
										pTimerTriggerPetStudyInfo[i].m_nIntervalTime,
										pTimerTriggerPetStudyInfo[i].m_nTimeFlag);

		if(pTimerTriggerPetStudy)
		{
			pTimerTriggerPetStudy->SetStartTime(Platform::getRealMilliseconds());
			pTimerTriggerPetStudy->SetStudyDataId(pTimerTriggerPetStudyInfo[i].m_nStudyDataId);
			AddToList(pTimerTriggerPetStudy);
		}
	}
}

void TimerTriggerManager::SaveData(stPlayerStruct *pPlayerData)
{
	std::list<TimerTriggerBase *>::iterator it = m_pList.begin();

	S32 i = 0;
	while(it != m_pList.end())
	{
		if (*it)
		{
			(*it)->SaveData(pPlayerData, i);
			++i;
		}
				
		++it;
	}

	pPlayerData->MainData.TimerTriggerCount = i;
}

void TimerTriggerManager::SavePetStudyData(stPlayerStruct *pPlayerData)
{
	std::list<TimerTriggerBase *>::iterator it = m_pList.begin();

	S32 i = 0;
	while(it != m_pList.end())
	{
		if (*it)
		{
			(*it)->SaveData(pPlayerData, i);
			++i;
		}
		++it;
	}

	pPlayerData->MainData.TimerPetStudyCount = i;
}

void TimerTriggerManager::SetPlayer(Player *pPlayer)
{
	m_pPlayer = pPlayer;
}

Player* TimerTriggerManager::GetPlayer()
{
	return m_pPlayer;
}

#ifdef NTJ_CLIENT
S32	TimerTriggerManager::GetFreeCtrlIndex(U32 nId)
{
	for (S32 i = 1; i <= 4; i++)
	{
		std::list<TimerTriggerBase *>::iterator it = m_pList.begin();
		bool bFree = true;
		while(it != m_pList.end())
		{
			TimerTrigger *pTimerTrigger = dynamic_cast<TimerTrigger *>(*it);

			if (pTimerTrigger)
			{
				if (pTimerTrigger->GetId() == nId)
				{
					++it;
					continue;					
				}
				if ( pTimerTrigger->m_ctrlIndex == i)
				{
					bFree = false;
					break;
				}
				++it;
			}
			else
				AssertFatal(false, "TimerTriggerManager::GetFreeCtrlIndex Node has not been freed!");
		}

		if (bFree)
			return i;
	}

	return 0;
}

TimerTrigger* TimerTriggerManager::GetTimerTriggerByIndex(S32 nCtrlIndex)
{
	if (nCtrlIndex < 1 || nCtrlIndex > 4)
		return NULL;

	std::list<TimerTriggerBase *>::iterator it = m_pList.begin();
	while(it != m_pList.end())
	{
		TimerTrigger *pTimerTrigger = dynamic_cast<TimerTrigger *>(*it);

		if (pTimerTrigger && pTimerTrigger->m_ctrlIndex == nCtrlIndex)
			return pTimerTrigger;
		++it;
	}

	return NULL;
}
#endif

#ifdef NTJ_SERVER
/*
AddTimerControl（
		$index			//计时器索引
		$elapseTime		//计时触发时间
		$intervalTime	//事件触发的间隔时间【计时单位】
		$CancelFlag		//计时功能可否取消
		$storageFlag	//存储行为【是否存储到服务器】
		$timeFlag		//计时行为（存储行为影响下线计时相关）【在线计时、下线计时、全部计时】
）*/


ConsoleMethod(Player, AddTimerControl, void, 8, 8, "")
{
	U32		nId				= atoi(argv[2]);
	U32		nLeftTime		= atoi(argv[3]);
	U32		nIntervalTime	= atoi(argv[4]);
	bool	bCancelFlag		= atoi(argv[5]);
	bool	bSaveFlag		= atoi(argv[6]);
	U32		nTimeFlag		= atoi(argv[7]);

	if (!object->mTimerTriggerMgr)
		return;

	TimerTrigger *pTimerTrigger = new TimerTrigger(nId, nLeftTime, nIntervalTime, bCancelFlag, bSaveFlag, nTimeFlag);
	if (!pTimerTrigger)
		return;

	if ( object->mTimerTriggerMgr->AddToList(pTimerTrigger) )
	{
		//发送此TimerTrigger到客户端
		ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER);
		GameConnection *conn = object->getControllingClient();
		if (ev)
		{
			ev->SetInt32ArgValues(5, 1, nId, nLeftTime, nIntervalTime, nTimeFlag);  //1表示生成计时器
			ev->SetIntArgValues(2, bCancelFlag, bSaveFlag);	
			conn->postNetEvent(ev);
		}
	}
	else
	{
		delete pTimerTrigger;
	}
}

/*
SetTimerControl(
		$index			//要启动的计时器索引
		$timePara		//启动、取消   /////暂停、继续
)*/
ConsoleMethod(Player, SetTimerControlLeftTime, void, 4, 4, "%player.SetTimerControlLeftTime(%nId, %nLeftTime);")
{
	U32 nId = atoi(argv[2]);
	U32 nLeftTime = atoi(argv[3]);

	TimerTrigger *pTimerTrigger = dynamic_cast<TimerTrigger*>(object->mTimerTriggerMgr->GetTimerTrigger(nId));
	if (!pTimerTrigger)
		return;

	pTimerTrigger->SetLeftTime(nLeftTime);
	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER);
	GameConnection *conn = object->getControllingClient();
	if (ev && conn)
	{
		ev->SetInt32ArgValues(3, 4, nId, nLeftTime);	//发送消息通知客户端启动计时器
		conn->postNetEvent(ev);
	}
}

ConsoleMethod(Player, SetTimerControl, void, 5, 5, "")
{
	U32 nId = atoi(argv[2]);
	U32 nTimePara = atoi(argv[3]);
	StringTableEntry sIds = argv[4];

	TimerTrigger *pTimerTrigger = dynamic_cast<TimerTrigger*>(object->mTimerTriggerMgr->GetTimerTrigger(nId));
	if (!pTimerTrigger)
		return;

	if (nTimePara == 1)				//启动
	{
		if ( pTimerTrigger->Start() )
		{
			//发送消息通知客户端开始计时
			ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER);
			GameConnection *conn = object->getControllingClient();
			if (ev && conn)
			{
				// 发送物品列表
				TimerTrigger::FillIdArrayFromString(sIds, pTimerTrigger->m_arrRewardIds);
				S32 count = pTimerTrigger->m_arrRewardIds.size();
				ev->SetInt32ArgValues(3, 2, nId, count);		//发送消息通知客户端启动计时器
				for (S32 i = 0; i < count; i++)
				{
					U32 nItemId = pTimerTrigger->m_arrRewardIds[i];
					ev->AddMemoryBlock((char *)&nItemId, sizeof(U32));
				}
				conn->postNetEvent(ev);
			}
		}
	}
	else if (nTimePara == 2)		//取消
	{	
		if ( pTimerTrigger->Cancel() )
		{
			//发送消息通知客户端取消计时
			ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_TIMER_TRIGGER);
			GameConnection *conn = object->getControllingClient();
			if (ev && conn)
			{
				ev->SetInt32ArgValues(2, 3, nId);		//发送消息通知客户端取消计时器
				conn->postNetEvent(ev);
			}
		}
	}
}

ConsoleMethod(Player, GetTimerControl, S32, 3, 3, "")
{
	//返回值整型(0-3)，分别表示不存在、存在未启动、正在计时
	U32 nId = atoi(argv[2]);
	TimerTrigger *pTimerTrigger = dynamic_cast<TimerTrigger*>(object->mTimerTriggerMgr->GetTimerTrigger(nId));
	if (!pTimerTrigger)
		return 0;			//不存在
	
	TimerTrigger::TimerTriggerState state = pTimerTrigger->GetState();
	if (state == TimerTrigger::STATE_NONE)
		return 1;			//未启动
	else if (state == TimerTrigger::STATE_START)
		return 2;			//已启动，正在计时
	else 
		return 3;			//已停止或被取消(不应该在这里返回)
}
#endif

#ifdef NTJ_CLIENT
ConsoleFunction(SendCancelTimerControl, void, 2, 2, "SendCancelTimerControl(%controlIndex);")
{
	S32 nCtrlIndex = atoi(argv[1]);

	if (nCtrlIndex < 1 || nCtrlIndex > 4)
		return;

	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	TimerTrigger *pTimerTrigger = NULL;
	if ( !pPlayer || !(pTimerTrigger = pPlayer->mTimerTriggerMgr->GetTimerTriggerByIndex(nCtrlIndex)) )
		return;

	if (!pTimerTrigger->CanCancel())
		return;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_TIMER_TRIGGER);
	GameConnection *conn = pPlayer->getControllingClient();
	if (ev && conn)
	{
		ev->SetInt32ArgValues(2, 1, pTimerTrigger->GetId());		//服务端取消为1
		conn->postNetEvent(ev);
	}
}
#endif