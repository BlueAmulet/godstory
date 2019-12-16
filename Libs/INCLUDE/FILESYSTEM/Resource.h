#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <string>

#include "Loki\SmartPtr.h"
#include "DataStream.h"

typedef unsigned int RESHANDLE;
#define INVALID_RESOUCE_HANDLE 0

class CResMgr;
 
/************************************************************************/
/* ��Դ                                                                     */
/************************************************************************/
class CResource
{
public:
    CResource(void);

    CResource(CResMgr* pResMgr,std::string resName,std::string resType,std::string grp,RESHANDLE handle);

    virtual ~CResource(void) {}

    bool IsLoaded(void) {return m_isLoaded;}

    bool IsReloadable(void) {return m_isReloadable;}

    void SetReloadable(bool isReloadable) { m_isReloadable = isReloadable;}

    //������Դ���ڴ�
    virtual bool Load(void);

    //ж����Դ
    virtual bool Unload(void);

    //��ȡ��Դ����
    DataStreamPtr GetData(void);

    const std::string& GetGroup(void) const {return m_group;}

    //��Դ�Ƿ����
    bool IsValid(void);
protected:
    std::string m_name;     //��Դ����
    std::string m_type;     //��Դ����
    std::string m_group;    //������
    RESHANDLE   m_handle;   //��Դ���

    CResMgr*    m_pResMgr;  //��Դ������

    bool m_isLoaded;        //��Դ�Ƿ��Ѿ�����
    bool m_isReloadable;    //��Դ�ɷ��μ���

    DataStreamPtr m_pRes;   //��Դ
};

typedef Loki::SmartPtr<CResource> ResourcePtr;

#endif /*_RESOURCE_H_*/