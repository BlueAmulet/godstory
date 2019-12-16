/********************************************************************************/
/* TPKLib.h																		*/
/*------------------------------------------------------------------------------*/
/* TPK文件系统库用于提供给外部用户使用的包含文件								*/
/*------------------------------------------------------------------------------*/
/*	   时间      版本		作者    备注										*/
/* -----------  ------		------  -------										*/
/* 2008.12.18   01.00.00    Daniel												*/
/*							1.改进文件包读取文件和新增文件的IO操作性能			*/
/*							2.支持文件包压缩与加密功能							*/
/*							3.支持文件包可能大于4G的情况						*/
/*							4.类似WIN32文件IO函数的参数接口						*/
/*							6.不支持文件包内单个文件大小大于4G的情况			*/
/*							7.不支持文件包内文件无限制增长（受限于HashTable大小）*/
/* 2009.02.23	01.00.01	Daniel												
/*							1.支持文件包变动文件内容的次数无限制(BlockTable大小可增长)*/
/*							2.支持文件包写入操作前断电保护						*/
/********************************************************************************/

#ifndef __TPKLIB_H__
#define __TPKLIB_H__

#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif

#include <assert.h>
#include <stdio.h>
#include <windows.h>

#define TPK_FILEID				0x1A4B5054		// TPK文件包标识  =>TPK
#define TPK_VERSION				0x1B010000		// TPK文件包版本  =>01.00.00
#define TPK_DATABLOCK_SIZE		4096			// TPK文件包内所有文件内容的数据块的存储大小

#define TPK_LISTFILE_NAME		"(listfile)"    // TPK文件包内的文件列表名
#define TPK_VERFILE_NAME		"(version)"		// TPK文件包内的版本更新文件（不是必需的）
#define TPK_HASHTABLE_KEY		"<hash_table>"	// HashTable加密密钥
#define TPK_BLOCKTABLE_KEY		"<block_table>"	// BlockTable加密密钥
#define HASH_TABLE_SIZE_MIN		0x00004			// HashTable的总项数的下限
#define HASH_TABLE_SIZE_MAX		0x40000			// HashTable的总项数的上限
#define DATA_ENTRY_DELETED      0xFFFFFFFE		// 数据项被删除
#define DATA_ENTRY_FREE         0xFFFFFFFF		// 数据项空闲，未使用
#define LISTFILE_CACHE_SIZE		0x1000			// 缺省的文件字符串列表缓冲区大小

// 提供给TPKOpenFile方法的参数值
#define TPK_OPEN_FROM_TPK		0				// 从文件包里打开文件
#define TPK_OPEN_BY_INDEX		1				// 文件名参数'szFileName'其实是一个dwBlockIndex值
#define TPK_OPEN_LOCAL_FILE		(DWORD)-1		// 打开本地文件

// 设置文件包一些特定操作标识
#define TPK_FLAG_CHANGED		0x00000001		// 有此标志说明文件包已经有改动
#define TPK_FLAG_PROTECTED      0x00000002		// 有此标志说明需要做更多的严格数据检查
#define TPK_FLAG_NOLISTFILE		0x00000004		// 有此标志说明打开文件包时不读取文件列表(用于TPKOpenArchive)

enum TPK_FILEOP
{
	TPK_FILE_COMPRESSED			= 0x00000100,	// 有此标志说明文件需要压缩操作或已经压缩过
	TPK_FILE_ENCRYPTED			= 0x00000200,	// 有此标志说明文件已经加密过
	TPK_FILE_FIXSEED			= 0x00001000,	// 有此标志说明文件加密种子非固定值（可能是一个计算后的值）
	TPK_FILE_SINGLE_UNIT		= 0x00010000,	// 有此标志说明文件数据是整块存储
	TPK_FILE_BREAKPROTECTED		= 0x00100000,	// 有此标志说明文件数据需要断电保护
	TPK_FILE_EXISTS				= 0x80000000,	// 有此标志说明Block项指定的文件数据内容是存在的（在文件被删除时，会被重置）
	TPK_FILE_REPLACEEXIST		= 0x80000000,	// 有此标志说明文件如果已经在文件包里存在，需要被替换(TPKAddFile)
	TPK_FILE_VALID_FLAGS		= (TPK_FILE_COMPRESSED|TPK_FILE_ENCRYPTED|TPK_FILE_FIXSEED|TPK_FILE_SINGLE_UNIT|TPK_FILE_EXISTS)
};

