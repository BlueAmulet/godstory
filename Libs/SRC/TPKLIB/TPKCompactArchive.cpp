#include "TPKLib.h"
#include "TPKCommon.h"
COMPACTCB CompactCB  = NULL;
void    * lpUserData = NULL;

/*****************************************************************************/
/* �ڲ�����			                                                         */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// ��¡�ļ�����HashTable
static TPKHash* CopyHashTable(TPKArchive* ha)
{
    TPKHash* pHashTableCopy = ALLOCMEM(TPKHash, ha->pHeader->dwHashTableSize);
    if(pHashTableCopy != NULL)
        memcpy(pHashTableCopy, ha->pHashTable, sizeof(TPKHash) * ha->pHeader->dwHashTableSize);
    return pHashTableCopy;
}

//-----------------------------------------------------------------------------
// ����ļ��б������ļ��Ƿ��ܻ�ȡ��Կ
static int CheckIfAllFilesKnown(TPKArchive* ha, const char* szListFile, DWORD* pFileSeeds)
{
    TPKHash* pHashTableCopy = NULL;		// �����Ƶ�HashTable
    TPKHash* pHash;
    TPKHash* pHashEnd		= NULL;		// HashTable�Ľ���
    DWORD dwFileCount		= 0;
    int nError				= ERROR_SUCCESS;

    // ���ȸ���HashTable
    if(nError == ERROR_SUCCESS)
    {
        if((pHashTableCopy = CopyHashTable(ha)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pHashEnd = pHashTableCopy + ha->pHeader->dwHashTableSize;

        // ��ʾ���û�����ʼ����ļ�
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_CHECKING_FILES, 0, ha->pHeader->dwHashTableSize);
    }

    // ͨ���ļ��б������������ļ�
    if(nError == ERROR_SUCCESS)
    {
        TPK_FIND_DATA wf;
        HANDLE hFind = TPKFindFirstFile((HANDLE)ha, "*", &wf, szListFile);
        BOOL bResult = TRUE;

        while(hFind != NULL && bResult)
        {
            TPKHash* pHash = GetHashEntry(ha, wf.cFileName);
            if(pHash != NULL)
            {
                pHash = pHashTableCopy + (pHash - ha->pHashTable);
                if(pHash->dwName1 != (DWORD)-1 && pHash->dwName2 != (DWORD)-1)
                {
                    TPKBlock* pBlock = ha->pBlockTable + pHash->dwBlockIndex;
                    DWORD dwSeed = 0;

                    // ����ļ��м��ܣ���ȡ��Կ
                    if(pBlock->dwFlags & TPK_FILE_ENCRYPTED)
                    {
                        char* szFileName = strrchr(wf.cFileName, '\\');
                        if(szFileName == NULL)
                            szFileName = wf.cFileName;
                        else
                            szFileName++;

                        dwSeed = DecryptFileSeed(szFileName);
                        if(pBlock->dwFlags & TPK_FILE_FIXSEED)
                            dwSeed = (dwSeed + pBlock->dwFilePos) ^ pBlock->dwFSize;
                    }
                    pFileSeeds[pHash->dwBlockIndex] = dwSeed;

                    pHash->dwName1      = 0xFFFFFFFF;
                    pHash->dwName2      = 0xFFFFFFFF;
                    pHash->dwBlockIndex = 0xFFFFFFFF;
                }
            }

            // ��ʾ���û�������ļ����ı仯
            if(CompactCB != NULL)
                CompactCB(lpUserData, CCB_CHECKING_FILES, ++dwFileCount, ha->pHeader->dwHashTableSize);

            // ������һ���ļ�
            bResult = TPKFindNextFile(hFind, &wf);
        }

        if(hFind != NULL)
            TPKFindClose(hFind);
    }

    // ���ļ��б�����ϣ���Ҫ�ٷ���HashTable����
    if(nError == ERROR_SUCCESS)
    {
        // ��ʾ�û�����ʼ���HashTable
        dwFileCount = 0;
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_CHECKING_HASH_TABLE, dwFileCount, ha->pHeader->dwHashTableSize);

        for(pHash = pHashTableCopy; pHash < pHashEnd; pHash++)
        {
            // �����HashTable��������Ȼ��δ�����Hash������ٴλ�ȡ��Կ�����ʧ�����˳�
            if(pHash->dwName1 != (DWORD)-1 && pHash->dwName2 != (DWORD)-1)
            {
                HANDLE hFile  = NULL;
                DWORD dwFlags = 0;
                DWORD dwSeed  = 0;

                if(TPKOpenFile((HANDLE)ha, (char*)(DWORD_PTR)pHash->dwBlockIndex, TPK_OPEN_BY_INDEX, &hFile))
                {
                    TPKFile* hf = (TPKFile*)hFile;
                    dwFlags = hf->pBlock->dwFlags;
                    dwSeed = hf->dwSeed1;
                    TPKCloseFile(hFile);
                }

                // ����ļ��м��ܣ�����ԿֵΪ0,��ʧ���˳�
                if(dwFlags & TPK_FILE_ENCRYPTED && dwSeed == 0)
                {
                    nError = ERROR_CAN_NOT_COMPLETE;
                    break;
                }

				// ��¼�ļ���Կ
                pFileSeeds[pHash->dwBlockIndex] = dwSeed;

                // ��ʾ�û������HashTable��ı仯
                if(CompactCB != NULL)
                    CompactCB(lpUserData, CCB_CHECKING_HASH_TABLE, ++dwFileCount, ha->pHeader->dwHashTableSize);
            }
        }
    }

    // ɾ��HashTable�ĸ���
    if(pHashTableCopy != NULL)
        FREEMEM(pHashTableCopy);
    return nError;
}

