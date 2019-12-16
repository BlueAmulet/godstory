#ifndef __LOCALSTRINGMANAGE_H__
#define __LOCALSTRINGMANAGE_H__
#pragma once
#include "platform/platform.h"
#include <hash_map>

// ============================================================================
// 本地字符串管理类
// ============================================================================
class LocalString
{
public:
	enum Constants
	{
		STRING_MAXLEN	= 4096,		// 字符串最大长度

		MODE_MEMORY		= 1,		// 常驻模式(常驻在内存），字符串索引编号[1-9999]
									// 通常用于系统提示，警告消息等

		MODE_NORMAL		= 2,		// 即时模式（每次都从本地读取),字符串索引编号[10000-MAX]
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
	LOCALSTRINGTABLE	mLocalTable; // 本地字符串表
};

extern LocalString* g_LocalString;
#define GetLocalStr g_LocalString->GetStr
#endif// __LOCALSTRINGMANAGE_H__