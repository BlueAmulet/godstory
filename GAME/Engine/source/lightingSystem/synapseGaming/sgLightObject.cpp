//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgLighting.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "sceneGraph/sceneGraph.h"
#include "sim/netConnection.h"

#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "gfx/cubemapData.h"

#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLightObject.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"

#include "T3D/shapeBase.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* sgLightObject::mSetSB = NULL;

extern bool gEditingMission;
extern SceneGraph* gClientSceneGraph;


static EnumTable::Enums gsgLightFeatureTypes[] =
{
	{sgLightInfo::sgFull,				"Full"},
	{sgLightInfo::sgNoCube,			   "NoCube"},
	{sgLightInfo::sgNoSpecCube,		"NoSpecCube"}//,
//	{sgLightInfo::sgNoBumpSpecCube,	"NoBumpSpecCube"}
};
static EnumTable gsgLightFeatureTypeTable(3, &gsgLightFeatureTypes[0]);


IMPLEMENT_CO_DATABLOCK_V1(sgLightObjectData);
IMPLEMENT_CO_NETOBJECT_V1(sgLightObject);

DECLARE_SERIALIBLE( sgLightObject );

sgLightObjectData::sgLightObjectData()
{
	sgStatic = false;
	sgSpot = false;
	sgSpotAngle = 1.0;
	sgAdvancedLightingModel = true;
	sgEffectsDTSObjects = true;
	sgCastsShadows = true;
	sgDiffuseRestrictZone = false;
	sgAmbientRestrictZone = false;
	sgLocalAmbientAmount = 0.0;
	sgSmoothSpotLight = false;
	sgDoubleSidedAmbient = false;
	sgLightingModelName = StringTable->insert("");
	sgUseNormals = false;
	// good fast default...
	sgSupportedFeatures = sgLightInfo::sgNoSpecCube;
	sgMountPoint = 0;
	sgMountTransform.identity();
	sgLightMaskCubeMap = StringTable->insert("");

	bCubemapDataFlag = false;
	pCubemapData = NULL;
}

void sgLightObjectData::initPersistFields()
{
	Parent::initPersistFields();

	addField("StaticLight", TypeBool, Offset(sgStatic, sgLightObjectData));
	addField("SpotLight", TypeBool, Offset(sgSpot, sgLightObjectData));
	addField("SpotAngle", TypeF32, Offset(sgSpotAngle, sgLightObjectData));
	addField("AdvancedLightingModel", TypeBool, Offset(sgAdvancedLightingModel, sgLightObjectData));
	addField("EffectsDTSObjects", TypeBool, Offset(sgEffectsDTSObjects, sgLightObjectData));
	addField("CastsShadows", TypeBool, Offset(sgCastsShadows, sgLightObjectData));
	addField("DiffuseRestrictZone", TypeBool, Offset(sgDiffuseRestrictZone, sgLightObjectData));
	addField("AmbientRestrictZone", TypeBool, Offset(sgAmbientRestrictZone, sgLightObjectData));
	addField("LocalAmbientAmount", TypeF32, Offset(sgLocalAmbientAmount, sgLightObjectData));
	addField("SmoothSpotLight", TypeBool, Offset(sgSmoothSpotLight, sgLightObjectData));
	addField("DoubleSidedAmbient", TypeBool, Offset(sgDoubleSidedAmbient, sgLightObjectData));
	addField("LightingModelName", TypeString, Offset(sgLightingModelName, sgLightObjectData));
	addField("UseNormals", TypeBool, Offset(sgUseNormals, sgLightObjectData));
	addField("SupportedFeatures", TypeEnum, Offset(sgSupportedFeatures, sgLightObjectData), 1, &gsgLightFeatureTypeTable);
	addField("MountPoint", TypeS32, Offset(sgMountPoint, sgLightObjectData));
	addField("MountPosition", TypeMatrixPosition, Offset(sgMountTransform, sgLightObjectData));
	addField("MountRotation", TypeMatrixRotation, Offset(sgMountTransform, sgLightObjectData));
	addField("LightMaskCubeMap", TypeString, Offset(sgLightMaskCubeMap, sgLightObjectData));
}