//-----------------------------------------------------------------------------
// ���������ļ������ݿ鵽��һ���ļ���
static int CopyTPKFileBlocks(HANDLE hFile, TPKArchive* ha, TPKBlock* pBlock, DWORD dwSeed)
{
    LARGE_INTEGER FilePos = {0};
    DWORD* pdwBlockPos2 = NULL;			// ��д��Ŀ���ļ���λ�����ݿ黺����
    DWORD* pdwBlockPos = NULL;			// δ���ܵ��ļ�λ�����ݿ黺����
    BYTE* pbBlock			= NULL;     // �ļ����ݿ黺����
    DWORD dwTransferred;                // ʵ��д����ֽ���
    DWORD dwCSize = 0;                  // ѹ�����ļ��Ĵ�С
    DWORD dwBytes = 0;                  // ��Ҫ�����Ĵ�С
    DWORD dwSeed1 = 0;                  // ��ʱ���ڽ��ܵ���Կ
    DWORD dwSeed2 = 0;                  // ��ʱ���ڼ��ܵ���Կ
    DWORD nBlocks = 0;                  // �ļ����ݿ�����
    DWORD nBlock = 0;                   // ��ǰ�ļ����ݿ�������
    int nError = ERROR_SUCCESS;

    // �ļ�����Ϊ0,ֱ�ӷ���
    if(pBlock->dwFSize == 0)
        return ERROR_SUCCESS;

    if(nError == ERROR_SUCCESS)
    {
        nBlocks = pBlock->dwFSize / TPK_DATABLOCK_SIZE;
        if(pBlock->dwFSize % TPK_DATABLOCK_SIZE)
            nBlocks++;
        pbBlock = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE);
        if(pbBlock == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

	// ��λ�ļ�������λ��
    if(nError == ERROR_SUCCESS)
    {
        FilePos.HighPart = pBlock->dwFilePosHigh;
        FilePos.LowPart = pBlock->dwFilePos;
        if(SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN) != FilePos.LowPart)
            nError = GetLastError();
    }

	// ��õ�ǰ�ļ�λ��
    if(nError == ERROR_SUCCESS)
    {
        FilePos.HighPart = 0;
        FilePos.LowPart = SetFilePointer(hFile, 0, &FilePos.HighPart, FILE_CURRENT);
    }

    // ���ݵ�ǰ�ļ�λ�ã����¼�����Կ
    if(nError == ERROR_SUCCESS && (pBlock->dwFlags & TPK_FILE_ENCRYPTED))
    {
        dwSeed1 = dwSeed;
        if(pBlock->dwFlags & TPK_FILE_FIXSEED)
            dwSeed = (dwSeed1 ^ pBlock->dwFSize) - pBlock->dwFilePos;

        dwSeed2 = dwSeed;
        if(pBlock->dwFlags & TPK_FILE_FIXSEED)
            dwSeed2 = (dwSeed + FilePos.LowPart) ^ pBlock->dwFSize;
    }

    // ���ļ�����ȡ�ļ���λ�����ݿ飬���¼��ܺ�д��Ŀ���ļ�
    if(pBlock->dwFlags & TPK_FILE_COMPRESSED)
    {
        // ����λ�����ݿ��ڴ�
        if(nError == ERROR_SUCCESS)
        {
            pdwBlockPos = ALLOCMEM(DWORD, nBlocks + 2);
            pdwBlockPos2 = ALLOCMEM(DWORD, nBlocks + 2);
            if(pdwBlockPos == NULL || pdwBlockPos2 == NULL)
                nError = ERROR_NOT_ENOUGH_MEMORY;
        }

        // ��ȡѹ���ļ���λ�����ݿ�����
        if(nError == ERROR_SUCCESS)
        {
            dwBytes = (nBlocks + 1) * sizeof(DWORD);
            ReadFile(ha->hFile, pdwBlockPos, dwBytes, &dwTransferred, NULL);
            if(dwTransferred != dwBytes)
                nError = ERROR_FILE_CORRUPT;
        }

        // ����λ�����ݿ�����¼���
        if(nError == ERROR_SUCCESS)
        {
            if(pBlock->dwFlags & TPK_FILE_ENCRYPTED)
            {
                DecrypTPKBlock(pdwBlockPos, dwBytes, dwSeed1 - 1);
                if(pdwBlockPos[0] != dwBytes)
                    nError = ERROR_FILE_CORRUPT;
            
                memcpy(pdwBlockPos2, pdwBlockPos, dwBytes);
                EncrypTPKBlock(pdwBlockPos2, dwBytes, dwSeed2 - 1);
            }
            else
            {
                memcpy(pdwBlockPos2, pdwBlockPos, dwBytes);
            }
        }

        // д��λ�����ݿ鵽Ŀ���ļ�
        if(nError == ERROR_SUCCESS)
        {
            WriteFile(hFile, pdwBlockPos2, dwBytes, &dwTransferred, NULL);
            dwCSize += dwTransferred;
            if(dwTransferred != dwBytes)
                nError = ERROR_DISK_FULL;
        }
    }

    // �����ļ����ݿ鵽Ŀ���ļ�
    if(nError == ERROR_SUCCESS)
    {
        for(nBlock = 0; nBlock < nBlocks; nBlock++)
        {            
            dwBytes = TPK_DATABLOCK_SIZE;
			// ���һ�����ݿ���ܲ���4096
            if(nBlock == nBlocks - 1)
            {
                dwBytes = pBlock->dwFSize - (TPK_DATABLOCK_SIZE * (nBlocks - 1));
            }

            if(pBlock->dwFlags & TPK_FILE_COMPRESSED)
                dwBytes = pdwBlockPos[nBlock+1] - pdwBlockPos[nBlock];

            // ���ļ�����ȡ�ļ����ݿ�����
            ReadFile(ha->hFile, pbBlock, dwBytes, &dwTransferred, NULL);
            if(dwTransferred != dwBytes)
            {
                nError = ERROR_FILE_CORRUPT;
                break;
            }

            // ���б�Ҫ�������¼����ļ����ݿ飨������Ҫ����ѹ����
			// ��Ϊ�ļ��������ݵ�ѹ�������������޹�����
            if((pBlock->dwFlags & TPK_FILE_ENCRYPTED) && dwSeed1 != dwSeed2)
            {
                DecrypTPKBlock((DWORD *)pbBlock, dwBytes, dwSeed1 + nBlock);
                EncrypTPKBlock((DWORD *)pbBlock, dwBytes, dwSeed2 + nBlock);
            }

            // д�ļ����ݿ鵽Ŀ���ļ�
            WriteFile(hFile, pbBlock, dwBytes, &dwTransferred, NULL);
            dwCSize += dwTransferred;
            if(dwTransferred != dwBytes)
            {
                nError = ERROR_DISK_FULL;
                break;
            }
        }
    }

    // ���¼�¼��BlockTable�е��ļ�λ��
    if(nError == ERROR_SUCCESS)
    {
        assert(dwCSize == pBlock->dwCSize);

        // ����Block���е��ļ�λ��
        pBlock->dwFilePosHigh = (USHORT)FilePos.HighPart;
        pBlock->dwFilePos = FilePos.LowPart;
    }

    if(pdwBlockPos2 != NULL)
        FREEMEM(pdwBlockPos2);
    if(pdwBlockPos != NULL)
        FREEMEM(pdwBlockPos);
    if(pbBlock != NULL)
        FREEMEM(pbBlock);
    return nError;
}

