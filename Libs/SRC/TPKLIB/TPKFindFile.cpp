#include "TPKLib.h"
#include "TPKCommon.h"

//-----------------------------------------------------------------------------
// �Ƿ���Ч�ļ����ļ�������
static BOOL IsValidSearchHandle(TPKSearch * hs)
{
    if(hs == NULL || IsBadReadPtr(hs, sizeof(TPKSearch)))
        return FALSE;

    if(!IsValidTPKHandle(hs->ha))
        return FALSE;

    return TRUE;
}

//-----------------------------------------------------------------------------
// �ļ����ļ�������
static int DoTPKSearch(TPKSearch* hs, TPK_FIND_DATA* lpFindFileData)
{
    TPKArchive* ha			= hs->ha;
    TPKHash*	pHashEnd	= ha->pHashTable + ha->pHeader->dwHashTableSize;
    TPKHash*	pHash		= ha->pHashTable + hs->dwNextIndex;
	TFileNode*	pNode;

    // ������ֱ���ҵ��ļ�
    while(pHash < pHashEnd)
    {
        pNode = ha->pListFile[hs->dwNextIndex++];

        // ���Block����ֵ���ļ��б�ڵ�
        if(pHash->dwBlockIndex < DATA_ENTRY_DELETED && (DWORD_PTR)pNode < DATA_ENTRY_DELETED)
        {
            // ����ļ��б�ڵ���ļ�������������ƥ����Ƚ�
            if(CheckWildCard(pNode->szFileName, hs->szSearchMask))
            {
                TPKBlock* pBlock = ha->pBlockTable + pHash->dwBlockIndex;

                lpFindFileData->dwFileSize   = pBlock->dwFSize;
                lpFindFileData->dwFileFlags  = pBlock->dwFlags;
                lpFindFileData->dwBlockIndex = pHash->dwBlockIndex;
                lpFindFileData->dwCompSize   = pBlock->dwCSize;

                // ���������ļ�������ļ���
                strcpy(lpFindFileData->cFileName, pNode->szFileName);
                lpFindFileData->szPlainName = strrchr(lpFindFileData->cFileName, '\\');
                if(lpFindFileData->szPlainName == NULL)
                    lpFindFileData->szPlainName = lpFindFileData->cFileName;
                else
                    lpFindFileData->szPlainName++;

                // �ɹ��˳�
                return ERROR_SUCCESS;
            }
        }

        pHash++;
    }

    // û���ҵ�������ļ������˳�
    return ERROR_NO_MORE_FILES;
}

//-----------------------------------------------------------------------------
// �ͷ�TPKSearch����
static void FreeTPKSearch(TPKSearch *& hs)
{
    if(hs != NULL)
    {
        FREEMEM(hs);
        hs = NULL;
    }
}

//-----------------------------------------------------------------------------
// ���ļ��б������ƥ����ļ�
HANDLE WINAPI TPKFindFirstFile(HANDLE hTPK, const char* szMask, TPK_FIND_DATA* lpFindFileData, const char* szListFile)
{
    TPKArchive* ha	= (TPKArchive *)hTPK;
    TPKSearch*	hs	= NULL;             // �ļ���������
    size_t nSize	= 0;
    int nError = ERROR_SUCCESS;

    // �����������Ƿ���ȷ
    if(nError == ERROR_SUCCESS)
    {
        if(!IsValidTPKHandle(ha))
            nError = ERROR_INVALID_PARAMETER;
       
        if(szMask == NULL || lpFindFileData == NULL)
            nError = ERROR_INVALID_PARAMETER;

        if(szListFile == NULL && !IsValidTPKHandle(ha))
            nError = ERROR_INVALID_PARAMETER;
    }

    // ����ļ��б��ļ����ļ�����
    if(nError == ERROR_SUCCESS && szListFile != NULL && *szListFile != 0)
        nError = TPKAddListFile((HANDLE)ha, szListFile);

    // Ϊ�ļ�������������ڴ�
    if(nError == ERROR_SUCCESS)
    {
        nSize = sizeof(TPKSearch) + strlen(szMask) + 1;
        if((hs = (TPKSearch *)ALLOCMEM(char, nSize)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // ִ�м�������
    if(nError == ERROR_SUCCESS)
    {
        memset(hs, 0, sizeof(TPKSearch));
        hs->ha          = ha;
        hs->dwNextIndex = 0;
        strcpy(hs->szSearchMask, szMask);
        nError = DoTPKSearch(hs, lpFindFileData);
    }

    // ������˳�
    if(nError != ERROR_SUCCESS)
    {
        FreeTPKSearch(hs);
        SetLastError(nError);
    }
    
    // ���ؽ��
    return (HANDLE)hs;
}

//-----------------------------------------------------------------------------
// ������һ���ļ�
BOOL WINAPI TPKFindNextFile(HANDLE hFind, TPK_FIND_DATA * lpFindFileData)
{
    TPKSearch * hs = (TPKSearch *)hFind;
    int nError = ERROR_SUCCESS;

    // ��������������Ч��
    if(nError == ERROR_SUCCESS)
    {
        if(!IsValidSearchHandle(hs) || lpFindFileData == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }
	
	// ִ�м�������
    if(nError == ERROR_SUCCESS)
        nError = DoTPKSearch(hs, lpFindFileData);

	// ����д��󣬱��������˳�
    if(nError != ERROR_SUCCESS)
    {
        SetLastError(nError);
        return FALSE;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// �رռ�������
BOOL WINAPI TPKFindClose(HANDLE hFind)
{
    TPKSearch * hs = (TPKSearch *)hFind;
    // ��������������Ч��
    if(!IsValidSearchHandle(hs))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    FreeTPKSearch(hs);
    return TRUE;
}