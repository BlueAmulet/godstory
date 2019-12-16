//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/profiler.h"
#include "core/stringTable.h"
#include <stdlib.h> // gotta use malloc and free directly
#include "console/console.h"
#include "core/tVector.h"
#include "core/fileStream.h"
#include "platform/threads/thread.h"
#include "core/frameAllocator.h"
#include "core/fileStream.h"

#if defined(POWER_OS_WIN32)
#include<Windows.h> // for SetThreadAffinityMask
#endif

#if defined(POWER_OS_MAC)
#include "platformMacCarb.h"
#endif

#ifdef POWER_ENABLE_PROFILER
ProfilerNodeData *ProfilerNodeData::sRootList = NULL;
Profiler *gProfiler = NULL;

#ifdef POWER_MULTITHREAD
U32 gMainThread = 0;
#endif

// Uncomment the following line to enable a debugging aid for mismatched profiler blocks.
//#define POWER_PROFILER_DEBUG

// Machinery to record the stack of node names, as a debugging aid to find
// mismatched PROFILE_START and PROFILE_END blocks. We profile from the 
// beginning to catch profile block errors that occur when PowerEngine is starting up.
#ifdef POWER_PROFILER_DEBUG
Vector<StringTableEntry> gProfilerNodeStack;
#define POWER_PROFILE_AT_ENGINE_START true
#define PROFILER_DEBUG_PUSH_NODE( nodename ) \
   gProfilerNodeStack.push_back( nodename );
#define PROFILER_DEBUG_POP_NODE() \
   gProfilerNodeStack.pop_back();
#else
#define POWER_PROFILE_AT_ENGINE_START false
#define PROFILER_DEBUG_PUSH_NODE( nodename ) ;
#define PROFILER_DEBUG_POP_NODE() ;
#endif

//把获取时间的函数简化成只考虑win平台
LARGE_INTEGER g_freq;
LARGE_INTEGER g_initial;

void gResetBaseTime()
{
    QueryPerformanceFrequency(&g_freq);
    QueryPerformanceCounter(&g_initial);
}

float gGetCurrentTimeInSec()
{
    static bool bFirst = true;

    if (bFirst)
    {
        gResetBaseTime();
        bFirst = false;
    }

    static LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (float)((long double)(counter.QuadPart - g_initial.QuadPart) / (long double) g_freq.QuadPart);
}

Profiler::Profiler()
{
   mMaxStackDepth = MaxStackDepth;
   mCurrentHash = 0;
   mRecordFrames = -1;

   mCurrentProfilerData = (ProfilerData *) malloc(sizeof(ProfilerData));
   mCurrentProfilerData->pNode = NULL;
   mCurrentProfilerData->mNextProfilerData = NULL;
   mCurrentProfilerData->mNextHash = NULL;
   mCurrentProfilerData->mParent = NULL;
   mCurrentProfilerData->mNextSibling = NULL;
   mCurrentProfilerData->mFirstChild = NULL;
   mCurrentProfilerData->mHash = 0;
   mCurrentProfilerData->mSubDepth = 0;
   mCurrentProfilerData->mInvokeCount = 0;
   mCurrentProfilerData->mTotalTime = 0;
   mCurrentProfilerData->mSubTime = 0;

#ifdef POWER_ENABLE_PROFILE_PATH   
   mCurrentProfilerData->mPath = "";
#endif
   mRootProfilerData = mCurrentProfilerData;

   for(U32 i = 0; i < ProfilerData::HashTableSize; i++)
      mCurrentProfilerData->mChildHash[i] = 0;

   mProfileList = NULL;

   mEnabled = POWER_PROFILE_AT_ENGINE_START;   
   mNextEnable = POWER_PROFILE_AT_ENGINE_START;
   mStackDepth = 0;
   gProfiler = this;
   mDumpToConsole   = false;
   mDumpToFile      = false;
   mDumpToBinaryFile = false;
   mDumpFileName[0] = '\0';

#ifdef POWER_MULTITHREAD
   gMainThread = ThreadManager::getCurrentThreadId();
#endif

   mStartTime = gGetCurrentTimeInSec();
}

Profiler::~Profiler()
{
    reset();
    free(mRootProfilerData);
    gProfiler = NULL;
}

