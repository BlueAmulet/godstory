#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

#include <string>


class CCSLock;

#define LEAK_MARK 0x56f83d

//���������
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

//��¼�Ѿ�������ڴ浥Ԫ
struct sAllocUnit
{
    unsigned int actualSize;              //ʵ�ʷ���Ĵ�С
    unsigned int reportedSize;            //���صĴ�С

    void* actualAddress;            //ʵ�ʷ���ĵ�ַ
    void* reportedAddress;          //���ؿͻ��˵ĵ�ַ

    //static link
    const char*  sourceFile;        //����ʱ���ڵ��ļ�
    const char*  sourceFunc;        //����ʱ���ڵĺ���
    unsigned int sourceLine;        //����������

    unsigned int allocationType;    //�������� eg:new or malloc

    unsigned int threadID;          //����ʱ�߳�����ID
    unsigned int time;              //����ʱ��ʱ��
    unsigned int mark;              //���ڴ��ı�ʶ
    unsigned int allocId;           //����ͨ�����id,��������Ӧ�Ķϵ�

    struct sAllocUnit *next;
    struct sAllocUnit *prev;
};

//��¼�ڴ���Ϣ
struct sMStats
{
    unsigned int totalReportedMemory;       //�Ѿ�������ͻ��˵��ڴ��С
    unsigned int totalActualMemory;         //ʵ���ѷ�����ڴ�

    unsigned int peakReportedMemory;        //���������ڴ����
    unsigned int peakActualMemory;          //ʵ���ڴ����

    unsigned int accumulatedReportedMemory;
    unsigned int accumulatedActualMemory;
    unsigned int accumulatedAllocUnitCount;

    unsigned int totalAllocUnitCount;
    unsigned int peakAllocUnitCount;
};

//�����
class CMemLogHelper
{
public:
    CMemLogHelper(void) {}
    virtual ~CMemLogHelper(void) {}

    virtual void Dump(const char* log) {}
};

class CLockableObject;

/************************************************************************/
/*  �ڴ���������ṩ�ڴ������ͷŵļ��
    1:�����ڴ�����ɾ��

    ���
    1:���ɾ���ڴ�
    2:�ڴ����
    3:�ڴ�й©
    4:���߳��ͷ�
    5:�������ͺ��ͷ����Ͳ�ƥ��
/************************************************************************/
class CMemoryManager
{
    const static unsigned int hashBits     = 24;
    const static unsigned int paddingSize  = 2;

    //����ѡ��
    const static bool enableWipe           = true;   //�������ڴ渲��,ͨ�����ҵ��ڴ������ִ���
    const static bool randomWipe           = true;   //���ڴ��������
    const static bool zeroWip              = false;  //Ҳ��ϣ����release�¶��ڴ��������������
    const static bool checkThread          = false;  //�Ƿ����߳�

    const static bool alwaysValidateAll    = false;  //�Ƿ������еķ��䵥Ԫ�����ͷ�ʱ
    const static bool alwaysLogAll         = true;   //log all.

    const static bool assertOnMemLeak      = false;   

    //����д���ͷź���ڴ棬���ѷ����ڴ�ǰ�󱡼�.�����ж����
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

    //���öϵ������  
    //������Զ�����괦�¶ϵ�
    void    SetBreakOnMark(unsigned int mark)   {m_breakOnMark = mark;}

    //��ѯblock��ȡ��Ӧ��id,�����öϵ�
    void    SetBreakOnAllocId(unsigned int id)  {m_breakOnAllocId = id;}

    //�����ɾ���̶���Сʱ
    void    SetBreakOnSize(unsigned int size)   {m_breakOnSize = size;}

    //����ָ����Χ��С�ڴ�ʱ
    void    SetBreakOnSizeRange(unsigned int min,unsigned int max) {m_breakOnSizeMin = min; m_breakOnSizeMax = max;}

    //��ָ��������
    void    SetBreakOnFunc(const char* funcName) {m_breakOnFunc = funcName;}

    //������е�������
    void    ClearAllBreaks(void);

    //����һЩ������ض���
    void    SetDumpTo(CMemLogHelper* pDumper);

    //�趨�ѷ����ڴ�ı�־
    void    SetMark(unsigned int mark);
    
    //�ж��ڴ�ָ���Ƿ�Ϸ�
    bool    ValidateAddr(const void *reportedAddress);
    bool    ValidateAll(void);

    //�������Ϣ��log
    void    DumpAllocUnit(const void *reportedAddress);
    
    //�����ǰ�������ڴ��
    void    DumpMemReport(const char *filename = "memreport.log", const bool overwrite = true,unsigned int mark = 0);
    
    //������б�ʶ���ڴ��
    void    DumpMark(unsigned int mark,const char* fileName);

    void    DumpFunAllocSize(const char* fun);
    void    DumpFileAllocSize(const char* fun);

    void    SetSelectLeakDump(bool isEnabled) {m_isSelDumped = true; m_isSelDumpEnabled = isEnabled;}
public:
    //����ĺ�����Ӧ���ͻ���ֱ�ӵ���
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
    
    //����ַ�Ƿ���ڣ����ѷ���ĵ�Ԫ
    bool  _validateAlloc(const sAllocUnit *allocUnit);
    bool  _validateAllAllocs();
protected:
    void log( const char *format, ... );

    //���ϵ㴥��
    void         __CheckBreak(sAllocUnit* allocUnit);

    sAllocUnit*  findAllocUnit(const void* reportedAddress);
    unsigned int calculateActualSize( const unsigned int reportedSize );
    unsigned int calculateReportedSize( const unsigned int actualSize );
    void*        calculateReportedAddress( const void *actualAddress );
    void         wipeWithPattern(sAllocUnit*   allocUnit,unsigned long pattern,const unsigned int  originalReportedSize = 0 );
    void         DumpAllocations(FILE *fp,int mark);
    void         DumpLeakReport();
    void         resetGlobals(void);

    //��������
    unsigned int m_breakOnMark;
    unsigned int m_breakOnAllocId;
    unsigned int m_breakOnSize;
    unsigned int m_breakOnSizeMax;
    unsigned int m_breakOnSizeMin;
    std::string  m_breakOnFunc;

    //��ͨ��Ϣ���
    CMemLogHelper   m_dumper;
    CMemLogHelper*  m_pDumper;

    //�������̲߳���
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

