#include "MemoryManager.h"
#include "MemoryManagerInclude.h"
#include "base/Locker.h"
#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include "time.h"
#include <new>
#include <hash_map>
#include "sceneGraph/lightManager.h"
#include "platform/threads/mutex.h"

#ifdef new
#undef new
#endif 

#ifdef POWER_MEMORY_MANAGER

#pragma warning( disable : 4073)
#pragma init_seg (compiler)
CMemoryManager g_memortyManager;
bool g_memManagerEnded = false;

#else

bool g_memManagerEnded = true;

#endif /*POWER_MEMORY_MANAGER*/

CMemoryManager* CMemoryManager::Instance(void)
{
#ifdef POWER_MEMORY_MANAGER
    return &g_memortyManager;
#else
    return 0;
#endif
}

void CMemoryManager::ClearAllBreaks(void)
{
    m_breakOnMark       = 0;
    m_breakOnAllocId    = 0;
    m_breakOnSize       = 0;
    m_breakOnSizeMax    = 0;
    m_breakOnSizeMin    = 0;
    m_breakOnFunc       = "";
}

const char *allocationTypes[] = 
{
    "Unknown", 
    "new",     
    "new[]",  
    "malloc",   
    "calloc", 
    "realloc", 
    "delete", 
    "delete[]", 
    "free"
};

/************************************************************************/
/* �����Ѿ�����ĵ�Ԫ
/************************************************************************/
sAllocUnit* CMemoryManager::findAllocUnit(const void* reportedAddress)
{
    if (0 == reportedAddress)
        return 0;

    //hash����
    //�ڴ��ַ�ĺ�4λ������Ϊ0
    unsigned int hashIndex = ( (unsigned int)reportedAddress >> 4 ) & ( hashSize - 1 );
    sAllocUnit *ptr = hashTable[ hashIndex ];

    while( ptr )
    {
        if( ptr->reportedAddress == reportedAddress )
            return ptr;

        ptr = ptr->next;
    }

    return 0;
}

/************************************************************************/
/* ����ʵ�ʷ�����ڴ��С�����ѷ����С��ǰ�����4 * sizeof(long)����Ϣ���ж��ڴ��Ƿ�����
/************************************************************************/
unsigned int CMemoryManager::calculateActualSize( const unsigned int reportedSize )
{
    return reportedSize + paddingSize * sizeof(long) * 2;
}

/************************************************************************/
/* ���ط����С
/************************************************************************/
unsigned int CMemoryManager::calculateReportedSize( const unsigned int actualSize )
{
    return actualSize - paddingSize * sizeof(long) * 2;
}

/************************************************************************/
/* ���ط�����ͻ��˵��ڴ��ַ����ȥpadding�Ĵ�С
/************************************************************************/
void* CMemoryManager::calculateReportedAddress( const void *actualAddress )
{
    if (!actualAddress)
        return 0;

    // Just account for the padding
    return (void *)((char *) actualAddress + sizeof(long) * paddingSize );
}

/************************************************************************/
/* ���ڴ�ǰ�����Ӽ����Ϣ
/************************************************************************/
void CMemoryManager::wipeWithPattern(sAllocUnit*   allocUnit, unsigned long pattern,const unsigned int  originalReportedSize)
{
    //�Ƿ�д�������Ϣ,����ǹ̶��ģ��Ӷ����������
    if( randomWipe )
        pattern = ((rand() & 0xff) << 24) | ((rand() & 0xff) << 16) | ((rand() & 0xff) << 8) | (rand() & 0xff);

     if(zeroWip)
        pattern = 0;

    //���縲�ǹ�������
    if( enableWipe && allocUnit->reportedSize > originalReportedSize )
    {
        // Fill the bulk
        long  *lptr = (long *) ((char *)allocUnit->reportedAddress + originalReportedSize);
        unsigned int length = allocUnit->reportedSize - originalReportedSize;

        unsigned int i;
        for( i = 0; i < (length >> 2); i++, lptr++ )
        {
            *lptr = pattern;
        }

        // Fill the remainder
        unsigned int shiftCount = 0;
        char *cptr = (char *) lptr;

        for( i = 0; i < ( length & 0x3 ); i++, cptr++, shiftCount += 8 )
        {
            *cptr =  (char)((( pattern & ( 0xff << shiftCount ) ) >> shiftCount) & 0xff);
        }
    }

    // Write in the prefix/postfix bytes
    long        *pre  = (long *)allocUnit->actualAddress;
    long        *post = (long *)((char *)allocUnit->actualAddress + allocUnit->actualSize - paddingSize * sizeof(long));

    for (unsigned int i = 0; i < paddingSize; i++, pre++, post++)
    {
        *pre  = prefixPattern;
        *post = postfixPattern;
    }
}

void CMemoryManager::DumpAllocations(FILE *fp,int mark)
{
    //return;

    fprintf(fp, "Addr       Size       Addr       Size                                                      \r\n");
    fprintf(fp, "Reported   Reported   Actual     Actual     Unused    Method  Dealloc Realloc Allocated by \r\n");
    fprintf(fp, "------ ---------- ---------- ---------- ---------- ---------- -------- ------- ------- -------------------- \r\n");
    
    //��¼ÿ����������Ĵ�С
    stdext::hash_map<std::string,unsigned int> funAllocSizeMap;

    for( unsigned int i = 0; i < hashSize; i++ )
    {
        sAllocUnit *ptr = hashTable[i];

        while( ptr )
        {
            if (0 != mark && ptr->mark != mark)
                break;

			/*if ( strstr(ptr->sourceFunc, "::initPersistFields") == NULL &&
				 _stricmp(ptr->sourceFunc, "Journal::DeclareFunction") != 0)
			{*/
				fprintf(fp, "0x%08X %d 0x%08X %d %-8s %s %d  %s\r\n",
					(unsigned int) ptr->reportedAddress, ptr->reportedSize,
					(unsigned int) ptr->actualAddress, ptr->actualSize,
					allocationTypes[ptr->allocationType],    
					ptr->sourceFile, ptr->sourceLine, ptr->sourceFunc);

				funAllocSizeMap[ptr->sourceFunc] += ptr->reportedSize;
			//}   
			
            ptr = ptr->next;
        }
    }
    
    fprintf(fp, "function name         Size \r\n");

    //���ÿ���������ύ�����С
    for (stdext::hash_map<std::string,unsigned int>::iterator iter = funAllocSizeMap.begin(); iter != funAllocSizeMap.end(); ++iter)
    {
        fprintf(fp,"%s      %d \r\n",iter->first.c_str(),iter->second);
    }
}

