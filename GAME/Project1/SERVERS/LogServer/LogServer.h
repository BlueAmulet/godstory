#ifndef LOG_SERVER_H
#define LOG_SERVER_H

#include "dblib/dbLib.h"
#include "Common/ServerFramwork.h"


class CLogManager;

class CLogServer : public CServerFramework<CLogServer>
{
public:
	void onInit();
	void onMainLoop();
	void onShutdown();

	CLogManager* getLogManger();
	CDBConnPool* getDBConnPool();

private:
	CLogManager* mLogManager;
	CDBConnPool* mDBConnPool;
};

#endif