void Profiler::reset()
{
   mEnabled = false; // in case we're in a profiler call.
   mRecordFrames = -1;

   while(mProfileList)
   {
       ProfilerData* next = mProfileList->mNextProfilerData;
       free(mProfileList);
       mProfileList = next;
   }

   for(ProfilerNodeData *walk = ProfilerNodeData::sRootList; walk; walk = walk->mNextNode)
   {
      walk->mTotalTime = 0;
      walk->mSubTime = 0;
      walk->mTotalInvokeCount = 0;
   }

   mCurrentProfilerData = mRootProfilerData;
   mCurrentProfilerData->mFirstChild = 0;
   for(U32 i = 0; i < ProfilerData::HashTableSize; i++)
      mCurrentProfilerData->mChildHash[i] = 0;
   mCurrentProfilerData->mInvokeCount = 0;
   mCurrentProfilerData->mTotalTime = 0;
   mCurrentProfilerData->mSubTime = 0;
   mCurrentProfilerData->mSubDepth = 0;
}

static Profiler aProfiler; // allocate the global profiler

ProfilerNodeData::ProfilerNodeData(const char *name)
{
   for(ProfilerNodeData *walk = sRootList; walk; walk = walk->mNextNode)
   {
      if(!dStrcmp(walk->mName, name))
         Platform::debugBreak();
   }

   mName              = name;
   mNameHash          = _StringTable::hashString(name);
   mNextNode          = sRootList;
   sRootList          = this;
   mTotalTime         = 0;
   mTotalInvokeCount  = 0;
   mEnabled           = true;
}

#ifdef POWER_ENABLE_PROFILE_PATH
const char * Profiler::getProfilePath()
{
#ifdef POWER_MULTITHREAD
   // Ignore non-main-thread profiler activity.
   if(ThreadManager::getCurrentThreadId() != gMainThread)
      return "[non-main thread]";
#endif

   return (mEnabled && mCurrentProfilerData) ? mCurrentProfilerData->mPath : "na";
}
#endif

#ifdef POWER_ENABLE_PROFILE_PATH
const char * Profiler::constructProfilePath(ProfilerData * pd)
{
   if (pd->mParent)
   {
      const bool saveEnable = gProfiler->mEnabled;
      gProfiler->mEnabled = false;

      const char * connector = " -> ";
      U32 len = dStrlen(pd->mParent->mPath);
      if (!len)
         connector = "";
      len += dStrlen(connector);
      len += dStrlen(pd->pNode->mName);

      U32 mark = FrameAllocator::getWaterMark();
      char * buf = (char*)FrameAllocator::alloc(len+1);
      dStrcpy(buf, len+1, pd->mParent->mPath);
      dStrcat(buf, len+1, connector);
      dStrcat(buf, len+1, pd->pNode->mName);
      const char * ret = StringTable->insert(buf);
      FrameAllocator::setWaterMark(mark);
      
      gProfiler->mEnabled = saveEnable;
      
      return ret;
   }
   return "root";
}
#endif

