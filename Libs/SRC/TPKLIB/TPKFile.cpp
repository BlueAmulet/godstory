#include "TPKLib.h"
#include "TPKCommon.h"
#include "crc32.h"
#include <string>
#include <vector>

/********************************************************************************/
/* �ڲ�����																		*/
/********************************************************************************/
// �򿪱����ļ�
static BOOL OpenLocalFile(const char* szFileName, HANDLE* phFile)
{
    TPKFile* hf = NULL;
    HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        // ΪTPKFile��������ڴ�
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
// �ͷ�TPKFile����
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
// �ж��ļ��Ƿ����ļ�����
BOOL WINAPI TPKHaTPK(HANDLE hTPK, char* szFileName)
{
    TPKArchive* ha = (TPKArchive*)hTPK;
    int nError = ERROR_SUCCESS;
    if(nError == ERROR_SUCCESS)
    {
        if(ha == NULL || szFileName == NULL || *szFileName == 0)
            nError = ERROR_INVALID_PARAMETER;
    }

    // ����Ƿ��д��ļ���Hash��
    if(nError == ERROR_SUCCESS)
    {
        if(GetHashEntry(ha, szFileName) == NULL)
        {
            nError = ERROR_FILE_NOT_FOUND;
        }
    }

    // ����д���ͱ���
    if(nError != ERROR_SUCCESS)
    {
        SetLastError(nError);
    }
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// ����ļ����ļ�����(������Hash�Block����ļ��б������)
int AddInternalFile(TPKArchive* ha, const char* szFileName)
{
    TPKBlock*	pBlockEnd;
    TPKBlock*	pBlock;
    TPKHash*	pHash;
    int nError = ERROR_SUCCESS;

    // ����ļ��Ƿ��Ѿ��������ļ�����
    pHash = GetHashEntry(ha, szFileName);
    if(pHash == NULL)
    {
		// ���ҿ��е�Hash��
        pHash = FindFreeHashEntry(ha, szFileName);
        if(pHash != NULL)
        {
			// ���ҿ��е�Block��
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
					// ����ļ������ļ��б���
					return TPKListFileCreateNode(ha, szFileName);
                }
            }
        }

		nError = ERROR_HANDLE_DISK_FULL;
    }
    return nError;
}

