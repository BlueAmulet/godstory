#include "LocalString.h"
#include <Windows.h>
#include <time.h>
#include <assert.h>
#include "platform/types.h"
#include "core/fileStream.h"
#include "core/resManager.h"

LocalString gLocalString;
LocalString* g_LocalString;

static U8 gMessage[LocalString::STRING_MAXLEN + 1];

LocalString::LocalString()
{
	g_LocalString = this;
}

LocalString::~LocalString()
{
	LOCALSTRINGTABLE::iterator itBegin = mLocalTable.begin();
	for(; itBegin != mLocalTable.end(); ++itBegin)
	{
		if(IsMemMode(itBegin->first))
		{
			U8* pEntry = (U8*)itBegin->second;
			if(pEntry != NULL)
				delete pEntry;
		}

	}
	mLocalTable.clear();
}

// ----------------------------------------------------------------------------
// 装载本地字符串数据
bool LocalString::LoadFile(const char* filename)
{
	if(filename == NULL || *filename == 0)
		return false;

	const char* postfix = strrchr(filename, '.');
	if(postfix != NULL && _stricmp(postfix, ".ini") == 0)
		return LoadFileByTxt(filename);
	else
		return LoadFileByBin(filename);
}

// ----------------------------------------------------------------------------
// 以二进制方式装备本地字符串数据
bool LocalString::LoadFileByBin(const char* filename)
{
	Stream* fp = NULL;
	U8* buf = NULL;
	U8* keyvalue = NULL;
	U8* value = NULL;
	int buf_size = 0;

	fp = ResourceManager->openStream(filename);
	if(fp == NULL)
		return -1;

	int KeyCount = 0;
	fp->read(sizeof(int), &KeyCount);					// 读取总数
	int* KeyTable = new int[KeyCount * 2];
	fp->read(sizeof(int) * KeyCount * 2, KeyTable);		// 读取键记录表
	fp->read(sizeof(int), &buf_size);					// 读取字符串区数据长度
	buf = new U8[buf_size];
	fp->read(buf_size, &buf);							// 读取字符串区数据
	fp->close();

	for(int i = 0; i < KeyCount * 2; i += 2)
	{		
		if(IsMemMode(KeyTable[i]))
		{
			keyvalue = buf + KeyTable[i+1];
			value = new U8[strlen((const char*)keyvalue) + 1];
			strcpy((char*)value, (const char*)keyvalue);
			mLocalTable.insert(LOCALSTRINGTABLE::value_type(KeyTable[i], (char*)value));
		}
		else
			mLocalTable.insert(LOCALSTRINGTABLE::value_type(KeyTable[i], (char*)filename));
	}

	if(KeyTable)
		delete[] KeyTable;
	if(buf)
		delete[] buf;
	if (fp)
		delete fp;
	return false;
}

// ----------------------------------------------------------------------------
// 以文本方式装载本地字符串数据
bool LocalString::LoadFileByTxt(const char* filename)
{
	Stream* fp	= NULL;
	U8* buf			= NULL;
	U8* value		= NULL;
	int nError		= 0;		// 无错误
	int buf_size	= 0;
	size_t len		= 0;
	size_t file_size= 0;
	int key_s, key_e, value_s, value_e;
	U32 skey = 0;
	U8 key[20];

	if(nError == 0)
	{
		fp = ResourceManager->openStream(filename);
		if(fp == NULL)
			nError = -1;
	}	

	// 获取文件长度，分配数据缓冲区大小
	if(nError == 0)
	{
		buf_size = fp->getStreamSize();
		if(buf_size <= 0)
			nError = -2;
	}

	// 从本地读取文件内容到数据缓冲区
	if(nError == 0)
	{
		buf = new U8[buf_size + 1];
		U32 blockCount = (buf_size + STRING_MAXLEN - 1) / STRING_MAXLEN;
		for(U32 i = 0; i < blockCount; ++i)
		{
			U32 len = getMin((U32)STRING_MAXLEN, (U32)(buf_size - (i * STRING_MAXLEN)));
			fp->read(len, buf + file_size);
			file_size += len;
		}
		buf[file_size] = '\0';
		fp->close();
	}

	// 搜索指定Key的键值
	if(nError == 0)
	{
		const U8 *p = buf;
		int i = 0;

		while(p[i] != '\0') 
		{
			// 跳过空格和换行
			if(p[i] == '\0' || isspace(p[i]))
			{
				i++;
				continue;
			}

			int newline_s = i;
			int j = newline_s;
			// 寻找到键值的结束位置
			while( p[i] != '\n' && p[i] != '\r' && p[i] != '\0') 
				i++;

			while(j < i && p[j] != '=') 
			{
				j++;
				if(p[j] == '=') 
				{
					// 处理Key
					{
						key_s = newline_s;
						key_e = j - 1;
						while(isspace(p[key_e]))
							key_e--;

						memset(key, 0, 20);
						memcpy(key, buf + key_s, key_e - key_s + 1);
						skey = atol((const char*)key);
					}					

					if(IsMemMode(skey))
					{
						// 处理Value
						{
							value_s = j + 1;
							value_e = i;
							while(isspace(p[value_s]))
								(value_s)++;

							int count = value_e - value_s - 2;
							value = new U8[count + 1];
							memset(value, 0, count + 1);
							memcpy(value, buf + value_s + 1, count);
							value[count] = '\0';
						}

						if(mLocalTable.find(skey) != mLocalTable.end())
						{
							char szMsg[128];
							dSprintf(szMsg, 128, "本地字符串表有重复键 key = %d", skey);
							AssertWarn(0, szMsg);
							continue;
						}
						mLocalTable.insert(LOCALSTRINGTABLE::value_type(skey, (char*)value));
					}
					else
						mLocalTable.insert(LOCALSTRINGTABLE::value_type(skey, (char*)filename));

				}
			}

			if(p[i] == '\0')
				break;
			i ++;
		}
	}

	if( buf )
		delete[] buf;
	if (fp)
		delete fp;
	return nError;
}