void Profiler::hashPush(ProfilerNodeData* pNodeAdded)
{
#ifdef POWER_MULTITHREAD
    // Ignore non-main-thread profiler activity.
    if(ThreadManager::getCurrentThreadId() != gMainThread)
        return;
#endif

    mStackDepth++;
    PROFILER_DEBUG_PUSH_NODE(pNodeAdded->mName);
    AssertFatal(mStackDepth <= mMaxStackDepth,"Stack overflow in profiler.  You may have mismatched PROFILE_START and PROFILE_ENDs");

    if(!mEnabled)
        return;
    
    //用来包装当前的pNodeAdded
    ProfilerData* pNewProfiler = NULL;

    if(!pNodeAdded->mEnabled || mCurrentProfilerData->pNode == pNodeAdded)
    {
        mCurrentProfilerData->mSubDepth++;
        return;
    }

    if(!pNewProfiler)
    {
        // first see if it's in the hash table...
        U32 index    = pNodeAdded->mNameHash & (ProfilerData::HashTableSize - 1);
        pNewProfiler = mCurrentProfilerData->mChildHash[index];

        while(pNewProfiler)
        {
            if(pNewProfiler->pNode == pNodeAdded)
                break;

            pNewProfiler = pNewProfiler->mNextHash;
        }

        if(!pNewProfiler)
        {
            pNewProfiler = (ProfilerData *)malloc(sizeof(ProfilerData));
            memset(pNewProfiler,0,sizeof(ProfilerData));

            pNewProfiler->pNode             = pNodeAdded;
            pNewProfiler->mNextProfilerData = mProfileList;
            mProfileList = pNewProfiler;

            pNewProfiler->mNextHash = mCurrentProfilerData->mChildHash[index];
            mCurrentProfilerData->mChildHash[index] = pNewProfiler;
            
            pNewProfiler->mParent               = mCurrentProfilerData;
            pNewProfiler->mNextSibling          = mCurrentProfilerData->mFirstChild;
            mCurrentProfilerData->mFirstChild   = pNewProfiler;
            pNewProfiler->mFirstChild           = NULL;

            pNewProfiler->mHash         = pNodeAdded->mNameHash;
            pNewProfiler->mInvokeCount  = 0;
            pNewProfiler->mTotalTime    = 0;
            pNewProfiler->mSubTime      = 0;
            pNewProfiler->mSubDepth     = 0;

#ifdef POWER_ENABLE_PROFILE_PATH
            pNewProfiler->mPath = constructProfilePath(pNewProfiler);
#endif
        }
    }

    pNodeAdded->mTotalInvokeCount++;
    pNewProfiler->mInvokeCount++;
    pNewProfiler->mStartTime = gGetCurrentTimeInSec();
    mCurrentProfilerData = pNewProfiler;
}

void Profiler::enable(bool enabled)
{
   mNextEnable = enabled;
}

void Profiler::dumpToConsole()
{
   mDumpToConsole = true;
   mDumpToFile = false;
   mDumpToBinaryFile = false;
   mDumpFileName[0] = '\0';
}

void Profiler::dumpToFile(const char* fileName,bool isBinary)
{
   AssertFatal(dStrlen(fileName) < DumpFileNameLength, "Error, dump filename too long");
   mDumpToFile = true;
   mDumpToConsole = false;
   mDumpToBinaryFile = false;
    
   if (isBinary)
   {
       mDumpToFile = false;
       mDumpToBinaryFile = true;
   }
   else
   {
        mDumpToFile = true;
        mDumpToBinaryFile = false;
   }

   dStrcpy(mDumpFileName, sizeof(mDumpFileName), fileName);
}

void Profiler::hashPop(const char* name)
{
#ifdef POWER_MULTITHREAD
   // Ignore non-main-thread profiler activity.
   if(ThreadManager::getCurrentThreadId() != gMainThread)
      return;
#endif

   mStackDepth--;
   PROFILER_DEBUG_POP_NODE();
   AssertFatal(mStackDepth >= 0, "Stack underflow in profiler.  You may have mismatched PROFILE_START and PROFILE_ENDs");

   if(mEnabled)
   {
      if(mCurrentProfilerData->mSubDepth)
      {
         mCurrentProfilerData->mSubDepth--;
         return;
      }
      
	  if(dStrcmp(mCurrentProfilerData->pNode->mName, name) != 0)
	  {
		  char ErrorMessage[128];
		  dSprintf(ErrorMessage,sizeof(ErrorMessage),"错误的时间追踪器配对: %s",name);
		  AssertISV(0, ErrorMessage);
	  }

      F64 fElapsed = gGetCurrentTimeInSec() - mCurrentProfilerData->mStartTime;

      mCurrentProfilerData->mTotalTime        += fElapsed;
      mCurrentProfilerData->mParent->mSubTime += fElapsed; // mark it in the parent as well...
      mCurrentProfilerData->pNode->mTotalTime += fElapsed;

      if(mCurrentProfilerData->mParent->pNode)
         mCurrentProfilerData->mParent->pNode->mSubTime += fElapsed; // mark it in the parent as well...

      mCurrentProfilerData = mCurrentProfilerData->mParent;
   }

   if(mStackDepth == 0)
   {
      // apply the next enable...
      if(mDumpToConsole || mDumpToFile || mDumpToBinaryFile)
      {
         if (!mDumpToBinaryFile)
            dump();
         else
         {
            dumpToFileInBinary(mDumpFileName);
         }

         mCurrentProfilerData->mStartTime = gGetCurrentTimeInSec();
      }

      if(!mEnabled && mNextEnable)
         mCurrentProfilerData->mStartTime = gGetCurrentTimeInSec();

#if defined(POWER_OS_WIN32)
      // The high performance counters under win32 are unreliable when running on multiple
      // processors. When the profiler is enabled, we restrict PowerEngine to a single processor.
      if(mNextEnable != mEnabled)
      {

         /*if(mNextEnable)
         {
            Con::warnf("Warning: forcing the PowerEngine profiler thread to run only on cou 1.");
            SetThreadAffinityMask(GetCurrentThread(), 1);
         }
         else
         {
            Con::warnf("Warning: the PowerEngine profiler thread may now run on any cpu.");
            DWORD procMask;
            DWORD sysMask;
            GetProcessAffinityMask( GetCurrentProcess(), &procMask, &sysMask);
            SetThreadAffinityMask( GetCurrentThread(), procMask);
         }*/
      }
#endif

      mEnabled = mNextEnable;
   }
}

