// stdafx.cpp : 只包括标准包含文件的源文件
// ServerManager.pch 将是预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

void GetCurrentPath( CString& str )
{
	static char buf[100];
	GetCurrentDirectoryA( 100, buf );
	static CString strPath = buf;
	str = strPath;
}


CString GetPath( CString strPath )
{
	int index = 0;
	for( int i = 0; i < strPath.GetLength(); i++ )
	{
		if( strPath[i] == '\\' )
			index = i;
	}

	return strPath.Left( index + 1 );
}

CString GetConfigPath( CString strName )
{
	CString strCurrentPath;
	GetCurrentPath( strCurrentPath );
	strCurrentPath += "\\Config.ini";

	char buf[100];
	::GetPrivateProfileString( "Paths", strName, NULL, buf, 100, strCurrentPath );
	CString str = buf;
	return str;
}