// -------------------------------------------------------------------------
//���¼����ļ�����
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

	// �ļ����뱻���ܹ���
	if((pBlock->dwFlags & TPK_FILE_ENCRYPTED) == 0)
		return ERROR_INVALID_BLOCK;

	// ������Կ���õ��ļ����ǲ���·���ģ�����Ҫ�Ƚ�ȡ
	szPlainName = strrchr(szFileName, '\\');
	if(szPlainName != NULL)
		szFileName = szPlainName + 1;
	szPlainName = strrchr(szNewFileName, '\\');
	if(szPlainName != NULL)
		szNewFileName = szPlainName + 1;

	// ���������Կ
	dwOldSeed = DecryptFileSeed(szFileName);
	dwNewSeed = DecryptFileSeed(szNewFileName);
	if(pBlock->dwFlags & TPK_FILE_FIXSEED)
	{
		dwOldSeed = (dwOldSeed + pBlock->dwFilePos) ^ pBlock->dwFSize;
		dwNewSeed = (dwNewSeed + pBlock->dwFilePos) ^ pBlock->dwFSize;
	}

	// �����Կ��ͬ�����������¼�����
	if(dwNewSeed == dwOldSeed)
		return ERROR_SUCCESS;

	// ����ļ��Ĺ̶�λ��
	TPKFilePos.LowPart = pBlock->dwFilePos;
	TPKFilePos.HighPart = pBlock->dwFilePosHigh;

	// �����ļ������ݿ���
	nBlocks = pBlock->dwFSize / TPK_DATABLOCK_SIZE;
	if(pBlock->dwFSize % TPK_DATABLOCK_SIZE)
		nBlocks++;

	// �������ļ�����һ���������ݿ�洢�ģ���ֻ�账��һ����
	if(pBlock->dwFlags & TPK_FILE_SINGLE_UNIT)
	{
		// �����ļ����ݿ黺����
		pbFileBlock = ALLOCMEM(BYTE, pBlock->dwCSize);
		if(pbFileBlock == NULL)
			return ERROR_NOT_ENOUGH_MEMORY;

		SetFilePointer(ha->hFile, TPKFilePos.LowPart, &TPKFilePos.HighPart, FILE_BEGIN);
		ReadFile(ha->hFile, pbFileBlock, pBlock->dwCSize, &dwTransferred, NULL);
		if(dwTransferred == pBlock->dwCSize)
			nError = ERROR_FILE_CORRUPT;

		if(nError == ERROR_SUCCESS)
		{
			// ���¼������ݿ�
			DecrypTPKBlock((DWORD*)pbFileBlock, pBlock->dwCSize, dwOldSeed);
			EncrypTPKBlock((DWORD*)pbFileBlock, pBlock->dwCSize, dwNewSeed);

			// �ٴ�д���ļ��������ļ�
			SetFilePointer(ha->hFile, TPKFilePos.LowPart, &TPKFilePos.HighPart, FILE_BEGIN);
			WriteFile(ha->hFile, pbFileBlock, pBlock->dwCSize, &dwTransferred, NULL);
			if(dwTransferred != pBlock->dwCSize)
				nError = ERROR_WRITE_FAULT;
		}
		FREEMEM(pbFileBlock);
		return nError;
	}

	// ����ļ��Ǿ���ѹ���ģ�����������¼��ܹ�λ�����ݿ����ݣ��ټ������ݿ�����
	if(pBlock->dwFlags & TPK_FILE_COMPRESSED)
	{
		// �����ļ���λ�����ݿ黺����
		pdwBlockPos1 = ALLOCMEM(DWORD, nBlocks + 2);
		pdwBlockPos2 = ALLOCMEM(DWORD, nBlocks + 2);
		if(pdwBlockPos1 == NULL || pdwBlockPos2 == NULL)
			return ERROR_NOT_ENOUGH_MEMORY;

		// ������Ҫ��ȡ��λ�ÿ����ݴ�С
		dwToRead = (nBlocks + 1) * sizeof(DWORD);

		// ��ȡλ�����ݿ�����
		SetFilePointer(ha->hFile, TPKFilePos.LowPart, &TPKFilePos.HighPart, FILE_BEGIN);
		ReadFile(ha->hFile, pdwBlockPos1, dwToRead, &dwTransferred, NULL);
		if(dwTransferred != dwToRead)
			nError = ERROR_FILE_CORRUPT;

		// ���¼���λ�����ݿ�
		if(nError == ERROR_SUCCESS)
		{
			DecrypTPKBlock(pdwBlockPos1, dwToRead, dwOldSeed - 1);
			if(pdwBlockPos1[0] != dwToRead)
				nError = ERROR_FILE_CORRUPT;

			memcpy(pdwBlockPos2, pdwBlockPos1, dwToRead);
			EncrypTPKBlock(pdwBlockPos2, dwToRead, dwNewSeed - 1);
		}

		// �����¼��ܹ���λ�����ݿ�д���ļ��������ļ�
		if(nError == ERROR_SUCCESS)
		{
			SetFilePointer(ha->hFile, TPKFilePos.LowPart, &TPKFilePos.HighPart, FILE_BEGIN);
			WriteFile(ha->hFile, pdwBlockPos2, dwToRead, &dwTransferred, NULL);
			if(dwTransferred != dwToRead)
				nError = ERROR_WRITE_FAULT;
		}
	}

	// �����ļ����ݿ黺����
	if(nError == ERROR_SUCCESS)
	{
		pbFileBlock = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE);
		if(pbFileBlock == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}

	// ���¼����ļ��������ݿ�
	if(nError == ERROR_SUCCESS)
	{
		for(int nBlock = 0; nBlock < nBlocks; nBlock++)
		{
			// �����ļ����ݿ������λ�ã���ѹ������µģ�
			BlockFilePos.QuadPart = TPKFilePos.QuadPart + (TPK_DATABLOCK_SIZE * nBlock);
			dwToRead = TPK_DATABLOCK_SIZE;
			if(nBlock == nBlocks - 1)
				dwToRead = pBlock->dwFSize - (TPK_DATABLOCK_SIZE * (nBlocks - 1));

			// �����ѹ�����ٴ��޸�ѹ��������ݿ������λ��
			if(pBlock->dwFlags & TPK_FILE_COMPRESSED)
			{
				BlockFilePos.QuadPart = TPKFilePos.QuadPart + pdwBlockPos1[nBlock];
				dwToRead = pdwBlockPos1[nBlock+1] - pdwBlockPos1[nBlock];
			}

			// ��ȡ���ݿ�����
			SetFilePointer(ha->hFile, BlockFilePos.LowPart, &BlockFilePos.HighPart, FILE_BEGIN);
			ReadFile(ha->hFile, pbFileBlock, dwToRead, &dwTransferred, NULL);
			if(dwTransferred != dwToRead)
				nError = ERROR_FILE_CORRUPT;

			// ���¼������ݿ������
			DecrypTPKBlock((DWORD*)pbFileBlock, dwToRead, dwOldSeed + nBlock);
			EncrypTPKBlock((DWORD*)pbFileBlock, dwToRead, dwNewSeed + nBlock);

			// ���»�д���ļ��������ļ���
			SetFilePointer(ha->hFile, BlockFilePos.LowPart, &BlockFilePos.HighPart, FILE_BEGIN);
			WriteFile(ha->hFile, pbFileBlock, dwToRead, &dwTransferred, NULL);
			if(dwTransferred != dwToRead)
				nError = ERROR_WRITE_FAULT;
		}
	}

	// �ͷ����
	if(pbFileBlock != NULL)
		FREEMEM(pbFileBlock);
	if(pdwBlockPos2 != NULL)
		FREEMEM(pdwBlockPos2);
	if(pdwBlockPos1 != NULL)
		FREEMEM(pdwBlockPos1);
	return nError;
}

