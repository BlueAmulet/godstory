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

//������Ҫ���̼߳��صĶ���ͨ����������
//�ṩ���غ�Ļص�����
class CBackgroundBase
{
public:
    CBackgroundBase(void);
    virtual ~CBackgroundBase(void);
     
    U32     GetLoadId(void) const      {return m_id;}
    void    SetLoading(bool isLoading) {m_isLoading = isLoading;}
    bool    GetLoading(void) const     {return m_isLoading;}
    
    virtual bool BackgroundLoad(void) {return false;}

    //ͬ������,����Դ������ɺ�
    virtual void OnLoaded(U32 type,U32 flag,Stream* pStream)     {}
    virtual void OnLoaded(U32 type,U32 flag,ResourceObject* pRes){}
protected:
    CBackgroundBase(const CBackgroundBase& other);
    const CBackgroundBase& operator = (const CBackgroundBase& other);

    U32     m_id;
    bool    m_isLoading;
};

//��¼���̵߳�id
extern unsigned int g_MainThreadId;

#ifdef ENABLE_MAINTHREAD_CHECK
#include <windows.h>
#define MAIN_THREAD_CHECK assert(GetCurrentThreadId() == g_MainThreadId && "multithread deteched");
#else
#define MAIN_THREAD_CHECK
#endif

/************************************************************************/
/* ��̨���ع�����,�������Ҫ��Ӧ�����¼������CBackgroundBase����
   �ص����������߳���
/************************************************************************/
class CBackgroundLoadMgr
{
    typedef void *HANDLE;

    enum LOAD_PRIORITY
    {
        LOAD_PRIORITY_HIGH   = 0,
        LOAD_PRIORITY_NORMAL,
    };

    //�ȴ��Ľڵ�
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
        //��ԴҲ����Ҫ��res instance,��ʱֱ�Ӵ���Ӧ���ļ�
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

    //��Ӧ�Ķ�����Ҫ�Ƕ�̬�����
    U32     GenUID(void) {return ++m_uid;}
    U32     AddObject(CBackgroundBase* pObj);
    void    DelObject(U32 id);
    CBackgroundBase* GetBackgroundObject(U32 id);

    //�����������Ĳο���,����ᰴ�պͲο���ľ���������
    void    SetRefPoint(Point3F playerPos) {m_playerPos = playerPos;}
    Point3F GetRefPoint(void) const {return m_playerPos;}
    bool    IsInDistance(const Point3F& pos);
    U32     CalPriority(const Point3F& pos);
    
    //��Ӵ����ض���
    void    Add(CBackgroundBase* pObj,bool isOpenDirectly,U32 type,U32 flag,const char* res,U32 priority = 0);
    //��Ӵ����ص���Դ������Ҫ�������
    void    Add(bool isOpenDirectly,const char* res,U32 priority = 0);

    //���¼��ض���
    void    Update(void);

    //�ȴ��������,distΪ�ȴ��ľ��뷶Χ
    void    Wait(float dist) {}

    //ѭ��������Դ
    void    Loop(void);

    CCSLock& GetLock(void) {return m_bkGroundMutex;}
private:
    static unsigned int __stdcall __WorkThread(void* pThis);
    
    void    __StartLoadLoop(void);

    CCSLock m_mutex;

    //�������������ͬ����Դ����
    CCSLock m_bkGroundMutex;
    
    //�����߳�
    HANDLE  m_thread;

    bool    m_isEnded;
    bool    m_isLoading;

    //ÿ�����ض����Ψһid
    U32     m_uid;

    //�Ƿ���Ҫ����
    bool    m_isNeedUpdated;

    //������б�
    typedef stdext::hash_map<U32,CBackgroundBase*> NODE_MAP;
    NODE_MAP m_nodes;

    //��ҵ������
    Point3F m_playerPos;

    //�ȴ�����Ķ���
    std::deque<CWaitNode>  m_waitList;

    //���ڼ��صĶ���
    std::vector<CWaitNode> m_loadingList;

    //�Ѿ���ɵĶ���
    std::vector<CWaitNode> m_loadedList;
};

//one lock to reduce thread errors
#define BACKGROUND_LOAD_LOCK CLockGuard bk_guard(CBackgroundLoadMgr::Instance()->GetLock());

#endif /*_BACKGROUNDLOADMGR_H_*/