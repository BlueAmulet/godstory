#undef _STRSAFE_H_INCLUDED_
#include "DumpHelper.h"
#include <stdlib.h>
#include <shellapi.h>

#define TINY_DUMP MiniDumpNormal
#define MINI_DUMP MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory | MiniDumpWithDataSegs

#define MIDI_DUMP MiniDumpWithPrivateReadWriteMemory |\
				MiniDumpWithDataSegs |\
				MiniDumpWithHandleData |\
				MiniDumpWithFullMemoryInfo |\
				MiniDumpWithThreadInfo |\
				MiniDumpWithUnloadedModules
#define MAXI_DUMP MiniDumpWithFullMemory |\
				MiniDumpWithFullMemoryInfo |\
				MiniDumpWithHandleData |\
				MiniDumpWithThreadInfo |\
				MiniDumpWithUnloadedModules

#define DUMP_TYPE MAXI_DUMP

#ifdef NTJ_CLIENT
#undef DUMP_TYPE
#define DUMP_TYPE MINI_DUMP
#endif

//#pragma comment(lib, "dbghelp.lib")

CDumpHelper g_dumper;

LPTOP_LEVEL_EXCEPTION_FILTER CDumpHelper::DefaultFail = NULL;

LONG WINAPI CDumpHelper::SystemFail( LPEXCEPTION_POINTERS pExceptionPointers )
{
#ifndef NTJ_CLIENT
	MessageBoxA( 0, "由于一个未知的异常导正应用程序异常终止,点击确定产生DUMP文件", "错误", 0 );
#endif

	BOOL bMiniDumpSuccessful; 
	char szPath[MAX_PATH]; 
	char szFileName[MAX_PATH]; 
	char szAppName[MAX_PATH] = { 0 }; // = TEXT("Dump"); 
	DWORD dwBufferSize = MAX_PATH; 
	HANDLE hDumpFile;  
	SYSTEMTIME stLocalTime; 
	MINIDUMP_EXCEPTION_INFORMATION ExpParam; 

	GetLocalTime( &stLocalTime ); 
	GetCurrentDirectoryA( dwBufferSize, szPath ); 

	CreateDirectoryA( "Dumps", NULL ); 

	::GetModuleFileNameA( NULL, szAppName,MAX_PATH );
	char* pChar = strrchr( szAppName, '\\' );
	pChar++;

	StringCchPrintfA( szFileName, MAX_PATH, "Dumps\\%s-%04d%02d%02d-%02d%02d%02d.dmp", 
		pChar,
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond 
		); 
	hDumpFile = CreateFileA(szFileName, GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0); 

	ExpParam.ThreadId = GetCurrentThreadId(); 
	ExpParam.ExceptionPointers = pExceptionPointers; 
	ExpParam.ClientPointers = TRUE; 

	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
		hDumpFile, static_cast<MINIDUMP_TYPE>(DUMP_TYPE), &ExpParam, NULL, NULL); 

    CloseHandle(hDumpFile);

#ifdef NTJ_CLIENT
    //显示CrashReport
    ShellExecuteA(0,"open","CrashReport.exe",szFileName,0,SW_SHOW);
#endif
	
    if (IsDebuggerPresent())
        return EXCEPTION_CONTINUE_SEARCH;
    else
        return EXCEPTION_EXECUTE_HANDLER;
}

CDumpHelper::CDumpHelper()
{
	if (!IsDebuggerPresent())
		DefaultFail = ::SetUnhandledExceptionFilter( (LPTOP_LEVEL_EXCEPTION_FILTER)&SystemFail );
}

void CDumpHelper::reset()
{
	LPTOP_LEVEL_EXCEPTION_FILTER fail = ::SetUnhandledExceptionFilter( (LPTOP_LEVEL_EXCEPTION_FILTER)&SystemFail );
	
	if( fail != (LPTOP_LEVEL_EXCEPTION_FILTER)&SystemFail )
	{
		DefaultFail = fail;
	}
}
