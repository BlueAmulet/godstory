//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgLighting.h"
#include "lightingSystem/synapseGaming/sgDecalProjector.h"
#include "sceneGraph/decalManager.h"
#include "sim/netConnection.h"
#include "sceneGraph/sceneState.h"
#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "renderInstance/renderInstMgr.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* sgDecalProjector::mSetSB = NULL;

extern bool gEditingMission;
IMPLEMENT_CO_NETOBJECT_V1(sgDecalProjector);

sgDecalProjector::sgDecalProjector()
{
	mTypeMask |= StaticObjectType | StaticTSObjectType | StaticRenderedObjectType | DecalObjectType;
	mNetFlags.set(Ghostable | ScopeAlways); 
	mDataBlock = NULL;

	sgProjection = false;
	sgProjectionPoint = Point3F(0.0f, 0.0f, 0.0f);
	sgProjectionNormal = Point3F(0.0f, 0.0f, 0.0f);

	// for after load relinking...
	sgInitNeeded = true;

	mGlobalId = 0xFFFFFFFF;
}

bool sgDecalProjector::onAdd()
{
	if(!Parent::onAdd())
		return false;

	mObjBox.min.set( -0.5, -0.5, -0.5 );
	mObjBox.max.set( 0.5, 0.5, 0.5 );
	resetWorldBox();
	setRenderTransform(mObjToWorld);

	addToScene();

	return true;
}

void sgDecalProjector::onRemove()
{
	if(isClientObject() && gProjectDecalManager)
		gProjectDecalManager->ageDecal(mGlobalId);
	
	removeFromScene();
	Parent::onRemove();
}

bool sgDecalProjector::onNewDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<ProjectDecalData *>(dptr);
	return Parent::onNewDataBlock(dptr);
}

bool sgDecalProjector::prepRenderImage(SceneState* state, const U32 stateKey,
		const U32 startZone, const bool modifyBaseZoneState)
{
	if(!gEditingMission)
		return false;
	if(isLastState(state, stateKey))
		return false;

	setLastState(state, stateKey);

	if(state->isObjectRendered(this))
	{
		RenderInst *ri = gRenderInstManager.allocInst();
		ri->obj = this;
		ri->state = state;
		ri->type = RenderInstManager::RIT_Object;
		gRenderInstManager.addInst(ri);

		/*SceneRenderImage* image = new SceneRenderImage;
		image->obj = this;
		image->isTranslucent = true;
		image->sortType = SceneRenderImage::EndSort;
		state->insertRenderImage(image);*/
	}

	return false;
}

void sgDecalProjector::renderObject(SceneState *state, RenderInst *ri)
{
	if(gEditingMission)
	{
#ifdef STATEBLOCK
		AssertFatal(mSetSB, "sgDecalProjector::renderObject -- mSetSB cannot be NULL.");
		mSetSB->apply();
#else
		// render the directional line...
		GFX->setAlphaBlendEnable(true);
		GFX->setZEnable(true);
		GFX->setSrcBlend(GFXBlendOne);
		GFX->setDestBlend(GFXBlendZero);
		GFX->setTextureStageColorOp(0, GFXTOPDisable);
#endif

		GFX->disableShaders();

		Point3F vector = SG_STATIC_SPOT_VECTOR_NORMALIZED * 10.0f;
		Point3F origin = Point3F(0, 0, 0);
		Point3F point = vector;
		mObjToWorld.mulP(origin);
		mObjToWorld.mulP(point);
		PrimBuild::color3f(0, 1, 0);
		PrimBuild::begin(GFXLineStrip, 2);
		PrimBuild::vertex3fv(origin);
		PrimBuild::vertex3fv(point);
		PrimBuild::end();
	}
}

void sgDecalProjector::sgResetProjection()
{
	sgProjection = false;
	sgProjectionPoint = Point3F(0.0f, 0.0f, 0.0f);
	sgProjectionNormal = Point3F(0.0f, 0.0f, 0.0f);

#ifdef NTJ_EDITOR
	if(isClientObject() && gEditingMission)
		return;
#endif

	//Ray: decal客户端本地载入
	Container *pContainer = NULL;
	if(isServerObject())
		pContainer = &gServerContainer;
	else
		pContainer = &gClientContainer;

	Point3F pos = getPosition();
	Point3F normal = SG_STATIC_SPOT_VECTOR_NORMALIZED * 100.0f;
	getTransform().mulV(normal);

	// <Edit> [8/4/2009 joy] 从上往下投影
	RayInfo info;
	if(!pContainer->castRay(pos, (pos - normal),
		ProjectDecalInstance::smProjectMask, &info, getLayerID()))
	{
		Con::errorf("Error in _sgDropDecal: no drop object found.");
		return;
	}

	sgProjection = true;
	sgProjectionPoint = info.point;
	sgProjectionNormal = info.normal;

	setMaskBits(0xffffffff);
}

bool sgDecalProjector::updateProject(MatrixF &mat)
{
	ProjectDecalInstance* pDecal = gProjectDecalManager->getDecal(mGlobalId);
	if(pDecal)
	{
		if(!getPosition().equal(mat.getPosition()))
		{
			setTransform(mat);
			pDecal->setPos(sgProjectionPoint);
		}
	}
	else
	{
		setTransform(mat);
		sgProject();
	}

	return true;
}

