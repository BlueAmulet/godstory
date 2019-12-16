//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/simBase.h"
#include "materialPropertyMap.h"
#include "base/Locker.h"

SimObjectPtr<MaterialPropertyMap> MaterialPropertyMap::smMap;

MaterialPropertyMap * MaterialPropertyMap::get()
{
   // If we have it, return it.
   if(bool(smMap))
      return smMap;

   AssertFatal(dynamic_cast<MaterialPropertyMap*>(Sim::findObject("GlobalMaterialPropertyMap")) || !Sim::findObject("GlobalMaterialPropertyMap"), 
      "MaterialPropertyMap::get - 'GlobalMaterialPropertyMap' wasn't a MaterialPropertyMap!");

   // Didn't have it, try to find it.
   if(!(smMap = static_cast<MaterialPropertyMap*>(Sim::findObject("GlobalMaterialPropertyMap"))))
   {
      // We failed to find it... Complain, but only once.
      static bool once = false;
      if (!once)
      {
         Con::errorf("MaterialPropertyMap::get - 'GlobalMaterialPropertyMap' not found!");
         once = true;
      }
   }

   return smMap;
}

//-----------------------------------------------------------------------------
// Add material property map
//-----------------------------------------------------------------------------

ConsoleFunction( addMaterialMapping, bool, 2, 99, "(string matName, ...) Set up a material mapping. See MaterialPropertyMap for details.")
{
   MaterialPropertyMap* pMap = MaterialPropertyMap::get();
   if (pMap == NULL) {
      Con::errorf(ConsoleLogEntry::General, "Error, cannot find the global material map object");
      return false;
   }

   return pMap->addMapping(argc - 1, argv + 1);
}


IMPLEMENT_CONOBJECT(MaterialPropertyMap);
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
MaterialPropertyMap::MaterialPropertyMap()
{
   VECTOR_SET_ASSOCIATION(mMapEntries);
}

