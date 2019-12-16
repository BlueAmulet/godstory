#include "TPKLib.h"
#include "TPKCommon.h"
COMPACTCB CompactCB  = NULL;
void    * lpUserData = NULL;

/*****************************************************************************/
/* 内部函数			                                                         */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// 克隆文件包的HashTable
static TPKHash* CopyHashTable(TPKArchive* ha)
{
    TPKHash* pHashTableCopy = ALLOCMEM(TPKHash, ha->pHeader->dwHashTableSize);
    if(pHashTableCopy != NULL)
        memcpy(pHashTableCopy, ha->pHashTable, sizeof(TPKHash) * ha->pHeader->dwHashTableSize);
    return pHashTableCopy;
}

//-----------------------------------------------------------------------------
// 检查文件列表所有文件是否都能获取密钥
static int CheckIfAllFilesKnown(TPKArchive* ha, const char* szListFile, DWORD* pFileSeeds)
{
    TPKHash* pHashTableCopy = NULL;		// 待复制的HashTable
    TPKHash* pHash;
    TPKHash* pHashEnd		= NULL;		// HashTable的结束
    DWORD dwFileCount		= 0;
    int nError				= ERROR_SUCCESS;

    // 首先复制HashTable
    if(nError == ERROR_SUCCESS)
    {
        if((pHashTableCopy = CopyHashTable(ha)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pHashEnd = pHashTableCopy + ha->pHeader->dwHashTableSize;

        // 提示给用户，开始检查文件
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_CHECKING_FILES, 0, ha->pHeader->dwHashTableSize);
    }

    // 通过文件列表来遍历所有文件
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

                    // 如果文件有加密，获取密钥
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

            // 提示给用户，检查文件数的变化
            if(CompactCB != NULL)
                CompactCB(lpUserData, CCB_CHECKING_FILES, ++dwFileCount, ha->pHeader->dwHashTableSize);

            // 检索下一个文件
            bResult = TPKFindNextFile(hFind, &wf);
        }

        if(hFind != NULL)
            TPKFindClose(hFind);
    }

    // 当文件列表检查完毕，需要再分析HashTable副本
    if(nError == ERROR_SUCCESS)
    {
        // 提示用户，开始检查HashTable
        dwFileCount = 0;
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_CHECKING_HASH_TABLE, dwFileCount, ha->pHeader->dwHashTableSize);

        for(pHash = pHashTableCopy; pHash < pHashEnd; pHash++)
        {
            // 如果在HashTable副本中仍然有未处理的Hash项，尝试再次获取密钥，如果失败则退出
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

                // 如果文件有加密，但密钥值为0,则失败退出
                if(dwFlags & TPK_FILE_ENCRYPTED && dwSeed == 0)
                {
                    nError = ERROR_CAN_NOT_COMPLETE;
                    break;
                }

				// 记录文件密钥
                pFileSeeds[pHash->dwBlockIndex] = dwSeed;

                // 提示用户，检查HashTable项的变化
                if(CompactCB != NULL)
                    CompactCB(lpUserData, CCB_CHECKING_HASH_TABLE, ++dwFileCount, ha->pHeader->dwHashTableSize);
            }
        }
    }

    // 删除HashTable的副本
    if(pHashTableCopy != NULL)
        FREEMEM(pHashTableCopy);
    return nError;
}

