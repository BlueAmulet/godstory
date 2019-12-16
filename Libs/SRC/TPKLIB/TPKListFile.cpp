#include "TPKLib.h"
#include "TPKCommon.h"
#include <assert.h>

struct TListFileCache
{
    HANDLE  hFile;                      // TPK文件包句柄
    char*	szMask;                     // 用于筛选的通配符
    DWORD   dwFileSize;                 // 列表文件的实际大小
    DWORD   dwBuffSize;                 // 当前文件字符串列表缓冲区大小
    DWORD   dwFilePos;                  // 操作文件字符串列表缓冲区在实际文件的当前位置
										//(可能无法申请与列表文件一样大小的内存,所以使用此值来实现分次读取)

    BYTE*	pBegin;                     // 文件字符串列表缓冲区的第一个字符位置
    BYTE*	pPos;						// 操作文件字符串列表缓冲区的当前位置
    BYTE*	pEnd;                       // 文件字符串列表缓冲区的最后一个字符位置

    BYTE	Buffer[1];					// 文件字符串列表缓冲区
};

//-----------------------------------------------------------------------------
// 重新加载Cache项的Buffer数据
//(检查列表文件的数据是否处理完,如果还有,继续重新加载到文件字符串列表缓冲区)
static int ReloadCache(TListFileCache* pCache)
{
    // 检查Cache项中Buffer数据是否有剩余位置未处理，如果已到
	// 结尾，则重新加载
    if(pCache->pPos >= pCache->pEnd)
    {
        // 如果Cache项的Buffer数据处理到最后，就直接退出
        if((pCache->dwFilePos + pCache->dwBuffSize) >= pCache->dwFileSize)
            return 0;

        pCache->dwFilePos += pCache->dwBuffSize;
        TPKReadFile(pCache->hFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);
        if(pCache->dwBuffSize == 0)
            return 0;

        // 设置Cache项的操作Buffer的指针位置
        pCache->pBegin = pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;
    }

    return pCache->dwBuffSize;
}

//-----------------------------------------------------------------------------
// 读取每行数据
static size_t ReadLine(TListFileCache* pCache, char* szLine, int nMaxChars)
{
    char* szLineBegin	= szLine;
    char* szLineEnd		= szLine + nMaxChars - 1;
    
__BeginLoading:

    // 跳过一些新行，空格，Tab和其它一些非打印字符
    while(pCache->pPos < pCache->pEnd && *pCache->pPos <= 0x20)
        pCache->pPos++;

    // 拷贝剩余字节内容
    while(pCache->pPos < pCache->pEnd && szLine < szLineEnd)
    {
        // 如果发现换行回车则略过
        if(*pCache->pPos == 0x0D || *pCache->pPos == 0x0A)
            break;

        *szLine++ = *pCache->pPos++;
    }

    // 如果需要重新加载Cache项的Buffer数据，则处理
    if(pCache->pPos == pCache->pEnd)
    {
        if(ReloadCache(pCache) > 0)
            goto __BeginLoading;
    }

    *szLine = 0;
    return (szLine - szLineBegin);
}

//-----------------------------------------------------------------------------
// 为文件列表文件创建一个临时文件目录中的完全路径名
static void GetListFileName(TPKArchive* /* ha */, char* szListFile)
{
    char szTemp[MAX_PATH];
    GetTempPath(sizeof(szTemp)-1, szTemp);
    GetTempFileName(szTemp, TPK_LISTFILE_NAME, 0, szListFile);
}