MaterialPropertyMap::~MaterialPropertyMap()
{
	MapEntry* pEntry;
	for (U32 i = 0; i < mMapEntries.size(); i++) 
	{
		pEntry = &mMapEntries[i];
		if(pEntry)
		{
			pEntry = pEntry->pNext;
			while(pEntry)
			{
				MapEntry* pEntry2 = pEntry->pNext;
				delete pEntry;
				pEntry = pEntry2;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Get map entry
//-----------------------------------------------------------------------------
const MaterialPropertyMap::MapEntry* MaterialPropertyMap::getMapEntry(StringTableEntry name) const
{
   // DMMNOTE: Really slow.  Shouldn't be a problem since these are one time scans
   //  for each object, but might want to replace this with a hash table
   //
	OLD_DO_LOCK( (void*)this );

   const MapEntry* ret = NULL;
   for (U32 i = 0; i < mMapEntries.size(); i++) {
      if (dStricmp(mMapEntries[i].name, name) == 0) {
         ret = &mMapEntries[i];
         break;
      }
   }

   return ret;
}

//-----------------------------------------------------------------------------
// Get map entry from index
//-----------------------------------------------------------------------------
const MaterialPropertyMap::MapEntry* MaterialPropertyMap::getMapEntryFromIndex(S32 index) const
{
	OLD_DO_LOCK( (void*)this );

   const MapEntry* ret = NULL;
   if(index < mMapEntries.size())
      ret = &mMapEntries[index];
   return ret;
}

//-----------------------------------------------------------------------------
// Get index from name
//-----------------------------------------------------------------------------
S32 MaterialPropertyMap::getIndexFromName(StringTableEntry name) const
{
	OLD_DO_LOCK( (void*)this );

   S32 ret = -1;
   for (U32 i = 0; i < mMapEntries.size(); i++) {
      if (dStricmp(mMapEntries[i].name, name) == 0) {
         ret = i;
         break;
      }
   }
   return ret;
}

//-----------------------------------------------------------------------------
// getNCMapEntry
//-----------------------------------------------------------------------------
MaterialPropertyMap::MapEntry* MaterialPropertyMap::getNCMapEntry(StringTableEntry name)
{
   return const_cast<MapEntry*>(getMapEntry(name));
}

//-----------------------------------------------------------------------------
// Add mapping
//-----------------------------------------------------------------------------
bool MaterialPropertyMap::addMapping(const S32 argc, const char** argv)
{
	OLD_DO_LOCK( this );

   const char* matName = StringTable->insert(argv[0]);

   //Ray: 允许相同文件但是不同属性的材质
   MapEntry* pEntry = getNCMapEntry(matName);
   if (pEntry != NULL)
   {
      while(pEntry->pNext)
	  {
		  pEntry = pEntry->pNext;
	  }

	  pEntry->pNext = new MapEntry;
	  pEntry = pEntry->pNext;
   } 
   else
   {
      mMapEntries.increment();
      pEntry = &mMapEntries.last();
   }

   pEntry->sound          = -1;
   pEntry->puffColor[0].set(0.0f, 0.0f, 0.0f);
   pEntry->puffColor[1].set(0.0f, 0.0f, 0.0f);

   pEntry->name           = matName;
   pEntry->detailMapName  = NULL;
   pEntry->environMapName = NULL;
   pEntry->bumpMapName    = NULL;
   pEntry->materialName   = NULL;
   pEntry->matType        = Default;
   pEntry->matFlags       = 0;
   pEntry->pNext			 = NULL;

   for (U32 i = 1; S32(i) < argc; i++)
   {
      const char* param = argv[i];

      if (dStrnicmp(param, "detail:", dStrlen("detail:")) == 0) {
         // Set the detail map
         const char* pColon = dStrchr(param, ':');
         pColon++;
         while (*pColon == ' ' || *pColon == '\t')
            pColon++;

         pEntry->detailMapName = StringTable->insert(pColon);
      }
      else if (dStrnicmp(param, "bump:", dStrlen("bump:")) == 0) {
         // Set the detail map
         const char* pColon = dStrchr(param, ':');
         pColon++;
         while (*pColon == ' ' || *pColon == '\t')
            pColon++;

         pEntry->bumpMapName = StringTable->insert(pColon);
      }
      else if (dStrnicmp(param, "material:", dStrlen("material:")) == 0) {
         // Set the detail map
         const char* pColon = dStrchr(param, ':');
         pColon++;
         while (*pColon == ' ' || *pColon == '\t')
            pColon++;

         pEntry->materialName = StringTable->insert(pColon);
      }
      else if (dStrnicmp(param, "environment:", dStrlen("environment:")) == 0) {
         // Set the detail map
         const char* pColon = dStrchr(param, ':');
         pColon++;
         while (*pColon == ' ' || *pColon == '\t')
            pColon++;

         const char* start = pColon;
         while (*pColon != ' ')
            pColon++;
         const char* end = pColon;
         pColon++;

         char buffer[256];
         dStrncpy(buffer, sizeof(buffer), start, end - start);
         buffer[end - start] = '\0';

         pEntry->environMapName   = StringTable->insert(buffer);
         pEntry->environMapFactor = dAtof(pColon);
      }
      else if (dStrnicmp(param, "color:", dStrlen("color:")) == 0) {
         const char* curChar = dStrchr(param, ':');
         curChar++;
         while (*curChar == ' ' || *curChar == '\t')
            curChar++;

         char buffer[5][256];
         S32 index = 0;
         for(S32 x = 0; x < 5; ++x, index = 0)
         {
            while(*curChar != ' ' && *curChar != '\0')
               buffer[x][index++] = *curChar++;
            buffer[x][index++] = '\0';
            while(*curChar == ' ')
               ++curChar;
         }
         pEntry->puffColor[0].set(dAtof(buffer[0]), dAtof(buffer[1]), dAtof(buffer[2]), dAtof(buffer[3]));                        
         pEntry->puffColor[1].set(dAtof(buffer[0]), dAtof(buffer[1]), dAtof(buffer[2]), dAtof(buffer[4]));                        
      }
      else if (dStrnicmp(param, "sound:", dStrlen("sound:")) == 0) {
         // Set the detail map
         const char* pColon = dStrchr(param, ':');
         pColon++;
         while (*pColon == ' ' || *pColon == '\t')
            pColon++;

         const char* start = pColon;
         while(*pColon != ' ' && *pColon != '\0')
            pColon++;
         const char* end = pColon;
         pColon++;

         char buffer[256];
         dStrncpy(buffer, sizeof(buffer), start, end - start);
         buffer[end - start] = '\0';

         pEntry->sound = dAtoi(buffer);
      }
      else if (param[0] == '\0') {
         // Empty statement allowed, does nothing
      }
      else {
         Con::warnf(ConsoleLogEntry::General, "Warning, misunderstood material parameter: %s in materialEntry %s", param, matName);
      }
   }

   return true;
}

//-----------------------------------------------------------------------------
// Del mapping
//-----------------------------------------------------------------------------
void MaterialPropertyMap::delMapping(StringTableEntry name)
{
    OLD_DO_LOCK( (void*)this );

    for (U32 i = 0; i < mMapEntries.size();) 
    {
        MapEntry* pHead = &mMapEntries[i];
        while ( pHead && dStricmp(pHead->name, name) == 0 )
        {
            MapEntry* next = pHead->pNext;
            if ( next )
            {
                dMemcmp( pHead, next, sizeof(MapEntry) );
                delete next;
            }
            else
                mMapEntries.erase(i);
            pHead = next;
        }

        if ( pHead == NULL )
            continue;

        MapEntry* pPrevEntry = pHead;
        MapEntry* pEntry = pHead->pNext;    
        while(pEntry)
        {
            MapEntry* curEntry = pEntry;
            MapEntry* nextEntry = pEntry->pNext;
            if ( dStricmp(curEntry->name, name) == 0 )
            {
                pPrevEntry->pNext = nextEntry;
                delete curEntry;
            }
            else
                pPrevEntry = curEntry;
            pEntry = nextEntry;
        }

        i++;
    }
}