//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "sceneGraph\sceneGraph.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay\GameObjects\PlayerObject.h"
#include ".\ObjectLoadManager.h"

U32 ObjectLoadManager::nUnSignedTerrainIndex = 0xFFFFFFFF;

ObjectLoadManager gClientObjectLoadMgr;

TerrainBlockInfo::TerrainBlockInfo(TerrainBlock *obj, U32 size)
{
	mblock = obj;
	mBinArrays.setSize(size);
	U32 count = mBinArrays.size();
	for(U32 i = 0; i < count; i++)
	{
		mBinArrays[i].reserve(4096);
	}
}

TerrainBlockInfo::~TerrainBlockInfo()
{
	for(int i =0; i < mBinArrays.size(); ++i)
    {
        destructInPlace(&mBinArrays[i]);
    }
}

ObjectLoadManager::ObjectLoadManager(void)
{
	nRadiusNum = 1;  //默认用1
	nTotalSize = 512;
	nBinSize = 20;
	nNumBins = (nTotalSize % nBinSize == 0) ? (nTotalSize / nBinSize) : (nTotalSize / nBinSize + 1) ;
	
	nTerrainBlockNum = 0;
	nMaxBlockNum = 8;

	m_nMinX = -1;
	m_nMaxX = -1;
	m_nMinY = -1;
	m_nMaxY = -1;

	mCurrBinIndexX = -1;
	mCurrBinIndexY = -1;

	mPrevBinIndexX = -1;
	mPrevBinIndexY = -1;

	mIsFirstUpdate = true;
	mCurrTerrainIndex = nUnSignedTerrainIndex;

	//mWorker = new CBackWorker(1);

	mWorkTime = 5000;  // us
	mTimer = PlatformTimer::create(PlatformTimer::WIN32_HIGH_PERFORMANCE_TIMER);
	mDynamicObjectLink = new DynamicObjectList;
}

void ObjectLoadManager::InitializeRadius(F32 fVisibleDistance)
{
	nRadiusNum = (S32)fVisibleDistance / nBinSize;
}

ObjectLoadManager::~ObjectLoadManager(void)
{
	clearMangerList();
	clearDynamicList();
	delete mTimer;
}

void ObjectLoadManager::clearMangerList()
{
	if (!mRegistedObjectLink.empty())
	{
		mRegistedObjectLink.clear();
	}
}

void ObjectLoadManager::clearDynamicList()
{
	if (mDynamicObjectLink)
	{
		mDynamicObjectLink->clear();
		delete mDynamicObjectLink;
		mDynamicObjectLink = NULL;
	}
}

void ObjectLoadManager::clearResList()
{
	mLoadResObjectLink.clear();
	mFreeResObjectLink.clear();
}

void ObjectLoadManager::addDynamicObject(GameObject *obj)
{
	SimObjectPtr<GameObject> ptr = obj;
	mDynamicObjectLink->push_back(ptr);
}

void ObjectLoadManager::registBlock(TerrainBlock *obj)
{
	Vector<TerrainBlockInfo *>::iterator it = mBlockInfoVec.begin();
	while(it != mBlockInfoVec.end())
	{
		if ( (*it)->mblock == obj )
		{
			return;
		}
		it++;
	}

	TerrainBlockInfo *pBlockInfo = new TerrainBlockInfo(obj, nNumBins * nNumBins);
	mBlockInfoVec.push_back(pBlockInfo);
	nTerrainBlockNum++;

	AssertFatal(nTerrainBlockNum <= nMaxBlockNum, "exceed 8 TerrainBlocks");
}

void ObjectLoadManager::registObject(RenderableSceneObject *obj, const Box3F &box)
{
	insertIntoBins(obj, box);
	mRegistedObjectLink.push_back(obj);
}