/************************************************************************/
/* ��־��Ϣ
/************************************************************************/
void CMemoryManager::log( const char *format, ... )
{
    // The buffer
    char buffer[2048];

    va_list ap;
    va_start( ap, format );
    vsprintf_s( buffer,sizeof(buffer),format, ap );
    va_end( ap );

    m_pDumper->Dump(buffer);
}

/************************************************************************/
/* �ڴ�й©��Ϣ���
/************************************************************************/
void CMemoryManager::DumpLeakReport()
{
    // Open the report file
	FILE* fp;
	errno_t err = fopen_s(&fp, "memleak.log", "w+b");
    if( err != 0 )
        return;

    // Header
    char timeString[25];
    memset( timeString, 0, sizeof(timeString) );
    time_t t = time(0);
    struct tm tme;
	err = localtime_s(&tme, &t);

    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "|                                          Memory leak report for:  %02d/%02d/%04d %02d:%02d:%02d                                            |\r\n", tme.tm_mon + 1, tme.tm_mday, tme.tm_year + 1900, tme.tm_hour, tme.tm_min, tme.tm_sec);
    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "\r\n");
    fprintf(fp, "\r\n");

    //��ʣ��û�ͷŵ��ڴ��
    if( stats.totalAllocUnitCount )
    {
        fprintf(fp, "%d memory leak%s found:\r\n", stats.totalAllocUnitCount,stats.totalAllocUnitCount == 1 ? "":"s" );
    }
    else
    {
        fprintf(fp, "Congratulations! No memory leaks found!\r\n");
    }

    fprintf(fp, "\r\n");

    //���й©�ĵط�
    if( stats.totalAllocUnitCount )
    {
        DumpAllocations(fp,m_isSelDumped ? LEAK_MARK : 0);

        if (assertOnMemLeak)
        {
            assert(0 && "�����ڴ�й©");
        }
    }

    // We can finally free up our own memory allocations
    if (reservoirBuffer)
    {
        for (unsigned int i = 0; i < reservoirBufferSize; i++)
        {
            free( reservoirBuffer[i] );
        }

        free(reservoirBuffer);

        reservoirBuffer = 0;
        reservoirBufferSize = 0;
        reservoir = 0;
    }

    fclose(fp);
}

/************************************************************************/
/* ���õ�ǰ�������Ϣ
/************************************************************************/
void CMemoryManager::_setOwner(const char *file, const unsigned int line, const char *func)
{
    sourceFile = file;
    sourceLine = line;
    sourceFunc = func;
}

void CMemoryManager::resetGlobals(void)
{
    sourceFile = "??";
    sourceLine = 0;
    sourceFunc = "??";
}

//�ڴ����!
void* CMemoryManager::_allocMem(const char *        sourceFile, 
                                const unsigned int  sourceLine,  
                                const char *        sourceFunc, 
                                const unsigned int  allocationType, 
                                const unsigned int  reportedSize)
{
    CLockGuard guard(*m_locker);

    if (g_memManagerEnded)
        return 0;
    
    unsigned threadID = GetCurrentThreadId();

    try
    {
        // Increase our allocation count
        currentAllocationCount++;

        //���ӷ��䵥Ԫ�Ļ�����
        if( !reservoir )
        {
            // Allocate 256 reservoir elements
            reservoir = (sAllocUnit *) malloc( sizeof(sAllocUnit) * 256 );

            // If you hit this assert, then the memory manager failed to 
            // allocate internal memory for tracking the allocations
            assert( reservoir != 0 );

            if( reservoir == 0 )
            {
                return 0;
            }

            // Build a linked-list of the elements in our reservoir
            memset( reservoir, 0, sizeof(sAllocUnit) * 256 );

            for (unsigned int i = 0; i < 256 - 1; i++)
            {
                reservoir[i].next = &reservoir[i+1];
            }

            // Add this address to our reservoirBuffer so we can free it later
            sAllocUnit **temp = (sAllocUnit **)realloc( reservoirBuffer, (reservoirBufferSize + 1) * sizeof(sAllocUnit *) );
            assert( temp );

            if( temp )
            {
                reservoirBuffer = temp;
                reservoirBuffer[reservoirBufferSize++] = reservoir;
            }
        }

        // Logical flow says this should never happen...
        assert( reservoir != 0 );

        // Grab a new allocaton unit from the front of the reservoir
        sAllocUnit * au = reservoir;
        reservoir = au->next;

        au->actualSize        = calculateActualSize(reportedSize);
        au->actualAddress     = malloc( au->actualSize );
        au->reportedSize      = reportedSize;
        au->reportedAddress   = calculateReportedAddress( au->actualAddress );
        au->allocationType    = allocationType;
        au->sourceLine        = sourceLine;
        au->threadID          = threadID;
        au->time              = (unsigned int)time(0);
        au->allocId           = currentAllocationCount;

        if( sourceFile ) 
            au->sourceFile = sourceFile;
        else
            au->sourceFile = "??";

        if( sourceFunc ) 
            au->sourceFunc = sourceFunc;
        else
            au->sourceFunc = "??";

        assert( au->actualAddress != 0 );

        //�޷������㹻���ڴ�
        if( au->actualAddress == 0 )
        {
            log("û�и�����ڴ����");
            return 0;
        }

        //��Ҫ�ĳ����е��ڴ���亯��
        assert( allocationType != m_alloc_unknown );

        if( allocationType == m_alloc_unknown )
        {
            log( "[!] Allocation made from outside memory tracker in %s(%d)::%s:", au->sourceFile, au->sourceLine, au->sourceFunc );
            _DumpAllocUnit( au," ");
        }

        __CheckBreak(au);

        // Insert the new allocation into the hash table
        unsigned int hashIndex = ((unsigned int) au->reportedAddress >> 4) & (hashSize - 1);

        if( hashTable[hashIndex]) 
        {
            hashTable[hashIndex]->prev = au;
        }

        au->next = hashTable[hashIndex];
        au->prev = 0;
        hashTable[hashIndex] = au;

        // Account for the new allocatin unit in our stats
        stats.totalReportedMemory += au->reportedSize;
        stats.totalActualMemory   += au->actualSize;
        stats.totalAllocUnitCount++;

        if( stats.totalReportedMemory > stats.peakReportedMemory )
            stats.peakReportedMemory = stats.totalReportedMemory;
        if( stats.totalActualMemory   > stats.peakActualMemory )
            stats.peakActualMemory   = stats.totalActualMemory;
        if( stats.totalAllocUnitCount > stats.peakAllocUnitCount )
            stats.peakAllocUnitCount = stats.totalAllocUnitCount;

        stats.accumulatedReportedMemory += au->reportedSize;
        stats.accumulatedActualMemory += au->actualSize;
        stats.accumulatedAllocUnitCount++;

        // Prepare the allocation unit for use (wipe it with recognizable garbage)
        wipeWithPattern(au, unusedPattern);

        // calloc() expects the reported memory address range to be filled with 0's
        memset( au->reportedAddress, 0, au->reportedSize );

        // Validate every single allocated unit in memory
        if( alwaysValidateAll )
            _validateAllAllocs();

        // Resetting the globals insures that if at some later time, somebody 
        // calls our memory manager from an unknown source (i.e. they didn't 
        // include our H file) then we won't think it was the last allocation.
        resetGlobals();

		if (au->allocationType == m_alloc_malloc && au->reportedSize == 3328)
		{
			//g_TestHulingyun.Insert(au->reportedAddress);
			int test = 0;
		}

        // Return the (reported) address of the new allocation unit
        return au->reportedAddress;
    }
    catch( const char *err )
    {
        // Deal with the errors

        log("[!] %s", err);
        resetGlobals();

        return 0;
    }
}


