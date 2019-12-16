#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <string>

#include "Loki\SmartPtr.h"
#include "DataStream.h"

typedef unsigned int RESHANDLE;
#define INVALID_RESOUCE_HANDLE 0

class CResMgr;
 
/************************************************************************/
/* 资源                                                                     */
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

    //加载资源到内存
    virtual bool Load(void);

    //卸载资源
    virtual bool Unload(void);

    //获取资源数据
    DataStreamPtr GetData(void);

    const std::string& GetGroup(void) const {return m_group;}

    //资源是否可用
    bool IsValid(void);
protected:
    std::string m_name;     //资源名称
    std::string m_type;     //资源类型
    std::string m_group;    //所在组
    RESHANDLE   m_handle;   //资源句柄

    CResMgr*    m_pResMgr;  //资源管理器

    bool m_isLoaded;        //资源是否已经加载
    bool m_isReloadable;    //资源可否多次加载

    DataStreamPtr m_pRes;   //资源
};

typedef Loki::SmartPtr<CResource> ResourcePtr;

#endif /*_RESOURCE_H_*/