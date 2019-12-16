#ifdef D3D_DEBUG_INFO

#include "DXDebugInfoLog.h"
#include "platform/platform.h"
#include "platform/types.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include <Windows.h>


//----------------------------------------------------------------------------------------------------
typedef void (WINAPI* TOutputDebugStringA)( __in_opt LPCSTR lpOutputString );
TOutputDebugStringA gOutputDebugStringA	= OutputDebugStringA;
void WINAPI Detoured_OutputDebugStringA( __in_opt LPCSTR lpOutputString )
{
    if ( lpOutputString && lpOutputString[0] && lpOutputString[0] != '\n' ) // 这里可以修改为更精确的过滤条件
        Con::errorf( "[DX Debug Info] - %s", lpOutputString );

    gOutputDebugStringA( lpOutputString );
}


//--------------------------------------------------------------------------------------------------------------
void *DetourFunc(BYTE *src, const BYTE *dst, const int len)
{
    BYTE *jmp = (BYTE*)malloc(len+5);
    DWORD dwback;

    VirtualProtect(src, len, PAGE_READWRITE, &dwback);

    memcpy(jmp, src, len);	jmp += len;

    jmp[0] = 0xE9;
    *(DWORD*)(jmp+1) = (DWORD)(src+len - jmp) - 5;

    src[0] = 0xE9;
    *(DWORD*)(src+1) = (DWORD)(dst - src) - 5;

    VirtualProtect(src, len, dwback, &dwback);

    return (jmp-len);
}

bool RetourFunc(BYTE *src, BYTE *restore, const int len)
{
    DWORD dwback;

    if(!VirtualProtect(src, len, PAGE_READWRITE, &dwback))	{ return false; }
    if(!memcpy(src, restore, len))							{ return false; }

    restore[0] = 0xE9;
    *(DWORD*)(restore+1) = (DWORD)(src - restore) - 5;

    if(!VirtualProtect(src, len, dwback, &dwback))			{ return false; }

    return true;
}


//----------------------------------------------------------------------------------------------------
bool gEnableDXDebugInfoLog = false;
void EnableDXDebugInfoLog()
{
    if ( !gEnableDXDebugInfoLog )
    {
        HMODULE hMod = LoadLibrary(L"kernel32.dll");
        gOutputDebugStringA = (TOutputDebugStringA)DetourFunc( (BYTE*)GetProcAddress( hMod, "OutputDebugStringA" ), (BYTE*)Detoured_OutputDebugStringA, 5 );
        gEnableDXDebugInfoLog = true;
    }
}

#endif