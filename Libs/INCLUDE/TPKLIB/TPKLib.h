/********************************************************************************/
/* TPKLib.h																		*/
/*------------------------------------------------------------------------------*/
/* TPK�ļ�ϵͳ�������ṩ���ⲿ�û�ʹ�õİ����ļ�								*/
/*------------------------------------------------------------------------------*/
/*	   ʱ��      �汾		����    ��ע										*/
/* -----------  ------		------  -------										*/
/* 2008.12.18   01.00.00    Daniel												*/
/*							1.�Ľ��ļ�����ȡ�ļ��������ļ���IO��������			*/
/*							2.֧���ļ���ѹ������ܹ���							*/
/*							3.֧���ļ������ܴ���4G�����						*/
/*							4.����WIN32�ļ�IO�����Ĳ����ӿ�						*/
/*							6.��֧���ļ����ڵ����ļ���С����4G�����			*/
/*							7.��֧���ļ������ļ�������������������HashTable��С��*/
/* 2009.02.23	01.00.01	Daniel												
/*							1.֧���ļ����䶯�ļ����ݵĴ���������(BlockTable��С������)*/
/*							2.֧���ļ���д�����ǰ�ϵ籣��						*/
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

#define TPK_FILEID				0x1A4B5054		// TPK�ļ�����ʶ  =>TPK
#define TPK_VERSION				0x1B010000		// TPK�ļ����汾  =>01.00.00
#define TPK_DATABLOCK_SIZE		4096			// TPK�ļ����������ļ����ݵ����ݿ�Ĵ洢��С

#define TPK_LISTFILE_NAME		"(listfile)"    // TPK�ļ����ڵ��ļ��б���
#define TPK_VERFILE_NAME		"(version)"		// TPK�ļ����ڵİ汾�����ļ������Ǳ���ģ�
#define TPK_HASHTABLE_KEY		"<hash_table>"	// HashTable������Կ
#define TPK_BLOCKTABLE_KEY		"<block_table>"	// BlockTable������Կ
#define HASH_TABLE_SIZE_MIN		0x00004			// HashTable��������������
#define HASH_TABLE_SIZE_MAX		0x40000			// HashTable��������������
#define DATA_ENTRY_DELETED      0xFFFFFFFE		// �����ɾ��
#define DATA_ENTRY_FREE         0xFFFFFFFF		// ��������У�δʹ��
#define LISTFILE_CACHE_SIZE		0x1000			// ȱʡ���ļ��ַ����б�������С

// �ṩ��TPKOpenFile�����Ĳ���ֵ
#define TPK_OPEN_FROM_TPK		0				// ���ļ�������ļ�
#define TPK_OPEN_BY_INDEX		1				// �ļ�������'szFileName'��ʵ��һ��dwBlockIndexֵ
#define TPK_OPEN_LOCAL_FILE		(DWORD)-1		// �򿪱����ļ�

// �����ļ���һЩ�ض�������ʶ
#define TPK_FLAG_CHANGED		0x00000001		// �д˱�־˵���ļ����Ѿ��иĶ�
#define TPK_FLAG_PROTECTED      0x00000002		// �д˱�־˵����Ҫ��������ϸ����ݼ��
#define TPK_FLAG_NOLISTFILE		0x00000004		// �д˱�־˵�����ļ���ʱ����ȡ�ļ��б�(����TPKOpenArchive)

enum TPK_FILEOP
{
	TPK_FILE_COMPRESSED			= 0x00000100,	// �д˱�־˵���ļ���Ҫѹ���������Ѿ�ѹ����
	TPK_FILE_ENCRYPTED			= 0x00000200,	// �д˱�־˵���ļ��Ѿ����ܹ�
	TPK_FILE_FIXSEED			= 0x00001000,	// �д˱�־˵���ļ��������ӷǹ̶�ֵ��������һ��������ֵ��
	TPK_FILE_SINGLE_UNIT		= 0x00010000,	// �д˱�־˵���ļ�����������洢
	TPK_FILE_BREAKPROTECTED		= 0x00100000,	// �д˱�־˵���ļ�������Ҫ�ϵ籣��
	TPK_FILE_EXISTS				= 0x80000000,	// �д˱�־˵��Block��ָ�����ļ����������Ǵ��ڵģ����ļ���ɾ��ʱ���ᱻ���ã�
	TPK_FILE_REPLACEEXIST		= 0x80000000,	// �д˱�־˵���ļ�����Ѿ����ļ�������ڣ���Ҫ���滻(TPKAddFile)
	TPK_FILE_VALID_FLAGS		= (TPK_FILE_COMPRESSED|TPK_FILE_ENCRYPTED|TPK_FILE_FIXSEED|TPK_FILE_SINGLE_UNIT|TPK_FILE_EXISTS)
};