//-----------------------------------------------------------------------------
// ���Ʒ��ļ���������
// hSrcFile			Դ�ļ����
// hTrgFile			Ŀ���ļ����
// DataSizeToCopy	��Ҫ���Ƶ����ݴ�С�ֽ�
static int CopyNonTPKData(HANDLE hSrcFile, HANDLE hTrgFile, LARGE_INTEGER& DataSizeToCopy)
{
    LARGE_INTEGER DataSize = DataSizeToCopy;
    DWORD dwTransferred;
    DWORD dwToRead;
    char DataBuffer[0x1000];
    int nError = ERROR_SUCCESS;

    while(DataSize.QuadPart > 0)
    {
        dwToRead = sizeof(DataBuffer);
        if(DataSize.HighPart == 0 && DataSize.LowPart < dwToRead)
            dwToRead = DataSize.LowPart;

        // ��ȡԴ�ļ�����
        ReadFile(hSrcFile, DataBuffer, dwToRead, &dwTransferred, NULL);
        if(dwTransferred != dwToRead)
        {
            nError = ERROR_CAN_NOT_COMPLETE;
            break;
        }

        // ������д��Ŀ���ļ�
        WriteFile(hTrgFile, DataBuffer, dwToRead, &dwTransferred, NULL);
        if(dwTransferred != dwToRead)
        {
            nError = ERROR_DISK_FULL;
            break;
        }

        // �ݼ���Ҫ���Ƶ����ݴ�С
        DataSize.QuadPart -= dwTransferred;
    }

    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// ��TPK�ļ����������ļ����ݸ��Ƶ�Ŀ���ļ�
static int CopyTPKFiles(HANDLE hFile, TPKArchive* ha, DWORD* pFileSeeds)
{
    TPKBlock * pBlock;
    DWORD dwSeed1;
    DWORD dwIndex;
    int nError = ERROR_SUCCESS;

    // ���������ļ���д�뵽Ŀ���ļ���
    for(dwIndex = 0; dwIndex < ha->pHeader->dwBlockTableSize; dwIndex++)
    {
        pBlock = ha->pBlockTable + dwIndex;
        dwSeed1 = pFileSeeds[dwIndex];

        // ��ʾ�û�����ʼ�����ļ���
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_COMPACTING_FILES, dwIndex, ha->pHeader->dwBlockTableSize);

        // ���������ļ����ݿ飨ò����BUG)
        if(pBlock->dwFlags & TPK_FILE_EXISTS)
        {
            nError = CopyTPKFileBlocks(hFile, ha, pBlock, dwSeed1);
            if(nError != ERROR_SUCCESS)
                break;
        }
    }

    // Cleanup and exit
    return nError;
}


