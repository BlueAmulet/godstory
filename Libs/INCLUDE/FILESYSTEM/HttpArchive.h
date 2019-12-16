#ifndef _HTTPARCHIVE_H_
#define _HTTPARCHIVE_H_

#include "Archive.h"
#include "Factory.h"
#include "DataStream.h"
#include "RyeolHttpClient.h"
#include <hash_map>
#include <string>

typedef unsigned long ulg;      // unsigned 32-bit value

/************************************************************************/
/*HTTP�ļ�����,Ϊ�Ӹ��·������ϻ�ȡ�ļ����Ƶ�                                               */
/************************************************************************/
class CHttpArchive:
    public IArchive
{
    struct stItem
    {
        std::string fileName;
        ulg         crc;
        size_t      size;
    };
public:
    CHttpArchive(std::string path,std::string type,std::string account,std::string pswd,bool isResuming);

    ~CHttpArchive(void);

    //method from IArchive
    virtual DataStreamPtr Open(std::string fileName);

    virtual bool FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive);

    virtual bool OnLoad(void);

    virtual void OnUnload(void);
private:
    //��http��ȡĪ���ļ�,����������ɺ���֤�ļ�������
    DataStreamPtr __GetFile(std::string fileName);

    bool __IsCorrectCrc(DataStreamPtr stream,const char* fileName);

    Ryeol::CHttpClient m_client; //http�ͻ���

    std::string m_account;    //���������http�ʺ�
    std::string m_pswd;       //����
    bool        m_isResuming; //�Ƿ�֧�ֶϵ�����
    int         m_curTryCount;

    typedef stdext::hash_map<std::string,stItem> ITEMS_MAP;
    ITEMS_MAP m_items;
};

/************************************************************************/
/* ����Http�ļ�����ʵ��                                                  */
/************************************************************************/

static const std::string HTTP_TYPE = "httpfiles";

class CHttpArchiveFactory:
    public CArchiveFactory
{
public:
    CHttpArchiveFactory(const char* account,const char* pswd,bool isResuming/*�Ƿ�֧�ֶϵ�����*/):
      m_account(account),m_pswd(pswd),m_isResuming(isResuming)
    {
    }

    virtual const std::string& GetType(void) const
    {
        return HTTP_TYPE;
    }

    virtual IArchive* CreateInstance( const std::string& name )
    {
        return new CHttpArchive(name,HTTP_TYPE,m_account,m_pswd,m_isResuming);
    }

    virtual void DestroyInstance(IArchive* pArchive)
    {
        delete pArchive;
    }

private:
    std::string m_account; //���������http�ʺ�
    std::string m_pswd;    //����
    bool        m_isResuming;
};


#endif /*_HTTPARCHIVE_H_*/