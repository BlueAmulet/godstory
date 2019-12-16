#pragma once

#include <WinSock2.h>
#include <windows.h>

#include <string>

[event_source(native)]
class VirtualConsole
{
public:
	VirtualConsole();
	virtual ~VirtualConsole();



	void printf( const char* format, ... );

	__event void onInput( const char* cmd, int size );

private:
	static void workMethod( void* param );
	void _printf( const char* format, ... );

	void process();
	static const int MaxCmdCount = 20;

	HANDLE mStdInput;
	HANDLE mStdOutput;
	HANDLE mStdError;

	char inbuf[512];
	char rgCmds[MaxCmdCount][512];
	int  iCmdIndex;
	int  inpos;

	std::string mPromote;

	HANDLE mWorkThread;

};