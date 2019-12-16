#include <string.h>
#include "base/bitStream.h"
#include <hash_map>
#include <string>
#include "LogServer.h"
#include "LogManager.h"

#include "Common/Log/LogTypes.h"
#include "Common/version.h"


void CLogServer::onInit()
{
	mLogManager = new CLogManager();
	mDBConnPool = new CDBConnPool();

	printf( "Build Version : %s", FULL_VERSION );

	mDBConnPool->Open( getScript()->get<int>("MaxDBConnection"), getScript()->get<const char*>("DBServer"), getScript()->get<const char*>("DBUser"), getScript()->get<const char*>("DBPwd"), getScript()->get<const char*>("DBName") );
	dbStructDAO::getInstance()->attachDbPool( mDBConnPool );
}

void CLogServer::onMainLoop()
{

}

void CLogServer::onShutdown()
{
	SAFE_DELETE( mLogManager );
}

CLogManager* CLogServer::getLogManger()
{
	return mLogManager;
}

CDBConnPool* CLogServer::getDBConnPool()
{
	return mDBConnPool;
}