void sgLightObjectData::packData(BitStream *stream)
{
	Parent::packData(stream);

	stream->write(sgStatic);
	stream->write(sgSpot);
	stream->write(sgSpotAngle);
	stream->write(sgAdvancedLightingModel);
	stream->write(sgEffectsDTSObjects);
	stream->write(sgCastsShadows);
	stream->write(sgDiffuseRestrictZone);
	stream->write(sgAmbientRestrictZone);
	stream->write(sgLocalAmbientAmount);
	stream->write(sgSmoothSpotLight);
	stream->write(sgDoubleSidedAmbient);
	stream->writeString(sgLightingModelName);
	stream->write(sgUseNormals);
	stream->write(sgSupportedFeatures);
	stream->write(sgMountPoint);
	stream->writeAffineTransform(sgMountTransform);
	stream->writeString(sgLightMaskCubeMap);
}

void sgLightObjectData::unpackData(BitStream *stream)
{
	Parent::unpackData(stream);

	stream->read(&sgStatic);
	stream->read(&sgSpot);
	stream->read(&sgSpotAngle);
	stream->read(&sgAdvancedLightingModel);
	stream->read(&sgEffectsDTSObjects);
	stream->read(&sgCastsShadows);
	stream->read(&sgDiffuseRestrictZone);
	stream->read(&sgAmbientRestrictZone);
	stream->read(&sgLocalAmbientAmount);
	stream->read(&sgSmoothSpotLight);
	stream->read(&sgDoubleSidedAmbient);
	sgLightingModelName = StringTable->insert(stream->readSTString());
	stream->read(&sgUseNormals);
	stream->read(&sgSupportedFeatures);
	stream->read(&sgMountPoint);
	stream->readAffineTransform(&sgMountTransform);
	sgLightMaskCubeMap = StringTable->insert(stream->readSTString());
}

//----------------------------------------------

bool sgLightObject::onAdd()
{
	Parent::onAdd();

	//resend data...
	setMaskBits(0xffffffff);

	return true;
}

sgLightObject::~sgLightObject()
{
   SAFE_DELETE(mLight);
}

/*void sgLightObject::onDeleteNotify(SimObject *object)
{
	Parent::onDeleteNotify(object);

	if(object == sgParticleEmitter)
	{
		sgParticleEmitter = NULL;
		sgParticleEmitterGhostIndex = -1;
	}
}*/

void sgLightObject::sgCalculateParticleSystemInfo(NetConnection *con)
{
	sgValidParticleEmitter = false;

	ParticleEmitterNode *node = NULL;
	if(sgParticleEmitterName != StringTable->insert(""))
	{
		node = dynamic_cast<ParticleEmitterNode *>(Sim::findObject(sgParticleEmitterName));
	}
	else
	{
		sgParticleEmitterGhostIndex = -1;
	}
	if(node)
	{
		// ok we have a valid object...
		sgValidParticleEmitter = true;
		if(con)
		{
			// try for the id...
			S32 id = con->getGhostIndex(node);
			if(id > -1)
				sgParticleEmitterGhostIndex = id;
			else
			{
				// this object is key, scope it even if I can't see it...
				node->setScopeAlways();
			}
		}
	}

	sgAttachedObjectGhostIndex = -1;
	if(!sgAttachedObjectPtr.isNull())
	{
		// try for the id...
		S32 id = con->getGhostIndex(sgAttachedObjectPtr);
		if(id > -1)
			sgAttachedObjectGhostIndex = id;
		else
		{
			// this object is key, scope it even if I can't see it...
			// this is bad news for player objects!!!
			// don't add back in...
			//sgAttachedObjectPtr->setScopeAlways();
		}
	}
}

bool sgLightObject::onNewDataBlock(GameBaseData *dptr)
{
   sgLightObjectData *newdb = dynamic_cast<sgLightObjectData*>(dptr);

	if(!newdb || !Parent::onNewDataBlock(dptr))
		return false;

   mDataBlock = newdb;

	return true;
}

void sgLightObject::initPersistFields()
{
	Parent::initPersistFields();

	addField("ParticleColorAttenuation", TypeF32, Offset(sgParticleColorAttenuation, sgLightObject));
	addField("ParticleEmitterName", TypeString, Offset(sgParticleEmitterName, sgLightObject));
}

