#include "platform/EngineConfig.h"
#include "ts/tsShapeInstance.h"

#include "util/processMgr.h"
#include "util/aniThread.h"

CMultThreadWorkMgr *g_MultThreadWorkMgr = NULL;

void CMultThreadWorkMgr::init()
{
#ifdef USE_MULTITHREAD_ANIMATE
	if(!g_MultThreadWorkMgr)
	{
		g_MultThreadWorkMgr = new CMultThreadWorkMgr;
	}

	CMultThreadProcess::init();

#endif
}

void CMultThreadWorkMgr::shutdown()
{
	if(g_MultThreadWorkMgr)
	{
		delete g_MultThreadWorkMgr;
		g_MultThreadWorkMgr = NULL;
	}

	CMultThreadProcess::shutdown();
}

CMultThreadWorkMgr::CMultThreadWorkMgr()
{
	mEnable = false;
	mBegin = false;
}

CMultThreadWorkMgr::~CMultThreadWorkMgr()
{
	clear();
}

void CMultThreadWorkMgr::clear()
{
	//Ray: 统一删除对象，防止在多线程中的对象被删除
	for(int i=0;i<mProcessList.size();i++)
	{
		delete mProcessList[i]->pParam;
		delete mProcessList[i];
	}
	mProcessList.clear();

	for(int i=0;i<mDeleteingObjects.size();i++)
		delete mDeleteingObjects[i];
	mDeleteingObjects.clear();

	for(int i=0;i<mAnimateShapeInstance.size();i++)
	{
		delete mAnimateShapeInstance[i];
	}
	mAnimateShapeInstance.clear();

	for(int i=0;i<mDeleteingShapeInstance.size();i++)
		delete mDeleteingShapeInstance[i];
	mDeleteingShapeInstance.clear();
	mStepOne = false;
	mZombifyFlag = false;
}

void CMultThreadWorkMgr::endProcess(bool clearFlag)
{
	g_CurrentThreadProcess->end();
	if(clearFlag)
		clear();
	g_CurrentThreadProcess = g_ThreadProcess1;
}

void CMultThreadWorkMgr::tryProcess(bool bEnd)
{
	if(g_CurrentThreadProcess && mStepOne)
	{
		bool end = false;
		if(bEnd)
			end = g_CurrentThreadProcess->end();
		else
			end = g_CurrentThreadProcess->isend();

		if(end)
		{
			g_CurrentThreadProcess = g_ThreadProcess2;

			for(int i=0;i<mProcessList.size();i++)
				g_CurrentThreadProcess->addWorkItem(mProcessList[i]);
			mProcessList.clear();
			mStepOne = false;
		}
	}
}

void CMultThreadWorkMgr::beginProcess()
{
	for(int i=0;i<mDeleteingObjects.size();i++)
	{
		for(int j=mProcessList.size()-1;j>=0;j--)
		{
			if(mProcessList[j]->pObj == mDeleteingObjects[i])
			{
				delete mProcessList[j]->pParam;
				delete mProcessList[j];
				mProcessList.erase(j);
			}
		}
		delete mDeleteingObjects[i];
	}
	mDeleteingObjects.clear();

	for(int i=0;i<mDeleteingShapeInstance.size();i++)
	{
		for(int j=mAnimateShapeInstance.size()-1;j>=0;j--)
		{
			if(mAnimateShapeInstance[j]->pShapeIns == mDeleteingShapeInstance[i])
			{
				delete mAnimateShapeInstance[j];
				mAnimateShapeInstance.erase(j);
			}
		}
		delete mDeleteingShapeInstance[i];
	}
	mDeleteingShapeInstance.clear();

	if(g_CurrentThreadProcess)
	{
		g_CurrentThreadProcess->start();

		for(int i=0;i<mAnimateShapeInstance.size();i++)
			g_CurrentThreadProcess->addWorkItem(mAnimateShapeInstance[i]);
	}

	mAnimateShapeInstance.clear();

	mBegin = true;
	mStepOne = true;
}

void CMultThreadWorkMgr::aniCallback(void* param)
{
	TSShapeInstance *pShapeIns =  (TSShapeInstance *)param;
	pShapeIns->animate();
}

void CMultThreadWorkMgr::addInAnimateShapeInsList(TSShapeInstance *obj)
{
	//for(int i=mAnimateShapeInstance.size()-1;i>=0;i--)
	//{
	//	if(obj == mAnimateShapeInstance[i]->pShapeIns)
	//	{
	//		delete obj;
	//		return;
	//	}
	//}

	stThreadParam *pParam = new stThreadParam;

	pParam->pParam = obj;
	pParam->pFunc = aniCallback;
	pParam->pShapeIns = obj;
	mAnimateShapeInstance.push_back(pParam);
}

void CMultThreadWorkMgr::addInProcessList(stThreadParam *obj)
{
	//for(int i=mProcessList.size()-1;i>=0;i--)
	//{
	//	if(obj->pObj == mProcessList[i]->pObj)
	//	{
	//		delete obj->pParam;
	//		delete obj;
	//		return;
	//	}
	//}

	mProcessList.push_back(obj);
}

