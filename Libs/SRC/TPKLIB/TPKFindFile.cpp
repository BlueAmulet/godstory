#include "TPKLib.h"
#include "TPKCommon.h"

//-----------------------------------------------------------------------------
// 是否有效的检索文件对象句柄
static BOOL IsValidSearchHandle(TPKSearch * hs)
{
    if(hs == NULL || IsBadReadPtr(hs, sizeof(TPKSearch)))
        return FALSE;

    if(!IsValidTPKHandle(hs->ha))
        return FALSE;

    return TRUE;
}

//-----------------------------------------------------------------------------
// 文件包的检索操作
static int DoTPKSearch(TPKSearch* hs, TPK_FIND_DATA* lpFindFileData)
{
    TPKArchive* ha			= hs->ha;
    TPKHash*	pHashEnd	= ha->pHashTable + ha->pHeader->dwHashTableSize;
    TPKHash*	pHash		= ha->pHashTable + hs->dwNextIndex;
	TFileNode*	pNode;

    // 遍历，直到找到文件
    while(pHash < pHashEnd)
    {
        pNode = ha->pListFile[hs->dwNextIndex++];

        // 检查Block索引值和文件列表节点
        if(pHash->dwBlockIndex < DATA_ENTRY_DELETED && (DWORD_PTR)pNode < DATA_ENTRY_DELETED)
        {
            // 检查文件列表节点的文件名与检索对象的匹配符比较
            if(CheckWildCard(pNode->szFileName, hs->szSearchMask))
            {
                TPKBlock* pBlock = ha->pBlockTable + pHash->dwBlockIndex;

                lpFindFileData->dwFileSize   = pBlock->dwFSize;
                lpFindFileData->dwFileFlags  = pBlock->dwFlags;
                lpFindFileData->dwBlockIndex = pHash->dwBlockIndex;
                lpFindFileData->dwCompSize   = pBlock->dwCSize;

                // 设置完整文件名与短文件名
                strcpy(lpFindFileData->cFileName, pNode->szFileName);
                lpFindFileData->szPlainName = strrchr(lpFindFileData->cFileName, '\\');
                if(lpFindFileData->szPlainName == NULL)
                    lpFindFileData->szPlainName = lpFindFileData->cFileName;
                else
                    lpFindFileData->szPlainName++;

                // 成功退出
                return ERROR_SUCCESS;
            }
        }

        pHash++;
    }

    // 没有找到更多的文件，并退出
    return ERROR_NO_MORE_FILES;
}

//-----------------------------------------------------------------------------
// 释放TPKSearch对象
static void FreeTPKSearch(TPKSearch *& hs)
{
    if(hs != NULL)
    {
        FREEMEM(hs);
        hs = NULL;
    }
}

//-----------------------------------------------------------------------------
// 从文件列表里检索匹配的文件
HANDLE WINAPI TPKFindFirstFile(HANDLE hTPK, const char* szMask, TPK_FIND_DATA* lpFindFileData, const char* szListFile)
{
    TPKArchive* ha	= (TPKArchive *)hTPK;
    TPKSearch*	hs	= NULL;             // 文件检索对象
    size_t nSize	= 0;
    int nError = ERROR_SUCCESS;

    // 检查输入参数是否正确
    if(nError == ERROR_SUCCESS)
    {
        if(!IsValidTPKHandle(ha))
            nError = ERROR_INVALID_PARAMETER;
       
        if(szMask == NULL || lpFindFileData == NULL)
            nError = ERROR_INVALID_PARAMETER;

        if(szListFile == NULL && !IsValidTPKHandle(ha))
            nError = ERROR_INVALID_PARAMETER;
    }

    // 添加文件列表文件到文件包内
    if(nError == ERROR_SUCCESS && szListFile != NULL && *szListFile != 0)
        nError = TPKAddListFile((HANDLE)ha, szListFile);

    // 为文件检索对象分配内存
    if(nError == ERROR_SUCCESS)
    {
        nSize = sizeof(TPKSearch) + strlen(szMask) + 1;
        if((hs = (TPKSearch *)ALLOCMEM(char, nSize)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // 执行检索操作
    if(nError == ERROR_SUCCESS)
    {
        memset(hs, 0, sizeof(TPKSearch));
        hs->ha          = ha;
        hs->dwNextIndex = 0;
        strcpy(hs->szSearchMask, szMask);
        nError = DoTPKSearch(hs, lpFindFileData);
    }

    // 清除并退出
    if(nError != ERROR_SUCCESS)
    {
        FreeTPKSearch(hs);
        SetLastError(nError);
    }
    
    // 返回结果
    return (HANDLE)hs;
}

//-----------------------------------------------------------------------------
// 检索下一个文件
BOOL WINAPI TPKFindNextFile(HANDLE hFind, TPK_FIND_DATA * lpFindFileData)
{
    TPKSearch * hs = (TPKSearch *)hFind;
    int nError = ERROR_SUCCESS;

    // 检查输入参数的有效性
    if(nError == ERROR_SUCCESS)
    {
        if(!IsValidSearchHandle(hs) || lpFindFileData == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }
	
	// 执行检索操作
    if(nError == ERROR_SUCCESS)
        nError = DoTPKSearch(hs, lpFindFileData);

	// 如果有错误，报出错误并退出
    if(nError != ERROR_SUCCESS)
    {
        SetLastError(nError);
        return FALSE;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// 关闭检索对象
BOOL WINAPI TPKFindClose(HANDLE hFind)
{
    TPKSearch * hs = (TPKSearch *)hFind;
    // 检查输入参数的有效性
    if(!IsValidSearchHandle(hs))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    FreeTPKSearch(hs);
    return TRUE;
}