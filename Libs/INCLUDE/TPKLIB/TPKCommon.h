/********************************************************************************/
/* TPKCommon.h																	*/
/*------------------------------------------------------------------------------*/
/* TPK文件系统库的内部共用的包含文件											*/
/*------------------------------------------------------------------------------*/
/*	   时间      版本     作者    备注											*/
/* -----------  ------   ------  -------										*/
/* 2008.12.18   1.00     Daniel													*/
/********************************************************************************/
#ifndef __TPKCOMMON_H__
#define __TPKCOMMON_H__

//-----------------------------------------------------------------------------
// 工具函数
int   PrepareCryptBuffer();

// 加解密HashTable
void  EncryptHashTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength);
void  DecryptHashTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength);

// 加解密BlockTable
void  EncryptBlockTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength);
void  DecryptBlockTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength);

DWORD DetectFileSeed(DWORD* block, DWORD decrypted);
DWORD DetectFileSeed2(DWORD* block, UINT nDwords, ...);

// 加解密文件数据块
void  EncrypTPKBlock(DWORD* pdwBlock, DWORD dwLength, DWORD dwSeed1);
void  DecrypTPKBlock(DWORD* pdwBlock, DWORD dwLength, DWORD dwSeed1);

DWORD DecryptHashIndex(TPKArchive* ha, const char* szFileName);
DWORD DecryptName1    (const char* szFileName);
DWORD DecryptName2    (const char* szFileName);
DWORD DecryptFileSeed (const char* szFileName);

// 根据文件名，从文件包获取对应的Hash项
TPKHash* GetHashEntry  (TPKArchive* ha, const char* szFileName);
// 根据文件名，从文件包获取空闲的Hash项
TPKHash* FindFreeHashEntry(TPKArchive* ha, const char* szFileName);

// 判断文件包对象是否有效
BOOL IsValidTPKHandle(TPKArchive* ha);
// 判断文件对象是否有效
BOOL IsValidFileHandle(TPKFile* hf);

// 添加文件到文件包内(仅增加Hash项、Block项和文件列表项而已)
int  AddInternalFile(TPKArchive* ha, const char* szFileName);

// 将一个本地文件添加到文件包中
int  AddFileToArchive(TPKArchive* ha, HANDLE hFile, const char* szArchivedName, DWORD dwFlags, BOOL* pbReplaced);
// 保存TPK文件包的Header、HashTable和BlockTable的数据到本地
int  SaveTPKTables(TPKArchive* ha);
DWORD SaveTPKTablesForAddFile(TPKArchive* ha, LARGE_INTEGER& dwNewHashTablePos);
// 释放TPK文件包对象数据
void FreeTPKArchive(TPKArchive *& ha);
// 释放TPK文件对象数据
void FreeTPKFile(TPKFile *& hf);

DWORD ExtendBlockTable(TPKArchive* ha);

// 匹配字符串方法
BOOL CheckWildCard(const char* szString, const char* szWildCard);
// 递归创建目录
void EnsureDirectory(const char* path);

// 解除文件不可写属性
void ClearFileAttributes(const char* szString);

extern COMPACTCB CompactCB;
extern void*	 lpUserData;

//-----------------------------------------------------------------------------
// 文件列表操作函数
int  TPKListFileCreateList(TPKArchive* ha);
int  TPKListFileCreateNode(TPKArchive* ha, const char* szFileName);
int  TPKListFileRemoveNode(TPKArchive* ha, const char* szFileName);
void TPKListFileFreeList(TPKArchive* ha);
int  TPKListFileSaveToTPK(TPKArchive* ha);

template<typename T>
struct TempAlloc
{
	T* ptr;
	DWORD size;
	TempAlloc(DWORD _size):size(_size) { ptr = (T*)malloc(size * sizeof(T)); }
	~TempAlloc() { if( ptr ) free( ptr );}
	operator T*() {	return ptr;	}
};

#endif // __TPKCOMMON_H__

