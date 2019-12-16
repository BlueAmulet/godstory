#include "WorldServer.h"
#include "ExportScript.h"
#include "Common/Script.h"
#include "PlayerMgr.h"

CTccFunctionBuilder::CTccFunctionBuilder( const char* functionName, void* param )
{
	CSCRIPT->registerSymbol( functionName, param );
}


// SYSTEM FUNCTIONS
//*********************************************************************************************
CScriptExport( printf );

#include "base/Log.h"
CScriptFunction( void, writeLog, const char* message, ... )
{
	char szBuffer[1024];
	va_list vaList;
	va_start( vaList, message );
	vsprintf_s( szBuffer, 1024, message, vaList );
	std::stringstream str;
	str<<szBuffer<<std::endl;
	g_Log.WriteLog( str );
}

void SERVER_CLASS_NAME::executeCmd( const char* cmd )
{
	static std::vector< std::string > argList;
	static std::string cmdString;
	static std::string cmdArg;

	cmdString.clear();

	char* strCmd = (char*)strchr( cmd, ' ' );
	if( strCmd )
	{
		// 得到命令
		for( char* p = (char*)cmd; p != strCmd; p++ )
		{
			if( *p != ' ' )
				cmdString.push_back( *p );
		}

		// 过滤空格
		while( *strCmd == ' ' )
			strCmd++;

		// 得到参数
		strCmd = (char*)strchr( strCmd, ',' );
		while(1)
		{
			char* p = (char*)strchr( strCmd, ',' );

			if( p == NULL )
				break;
			else
			{
				cmdArg.clear();

				for( char* c = strCmd; c != p; c++ )
				{
					if( *c != ' ' )
						cmdArg.push_back( *c );
				}

				strCmd = p + 1;

				// 过滤空格
				while( *strCmd == ' ' )
					strCmd++;

				argList.push_back( cmdArg );
			}
		}

		for( char* p = strCmd; *p != '\0'; p++ )
			if( *p != ' ' )
				cmdArg.push_back( *p );

		argList.push_back( cmdArg );
	}
	else
	{
		cmdString = cmd;
	}

	char buf[128];
	strcpy_s( buf, 128, cmdString.c_str() );
	_strlwr_s( buf, 128 );	

	cmdString = buf;
	CmdFunction funct = CmdFunctionBuilder::getInst()[cmdString];

	argList.clear();

	if( funct != NULL )
		funct( argList );
	else
		printf( "parser error !\n" );
}

CmdFunctionBuilder::CmdFunctionBuilder( const char* name, CmdFunction funct )
{
	char buf[128];
	strcpy_s( buf, 128, name );
	_strlwr_s( buf, 128 );
	std::string strName = buf;
	getInst()[ buf ] = funct;
}

HashCmdFunctionMap& CmdFunctionBuilder::getInst()
{
	static HashCmdFunctionMap inst;
	return inst;

}