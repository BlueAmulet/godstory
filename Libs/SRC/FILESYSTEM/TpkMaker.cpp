#include "TpkMaker.h"
#include <assert.h>
#include "ArchiveMgr.h"
#include "FileArchive.h"
#include "TpkStruct.h"
#include "FileDataStream.h"
#include "TpkVersion.h"
#include "FSUtils.h"

#include "StreamCompressor.h"
#include "StreamUncompressor.h"

//定义每次从文件读入的最大数据
#define MAX_READ_ONETIME 1024

//定义最小细分大小
#define SPLIT_MIN 1024 * 1024 * 5 //5M [1M->无穷]

CTpkMaker::CTpkMaker(void):
    m_pIdxFile(NULL)
{
}

CTpkMaker::~CTpkMaker(void)
{
    if (NULL != m_pIdxFile)
        delete m_pIdxFile;

    m_pIdxFile = NULL;

    for (size_t i = 0; i < m_dataFiles.size(); i++)
    {
        delete m_dataFiles[i];
    }

    m_dataFiles.clear();
}

bool CTpkMaker::__HasDataFile(const char* pDataFile)
{
    std::string dataFilePath;
    std::string dataFileName;
    GetPathAndName(pDataFile,dataFilePath,dataFileName);

    for (size_t i = 0; i < m_dataFiles.size(); i++)
    {
        RPC_STATUS status;

        if (stricmp(m_dataFiles[i]->GetFileName().c_str(),dataFileName.c_str()) == 0)
        {
            return true;
        }
    }

    return false;
}