void * CMemoryManager::_rllocMem(const char *           sourceFile, 
                                 const unsigned int     sourceLine, 
                                 const char *           sourceFunc, 
                                 const unsigned int     reallocationType, 
                                 const unsigned int     reportedSize, 
                                 void *                 reportedAddress)
{
    CLockGuard guard(*m_locker);
    
    if (g_memManagerEnded)
        return 0;

    unsigned threadID = GetCurrentThreadId();

    try
    {
        // ANSI says: Calling realloc with a 0 should force same operations 
        // as a malloc
        if( !reportedAddress )
        {
            return _allocMem(sourceFile, sourceLine, sourceFunc, reallocationType, reportedSize);
        }

        // Increase our allocation count
        currentAllocationCount++;

        // Locate the existing allocation unit
        sAllocUnit *au = findAllocUnit( reportedAddress );

        __CheckBreak(au);

        //1����Чָ�룬Ҳ��ǰ���Ѿ��ͷŹ�������ͨ���ڴ��������ָ��
        assert(au != 0 && "ָ�������Чָ��");

        if( au == 0 )
            return 0;

        //����ڴ�������
        assert( _validateAlloc( au ) );        

        // If you hit this assert, then this reallocation was made from a source 
        // that isn't setup to use this memory tracking software, use the stack
        // frame to locate the source and include our H file.
        assert( reallocationType != m_alloc_unknown );

        if( reallocationType == m_alloc_unknown )
        {
            log( "[!] Allocationfrom outside memory tracker in %s(%d)::%s :", sourceFile, sourceLine, sourceFunc );
            _DumpAllocUnit( au, "  " );
        }

        //�ܲ��ܶ�new����ĵ���realloc
        assert(
            au->allocationType == m_alloc_malloc ||
            au->allocationType == m_alloc_calloc ||
            au->allocationType == m_alloc_realloc);
        if( reallocationType == m_alloc_unknown )
        {
            log( "[!] Allocation-deallocation mismatch in %s(%d)::%s :", sourceFile, sourceLine, sourceFunc );
            _DumpAllocUnit( au, "  " );
        }

        //����Ƿ�����ͬ1���߳��ڵ�
        if (au->threadID != threadID)
        {
            log("[!] �ڴ���������·��䲻��ͬ1���߳� %s(%d)::%s :", sourceFile, sourceLine, sourceFunc );
        }

        // Keep track of the original size
        unsigned int originalReportedSize = au->reportedSize;

        // Do the reallocation
        void   *oldReportedAddress = reportedAddress;
        unsigned int newActualSize = calculateActualSize(reportedSize);
        void   *newActualAddress   = 0;

        newActualAddress = realloc(au->actualAddress, newActualSize);
        assert(newActualAddress);

        if (!newActualAddress) 
        {
            log("û�и�����ڴ����");
            return 0;
        }

        // Remove this allocation from our stats (we'll add the new reallocation again later)
        stats.totalReportedMemory -= au->reportedSize;
        stats.totalActualMemory   -= au->actualSize;

        // Update the allocation with the new information

        au->actualSize        = newActualSize;
        au->actualAddress     = newActualAddress;
        au->reportedSize      = calculateReportedSize(newActualSize);
        au->reportedAddress   = calculateReportedAddress(newActualAddress);
        au->allocationType    = reallocationType;
        au->sourceLine        = sourceLine;
        au->threadID          = threadID;
        au->time              = (unsigned int)time(0);

        if(sourceFile)
            au->sourceFile = sourceFile;
        else
            au->sourceFile = "??";

        if( sourceFunc )
            au->sourceFunc = sourceFunc;
        else
            au->sourceFunc = "??";

        // The reallocation may cause the address to change, so we should 
        // relocate our allocation unit within the hash table

        unsigned int hashIndex = (unsigned int) -1;
        if( oldReportedAddress != au->reportedAddress )
        {
            // Remove this allocation unit from the hash table
            {
                unsigned int hashIndex = ((unsigned int) oldReportedAddress >> 4) & (hashSize - 1);
                if( hashTable[hashIndex] == au )
                {
                    hashTable[hashIndex] = hashTable[hashIndex]->next;
                }
                else
                {
                    if (au->prev)
                        au->prev->next = au->next;
                    if (au->next)
                        au->next->prev = au->prev;
                }
            }

            // Re-insert it back into the hash table
            hashIndex = ((unsigned int) au->reportedAddress >> 4) & (hashSize - 1);
            if (hashTable[hashIndex]) 
                hashTable[hashIndex]->prev = au;
            au->next = hashTable[hashIndex];
            au->prev = 0;
            hashTable[hashIndex] = au;
        }

        // Account for the new allocatin unit in our stats
        stats.totalReportedMemory += au->reportedSize;
        stats.totalActualMemory   += au->actualSize;
        if (stats.totalReportedMemory > stats.peakReportedMemory) 
            stats.peakReportedMemory = stats.totalReportedMemory;
        if (stats.totalActualMemory   > stats.peakActualMemory)   
            stats.peakActualMemory   = stats.totalActualMemory;
        unsigned int deltaReportedSize = reportedSize - originalReportedSize;
        if( deltaReportedSize > 0 )
        {
            stats.accumulatedReportedMemory += deltaReportedSize;
            stats.accumulatedActualMemory += deltaReportedSize;
        }

        // Prepare the allocation unit for use (wipe it with recognizable 
        // garbage)
        wipeWithPattern( au, unusedPattern, originalReportedSize );

        // If you hit this assert, then something went wrong, because the 
        // allocation unit was properly validated PRIOR to the reallocation. 
        // This should not happen.
        assert( _validateAlloc(au) );

        // Validate every single allocated unit in memory
        if( alwaysValidateAll ) 
            _validateAllAllocs();

        // Resetting the globals insures that if at some later time, somebody 
        // calls our memory manager from an unknown source (i.e. they didn't 
        // include our H file) then we won't think it was the last allocation.
        resetGlobals();

        // Return the (reported) address of the new allocation unit
        return au->reportedAddress;
    }
    catch(const char *err)
    {
        // Deal with the errors
        log("[!] %s", err);
        resetGlobals();

        return 0;
    }
}

