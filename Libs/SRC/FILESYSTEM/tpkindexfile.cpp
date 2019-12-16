#include "TpkIndexFile.h"
#include <assert.h>
#include <fstream>

#include "FileDataStream.h"
#include "FSUtils.h"
#include "MemDataStream.h"

//定义最小细分大小
#define SPLIT_MIN 0 //5M [1M->无穷]

CTpkIndexFile::CTpkIndexFile(const char* fileName):
    m_fileName(fileName),
    m_isValid(true)
{
    //假如存在索引文件,则打开并初始化索引数据
    std::fstream* fs = new std::fstream;
    fs->open(fileName,std::ios_base::in | std::ios_base::binary);

    if (fs->is_open())
    {
        DataStreamPtr idxStream = new CFileDataStream(fs);

        if (!__InitIndex(idxStream))
        {
            m_isValid = false;
            assert(0 && "索引文件读取错误,索引文件损坏");
            return;
        }
    }
    else
    {
        //为当前的索引产生1个唯一ID
        UuidCreate(&m_uid);
    }

    return;
}

CTpkIndexFile::CTpkIndexFile(const char* fileName,DataStreamPtr stream):
    m_fileName(fileName),
    m_isValid(true)
{
    if (!__InitIndex(stream))
    {
        assert(0 && "索引文件读取错误,索引文件损坏");
        m_isValid = false;
        return;
    }
}

void CTpkIndexFile::AddDataFile(const char* dataFileName)
{
    __GetIdxMap(dataFileName);
}

bool CTpkIndexFile::DeleteDataFile(const char* dataFileName)
{   
    if (!HasDataFile(dataFileName))
        return true;

    m_fileIdxListMap.erase(m_fileIdxListMap.find(dataFileName));
    m_freeItemMap.erase(m_freeItemMap.find(dataFileName));
    return true;
}

/************************************************************************/
/* 删除数据文件下的文件
/************************************************************************/
bool CTpkIndexFile::DeleteFile(const char* dataFileName,const char* fileName,size_t dataFileSize,size_t& curAppendSize)
{
    if (!HasDataFile(dataFileName))
        return true;

    INDEXLIST_MAP& idxMap = __GetIdxMap(dataFileName);
    INDEXLIST_MAP::iterator iter = idxMap.find(fileName);

    if (iter == idxMap.end())
        return true;

    stIndexItem& item = iter->second;

    //判断是已经存在的文件还是待添加的
    if (item.data)
    {
        idxMap.erase(iter);

        curAppendSize = 0;
        //重新计算剩余所有文件的偏移
        for (INDEXLIST_MAP::iterator iterFiles = idxMap.begin(); iterFiles != idxMap.end(); iterFiles++)
        {
            stIndexItem& tmp = iterFiles->second;

            tmp.idx.offset = dataFileSize + curAppendSize;
            curAppendSize  += tmp.idx.itemSize;
        }
    }
    else
    {
        //这个文件已经在数据文件里，删除该项，填充到空闲块
        CFreeItemEx freeItem;
        freeItem.offset     = item.idx.offset;
        freeItem.itemSize   = item.idx.itemSize;
        freeItem.isCurAdded = false;

        __MergeFreeItems(dataFileName,freeItem);

        FREEITEM_MAP::iterator iterFreeItem = m_freeItemMap.find(dataFileName);

        if (iterFreeItem == m_freeItemMap.end())
            iterFreeItem = m_freeItemMap.insert(std::make_pair(dataFileName,std::vector<CFreeItemEx>())).first;

        iterFreeItem->second.push_back(freeItem);

        //删除当前块
        idxMap.erase(iter);
    }

    return true;
}

