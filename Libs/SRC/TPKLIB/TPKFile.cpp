#include "TPKLib.h"
#include "TPKCommon.h"
#include "crc32.h"
#include <string>
#include <vector>

/********************************************************************************/
/* 内部函数																		*/
/********************************************************************************/
// 打开本地文件
static BOOL OpenLocalFile(const char* szFileName, HANDLE* phFile)
{
    TPKFile* hf = NULL;
    HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        // 为TPKFile对象分配内存
        size_t nHandleSize = sizeof(TPKFile) + strlen(szFileName); 
        if((hf = (TPKFile*)ALLOCMEM(char, nHandleSize)) != NULL)
        {
            memset(hf, 0, nHandleSize);
            strcpy(hf->szFileName, szFileName);
            hf->hFile = hFile;
            *phFile   = hf;
            return TRUE;
        }
        else
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    }
    *phFile = NULL;
    return FALSE;
}

//-----------------------------------------------------------------------------
// 释放TPKFile对象
void FreeTPKFile(TPKFile *& hf)
{
    if(hf != NULL)
    {
        if(hf->hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hf->hFile);
        if(hf->pdwBlockPos != NULL)
            FREEMEM(hf->pdwBlockPos);
        if(hf->pbFileBuffer != NULL)
            FREEMEM(hf->pbFileBuffer);
        FREEMEM(hf);
        hf = NULL;
    }
}

