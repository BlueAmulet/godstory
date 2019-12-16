#ifndef _FILESMAP_H_
#define _FILESMAP_H_

#include <string>
#include <hash_map>

#include "Loki\\singleton.h"

class IArchive;

/************************************************************************/
/* ���������ļ�Ŀ¼���ļ����Ӧ������ӳ��,���Է�����Դ�Ĳ���            */
/************************************************************************/
class CFilesMap
{
public:
    typedef stdext::hash_map<std::string,IArchive*> FILEMAP;

    //�����ĵ����ڵ�λ�ã��������ĵ����ļ����ĵ���ӳ���ϵ
    void AddArchiveLocation(std::string path,std::string type);

    void RemoveAll(void);

    //�����ļ����ҵ���Ӧ������
    IArchive* Find(std::string fileName);

    //��ȡ��ǰ�ļ�����Ŀ
    size_t GetCount(void) {return m_filesmap.size();}

    FILEMAP& GetFiles(void) {return m_filesmap;}
private:
    
    FILEMAP m_filesmap;
};

typedef Loki::SingletonHolder<CFilesMap> ResFileMap;

#endif /*_FILESMAP_H_*/