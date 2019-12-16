//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SIMFIELDDICTIONARY_H_
#define _SIMFIELDDICTIONARY_H_

// Forward Refs
class ConsoleBaseType;
class SimObject;

#include "core/stringTable.h"
#include "core/stream.h"

/// Dictionary to keep track of dynamic fields on SimObject.
class SimFieldDictionary
{
   friend class SimFieldDictionaryIterator;

public:
   struct Entry
   {
      Entry() : type( NULL ) {};

      StringTableEntry slotName;
      char *value;
      Entry *next;
      ConsoleBaseType *type;
   };
private:
   enum
   {
      HashTableSize = 19
   };
   Entry *mHashTable[HashTableSize];

   static Entry *mFreeList;
   static void freeEntry(Entry *entry);
   static Entry *allocEntry();

   /// In order to efficiently detect when a dynamic field has been
   /// added or deleted, we increment this every time we add or
   /// remove a field.
   U32 mVersion;

public:
   const U32 getVersion() const { return mVersion; }

   SimFieldDictionary();
   ~SimFieldDictionary();
   void setFieldType(StringTableEntry slotName, const char *typeString);
   void setFieldType(StringTableEntry slotName, const U32 typeId);
   void setFieldType(StringTableEntry slotName, ConsoleBaseType *type);
   void setFieldValue(StringTableEntry slotName, const char *value);
   const char *getFieldValue(StringTableEntry slotName);
   U32 getFieldType(StringTableEntry slotName) const;
   void writeFields(SimObject *obj, Stream &strem, U32 tabStop);
   void printFields(SimObject *obj);
   void assignFrom(SimFieldDictionary *dict);
};

class SimFieldDictionaryIterator
{
   SimFieldDictionary *          mDictionary;
   S32                           mHashIndex;
   SimFieldDictionary::Entry *   mEntry;

public:
   SimFieldDictionaryIterator(SimFieldDictionary*);
   SimFieldDictionary::Entry* operator++();
   SimFieldDictionary::Entry* operator*();
};


#endif // _SIMFIELDDICTIONARY_H_