//�ͷ��ڴ�
void CMemoryManager::_dllocMem(const char *         _sourceFile, 
                               const unsigned int   _sourceLine,
                               const char *         _sourceFunc, 
                               const unsigned int   deallocationType, 
                               const void *         reportedAddress)
{
    CLockGuard guard(*m_locker);

	//g_TestHulingyun.Delete((void *)reportedAddress);

    if (g_memManagerEnded)
        return;
    
    unsigned threadID = GetCurrentThreadId();

    // early-out for 0
    if (!reportedAddress)
        return;

    try
    {
        sAllocUnit *au = findAllocUnit( reportedAddress );

        //�ͷŵ�ָ����Ч
        assert(au != 0 && "�ظ�ɾ������ɾ�������ڴ������������ڴ�");

        if (au == 0) 
        {
            throw "Request to deallocate RAM that was never allocated";
        }

        __CheckBreak(au);

        assert(_validateAlloc(au));

        // If you hit this assert, then this deallocation was made from a 
        // source that isn't setup to use this memory tracking software, use 
        // the stack frame to locate the source and include our H file.
        assert(deallocationType != m_alloc_unknown);

        if( deallocationType == m_alloc_unknown )
        {
            log( "[!] Allocation-deallocation mismatch in %s(%d)::%s :", sourceFile, sourceLine, sourceFunc );
            _DumpAllocUnit( au, "  " );
        }

        //������ͷŵĺ�����ƥ��
        /*assert(
            (deallocationType == m_alloc_delete       && au->allocationType == m_alloc_new      ) ||
            (deallocationType == m_alloc_delete_array && au->allocationType == m_alloc_new_array) ||
            (deallocationType == m_alloc_free         && au->allocationType == m_alloc_malloc   ) ||
            (deallocationType == m_alloc_free         && au->allocationType == m_alloc_calloc   ) ||
            (deallocationType == m_alloc_free         && au->allocationType == m_alloc_realloc  ) ||
            (deallocationType == m_alloc_unknown                                                ) );*/
        if( 
            !(
            (deallocationType == m_alloc_delete       && au->allocationType == m_alloc_new      ) ||
            (deallocationType == m_alloc_delete_array && au->allocationType == m_alloc_new_array) ||
            (deallocationType == m_alloc_free         && au->allocationType == m_alloc_malloc   ) ||
            (deallocationType == m_alloc_free         && au->allocationType == m_alloc_calloc   ) ||
            (deallocationType == m_alloc_free         && au->allocationType == m_alloc_realloc  ) ||
            (deallocationType == m_alloc_unknown                                                ) ) )
        {
            log( "[!] Allocation-deallocation mismatch in %s(%d)::%s :", sourceFile, sourceLine, sourceFunc );
            _DumpAllocUnit( au, "  " );
        }

        //����Ƿ�����ͬ1���߳��ڵ�
        if (au->threadID != threadID && checkThread)
        {
            log("[!] �ڴ�������ͷŲ���ͬ1���߳� %s(%d)::%s :", sourceFile, sourceLine, sourceFunc );
            DumpAllocUnit(au);
        }

        //ģ��relase�µ��ڴ����
        wipeWithPattern( au, releasedPattern );
        free(au->actualAddress);

        // Remove this allocation unit from the hash table
        unsigned int hashIndex = ((unsigned int) au->reportedAddress >> 4) & (hashSize - 1);

        if( hashTable[hashIndex] == au )
        {
            hashTable[hashIndex] = au->next;
        }
        else
        {
            if (au->prev)
                au->prev->next = au->next;
            if (au->next)
                au->next->prev = au->prev;
        }

        // Remove this allocation from our stats
        stats.totalReportedMemory -= au->reportedSize;
        stats.totalActualMemory   -= au->actualSize;
        stats.totalAllocUnitCount--;

        // Add this allocation unit to the front of our reservoir of unused allocation units
        memset( au, 0, sizeof(sAllocUnit) );
        au->next = reservoir;
        reservoir = au;

        // Resetting the globals insures that if at some later time, somebody 
        // calls our memory manager from an unknown source (i.e. they didn't 
        // include our H file) then we won't think it was the last allocation.
        resetGlobals();

		if (deallocationType == m_alloc_malloc)
		{
			//g_TestHulingyun.Delete((void *)reportedAddress);
		}

        // Validate every single allocated unit in memory
        if( alwaysValidateAll )
            _validateAllAllocs();
    }
    catch(const char *err)
    {
        // Deal with errors
        log("[!] %s", err);
        resetGlobals();
    }
}

//����ڴ��Ƿ�Ϸ�
bool CMemoryManager::ValidateAddr( const void *reportedAddress )
{
    CLockGuard guard(*m_locker);

    sAllocUnit* pUnit = findAllocUnit(reportedAddress);

    if (0 == pUnit)
        return false;

    if (!_validateAlloc(pUnit))
        return false;

    return true;
}

bool CMemoryManager::ValidateAll(void)
{
    CLockGuard guard(*m_locker);

    for( unsigned int i = 0; i < hashSize; ++i)
    {
        sAllocUnit *ptr = hashTable[i];

        while( ptr )
        {
            if (!ValidateAddr(ptr->reportedAddress))
                return false;

            ptr = ptr->next;
        }
    }

    return true;
}

void CMemoryManager::SetMark(unsigned int mark)
{
    for( unsigned int i = 0; i < hashSize; ++i)
    {
        sAllocUnit *ptr = hashTable[i];

        while( ptr )
        {
            if (0 == ptr->mark)
                ptr->mark = mark;

            ptr = ptr->next;
        }
    }
}

void CMemoryManager::DumpMark(unsigned int mark,const char* fileName)
{
    DumpMemReport(fileName,true,mark);
}