void sgLightObject::inspectPostApply()
{
	Parent::inspectPostApply();

	setMaskBits(0xffffffff);
}

void sgLightObject::processTick(const Move* move)
{
	Parent::processTick(move);

	if(isServerObject() && sgValidParticleEmitter && (sgParticleEmitterGhostIndex == -1))
	{
		// get the object...
		ParticleEmitterNode *node = dynamic_cast<ParticleEmitterNode *>(Sim::findObject(sgParticleEmitterName));
		if(node)
		{
			setMaskBits(sgParticleSystemMask);
		}
	}

	// alright lets fix attachObject...
	if(isServerObject() && !sgAttachedObjectPtr.isNull() && (sgAttachedObjectGhostIndex == -1))
	{
		// no? keep trying...
		setMaskBits(sgAttachedObjectMask);
	}
}

void sgLightObject::calculateLightPosition()
{
	if(sgAttachedObjectGhostIndex == -1)
		return;

	if(sgAttachedObjectPtr.isNull())
	{
		// try to find the attached object...
		NetConnection *connection = NetConnection::getConnectionToServer();
		AssertFatal((connection), "Invalid net connection.");
		SimObject *sim = connection->resolveGhost(sgAttachedObjectGhostIndex);
		sgAttachedObjectPtr = dynamic_cast<GameBase *>(sim);

		if(sgAttachedObjectPtr.isNull())
			return;
	}

	GameBase *obj = sgAttachedObjectPtr;
	ShapeBase *shape = dynamic_cast<ShapeBase *>(obj);
	if(shape)
	{
		MatrixF mat;
		// this doesn't work well, the object transform
		// is updated less frequently than the render
		// transform, but luckily they're the same...
		//shape->getMountTransform(sgMountPoint, &mat);

		// use this instead!!!
		shape->getRenderMountTransform(mountPoint, &mat);
		mat.mul(mountTransform);
		mAnimationPosition = mat.getPosition();
		setTransform(mat);
	}
	else
	{
		// Yes, so set to attached position.
		mAnimationPosition = obj->getPosition();
		// Set Current Position.
		setPosition(mAnimationPosition);
	}
}

U64 sgLightObject::packUpdate(NetConnection *con, U64 mask, BitStream *stream)
{
	U64 res = Parent::packUpdate(con, mask, stream);

	if(stream->writeFlag(mask & fxLightConfigChangeMask))
	stream->write(sgParticleColorAttenuation);

	if(stream->writeFlag(mask & sgParticleSystemMask))
	{
		if(isServerObject())
		{
			sgCalculateParticleSystemInfo(con);
			if(sgParticleEmitterGhostIndex > -1)
			{
				// transmit the id...
				stream->writeFlag(true);
				stream->writeInt(sgParticleEmitterGhostIndex, NetConnection::GhostIdBitSize);
			}
			else
			{
				stream->writeFlag(false);
			}
		}
		else
		{
			//for demo recording!!!
			//this is called on the client during recording
			//and the server should've already provided the ghostid...
			stream->writeFlag(true);
			stream->writeInt(sgParticleEmitterGhostIndex, NetConnection::GhostIdBitSize);
		}
	}

	if(stream->writeFlag(mask & sgAttachedObjectMask))
	{
		if(isServerObject())
		{
			sgCalculateParticleSystemInfo(con);
			if(sgAttachedObjectGhostIndex > -1)
			{
				// transmit the id...
				stream->writeFlag(true);
				stream->writeInt(sgAttachedObjectGhostIndex, NetConnection::GhostIdBitSize);
			}
			else
			{
				stream->writeFlag(false);
			}
		}
		else
		{
			//for demo recording!!!
			//this is called on the client during recording
			//and the server should've already provided the ghostid...
			stream->writeFlag(true);
			stream->writeInt(sgAttachedObjectGhostIndex, NetConnection::GhostIdBitSize);
		}
	}

	return res;
}