void ObjectLoadManager::unregistObject(RenderableSceneObject *obj, const Box3F &box)
{
	if (mCurrTerrainIndex == nUnSignedTerrainIndex)
	{
		return;
	}

	const Box3F *pTerrainBox = &mBlockInfoVec[mCurrTerrainIndex]->mblock->getWorldBox();

	U32 minX = 0;
	U32 maxX = 0;
	U32 minY = 0;
	U32 maxY = 0;

	getBinRange(box.min.x - pTerrainBox->min.x, box.max.x - pTerrainBox->min.x, minX, maxX);
	getBinRange(box.min.y - pTerrainBox->min.y, box.max.y - pTerrainBox->min.y, minY, maxY);

	deleteFromBinsBlock(obj, mCurrTerrainIndex, minX, maxX, minY, maxY);

	RenderableSceneObjectList::iterator it;

	for (it = mRegistedObjectLink.begin(); it != mRegistedObjectLink.end(); ++it)
	{
		if (*it == obj)
		{
			mRegistedObjectLink.erase(it);
			break;
		}
	}
}

void ObjectLoadManager::unregistBlock(TerrainBlock *obj)
{
	Vector<TerrainBlockInfo *>::iterator it;
	
	for (it = mBlockInfoVec.begin(); it != mBlockInfoVec.end(); ++it)
	{
		TerrainBlockInfo *pTerrainBlockInfo = *it;

		if (obj == (*it)->mblock)
		{
			if (mCurrTerrainIndex != nUnSignedTerrainIndex &&
				*it == mBlockInfoVec[mCurrTerrainIndex])
			{
				clearMangerList();
				mCurrTerrainIndex = nUnSignedTerrainIndex;
				mIsFirstUpdate = true;
			}
			delete (*it);
			mBlockInfoVec.erase(it);
			nTerrainBlockNum--;
			break;
		}
	}
}

void ObjectLoadManager::update(const Point3F &pos)
{
	PROFILE_SCOPE(ObjectLoadManager_update);

	mTimer->getElapsedUs();
	mTimer->reset();

	if (mIsFirstUpdate)
	{
		//freeRegistedObjectRes();
	}

	U32 nBlockID = 0;
	U32 nBinIndexX = 0;
	U32 nBinIndexY = 0;
	bool bUpdateTerrainBlock = false;
	bool bInitial = false;

	if (getBinIndex(pos, nBlockID, nBinIndexX, nBinIndexY))
	{
		if (mIsFirstUpdate)
		{
			initialTerrain(nBlockID);
			bInitial = true;			
		}
		else if (mCurrTerrainIndex != nBlockID)
		{
			// free the current terrain and load the new terrain
			UpdateCurrentTerrain(mCurrTerrainIndex, nBlockID);
			mCurrTerrainIndex = nBlockID;
			bUpdateTerrainBlock = true;
		}

		if ( nBinIndexX != mPrevBinIndexX || 
			 nBinIndexY != mPrevBinIndexY || 
			 bUpdateTerrainBlock)
		{
			mCurrBinIndexX = nBinIndexX;
			mCurrBinIndexY = nBinIndexY;
 			updateRenderdObjectLink(bUpdateTerrainBlock);			
		}
	}

	if (bInitial && mIsFirstUpdate)
	{
		mIsFirstUpdate = false;
	}

	work();
}

bool ObjectLoadManager::getBinIndex(const Point3F &pos, U32 &nBlockID, U32 &nBinIndexX, U32 &nBinIndexY)
{
	bool ret = false;
	const Box3F *pTerrainBox = NULL;

	for (nBlockID = 0; nBlockID < mBlockInfoVec.size(); nBlockID++)
	{
		pTerrainBox = &mBlockInfoVec[nBlockID]->mblock->getWorldBox();

		if (pos.x >= pTerrainBox->min.x &&
			pos.x <= pTerrainBox->max.x &&
			pos.y >= pTerrainBox->min.y &&
			pos.y <= pTerrainBox->max.y)
		{
			nBinIndexX = U32 ( (pos.x - pTerrainBox->min.x) / nBinSize );
			nBinIndexY = U32 ( (pos.y - pTerrainBox->min.y) / nBinSize );

			return true;
		}
	}

	return false;
}

