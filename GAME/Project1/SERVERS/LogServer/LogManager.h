#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include "LogServer.h"
#include "Common/dbStruct.h"
#include "Common/ServerFramwork.h"

class CLogManager : public IServerModule< CLogServer, MSG_LOG_BEGIN, MSG_LOG_END > 
{
public:
	
}; 

#endif