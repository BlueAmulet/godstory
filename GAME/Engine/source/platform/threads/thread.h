//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/types.h"
#include "core/tVector.h"
#include "platform/threads/mutex.h"

#ifndef _PLATFORM_THREADS_THREAD_H_
#define _PLATFORM_THREADS_THREAD_H_

// Forward ref used by platform code
class PlatformThreadData;

// Typedefs
typedef void (*ThreadRunFunction)(void *data);

class Thread
{
protected:
   PlatformThreadData*  mData;

   /// Used to signal threads need to stop. 
   /// Threads set this flag to false in start()
   bool shouldStop;

public:
   /// If set, the thread will delete itself once it has finished running.
   bool autoDelete;

   /// Create a thread.
   /// @param func The starting function for the thread.
   /// @param arg Data to be passed to func, when the thread starts.
   /// @param start_thread Whether to start the Thread immediately.
   Thread(ThreadRunFunction func = 0, void *arg = 0, bool start_thread = true, bool autodelete = false);
   
   /// Destroy a thread.
   /// The thread MUST be allowed to exit before it is destroyed.
   virtual ~Thread();

   /// Start a thread. 
   /// Sets shouldStop to false and calls run() in a new thread of execution.
   void start();

   /// Ask a thread to stop running.
   void stop() { shouldStop = true; }

   /// Block until the thread stops running.
   bool join();

   /// Threads may call checkForStop() periodically to check if they've been 
   /// asked to stop. As soon as checkForStop() returns true, the thread should
   /// clean up and return.
   bool checkForStop() {  return shouldStop; }

   /// Run the Thread's entry point function.
   /// Override this method in a subclass of Thread to create threaded code in
   /// an object oriented way, and without passing a function ptr to Thread().
   /// Also, you can call this method directly to execute the thread's
   /// code in a non-threaded way.
   virtual void run(void *arg = 0);

   /// Returns true if the thread is running.
   bool isAlive();

   /// Returns the platform specific thread id for this thread.
   U32 getId();
};

class ThreadManager 
{
   static ThreadManager* singleton()
   {
      static ThreadManager* man = NULL;
      if(!man) man = new ThreadManager;
      AssertISV(man, "Thread manager doesn't exist.");
      return man;
   }

   Vector<Thread*> threadPool;
   Mutex poolLock;
   
public:

   /// Returns true if threadId is the same as the calling thread's id.
   static bool isCurrentThread(U32 threadId);

   /// Returns true if the 2 thread ids represent the same thread. Some thread
   /// APIs return an opaque object as a thread id, so the == operator cannot
   /// reliably compare thread ids.
   // this comparator is needed by pthreads and ThreadManager.
   static bool compare(U32 threadId_1, U32 threadId_2);
      
   /// Returns the platform specific thread id of the calling thread. Some 
   /// platforms do not guarantee that this ID stays the same over the life of 
   /// the thread, so use ThreadManager::compare() to compare thread ids.
   static U32 getCurrentThreadId();
   
   /// Each thread should add itself to the thread pool the first time it runs.
   static void addThread(Thread* thread)
   {
      ThreadManager &manager = *singleton();
      manager.poolLock.lock();
      Thread *alreadyAdded = getThreadById(thread->getId());
      if(!alreadyAdded)
         manager.threadPool.push_back(thread);
      manager.poolLock.unlock();
   }

   static void removeThread(Thread* thread)
   {
      ThreadManager &manager = *singleton();
      manager.poolLock.lock();
      
      U32 threadID = thread->getId();
      for(U32 i = 0;i < manager.threadPool.size();++i)
      {
         if(manager.threadPool[i]->getId() == threadID)
         {
            manager.threadPool.erase(i);
            break;
         }
      }
      
      manager.poolLock.unlock();
   }
   
   /// Searches the pool of known threads for a thread whose id is equivalent to
   /// the given threadid. Compares thread ids with ThreadManager::compare().
   static Thread* getThreadById(U32 threadid)
   {
      AssertFatal(threadid != 0, "ThreadManager::getThreadById() Searching for a bad thread id.");
      Thread* ret = NULL;
      
      singleton()->poolLock.lock();
      Vector<Thread*> &pool = singleton()->threadPool;
      for( S32 i = pool.size() - 1; i >= 0; i--)
      {
         Thread* p = pool[i];
         if(compare(p->getId(), threadid))
         {
            ret = p;
            break;
         }
      }
      singleton()->poolLock.unlock();
      return ret;
   }

   static Thread* getCurrentThread()
   {
      return getThreadById(ThreadManager::getCurrentThreadId());
   }
};

inline bool ThreadManager::isCurrentThread(U32 threadId)
{
   U32 current = getCurrentThreadId();
   return compare(current, threadId);
}

#endif // _PLATFORM_THREADS_THREAD_H_
