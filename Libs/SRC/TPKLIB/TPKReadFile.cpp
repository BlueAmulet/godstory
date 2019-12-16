#include "TPKLib.h"
#include "TPKCommon.h"

//-----------------------------------------------------------------------------
// ��ȡ�ļ����ݿ�
//  �������ͣ�
//  hf          - TPK�ļ�����
//  dwBlockPos  - ���ݿ�λ�ã�����ļ���ʼλ�ã�
//  buffer      - Ŀ������
//  dwBlockSize - ����ȡ���ֽ�����������TPK_DATABLOCK_SIZE�ı�����
//  ����ֵ�� ʵ�ʶ�ȡ���ֽ���
static DWORD WINAPI ReadTPKBlocks(TPKFile* hf, DWORD dwBlockPos, BYTE* buffer, DWORD blockBytes)
{
    LARGE_INTEGER FilePos;
    TPKArchive* ha		= hf->ha;		// �ļ���������
    BYTE* tempBuffer	= NULL;         // ���ڶ�ȡѹ�����ݵ���ʱBUFFER
    DWORD dwFilePos		= dwBlockPos;   // ��ȡ�ļ���λ��
    DWORD dwToRead;						// ��ȡ���ݵ��ֽڴ�С
    DWORD blockNum;						// ���ݿ�������� (���ڽ���)
    DWORD dwBytesRead	= 0;            // ʵ�ʶ�ȡ���ֽ�����
    DWORD bytesRemain	= 0;            // ��dwBlockPos�㵽�ļ�β����ʣ���ֽ���
    DWORD nBlocks;						// ��Ҫ��ȡ�����ݿ���

    // �������Ƿ���ȷ��Block��λ�ñ�����룬���Ҵ���ȡ���ֽڲ���Ϊ0)
    if((dwBlockPos & (TPK_DATABLOCK_SIZE - 1)) || blockBytes == 0)
        return 0;

    // Ƕ��Խ���ȡ����
    if((dwBlockPos + blockBytes) > hf->pBlock->dwFSize)
        blockBytes = hf->pBlock->dwFSize - dwBlockPos;

    bytesRemain = hf->pBlock->dwFSize - dwBlockPos;
    blockNum    = dwBlockPos / TPK_DATABLOCK_SIZE;
    nBlocks     = blockBytes / TPK_DATABLOCK_SIZE;
    if(blockBytes % TPK_DATABLOCK_SIZE)
        nBlocks++;

    // ������ݿ�ѹ��������Ҫ�ж�λ�ÿ����ݵļ���
    if((hf->pBlock->dwFlags & TPK_FILE_COMPRESSED) && hf->bBlockPosLoaded == FALSE)
    {
        SetFilePointer(ha->hFile, hf->TPKFilePos.LowPart, &hf->TPKFilePos.HighPart, FILE_BEGIN);
		// ������Ҫ��ȡλ�ÿ����ݵĴ�С
        dwToRead = (hf->nBlocks+1) * sizeof(DWORD);
        ReadFile(ha->hFile, hf->pdwBlockPos, dwToRead, &dwBytesRead, NULL);
		// �����һ��λ�ÿ����ݼ�¼��������λ�ÿ�ĳ��ȣ��Ϳ��ܱ����ܹ�
        if(hf->pdwBlockPos[0] != dwBytesRead)
            hf->pBlock->dwFlags |= TPK_FILE_ENCRYPTED;

        // ��������ܹ���������
        if(hf->pBlock->dwFlags & TPK_FILE_ENCRYPTED)
        {
            // �������֪����Կ�����ȡ��
            if(hf->dwSeed1 == 0)
                hf->dwSeed1 = DetectFileSeed(hf->pdwBlockPos, dwBytesRead);

            // �����Ȼ����֪����Կ��ֻ���˳�
            if(hf->dwSeed1 == 0)
                return 0;

            // ����λ�����ݿ�
            DecrypTPKBlock(hf->pdwBlockPos, dwBytesRead, hf->dwSeed1 - 1);
			// ���λ�����ݿ����ݳ��Ȳ���ȷ��ֱ���˳�
            if((hf->pdwBlockPos[1] - hf->pdwBlockPos[0]) > TPK_DATABLOCK_SIZE)
				return 0;
        }

        // ȷ��λ�����ݿ��Ѿ�����
        hf->bBlockPosLoaded = TRUE;
    }

    // ����׼����ȡ�ļ����ݵ�λ�úʹ�С
    dwFilePos = dwBlockPos;
    dwToRead  = blockBytes;
    if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
    {
        dwFilePos = hf->pdwBlockPos[blockNum];
        dwToRead  = hf->pdwBlockPos[blockNum + nBlocks] - dwFilePos;
    }
	// ����ʵ���ļ�λ��
    FilePos.QuadPart = hf->TPKFilePos.QuadPart + dwFilePos;

    // ׼��һ����ʱBUFFER���ڶ�ȡ���ݣ����δѹ������ֱ��ʹ�����BUFFER)
    tempBuffer = buffer;
    if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
    {
        if((tempBuffer = ALLOCMEM(BYTE, dwToRead)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return 0;
        }
    }

    // �����ļ�λ�ã�һ���Զ�ȡ�����ļ����ݿ�����
    SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
    ReadFile(ha->hFile, tempBuffer, dwToRead, &dwBytesRead, NULL);

    // �ٽ��Ŵ�����ʱBUFFER�е����ݿ�����
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

        // �����ѹ�������λ�����ݿ����ȡ���ݿ�ĳ���
        if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
            blockSize = hf->pdwBlockPos[index+1] - hf->pdwBlockPos[index];

        // ������ݿ鱻���ܹ�������Ҫ����
        if(hf->pBlock->dwFlags & TPK_FILE_ENCRYPTED)
        {
            DecrypTPKBlock((DWORD *)inputBuffer, blockSize, hf->dwSeed1 + index);
        }

        // �ж����ݿ��Ƿ�ȷʵѹ����
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

    // ����д���ѹ������Ҫɾ����ʱBUFFER
    if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
        FREEMEM(tempBuffer);

    return dwBytesRead;
}

//-----------------------------------------------------------------------------
// �ļ�������Ϊ��һ�������ݶ�ȡ
// ��ע���˷���������ʱ������һ����֤��ȷ�ԣ�����:dwToRead + dwFilePos���ܴ����ļ���С��
static DWORD WINAPI ReadTPKFileSingleUnit(TPKFile * hf, DWORD dwFilePos, BYTE * pbBuffer, DWORD dwToRead)
{
    TPKArchive * ha = hf->ha; 
    DWORD dwBytesRead = 0;

    // �������ļ�ȷʵ��ѹ�������Ƚ�ѹ��
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
                // �ƶ��ļ�ָ�뵽�ļ���ʼ��
                SetFilePointer(ha->hFile, hf->TPKFilePos.LowPart, &hf->TPKFilePos.HighPart, FILE_BEGIN);

                // ��ȡѹ������
                ReadFile(ha->hFile, inputBuffer, inputBufferSize, &dwBytesRead, NULL);

                if(hf->pBlock->dwFlags & TPK_FILE_COMPRESSED)
                    Decompress_zlib((char *)hf->pbFileBuffer, &outputBufferSize, (char *)inputBuffer, (int)inputBufferSize);
            }

            // �ͷ���ʱBUFFER
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
        // ��ȡ��ѹ��������
        ReadFile(ha->hFile, pbBuffer, dwToRead, &dwBytesRead, NULL);
    }

    return dwBytesRead;
}