/************************************************************************/
/* 往索引里增加数据文件
/************************************************************************/    
bool CTpkMaker::AddDataFile(const char* dataFilePath,bool isNewAdded,bool isCmpUid)
{
    if (NULL == dataFilePath || NULL == m_pIdxFile)
        return false;

    EnsureFloder(dataFilePath);

    //打开数据文件
    if (__HasDataFile(dataFilePath))
        return true;

    //打开数据文件
    std::fstream* fs = new std::fstream;

    if (!isNewAdded)
        fs->open(dataFilePath,std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    else
        fs->open(dataFilePath,std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
 
    if (isNewAdded && !fs->is_open())
    {
        fs->clear();
        fs->open(dataFilePath,std::ios_base::out | std::ios_base::binary);
        fs->close();
        fs->clear();
        fs->open(dataFilePath,std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    }

    if (!fs->is_open())
    {
        delete fs;
        return false;
    }

    DataStreamPtr stream = new CFileDataStream(fs);

    if (isNewAdded)
    {
        //写入文件头
        CTpkHeader tpkHeader;

        tpkHeader.flag     = TPK_FILE;
        tpkHeader.version  = TPK_VERSION;
        tpkHeader.id       = m_pIdxFile->GetUniqueID();

        if (!tpkHeader.Write(stream))
        {
            return false;
        }
    }

    CTpkDataFile* pDataFile = new CTpkDataFile(dataFilePath,stream);

    if (!pDataFile->IsValid())
    {
        delete pDataFile;
        return false;
    }

    if (!isNewAdded)
    {
        //检测index文件是否有这个数据
        if (!m_pIdxFile->HasDataFile(pDataFile->GetFileName().c_str()))
        {
            delete pDataFile;
            return false;
        }

        //检测UID是否相同
        RPC_STATUS status;
        if (isCmpUid && !UuidEqual(&m_pIdxFile->GetUniqueID(),&pDataFile->GetUniqueID(),&status))
        {
            delete pDataFile;
            return false;
        }
    }

    //设置为当前的数据操作文件
    m_curDataFile = pDataFile->GetFileName();
    m_dataFiles.push_back(pDataFile);

    //增加到索引文件
    m_pIdxFile->AddDataFile(pDataFile->GetFileName().c_str());
    return true;
}

bool CTpkMaker::HasDataFile(const char* dataFile)
{
    return __HasDataFile(dataFile);
}

/************************************************************************/
/* 打开索引文件
/************************************************************************/
bool CTpkMaker::OpenIndexFile(const char* indexFilePath)
{
    if (NULL != m_pIdxFile || NULL == indexFilePath)
    {
        assert(0 && "多次打开索引文件");
        return false;
    }

    m_pIdxFile = new CTpkIndexFile(indexFilePath);

    if (!m_pIdxFile->IsValid())
    {
        delete m_pIdxFile;
        m_pIdxFile = NULL;
        return false;
    }

    //数据文件默认从索引文件的目录开始查找
    std::string dataFilePath;
    std::string tmp;

    GetPathAndName(indexFilePath,dataFilePath,tmp);

    //读取当前所有的数据文件名称
    FILE_INDEXLIST_MAP& dataFileMap = m_pIdxFile->GetFileIndexMap();

    for (FILE_INDEXLIST_MAP::iterator iter = dataFileMap.begin(); iter != dataFileMap.end(); iter++)
    {   
        if (!__HasDataFile(iter->first.c_str()))
        {
            tmp = dataFilePath + iter->first;

            while (!is_exist(tmp.c_str()))
            {
                //提示文件选择数据文件所在的目录
                tmp = GetSelectedFolder(("请选择" + iter->first + "的目录").c_str());
                tmp += "\\";
                tmp += iter->first;
            }

            AddDataFile(tmp.c_str(),false);
        }
    }

    return NULL != m_pIdxFile;
}

bool CTpkMaker::OpenIndexFile(const char* indexFilePath,DataStreamPtr stream)
{
    if (NULL != m_pIdxFile)
    {
        assert(0 && "多次打开索引文件");
        return false;
    }

    m_pIdxFile = new CTpkIndexFile(indexFilePath,stream);

    if (!m_pIdxFile->IsValid())
    {
        delete m_pIdxFile;
        m_pIdxFile = NULL;
        return false;
    }

    return true;
}

/************************************************************************/
/* 添加索引文件
   @param idxName stram在pack文件里的索引名称
   */
/************************************************************************/
bool CTpkMaker::AddFile(const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod)
{
    if ("" == m_curDataFile || m_pIdxFile == NULL)
        return false;

    //查找当前的数据文件
    size_t i = 0;

    for (i = 0; i < m_dataFiles.size(); i++)
    {
        if (m_dataFiles[i]->GetFileName() == m_curDataFile)
            break;
    }

    if (i >= m_dataFiles.size())
        return false;
    
    return m_pIdxFile->AddFile(m_curDataFile.c_str(),m_dataFiles[i]->GetSize(),m_dataFiles[i]->GetAppendSize(),idxName,stream,cmpMethod,false,m_dataFiles[i]->GetSize());
}

bool CTpkMaker::AddCompressedFile(const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod,size_t fileSize)
{
    if ("" == m_curDataFile || m_pIdxFile == NULL)
        return false;

    //查找当前的数据文件
    size_t i = 0;

    for (i = 0; i < m_dataFiles.size(); i++)
    {
        if (m_dataFiles[i]->GetFileName() == m_curDataFile)
            break;
    }

    if (i >= m_dataFiles.size())
        return false;

    return m_pIdxFile->AddFile(m_curDataFile.c_str(),m_dataFiles[i]->GetSize(),m_dataFiles[i]->GetAppendSize(),idxName,stream,cmpMethod,true,fileSize);
}

/************************************************************************/
/* 保存数据
/************************************************************************/
bool CTpkMaker::Save(const char* savePath,bool isSaveAs)
{
    if (NULL == m_pIdxFile)
        return true;

    //先保存索引文件
    if (!m_pIdxFile->SaveIndex(savePath))
        return false;

    //依次保存数据文件
    std::string path;
    std::string fileName;

    GetPathAndName(savePath,path,fileName);

    for (size_t i = 0; i < m_dataFiles.size(); i++)
    {
        if(path[path.length() -1] != '/' && path[path.length() -1] != '\\')
            path += '/';

        std::string tmpPath = path + m_dataFiles[i]->GetFileName();

        {
            DataStreamPtr stream = m_dataFiles[i]->GetDataFileStream();

            if (!stream)
                return false;

            stream->Seek(0);
            //保存当前的数据
            CTpkHeader tpkHeader;
            tpkHeader.flag     = TPK_FILE;
            tpkHeader.version  = TPK_VERSION;
            tpkHeader.id       = m_pIdxFile->GetUniqueID();

            if (!tpkHeader.Write(stream))
                return false;

            FILE_INDEXLIST_MAP& filemap = m_pIdxFile->GetFileIndexMap();
            INDEXLIST_MAP& idxMap = filemap.find(m_dataFiles[i]->GetFileName())->second;

            std::vector<stIndexItem> savedItems;

            for (INDEXLIST_MAP::iterator iter = idxMap.begin(); iter != idxMap.end(); iter++)
            {
                stIndexItem& idxItem = iter->second;

                //判断是否有数据需要写入
                if (idxItem.data)
                {
                    savedItems.push_back(idxItem);
                }
            }

            //对要保存的数据按偏移进行排序
            std::sort(savedItems.begin(),savedItems.end());

            for (size_t iSavedItem = 0; iSavedItem < savedItems.size(); iSavedItem++)
            {
                stIndexItem& idxItem = savedItems[iSavedItem];

                CTpkItem dataItem;
                dataItem.crc  = 0;

                if (idxItem.cmpMethod == E_COMPRESS_NONE)
                {
                    dataItem.flag = BIT(0);
                }
                else if (idxItem.cmpMethod == E_COMPRESS_ZLIB)
                {
                    dataItem.flag = BIT(1);
                }

                stream->Seek(0);
                stream->Seek(idxItem.idx.offset);

                if (!dataItem.Write(stream))
                    return false;

                //写入stream数据
                size_t size = idxItem.data->Size();

                if (0 != size)
                { 
                    idxItem.data->Seek(0);
                    char* pBuf = new char[size];
                    idxItem.data->Read(pBuf,size);

                    if (!stream->Write(pBuf,size))
                        return false;

                    delete []pBuf;
                }

                idxItem.data = NULL;
            }

            stream->Flush();
        }

        m_dataFiles[i]->Reset();

        if (isSaveAs && tmpPath != m_dataFiles[i]->GetFilePath())
            ::CopyFile(m_dataFiles[i]->GetFilePath().c_str(),tmpPath.c_str(),false);
    }

    return true;
}

bool CTpkMaker::SetCurDataFile(const char* dataFileName)
{
    for (size_t i = 0; i < m_dataFiles.size(); i++)
    {
        if (m_dataFiles[i]->GetFileName() == dataFileName)
        {
            m_curDataFile = dataFileName;
            return true;
        }
    }

    return false;
}

bool CTpkMaker::HasIdxItem(const char* idxItemName)
{
    if (NULL == m_pIdxFile)
        return false;

    return m_pIdxFile->HasIdxItem(m_curDataFile.c_str(),idxItemName);
}

/************************************************************************/
/* 查找所有的数据文件，找到并返回数据文件
/************************************************************************/
bool CTpkMaker::HasIdxItem(const char* idxItemName,std::string& dataFileName)
{
    if (NULL == m_pIdxFile)
        return false;

    FILE_INDEXLIST_MAP& fileMap = m_pIdxFile->GetFileIndexMap();

    for (FILE_INDEXLIST_MAP::iterator iter = fileMap.begin(); iter != fileMap.end(); iter++)
    {
        if (m_pIdxFile->HasIdxItem(iter->first.c_str(),idxItemName))
        {
            dataFileName = iter->first;
            return true;
        }
    }

    return false;
}

/************************************************************************/
/* 获取索引文件的具体信息
/************************************************************************/
bool CTpkMaker::GetIdxItemInfo(const char* idxItemName,stIdxInfo& info)
{
    if (NULL == m_pIdxFile)
        return false;

    std::string fileName;
    std::string filePath;
    GetPathAndName(idxItemName,filePath,fileName);
    info.fileName = fileName;
    info.idxPath  = filePath;

    FILE_INDEXLIST_MAP& fileMap       = m_pIdxFile->GetFileIndexMap();
    FILE_INDEXLIST_MAP::iterator iter = fileMap.find(m_curDataFile.c_str());

    if (iter == fileMap.end())
        return false;

    INDEXLIST_MAP::iterator iterIdx = iter->second.find(idxItemName);

    if (iterIdx == iter->second.end())
        return false;

    stIndexItem& item = iterIdx->second;

    info.cmpSize = item.idx.cmpSize;
    info.size    = item.idx.size;

    if (item.data)
    {
        //新添加的数据，读取保存的数据
    }
    else
    {
        //分析数据文件
    }

    return true;
}

/************************************************************************/
/* 获取数据流，从数据文件读取或从待添加的所引项中读取
/************************************************************************/
DataStreamPtr CTpkMaker::GetDataStream(const char* idxName)
{
    if (NULL == idxName || NULL == m_pIdxFile || "" == m_curDataFile)
        return NULL;
    
    stIndexItem itemInfo;
    if (!m_pIdxFile->GetDataStream(m_curDataFile.c_str(),idxName,itemInfo))
        return NULL;

    DataStreamPtr data = itemInfo.data;

    bool isCmp = itemInfo.cmpMethod != E_COMPRESS_NONE;

    if (!data)
    {
        //从数据文件里查找
        size_t i = 0;

        for (i = 0; i < m_dataFiles.size(); i++)
        {
            if (m_dataFiles[i]->GetFileName() == m_curDataFile)
                break;
        }

        if (i >= m_dataFiles.size())
            return NULL;

        data = m_dataFiles[i]->GetItemData(itemInfo.idx.offset,itemInfo.idx.cmpSize,isCmp);

        if (!data)
            return NULL;
    }

    if (isCmp && itemInfo.idx.cmpSize > 0)
    {
        //对数据进行解压缩
        return CStreamUncompressor::Uncompress(data,itemInfo.idx.size,E_COMPRESS_ZLIB);
    }
    else
        return data;
}

/************************************************************************/
/*删除数据包 
/************************************************************************/
bool CTpkMaker::DeleteDataFile(const char* dataFileName)
{
    if(NULL == dataFileName)
        return false;

    for (std::vector<CTpkDataFile*>::iterator iter = m_dataFiles.begin(); iter != m_dataFiles.end(); iter++)
    {
        if ((*iter)->GetFileName() == dataFileName)
        {
            delete (*iter);
            m_dataFiles.erase(iter);
            break;
        }
    }

    m_curDataFile = "";
    //删除所引文件中的内容
    return m_pIdxFile->DeleteDataFile(dataFileName);
}

/************************************************************************/
/* 删除当前数据文件下的文件
/************************************************************************/
bool CTpkMaker::DeleteFile(const char* fileName)
{
    if(NULL == fileName || "" == m_curDataFile)
        return false;

    size_t i = 0;

    for (i = 0; i < m_dataFiles.size(); i++)
    {
        if (m_dataFiles[i]->GetFileName() == m_curDataFile)
            break;
    }

    if (i >= m_dataFiles.size())
        return false;

    return m_pIdxFile->DeleteFile(m_curDataFile.c_str(),fileName,m_dataFiles[i]->GetSize(),m_dataFiles[i]->GetAppendSize());
}

/************************************************************************/
/* 合并索引文件到当前文件
/************************************************************************/
bool CTpkMaker::MergeIdxFile(const char* idxFileName)
{
    if (NULL == m_pIdxFile)
        return false;

    CTpkIndexFile idxFile(idxFileName);
    
    //验证是否可以添加
    FILE_INDEXLIST_MAP& addedFileIdxMap = idxFile.GetFileIndexMap();

    for(FILE_INDEXLIST_MAP::iterator iterFile = addedFileIdxMap.begin(); iterFile != addedFileIdxMap.end(); iterFile++)
    {
        if (m_pIdxFile->HasDataFile(iterFile->first.c_str()))
            return false;

        INDEXLIST_MAP& addedIdxMap = iterFile->second;

        for (INDEXLIST_MAP::iterator iterIdx = addedIdxMap.begin(); iterIdx != addedIdxMap.end(); iterIdx++)
        {
            if (m_pIdxFile->HasIdxItem(iterFile->first.c_str(),iterIdx->first.c_str()))
                return false;
        }
    }

    //合并当前索引
    for(FILE_INDEXLIST_MAP::iterator iterFile = addedFileIdxMap.begin(); iterFile != addedFileIdxMap.end(); iterFile++)
    {
        m_pIdxFile->AddDataFile(iterFile->first.c_str());

        INDEXLIST_MAP& addedIdxMap = iterFile->second;

        for (INDEXLIST_MAP::iterator iterIdx = addedIdxMap.begin(); iterIdx != addedIdxMap.end(); iterIdx++)
        {
            if (!m_pIdxFile->AddFile(iterFile->first.c_str(),iterIdx->first.c_str(),iterIdx->second))
                return false;
        }
    }

    //把数据文件保存到maker里
    for(FILE_INDEXLIST_MAP::iterator iterFile = addedFileIdxMap.begin(); iterFile != addedFileIdxMap.end(); iterFile++)
    {
        AddDataFile(iterFile->first.c_str(),false,false);
    }
    
    return true;
}