//-----------------------------------------------------------------------------
// 创建文件列表文件
// 文件列表文件是TListFileNode的数组结构，这个数组大小同HashTable大小一致。
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
// 为文件包的文件列表增加一个文件名，如果文件名已经存在则不增加
int TPKListFileCreateNode(TPKArchive* ha, const char* szFileName)
{
    TFileNode*	pNode = NULL;
    TPKHash*	pHash = GetHashEntry(ha, szFileName);
    DWORD dwHashIndex = 0;
    size_t nLength;                     // 文件名长度

    // 如果文件不在文件包内，直接返回
    if(pHash == NULL || pHash->dwBlockIndex >= DATA_ENTRY_DELETED)
        return ERROR_SUCCESS;

    // 如果已经存在在文件列表里，也直接返回
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
// 从文件包文件列表中删除文件名
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
// 释放文件列表
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
// 存储文件列表到TPK文件包
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

    // 如果文件列表对象为空，直接返回
    if(ha->pListFile == NULL)
        return ERROR_SUCCESS;

    // 创建文件列表本地文件
    if(nError == ERROR_SUCCESS)
    {
        GetListFileName(ha, szListFile);
        hFile = CreateFile(szListFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // 搜索对应文件列表的Hash项
    pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    pHash0 = pHash = GetHashEntry(ha, 0);
    if(pHash == NULL)
        pHash0 = pHash = ha->pHashTable;

    // 保存文件
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

        // 仍然还是要写文件列表自己的文件名（如果没有对应Hash项的情况）
        if(GetHashEntry(ha, TPK_LISTFILE_NAME) == NULL)
        {
            nLength = strlen(TPK_LISTFILE_NAME);
            memcpy(szBuffer, TPK_LISTFILE_NAME, nLength);
            szBuffer[nLength + 0] = 0x0D;
            szBuffer[nLength + 1] = 0x0A;
            WriteFile(hFile, szBuffer, (DWORD)(nLength + 2), &dwTransferred, NULL);
        }                     
        
        // 添加这个本地文件列表文件到文件包内
        nError = AddFileToArchive(ha,
                                  hFile,
                                  TPK_LISTFILE_NAME,
                                  TPK_FILE_ENCRYPTED | TPK_FILE_COMPRESSED | TPK_FILE_REPLACEEXIST,
                                  NULL);
    }

    // 处理完毕删除文件列表临时文件
    if(hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
    DeleteFile(szListFile);
    return nError;
}

//-----------------------------------------------------------------------------
// 添加一个文件列表文件到文件包内
// （注：在文件包里已有的文件列表仍然保留，所以此函数可以组合更多的文件列表到文件包里,
// 这个函数内部调用了TPKFindFirstFile）
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

    // 如果参数szListFile为NULL，说明需要打开缺省文件列表文件
    if(szListFile == NULL)
    {
        szListFile = TPK_LISTFILE_NAME;
        dwSearchScope = TPK_OPEN_FROM_TPK;
    }

    // 打开文件列表文件
    if(nError == ERROR_SUCCESS)
    {
        if(!TPKOpenFile((HANDLE)ha, szListFile, dwSearchScope, &hListFile))
            nError = GetLastError();
    }

    if(nError == ERROR_SUCCESS)
    {
		// 创建Cache大小与文件大小相同
        dwCacheSize = dwFileSize = TPKGetFileSize(hListFile, NULL);

        // 尝试分配整个文件列表内容大小的内存，如果失败则只分配文件部分大小空间
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
        // 初始化文件列表Cache数据
        memset(pCache, 0, sizeof(TListFileCache));
        pCache->hFile      = hListFile;
        pCache->dwFileSize = dwFileSize;
        pCache->dwBuffSize = dwCacheSize;
        pCache->dwFilePos  = 0;

        // 获取文件内容
        TPKReadFile(hListFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);

        // 初始化操作BUFFER的指针位置
        pCache->pBegin = pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;

        // 读取每一行，添加文件名到文件列表中
        while((nLength = ReadLine(pCache, szFileName, sizeof(szFileName) - 1)) > 0)
            TPKListFileCreateNode(ha, szFileName);
    }

    // 清除并退出
    if(pCache != NULL)
        TPKListFileFindClose((HANDLE)pCache);
    return nError;
}

//-----------------------------------------------------------------------------
// 遍历文件列表内容的每项，搜索文件到文件检索结构中
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

    // 初始化搜索文件的结构数据
    memset(lpFindFileData, 0, sizeof(TPK_FIND_DATA));

    // 如果szListFile为NULL,则使用文件包内部缺省的文件列表名
    if(szListFile == NULL)
    {
        szListFile = TPK_LISTFILE_NAME;
        dwSearchScope = TPK_OPEN_FROM_TPK;
    }

    // 打开文件
    if(nError == ERROR_SUCCESS)
    {
        if(!TPKOpenFile((HANDLE)ha, szListFile, dwSearchScope, &hListFile))
            nError = GetLastError();
    }

    if(nError == ERROR_SUCCESS)
    {
		// 创建Cache大小与文件大小相同
        dwCacheSize = dwFileSize = TPKGetFileSize(hListFile, NULL);

        // 尝试分配整个文件列表内容大小的内存，如果失败则只分配文件部分大小空间
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
         // 初始化文件列表Cache数据
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

        // 获取文件内容数据
        TPKReadFile(hListFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);

        // 初始化操作BUFFER的指针位置
        pCache->pBegin = pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;

        for(;;)
        {
            // 读取文件列表文件内容的每一行数据
            nLength = ReadLine(pCache, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName));
            if(nLength == 0)
            {
                nError = ERROR_NO_MORE_FILES;
                break;
            }

            // 检查可能的一些匹配符
            if(CheckWildCard(lpFindFileData->cFileName, pCache->szMask))
                break;                
        }
    }

    // 清除并退出
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
// 从文件列表Cache里获得下一个文件检索数据
BOOL TPKListFileFindNext(HANDLE hFind, TPK_FIND_DATA* lpFindFileData)
{
    TListFileCache* pCache = (TListFileCache*)hFind;
    size_t nLength;
    BOOL bResult = FALSE;
    int nError = ERROR_SUCCESS;
    for(;;)
    {
        // 读取每一行文件名数据
        nLength = ReadLine(pCache, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName));
        if(nLength == 0)
        {
            nError = ERROR_NO_MORE_FILES;
            break;
        }

        // 检查可能存在的匹配符
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
// 关闭文件列表数据
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