void CMemoryManager::DumpFunAllocSize(const char* fun)
{
    if (0 == fun)
        return;

    unsigned int totalReportedSize = 0;
    unsigned int totalActualSize = 0;

    CLockGuard guard(*m_locker);;

    log("fun       Size Reported      Size Actual \r\n");
    log("------ ---------- ---------- ---------- ---------- ---------- -------- ------- ------- -------------------- \r\n");

    for( unsigned int i = 0; i < hashSize; i++ )
    {
        sAllocUnit *ptr = hashTable[i];

        while( ptr )
        {
            if (0 != _stricmp(ptr->sourceFunc,fun))
                break;

            totalReportedSize += ptr->reportedSize;
            totalActualSize   += ptr->actualSize;

            ptr = ptr->next;
        }
    }

    log("%s        %d           %d \r\n",fun,totalReportedSize,totalActualSize);
}

void CMemoryManager::DumpFileAllocSize(const char* fun)
{
    if (0 == fun)
        return;

    unsigned int totalReportedSize = 0;
    unsigned int totalActualSize = 0;

    CLockGuard guard(*m_locker);;

    log("file       Size Reported      Size Actual \r\n");
    log("------ ---------- ---------- ---------- ---------- ---------- -------- ------- ------- -------------------- \r\n");

    for( unsigned int i = 0; i < hashSize; i++ )
    {
        sAllocUnit *ptr = hashTable[i];

        while( ptr )
        {
            if (0 != _stricmp(ptr->sourceFile,fun))
                break;

            totalReportedSize += ptr->reportedSize;
            totalActualSize   += ptr->actualSize;

            ptr = ptr->next;
        }
    }

    log("%s        %d           %d \r\n",fun,totalReportedSize,totalActualSize);
}

/************************************************************************/
/* ����ڴ��Ƿ�������ͨ���ڴ�ǰ��ı�ʶ��Ϣ
/************************************************************************/
bool CMemoryManager::_validateAlloc( const sAllocUnit *allocUnit )
{
    // Make sure the padding is untouched
    long *pre      = (long *)allocUnit->actualAddress;
    long *post     = (long *)((char *)allocUnit->actualAddress + allocUnit->actualSize - paddingSize * sizeof(long));
    bool errorFlag = false;

    for( unsigned int i = 0; i < paddingSize; i++, pre++, post++ )
    {
        //������
        if( *pre != (long)prefixPattern )
        {
            log("[!] A memory allocation unit was corrupt because of an underrun:");
            _DumpAllocUnit( allocUnit, "  " );
            errorFlag = true;
        }

        assert(*pre == (long) prefixPattern && "�ڴ渲��,���������");

        if (*post != (long) postfixPattern)
        {
            log("[!] A memory allocation unit was corrupt because of an overrun:");
            _DumpAllocUnit(allocUnit, "  ");
            errorFlag = true;
        }

        assert(*post == (long) postfixPattern && "�ڴ渲��,���������");
    }

    // Return the error status (we invert it, because a return of 'false' means error)
    return !errorFlag;
}

//�������
bool CMemoryManager::_validateAllAllocs(void)
{
    // Just go through each allocation unit in the hash table and count the ones that have errors
    unsigned int errors = 0;
    unsigned int allocCount = 0;

    for( unsigned int i = 0; i < hashSize; i++ )
    {
        sAllocUnit *ptr = hashTable[i];

        while(ptr)
        {
            allocCount++;
            if (!_validateAlloc(ptr)) 
                errors++;

            ptr = ptr->next;
        }
    }

    // Test for hash-table correctness
    if( allocCount != stats.totalAllocUnitCount )
    {
        log("[!] Memory tracking hash table corrupt!");
        errors++;
    }

    // If you hit this assert, then the internal memory (hash table) used by 
    // this memory tracking software is damaged! The best way to track this 
    // down is to use the alwaysLogAll flag in conjunction with STRESS_TEST 
    // macro to narrow in on the offending code. After running the application 
    // with these settings (and hitting this assert again), interrogate the 
    // memory.log file to find the previous successful operation. The 
    // corruption will have occurred between that point and this assertion.
    assert( allocCount == stats.totalAllocUnitCount );

    // If you hit this assert, then you've probably already been notified that 
    // there was a problem with a allocation unit in a prior call to 
    // validateAllocUnit(), but this assert is here just to make sure you know 
    // about it. :)
    assert( errors == 0 );

    // Log any errors
    if (errors) 
        log("[!] While validating all allocation units, %d allocation unit(s) were found to have problems", 
        errors );

    // Return the error status
    return errors != 0;
}

/************************************************************************/
/* �����Ԫ��Ϣ
/************************************************************************/
void CMemoryManager::_DumpAllocUnit(const sAllocUnit *allocUnit, const char *prefix,bool isUserAlloc)
{
    if (isUserAlloc)
    {
        //��⺯�������Ƿ��м�¼
        if(0 == _stricmp(allocUnit->sourceFile,"?"))
            return;
    }

    log("[I] %sAddress (reported): %010p",       prefix, allocUnit->reportedAddress);
    log("[I] %sAddress (actual)  : %010p",       prefix, allocUnit->actualAddress);
    log("[I] %sSize (reported)   : 0x%08X (%d)", prefix, allocUnit->reportedSize, 
        (allocUnit->reportedSize));
    log("[I] %sSize (actual)     : 0x%08X (%d)", prefix, allocUnit->actualSize, 
        (allocUnit->actualSize));
    log("[I] %sOwner id : %d     : %s(%d)::%s",  prefix, allocUnit->allocId,allocUnit->sourceFile, allocUnit->sourceLine, allocUnit->sourceFunc);
    log("[I] %sAllocation type   : %s",          prefix, allocationTypes[allocUnit->allocationType]);
}

void CMemoryManager::DumpAllocUnit(const void *reportedAddress)
{
    CLockGuard guard(*m_locker);

    sAllocUnit* pUnit = findAllocUnit(reportedAddress);

    if (0 == pUnit)
        return;

    _DumpAllocUnit(pUnit,"");
}