/************************************************************************/
/* 从流中初始化索引信息
/************************************************************************/
bool CTpkIndexFile::__InitIndex(DataStreamPtr stream)
{
    stream->Seek(0);

    m_fileIdxListMap.clear();

    CIndexHeader idxHeader; //索引文件头

    if (!idxHeader.Read(stream))
        return false;

    m_uid = idxHeader.id;

    //循环遍历每个file文件
    for (size_t iFile = 0; iFile < idxHeader.nFiles; iFile++)
    {
        CIndexFile idxFileHeader;

        if (!idxFileHeader.Read(stream))
            return false;

        INDEXLIST_MAP& idxMap = __GetIdxMap(idxFileHeader.name.c_str());

        //获取改文件下所有的索引文件
        for (size_t i = 0; i < idxFileHeader.count; i++)
        {
            stIndexItem idxItem;

            if (!idxItem.idx.Read(stream))
                return false;

            idxItem.dataFile = idxFileHeader.name;

            //把当前的索引插入到hash列表里面
            idxMap.insert(std::make_pair(idxItem.idx.name,idxItem));
        }

        FREEITEM_MAP::iterator iterFreeItem = m_freeItemMap.insert(std::make_pair(idxFileHeader.name,std::vector<CFreeItemEx>())).first;

        //读取改文件下所有空闲块
        for (size_t i = 0; i < idxFileHeader.nFreeItems; i++)
        {
            CFreeItemEx freeItem;

            if (!freeItem.Read(stream))
                return false;

            freeItem.isCurAdded = false;

            iterFreeItem->second.push_back(freeItem);
        }
    }

    return true;
}

/************************************************************************/
/* 添加索引文件
   @param idxName stram在pack文件里的索引名称
/************************************************************************/
bool CTpkIndexFile::AddFile(const char* dataFileName,size_t dataFileSize,size_t& curAppendSize,const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod,bool isCompressed,size_t fileSize)
{
    INDEXLIST_MAP& idxMap = __GetIdxMap(dataFileName);

    //查看其他的数据文件是否有这个索引了
    if (!__IsIdxNameUnique(dataFileName,idxName))
    {
        assert(0 && "添加的文件索引与其他数据文件冲突。文件名重叠");
        return false;
    }
    
    //查看是否已经有这个索引了
    INDEXLIST_MAP::iterator iter = idxMap.find(idxName);

    bool isModify = (iter != idxMap.end()) ? true : false;

    DataStreamPtr cmpStream = NULL;

    if (cmpMethod != E_COMPRESS_NONE && !isCompressed)
    {
        //压缩数据,获取压缩后的大小
        cmpStream = CStreamCompressor::Compress(stream,cmpMethod);
    }
    else
    {
        //不压缩数据,读取文件数据到内存
        size_t memSize = stream->Size();
        char* pMem = new char[memSize];
        stream->Seek(0);
        stream->Read(pMem,memSize);
        cmpStream = new CMemDataStream(pMem,memSize);
        cmpStream->Seek(memSize);
    }

    if (!cmpStream)
        return false;

    size_t addedItemSize = cmpStream->Size() + sizeof(CTpkItem);

    //从现有空闲列表里查找是否有满足当前大小的块
    std::vector<CFreeItemEx>::iterator iterFinded; 
    stIndexItem idxItem;

    idxItem.cmpMethod = cmpMethod;

    if (__FindFreeItem(dataFileName,addedItemSize,iterFinded))
    {
        //找到空余的ITEM
        CFreeItemEx& freeItem = *iterFinded;

        idxItem.dataFile = dataFileName;

        idxItem.idx.cmpSize    = cmpStream->Size();
        idxItem.idx.name       = idxName;
        idxItem.idx.offset     = freeItem.offset;
        idxItem.idx.size       = !isCompressed ? stream->Size() : fileSize;

        //假如找到的块太大.则细分出空闲的ITEM,否则删除当前空闲块
        if (freeItem.itemSize - addedItemSize > SPLIT_MIN)
        {
            idxItem.idx.itemSize = addedItemSize;

            //修改当前空闲块偏移
            freeItem.itemSize = freeItem.itemSize - idxItem.idx.itemSize;
            freeItem.offset   = freeItem.offset + idxItem.idx.itemSize;
        }
        else
        {
            idxItem.idx.itemSize   = freeItem.itemSize;

            //删除空闲块
            FREEITEM_MAP::iterator iterDataFile = m_freeItemMap.find(dataFileName);
            iterDataFile->second.erase(iterFinded);
        }
    }
    else
    {
        //在数据文件末尾追加
        idxItem.dataFile       = dataFileName;
        idxItem.idx.cmpSize    = cmpStream->Size();
        idxItem.idx.name       = idxName;
        idxItem.idx.offset     = dataFileSize + curAppendSize;
        idxItem.idx.size       = !isCompressed ? stream->Size() : fileSize;
        idxItem.idx.itemSize   = addedItemSize;

        curAppendSize += addedItemSize;
    }

    idxItem.data = cmpStream;
    
    //假如是修改文件,则回收原来文件的空间,添加到空闲列表
    if (!isModify)
    {
        idxMap.insert(std::make_pair(idxName,idxItem));
        return true;
    }
    
    //合并后面所有的空闲块
    CFreeItemEx freeItem;

    freeItem.offset     = iter->second.idx.offset;
    freeItem.itemSize   = iter->second.idx.itemSize;
    freeItem.isCurAdded = false;

    __MergeFreeItems(dataFileName,freeItem);

    FREEITEM_MAP::iterator iterDataFile = m_freeItemMap.find(dataFileName);

    if (iterDataFile == m_freeItemMap.end())
        iterDataFile = m_freeItemMap.insert(std::make_pair(dataFileName,std::vector<CFreeItemEx>())).first;

    iterDataFile->second.push_back(freeItem);

    //插入新的数据
    idxMap.erase(iter);
    idxMap.insert(std::make_pair(idxName,idxItem));

    return true;
}

