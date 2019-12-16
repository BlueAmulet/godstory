#include "TPKLib.h"
#include "TPKCommon.h"
#include "crc32.h"
#include "zlib.h"

// ============================================================================
// ѹ�����ѹ����
// ============================================================================
//-----------------------------------------------------------------------------
// ѹ�����ݷ���
int WINAPI Compress_zlib(char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength)
{
    z_stream z; 
    int nResult;
    // ���zlib��stream�ṹ����
    z.next_in   = (Bytef *)pbInBuffer;
    z.avail_in  = (uInt)dwInLength;
    z.total_in  = dwInLength;
    z.next_out  = (Bytef *)pbOutBuffer;
    z.avail_out = *pdwOutLength;
    z.total_out = 0;
    z.zalloc    = NULL;
    z.zfree     = NULL;
	// ��ʼ��ѹ������
    *pdwOutLength = 0;
    if((nResult = deflateInit(&z, Z_DEFAULT_COMPRESSION)) == 0)
    {
		// ִ��zlib��ѹ��
        nResult = deflate(&z, Z_FINISH);        
        if(nResult == Z_OK || nResult == Z_STREAM_END)
            *pdwOutLength = z.total_out;
        deflateEnd(&z);
	}
    return nResult;
}

//-----------------------------------------------------------------------------
// ��ѹ������
int WINAPI Decompress_zlib(char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength)
{
    z_stream z;
    int nResult;

    // ���zlib��stream�ṹ����
    z.next_in   = (Bytef *)pbInBuffer;
    z.avail_in  = (uInt)dwInLength;
    z.total_in  = dwInLength;
    z.next_out  = (Bytef *)pbOutBuffer;
    z.avail_out = *pdwOutLength;
    z.total_out = 0;
    z.zalloc    = NULL;
    z.zfree     = NULL;
	// ��ʼ��ѹ������
    if((nResult = inflateInit(&z)) == 0)
    {
        // ִ��zlib�Ľ�ѹ
        nResult = inflate(&z, Z_FINISH);
        *pdwOutLength = z.total_out;
        inflateEnd(&z);
    }
    return nResult;
}

// ============================================================================
// ��ѩ�����㷨
// ============================================================================
#define CRYPT_BUFFER_SIZE   0x500
static DWORD CryptBuffer[CRYPT_BUFFER_SIZE];
static BOOL  bCryptBufferCreated = FALSE;

//-----------------------------------------------------------------------------
// ��ʼ������BUFFER
int PrepareCryptBuffer()
{
    DWORD dwSeed = 0x00100001;
    // ��ʼ������BUFFER������Ѿ���ʼ�������Թ�
    if(bCryptBufferCreated == FALSE)
    {
        for(DWORD index1 = 0; index1 < 0x100; index1++)
        {
            for(DWORD index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
            {
                DWORD temp1, temp2;
                dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
                temp1  = (dwSeed & 0xFFFF) << 0x10;
                dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
                temp2  = (dwSeed & 0xFFFF);
                CryptBuffer[index2] = (temp1 | temp2);
            }
        }
        bCryptBufferCreated = TRUE;
    }
    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// ����HashTable
void EncryptHashTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);
        dwSeed1 = CryptBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2   += CryptBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable;
        *pdwTable++ = ch ^ (dwSeed1 + dwSeed2);
        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

//-----------------------------------------------------------------------------
// ����HashTable
void DecryptHashTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);
        dwSeed1 = CryptBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2 += CryptBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable ^ (dwSeed1 + dwSeed2);
        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *pdwTable++ = ch;
    }
}

//-----------------------------------------------------------------------------
// ����BlockTable
void EncryptBlockTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);
        dwSeed1 = CryptBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2   += CryptBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable;
        *pdwTable++ = ch ^ (dwSeed1 + dwSeed2);
        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

//-----------------------------------------------------------------------------
// ����BlockTable
void DecryptBlockTable(DWORD* pdwTable, BYTE* pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);
        dwSeed1 = CryptBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2 += CryptBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *pdwTable ^ (dwSeed1 + dwSeed2);
        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *pdwTable++ = ch;
    }
}

//-----------------------------------------------------------------------------
// ���Ի�ȡ������Կ������������ݱ�����ÿ��ѹ���ļ��Ŀ�ʼλ�ã��������Ǹ���λ��
// ���ݿ��һ��DWORDֵ����֪�ļ������ݿ��������Լ��ļ��Ĵ�С���������ܰ�������
// ��֪������Կ��
// block		�ļ�λ�����ݿ�
// decrypted	�ļ�λ�����ݿ�ĳ���

