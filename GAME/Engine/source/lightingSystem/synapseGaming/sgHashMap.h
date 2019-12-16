//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef HASHMAP_H_
#define HASHMAP_H_

#include "platform/types.h"
#include "core/tVector.h"


template<class T, class Tinfo> class hash_multimap
{
public:
   U32 hashCode;
   // entry level info or single object per-entry...
   Tinfo info;
   // for storing multiple objects per-entry...
   Vector<T> object;
   hash_multimap *mapHigh;
   hash_multimap *mapLow;
   hash_multimap *linkHigh;
   hash_multimap *linkLow;
   hash_multimap()
   {
      // theoretical mean for hash code (balance the tree)...
      hashCode = 0x7fffffff;
      // this unfortunately clears objects after the constructor
      // setups up values, but is necessary for clearing pointers...
      dMemset(&info, 0, sizeof(Tinfo));
      mapHigh = mapLow = linkHigh = linkLow = NULL;
   }
   hash_multimap(U32 hashcode)
   {
      hashCode = hashcode;
      // this unfortunately clears objects after the constructor
      // setups up values, but is necessary for clearing pointers...
      dMemset(&info, 0, sizeof(Tinfo));
      mapHigh = mapLow = linkHigh = linkLow = NULL;
   }
   virtual ~hash_multimap()
   {
      clear();
   }

   /// Deletes everything in the multimap (calling clear on a single entry will DELETE the multimap)
   void clear()
   {
      if(mapHigh)
         delete mapHigh;
      if(mapLow)
         delete mapLow;
      mapHigh = mapLow = linkHigh = linkLow = NULL;
   }

   /// Inserts insert into the list, with high above it and low below it.
   void relink(hash_multimap *high, hash_multimap *low, hash_multimap *insert)
   {
      if(high)
         high->linkLow = insert;
      if(low)
         low->linkHigh = insert;
      insert->linkHigh = high;
      insert->linkLow = low;
   }

   /// Finds the entry with hash hashcode
   hash_multimap *find(U32 hashcode)
   {
      // We're it
      if(hashcode == hashCode)
         return this;

      // It's below us
      else if(hashcode < hashCode)
      {
         // But wait, there is nothing below us!
         if(!mapLow)
         {
            // Create an entry below us
            mapLow = new hash_multimap(hashcode);
            // Link it
            relink(this, this->linkLow, mapLow);
            // Return it
            return mapLow;
         }
         // Keep going down the list
         return mapLow->find(hashcode);
      }
      // It's above us!
      else
      {
         // But there is nothing above us!
         if(!mapHigh)
         {
            // Create a new entry above us
            mapHigh = new hash_multimap(hashcode);
            // Link it
            relink(this->linkHigh, this, mapHigh);
            // Return it
            return mapHigh;
         }
         // Keep going up the list
         return mapHigh->find(hashcode);
      }
   }
};


#endif//HASHMAP_H_
