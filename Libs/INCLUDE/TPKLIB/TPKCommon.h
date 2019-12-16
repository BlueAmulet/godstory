/********************************************************************************/
/* TPKCommon.h																	*/
/*------------------------------------------------------------------------------*/
/* TPK�ļ�ϵͳ����ڲ����õİ����ļ�											*/
/*------------------------------------------------------------------------------*/
/*	   ʱ��      �汾     ����    ��ע											*/
/* -----------  ------   ------  -------										*/
/* 2008.12.18   1.00     Daniel													*/
/********************************************************************************/
#ifndef __TPKCOMMON_H__
#define __TPKCOMMON_H__

//-----------------------------------------------------------------------------
// ���ߺ���
int   PrepareCryptBuffer();

// �ӽ���HashTable
void  EncryptHashTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength);
void  DecryptHashTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength);

// �ӽ���BlockTable
void  EncryptBlockTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength);
void  DecryptBlockTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength);

DWORD DetectFileSeed(DWORD* block, DWORD decrypted);
DWORD DetectFileSeed2(DWORD* block, UINT nDwords, ...);

// �ӽ����ļ����ݿ�
void  EncrypTPKBlock(DWORD* pdwBlock, DWORD dwLength, DWORD dwSeed1);
void  DecrypTPKBlock(DWORD* pdwBlock, DWORD dwLength, DWORD dwSeed1);

DWORD DecryptHashIndex(TPKArchive* ha, const char* szFileName);
DWORD DecryptName1    (const char* szFileName);
DWORD DecryptName2    (const char* szFileName);
DWORD DecryptFileSeed (const char* szFileName);

// �����ļ��������ļ�����ȡ��Ӧ��Hash��
TPKHash* GetHashEntry  (TPKArchive* ha, const char* szFileName);
// �����ļ��������ļ�����ȡ���е�Hash��
TPKHash* FindFreeHashEntry(TPKArchive* ha, const char* szFileName);

// �ж��ļ��������Ƿ���Ч
BOOL IsValidTPKHandle(TPKArchive* ha);
// �ж��ļ������Ƿ���Ч
BOOL IsValidFileHandle(TPKFile* hf);

// ����ļ����ļ�����(������Hash�Block����ļ��б������)
int  AddInternalFile(TPKArchive* ha, const char* szFileName);

// ��һ�������ļ���ӵ��ļ�����
int  AddFileToArchive(TPKArchive* ha, HANDLE hFile, const char* szArchivedName, DWORD dwFlags, BOOL* pbReplaced);
// ����TPK�ļ�����Header��HashTable��BlockTable�����ݵ�����
int  SaveTPKTables(TPKArchive* ha);
DWORD SaveTPKTablesForAddFile(TPKArchive* ha, LARGE_INTEGER& dwNewHashTablePos);
// �ͷ�TPK�ļ�����������
void FreeTPKArchive(TPKArchive *& ha);
// �ͷ�TPK�ļ���������
void FreeTPKFile(TPKFile *& hf);

DWORD ExtendBlockTable(TPKArchive* ha);

// ƥ���ַ�������
BOOL CheckWildCard(const char* szString, const char* szWildCard);
// �ݹ鴴��Ŀ¼
void EnsureDirectory(const char* path);

// ����ļ�����д����
void ClearFileAttributes(const char* szString);

extern COMPACTCB CompactCB;
extern void*	 lpUserData;

//-----------------------------------------------------------------------------
// �ļ��б��������
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

