//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include <WinSock2.h>
#include <Windows.h>

#include "platform/platformTimer.h"
#ifndef _SCENEOBJECT_H_
#include "sceneGraph/renderableSceneObject.h"
#endif
#ifndef _TERRDATA_H_
#include "terrain/terrData.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif
#ifndef _TSSTATIC_H_
#include "T3D/tsStatic.h"
#endif

#include "Common/BackWorker.h"

#include <list>

class GameObject;

typedef std::list<RenderableSceneObject *> RenderableSceneObjectList;
typedef std::list< SimObjectPtr<GameObject> > DynamicObjectList;

class TerrainBlockInfo
{
public:
	TerrainBlockInfo(TerrainBlock *obj, U32 size);

	~TerrainBlockInfo();

public:
	TerrainBlock *mblock;
	Vector< Vector<RenderableSceneObject *> > mBinArrays;
};

class ObjectLoadManager
{
public:
	ObjectLoadManager(void);
	virtual ~ObjectLoadManager(void);

	void work();
	void update(const Point3F &pos);

	void registBlock(TerrainBlock *obj);
	void unregistBlock(TerrainBlock *obj);
	void registObject(RenderableSceneObject *obj, const Box3F &box);
	void unregistObject(RenderableSceneObject *obj, const Box3F &box);
	void addDynamicObject(GameObject *obj);

	void freeRegistedObjectRes();
	void InitializeRadius(F32 fVisibleDistance);
	CBackWorker* getBackWorker();
	void clearResList();

private:
	void initialTerrain(U32 nBlockID);
	void insertIntoBins(RenderableSceneObject *obj, const Box3F &box);
	void insertIntoBinsBlock(RenderableSceneObject* obj, U32 nBlockID, U32 minX, U32 maxX, U32 minY, U32 maxY);
	void deleteFromBinsBlock(RenderableSceneObject* obj, U32 nBlockID, U32 minX, U32 maxX, U32 minY, U32 maxY);
	void getBinRange(const F32 min, const F32 max, U32& minBin, U32& maxBin);
	bool getBinIndex(const Point3F &pos, U32 &nBlockID, U32 &nBinIndexX, U32 &nBinIndexY);
	void updateRenderdObjectLink(bool bUpdateTerrainBlock = false);
	void UpdateCurrentTerrain(U32 nOld, U32 nNew);
	void clearMangerList();
	void clearDynamicList();

private:
	S32 mWorkTime;
	PlatformTimer *mTimer;
	DWORD mThreadId;
	HANDLE mThreadHandle;

	F32 mVisibleDistance;
	U32 nTerrainBlockNum;
	U32 nNumBins;
	S32 nRadiusNum;
	U32 nTotalSize;
	U32 nBinSize;
	
	U32 nMaxBlockNum;
	Vector<TerrainBlockInfo *> mBlockInfoVec;
	
	
	RenderableSceneObjectList mRegistedObjectLink;

	S32 m_nMinX;
	S32 m_nMaxX;
	S32 m_nMinY;
	S32 m_nMaxY;

	S32 mPrevBinIndexX;
	S32 mPrevBinIndexY;

	S32 mCurrBinIndexX;
	S32 mCurrBinIndexY;

	U32 mCurrTerrainIndex;

	bool mIsFirstUpdate;

	static U32 nUnSignedTerrainIndex;

	CBackWorker* mWorker;

	RenderableSceneObjectList mLoadResObjectLink;
	RenderableSceneObjectList mFreeResObjectLink;
	DynamicObjectList *mDynamicObjectLink;
};

extern ObjectLoadManager gClientObjectLoadMgr;