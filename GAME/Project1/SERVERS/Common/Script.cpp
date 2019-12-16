#include "Common/Script.h"

CScript *CScript::m_pInstance = NULL;
stLuaFuncList *CLuaCReg::pAutoRegList = NULL;

CTCCWarper g_TccWapper;


ScriptFunction(Hello)
{
	printf("hello...........;");

	char returnValue[256] = "helloReturnValue";
	RETURN<CStr>::push(L,returnValue);

	return 1;
}
