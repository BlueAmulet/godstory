#include "FSUtils.h"
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>

#include <windows.h>
#include <shlobj.h>


bool StringMatch(const std::string& str, const std::string& pattern, bool caseSensitive)
{
    std::string tmpStr = str;
    std::string tmpPattern = pattern;

    if (!caseSensitive)
    {
        ToLowerCase(tmpStr);
        ToLowerCase(tmpPattern);
    }

    std::string::const_iterator strIt           = tmpStr.begin();
    std::string::const_iterator patIt          = tmpPattern.begin();
    std::string::const_iterator lastWildCardIt = tmpPattern.end();

    while (strIt != tmpStr.end() && patIt != tmpPattern.end())
    {
        if (*patIt == '*')
        {
            lastWildCardIt = patIt;
            // Skip over looking for next character
            ++patIt;
            if (patIt == tmpPattern.end())
            {
                // Skip right to the end since * matches the entire rest of the string
                strIt = tmpStr.end();
            }
            else
            {
                // scan until we find next pattern character
                while(strIt != tmpStr.end() && *strIt != *patIt)
                    ++strIt;
            }
        }
        else
        {
            if (*patIt != *strIt)
            {
                if (lastWildCardIt != tmpPattern.end())
                {
                    // The last wildcard can match this incorrect sequence
                    // rewind pattern to wildcard and keep searching
                    patIt = lastWildCardIt;
                    lastWildCardIt = tmpPattern.end();
                }
                else
                {
                    // no wildwards left
                    return false;
                }
            }
            else
            {
                ++patIt;
                ++strIt;
            }
        }

    }
    // If we reached the end of both the pattern and the string, we succeeded
    if (patIt == tmpPattern.end() && strIt == tmpStr.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool is_reserved_dir(const char *fn)
{
    return (fn [0] == '.' && (fn [1] == 0 || (fn [1] == '.' && fn [2] == 0)));
}

bool is_exist(const char* filename)
{
    if (NULL == filename)
        return false;
         
    struct stat tagStat;
    bool ret = (stat(filename, &tagStat) == 0);

    return ret;
}

bool IsFloder(const char* filename)
{
    if (NULL == filename)
        return false;

    return *(std::find(filename,filename + strlen(filename),'.')) == NULL;
}

std::string& ToLowerCase(std::string& str)
{
    std::transform(str.begin(),str.end(),str.begin(),tolower);

    return str;
}

void GetPathAndName(const char*  fullName,std::string& path,std::string& fileName)
{
    static char strDiv[256]      = {0};
    static char strPath[256]     = {0};
    static char strFileName[256] = {0};
    static char strExt[256]      = {0};

    _splitpath(fullName,strDiv,strPath,strFileName,strExt);

    path     = strDiv;
    path     += strPath;
    fileName = strFileName;
    fileName += strExt;
}

std::string GetSelectedFolder(const char* desc)
{
    std::string rslt = "";

    IMalloc *imalloc;
    SHGetMalloc(&imalloc);
    BROWSEINFO bi;
    ZeroMemory(&bi,sizeof(bi));

    bi.hwndOwner = NULL;

    if (NULL == desc)
        bi.lpszTitle  = TEXT("选择目录:");
    else
        bi.lpszTitle = desc;

    bi.ulFlags   = BIF_RETURNONLYFSDIRS;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != NULL)
    {
        TCHAR szInstallPath[MAX_PATH];
        SHGetPathFromIDList(pidl, szInstallPath);

        rslt = szInstallPath;
    }

    imalloc->Free(pidl);
    imalloc->Release();

    return rslt;
}

std::string GetSelectedFile(void)
{
    std::string rslt = "";

    IMalloc *imalloc;
    SHGetMalloc(&imalloc);
    BROWSEINFO bi;
    ZeroMemory(&bi,sizeof(bi));

    bi.hwndOwner = NULL;
    bi.lpszTitle = TEXT("选择文件:");
    bi.ulFlags   = BIF_BROWSEINCLUDEFILES;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != NULL)
    {
        TCHAR szInstallPath[MAX_PATH];
        SHGetPathFromIDList(pidl, szInstallPath);

        rslt = szInstallPath;
    }

    imalloc->Free(pidl);
    imalloc->Release();

    return rslt;
}

std::vector<std::string> SplitString(std::string str,char splitter)
{
    std::vector<std::string> rslt;

    int start = 0;

    while(start < str.length())
    {
        int pos = str.find_first_not_of(splitter,start);

        if (pos < 0)
            return rslt;

        int end = str.find_first_of(splitter,pos);

        if (end < 0)
            end = str.length();

        std::string elem = str.substr(pos,end - pos);
        rslt.push_back(elem);
        start = end;
    }

    return rslt;
}

size_t GetFileSize(const char* file)
{
    struct stat tagStat;
    bool ret = (stat(file, &tagStat) == 0);

    if (!ret)
        return 0;
         
    return tagStat.st_size;
}

