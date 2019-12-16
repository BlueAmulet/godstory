//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/resManager.h"
#include "core/tAlgorithm.h"

#include "Base/Locker.h"

//=============================================================================
//	资源文件映射表类（ResDictionary）相关方法

ResDictionary::ResDictionary()
{
   entryCount = 0;
   hashTableSize = 1023; //DefaultTableSize;
   hashTable = new ResourceObject*[hashTableSize];
   for(S32 i = 0; i < hashTableSize; i++)
      hashTable[i] = NULL;
}

ResDictionary::~ResDictionary()
{
   delete[] hashTable;
}


//-----------------------------------------------------------------------------
// 根据路径和文件名计算哈西值
S32 ResDictionary::hash(StringTableEntry path, StringTableEntry file)
{
   return ((S32)((((dsize_t)path) >> 2) + (((dsize_t)file) >> 2) )) % hashTableSize;
}

//-----------------------------------------------------------------------------
// 
void ResDictionary::insert(ResourceObject *obj, StringTableEntry path, StringTableEntry file)
{
	OLD_DO_LOCK( this );

   if(path)
   {
      char fullPath[1024];
      Platform::makeFullPathName(path, fullPath, sizeof(fullPath));
      path = StringTable->insert(fullPath);
   }

   obj->name = file;
   obj->path = path;

   S32 idx = hash(path, file);
   obj->nextEntry = hashTable[idx];
   hashTable[idx] = obj;
   entryCount++;

   // 如果添加的对象数量超过了哈西表的大小，则需要重新扩建哈西表
   if(entryCount > hashTableSize) 
   {
      ResourceObject *head = NULL, *temp, *walk;
      for(idx = 0; idx < hashTableSize;idx++) 
	  {
         walk = hashTable[idx];
         while(walk)
         {
            temp = walk->nextEntry;
            walk->nextEntry = head;
            head = walk;
            walk = temp;
         }
      }
      delete[] hashTable;
      hashTableSize = 2 * hashTableSize - 1;
      hashTable = new ResourceObject *[hashTableSize];
      for(idx = 0; idx < hashTableSize; idx++)
         hashTable[idx] = NULL;
      walk = head;
      while(walk)
      {
         temp = walk->nextEntry;
         idx = hash(walk);
         walk->nextEntry = hashTable[idx];
         hashTable[idx] = walk;
         walk = temp;
      }
   }
}

//-----------------------------------------------------------------------------
// 通过路径和文件名快速查找资源文件对象
ResourceObject* ResDictionary::find(StringTableEntry path, StringTableEntry name)
{
	OLD_DO_LOCK( this );
 
	if(path)
   {
      char fullPath[1024];
      Platform::makeFullPathName(path, fullPath, sizeof(fullPath));
      path = StringTable->insert(fullPath);
   }

   for(ResourceObject *walk = hashTable[hash(path, name)]; walk; walk = walk->nextEntry)
      if(walk->name == name && walk->path == path)
         return walk;
   return NULL;
}

//-----------------------------------------------------------------------------
// 通过路径和文件名快速查找压缩包的资源文件对象
ResourceObject* ResDictionary::find(StringTableEntry path, StringTableEntry name, StringTableEntry zipPath, StringTableEntry zipName)
{
	OLD_DO_LOCK( this );

   if(path)
   {
      char fullPath[1024];
      Platform::makeFullPathName(path, fullPath, sizeof(fullPath));
      path = StringTable->insert(fullPath);
   }

   for(ResourceObject *walk = hashTable[hash(path, name)]; walk; walk = walk->nextEntry)
      if(walk->name == name && walk->path == path && walk->zipName == zipName && walk->zipPath == zipPath)
         return walk;
   return NULL;
}

//-----------------------------------------------------------------------------
// 通过路径和文件快速查找特定文件类型的资源文件对象（VolumeBlock、File或Added）
ResourceObject* ResDictionary::find(StringTableEntry path, StringTableEntry name, U32 flags)
{
	OLD_DO_LOCK( this );

   if(path)
   {
      char fullPath[1024];
      Platform::makeFullPathName(path, fullPath, sizeof(fullPath));
      path = StringTable->insert(fullPath);
   }

   for(ResourceObject *walk = hashTable[hash(path, name)]; walk; walk = walk->nextEntry)
      if(walk->name == name && walk->path == path && U32(walk->flags) == flags)
         return walk;
   return NULL;
}
//-----------------------------------------------------------------------------
// 重新将某资源文件对象放到特定文件类型的资源文件对象后面（同一哈希值的上链表排序）
void ResDictionary::pushBehind(ResourceObject *resObj, S32 flagMask)
{
	OLD_DO_LOCK( this );

   remove(resObj);
   entryCount++;
   ResourceObject **walk = &hashTable[hash(resObj)];
   for(; *walk; walk = &(*walk)->nextEntry)
   {
      if(!((*walk)->flags & flagMask))
      {
         resObj->nextEntry = *walk;
         *walk = resObj;
         return;
      }
   }
   resObj->nextEntry = NULL;
   *walk = resObj;
}

//-----------------------------------------------------------------------------
// 从文件映射表内去除某资源文件对象
void ResDictionary::remove(ResourceObject *resObj)
{
	OLD_DO_LOCK( this );

   for(ResourceObject **walk = &hashTable[hash(resObj)]; *walk; walk = &(*walk)->nextEntry)
   {
      if(*walk == resObj)
      {
         entryCount--;
         *walk = resObj->nextEntry;
         return;
      }
   }
}