// -------------------------------------------------------------------------
// У���ļ���CRC32ֵ
BOOL WINAPI TPKVerifyFile(HANDLE hTPK, const char* szFileName)
{    
    HANDLE hFile = NULL;
    BOOL bResult = TRUE;

    // ���δ��ļ�
    if(TPKOpenFile(hTPK, szFileName, 0, &hFile))
    {
		crc32_context crc32_ctx;
		TPKFile* hf;
		DWORD Crc32;
		DWORD dwBytesRead;
		BYTE Buffer[0x1000];
        // ��ʼCrc32����
        CRC32_Init(&crc32_ctx);
        hf = (TPKFile*)hFile;

        // ������ȡ���ݿ鲢����Crc32ֵ
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
// �ж�str1�Ƿ������str2���ַ����б��У��ַ����б���";"�ָ���
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

	std::string			InputDir;		// �����������Ķ���Ŀ¼·��
	std::string			FilterExt;		// �����˵��ļ�����׺(�Էֺŷָ�)
	std::string			FilterDir;		// �����˵��ļ�Ŀ¼(�Էֺŷָ�)

	std::string			CurrentPath;	// ��ǰ�ļ�·��
	WIN32_FIND_DATA*	FindData;
	FINDOP				FindOp;			// �������ļ����д���Ļص�����
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
// �ݹ�����ļ�Ŀ¼�������ļ�������ļ����ļ�����
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
			// ȷ������һ��Ŀ¼
			if (findData.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE|FILE_ATTRIBUTE_SYSTEM) )
				continue;

			// ����. �� .. Ŀ¼������
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
				continue;

			// �����ų�Ŀ¼
			if(!param->FilterDir.empty() && IsInString(findData.cFileName, param->FilterDir.c_str()))
				continue;

			sprintf(search,"%s\\%s", path, findData.cFileName);
			param->CurrentPath = search;
			SearchPath(search, param);
		}
		else
		{
			// ȷ������������Ҫ�ĸ����ļ����Ե��ļ�
			if (findData.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY| FILE_ATTRIBUTE_OFFLINE|
				FILE_ATTRIBUTE_SYSTEM| FILE_ATTRIBUTE_TEMPORARY) )
				continue;

			char* extension = strrchr(findData.cFileName, '.');
			if(!extension)
				continue;
			extension++;

			// �����ų��ĺ�׺���ļ�
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
// ��������ļ����ļ�������������Ŀ��·���������ļ���Ȼ������ļ����ļ�����)
// PackDir Ϊ��Ҫ���TPK�ļ�����Ŀ¼����\��β���ֺ��������:"Res\Data\;Res\Gui\;"
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
		// ��Ϊ�汾�ļ���д����׷�ӷ�ʽ������Ϊ�˱�֤ÿ�����ɵ�
		// �汾�ļ��������µģ�������ɾ������
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

	// ���PackDirĿ¼�ǿգ�����Ҫ���TPK_VERFILE_NAME�ļ�
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
// ���ļ����ﻹԭĳ�ļ�Ϊ���ش����ļ�
// hTPK				�ļ������
// szToExtract		�ļ����ڴ�������ļ���
// szExtracted		׼����������ش��̵��ļ���
BOOL WINAPI TPKExtractFile(HANDLE hTPK, const char* szToExtract, const char* szExtracted)
{
    HANDLE hLocalFile = INVALID_HANDLE_VALUE;
    HANDLE hTPKFile = NULL;
    int nError = ERROR_SUCCESS;

    // ���������ļ����
    if(nError == ERROR_SUCCESS)
    {
        hLocalFile = CreateFile(szExtracted, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
        if(hLocalFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // ��TPK�ļ�
    if(nError == ERROR_SUCCESS)
    {
        if(!TPKOpenFile(hTPK, szToExtract, 0, &hTPKFile))
            nError = GetLastError();
    }

    // �����ļ�����д�뵽���ش����ļ�
    if(nError == ERROR_SUCCESS)
    {
        char  szBuffer[0x1000];
        DWORD dwTransferred;
        for(;;)
        {
            // ��ȡ�ļ�����
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

    // �ر��ļ����
    if(hTPKFile != NULL)
        TPKCloseFile(hTPKFile);
    if(hLocalFile != INVALID_HANDLE_VALUE)
        CloseHandle(hLocalFile);
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (BOOL)(nError == ERROR_SUCCESS);
}

// ---------------------------------------------------------------------
// ��ԭ�ļ����������ļ��������������ļ������ļ������Ŀ��·����
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

		// ������ֱ���ҵ��ļ�
		for( DWORD i = 0; i < ha->pHeader->dwHashTableSize; i++)
		{
			pNode = ha->pListFile[i];

			// ���Block����ֵ���ļ��б�ڵ�
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
/* �ⲿ����																		*/
/********************************************************************************/

//-----------------------------------------------------------------------------
// ���ļ�����
// ����˵������:
//   hTPK          - �ļ�������
//   szFileName    - ���򿪵��ļ���
//   dwSearchScope - ��δ��ļ����ļ�������أ��ļ�����Ϊ����ֵ����
//   phFile        - TPKFile�ļ�����
// ����ֵ���ɹ�(TRUE)��ʧ��(FALSE)
BOOL WINAPI TPKOpenFile(HANDLE hTPK, const char* szFileName, DWORD dwSearchScope, HANDLE* phFile)
{
	LARGE_INTEGER FilePos;
	TPKArchive* ha		= (TPKArchive *)hTPK;
	TPKFile* hf			= NULL;
	TPKHash* pHash		= NULL;			// ��ӦHashTable��Hash��
	TPKBlock* pBlock	= NULL;			// ��ӦBlockTable��Block��
	DWORD dwHashIndex	= 0;            // ��ӦHashTable�е�Hash�������λ��
	DWORD dwBlockIndex	= (DWORD)-1;    // ��ӦBlockTable�е�Block�������λ��
	size_t nHandleSize	= 0;            // TPKFile�����ڴ�����ʵ�ʴ�С
	int nError			= ERROR_SUCCESS;

	// �������Ƿ���ȷ
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
		// ����ļ�����ʵ��һ������ֵʱ
		if(dwSearchScope == TPK_OPEN_BY_INDEX)
		{
			TPKHash* pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;

			// ����TPKFile��������Ҫ�����ڴ棨sizeof(TPKFile) + FileXXXXXXXX.xxx��
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
			// ����ļ���һ�������ļ�
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
		// ���Block����ֵ�Ƿ���ȷ
		if(dwBlockIndex > ha->pHeader->dwBlockTableSize)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// ���Block��������Ƿ���ȷ
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

	// ����TPKFile�����ڴ�
	if(nError == ERROR_SUCCESS)
	{
		if((hf = (TPKFile*)ALLOCMEM(char, nHandleSize)) == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}

	// ��ʼ������TPKFile��������
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

		// �����ѹ������ҪΪλ�����ݷ��仺����
		if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
		{            
			if((hf->pdwBlockPos = ALLOCMEM(DWORD, hf->nBlocks + 2)) == NULL)
				nError = ERROR_NOT_ENOUGH_MEMORY;
		}

		// �����ļ�����Կ��ע�⣺�����TPK_OPEN_BY_INDEX������������
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

	// ����˳�
	if(nError != ERROR_SUCCESS)
	{
		FreeTPKFile(hf);
		SetLastError(nError);
	}

	*phFile = hf;
	return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// �رպ�����ļ�����
BOOL WINAPI TPKCloseFile(HANDLE hFile)
{
	TPKFile* hf = (TPKFile*)hFile;

	if(!IsValidFileHandle(hf))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	// ���������ʹ����ļ�
	if(hf->ha != NULL)
		hf->ha->pLastFile = NULL;

	// �ͷ�TPK��һЩ����
	FreeTPKFile(hf);
	return TRUE;
}

//-----------------------------------------------------------------------------
// ����һ���ļ����ļ�����
BOOL WINAPI TPKAddFile(HANDLE hTPK, const char* szFileName, const char* szArchivedName, DWORD dwFlags)
{
	TPKArchive* ha		= (TPKArchive *)hTPK;
	HANDLE hFile		= INVALID_HANDLE_VALUE;
	BOOL   bReplaced	= FALSE;				// �Ƿ��滻�ļ�����
	int    nError		= ERROR_SUCCESS;

	if(nError == ERROR_SUCCESS)
	{
		// �����������Ƿ���Ч
		if(IsValidTPKHandle(ha) == FALSE || szFileName == NULL || *szFileName == 0 || szArchivedName == NULL || *szArchivedName == 0)
			nError = ERROR_INVALID_PARAMETER;
	}

	// �����ӵ��ļ���һ���Ѵ��ڵ��ļ��б��ļ���ֱ�ӳɹ�����
	if(nError == ERROR_SUCCESS)
	{
		if(ha->pListFile != NULL && !_stricmp(szFileName, TPK_LISTFILE_NAME))
			return ERROR_SUCCESS;
	}

	// ���ļ�
	if(nError == ERROR_SUCCESS)
	{
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			nError = GetLastError();
	}

	// ��������ļ����ļ���
	if(nError == ERROR_SUCCESS)
		nError = AddFileToArchive(ha, hFile, szArchivedName, dwFlags, &bReplaced);

	// ����ļ����ļ��б�
	if(nError == ERROR_SUCCESS && bReplaced == FALSE)
		nError = TPKListFileCreateNode(ha, szArchivedName);

	// ������˳�
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
// �½�һ���ļ������������ָ��·���µ��ļ�������
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
// ���ļ�����ɾ��һ���ļ����ļ����ݱ�������������HashTable��BlockTable)
BOOL WINAPI TPKRemoveFile(HANDLE hTPK, const char* szFileName, DWORD dwSearchScope)
{
	TPKArchive* ha		= (TPKArchive *)hTPK;
	TPKBlock* pBlock	= NULL;		// ��ɾ���ļ���Block��
	TPKHash* pHash		= NULL;     // ��ɾ���ļ���Hash��
	DWORD dwBlockIndex	= 0;
	int nError			= ERROR_SUCCESS;

	// �����������Ƿ���ȷ
	if(nError == ERROR_SUCCESS)
	{
		if(IsValidTPKHandle(ha) == FALSE)
			nError = ERROR_INVALID_PARAMETER;
		if(dwSearchScope != TPK_OPEN_BY_INDEX && *szFileName == 0)
			nError = ERROR_INVALID_PARAMETER;
	}

	// ������ɾ���ļ��б��ļ�
	if(nError == ERROR_SUCCESS)
	{
		if(dwSearchScope != TPK_OPEN_BY_INDEX && !_stricmp(szFileName, TPK_LISTFILE_NAME))
			nError = ERROR_ACCESS_DENIED;
	}

	// ���ļ��б���ɾ�����ļ���
	if(nError == ERROR_SUCCESS)
		nError = TPKListFileRemoveNode(ha, szFileName);

	// �����ǰ�ļ���Hash��
	if(nError == ERROR_SUCCESS)
	{
		if((pHash = GetHashEntry(ha, (char*)szFileName)) == NULL)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// ���Block�������ֵԽ�磬ֱ���˳�
	if(nError == ERROR_SUCCESS)
	{
		if((dwBlockIndex = pHash->dwBlockIndex) > ha->pHeader->dwBlockTableSize)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// ����ļ����ݿ鲻���ڣ�ֱ���˳�
	if(nError == ERROR_SUCCESS)
	{
		pBlock = ha->pBlockTable + dwBlockIndex;
		if((pBlock->dwFlags & TPK_FILE_EXISTS) == 0)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// �����Hash���Block�����ݣ�����TPKCompactArchive������
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

		// ����TPK�Ĳ�����ʶ
		ha->dwFlags |= TPK_FLAG_CHANGED;
	}

	if(nError != ERROR_SUCCESS)
		SetLastError(nError);
	return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// �������ļ�����ĳ�ļ�
BOOL WINAPI TPKRenameFile(HANDLE hTPK, const char* szFileName, const char* szNewFileName)
{
	TPKArchive* ha			= (TPKArchive *)hTPK;
	TPKBlock*	pBlock;
	TPKHash*	pOldHash	= NULL;         // ���ļ�����Hash��
	TPKHash*	pNewHash	= NULL;         // ���ļ�����Hash��
	DWORD dwSaveBlockIndex	= 0;
	int nError				= ERROR_SUCCESS;

	// �������Ĳ�������ȷ��
	if(nError == ERROR_SUCCESS)
	{
		if(hTPK == NULL || szNewFileName == NULL || *szNewFileName == 0 || szFileName == NULL || *szFileName == 0)
			nError = ERROR_INVALID_PARAMETER;
	}

	// ��ֹ�������ļ��б��ļ�
	if(nError == ERROR_SUCCESS)
	{
		if(!_stricmp(szFileName, TPK_LISTFILE_NAME))
			nError = ERROR_ACCESS_DENIED;
	}

	// ����Դ�ļ�����ȡHashTable����
	if(nError == ERROR_SUCCESS)
	{
		if((pOldHash = GetHashEntry(ha, szFileName)) == NULL)
			nError = ERROR_FILE_NOT_FOUND;
	}

	// ������ļ����Ƿ��Ѿ���HashTable����
	if(nError == ERROR_SUCCESS)
	{
		if((pNewHash = GetHashEntry(ha, szNewFileName)) != NULL)
			nError = ERROR_ALREADY_EXISTS;
	}

	// ����֪��������Կ���������ļ��������޷����½���
	if(nError == ERROR_SUCCESS)
	{
		// ����BlockTable�д��ļ�����λ�ã���ԴHash�ɾ���ᶪʧ������ֵ��
		dwSaveBlockIndex = pOldHash->dwBlockIndex;
		pBlock = ha->pBlockTable + dwSaveBlockIndex;

		// ���ԭ�ļ��м��ܣ��ͱ����������µ���Կ���ܹ�
		if(pBlock->dwFlags & TPK_FILE_ENCRYPTED)
		{
			nError = RecryptFileData(ha, dwSaveBlockIndex, szFileName, szNewFileName);
		}
	}

	// Ϊ���ļ����õ��µ�Hash��
	if(nError == ERROR_SUCCESS)
	{
		// ɾ�����ļ������ļ��б��еĽڵ�
		TPKListFileRemoveNode(ha, szFileName);

		pOldHash->dwName1      = 0xFFFFFFFF;
		pOldHash->dwName2      = 0xFFFFFFFF;
		pOldHash->dwBlockIndex = DATA_ENTRY_DELETED;

		if((pNewHash = FindFreeHashEntry(ha, szNewFileName)) == NULL)
			nError = ERROR_CAN_NOT_COMPLETE;
	}

	// ����Block�������ֵ
	if(nError == ERROR_SUCCESS)
	{
		pNewHash->dwBlockIndex = dwSaveBlockIndex;
		ha->dwFlags |= TPK_FLAG_CHANGED;

		// Ϊ���ļ������ļ��б�ڵ�
		nError = TPKListFileCreateNode(ha, szNewFileName);
	}

	if(nError != ERROR_SUCCESS)
		SetLastError(nError);
	return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// ��ĳ�ļ����ļ�������һ���ļ�����
// hFrom	Դ�ļ�����Դ�ļ�����
// hTPKTo	Ŀ���ļ�������
// szFileTo Ŀ���ļ�����Ŀ���ļ�����
BOOL WINAPI TPKCopyFile(HANDLE hFrom, HANDLE hTPKTo, const char* szFileTo)
{
	LARGE_INTEGER TempPos;
	TPKFile* hFileFrom	= NULL;
	TPKArchive* haFrom	= NULL;
	TPKArchive* haTo	= NULL;
	TPKFile* hFileTo	= NULL;
	TPKHash* pHash		= NULL;
	TPKBlock* pBlockEnd = NULL;				// BlockTable��β����
	DWORD dwFileSize	= 0;				// �ļ���С
	DWORD dwBlockPosLen = 0;				// Blockλ�����ݿ鳤��
	DWORD dwTransferred = 0;				// ��д������ʵ�ʴ�С
	BOOL  bReplaced		= FALSE;
	int nError			= ERROR_SUCCESS;

	// ΪTPKFile��������ڴ�
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

	// ���￼�ǡ��ϵ籣��������Ŀǰ����ļ��Ĵ�С���ļ���βŲ����ͬ
	// �ֽں�д��TPKHeader���ݵ������ļ�����Ҳ����˵������
	// �ں����ļ�����д�����ʱͻȻ�ϵ磬Ҳ���ᶪʧTPKHeader����
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

	// ��BlockTable������һ�����е�Block�������ļ�
	if(nError == ERROR_SUCCESS)
	{
		// ������HashTable�е�����λ��
		hFileTo->dwHashIndex = (DWORD)(hFileTo->pHash - haTo->pHashTable);
		// �ҳ���һ���ļ��������ݵ�
		hFileTo->TPKFilePos.QuadPart = sizeof(TPKHeader);

		// �������е�Block�˳�������ܴ洢��ǰ�ļ���λ��
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

		// �����Hash�ȴ�Ҳ���һ�����е�Block����ã����˳�
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

	// ����ļ���Ҫ���ܣ�������Կ
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

	// Ϊѹ�����ݷ�����ʱ������
	if(nError == ERROR_SUCCESS)
	{
		hFileTo->dwBlockIndex = (DWORD)(hFileTo->pBlock - haTo->pBlockTable);
		hFileTo->nBlocks = dwFileSize / TPK_DATABLOCK_SIZE;
		if(dwFileSize % TPK_DATABLOCK_SIZE)
			hFileTo->nBlocks++;

		if((hFileTo->pbFileBuffer = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE)) == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}	

	// �����ļ�����λ�ã�������Hash���Block������
	if(nError == ERROR_SUCCESS)
	{
		// ���õ��ļ�������ʼλ��
		SetFilePointer(haTo->hFile, hFileTo->TPKFilePos.LowPart, &hFileTo->TPKFilePos.HighPart, FILE_BEGIN);

		// ���õ�Hash���BlockTable������ֵ
		hFileTo->pHash->dwBlockIndex = hFileTo->dwBlockIndex;

		// ����Block������
		hFileTo->pBlock->dwFilePosHigh = (USHORT)hFileTo->TPKFilePos.HighPart;
		hFileTo->pBlock->dwFilePos = hFileTo->TPKFilePos.LowPart;
		hFileTo->pBlock->dwFSize   = dwFileSize;
		hFileTo->pBlock->dwCSize   = 0;
		hFileTo->pBlock->dwFlags   = hFileFrom->pBlock->dwFlags;
		hFileTo->pBlock->dwCrc32	= hFileFrom->pBlock->dwCrc32;
	}

	if(hFileTo->pBlock->dwFlags & TPK_FILE_COMPRESSED)
	{
		// Ϊѹ���ļ�������ѹ����λ�û��������洢ÿ��ѹ�������ݿ�Ĵ�С��
		if(nError == ERROR_SUCCESS)
		{
			hFileTo->pdwBlockPos = ALLOCMEM(DWORD, hFileTo->nBlocks + 2);
			hFileFrom->pdwBlockPos = ALLOCMEM(DWORD, hFileTo->nBlocks + 2);		
			if(hFileTo->pdwBlockPos == NULL || hFileFrom->pdwBlockPos == NULL)
				nError = ERROR_NOT_ENOUGH_MEMORY;
		}

		// �������Ҫѹ����Ҫ�������ݿ��λ������
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

		// ����λ�����ݿ�����¼���
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

	// �����ļ����ݿ鵽Ŀ���ļ�
	if(nError == ERROR_SUCCESS)
	{
		DWORD dwBytes;
		for(DWORD nBlock = 0; nBlock < hFileTo->nBlocks; nBlock++)
		{            
			dwBytes = TPK_DATABLOCK_SIZE;
			// ���һ�����ݿ���ܲ���4096
			if(nBlock == hFileTo->nBlocks - 1)
			{
				dwBytes = hFileTo->pBlock->dwFSize - (TPK_DATABLOCK_SIZE * (hFileTo->nBlocks - 1));
			}

			if(hFileTo->pBlock->dwFlags & TPK_FILE_COMPRESSED)
				dwBytes = hFileFrom->pdwBlockPos[nBlock+1] - hFileFrom->pdwBlockPos[nBlock];

			// ���ļ�����ȡ�ļ����ݿ�����
			ReadFile(haFrom->hFile, hFileTo->pbFileBuffer, dwBytes, &dwTransferred, NULL);
			if(dwTransferred != dwBytes)
			{
				nError = ERROR_FILE_CORRUPT;
				break;
			}

			// ���б�Ҫ�������¼����ļ����ݿ飨������Ҫ����ѹ����
			// ��Ϊ�ļ��������ݵ�ѹ�������������޹�����
			if((hFileTo->pBlock->dwFlags & TPK_FILE_ENCRYPTED) && hFileTo->dwSeed1 != hFileFrom->dwSeed1)
			{
				DecrypTPKBlock((DWORD *)hFileTo->pbFileBuffer, dwBytes, hFileFrom->dwSeed1 + nBlock);
				EncrypTPKBlock((DWORD *)hFileTo->pbFileBuffer, dwBytes, hFileTo->dwSeed1 + nBlock);
			}

			// д�ļ����ݿ鵽Ŀ���ļ�
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

	// ����ɹ�����Ҫ֪ͨ�����ļ���ʱ��һЩ�ı����
	if(nError == ERROR_SUCCESS)
	{
		DWORD dwTableSize;

		haTo->pLastFile  = NULL;
		haTo->dwBlockPos = 0;
		haTo->dwBuffPos  = 0;

		// ����HashTable���ļ����Ĵ洢λ��
		TempPos.QuadPart = hFileTo->TPKFilePos.QuadPart + hFileTo->pBlock->dwCSize;		
		haTo->HashTablePos.QuadPart = TempPos.QuadPart;
		haTo->pHeader->dwHashTablePos = TempPos.LowPart;
		haTo->pHeader->dwHashTablePosHigh = (USHORT)TempPos.HighPart;
		dwTableSize = haTo->pHeader->dwHashTableSize * sizeof(TPKHash);

		// ����BlockTable���ļ����Ĵ洢λ��
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
		// ������õ�Hash���Block������
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