// �ṩ��TPKGetFileInfo�õ�һϵ��ö��ֵ
enum TPK_INFO
{
	TPK_INFO_HASH_TABLE_SIZE,					// �ļ���HashTable�Ĵ�С
	TPK_INFO_HASH_TABLE,						// �ļ���HashTable����
	TPK_INFO_BLOCK_TABLE_SIZE,					// �ļ���BlockTable��С
	TPK_INFO_BLOCK_TABLE,						// �ļ���BlockTable����
	TPK_INFO_NUM_FILES,							// �ļ�����ʵ���ļ�����

	TPK_INFO_HASH_INDEX,						// �ļ��������ļ���HashTable������ֵ
	TPK_INFO_CODENAME1,							// �ļ����Ĺ�ϣֵ1
	TPK_INFO_CODENAME2,							// �ļ����Ĺ�ϣֵ2
	TPK_INFO_BLOCKINDEX,						// �ļ���BlockTable�е�����ֵ
	TPK_INFO_FILE_SIZE,							// �ļ���ʵ�ʴ�С
	TPK_INFO_COMPRESSED_SIZE,					// �ļ�ѹ�����С
	TPK_INFO_FLAGS,								// �ļ��洢ʱ��־(��ӦTPK_FILE_XXXö��ֵ)
	TPK_INFO_POSITION,							// �ļ�����λ��
	TPK_INFO_CRC32,								// �ļ����ݵ�CRC32ֵ
	TPK_INFO_FILENAME,							// �ļ����ַ���
	TPK_INFO_SEED,								// �ļ����ݾ�̬��������
	TPK_INFO_SEED_UNFIXED,						// �ļ����ݶ�̬��������
};

// �����ļ��������������û�����Ϣ����
// ����ʵ�ֺ���ָ��COMPACTCB����Ϣ����
enum TPK_COMPACT
{
	CCB_CHECKING_FILES,							// ���ڼ���ļ���(dwParam1=��ǰֵ,dwParam2=��ֵ)
	CCB_CHECKING_HASH_TABLE,					// ���ڼ��HashTable(dwParam1=��ǰֵ,dwParam2=��ֵ)
	CCB_COPYING_NON_TPK_DATA,					// ���ڸ��Ʒ�TPK�ļ������ݣ��޲�������
	CCB_COMPACTING_FILES,						// ���������ļ���(dwParam1=��ǰֵ,dwParam2=��ֵ)
	CCB_CLOSING_ARCHIVE,						// ���ڹر��ļ������޲�������
	CCB_COPYING_FILES
};

//-----------------------------------------------------------------------------
// ���ݽṹ
#if (defined(WIN32) || defined(WIN64))
#include <pshpack1.h>
#else
#pragma pack(push,1)
#endif

// �ļ���ͷ���ݽṹ
struct TPKHeader
{
    DWORD dwID;					// TPK�����ݱ�־('TPK1A')
	DWORD dwVer;				// �ļ�����ʽ�İ汾��
	DWORD dwHashTableSize;		// HashTable����Ķ���
    DWORD dwHashTablePos;		// HashTable���ڰ��ڵ���ʼλ�ã����ֽڣ�
    DWORD dwHashTablePosHigh;	// HashTable���ڰ��ڵ���ʼλ�ã����ֽڣ�
	DWORD dwBlockTableSize;		// BlockTable����Ķ���
    DWORD dwBlockTablePos;		// BlockTable���ڰ��ڵ���ʼλ�ã����ֽڣ�
	DWORD dwBlockTablePosHigh;	// BlockTable���ڰ��ڵ���ʼλ�ã����ֽڣ�
};

