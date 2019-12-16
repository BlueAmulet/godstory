#include "TPKLib.h"
#include "TPKCommon.h"
#include <assert.h>

struct TListFileCache
{
    HANDLE  hFile;                      // TPK�ļ������
    char*	szMask;                     // ����ɸѡ��ͨ���
    DWORD   dwFileSize;                 // �б��ļ���ʵ�ʴ�С
    DWORD   dwBuffSize;                 // ��ǰ�ļ��ַ����б�������С
    DWORD   dwFilePos;                  // �����ļ��ַ����б�������ʵ���ļ��ĵ�ǰλ��
										//(�����޷��������б��ļ�һ����С���ڴ�,����ʹ�ô�ֵ��ʵ�ִַζ�ȡ)

    BYTE*	pBegin;                     // �ļ��ַ����б������ĵ�һ���ַ�λ��
    BYTE*	pPos;						// �����ļ��ַ����б������ĵ�ǰλ��
    BYTE*	pEnd;                       // �ļ��ַ����б����������һ���ַ�λ��

    BYTE	Buffer[1];					// �ļ��ַ����б�����
};

//-----------------------------------------------------------------------------
// ���¼���Cache���Buffer����
//(����б��ļ��������Ƿ�����,�������,�������¼��ص��ļ��ַ����б�����)
static int ReloadCache(TListFileCache* pCache)
{
    // ���Cache����Buffer�����Ƿ���ʣ��λ��δ��������ѵ�
	// ��β�������¼���
    if(pCache->pPos >= pCache->pEnd)
    {
        // ���Cache���Buffer���ݴ�����󣬾�ֱ���˳�
        if((pCache->dwFilePos + pCache->dwBuffSize) >= pCache->dwFileSize)
            return 0;

        pCache->dwFilePos += pCache->dwBuffSize;
        TPKReadFile(pCache->hFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);
        if(pCache->dwBuffSize == 0)
            return 0;

        // ����Cache��Ĳ���Buffer��ָ��λ��
        pCache->pBegin = pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;
    }

    return pCache->dwBuffSize;
}

//-----------------------------------------------------------------------------
// ��ȡÿ������
static size_t ReadLine(TListFileCache* pCache, char* szLine, int nMaxChars)
{
    char* szLineBegin	= szLine;
    char* szLineEnd		= szLine + nMaxChars - 1;
    
__BeginLoading:

    // ����һЩ���У��ո�Tab������һЩ�Ǵ�ӡ�ַ�
    while(pCache->pPos < pCache->pEnd && *pCache->pPos <= 0x20)
        pCache->pPos++;

    // ����ʣ���ֽ�����
    while(pCache->pPos < pCache->pEnd && szLine < szLineEnd)
    {
        // ������ֻ��лس����Թ�
        if(*pCache->pPos == 0x0D || *pCache->pPos == 0x0A)
            break;

        *szLine++ = *pCache->pPos++;
    }

    // �����Ҫ���¼���Cache���Buffer���ݣ�����
    if(pCache->pPos == pCache->pEnd)
    {
        if(ReloadCache(pCache) > 0)
            goto __BeginLoading;
    }

    *szLine = 0;
    return (szLine - szLineBegin);
}

//-----------------------------------------------------------------------------
// Ϊ�ļ��б��ļ�����һ����ʱ�ļ�Ŀ¼�е���ȫ·����
static void GetListFileName(TPKArchive* /* ha */, char* szListFile)
{
    char szTemp[MAX_PATH];
    GetTempPath(sizeof(szTemp)-1, szTemp);
    GetTempFileName(szTemp, TPK_LISTFILE_NAME, 0, szListFile);
}