/*****************************************************************************/
/* �ⲿ���÷���		                                                         */
/*****************************************************************************/
//-----------------------------------------------------------------------------
// ���ûص�����
BOOL WINAPI TPKSetCompactCallback(HANDLE /* hTPK */, COMPACTCB aCompactCB, void* lpData)
{
    CompactCB = aCompactCB;
    lpUserData = lpData;
    return TRUE;
}

//-----------------------------------------------------------------------------
// �����ļ���
BOOL WINAPI TPKCompactArchive(HANDLE hTPK, const char* szListFile, BOOL /* bReserved */)
{
	char szTPKName[MAX_PATH];
	char szTPKCopyName[MAX_PATH];			// ��ǰTPK�ļ����ĸ���
    TPKArchive* ha = (TPKArchive*)hTPK;
	TPKArchive* haCopy = NULL;
	TPKHash* pListFileHash = NULL;			// �ļ��б��Hash��
    HANDLE hFile = INVALID_HANDLE_VALUE;
    int nError = ERROR_SUCCESS;

    if(!IsValidTPKHandle(ha))
        nError = ERROR_INVALID_PARAMETER;

	if(nError == ERROR_SUCCESS)
	{
		char * szTemp = NULL;
		strcpy(szTPKName, ha->szFileName);
		strcpy(szTPKCopyName, ha->szFileName);
		if((szTemp = strrchr(szTPKCopyName, '.')) != NULL)
			strcpy(szTemp, "_copy.tpk");
		BOOL ret = TPKCreateArchive(szTPKCopyName, CREATE_ALWAYS, ha->pHeader->dwHashTableSize, (HANDLE*)&haCopy);
		if(ret == FALSE)
			nError = ERROR_FILE_INVALID;
	}

	if(nError == ERROR_SUCCESS)
	{
		pListFileHash = GetHashEntry(ha, TPK_LISTFILE_NAME);
		if(pListFileHash == NULL)
			nError = ERROR_FILE_NOT_FOUND;
	}

	if(nError == ERROR_SUCCESS)
	{
		TPKHash*	pHashBegin  = ha->pHashTable;
		TPKHash*	pHash		= pHashBegin;
		TFileNode*	pNode;
		for(DWORD i = 0; i < ha->pHeader->dwHashTableSize; i++)
		{
			if(pHash == pListFileHash)
			{
				pHash++;
				continue;
			}

			pNode = ha->pListFile[i];
			if(pHash->dwBlockIndex < DATA_ENTRY_DELETED && (DWORD_PTR)pNode < DATA_ENTRY_DELETED)
			{
				BOOL ret = TPKOpenFile(hTPK, pNode->szFileName, 0, &hFile);
				if(ret == FALSE)
				{
					nError = ERROR_OPEN_FAILED;
					break;
				}

				ret = TPKCopyFile(hFile, haCopy, pNode->szFileName);
				if(ret == FALSE)
				{
					nError = ERROR_FILE_CORRUPT;
					break;
				}				
			}
			pHash++;
		}
	}

    // ����ɹ����л���ʱ�ļ�Ϊ��ǰ�ļ���
    if(nError == ERROR_SUCCESS)
    {
		// ��ʾ�û��ر��ļ���
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_CLOSING_ARCHIVE, 0, 0);

		TPKCloseArchive(haCopy);
		TPKCloseArchive(ha);

        if(!DeleteFile(szTPKName) || !MoveFile(szTPKCopyName, szTPKName))
            nError = GetLastError();
    }

    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    DeleteFile(szTPKCopyName);
    CompactCB = NULL;
    return (nError == ERROR_SUCCESS);
}


