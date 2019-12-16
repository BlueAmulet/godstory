//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGDECALMANAGER_H_
#define _SGDECALMANAGER_H_

#include "core/bitStream.h"
#include "T3D/gameBase.h"
#include "sceneGraph/decalManager.h"

class GFXStateBlock;
class sgDecalProjector : public GameBase
{
	typedef GameBase Parent;
	ProjectDecalData *mDataBlock;
	bool onNewDataBlock(GameBaseData* dptr);
protected:
	bool sgInitNeeded;
	bool sgProjection;
	Point3F sgProjectionPoint;
	Point3F sgProjectionNormal;
	U32     mGlobalId;
	bool onAdd();
	void onRemove();
	void sgResetProjection();
	void sgProject();

public:
	sgDecalProjector();
	void inspectPostApply();
	void setTransform(const MatrixF &mat);
	bool prepRenderImage(SceneState* state, const U32 stateKey,
		const U32 startZone, const bool modifyBaseZoneState);
	void renderObject(SceneState *state, RenderInst *ri);
	U64 packUpdate(NetConnection *con, U64 mask, BitStream *stream);
	void unpackUpdate(NetConnection *con, BitStream *stream);
	void triggerTerrainChange(SceneObject*);
	U32 getDecalId(){return mGlobalId;}
	bool updateProject(MatrixF &mat);

	DECLARE_CONOBJECT(sgDecalProjector);
private:
	static GFXStateBlock* mSetSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

#endif//_SGDECALMANAGER_H_
