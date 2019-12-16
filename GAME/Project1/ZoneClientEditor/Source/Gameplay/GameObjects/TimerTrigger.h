#ifndef _TIMER_TRIGGER_H_
#define _TIMER_TRIGGER_H_

#include <list>
#include "platform/types.h"

class TimerTriggerBase
{
public:
	enum TimerTriggerType
	{
		TypeReward			= 1,		//��ʱ����
		TypePetStudy		= 2,		//�������м�ʱ
	};

	enum TimeFlagType
	{
		Timer_OnLine		= 1,		//���߼�ʱ
		Timer_All			= 2,		//ȫ����ʱ
	};

	TimerTriggerBase(U32 nId, U32 nLeftTime, U32 nTimeFlag, U32 nInterval, TimerTriggerType nType = TypeReward);
	~TimerTriggerBase();

	virtual bool		ProcessTick			();	//Called by TimerTriggerManager::ProcessTick()
	virtual void		ExecuteEvent		() = 0;
	virtual bool		Start				();
	virtual void		End					() = 0;
	virtual void		RefreshWindow		() = 0;
	virtual void		SaveData			(stPlayerStruct *pPlayerData, S32 nIndex) = 0;
	virtual void		InitData			(stPlayerStruct *pPlayerData, S32 nIndex) = 0;
	virtual bool		SendInitialData		(GameConnection *conn) = 0;
	virtual void		OnClear				() = 0;


	U32					GetTimeFlag			()							{ return m_nTimeFlag; }
	void				SetTimeFlag			(U32 nTimeFlag)				{ m_nTimeFlag = nTimeFlag; }
	void				SetLeftTime			(U32 nLeftTime)				{ m_nLeftTime = nLeftTime; }
	void				SetStartTime		(U32 nStartTime)			{ m_nStartTime = nStartTime; }
	U32					GetStartTime		()							{ return m_nStartTime; }
	U32					GetLeftTime			()							{ return m_nLeftTime; }
	U32					GetInterval			()							{ return m_nIntervalTime; }
	void				SetInterval			(U32 nIntervalTime)			{ m_nIntervalTime = nIntervalTime; }
	TimerTriggerType	GetType				()							{ return m_nType; }
	void				SetType				(TimerTriggerType nType)	{ m_nType = nType; }

	U32					GetId				()							{ return m_Id; }
	void				SetId				(U32 nId)					{ m_Id = nId; }
	void				SetManager			(TimerTriggerManager *mgr);
	TimerTriggerManager*GetManager			();

protected:
	U32					m_nIntervalTime;	//ˢ��ʱ��
	U32					m_nStartTime;		//��ʼʱ��
	U32					m_nLeftTime;		//����ʱ��
	U32					m_nTimeFlag;		//��ʱ��Ϊ
	U32					m_Id;				//��ʱ��ID
	TimerTriggerType	m_nType;			//��ʱ������

	TimerTriggerManager* m_TimerTriggerMgr;
};

class TimerTrigger : public TimerTriggerBase
{
public:
	typedef TimerTriggerBase Parent;

	enum TimerTriggerState
	{
		STATE_NONE		= 0,		//��״̬
		STATE_START		= 1,		//��������ʼ
		STATE_END		= 2,		//������ֹͣ
		STATE_CANCEL	= 3,		//������ȡ��
		STATE_MAX,
	};

	
public:
	TimerTrigger(U32 nId, U32 nLeftTime, U32 nIntervalTime, bool bCanCancel, bool bCanSave, U32 nTimeFlag);
	~TimerTrigger();

	bool				ProcessTick();	//Called by TimerTriggerManager::ProcessTick()
	void				ExecuteEvent();
	bool				Start();
	bool				Cancel				();
	void				End					();
	void				SaveData			(stPlayerStruct *pPlayerData, S32 nIndex);
	void				InitData			(stPlayerStruct *pPlayerData, S32 nIndex);
	bool				SendInitialData		(GameConnection *conn);
	void				OnClear				();
	