void ObjectLoadManager::insertIntoBins(RenderableSceneObject* obj, const Box3F &box)
{
	AssertFatal(obj != NULL, "No object?");

	// The first thing we do is find which bins are covered in x and y...
	const Box3F *pTerrainBox = NULL;
	U32 nBlockID = 0;
	bool bInsert = false;

	for (nBlockID = 0; nBlockID < mBlockInfoVec.size(); nBlockID++)
	{
		pTerrainBox = &mBlockInfoVec[nBlockID]->mblock->getWorldBox();

		if (box.min.x >= pTerrainBox->min.x && 
			box.max.x <= pTerrainBox->max.x &&
			box.min.y >= pTerrainBox->min.y &&
			box.max.y <= pTerrainBox->max.y)
		{
			bInsert = true;
			break; // obj belong to this TerrainBlock
		}
	}

	U32 minX, maxX, minY, maxY;

	if (bInsert)
	{
		getBinRange(box.min.x - pTerrainBox->min.x, box.max.x - pTerrainBox->min.x, minX, maxX);
		getBinRange(box.min.y - pTerrainBox->min.y, box.max.y - pTerrainBox->min.y, minY, maxY);

		insertIntoBinsBlock(obj, nBlockID, minX, maxX, minY, maxY);

		if (mCurrBinIndexX != -1 && mCurrBinIndexY != -1)
		{
			/*
			if (mIsFirstUpdate)
				loadResource(obj);
			else
				mWorker->postWork( WorkMethod( ObjectLoadManager::loadResource ), this, 1, obj );
			*/
			obj->addBinRef(!mIsFirstUpdate);
		}
	}
}

void ObjectLoadManager::getBinRange(const F32 min,
									const F32 max,
									U32&      minBin,
									U32&      maxBin)
{
	/*
	AssertFatal(max >= min, "Error, bad range! in getBinRange");

	if ((max - min) >= (nTotalSize - nBinSize))
	{
		F32 minCoord = mFmod(min, nTotalSize);
		if (minCoord < 0.0f) 
		{
			minCoord += nTotalSize;

			// This is truly lame, but it can happen.  There must be a better way to
			//  deal with this.
			if (minCoord == nTotalSize)
				minCoord = nTotalSize - 0.01;
		}

		AssertFatal(minCoord >= 0.0 && minCoord < nTotalSize, "Bad minCoord");

		minBin = U32(minCoord / nBinSize);
		AssertFatal(minBin < nNumBins, avar("Error, bad clipping! (%g, %d)", minCoord, minBin));

		maxBin = minBin + (nNumBins - 1);
		return;
	}
	else 
	{
		F32 minCoord = mFmod(min, nTotalSize);

		if (minCoord < 0.0f) 
		{
			minCoord += nTotalSize;

			// This is truly lame, but it can happen.  There must be a better way to
			//  deal with this.
			if (minCoord == nTotalSize)
				minCoord = nTotalSize - 0.01;
		}
		AssertFatal(minCoord >= 0.0 && minCoord < nTotalSize, "Bad minCoord");

		F32 maxCoord = mFmod(max, nTotalSize);
		if (maxCoord < 0.0f) {
			maxCoord += nTotalSize;

			// This is truly lame, but it can happen.  There must be a better way to
			//  deal with this.
			if (maxCoord == nTotalSize)
				maxCoord = nTotalSize - 0.01;
		}
		AssertFatal(maxCoord >= 0.0 && maxCoord < nTotalSize, "Bad maxCoord");

		minBin = U32(minCoord / nBinSize);
		maxBin = U32(maxCoord / nBinSize);
		AssertFatal(minBin < nNumBins, avar("Error, bad clipping(min)! (%g, %d)", maxCoord, minBin));
		AssertFatal(minBin < nNumBins, avar("Error, bad clipping(max)! (%g, %d)", maxCoord, maxBin));

		// MSVC6 seems to be generating some bad floating point code around
		// here when full optimizations are on.  The min != max test should
		// not be needed, but it clears up the VC issue.
		if (min != max && minCoord > maxCoord)
			maxBin += nNumBins;

		AssertFatal(maxBin >= minBin, "Error, min should always be less than max!");
	}
	*/

	minBin = U32(min / nBinSize);
	maxBin = U32(max / nBinSize);

	/*
	if (minBin < 0)
	{
		minBin = 0;
	}
	else if (minBin >= nNumBins)
	{
		minBin = nNumBins - 1;
	}

	if (minBin < 0)
	{
		minBin = 0;
	}
	else if (minBin >= nNumBins)
	{
		minBin = nNumBins - 1;
	}
	*/

	minBin = mClamp(minBin, 0, nNumBins-1);
	maxBin = mClamp(maxBin, 0, nNumBins-1);
	AssertFatal(maxBin >= minBin, "Error, min should always be less than max!");
}

