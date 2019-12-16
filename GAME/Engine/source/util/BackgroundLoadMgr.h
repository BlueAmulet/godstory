#ifndef _BACKGROUNDLOADMGR_H_
#define _BACKGROUNDLOADMGR_H_

#include "console/simObject.h"
#include "math\mPoint.h"
#include "platform\threads\mutex.h"
#include "core/stream.h"
#include "core/memstream.h"
#include <assert.h>
#include <string>
#include <hash_map>
#include <vector>
#include <deque>
#include <algorithm>

class ResourceObject;

//所有需要多线程加载的对象都通过此类派生
//提供加载后的回调操作
class CBackgroundBase
{
public:
    CBackgroundBase(void);
    virtual ~CBackgroundBase(void);
     
    U32     GetLoadId(void) const      {return m_id;}
    void    SetLoading(bool isLoading) {m_isLoading = isLoading;}
    bool    GetLoading(void) const     {return m_isLoading;}
    
    virtual bool BackgroundLoad(void) {return false;}

    //同步调用,在资源加载完成后
    virtual void OnLoaded(U32 type,U32 flag,Stream* pStream)     {}
    virtual void OnLoaded(U32 type,U32 flag,ResourceObject* pRes){}
protected:
    CBackgroundBase(const CBackgroundBase& other);
    const CBackgroundBase& operator = (const CBackgroundBase& other);

    U32     m_id;
    bool    m_isLoading;
};

//记录主线程的id
extern unsigned int g_MainThreadId;

#ifdef ENABLE_MAINTHREAD_CHECK
#include <windows.h>
#define MAIN_THREAD_CHECK assert(GetCurrentThreadId() == g_MainThreadId && "multithread deteched");
#else
#define MAIN_THREAD_CHECK
#endif

/************************************************************************/
/* 后台加载管理器,如果对象要相应加载事件，则从CBackgroundBase派生
   回调函数在主线程内
/************************************************************************/
class CBackgroundLoadMgr
{
    typedef void *HANDLE;

    enum LOAD_PRIORITY
    {
        LOAD_PRIORITY_HIGH   = 0,
        LOAD_PRIORITY_NORMAL,
    };

    //等待的节点
    struct CWaitNode
    {
        CWaitNode(U32 _id,bool _isOpenDirectly,U32 _type,U32 _flag,U32 _priority,const char* _res) : 
            nodeId(_id),isOpenDirectly(_isOpenDirectly),type(_type),flag(_flag),priority(_priority),res(_res),pStream(0),pRes(0),isFinished(false)
       {}

       inline bool operator <  (const CWaitNode& b ){return (priority < b.priority);}
       inline bool operator >  (const CWaitNode& b ){return (priority < b.priority);}
       inline bool operator == (const CWaitNode& b ){return (priority < b.priority);}

        U32         nodeId;
        U32         type;
        U32         flag;
        U32         priority;
        //资源也许不需要绑定res instance,此时直接打开相应的文件
        bool        isOpenDirectly;
        bool        isFinished;
        Stream*     pStream;
        ResourceObject* pRes;
        std::string res;
    };

    std::greater<CWaitNode> NODE_CMP;
public:
    CBackgroundLoadMgr(void);
    ~CBackgroundLoadMgr(void);

    //it should be instanced
    static CBackgroundLoadMgr* Instance(void)
    {
        static CBackgroundLoadMgr local;
        return &local;
    }

    //相应的对象需要是动态分配的
    U32     GenUID(void) {return ++m_uid;}
    U32     AddObject(CBackgroundBase* pObj);
    void    DelObject(U32 id);
    CBackgroundBase* GetBackgroundObject(U32 id);

    //设置玩家坐标的参考点,物体会按照和参考点的距离来加载
    void    SetRefPoint(Point3F playerPos) {m_playerPos = playerPos;}
    Point3F GetRefPoint(void) const {return m_playerPos;}
    bool    IsInDistance(const Point3F& pos);
    U32     CalPriority(const Point3F& pos);
    
    //添加待加载对象
    void    Add(CBackgroundBase* pObj,bool isOpenDirectly,U32 type,U32 flag,const char* res,U32 priority = 0);
    //添加待加载的资源，不需要处理对象
    void    Add(bool isOpenDirectly,const char* res,U32 priority = 0);

    //更新加载队列
    void    Update(void);

    //等待加载完成,dist为等待的距离范围
    void    Wait(float dist) {}

    //循环加载资源
    void    Loop(void);

    CCSLock& GetLock(void) {return m_bkGroundMutex;}
private:
    static unsigned int __stdcall __WorkThread(void* pThis);
    
    void    __StartLoadLoop(void);

    CCSLock m_mutex;

    //尽量用这个锁来同步资源载入
    CCSLock m_bkGroundMutex;
    
    //加载线程
    HANDLE  m_thread;

    bool    m_isEnded;
    bool    m_isLoading;

    //每个加载对象的唯一id
    U32     m_uid;

    //是否需要更新
    bool    m_isNeedUpdated;

    //对象的列表
    typedef stdext::hash_map<U32,CBackgroundBase*> NODE_MAP;
    NODE_MAP m_nodes;

    //玩家的坐标点
    Point3F m_playerPos;

    //等待处理的对象
    std::deque<CWaitNode>  m_waitList;

    //正在加载的队列
    std::vector<CWaitNode> m_loadingList;

    //已经完成的队列
    std::vector<CWaitNode> m_loadedList;
};

//one lock to reduce thread errors
#define BACKGROUND_LOAD_LOCK CLockGuard bk_guard(CBackgroundLoadMgr::Instance()->GetLock());

#endif /*_BACKGROUNDLOADMGR_H_*/