	bool				GetCanSave			()							{ return m_bCanSave; }
	void				SetCanSave			(bool bCanSave)				{ m_bCanSave = bCanSave; }
	bool				CanCancel			()							{ return m_bCanCancel; }
	void				SetState			(U32 nState)				{ m_nState = (TimerTriggerState)nState; }
	TimerTriggerState	GetState			()							{ return m_nState; }

	void				RefreshWindow		();

#ifdef NTJ_SERVER
	void				ServerSendEndMsg	();
	void				ServerSendCancelMsg	();
#endif

private:
	bool				m_bCanCancel;		//�ܷ�ȡ��
	TimerTriggerState	m_nState;			//��ǰ״̬
	bool				m_bCanSave;			//�ܷ�洢
	
public:
	static void			FillIdArrayFromString(StringTableEntry sIds, Vector<U32> &arrIds);
	Vector<U32>			m_arrRewardIds;

#ifdef NTJ_CLIENT
	void				SetVisible(bool bVisible);
	StringTableEntry	GetRewardDesc();
	void				InitialControlIcons();
	S32					m_ctrlIndex;
#endif
};

class TimerTriggerPetStudy : public TimerTriggerBase
{
public:
	typedef TimerTriggerBase Parent;

public:
	TimerTriggerPetStudy(U32 nId, U32 nLeftTime, U32 nTotalTime, U32 nIntervalTime, U32 nTimeFlag);
	~TimerTriggerPetStudy();

	bool				ProcessTick			();	//Called by TimerTriggerManager::ProcessTick()
	void				ExecuteEvent		();
	bool				Start				();
	void				End					();
	void				SaveData			(stPlayerStruct *pPlayerData, S32 nIndex);
	void				InitData			(stPlayerStruct *pPlayerData, S32 nIndex);
	bool				SendInitialData		(GameConnection *conn);
	void				OnClear				();

	void				RefreshWindow		();

#ifdef NTJ_SERVER
	bool				ServerSendStartMsg	();
	void				ServerSendEndMsg	();
#endif


	U32					GetStudyDataId		()					{ return m_nStudyDataId; }
	void				SetStudyDataId		(U32 nDataId)		{ m_nStudyDataId = nDataId; }
	U32					GetTotalTime		()					{ return m_nTotalTime; }
	void				SetTotalTime		(U32 nTotalTime)		{ m_nTotalTime = nTotalTime; }

protected:
	U32					m_nStudyDataId;
	U32					m_nTotalTime;
};


class TimerTriggerManager 
{
public:
	enum
	{
		MAX_SIZE = 4,
	};


	TimerTriggerManager();
	~TimerTriggerManager();

	bool				AddToList					(TimerTriggerBase *pTimerTriggerBase);
	void				DeleteFromList				(U32 nId);
	void				ClearList					();

	void				ProcessTick					();			//Called by Player::ProcessTick()
	TimerTriggerBase*	GetTimerTrigger				(U32 nId);
	bool				Start						(U32 nId);
	bool				Cancel						(U32 nId);

	void				Initialize					(stPlayerStruct *pPlayerData);
	void				InitializePetStudy			(stPlayerStruct *pPlayerData);
	void				SaveData					(stPlayerStruct *pPlayerData);
	void				SavePetStudyData			(stPlayerStruct *pPlayerData);

	void				SetPlayer					(Player *pPlayer);
	Player*				GetPlayer					();
	void				SendInitialData				();
	
	
	bool				CreateTimerPetStudy			(U32 nId, U32 nLeftTime,  U32 nTotalTime, U32 nIntervalTime, U32 nTimeFlag, U32 nPetStudyDataId);
	
#ifdef NTJ_CLIENT
	S32					GetFreeCtrlIndex			(U32 nId);
	TimerTrigger*		GetTimerTriggerByIndex		(S32 nCtrlIndex);
#endif

	void				SwapTimerPetStudyId			(U32 nId1, U32 nId2);

private:
	std::list<TimerTriggerBase *> m_pList;
	Player*			m_pPlayer;			
};

#endif