//-----------------------------------------------------------------------------
// 判断文件是否在文件包内
BOOL WINAPI TPKHaTPK(HANDLE hTPK, char* szFileName)
{
    TPKArchive* ha = (TPKArchive*)hTPK;
    int nError = ERROR_SUCCESS;
    if(nError == ERROR_SUCCESS)
    {
        if(ha == NULL || szFileName == NULL || *szFileName == 0)
            nError = ERROR_INVALID_PARAMETER;
    }

    // 检查是否有此文件的Hash项
    if(nError == ERROR_SUCCESS)
    {
        if(GetHashEntry(ha, szFileName) == NULL)
        {
            nError = ERROR_FILE_NOT_FOUND;
        }
    }

    // 如果有错误就报错
    if(nError != ERROR_SUCCESS)
    {
        SetLastError(nError);
    }
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// 添加文件到文件包内(仅增加Hash项、Block项和文件列表项而已)
int AddInternalFile(TPKArchive* ha, const char* szFileName)
{
    TPKBlock*	pBlockEnd;
    TPKBlock*	pBlock;
    TPKHash*	pHash;
    int nError = ERROR_SUCCESS;

    // 检查文件是否已经存在在文件包里
    pHash = GetHashEntry(ha, szFileName);
    if(pHash == NULL)
    {
		// 查找空闲的Hash项
        pHash = FindFreeHashEntry(ha, szFileName);
        if(pHash != NULL)
        {
			// 查找空闲的Block项
            pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
            for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
            {
                if((pBlock->dwFlags & TPK_FILE_EXISTS) == 0)
                {
					pHash->dwBlockIndex = (DWORD)(pBlock - ha->pBlockTable);
					pBlock->dwFilePosHigh = (USHORT)ha->HashTablePos.HighPart;
					pBlock->dwFilePos = ha->HashTablePos.LowPart;
					pBlock->dwFSize   = 0;
					pBlock->dwCSize   = 0;
					pBlock->dwFlags   = TPK_FILE_EXISTS;
					// 添加文件名到文件列表中
					return TPKListFileCreateNode(ha, szFileName);
                }
            }
        }

		nError = ERROR_HANDLE_DISK_FULL;
    }
    return nError;
}

// -------------------------------------------------------------------------
//重新加密文件数据
static int RecryptFileData(TPKArchive* ha, DWORD dwSaveBlockIndex, const char* szFileName, const char* szNewFileName)
{
	LARGE_INTEGER BlockFilePos;
	LARGE_INTEGER TPKFilePos;
	TPKBlock * pBlock = ha->pBlockTable + dwSaveBlockIndex;
	const char * szPlainName;
	LPDWORD pdwBlockPos1 = NULL;
	LPDWORD pdwBlockPos2 = NULL;
	LPBYTE pbFileBlock = NULL;
	DWORD dwTransferred;
	DWORD dwOldSeed;
	DWORD dwNewSeed;
	DWORD dwToRead;
	int nBlocks;
	int nError = ERROR_SUCCESS;

	// 文件必须被加密过的
	if((pBlock->dwFlags & TPK_FILE_ENCRYPTED) == 0)
		return ERROR_INVALID_BLOCK;

	// 加密密钥所用的文件名是不含路径的，所以要先截取
	szPlainName = strrchr(szFileName, '\\');
	if(szPlainName != NULL)
		szFileName = szPlainName + 1;
	szPlainName = strrchr(szNewFileName, '\\');
	if(szPlainName != NULL)
		szNewFileName = szPlainName + 1;

	// 计算加密密钥
	dwOldSeed = DecryptFileSeed(szFileName);
	dwNewSeed = DecryptFileSeed(szNewFileName);
	if(pBlock->dwFlags & TPK_FILE_FIXSEED)
	{
		dwOldSeed = (dwOldSeed + pBlock->dwFilePos) ^ pBlock->dwFSize;
		dwNewSeed = (dwNewSeed + pBlock->dwFilePos) ^ pBlock->dwFSize;
	}

	// 如果密钥相同，则无需重新加密了
	if(dwNewSeed == dwOldSeed)
		return ERROR_SUCCESS;

	// 获得文件的固定位置
	TPKFilePos.LowPart = pBlock->dwFilePos;
	TPKFilePos.HighPart = pBlock->dwFilePosHigh;

	// 计算文件的数据块数
	nBlocks = pBlock->dwFSize / TPK_DATABLOCK_SIZE;
	if(pBlock->dwFSize % TPK_DATABLOCK_SIZE)
		nBlocks++;

	// 如果这个文件仅做一个单独数据块存储的，则只需处理一个块
	if(pBlock->dwFlags & TPK_FILE_SINGLE_UNIT)
	{
		// 分配文件数据块缓冲区
		pbFileBlock = ALLOCMEM(BYTE, pBlock->dwCSize);
		if(pbFileBlock == NULL)
			return ERROR_NOT_ENOUGH_MEMORY;

		SetFilePointer(ha->hFile, TPKFilePos.LowPart, &TPKFilePos.HighPart, FILE_BEGIN);
		ReadFile(ha->hFile, pbFileBlock, pBlock->dwCSize, &dwTransferred, NULL);
		if(dwTransferred == pBlock->dwCSize)
			nError = ERROR_FILE_CORRUPT;

		if(nError == ERROR_SUCCESS)
		{
			// 重新加密数据块
			DecrypTPKBlock((DWORD*)pbFileBlock, pBlock->dwCSize, dwOldSeed);
			EncrypTPKBlock((DWORD*)pbFileBlock, pBlock->dwCSize, dwNewSeed);

			// 再次写回文件包本地文件
			SetFilePointer(ha->hFile, TPKFilePos.LowPart, &TPKFilePos.HighPart, FILE_BEGIN);
			WriteFile(ha->hFile, pbFileBlock, pBlock->dwCSize, &dwTransferred, NULL);
			if(dwTransferred != pBlock->dwCSize)
				nError = ERROR_WRITE_FAULT;
		}
		FREEMEM(pbFileBlock);
		return nError;
	}

	// 如果文件是经过压缩的，则必须先重新加密过位置数据块内容，再加密数据块内容
	if(pBlock->dwFlags & TPK_FILE_COMPRESSED)
	{
		// 分配文件的位置数据块缓冲区
		pdwBlockPos1 = ALLOCMEM(DWORD, nBlocks + 2);
		pdwBlockPos2 = ALLOCMEM(DWORD, nBlocks + 2);
		if(pdwBlockPos1 == NULL || pdwBlockPos2 == NULL)
			return ERROR_NOT_ENOUGH_MEMORY;

		// 计算需要读取的位置块数据大小
		dwToRead = (nBlocks + 1) * sizeof(DWORD);

		// 读取位置数据块内容
		SetFilePointer(ha->hFile, TPKFilePos.LowPart, &TPKFilePos.HighPart, FILE_BEGIN);
		ReadFile(ha->hFile, pdwBlockPos1, dwToRead, &dwTransferred, NULL);
		if(dwTransferred != dwToRead)
			nError = ERROR_FILE_CORRUPT;

		// 重新加密位置数据块
		if(nError == ERROR_SUCCESS)
		{
			DecrypTPKBlock(pdwBlockPos1, dwToRead, dwOldSeed - 1);
			if(pdwBlockPos1[0] != dwToRead)
				nError = ERROR_FILE_CORRUPT;

			memcpy(pdwBlockPos2, pdwBlockPos1, dwToRead);
			EncrypTPKBlock(pdwBlockPos2, dwToRead, dwNewSeed - 1);
		}

		// 将重新加密过的位置数据块写回文件包本地文件
		if(nError == ERROR_SUCCESS)
		{
			SetFilePointer(ha->hFile, TPKFilePos.LowPart, &TPKFilePos.HighPart, FILE_BEGIN);
			WriteFile(ha->hFile, pdwBlockPos2, dwToRead, &dwTransferred, NULL);
			if(dwTransferred != dwToRead)
				nError = ERROR_WRITE_FAULT;
		}
	}

	// 分配文件数据块缓冲区
	if(nError == ERROR_SUCCESS)
	{
		pbFileBlock = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE);
		if(pbFileBlock == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}

	// 重新加密文件所有数据块
	if(nError == ERROR_SUCCESS)
	{
		for(int nBlock = 0; nBlock < nBlocks; nBlock++)
		{
			// 计算文件数据块的内容位置（非压缩情况下的）
			BlockFilePos.QuadPart = TPKFilePos.QuadPart + (TPK_DATABLOCK_SIZE * nBlock);
			dwToRead = TPK_DATABLOCK_SIZE;
			if(nBlock == nBlocks - 1)
				dwToRead = pBlock->dwFSize - (TPK_DATABLOCK_SIZE * (nBlocks - 1));

			// 如果有压缩，再次修改压缩后的数据块的内容位置
			if(pBlock->dwFlags & TPK_FILE_COMPRESSED)
			{
				BlockFilePos.QuadPart = TPKFilePos.QuadPart + pdwBlockPos1[nBlock];
				dwToRead = pdwBlockPos1[nBlock+1] - pdwBlockPos1[nBlock];
			}

			// 读取数据块内容
			SetFilePointer(ha->hFile, BlockFilePos.LowPart, &BlockFilePos.HighPart, FILE_BEGIN);
			ReadFile(ha->hFile, pbFileBlock, dwToRead, &dwTransferred, NULL);
			if(dwTransferred != dwToRead)
				nError = ERROR_FILE_CORRUPT;

			// 重新加密数据块的内容
			DecrypTPKBlock((DWORD*)pbFileBlock, dwToRead, dwOldSeed + nBlock);
			EncrypTPKBlock((DWORD*)pbFileBlock, dwToRead, dwNewSeed + nBlock);

			// 重新回写到文件包本地文件中
			SetFilePointer(ha->hFile, BlockFilePos.LowPart, &BlockFilePos.HighPart, FILE_BEGIN);
			WriteFile(ha->hFile, pbFileBlock, dwToRead, &dwTransferred, NULL);
			if(dwTransferred != dwToRead)
				nError = ERROR_WRITE_FAULT;
		}
	}

	// 释放清除
	if(pbFileBlock != NULL)
		FREEMEM(pbFileBlock);
	if(pdwBlockPos2 != NULL)
		FREEMEM(pdwBlockPos2);
	if(pdwBlockPos1 != NULL)
		FREEMEM(pdwBlockPos1);
	return nError;
}

// -------------------------------------------------------------------------
// 校验文件的CRC32值
BOOL WINAPI TPKVerifyFile(HANDLE hTPK, const char* szFileName)
{    
    HANDLE hFile = NULL;
    BOOL bResult = TRUE;

    // 尝次打开文件
    if(TPKOpenFile(hTPK, szFileName, 0, &hFile))
    {
		crc32_context crc32_ctx;
		TPKFile* hf;
		DWORD Crc32;
		DWORD dwBytesRead;
		BYTE Buffer[0x1000];
        // 初始Crc32对象
        CRC32_Init(&crc32_ctx);
        hf = (TPKFile*)hFile;

        // 遍历读取数据块并计算Crc32值
        for(;;)
        {
            TPKReadFile(hFile, Buffer, sizeof(Buffer), &dwBytesRead, NULL);
            if(dwBytesRead == 0)
                break;

            CRC32_Update(&crc32_ctx, Buffer, (int)dwBytesRead);
        }

        CRC32_Finish(&crc32_ctx, (unsigned long *)&Crc32);
		if(Crc32 != hf->pBlock->dwCrc32)
			bResult = FALSE;

        TPKCloseFile(hFile);
    }
    return bResult;
}

//-----------------------------------------------------------------------------
// 判断str1是否存在在str2的字符串列表中（字符串列表以";"分隔）
static BOOL IsInString(const char* str1, const char* str2)
{
	if(str1 == NULL || str2 == NULL || *str1 == 0 || *str2 == 0)
		return FALSE;
	size_t len = strlen(str2);
	char* str2copy = (char*)malloc(len +1);
	strcpy(str2copy, str2);
	char* start = str2copy;
	for (;;)
	{
		const char* end =strrchr(start, ';');
		if(!end || end <= start)
			break;
		end ++;
		if(_stricmp(end, str1) == 0)
		{
			free(str2copy);
			return TRUE;
		}
		start[end - start - 1] = 0;
	}
	free(str2copy);
	return FALSE;
}

static BOOL CatchFileName(HANDLE hFile, const char* szFileName, const char* szPackDir)
{
	if(hFile == INVALID_HANDLE_VALUE || szFileName == 0 ||
		*szFileName == 0 || szPackDir == 0 || *szPackDir == 0)
		return FALSE;
	TempAlloc< char > szPack((DWORD)strlen(szPackDir) + 1);
	TempAlloc< char > search((DWORD)strlen(szFileName) + MAX_PATH);
	strcpy(szPack, szPackDir);
	char* token = strtok(szPack, ";\0");
	while(token != NULL)
	{
		size_t len = strlen(token);
		if(_strnicmp(szFileName, token, len) == 0)
		{
			char* cShortFileName = (char*)szFileName + len;
			token[len-1] = 0;
			sprintf(search, "%s|%s.tpk\n", cShortFileName, token);
			DWORD dwWrite = (DWORD)strlen(search);
			DWORD dwBytes;
			WriteFile(hFile, search, dwWrite, &dwBytes, NULL);
			return dwBytes == dwWrite;
		}
		token = strtok(NULL, ";\0");
	}
	return FALSE;
}

class SearchParam;

DWORD default_findop(SearchParam* param) { return 0;}

class SearchParam
{
public:
	typedef DWORD (*FINDOP)(SearchParam* param);
	SearchParam() { FindData = NULL; FindOp = default_findop; }

	std::string			InputDir;		// 待搜索遍历的顶层目录路径
	std::string			FilterExt;		// 待过滤的文件名后缀(以分号分隔)
	std::string			FilterDir;		// 待过滤的文件目录(以分号分隔)

	std::string			CurrentPath;	// 当前文件路径
	WIN32_FIND_DATA*	FindData;
	FINDOP				FindOp;			// 搜索到文件进行处理的回调函数
};

struct FindInfo
{
	std::string path;
	std::string file;
	DWORD	filesize;
};

class TPKAdd_SearchParam : public SearchParam
{
public:
	std::vector<FindInfo*>* pInfo;
};

DWORD TPKAddFileCallBack(SearchParam* param)
{
	TPKAdd_SearchParam* pa = (TPKAdd_SearchParam*)param;
	TempAlloc<char> search(pa->CurrentPath.length() + strlen(pa->FindData->cFileName) + 3);
	sprintf(search,"%s\\%s", pa->CurrentPath.c_str(), pa->FindData->cFileName);
	char* cFileName = search + pa->InputDir.length() + 1;
	if(cFileName == NULL || *cFileName == 0)
	{
		for(size_t i = 0; i < pa->pInfo->size(); i++)
			delete (*pa->pInfo)[i];
		pa->pInfo->clear();
		return 0;
	}

	FindInfo* findinfo = new FindInfo;
	findinfo->file = cFileName;
	findinfo->path = search;
	findinfo->filesize = pa->FindData->nFileSizeLow;
	pa->pInfo->push_back(findinfo);
	return 1;
}

//-----------------------------------------------------------------------------
// 递归遍历文件目录下所有文件，添加文件到文件包内
static BOOL SearchPath(const char *path, SearchParam* param)
{
	if(path == NULL || *path == 0 || param == NULL)
		return FALSE;

	WIN32_FIND_DATA findData;
	TempAlloc< char > search( (DWORD)strlen(path) + MAX_PATH);
	sprintf(search, "%s\\*", path);

	HANDLE handle = FindFirstFile(search, &findData);
	if (handle == INVALID_HANDLE_VALUE)
		return FALSE;	
	do
	{
		param->FindData = &findData;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// 确定这是一个目录
			if (findData.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE|FILE_ATTRIBUTE_SYSTEM) )
				continue;

			// 跳过. 和 .. 目录符对象
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
				continue;

			// 跳过排除目录
			if(!param->FilterDir.empty() && IsInString(findData.cFileName, param->FilterDir.c_str()))
				continue;

			sprintf(search,"%s\\%s", path, findData.cFileName);
			param->CurrentPath = search;
			SearchPath(search, param);
		}
		else
		{
			// 确定是我们所需要的各种文件属性的文件
			if (findData.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY| FILE_ATTRIBUTE_OFFLINE|
				FILE_ATTRIBUTE_SYSTEM| FILE_ATTRIBUTE_TEMPORARY) )
				continue;

			char* extension = strrchr(findData.cFileName, '.');
			if(!extension)
				continue;
			extension++;

			// 跳过排除的后缀名文件
			if(!param->FilterExt.empty() && IsInString(extension, param->FilterExt.c_str()))
				continue;

			param->CurrentPath = path;
			if(!param->FindOp(param))
			{
				FindClose(handle);
				return FALSE;
			}
		}

	}while(FindNextFile(handle, &findData));

	FindClose(handle);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 批量添加文件到文件包方法（遍历目标路径下所有文件，然后添加文件到文件包内)
