#include "TpkDataFile.h"
#include "FSUtils.h"
#include <fstream>
#include "TpkStruct.h"
#include "FileDataStream.h"
#include "MemDataStream.h"

CTpkDataFile::CTpkDataFile(const char* filePath,DataStreamPtr stream):
    m_isValid(true)
{
    assert(stream->Size() > 0 && is_exist(filePath) && "数据文件不存在！");

    m_dataFileName  = ::GetFileName(filePath);
    m_dataFilePath  = filePath;
    m_dataFileSize  = stream->Size();
    m_curAppendSize = 0;

    __GetUniqueID(stream);

    m_stream = stream;
}

void CTpkDataFile::__GetUniqueID(DataStreamPtr stream)
{
    stream->Seek(0);
    //假如存在索引文件,则打开并初始化索引数据
    CTpkHeader header;
    
    if (!header.Read(stream))
        m_isValid = false;

    m_uid = header.id;
}

DataStreamPtr CTpkDataFile::GetDataFileStream(void)
{
    return m_stream;
}

/************************************************************************/
/* 从offset处获取size大小的数据
/************************************************************************/
DataStreamPtr CTpkDataFile::GetItemData(size_t offset,size_t size,bool& isCmp)
{
    if (!m_stream)
        return NULL;
    
    //判断文件大小是否满足
    if (offset + size + sizeof(CTpkItem) > m_dataFileSize)
        return NULL;

    m_stream->Seek(0);
    m_stream->Seek(offset);

    //读取item头
    CTpkItem item;

    if (!item.Read(m_stream))
        return NULL;

    isCmp = (item.flag != BIT(0));
    
    DataStreamPtr rslt;
    //读取数据
    if (size == 0)
    {
        size = 1;
        char* pData = new char[1];
        pData[0] = 0;
        rslt = new CMemDataStream(pData,size);
        rslt->Seek(0);
    }
    else
    {
        char* pData = new char[size];
        m_stream->Read(pData,size);
        rslt = new CMemDataStream(pData,size);
        rslt->Seek(0);
        rslt->Seek(size);
    }
    
    return rslt;
}

void CTpkDataFile::Reset(void)
{
    m_curAppendSize = 0;
    m_dataFileSize  = GetFileSize(m_dataFilePath.c_str());
}