// �ļ���HashTable��ı���ṹ(���ļ����ַ�������Hash������
struct TPKHash
{
    DWORD dwName1;
    DWORD dwName2;
	// ������ֵ��Ч����ָ��BlockTable������λ��
	// ���ֵΪ:
	//    0xFFFFFFFF, ˵��HashTabel�����ǿ������BlockTable���ǿյģ������ļ�ʱ�Թ�����
	//	  0xFFFFFFFE, ˵��HashTable�����ǿ������������ɾ�����������ļ�ʱ��Ȼ�ῼ��
    DWORD dwBlockIndex;
};

// �ļ�����BlockTable��ı���ṹ�����������ļ����ݵ���Ϣ��
struct TPKBlock
{
    DWORD dwFilePos;			// �ļ����ڵ����ƫ��λ�ã����ֽڣ�
	DWORD dwFilePosHigh;		// �ļ����ڵ����ƫ��λ�ã����ֽڣ�
    DWORD dwCSize;				// �ļ�ѹ������ֽ�
    DWORD dwFSize;				// �ļ�δѹ�����ֽ�
    DWORD dwFlags;				// ��ѡTPK_FILE_XXX������ö��ֵ
	DWORD dwCrc32;				// �ļ�����CRC32ֵ
};

struct TFileNode
{
    size_t nLength;				// �ļ�������
    char  szFileName[1];		// �ļ���ָ��
};

#if (defined(WIN32) || defined(WIN64))
#include <poppack.h>
#else
#pragma pack(pop)
#endif

struct TPKFile;
// �ļ�������ṹ
struct TPKArchive
{
    char			szFileName[MAX_PATH];// �ļ�����
    HANDLE			hFile;              // �ļ��������ļ����
    LARGE_INTEGER	HashTablePos;       // HashTable����ʼλ��
    LARGE_INTEGER	BlockTablePos;      // BlockTable����ʼλ��
    LARGE_INTEGER	TPKSize;            // �ļ����Ĵ�С
	DWORD			dwFlags;            // �����ļ�����һЩ��ʶTPK_FLAG_XXXXX

	BYTE*			pbBlockBuffer;      // �ļ�����Ԥ�����ݻ�����
    DWORD			dwBlockPos;         // �ļ�����Ԥ���������ļ����ݵ�ƫ��λ��
    DWORD			dwBuffPos;          // ʵ����Ҫ����Ԥ�������е�λ��

    TPKHeader*		pHeader;            // TPKHeader����ָ��
	TPKHeader		Header;             // TPKHeader����
    TPKHash*		pHashTable;         // HashTable����ָ��
    TPKBlock*		pBlockTable;        // BlockTable����ָ��
	TPKFile*		pLastFile;          // ���һ�ζ�ȡ���ļ�����
    TFileNode**		pListFile;			// �ļ��б�
};

// �ļ�����ṹ
struct TPKFile
{
    HANDLE			hFile;              // �����ļ����
    TPKArchive*		ha;                 // �ļ�������
    TPKHash*		pHash;              // HashTable����
    TPKBlock*		pBlock;             // BlockTable����
    DWORD			dwSeed1;            // �ļ��ļ�����Կ
    DWORD			dwFilePos;          // ��ǰ�ļ����ݵĶ�ȡ��λ��
    LARGE_INTEGER	TPKFilePos;         // ��ǰ�ļ����ļ����ڵ�ƫ��λ��

    DWORD*			pdwBlockPos;        // λ�����ݿ黺��������Ϊѹ���ļ�ʱ�ã�
    DWORD			nBlocks;            // �ļ����ݿ�����
    BOOL			bBlockPosLoaded;    // λ�����ݿ��Ƿ��Ѿ����ع�
    BYTE*			pbFileBuffer;		// ��ѹ�ļ����ݵĻ��������������鵥һ�ļ�����������С��δѹ��ʱ�Ĵ�С��

    DWORD			dwHashIndex;		// ��ӦHashTable������ֵ
    DWORD			dwBlockIndex;       // ��ӦBlockTable������ֵ
    char			szFileName[1];      // �ļ���(�ɱ䳤��)
};

// �ļ������ṹ
struct TPKSearch
{
    TPKArchive* ha;						// �ļ�������
    DWORD  dwNextIndex;                 // ��һ����������Hash��
    DWORD  dwName1;                     // ������������ļ�Hash����1
    DWORD  dwName2;                     // ������������ļ�Hash����2
    char   szSearchMask[1];             // ����ͨ������ɱ䳤�ȣ�
};