// PackDir 为需要打成TPK文件包的目录，以\结尾，分号相隔，如:"Res\Data\;Res\Gui\;"
BOOL WINAPI TPKPackFileAll(const char* szTPKName, const char* TargetPath, const char* PackDir, const char* FilterExt)
{
	HANDLE hTPK = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hVersionFile = INVALID_HANDLE_VALUE;
	int nError = ERROR_SUCCESS;
	DWORD dwFlags = TPK_FILE_ENCRYPTED| TPK_FILE_COMPRESSED | TPK_FILE_REPLACEEXIST;
	DWORD dwBytes;

	std::vector<FindInfo*> arrFindInfo;
	TPKAdd_SearchParam param;
    param.InputDir  = TargetPath ? TargetPath : "";
    param.FilterExt = FilterExt ? FilterExt : "";
	param.pInfo = &arrFindInfo;
	param.FindOp = TPKAddFileCallBack;

	if(nError == ERROR_SUCCESS)
	{
		if(!SearchPath(TargetPath, &param))
			nError = ERROR_INVALID_PARAMETER;
	}

	if(nError == ERROR_SUCCESS)
	{
        int tpkSize = arrFindInfo.size() + 3;

        if (tpkSize < 30000)
            tpkSize = 30000;

		if(!TPKCreateArchive(szTPKName, OPEN_ALWAYS,tpkSize , &hTPK))
			nError = GetLastError();
	}

	if(nError == ERROR_SUCCESS)
	{
		// 因为版本文件的写入是追加方式，所以为了保证每次生成的
		// 版本文件都是最新的，这里先删除操作
		DeleteFile(TPK_VERFILE_NAME);
	}

	if(nError == ERROR_SUCCESS)
	{
		hVersionFile = CreateFile(TPK_VERFILE_NAME, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0,0);

		if(hVersionFile == INVALID_HANDLE_VALUE)
			nError = ERROR_CAN_NOT_COMPLETE;
	}

	if(nError == ERROR_SUCCESS)
	{
		for(size_t i = 0; i < arrFindInfo.size(); i++)
		{
			if(!TPKAddFile(hTPK, arrFindInfo[i]->path.c_str(), arrFindInfo[i]->file.c_str(), dwFlags))
			{
				nError = ERROR_CAN_NOT_COMPLETE;
				break;
			}

			if(PackDir && *PackDir)
			{				
				::SetFilePointer(hVersionFile, 0, 0, FILE_END);
				if(CatchFileName(hVersionFile, arrFindInfo[i]->file.c_str(), PackDir) == FALSE)
				{
					arrFindInfo[i]->path += arrFindInfo[i]->file;
					arrFindInfo[i]->path += "|\n";
					
					::WriteFile(hVersionFile, arrFindInfo[i]->path.c_str(), (DWORD)arrFindInfo[i]->path.length(), &dwBytes, 0);
					if(arrFindInfo[i]->path.length() != dwBytes)
					{
						nError = ERROR_CAN_NOT_COMPLETE;
						break;
					}
				}
			}

			delete arrFindInfo[i];
		}
		arrFindInfo.clear();
	}

	// 如果PackDir目录非空，则需要添加TPK_VERFILE_NAME文件
	if(nError == ERROR_SUCCESS)
	{
		if(PackDir && *PackDir)
			if(!TPKAddFile(hTPK, TPK_VERFILE_NAME, TPK_VERFILE_NAME, dwFlags))
				nError = ERROR_FILE_INVALID;
	}

	if(hVersionFile != INVALID_HANDLE_VALUE)
		CloseHandle(hVersionFile);

	if(hTPK != NULL)
		TPKCloseArchive(hTPK);

	if(nError != ERROR_SUCCESS)
		DeleteFile(szTPKName);
	return nError == ERROR_SUCCESS;
}

