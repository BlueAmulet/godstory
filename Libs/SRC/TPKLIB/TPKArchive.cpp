#include "TPKLib.h"
#include "TPKCommon.h"
#include "crc32.h"
#include <string>
#include <hash_map>

//-----------------------------------------------------------------------------
// Ƕ��HashTable��BlockTable�Ĵ�С
static DWORD ClampTableSize(DWORD Size)
{
	static const DWORD PowersOfTwo[] = 
	{
		0x0000004, 0x0000008, 0x0000010, 
		0x0000020, 0x0000040, 0x0000080,
		0x0000100, 0x0000200, 0x0000400, 
		0x0000800, 0x0001000, 0x0002000,
		0x0004000, 0x0008000, 0x0010000,
		0x0020000, 0x0040000
	};

	DWORD dwSize = Size;
	if(dwSize >= HASH_TABLE_SIZE_MAX)
	{
		return HASH_TABLE_SIZE_MAX;
	}

	// ȷ��HashTableSizeֻ����2��n����
	for(int nIndex = 0; PowersOfTwo[nIndex] != 0; nIndex++)
	{
		if(dwSize <= PowersOfTwo[nIndex])
		{
			dwSize = PowersOfTwo[nIndex];
			break;
		}
	}
	return dwSize;
}

//-----------------------------------------------------------------------------
// ΪTPKArchive�����趨HashTable��BlockTable���������
static int RelocateTPKTablePositions(TPKArchive* ha)
{
    TPKHeader* pHeader = ha->pHeader;
    LARGE_INTEGER TempSize;
    LARGE_INTEGER FileSize;

    // ��ȡ�ļ�����С
    FileSize.LowPart = GetFileSize(ha->hFile, (LPDWORD)&(FileSize.HighPart));

    // ����HashTable��λ��
    ha->HashTablePos.HighPart = pHeader->dwHashTablePosHigh;
    ha->HashTablePos.LowPart = pHeader->dwHashTablePos;
    if(ha->HashTablePos.QuadPart > FileSize.QuadPart)
        return ERROR_BAD_FORMAT;

    // ����BlockTable��λ��
    ha->BlockTablePos.HighPart = pHeader->dwBlockTablePosHigh;
    ha->BlockTablePos.LowPart = pHeader->dwBlockTablePos;
    if(ha->BlockTablePos.QuadPart > FileSize.QuadPart)
        return ERROR_BAD_FORMAT;

    TempSize.QuadPart = ha->HashTablePos.QuadPart + (pHeader->dwHashTableSize * sizeof(TPKHash));
    if(TempSize.QuadPart > ha->TPKSize.QuadPart)
        ha->TPKSize.QuadPart = TempSize.QuadPart;

    TempSize.QuadPart = ha->BlockTablePos.QuadPart + (pHeader->dwBlockTableSize * sizeof(TPKBlock));
    if(TempSize.QuadPart > ha->TPKSize.QuadPart)
        ha->TPKSize.QuadPart = TempSize.QuadPart;

    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// �򿪻򴴽�һ���µ��ļ���
// ����˵����
//   szTPKName				�ļ����ļ���
//   dwCreationDisposition	�ļ�������ʽ
//   ֵ				�ļ�����				�ļ�������
//   ----------     ---------------------  ---------------------
//   CREATE_NEW     ʧ��					�������ļ���
//   CREATE_ALWAYS  �����Ѵ��ڵ�			�������ļ���
//   OPEN_EXISTING  ���Ѵ��ڵ�			ʧ��
//   OPEN_ALWAYS    ���Ѵ��ڵ�			�������ļ���
//   
//   dwHashTableSize - HashTable�Ĵ�С�����ڴ������ļ���ʱ���ã����ұ�����16-262144֮���2�ı���ֵ��
//
//   phTPK - �������õ��ļ�������
//
BOOL WINAPI TPKCreateArchive(const char* szTPKName, DWORD dwCreationDisposition, DWORD dwHashTableSize, HANDLE* phTPK)
{
    TPKArchive* ha		= NULL;					// �ļ�������
    HANDLE hFile		= INVALID_HANDLE_VALUE;	// �ļ��������ļ����
    DWORD dwTransferred = 0;					// д�뵽�ļ������ֽ���
    BOOL bFileExists	= FALSE;				// �ļ��������ļ��Ƿ����
    int nError			= ERROR_SUCCESS;

    // Ԥ�����ļ�������ָ��
    if(phTPK != NULL)
        *phTPK = NULL;

    // ���������������Ƿ���Ч
    if(szTPKName == NULL || *szTPKName == 0 || phTPK == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    // ��ȡ�ļ��Ƿ��Ѿ�����
    bFileExists = (GetFileAttributes(szTPKName) != 0xFFFFFFFF);
    dwCreationDisposition &= 0x0000FFFF;
	// �ж��ļ��������ļ������벻���ڵ����
    if(bFileExists)
	{
		if(dwCreationDisposition == OPEN_EXISTING)
		{
			// ���Դ��ļ��������ʧ�ܣ���˵������TPK�ļ���
			return TPKOpenArchive(szTPKName, 0, phTPK, GENERIC_READ | GENERIC_WRITE);
		}
		else if(dwCreationDisposition == OPEN_ALWAYS)
		{
			if(TPKOpenArchive(szTPKName, 0, phTPK, GENERIC_READ | GENERIC_WRITE))
				return TRUE;
		}
		else if(dwCreationDisposition == CREATE_NEW)
		{
			SetLastError(ERROR_ALREADY_EXISTS);
			return FALSE;
		}
	}
	else
	{
		if(dwCreationDisposition == OPEN_EXISTING)
		{
			SetLastError(ERROR_FILE_NOT_FOUND);
			return FALSE;
		}
	}

    // Ƕ��HashTable�Ĵ�С��HASH_TABLE_SIZE_MIN��HASH_TABLE_SIZE_MAX֮��
	dwHashTableSize = ClampTableSize(dwHashTableSize);

    // ȷ����ʼ������ϣ�㷨BUFFER
    if(nError == ERROR_SUCCESS)
        nError = PrepareCryptBuffer();

    // ִ�е���һ����ֻ���Ǵ����ļ����ı��ؾ����
    if(nError == ERROR_SUCCESS)
    {
        EnsureDirectory(szTPKName);

        hFile = CreateFile(szTPKName,
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ,
                           NULL,
                           CREATE_ALWAYS,
                           0,
                           NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // Ϊ�ļ�����������ڴ�
    if(nError == ERROR_SUCCESS)
    {
        if((ha = ALLOCMEM(TPKArchive, 1)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // ��ʼ���ļ������󣬲�����HashTable��BlockTable����ض����ڴ�
    if(nError == ERROR_SUCCESS)
    {
        memset(ha, 0, sizeof(TPKArchive));
        strcpy(ha->szFileName, szTPKName);
        ha->hFile          = hFile;
        ha->pHeader        = &ha->Header;
        ha->pHashTable     = ALLOCMEM(TPKHash, dwHashTableSize);
        ha->pBlockTable    = ALLOCMEM(TPKBlock, dwHashTableSize);
        ha->pbBlockBuffer  = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE);
        ha->pListFile      = NULL;
        ha->dwFlags       |= TPK_FLAG_CHANGED;

        if(!ha->pHashTable || !ha->pBlockTable || !ha->pbBlockBuffer)
            nError = GetLastError();
        hFile = INVALID_HANDLE_VALUE;
    }

    // ���TPKHeader���ݺ�����һЩ����
    if(nError == ERROR_SUCCESS)
    {
        TPKHeader* pHeader = ha->pHeader;
        memset(pHeader, 0, sizeof(TPKHeader));
        pHeader->dwID				= TPK_FILEID;
		pHeader->dwVer				= TPK_VERSION;
        pHeader->dwHashTableSize	= dwHashTableSize;
        pHeader->dwHashTablePos		= sizeof(TPKHeader);
        pHeader->dwHashTablePosHigh = 0;
		pHeader->dwBlockTableSize	= dwHashTableSize;
        pHeader->dwBlockTablePos	= pHeader->dwHashTablePos + (pHeader->dwHashTableSize * sizeof(TPKHash));
        pHeader->dwBlockTablePosHigh= 0;

        // ΪHashTable��BlockTable��ʼ������
        memset(ha->pBlockTable, 0, sizeof(TPKBlock) * dwHashTableSize);
        memset(ha->pHashTable, 0xFF, sizeof(TPKHash) * dwHashTableSize);
    }

    // д��TPKHeaderͷ���ݵ��ļ��������ļ�
    if(nError == ERROR_SUCCESS)
    {
        WriteFile(ha->hFile, ha->pHeader, sizeof(TPKHeader), &dwTransferred, NULL);   
        if(dwTransferred != sizeof(TPKHeader))
            nError = ERROR_DISK_FULL;

        ha->TPKSize.QuadPart += dwTransferred;
    }

    //ȷ��tpk�ļ����л�������Ϣ
    RelocateTPKTablePositions(ha);
    SaveTPKTables(ha);

    // �����ļ��б��ļ�����
    if(nError == ERROR_SUCCESS)
        nError = TPKListFileCreateList(ha);

    // ��������ȱʡ���ļ��б��ļ�������Ҳ�������뵽�����б���
    if(nError == ERROR_SUCCESS)
    {
        if(TPKAddListFile((HANDLE)ha, NULL) != ERROR_SUCCESS)
            AddInternalFile(ha, TPK_LISTFILE_NAME);
    }

    // �������������д�����������ڴ����
    if(nError != ERROR_SUCCESS)
    {
        FreeTPKArchive(ha);
        if(hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
        SetLastError(nError);
        ha = NULL;
    }
    
    // ���ش��������ύ�ļ�������
    *phTPK = (HANDLE)ha;
    return nError == ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// ���ļ�������
// szTPKName		�ļ����ļ���
// dwFlags			�ļ���������ʶ(��:TPK_FLAG_NOLISTFILE��TPK_FLAG_PROTECTED)
// phTPK			�ļ�������
// dwAccessMode		����д������ʶ
BOOL WINAPI TPKOpenArchive(const char* szTPKName, DWORD dwFlags, HANDLE* phTPK, DWORD dwAccessMode)
{
    TPKArchive* ha	= NULL;
    HANDLE hFile	= INVALID_HANDLE_VALUE;    
    DWORD dwBytes	= 0;
    int nError		= ERROR_SUCCESS;
	LARGE_INTEGER TempPos;
	DWORD dwTransferred;   

    // �������Ƿ���ȷ
    if(nError == ERROR_SUCCESS)
    {
        if(szTPKName == NULL || *szTPKName == 0 || phTPK == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }

    //ȥ���ļ���ֻ������
    if(nError == ERROR_SUCCESS)
    {
        DWORD fileAttri = GetFileAttributes(szTPKName);

        if ((fileAttri & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
        {
            fileAttri &= ~FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(szTPKName,fileAttri);
        }
    }

    // ����ϣ�㷨Buffer�Ƿ��ʼ����
    if(nError == ERROR_SUCCESS)
        nError = PrepareCryptBuffer();

    // ���ļ���
    if(nError == ERROR_SUCCESS)
    {
        hFile = CreateFile(szTPKName, dwAccessMode, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        if(hFile == INVALID_HANDLE_VALUE)
        {
            nError = GetLastError();

            if (ERROR_SHARING_VIOLATION == nError)
            {
                Sleep(1000);
                hFile = CreateFile(szTPKName, dwAccessMode, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
            }

            if(hFile == INVALID_HANDLE_VALUE)
            {
                nError = GetLastError();
            }
        }
    }
    
	// �����ļ��������ڴ�
    if(nError == ERROR_SUCCESS)
    {
        if((ha = ALLOCMEM(TPKArchive, 1)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // ��ʼ���ļ�����������
    if(nError == ERROR_SUCCESS)
    {
        memset(ha, 0, sizeof(TPKArchive));
        strncpy(ha->szFileName, szTPKName, strlen(szTPKName));
        ha->hFile		= hFile;
        ha->pHeader		= &ha->Header;
        ha->pListFile	= NULL;
        hFile			= INVALID_HANDLE_VALUE;
    }

    if(nError == ERROR_SUCCESS)
    {
		SetFilePointer(ha->hFile, 0, 0, FILE_BEGIN);
		ReadFile(ha->hFile, ha->pHeader, sizeof(TPKHeader), &dwTransferred, NULL);
		// ���ȼ���ȡ�Ƿ�ɹ�
		if(dwTransferred != sizeof(TPKHeader) || ha->pHeader->dwID != TPK_FILEID || ha->pHeader->dwVer != TPK_VERSION)
			nError = ERROR_BAD_FORMAT;
	}

    // �����ļ���HashTable��BlockTable��һЩ����
    if(nError == ERROR_SUCCESS)
    {
        nError = RelocateTPKTablePositions(ha);
    }

    // ΪHashTable��BlockTable�����ڴ�
    if(nError == ERROR_SUCCESS)
    {
        ha->pHashTable     = ALLOCMEM(TPKHash,	ha->pHeader->dwHashTableSize);
        ha->pBlockTable    = ALLOCMEM(TPKBlock, ha->pHeader->dwBlockTableSize);
        ha->pbBlockBuffer  = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE);
        if(!ha->pHashTable || !ha->pBlockTable || !ha->pbBlockBuffer)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // ��ȡHashTable����
    if(nError == ERROR_SUCCESS)
    {
		dwBytes = ha->pHeader->dwHashTableSize * sizeof(TPKHash);
		memset(ha->pHashTable, 0, dwBytes);        
        SetFilePointer(ha->hFile, ha->HashTablePos.LowPart, &ha->HashTablePos.HighPart, FILE_BEGIN);
        ReadFile(ha->hFile, ha->pHashTable, dwBytes, &dwTransferred, NULL);
        if(dwTransferred != dwBytes)
            nError = ERROR_FILE_CORRUPT;
    }

    // ����HashTable���ݣ������������ȷ��
    if(nError == ERROR_SUCCESS)
    {
        DecryptHashTable((DWORD*)ha->pHashTable, (BYTE*)TPK_HASHTABLE_KEY, dwBytes >> 2);		
		if(ha->dwFlags & TPK_FLAG_PROTECTED)
		{
			//������Կ��ǶԽ���HashTable������ݽ��м��
		}
    }

    // ��ȡBlockTable����
    if(nError == ERROR_SUCCESS)
    {
		dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TPKBlock);
        memset(ha->pBlockTable, 0, dwBytes);		
        SetFilePointer(ha->hFile, ha->BlockTablePos.LowPart, &ha->BlockTablePos.HighPart, FILE_BEGIN);
        ReadFile(ha->hFile, ha->pBlockTable, dwBytes, &dwTransferred, NULL);
		if(dwTransferred != dwBytes)
			nError = ERROR_FILE_CORRUPT;
    }

    // ����BlockTable���ݣ������������ȷ��
    if(nError == ERROR_SUCCESS)
    {
        DecryptBlockTable((DWORD*)ha->pBlockTable, (BYTE*)TPK_BLOCKTABLE_KEY, dwBytes >> 2);
		if(ha->dwFlags & TPK_FLAG_PROTECTED)
		{
			TPKBlock* pBlockEnd	= ha->pBlockTable + ha->pHeader->dwBlockTableSize;
			TPKBlock* pBlock	= ha->pBlockTable;
			for(; pBlock < pBlockEnd; pBlock++)
			{
				if(pBlock->dwFlags & TPK_FILE_EXISTS)
				{
					TempPos.HighPart = pBlock->dwFilePosHigh;
					TempPos.LowPart = pBlock->dwFilePos;
					if(TempPos.QuadPart > ha->TPKSize.QuadPart || pBlock->dwCSize > ha->TPKSize.QuadPart)
					{
						nError = ERROR_BAD_FORMAT;
						break;
					}
				}
			}
		}
    }

    // ���δ�ر�ָ��TPK_FLAG_NOLISTFILE���򴴽��ڲ�ȱʡ���ļ��б�
    if(nError == ERROR_SUCCESS)
    {
        if((dwFlags & TPK_FLAG_NOLISTFILE) == 0)
        {
            if(nError == ERROR_SUCCESS)
                nError = TPKListFileCreateList(ha);

            // ����ļ��б�
            if(nError == ERROR_SUCCESS)
                TPKAddListFile((HANDLE)ha, NULL);
        }
    }

    // ������˳�
    if(nError != ERROR_SUCCESS)
    {
        FreeTPKArchive(ha);

        if(hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);

        SetLastError(nError);
        ha = NULL;
    }
    *phTPK = ha;
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// ��д�ļ��������ݵ�����Ӳ��
BOOL WINAPI TPKFlushArchive(HANDLE hTPK)
{
    TPKArchive* ha = (TPKArchive*)hTPK;    
    // У���Ƿ���һ��TPK�ļ�������
    if(!IsValidTPKHandle(ha))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    // ����ļ��������и��£���д��Ӳ��
    if(ha->dwFlags & TPK_FLAG_CHANGED)
    {
        TPKListFileSaveToTPK(ha);
        SaveTPKTables(ha);
        ha->dwFlags &= ~TPK_FLAG_CHANGED;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// �ر��ļ���
BOOL WINAPI TPKCloseArchive(HANDLE hTPK)
{
    TPKArchive * ha = (TPKArchive *)hTPK;
    
    // ��д���ĵ����ݵ�����Ӳ��
    if(!TPKFlushArchive(hTPK))
        return FALSE;

    // �ͷ��ļ��������ж����ڴ�
    FreeTPKArchive(ha);
    return TRUE;
}

//-----------------------------------------------------------------------------
// �ͷ�TPK�ļ�������
void FreeTPKArchive(TPKArchive *& ha)
{
    if(ha != NULL)
    {
        FREEMEM(ha->pbBlockBuffer);
        FREEMEM(ha->pBlockTable);
        FREEMEM(ha->pHashTable);
        if(ha->pListFile != NULL)
            TPKListFileFreeList(ha);
        if(ha->hFile != INVALID_HANDLE_VALUE)
            CloseHandle(ha->hFile);
        FREEMEM(ha);
        ha = NULL;
    }
}

//-----------------------------------------------------------------------------
// �����ļ��������TPKHeader��HashTable��BlockTable�������ݣ����ĳ�ļ����ļ�
// ��֮ǰ���������ļ�����С������Щ���ݣ�����ϵ�ʱ����Щ���ݱ����ļ������ݸ���
// �������ļ������ɶ���
DWORD SaveTPKTablesForAddFile(TPKArchive* ha, LARGE_INTEGER& dwNewHashTablePos)
{
	TPKHeader		tmpHeader;
	TPKHash*		tmpHashTable		= NULL;
	TPKBlock*		tmpBlockTable		= NULL;
	DWORD			dwHashTableBytes;
	DWORD			dwBlockTableBytes;
	DWORD			dwTransferred;
	LARGE_INTEGER	dwHashTablePos;
	LARGE_INTEGER	dwBlockTablePos;
	DWORD nError						= ERROR_SUCCESS;

	if(nError == ERROR_SUCCESS)
	{
		SetFilePointer(ha->hFile, 0, 0, FILE_BEGIN);
		ReadFile(ha->hFile, &tmpHeader, sizeof(TPKHeader), &dwTransferred, NULL);		
	}

	if(nError == ERROR_SUCCESS)
	{
		tmpHashTable	= ALLOCMEM(TPKHash,	tmpHeader.dwHashTableSize);
		tmpBlockTable	= ALLOCMEM(TPKBlock, tmpHeader.dwBlockTableSize);
		if(!tmpHashTable || !tmpBlockTable)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}

	// ��ȡHashTable����
	if(nError == ERROR_SUCCESS)
	{
		dwHashTablePos = dwNewHashTablePos;
		dwHashTableBytes = tmpHeader.dwHashTableSize * sizeof(TPKHash);
		dwBlockTablePos.QuadPart = dwHashTablePos.QuadPart + dwHashTableBytes;
		memset(tmpHashTable, 0, dwHashTableBytes);        
		SetFilePointer(ha->hFile, tmpHeader.dwHashTablePos, (PLONG)&tmpHeader.dwHashTablePosHigh, FILE_BEGIN);
		ReadFile(ha->hFile, tmpHashTable, dwHashTableBytes, &dwTransferred, NULL);
		if(dwTransferred != dwHashTableBytes)
			nError = ERROR_FILE_CORRUPT;
	}

	// ��ȡBlockTable����
	if(nError == ERROR_SUCCESS)
	{
		dwBlockTableBytes = tmpHeader.dwBlockTableSize * sizeof(TPKBlock);
		memset(tmpBlockTable, 0, dwBlockTableBytes);
		SetFilePointer(ha->hFile, tmpHeader.dwBlockTablePos, (PLONG)&tmpHeader.dwBlockTablePosHigh, FILE_BEGIN);
		ReadFile(ha->hFile, tmpBlockTable, dwBlockTableBytes, &dwTransferred, NULL);
		if(dwTransferred != dwBlockTableBytes)
			nError = ERROR_FILE_CORRUPT;
	}

	if(nError == ERROR_SUCCESS)
	{
		if(SetFilePointer(ha->hFile, dwNewHashTablePos.LowPart, &dwNewHashTablePos.HighPart, FILE_BEGIN) == 0xFFFFFFFF)
		{
			nError = GetLastError();
		}
	}

	if(nError == ERROR_SUCCESS)
	{
		if(!SetEndOfFile(ha->hFile))
			nError = GetLastError();
	}

	// �洢HashTable����
	if(nError == ERROR_SUCCESS)
	{
		// ����HashTable������λ��
		SetFilePointer(ha->hFile, dwHashTablePos.LowPart, &dwHashTablePos.HighPart, FILE_BEGIN);
		WriteFile(ha->hFile, tmpHashTable, dwHashTableBytes, &dwTransferred, NULL);
		if(dwTransferred != dwHashTableBytes)
			nError = ERROR_DISK_FULL;
	}

	// �洢BlockTable����
	if(nError == ERROR_SUCCESS)
	{
		// ����BlockTable������λ��
		SetFilePointer(ha->hFile, dwBlockTablePos.LowPart, (PLONG)&dwBlockTablePos.HighPart, FILE_BEGIN);
		WriteFile(ha->hFile, tmpBlockTable, dwBlockTableBytes, &dwTransferred, NULL);
		if(dwTransferred != dwBlockTableBytes)
			nError = ERROR_DISK_FULL;
	}

	if(nError == ERROR_SUCCESS)
	{
		if(!SetEndOfFile(ha->hFile))
			nError = GetLastError();
	}

	if(nError == ERROR_SUCCESS)
	{
		tmpHeader.dwHashTablePos		= dwHashTablePos.LowPart;
		tmpHeader.dwHashTablePosHigh	= dwHashTablePos.HighPart;
		tmpHeader.dwBlockTablePos		= dwBlockTablePos.LowPart;
		tmpHeader.dwBlockTablePosHigh	= dwBlockTablePos.HighPart;
		// �����ļ���ͷ����������λ��
		SetFilePointer(ha->hFile, 0, NULL, FILE_BEGIN);
		WriteFile(ha->hFile, &tmpHeader, sizeof(TPKHeader), &dwTransferred, NULL);
		if(dwTransferred != sizeof(TPKHeader))
			nError = ERROR_DISK_FULL;
	}

	if(tmpHashTable)
		FREEMEM(tmpHashTable);
	if(tmpBlockTable)
		FREEMEM(tmpBlockTable);

    return nError;
}

//-----------------------------------------------------------------------------
// �����ļ��������TPKHeader��HashTable��BlockTable��������
int SaveTPKTables(TPKArchive* ha)
{
	BYTE* pbBuffer	= NULL;		// ��ʱ���ڼ��ܵ����ݻ�����
	int   nError	= ERROR_SUCCESS;
	DWORD dwBytes;
	DWORD dwWritten;

	// Ϊ����Buffer��ʱ�����ڴ�
	if(nError == ERROR_SUCCESS)
	{
		pbBuffer = ALLOCMEM(BYTE, sizeof(TPKBlock) * ha->pHeader->dwBlockTableSize);
		if(pbBuffer == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}

	// �洢�ļ�ͷ����
	if(nError == ERROR_SUCCESS)
	{
		// �����ļ���ͷ����������λ��
		SetFilePointer(ha->hFile, 0, NULL, FILE_BEGIN);
		WriteFile(ha->hFile, ha->pHeader, sizeof(TPKHeader), &dwWritten, NULL);
		if(dwWritten != sizeof(TPKHeader))
			nError = ERROR_DISK_FULL;
	}

	// �洢HashTable����
	if(nError == ERROR_SUCCESS)
	{
		// ����HashTable���ݵ���ʱBuffer
		dwBytes = ha->pHeader->dwHashTableSize * sizeof(TPKHash);
		memcpy(pbBuffer, ha->pHashTable, dwBytes);

		// ����HashTable����
		EncryptHashTable((DWORD*)pbBuffer, (BYTE*)TPK_HASHTABLE_KEY, dwBytes >> 2);

		// ����HashTable������λ��
		SetFilePointer(ha->hFile, ha->HashTablePos.LowPart, (PLONG)&ha->HashTablePos.HighPart, FILE_BEGIN);
		WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
		if(dwWritten != dwBytes)
			nError = ERROR_DISK_FULL;
	}

	// �洢BlockTable����
	if(nError == ERROR_SUCCESS)
	{
		// ����BlockTable���ݵ���ʱBuffer
		dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TPKBlock);
		memcpy(pbBuffer, ha->pBlockTable, dwBytes);

		// ����BlockTable����
		EncryptBlockTable((DWORD*)pbBuffer, (BYTE*)TPK_BLOCKTABLE_KEY, dwBytes >> 2);

		// ����HashTable������λ��
		SetFilePointer(ha->hFile, ha->BlockTablePos.LowPart, (PLONG)&ha->BlockTablePos.HighPart, FILE_BEGIN);
		WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
		if(dwWritten != dwBytes)
			nError = ERROR_DISK_FULL;
	}

	if(nError == ERROR_SUCCESS)
	{
		SetEndOfFile(ha->hFile);
	}

	// ������˳�
	if(pbBuffer != NULL)
		FREEMEM(pbBuffer);
	return nError;
}

//-----------------------------------------------------------------------------
// ��չBlockTable
DWORD ExtendBlockTable(TPKArchive* ha)
{
	DWORD nError = ERROR_SUCCESS;
	DWORD dwBlockTableSize = 0;
	TPKBlock* pBlockTableCopy = NULL;

	if(nError == ERROR_SUCCESS)
	{
		dwBlockTableSize = ha->pHeader->dwBlockTableSize << 1;
		ClampTableSize(dwBlockTableSize);

		pBlockTableCopy = ALLOCMEM(TPKBlock, dwBlockTableSize);
		if(pBlockTableCopy == NULL)
			return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	if(nError == ERROR_SUCCESS)
	{
		memset(pBlockTableCopy, 0x0, sizeof(TPKBlock) * dwBlockTableSize);
		memcpy(pBlockTableCopy, ha->pBlockTable, ha->pHeader->dwBlockTableSize * sizeof(TPKBlock));
		FREEMEM(ha->pBlockTable);
		ha->pBlockTable = pBlockTableCopy;
		ha->pHeader->dwBlockTableSize = dwBlockTableSize;
	}
	return nError;
}

//-----------------------------------------------------------------------------
// ��һ�������ļ���ӵ��ļ�����
int AddFileToArchive(TPKArchive* ha,			// �ļ�������
					 HANDLE hFile,				// ����ӵı����ļ����
					 const char* szFileName,	// ����ӵı����ļ���
					 DWORD dwFlags,				// �ļ�������ʶ����TPK_FILEOPö�٣�
					 BOOL* pbReplaced			// �Ƿ����滻
					 )
{
    LARGE_INTEGER TempPos;					// ������ʱ�洢ƫ�Ƶ�ַ�ļ�����
    TPKBlock* pBlockEnd = NULL;				// BlockTable��β����
    TPKFile*  hf = NULL;					// ����ӵ��ļ�����
    BYTE* pbCompressed = NULL;				// ���ڴ���ѹ�������ݻ�����
    BYTE* pbToWrite = NULL;					// ׼��д����̵����ݻ�����
    DWORD dwBlockPosLen = 0;				// Blockλ�����ݿ鳤��
    DWORD dwTransferred = 0;				// ��д������ʵ�ʴ�С
    DWORD dwFileSizeHigh = 0;				// �ļ���С�ĸ�λ�ֽڣ�֧��4G���ϣ�
    DWORD dwFileSize = 0;					// �ļ���С�ĵ�λ�ֽ�
    BOOL bReplaced = FALSE;					// �Ƿ��滻�ļ�������ļ�
    int nError = ERROR_SUCCESS;

	// �����������Ƿ���ȷ
	if(ha == NULL || hFile == INVALID_HANDLE_VALUE || szFileName == NULL || *szFileName == 0)
	{
		nError = ERROR_INVALID_PARAMETER;
	}

    // ��ò������ļ���С
    if(nError == ERROR_SUCCESS)
    {
		pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
        dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

        // ����ļ�̫С�����ؼ���
        if(dwFileSize < 0x04)
            dwFlags &= ~(TPK_FILE_ENCRYPTED | TPK_FILE_FIXSEED);
		// ����ļ���С������ѹ��
        if(dwFileSize < 0x20)
            dwFlags &= ~TPK_FILE_COMPRESSED;

        // �����ļ����ܳ���4GB�ֽ�
        if(dwFileSizeHigh != 0)
            nError = ERROR_PARAMETER_QUOTA_EXCEEDED;
    }

	// ���￼�ǡ��ϵ籣��������Ŀǰ����ļ��Ĵ�С���ļ���βŲ����ͬ
	// �ֽں�д��TPKHeader���ݵ������ļ�����Ҳ����˵������
	// �ں����ļ�����д�����ʱͻȻ�ϵ磬Ҳ���ᶪʧTPKHeader����
	if(nError == ERROR_SUCCESS && (dwFlags & TPK_FILE_BREAKPROTECTED))
	{
		LARGE_INTEGER dwNewHashTablePos;
		dwNewHashTablePos.LowPart = ha->pHeader->dwHashTablePos;
		dwNewHashTablePos.HighPart = ha->pHeader->dwHashTablePosHigh;
		dwNewHashTablePos.QuadPart += dwFileSize;
		nError = SaveTPKTablesForAddFile(ha, dwNewHashTablePos);
	}

    // ΪTPKFile��������ڴ�
    if(nError == ERROR_SUCCESS)
    {
        hf = (TPKFile*)ALLOCMEM(BYTE, sizeof(TPKFile) + strlen(szFileName));
        if(hf == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // ��ʼ��TPKFile��������
    if(nError == ERROR_SUCCESS)
    {
        memset(hf, 0, sizeof(TPKFile));
        strcpy(hf->szFileName, szFileName);
        hf->hFile = INVALID_HANDLE_VALUE;
        hf->ha = ha;

        // ����Ƿ���ͬ���ļ��Ѿ�����
        if((hf->pHash = GetHashEntry(ha, szFileName)) != NULL)
        {
			if((dwFlags & TPK_FILE_REPLACEEXIST) == 0)
			{
				nError = ERROR_ALREADY_EXISTS;
				hf->pHash = NULL;
			}
			else
			{
				hf->pBlock = ha->pBlockTable + hf->pHash->dwBlockIndex;
				bReplaced = TRUE;
			}
        }

		// ���ȷ�������ļ������HashTable���ҳ�һ������Hash��
        if(nError == ERROR_SUCCESS && hf->pHash == NULL)
        {
            hf->pHash = FindFreeHashEntry(ha, szFileName);			
            if(hf->pHash == NULL)
			{
				// Note:��������HashTable�������������������·���HashTable��
				//��������Hash������¼ӽ������ݵȵȣ�̫�࣬�ò���ʧ��
				nError = ERROR_HANDLE_DISK_FULL;				
			}
		}        
    }

    // ��BlockTable������һ�����е�Block�������ļ�
    if(nError == ERROR_SUCCESS)
    {
		// ������HashTable�е�����λ��
		hf->dwHashIndex = (DWORD)(hf->pHash - ha->pHashTable);
        // �ҳ���һ���ļ��������ݵ�
        hf->TPKFilePos.QuadPart = sizeof(TPKHeader);

        // �������е�Block�˳�������ܴ洢��ǰ�ļ���λ��
        for(TPKBlock* pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
        {
            if(pBlock->dwFlags & TPK_FILE_EXISTS)
            {
                TempPos.HighPart  = pBlock->dwFilePosHigh;
                TempPos.LowPart   = pBlock->dwFilePos;
                TempPos.QuadPart += pBlock->dwCSize;

                if(TempPos.QuadPart > hf->TPKFilePos.QuadPart)
                    hf->TPKFilePos = TempPos;
            }
            else
            {
                //�����replace,��block���䣬���������¼���һ���µ�block�����������
				if(hf->pBlock == NULL)
					hf->pBlock = pBlock;
            }
        }

		// �����Hash�ȴ�Ҳ���һ�����е�Block����ã����˳�
		if(hf->pBlock == NULL)
        {	
			DWORD dwOldBlockTableSize = ha->pHeader->dwBlockTableSize;
			if(ExtendBlockTable(ha))
			{
				hf->pBlock = ha->pBlockTable+ dwOldBlockTableSize;
			}
			
			if(hf->pBlock == NULL)
			{
				nError = ERROR_HANDLE_DISK_FULL;
			}
		}	
	}

    // ����ļ���Ҫ���ܣ�������Կ
    if(nError == ERROR_SUCCESS && (dwFlags & TPK_FILE_ENCRYPTED))
    {
        const char* szTemp = strrchr(szFileName, '\\');
        if(szTemp != NULL)
            szFileName = szTemp + 1;

        hf->dwSeed1 = DecryptFileSeed(szFileName);
        if(dwFlags & TPK_FILE_FIXSEED)
            hf->dwSeed1 = (hf->dwSeed1 + hf->TPKFilePos.LowPart) ^ dwFileSize;
    }

    // Ϊѹ�����ݷ�����ʱ������
    if(nError == ERROR_SUCCESS)
    {
		hf->dwBlockIndex = (DWORD)(hf->pBlock - ha->pBlockTable);
        hf->nBlocks = (dwFileSize / TPK_DATABLOCK_SIZE) + 1;
        if(dwFileSize % TPK_DATABLOCK_SIZE)
            hf->nBlocks++;

        if((hf->pbFileBuffer = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = hf->pbFileBuffer;
    }

    // Ϊѹ���ļ�������ѹ����λ�û��������洢ÿ��ѹ�������ݿ�Ĵ�С��
    if(nError == ERROR_SUCCESS && (dwFlags & TPK_FILE_COMPRESSED))
    {
        hf->pdwBlockPos = ALLOCMEM(DWORD, hf->nBlocks + 1);
        pbCompressed = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE * 2);
        if(hf->pdwBlockPos == NULL || pbCompressed == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = pbCompressed;
    }

    // �����ļ�����λ�ã�������Hash���Block������
    if(nError == ERROR_SUCCESS)
    {
        // ���õ��ļ�������ʼλ��
        SetFilePointer(ha->hFile, hf->TPKFilePos.LowPart, &hf->TPKFilePos.HighPart, FILE_BEGIN);

        // ���õ�Hash���BlockTable������ֵ
        hf->pHash->dwBlockIndex = hf->dwBlockIndex;

        // ����Block������
        hf->pBlock->dwFilePosHigh = (USHORT)hf->TPKFilePos.HighPart;
        hf->pBlock->dwFilePos = hf->TPKFilePos.LowPart;
        hf->pBlock->dwFSize   = dwFileSize;
        hf->pBlock->dwCSize   = 0;
        hf->pBlock->dwFlags   = dwFlags | TPK_FILE_EXISTS;
    }

    // �������Ҫѹ����Ҫ�������ݿ��λ������
    if(nError == ERROR_SUCCESS && (dwFlags & TPK_FILE_COMPRESSED))
    {
        dwBlockPosLen = hf->nBlocks * sizeof(DWORD);
        memset(hf->pdwBlockPos, 0, dwBlockPosLen);
        hf->pdwBlockPos[0] = dwBlockPosLen;        

        WriteFile(ha->hFile, hf->pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
        if(dwTransferred == dwBlockPosLen)
            hf->pBlock->dwCSize += dwBlockPosLen;
        else
            nError = GetLastError();
    }

    // д�����е����ݿ�
    if(nError == ERROR_SUCCESS)
    {
        crc32_context crc32_ctx;
        DWORD nBlock;       

        // ��ʼ��crc32
        CRC32_Init(&crc32_ctx);

        // �ƶ��ļ�����ָ�뵽��ʼλ��
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        for(nBlock = 0; nBlock < hf->nBlocks-1; nBlock++)
        {
            DWORD dwInLength  = TPK_DATABLOCK_SIZE;
            DWORD dwOutLength = TPK_DATABLOCK_SIZE;

            // ÿ�ζ�ȡTPK_DATABLOCK_SIZE��С������
            ReadFile(hFile, hf->pbFileBuffer, TPK_DATABLOCK_SIZE, &dwInLength, NULL);
            if(dwInLength == 0)
                break;

            // ����CRC32ֵ
            CRC32_Update(&crc32_ctx, hf->pbFileBuffer, dwInLength);

            // �����Ҫѹ��������ѹ������
            dwOutLength = dwInLength;
            if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
            {
                int nOutLength = TPK_DATABLOCK_SIZE * 2;
				Compress_zlib((char*)pbCompressed, &nOutLength,(char*)hf->pbFileBuffer,dwInLength);
                // ���ѹ��ʧ�ܻ�ѹ�������������Դ���ݻ�Ҫ�󣬾�ֱ�ӿ�����ѹ��
                if(nOutLength >= (int)dwInLength)
                {
                    memcpy(pbCompressed, hf->pbFileBuffer, dwInLength);
                    nOutLength = dwInLength;
                }

                // �������ݿ�λ�ü�¼
                dwOutLength = nOutLength;
                hf->pdwBlockPos[nBlock+1] = hf->pdwBlockPos[nBlock] + dwOutLength;
            }

            // �����Ҫ���ܣ������������
            if(hf->pBlock->dwFlags & TPK_FILE_ENCRYPTED)
            {
                EncrypTPKBlock((DWORD*)pbToWrite, dwOutLength, hf->dwSeed1 + nBlock);
            }
            
            // д�ļ����ݿ鵽���ش���
            WriteFile(ha->hFile, pbToWrite, dwOutLength, &dwTransferred, NULL);
            if(dwTransferred != dwOutLength)
            {
                nError = ERROR_DISK_FULL;
                break;
            }

            // ����Block�����ļ���ѹ���ֽڴ�С
            hf->pBlock->dwCSize += dwTransferred;
        }

        // ����CRC32ֵд�뵽Block����ļ�Crc32�ֶ���
        CRC32_Finish(&crc32_ctx, (unsigned long*)&hf->pBlock->dwCrc32);
    }

    // ���´洢���ݿ������λ��
    if(nError == ERROR_SUCCESS && (hf->pBlock->dwFlags & TPK_FILE_COMPRESSED))
    {
        // ������ݿ����ݱ����ܣ���ôλ�ÿ�Ҳ��Ҫ����
        if(dwFlags & TPK_FILE_ENCRYPTED)
            EncrypTPKBlock(hf->pdwBlockPos, dwBlockPosLen, hf->dwSeed1 - 1);
        
        // �������õ��ļ���ʼλ��
        SetFilePointer(ha->hFile, hf->TPKFilePos.LowPart, &hf->TPKFilePos.HighPart, FILE_BEGIN);
        
        // ��дλ�����ݿ�
        WriteFile(ha->hFile, hf->pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
        if(dwTransferred != dwBlockPosLen)
            nError = ERROR_DISK_FULL;
    }


    // ����ɹ�����Ҫ֪ͨ�����ļ���ʱ��һЩ�ı����
    if(nError == ERROR_SUCCESS)
	{
		DWORD dwTableSize;

		ha->pLastFile  = NULL;
		ha->dwBlockPos = 0;
		ha->dwBuffPos  = 0;

		// ����HashTable���ļ����Ĵ洢λ��
		TempPos.QuadPart = hf->TPKFilePos.QuadPart + hf->pBlock->dwCSize;		
		ha->HashTablePos.QuadPart = TempPos.QuadPart;
		ha->pHeader->dwHashTablePos = TempPos.LowPart;
		ha->pHeader->dwHashTablePosHigh = (USHORT)TempPos.HighPart;
		dwTableSize = ha->pHeader->dwHashTableSize * sizeof(TPKHash);

		// ����BlockTable���ļ����Ĵ洢λ��
		TempPos.QuadPart += dwTableSize;
		ha->BlockTablePos.QuadPart = TempPos.QuadPart;
		ha->pHeader->dwBlockTablePosHigh = (USHORT)TempPos.HighPart;
		ha->pHeader->dwBlockTablePos = TempPos.LowPart;
		dwTableSize = ha->pHeader->dwBlockTableSize * sizeof(TPKBlock);

		TempPos.QuadPart += dwTableSize;
		ha->TPKSize = TempPos;
		ha->dwFlags |= TPK_FLAG_CHANGED;
	}
	else
    {
		// ������õ�Hash���Block������
		if(hf != NULL)
		{
			if(hf->pHash != NULL)
				memset(hf->pHash, 0xFF, sizeof(TPKHash));
			if(hf->pBlock != NULL)
				memset(hf->pBlock, 0, sizeof(TPKBlock));
		}
    }

    // ������˳�
    if(pbCompressed != NULL)
        FREEMEM(pbCompressed);
    if(pbReplaced != NULL)
        *pbReplaced = bReplaced;
    FreeTPKFile(hf);
    return nError;
}

//-----------------------------------------------------------------------------
// ��ת��б��
static void forwardslash(char *str)
{
	while(*str)
	{
		if(*str == '\\')
			*str = '/';
		str++;
	}
}

//-----------------------------------------------------------------------------
// ��ת��б��
static void backslash(char *str)
{
	while(*str)
	{
		if(*str == '/')
			*str = '\\';
		str++;
	}
}

static BOOL CreateDir(const char* file)
{
	TempAlloc<char> pathbuf((DWORD)strlen(file) + MAX_PATH * 3);
	const char* dir;
	pathbuf[0] = 0;
	unsigned int pathLen = 0;

	BOOL First = TRUE;
	while((dir = strchr(file, '\\')) != NULL)
	{
		strncpy(pathbuf + pathLen, file, dir - file + 1);
		pathbuf[pathLen + dir-file + 1] = 0;

		if(First && strchr(pathbuf, ':') != NULL)
		{
			First = FALSE;
			pathLen = pathLen + (dir - file);
			pathbuf[pathLen++] = '\\';
			file = dir + 1;
			continue;
		}
		CreateDirectory((LPCTSTR)(char*)pathbuf, NULL);
		if(GetLastError() == ERROR_PATH_NOT_FOUND)
			return FALSE;
		pathLen = pathLen + (dir - file);
		pathbuf[pathLen++] = '\\';
		file = dir + 1;
	}
	return TRUE;
}

static BOOL ParseFileName(const char* filename, const char* TargetPath, char** tpkfile, char**)
{
	static char buffer[_MAX_PATH] = {0};
	static char dir[_MAX_PATH] = {0};
	strncpy(dir, filename, _MAX_PATH);
	size_t len = strlen(dir);
	BOOL ret = FALSE;
	*tpkfile = NULL;

	for (size_t i = 0; i < len; i++)
	{
		if (dir[i] == '\\' || dir[i] == '/')
		{
			char tmp = dir[i];
			dir[i] = 0;
			sprintf(buffer, "%s\\%s.tpk", TargetPath, dir);
			if(0xFFFFFFFF != GetFileAttributes(buffer))
			{
				*tpkfile = buffer;
				return TRUE;
			}
			sprintf(buffer, "%s\\%s", TargetPath, dir);
			if (0xFFFFFFFF == GetFileAttributes(buffer))
			{
				if(::CreateDirectory(buffer,0) == FALSE)
					return FALSE;
			}
			dir[i] = tmp;
		}
	}
	return TRUE;
}


struct stLine
{
	char ToFileName[MAX_PATH];
	char FromFileName[MAX_PATH];
	char TPKFileName[MAX_PATH];
};

// ����(version)�ļ�����ȡ�ļ�����
BOOL FindVersionFile(HANDLE hTPK, std::vector<stLine*>& FileList)
{
	DWORD nError = ERROR_SUCCESS;
	HANDLE hFile = NULL;
	if(TPKOpenFile(hTPK, TPK_VERFILE_NAME, 0, &hFile))
	{
		FileList.clear();
		char buffer[0x400];
		DWORD dwBytes;
		int pos = 0;
		for(;;)
		{
			TPKSetFilePointer(hFile, pos, 0, FILE_BEGIN);
			if(!TPKReadFile(hFile, buffer, sizeof(buffer), &dwBytes, NULL))
				nError = GetLastError();
			if(nError == ERROR_HANDLE_EOF)
				nError = ERROR_SUCCESS;
			if(dwBytes == 0)
				break;

			int i = 0;
			while(i < sizeof(buffer))
			{	
				if(buffer[i] == 0x0D || buffer[i] == 0x0A)
					break;
				i++;
			}
			pos += i+1;

			buffer[i] = 0;
			char* szPrefix = strchr(buffer, '|');
			if(szPrefix)
			{
				stLine* line = new stLine;
				memset(line , 0, sizeof(stLine));
				strncpy(line->ToFileName, buffer, szPrefix - buffer);
				szPrefix++;
				if(szPrefix && szPrefix[0])
				{
					strcpy(line->TPKFileName, szPrefix);
					char* ext = strchr(line->TPKFileName, '.');
					strncpy(line->FromFileName, line->TPKFileName, ext - line->TPKFileName);
					strcat(line->FromFileName, "\\");
					strcat(line->FromFileName, line->ToFileName);
				}
				else
				{
					line->TPKFileName[0] = 0;
					strcpy(line->FromFileName, line->ToFileName);
				}
				FileList.push_back(line);
			}
		}
		if(FileList.empty())
			nError = ERROR_FILE_INVALID;
	}
	else
		nError = ERROR_OPEN_FAILED;
	return nError == ERROR_SUCCESS;
}


// ---------------------------------------------------------------------
// ������������ļ����������ļ������������ӦĿ���ļ����ڣ�
BOOL WINAPI TPKUnPackFileAll(HANDLE hTPK, const char* TargetPath)
{
	DWORD nError = ERROR_SUCCESS;
	typedef stdext::hash_map<std::string, HANDLE> TPKCACHE;
	TPKCACHE cache;
	TempAlloc<char> szPackFileName((DWORD)strlen(TargetPath) + MAX_PATH * 3);

	// ��ȡ�ļ����ڵĸ����ļ��б�
	std::vector<stLine*> arrLine;	
	if(FindVersionFile(hTPK, arrLine) == FALSE)
		nError = ERROR_FILE_NOT_FOUND;
    
	if(nError == ERROR_SUCCESS)
	{
		HANDLE hFileFrom = NULL;
		for(size_t i = 0; i < arrLine.size(); i++)
		{
			stLine* line = arrLine[i];
			// �ж��Ƿ���Ҫ������ļ�����
			if(line->TPKFileName[0])
			{
				if(!TPKOpenFile(hTPK, line->FromFileName, 0, &hFileFrom))
				{
					nError = ERROR_FILE_NOT_FOUND;
					break;
				}

				HANDLE hTPKTo = NULL;
				// ��ȡĿ���ļ����ľ�����ž�Ƶ�����ļ�������Cache��ʽ������
				{
					sprintf(szPackFileName, "%s\\%s", TargetPath, line->TPKFileName);
					std::string strPackFileName = szPackFileName;
					TPKCACHE::iterator itFind = cache.find(strPackFileName);
					if(itFind == cache.end())
					{
						//�������д���ļ�����
						ClearFileAttributes(szPackFileName);
						if(!TPKOpenArchive(szPackFileName, 0, &hTPKTo, GENERIC_READ | GENERIC_WRITE))
						{
							nError = ERROR_OPEN_FAILED;
							break;
						}
						cache.insert(TPKCACHE::value_type(strPackFileName, hTPKTo));
					}
					else
					{
						hTPKTo = (HANDLE)itFind->second;
					}
				}
				
				// ��ԴTPK�ļ����︴���ļ����ݵ�Ŀ���ļ�����
				if(!TPKCopyFile(hFileFrom, hTPKTo, line->ToFileName))
				{
					nError = ERROR_FILE_INVALID;
					break;
				}
				if(CompactCB != NULL)
					CompactCB(lpUserData, CCB_COPYING_FILES, (DWORD)i, (DWORD)arrLine.size());
			}
			else
			{
				sprintf(szPackFileName, "%s\\%s", TargetPath, line->FromFileName);
				// �ݹ�Ŀ��·�������ڽ���ļ�����ȷ��·����
				EnsureDirectory(szPackFileName);
				//�������д���ļ�����
				ClearFileAttributes(szPackFileName);
				if(!TPKExtractFile(hTPK, line->FromFileName, szPackFileName))
				{
					nError = ERROR_FILE_INVALID;
					break;
				}

				if(CompactCB != NULL)
					CompactCB(lpUserData, CCB_COPYING_FILES, (DWORD)i, (DWORD)arrLine.size());
			}
		}
	}

	// ���Ŀ��TPK�ļ���Cache�б�
	{
		for(TPKCACHE::iterator itB = cache.begin(); itB != cache.end(); ++itB)
		{
			TPKCloseArchive(itB->second);
		}
		cache.clear();
	}

	if(hTPK != NULL)
		TPKCloseArchive(hTPK);
	return nError == ERROR_SUCCESS;
}