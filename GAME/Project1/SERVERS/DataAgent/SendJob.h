#ifndef SEND_JOB_H
#define SEND_JOB_H

#include "Common/TimerMgr.h"

// 用于發送未成功發送的數據
class CBackupSendJob : 
	public CTimeCriticalObject
{
public:
	bool TimeProcess( bool bExit );
};


// 用于立即發送數據
class CSendJob :
	public CTimeCriticalObject
{
public:
	bool TimeProcess( bool bExit );
};

#endif