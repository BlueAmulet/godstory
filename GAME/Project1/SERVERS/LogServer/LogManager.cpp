#include "LogManager.h"
#include "Common/dbStruct.h"
//#include "Common/Log/LogTypes.h"
#include "LogServer.h"

ServerEventFunction( CLogManager, HandleLogRecordRequest, ANY_LOGSERVER_Record )
{
	char szName[256] = { 0 };
	pPack->readString( szName,256 );

	dbStruct* pStruct = CLogInfoContainer::Instance()->Alloc(szName);

    if (0 == pStruct)
        return false;

    pStruct->unpack(*pPack);

    if (CLogServer::getInstance()->getDBConnPool())
        dbStructDAO::getInstance()->insert(pStruct);
    
    CLogInfoContainer::Instance()->Release(pStruct);
    return true;
}