//-----------------------------------------------------------------------------
// �����ļ��б��ļ�
// �ļ��б��ļ���TListFileNode������ṹ����������СͬHashTable��Сһ�¡�
int TPKListFileCreateList(TPKArchive* ha)
{    
	if(ha->pListFile != NULL)
		return ERROR_INVALID_PARAMETER;

	DWORD dwItems = ha->pHeader->dwHashTableSize;
    ha->pListFile = ALLOCMEM(TFileNode*, dwItems);
    if(ha->pListFile == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    memset(ha->pListFile, 0xFF, dwItems * sizeof(TFileNode*));
    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// Ϊ�ļ������ļ��б�����һ���ļ���������ļ����Ѿ�����������
int TPKListFileCreateNode(TPKArchive* ha, const char* szFileName)
{
    TFileNode*	pNode = NULL;
    TPKHash*	pHash = GetHashEntry(ha, szFileName);
    DWORD dwHashIndex = 0;
    size_t nLength;                     // �ļ�������

    // ����ļ������ļ����ڣ�ֱ�ӷ���
    if(pHash == NULL || pHash->dwBlockIndex >= DATA_ENTRY_DELETED)
        return ERROR_SUCCESS;

    // ����Ѿ��������ļ��б��Ҳֱ�ӷ���
    dwHashIndex = (DWORD)(pHash - ha->pHashTable);
    if((DWORD_PTR)ha->pListFile[dwHashIndex] < DATA_ENTRY_DELETED)
        return ERROR_SUCCESS;

    if(pNode == NULL)
    {
        nLength = strlen(szFileName);
        pNode = (TFileNode*)ALLOCMEM(char, sizeof(TFileNode) + nLength);
        pNode->nLength = nLength;
        strcpy(pNode->szFileName, szFileName);
        ha->pListFile[dwHashIndex] = pNode;
    }

    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// ���ļ����ļ��б���ɾ���ļ���
int TPKListFileRemoveNode(TPKArchive* ha, const char* szFileName)
{
    TFileNode* pNode = NULL;
    TPKHash* pHash = GetHashEntry(ha, szFileName);
    size_t nHashIndex = 0;

    if(pHash != NULL)
    {
        nHashIndex = pHash - ha->pHashTable;
        pNode = ha->pListFile[nHashIndex];
        ha->pListFile[nHashIndex] = (TFileNode*)(DWORD_PTR)DATA_ENTRY_DELETED;
        FREEMEM(pNode);
    }
    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// �ͷ��ļ��б�
void TPKListFileFreeList(TPKArchive* ha)
{
    if(ha->pListFile != NULL)
    {
        for(DWORD i = 0; i < ha->pHeader->dwHashTableSize; ++i)
        {
            TFileNode* pNode = ha->pListFile[i];
            if((DWORD_PTR)pNode < DATA_ENTRY_DELETED)
            {
                ha->pListFile[i] = (TFileNode*)(DWORD_PTR)DATA_ENTRY_FREE;
				FREEMEM(pNode);
            }
        }

        FREEMEM(ha->pListFile);
        ha->pListFile = NULL;
    }
}

//-----------------------------------------------------------------------------
// �洢�ļ��б�TPK�ļ���
int TPKListFileSaveToTPK(TPKArchive* ha)
{
    TFileNode*	pNode		= NULL;
    TPKHash*	pHashEnd	= NULL;
    TPKHash*	pHash0		= NULL;
    TPKHash*	pHash		= NULL;
    HANDLE		hFile		= INVALID_HANDLE_VALUE;
	size_t		nLength		= 0;
	DWORD		dwName1		= 0;
	DWORD		dwName2		= 0;
	int			nError		= ERROR_SUCCESS;
    char		szListFile[MAX_PATH];
    char		szBuffer[MAX_PATH+4];
    DWORD		dwTransferred;

    // ����ļ��б����Ϊ�գ�ֱ�ӷ���
    if(ha->pListFile == NULL)
        return ERROR_SUCCESS;

    // �����ļ��б����ļ�
    if(nError == ERROR_SUCCESS)
    {
        GetListFileName(ha, szListFile);
        hFile = CreateFile(szListFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // ������Ӧ�ļ��б��Hash��
    pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    pHash0 = pHash = GetHashEntry(ha, 0);
    if(pHash == NULL)
        pHash0 = pHash = ha->pHashTable;

    // �����ļ�
    if(nError == ERROR_SUCCESS)
    {
        for(;;)
        {
            if(pHash->dwName1 != dwName1 && pHash->dwName2 != dwName2 && pHash->dwBlockIndex < DATA_ENTRY_DELETED)
            {
                dwName1 = pHash->dwName1;
                dwName2 = pHash->dwName2;
                pNode = ha->pListFile[pHash - ha->pHashTable];

                if((DWORD_PTR)pNode < DATA_ENTRY_DELETED)
                {
                    memcpy(szBuffer, pNode->szFileName, pNode->nLength);
                    szBuffer[pNode->nLength + 0] = 0x0D;
                    szBuffer[pNode->nLength + 1] = 0x0A;
                    WriteFile(hFile, szBuffer, (DWORD)(pNode->nLength + 2), &dwTransferred, NULL);
                }
            }

            if(++pHash >= pHashEnd)
                pHash = ha->pHashTable;
            if(pHash == pHash0)
                break;
        }

        // ��Ȼ����Ҫд�ļ��б��Լ����ļ��������û�ж�ӦHash��������
        if(GetHashEntry(ha, TPK_LISTFILE_NAME) == NULL)
        {
            nLength = strlen(TPK_LISTFILE_NAME);
            memcpy(szBuffer, TPK_LISTFILE_NAME, nLength);
            szBuffer[nLength + 0] = 0x0D;
            szBuffer[nLength + 1] = 0x0A;
            WriteFile(hFile, szBuffer, (DWORD)(nLength + 2), &dwTransferred, NULL);
        }                     
        
        // �����������ļ��б��ļ����ļ�����
        nError = AddFileToArchive(ha,
                                  hFile,
                                  TPK_LISTFILE_NAME,
                                  TPK_FILE_ENCRYPTED | TPK_FILE_COMPRESSED | TPK_FILE_REPLACEEXIST,
                                  NULL);
    }

    // �������ɾ���ļ��б���ʱ�ļ�
    if(hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
    DeleteFile(szListFile);
    return nError;
}

//-----------------------------------------------------------------------------
// ���һ���ļ��б��ļ����ļ�����
// ��ע�����ļ��������е��ļ��б���Ȼ���������Դ˺���������ϸ�����ļ��б��ļ�����,
// ��������ڲ�������TPKFindFirstFile��
int WINAPI TPKAddListFile(HANDLE hTPK, const char* szListFile)
{
    TListFileCache* pCache = NULL;
    TPKArchive* ha = (TPKArchive*)hTPK;
    HANDLE hListFile = NULL;
    char  szFileName[MAX_PATH + 1];
    DWORD dwSearchScope = TPK_OPEN_LOCAL_FILE;
    DWORD dwCacheSize = 0;
    DWORD dwFileSize = 0;
    size_t nLength = 0;
    int nError = ERROR_SUCCESS;

    // �������szListFileΪNULL��˵����Ҫ��ȱʡ�ļ��б��ļ�
    if(szListFile == NULL)
    {
        szListFile = TPK_LISTFILE_NAME;
        dwSearchScope = TPK_OPEN_FROM_TPK;
    }

    // ���ļ��б��ļ�
    if(nError == ERROR_SUCCESS)
    {
        if(!TPKOpenFile((HANDLE)ha, szListFile, dwSearchScope, &hListFile))
            nError = GetLastError();
    }

    if(nError == ERROR_SUCCESS)
    {
		// ����Cache��С���ļ���С��ͬ
        dwCacheSize = dwFileSize = TPKGetFileSize(hListFile, NULL);

        // ���Է��������ļ��б����ݴ�С���ڴ棬���ʧ����ֻ�����ļ����ִ�С�ռ�
        pCache = (TListFileCache*)ALLOCMEM(char, (sizeof(TListFileCache) + dwCacheSize));
        if(pCache == NULL)
        {
            dwCacheSize = LISTFILE_CACHE_SIZE;
            pCache = (TListFileCache*)ALLOCMEM(char, sizeof(TListFileCache) + dwCacheSize);
        }

        if(pCache == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    if(nError == ERROR_SUCCESS)
    {
        // ��ʼ���ļ��б�Cache����
        memset(pCache, 0, sizeof(TListFileCache));
        pCache->hFile      = hListFile;
        pCache->dwFileSize = dwFileSize;
        pCache->dwBuffSize = dwCacheSize;
        pCache->dwFilePos  = 0;

        // ��ȡ�ļ�����
        TPKReadFile(hListFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);

        // ��ʼ������BUFFER��ָ��λ��
        pCache->pBegin = pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;

        // ��ȡÿһ�У�����ļ������ļ��б���
        while((nLength = ReadLine(pCache, szFileName, sizeof(szFileName) - 1)) > 0)
            TPKListFileCreateNode(ha, szFileName);
    }

    // ������˳�
    if(pCache != NULL)
        TPKListFileFindClose((HANDLE)pCache);
    return nError;
}

//-----------------------------------------------------------------------------
// �����ļ��б����ݵ�ÿ������ļ����ļ������ṹ��
HANDLE TPKListFileFindFirst(HANDLE hTPK, const char* szListFile, const char* szMask, TPK_FIND_DATA* lpFindFileData)
{
    TListFileCache* pCache = NULL;
    TPKArchive* ha = (TPKArchive*)hTPK;
    HANDLE hListFile = NULL;
    DWORD dwSearchScope = TPK_OPEN_LOCAL_FILE;
    DWORD dwCacheSize = 0;
    DWORD dwFileSize = 0;
    size_t nLength = 0;
    int nError = ERROR_SUCCESS;

    // ��ʼ�������ļ��Ľṹ����
    memset(lpFindFileData, 0, sizeof(TPK_FIND_DATA));

    // ���szListFileΪNULL,��ʹ���ļ����ڲ�ȱʡ���ļ��б���
    if(szListFile == NULL)
    {
        szListFile = TPK_LISTFILE_NAME;
        dwSearchScope = TPK_OPEN_FROM_TPK;
    }

    // ���ļ�
    if(nError == ERROR_SUCCESS)
    {
        if(!TPKOpenFile((HANDLE)ha, szListFile, dwSearchScope, &hListFile))
            nError = GetLastError();
    }

    if(nError == ERROR_SUCCESS)
    {
		// ����Cache��С���ļ���С��ͬ
        dwCacheSize = dwFileSize = TPKGetFileSize(hListFile, NULL);

        // ���Է��������ļ��б����ݴ�С���ڴ棬���ʧ����ֻ�����ļ����ִ�С�ռ�
        pCache = (TListFileCache *)ALLOCMEM(char, sizeof(TListFileCache) + dwCacheSize);
        if(pCache == NULL)
        {
            dwCacheSize = LISTFILE_CACHE_SIZE;
            pCache = (TListFileCache *)ALLOCMEM(char, sizeof(TListFileCache) + dwCacheSize);
        }

        if(pCache == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    if(nError == ERROR_SUCCESS)
    {
         // ��ʼ���ļ��б�Cache����
        memset(pCache, 0, sizeof(TListFileCache));
        pCache->hFile      = hListFile;
        pCache->dwFileSize = dwFileSize;
        pCache->dwBuffSize = dwCacheSize;
        pCache->dwFilePos  = 0;
        if(szMask != NULL)
        {
            pCache->szMask = ALLOCMEM(char, strlen(szMask) + 1);
            strcpy(pCache->szMask, szMask);
        }

        // ��ȡ�ļ���������
        TPKReadFile(hListFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);

        // ��ʼ������BUFFER��ָ��λ��
        pCache->pBegin = pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;

        for(;;)
        {
            // ��ȡ�ļ��б��ļ����ݵ�ÿһ������
            nLength = ReadLine(pCache, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName));
            if(nLength == 0)
            {
                nError = ERROR_NO_MORE_FILES;
                break;
            }

            // �����ܵ�һЩƥ���
            if(CheckWildCard(lpFindFileData->cFileName, pCache->szMask))
                break;                
        }
    }

    // ������˳�
    if(nError != ERROR_SUCCESS)
    {
        memset(lpFindFileData, 0, sizeof(TPK_FIND_DATA));
        TPKListFileFindClose((HANDLE)pCache);
        pCache = NULL;
        SetLastError(nError);
    }
    return (HANDLE)pCache;
}

//-----------------------------------------------------------------------------
// ���ļ��б�Cache������һ���ļ���������
BOOL TPKListFileFindNext(HANDLE hFind, TPK_FIND_DATA* lpFindFileData)
{
    TListFileCache* pCache = (TListFileCache*)hFind;
    size_t nLength;
    BOOL bResult = FALSE;
    int nError = ERROR_SUCCESS;
    for(;;)
    {
        // ��ȡÿһ���ļ�������
        nLength = ReadLine(pCache, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName));
        if(nLength == 0)
        {
            nError = ERROR_NO_MORE_FILES;
            break;
        }

        // �����ܴ��ڵ�ƥ���
        if(CheckWildCard(lpFindFileData->cFileName, pCache->szMask))
        {
            bResult = TRUE;
            break;
        }
    }

    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return bResult;
}

//-----------------------------------------------------------------------------
// �ر��ļ��б�����
BOOL TPKListFileFindClose(HANDLE hFind)
{
    TListFileCache* pCache = (TListFileCache*)hFind;
    if(pCache != NULL)
    {
        if(pCache->hFile != NULL)
            TPKCloseFile(pCache->hFile);
        if(pCache->szMask != NULL)
            FREEMEM(pCache->szMask);

        FREEMEM(pCache);
        return TRUE;
    }
    return FALSE;
}