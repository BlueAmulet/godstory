#include "log.h"


CMyLog::CMyLog()
{
	m_hCon= GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO Info;
	GetConsoleScreenBufferInfo( m_hCon, &Info);
	m_ColorSave = Info.wAttributes;

	m_Year=0;
	m_Mon=0;
	m_Day=0;
	m_File = NULL;

	m_LogName = "DefaultLog";
}

void CMyLog::SetLogName(std::string name)
{
	m_LogName = name;
}

void CMyLog::GetDate(char *sDate)
{
	CLocker lock(m_cs);

	SYSTEMTIME systime;
	GetLocalTime(&systime);
	sprintf(sDate,"%d-%d-%d %d:%d:%d",systime.wYear, systime.wMonth, systime.wDay,
		systime.wHour,systime.wMinute,systime.wSecond);
}

bool CMyLog::FindDir(std::string dir)
{
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch;
	hSearch = FindFirstFile(dir.c_str(), &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE){ 
		FindClose(hSearch);
		return false;
	}

	if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
		FindClose(hSearch);
		return true;
	}

	while(1){ 
		if (!FindNextFile(hSearch, &FileData)){
			FindClose(hSearch);
			return false;
		}

		if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			FindClose(hSearch);
			return true;
		}
	} 

	return false;
}

void CMyLog::CreateAFile()
{
	CLocker lock(m_cs);

	SYSTEMTIME systime;
	GetLocalTime(&systime);

	if(m_Year!=systime.wYear || m_Mon!=systime.wMonth || m_Day!=systime.wDay){
		m_Year	=	systime.wYear;
		m_Mon	=	systime.wMonth;
		m_Day	=	systime.wDay ;

		if(m_File)
			CloseHandle(m_File);

		bool dirFlag = true;
		if(!FindDir("Log")){
			if(!CreateDirectory("Log",NULL)){
				dirFlag = false;
			}
		}

		char Filename[512];
		if(dirFlag)
			sprintf(Filename,"Log\\%s%d%d%d.log",m_LogName.c_str(),m_Year,m_Mon,m_Day);
		else
			sprintf(Filename,"%s%d%d%d.log",m_LogName.c_str(),m_Year,m_Mon,m_Day);

		m_File = CreateFile(Filename,FILE_APPEND_DATA,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	}
}

CMyLog::~CMyLog()
{
	if(m_File)
		CloseHandle(m_File);
}


void CMyLog::WriteLog(std::string str)
{
	CLocker lock(m_cs);

	char Date[100];
	GetDate(Date);
	std::string TimeStr = Date;
	TimeStr += " ";
	TimeStr += str;
	std::cout << TimeStr << std::endl;

	TimeStr += "\r\n";

	CreateAFile();
	DWORD size;
	WriteFile(m_File,TimeStr.c_str(),TimeStr.length(),&size,NULL);
}

void CMyLog::WriteLog(std::stringstream &str)
{
	CLocker lock(m_cs);

	char Date[100];
	GetDate(Date);
	std::string TimeStr = Date;
	TimeStr += " ";
	TimeStr += str.str();
	std::cout << TimeStr << std::endl;

	TimeStr += "\r\n";
	CreateAFile();
	DWORD size;
	WriteFile(m_File,TimeStr.c_str(),TimeStr.length(),&size,NULL);

	str.seekp(0,std::ios::beg);
}

void CMyLog::WriteLog( const char* format, ... )
{
	char buf[1024];
	va_list args;
	va_start( args, format );
	vsprintf_s( buf, 1024, format, args );
	va_end( args );
	WriteLog( std::string( buf ) );
}

void CMyLog::WriteError(std::string str)
{
	CLocker lock(m_cs);

	// 设置黄色字符前景色
	DWORD Attributes = m_ColorSave;

	Attributes &= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY; 
	Attributes |= FOREGROUND_RED | FOREGROUND_INTENSITY; 

	SetConsoleTextAttribute( m_hCon, Attributes );

	str += "<--------------错误" ;
	WriteLog(str);

	SetConsoleTextAttribute( m_hCon, m_ColorSave );
}

void CMyLog::WriteError(std::stringstream &str)
{
	CLocker lock(m_cs);

	// 设置黄色字符前景色
	DWORD Attributes = m_ColorSave;

	Attributes &= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY; 
	Attributes |= FOREGROUND_RED | FOREGROUND_INTENSITY; 

	SetConsoleTextAttribute( m_hCon, Attributes );

	str << "<--------------错误" ;
	WriteLog(str);

	SetConsoleTextAttribute( m_hCon, m_ColorSave );
}

void CMyLog::WriteError( const char* format, ... )
{
	char buf[1024];
	va_list args;
	va_start( args, format );
	vsprintf_s( buf, 1024, format, args );
	va_end( args );
	WriteError( std::string( buf ) );
}
void CMyLog::WriteWarn(std::string str)
{
	CLocker lock(m_cs);

	// 设置黄色字符前景色
	DWORD Attributes = m_ColorSave;

	Attributes &= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY; 
	Attributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; 

	SetConsoleTextAttribute( m_hCon, Attributes );

	//cout << str << endl;
	str += "<--------------警告" ;
	WriteLog(str);

	SetConsoleTextAttribute( m_hCon, m_ColorSave );
}
void CMyLog::WriteWarn(std::stringstream &str)
{
	CLocker lock(m_cs);

	// 设置黄色字符前景色
	DWORD Attributes = m_ColorSave;

	Attributes &= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY; 
	Attributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; 

	SetConsoleTextAttribute( m_hCon, Attributes );

	//cout << str.str() << endl;
	//str.seekp(0,ios::beg);
	str << "<--------------警告";
	WriteLog(str);

	SetConsoleTextAttribute( m_hCon, m_ColorSave );
}

void CMyLog::WriteWarn( const char* format, ... )
{
	char buf[1024];
	va_list args;
	va_start( args, format );
	vsprintf_s( buf, 1024, format, args );
	va_end( args );
	WriteWarn( std::string( buf ) );
}

#pragma warning( disable : 4073)
#pragma init_seg (lib)
CMyLog g_Log;