void sgLightObject::unpackUpdate(NetConnection *con, BitStream *stream)
{
	Parent::unpackUpdate(con, stream);

	if(stream->readFlag())
	stream->read(&sgParticleColorAttenuation);

	if(stream->readFlag())
	{
		if(stream->readFlag())
			sgParticleEmitterGhostIndex = stream->readInt(NetConnection::GhostIdBitSize);
		else
			sgParticleEmitterGhostIndex = -1;
	}

	if(stream->readFlag())
	{
		if(stream->readFlag())
		{
			sgAttachedObjectGhostIndex = stream->readInt(NetConnection::GhostIdBitSize);
			mAttached = true;
		}
		else
		{
			sgAttachedObjectGhostIndex = -1;
			sgAttachedObjectPtr = NULL;
			mAttached = false;
		}
	}
}

void sgLightObject::setTransform(const MatrixF & mat)
{
	Parent::setTransform(mat);
	sgMainZone = -1;
}

void sgLightObject::registerLights(LightManager *lightManager, bool lightingScene)
{
	sgAnimateState state;
	
	if(!mEnable || !mDataBlock) return;
	
	// copy and disable animation state...
	if(lightingScene)
	{
		state.sgCopyState(mDataBlock);
		state.sgDisableState(mDataBlock);
	}

	// set mount info...
	char buff[256];
	dSprintf(buff, sizeof(buff), "link%02dPoint", mDataBlock->sgMountPoint);
	mountPoint = StringTable->insert(buff);
	mountTransform = mDataBlock->sgMountTransform;
	
	// get color etc...
	AnimateLight();
	
	// restore animation state...
	if(lightingScene)
		state.sgRestoreState(mDataBlock);

	bool res = (mDataBlock->mLightOn) &&
		((mDataBlock->sgStatic && (mDataBlock->sgEffectsDTSObjects || lightingScene)) ||
		(((!mDataBlock->sgStatic) && (!lightingScene))));
	if(!res)
		return;

   if (!mLight)
      mLight = SGLM->createSGLightInfo();

	mLight->mPos = mAnimationPosition;
	mLight->mColor = mAnimationColour;
	mLight->mAmbient = ColorF(0.0f, 0.0f, 0.0f);
	
	mLight->mRadius = mAnimationRadius;
	mLight->sgCastsShadows = mDataBlock->sgCastsShadows;
	mLight->sgDiffuseRestrictZone = mDataBlock->sgDiffuseRestrictZone;
	mLight->sgAmbientRestrictZone = mDataBlock->sgAmbientRestrictZone;
	mLight->sgLocalAmbientAmount = mDataBlock->sgLocalAmbientAmount;
	mLight->sgSmoothSpotLight = mDataBlock->sgSmoothSpotLight;
	mLight->sgDoubleSidedAmbient = mDataBlock->sgDoubleSidedAmbient;
	mLight->sgAssignedToParticleSystem = false;
	mLight->sgUseNormals = mDataBlock->sgUseNormals;
	mLight->sgSupportedFeatures = sgLightInfo::sgFeatures(mDataBlock->sgSupportedFeatures);

	mLight->sgLightingTransform = getTransform();
	mLight->sgLightingTransform.setPosition(Point3F(0.0, 0.0, 0.0));
	//mLight->sgLightingTransform.inverse();

	if(!mDataBlock->bCubemapDataFlag && !mDataBlock->pCubemapData)
	{
		mDataBlock->pCubemapData = Sim::findObject(mDataBlock->sgLightMaskCubeMap);
		mDataBlock->bCubemapDataFlag = true;
	}

	if(mDataBlock->pCubemapData)
		mLight->sgLightMask = dynamic_cast<CubemapData *>(mDataBlock->pCubemapData)->cubemap;
	else
		mLight->sgLightMask = NULL;
	
	if(mDataBlock->sgLightingModelName && (dStrlen(mDataBlock->sgLightingModelName) > 0))
		mLight->sgLightingModelName = mDataBlock->sgLightingModelName;
	else
	{
		// Lighting Pack 1.3 compatibility...
		if(mDataBlock->sgAdvancedLightingModel)
			mLight->sgLightingModelName = sgLightingModelManager::sgGetAdvancedLightingModelName();
		else
			mLight->sgLightingModelName = sgLightingModelManager::sgGetStockLightingModelName();
	}

	// this is heavy, but updates are rare on statics...
	if((sgMainZone == -1) && isClientObject())
	{
		U32 zone = 0;
		SceneObject *obj;
		gClientSceneGraph->findZone(getPosition(), obj, zone);
		sgMainZone = zone;
	}

	mLight->sgZone[0] = sgMainZone;
	mLight->sgZone[1] = -1;
	for(U32 i=0; i<getNumCurrZones(); i++)
	{
		S32 zone = getCurrZone(i);
		if(zone != sgMainZone)
		{
			mLight->sgZone[1] = zone;
			break;
		}
	}

	if(sgParticleEmitterGhostIndex > -1)
	{
		if(sgParticleEmitterPtr.isNull())
		{
			// try to find the particle emitter...
			NetConnection *connection = NetConnection::getConnectionToServer();
			AssertFatal((connection), "Invalid net connection.");
			SimObject *obj = connection->resolveGhost(sgParticleEmitterGhostIndex);
			ParticleEmitterNode *node = dynamic_cast<ParticleEmitterNode *>(obj);
			if(node)
				sgParticleEmitterPtr = NULL; //node->getParticleEmitter();  Ray: 粒子系统修改过了，这里会有一个粒子列表
			if(!sgParticleEmitterPtr.isNull())
			{
				mLight->mColor = sgParticleEmitterPtr->getCollectiveColor() * sgParticleColorAttenuation;
				mLight->sgAssignedToParticleSystem = true;
			}
		}
		else
		{
			mLight->mColor = sgParticleEmitterPtr->getCollectiveColor() * sgParticleColorAttenuation;
			mLight->mColor.clamp();
			mLight->sgAssignedToParticleSystem = true;
		}
	//if(mLight->mColor.red == 0.0f && mLight->mColor.green == 0.0f && mLight->mColor.blue == 0.0f)
	//	mLight->mColor = mLight->mColor;
	//	Con::printf("%f %f %f %f", mLight->mColor.red, mLight->mColor.green, mLight->mColor.blue, mLight->mColor.alpha);
	}
	
	// do after finding the particle color...
	sgLightManager::sgGetFilteredLightColor(mLight->mColor, mLight->mAmbient, sgMainZone);

	if((mLight->mColor.red == 0.0f) && (mLight->mColor.green == 0.0f) &&
	   (mLight->mColor.blue == 0.0f))
		return;

	if(mDataBlock->sgStatic)
	{
		if(mDataBlock->sgSpot)
		{
			Point3F origin = Point3F(0, 0, 0);
			mObjToWorld.mulP(origin);
			mLight->mDirection = SG_STATIC_SPOT_VECTOR_NORMALIZED;
			mObjToWorld.mulP(mLight->mDirection);
			mLight->mDirection = origin - mLight->mDirection;

			mLight->mType = sgLightInfo::SGStaticSpot;
			mLight->sgSpotAngle = mDataBlock->sgSpotAngle;
			mLight->sgSpotPlane = PlaneF(mLight->mPos, -mLight->mDirection);
		}
		else
			mLight->mType = sgLightInfo::SGStaticPoint;
	}
	else
	{
		if(mDataBlock->sgSpot)
		{
			Point3F origin = Point3F(0, 0, 0);
			mObjToWorld.mulP(origin);
			mLight->mDirection = SG_STATIC_SPOT_VECTOR_NORMALIZED;
			mObjToWorld.mulP(mLight->mDirection);
			mLight->mDirection = origin - mLight->mDirection;

			mLight->mType = sgLightInfo::Spot;
			mLight->sgSpotAngle = mDataBlock->sgSpotAngle;
			mLight->sgSpotPlane = PlaneF(mLight->mPos, -mLight->mDirection);
		}
		else
			mLight->mType = sgLightInfo::Point;
	}

	SGLM->registerGlobalLight(mLight, this, true);
}