DWORD DetectFileSeed(DWORD* block, DWORD decrypted)
{
    DWORD saveSeed1;
    DWORD temp = *block ^ decrypted;    // temp = seed1 + seed2
    temp -= 0xEEEEEEEE;                 // temp = seed1 + CryptBuffer[0x400 + (seed1 & 0xFF)]

    for(int i = 0; i < 0x100; i++)      // Try all 255 possibilities
    {
        DWORD seed1;
        DWORD seed2 = 0xEEEEEEEE;
        DWORD ch;

        // Try the first DWORD (We exactly know the value)
        seed1  = temp - CryptBuffer[0x400 + i];
        seed2 += CryptBuffer[0x400 + (seed1 & 0xFF)];
        ch     = block[0] ^ (seed1 + seed2);

        if(ch != decrypted)
            continue;

        // Add 1 because we are decrypting block positions
        saveSeed1 = seed1 + 1;

        // If OK, continue and test the second value. We don't know exactly the value,
        // but we know that the second one has lower 16 bits set to zero
        // (no compressed block is larger than 0xFFFF bytes)
        seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
        seed2  = ch + seed2 + (seed2 << 5) + 3;

        seed2 += CryptBuffer[0x400 + (seed1 & 0xFF)];
        ch     = block[1] ^ (seed1 + seed2);

        if((ch & 0xFFFF0000) == 0)
            return saveSeed1;
    }
    return 0;
}

// Function tries to detect file seed. It expectes at least two uncompressed bytes
DWORD DetectFileSeed2(DWORD * pdwBlock, UINT nDwords, ...)
{
    va_list argList;
    DWORD dwDecrypted[0x10];
    DWORD saveSeed1;
    DWORD dwTemp;
    DWORD i, j;
    
    // We need at least two DWORDS to detect the seed
    if(nDwords < 0x02 || nDwords > 0x10)
        return 0;
    
    va_start(argList, nDwords);
    for(i = 0; i < nDwords; i++)
        dwDecrypted[i] = va_arg(argList, DWORD);
    va_end(argList);
    
    dwTemp = (*pdwBlock ^ dwDecrypted[0]) - 0xEEEEEEEE;
    for(i = 0; i < 0x100; i++)      // Try all 255 possibilities
    {
        DWORD seed1;
        DWORD seed2 = 0xEEEEEEEE;
        DWORD ch;

        // Try the first DWORD
        seed1  = dwTemp - CryptBuffer[0x400 + i];
        seed2 += CryptBuffer[0x400 + (seed1 & 0xFF)];
        ch     = pdwBlock[0] ^ (seed1 + seed2);

        if(ch != dwDecrypted[0])
            continue;

        saveSeed1 = seed1;

        // If OK, continue and test all bytes.
        for(j = 1; j < nDwords; j++)
        {
            seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
            seed2  = ch + seed2 + (seed2 << 5) + 3;

            seed2 += CryptBuffer[0x400 + (seed1 & 0xFF)];
            ch     = pdwBlock[j] ^ (seed1 + seed2);

            if(ch == dwDecrypted[j] && j == nDwords - 1)
                return saveSeed1;
        }
    }
    return 0;
}