static S32 QSORT_CALLBACK rootDataCompare(const void *s1, const void *s2)
{
   const ProfilerNodeData *r1 = *((ProfilerNodeData **) s1);
   const ProfilerNodeData *r2 = *((ProfilerNodeData **) s2);

   if((r2->mTotalTime - r2->mSubTime) > (r1->mTotalTime - r1->mSubTime))
      return 1;

   return -1;
}

static void profilerDataDumpRecurse(ProfilerData *data, char *buffer, U32 bufferLen, F64 totalTime)
{
   // dump out this one:
   Con::printf("%7.3f %7.3f %8d %s%s",
         100 * data->mTotalTime / totalTime,
         100 * (data->mTotalTime - data->mSubTime) / totalTime,
         data->mInvokeCount,
         buffer,
         data->pNode ? data->pNode->mName : "ROOT" );

   data->mTotalTime = 0;
   data->mSubTime = 0;
   data->mInvokeCount = 0;

   buffer[bufferLen] = ' ';
   buffer[bufferLen+1] = ' ';
   buffer[bufferLen+2] = 0;
   // sort data's children...
   ProfilerData *list = NULL;
   while(data->mFirstChild)
   {
      ProfilerData *ins = data->mFirstChild;
      data->mFirstChild = ins->mNextSibling;
      ProfilerData **walk = &list;
      while(*walk && (*walk)->mTotalTime > ins->mTotalTime)
         walk = &(*walk)->mNextSibling;
      ins->mNextSibling = *walk;
      *walk = ins;
   }
   data->mFirstChild = list;

   while(list)
   {
      if(list->mInvokeCount)
         profilerDataDumpRecurse(list, buffer, bufferLen + 2, totalTime);
      list = list->mNextSibling;
   }

   buffer[bufferLen] = 0;
}

static void profilerDataDumpRecurseFile(ProfilerData *data, char *buffer, U32 bufferLen, F64 totalTime, FileStream& fws)
{
   char pbuffer[256];
   dSprintf(pbuffer, 255, "%7.3f %7.3f %8d %s%s\n",
         100 * data->mTotalTime / totalTime,
         100 * (data->mTotalTime - data->mSubTime) / totalTime,
         data->mInvokeCount,
         buffer,
         data->pNode ? data->pNode->mName : "ROOT" );
   fws.write(dStrlen(pbuffer), pbuffer);
   data->mTotalTime = 0;
   data->mSubTime = 0;
   data->mInvokeCount = 0;

   buffer[bufferLen] = ' ';
   buffer[bufferLen+1] = ' ';
   buffer[bufferLen+2] = 0;

   // sort data's children...
   ProfilerData *list = NULL;

   while(data->mFirstChild)
   {
      ProfilerData *ins = data->mFirstChild;
      data->mFirstChild = ins->mNextSibling;

      ProfilerData **walk = &list;

      while(*walk && (*walk)->mTotalTime > ins->mTotalTime)
         walk = &(*walk)->mNextSibling;

      ins->mNextSibling = *walk;
      *walk = ins;
   }

   data->mFirstChild = list;
   while(list)
   {
      if(list->mInvokeCount)
         profilerDataDumpRecurseFile(list, buffer, bufferLen + 2, totalTime, fws);
      list = list->mNextSibling;
   }
   buffer[bufferLen] = 0;
}

