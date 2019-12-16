//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/simDatablock.h"

IMPLEMENT_CO_DATABLOCK_V1(SimDataBlock);
SimObjectId SimDataBlock::sNextObjectId = DataBlockObjectIdFirst;
S32 SimDataBlock::sNextModifiedKey = 0;

//---------------------------------------------------------------------------

SimDataBlock::SimDataBlock()
{
   setModDynamicFields(true);
   setModStaticFields(true);
}

bool SimDataBlock::onAdd()
{
   Parent::onAdd();

   // This initialization is done here, and not in the constructor,
   // because some jokers like to construct and destruct objects
   // (without adding them to the manager) to check what class
   // they are.
   modifiedKey = ++sNextModifiedKey;
   AssertFatal(sNextObjectId <= DataBlockObjectIdLast,
      "Exceeded maximum number of data blocks");

   // add DataBlock to the DataBlockGroup unless it is client side ONLY DataBlock
   if (getId() >= DataBlockObjectIdFirst && getId() <= DataBlockObjectIdLast)
      if (SimGroup* grp = Sim::getDataBlockGroup())
         grp->addObject(this);
   return true;
}

void SimDataBlock::assignId()
{
   // We don't want the id assigned by the manager, but it may have
   // already been assigned a correct data block id.
   if (getId() < DataBlockObjectIdFirst || getId() > DataBlockObjectIdLast)
      setId(sNextObjectId++);
}

void SimDataBlock::onStaticModified(const char* slotName, const char* newValue)
{
   modifiedKey = sNextModifiedKey++;

}

/*void SimDataBlock::setLastError(const char*)
{
} */

void SimDataBlock::packData(BitStream*)
{
}

void SimDataBlock::unpackData(BitStream*)
{
}

bool SimDataBlock::preload(bool, char[256])
{
   return true;
}

ConsoleFunction(preloadClientDataBlocks, void, 1, 1, "Preload all datablocks in client mode.  "
                "(Server parameter is set to false).  This will take some time to complete.")
{
   argc; argv;
   // we go from last to first because we cut 'n pasted the loop from deleteDataBlocks
   SimGroup *grp = Sim::getDataBlockGroup();
   char errorBuffer[256];
   for(S32 i = grp->size() - 1; i >= 0; i--)
   {
      AssertFatal(dynamic_cast<SimDataBlock*>((*grp)[i]), "Doh! non-datablock in datablock group!");
      SimDataBlock *obj = (SimDataBlock*)(*grp)[i];
      if (!obj->preload(false, errorBuffer))
         Con::errorf("Failed to preload client datablock, %s: %s", obj->getName(), errorBuffer);
   }
}

ConsoleFunction(deleteDataBlocks, void, 1, 1, "Delete all the datablocks we've downloaded. "
                "This is usually done in preparation of downloading a new set of datablocks, "
                " such as occurs on a mission change, but it's also good post-mission cleanup.")
{
   argc; argv;
   // delete from last to first:
   SimGroup *grp = Sim::getDataBlockGroup();
   for(S32 i = grp->size() - 1; i >= 0; i--)
   {
      SimObject *obj = (*grp)[i];
      obj->deleteObject();
   }
   SimDataBlock::sNextObjectId = DataBlockObjectIdFirst;
   SimDataBlock::sNextModifiedKey = 0;
}

#ifdef DEBUG
#include <hash_map>
#include <string>
ConsoleFunction(dataBlockCount, void, 1, 1, "列出dataBlock统计")
{
	stdext::hash_map<std::string,int> cont;

	SimGroup *grp = Sim::getDataBlockGroup();
	for(S32 i = grp->size() - 1; i >= 0; i--)
	{
		AssertFatal(dynamic_cast<SimDataBlock*>((*grp)[i]), "Doh! non-datablock in datablock group!");
		SimDataBlock *obj = (SimDataBlock*)(*grp)[i];
		const char *pClassName = obj->getClassRep()->getClassName();
		if(cont.find(pClassName) == cont.end())
			cont[pClassName] = 1;
		else
			cont[pClassName]++;
	}

	grp = Sim::getUserDataBlockGroup();
	for(S32 i = grp->size() - 1; i >= 0; i--)
	{
		AssertFatal(dynamic_cast<SimDataBlock*>((*grp)[i]), "Doh! non-datablock in datablock group!");
		SimDataBlock *obj = (SimDataBlock*)(*grp)[i];
		const char *pClassName = obj->getClassRep()->getClassName();
		if(cont.find(pClassName) == cont.end())
			cont[pClassName] = 1;
		else
			cont[pClassName]++;
	}

	int count = 0;
	Con::printf("========================dataBlocks=====================");
	stdext::hash_map<std::string,int>::iterator it = cont.begin();
	for(;it!=cont.end();it++)
	{
		count+=it->second;
		Con::printf("%s : 数量:%d",it->first.c_str(),it->second);
	}
	Con::printf("总计:%d================================================",count);
}
#endif