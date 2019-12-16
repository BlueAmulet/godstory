#ifndef _RESMGR_H_
#define _RESMGR_H_

#include "Resource.h"
#include <string>
#include <hash_map>

/************************************************************************/
/* 管理莫一类资源，eg:纹理                                              */
/***********************************************************************/
class CResMgr
{
public:
    CResMgr(std::string type);

    virtual ~CResMgr(void);

    virtual ResourcePtr DeclareResource(std::string resName,std::string grp);
    //载入资源
    virtual ResourcePtr LoadResource(std::string resName,std::string grp);

    virtual void UnloadResource(std::string resName);

    std::string GetType(void) {return m_type;}

    ResourcePtr GetRes(const std::string& resName);
private:
    RESHANDLE __AllocHandle(void);

    virtual ResourcePtr __CreateResImpl(std::string resName,std::string grp,RESHANDLE handle); //子类可以重载这个提供自己的资源对象

    typedef stdext::hash_map<std::string,ResourcePtr> RES_MAP;

    static RESHANDLE  m_resHandle; //递增来返回唯一的资源ID
    std::string       m_type;      //当前管理器管理的资源类型
    RES_MAP           m_resources;
};

#endif /*_RESMGR_H_*/