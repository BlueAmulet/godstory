#include "Crc.h"

#define MAX_BUFFER_SIZE	4096

ulg CalCrc(DataStreamPtr stream)
{
    ulg crcRslt = 0;

    char buffer[MAX_BUFFER_SIZE];

    size_t nCount = stream->Read(buffer,MAX_BUFFER_SIZE);

    while(nCount)
    {
        for(size_t i = 0; i < nCount; i++)
        {
            crcRslt = CRC32(crcRslt,buffer[i]);
        }

        nCount = stream->Read(buffer,MAX_BUFFER_SIZE);
    }

    return crcRslt;
}