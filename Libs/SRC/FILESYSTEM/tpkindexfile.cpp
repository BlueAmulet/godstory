#include "TpkIndexFile.h"
#include <assert.h>
#include <fstream>

#include "FileDataStream.h"
#include "FSUtils.h"
#include "MemDataStream.h"

//������Сϸ�ִ�С
#define SPLIT_MIN 0 //5M [1M->����]

CTpkIndexFile::CTpkIndexFile(const char* fileName):
    m_fileName(fileName),
    m_isValid(true)
{
    //������������ļ�,��򿪲���ʼ����������
    std::fstream* fs = new std::fstream;
    fs->open(fileName,std::ios_base::in | std::ios_base::binary);

    if (fs->is_open())
    {
        DataStreamPtr idxStream = new CFileDataStream(fs);

        if (!__InitIndex(idxStream))
        {
            m_isValid = false;
            assert(0 && "�����ļ���ȡ����,�����ļ���");
            return;
        }
    }
    else
    {
        //Ϊ��ǰ����������1��ΨһID
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
        assert(0 && "�����ļ���ȡ����,�����ļ���");
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
/* ɾ�������ļ��µ��ļ�
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

    //�ж����Ѿ����ڵ��ļ����Ǵ���ӵ�
    if (item.data)
    {
        idxMap.erase(iter);

        curAppendSize = 0;
        //���¼���ʣ�������ļ���ƫ��
        for (INDEXLIST_MAP::iterator iterFiles = idxMap.begin(); iterFiles != idxMap.end(); iterFiles++)
        {
            stIndexItem& tmp = iterFiles->second;

            tmp.idx.offset = dataFileSize + curAppendSize;
            curAppendSize  += tmp.idx.itemSize;
        }
    }
    else
    {
        //����ļ��Ѿ��������ļ��ɾ�������䵽���п�
        CFreeItemEx freeItem;
        freeItem.offset     = item.idx.offset;
        freeItem.itemSize   = item.idx.itemSize;
        freeItem.isCurAdded = false;

        __MergeFreeItems(dataFileName,freeItem);

        FREEITEM_MAP::iterator iterFreeItem = m_freeItemMap.find(dataFileName);

        if (iterFreeItem == m_freeItemMap.end())
            iterFreeItem = m_freeItemMap.insert(std::make_pair(dataFileName,std::vector<CFreeItemEx>())).first;

        iterFreeItem->second.push_back(freeItem);

        //ɾ����ǰ��
        idxMap.erase(iter);
    }

    return true;
}

/************************************************************************/
/* �����г�ʼ��������Ϣ
/************************************************************************/
bool CTpkIndexFile::__InitIndex(DataStreamPtr stream)
{
    stream->Seek(0);

    m_fileIdxListMap.clear();

    CIndexHeader idxHeader; //�����ļ�ͷ

    if (!idxHeader.Read(stream))
        return false;

    m_uid = idxHeader.id;

    //ѭ������ÿ��file�ļ�
    for (size_t iFile = 0; iFile < idxHeader.nFiles; iFile++)
    {
        CIndexFile idxFileHeader;

        if (!idxFileHeader.Read(stream))
            return false;

        INDEXLIST_MAP& idxMap = __GetIdxMap(idxFileHeader.name.c_str());

        //��ȡ���ļ������е������ļ�
        for (size_t i = 0; i < idxFileHeader.count; i++)
        {
            stIndexItem idxItem;

            if (!idxItem.idx.Read(stream))
                return false;

            idxItem.dataFile = idxFileHeader.name;

            //�ѵ�ǰ���������뵽hash�б�����
            idxMap.insert(std::make_pair(idxItem.idx.name,idxItem));
        }

        FREEITEM_MAP::iterator iterFreeItem = m_freeItemMap.insert(std::make_pair(idxFileHeader.name,std::vector<CFreeItemEx>())).first;

        //��ȡ���ļ������п��п�
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
/* ��������ļ�
   @param idxName stram��pack�ļ������������
/************************************************************************/
bool CTpkIndexFile::AddFile(const char* dataFileName,size_t dataFileSize,size_t& curAppendSize,const char* idxName,DataStreamPtr stream,ECompressMethod cmpMethod,bool isCompressed,size_t fileSize)
{
    INDEXLIST_MAP& idxMap = __GetIdxMap(dataFileName);

    //�鿴�����������ļ��Ƿ������������
    if (!__IsIdxNameUnique(dataFileName,idxName))
    {
        assert(0 && "��ӵ��ļ����������������ļ���ͻ���ļ����ص�");
        return false;
    }
    
    //�鿴�Ƿ��Ѿ������������
    INDEXLIST_MAP::iterator iter = idxMap.find(idxName);

    bool isModify = (iter != idxMap.end()) ? true : false;

    DataStreamPtr cmpStream = NULL;

    if (cmpMethod != E_COMPRESS_NONE && !isCompressed)
    {
        //ѹ������,��ȡѹ����Ĵ�С
        cmpStream = CStreamCompressor::Compress(stream,cmpMethod);
    }
    else
    {
        //��ѹ������,��ȡ�ļ����ݵ��ڴ�
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

    //�����п����б�������Ƿ������㵱ǰ��С�Ŀ�
    std::vector<CFreeItemEx>::iterator iterFinded; 
    stIndexItem idxItem;

    idxItem.cmpMethod = cmpMethod;

    if (__FindFreeItem(dataFileName,addedItemSize,iterFinded))
    {
        //�ҵ������ITEM
        CFreeItemEx& freeItem = *iterFinded;

        idxItem.dataFile = dataFileName;

        idxItem.idx.cmpSize    = cmpStream->Size();
        idxItem.idx.name       = idxName;
        idxItem.idx.offset     = freeItem.offset;
        idxItem.idx.size       = !isCompressed ? stream->Size() : fileSize;

        //�����ҵ��Ŀ�̫��.��ϸ�ֳ����е�ITEM,����ɾ����ǰ���п�
        if (freeItem.itemSize - addedItemSize > SPLIT_MIN)
        {
            idxItem.idx.itemSize = addedItemSize;

            //�޸ĵ�ǰ���п�ƫ��
            freeItem.itemSize = freeItem.itemSize - idxItem.idx.itemSize;
            freeItem.offset   = freeItem.offset + idxItem.idx.itemSize;
        }
        else
        {
            idxItem.idx.itemSize   = freeItem.itemSize;

            //ɾ�����п�
            FREEITEM_MAP::iterator iterDataFile = m_freeItemMap.find(dataFileName);
            iterDataFile->second.erase(iterFinded);
        }
    }
    else
    {
        //�������ļ�ĩβ׷��
        idxItem.dataFile       = dataFileName;
        idxItem.idx.cmpSize    = cmpStream->Size();
        idxItem.idx.name       = idxName;
        idxItem.idx.offset     = dataFileSize + curAppendSize;
        idxItem.idx.size       = !isCompressed ? stream->Size() : fileSize;
        idxItem.idx.itemSize   = addedItemSize;

        curAppendSize += addedItemSize;
    }

    idxItem.data = cmpStream;
    
    //�������޸��ļ�,�����ԭ���ļ��Ŀռ�,��ӵ������б�
    if (!isModify)
    {
        idxMap.insert(std::make_pair(idxName,idxItem));
        return true;
    }
    
    //�ϲ��������еĿ��п�
    CFreeItemEx freeItem;

    freeItem.offset     = iter->second.idx.offset;
    freeItem.itemSize   = iter->second.idx.itemSize;
    freeItem.isCurAdded = false;

    __MergeFreeItems(dataFileName,freeItem);

    FREEITEM_MAP::iterator iterDataFile = m_freeItemMap.find(dataFileName);

    if (iterDataFile == m_freeItemMap.end())
        iterDataFile = m_freeItemMap.insert(std::make_pair(dataFileName,std::vector<CFreeItemEx>())).first;

    iterDataFile->second.push_back(freeItem);

    //�����µ�����
    idxMap.erase(iter);
    idxMap.insert(std::make_pair(idxName,idxItem));

    return true;
}

/************************************************************************/
/* ��ȡdataFileName�����ļ��µ���������
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
/* ���Ҫ��ӵ��������Ƿ�Ψһ����ֹ�����ص��������Դ���ʲ���
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
/* ���ҵ�ǰ�ļ����Ƿ���ڿ��п�
/************************************************************************/
bool CTpkIndexFile::__FindFreeItem(const char* dataFileName,size_t size,std::vector<CFreeItemEx>::iterator& iter)
{
    FREEITEM_MAP::iterator iterFreeItem = m_freeItemMap.find(dataFileName);

    if (iterFreeItem == m_freeItemMap.end())
        return false;

    for (std::vector<CFreeItemEx>::iterator iterFind = iterFreeItem->second.begin(); iterFind != iterFreeItem->second.end(); iterFind++)
    {
        //����С�Ƿ�ƥ��
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
/*�ϲ���ǰ�鼰�������е��������п�
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
/* ������������
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

    //д��ÿ���ļ�������
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

        //д���ļ�����
        for (INDEXLIST_MAP::iterator iterIdx = iterFile->second.begin(); iterIdx != iterFile->second.end(); iterIdx++)
        {
            iterIdx->second.idx.Write(idxStream);
        }

        //д����п�
        for (size_t iFreeItem = 0; iFreeItem < idxFile.nFreeItems; iFreeItem++)
        {
            iterFree->second[iFreeItem].Write(idxStream);
        }
    }

    return true;
}

/************************************************************************/
/* ��������ļ��Ƿ����
/************************************************************************/
bool CTpkIndexFile::HasDataFile(const char* dataFileName)
{
    return m_fileIdxListMap.end() != m_fileIdxListMap.find(dataFileName);
}

/************************************************************************/
/* ����������Ӧ��������Ϣ
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
/* ���������ָ�������ļ�
/************************************************************************/
bool CTpkIndexFile::AddFile(const char* dataFileName,const char* idxName,stIndexItem& item)
{
    INDEXLIST_MAP& idxMap = __GetIdxMap(dataFileName);

    return idxMap.insert(std::make_pair(idxName,item)).second;    
}