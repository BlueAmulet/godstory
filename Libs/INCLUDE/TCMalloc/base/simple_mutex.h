/* Copyright (c) 2007, Google Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---
 * Author: Craig Silverstein.
 *
 * A simple mutex wrapper, supporting locks and read-write locks.
 * You should assume the locks are *not* re-entrant.
 *
 * To use: you should define the following macros in your configure.ac:
 *   ACX_PTHREAD
 *   AC_RWLOCK
 * The latter is defined in ../autoconf.
 *
 * This class is meant to be internal-only, so it's defined in the
 * global namespace.  If you want to expose it, you'll want to move
 * it to the Google namespace.
 *
 * NOTE: TryLock() is broken for NO_THREADS mode, at least in NDEBUG
 *       mode.
 */

#ifndef GOOGLE_SIMPLE_MUTEX_H__
#define GOOGLE_SIMPLE_MUTEX_H__

#include "config.h"           // to figure out pthreads support

#if defined(NO_THREADS)
  typedef int MutexType;      // to keep a lock-count
#elif defined(HAVE_PTHREAD) && defined(HAVE_RWLOCK)
  // Needed for pthread_rwlock_*.  If it causes problems, you could take it
  // out, but then you'd have to unset HAVE_RWLOCK (at least on linux -- it
  // *does* cause problems for FreeBSD, but isn't needed for locking there.)
# ifndef __FreeBSD__
#   define _XOPEN_SOURCE 500  // may be needed to get the rwlock calls
# endif
# include <pthread.h>
  typedef pthread_rwlock_t MutexType;
#elif defined(HAVE_PTHREAD)
# include <pthread.h>
  typedef pthread_mutex_t MutexType;
#elif defined(WIN32)
# define WIN32_LEAN_AND_MEAN  // We only need minimal includes
# ifdef GMUTEX_TRYLOCK
  // We need Windows NT or later for TryEnterCriticalSection().  If you
  // don't need that functionality, you can remove these _WIN32_WINNT
  // lines, and change TryLock() to assert(0) or something.
#   ifndef _WIN32_WINNT
#     define _WIN32_WINNT 0x0400
#   endif
# endif
# include <windows.h>
  typedef CRITICAL_SECTION MutexType;
#else
# error Need to implement mutex.h for your architecture, or #define NO_THREADS
#endif

class Mutex {
 public:
  // Create a Mutex that is not held by anybody.
  inline Mutex();

  // Destructor
  inline ~Mutex();

  inline void Lock();    // Block if needed until free then acquire exclusively
  inline void Unlock();  // Release a lock acquired via Lock()
  inline bool TryLock(); // If free, Lock() and return true, else return false
  // Note that on systems that don't support read-write locks, these may
  // be implemented as synonyms to Lock() and Unlock().  So you can use
  // these for efficiency, but don't use them anyplace where being able
  // to do shared reads is necessary to avoid deadlock.
  inline void ReaderLock();   // Block until free or shared then acquire a share
  inline void ReaderUnlock(); // Release a read share of this Mutex
  inline void WriterLock() { Lock(); }     // Acquire an exclusive lock
  inline void WriterUnlock() { Unlock(); } // Release a lock from WriterLock()

 private:
  MutexType mutex_;

  // Catch the error of writing Mutex when intending MutexLock.
  Mutex(Mutex *ignored) {}
  // Disallow "evil" constructors
  Mutex(const Mutex&);
  void operator=(const Mutex&);
};

// Now the implementation of Mutex for various systems
#if defined(NO_THREADS)

// When we don't have threads, we can be either reading or writing,
// but not both.  We can have lots of readers at once (in no-threads
// mode, that's most likely to happen in recursive function calls),
// but only one writer.  We represent this by having mutex_ be -1 when
// writing and a number > 0 when reading (and 0 when no lock is held).
//
// In debug mode, we assert these invariants, while in non-debug mode
// we do nothing, for efficiency.  That's why everything is in an
// assert.
#include <assert.h>

Mutex::Mutex() : mutex_(0) { }
Mutex::~Mutex()            { assert(mutex_ == 0); }
void Mutex::Lock()         { assert(--mutex_ == -1); }
void Mutex::Unlock()       { assert(mutex_++ == -1); }
bool Mutex::TryLock()      { if (mutex_) return false; Lock(); return true; }
void Mutex::ReaderLock()   { assert(++mutex_ > 0); }
void Mutex::ReaderUnlock() { assert(mutex_-- > 0); }