//-----------------------------------------------------------------------------
// �����ļ����ݿ�
void EncrypTPKBlock(DWORD* block, DWORD dwLength, DWORD dwSeed1)
{
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    dwLength >>= 2;
    while(dwLength-- > 0)
    {
        dwSeed2 += CryptBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *block;
        *block++ = ch ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

//-----------------------------------------------------------------------------
// �����ļ����ݿ�
void DecrypTPKBlock(DWORD* block, DWORD dwLength, DWORD dwSeed1)
{
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    dwLength >>= 2;
    while(dwLength-- > 0)
    {
        dwSeed2 += CryptBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *block ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *block++ = ch;
    }
}

//-----------------------------------------------------------------------------
// �����ļ������HashTable��Hash�������ֵ
DWORD DecryptHashIndex(TPKArchive* ha, const char* szFileName)
{
    BYTE*	pbKey   = (BYTE *)szFileName;
    DWORD	dwSeed1 = 0x7FED7FED;
    DWORD	dwSeed2 = 0xEEEEEEEE;
    DWORD	ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);
        dwSeed1 = CryptBuffer[0x000 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return (dwSeed1 & (ha->pHeader->dwHashTableSize - 1));
}

//-----------------------------------------------------------------------------
// �ļ�����ϣ����ֵ1
DWORD DecryptName1(const char* szFileName)
{
    BYTE*  pbKey	= (BYTE *)szFileName;
    DWORD  dwSeed1	= 0x7FED7FED;
    DWORD  dwSeed2	= 0xEEEEEEEE;
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);
        dwSeed1 = CryptBuffer[0x100 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

//-----------------------------------------------------------------------------
// �ļ�����ϣ����ֵ2
DWORD DecryptName2(const char* szFileName)
{
    BYTE*  pbKey	= (BYTE *)szFileName;
    DWORD  dwSeed1	= 0x7FED7FED;
    DWORD  dwSeed2	= 0xEEEEEEEE;
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);
        dwSeed1 = CryptBuffer[0x200 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

//-----------------------------------------------------------------------------
// ��ȡ�ļ��ļ�����Կ
DWORD DecryptFileSeed(const char* szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;          // EBX
    DWORD  dwSeed2 = 0xEEEEEEEE;          // ESI
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);           // ECX
        dwSeed1 = CryptBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

//-----------------------------------------------------------------------------
// �����ļ������ļ������ȡHashTable��Hash��
TPKHash* GetHashEntry(TPKArchive* ha, const char* szFileName)
{
    TPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TPKHash * pHash0;                      // HashTable����ʼHash��
    TPKHash * pHash;                       // ��ǰHash��
    DWORD dwIndex = (DWORD)(DWORD_PTR)szFileName;
    DWORD dwName1;
    DWORD dwName2;

    // ����ļ�����һ������ֵ,��ͨ������ֵ��ʽ������Hash��
    if(dwIndex <= ha->pHeader->dwHashTableSize)
    {
        for(pHash = ha->pHashTable; pHash < pHashEnd; pHash++)
        {
            if(pHash->dwBlockIndex == dwIndex)
                return pHash;
        }
        return NULL;
    }

    // ����Hash���Block���������
    dwIndex = DecryptHashIndex(ha, szFileName);
    dwName1 = DecryptName1(szFileName);
    dwName2 = DecryptName2(szFileName);
    pHash   = pHash0 = ha->pHashTable + dwIndex;
    
    while(pHash->dwBlockIndex != DATA_ENTRY_FREE)
    {
        if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2 && pHash->dwBlockIndex != DATA_ENTRY_DELETED)
            return pHash;

        if(++pHash >= pHashEnd)
            pHash = ha->pHashTable;
        if(pHash == pHash0)
            break;
    }

    // ��ǰ�ļ�����Hash���޷��ҵ�
    return NULL;
}

//-----------------------------------------------------------------------------
// ���ļ�����HashTable��Ѱ��һ�����е�Hash��
TPKHash* FindFreeHashEntry(TPKArchive* ha, const char* szFileName)
{
    TPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TPKHash * pHash0;
    TPKHash * pHash;
    DWORD dwIndex = DecryptHashIndex(ha, szFileName);
    DWORD dwName1 = DecryptName1(szFileName);
    DWORD dwName2 = DecryptName2(szFileName);
    DWORD dwBlockIndex = 0xFFFFFFFF;

    pHash = pHash0 = ha->pHashTable + dwIndex;
    while(pHash->dwBlockIndex < DATA_ENTRY_DELETED)
    {
        if(++pHash >= pHashEnd)
            pHash = ha->pHashTable;
        if(pHash == pHash0)
            return NULL;
    }

    pHash->dwName1   = dwName1;
    pHash->dwName2   = dwName2;

	// ע�⣺����û��Ѱ��һ�����е�Block����Hash����Ե��ô˷���
	// �ĵط�����ǵ�����pHash->dwBlockIndex��ֵ

 //   // ����һ������Block��õ�BlockIndexֵ
 //   for(dwIndex = 0; dwIndex < ha->pHeader->dwBlockTableSize; dwIndex++)
 //   {
 //       TPKBlock* pBlock = ha->pBlockTable + dwIndex;
 //       if((pBlock->dwFlags & TPK_FILE_EXISTS) == 0)
 //       {
 //           dwBlockIndex = dwIndex;
 //           break;
 //       }
 //   }

 //   // If no free block entry found, we have to use the index
 //   // at the end of the current block table
 //   if(dwBlockIndex == 0xFFFFFFFF)
 //       dwBlockIndex = ha->pHeader->dwBlockTableSize;
 //   pHash->dwBlockIndex = dwBlockIndex;
    return pHash;
}

// ============================================================================
// �������ߺ���
// ============================================================================
//-----------------------------------------------------------------------------
// ����Ƿ���Ч��TPK�ļ������
BOOL IsValidTPKHandle(TPKArchive * ha)
{
    if(ha == NULL || IsBadReadPtr(ha, sizeof(TPKArchive)))
        return FALSE;
    if(ha->pHeader == NULL || IsBadReadPtr(ha->pHeader, sizeof(TPKHeader)))
        return FALSE;    
    return (ha->pHeader->dwID == TPK_FILEID);
}

//-----------------------------------------------------------------------------
// ����Ƿ���Ч�ļ����
BOOL IsValidFileHandle(TPKFile * hf)
{
    if(hf == NULL || IsBadReadPtr(hf, sizeof(TPKFile)))
        return FALSE;

    if(hf->hFile != INVALID_HANDLE_VALUE)
        return TRUE;

    return IsValidTPKHandle(hf->ha);
}

//-----------------------------------------------------------------------------
// ����ƥ���ַ����Ƿ���ͨ���
BOOL CheckWildCard(const char * szString, const char * szWildCard)
{
    char * szTemp;
    int nResult = 0;
    int nMustNotMatch = 0;
    int nMustMatch = 0;

    // �������Ƿ���ȷ
    if(*szString == 0 || szWildCard == NULL || *szWildCard == 0)
        return FALSE;

    for(;;)
    {
        switch(*szWildCard)
        {
            case '*': // ˵�������ַ���ƥ��
                while(*szWildCard == '*')
                    szWildCard++;

                // ���û�и�����ַ���ͨ������ʾ�����ַ���ƥ��
                if(*szWildCard == 0)
                    return TRUE;

                // ͨ����ַ����к������ַ��Թ�
                nMustNotMatch |= 0x70000000;
                break;
            
            case '?':  // ˵����ƥ�䵥���ַ�
                while(*szWildCard == '?')
                {
                    nMustNotMatch++;
                    szWildCard++;
                }
                break;

            default:
                // ������ַ���ͬ
                if(toupper(*szString) == toupper(*szWildCard))
                {
                    // �Ѿ��ǽ�������ƥ��ɹ�
                    if(*szString == 0)
                        return TRUE;

                    nMustNotMatch = 0;
                    szWildCard++;
                    szString++;
                    break;
                }

                // ���Ҫ�������ַ���ȷƥ�䣬����һ������ƥ��ʧ����
                if(nMustNotMatch == 0)
                    return FALSE;

                // ͳ���ڲ���Ҫƥ����ַ�����Щ����ƥ����ַ�
                szTemp = (char *)szWildCard;
                nMustMatch = 0;
                while(*szTemp != 0 && *szTemp != '*' && *szTemp != '?')
                {
                    nMustMatch++;
                    szTemp++;
                }

                nResult = -1;
                while(nMustNotMatch > 0 && *szString != 0)
                {
                    if((nResult = _strnicmp(szString, szWildCard, nMustMatch)) == 0)
                        break;
                    
                    szString++;
                    nMustNotMatch--;
                }

                if(nMustNotMatch == 0)
                    nResult = _strnicmp(szString, szWildCard, nMustMatch);

                if(nResult == 0)
                {
                    nMustNotMatch = 0;
                    szWildCard += nMustMatch;
                    szString   += nMustMatch;
                    break;
                }
                return FALSE;
        }
    }
}

//-----------------------------------------------------------------------------
// �ݹ鴴��Ŀ¼
void EnsureDirectory(const char* path)
{
	static char dir[_MAX_PATH] = {0};
	static char div[_MAX_PATH] = {0};
	_splitpath(path,dir,div,NULL,NULL);
	strcat(dir,div);

	if (0xFFFFFFFF != GetFileAttributes(path))
		return;

	for (size_t i = 0; i < strlen(dir); i++)
	{
		if (dir[i] == '\\' || dir[i] == '/')
		{
			char tmp = dir[i];
			dir[i] = 0;
			if (0xFFFFFFFF == GetFileAttributes(dir))
			{
				::CreateDirectory(dir,0);
			}
			dir[i] = tmp;
		}
	}
}

//-----------------------------------------------------------------------------
// ����ļ�ֻ����ϵͳ����������
void ClearFileAttributes(const char* szString)
{
	DWORD dwFileAttribs = GetFileAttributes(szString);
	if(dwFileAttribs == 0xFFFFFFFF)
		return;

	dwFileAttribs &= ~FILE_ATTRIBUTE_HIDDEN;
	dwFileAttribs &= ~FILE_ATTRIBUTE_READONLY;
	dwFileAttribs &= ~FILE_ATTRIBUTE_SYSTEM;
	if(SetFileAttributes(szString, dwFileAttribs) == 0)
	{
		char szCmd[1024];
		sprintf(szCmd, "cmd/C/D attrib -r -s -h %s", szString);
		WinExec(szCmd, SW_HIDE);
	}
}