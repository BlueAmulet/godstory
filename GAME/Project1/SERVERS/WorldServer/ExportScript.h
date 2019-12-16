#ifndef EXPORT_SCRIPT_H
#define EXPORT_SCRIPT_H

#include "TCCLib/TCCWapper.h"
#include "Common/Script.h"

#include <string>
#include <hash_map>
#include <vector>

#define CSCRIPT (&g_TccWapper)

#ifndef CStr
#define CStr const char*
#endif

struct CTccFunctionBuilder
{
	CTccFunctionBuilder( const char* functionName, void* param );
};

// CScriptFunction用法: CScriptFunction( 返回类型, 名称, [参数列表] )
#define CScriptFunction( _ret, _name, ... ) \
	_ret C_API _name( __VA_ARGS__ ); \
	CTccFunctionBuilder _name##builder( #_name, _name ); \
	_ret C_API _name( __VA_ARGS__ )

#define CScriptExport( _name ) \
	CTccFunctionBuilder _name##builder( #_name, _name );

typedef void ( *CmdFunction )( std::vector< std::string>& args );
typedef stdext::hash_map< std::string , CmdFunction > HashCmdFunctionMap;

struct CmdFunctionBuilder
{
	CmdFunctionBuilder( const char* name, CmdFunction funct );

	static HashCmdFunctionMap& getInst();
};

#define CmdFunction( name ) \
	static void cmd##name( std::vector< std::string>& args ); \
	static CmdFunctionBuilder name##builder( #name, cmd##name ); \
	static void cmd##name( std::vector< std::string>& args )


#endif