//-----------------------------------------------------------------------------
// 复制所有文件的数据块到另一个文件包
static int CopyTPKFileBlocks(HANDLE hFile, TPKArchive* ha, TPKBlock* pBlock, DWORD dwSeed)
{
    LARGE_INTEGER FilePos = {0};
    DWORD* pdwBlockPos2 = NULL;			// 待写入目标文件的位置数据块缓冲区
    DWORD* pdwBlockPos = NULL;			// 未加密的文件位置数据块缓冲区
    BYTE* pbBlock			= NULL;     // 文件数据块缓冲区
    DWORD dwTransferred;                // 实际写入的字节数
    DWORD dwCSize = 0;                  // 压缩后文件的大小
    DWORD dwBytes = 0;                  // 需要操作的大小
    DWORD dwSeed1 = 0;                  // 临时用于解密的密钥
    DWORD dwSeed2 = 0;                  // 临时用于加密的密钥
    DWORD nBlocks = 0;                  // 文件数据块总数
    DWORD nBlock = 0;                   // 当前文件数据块索引号
    int nError = ERROR_SUCCESS;

    // 文件长度为0,直接返回
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

	// 定位文件包操作位置
    if(nError == ERROR_SUCCESS)
    {
        FilePos.HighPart = pBlock->dwFilePosHigh;
        FilePos.LowPart = pBlock->dwFilePos;
        if(SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN) != FilePos.LowPart)
            nError = GetLastError();
    }

	// 获得当前文件位置
    if(nError == ERROR_SUCCESS)
    {
        FilePos.HighPart = 0;
        FilePos.LowPart = SetFilePointer(hFile, 0, &FilePos.HighPart, FILE_CURRENT);
    }

    // 根据当前文件位置，重新计算密钥
    if(nError == ERROR_SUCCESS && (pBlock->dwFlags & TPK_FILE_ENCRYPTED))
    {
        dwSeed1 = dwSeed;
        if(pBlock->dwFlags & TPK_FILE_FIXSEED)
            dwSeed = (dwSeed1 ^ pBlock->dwFSize) - pBlock->dwFilePos;

        dwSeed2 = dwSeed;
        if(pBlock->dwFlags & TPK_FILE_FIXSEED)
            dwSeed2 = (dwSeed + FilePos.LowPart) ^ pBlock->dwFSize;
    }

    // 从文件包读取文件的位置数据块，重新加密后写入目标文件
    if(pBlock->dwFlags & TPK_FILE_COMPRESSED)
    {
        // 分配位置数据块内存
        if(nError == ERROR_SUCCESS)
        {
            pdwBlockPos = ALLOCMEM(DWORD, nBlocks + 2);
            pdwBlockPos2 = ALLOCMEM(DWORD, nBlocks + 2);
            if(pdwBlockPos == NULL || pdwBlockPos2 == NULL)
                nError = ERROR_NOT_ENOUGH_MEMORY;
        }

        // 读取压缩文件的位置数据块数据
        if(nError == ERROR_SUCCESS)
        {
            dwBytes = (nBlocks + 1) * sizeof(DWORD);
            ReadFile(ha->hFile, pdwBlockPos, dwBytes, &dwTransferred, NULL);
            if(dwTransferred != dwBytes)
                nError = ERROR_FILE_CORRUPT;
        }

        // 解密位置数据块后重新加密
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

        // 写入位置数据块到目标文件
        if(nError == ERROR_SUCCESS)
        {
            WriteFile(hFile, pdwBlockPos2, dwBytes, &dwTransferred, NULL);
            dwCSize += dwTransferred;
            if(dwTransferred != dwBytes)
                nError = ERROR_DISK_FULL;
        }
    }

    // 拷贝文件数据块到目标文件
    if(nError == ERROR_SUCCESS)
    {
        for(nBlock = 0; nBlock < nBlocks; nBlock++)
        {            
            dwBytes = TPK_DATABLOCK_SIZE;
			// 最后一个数据块可能不足4096
            if(nBlock == nBlocks - 1)
            {
                dwBytes = pBlock->dwFSize - (TPK_DATABLOCK_SIZE * (nBlocks - 1));
            }

            if(pBlock->dwFlags & TPK_FILE_COMPRESSED)
                dwBytes = pdwBlockPos[nBlock+1] - pdwBlockPos[nBlock];

            // 从文件包读取文件数据块内容
            ReadFile(ha->hFile, pbBlock, dwBytes, &dwTransferred, NULL);
            if(dwTransferred != dwBytes)
            {
                nError = ERROR_FILE_CORRUPT;
                break;
            }

            // 如有必要，需重新加密文件数据块（但不需要重新压缩，
			// 因为文件数据内容的压缩与其它数据无关联）
            if((pBlock->dwFlags & TPK_FILE_ENCRYPTED) && dwSeed1 != dwSeed2)
            {
                DecrypTPKBlock((DWORD *)pbBlock, dwBytes, dwSeed1 + nBlock);
                EncrypTPKBlock((DWORD *)pbBlock, dwBytes, dwSeed2 + nBlock);
            }

            // 写文件数据块到目标文件
            WriteFile(hFile, pbBlock, dwBytes, &dwTransferred, NULL);
            dwCSize += dwTransferred;
            if(dwTransferred != dwBytes)
            {
                nError = ERROR_DISK_FULL;
                break;
            }
        }
    }

    // 更新记录在BlockTable中的文件位置
    if(nError == ERROR_SUCCESS)
    {
        assert(dwCSize == pBlock->dwCSize);

        // 更新Block项中的文件位置
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
// 复制非文件包的数据
// hSrcFile			源文件句柄
// hTrgFile			目标文件句柄
// DataSizeToCopy	需要复制的数据大小字节
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

        // 读取源文件数据
        ReadFile(hSrcFile, DataBuffer, dwToRead, &dwTransferred, NULL);
        if(dwTransferred != dwToRead)
        {
            nError = ERROR_CAN_NOT_COMPLETE;
            break;
        }

        // 将数据写入目标文件
        WriteFile(hTrgFile, DataBuffer, dwToRead, &dwTransferred, NULL);
        if(dwTransferred != dwToRead)
        {
            nError = ERROR_DISK_FULL;
            break;
        }

        // 递减需要复制的数据大小
        DataSize.QuadPart -= dwTransferred;
    }

    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// 将TPK文件包内所有文件数据复制到目标文件
