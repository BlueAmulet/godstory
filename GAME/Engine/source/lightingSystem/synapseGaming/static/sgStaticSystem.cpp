
//Ray:用于生成TSStatic静态光照，目前只是简单的用太阳光来判断遮挡关系，然后生成固定的阴暗值来降低物体表面的颜色。

#include "core/bitVector.h"
#include "sceneGraph/lightingInterfaces.h"
#include "lightingSystem/basicLighting/basicLightManager.h"
#include "lightingSystem/common/sceneLighting.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/static/sgStaticLightmap.h"
#include "lightingSystem/synapseGaming/static/sgStaticProxy.h"
#include "T3D/tsStatic.h"
#include "terrain/terrData.h"
#include "scenegraph/sceneGraph.h"


//
// Lighting system interface
//
class sgStaticSystem : public SceneLightingInterface
{
public:
   sgStaticSystem();
   virtual void init();
   virtual U32 addObjectType();
   virtual SceneLighting::ObjectProxy* createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects);
   virtual PersistInfo::PersistChunk* createPersistChunk(const U32 chunkType);
   virtual bool createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret);      

   // Given a ray, this will return the color from the lightmap of this object, return true if handled
   virtual bool getColorFromRayInfo(RayInfo collision, ColorF& result);
};

//------------------------------------------------------------------------------
// Class sgStaticSystem
//------------------------------------------------------------------------------
sgStaticSystem::sgStaticSystem()
{
	SGLM->getSceneLightingInterface()->registerSystem(this);   
}

void sgStaticSystem::init()
{
}

U32 sgStaticSystem::addObjectType()
{
	return StaticTSObjectType;
}

SceneLighting::ObjectProxy* sgStaticSystem::createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects)
{
	//if ((obj->getTypeMask() & StaticTSObjectType ) != 0)
	//	return new StaticProxy(obj);
	//else
	//	return NULL;

	return NULL;
}

PersistInfo::PersistChunk* sgStaticSystem::createPersistChunk(const U32 chunkType)
{
	//if (chunkType == PersistInfo::PersistChunk::StaticChunkType)
	//	return new StaticChunk();
	//else
	//	return NULL;

	return NULL;
}

bool sgStaticSystem::createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret)
{
	//if (dynamic_cast<StaticProxy*>(objproxy) != NULL)
	//{
	//	*ret = new StaticChunk();
	//	return true;
	//} else {
	//	return false;
	//}
	return false;
}

// Given a ray, this will return the color from the lightmap of this object, return true if handled
bool sgStaticSystem::getColorFromRayInfo(RayInfo collision, ColorF& result)
{
	LightInfo *pLight = gClientSceneGraph->getLightManager()->getSpecialLight(LightManager::slSunLightType);
	if(!pLight)
		return false;

	result = pLight->mAmbient * 2.0;
	return true;

	TSStatic *pObj = dynamic_cast<TSStatic *>(collision.object);
	if (!pObj)
		return false;

	//LightInfo *pLight = gClientSceneGraph->getLightManager()->getSpecialLight(LightManager::slSunLightType);
	//if(!pLight)
	//	return false;

	//Point3F start = collision.point;
	//Point3F end = start + 1000 * pLight->mDirection;

	TerrainBlock *pTerrain = gClientSceneGraph->getCurrentTerrain();
	if(!pTerrain)
		return false;

	Point2F uv;
	F32 terrainlength = (F32)(pTerrain->getSquareSize() * TerrainBlock::BlockSize);
	Point3F pos = pTerrain->getPosition();

	//超过10米不考虑地表了
	F32 height;
	pTerrain->getHeight(Point2F(collision.point.x,collision.point.y),&height);
	if(height>collision.point.z  || collision.point.z-height>10)
		return false;

	uv.x = (collision.point.x - pos.x) / terrainlength;
	uv.y = (collision.point.y - pos.y) / terrainlength;

	// similar to x = x & width...
	uv.x = uv.x - F32(U32(uv.x));
	uv.y = uv.y - F32(U32(uv.y));
	GBitmap* lightmap = pTerrain->lightMap;
	if (!lightmap)
		return false;

	result = lightmap->sampleTexel(uv.x, uv.y);
	// terrain lighting is dim - look into this (same thing done in shaders)...
	result *= 2.0;

	return true;
}

// File private static factory
static sgStaticSystem p_sgStaticSystem;