// �ļ��������ݽṹ
struct TPK_FIND_DATA
{
    char	cFileName[MAX_PATH];        // �����ļ�����������·����
    char*	szPlainName;                // �ļ���������·����
    DWORD	dwFileSize;                 // �ļ���С
    DWORD	dwFileFlags;                // �ļ���־���Ƿ�ѹ������ܣ�
    DWORD	dwBlockIndex;               // Block������ֵ
    DWORD	dwCompSize;                 // ѹ�����ļ���С
};

#ifndef ALLOCMEM
  #define ALLOCMEM(type, nitems)   (type *)malloc((nitems) * sizeof(type))
  #define FREEMEM(ptr) free(ptr)
#endif

// ----------------------------------------------------------------------------
// �ļ�������
BOOL  WINAPI TPKCreateArchive(const char* szTPKName, DWORD dwCreationDisposition, DWORD dwHashTableSize, HANDLE* phTPK);
BOOL  WINAPI TPKOpenArchive(const char* szTPKName, DWORD dwFlags, HANDLE* phTPK, DWORD dwAccessMode = GENERIC_READ);
BOOL  WINAPI TPKFlushArchive(HANDLE hTPK);
BOOL  WINAPI TPKCloseArchive(HANDLE hTPK);

BOOL  WINAPI TPKAddFileAll(const char* szTPKName, const char* TargetPath, const char* FilterDir, const char* FilterExt);
BOOL  WINAPI TPKExtractFileAll(HANDLE hTPK, const char* TargetPath);

BOOL  WINAPI TPKPackFileAll(const char* szTPKName, const char* TargetPath, const char* PackDir, const char* FilterExt);
BOOL  WINAPI TPKUnPackFileAll(HANDLE hTPK, const char* TargetPath);

// ----------------------------------------------------------------------------
// �ļ�����
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
// �������ߺ���
DWORD WINAPI TPKSetFilePointer(HANDLE hFile, LONG lFilePos, LONG* pdwFilePosHigh, DWORD dwMethod);
DWORD WINAPI TPKGetFilePos(HANDLE hFile, DWORD* pdwFilePosHigh = NULL);
DWORD WINAPI TPKGetFileSize(HANDLE hFile, DWORD* pdwFileSizeHigh = NULL);
BOOL  WINAPI TPKHaTPK(HANDLE hTPK, char* szFileName);
DWORD_PTR WINAPI TPKGetFileInfo(HANDLE hTPKOrFile, DWORD dwInfoType);
int   WINAPI TPKAddListFile(HANDLE hTPK, const char* szListFile);

// ----------------------------------------------------------------------------
// �ļ���������
HANDLE WINAPI TPKFindFirstFile(HANDLE hTPK, const char* szMask, TPK_FIND_DATA* lpFindFileData, const char* szListFile);
BOOL   WINAPI TPKFindNextFile(HANDLE hFind, TPK_FIND_DATA* lpFindFileData);
BOOL   WINAPI TPKFindClose(HANDLE hFind);

// ----------------------------------------------------------------------------
// �ļ��б��������
HANDLE TPKListFileFindFirst(HANDLE hTPK, const char* szListFile, const char* szMask, TPK_FIND_DATA* lpFindFileData);
BOOL   TPKListFileFindNext(HANDLE hFind, TPK_FIND_DATA* lpFindFileData);
BOOL   TPKListFileFindClose(HANDLE hFind);

// ----------------------------------------------------------------------------
// ѹ�����ѹ������
int WINAPI Compress_zlib   (char* pbOutBuffer, int* pdwOutLength, char* pbInBuffer, int dwInLength);
int WINAPI Decompress_zlib (char* pbOutBuffer, int* pdwOutLength, char* pbInBuffer, int dwInLength);

// ----------------------------------------------------------------------------
// �����ļ�������
// �����ļ��������û��Ļص���������
typedef void  (WINAPI *COMPACTCB)(void * lpUserData, DWORD dwWorkType, DWORD dwParam1, DWORD dwParam2);
// ���ûص�����
BOOL  WINAPI TPKSetCompactCallback(HANDLE hTPK, COMPACTCB CompactCB, void* lpData);
// �����ļ�������
BOOL  WINAPI TPKCompactArchive(HANDLE hTPK, const char* szListFile = NULL, BOOL bReserved = 0);

#endif  // __TPKLIB_H__