// 提供给TPKGetFileInfo用的一系列枚举值
enum TPK_INFO
{
	TPK_INFO_HASH_TABLE_SIZE,					// 文件包HashTable的大小
	TPK_INFO_HASH_TABLE,						// 文件包HashTable对象
	TPK_INFO_BLOCK_TABLE_SIZE,					// 文件包BlockTable大小
	TPK_INFO_BLOCK_TABLE,						// 文件包BlockTable对象
	TPK_INFO_NUM_FILES,							// 文件包内实际文件总数

	TPK_INFO_HASH_INDEX,						// 文件对象在文件包HashTable的索引值
	TPK_INFO_CODENAME1,							// 文件名的哈希值1
	TPK_INFO_CODENAME2,							// 文件名的哈希值2
	TPK_INFO_BLOCKINDEX,						// 文件在BlockTable中的索引值
	TPK_INFO_FILE_SIZE,							// 文件的实际大小
	TPK_INFO_COMPRESSED_SIZE,					// 文件压缩后大小
	TPK_INFO_FLAGS,								// 文件存储时标志(对应TPK_FILE_XXX枚举值)
	TPK_INFO_POSITION,							// 文件数据位置
	TPK_INFO_CRC32,								// 文件数据的CRC32值
	TPK_INFO_FILENAME,							// 文件名字符串
	TPK_INFO_SEED,								// 文件数据静态加密种子
	TPK_INFO_SEED_UNFIXED,						// 文件数据动态加密种子
};

// 整理文件包操作中提醒用户的消息类型
// 用于实现函数指针COMPACTCB的消息参数
enum TPK_COMPACT
{
	CCB_CHECKING_FILES,							// 正在检查文件包(dwParam1=当前值,dwParam2=总值)
	CCB_CHECKING_HASH_TABLE,					// 正在检查HashTable(dwParam1=当前值,dwParam2=总值)
	CCB_COPYING_NON_TPK_DATA,					// 正在复制非TPK文件包数据，无参数可用
	CCB_COMPACTING_FILES,						// 正在整理文件包(dwParam1=当前值,dwParam2=总值)
	CCB_CLOSING_ARCHIVE,						// 正在关闭文件包，无参数可用
	CCB_COPYING_FILES
};

//-----------------------------------------------------------------------------
// 数据结构
#if (defined(WIN32) || defined(WIN64))
#include <pshpack1.h>
#else
#pragma pack(push,1)
#endif

// 文件包头数据结构
struct TPKHeader
{
    DWORD dwID;					// TPK的数据标志('TPK1A')
	DWORD dwVer;				// 文件包格式的版本号
	DWORD dwHashTableSize;		// HashTable表项的多少
    DWORD dwHashTablePos;		// HashTable表在包内的起始位置（低字节）
    DWORD dwHashTablePosHigh;	// HashTable表在包内的起始位置（高字节）
	DWORD dwBlockTableSize;		// BlockTable表项的多少
    DWORD dwBlockTablePos;		// BlockTable表在包内的起始位置（低字节）
	DWORD dwBlockTablePosHigh;	// BlockTable表在包内的起始位置（高字节）
};

// 文件的HashTable表的表项结构(对文件名字符串进行Hash索引）
struct TPKHash
{
    DWORD dwName1;
    DWORD dwName2;
	// 如果这个值有效，则指向BlockTable的索引位置
	// 如果值为:
	//    0xFFFFFFFF, 说明HashTabel的项是空项，而且BlockTable总是空的，搜索文件时略过此项
	//	  0xFFFFFFFE, 说明HashTable的项是空项，但是曾经被删除过，搜索文件时仍然会考虑
    DWORD dwBlockIndex;
};

// 文件包的BlockTable表的表项结构（描述具体文件数据的信息）
struct TPKBlock
{
    DWORD dwFilePos;			// 文件包内的相对偏移位置（低字节）
	DWORD dwFilePosHigh;		// 文件包内的相对偏移位置（高字节）
    DWORD dwCSize;				// 文件压缩后的字节
    DWORD dwFSize;				// 文件未压缩的字节
    DWORD dwFlags;				// 可选TPK_FILE_XXX的所有枚举值
	DWORD dwCrc32;				// 文件数据CRC32值
};