void CMemoryManager::DumpMemReport(const char *filename, const bool overwrite,unsigned int mark)
{
    CLockGuard guard(*m_locker);

    // Open the report file
    FILE *fp = 0;
	errno_t err = 0;

    if (overwrite)    
        err = fopen_s(&fp, filename, "w+b");
    else        
        err = fopen_s(&fp, filename, "ab");

	if (err != 0) 
        return;

    // Header
    char    timeString[25];
    memset(timeString, 0, sizeof(timeString));
    time_t  t = time(0);
    struct  tm tme;
	err = localtime_s(&tme, &t);

    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "|                                             Memory report for: %02d/%02d/%04d %02d:%02d:%02d                                     |\r\n", tme.tm_mon + 1, tme.tm_mday, tme.tm_year + 1900, tme.tm_hour, tme.tm_min, tme.tm_sec);
    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "\r\n");
    fprintf(fp, "\r\n");

    // Report summary
    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "|                                                           T O T A L S                                                            |\r\n");
    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "              Allocation unit count: %d\r\n", (stats.totalAllocUnitCount));
    fprintf(fp, "            Reported to application: %d\r\n", stats.totalReportedMemory);
    fprintf(fp, "         Actual total memory in use: %d\r\n", stats.totalActualMemory);
    fprintf(fp, "           Memory tracking overhead: %d\r\n", stats.totalActualMemory - stats.totalReportedMemory);
    fprintf(fp, "\r\n");

    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "|                                                            P E A K S                                                             |\r\n");
    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "              Allocation unit count: %d\r\n", stats.peakAllocUnitCount);
    fprintf(fp, "            Reported to application: %d\r\n", stats.peakReportedMemory);
    fprintf(fp, "                             Actual: %d\r\n", stats.peakActualMemory);
    fprintf(fp, "           Memory tracking overhead: %d\r\n", stats.peakActualMemory - stats.peakReportedMemory);
    fprintf(fp, "\r\n");

    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "|                                                      A C C U M U L A T E D                                                       |\r\n");
    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "              Allocation unit count: %d\r\n", stats.accumulatedAllocUnitCount);
    fprintf(fp, "            Reported to application: %d\r\n", stats.accumulatedReportedMemory);
    fprintf(fp, "                             Actual: %d\r\n", stats.accumulatedActualMemory);
    fprintf(fp, "\r\n");

    DumpAllocations(fp,mark);

    fclose(fp);
}