void ObjectLoadManager::insertIntoBinsBlock(RenderableSceneObject* obj,
									U32 nBlockID,
									U32 minX, U32 maxX,
									U32 minY, U32 maxY)
{
	Vector<RenderableSceneObject *> *pBinArray;
	// For huge objects, do not dump them into the bins.  
	// Otherwise, everything goes into the grid...
	if ((maxX - minX + 1) < nNumBins || (maxY - minY + 1) < nNumBins && !obj->isGlobalBounds())
	{
		for (U32 i = minY; i <= maxY; i++)
		{
			U32 insertY = i % nNumBins;
			U32 base    = insertY * nNumBins;
			for (U32 j = minX; j <= maxX; j++)
			{
				U32 insertX = j % nNumBins;

				pBinArray = &mBlockInfoVec[nBlockID]->mBinArrays[base + insertX];
				pBinArray->push_back(obj);
			}
		}
	}
}

void ObjectLoadManager::deleteFromBinsBlock(RenderableSceneObject* obj,
											U32 nBlockID,
											U32 minX, U32 maxX,
											U32 minY, U32 maxY)
{
	Vector<RenderableSceneObject *> *pBinArray;
	Vector<RenderableSceneObject *>::iterator it;

	// For huge objects, do not dump them into the bins.  
	// Otherwise, everything goes into the grid...
	if ((maxX - minX + 1) < nNumBins || (maxY - minY + 1) < nNumBins && !obj->isGlobalBounds())
	{
		for (U32 i = minY; i <= maxY; i++)
		{
			U32 insertY = i % nNumBins;
			U32 base    = insertY * nNumBins;
			for (U32 j = minX; j <= maxX; j++)
			{
				pBinArray = &mBlockInfoVec[nBlockID]->mBinArrays[base + j];
				for (it = pBinArray->begin(); it != pBinArray->end(); ++it)
				{
					if (*it == obj)
					{
						pBinArray->erase(it);
						break;
					}
				}
			}
		}
	}
}

void ObjectLoadManager::freeRegistedObjectRes()
{
	RenderableSceneObjectList::iterator it;

	for (it = mRegistedObjectLink.begin(); it != mRegistedObjectLink.end(); ++it)
	{	
		(*it)->releaseBinRef();
	}
}