struct TFileNode
{
    size_t nLength;				// 文件名长度
    char  szFileName[1];		// 文件名指针
};

#if (defined(WIN32) || defined(WIN64))
#include <poppack.h>
#else
#pragma pack(pop)
#endif

struct TPKFile;
// 文件包对象结构
struct TPKArchive
{
    char			szFileName[MAX_PATH];// 文件包名
    HANDLE			hFile;              // 文件包本地文件句柄
    LARGE_INTEGER	HashTablePos;       // HashTable的起始位置
    LARGE_INTEGER	BlockTablePos;      // BlockTable的起始位置
    LARGE_INTEGER	TPKSize;            // 文件包的大小
	DWORD			dwFlags;            // 设置文件包的一些标识TPK_FLAG_XXXXX

	BYTE*			pbBlockBuffer;      // 文件内容预读数据缓冲区
    DWORD			dwBlockPos;         // 文件内容预读数据在文件数据的偏移位置
    DWORD			dwBuffPos;          // 实际需要的在预读数据中的位置

    TPKHeader*		pHeader;            // TPKHeader对象指针
	TPKHeader		Header;             // TPKHeader对象
    TPKHash*		pHashTable;         // HashTable对象指针
    TPKBlock*		pBlockTable;        // BlockTable对象指针
	TPKFile*		pLastFile;          // 最近一次读取的文件对象
    TFileNode**		pListFile;			// 文件列表
};

// 文件对象结构
struct TPKFile
{
    HANDLE			hFile;              // 本地文件句柄
    TPKArchive*		ha;                 // 文件包对象
    TPKHash*		pHash;              // HashTable对象
    TPKBlock*		pBlock;             // BlockTable对象
    DWORD			dwSeed1;            // 文件的加密密钥
    DWORD			dwFilePos;          // 当前文件内容的读取的位置
    LARGE_INTEGER	TPKFilePos;         // 当前文件在文件包内的偏移位置

    DWORD*			pdwBlockPos;        // 位置数据块缓冲区（仅为压缩文件时用）
    DWORD			nBlocks;            // 文件数据块总数
    BOOL			bBlockPosLoaded;    // 位置数据块是否已经加载过
    BYTE*			pbFileBuffer;		// 解压文件数据的缓冲区（对于整块单一文件，缓冲区大小是未压缩时的大小）

    DWORD			dwHashIndex;		// 对应HashTable的索引值
    DWORD			dwBlockIndex;       // 对应BlockTable的索引值
    char			szFileName[1];      // 文件名(可变长度)
};

// 文件检索结构
struct TPKSearch
{
    TPKArchive* ha;						// 文件包对象
    DWORD  dwNextIndex;                 // 下一个待检索的Hash项
    DWORD  dwName1;                     // 最近检索到的文件Hash项名1
    DWORD  dwName2;                     // 最近检索到的文件Hash项名2
    char   szSearchMask[1];             // 检索通配符（可变长度）
};

// 文件检索数据结构
struct TPK_FIND_DATA
{
    char	cFileName[MAX_PATH];        // 完整文件名（含完整路径）
    char*	szPlainName;                // 文件名（不含路径）
    DWORD	dwFileSize;                 // 文件大小
    DWORD	dwFileFlags;                // 文件标志（是否压缩或加密）
    DWORD	dwBlockIndex;               // Block项索引值
    DWORD	dwCompSize;                 // 压缩后文件大小
};

#ifndef ALLOCMEM
  #define ALLOCMEM(type, nitems)   (type *)malloc((nitems) * sizeof(type))
  #define FREEMEM(ptr) free(ptr)
#endif

// ----------------------------------------------------------------------------
// 文件包操作
BOOL  WINAPI TPKCreateArchive(const char* szTPKName, DWORD dwCreationDisposition, DWORD dwHashTableSize, HANDLE* phTPK);
BOOL  WINAPI TPKOpenArchive(const char* szTPKName, DWORD dwFlags, HANDLE* phTPK, DWORD dwAccessMode = GENERIC_READ);
BOOL  WINAPI TPKFlushArchive(HANDLE hTPK);
BOOL  WINAPI TPKCloseArchive(HANDLE hTPK);