void sgLightObject::renderObject(SceneState *state, RenderInst *ri)
{
	// render the fxLight?
	if(mDataBlock->mFlareOn || gEditingMission)
		Parent::renderObject(state, ri);

	if(!mDataBlock->sgSpot)
		return;

	if(gEditingMission)
	{

#ifdef STATEBLOCK
		AssertFatal(mSetSB, "sgLightObject::renderObject -- mSetSB cannot be NULL.");
		mSetSB->apply();
		GFX->disableShaders();
#else
		// render the spotlight cone...
		GFX->setAlphaBlendEnable(false);
		GFX->disableShaders();
		GFX->setTextureStageColorOp(0, GFXTOPDisable);
#endif
		Point3F vector = SG_STATIC_SPOT_VECTOR_NORMALIZED * mDataBlock->mRadius;
		Point3F origin = Point3F(0, 0, 0);
		Point3F point = vector;
		mObjToWorld.mulP(origin);
		mObjToWorld.mulP(point);

		PrimBuild::color(ColorF(0.0, 1.0, 0.0));
		PrimBuild::begin(GFXLineList, 2);
		PrimBuild::vertex3fv(origin);
		PrimBuild::vertex3fv(point);
		PrimBuild::end();

		S32 i;
		F32 halfangle = mDegToRad(mDataBlock->sgSpotAngle) * 0.5;
		F32 zamount = mCos(halfangle) * vector.z;
		F32 otheramount = mSin(halfangle) * vector.z;

		for(i=0; i<4; i++)
		{
			U32 nonzcomponent = (i & 0x1);
			Point3F outerpoint = vector;
			outerpoint[2] = zamount;
			outerpoint[(int)nonzcomponent] = otheramount;
			if(i & 2)
				outerpoint[(int)nonzcomponent] *= -1;
			mObjToWorld.mulP(outerpoint);

			PrimBuild::color(ColorF(1.0, 1.0, 0.0));
			PrimBuild::begin(GFXLineList, 2);
			PrimBuild::vertex3fv(origin);
			PrimBuild::vertex3fv(outerpoint);
			PrimBuild::end();
		}
	}
}

