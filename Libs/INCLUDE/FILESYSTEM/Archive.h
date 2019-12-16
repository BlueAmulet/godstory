#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "DataStream.h"
#include "Factory.h"

class IArchive;

struct stFileInfo
{
    stFileInfo(void): pArchive(NULL), crc(0),size(0) {}

    IArchive*   pArchive;  //���淽��������
    std::string fullName;  //ȫ·��
    std::string path;      //����Ŀ¼
    std::string fileName;  //�ļ���
    unsigned long crc;     //�ļ���CRCֵ
    size_t      size;      //�ļ��ĳ���
};

/************************************************************************/
/* �ļ�����,������ʾ1���ļ��л�1��ZIP�ļ�,�������Զ���ѹ���ļ�          */
/************************************************************************/
class IArchive
{
public:
    IArchive(std::string path,std::string type):
      m_path(path),m_type(type) {}

    virtual ~IArchive(void) {}

    const std::string& GetPath(void) const {return m_path;}

    const std::string& GetType(void) const {return m_type;}

    //virtual methods
    //�����Ӧ���ĵ�,�����zip�ļ�
    virtual bool OnLoad(void) {return true;}

    virtual bool OnLoad(DataStreamPtr stream) {return true;}

    //�ر��ĵ�����,eg:�ر�zip�ļ�
    virtual void OnUnload(void) {}

    //���ļ�:
    virtual DataStreamPtr Open(std::string fileName) = 0;

    //�����ļ���Ϣ
    virtual bool FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive) = 0;
protected:
    std::string m_path; //��ǰarchive������ļ�·�� eg ..data
    std::string m_type; //�ĵ�����
};

/************************************************************************/
/* archive������,archive��archive manager�ڲ�������                           */
/************************************************************************/
class CArchiveFactory:
    public IFactory<IArchive>
{
    friend IArchive;
};

#endif /*_ARCHIVE_H_*/