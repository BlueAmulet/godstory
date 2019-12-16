#include <WinSock2.h>
#include "DataAgentServer.h"

const char* g_genExitEventName(void)
{
    char path[256] = {0};
    GetModuleFileName(0,path,sizeof(path));

    static std::string eventName;

    eventName = "e_";

    char buf[256] = {0};
    _splitpath(path,0,0,buf,0);
    eventName += buf;

    itoa(GetCurrentProcessId(),buf,10);
    eventName += buf;

    return eventName.c_str();
}

void main( int argc, char* argv[] )
{
 	CDataAgent::getInstance()->init( "DataAgent", g_genExitEventName() );

	CDataAgent::getInstance()->main();

	CDataAgent::getInstance()->shutdown();

	return ;
}