//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gui/core/guiTypes.h"
#include "util/stringUnit.h"
#include "NetWork/UserPacketProcess.h"
#include "NetWork/EventCommon.h"
#include "BuildPrefix.h"
#include "Util/LocalString.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/Data/CallboardData.h"
#include "Gameplay/GlobalEvent/ClientGlobalEvent.h"
#include "UI/guiListBoxAdvCtrl.h"


CClientGlobalEvent g_CClientGlobalEvent;
CClientGlobalEvent::CClientGlobalEvent() : /*mbShowAliveNpc(false),
                                           mbReliveNpc(true),
										   mbShowActiveStart(false),
										   mbShowCurrentDay(false),
										   mbEventNotify(true),
										   mbActivityNotify(true),*/
										   mFirstUpdate(true),
										   mLastUpdateTime(0),
										   mCurrentTime(0)
{
	dMemset( &mLocalTime,0,sizeof(mLocalTime));
	dMemset( mEvents, 0, sizeof( stGlobalEvent ) * stGlobalEvent::MaxEventCount );
	gEventMethod[CLIENT_WORLD_EventResponse] = &CClientGlobalEvent::handleClientWorldEventResponse;
	loadConfigIni();
}

CClientGlobalEvent::~CClientGlobalEvent()
{
	writeCoinfgIni();
}
void CClientGlobalEvent::loadConfigIni()
{
	mbShowAliveNpc    = GetPrivateProfileIntA("CallboardNotify",   "ShowAliveNpc",    0,  GAME_CONFIGINI_FILE);
	//mbReliveNpc       = GetPrivateProfileIntA("CallboardNotify",   "ReliveNpc",       1,  GAME_CONFIGINI_FILE);
	mbShowActiveStart = GetPrivateProfileIntA("CallboardNotify",   "ShowActiveStart", 0,  GAME_CONFIGINI_FILE);
	mbShowCurrentDay  = GetPrivateProfileIntA("CallboardNotify",   "ShowCurrentDay",  0,  GAME_CONFIGINI_FILE);
	//mbEventNotify     = GetPrivateProfileIntA("CallboardNotify",   "EventNotify",     1,  GAME_CONFIGINI_FILE);
	mbActivityNotify  = GetPrivateProfileIntA("CallboardNotify",   "ActivityNotify",  1,  GAME_CONFIGINI_FILE);

}

void CClientGlobalEvent::writeCoinfgIni()
{
	WritePrivateProfileStringA("CallboardNotify", "ShowAliveNpc",    avar("%d",mbShowAliveNpc),   GAME_CONFIGINI_FILE);
	//WritePrivateProfileStringA("CallboardNotify", "ReliveNpc",       avar("%d",mbReliveNpc),      GAME_CONFIGINI_FILE);
	WritePrivateProfileStringA("CallboardNotify", "ShowActiveStart",   avar("%d",mbShowActiveStart),GAME_CONFIGINI_FILE);
	WritePrivateProfileStringA("CallboardNotify", "ShowCurrentDay",  avar("%d",mbShowCurrentDay), GAME_CONFIGINI_FILE);
	//WritePrivateProfileStringA("CallboardNotify", "EventNotify",     avar("%d",mbEventNotify),    GAME_CONFIGINI_FILE);
	WritePrivateProfileStringA("CallboardNotify", "ActivityNotify",  avar("%d",mbActivityNotify), GAME_CONFIGINI_FILE);
}

bool CClientGlobalEvent::updateData()
{
	mCurrentTime = Platform::getRealMilliseconds();
	if((mCurrentTime - mLastUpdateTime) < UpdateInterval)
		return false;

	mLastUpdateTime = mCurrentTime;
	return true;
}


