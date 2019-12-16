#ifndef __MYLOG__
#define __MYLOG__

#include <iostream>
#include <sstream>
#include <string>

#include "Locker.h"

class CMyLog
{
	CMyCriticalSection m_cs;

	HANDLE m_File;
	HANDLE m_hCon;
	WORD m_ColorSave;

	int m_Year,m_Mon,m_Day;
	std::string m_LogName;

public:
	CMyLog();
	~CMyLog();

	void SetLogName(std::string name);

	bool FindDir(std::string dir);
	void CreateAFile();
	void GetDate(char *sDate);

	void WriteLog( const char* format, ... );
	void WriteError( const char* format, ... );
	void WriteWarn( const char* format, ... );

	void WriteLog(std::string str);
	void WriteLog(std::stringstream &str);

	void WriteError(std::string str);
	void WriteError(std::stringstream &str);

	void WriteWarn(std::stringstream &str);
	void WriteWarn(std::string str);
};

extern CMyLog g_Log;

#endif