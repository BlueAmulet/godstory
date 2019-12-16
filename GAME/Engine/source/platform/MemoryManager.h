#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

#include <string>


class CCSLock;

#define LEAK_MARK 0x56f83d

//分配的类型
enum
{
    m_alloc_unknown        = 0,
    m_alloc_new            = 1,
    m_alloc_new_array      = 2,
    m_alloc_malloc         = 3,
    m_alloc_calloc         = 4,
    m_alloc_realloc        = 5,
    m_alloc_delete         = 6,
    m_alloc_delete_array   = 7,
    m_alloc_free           = 8
};

//记录已经分配的内存单元
struct sAllocUnit
{
    unsigned int actualSize;              //实际分配的大小
    unsigned int reportedSize;            //返回的大小

    void* actualAddress;            //实际分配的地址
    void* reportedAddress;          //返回客户端的地址

    //static link
    const char*  sourceFile;        //分配时所在的文件
    const char*  sourceFunc;        //分配时所在的函数
    unsigned int sourceLine;        //分配所在行

    unsigned int allocationType;    //分配类型 eg:new or malloc

    unsigned int threadID;          //分配时线程所在ID
    unsigned int time;              //分配时的时间
    unsigned int mark;              //给内存打的标识
    unsigned int allocId;           //可以通过这个id,来设置相应的断点

    struct sAllocUnit *next;
    struct sAllocUnit *prev;
};

//记录内存信息
struct sMStats
{
    unsigned int totalReportedMemory;       //已经分配给客户端的内存大小
    unsigned int totalActualMemory;         //实际已分配的内存

    unsigned int peakReportedMemory;        //申请分配的内存峰量
    unsigned int peakActualMemory;          //实际内存峰量

    unsigned int accumulatedReportedMemory;
    unsigned int accumulatedActualMemory;
    unsigned int accumulatedAllocUnitCount;

    unsigned int totalAllocUnitCount;
    unsigned int peakAllocUnitCount;
};

//输出绑定
class CMemLogHelper
{
public:
    CMemLogHelper(void) {}
    virtual ~CMemLogHelper(void) {}

    virtual void Dump(const char* log) {}
};

class CLockableObject;

/************************************************************************/
/*  内存管理器，提供内存分配和释放的监控
    1:跟踪内存分配和删除

    解决
    1:多次删除内存
    2:内存溢出
    3:内存泄漏
    4:跨线程释放
    5:分配类型和释放类型不匹配
/************************************************************************/
class CMemoryManager
{
    const static unsigned int hashBits     = 24;
    const static unsigned int paddingSize  = 2;

    //功能选项
    const static bool enableWipe           = true;   //开启对内存覆盖,通过杂乱的内存来发现错误
    const static bool randomWipe           = true;   //对内存随机覆盖
    const static bool zeroWip              = false;  //也许希望对release下对内存清零来避免错误
    const static bool checkThread          = false;  //是否检测线程

    const static bool alwaysValidateAll    = false;  //是否检测所有的分配单元，在释放时
    const static bool alwaysLogAll         = true;   //log all.

    const static bool assertOnMemLeak      = false;   

    //用来写入释放后的内存，或已分配内存前后薄记.用来判断溢出
    const static unsigned int prefixPattern   = 0xbaadf00d;
    const static unsigned int postfixPattern  = 0xdeadc0de;
    const static unsigned int unusedPattern   = 0xfeedface;
    const static unsigned int releasedPattern = 0xdeadbeef;

    const static unsigned int hashSize = 1 << hashBits;

    const char * sourceFile;
    const char * sourceFunc;
public:
    CMemoryManager(void);
    ~CMemoryManager(void);

    static CMemoryManager* Instance(void);

    //设置断点的条件  
    //在玩家自定义旗标处下断点
    void    SetBreakOnMark(unsigned int mark)   {m_breakOnMark = mark;}

    //查询block获取相应的id,并设置断点
    void    SetBreakOnAllocId(unsigned int id)  {m_breakOnAllocId = id;}

    //分配或删除固定大小时
    void    SetBreakOnSize(unsigned int size)   {m_breakOnSize = size;}

