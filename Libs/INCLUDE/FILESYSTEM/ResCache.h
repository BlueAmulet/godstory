#ifndef _RESCACHE_H_
#define _RESCACHE_H_

#include <hash_map>
#include "loki/Functor.h"

//单线程guard
class CSingleThreadGuard {};

/************************************************************************/
/* 定义资源的加载和释放缓冲区，提供对多次加载资源的cache处理
/************************************************************************/
template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard = CSingleThreadGuard>
class CResCache
{
    struct CResElem
    {
        CResElem(void) : refCount(0) {}

        _Res res;
        int  refCount;  //资源的引用计数

        void IncRefCount(void)
        {
            refCount++;
        }

        void DecRefCount(void)
        {
            refCount--;
        }
    };

public:
    typedef Loki::Functor<void,LOKI_TYPELIST_1(_Res)> ON_RES_CLOSE; //资源析构回调

    ~CResCache(void) {}

    //从cache加载制定的资源
    _Res Load(_Id& id);

    //把资源加载到缓冲区,或递减引用计数
    bool Add2Cache(_Id& id,_Res res);

    //释放资源到缓冲区
    void Release(_Id& id,_Res res,ON_RES_CLOSE& onResClose);

    //释放所有缓存的资源
    void ForceReleaseAll(ON_RES_CLOSE& onResClose);

    //定时更新检测，推进资源的生命周期
    void Update(float delta,ON_RES_CLOSE& onResClose);

    //dump
    void Dump(void);
private:
    typedef stdext::hash_map<_Id,CResElem> CACHE_MAP;

    CACHE_MAP m_openList;       //资源的开启列表，保存大概cacheSize个资源
    CACHE_MAP m_closedList;     //资源的关系列表，保存大概cacheSize个资源
};

//
template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard>
_Res CResCache<_Id,_Res,_CacheSize,_ThreadGuard>::Load(_Id& id)
{
    _ThreadGuard guard;
    //查找是否在开启列表里
    CACHE_MAP::iterator iter = m_openList.find(id);

    if (iter != m_openList.end())
    {
        CResElem& elem = iter->second;
        //增加引用计数
        elem.IncRefCount();
        return elem.res;
    }

    //检查关闭列表
    iter = m_closedList.find(id);

    if (iter != m_closedList.end())
    {
        //删除关闭列表元素加入到开启列表
        CResElem& elem = iter->second;
        _Res rslt = elem.res;

        {
            elem.refCount = 1;
            m_openList.insert(std::make_pair(id,elem));
            m_closedList.erase(iter);
        }

        return rslt;
    }

    return NULL;
}

template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard>
bool CResCache<_Id,_Res,_CacheSize,_ThreadGuard>::Add2Cache(_Id& id,_Res res)
{
    _ThreadGuard guard;
    //假如资源已经在缓冲区里，返回失败
    CACHE_MAP::iterator iter = m_openList.find(id);

    if (iter != m_openList.end())
    {
        return false;
    }

    //资源缓冲数量已满
    if (m_openList.size() > _CacheSize)
    {
        return false;
    }

    CResElem elem;
    elem.refCount = 1;
    elem.res      = res;
    m_openList.insert(std::make_pair(id,elem));

    return true;
}

template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard>
void CResCache<_Id,_Res,_CacheSize,_ThreadGuard>::Release(_Id& id,_Res res,ON_RES_CLOSE& onResClose)
{
    _ThreadGuard guard;

    //检测是否在开启列表里
    CACHE_MAP::iterator iter = m_openList.find(id);

    if (iter == m_openList.end())
        return;

    //递减引用计数，为0时，添加到删除队列
    CResElem& elem = iter->second;
    elem.DecRefCount();

    if (elem.refCount <= 0)
    {
        if (m_closedList.size() > _CacheSize)
        {
            //暂时简单的删除begin节点
            CResElem& elem2Del = m_closedList.begin()->second;
            onResClose(elem2Del.res);
            m_closedList.erase(m_closedList.begin());
        }

        m_closedList.insert(std::make_pair(id,elem));
        m_openList.erase(iter);
    }
}

template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard>
void CResCache<_Id,_Res,_CacheSize,_ThreadGuard>::ForceReleaseAll(ON_RES_CLOSE& onResClose)
{
    _ThreadGuard guard;

    //强制删除所有文件
    for (CACHE_MAP::iterator iter = m_closedList.begin(); iter != m_closedList.end(); iter++)
    {
        onResClose(iter->second.res);
    }

    m_closedList.clear();
}

template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard>
void CResCache<_Id,_Res,_CacheSize,_ThreadGuard>::Update(float delta,ON_RES_CLOSE& onResClose)
{
    _ThreadGuard guard;

    static float _delta = 0;

    _delta += delta;

    if (_delta < 5 * 1024)
        return;

    _delta = 0;

    if (m_closedList.size() > 0)
    {
        //暂时先简单的删除begin节点
        CResElem& elem2Del = m_closedList.begin()->second;
        onResClose(elem2Del.res);
        m_closedList.erase(m_closedList.begin());
    }
}

template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard>
void CResCache<_Id,_Res,_CacheSize,_ThreadGuard>::Dump(void)
{
    for (CACHE_MAP::iterator iter = m_openList.begin(); iter != m_openList.end(); iter++)
    {
        Con::printf("开启列表中的缓冲的节点名称-%s",iter->first.c_str());
    }

    for (CACHE_MAP::iterator iter = m_closedList.begin(); iter != m_closedList.end(); iter++)
    {
        Con::printf("关闭列表中的缓冲的节点名称-%s",iter->first.c_str());
    }
}

#endif /*_RESCACHE_H_*/