////-----------------------------------------------------------------------------
//// �����ļ���
//BOOL WINAPI TPKCompactArchive(HANDLE hTPK, const char* szListFile, BOOL /* bReserved */)
//{
//	TPKArchive* ha = (TPKArchive*)hTPK;
//	HANDLE hFile = INVALID_HANDLE_VALUE;
//	DWORD * pFileSeeds = NULL;
//	char szTempFile[MAX_PATH] = "";
//	char * szTemp = NULL;
//	DWORD dwTransferred;
//	int nError = ERROR_SUCCESS;
//
//	if(!IsValidTPKHandle(ha))
//		nError = ERROR_INVALID_PARAMETER;
//
//	// Ϊ�洢�ļ���Կ�����ڴ�
//	if(nError == ERROR_SUCCESS)
//	{
//		if((pFileSeeds = ALLOCMEM(DWORD, ha->pHeader->dwHashTableSize)) != NULL)
//			memset(pFileSeeds, 0, sizeof(DWORD) * ha->pHeader->dwHashTableSize);
//		else
//			nError = ERROR_NOT_ENOUGH_MEMORY;
//	}
//
//	// ���ȱ������Ƿ��ܽ��������ļ������򽫲��ܼ���
//	if(nError == ERROR_SUCCESS)
//		nError = CheckIfAllFilesKnown(ha, szListFile, pFileSeeds);
//
//	// ��ȡ��ʱ�ļ���������������ʱ�ļ�
//	if(nError == ERROR_SUCCESS)
//	{
//		if(CompactCB != NULL)
//			CompactCB(lpUserData, CCB_COPYING_NON_TPK_DATA, 0, 0);
//
//		strcpy(szTempFile, ha->szFileName);
//		if((szTemp = strrchr(szTempFile, '.')) != NULL)
//			strcpy(szTemp + 1, "mp_");
//		else
//			strcat(szTempFile, "_");
//
//		hFile = CreateFile(szTempFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
//		if(hFile == INVALID_HANDLE_VALUE)
//			nError = GetLastError();
//	}
//
//	// д���ļ���ͷ����
//	if(nError == ERROR_SUCCESS)
//	{
//		WriteFile(hFile, ha->pHeader, sizeof(TPKHeader), &dwTransferred, NULL);
//		if(dwTransferred != sizeof(TPKHeader))
//			nError = ERROR_DISK_FULL;
//	}
//
//	// д���ļ���ͷ���һ���ļ�֮�������
//	// ������Ҫ�ж���һ���ļ���λ��
//	if(nError == ERROR_SUCCESS)
//	{
//		LARGE_INTEGER FirstFilePos;
//		LARGE_INTEGER TempPos;
//		TPKBlock* pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
//		TPKBlock* pBlock = ha->pBlockTable;
//
//		FirstFilePos.HighPart = 0x7FFFFFFF;
//		FirstFilePos.LowPart = 0xFFFFFFFF;
//
//		// Ѱ�ҵ�һ���ļ�λ��
//		while(pBlock < pBlockEnd)
//		{
//			if(pBlock->dwFlags & TPK_FILE_EXISTS)
//			{
//				TempPos.HighPart = pBlock->dwFilePosHigh;
//				TempPos.LowPart = pBlock->dwFilePos;
//				if(TempPos.QuadPart < FirstFilePos.QuadPart)
//					FirstFilePos = TempPos;
//			}            
//
//			pBlock++;
//		}
//
//		TempPos.QuadPart = sizeof(TPKHeader);
//		SetFilePointer(ha->hFile, TempPos.LowPart, &TempPos.HighPart, FILE_BEGIN);
//
//		// �����ļ���ͷ���һ���ļ�λ��֮�����ݵĴ�С
//		FirstFilePos.QuadPart -= sizeof(TPKHeader);
//		nError = CopyNonTPKData(ha->hFile, hFile, FirstFilePos);
//	}
//
//	// д��TPK�ļ����������ļ�����
//	if(nError == ERROR_SUCCESS)
//		nError = CopyTPKFiles(hFile, ha, pFileSeeds);
//
//	// ����HashTable��BlockTable��λ������
//	if(nError == ERROR_SUCCESS)
//	{
//		LARGE_INTEGER FilePos = {0};
//		// ����HashTable��λ��
//		FilePos.LowPart = SetFilePointer(hFile, 0, &FilePos.HighPart, FILE_CURRENT);
//		ha->pHeader->dwHashTablePosHigh = (USHORT)FilePos.HighPart;
//		ha->pHeader->dwHashTablePos = FilePos.LowPart;
//		ha->HashTablePos = FilePos;
//
//		// ����BlockTable��λ��
//		FilePos.QuadPart += ha->pHeader->dwHashTableSize * sizeof(TPKHash);
//		ha->pHeader->dwBlockTablePosHigh = (USHORT)FilePos.HighPart;
//		ha->pHeader->dwBlockTablePos = FilePos.LowPart;
//		ha->BlockTablePos = FilePos;
//
//		// �ļ����ֽڴ�С
//		FilePos.QuadPart += ha->pHeader->dwBlockTableSize * sizeof(TPKBlock);
//		ha->TPKSize = FilePos;
//	}
//
//	// ����ɹ�������Ŀ���ļ�Ϊ�ļ������ļ���������ұ��������
//	if(nError == ERROR_SUCCESS)
//	{
//		CloseHandle(ha->hFile);
//		ha->hFile = hFile;
//		hFile = INVALID_HANDLE_VALUE;
//		nError = SaveTPKTables(ha);
//	}
//
//	// ����ɹ����л���ʱ�ļ�Ϊ��ǰ�ļ���
//	if(nError == ERROR_SUCCESS)
//	{
//		// ��ʾ�û��ر��ļ���
//		if(CompactCB != NULL)
//			CompactCB(lpUserData, CCB_CLOSING_ARCHIVE, 0, 0);
//
//		if(!DeleteFile(ha->szFileName) ||           // ɾ�����ļ���
//			!CloseHandle(ha->hFile)     ||           // �ر��ļ���
//			!MoveFile(szTempFile, ha->szFileName))   // ��������ʱ�ļ�Ϊ�ļ�����
//			nError = GetLastError();
//	}
//
//	if(nError == ERROR_SUCCESS)
//	{
//		ha->hFile = CreateFile(ha->szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
//		if(ha->hFile == INVALID_HANDLE_VALUE)
//			nError = GetLastError();
//	}
//
//	if(nError == ERROR_SUCCESS)
//	{
//		ha->pLastFile  = NULL;
//		ha->dwBlockPos = 0;
//		ha->dwBuffPos  = 0;
//	}
//
//	if(hFile != INVALID_HANDLE_VALUE)
//		CloseHandle(hFile);
//	if(pFileSeeds != NULL)
//		FREEMEM(pFileSeeds);
//	if(nError != ERROR_SUCCESS)
//		SetLastError(nError);
//	DeleteFile(szTempFile);
//	CompactCB = NULL;
//	return (nError == ERROR_SUCCESS);
//}
//
