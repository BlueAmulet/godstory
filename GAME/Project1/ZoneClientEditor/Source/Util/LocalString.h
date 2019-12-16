#ifndef __LOCALSTRINGMANAGE_H__
#define __LOCALSTRINGMANAGE_H__
#pragma once
#include "platform/platform.h"
#include <hash_map>

// ============================================================================
// �����ַ���������
// ============================================================================
class LocalString
{
public:
	enum Constants
	{
		STRING_MAXLEN	= 4096,		// �ַ�����󳤶�

		MODE_MEMORY		= 1,		// ��פģʽ(��פ���ڴ棩���ַ����������[1-9999]
									// ͨ������ϵͳ��ʾ��������Ϣ��

		MODE_NORMAL		= 2,		// ��ʱģʽ��ÿ�ζ��ӱ��ض�ȡ),�ַ����������[10000-MAX]
	};
	typedef stdext::hash_map<U32, char*> LOCALSTRINGTABLE;
public:
	LocalString();
	~LocalString();

	bool LoadFile(const char* filename);
	const char* GetStr(U32 id);

private:	
	bool IsMemMode(U32 id) { return (id >= 1 && id <= 9999) ? true : false;}
	int ReadLocalString(U32 key, U8 *value, int size,  const char *file);
	int ReadLocalStringByTxt(U32 key, U8* value, int size,  const char* file);
	int ReadLocalStringByBin( U32 key, U8* value, int size, const char* file);
	bool LoadFileByTxt(const char* filename);
	bool LoadFileByBin(const char* filename);
private:
	LOCALSTRINGTABLE	mLocalTable; // �����ַ�����
};

extern LocalString* g_LocalString;
#define GetLocalStr g_LocalString->GetStr
#endif// __LOCALSTRINGMANAGE_H__