#elif defined(HAVE_PTHREAD) && defined(HAVE_RWLOCK)

#include <stdlib.h>      // for abort()
#define SAFE_PTHREAD(fncall)  do { if ((fncall) != 0) abort(); } while (0)

Mutex::Mutex()             { SAFE_PTHREAD(pthread_rwlock_init(&mutex_, NULL)); }
Mutex::~Mutex()            { SAFE_PTHREAD(pthread_rwlock_destroy(&mutex_)); }
void Mutex::Lock()         { SAFE_PTHREAD(pthread_rwlock_wrlock(&mutex_)); }
void Mutex::Unlock()       { SAFE_PTHREAD(pthread_rwlock_unlock(&mutex_)); }
bool Mutex::TryLock()      { return pthread_rwlock_trywrlock(&mutex_) == 0; }
void Mutex::ReaderLock()   { SAFE_PTHREAD(pthread_rwlock_rdlock(&mutex_)); }
void Mutex::ReaderUnlock() { SAFE_PTHREAD(pthread_rwlock_unlock(&mutex_)); }
#undef SAFE_PTHREAD

#elif defined(HAVE_PTHREAD)

#include <stdlib.h>      // for abort()
#define SAFE_PTHREAD(fncall)  do { if ((fncall) != 0) abort(); } while (0)

Mutex::Mutex()             { SAFE_PTHREAD(pthread_mutex_init(&mutex_, NULL)); }
Mutex::~Mutex()            { SAFE_PTHREAD(pthread_mutex_destroy(&mutex_)); }
void Mutex::Lock()         { SAFE_PTHREAD(pthread_mutex_lock(&mutex_)); }
void Mutex::Unlock()       { SAFE_PTHREAD(pthread_mutex_unlock(&mutex_)); }
bool Mutex::TryLock()      { return pthread_mutex_trylock(&mutex_) == 0; }
void Mutex::ReaderLock()   { Lock(); }      // we don't have read-write locks
void Mutex::ReaderUnlock() { Unlock(); }
#undef SAFE_PTHREAD

#elif defined(WIN32)

Mutex::Mutex()             { InitializeCriticalSection(&mutex_); }
Mutex::~Mutex()            { DeleteCriticalSection(&mutex_); }
void Mutex::Lock()         { EnterCriticalSection(&mutex_); }
void Mutex::Unlock()       { LeaveCriticalSection(&mutex_); }
bool Mutex::TryLock()      { return TryEnterCriticalSection(&mutex_) != 0; }
void Mutex::ReaderLock()   { Lock(); }      // we don't have read-write locks
void Mutex::ReaderUnlock() { Unlock(); }

#endif


// --------------------------------------------------------------------------
// Some helper classes

// MutexLock(mu) acquires mu when constructed and releases it when destroyed.
class MutexLock {
 public:
  explicit MutexLock(Mutex *mu) : mu_(mu) { mu_->Lock(); }
  ~MutexLock() { mu_->Unlock(); }
 private:
  Mutex * const mu_;
  // Disallow "evil" constructors
  MutexLock(const MutexLock&);
  void operator=(const MutexLock&);
};

// ReaderMutexLock and WriterMutexLock do the same, for rwlocks
class ReaderMutexLock {
 public:
  explicit ReaderMutexLock(Mutex *mu) : mu_(mu) { mu_->ReaderLock(); }
  ~ReaderMutexLock() { mu_->ReaderUnlock(); }
 private:
  Mutex * const mu_;
  // Disallow "evil" constructors
  ReaderMutexLock(const ReaderMutexLock&);
  void operator=(const ReaderMutexLock&);
};

class WriterMutexLock {
 public:
  explicit WriterMutexLock(Mutex *mu) : mu_(mu) { mu_->WriterLock(); }
  ~WriterMutexLock() { mu_->WriterUnlock(); }
 private:
  Mutex * const mu_;
  // Disallow "evil" constructors
  WriterMutexLock(const WriterMutexLock&);
  void operator=(const WriterMutexLock&);
};

// Catch bug where variable name is omitted, e.g. MutexLock (&mu);
#define MutexLock(x) COMPILE_ASSERT(0, mutex_lock_decl_missing_var_name)
#define ReaderMutexLock(x) COMPILE_ASSERT(0, rmutex_lock_decl_missing_var_name)
#define WriterMutexLock(x) COMPILE_ASSERT(0, wmutex_lock_decl_missing_var_name)

#endif  /* #define GOOGLE_SIMPLE_MUTEX_H__ */