void sgDecalProjector::sgProject()
{
	if((isServerObject()) || (!mDataBlock))
		return;

	if(mGlobalId!=0xFFFFFFFF)
		gProjectDecalManager->ageDecal(mGlobalId);

	if(!sgProjection)
		return;

	//Point3F tandir;
	//getTransform().getColumn(0, &tandir);

	MatrixF trans = getTransform();
	F32 size = mDataBlock->mDiameter * getScale().x;
	F32 RotZ = atan2(trans(1,0),trans(1,1));

	int mode = ProjectDecalInstance::Rotation;
	if(mDataBlock->lifeSpan==0)
		mode |= ProjectDecalInstance::Permanent;

	gProjectDecalManager->addDecal( mode,
									mDataBlock, sgProjectionPoint, mGlobalId, RotZ,
									mDataBlock->mDiameter, mDataBlock->mDiameter*2, 1.0);
}

void sgDecalProjector::triggerTerrainChange(SceneObject* pTerrain)
{
	sgResetProjection();
	sgProject();
}

void sgDecalProjector::setTransform(const MatrixF & mat)
{
	Parent::setTransform(mat);
	sgResetProjection();
	setMaskBits(0xffffffff);
}

U64 sgDecalProjector::packUpdate(NetConnection *con, U64 mask, BitStream *stream)
{
	U64 res = Parent::packUpdate(con, mask, stream);

	if(sgInitNeeded && isServerObject())
	{
		sgInitNeeded = false;
		sgResetProjection();
	}
	
	stream->writeAffineTransform(mObjToWorld);

	if(stream->writeFlag(sgProjection))
	{
		// this is a joke right (no Point3F support)?!?
		stream->write(sgProjectionPoint.x);
		stream->write(sgProjectionPoint.y);
		stream->write(sgProjectionPoint.z);
		stream->write(sgProjectionNormal.x);
		stream->write(sgProjectionNormal.y);
		stream->write(sgProjectionNormal.z);
	}
	
	return res;
}

void sgDecalProjector::unpackUpdate(NetConnection *con, BitStream *stream)
{
	Parent::unpackUpdate(con, stream);
	
	MatrixF ObjectMatrix;
	stream->readAffineTransform(&ObjectMatrix);
	setTransform(ObjectMatrix);

	sgProjection = stream->readFlag();
	if(sgProjection)
	{
		// this is a joke right (no Point3F support)?!?
		stream->read(&sgProjectionPoint.x);
		stream->read(&sgProjectionPoint.y);
		stream->read(&sgProjectionPoint.z);
		stream->read(&sgProjectionNormal.x);
		stream->read(&sgProjectionNormal.y);
		stream->read(&sgProjectionNormal.z);
	}

	sgProject();
}

void sgDecalProjector::inspectPostApply()
{
	Parent::inspectPostApply();
	sgResetProjection();
	setMaskBits(0xffffffff);
}

//-----------------------------------------------

ConsoleFunction(_createDecal, void, 6, 6, "(Point3F pos, Point3F tandir, Point3F norm, "
				"Point3F scale, decalDataBlock) - this method must be called on the client side!")
{
	// is this the client side?
	// doesn't work...
	//if(gDecalManager->isServerObject())
	//	return;

	Point3F pos = Point3F(0, 0, 0);
    Point3F tandir = Point3F(1, 0, 0);
    Point3F normal = Point3F(0, 0, 1);
    Point3F scale = Point3F(1, 1, 1);
    DecalData *decaldata = NULL;

	dSscanf(argv[1],"%f %f %f",&pos.x,&pos.y,&pos.z);
	dSscanf(argv[2],"%f %f %f",&tandir.x,&tandir.y,&tandir.z);
	dSscanf(argv[3],"%f %f %f",&normal.x,&normal.y,&normal.z);
	dSscanf(argv[4],"%f %f %f",&scale.x,&scale.y,&scale.z);

	decaldata = dynamic_cast<DecalData *>(Sim::findObject(argv[5]));

	if(!decaldata)
		return;

	gDecalManager->addDecal(pos, tandir, normal, scale, decaldata);
}

ConsoleFunction(_dropDecal, void, 6, 6, "(Point3F pos, Point3F tandir, Point3F norm, "
				"Point3F scale, decalDataBlock) - this method must be called on the client side! "
				"This method drops a decal onto the interior or "
				"terrain directly below the given position.")
{
	// is this the client side?
	// doesn't work...
	//if(!gDecalManager->isGhost())
	//	return;

	Point3F pos = Point3F(0, 0, 0);
    Point3F tandir = Point3F(1, 0, 0);
    Point3F normal = Point3F(0, 0, 1);
    Point3F scale = Point3F(1, 1, 1);
    DecalData *decaldata = NULL;

	dSscanf(argv[1],"%f %f %f",&pos.x,&pos.y,&pos.z);
	dSscanf(argv[2],"%f %f %f",&tandir.x,&tandir.y,&tandir.z);
	dSscanf(argv[3],"%f %f %f",&normal.x,&normal.y,&normal.z);
	dSscanf(argv[4],"%f %f %f",&scale.x,&scale.y,&scale.z);

	decaldata = dynamic_cast<DecalData *>(Sim::findObject(argv[5]));

	if(!decaldata)
		return;

	RayInfo info;
	if(!gClientContainer.castRay(pos, (pos + Point3F(0, 0, -100)),
		InteriorObjectType | TerrainObjectType, &info))
	{
		Con::errorf("Error in _sgDropDecal: no drop object found.");
		return;
	}

	pos = info.point;
	normal = info.normal;

	gDecalManager->addDecal(pos, tandir, normal, scale, decaldata);
}

void sgDecalProjector::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mSetSB);
}


void sgDecalProjector::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void sgDecalProjector::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void sgDecalProjector::shutdown()
{
	SAFE_DELETE(mSetSB);
}
