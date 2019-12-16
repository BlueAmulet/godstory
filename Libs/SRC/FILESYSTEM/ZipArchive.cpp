#include "ZipArchive.h"
#include <fstream>
#include "FSUtils.h"
#include "FileDataStream.h"
#include "MemDataStream.h"

CZipArchive::CZipArchive(std::string path,std::string type):
    IArchive(path,type),
    m_hZip(NULL),
    m_zipTarget(NULL)
{
}

CZipArchive::~CZipArchive(void)
{
    OnUnload();
}

//method from IArchive
DataStreamPtr CZipArchive::Open(std::string fileName)
{
    if (NULL == m_hZip)
        return NULL;

    ITEMS_MAP::iterator iter = m_items.find(fileName);

    if (m_items.end() == iter)
        return NULL;

    ItemInfo& item = iter->second;

    char* pBuf = new char[item.size];
    DataStreamPtr ptr = new CMemDataStream(pBuf,item.size);

    if (!UnzipItem(m_hZip,item.idx,ZIPTARGET(ptr)))
        return NULL;

    return ptr;
}

bool CZipArchive::FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive)
{
    for (ITEMS_MAP::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
    {
        if (StringMatch(iter->first,pattern,false))
        {
            stFileInfo info;
            info.pArchive = this;
            info.fullName = iter->first;
            info.size     = iter->second.size;
            GetPathAndName(iter->first.c_str(),info.path,info.fileName);

            infos.push_back(info);
        }
    }

    return true;
}

bool CZipArchive::OnLoad(void)
{
    if (NULL != m_zipTarget)
        delete m_zipTarget;

    m_zipTarget = new ZIPTARGET(m_path.c_str());
    //打开相应的ZIP文件
    std::vector<std::string> exList;
    m_hZip = OpenZip(*m_zipTarget,"",exList);

    if (NULL == m_hZip)
        return false;

    m_items.clear();
    ZipItemInfo zipInfo;
    ItemInfo    itemInfo;

    for (int i = 0; i < GetZipItemCount(m_hZip); i++)
    {
        GetZipItem(m_hZip,i,zipInfo);   //获取zip里每个文件的文件名

        itemInfo.name = zipInfo.name;
        itemInfo.size = zipInfo.size;
        itemInfo.idx  = i;

        m_items.insert(std::make_pair(itemInfo.name,itemInfo));
    }

    return true;
}

bool CZipArchive::OnLoad(DataStreamPtr stream)
{
    if (NULL != m_zipTarget)
        delete m_zipTarget;

    m_zipTarget = new ZIPTARGET(stream);
    //打开相应的ZIP文件
    std::vector<std::string> exList;
    m_hZip = OpenZip(*m_zipTarget,"",exList);

    if (NULL == m_hZip)
        return false;

    m_items.clear();
    ZipItemInfo zipInfo;
    ItemInfo    itemInfo;

    for (int i = 0; i < GetZipItemCount(m_hZip); i++)
    {
        GetZipItem(m_hZip,i,zipInfo);   //获取zip里每个文件的文件名

        itemInfo.name = zipInfo.name;
        itemInfo.size = zipInfo.size;
        itemInfo.idx  = i;

        m_items.insert(std::make_pair(itemInfo.name,itemInfo));
    }

    return true;
}

void CZipArchive::OnUnload(void)
{
    m_items.clear();

    if (NULL != m_hZip)
        CloseZip(m_hZip);

    m_hZip = NULL;

    if (NULL != m_zipTarget)
        delete m_zipTarget;

    m_zipTarget = NULL;
}