void CClientGlobalEvent::queryEveryday(Platform::LocalTime &lt)
{
	static GuiListBoxAdvCtrl* ctrl = dynamic_cast<GuiListBoxAdvCtrl*>(Sim::findObject("EverydayListBox"));
	if(!ctrl)
		return;

	Vector<CallboardDailyData*>& pData = g_CallboardDailyRepository.getDailyData();
	Vector<CallboardDailyData*>::iterator itr = pData.begin();
	CallboardDailyData* pDaily = NULL;

	for (; itr != pData.end(); ++itr)
	{
		pDaily = *itr;
		if(NULL == pDaily || pDaily->getWeekDay() != 0)
			continue;

		char strCurDay[11] = {0,};
		char strStartDay[11] = {0,};
		char strEndDay[11] = {0,};
		dSprintf(strCurDay,sizeof(strCurDay),"%d-%d-%d",lt.year,lt.month,lt.monthday);
		dMemcpy(strStartDay,pDaily->getStartDataTime().yearday.c_str(),sizeof(strStartDay));
		dMemcpy(strEndDay,pDaily->getEndDataTime().yearday.c_str(),sizeof(strEndDay));

		U16 currentMin = lt.hour * 60 + lt.min;
		U16 startMin = pDaily->getStartDataTime().min;
		U16 endMin = pDaily->getEndDataTime().min;

		bool status = false;        //开放状态
		bool isUpdate = false;      //是否需要更新

		//更新当前记录的状态
		if((strStartDay[0] == '\0' && (startMin == 0 || (currentMin >=startMin && currentMin<=endMin))) 
			|| (dStrcmp(strCurDay,strStartDay) >= 0 && dStrcmp(strCurDay,strEndDay) <= 0 && currentMin >= startMin && currentMin <= endMin))
		{
			status = true;
		}

		//如果不是初始化则检查需要更新的记录
		if(!mFirstUpdate)
		{
			char strKey[8] = {0,};
			dSprintf(strKey,sizeof(strKey),"%d",pDaily->getIndexID());
			std::string temStr = ctrl->getListBoxTagText(strKey);
			bool flage = atoi(temStr.c_str());
			if(status != flage)
			{
				ctrl->removeItem(pDaily->getIndexID());
				isUpdate = true;
			}
		}	

		//当前开启的活动
		bool bActiveStart =  true;
		if(mbShowActiveStart && !status)
			bActiveStart =  false;

		//更新数据到UI
		if((mFirstUpdate || isUpdate) && bActiveStart)
		{
			Con::executef("updateEverydayList",
				Con::getIntArg(pDaily->getIndexID()),
				Con::getIntArg(pDaily->getDegree()),
				pDaily->getCaptionName(),
				pDaily->getStartDataTime().longTime.c_str(),
				pDaily->getEndDataTime().longTime.c_str(),
				Con::getIntArg(pDaily->getLimitLevel()),
				pDaily->getLineName(),
				Con::getIntArg(status));
		}

		// 屏蔽活动提示消息
		if(!mbActivityNotify)
			continue;

		U8 count = pDaily->getNotifyCount();
		if(count>0 && dStrcmp(strCurDay,strStartDay) >= 0 && dStrcmp(strCurDay,strEndDay) <= 0)
		{
			for(int j=0; j<count; j++)
			{
				U16 notfyTime = atoi(StringUnit::getUnit(pDaily->getNotifyTime(),j,"/\n"));
				U16 iMin = currentMin + atoi(StringUnit::getUnit(pDaily->getNotifyTime(),j,"/\n"));
				if(iMin == startMin)
				{
					Con::executef("EverydayStartNotify",
						Con::getIntArg(pDaily->getIndexID()),
						Con::getIntArg(notfyTime));
					break;
				}
			}
		}
	}
}

void CClientGlobalEvent::queryWeekDay(Platform::LocalTime &lt)
{
	static GuiListBoxAdvCtrl* ctrl = dynamic_cast<GuiListBoxAdvCtrl*>(Sim::findObject("EveryweekListBox"));
	if(!ctrl)
		return;

	Vector<CallboardDailyData*>& pData = g_CallboardDailyRepository.getDailyData();
	Vector<CallboardDailyData*>::iterator itr = pData.begin();
	CallboardDailyData* pWeekDay = NULL;

	for (; itr != pData.end(); ++itr)
	{
		pWeekDay = *itr;
		if (NULL == pWeekDay || pWeekDay->getWeekDay() == 0)
			continue;

		U16 currentMin = lt.hour * 60 + lt.min;
		U16 startMin = pWeekDay->getStartDataTime().min;
		U16 endMin = pWeekDay->getEndDataTime().min;
		U8 iWeekDay = pWeekDay->getWeekDay();

		bool status = false;        //开放状态
		bool isUpdate = false;      //是否需要更新

		//更新当前记录的状态
		if((iWeekDay % 7 == lt.weekday) && (startMin == 0 || currentMin >= startMin && currentMin <= endMin))
		{
			status = true;
		}

		//如果不是初始化则检查需要更新的记录
		if(!mFirstUpdate)
		{
			char strKey[8] = {0,};
			dSprintf(strKey,sizeof(strKey),"%d",pWeekDay->getIndexID());
			std::string temStr = ctrl->getListBoxTagText(strKey);
			bool flage = atoi(temStr.c_str());
			if(status != flage)
			{
				ctrl->removeItem(pWeekDay->getIndexID());
				isUpdate = true;
			}
		}

		//是否不限制当天开放的活动(当前开放)
		bool nonCurrentDay =  true;
		if(mbShowCurrentDay && !status && mLocalTime.weekday != iWeekDay % 7 || iWeekDay == 0)
			nonCurrentDay =  false;

		//更新数据到UI
		if((mFirstUpdate || isUpdate) && nonCurrentDay)
		{
			Con::executef("updateWeekDayList",
				Con::getIntArg(pWeekDay->getIndexID()),
				Con::getIntArg(pWeekDay->getDegree()),
				pWeekDay->getCaptionName(),
				Con::getIntArg(pWeekDay->getWeekDay()),
				pWeekDay->getStartDataTime().longTime.c_str(),
				pWeekDay->getEndDataTime().longTime.c_str(),
				Con::getIntArg(pWeekDay->getLimitLevel()),
				pWeekDay->getLineName(),
				Con::getIntArg(status));
		}

		// 屏蔽活动提示消息
		if(!mbActivityNotify)
			continue;

		// 非当天或无时段限制
		if(iWeekDay % 7 != lt.weekday || startMin == 0)
			continue;

		U8 count = pWeekDay->getNotifyCount();
		for(int j=0; j<count; j++)
		{
			U16 notfyTime = atoi(StringUnit::getUnit(pWeekDay->getNotifyTime(),j,",\n"));
			U16 iMin = currentMin + notfyTime;
			if(iMin == startMin)
			{
				Con::executef("WeekDayStartNotify",
					Con::getIntArg(pWeekDay->getIndexID()),
					Con::getIntArg(notfyTime));
				break;
			}
		}
	}
}

