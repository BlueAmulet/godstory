#include "TpkStruct.h"

#define MAX_FILENAME_LEN 256

bool CTpkHeader::Read(DataStreamPtr& stream)
{
    if (0 ==stream->Read(flag))
        return false;

    if (flag != TPK_FILE)
        return false;

    if (0 == stream->Read(version))
        return false;

    if (version != TPK_VERSION)
        return false;

    if (0 == stream->Read(&id,sizeof(id)))
        return false;

    return !stream->IsEof();
}

bool CTpkHeader::Write(DataStreamPtr& stream)
{
    if (0 == stream->Write(flag))
        return false;

    if (0 == stream->Write(version))
        return false;

    if (0 == stream->Write(&id,sizeof(id)))
        return false;

    return true;
}

//---------------------------------------------------------------------------------------------------
bool CTpkItem::Read(DataStreamPtr& stream)
{
    if (0 == stream->Read(crc))
        return false;

    if (0 == stream->Read(flag))
        return false;

    return true;
}

bool CTpkItem::Write(DataStreamPtr& stream)
{
    if (0 == stream->Write(crc))
        return false;

    if (0 == stream->Write(flag))
        return false;

    return true;
}

//---------------------------------------------------------------------------------------------------
bool CIndexHeader::Read(DataStreamPtr& stream)
{   
    if (0 ==stream->Read(flag))
        return false;

    if (flag != TPK_INDEX)
        return false;

    if (0 == stream->Read(version))
        return false;

    if (version != TPK_INDEX_VERSION)
        return false;

    if (0 == stream->Read(encrptMethod))
        return false;

    if (0 == stream->Read(nFiles))
        return false;

    if (0 == stream->Read(&id,sizeof(id)))
        return false;

    return !stream->IsEof();
}

bool CIndexHeader::Write(DataStreamPtr& stream)
{
    if (0 == stream->Write(flag))
        return false;

    if (0 == stream->Write(version))
        return false;

    if (0 == stream->Write(encrptMethod))
        return false;

    if (0 == stream->Write(nFiles))
        return false;

    if (0 == stream->Write(&id,sizeof(id)))
        return false;

    return true;
}

//---------------------------------------------------------------------------------------------------
bool CIndexFile::Read(DataStreamPtr& stream)
{
    size_t nameLength = 0;

    if (0 ==stream->Read(nameLength))
        return false;

    if (nameLength >= MAX_FILENAME_LEN || nameLength <= 0)
        return false;

    static char nameBuf[MAX_FILENAME_LEN] = {0};

    if (0 == stream->Read(nameBuf,nameLength))
        return false;

    nameBuf[nameLength] = 0;

    name = nameBuf;

    if (0 ==stream->Read(count))
        return false;

    if (0 == stream->Read(nFreeItems))
        return false;

    return !stream->IsEof();
}

bool CIndexFile::Write(DataStreamPtr& stream)
{
    size_t nameLength = name.length();

    if (0 == stream->Write(nameLength))
        return false;

    if (0 == stream->Write(name.c_str(),nameLength))
        return false;

    if (0 == stream->Write(count))
        return false;

    if (0 == stream->Write(nFreeItems))
        return false;

    return true;
}

//---------------------------------------------------------------------------------------------------
bool CIndexList::Read(DataStreamPtr& stream)
{
    size_t nameLength = 0;

    if (0 ==stream->Read(nameLength))
        return false;

    if (nameLength >= MAX_FILENAME_LEN || nameLength <= 0)
        return false;

    static char nameBuf[MAX_FILENAME_LEN] = {0};

    if (0 == stream->Read(nameBuf,nameLength))
        return false;

    nameBuf[nameLength] = 0;

    name = nameBuf;

    if (0 ==stream->Read(offset))
        return false;

    if (0 ==stream->Read(size))
        return false;

    if (0 ==stream->Read(cmpSize))
        return false;

    if (0 ==stream->Read(itemSize))
        return false;

    return true;
}

bool CIndexList::Write(DataStreamPtr& stream)
{
    size_t nameLength = name.length();

    if (0 == stream->Write(nameLength))
        return false;

    if (0 == stream->Write(name.c_str(),nameLength))
        return false;

    if (0 == stream->Write(offset))
        return false;

    if (0 == stream->Write(size))
        return false;

    if (0 == stream->Write(cmpSize))
        return false;

    if (0 == stream->Write(itemSize))
        return false;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool CFreeItem::Read(DataStreamPtr& stream)
{
    if (0 ==stream->Read(offset))
        return false;

    if (0 ==stream->Read(itemSize))
        return false;

    return true;
}

bool CFreeItem::Write(DataStreamPtr& stream)
{
    if (0 == stream->Write(offset))
        return false;

    if (0 == stream->Write(itemSize))
        return false;

    return true;
}