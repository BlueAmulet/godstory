#include "FilesMap.h"
#include "ArchiveMgr.h"
#include "Archive.h"
#include "FSUtils.h"

void CFilesMap::AddArchiveLocation(std::string path,std::string type)
{
    IArchive* pArchive = ArchiveMgr::Instance().Load(path,type);

    if (NULL == pArchive)
        return;

    std::vector<stFileInfo> infos;
    pArchive->FindFileInfo(infos,"*.*",true);

    //把相应的文件名和archive关连
    for (size_t i = 0; i < infos.size(); i++)
    {
        if (stricmp(".tpk",GetFileExt(infos[i].fileName).c_str()) == 0)
        {
            //假如是TPK文件，则往tpk文件里面继续查找
            IArchive* pTpkArchive = ArchiveMgr::Instance().Load(pArchive->GetPath() + "/" + infos[i].fullName,"tpkfiles");

            if (pTpkArchive)
                pTpkArchive->FindFileInfo(infos,"*.*",true);
        }

        ToLowerCase(infos[i].fullName);
        m_filesmap.insert(std::make_pair(infos[i].fullName,pArchive));
    }
}

void CFilesMap::RemoveAll(void)
{   
    m_filesmap.clear();
}

IArchive* CFilesMap::Find(std::string fileName)
{
    ToLowerCase(fileName);
    FILEMAP::iterator iter = m_filesmap.find(fileName);

    return (iter == m_filesmap.end()) ? NULL : iter->second;
}