/************************************************************************/
/* 获取dataFileName数据文件下的所有索引
/************************************************************************/
INDEXLIST_MAP& CTpkIndexFile::__GetIdxMap(const char* dataFileName)
{
    FILE_INDEXLIST_MAP::iterator iter = m_fileIdxListMap.find(dataFileName);

    if (iter == m_fileIdxListMap.end())
    {
        iter = m_fileIdxListMap.insert(std::make_pair(dataFileName,INDEXLIST_MAP())).first;
    }

    return iter->second;
}

/************************************************************************/
/* 检测要添加的索引名是否唯一，防止名称重叠，造成资源访问不到
/************************************************************************/
bool CTpkIndexFile::__IsIdxNameUnique(const char* dataFileName,const char* idxName)
{
    for (FILE_INDEXLIST_MAP::iterator iter = m_fileIdxListMap.begin(); iter != m_fileIdxListMap.end(); iter++)
    {
        if (iter->first == dataFileName)
            continue;

        if (iter->second.find(idxName) != iter->second.end())
            return false;
    }
    return true;
}

/************************************************************************/
/* 查找当前文件下是否存在空闲块
/************************************************************************/
bool CTpkIndexFile::__FindFreeItem(const char* dataFileName,size_t size,std::vector<CFreeItemEx>::iterator& iter)
{
    FREEITEM_MAP::iterator iterFreeItem = m_freeItemMap.find(dataFileName);

    if (iterFreeItem == m_freeItemMap.end())
        return false;

    for (std::vector<CFreeItemEx>::iterator iterFind = iterFreeItem->second.begin(); iterFind != iterFreeItem->second.end(); iterFind++)
    {
        //看大小是否匹配
        CFreeItemEx& item = *iterFind;

        if (size <= item.itemSize && !item.isCurAdded)
        {
            iter = iterFind;
            return true;
        }
    }

    return false;
}

/************************************************************************/
/*合并当前块及后面所有的连续空闲块
/************************************************************************/
void CTpkIndexFile::__MergeFreeItems(const char* dataFileName,CFreeItemEx& item)
{
    FREEITEM_MAP::iterator iterDataFile = m_freeItemMap.find(dataFileName);

    if (iterDataFile == m_freeItemMap.end())
        return;

    size_t nextOffset = item.offset + item.itemSize;

    for (std::vector<CFreeItemEx>::iterator iter = iterDataFile->second.begin(); iter != iterDataFile->second.end(); iter++)
    {
        if ((*iter).offset == nextOffset)
        {
            item.itemSize += (*iter).itemSize;
            iterDataFile->second.erase(iter);

            return __MergeFreeItems(dataFileName,item);
        }
    }
}

