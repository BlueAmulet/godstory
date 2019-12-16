#include "LuaWarp.h"

CLuaWarp::CLuaWarp(void)
{
	L = lua_open();
	if (L == NULL)
		return ;

	//打开所有标准LUA函数库
	luaL_openlibs(L);
 }

CLuaWarp::~CLuaWarp(void)
{
	if (!L)		
		return ;
	lua_close(L);
	L = NULL;	
}

int CLuaWarp::DoString(const char *pBuffer)
{
	return luaL_dostring(L,pBuffer);
}

int CLuaWarp::DoFile(const char * Filename)
{
	int status = -1;
	try
	{
		status = luaL_dofile(L,Filename);
	}
	catch(...)
	{
		return status;
	}

	if (status == 0)
		return 0;
	return status;
}

void CLuaWarp::RegisterCFunction(const char * FuncName , lua_CFunction Func)
{
	if (!IsValid())
		return;

	lua_register(L, FuncName, Func);
}

void CLuaWarp::RegisterCFunction(stLuaFuncList *pFuncs)
{
	while(pFuncs)
	{
		if(pFuncs->name == "" || pFuncs->func == NULL)
			continue;

		lua_register(L, pFuncs->name.c_str(), pFuncs->func);
		pFuncs = pFuncs->pNext;
	}
}

bool CLuaWarp::CallLuaFunction(const char * cFuncName)
{
	if (!IsValid())
		return false;

	lua_getglobal(L, cFuncName); //在堆栈中加入需要调用的函数名
	int error = lua_pcall(L, 0, 0,0);	
	if (error != 0)
		return false;
	
	return	true;
}




