#include "UpdatePkt.h"
#include "MemDataStream.h"
#include "FSUtils.h"

CUpdatePkt::CUpdatePkt(void):
    m_pIdxFile(NULL)
{
}

CUpdatePkt::~CUpdatePkt(void)
{
    if (NULL != m_pIdxFile)
        delete m_pIdxFile;

    m_pIdxFile = NULL;
}

bool CUpdatePkt::Read(DataStreamPtr& stream)
{
    if (0 ==stream->Read(m_flag))
        return false;

    if (m_flag != UPDATEPKTFLAG)
        return false;

    if (!stream->ReadString(m_versionFrom))
        return false;

    if (!stream->ReadString(m_versionTo))
        return false;

    if (!stream->ReadString(m_describe))
        return false;

    if (0 ==stream->Read(m_type))
        return false;

    //读取文件条目信息
    size_t nFiles = 0;
    if (0 == stream->Read(nFiles))
        return false;

    for (size_t i = 0; i < nFiles; i++)
    {
        FileItem fileItem;
        
        if (!stream->ReadString(fileItem.name))
            return false;

        if (0 == stream->Read(fileItem.size))
            return false;

        char* pBuf = new char[fileItem.size];
        stream->Read(pBuf,fileItem.size);
        fileItem.data = new CMemDataStream(pBuf,fileItem.size);

        m_fileItems.insert(std::make_pair(fileItem.name,fileItem));
    }

    //读取文件包里的索引文件，建立更新文件列表
    if (!__InitIdxMap())
        return false;

    //建立索引列表
    FILE_INDEXLIST_MAP& dataFileMap = m_pIdxFile->GetFileIndexMap();

    for (FILE_INDEXLIST_MAP::iterator iterDataFile = dataFileMap.begin(); iterDataFile != dataFileMap.end(); iterDataFile++)
    {
        INDEXLIST_MAP& idxMap = iterDataFile->second;

        for (INDEXLIST_MAP::iterator iterIdxFile = idxMap.begin(); iterIdxFile != idxMap.end(); iterIdxFile++)
        {
            m_idxFiles.push_back(iterIdxFile->first);
        }
    }

    return true;
}

bool CUpdatePkt::__InitIdxMap(void)
{
    if (m_type != BIT(0))
        return true;

    //查找索引文件
    for (FILES_MAP::iterator iter = m_fileItems.begin(); iter != m_fileItems.end(); iter++)
    {
        std::string ext = GetFileExt(iter->first);

        if (ext == ".tpkidx")
        {
            //初始化索引文件
            m_pIdxFile = new CTpkIndexFile("",iter->second.data);
        }
    }

    return m_pIdxFile != NULL;
}

DataStreamPtr CUpdatePkt::GetData(const char* fileName,int& flag,size_t& size)
{
    if (NULL == fileName || NULL == m_pIdxFile)
        return NULL;

    FILE_INDEXLIST_MAP& dataFileMap = m_pIdxFile->GetFileIndexMap();

    for (FILE_INDEXLIST_MAP::iterator iterDataFile = dataFileMap.begin(); iterDataFile != dataFileMap.end(); iterDataFile++)
    {
        stIndexItem itemInfo;

        if (m_pIdxFile->GetDataStream(iterDataFile->first.c_str(),fileName,itemInfo))
        {
            //查找数据文件
            FILES_MAP::iterator iter = m_fileItems.find(iterDataFile->first);

            if (iter == m_fileItems.end())
                return NULL;

            FileItem& fileItem = iter->second;
            fileItem.data->Seek(0);
            fileItem.data->Seek(itemInfo.idx.offset);

            CTpkItem item;
            item.Read(fileItem.data);

            flag = item.flag;

            char* pBuf = new char[itemInfo.idx.cmpSize];
            fileItem.data->Read(pBuf,itemInfo.idx.cmpSize);

            DataStreamPtr rslt = new CMemDataStream(pBuf,itemInfo.idx.cmpSize);
            rslt->Seek(itemInfo.idx.cmpSize);

            size = itemInfo.idx.size;

            return rslt;
        }
    }

    return NULL;
}

bool CUpdatePkt::Write(DataStreamPtr& stream)
{
    int flag = UPDATEPKTFLAG;
    if (0 ==stream->Write(flag))
        return false;

    if (!stream->WriteString(m_versionFrom))
        return false;

    if (!stream->WriteString(m_versionTo))
        return false;

    if (!stream->WriteString(m_describe))
        return false;

    if (0 ==stream->Write(m_type))
        return false;

    return true;
}

std::vector<std::string>& CUpdatePkt::GetIdxFiles(void)
{
    return m_idxFiles;
}