void ObjectLoadManager::updateRenderdObjectLink(bool bUpdateTerrainBlock)
{
	PROFILE_SCOPE(updateRenderdObjectLink);

	int testFreeCount = 0;
	int testLoadCount = 0;
	S32 nX, nY;
	U32 base;

	Vector<RenderableSceneObject *> *pBinArray = NULL;
	Vector<RenderableSceneObject *>::iterator it;

	S32 nMinX = (mCurrBinIndexX - nRadiusNum) >= 0 ? (mCurrBinIndexX - nRadiusNum) : 0;
	S32 nMaxX = (mCurrBinIndexX + nRadiusNum) < nNumBins ? (mCurrBinIndexX + nRadiusNum) : (nNumBins - 1);
	S32 nMinY = (mCurrBinIndexY - nRadiusNum) >= 0 ? (mCurrBinIndexY - nRadiusNum) : 0;
	S32 nMaxY = (mCurrBinIndexY + nRadiusNum) < nNumBins ? (mCurrBinIndexY + nRadiusNum) : (nNumBins - 1);
	
	if (!mIsFirstUpdate)
	{
		// free resource
		for (nY = m_nMinY; nY <= m_nMaxY; nY++)
		{
			base    = nY * nNumBins;
			for (nX = m_nMinX; nX <= m_nMaxX; nX++)
			{
				if (nX >= nMinX && nX <= nMaxX && nY >= nMinY && nY <= nMaxY)
					continue;

				pBinArray = &mBlockInfoVec[mCurrTerrainIndex]->mBinArrays[base + nX];
				it = pBinArray->begin();
				testFreeCount += pBinArray->size();
				while(it != pBinArray->end())
				{
					if (mIsFirstUpdate)
					{
						(*it)->releaseBinRef();
					}
					else
					{
						mFreeResObjectLink.push_back(*it);
					}
					++it;
				}
			}
		}
	}

	// load resource
	for (nY = nMinY; nY <= nMaxY; nY++)
	{
		base    = nY * nNumBins;
		for (nX = nMinX; nX <= nMaxX; nX++)
		{
			if (!mIsFirstUpdate && nX >= m_nMinX && nX <= m_nMaxX && nY >= m_nMinY && nY <= m_nMaxY)
				continue;

			pBinArray = &mBlockInfoVec[mCurrTerrainIndex]->mBinArrays[base + nX];
			it = pBinArray->begin();
			testLoadCount += pBinArray->size();
			while(it != pBinArray->end())
			{
				if (mIsFirstUpdate)
				{
					(*it)->addBinRef();
				}
				else
				{
					mLoadResObjectLink.push_back(*it);
				}
				++it;
			}
		}
	}

	// save the current bin indexes
	m_nMinX = nMinX;
	m_nMaxX = nMaxX;
	m_nMinY = nMinY;
	m_nMaxY = nMaxY;

	mPrevBinIndexX = mCurrBinIndexX;
	mPrevBinIndexY = mCurrBinIndexY;

	//Con::printf("testLoadCount = %d", testLoadCount);
	//Con::printf("testFreeCount = %d", testFreeCount);
}

void ObjectLoadManager::initialTerrain(U32 nBlockID)
{
	TerrainBlock *pTerrain = mBlockInfoVec[nBlockID]->mblock;
	if (!pTerrain->isResourceLoaded())
	{
		gClientSceneGraph->setCurrentTerrain(pTerrain);
		pTerrain->loadRenderResource();
	}

	mCurrTerrainIndex = nBlockID;
}

void ObjectLoadManager::UpdateCurrentTerrain(U32 nOld, U32 nNew)
{
	if (nOld == nNew)
	{
		return;
	}

	TerrainBlock *pOldTerrain = mBlockInfoVec[nOld]->mblock;
	TerrainBlock *pNewTerrain = mBlockInfoVec[nNew]->mblock;

	if (pOldTerrain->isResourceLoaded())
	{
		pOldTerrain->freeRenderResource();
	}

	if (!pNewTerrain->isResourceLoaded())
	{
		pNewTerrain->loadRenderResource();
	}

	gClientSceneGraph->setCurrentTerrain(pNewTerrain);
}

CBackWorker* ObjectLoadManager::getBackWorker()
{
	return mWorker;
}

void ObjectLoadManager::work()
{
	PROFILE_SCOPE(ObjectLoadManager_work);

	S32 nMaxLoadCount = 10;
	S32 nCount = 0;
	S32 nElapseTime = 0;

	// 释放最多10个对象的资源
	DynamicObjectList::iterator itr = mDynamicObjectLink->begin();
	while(itr != mDynamicObjectLink->end())
	{
		GameObject *obj = *itr;
		if (obj)
		{
			//obj->onAddInClient();
		}
		mDynamicObjectLink->erase(itr++);
		nElapseTime += mTimer->getElapsedUs();
		if (nElapseTime >= mWorkTime)
		{
			break;
		}
	}

	RenderableSceneObjectList::iterator it;

	// 加载对象的资源
	it = mLoadResObjectLink.begin();
	while(it != mLoadResObjectLink.end())
	{
		// 至少做一次加载
		(*it)->addBinRef();
		mLoadResObjectLink.erase(it++);

		nElapseTime += mTimer->getElapsedUs();
		if (nElapseTime >= mWorkTime)
		{
			break;
		}
	}

	// 释放对象的资源
	it = mFreeResObjectLink.begin();
	while(it != mFreeResObjectLink.end())
	{
		(*it)->releaseBinRef();
		mFreeResObjectLink.erase(it++);
	}
}