void CClientGlobalEvent::queryDaily(Platform::LocalTime &lt)
{
	static GuiListBoxAdvCtrl* ctrl = dynamic_cast<GuiListBoxAdvCtrl*>(Sim::findObject("DayListBox"));
	if(!ctrl)
		return;

	Vector<CallboardMissionData*>& pData = g_CallboardMissionRepository.getMissionData();
	Vector<CallboardMissionData*>::iterator itr = pData.begin();
	CallboardMissionData* pMission = NULL;

	for (; itr != pData.end(); ++itr)
	{
		pMission = *itr;
		if(NULL == pMission)
			continue;

		U8 iWeekDay = pMission->getWeekDay();
		bool status = false;        //开放状态
		bool isUpdate = false;      //是否需要更新

		//更新当前记录的状态
		if (iWeekDay == 0 || iWeekDay % 7 == lt.weekday)
		{
			status = true;
		}

		//如果不是初始化则检查需要更新的记录
		if(!mFirstUpdate)
		{
			char strKey[8] = {0,};
			dSprintf(strKey,sizeof(strKey),"%d",pMission->getIndexID());
			std::string temStr = ctrl->getListBoxTagText(strKey);
			bool flage = atoi(temStr.c_str());
			if(status != flage)
			{
				ctrl->removeItem(pMission->getIndexID());
				isUpdate = true;
			}
		}

		//是否不限制当天开放的活动(当前开放)
		bool nonCurrentDay =  true;
		if(mbShowCurrentDay  && mLocalTime.weekday != iWeekDay % 7 && iWeekDay != 0)
			nonCurrentDay =  false;

		if((mFirstUpdate || isUpdate) && nonCurrentDay)
		{
			Con::executef("updateDailyList",
				Con::getIntArg(pMission->getIndexID()),
				Con::getIntArg(pMission->getMissionID()),
				Con::getIntArg(pMission->getDegree()),
				pMission->getCaptionName(),
				Con::getIntArg(pMission->getWeekDay()),
				Con::getIntArg(pMission->getLimitLevel()),
				Con::getIntArg(pMission->getLoopNum()),
				Con::getIntArg(pMission->isAppend()),
				Con::getIntArg(status));

			// 活动提示消息
			if(mbActivityNotify && status && !mFirstUpdate)
				MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_CALLBOARDDAILYSTART),pMission->getCaptionName()),SHOWPOS_SCRANDCHAT);
		}
	}
}


void CClientGlobalEvent::queryEvents()
{
	char buf[MAX_PACKET_SIZE];

	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );

	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if( !pPlayer )
		return ;

	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_EventRequest, pPlayer->getAccountID(), SERVICE_WORLDSERVER );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}

