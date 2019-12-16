#include "BackgroundLoadMgr.h"
#include "ts/tsShape.h"
#include "console/sim.h"
#include "core/resManager.h"
#include "core/bitStream.h"
#include <process.h>
#include <Windows.h>


unsigned int g_MainThreadId = 0;

CBackgroundBase::CBackgroundBase(void):
    m_isLoading(false)
{
    m_id = CBackgroundLoadMgr::Instance()->AddObject(this);
}

CBackgroundBase::~CBackgroundBase(void)
{
    CBackgroundLoadMgr::Instance()->DelObject(m_id);
}

CBackgroundLoadMgr::CBackgroundLoadMgr(void):
    m_uid(1),
    m_isNeedUpdated(false),
    m_isEnded(false),
    m_isLoading(false)
{
    unsigned int threadID;
    m_thread = (HANDLE)_beginthreadex(0,0, &__WorkThread, this, 0,&threadID );

    SetThreadPriority(m_thread,THREAD_PRIORITY_BELOW_NORMAL);
}

CBackgroundLoadMgr::~CBackgroundLoadMgr(void)
{
    m_isEnded = true;

    if (0 != m_thread)
    {
        __StartLoadLoop();
        WaitForSingleObject(m_thread,INFINITE);
        CloseHandle(m_thread);
        m_thread = 0;
    }
}

U32 CBackgroundLoadMgr::AddObject(CBackgroundBase* pObj)
{
    if (0 == pObj)
        return 0;

    m_uid++;
    m_nodes.insert(std::make_pair(m_uid,pObj));
    return m_uid;
}

void CBackgroundLoadMgr::DelObject(U32 id)
{
    m_nodes.erase(id);
}

CBackgroundBase* CBackgroundLoadMgr::GetBackgroundObject(U32 id)
{
    NODE_MAP::iterator iter = m_nodes.find(id);

    if (iter == m_nodes.end())
        return 0;

    return iter->second;
}

void CBackgroundLoadMgr::Add(CBackgroundBase* pObj,bool isOpenDirectly,U32 type,U32 flag,const char* res,U32 priority)
{
    m_isNeedUpdated = true;
    pObj->SetLoading(true);

    if (priority == LOAD_PRIORITY_HIGH)
    {
        m_waitList.push_front(CWaitNode(pObj->GetLoadId(),isOpenDirectly,type,flag,priority,res));
    }
    else
    {
        m_waitList.push_back(CWaitNode(pObj->GetLoadId(),isOpenDirectly,type,flag,priority,res));
    }
}

void CBackgroundLoadMgr::Add(bool isOpenDirectly,const char* res,U32 priority)
{
    m_isNeedUpdated = true;

    if (priority == LOAD_PRIORITY_HIGH)
    {
        m_waitList.push_front(CWaitNode(0,isOpenDirectly,0,0,priority,res));
    }
    else
    {
        m_waitList.push_back(CWaitNode(0,isOpenDirectly,0,0,priority,res));
    }
}

#define LOAD_DISTANCE 80

bool CBackgroundLoadMgr::IsInDistance(const Point3F& pos)
{
    return (pos - m_playerPos).len() <= LOAD_DISTANCE;
}

U32 CBackgroundLoadMgr::CalPriority(const Point3F& pos)
{
    if ((pos - m_playerPos).lenSquared() <= LOAD_DISTANCE * LOAD_DISTANCE)
    {
        return LOAD_PRIORITY_HIGH;
    }

    return LOAD_PRIORITY_NORMAL;
}

void CBackgroundLoadMgr::Update(void)
{
    CLockGuard guard(m_mutex);

    //是否需要载入更多的资源
    if (m_waitList.size() > 0)
    {
        while (m_waitList.size() > 0)
        {
            CWaitNode& waitNode = m_waitList.front();
            m_loadingList.push_back(waitNode);
            m_waitList.pop_front();
        }
    }

    if (m_loadingList.size() > 0 && !m_isLoading)
    {
        __StartLoadLoop();
    }
    
    if (0 == m_loadedList.size())
        return;

    for (size_t i = 0; i < m_loadedList.size(); ++i)
    {
        CWaitNode& node = m_loadedList[i];

        if (0 == node.pRes && 0 == node.pStream)
        {
            continue;
        }

        //查找对应的sim对象
        CBackgroundBase* pObject = GetBackgroundObject(node.nodeId);

        if (0 != pObject)
        {
            if (0 != node.pStream)
            {
                pObject->OnLoaded(node.type,node.flag,node.pStream);
                delete node.pStream;
                node.pStream = 0;
            }
            
            if (0 != node.pRes)
            {
                pObject->OnLoaded(node.type,node.flag,node.pRes);
                node.pRes = 0;
            }

            pObject->SetLoading(false);
        }

        if (0 != node.pStream)
        {
            delete node.pStream;
            node.pStream = 0;
        }

        if (0 != node.pRes)
        {
            ResourceManager->unlock(node.pRes);
        }
    }
    
    m_loadedList.clear();
}

void CALLBACK s_apc(ULONG_PTR dwParam)
{

}

void CBackgroundLoadMgr::__StartLoadLoop(void)
{
    if (0 == m_thread)
        return;

    QueueUserAPC(s_apc,m_thread,0);
}

void CBackgroundLoadMgr::Loop(void)
{
    while(!m_isEnded)
    {
        m_isLoading = false;
        //处理需要加载的结点
        std::vector<CWaitNode> loadingList;
        
        do
        {
            CLockGuard guard(m_mutex);
            loadingList = m_loadingList;
            m_loadingList.clear();
        }
        while(0);

        if (0 == loadingList.size())
        {
            //目前没有需要载入的，挂起线程
            SleepEx(INFINITE,TRUE);
            continue;
        }

        m_isLoading = true;

        for (size_t i = 0; i < loadingList.size(); ++i)
        {
            if (0 == ResourceManager)
                return;

            CWaitNode& node = loadingList[i];

            if (node.isFinished)
                continue;

            //动态加载资源
            std::string& res = node.res;
            
            node.pStream = 0;

            if (node.isOpenDirectly)
            {   
                Stream* pStream = ResourceManager->openStream(res.c_str());

                if (0 != pStream)
                {
                    //读取文件内容到内存
                    MemStream* pMemStream = new MemStream(pStream->getStreamSize());
                    pStream->read(pMemStream->getStreamSize(),pMemStream->getBuffer());
            
                    ResourceManager->closeStream(pStream);
                    node.pStream = pMemStream;
                }
            }
            else
            {
                BACKGROUND_LOAD_LOCK

                node.pRes = ResourceManager->load(res.c_str(),false);

                //如果是模型，则载入对应的资源
                if (node.pRes && node.pRes->mInstance)
                {
                    TSShape* pShape = dynamic_cast<TSShape*>(node.pRes->mInstance);

                    if (0 != pShape)
                        pShape->preloadMaterialList();
                }
            }

            node.isFinished = true;

            //添加到完成队列
            CLockGuard guard(m_mutex);
            m_loadedList.push_back(node);
        }
    }
}

unsigned int __stdcall CBackgroundLoadMgr::__WorkThread(void* pThis)
{
    CBackgroundLoadMgr::Instance()->Loop();
    return 0;
}