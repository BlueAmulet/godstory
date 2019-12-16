//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "sceneGraph/lightManager.h"
#include "console/console.h"
#include "util/safeDelete.h"
#include "console/sim.h"
#include "console/simSet.h"

#pragma init_seg (lib)
TestHulingyun g_TestHulingyun;

U32 TestHulingyun::smTestId = 1;
int TestHulingyun::MAX_SIZE = 4096 * 100;

TestHulingyun::TestHulingyun()
{
	list = new Test_Struct[MAX_SIZE];
	m_count = 0;
}

TestHulingyun::~TestHulingyun()
{
	delete [] list;
	m_count = 0;
}

void TestHulingyun::Insert(void *pObj)
{
	AssertFatal(m_count < MAX_SIZE, "TestHulingyun: too many elements!");

	Test_Struct t;
	if (smTestId == 3242)
	{
		int breakPt = 0;
	}
	t.nTestId = smTestId;
	t.nTestObj = pObj;
	smTestId++;

	for (S32 i = 0; i < MAX_SIZE; i++)
	{
		if (list[i].nTestId == 0)
		{
			list[i] = t;
			m_count++;
			break;
		}
	}
}

void TestHulingyun::Delete(void *pObj)
{
	for (S32 i = 0; i < m_count; i++)
	{
		if (list[i].nTestId == 0)
			continue;

		if (list[i].nTestObj == pObj)
		{
			list[i].nTestId = 0;
			list[i].nTestObj = NULL;
			m_count--;
			break;
		}
	}
}

void TestHulingyun::Print()
{
	for (S32 i = 0; i < m_count; i++)
	{
		if (list[i].nTestId == 0)
			continue;

		int test = list[i].nTestId;
	}
}

//
// LightManager
//

// Called when the scene lighting should be computed.
bool LightManager::lightScene(const char* callback, const char* param)
{
   Con::executef(callback);
   return true;
}

bool LightManager::canActivate()
{
   if (Sim::getLightSet()->size())
   {
      Con::errorf("There are active lights, lighting system changes are not allowed.");
      return false;
   } else {
      return true;
   }   
}

//
// AvailableLightManagers
//
AvailableLightManagers* AvailableLightManagers::smSingleton = NULL;

AvailableLightManagers::AvailableLightManagers()
{
   mCurrentLM = NULL;
   mCurrentLMIndex = -1;
}

AvailableLightManagers::~AvailableLightManagers()
{
	for (S32 i = 0; i < mFactoryList.size(); i++)
	{
		if (mFactoryList[i])
		{
			//delete mFactoryList[i];
			//mFactoryList[i] = NULL;
		}
	}
	mFactoryList.clear();
}

void AvailableLightManagers::registerFactory(LightManagerFactory* lmf)
{
   mFactoryList.push_back(lmf);
}

ConsoleFunction( getLightManager, const char*, 1, 1, "Get the name of the current lighting manager")
{
   return AvailableLightManagers::get()->getCurrentLMName();
}

const char* AvailableLightManagers::getLMName(const U32 index) const
{
   AssertFatal(index < mFactoryList.size(), "Invalid index into Light Manager Factory list!");
   return mFactoryList[index]->getName();
}

U32 AvailableLightManagers::getLMCount() const
{
   return mFactoryList.size();
}

LightManager* AvailableLightManagers::getDefaultLM()
{
   if (mFactoryList.size() > 0)
   {
      return getLM(0); 
   } else {
      return NULL;
   }
}

LightManager* AvailableLightManagers::getLMByName(const char* lmName)
{
   for (U32 i = 0; i < mFactoryList.size(); i++)
   {
      if (dStricmp(lmName, mFactoryList[i]->getName()) == 0)
      {
         return getLM(i);
      }
   }
   Con::errorf("%s is not a registered lighting system!", lmName);
   return NULL;
}

LightManager* AvailableLightManagers::getLM(const U32 index)
{
   AssertFatal(index < mFactoryList.size(), "Invalid index into Light Manager Factory list!");
   if (index != mCurrentLMIndex)
   {
      LightManager* potentialLM = mFactoryList[index]->createLightManager();
      if (potentialLM->canActivate())
      {
         cleanupCurrentLM();
         mCurrentLM = mFactoryList[index]->createLightManager();
         mCurrentLM->activate();
         mCurrentLMIndex = index;
      }
   } 
   return mCurrentLM;
}

void AvailableLightManagers::cleanupCurrentLM()
{
   if (mCurrentLM)
   {
      mCurrentLM->deactivate();
      mFactoryList[mCurrentLMIndex]->cleanupLightManager();
      mCurrentLM = NULL;      
      mCurrentLMIndex = -1;
   }
}
