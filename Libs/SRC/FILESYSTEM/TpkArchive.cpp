#include "TpkArchive.h"
#include <fstream>
#include "FileDataStream.h"
#include "FSUtils.h"
#include "MemDataStream.h"
#include "ZipArchive.h"
#include "ArchiveMgr.h"

CTpkArchive::CTpkArchive(std::string path,std::string type):
    IArchive(path,type)
{
    ArchiveMgr::Instance().AddFactory(new CZipArchiveFactory); //也许tpk里面包含了ZIP文件
}

CTpkArchive::~CTpkArchive(void)
{
    OnUnload();
}

//method from IArchive
DataStreamPtr CTpkArchive::Open(std::string fileName)
{
    /*ITEMS_MAP::iterator iter = m_items.find(fileName);

    if (m_items.end() == iter)
        return NULL;

    CTpkItem& item = iter->second;

    m_fstream->Seek(0);
    m_fstream->Seek(item.offset);

    char* pData = new char[item.size];
    m_fstream->Read(pData,item.size);

    return new CMemDataStream(pData,item.size);*/

    return NULL;
}

bool CTpkArchive::FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive)
{
    //for (ITEMS_MAP::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
    //{
    //    if (stricmp(".zip",GetFileExt(iter->first).c_str()) == 0)
    //    {
    //        //假如是ZIP文件，则往ZIP文件里面继续查找]
    //        DataStreamPtr stream = Open(iter->first);

    //        if (stream)
    //        {
    //            IArchive* pArchive = ArchiveMgr::Instance().Load(stream,iter->first,"zipfiles");

    //            if (pArchive)
    //                pArchive->FindFileInfo(infos,pattern,isRecursive);
    //        }
    //    }
    //    else if (StringMatch(iter->first,pattern,false))
    //    {
    //        stFileInfo info;
    //        info.pArchive = this;
    //        info.fullName = iter->first;
    //        info.size     = iter->second.size;
    //        GetPathAndName(iter->first,info.path,info.fileName);

    //        infos.push_back(info);
    //    }
    //}

    return true;
}

bool CTpkArchive::OnLoad(void)
{
    //载入文件数据
    //std::fstream* fs = new std::fstream;
    //fs->open(m_path.c_str(),std::ios_base::in | std::ios_base::binary);

    //if (!fs->is_open())
    //    return false;

    //m_fstream = new CFileDataStream(fs);

    ////读取TPK头
    //if (!m_header.Read(m_fstream))
    //    return false;

    //m_items.clear();
    //CTpkItem item;

    //for (size_t i = 0; i < m_header.nFiles; i++)
    //{
    //    if (!item.Read(m_fstream))
    //        return false;

    //    if (item.isEnabled) //假如文件未被排除
    //        m_items.insert(std::make_pair(item.name,item));
    //}

    return true;
}

bool CTpkArchive::OnLoad(DataStreamPtr stream)
{
    //m_fstream = stream;
    ////读取TPK头
    //if (!m_header.Read(m_fstream))
    //    return false;

    //m_items.clear();
    //CTpkItem item;

    //for (size_t i = 0; i < m_header.nFiles; i++)
    //{
    //    if (!item.Read(m_fstream))
    //        return false;

    //    if (item.isEnabled) //假如文件未被排除
    //        m_items.insert(std::make_pair(item.name,item));
    //}

    return true;
}

void CTpkArchive::OnUnload(void)
{
    m_items.clear();
    //m_fstream = NULL;
}