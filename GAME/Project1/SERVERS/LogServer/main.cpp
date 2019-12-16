#include "LogServer.h"
#include "Common/dbStruct.h"

#include "Common/Log/LogCommon.h"

const char* g_genExitEventName(void)
{
    char path[256] = {0};
    GetModuleFileNameA(0,path,sizeof(path));

    static std::string eventName;

    eventName = "e_";

    char buf[256] = {0};
    _splitpath(path,0,0,buf,0);
    eventName += buf;

    itoa(GetCurrentProcessId(),buf,10);
    eventName += buf;

    return eventName.c_str();
}

void main()
{
	CLogServer::getInstance()->init( "LogServer", g_genExitEventName() );

	CLogServer::getInstance()->main();

	CLogServer::getInstance()->shutdown();
}		  