namespace
{
    void __EnsureDirectory(const char* path)
    {
        static char dir[_MAX_PATH] = {0};
        static char div[_MAX_PATH] = {0};
        _splitpath(path,dir,div,NULL,NULL);
        strcat(dir,div);

        if (0xFFFFFFFF != GetFileAttributes(path))
            return;

        for (int i = 0; i < strlen(dir); i++)
        {
            if (dir[i] == '\\' || dir[i] == '/')
            {
                char tmp = dir[i];

                dir[i] = 0;

                if (0xFFFFFFFF == GetFileAttributes(dir))
                {
                    ::CreateDirectory(dir,0);
                }

                dir[i] = tmp;
            }
        }
    }
}

#define TMP_PREFIX "tc518_tmp\\"

std::string CreateTempFile(const char* name,bool isClear)
{
    std::string rslt;

    char strTmp[MAX_PATH];
    ::GetTempPath(MAX_PATH,strTmp);

    rslt = strTmp;
    rslt += TMP_PREFIX;
    rslt += name;

    //保证路径有效
    if (IsFloder(name))
    {
        __EnsureDirectory((rslt + "\\.tmp").c_str());

        if (isClear)
            DelFileUnderDir(rslt.c_str());
    }
    else
    {
        __EnsureDirectory(rslt.c_str());

        if (isClear)
            ::DeleteFile(rslt.c_str());
    }

    return rslt;
}

std::string GetTempFloder(void)
{
    std::string rslt;

    char strTmp[MAX_PATH];
    ::GetTempPath(MAX_PATH,strTmp);

    rslt = strTmp;
    return rslt;
}

bool DelDir(const char* dir)
{
    if (NULL == dir)
        return true;

    char szSearch[1024];
    WIN32_FIND_DATA findData;
    sprintf(szSearch, "%s\\*",dir);
    static DWORD dwPos = 0;

    HANDLE handle = FindFirstFile(szSearch, &findData);

    if (handle == INVALID_HANDLE_VALUE)
        return true;

    char szFullPath[MAX_PATH];  //各级子目录的路径

    do
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // 确定这是一个目录
            if (findData.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE|FILE_ATTRIBUTE_SYSTEM) )
                continue;

            // 跳过. 和 .. 目录符对象
            if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
                continue;

            sprintf(szFullPath, "%s\\%s",dir,findData.cFileName);	
            DelDir(szFullPath); //继续查找子目录
        }
        else
        {
            //删除文件
            char tmp[_MAX_PATH] = {0};
            sprintf(tmp,"%s\\%s",dir,findData.cFileName);
            ::DeleteFile(tmp);
        }
    }
    while(FindNextFile(handle,&findData));
    FindClose(handle);

    //删除文件夹
    ::RemoveDirectory(dir);
    return true;
}

bool DelFileUnderDir(const char* dir)
{
    if (NULL == dir)
        return true;

    char szSearch[1024];
    WIN32_FIND_DATA findData;
    sprintf(szSearch, "%s\\*",dir);
    static DWORD dwPos = 0;

    HANDLE handle = FindFirstFile(szSearch, &findData);

    if (handle == INVALID_HANDLE_VALUE)
        return true;

    char szFullPath[MAX_PATH];  //各级子目录的路径

    do
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // 确定这是一个目录
            if (findData.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE|FILE_ATTRIBUTE_SYSTEM) )
                continue;

            // 跳过. 和 .. 目录符对象
            if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
                continue;

            sprintf(szFullPath, "%s\\%s",dir,findData.cFileName);	
            DelDir(szFullPath); //继续查找子目录
        }
        else
        {
            //删除文件
            char tmp[_MAX_PATH] = {0};
            sprintf(tmp,"%s\\%s",dir,findData.cFileName);
            ::DeleteFile(tmp);
        }
    }
    while(FindNextFile(handle,&findData));
    FindClose(handle);

    return true;
}

std::string GetFileExt(std::string fileName)
{
    int pos = fileName.find_last_of(".");

    if (pos < 0)
        return "";

    return fileName.substr(pos,fileName.length());
}

bool IsAbsolutlyPath(std::string path)
{
    return path.find(":") >= 0;
}

std::string GetFileName(const char* path)
{
    std::string rslt = path;

    int pos1 = rslt.find_last_of('/');
    int pos2 = rslt.find_last_of('\\');

    pos1 = (pos1 >= 0) ? pos1 : pos2;

    rslt = rslt.substr(pos1 + 1,rslt.length());
    return rslt;
}

bool EnsureFloder(const char* filePath)
{
    __EnsureDirectory(filePath);
    return true;
}

void ReplaceSlash(char* pBuffer,size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (pBuffer[i] == '\\')
            pBuffer[i] = '/';
    }
}

void ReplaceSlash(std::string& buffer)
{
    ReplaceSlash((char*)buffer.c_str(),buffer.length());
}