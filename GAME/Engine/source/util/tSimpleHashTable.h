//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"

#include "core/tSparseArray.h"
#include "util/hashFunction.h"

#ifndef _TSIMPLEHASHTABLE_H
#define _TSIMPLEHASHTABLE_H

template <class T> class SimpleHashTable : public SparseArray<T>
{
   typedef SparseArray<T> Parent;

public:
   SimpleHashTable(const U32 modulusSize = 64) : Parent(modulusSize)
   {
   }

   void insert(T* pObject, U8 *key, U32 keyLen);
   T*   remove(U8 *key, U32 keyLen);
   T*   retreive(U8 *key, U32 keyLen);

   void insert(T* pObject, const char *key);
   T*   remove(const char *key);
   T*   retreive(const char *key);
};

template <class T> inline void SimpleHashTable<T>::insert(T* pObject, U8 *key, U32 keyLen)
{
   Parent::insert(pObject, hash(key, keyLen, 0));
}

template <class T> inline T* SimpleHashTable<T>::remove(U8 *key, U32 keyLen)
{
   return Parent::remove(hash(key, keyLen, 0));
}

template <class T> inline T* SimpleHashTable<T>::retreive(U8 *key, U32 keyLen)
{
   return Parent::retreive(hash(key, keyLen, 0));
}

template <class T> inline void SimpleHashTable<T>::insert(T* pObject, const char *key)
{
   insert(pObject, (U8 *)key, dStrlen(key));
}

template <class T> T* SimpleHashTable<T>::remove(const char *key)
{
   return remove((U8 *)key, dStrlen(key));
}

template <class T> T* SimpleHashTable<T>::retreive(const char *key)
{
   return retreive((U8 *)key, dStrlen(key));
}


#endif // _TSIMPLEHASHTABLE_H
