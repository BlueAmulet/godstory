#ifndef _RESMGR_H_
#define _RESMGR_H_

#include "Resource.h"
#include <string>
#include <hash_map>

/************************************************************************/
/* ����Īһ����Դ��eg:����                                              */
/***********************************************************************/
class CResMgr
{
public:
    CResMgr(std::string type);

    virtual ~CResMgr(void);

    virtual ResourcePtr DeclareResource(std::string resName,std::string grp);
    //������Դ
    virtual ResourcePtr LoadResource(std::string resName,std::string grp);

    virtual void UnloadResource(std::string resName);

    std::string GetType(void) {return m_type;}

    ResourcePtr GetRes(const std::string& resName);
private:
    RESHANDLE __AllocHandle(void);

    virtual ResourcePtr __CreateResImpl(std::string resName,std::string grp,RESHANDLE handle); //���������������ṩ�Լ�����Դ����

    typedef stdext::hash_map<std::string,ResourcePtr> RES_MAP;

    static RESHANDLE  m_resHandle; //����������Ψһ����ԴID
    std::string       m_type;      //��ǰ�������������Դ����
    RES_MAP           m_resources;
};

#endif /*_RESMGR_H_*/