static int CopyTPKFiles(HANDLE hFile, TPKArchive* ha, DWORD* pFileSeeds)
{
    TPKBlock * pBlock;
    DWORD dwSeed1;
    DWORD dwIndex;
    int nError = ERROR_SUCCESS;

    // 遍历所有文件并写入到目标文件中
    for(dwIndex = 0; dwIndex < ha->pHeader->dwBlockTableSize; dwIndex++)
    {
        pBlock = ha->pBlockTable + dwIndex;
        dwSeed1 = pFileSeeds[dwIndex];

        // 提示用户，开始整理文件包
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_COMPACTING_FILES, dwIndex, ha->pHeader->dwBlockTableSize);

        // 复制所有文件数据块（貌似有BUG)
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
/* 外部共用方法		                                                         */
/*****************************************************************************/
//-----------------------------------------------------------------------------
// 设置回调函数
BOOL WINAPI TPKSetCompactCallback(HANDLE /* hTPK */, COMPACTCB aCompactCB, void* lpData)
{
    CompactCB = aCompactCB;
    lpUserData = lpData;
    return TRUE;
}

//-----------------------------------------------------------------------------
// 整理文件包
BOOL WINAPI TPKCompactArchive(HANDLE hTPK, const char* szListFile, BOOL /* bReserved */)
{
	char szTPKName[MAX_PATH];
	char szTPKCopyName[MAX_PATH];			// 当前TPK文件包的副本
    TPKArchive* ha = (TPKArchive*)hTPK;
	TPKArchive* haCopy = NULL;
	TPKHash* pListFileHash = NULL;			// 文件列表的Hash项
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

    // 如果成功，切换临时文件为当前文件包
    if(nError == ERROR_SUCCESS)
    {
		// 提示用户关闭文件包
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
//// 整理文件包
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
//	// 为存储文件密钥分配内存
//	if(nError == ERROR_SUCCESS)
//	{
//		if((pFileSeeds = ALLOCMEM(DWORD, ha->pHeader->dwHashTableSize)) != NULL)
//			memset(pFileSeeds, 0, sizeof(DWORD) * ha->pHeader->dwHashTableSize);
//		else
//			nError = ERROR_NOT_ENOUGH_MEMORY;
//	}
//
//	// 首先必须检查是否能解密所有文件，否则将不能继续
//	if(nError == ERROR_SUCCESS)
//		nError = CheckIfAllFilesKnown(ha, szListFile, pFileSeeds);
//
//	// 获取临时文件名，并创建此临时文件
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
//	// 写入文件包头数据
//	if(nError == ERROR_SUCCESS)
//	{
//		WriteFile(hFile, ha->pHeader, sizeof(TPKHeader), &dwTransferred, NULL);
//		if(dwTransferred != sizeof(TPKHeader))
//			nError = ERROR_DISK_FULL;
//	}
//
//	// 写入文件包头与第一个文件之间的内容
//	// 所以需要判定第一个文件的位置
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
//		// 寻找第一个文件位置
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
//		// 计算文件包头与第一个文件位置之间数据的大小
//		FirstFilePos.QuadPart -= sizeof(TPKHeader);
//		nError = CopyNonTPKData(ha->hFile, hFile, FirstFilePos);
//	}
//
//	// 写入TPK文件包内所有文件数据
//	if(nError == ERROR_SUCCESS)
//		nError = CopyTPKFiles(hFile, ha, pFileSeeds);
//
//	// 更新HashTable和BlockTable的位置数据
//	if(nError == ERROR_SUCCESS)
//	{
//		LARGE_INTEGER FilePos = {0};
//		// 设置HashTable的位置
//		FilePos.LowPart = SetFilePointer(hFile, 0, &FilePos.HighPart, FILE_CURRENT);
//		ha->pHeader->dwHashTablePosHigh = (USHORT)FilePos.HighPart;
//		ha->pHeader->dwHashTablePos = FilePos.LowPart;
//		ha->HashTablePos = FilePos;
//
//		// 设置BlockTable的位置
//		FilePos.QuadPart += ha->pHeader->dwHashTableSize * sizeof(TPKHash);
//		ha->pHeader->dwBlockTablePosHigh = (USHORT)FilePos.HighPart;
//		ha->pHeader->dwBlockTablePos = FilePos.LowPart;
//		ha->BlockTablePos = FilePos;
//
//		// 文件总字节大小
//		FilePos.QuadPart += ha->pHeader->dwBlockTableSize * sizeof(TPKBlock);
//		ha->TPKSize = FilePos;
//	}
//
//	// 如果成功，更新目标文件为文件包的文件句柄，并且保存表数据
//	if(nError == ERROR_SUCCESS)
//	{
//		CloseHandle(ha->hFile);
//		ha->hFile = hFile;
//		hFile = INVALID_HANDLE_VALUE;
//		nError = SaveTPKTables(ha);
//	}
//
//	// 如果成功，切换临时文件为当前文件包
//	if(nError == ERROR_SUCCESS)
//	{
//		// 提示用户关闭文件包
//		if(CompactCB != NULL)
//			CompactCB(lpUserData, CCB_CLOSING_ARCHIVE, 0, 0);
//
//		if(!DeleteFile(ha->szFileName) ||           // 删除旧文件包
//			!CloseHandle(ha->hFile)     ||           // 关闭文件包
//			!MoveFile(szTempFile, ha->szFileName))   // 重命名临时文件为文件包名
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