// -------------------------------------------------------------------------
// 从文件包里还原某文件为本地磁盘文件
// hTPK				文件包句柄
// szToExtract		文件包内待解出的文件名
// szExtracted		准备解出到本地磁盘的文件名
BOOL WINAPI TPKExtractFile(HANDLE hTPK, const char* szToExtract, const char* szExtracted)
{
    HANDLE hLocalFile = INVALID_HANDLE_VALUE;
    HANDLE hTPKFile = NULL;
    int nError = ERROR_SUCCESS;

    // 创建本地文件句柄
    if(nError == ERROR_SUCCESS)
    {
        hLocalFile = CreateFile(szExtracted, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
        if(hLocalFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // 打开TPK文件
    if(nError == ERROR_SUCCESS)
    {
        if(!TPKOpenFile(hTPK, szToExtract, 0, &hTPKFile))
            nError = GetLastError();
    }

    // 拷贝文件数据写入到本地磁盘文件
    if(nError == ERROR_SUCCESS)
    {
        char  szBuffer[0x1000];
        DWORD dwTransferred;
        for(;;)
        {
            // 读取文件数据
            if(!TPKReadFile(hTPKFile, szBuffer, sizeof(szBuffer), &dwTransferred, NULL))
                nError = GetLastError();
            if(nError == ERROR_HANDLE_EOF)
                nError = ERROR_SUCCESS;
            if(dwTransferred == 0)
                break;

            WriteFile(hLocalFile, szBuffer, dwTransferred, &dwTransferred, NULL);
            if(dwTransferred == 0)
                nError = ERROR_DISK_FULL;
        }
    }

    // 关闭文件句柄
    if(hTPKFile != NULL)
        TPKCloseFile(hTPKFile);
    if(hLocalFile != INVALID_HANDLE_VALUE)
        CloseHandle(hLocalFile);
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (BOOL)(nError == ERROR_SUCCESS);
}

// ---------------------------------------------------------------------
// 还原文件包内所有文件方法（将所有文件包内文件解出到目标路径）
BOOL WINAPI TPKExtractFileAll(HANDLE hTPK, const char* TargetPath)
{
	TPKArchive* ha = (TPKArchive*)hTPK;
	TempAlloc<char> szExtractFile((DWORD)strlen(TargetPath) + MAX_PATH * 3);
	DWORD nError = ERROR_SUCCESS;

	if(nError == ERROR_SUCCESS)
	{
		TPKHash*	pHashEnd	= ha->pHashTable + ha->pHeader->dwHashTableSize;
		TPKHash*	pHash		= ha->pHashTable;
		TFileNode*	pNode;

		// 遍历，直到找到文件
		for( DWORD i = 0; i < ha->pHeader->dwHashTableSize; i++)
		{
			pNode = ha->pListFile[i];

			// 检查Block索引值和文件列表节点
			if(pHash->dwBlockIndex < DATA_ENTRY_DELETED && (DWORD_PTR)pNode < DATA_ENTRY_DELETED)
			{	
				if(_stricmp(pNode->szFileName, "(listfile)") == 0)
				{
					pHash++;
					continue;
				}

				sprintf(szExtractFile, "%s\\%s", TargetPath, pNode->szFileName);
				EnsureDirectory(szExtractFile);
				ClearFileAttributes(szExtractFile);
				if(!TPKExtractFile(hTPK, pNode->szFileName, szExtractFile))
				{
					nError = ERROR_FILE_INVALID;
					break;
				}				
			}
			pHash ++;
		}
	}
	return nError == ERROR_SUCCESS;
}


/********************************************************************************/
/* 外部函数																		*/
/********************************************************************************/

//-----------------------------------------------------------------------------
// 打开文件操作
// 参数说明如下:
//   hTPK          - 文件包对象
//   szFileName    - 待打开的文件名
//   dwSearchScope - 如何打开文件（文件包里？本地？文件名作为索引值？）
//   phFile        - TPKFile文件对象
// 返回值：成功(TRUE)或失败(FALSE)
BOOL WINAPI TPKOpenFile(HANDLE hTPK, const char* szFileName, DWORD dwSearchScope, HANDLE* phFile)
{
	LARGE_INTEGER FilePos;
	TPKArchive* ha		= (TPKArchive *)hTPK;
	TPKFile* hf			= NULL;
	TPKHash* pHash		= NULL;			// 对应HashTable的Hash项
	TPKBlock* pBlock	= NULL;			// 对应BlockTable的Block项
	DWORD dwHashIndex	= 0;            // 对应HashTable中的Hash项的索引位置
	DWORD dwBlockIndex	= (DWORD)-1;    // 对应BlockTable中的Block项的索引位置
	size_t nHandleSize	= 0;            // TPKFile对象内存分配的实际大小
	int nError			= ERROR_SUCCESS;

	// 检查参数是否正确
	if(nError == ERROR_SUCCESS)
	{
		if(ha == NULL && dwSearchScope == TPK_OPEN_FROM_TPK)
			nError = ERROR_INVALID_PARAMETER;
		if(phFile == NULL)
			nError = ERROR_INVALID_PARAMETER;
		if(dwSearchScope == TPK_OPEN_BY_INDEX && (DWORD_PTR)szFileName > ha->pHeader->dwBlockTableSize)
			nError = ERROR_INVALID_PARAMETER;
		if(dwSearchScope != TPK_OPEN_BY_INDEX && (szFileName == NULL || *szFileName == 0))
			nError = ERROR_INVALID_PARAMETER;
	}

	if(nError == ERROR_SUCCESS)
	{
		// 如果文件名其实是一个索引值时
		if(dwSearchScope == TPK_OPEN_BY_INDEX)
		{
			TPKHash* pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;

			// 计算TPKFile对象大概需要多少内存（sizeof(TPKFile) + FileXXXXXXXX.xxx）
			nHandleSize  = sizeof(TPKFile) + 20;

			for(pHash = ha->pHashTable; pHash < pHashEnd; pHash++)
			{
				if((DWORD_PTR)szFileName == pHash->dwBlockIndex)
				{
					dwHashIndex  = (DWORD)(pHash - ha->pHashTable);
					dwBlockIndex = pHash->dwBlockIndex;
					break;
				}
			}
		}
		else
		{
			// 如果文件是一个本地文件
			if(dwSearchScope == TPK_OPEN_LOCAL_FILE)
				return OpenLocalFile(szFileName, phFile);

			nHandleSize = sizeof(TPKFile) + strlen(szFileName);

			if((pHash = GetHashEntry(ha, szFileName)) != NULL)
			{
				dwHashIndex  = (DWORD)(pHash - ha->pHashTable);
				dwBlockIndex = pHash->dwBlockIndex;
			}
		}
	}

	if(nError == ERROR_SUCCESS)
	{
		// 检查Block索引值是否正确
		if(dwBlockIndex > ha->pHeader->dwBlockTableSize)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// 检查Block项的数据是否正确
	if(nError == ERROR_SUCCESS)
	{
		pBlock = ha->pBlockTable + dwBlockIndex;
		FilePos.HighPart = pBlock->dwFilePosHigh;
		FilePos.LowPart = pBlock->dwFilePos;

		if(FilePos.QuadPart > ha->TPKSize.QuadPart || pBlock->dwCSize > ha->TPKSize.QuadPart)
			nError = ERROR_FILE_CORRUPT;
		if((pBlock->dwFlags & TPK_FILE_EXISTS) == 0)
			nError = ERROR_FILE_NOT_FOUND;
		/*if(pBlock->dwFlags & ~TPK_FILE_VALID_FLAGS)
			nError = ERROR_NOT_SUPPORTED;*/
	}

	// 分配TPKFile对象内存
	if(nError == ERROR_SUCCESS)
	{
		if((hf = (TPKFile*)ALLOCMEM(char, nHandleSize)) == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}

	// 初始化设置TPKFile对象内容
	if(nError == ERROR_SUCCESS)
	{
		memset(hf, 0, nHandleSize);
		hf->hFile    = INVALID_HANDLE_VALUE;
		hf->ha       = ha;
		hf->pBlock   = pBlock;
		hf->nBlocks  = (hf->pBlock->dwFSize + TPK_DATABLOCK_SIZE - 1) / TPK_DATABLOCK_SIZE;
		hf->pHash    = pHash;

		hf->TPKFilePos.HighPart = pBlock->dwFilePosHigh;
		hf->TPKFilePos.LowPart  = pBlock->dwFilePos;

		hf->dwHashIndex  = dwHashIndex;
		hf->dwBlockIndex = dwBlockIndex; 

		// 如果有压缩，还要为位置数据分配缓冲区
		if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
		{            
			if((hf->pdwBlockPos = ALLOCMEM(DWORD, hf->nBlocks + 2)) == NULL)
				nError = ERROR_NOT_ENOUGH_MEMORY;
		}

		// 计算文件的密钥（注意：如果是TPK_OPEN_BY_INDEX，则跳过处理）
		if(dwSearchScope != TPK_OPEN_BY_INDEX)
		{
			if(hf->pBlock->dwFlags & TPK_FILE_ENCRYPTED)
			{
				const char* szTemp = strrchr(szFileName, '\\');
				strcpy(hf->szFileName, szFileName);
				if(szTemp != NULL)
					szFileName = szTemp + 1;

				hf->dwSeed1 = DecryptFileSeed((char*)szFileName);
				if(hf->pBlock->dwFlags & TPK_FILE_FIXSEED)
				{
					hf->dwSeed1 = (hf->dwSeed1 + hf->pBlock->dwFilePos) ^ hf->pBlock->dwFSize;
				}
			}
		}
	}

	// 清除退出
	if(nError != ERROR_SUCCESS)
	{
		FreeTPKFile(hf);
		SetLastError(nError);
	}

	*phFile = hf;
	return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// 关闭和清除文件对象
BOOL WINAPI TPKCloseFile(HANDLE hFile)
{
	TPKFile* hf = (TPKFile*)hFile;

	if(!IsValidFileHandle(hf))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	// 清除最近访问过的文件
	if(hf->ha != NULL)
		hf->ha->pLastFile = NULL;

	// 释放TPK的一些对象
	FreeTPKFile(hf);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 增加一个文件到文件包内
BOOL WINAPI TPKAddFile(HANDLE hTPK, const char* szFileName, const char* szArchivedName, DWORD dwFlags)
{
	TPKArchive* ha		= (TPKArchive *)hTPK;
	HANDLE hFile		= INVALID_HANDLE_VALUE;
	BOOL   bReplaced	= FALSE;				// 是否替换文件包内
	int    nError		= ERROR_SUCCESS;

	if(nError == ERROR_SUCCESS)
	{
		// 检查输入参数是否有效
		if(IsValidTPKHandle(ha) == FALSE || szFileName == NULL || *szFileName == 0 || szArchivedName == NULL || *szArchivedName == 0)
			nError = ERROR_INVALID_PARAMETER;
	}

	// 如果添加的文件是一个已存在的文件列表文件，直接成功返回
	if(nError == ERROR_SUCCESS)
	{
		if(ha->pListFile != NULL && !_stricmp(szFileName, TPK_LISTFILE_NAME))
			return ERROR_SUCCESS;
	}

	// 打开文件
	if(nError == ERROR_SUCCESS)
	{
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			nError = GetLastError();
	}

	// 调用添加文件到文件包
	if(nError == ERROR_SUCCESS)
		nError = AddFileToArchive(ha, hFile, szArchivedName, dwFlags, &bReplaced);

	// 添加文件到文件列表
	if(nError == ERROR_SUCCESS && bReplaced == FALSE)
		nError = TPKListFileCreateNode(ha, szArchivedName);

	// 清除并退出
	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if(nError != ERROR_SUCCESS)
		SetLastError(nError);
	return (nError == ERROR_SUCCESS);
}

//struct FindInfo
//{
//	std::string path;
//	std::string file;
//	DWORD	filesize;
//};
//
//class TPKAdd_SearchParam : public SearchParam
//{
//public:
//	std::vector<FindInfo*>* pInfo;
//};
//
//DWORD TPKAddFileCallBack(SearchParam* param)
//{
//	TPKAdd_SearchParam* pa = (TPKAdd_SearchParam*)param;
//	TempAlloc<char> search(pa->CurrentPath.length() + strlen(pa->FindData->cFileName) + 3);
//	sprintf(search,"%s\\%s", pa->CurrentPath.c_str(), pa->FindData->cFileName);
//	char* cFileName = search + pa->InputDir.length() + 1;
//	if(cFileName == NULL || *cFileName == 0)
//		return 0;
//
//	FindInfo* findinfo = new FindInfo;
//	findinfo->file = cFileName;
//	findinfo->path = search;
//	findinfo->filesize = pa->FindData->nFileSizeLow;
//	pa->pInfo->push_back(findinfo);
//	return 0;
//}

// ----------------------------------------------------------------------------
// 新建一个文件包，批量添加指定路径下的文件到包内
BOOL WINAPI TPKAddFileAll(const char* szTPKName, const char* TargetPath, const char* FilterDir, const char* FilterExt)
{
	HANDLE hTPK = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	char szAddFileName[MAX_PATH] = "";
	int nError = ERROR_SUCCESS;
	DWORD dwFlags = TPK_FILE_ENCRYPTED| TPK_FILE_COMPRESSED | TPK_FILE_REPLACEEXIST;

	std::vector<FindInfo*> arrFindInfo;
	TPKAdd_SearchParam param;
    param.InputDir = TargetPath ? TargetPath : "";
    param.FilterDir = FilterDir ? FilterDir  : "";
    param.FilterExt = FilterExt ? FilterExt  : "";
	param.pInfo = &arrFindInfo;
	param.FindOp = TPKAddFileCallBack;
	
	if(nError == ERROR_SUCCESS)
	{
		if(!SearchPath(TargetPath, &param))
			nError = ERROR_INVALID_PARAMETER;
	}

    if(0 == arrFindInfo.size())
        return ERROR_SUCCESS;

	if(nError == ERROR_SUCCESS)
	{
		if(!TPKCreateArchive(szTPKName, OPEN_ALWAYS, arrFindInfo.size() + 1, &hTPK))
			nError = GetLastError();
	}

	if(nError == ERROR_SUCCESS)
	{
		for(size_t i = 0; i < arrFindInfo.size(); i++)
		{
			if(!TPKAddFile(hTPK, arrFindInfo[i]->path.c_str(), arrFindInfo[i]->file.c_str(), dwFlags))
			{
				nError = ERROR_CAN_NOT_COMPLETE;
				break;
			}
			else
			{
				delete arrFindInfo[i];
			}
		}
		arrFindInfo.clear();
	}

	if(hTPK != NULL)
		TPKCloseArchive(hTPK);

	if(nError != ERROR_SUCCESS)
		DeleteFile(szTPKName);
	
	return nError;
}

//-----------------------------------------------------------------------------
// 从文件包里删除一个文件（文件内容保留，仅更新了HashTable和BlockTable)
BOOL WINAPI TPKRemoveFile(HANDLE hTPK, const char* szFileName, DWORD dwSearchScope)
{
	TPKArchive* ha		= (TPKArchive *)hTPK;
	TPKBlock* pBlock	= NULL;		// 待删除文件的Block项
	TPKHash* pHash		= NULL;     // 待删除文件的Hash项
	DWORD dwBlockIndex	= 0;
	int nError			= ERROR_SUCCESS;

	// 检查输入参数是否正确
	if(nError == ERROR_SUCCESS)
	{
		if(IsValidTPKHandle(ha) == FALSE)
			nError = ERROR_INVALID_PARAMETER;
		if(dwSearchScope != TPK_OPEN_BY_INDEX && *szFileName == 0)
			nError = ERROR_INVALID_PARAMETER;
	}

	// 不允许删除文件列表文件
	if(nError == ERROR_SUCCESS)
	{
		if(dwSearchScope != TPK_OPEN_BY_INDEX && !_stricmp(szFileName, TPK_LISTFILE_NAME))
			nError = ERROR_ACCESS_DENIED;
	}

	// 从文件列表中删除此文件名
	if(nError == ERROR_SUCCESS)
		nError = TPKListFileRemoveNode(ha, szFileName);

	// 查出当前文件的Hash项
	if(nError == ERROR_SUCCESS)
	{
		if((pHash = GetHashEntry(ha, (char*)szFileName)) == NULL)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// 如果Block项的索引值越界，直接退出
	if(nError == ERROR_SUCCESS)
	{
		if((dwBlockIndex = pHash->dwBlockIndex) > ha->pHeader->dwBlockTableSize)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// 如果文件数据块不存在，直接退出
	if(nError == ERROR_SUCCESS)
	{
		pBlock = ha->pBlockTable + dwBlockIndex;
		if((pBlock->dwFlags & TPK_FILE_EXISTS) == 0)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// 清除此Hash项和Block项数据，交给TPKCompactArchive来处理
	if(nError == ERROR_SUCCESS)
	{
		pBlock->dwFilePosHigh = 0;
		pBlock->dwFilePos   = 0;
		pBlock->dwFSize     = 0;
		pBlock->dwCSize     = 0;
		pBlock->dwFlags     = 0;
		pHash->dwName1      = 0xFFFFFFFF;
		pHash->dwName2      = 0xFFFFFFFF;
		pHash->dwBlockIndex = DATA_ENTRY_DELETED;

		// 更新TPK的操作标识
		ha->dwFlags |= TPK_FLAG_CHANGED;
	}

	if(nError != ERROR_SUCCESS)
		SetLastError(nError);
	return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// 重命名文件包内某文件
BOOL WINAPI TPKRenameFile(HANDLE hTPK, const char* szFileName, const char* szNewFileName)
{
	TPKArchive* ha			= (TPKArchive *)hTPK;
	TPKBlock*	pBlock;
	TPKHash*	pOldHash	= NULL;         // 旧文件名的Hash项
	TPKHash*	pNewHash	= NULL;         // 新文件名的Hash项
	DWORD dwSaveBlockIndex	= 0;
	int nError				= ERROR_SUCCESS;

	// 检查输入的参数的正确性
	if(nError == ERROR_SUCCESS)
	{
		if(hTPK == NULL || szNewFileName == NULL || *szNewFileName == 0 || szFileName == NULL || *szFileName == 0)
			nError = ERROR_INVALID_PARAMETER;
	}

	// 禁止重命名文件列表文件
	if(nError == ERROR_SUCCESS)
	{
		if(!_stricmp(szFileName, TPK_LISTFILE_NAME))
			nError = ERROR_ACCESS_DENIED;
	}

	// 根据源文件名获取HashTable的项
	if(nError == ERROR_SUCCESS)
	{
		if((pOldHash = GetHashEntry(ha, szFileName)) == NULL)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// 检查新文件名是否已经有HashTable项了
	if(nError == ERROR_SUCCESS)
	{
		if((pNewHash = GetHashEntry(ha, szNewFileName)) != NULL)
			nError = ERROR_ALREADY_EXISTS;
	}

	// 必须知道解密密钥，否则在文件更名后无法重新解密
	if(nError == ERROR_SUCCESS)
	{
		// 保存BlockTable中此文件索引位置（因源Hash项将删除会丢失此索引值）
		dwSaveBlockIndex = pOldHash->dwBlockIndex;
		pBlock = ha->pBlockTable + dwSaveBlockIndex;

		// 如果原文件有加密，就必须重新用新的密钥加密过
		if(pBlock->dwFlags & TPK_FILE_ENCRYPTED)
		{
			nError = RecryptFileData(ha, dwSaveBlockIndex, szFileName, szNewFileName);
		}
	}

	// 为新文件名得到新的Hash项
	if(nError == ERROR_SUCCESS)
	{
		// 删除旧文件名在文件列表中的节点
		TPKListFileRemoveNode(ha, szFileName);

		pOldHash->dwName1      = 0xFFFFFFFF;
		pOldHash->dwName2      = 0xFFFFFFFF;
		pOldHash->dwBlockIndex = DATA_ENTRY_DELETED;

		if((pNewHash = FindFreeHashEntry(ha, szNewFileName)) == NULL)
			nError = ERROR_CAN_NOT_COMPLETE;
	}

	// 保存Block项的索引值
	if(nError == ERROR_SUCCESS)
	{
		pNewHash->dwBlockIndex = dwSaveBlockIndex;
		ha->dwFlags |= TPK_FLAG_CHANGED;

		// 为新文件创建文件列表节点
		nError = TPKListFileCreateNode(ha, szNewFileName);
	}

	if(nError != ERROR_SUCCESS)
		SetLastError(nError);
	return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// 将某文件包文件复制另一个文件包内
// hFrom	源文件包内源文件对象
// hTPKTo	目标文件包对象
// szFileTo 目标文件包内目标文件名称
BOOL WINAPI TPKCopyFile(HANDLE hFrom, HANDLE hTPKTo, const char* szFileTo)
{
	LARGE_INTEGER TempPos;
	TPKFile* hFileFrom	= NULL;
	TPKArchive* haFrom	= NULL;
	TPKArchive* haTo	= NULL;
	TPKFile* hFileTo	= NULL;
	TPKHash* pHash		= NULL;
	TPKBlock* pBlockEnd = NULL;				// BlockTable的尾部项
	DWORD dwFileSize	= 0;				// 文件大小
	DWORD dwBlockPosLen = 0;				// Block位置数据块长度
	DWORD dwTransferred = 0;				// 读写操作的实际大小
	BOOL  bReplaced		= FALSE;
	int nError			= ERROR_SUCCESS;

	// 为TPKFile对象分配内存
	if(nError == ERROR_SUCCESS)
	{
		hFileFrom = (TPKFile*)hFrom;
		haFrom = hFileFrom->ha;
		haTo = (TPKArchive*)hTPKTo;

		hFileTo = (TPKFile*)ALLOCMEM(BYTE, sizeof(TPKFile) + strlen(szFileTo));
		if(hFileTo == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}

	if(nError == ERROR_SUCCESS)
	{
		pBlockEnd = haTo->pBlockTable + haTo->pHeader->dwBlockTableSize;
		dwFileSize = hFileFrom->pBlock->dwFSize;

		memset(hFileTo, 0, sizeof(TPKFile));
		strcpy(hFileTo->szFileName, szFileTo);
		hFileTo->hFile = INVALID_HANDLE_VALUE;
		hFileTo->ha = haTo;		

		hFileTo->pHash = GetHashEntry(haTo, szFileTo);
		if(hFileTo->pHash != NULL)
		{
			hFileTo->pBlock = haTo->pBlockTable + hFileTo->pHash->dwBlockIndex;
			bReplaced = TRUE;
		}
	}

	// 这里考虑“断电保护”，按目前添加文件的大小在文件包尾挪动相同
	// 字节后写入TPKHeader数据到本地文件包。也就是说即便是
	// 在后续文件内容写入操作时突然断电，也不会丢失TPKHeader数据
	if(nError == ERROR_SUCCESS)
	{
		LARGE_INTEGER dwNewHashTablePos;
		dwNewHashTablePos.LowPart = haTo->pHeader->dwHashTablePos;
		dwNewHashTablePos.HighPart = haTo->pHeader->dwHashTablePosHigh;
		dwNewHashTablePos.QuadPart += dwFileSize;
		nError = SaveTPKTablesForAddFile(haTo, dwNewHashTablePos);
	}

	if(nError == ERROR_SUCCESS && hFileTo->pHash == NULL)
	{
		hFileTo->pHash = FindFreeHashEntry(haTo, szFileTo);
		if(hFileTo->pHash == NULL)
		{
			nError = ERROR_HANDLE_DISK_FULL;
		}
	}

	// 从BlockTable中搜索一个空闲的Block项给这个文件
	if(nError == ERROR_SUCCESS)
	{
		// 记下在HashTable中的索引位置
		hFileTo->dwHashIndex = (DWORD)(hFileTo->pHash - haTo->pHashTable);
		// 找出第一个文件数据内容的
		hFileTo->TPKFilePos.QuadPart = sizeof(TPKHeader);

		// 搜索空闲的Block项，顺便计算出能存储当前文件的位置
		for(TPKBlock* pBlock = haTo->pBlockTable; pBlock < pBlockEnd; pBlock++)
		{
			if(pBlock->dwFlags & TPK_FILE_EXISTS)
			{
				TempPos.HighPart  = pBlock->dwFilePosHigh;
				TempPos.LowPart   = pBlock->dwFilePos;
				TempPos.QuadPart += pBlock->dwCSize;

				if(TempPos.QuadPart > hFileTo->TPKFilePos.QuadPart)
					hFileTo->TPKFilePos = TempPos;
			}
			else
			{
				if(hFileTo->pBlock == NULL)
					hFileTo->pBlock = pBlock;
			}
		}

		// 如果有Hash项，却找不出一个空闲的Block项可用，则退出
		if(hFileTo->pBlock == NULL)
		{
			DWORD dwOldBlockTableSize = haTo->pHeader->dwBlockTableSize;
			if(ExtendBlockTable(haTo))
			{
				hFileTo->pBlock = haTo->pBlockTable+ dwOldBlockTableSize;
			}

			if(hFileTo->pBlock == NULL)
			{
				nError = ERROR_HANDLE_DISK_FULL;
			}
		}		
	}

	// 如果文件需要加密，则获得密钥
	if(nError == ERROR_SUCCESS && (hFileFrom->pBlock->dwFlags & TPK_FILE_ENCRYPTED))
	{
		const char* szTemp = (char*)strrchr(szFileTo, '\\');
		if(szTemp != NULL)
			szTemp ++;
		else
			szTemp = (char*)szFileTo;

		hFileTo->dwSeed1 = DecryptFileSeed(szTemp);
		if(hFileFrom->pBlock->dwFlags & TPK_FILE_FIXSEED)
			hFileTo->dwSeed1 = (hFileTo->dwSeed1 + hFileTo->TPKFilePos.LowPart) ^ dwFileSize;
	}

	// 为压缩数据分配临时缓冲区
	if(nError == ERROR_SUCCESS)
	{
		hFileTo->dwBlockIndex = (DWORD)(hFileTo->pBlock - haTo->pBlockTable);
		hFileTo->nBlocks = dwFileSize / TPK_DATABLOCK_SIZE;
		if(dwFileSize % TPK_DATABLOCK_SIZE)
			hFileTo->nBlocks++;

		if((hFileTo->pbFileBuffer = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE)) == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}	

	// 设置文件操作位置，并设置Hash项和Block项数据
	if(nError == ERROR_SUCCESS)
	{
		// 设置到文件数据起始位置
		SetFilePointer(haTo->hFile, hFileTo->TPKFilePos.LowPart, &hFileTo->TPKFilePos.HighPart, FILE_BEGIN);

		// 设置到Hash项的BlockTable的索引值
		hFileTo->pHash->dwBlockIndex = hFileTo->dwBlockIndex;

		// 设置Block项数据
		hFileTo->pBlock->dwFilePosHigh = (USHORT)hFileTo->TPKFilePos.HighPart;
		hFileTo->pBlock->dwFilePos = hFileTo->TPKFilePos.LowPart;
		hFileTo->pBlock->dwFSize   = dwFileSize;
		hFileTo->pBlock->dwCSize   = 0;
		hFileTo->pBlock->dwFlags   = hFileFrom->pBlock->dwFlags;
		hFileTo->pBlock->dwCrc32	= hFileFrom->pBlock->dwCrc32;
	}

	if(hFileTo->pBlock->dwFlags & TPK_FILE_COMPRESSED)
	{
		// 为压缩文件，分配压缩块位置缓冲区（存储每个压缩后数据块的大小）
		if(nError == ERROR_SUCCESS)
		{
			hFileTo->pdwBlockPos = ALLOCMEM(DWORD, hFileTo->nBlocks + 2);
			hFileFrom->pdwBlockPos = ALLOCMEM(DWORD, hFileTo->nBlocks + 2);		
			if(hFileTo->pdwBlockPos == NULL || hFileFrom->pdwBlockPos == NULL)
				nError = ERROR_NOT_ENOUGH_MEMORY;
		}

		// 如果有需要压缩，要设置数据块的位置数据
		if(nError == ERROR_SUCCESS)
		{
			dwBlockPosLen = (hFileTo->nBlocks + 1) * sizeof(DWORD);
			memset(hFileTo->pdwBlockPos, 0, dwBlockPosLen);
			hFileTo->pdwBlockPos[0] = dwBlockPosLen;
			SetFilePointer(haFrom->hFile, hFileFrom->TPKFilePos.LowPart, &hFileFrom->TPKFilePos.HighPart, FILE_BEGIN);
			ReadFile(haFrom->hFile, hFileFrom->pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
			if(dwTransferred != dwBlockPosLen)
				nError = ERROR_FILE_CORRUPT;
		}

		// 解密位置数据块后重新加密
		if(nError == ERROR_SUCCESS)
		{
			if(hFileTo->pBlock->dwFlags & TPK_FILE_ENCRYPTED)
			{
				DecrypTPKBlock(hFileFrom->pdwBlockPos, dwBlockPosLen, hFileFrom->dwSeed1 - 1);
				if(hFileFrom->pdwBlockPos[0] != dwBlockPosLen)
					nError = ERROR_FILE_CORRUPT;

				memcpy(hFileTo->pdwBlockPos, hFileFrom->pdwBlockPos, dwBlockPosLen);
				EncrypTPKBlock(hFileTo->pdwBlockPos, dwBlockPosLen, hFileTo->dwSeed1 - 1);
			}
			else
			{
				memcpy(hFileTo->pdwBlockPos, hFileFrom->pdwBlockPos, dwBlockPosLen);
			}
		}

		if(nError == ERROR_SUCCESS)
		{
			WriteFile(haTo->hFile, hFileTo->pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
			if(dwTransferred == dwBlockPosLen)
				hFileTo->pBlock->dwCSize += dwBlockPosLen;
			else
				nError = GetLastError();
		}
	}

	// 拷贝文件数据块到目标文件
	if(nError == ERROR_SUCCESS)
	{
		DWORD dwBytes;
		for(DWORD nBlock = 0; nBlock < hFileTo->nBlocks; nBlock++)
		{            
			dwBytes = TPK_DATABLOCK_SIZE;
			// 最后一个数据块可能不足4096
			if(nBlock == hFileTo->nBlocks - 1)
			{
				dwBytes = hFileTo->pBlock->dwFSize - (TPK_DATABLOCK_SIZE * (hFileTo->nBlocks - 1));
			}

			if(hFileTo->pBlock->dwFlags & TPK_FILE_COMPRESSED)
				dwBytes = hFileFrom->pdwBlockPos[nBlock+1] - hFileFrom->pdwBlockPos[nBlock];

			// 从文件包读取文件数据块内容
			ReadFile(haFrom->hFile, hFileTo->pbFileBuffer, dwBytes, &dwTransferred, NULL);
			if(dwTransferred != dwBytes)
			{
				nError = ERROR_FILE_CORRUPT;
				break;
			}

			// 如有必要，需重新加密文件数据块（但不需要重新压缩，
			// 因为文件数据内容的压缩与其它数据无关联）
			if((hFileTo->pBlock->dwFlags & TPK_FILE_ENCRYPTED) && hFileTo->dwSeed1 != hFileFrom->dwSeed1)
			{
				DecrypTPKBlock((DWORD *)hFileTo->pbFileBuffer, dwBytes, hFileFrom->dwSeed1 + nBlock);
				EncrypTPKBlock((DWORD *)hFileTo->pbFileBuffer, dwBytes, hFileTo->dwSeed1 + nBlock);
			}

			// 写文件数据块到目标文件
			WriteFile(haTo->hFile, hFileTo->pbFileBuffer, dwBytes, &dwTransferred, NULL);
			hFileTo->pBlock->dwCSize += dwTransferred;
			if(dwTransferred != dwBytes)
			{
				nError = ERROR_DISK_FULL;
				break;
			}
		}
	}

	if(nError == ERROR_SUCCESS && bReplaced == FALSE)
	{
		nError = TPKListFileCreateNode(haTo, szFileTo);
	}

	// 如果成功，需要通知保存文件包时作一些改变操作
	if(nError == ERROR_SUCCESS)
	{
		DWORD dwTableSize;

		haTo->pLastFile  = NULL;
		haTo->dwBlockPos = 0;
		haTo->dwBuffPos  = 0;

		// 设置HashTable在文件包的存储位置
		TempPos.QuadPart = hFileTo->TPKFilePos.QuadPart + hFileTo->pBlock->dwCSize;		
		haTo->HashTablePos.QuadPart = TempPos.QuadPart;
		haTo->pHeader->dwHashTablePos = TempPos.LowPart;
		haTo->pHeader->dwHashTablePosHigh = (USHORT)TempPos.HighPart;
		dwTableSize = haTo->pHeader->dwHashTableSize * sizeof(TPKHash);

		// 设置BlockTable在文件包的存储位置
		TempPos.QuadPart += dwTableSize;
		haTo->BlockTablePos.QuadPart = TempPos.QuadPart;
		haTo->pHeader->dwBlockTablePosHigh = (USHORT)TempPos.HighPart;
		haTo->pHeader->dwBlockTablePos = TempPos.LowPart;
		dwTableSize = haTo->pHeader->dwBlockTableSize * sizeof(TPKBlock);

		TempPos.QuadPart += dwTableSize;
		haTo->TPKSize = TempPos;
		haTo->dwFlags |= TPK_FLAG_CHANGED;
	}
	else
	{
		// 清除设置的Hash项或Block项数据
		if(hFileTo != NULL)
		{
			if(hFileTo->pHash != NULL)
				memset(hFileTo->pHash, 0xFF, sizeof(TPKHash));
			if(hFileTo->pBlock != NULL)
				memset(hFileTo->pBlock, 0, sizeof(TPKBlock));
		}
	}
	
	FreeTPKFile(hFileTo);
	return nError == ERROR_SUCCESS;
}