#ifndef _RESCACHE_H_
#define _RESCACHE_H_

#include <hash_map>
#include "loki/Functor.h"

//���߳�guard
class CSingleThreadGuard {};

/************************************************************************/
/* ������Դ�ļ��غ��ͷŻ��������ṩ�Զ�μ�����Դ��cache����
/************************************************************************/
template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard = CSingleThreadGuard>
class CResCache
{
    struct CResElem
    {
        CResElem(void) : refCount(0) {}

        _Res res;
        int  refCount;  //��Դ�����ü���

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
    typedef Loki::Functor<void,LOKI_TYPELIST_1(_Res)> ON_RES_CLOSE; //��Դ�����ص�

    ~CResCache(void) {}

    //��cache�����ƶ�����Դ
    _Res Load(_Id& id);

    //����Դ���ص�������,��ݼ����ü���
    bool Add2Cache(_Id& id,_Res res);

    //�ͷ���Դ��������
    void Release(_Id& id,_Res res,ON_RES_CLOSE& onResClose);

    //�ͷ����л������Դ
    void ForceReleaseAll(ON_RES_CLOSE& onResClose);

    //��ʱ���¼�⣬�ƽ���Դ����������
    void Update(float delta,ON_RES_CLOSE& onResClose);

    //dump
    void Dump(void);
private:
    typedef stdext::hash_map<_Id,CResElem> CACHE_MAP;

    CACHE_MAP m_openList;       //��Դ�Ŀ����б�������cacheSize����Դ
    CACHE_MAP m_closedList;     //��Դ�Ĺ�ϵ�б�������cacheSize����Դ
};

//
template<typename _Id,typename _Res,int _CacheSize,typename _ThreadGuard>
_Res CResCache<_Id,_Res,_CacheSize,_ThreadGuard>::Load(_Id& id)
{
    _ThreadGuard guard;
    //�����Ƿ��ڿ����б���
    CACHE_MAP::iterator iter = m_openList.find(id);

    if (iter != m_openList.end())
    {
        CResElem& elem = iter->second;
        //�������ü���
        elem.IncRefCount();
        return elem.res;
    }

    //���ر��б�
    iter = m_closedList.find(id);

    if (iter != m_closedList.end())
    {
        //ɾ���ر��б�Ԫ�ؼ��뵽�����б�
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
    //������Դ�Ѿ��ڻ����������ʧ��
    CACHE_MAP::iterator iter = m_openList.find(id);

    if (iter != m_openList.end())
    {
        return false;
    }

    //��Դ������������
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

    //����Ƿ��ڿ����б���
    CACHE_MAP::iterator iter = m_openList.find(id);

    if (iter == m_openList.end())
        return;

    //�ݼ����ü�����Ϊ0ʱ����ӵ�ɾ������
    CResElem& elem = iter->second;
    elem.DecRefCount();

    if (elem.refCount <= 0)
    {
        if (m_closedList.size() > _CacheSize)
        {
            //��ʱ�򵥵�ɾ��begin�ڵ�
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

    //ǿ��ɾ�������ļ�
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
        //��ʱ�ȼ򵥵�ɾ��begin�ڵ�
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
        Con::printf("�����б��еĻ���Ľڵ�����-%s",iter->first.c_str());
    }

    for (CACHE_MAP::iterator iter = m_closedList.begin(); iter != m_closedList.end(); iter++)
    {
        Con::printf("�ر��б��еĻ���Ľڵ�����-%s",iter->first.c_str());
    }
}

#endif /*_RESCACHE_H_*/