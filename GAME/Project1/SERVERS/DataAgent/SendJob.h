#ifndef SEND_JOB_H
#define SEND_JOB_H

#include "Common/TimerMgr.h"

// ���ڰl��δ�ɹ��l�͵Ĕ���
class CBackupSendJob : 
	public CTimeCriticalObject
{
public:
	bool TimeProcess( bool bExit );
};


// ���������l�͔���
class CSendJob :
	public CTimeCriticalObject
{
public:
	bool TimeProcess( bool bExit );
};

#endif