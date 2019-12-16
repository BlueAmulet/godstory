//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleInternal.h"
#include "core/frameAllocator.h"
#include "console/simFieldDictionary.h"

SimFieldDictionary::Entry *SimFieldDictionary::mFreeList = NULL;

static Chunker<SimFieldDictionary::Entry> fieldChunker;

SimFieldDictionary::Entry *SimFieldDictionary::allocEntry()
{
   if(mFreeList)
   {
      Entry *ret = mFreeList;
      mFreeList = ret->next;
      return ret;
   }
   else
      return fieldChunker.alloc();
}

void SimFieldDictionary::freeEntry(SimFieldDictionary::Entry *ent)
{
   ent->next = mFreeList;
   mFreeList = ent;
}

SimFieldDictionary::SimFieldDictionary()
{
   for(U32 i = 0; i < HashTableSize; i++)
      mHashTable[i] = 0;

   mVersion = 0;
}

SimFieldDictionary::~SimFieldDictionary()
{
   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i]; walk;)
      {
         Entry *temp = walk;
         walk = temp->next;

         dFree(temp->value);
         freeEntry(temp);
      }
   }
}

void SimFieldDictionary::setFieldType(StringTableEntry slotName, const char *typeString)
{
   ConsoleBaseType *cbt = ConsoleBaseType::getTypeByName( typeString );
   setFieldType(slotName, cbt);
}

void SimFieldDictionary::setFieldType(StringTableEntry slotName, const U32 typeId)
{
   ConsoleBaseType *cbt = ConsoleBaseType::getType(typeId);
   setFieldType(slotName, cbt);
}

void SimFieldDictionary::setFieldType(StringTableEntry slotName, ConsoleBaseType *type)
{
   // If the field exists on the object, set the type
   U32 bucket = HashPointer( slotName ) % HashTableSize;

   for( Entry *walk = mHashTable[bucket]; walk; walk = walk->next )
   {
      if( walk->slotName == slotName )
      {
         // Found and type assigned, let's bail
         walk->type = type;
         return;
      }
   }

   // Otherwise create the field, and set the type. Assign a null value.
   mVersion++;

   Entry *field = allocEntry();
   field->value = NULL;
   field->slotName = slotName;
   field->next = NULL;
   field->type = type;
}

U32 SimFieldDictionary::getFieldType(StringTableEntry slotName) const
{
   U32 bucket = HashPointer( slotName ) % HashTableSize;

   for( Entry *walk = mHashTable[bucket]; walk; walk = walk->next )
      if( walk->slotName == slotName )
         return walk->type ? walk->type->getTypeID() : TypeString;

   return TypeString;
}

void SimFieldDictionary::setFieldValue(StringTableEntry slotName, const char *value)
{
   U32 bucket = HashPointer(slotName) % HashTableSize;
   Entry **walk = &mHashTable[bucket];
   while(*walk && (*walk)->slotName != slotName)
      walk = &((*walk)->next);

   Entry *field = *walk;
   if(!*value)
   {
      if(field)
      {
         mVersion++;

         dFree(field->value);
         *walk = field->next;
         freeEntry(field);
      }
   }
   else
   {
      if(field)
      {
         dFree(field->value);
         field->value = dStrdup(value);
      }
      else
      {
         mVersion++;

         field = allocEntry();
         field->value = dStrdup(value);
         field->slotName = slotName;
         field->next = NULL;
         field->type = NULL;
         *walk = field;
      }
   }
}

const char *SimFieldDictionary::getFieldValue(StringTableEntry slotName)
{
   U32 bucket = HashPointer(slotName) % HashTableSize;

   for(Entry *walk = mHashTable[bucket];walk;walk = walk->next)
      if(walk->slotName == slotName)
         return walk->value;

   return NULL;
}

void SimFieldDictionary::assignFrom(SimFieldDictionary *dict)
{
   mVersion++;

   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = dict->mHashTable[i];walk; walk = walk->next)
      {
         setFieldValue(walk->slotName, walk->value);
         setFieldType(walk->slotName, walk->type);
      }
   }
}

static S32 QSORT_CALLBACK compareEntries(const void* a,const void* b)
{
   SimFieldDictionary::Entry *fa = *((SimFieldDictionary::Entry **)a);
   SimFieldDictionary::Entry *fb = *((SimFieldDictionary::Entry **)b);
   return dStricmp(fa->slotName, fb->slotName);
}

void SimFieldDictionary::writeFields(SimObject *obj, Stream &stream, U32 tabStop)
{

   const AbstractClassRep::FieldList &list = obj->getFieldList();
   Vector<Entry *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i];walk; walk = walk->next)
      {
         // make sure we haven't written this out yet:
         U32 i;
         for(i = 0; i < list.size(); i++)
            if(list[i].pFieldname == walk->slotName)
               break;

         if(i != list.size())
            continue;


         if (!obj->writeField(walk->slotName, walk->value))
            continue;

         flist.push_back(walk);
      }
   }

   // Sort Entries to prevent version control conflicts
   dQsort(flist.address(),flist.size(),sizeof(Entry *),compareEntries);

   // Save them out
   for(Vector<Entry *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      U32 nBufferSize = (dStrlen( (*itr)->value ) * 2) + dStrlen( (*itr)->slotName ) + 16;
      FrameTemp<char> expandedBuffer( nBufferSize );

      stream.writeTabs(tabStop+1);

      const char *typeName = (*itr)->type && (*itr)->type->getTypeID() != TypeString ? (*itr)->type->getTypeName() : "";
      dSprintf(expandedBuffer, nBufferSize, "%s%s%s = \"", typeName, *typeName ? " " : "", (*itr)->slotName);
      expandEscape((char*)expandedBuffer + dStrlen(expandedBuffer), (*itr)->value);
      dStrcat(expandedBuffer, nBufferSize, "\";\r\n");

      stream.write(dStrlen(expandedBuffer),expandedBuffer);
   }

}
void SimFieldDictionary::printFields(SimObject *obj)
{
   const AbstractClassRep::FieldList &list = obj->getFieldList();
   char expandedBuffer[4096];
   Vector<Entry *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i];walk; walk = walk->next)
      {
         // make sure we haven't written this out yet:
         U32 i;
         for(i = 0; i < list.size(); i++)
            if(list[i].pFieldname == walk->slotName)
               break;

         if(i != list.size())
            continue;

         flist.push_back(walk);
      }
   }
   dQsort(flist.address(),flist.size(),sizeof(Entry *),compareEntries);

   for(Vector<Entry *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s = \"", (*itr)->slotName);
      expandEscape(expandedBuffer + dStrlen(expandedBuffer), (*itr)->value);
      Con::printf("%s\"", expandedBuffer);
   }
}

//------------------------------------------------------------------------------
SimFieldDictionaryIterator::SimFieldDictionaryIterator(SimFieldDictionary * dictionary)
{
   mDictionary = dictionary;
   mHashIndex = -1;
   mEntry = 0;
   operator++();
}

SimFieldDictionary::Entry* SimFieldDictionaryIterator::operator++()
{
   if(!mDictionary)
      return(mEntry);

   if(mEntry)
      mEntry = mEntry->next;

   while(!mEntry && (mHashIndex < (SimFieldDictionary::HashTableSize-1)))
      mEntry = mDictionary->mHashTable[++mHashIndex];

   return(mEntry);
}

SimFieldDictionary::Entry* SimFieldDictionaryIterator::operator*()
{
   return(mEntry);
}