void CClientGlobalEvent::queryCallboardMessage(Player* pPlayer)
{
	if(!pPlayer || !updateData())
		return;

	Platform::LocalTime lt;
	g_ClientGameplayState->getGameFormatTime(lt);
	if(lt.year == 0)
		return;

	if(lt.min != mLocalTime.min)
	{
		mLocalTime = lt;
		queryEveryday(lt);
		queryWeekDay(lt);
		queryDaily(lt);

		if(mFirstUpdate)
			mFirstUpdate = false;

		Con::executef("updateCallboardTime",Con::getIntArg(lt.weekday),avar("%02d:%02d",lt.hour,lt.min));
	}
}
// 1~99 首领击杀类 ,100~199 全服重要事件类,200~255 预留
void CClientGlobalEvent::handleClientWorldEventResponse( stPacketHead* pHead, Base::BitStream& packet )
{
	Vector<CallboardEventData*>& pData = g_CallboardEventRepository.getEventData();
	CallboardEventData* pEvent = NULL;

	for( int i = 0; i < stGlobalEvent::MaxEventCount; i++ )
	{
		mEvents[i].id = i;
		mEvents[i].time = packet.readInt( 32 );
		mEvents[i].type = packet.readInt( 8 );
		
		pEvent = pData[i];
		if(NULL == pEvent || dStrcmp(pEvent->getMapName(),"") == 0 || (mEvents[i].id +1) != pEvent->getIndexID())
			continue;

		if(mEvents[i].id>0 && mEvents[i].id<100)                //首领击杀类
		{
			bool isUpdate = true;
			if(mbShowAliveNpc && mEvents[i].type == 0)
				isUpdate = false;

			if(isUpdate)
			{
				Con::executef("updateKillNpcList",
					Con::getIntArg(pEvent->getIndexID()),
					Con::getIntArg(pEvent->getNpcID()),
					Con::getIntArg(mEvents[i].type),
					Con::getIntArg(mEvents[i].time),
					pEvent->getMapName());
			}
		}
		else if(mEvents[i].id>99 && mEvents[i].id<200)         //全服重要事件类
		{
			Con::executef("UpDateGlobalEventList",
				Con::getIntArg(pEvent->getIndexID() - 99),
				pEvent->getEventName(),
				Con::getIntArg(mEvents[i].type),
				pEvent->getMapName());
		}
		else
		{
			// 预留
		}
	}
}

// 界面打开初始化事件
ConsoleFunction(QueryCallboardEvent,void,1,1,"QueryCallboardEvent()")
{
	g_CClientGlobalEvent.queryEvents();
}

ConsoleFunction(callboardEveryday,void,1,1,"callboardEveryday()")
{
	Platform::LocalTime lt;
	g_ClientGameplayState->getGameFormatTime(lt);
	if(lt.year == 0)
		return;

	if(!g_CClientGlobalEvent.isFirstUpdate())
		g_CClientGlobalEvent.setFirstUpdate(true);

	g_CClientGlobalEvent.queryEveryday(lt);
	g_CClientGlobalEvent.setFirstUpdate(false);
}

ConsoleFunction(callboardCurrentDay,void,1,1,"callboardCurrentDay()")
{
	Platform::LocalTime lt;
	g_ClientGameplayState->getGameFormatTime(lt);
	if(lt.year == 0)
		return;

	if(!g_CClientGlobalEvent.isFirstUpdate())
		g_CClientGlobalEvent.setFirstUpdate(true);

	g_CClientGlobalEvent.queryWeekDay(lt);
	g_CClientGlobalEvent.queryDaily(lt);

	g_CClientGlobalEvent.setFirstUpdate(false);

}

ConsoleFunction(setActivityNotifyStatus,void,2,2,"setActivityNotifyStatus(%bool)")
{
	g_CClientGlobalEvent.setActivityNotify(atoi(argv[1]));
}

ConsoleFunction(getActivityNotifyStatus,bool,1,1,"getActivityNotifyStatus()")
{
	return g_CClientGlobalEvent.getActivityNotify();
}

ConsoleFunction(setShowAliveNpc,void ,2,2, "setShowAliveNpc(%bool)")
{
	g_CClientGlobalEvent.setShowAliveNpc(atoi(argv[1]));
}

ConsoleFunction(getShowAliveNpc,bool,1,1,"getShowAliveNpc()")
{
	return g_CClientGlobalEvent.getShowAliveNpc();
}

ConsoleFunction(setShowActiveStart,void,2,2,"setShowFixedTime(%bool)")
{
	g_CClientGlobalEvent.setActiveStart(atoi(argv[1]));
}

ConsoleFunction(getShowActiveStart,bool,1,1,"getShowFixedTime")
{
	return g_CClientGlobalEvent.getActiveStart();
}

ConsoleFunction(setShowCurrentDay,void,2,2,"setShowCurrentDay(%bool)")
{
	g_CClientGlobalEvent.setShowCurrentDay(atoi(argv[1]));
}

ConsoleFunction(getShowCurrentDay,bool,1,1,"getShowCurrentDay()")
{
	return g_CClientGlobalEvent.getShowCurrentDay();
}