void Profiler::dump()
{
   bool enableSave = mEnabled;
   mEnabled = false;
   mStackDepth++;
   // may have some profiled calls... gotta turn em off.

   Vector<ProfilerNodeData *> rootVector;
   F64 totalTime = 0;

   for(ProfilerNodeData *walk = ProfilerNodeData::sRootList; walk; walk = walk->mNextNode)
   {
      totalTime += walk->mTotalTime - walk->mSubTime;
      rootVector.push_back(walk);
   }

   dQsort((void *) &rootVector[0], rootVector.size(), sizeof(ProfilerNodeData *), rootDataCompare);


   if (mDumpToConsole == true)
   {
      Con::printf("Profiler Data Dump:");
      Con::printf("Ordered by non-sub total time -");
      Con::printf("%%NSTime  %% Time  Invoke #  Name");

      for(U32 i = 0; i < rootVector.size(); i++)
      {
         Con::printf("%7.3f %7.3f %8d %s",
                     100 * (rootVector[i]->mTotalTime - rootVector[i]->mSubTime) / totalTime,
                     100 * rootVector[i]->mTotalTime / totalTime,
                     rootVector[i]->mTotalInvokeCount,
                     rootVector[i]->mName);

         rootVector[i]->mTotalInvokeCount = 0;
         rootVector[i]->mTotalTime = 0;
         rootVector[i]->mSubTime = 0;
      }

      Con::printf("");
      Con::printf("Ordered by stack trace total time -");
      Con::printf("%% Time  %% NSTime  Invoke #  Name");

      mCurrentProfilerData->mTotalTime = gGetCurrentTimeInSec() -mCurrentProfilerData->mStartTime;

      char depthBuffer[MaxStackDepth * 2 + 1];
      depthBuffer[0] = 0;
      profilerDataDumpRecurse(mCurrentProfilerData, depthBuffer, 0, totalTime);
      mEnabled = enableSave;
      mStackDepth--;
   }
   else if (mDumpToFile == true && mDumpFileName[0] != '\0')
   {
      FileStream fws;
      bool success = fws.open(mDumpFileName, FileStream::Write);
      AssertFatal(success, "Nuts! Cannot write profile dump to specified file!");
         char buffer[1024];

         dStrcpy(buffer, sizeof(buffer), "Profiler Data Dump:\n");
         fws.write(dStrlen(buffer), buffer);
         dStrcpy(buffer, sizeof(buffer), "Ordered by non-sub total time -\n");
         fws.write(dStrlen(buffer), buffer);
         dStrcpy(buffer, sizeof(buffer), "%%NSTime  %% Time  Invoke #  Name\n");
         fws.write(dStrlen(buffer), buffer);

         for(U32 i = 0; i < rootVector.size(); i++)
         {
            dSprintf(buffer, 1023, "%7.3f %7.3f %8d %s\n",
                     100 * (rootVector[i]->mTotalTime - rootVector[i]->mSubTime) / totalTime,
                     100 * rootVector[i]->mTotalTime / totalTime,
                     rootVector[i]->mTotalInvokeCount,
                     rootVector[i]->mName);
            fws.write(dStrlen(buffer), buffer);

            rootVector[i]->mTotalInvokeCount = 0;
            rootVector[i]->mTotalTime = 0;
            rootVector[i]->mSubTime = 0;
         }
         dStrcpy(buffer, sizeof(buffer), "\nOrdered by non-sub total time -\n");
         fws.write(dStrlen(buffer), buffer);
         dStrcpy(buffer, sizeof(buffer), "%%NSTime  %% Time  Invoke #  Name\n");
         fws.write(dStrlen(buffer), buffer);

      mCurrentProfilerData->mTotalTime = gGetCurrentTimeInSec() -mCurrentProfilerData->mStartTime;

      char depthBuffer[MaxStackDepth * 2 + 1];
      depthBuffer[0] = 0;
      profilerDataDumpRecurseFile(mCurrentProfilerData, depthBuffer, 0, totalTime, fws);
      mEnabled = enableSave;
      mStackDepth--;

      fws.close();
   }

   mDumpToConsole = false;
   mDumpToFile    = false;
   mDumpToBinaryFile = false;
   mDumpFileName[0] = '\0';
}

