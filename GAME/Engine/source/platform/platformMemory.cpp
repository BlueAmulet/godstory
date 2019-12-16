//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/fileStream.h"
#include "console/console.h"
#include "platform/profiler.h"
#include "platform/threads/mutex.h"
#include "MemoryManagerInclude.h"
#include "sceneGraph/lightManager.h"

#ifdef POWER_DEBUG
#ifdef POWER_MEMORY_MANAGER

ConsoleFunction(MemValidate, void, 1, 1, "MemValidate();")
{
   argc; argv;
   CMemoryManager::Instance()->ValidateAll();
}

ConsoleFunction(MemSnapshot,void,2,2,"(string fileName) Dump a snapshot of current memory to a file.")
{
   argc;
   CMemoryManager::Instance()->DumpMemReport(argv[1]);
}

ConsoleFunction(MemMark,void,2,2,"MemMark(int flag);")
{
    unsigned int mark = dAtoi(argv[1]);
    CMemoryManager::Instance()->SetMark(mark);
}

ConsoleFunction(MemDumpMark,void,3,3,"MemDumpMark(flag,filename);")
{
    unsigned int mark = dAtoi(argv[1]);

    CMemoryManager::Instance()->DumpMark(mark,argv[2]);
}

//输出对应函数分配的内存大小
ConsoleFunction(MemDumpFunSize,void,2,2,"MemDumpFunSize(filename);")
{
    CMemoryManager::Instance()->DumpFunAllocSize(argv[1]);
}

//输出对应文件分配的内存大小
ConsoleFunction(MemDumpFileSize,void,2,2,"MemDumpFileSize(filename);")
{
    CMemoryManager::Instance()->DumpFileAllocSize(argv[1]);
}

//断点调试
ConsoleFunction(MemBreakClear,void,1,1,"MemBreakClear();")
{
    CMemoryManager::Instance()->ClearAllBreaks();
}

ConsoleFunction(MemBreakOnSize,void,2,2,"MemBreakOnSize(size);")
{
    CMemoryManager::Instance()->SetBreakOnSize(dAtoi(argv[1]));
}

ConsoleFunction(MemLeakEnable,void,2,2,"MemLeakEnable(bool)")
{
    bool isEnabled = (bool)dAtoi(argv[1]);

    //设置内存泄漏为有选择的输出
    CMemoryManager::Instance()->SetSelectLeakDump(isEnabled);
    
    static bool s_isEnabled = false;
    
    if (!s_isEnabled)
    {
        if (isEnabled)
            CMemoryManager::Instance()->SetMark(LEAK_MARK - 1);
    }
    else
    {
        if (!isEnabled)
            CMemoryManager::Instance()->SetMark(LEAK_MARK);
    }

    s_isEnabled = isEnabled;
}

#endif
#endif

ConsoleFunctionGroupEnd( Memory );

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <stdlib.h>

void* dMalloc_r(dsize_t in_size, const char* fileName, const dsize_t line)
{   
   //void *ret =  dMalloc(in_size);
	void* ret = dMallocEx(in_size,fileName,line);
   return ret;
}

void* dRealloc_r(void* in_pResize, dsize_t in_size, const char* fileName, const dsize_t line)
{
   //return dRealloc(in_pResize,in_size);
	return dReallocEx(in_pResize,in_size,fileName,line);
}

#ifdef POWER_MEMORY_MANAGER

class CConMemLog:public CMemLogHelper
{
public:
    CConMemLog(void)
    {
        InitMemManager();
    }

    ~CConMemLog(void)
    {
        CMemoryManager::Instance()->SetDumpTo(0);
    }

    virtual void Dump(const char* log)
    {
        Con::printf(log);
    }
};

static CConMemLog g_memlog;

void InitMemManager(void)
{   
    CMemoryManager::Instance()->SetDumpTo(&g_memlog);
}

#endif /*POWER_MEMORY_MANAGER*/