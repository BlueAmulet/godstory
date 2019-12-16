#include "ZipStruct.h"

#define SIZEZIPLOCALHEADER (0x1e)

bool CLocalHeader::Read(ZIPTARGET& stream)
{
    if (0 == stream.Read(signature))
        return false;

    //检测是否是目录
    if (signature != 0x04034b50)
        return false;

    if (0 == stream.Read(version))
        return false;

    if (0 == stream.Read(flag))
        return false;

    if (0 == stream.Read(cmethod))
        return false;

    if (0 == stream.Read(time))
        return false;

    if (0 == stream.Read(crc))
        return false;

    if (0 == stream.Read(csize))
        return false;

    if (0 == stream.Read(ucsize))
        return false;

    if (0 == stream.Read(namelen))
        return false;

    if (0 == stream.Read(extralen))
        return false;

    if (namelen > 0)
    {
        if (NULL != name)
            delete []name;

        name = new char[namelen + 1];
        stream.Read(name,namelen);
        name[namelen] = 0;
    }

    if (extralen > 0)
    {
        if (NULL != extra)
            delete []extra;

        extra = new char[extralen + 1];
        stream.Read(extra,extralen);
        extra[extralen] = 0;
    }

    return true;
}

bool CLocalHeader::Write(ZIPTARGET& stream)
{
    if (0 == stream.Write(signature))
        return false;

    if (0 == stream.Write(version))
        return false;

    if (0 == stream.Write(flag))
        return false;

    if (0 == stream.Write(cmethod))
        return false;

    if (0 == stream.Write(time))
        return false;

    if (0 == stream.Write(crc))
        return false;

    if (0 == stream.Write(csize))
        return false;

    if (0 == stream.Write(ucsize))
        return false;

    if (0 == stream.Write(namelen))
        return false;

    if (0 == stream.Write(extralen))
        return false;

    if (namelen > 0)
    {
        stream.Write(name,namelen);
    }

    if (extralen > 0)
    {
        stream.Write(extra,extralen);
    }

    return true;
}

/************************************************************************/
/* 从文件中读入目录信息                                                                  */
/************************************************************************/
bool zlist::Read(ZIPTARGET& stream)
{
    if (0 == stream.Read(signature))
        return false;

    //检测是否是目录
    if (signature != 0x02014b50)
        return false;

    if (0 == stream.Read(vem))
        return false;

    if (0 == stream.Read(ver))
        return false;

    if (0 == stream.Read(flg))
        return false;

    if (0 == stream.Read(how))
        return false;

    if (0 == stream.Read(tim))
        return false;

    if (0 == stream.Read(crc))
        return false;

    if (0 == stream.Read(siz))
        return false;

    if (0 == stream.Read(len))
        return false;

    if (0 == stream.Read(nam))
        return false;

    if (0 == stream.Read(cext))
        return false;

    if (0 == stream.Read(com))
        return false;

    if (0 == stream.Read(dsk))
        return false;

    if (0 == stream.Read(att))
        return false;

    if (0 == stream.Read(atx))
        return false;

    if (0 == stream.Read(off))
        return false;

    if (0 == stream.Read(name,nam))
        return false;

    name[nam] = 0;

    if (cext > 0)
    {
        if (NULL != cextra)
            delete []cextra;

        cextra = new char[cext + 1];

        if (0 == stream.Read(cextra,cext))
            return false;

        cextra[cext] = 0;
    }

    if (com > 0)
    {
        if (NULL != comment)
            delete []comment;

        comment = new char[com + 1];

        if (0 == stream.Read(comment,com))
            return false;

        comment[com] = 0;
    }

    return true;
}

bool zlist::Write(ZIPTARGET& stream)
{
    signature = 0x02014b50L;

    if (0 == stream.Write(signature))
        return false;

    if (0 == stream.Write(vem))
        return false;

    if (0 == stream.Write(ver))
        return false;

    if (0 == stream.Write(flg))
        return false;

    if (0 == stream.Write(how))
        return false;

    if (0 == stream.Write(tim))
        return false;

    if (0 == stream.Write(crc))
        return false;

    if (0 == stream.Write(siz))
        return false;

    if (0 == stream.Write(len))
        return false;

    if (0 == stream.Write(nam))
        return false;

    if (0 == stream.Write(cext))
        return false;

    if (0 == stream.Write(com))
        return false;

    if (0 == stream.Write(dsk))
        return false;

    if (0 == stream.Write(att))
        return false;

    if (0 == stream.Write(atx))
        return false;

    if (0 == stream.Write(off))
        return false;

    if (0 == stream.Write(name,nam))
        return false;

    if (cext > 0)
    {
        if (0 == stream.Write(cextra,cext))
            return false;
    }

    if (com > 0)
    {
        if (0 == stream.Write(comment,com))
            return false;
    }

    return true;
}

bool CEndCentralDir::Read(ZIPTARGET& stream)
{
    if (0 == stream.Read(signature))
        return false;

    //检测是否是目录
    if (signature != 0x06054b50)
        return false;

    if (0 == stream.Read(nDisk))
        return false;

    if (0 == stream.Read(nDiskDir))
        return false;

    if (0 == stream.Read(nEntryOnDir))
        return false;

    if (0 == stream.Read(nEntry))
        return false;

    if (nEntryOnDir != nEntry || nDisk != 0 || nDiskDir != 0)
        return false;

    if (0 == stream.Read(size))
        return false;

    if (0 == stream.Read(dirStart))
        return false;

    if (0 == stream.Read(commentLen))
        return false;

    if (commentLen > 0)
    {
        if (NULL != comment)
            delete []comment;

        comment = new char[commentLen + 1];

        if (0 == stream.Read(comment,commentLen))
            return false;

        comment[commentLen] = 0;
    }

    return true;
}

bool CEndCentralDir::Write(ZIPTARGET& stream)
{
    signature = 0x06054b50L;

    if (0 == stream.Write(signature))
        return false;

    if (0 == stream.Write(nDisk))
        return false;

    if (0 == stream.Write(nDiskDir))
        return false;
    
    if (0 == stream.Write(nEntryOnDir))
        return false;

    if (0 == stream.Write(nEntry))
        return false;

    if (0 == stream.Write(size))
        return false;

    if (0 == stream.Write(dirStart))
        return false;

    if (0 == stream.Write(commentLen))
        return false;
    
    if (commentLen > 0)
    {
        if (0 == stream.Write(comment,commentLen))
            return false;
    }

    return true;
}