/************************************************************************/
/* 保存索引数据
/************************************************************************/
bool CTpkIndexFile::SaveIndex(std::string savePath)
{
    std::fstream* fs = new std::fstream;

    if (savePath != "")
        fs->open(savePath.c_str(),std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    else
    {
        if (is_exist(m_fileName.c_str()))
        {
            fs->clear();
            fs->open(m_fileName.c_str(),std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        }
        else
            return false;
    }

    if (!fs->is_open())
        return false;

    DataStreamPtr idxStream = new CFileDataStream(fs);

    CIndexHeader idxHeader;

    idxHeader.flag    = TPK_INDEX;
    idxHeader.nFiles  = m_fileIdxListMap.size();
    idxHeader.version = TPK_INDEX_VERSION;
    idxHeader.id      = m_uid;

    idxHeader.Write(idxStream);

    //写入每个文件的数据
    for (FILE_INDEXLIST_MAP::iterator iterFile = m_fileIdxListMap.begin(); iterFile != m_fileIdxListMap.end(); iterFile++)
    {
        CIndexFile idxFile;

        idxFile.name       = iterFile->first;
        idxFile.count      = iterFile->second.size();

        FREEITEM_MAP::iterator iterFree = m_freeItemMap.find(iterFile->first);

        if (iterFree != m_freeItemMap.end())
            idxFile.nFreeItems = iterFree->second.size();
        else
            idxFile.nFreeItems = 0;

        idxFile.Write(idxStream);

        //写入文件索引
        for (INDEXLIST_MAP::iterator iterIdx = iterFile->second.begin(); iterIdx != iterFile->second.end(); iterIdx++)
        {
            iterIdx->second.idx.Write(idxStream);
        }

        //写入空闲块
        for (size_t iFreeItem = 0; iFreeItem < idxFile.nFreeItems; iFreeItem++)
        {
            iterFree->second[iFreeItem].Write(idxStream);
        }
    }

    return true;
}

/************************************************************************/
/* 检测数据文件是否存在
/************************************************************************/
bool CTpkIndexFile::HasDataFile(const char* dataFileName)
{
    return m_fileIdxListMap.end() != m_fileIdxListMap.find(dataFileName);
}

/************************************************************************/
/* 返回索引对应的数据信息
/************************************************************************/
bool CTpkIndexFile::GetDataStream(const char* dataFileName,const char* idxName,stIndexItem& itemInfo)
{
    INDEXLIST_MAP& idxMap = __GetIdxMap(dataFileName);

    INDEXLIST_MAP::iterator iter = idxMap.find(idxName);

    if (iter != idxMap.end())
    {
        itemInfo = idxMap.find(idxName)->second;
        return true;
    }
    else
        return false;
}

bool CTpkIndexFile::IsValidIndexFile(void)
{
    return is_exist(m_fileName.c_str());
}

bool CTpkIndexFile::HasIdxItem(const char* dataFileName,const char* idxItemName)
{
    if (!HasDataFile(dataFileName))
        return false;

    INDEXLIST_MAP& idxMap = m_fileIdxListMap.find(dataFileName)->second;
    
    INDEXLIST_MAP::iterator iter = idxMap.find(idxItemName);

    if(idxMap.end() != idxMap.find(idxItemName))
    {
        return true;
    }

    return false;
}

/************************************************************************/
/* 添加索引到指定数据文件
/************************************************************************/
bool CTpkIndexFile::AddFile(const char* dataFileName,const char* idxName,stIndexItem& item)
{
    INDEXLIST_MAP& idxMap = __GetIdxMap(dataFileName);

    return idxMap.insert(std::make_pair(idxName,item)).second;    
}