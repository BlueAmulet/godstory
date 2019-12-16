#include "FileArchive.h"
#include <fstream>
#include "FileDataStream.h"
#include <vector>
#include "FSUtils.h"
#include <io.h>
#include <windows.h>

CFileArchive::CFileArchive(std::string path,std::string type):
    IArchive(path,type)
{
    if (path[path.length() -1] == '/' || path[path.length() -1] == '\\')
        path[path.length() -1] = 0;

    assert(is_exist(path.c_str()));

    //防止中文路径打不开的问题
    std::locale::global(std::locale("chs"));
}

CFileArchive::~CFileArchive(void)
{

}

//method from IArchive
DataStreamPtr CFileArchive::Open(std::string fileName)
{
    std::string path = m_path + "/" + fileName;

    size_t pathLen = path.length();
   
    std::fstream* fs = new std::fstream;
    fs->open(path.c_str(),std::ios_base::in | std::ios_base::binary);

    if (!fs->is_open())
    {
        delete fs;
        return NULL;
    }

    //fstream保持同时打开的文件数目有限制，需要及时的关闭stream对象
    return new CFileDataStream(fs);
}

bool CFileArchive::FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive)
{
    //获取当前查找的目录
    size_t pos1 = pattern.rfind ('/');
    size_t pos2 = pattern.rfind ('\\');

    if (pos1 == pattern.npos || ((pos2 != pattern.npos) && (pos1 < pos2)))
        pos1 = pos2;

    std::string directory;

    if (pos1 != pattern.npos)
        directory = pattern.substr (0, pos1 + 1);

    std::string full_pattern = m_path + "/" + pattern;

    _finddata_t tagData;
    intptr_t lHandle = _findfirst(full_pattern.c_str(), &tagData);

    std::vector<std::string> dirs;  //遍历文件产生的目录列表
 
    while (lHandle != -1)
    {
        if((tagData.attrib & _A_SUBDIR) == 0 &&
           (tagData.attrib & _A_HIDDEN) == 0)
        {
            //去掉文件的只读属性
            static char filePath[MAX_PATH] = {0};

            if ("" == directory)
                sprintf(filePath, "%s\\%s", m_path.c_str(),tagData.name);
            else
                sprintf(filePath, "%s\\%s\\%s", m_path.c_str(),directory.c_str(),tagData.name);

            DWORD fileAttri = GetFileAttributes(filePath);

            if ((fileAttri & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
            {
                fileAttri &= ~FILE_ATTRIBUTE_READONLY;
                SetFileAttributes(filePath,fileAttri);
            }

            stFileInfo info;
            info.pArchive = this;
            info.fullName = directory + tagData.name;
            info.path     = directory;
            info.fileName = tagData.name;
            info.size     = GetFileSize((m_path + "/" + info.fullName).c_str());

            infos.push_back(info);
        }
        else
        {
            if (!is_reserved_dir (tagData.name) && (tagData.attrib & _A_SUBDIR))
                dirs.push_back(tagData.name);
        }

        if (-1 == _findnext( lHandle, &tagData ))
            break;
    }

    // Close if we found any files
    if(lHandle != -1)
        _findclose(lHandle);

    // Now find directories
    if (isRecursive && dirs.size() > 0)
    {
        std::string base_dir = "";

        if (!directory.empty ()) //查找匹配里自带的目录
        {
            base_dir = directory;
            // Remove the last '/'
            base_dir.erase (base_dir.length () - 1);
        }

        //获取查找的匹配规则
        std::string mask ("/");

        if (pos1 != pattern.npos)
            mask.append (pattern.substr (pos1 + 1));
        else
            mask.append (pattern);

        std::string subDir;

        for(size_t i = 0; i < dirs.size(); i++)
        {   
            subDir = (base_dir == "") ? base_dir : (base_dir + "/");
            subDir.append (dirs[i]).append (mask);

            FindFileInfo(infos,subDir,isRecursive);
        }
    }

    return true;
}