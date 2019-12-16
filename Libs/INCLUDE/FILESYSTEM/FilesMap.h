#ifndef _FILESMAP_H_
#define _FILESMAP_H_

#include <string>
#include <hash_map>

#include "Loki\\singleton.h"

class IArchive;

/************************************************************************/
/* 建立查找文件目录和文件与对应容器的映射,可以方便资源的查找            */
/************************************************************************/
class CFilesMap
{
public:
    typedef stdext::hash_map<std::string,IArchive*> FILEMAP;

    //增加文档所在的位置，并建立文档下文件和文档的映射关系
    void AddArchiveLocation(std::string path,std::string type);

    void RemoveAll(void);

    //根据文件名找到相应的容器
    IArchive* Find(std::string fileName);

    //获取当前文件的数目
    size_t GetCount(void) {return m_filesmap.size();}

    FILEMAP& GetFiles(void) {return m_filesmap;}
private:
    
    FILEMAP m_filesmap;
};

typedef Loki::SingletonHolder<CFilesMap> ResFileMap;

#endif /*_FILESMAP_H_*/