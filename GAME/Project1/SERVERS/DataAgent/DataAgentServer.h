#ifndef DATA_AGENT_SERVER_H
#define DATA_AGENT_SERVER_H

class CDataAgent;
class CPlayerManager;

#include "Common/ServerFramwork.h"
#include "Common/TimerMgr.h"
#include "wintcp/AsyncSocket.h"
#include "Common/LogHelper.h"

class CDataAgent : public CServerFramework<CDataAgent>
{
public:
	CPlayerManager* getPlayerManager() { return mPlayerManager; }

	void onInit();
	void onShutdown();
	void onMainLoop();

	CAsyncSocket2* getNetGateSocket();

private:
	CPlayerManager* mPlayerManager;
	CAsyncSocket2*	m_pNetGateClient;
	CLogHelper*		mLogHelper;
	HANDLE m_StartEvent;
};

#endif