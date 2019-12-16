#include "TPKLib.h"
#include "TPKCommon.h"

//-----------------------------------------------------------------------------
// 读取文件数据块
//  参数解释：
//  hf          - TPK文件对象
//  dwBlockPos  - 数据块位置（相对文件开始位置）
//  buffer      - 目标数据
//  dwBlockSize - 待读取的字节数（必须是TPK_DATABLOCK_SIZE的倍数）
//  返回值： 实际读取的字节数
static DWORD WINAPI ReadTPKBlocks(TPKFile* hf, DWORD dwBlockPos, BYTE* buffer, DWORD blockBytes)
{
    LARGE_INTEGER FilePos;
    TPKArchive* ha		= hf->ha;		// 文件包对象句柄
    BYTE* tempBuffer	= NULL;         // 用于读取压缩数据的临时BUFFER
    DWORD dwFilePos		= dwBlockPos;   // 读取文件的位置
    DWORD dwToRead;						// 读取数据的字节大小
    DWORD blockNum;						// 数据块索引编号 (用于解密)
    DWORD dwBytesRead	= 0;            // 实际读取的字节总数
    DWORD bytesRemain	= 0;            // 从dwBlockPos算到文件尾部的剩余字节数
    DWORD nBlocks;						// 需要读取的数据块数

    // 检查参数是否正确（Block的位置必须对齐，并且待读取的字节不能为0)
    if((dwBlockPos & (TPK_DATABLOCK_SIZE - 1)) || blockBytes == 0)
        return 0;

    // 嵌制越界读取数据
    if((dwBlockPos + blockBytes) > hf->pBlock->dwFSize)
        blockBytes = hf->pBlock->dwFSize - dwBlockPos;

    bytesRemain = hf->pBlock->dwFSize - dwBlockPos;
    blockNum    = dwBlockPos / TPK_DATABLOCK_SIZE;
    nBlocks     = blockBytes / TPK_DATABLOCK_SIZE;
    if(blockBytes % TPK_DATABLOCK_SIZE)
        nBlocks++;

    // 如果数据块压缩过，需要判断位置块数据的加载
    if((hf->pBlock->dwFlags & TPK_FILE_COMPRESSED) && hf->bBlockPosLoaded == FALSE)
    {
        SetFilePointer(ha->hFile, hf->TPKFilePos.LowPart, &hf->TPKFilePos.HighPart, FILE_BEGIN);
		// 计算需要读取位置块数据的大小
        dwToRead = (hf->nBlocks+1) * sizeof(DWORD);
        ReadFile(ha->hFile, hf->pdwBlockPos, dwToRead, &dwBytesRead, NULL);
		// 如果第一个位置块数据记录不是整个位置块的长度，就可能被加密过
        if(hf->pdwBlockPos[0] != dwBytesRead)
            hf->pBlock->dwFlags |= TPK_FILE_ENCRYPTED;

        // 如果被加密过，则处理它
        if(hf->pBlock->dwFlags & TPK_FILE_ENCRYPTED)
        {
            // 如果还不知道密钥，则获取它
            if(hf->dwSeed1 == 0)
                hf->dwSeed1 = DetectFileSeed(hf->pdwBlockPos, dwBytesRead);

            // 如果仍然还不知道密钥，只有退出
            if(hf->dwSeed1 == 0)
                return 0;

            // 解密位置数据块
            DecrypTPKBlock(hf->pdwBlockPos, dwBytesRead, hf->dwSeed1 - 1);
			// 如果位置数据块数据长度不正确，直接退出
            if((hf->pdwBlockPos[1] - hf->pdwBlockPos[0]) > TPK_DATABLOCK_SIZE)
				return 0;
        }

        // 确认位置数据块已经载入
        hf->bBlockPosLoaded = TRUE;
    }

    // 设置准备读取文件内容的位置和大小
    dwFilePos = dwBlockPos;
    dwToRead  = blockBytes;
    if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
    {
        dwFilePos = hf->pdwBlockPos[blockNum];
        dwToRead  = hf->pdwBlockPos[blockNum + nBlocks] - dwFilePos;
    }
	// 计算实际文件位置
    FilePos.QuadPart = hf->TPKFilePos.QuadPart + dwFilePos;

    // 准备一个临时BUFFER用于读取数据（如果未压缩，则直接使用输出BUFFER)
    tempBuffer = buffer;
    if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
    {
        if((tempBuffer = ALLOCMEM(BYTE, dwToRead)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return 0;
        }
    }

    // 设置文件位置，一次性读取所有文件数据块内容
    SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
    ReadFile(ha->hFile, tempBuffer, dwToRead, &dwBytesRead, NULL);

    // 再接着处理临时BUFFER中的数据块内容
    DWORD blockStart = 0;               // Index of block start in work buffer
    DWORD blockSize  = min(blockBytes, TPK_DATABLOCK_SIZE);
    DWORD index      = blockNum;        // Current block index

    dwBytesRead = 0;                      // Clear read byte counter

    for(DWORD i = 0; i < nBlocks; i++, index++)
    {
        BYTE * inputBuffer = tempBuffer + blockStart;
        int    outLength = TPK_DATABLOCK_SIZE;

        if(bytesRemain < (DWORD)outLength)
            outLength = bytesRemain;

        // 如果有压缩，则从位置数据块里获取数据块的长度
        if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
            blockSize = hf->pdwBlockPos[index+1] - hf->pdwBlockPos[index];

        // 如果数据块被加密过，则需要解密
        if(hf->pBlock->dwFlags & TPK_FILE_ENCRYPTED)
        {
            DecrypTPKBlock((DWORD *)inputBuffer, blockSize, hf->dwSeed1 + index);
        }

        // 判定数据块是否确实压缩过
        if(blockSize < (DWORD)outLength)
        {
            if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
                Decompress_zlib((char *)buffer, &outLength, (char *)inputBuffer, (int)blockSize);
            dwBytesRead += outLength;
            buffer    += outLength;
        }
        else
        {
            if(buffer != inputBuffer)
                memcpy(buffer, inputBuffer, blockSize);

            dwBytesRead += blockSize;
            buffer    += blockSize;
        }
        blockStart  += blockSize;
        bytesRemain -= outLength;
    }

    // 如果有处理压缩，需要删除临时BUFFER
    if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
        FREEMEM(tempBuffer);

    return dwBytesRead;
}

//-----------------------------------------------------------------------------
// 文件数据作为单一整块数据读取
// （注：此方法被调用时，参数一定保证正确性，例如:dwToRead + dwFilePos不能大于文件大小）
static DWORD WINAPI ReadTPKFileSingleUnit(TPKFile * hf, DWORD dwFilePos, BYTE * pbBuffer, DWORD dwToRead)
{
    TPKArchive * ha = hf->ha; 
    DWORD dwBytesRead = 0;

    // 如果这个文件确实被压缩过，先解压缩
    if(hf->pBlock->dwCSize < hf->pBlock->dwFSize)
    {
        if(hf->pbFileBuffer == NULL)
        {
            BYTE * inputBuffer = NULL;
            int outputBufferSize = (int)hf->pBlock->dwFSize;
            int inputBufferSize = (int)hf->pBlock->dwCSize;

            hf->pbFileBuffer = ALLOCMEM(BYTE, outputBufferSize);
            inputBuffer = ALLOCMEM(BYTE, inputBufferSize);
            if(inputBuffer != NULL && hf->pbFileBuffer != NULL)
            {
                // 移动文件指针到文件开始处
                SetFilePointer(ha->hFile, hf->TPKFilePos.LowPart, &hf->TPKFilePos.HighPart, FILE_BEGIN);

                // 读取压缩数据
                ReadFile(ha->hFile, inputBuffer, inputBufferSize, &dwBytesRead, NULL);

                if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
                    Decompress_zlib((char *)hf->pbFileBuffer, &outputBufferSize, (char *)inputBuffer, (int)inputBufferSize);
            }

            // 释放临时BUFFER
            if(inputBuffer != NULL)
                FREEMEM(inputBuffer);
        }

        if(hf->pbFileBuffer != NULL)
        {
            memcpy(pbBuffer, hf->pbFileBuffer + dwFilePos, dwToRead);
            dwBytesRead = dwToRead;
        }
    }
    else
    {
        LARGE_INTEGER RawFilePos = hf->TPKFilePos;
        RawFilePos.QuadPart += dwFilePos;
        SetFilePointer(ha->hFile, RawFilePos.LowPart, &RawFilePos.HighPart, FILE_BEGIN); 
        // 读取非压缩的数据
        ReadFile(ha->hFile, pbBuffer, dwToRead, &dwBytesRead, NULL);
    }

    return dwBytesRead;
}


//-----------------------------------------------------------------------------
// 从文件包内读取某文件的数据
static DWORD WINAPI ReadTPKFile(TPKFile* hf, DWORD dwFilePos, BYTE* pbBuffer, DWORD dwToRead)
{
    TPKArchive* ha		= hf->ha; 
    TPKBlock* pBlock	= hf->pBlock;		// 对应当前文件的Block项
    DWORD dwBytesRead	= 0;				// 读出的总字节数
    DWORD dwBlockPos;						// 对齐整块文件内容的位置
    DWORD dwLoaded;

    // 相对文件的偏移位置是否已经超过文件大小？
    if(dwFilePos >= pBlock->dwFSize)
        return dwBytesRead;

    // 嵌制越界读取
    if((pBlock->dwFSize - dwFilePos) < dwToRead)
        dwToRead = (pBlock->dwFSize - dwFilePos);

    // 如果文件被作为独立数据块，则另外处理
    if(pBlock->dwFlags & TPK_FILE_SINGLE_UNIT)
        return ReadTPKFileSingleUnit(hf, dwFilePos, pbBuffer, dwToRead);

    // 准确定位文件的读取位置(因文件都是以固定大小的数据块存储的)
    dwBlockPos = dwFilePos & ~(TPK_DATABLOCK_SIZE - 1);

    // 如果数据块的读取位置未对齐TPK_FILE_BLOCKSIZE，说明第一个数据块非标准位置读取，
	// 则应该先预读到缓冲区再截取处理
    if((dwFilePos % TPK_DATABLOCK_SIZE) != 0)
    {        
        DWORD dwToCopy;	// 在pbBlockBuffer里剩余的字节数
        DWORD dwLoaded = TPK_DATABLOCK_SIZE;

        // 检查上次操作的文件是否也是当前文件，而且已经预载过到cache
        if(hf != ha->pLastFile || dwBlockPos != ha->dwBlockPos)
        {
            // 先预载入第一个数据块
            dwLoaded = ReadTPKBlocks(hf, dwBlockPos, ha->pbBlockBuffer, TPK_DATABLOCK_SIZE);
            if(dwLoaded == 0)
                return (DWORD)-1;

            // 记录这次预读的一些参数以备后来使用
            ha->pLastFile  = hf;
            ha->dwBlockPos = dwBlockPos;
            ha->dwBuffPos  = dwFilePos % TPK_DATABLOCK_SIZE;
        }
        dwToCopy = dwLoaded - ha->dwBuffPos;
        if(dwToCopy > dwToRead)
            dwToCopy = dwToRead;

        // 从block buffer拷贝到目标buffer
        memcpy(pbBuffer, ha->pbBlockBuffer + ha->dwBuffPos, dwToCopy);
    
        // 更新一些读取的操作结果
        dwToRead      -= dwToCopy;
        dwBytesRead   += dwToCopy;
        pbBuffer      += dwToCopy;
        dwBlockPos    += TPK_DATABLOCK_SIZE;
        ha->dwBuffPos += dwToCopy;

        // 如果没有可读的，退出
        if(dwToRead == 0)
            return dwBytesRead;
    }

    // 载入待读数据(dwToRead)的中间对齐TPK_DATABLOCK_SIZE大小的一大整块数据
    if(dwToRead > TPK_DATABLOCK_SIZE)
    {                                           
        DWORD dwBlockBytes = dwToRead & ~(TPK_DATABLOCK_SIZE - 1);

        dwLoaded = ReadTPKBlocks(hf, dwBlockPos, pbBuffer, dwBlockBytes);
        if(dwLoaded == 0)
            return (DWORD)-1;

        // 更新一些读取的操作结果
        dwToRead    -= dwLoaded;
        dwBytesRead += dwLoaded;
        pbBuffer    += dwLoaded;
        dwBlockPos  += dwLoaded;

        // 如果没有可读的，退出
        if(dwToRead == 0)
            return dwBytesRead;
    }

    // 载入待读数据(dwToRead)未对齐的尾部字节数据
    if(dwToRead > 0)
    {
        DWORD dwToCopy = TPK_DATABLOCK_SIZE;

        // 检查上次操作的文件是否也是当前文件，而且已经预载过到cache
        if(hf != ha->pLastFile || dwBlockPos != ha->dwBlockPos)
        {
            // 先以TPK_DATABLOCK_SIZE大小读取数据
            dwToCopy = ReadTPKBlocks(hf, dwBlockPos, ha->pbBlockBuffer, TPK_DATABLOCK_SIZE);
            if(dwToCopy == 0)
                return (DWORD)-1;

            // 记录这次预读的一些参数以备后来使用
            ha->pLastFile  = hf;
            ha->dwBlockPos = dwBlockPos;
        }
        ha->dwBuffPos  = 0;

        if(dwToCopy > dwToRead)
            dwToCopy = dwToRead;

        memcpy(pbBuffer, ha->pbBlockBuffer, dwToCopy);
        dwBytesRead  += dwToCopy;
        ha->dwBuffPos = dwToCopy;
    }
    
    return dwBytesRead;
}

//-----------------------------------------------------------------------------
// 读取文件内容
BOOL WINAPI TPKReadFile(HANDLE hFile, VOID* lpBuffer, DWORD dwToRead, DWORD* pdwRead, LPOVERLAPPED lpOverlapped)
{
    TPKFile* hf = (TPKFile*)hFile;
    DWORD dwBytes = 0;                  // Number of bytes (for everything)
    int nError = ERROR_SUCCESS;

    if(pdwRead != NULL)
        *pdwRead = 0;

    // 检查参数是否有效
    if(nError == ERROR_SUCCESS)
    {
        if(hf == NULL || lpBuffer == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }

    // 如果操作的文件句柄已经存在，则直接使用WIN32API读取（本地文件读取）
    if(nError == ERROR_SUCCESS && hf->hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwTransferred;
        ReadFile(hf->hFile, lpBuffer, dwToRead, &dwTransferred, lpOverlapped);
        if(dwTransferred < dwToRead)
        {
            SetLastError(ERROR_HANDLE_EOF);
            return FALSE;
        }
        
        if(pdwRead != NULL)
            *pdwRead = dwTransferred;
        return TRUE;
    }

    // 文件包内读取文件
    if(nError == ERROR_SUCCESS)
    {
        if(dwToRead > 0)
        {
            dwBytes = ReadTPKFile(hf, hf->dwFilePos, (BYTE*)lpBuffer, dwToRead);
            if(dwBytes == (DWORD)-1)
            {
                SetLastError(ERROR_CAN_NOT_COMPLETE);
                return FALSE;
            }
            hf->ha->pLastFile = hf;
            hf->dwFilePos += dwBytes;
        }
        if(pdwRead != NULL)
            *pdwRead = dwBytes;
    }

    // 检查读取的字节总数
    if(dwBytes < dwToRead)
    {
        SetLastError(ERROR_HANDLE_EOF);
        return FALSE;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// 获取文件位置
DWORD WINAPI TPKGetFilePos(HANDLE hFile, DWORD* pdwFilePosHigh)
{
    TPKFile* hf = (TPKFile*)hFile;    
    if(pdwFilePosHigh != NULL)
        *pdwFilePosHigh = 0;

    if(hf == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (DWORD)-1;
    }

    // 如果打开的是一个本地文件，则直接退出
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return 0;

    // 如果从文件包内打开文件，则返回文件位置
    if(pdwFilePosHigh != NULL)
        *pdwFilePosHigh = hf->TPKFilePos.HighPart;
    return hf->TPKFilePos.LowPart;
}

//-----------------------------------------------------------------------------
// 获取文件大小
DWORD WINAPI TPKGetFileSize(HANDLE hFile, DWORD* pdwFileSizeHigh)
{
    TPKFile* hf = (TPKFile*)hFile;    
    if(pdwFileSizeHigh != NULL)
        *pdwFileSizeHigh = 0;

    if(hf == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (DWORD)-1;
    }

    // 如果打开的是一个本地文件，则直接调用WIN32API获得这个文件的大小
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return GetFileSize(hf->hFile, pdwFileSizeHigh);

    // 如果从文件包内打开文件，则返回文件大小
    return hf->pBlock->dwFSize;
}

//-----------------------------------------------------------------------------
// 设置文件操作位置
DWORD WINAPI TPKSetFilePointer(HANDLE hFile, LONG lFilePos, LONG* pdwFilePosHigh, DWORD dwMethod)
{
    TPKArchive* ha;
    TPKFile* hf = (TPKFile*)hFile;
	// 检查输入参数是否正确
    if(hf == NULL || (pdwFilePosHigh != NULL && *pdwFilePosHigh != 0))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (DWORD)-1;
    }

    // 如果打开的是一个本地文件，则使用Win32API
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return SetFilePointer(hf->hFile, lFilePos, pdwFilePosHigh, dwMethod);

    ha = hf->ha;

    switch(dwMethod)
    {
        case FILE_BEGIN:
            // 不能向前越出当前文件内容的起始位置，杜绝越界读取操作
            if(-lFilePos > (LONG)hf->dwFilePos)
                hf->dwFilePos = 0;
            else
                hf->dwFilePos = lFilePos;
            break;

        case FILE_CURRENT:
            // 不能向前越出当前文件内容的起始位置，杜绝越界读取操作
            if(-lFilePos > (LONG)hf->dwFilePos)
                hf->dwFilePos = 0;
            else
                hf->dwFilePos += lFilePos;
            break;

        case FILE_END:
            // 不能向前越出当前文件内容的起始位置，杜绝越界读取操作
            if(-lFilePos >= (LONG)hf->pBlock->dwFSize)
                hf->dwFilePos = 0;
            else
                hf->dwFilePos = hf->pBlock->dwFSize + lFilePos;
            break;

        default:
            return ERROR_INVALID_PARAMETER;
    }

    if(hf == ha->pLastFile && (hf->dwFilePos & ~(TPK_DATABLOCK_SIZE - 1)) == ha->dwBlockPos)
        ha->dwBuffPos = hf->dwFilePos & (TPK_DATABLOCK_SIZE - 1);
    else
    {
        ha->pLastFile = NULL;
        ha->dwBuffPos = 0;
    }

    return hf->dwFilePos;
}

//-----------------------------------------------------------------------------
// 获取文件信息
DWORD_PTR WINAPI TPKGetFileInfo(HANDLE hTPKOrFile, DWORD dwInfoType)
{
    TPKArchive* ha	= (TPKArchive*)hTPKOrFile;
    TPKFile* hf		= (TPKFile*)hTPKOrFile;
    TPKBlock* pBlockEnd;
    TPKBlock* pBlock;   
    DWORD dwSeed;

    switch(dwInfoType)
    {
        case TPK_INFO_HASH_TABLE_SIZE:
            if(IsValidTPKHandle(ha))
                return ha->pHeader->dwHashTableSize;
            break;

        case TPK_INFO_HASH_TABLE:
            if(IsValidTPKHandle(ha))
                return (DWORD_PTR)ha->pHashTable;
            break;

		case TPK_INFO_BLOCK_TABLE_SIZE:
			if(IsValidTPKHandle(ha))
				return ha->pHeader->dwBlockTableSize;
			break;

        case TPK_INFO_BLOCK_TABLE:
            if(IsValidTPKHandle(ha))
                return (DWORD_PTR)ha->pBlockTable;
            break;

        case TPK_INFO_NUM_FILES:
            if(IsValidTPKHandle(ha))
            {
				DWORD dwFileCount = 0;
                pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
                for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
                {
                    if(pBlock->dwFlags & TPK_FILE_EXISTS)
                        dwFileCount++;
                }
                return dwFileCount;
            }
            break;

        case TPK_INFO_HASH_INDEX:
            if(IsValidFileHandle(hf))
                return hf->dwHashIndex;
            break;

        case TPK_INFO_CODENAME1:
            if(IsValidFileHandle(hf))
                return hf->pHash->dwName1;
            break;

        case TPK_INFO_CODENAME2:
            if(IsValidFileHandle(hf))
                return hf->pHash->dwName2;
            break;

        case TPK_INFO_BLOCKINDEX:
            if(IsValidFileHandle(hf))
                return hf->dwBlockIndex;
            break;

        case TPK_INFO_FILE_SIZE:
            if(IsValidFileHandle(hf))
                return hf->pBlock->dwFSize;
            break;

        case TPK_INFO_COMPRESSED_SIZE:
            if(IsValidFileHandle(hf))
                return hf->pBlock->dwCSize;
            break;

        case TPK_INFO_FLAGS:
            if(IsValidFileHandle(hf))
                return hf->pBlock->dwFlags;
            break;

        case TPK_INFO_POSITION:
            if(IsValidFileHandle(hf))
                return hf->pBlock->dwFilePos;
            break;

		case TPK_INFO_CRC32:
			 if(IsValidFileHandle(hf))
                return hf->pBlock->dwCrc32;
            break;

		case TPK_INFO_FILENAME:
			 if(IsValidFileHandle(hf) && *hf->szFileName != 0)
				return (DWORD_PTR)hf->szFileName;
			 break;

        case TPK_INFO_SEED:
            if(IsValidFileHandle(hf))
                return hf->dwSeed1;
            break;

        case TPK_INFO_SEED_UNFIXED:
            if(IsValidFileHandle(hf))
            {
                dwSeed = hf->dwSeed1;
                if(hf->pBlock->dwFlags & TPK_FILE_FIXSEED)
                    dwSeed = (dwSeed ^ hf->pBlock->dwFSize) - hf->TPKFilePos.LowPart;
                return dwSeed;
            }
            break;
    }

    // 未知的参数或无效的文件句柄
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0xFFFFFFFF;
}