void Profiler::enableMarker(const char *marker, bool enable)
{
   reset();
   U32 markerLen = dStrlen(marker);
   if(markerLen == 0)
      return;
   bool sn = marker[markerLen - 1] == '*';
   for(ProfilerNodeData *data = ProfilerNodeData::sRootList; data; data = data->mNextNode)
   {
      if(sn)
      {
         if(!dStrncmp(marker, data->mName, markerLen - 1))
            data->mEnabled = enable;
      }
      else
      {
         if(!dStrcmp(marker, data->mName))
            data->mEnabled = enable;
      }
   }
}

void Profiler::dumpToFileInBinary(const char* fileName)
{
    if (0 == mRecordFrames)
        return;
    
    if (mRecordFrames > 0)
    {
        mRecordFrames--;

        if (0 == mRecordFrames)
            Con::printf("profile record completed!");
    }

    float totalTimeCost = gGetCurrentTimeInSec() - mStartTime;

    bool enableSave = mEnabled;
    mEnabled = false;
    mStackDepth++;

    FileStream fs;
    bool success = fs.open(fileName, FileStream::WriteAppend);

    if (!success)
        return;

    fs.setPosition(fs.getStreamSize());

    int id = 1;
    
    U32 count = 0;
    
    for(ProfilerNodeData* walk = ProfilerNodeData::sRootList; walk; walk = walk->mNextNode)
    {
        count++;
    }

    fs.write(count);

    //写入每个节点信息，并设定ID
    for(ProfilerNodeData* walk = ProfilerNodeData::sRootList; walk; walk = walk->mNextNode)
    {
        walk->mId = id++;

        fs.write(walk->mId);
        fs.writeString(walk->mName);
        fs.write(walk->mTotalTime);
        fs.write(totalTimeCost);
        fs.write(walk->mTotalInvokeCount);

        walk->mTotalTime = 0;
        walk->mTotalInvokeCount = 0;
    }

    //写入节点的层次关系
    ProfilerData* pRoot = mProfileList;

    while(pRoot)
    {
        if (0 == pRoot->pNode)
            fs.write(0);
        else
            fs.write(pRoot->pNode->mId);
        
        ProfilerData* pChild = pRoot->mFirstChild;
        
        count = 0;

        while(pChild)
        {
            count++;
            pChild = pChild->mNextSibling;
        }
        
        fs.write(count);

        pChild = pRoot->mFirstChild;

        while(pChild)
        {
            fs.write(pChild->pNode->mId);
            pChild = pChild->mNextSibling;
        }

        pRoot = pRoot->mNextProfilerData;
    }

    fs.write(-1);

    fs.flush();
    fs.close();

    mEnabled = enableSave;
    mStackDepth--;

    mStartTime = gGetCurrentTimeInSec();
}

ConsoleFunctionGroupBegin( Profiler, "Profiler functionality.");

ConsoleFunction(profilerMarkerEnable, void, 3, 3, "(string markerName, bool enable)")
{
   argc;
   if(gProfiler)
      gProfiler->enableMarker(argv[1], dAtob(argv[2]));
}

ConsoleFunction(profilerEnable, void, 2, 2, "(bool enable);")
{
   argc;
   if(gProfiler)
      gProfiler->enable(dAtob(argv[1]));
}

ConsoleFunction(profilerDump, void, 1, 1, "Dump the current state of the profiler.")
{
   argc; argv;
   if(gProfiler)
      gProfiler->dumpToConsole();
}

ConsoleFunction(profilerDumpToFile, void, 2, 2, "(string filename) Dump profiling stats to a file.")
{
   argc; argv;
   if(gProfiler)
      gProfiler->dumpToFile(argv[1]);
}

ConsoleFunction(profilerReset, void, 1, 1, "Resets the profiler, clearing all of its data.")
{
   argc; argv;
   if(gProfiler)
      gProfiler->reset();
}

//输出连续时间内的信息
ConsoleFunction(ProfilerBinaryDump, void, 3, 3, "Dump the current state of the profiler.")
{
    argc; argv;

    if(gProfiler)
    {
        ::DeleteFileA(argv[1]);

        gProfiler->enable(true);
        gProfiler->mRecordFrames = dAtoi(argv[2]);
        gProfiler->dumpToFile(argv[1],true);
    }
}

ConsoleFunctionGroupEnd( Profiler );

#endif
