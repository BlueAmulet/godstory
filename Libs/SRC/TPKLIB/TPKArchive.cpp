#include "TPKLib.h"
#include "TPKCommon.h"
#include "crc32.h"
#include <string>
#include <hash_map>

//-----------------------------------------------------------------------------
// 嵌制HashTable或BlockTable的大小
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

	// 确定HashTableSize只能是2的n次幂
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
// 为TPKArchive对象设定HashTable和BlockTable的相关数据
static int RelocateTPKTablePositions(TPKArchive* ha)
{
    TPKHeader* pHeader = ha->pHeader;
    LARGE_INTEGER TempSize;
    LARGE_INTEGER FileSize;

    // 获取文件包大小
    FileSize.LowPart = GetFileSize(ha->hFile, (LPDWORD)&(FileSize.HighPart));

    // 设置HashTable的位置
    ha->HashTablePos.HighPart = pHeader->dwHashTablePosHigh;
    ha->HashTablePos.LowPart = pHeader->dwHashTablePos;
    if(ha->HashTablePos.QuadPart > FileSize.QuadPart)
        return ERROR_BAD_FORMAT;

    // 设置BlockTable的位置
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
// 打开或创建一个新的文件包
// 参数说明：
//   szTPKName				文件包文件名
//   dwCreationDisposition	文件操作方式
//   值				文件存在				文件不存在
//   ----------     ---------------------  ---------------------
//   CREATE_NEW     失败					创建新文件包
//   CREATE_ALWAYS  覆盖已存在的			创建新文件包
//   OPEN_EXISTING  打开已存在的			失败
//   OPEN_ALWAYS    打开已存在的			创建新文件包
//   
//   dwHashTableSize - HashTable的大小（仅在创建新文件包时有用，而且必须是16-262144之间的2的倍数值）
//
//   phTPK - 处理完获得的文件包对象
//
BOOL WINAPI TPKCreateArchive(const char* szTPKName, DWORD dwCreationDisposition, DWORD dwHashTableSize, HANDLE* phTPK)
{
    TPKArchive* ha		= NULL;					// 文件包对象
    HANDLE hFile		= INVALID_HANDLE_VALUE;	// 文件包本地文件句柄
    DWORD dwTransferred = 0;					// 写入到文件包的字节数
    BOOL bFileExists	= FALSE;				// 文件包本地文件是否存在
    int nError			= ERROR_SUCCESS;

    // 预处理文件包对象指针
    if(phTPK != NULL)
        *phTPK = NULL;

    // 检查所有输入参数是否有效
    if(szTPKName == NULL || *szTPKName == 0 || phTPK == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    // 获取文件是否已经存在
    bFileExists = (GetFileAttributes(szTPKName) != 0xFFFFFFFF);
    dwCreationDisposition &= 0x0000FFFF;
	// 判断文件包本地文件存在与不存在的情况
    if(bFileExists)
	{
		if(dwCreationDisposition == OPEN_EXISTING)
		{
			// 尝试打开文件包，如果失败，则说明不是TPK文件包
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

    // 嵌制HashTable的大小在HASH_TABLE_SIZE_MIN和HASH_TABLE_SIZE_MAX之间
	dwHashTableSize = ClampTableSize(dwHashTableSize);

    // 确定初始化过哈希算法BUFFER
    if(nError == ERROR_SUCCESS)
        nError = PrepareCryptBuffer();

    // 执行到这一步，只能是创建文件包的本地句柄了
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

    // 为文件包对象分配内存
    if(nError == ERROR_SUCCESS)
    {
        if((ha = ALLOCMEM(TPKArchive, 1)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // 初始化文件包对象，并分配HashTable、BlockTable等相关对象内存
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

    // 填充TPKHeader数据和其它一些数据
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

        // 为HashTable和BlockTable初始化数据
        memset(ha->pBlockTable, 0, sizeof(TPKBlock) * dwHashTableSize);
        memset(ha->pHashTable, 0xFF, sizeof(TPKHash) * dwHashTableSize);
    }

    // 写入TPKHeader头数据到文件包本地文件
    if(nError == ERROR_SUCCESS)
    {
        WriteFile(ha->hFile, ha->pHeader, sizeof(TPKHeader), &dwTransferred, NULL);   
        if(dwTransferred != sizeof(TPKHeader))
            nError = ERROR_DISK_FULL;

        ha->TPKSize.QuadPart += dwTransferred;
    }

    //确保tpk文件里有基本的信息
    RelocateTPKTablePositions(ha);
    SaveTPKTables(ha);

    // 创建文件列表文件对象
    if(nError == ERROR_SUCCESS)
        nError = TPKListFileCreateList(ha);

    // 尝试增加缺省的文件列表文件，并且也把它加入到搜索列表里
    if(nError == ERROR_SUCCESS)
    {
        if(TPKAddListFile((HANDLE)ha, NULL) != ERROR_SUCCESS)
            AddInternalFile(ha, TPK_LISTFILE_NAME);
    }

    // 清除结束，如果有错误，清除所有内存对象
    if(nError != ERROR_SUCCESS)
    {
        FreeTPKArchive(ha);
        if(hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
        SetLastError(nError);
        ha = NULL;
    }
    
    // 返回处理结果，提交文件包对象
    *phTPK = (HANDLE)ha;
    return nError == ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// 打开文件包方法
// szTPKName		文件包文件名
// dwFlags			文件包操作标识(如:TPK_FLAG_NOLISTFILE或TPK_FLAG_PROTECTED)
// phTPK			文件包对象
// dwAccessMode		读或写操作标识
BOOL WINAPI TPKOpenArchive(const char* szTPKName, DWORD dwFlags, HANDLE* phTPK, DWORD dwAccessMode)
{
    TPKArchive* ha	= NULL;
    HANDLE hFile	= INVALID_HANDLE_VALUE;    
    DWORD dwBytes	= 0;
    int nError		= ERROR_SUCCESS;
	LARGE_INTEGER TempPos;
	DWORD dwTransferred;   

    // 检查参数是否正确
    if(nError == ERROR_SUCCESS)
    {
        if(szTPKName == NULL || *szTPKName == 0 || phTPK == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }

    //去掉文件的只读属性
    if(nError == ERROR_SUCCESS)
    {
        DWORD fileAttri = GetFileAttributes(szTPKName);

        if ((fileAttri & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
        {
            fileAttri &= ~FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(szTPKName,fileAttri);
        }
    }

    // 检查哈希算法Buffer是否初始化过
    if(nError == ERROR_SUCCESS)
        nError = PrepareCryptBuffer();

    // 打开文件包
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
    
	// 分配文件包对象内存
    if(nError == ERROR_SUCCESS)
    {
        if((ha = ALLOCMEM(TPKArchive, 1)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // 初始化文件包对象数据
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
		// 首先检查读取是否成功
		if(dwTransferred != sizeof(TPKHeader) || ha->pHeader->dwID != TPK_FILEID || ha->pHeader->dwVer != TPK_VERSION)
			nError = ERROR_BAD_FORMAT;
	}

    // 重置文件包HashTable和BlockTable的一些数据
    if(nError == ERROR_SUCCESS)
    {
        nError = RelocateTPKTablePositions(ha);
    }

    // 为HashTable和BlockTable分配内存
    if(nError == ERROR_SUCCESS)
    {
        ha->pHashTable     = ALLOCMEM(TPKHash,	ha->pHeader->dwHashTableSize);
        ha->pBlockTable    = ALLOCMEM(TPKBlock, ha->pHeader->dwBlockTableSize);
        ha->pbBlockBuffer  = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE);
        if(!ha->pHashTable || !ha->pBlockTable || !ha->pbBlockBuffer)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // 读取HashTable数据
    if(nError == ERROR_SUCCESS)
    {
		dwBytes = ha->pHeader->dwHashTableSize * sizeof(TPKHash);
		memset(ha->pHashTable, 0, dwBytes);        
        SetFilePointer(ha->hFile, ha->HashTablePos.LowPart, &ha->HashTablePos.HighPart, FILE_BEGIN);
        ReadFile(ha->hFile, ha->pHashTable, dwBytes, &dwTransferred, NULL);
        if(dwTransferred != dwBytes)
            nError = ERROR_FILE_CORRUPT;
    }

    // 解密HashTable数据，并检查它的正确性
    if(nError == ERROR_SUCCESS)
    {
        DecryptHashTable((DWORD*)ha->pHashTable, (BYTE*)TPK_HASHTABLE_KEY, dwBytes >> 2);		
		if(ha->dwFlags & TPK_FLAG_PROTECTED)
		{
			//这里可以考虑对解密HashTable后的数据进行检查
		}
    }

    // 读取BlockTable数据
    if(nError == ERROR_SUCCESS)
    {
		dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TPKBlock);
        memset(ha->pBlockTable, 0, dwBytes);		
        SetFilePointer(ha->hFile, ha->BlockTablePos.LowPart, &ha->BlockTablePos.HighPart, FILE_BEGIN);
        ReadFile(ha->hFile, ha->pBlockTable, dwBytes, &dwTransferred, NULL);
		if(dwTransferred != dwBytes)
			nError = ERROR_FILE_CORRUPT;
    }

    // 解密BlockTable数据，并检查它的正确性
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

    // 如果未特别指定TPK_FLAG_NOLISTFILE，则创建内部缺省的文件列表
    if(nError == ERROR_SUCCESS)
    {
        if((dwFlags & TPK_FLAG_NOLISTFILE) == 0)
        {
            if(nError == ERROR_SUCCESS)
                nError = TPKListFileCreateList(ha);

            // 添加文件列表
            if(nError == ERROR_SUCCESS)
                TPKAddListFile((HANDLE)ha, NULL);
        }
    }

    // 清除并退出
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
// 回写文件包脏数据到本地硬盘
BOOL WINAPI TPKFlushArchive(HANDLE hTPK)
{
    TPKArchive* ha = (TPKArchive*)hTPK;    
    // 校验是否是一个TPK文件包对象
    if(!IsValidTPKHandle(ha))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    // 如果文件包数据有更新，则写回硬盘
    if(ha->dwFlags & TPK_FLAG_CHANGED)
    {
        TPKListFileSaveToTPK(ha);
        SaveTPKTables(ha);
        ha->dwFlags &= ~TPK_FLAG_CHANGED;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// 关闭文件包
BOOL WINAPI TPKCloseArchive(HANDLE hTPK)
{
    TPKArchive * ha = (TPKArchive *)hTPK;
    
    // 回写更改的数据到本地硬盘
    if(!TPKFlushArchive(hTPK))
        return FALSE;

    // 释放文件包的所有对象内存
    FreeTPKArchive(ha);
    return TRUE;
}

//-----------------------------------------------------------------------------
// 释放TPK文件包对象
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
// 保存文件包对象的TPKHeader、HashTable和BlockTable对象数据（添加某文件到文件
// 包之前首先扩张文件包大小保存这些数据，以免断电时，这些数据被此文件的内容覆盖
// 而导致文件包不可读）
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

	// 读取HashTable数据
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

	// 读取BlockTable数据
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

	// 存储HashTable数据
	if(nError == ERROR_SUCCESS)
	{
		// 设置HashTable的物理位置
		SetFilePointer(ha->hFile, dwHashTablePos.LowPart, &dwHashTablePos.HighPart, FILE_BEGIN);
		WriteFile(ha->hFile, tmpHashTable, dwHashTableBytes, &dwTransferred, NULL);
		if(dwTransferred != dwHashTableBytes)
			nError = ERROR_DISK_FULL;
	}

	// 存储BlockTable数据
	if(nError == ERROR_SUCCESS)
	{
		// 设置BlockTable的物理位置
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
		// 设置文件包头操作的物理位置
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
// 保存文件包对象的TPKHeader、HashTable和BlockTable对象数据
int SaveTPKTables(TPKArchive* ha)
{
	BYTE* pbBuffer	= NULL;		// 临时用于加密的数据缓冲区
	int   nError	= ERROR_SUCCESS;
	DWORD dwBytes;
	DWORD dwWritten;

	// 为加密Buffer临时分配内存
	if(nError == ERROR_SUCCESS)
	{
		pbBuffer = ALLOCMEM(BYTE, sizeof(TPKBlock) * ha->pHeader->dwBlockTableSize);
		if(pbBuffer == NULL)
			nError = ERROR_NOT_ENOUGH_MEMORY;
	}

	// 存储文件头数据
	if(nError == ERROR_SUCCESS)
	{
		// 设置文件包头操作的物理位置
		SetFilePointer(ha->hFile, 0, NULL, FILE_BEGIN);
		WriteFile(ha->hFile, ha->pHeader, sizeof(TPKHeader), &dwWritten, NULL);
		if(dwWritten != sizeof(TPKHeader))
			nError = ERROR_DISK_FULL;
	}

	// 存储HashTable数据
	if(nError == ERROR_SUCCESS)
	{
		// 复制HashTable数据到临时Buffer
		dwBytes = ha->pHeader->dwHashTableSize * sizeof(TPKHash);
		memcpy(pbBuffer, ha->pHashTable, dwBytes);

		// 加密HashTable数据
		EncryptHashTable((DWORD*)pbBuffer, (BYTE*)TPK_HASHTABLE_KEY, dwBytes >> 2);

		// 设置HashTable的物理位置
		SetFilePointer(ha->hFile, ha->HashTablePos.LowPart, (PLONG)&ha->HashTablePos.HighPart, FILE_BEGIN);
		WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
		if(dwWritten != dwBytes)
			nError = ERROR_DISK_FULL;
	}

	// 存储BlockTable数据
	if(nError == ERROR_SUCCESS)
	{
		// 复制BlockTable数据到临时Buffer
		dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TPKBlock);
		memcpy(pbBuffer, ha->pBlockTable, dwBytes);

		// 加密BlockTable数据
		EncryptBlockTable((DWORD*)pbBuffer, (BYTE*)TPK_BLOCKTABLE_KEY, dwBytes >> 2);

		// 设置HashTable的物理位置
		SetFilePointer(ha->hFile, ha->BlockTablePos.LowPart, (PLONG)&ha->BlockTablePos.HighPart, FILE_BEGIN);
		WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
		if(dwWritten != dwBytes)
			nError = ERROR_DISK_FULL;
	}

	if(nError == ERROR_SUCCESS)
	{
		SetEndOfFile(ha->hFile);
	}

	// 清除并退出
	if(pbBuffer != NULL)
		FREEMEM(pbBuffer);
	return nError;
}

//-----------------------------------------------------------------------------
// 扩展BlockTable
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
// 将一个本地文件添加到文件包中
int AddFileToArchive(TPKArchive* ha,			// 文件包对象
					 HANDLE hFile,				// 待添加的本地文件句柄
					 const char* szFileName,	// 待添加的本地文件名
					 DWORD dwFlags,				// 文件操作标识（见TPK_FILEOP枚举）
					 BOOL* pbReplaced			// 是否有替换
					 )
{
    LARGE_INTEGER TempPos;					// 用于临时存储偏移地址的计算结果
    TPKBlock* pBlockEnd = NULL;				// BlockTable的尾部项
    TPKFile*  hf = NULL;					// 新添加的文件对象
    BYTE* pbCompressed = NULL;				// 用于处理压缩的数据缓冲区
    BYTE* pbToWrite = NULL;					// 准备写入磁盘的数据缓冲区
    DWORD dwBlockPosLen = 0;				// Block位置数据块长度
    DWORD dwTransferred = 0;				// 读写操作的实际大小
    DWORD dwFileSizeHigh = 0;				// 文件大小的高位字节（支持4G以上）
    DWORD dwFileSize = 0;					// 文件大小的低位字节
    BOOL bReplaced = FALSE;					// 是否替换文件包里的文件
    int nError = ERROR_SUCCESS;

	// 检查输入参数是否正确
	if(ha == NULL || hFile == INVALID_HANDLE_VALUE || szFileName == NULL || *szFileName == 0)
	{
		nError = ERROR_INVALID_PARAMETER;
	}

    // 获得并处理文件大小
    if(nError == ERROR_SUCCESS)
    {
		pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
        dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

        // 如果文件太小，不必加密
        if(dwFileSize < 0x04)
            dwFlags &= ~(TPK_FILE_ENCRYPTED | TPK_FILE_FIXSEED);
		// 如果文件较小，不必压缩
        if(dwFileSize < 0x20)
            dwFlags &= ~TPK_FILE_COMPRESSED;

        // 单个文件不能超过4GB字节
        if(dwFileSizeHigh != 0)
            nError = ERROR_PARAMETER_QUOTA_EXCEEDED;
    }

	// 这里考虑“断电保护”，按目前添加文件的大小在文件包尾挪动相同
	// 字节后写入TPKHeader数据到本地文件包。也就是说即便是
	// 在后续文件内容写入操作时突然断电，也不会丢失TPKHeader数据
	if(nError == ERROR_SUCCESS && (dwFlags & TPK_FILE_BREAKPROTECTED))
	{
		LARGE_INTEGER dwNewHashTablePos;
		dwNewHashTablePos.LowPart = ha->pHeader->dwHashTablePos;
		dwNewHashTablePos.HighPart = ha->pHeader->dwHashTablePosHigh;
		dwNewHashTablePos.QuadPart += dwFileSize;
		nError = SaveTPKTablesForAddFile(ha, dwNewHashTablePos);
	}

    // 为TPKFile对象分配内存
    if(nError == ERROR_SUCCESS)
    {
        hf = (TPKFile*)ALLOCMEM(BYTE, sizeof(TPKFile) + strlen(szFileName));
        if(hf == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // 初始化TPKFile对象数据
    if(nError == ERROR_SUCCESS)
    {
        memset(hf, 0, sizeof(TPKFile));
        strcpy(hf->szFileName, szFileName);
        hf->hFile = INVALID_HANDLE_VALUE;
        hf->ha = ha;

        // 检查是否有同名文件已经存在
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

		// 如果确定是新文件，则从HashTable里找出一个空闲Hash块
        if(nError == ERROR_SUCCESS && hf->pHash == NULL)
        {
            hf->pHash = FindFreeHashEntry(ha, szFileName);			
            if(hf->pHash == NULL)
			{
				// Note:尝试扩大HashTable，引发连锁操作（重新分配HashTable、
				//重新索引Hash项和重新加解密数据等等）太多，得不偿失。
				nError = ERROR_HANDLE_DISK_FULL;				
			}
		}        
    }

    // 从BlockTable中搜索一个空闲的Block项给这个文件
    if(nError == ERROR_SUCCESS)
    {
		// 记下在HashTable中的索引位置
		hf->dwHashIndex = (DWORD)(hf->pHash - ha->pHashTable);
        // 找出第一个文件数据内容的
        hf->TPKFilePos.QuadPart = sizeof(TPKHeader);

        // 搜索空闲的Block项，顺便计算出能存储当前文件的位置
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
                //如果是replace,则block不变，由上面重新计算一个新的block用来填充数据
				if(hf->pBlock == NULL)
					hf->pBlock = pBlock;
            }
        }

		// 如果有Hash项，却找不出一个空闲的Block项可用，则退出
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

    // 如果文件需要加密，则获得密钥
    if(nError == ERROR_SUCCESS && (dwFlags & TPK_FILE_ENCRYPTED))
    {
        const char* szTemp = strrchr(szFileName, '\\');
        if(szTemp != NULL)
            szFileName = szTemp + 1;

        hf->dwSeed1 = DecryptFileSeed(szFileName);
        if(dwFlags & TPK_FILE_FIXSEED)
            hf->dwSeed1 = (hf->dwSeed1 + hf->TPKFilePos.LowPart) ^ dwFileSize;
    }

    // 为压缩数据分配临时缓冲区
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

    // 为压缩文件，分配压缩块位置缓冲区（存储每个压缩后数据块的大小）
    if(nError == ERROR_SUCCESS && (dwFlags & TPK_FILE_COMPRESSED))
    {
        hf->pdwBlockPos = ALLOCMEM(DWORD, hf->nBlocks + 1);
        pbCompressed = ALLOCMEM(BYTE, TPK_DATABLOCK_SIZE * 2);
        if(hf->pdwBlockPos == NULL || pbCompressed == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = pbCompressed;
    }

    // 设置文件操作位置，并设置Hash项和Block项数据
    if(nError == ERROR_SUCCESS)
    {
        // 设置到文件数据起始位置
        SetFilePointer(ha->hFile, hf->TPKFilePos.LowPart, &hf->TPKFilePos.HighPart, FILE_BEGIN);

        // 设置到Hash项的BlockTable的索引值
        hf->pHash->dwBlockIndex = hf->dwBlockIndex;

        // 设置Block项数据
        hf->pBlock->dwFilePosHigh = (USHORT)hf->TPKFilePos.HighPart;
        hf->pBlock->dwFilePos = hf->TPKFilePos.LowPart;
        hf->pBlock->dwFSize   = dwFileSize;
        hf->pBlock->dwCSize   = 0;
        hf->pBlock->dwFlags   = dwFlags | TPK_FILE_EXISTS;
    }

    // 如果有需要压缩，要设置数据块的位置数据
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

    // 写入所有的数据块
    if(nError == ERROR_SUCCESS)
    {
        crc32_context crc32_ctx;
        DWORD nBlock;       

        // 初始化crc32
        CRC32_Init(&crc32_ctx);

        // 移动文件操作指针到起始位置
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        for(nBlock = 0; nBlock < hf->nBlocks-1; nBlock++)
        {
            DWORD dwInLength  = TPK_DATABLOCK_SIZE;
            DWORD dwOutLength = TPK_DATABLOCK_SIZE;

            // 每次读取TPK_DATABLOCK_SIZE大小的数据
            ReadFile(hFile, hf->pbFileBuffer, TPK_DATABLOCK_SIZE, &dwInLength, NULL);
            if(dwInLength == 0)
                break;

            // 计算CRC32值
            CRC32_Update(&crc32_ctx, hf->pbFileBuffer, dwInLength);

            // 如果需要压缩，则处理压缩数据
            dwOutLength = dwInLength;
            if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
            {
                int nOutLength = TPK_DATABLOCK_SIZE * 2;
				Compress_zlib((char*)pbCompressed, &nOutLength,(char*)hf->pbFileBuffer,dwInLength);
                // 如果压缩失败或压缩后数据如果比源数据还要大，就直接拷贝不压缩
                if(nOutLength >= (int)dwInLength)
                {
                    memcpy(pbCompressed, hf->pbFileBuffer, dwInLength);
                    nOutLength = dwInLength;
                }

                // 更新数据块位置记录
                dwOutLength = nOutLength;
                hf->pdwBlockPos[nBlock+1] = hf->pdwBlockPos[nBlock] + dwOutLength;
            }

            // 如果需要加密，则处理加密数据
            if(hf->pBlock->dwFlags & TPK_FILE_ENCRYPTED)
            {
                EncrypTPKBlock((DWORD*)pbToWrite, dwOutLength, hf->dwSeed1 + nBlock);
            }
            
            // 写文件数据块到本地磁盘
            WriteFile(ha->hFile, pbToWrite, dwOutLength, &dwTransferred, NULL);
            if(dwTransferred != dwOutLength)
            {
                nError = ERROR_DISK_FULL;
                break;
            }

            // 更新Block项中文件的压缩字节大小
            hf->pBlock->dwCSize += dwTransferred;
        }

        // 计算CRC32值写入到Block项的文件Crc32字段上
        CRC32_Finish(&crc32_ctx, (unsigned long*)&hf->pBlock->dwCrc32);
    }

    // 重新存储数据块的数据位置
    if(nError == ERROR_SUCCESS && (hf->pBlock->dwFlags & TPK_FILE_COMPRESSED))
    {
        // 如果数据块内容被加密，那么位置块也需要加密
        if(dwFlags & TPK_FILE_ENCRYPTED)
            EncrypTPKBlock(hf->pdwBlockPos, dwBlockPosLen, hf->dwSeed1 - 1);
        
        // 重新设置到文件起始位置
        SetFilePointer(ha->hFile, hf->TPKFilePos.LowPart, &hf->TPKFilePos.HighPart, FILE_BEGIN);
        
        // 重写位置数据块
        WriteFile(ha->hFile, hf->pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
        if(dwTransferred != dwBlockPosLen)
            nError = ERROR_DISK_FULL;
    }


    // 如果成功，需要通知保存文件包时作一些改变操作
    if(nError == ERROR_SUCCESS)
	{
		DWORD dwTableSize;

		ha->pLastFile  = NULL;
		ha->dwBlockPos = 0;
		ha->dwBuffPos  = 0;

		// 设置HashTable在文件包的存储位置
		TempPos.QuadPart = hf->TPKFilePos.QuadPart + hf->pBlock->dwCSize;		
		ha->HashTablePos.QuadPart = TempPos.QuadPart;
		ha->pHeader->dwHashTablePos = TempPos.LowPart;
		ha->pHeader->dwHashTablePosHigh = (USHORT)TempPos.HighPart;
		dwTableSize = ha->pHeader->dwHashTableSize * sizeof(TPKHash);

		// 设置BlockTable在文件包的存储位置
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
		// 清除设置的Hash项或Block项数据
		if(hf != NULL)
		{
			if(hf->pHash != NULL)
				memset(hf->pHash, 0xFF, sizeof(TPKHash));
			if(hf->pBlock != NULL)
				memset(hf->pBlock, 0, sizeof(TPKBlock));
		}
    }

    // 清除并退出
    if(pbCompressed != NULL)
        FREEMEM(pbCompressed);
    if(pbReplaced != NULL)
        *pbReplaced = bReplaced;
    FreeTPKFile(hf);
    return nError;
}

//-----------------------------------------------------------------------------
// 反转右斜杠
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
// 反转左斜杠
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

// 查找(version)文件，获取文件内容
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
// 解包方法（将文件包内所有文件解出到其它对应目标文件包内）
BOOL WINAPI TPKUnPackFileAll(HANDLE hTPK, const char* TargetPath)
{
	DWORD nError = ERROR_SUCCESS;
	typedef stdext::hash_map<std::string, HANDLE> TPKCACHE;
	TPKCACHE cache;
	TempAlloc<char> szPackFileName((DWORD)strlen(TargetPath) + MAX_PATH * 3);

	// 获取文件包内的更新文件列表
	std::vector<stLine*> arrLine;	
	if(FindVersionFile(hTPK, arrLine) == FALSE)
		nError = ERROR_FILE_NOT_FOUND;
    
	if(nError == ERROR_SUCCESS)
	{
		HANDLE hFileFrom = NULL;
		for(size_t i = 0; i < arrLine.size(); i++)
		{
			stLine* line = arrLine[i];
			// 判断是否需要解包到文件包内
			if(line->TPKFileName[0])
			{
				if(!TPKOpenFile(hTPK, line->FromFileName, 0, &hFileFrom))
				{
					nError = ERROR_FILE_NOT_FOUND;
					break;
				}

				HANDLE hTPKTo = NULL;
				// 获取目标文件包的句柄，杜绝频繁打开文件包，以Cache方式保存句柄
				{
					sprintf(szPackFileName, "%s\\%s", TargetPath, line->TPKFileName);
					std::string strPackFileName = szPackFileName;
					TPKCACHE::iterator itFind = cache.find(strPackFileName);
					if(itFind == cache.end())
					{
						//清除不可写的文件属性
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
				
				// 从源TPK文件包里复制文件数据到目标文件包中
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
				// 递归目标路径，便于解出文件到正确的路径下
				EnsureDirectory(szPackFileName);
				//清除不可写的文件属性
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

	// 清除目标TPK文件包Cache列表
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