void sgLightObject::attachToObject(GameBase *obj)
{
	if(isServerObject())
	{
		processAfter(obj);
		sgAttachedObjectPtr = obj;
		setMaskBits(sgAttachedObjectMask);
	}
}

void sgLightObject::detachFromObject()
{
	if(isServerObject())
	{
		clearProcessAfter();
		sgAttachedObjectPtr = NULL;
		setMaskBits(sgAttachedObjectMask);
	}
}

void sgLightObject::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mSetSB);
}


void sgLightObject::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void sgLightObject::init()
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

void sgLightObject::shutdown()
{
	SAFE_DELETE(mSetSB);
}

//addField("ParticleColorAttenuation", TypeF32, Offset(sgParticleColorAttenuation, sgLightObject));
//addField("ParticleEmitterName", TypeString, Offset(sgParticleEmitterName, sgLightObject));


void sgLightObject::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	stream << sgParticleColorAttenuation;

	stream.writeString( sgParticleEmitterName );
}

void sgLightObject::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	stream >> sgParticleColorAttenuation;

	char buf[1024];
	stream.readString( buf, 1024 );
	sgParticleEmitterName = StringTable->insert( buf );

}

ConsoleMethod(sgLightObject, attachToObject, void, 3, 3, "(SimObject obj) Attach to the SimObject obj.")
{
	GameBase *obj = dynamic_cast<GameBase*>(Sim::findObject(argv[2]));
	if(obj)
		object->attachToObject(obj);
}

ConsoleMethod(sgLightObject, detachFromObject, void, 2, 2, "() Detach from the object previously set by attachToObject.")
{
	object->detachFromObject();
}

//-----------------------------------------------
//-----------------------------------------------

/* Support for PowerEngine Lighting Kit object types.
 *
 * Some objects types were renamed, these definitions
 * allow porting of existing objects and data.
 */
class sgUniversalStaticLightData : public sgLightObjectData
{
	typedef sgLightObjectData Parent;
public:
	DECLARE_CONOBJECT(sgUniversalStaticLightData);
};

class sgUniversalStaticLight : public sgLightObject
{
	typedef sgLightObject Parent;
public:
	DECLARE_CONOBJECT(sgUniversalStaticLight);
};

IMPLEMENT_CO_DATABLOCK_V1(sgUniversalStaticLightData);
IMPLEMENT_CO_NETOBJECT_V1(sgUniversalStaticLight);