BOOL  WINAPI TPKAddFileAll(const char* szTPKName, const char* TargetPath, const char* FilterDir, const char* FilterExt);
BOOL  WINAPI TPKExtractFileAll(HANDLE hTPK, const char* TargetPath);

BOOL  WINAPI TPKPackFileAll(const char* szTPKName, const char* TargetPath, const char* PackDir, const char* FilterExt);
BOOL  WINAPI TPKUnPackFileAll(HANDLE hTPK, const char* TargetPath);

// ----------------------------------------------------------------------------
// 文件操作
BOOL  WINAPI TPKOpenFile(HANDLE hTPK, const char* szFileName, DWORD dwSearchScope, HANDLE* phFile);
BOOL  WINAPI TPKCloseFile(HANDLE hFile);
BOOL  WINAPI TPKReadFile(HANDLE hFile, VOID* lpBuffer, DWORD dwToRead, DWORD* pdwRead = NULL, LPOVERLAPPED lpOverlapped = NULL);
BOOL  WINAPI TPKAddFile(HANDLE hTPK, const char* szFileName, const char* szArchivedName, DWORD dwFlags); 
BOOL  WINAPI TPKRemoveFile(HANDLE hTPK, const char* szFileName, DWORD dwSearchScope = TPK_OPEN_BY_INDEX);
BOOL  WINAPI TPKRenameFile(HANDLE hTPK, const char* szOldFileName, const char* szNewFileName);
BOOL  WINAPI TPKCopyFile(HANDLE hFileFrom, HANDLE hTpkTo, const char* szFileTo);
BOOL  WINAPI TPKVerifyFile(HANDLE hTPK, const char* szFileName);
BOOL  WINAPI TPKExtractFile(HANDLE hTPK, const char* szToExtract, const char* szExtracted);

// ----------------------------------------------------------------------------
// 其它工具函数
DWORD WINAPI TPKSetFilePointer(HANDLE hFile, LONG lFilePos, LONG* pdwFilePosHigh, DWORD dwMethod);
DWORD WINAPI TPKGetFilePos(HANDLE hFile, DWORD* pdwFilePosHigh = NULL);
DWORD WINAPI TPKGetFileSize(HANDLE hFile, DWORD* pdwFileSizeHigh = NULL);
BOOL  WINAPI TPKHaTPK(HANDLE hTPK, char* szFileName);
DWORD_PTR WINAPI TPKGetFileInfo(HANDLE hTPKOrFile, DWORD dwInfoType);
int   WINAPI TPKAddListFile(HANDLE hTPK, const char* szListFile);

// ----------------------------------------------------------------------------
// 文件检索操作
HANDLE WINAPI TPKFindFirstFile(HANDLE hTPK, const char* szMask, TPK_FIND_DATA* lpFindFileData, const char* szListFile);
BOOL   WINAPI TPKFindNextFile(HANDLE hFind, TPK_FIND_DATA* lpFindFileData);
BOOL   WINAPI TPKFindClose(HANDLE hFind);

// ----------------------------------------------------------------------------
// 文件列表检索操作
HANDLE TPKListFileFindFirst(HANDLE hTPK, const char* szListFile, const char* szMask, TPK_FIND_DATA* lpFindFileData);
BOOL   TPKListFileFindNext(HANDLE hFind, TPK_FIND_DATA* lpFindFileData);
BOOL   TPKListFileFindClose(HANDLE hFind);

// ----------------------------------------------------------------------------
// 压缩或解压缩数据
int WINAPI Compress_zlib   (char* pbOutBuffer, int* pdwOutLength, char* pbInBuffer, int dwInLength);
int WINAPI Decompress_zlib (char* pbOutBuffer, int* pdwOutLength, char* pbInBuffer, int dwInLength);

// ----------------------------------------------------------------------------
// 整理文件包操作
// 整理文件包提醒用户的回调函数声明
typedef void  (WINAPI *COMPACTCB)(void * lpUserData, DWORD dwWorkType, DWORD dwParam1, DWORD dwParam2);
// 设置回调函数
BOOL  WINAPI TPKSetCompactCallback(HANDLE hTPK, COMPACTCB CompactCB, void* lpData);
// 整理文件包操作
BOOL  WINAPI TPKCompactArchive(HANDLE hTPK, const char* szListFile = NULL, BOOL bReserved = 0);

#endif  // __TPKLIB_H__
