#include  "DataStream.h"

#define READED_LENGTH 128

std::string CDataStream::ReadLine(void)
{
    static char tmpBuf[READED_LENGTH];

    std::string retString = "";
    size_t      readCount = 0;

    // Keep looping while not hitting delimiter
    while ((readCount = Read(tmpBuf, READED_LENGTH-1)) != 0)
    {
        // Terminate string
        tmpBuf[readCount] = '\0';

        char* p = strchr(tmpBuf, '\n');

        if (p != 0)
        {
            // Reposition backwards
            Skip((long)(p + 1 - tmpBuf - readCount));
            *p = '\0';
        }

        retString += tmpBuf;

        if (p != 0)
        {
            // Trim off trailing CR if this was a CR/LF entry
            if (retString.length() && retString[retString.length()-1] == '\r')
            {
                retString.erase(retString.length()-1, 1);
            }

            break;
        }
    }

    return retString;
}