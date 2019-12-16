// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// ServerManager.pch ����Ԥ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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