// ----------------------------------------------------------------------------
// 获取本地字符串
const char* LocalString::GetStr(U32 id)
{
	if(IsMemMode(id))
	{
		// 常驻内存模式,从缓冲表里读取
		LOCALSTRINGTABLE::iterator itF = mLocalTable.find(id);
		return (itF != mLocalTable.end()) ? (const char*)(itF->second) : "";
	}
	else // 普通模式
	{
		// 即时模式，每次都从本地磁盘读取
		LOCALSTRINGTABLE::iterator itFind = mLocalTable.find(id);
		if(itFind != mLocalTable.end())
		{
			char* filename = itFind->second;
			if(filename && *filename)
			{
				ReadLocalString(id, gMessage, STRING_MAXLEN, filename);
				return (const char*)gMessage;
			}
		}
		return "";
	}
}

// ----------------------------------------------------------------------------
// 读取本地字符串数据
int LocalString::ReadLocalString( U32 key, U8 *value, int size,  const char *file)
{
	if( key <= 0  || value == NULL || size <= 0 || file == NULL)
		return -2;

	const char* postfix = strrchr(file, '.');
	if(postfix != NULL && _stricmp(postfix, ".ini") == 0)
		return ReadLocalStringByTxt(key, value, size, file);
	else
		return ReadLocalStringByBin(key, value, size, file);
}

// ----------------------------------------------------------------------------
// 文本方式读取本地字符串数据文件，利于编辑，但速度较二进制方式慢
int LocalString::ReadLocalStringByTxt(U32 key, U8* value, int size,  const char* file)
{
	Stream* fp		= NULL;
	U8* buf		= NULL;
	int buf_size	= 0;
	size_t len		= 0;
	size_t file_size= 0;
	int value_s, value_e;
	int nError		= 0;		// 无错误
	U8 szKey[20];

	if(nError == 0)
	{
		fp = ResourceManager->openStream(file);
		if(fp == NULL)
			nError = -1;
	}	

	// 获取文件长度，分配数据缓冲区大小
	if(nError == 0)
	{
		buf_size = fp->getStreamSize();
		if(buf_size <= 0)
			nError = -2;
	}

	// 从本地读取文件内容到数据缓冲区
	if(nError == 0)
	{
		buf = new U8[buf_size + 1];
		U32 blockCount = (buf_size + STRING_MAXLEN - 1) / STRING_MAXLEN;
		for(U32 i = 0; i < blockCount; ++i)
		{
			U32 len = getMin((U32)STRING_MAXLEN, (U32)(buf_size - (i * STRING_MAXLEN)));
			fp->read(len, buf + file_size);
			file_size += len;
		}
		buf[file_size] = '\0';
		fp->close();
	}

	// 搜索指定Key的键值
	if(nError == 0)
	{
		const U8 *p = buf;
		int i = 0;

		while(p[i] != '\0') 
		{
			// 跳过空格和换行
			if(p[i] == '\0' || isspace(p[i]))
			{
				i++;
				continue;
			}

			int newline_s = i;
			int j = newline_s;
			// 寻找到键值的结束位置
			while( p[i] != '\n' && p[i] != '\r' && p[i] != '\0') 
				i++;

			while(j < i && p[j] != '=') 
			{
				j++;
				if(p[j] == '=') 
				{
					int newkey_e = j-1;
					while(isspace(p[newkey_e]))
						newkey_e--;

					memset(szKey, 0, 20);
					memcpy(szKey, buf + newline_s, newkey_e - newline_s + 1);
					U32 skey = atol((const char*)szKey);

					if(skey == key)
					{
						value_s = j + 1;
						value_e = i;
						while(isspace(p[value_s]))
							(value_s)++;

						int count = value_e - value_s - 2;
						if( size - 1 < count)
							count =  size - 1;
						memset(value, 0, size);
						memcpy(value, buf + value_s + 1, count);
						value[count] = '\0';
						goto clear_all;
					}
				}
			}

			if(p[i] == '\0')
				break;
			i ++;
		}
	}

clear_all:
	if(buf)
		delete[] buf;
	if (fp)
		delete fp;
	return nError;
}

// ----------------------------------------------------------------------------
// 二进制方式读取本地字符串数据，速度更快，但不利于随意编辑
int LocalString::ReadLocalStringByBin( U32 key, U8* value, int size, const char* file)
{
	Stream* fp  = NULL;
	U8* buf = NULL;
	int buf_size = 0;

	if( key == 0  || value == NULL || size <= 0 || file == NULL)
		return -2;

	fp = ResourceManager->openStream(file);
	if(fp == NULL)
		return -1;

	int KeyCount = 0;
	fp->read(sizeof(int), &KeyCount);					// 读取总数
	int* KeyTable = new int[KeyCount * 2];
	fp->read(sizeof(int) * KeyCount * 2, KeyTable);		// 读取键记录表
	fp->read(sizeof(int), &buf_size);					// 读取字符串区数据长度
	buf = new U8[buf_size];
	fp->read(buf_size, &buf);							// 读取字符串区数据
	fp->close();

	for(int i = 0; i < KeyCount * 2; i+=2)
	{
		if(KeyTable[i] == key)
		{
			value = buf + KeyTable[i+1];
			delete[] KeyTable;
			delete[] buf;
			return 1;
		}
	}
	
	if(KeyTable)
		delete[] KeyTable;
	if(buf)
		delete[] buf;
	if (fp)
		delete fp;
	return 0;
}