    //分配指定范围大小内存时
    void    SetBreakOnSizeRange(unsigned int min,unsigned int max) {m_breakOnSizeMin = min; m_breakOnSizeMax = max;}

    //在指定函数处
    void    SetBreakOnFunc(const char* funcName) {m_breakOnFunc = funcName;}

    //清除所有调试条件
    void    ClearAllBreaks(void);

    //设置一些输出的重定向
    void    SetDumpTo(CMemLogHelper* pDumper);

    //设定已分配内存的标志
    void    SetMark(unsigned int mark);
    
    //判断内存指针是否合法
    bool    ValidateAddr(const void *reportedAddress);
    bool    ValidateAll(void);

    //输出块信息到log
    void    DumpAllocUnit(const void *reportedAddress);
    
    //输出当前的所有内存块
    void    DumpMemReport(const char *filename = "memreport.log", const bool overwrite = true,unsigned int mark = 0);
    
    //输出所有标识的内存块
    void    DumpMark(unsigned int mark,const char* fileName);

    void    DumpFunAllocSize(const char* fun);
    void    DumpFileAllocSize(const char* fun);

    void    SetSelectLeakDump(bool isEnabled) {m_isSelDumped = true; m_isSelDumpEnabled = isEnabled;}
public:
    //下面的函数不应被客户端直接调用
    void _dllocMem(const char *         sourceFile, 
                   const unsigned int   sourceLine, 
                   const char *         sourceFunc,
                   const unsigned int   deallocationType, 
                   const void *         reportedAddress);

    void* _rllocMem(const char *       sourceFile, 
                     const unsigned int sourceLine, 
                     const char *       sourceFunc,
                     const unsigned int reallocationType, 
                     const unsigned int       reportedSize, 
                     void *             reportedAddress);

    void* _allocMem(const char *        sourceFile, 
                    const unsigned int  sourceLine, 
                    const char *        sourceFunc,
                    const unsigned int  allocationType, 
                    const unsigned int  reportedSize);

    void _setOwner(const char *file, const unsigned int line, const char *func);

    void* _op_new_sc( unsigned int reportedSize); //new
    void* _op_new_vc( unsigned int reportedSize); //new []

    void  _op_del_sc( void *reportedAddress);      //delete
    void  _op_del_vc( void *reportedAddress);      //delete []

    void  _DumpAllocUnit(const sAllocUnit *allocUnit, const char *prefix = "",bool isUserAlloc = false);
    
    //检测地址是否存在，从已分配的单元
    bool  _validateAlloc(const sAllocUnit *allocUnit);
    bool  _validateAllAllocs();
protected:
    void log( const char *format, ... );

    //检测断点触发
    void         __CheckBreak(sAllocUnit* allocUnit);

    sAllocUnit*  findAllocUnit(const void* reportedAddress);
    unsigned int calculateActualSize( const unsigned int reportedSize );
    unsigned int calculateReportedSize( const unsigned int actualSize );
    void*        calculateReportedAddress( const void *actualAddress );
    void         wipeWithPattern(sAllocUnit*   allocUnit,unsigned long pattern,const unsigned int  originalReportedSize = 0 );
    void         DumpAllocations(FILE *fp,int mark);
    void         DumpLeakReport();
    void         resetGlobals(void);

    //调试条件
    unsigned int m_breakOnMark;
    unsigned int m_breakOnAllocId;
    unsigned int m_breakOnSize;
    unsigned int m_breakOnSizeMax;
    unsigned int m_breakOnSizeMin;
    std::string  m_breakOnFunc;

    //普通信息输出
    CMemLogHelper   m_dumper;
    CMemLogHelper*  m_pDumper;

    //锁定多线程操作
    CCSLock*		 m_locker;
	char             m_lockerBuf[256];
	
    //DATA
    sAllocUnit**     hashTable;
    sAllocUnit*      reservoir;

    unsigned int     currentAllocationCount;

    sMStats          stats;

    unsigned int     sourceLine;

    sAllocUnit**     reservoirBuffer;
    unsigned int     reservoirBufferSize;

    bool             m_isSelDumped;
    bool             m_isSelDumpEnabled;
};

#endif /*_MEMORYMANAGER_H_*/