void *CMemoryManager::_op_new_sc( unsigned int reportedSize)
{
    if (reportedSize == 0) 
        reportedSize = 1;

    // ANSI says: loop continuously because the error handler could possibly free up some memory
    for(;;)
    {
        // Try the allocation
        void *ptr = _allocMem("?",0,"?", m_alloc_new, reportedSize);

        if( ptr )
        {
            return ptr;
        }

        std::new_handler nh = std::set_new_handler(0);
        std::set_new_handler(nh);

        // If there is an error handler, call it
        if (nh)
        {
            (*nh)();
        }
        // Otherwise, throw the exception
        else
        {
            throw std::bad_alloc();
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------
void *CMemoryManager::_op_new_vc( unsigned int reportedSize)
{
    if (reportedSize == 0) reportedSize = 1;

    for(;;)
    {
        // Try the allocation
        void    *ptr = _allocMem("?",0,"?", m_alloc_new_array, reportedSize);
        if( ptr )
        {
            return ptr;
        }

        std::new_handler    nh = std::set_new_handler(0);
        std::set_new_handler(nh);

        // If there is an error handler, call it
        if (nh)
        {
            (*nh)();
        }

        // Otherwise, throw the exception
        else
        {
            throw std::bad_alloc();
        }
    }
}

//delete
void CMemoryManager::_op_del_sc(void *reportedAddress)
{
    // ANSI says: delete & delete[] allow 0 pointers (they do nothing)
    if( reportedAddress )
        _dllocMem(sourceFile, sourceLine, sourceFunc, m_alloc_delete, reportedAddress);

    resetGlobals();
}

/************************************************************************/
/* ɾ������ָ��
/************************************************************************/
void CMemoryManager::_op_del_vc(void *reportedAddress)
{
    // ANSI says: delete & delete[] allow 0 pointers (they do nothing)
    if (reportedAddress) 
        _dllocMem(
        sourceFile, 
        sourceLine, 
        sourceFunc, 
        m_alloc_delete_array, 
        reportedAddress);

    resetGlobals();
}

CMemoryManager::CMemoryManager(void):
    sourceFile("??"),
    sourceFunc("??"),
    currentAllocationCount(0),
    sourceLine(0),
    reservoirBuffer(0),
    reservoirBufferSize(0),
    reservoir(0),
    m_isSelDumped(false),
    m_isSelDumpEnabled(false)
{
    hashTable = (sAllocUnit**)malloc(sizeof(sAllocUnit*) * hashSize);

    m_pDumper = &m_dumper;

    ClearAllBreaks();
    m_locker = new(m_lockerBuf) CCSLock;
    SetDumpTo(0);
    memset(hashTable,0,sizeof(sAllocUnit*) * hashSize);
}

CMemoryManager::~CMemoryManager(void)
{
    if (m_isSelDumped && m_isSelDumpEnabled)
    {
        CMemoryManager::Instance()->SetMark(LEAK_MARK);
    }

    CLockGuard guard(*m_locker);

    DumpLeakReport();

    g_memManagerEnded = true;

    free(hashTable);
}

void CMemoryManager::SetDumpTo(CMemLogHelper* pDumper)
{
    m_pDumper = pDumper;
	
	if (0 == m_pDumper)
	{
		m_pDumper = &m_dumper;
	}
}

void CMemoryManager::__CheckBreak(sAllocUnit* allocUnit)
{
    if (0 == allocUnit)
        return;

    if (0 != m_breakOnMark && allocUnit->mark == m_breakOnMark)
    {
        DebugBreak();
    }

    if (0 != m_breakOnAllocId && allocUnit->allocId == m_breakOnAllocId)
    {
        DebugBreak();
    }

    if (0 != m_breakOnSize && allocUnit->reportedSize == m_breakOnSize)
    {
        DebugBreak();
    }

    if (0 != m_breakOnSizeMax || 0 != m_breakOnSizeMin)
    {
        if (0 != m_breakOnSizeMax && allocUnit->reportedSize <= m_breakOnSizeMax)
        {
            DebugBreak();
        }

        if (0 != m_breakOnSizeMin && allocUnit->reportedSize >= m_breakOnSizeMin)
        {
            DebugBreak();
        }
    }

    if (m_breakOnFunc != ""  && m_breakOnFunc == allocUnit->sourceFunc)
    {
        DebugBreak();
    }
}

#if defined POWER_MEMORY_MANAGER || defined POWER_MEMORY_ALLOC

#ifdef POWER_MEMORY_MANAGER
void* operator new(size_t size, const char* file,const unsigned int line,const char* fun)
{
    return CMemoryManager::Instance()->_allocMem(file,line,fun,m_alloc_new,size);
}

void* operator new[](size_t size,const char* file,const unsigned int line,const char* fun)
{
    return CMemoryManager::Instance()->_allocMem(file,line,fun,m_alloc_new_array,size);
}

void  operator delete(void* ptr,const char* file,const unsigned int,const char* fun)
{ 
    return operator delete(ptr);
}

void  operator delete[](void* ptr,const char* file,const unsigned int,const char* fun)
{
    return operator delete[](ptr);
}

#endif

void* operator new(size_t reportedSize)
{
#ifdef POWER_MEMORY_MANAGER
    return CMemoryManager::Instance()->_op_new_sc(reportedSize);
#else
    return malloc(reportedSize);
#endif
}

void* operator new[](size_t reportedSize)
{
#ifdef POWER_MEMORY_MANAGER
    return CMemoryManager::Instance()->_op_new_vc( reportedSize);
#else
    return malloc(reportedSize);
#endif
}

void operator delete(void *reportedAddress)
{
#ifdef POWER_MEMORY_MANAGER
    CMemoryManager::Instance()->_op_del_sc( reportedAddress);
#else
    free(reportedAddress);
#endif
}

void operator delete[](void *reportedAddress)
{
#ifdef POWER_MEMORY_MANAGER
    CMemoryManager::Instance()->_op_del_vc( reportedAddress);
#else
    free(reportedAddress);
#endif
}

#endif

#ifdef VIRTUAL_ALLOC

#include <hash_map>
#include <vector>

#include <assert.h>
#include <stack>
#define TOTAL_ALLOC_PAGE 32768

//��Ϊһ�η�����ڴ���1��ҳ�棬����һ��ֻ�ܼ�����������
#define DETECT_UP_OVERWRITTEN

//�ڴ�أ�һ�������������е��ڴ�
class CVirtualAllocPool
{
public:
    CVirtualAllocPool(void):
      m_curPage(0)
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        m_pageSize = sysInfo.dwPageSize;

        m_pBaseAddr = (char*)VirtualAlloc(0,m_pageSize * TOTAL_ALLOC_PAGE,MEM_COMMIT,PAGE_READWRITE);
        assert(m_pBaseAddr);
    }

    ~CVirtualAllocPool(void)
    {
        if (0 != m_pBaseAddr)
            VirtualFree(m_pBaseAddr,m_pageSize * TOTAL_ALLOC_PAGE,MEM_DECOMMIT);

        m_pBaseAddr = 0;
    }

    DWORD GetPageSize(void) const {return m_pageSize;}

    //��ȡ�ڴ�ҳ��ַ,������ʵ�ʻ�ȡ��ҳ��
    char* AllocPage(int& count)
    {
        if (m_curPage >= TOTAL_ALLOC_PAGE)
            return 0;

        int remainPage = TOTAL_ALLOC_PAGE - m_curPage;

        if (count > remainPage)
            return 0;

        char* pAddr = m_pBaseAddr + m_curPage * m_pageSize;
        m_curPage += count;

        return pAddr;
    }
private:
    char* m_pBaseAddr;
    int   m_curPage;
    DWORD m_pageSize;
};

//�������е�pool
class CVirtualAllocPoolMgr
{
public:
    CVirtualAllocPoolMgr(void):m_pActivePool(0) {}

    ~CVirtualAllocPoolMgr(void)
    {
        for (size_t i = 0; i < m_pools.size(); ++i)
        {
            delete m_pools[i];
        }

        m_pools.clear();
    }

    //only one instance allowed
    static CVirtualAllocPoolMgr* Instance(void)
    {
        static CVirtualAllocPoolMgr local;
        return &local;
    }
    
    DWORD GetPageSize(void) 
    {
        __Check();
        return m_pActivePool->GetPageSize();
    }

    //��ȡ�ڴ�ҳ��ַ,������ʵ�ʻ�ȡ��ҳ��
    char* AllocPage(int& count)
    {
        __Check();

        char* pData = m_pActivePool->AllocPage(count);

        if (0 != pData)
            return pData;

        //��⵱ǰ������pool����
        for (size_t i = 0; i < m_pools.size(); ++i)
        {
            pData = m_pools[i]->AllocPage(count);

            if (0 != pData)
            {
                m_pActivePool = m_pools[i];
                return pData;
            }
        }

        //��չһ���µ�pool
        m_pools.push_back(new CVirtualAllocPool);
        m_pActivePool = m_pools.back();

        return m_pActivePool->AllocPage(count);
    }
private:
    void __Check(void)
    {
        if (0 == m_pools.size())
        {
            m_pools.push_back(new CVirtualAllocPool);
        }

        if (0 == m_pActivePool)
        {
            m_pActivePool = m_pools[0];
        }
    }

    std::vector<CVirtualAllocPool*> m_pools;
    CVirtualAllocPool* m_pActivePool;
};

template<int _count>
class CVirtualPageAllocUnit
{
public:
    CVirtualPageAllocUnit(int pagePerUnit):
      _pagePerUnit(pagePerUnit)
      {

      }

      ~CVirtualPageAllocUnit(void)
      {

      }

      //�����ڴ�
      char* Alloc(void)
      {
          if (0 == m_freePool.size())
          {
              __DoAllocPage();
          }

          if (0 == m_freePool.size())
          {
              assert(0 && "�޷��������������ڴ�");
              return 0;
          }

          char* ptr = m_freePool.top();
          m_freePool.pop();

          //���õ�ǰ�ڴ�Ϊ��д
          char* pAddr = (char*)VirtualAlloc(ptr,GetSize(),MEM_COMMIT,PAGE_READWRITE);
          assert(pAddr == ptr);

          return ptr;
      }

      //�ջ��ڴ�
      void Free(char* ptr)
      {
          if (0 == ptr)
              return;

          //!����ָ�뱻�ͷ��ˣ���Ӧ���ٱ�ʹ�û����
          DWORD old;

          if (!VirtualProtect(ptr,GetSize(),PAGE_NOACCESS,&old))
          {
              assert(0 && "�����ڴ�ֻ��ʧ��");
          }

          m_freePool.push(ptr);
      }

      DWORD GetSize(void) const {return _pagePerUnit * CVirtualAllocPoolMgr::Instance()->GetPageSize();}
private:
    //�������Ŀռ�,�����˾Ͳ����ˡ�
    void __DoAllocPage(void)
    {
        CVirtualAllocPoolMgr& pool = *CVirtualAllocPoolMgr::Instance();

        //ÿ���ڴ���Ҫpage��Ŀ
        int pageCount   = _count + 1 + _pagePerUnit * _count;
        char* pBaseAddr = pool.AllocPage(pageCount);

        if(0 == pBaseAddr)
            return;

        //��ʼ��page,ʹ��ʵ��ʹ�õ��ڴ汻2��ֻ���ڴ����,�������Լ�����磬�������ͷ�ָ���ʱ���ж�
        DWORD old;
        int iStep = 0;

        for (int i = 0; i < pageCount; ++i)
        {    
            if (_pagePerUnit + 1 == iStep)
                iStep = 0;

            //if (0 == iStep)
            {
                if (!VirtualProtect(pBaseAddr + i * pool.GetPageSize(),pool.GetPageSize(),PAGE_NOACCESS,&old))
                //if (!VirtualProtect(pBaseAddr + i * pool.GetPageSize(),pool.GetPageSize(),PAGE_READONLY,&old))
                {
                    assert(0 && "�����ڴ�ֻ��ʧ��");
                    return;
                }
            }

            if (1 == iStep)
            {
                //this is ower start
                m_freePool.push(pBaseAddr + i * pool.GetPageSize());
            }

            iStep++;
        }
    }

    int _pagePerUnit;
    std::stack<char*> m_freePool;
};

//ÿ���ڴ浥Ԫ��ʼ�������Ŀ
#define INIT_ALLOC_COUNT 30

class CDumyLock
{
public:
    void Lock(){}
    void Unlock(){}
};

//��װvirtual alloc,�����ӿ��ڴ������ͷŵ��ٶ�
template<typename _lock>
class CVirtualAlloc
{
    //for simple use all initcount for all unit
    typedef CVirtualPageAllocUnit<INIT_ALLOC_COUNT> ALLOC_UNIT;
public:
    CVirtualAlloc(void) {}

    char* Alloc(unsigned int len)
    {
        //������䵥Ԫ�ı�����Ϣ
        unsigned int allocLen = len + 8;

        m_lock.Lock();

        int index = 0;
        ALLOC_UNIT* pUnit = __FindUnit(allocLen,index);

        assert(index < 65525);

        if (0 == pUnit)
        {
            m_lock.Unlock();
            return 0;
        }

        //�����ڴ�
        char* pData = pUnit->Alloc();

        if (0 == pData)
        {
            m_lock.Unlock();
            return 0;
        }

        DWORD memSize = pUnit->GetSize();
        assert(memSize >= allocLen);

        //�����ڴ����Ǽ���������������������ڴ��λ�ñ�ʶ�ŵ��ڴ��ǰ������
#ifdef DETECT_UP_OVERWRITTEN
        pData += 8;
#else
        pData = pData + memSize - len;
#endif  /*DETECT_UP_OVERWRITTEN*/

        __AddMark(pData,memSize,len,index);

        m_lock.Unlock();
        return pData;
    }

    void* Realloc(void* _pMem,unsigned int newlen)
    {
        if (0 == _pMem)
        {
            return Alloc(newlen);
        }

        m_lock.Lock();

        char* pMem = (char*)_pMem;

        pMem -= 8;

        unsigned short index = *((unsigned short*)pMem);
        pMem += 2;
        unsigned int   len   = *((unsigned int*)pMem);
        pMem += 4;

        if (pMem[0] != '&' || pMem[1] != '%')
        {
            assert(0 && "�������ݱ��ƻ�");
            return 0;
        }

        pMem += 2;

        if (len >= newlen)
            return pMem;

        //���·���1���ڴ�,������ԭʼ����
        char* pNewData = Alloc(newlen);
        
        if (0 == pNewData)
            return 0;

        memcpy(pNewData,pMem,len);

        //�ͷ�ԭ����
        Free(pMem);
        
        m_lock.Unlock();

        return pNewData;
    }

    void Free(void* _pMem)
    {
        if (0 == _pMem)
            return;

        char* pMem = (char*)_pMem;

        pMem -= 8;

        unsigned short index = *((unsigned short*)pMem);
        pMem += 2;
        unsigned int   len   = *((unsigned int*)pMem);
        pMem += 4;

        if (pMem[0] != '&' || pMem[1] != '%')
        {
            assert(0 && "�������ݱ��ƻ�");
            return;
        }

        pMem += 2;

        m_lock.Lock();

        if (index >= m_allocs.size())
        {
            assert(0 && "�������ݱ��ƻ�");
            m_lock.Unlock();
            return;
        }

        ALLOC_UNIT* pUnit = &m_allocs[index];

        if (len >= pUnit->GetSize())
        {
            assert(0 && "�������ݱ��ƻ�");
            m_lock.Unlock();
            return;
        }

        char* pPageStart = 0;

#ifdef DETECT_UP_OVERWRITTEN
        pPageStart = pMem - 8;
#else
        pPageStart = pMem + len - pUnit->GetSize();
#endif  /*DETECT_UP_OVERWRITTEN*/

        pUnit->Free(pPageStart);

        m_lock.Unlock();
    }

private:
    ALLOC_UNIT* __FindUnit(unsigned int len,int &index)
    {
        DWORD pageSize = CVirtualAllocPoolMgr::Instance()->GetPageSize();

        unsigned int pageCount = len / pageSize;

        if (pageCount * pageSize != len)
            pageCount++;

        while((int)m_allocs.size() < pageCount)
        {
            m_allocs.push_back(ALLOC_UNIT(m_allocs.size() + 1));
        }

        index = pageCount - 1;

        return &m_allocs[index];
    }

    void __AddMark(char* pData,int pageSize,int len,int index)
    {
        char* pFlagStart = pData - 8;
        *((unsigned short*)pFlagStart) = (unsigned short)index;
        pFlagStart+=2;
        *((unsigned int*)pFlagStart) = len;
        pFlagStart+=4;

        pFlagStart[0] = '&';
        pFlagStart[1] = '%';
    }

    _lock m_lock;

    //�ڴ���䵥Ԫ
    std::vector<ALLOC_UNIT> m_allocs;
};

CVirtualAlloc<CCSLock>* g_virtualAlloc;

void* _VirtualAlloc(unsigned int len)
{   
    if (0 == g_virtualAlloc)
        g_virtualAlloc = new CVirtualAlloc<CCSLock>();


    return g_virtualAlloc->Alloc(len);
    //return malloc(len);
}

void* _VirtualRealloc(void* ptr,unsigned int len)
{
    if (0 == g_virtualAlloc)
        g_virtualAlloc = new CVirtualAlloc<CCSLock>();

    return g_virtualAlloc->Realloc(ptr,len);
}

void  _VirtualFree(void* ptr)
{
    if (0 == g_virtualAlloc)
        g_virtualAlloc = new CVirtualAlloc<CCSLock>();

    g_virtualAlloc->Free(ptr);
    //free(ptr);
}

#endif