//-----------------------------------------------------------------------------
// ���ļ����ڶ�ȡĳ�ļ�������
static DWORD WINAPI ReadTPKFile(TPKFile* hf, DWORD dwFilePos, BYTE* pbBuffer, DWORD dwToRead)
{
    TPKArchive* ha		= hf->ha; 
    TPKBlock* pBlock	= hf->pBlock;		// ��Ӧ��ǰ�ļ���Block��
    DWORD dwBytesRead	= 0;				// ���������ֽ���
    DWORD dwBlockPos;						// ���������ļ����ݵ�λ��
    DWORD dwLoaded;

    // ����ļ���ƫ��λ���Ƿ��Ѿ������ļ���С��
    if(dwFilePos >= pBlock->dwFSize)
        return dwBytesRead;

    // Ƕ��Խ���ȡ
    if((pBlock->dwFSize - dwFilePos) < dwToRead)
        dwToRead = (pBlock->dwFSize - dwFilePos);

    // ����ļ�����Ϊ�������ݿ飬�����⴦��
    if(pBlock->dwFlags & TPK_FILE_SINGLE_UNIT)
        return ReadTPKFileSingleUnit(hf, dwFilePos, pbBuffer, dwToRead);

    // ׼ȷ��λ�ļ��Ķ�ȡλ��(���ļ������Թ̶���С�����ݿ�洢��)
    dwBlockPos = dwFilePos & ~(TPK_DATABLOCK_SIZE - 1);

    // ������ݿ�Ķ�ȡλ��δ����TPK_FILE_BLOCKSIZE��˵����һ�����ݿ�Ǳ�׼λ�ö�ȡ��
	// ��Ӧ����Ԥ�����������ٽ�ȡ����
    if((dwFilePos % TPK_DATABLOCK_SIZE) != 0)
    {        
        DWORD dwToCopy;	// ��pbBlockBuffer��ʣ����ֽ���
        DWORD dwLoaded = TPK_DATABLOCK_SIZE;

        // ����ϴβ������ļ��Ƿ�Ҳ�ǵ�ǰ�ļ��������Ѿ�Ԥ�ع���cache
        if(hf != ha->pLastFile || dwBlockPos != ha->dwBlockPos)
        {
            // ��Ԥ�����һ�����ݿ�
            dwLoaded = ReadTPKBlocks(hf, dwBlockPos, ha->pbBlockBuffer, TPK_DATABLOCK_SIZE);
            if(dwLoaded == 0)
                return (DWORD)-1;

            // ��¼���Ԥ����һЩ�����Ա�����ʹ��
            ha->pLastFile  = hf;
            ha->dwBlockPos = dwBlockPos;
            ha->dwBuffPos  = dwFilePos % TPK_DATABLOCK_SIZE;
        }
        dwToCopy = dwLoaded - ha->dwBuffPos;
        if(dwToCopy > dwToRead)
            dwToCopy = dwToRead;

        // ��block buffer������Ŀ��buffer
        memcpy(pbBuffer, ha->pbBlockBuffer + ha->dwBuffPos, dwToCopy);
    
        // ����һЩ��ȡ�Ĳ������
        dwToRead      -= dwToCopy;
        dwBytesRead   += dwToCopy;
        pbBuffer      += dwToCopy;
        dwBlockPos    += TPK_DATABLOCK_SIZE;
        ha->dwBuffPos += dwToCopy;

        // ���û�пɶ��ģ��˳�
        if(dwToRead == 0)
            return dwBytesRead;
    }

    // �����������(dwToRead)���м����TPK_DATABLOCK_SIZE��С��һ����������
    if(dwToRead > TPK_DATABLOCK_SIZE)
    {                                           
        DWORD dwBlockBytes = dwToRead & ~(TPK_DATABLOCK_SIZE - 1);

        dwLoaded = ReadTPKBlocks(hf, dwBlockPos, pbBuffer, dwBlockBytes);
        if(dwLoaded == 0)
            return (DWORD)-1;

        // ����һЩ��ȡ�Ĳ������
        dwToRead    -= dwLoaded;
        dwBytesRead += dwLoaded;
        pbBuffer    += dwLoaded;
        dwBlockPos  += dwLoaded;

        // ���û�пɶ��ģ��˳�
        if(dwToRead == 0)
            return dwBytesRead;
    }

    // �����������(dwToRead)δ�����β���ֽ�����
    if(dwToRead > 0)
    {
        DWORD dwToCopy = TPK_DATABLOCK_SIZE;

        // ����ϴβ������ļ��Ƿ�Ҳ�ǵ�ǰ�ļ��������Ѿ�Ԥ�ع���cache
        if(hf != ha->pLastFile || dwBlockPos != ha->dwBlockPos)
        {
            // ����TPK_DATABLOCK_SIZE��С��ȡ����
            dwToCopy = ReadTPKBlocks(hf, dwBlockPos, ha->pbBlockBuffer, TPK_DATABLOCK_SIZE);
            if(dwToCopy == 0)
                return (DWORD)-1;

            // ��¼���Ԥ����һЩ�����Ա�����ʹ��
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
// ��ȡ�ļ�����
BOOL WINAPI TPKReadFile(HANDLE hFile, VOID* lpBuffer, DWORD dwToRead, DWORD* pdwRead, LPOVERLAPPED lpOverlapped)
{
    TPKFile* hf = (TPKFile*)hFile;
    DWORD dwBytes = 0;                  // Number of bytes (for everything)
    int nError = ERROR_SUCCESS;

    if(pdwRead != NULL)
        *pdwRead = 0;

    // �������Ƿ���Ч
    if(nError == ERROR_SUCCESS)
    {
        if(hf == NULL || lpBuffer == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }

    // ����������ļ�����Ѿ����ڣ���ֱ��ʹ��WIN32API��ȡ�������ļ���ȡ��
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

    // �ļ����ڶ�ȡ�ļ�
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

    // ����ȡ���ֽ�����
    if(dwBytes < dwToRead)
    {
        SetLastError(ERROR_HANDLE_EOF);
        return FALSE;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// ��ȡ�ļ�λ��
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

    // ����򿪵���һ�������ļ�����ֱ���˳�
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return 0;

    // ������ļ����ڴ��ļ����򷵻��ļ�λ��
    if(pdwFilePosHigh != NULL)
        *pdwFilePosHigh = hf->TPKFilePos.HighPart;
    return hf->TPKFilePos.LowPart;
}

//-----------------------------------------------------------------------------
// ��ȡ�ļ���С
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

    // ����򿪵���һ�������ļ�����ֱ�ӵ���WIN32API�������ļ��Ĵ�С
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return GetFileSize(hf->hFile, pdwFileSizeHigh);

    // ������ļ����ڴ��ļ����򷵻��ļ���С
    return hf->pBlock->dwFSize;
}

//-----------------------------------------------------------------------------
// �����ļ�����λ��
DWORD WINAPI TPKSetFilePointer(HANDLE hFile, LONG lFilePos, LONG* pdwFilePosHigh, DWORD dwMethod)
{
    TPKArchive* ha;
    TPKFile* hf = (TPKFile*)hFile;
	// �����������Ƿ���ȷ
    if(hf == NULL || (pdwFilePosHigh != NULL && *pdwFilePosHigh != 0))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (DWORD)-1;
    }

    // ����򿪵���һ�������ļ�����ʹ��Win32API
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return SetFilePointer(hf->hFile, lFilePos, pdwFilePosHigh, dwMethod);

    ha = hf->ha;

    switch(dwMethod)
    {
        case FILE_BEGIN:
            // ������ǰԽ����ǰ�ļ����ݵ���ʼλ�ã��ž�Խ���ȡ����
            if(-lFilePos > (LONG)hf->dwFilePos)
                hf->dwFilePos = 0;
            else
                hf->dwFilePos = lFilePos;
            break;

        case FILE_CURRENT:
            // ������ǰԽ����ǰ�ļ����ݵ���ʼλ�ã��ž�Խ���ȡ����
            if(-lFilePos > (LONG)hf->dwFilePos)
                hf->dwFilePos = 0;
            else
                hf->dwFilePos += lFilePos;
            break;

        case FILE_END:
            // ������ǰԽ����ǰ�ļ����ݵ���ʼλ�ã��ž�Խ���ȡ����
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
// ��ȡ�ļ���Ϣ
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

    // δ֪�Ĳ�������Ч���ļ����
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0xFFFFFFFF;
}