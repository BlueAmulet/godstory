//Ray:操作代理
#include "lightingSystem/synapseGaming/static/sgStaticProxy.h"
#include "core/bitVector.h"
#include "lightingSystem/common/sceneLighting.h"
#include "sceneGraph/shadowVolumeBSP.h"
#include "T3D/staticShape.h"
#include "T3D/tsStatic.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "sceneGraph/lightingInterfaces.h"
#include "lightingSystem/common/sceneLighting.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgLightMap.h"
#include "lightingSystem/common/sceneLightingGlobals.h"
#include "lightingSystem/synapseGaming/static/sgStaticLightmap.h"


extern SceneLighting *gLighting;

//------------------------------------------------------------------------------
// Class SceneLighting::TerrainChunk
//------------------------------------------------------------------------------
StaticChunk::StaticChunk()
{
   mChunkType = PersistChunk::StaticChunkType;
   mLightmap = NULL;
}

StaticChunk::~StaticChunk()
{
   SAFE_DELETE(mLightmap);
}

//------------------------------------------------------------------------------

bool StaticChunk::read(Stream & stream)
{
   if(!Parent::read(stream))
      return(false);

   mLightmap = new GBitmap();
   return mLightmap->readPNG(stream);
}

bool StaticChunk::write(Stream & stream)
{
   if(!Parent::write(stream))
      return(false);

   if(!mLightmap)
      return(true);

   if(!mLightmap->writePNG(stream))
      return(false);

   return(true);
}


StaticProxy::StaticProxy(SceneObject * obj) :
Parent(obj)
{
   mLightmap		= NULL;
   sgBakedLightmap	= NULL;
   mLightMapObj		= NULL;
   mHaveLightmap    = false;
}

StaticProxy::~StaticProxy()
{
   delete [] mLightmap;
   delete[] sgBakedLightmap;

   delete mLightMapObj;
}

//-------------------------------------------------------------------------------
void StaticProxy::init()
{
	TSStatic * pObject = getObject();

	AssertFatal(pObject, "no shape");

	pObject->GetLightMapSize(mLightMapWidth,mLightMapHeight);

   mLightmap = new ColorF[mLightMapWidth * mLightMapHeight];
   for(int i=0;i<mLightMapWidth * mLightMapHeight;i++)
   {
	   mLightmap[i].red = 0.0;
	   mLightmap[i].blue= 0.0;
	   mLightmap[i].green = 0.0;
	   mLightmap[i].alpha= 0.0;
   }

   sgBakedLightmap = new ColorF[mLightMapWidth * mLightMapHeight];
   for(int i=0;i<mLightMapWidth * mLightMapHeight;i++)
   {
	   sgBakedLightmap[i].red = 0.0;
	   sgBakedLightmap[i].blue= 0.0;
	   sgBakedLightmap[i].green = 0.0;
	   sgBakedLightmap[i].alpha= 0.0;
   }

   mLightMapObj = new CLightmap;
   mLightMapObj->Initialize(pObject->getShapeInstance(),pObject->getTransform(),mLightMapWidth,mLightMapHeight);
}

void StaticProxy::BakedLightmapping(LightInfo * light, ColorF *pBuffer)
{
	if(sgLights.first() != light)
		return;

	TSStatic * pObject = getObject();
	if(!pObject)
		return;

	pObject->disableCollision();

	ColorF diffuse = ColorF(0.0, 0.0, 0.0);
	ColorF ambient = ColorF(0.0, 0.0, 0.0);
	Point3F lightingnormal = Point3F(0.0, 0.0, 0.0);

	CLightmap::CLightmapInfo *pInfo = NULL;
	sgLightInfo *pLight = NULL;
	for(U32 i=0; i<sgLights.size(); i++)
	{
		pLight = sgLights[i];
		if(pLight->sgCastsShadows && sgLightManager::sgAllowShadows())
		{
			// setup zone info...
			bool isinzone = (pLight->sgZone[0] == 0) || (pLight->sgZone[1] == 0);

			// allow what?
			bool allowdiffuse = (!pLight->sgDiffuseRestrictZone) || isinzone;
			bool allowambient = (!pLight->sgAmbientRestrictZone) || isinzone;

			sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(pLight);   
			model.sgInitStateLM();

			for (U32 v = 0; v < mLightMapHeight; v++)
			{
				for (U32 u = 0; u < mLightMapWidth; u++)
				{
					pInfo = mLightMapObj->GetLightAt(u,v);
					if(pInfo->isValid)
					{
						ambient = diffuse = ColorF(0.0f, 0.0f, 0.0f);
						lightingnormal = VectorF(0.0f, 0.0f, 0.0f);
						model.sgLightingLM(pInfo->pos, pInfo->normal, diffuse, ambient, lightingnormal);

						if(allowdiffuse && ((diffuse.red > SG_MIN_LEXEL_INTENSITY) ||
							(diffuse.green > SG_MIN_LEXEL_INTENSITY) || (diffuse.blue > SG_MIN_LEXEL_INTENSITY)))
						{
							Point3F lightpos = pLight->mPos;
							F32 radius = model.sgGetMaxRadius();
							if( (pInfo->pos-pLight->mPos).len() > radius)
								continue;

							RayInfo info;
							if(pObject->getContainer()->castShadow(lightpos,pInfo->pos,ShadowCasterObjectType, &info, pObject->getLayerID()))
							{
								pBuffer[u + (v * mLightMapWidth)] -= ColorF(0.1,0.1,0.1);
								mHaveLightmap = true;
							}
						}
					}
				}
			}

			model.sgResetState();
		}
	}

	pObject->enableCollision();
}

void StaticProxy::lightVector(LightInfo* light, ColorF *pBuffer)
{
	TSStatic * pObject = getObject();
	if(!pObject)
		return;

	pObject->disableCollision();

	CLightmap::CLightmapInfo *pInfo = NULL;
	sgLightInfo *pLight = (sgLightInfo *)light;

	if(pLight->sgCastsShadows && sgLightManager::sgAllowShadows())
	{
		for (U32 v = 0; v < mLightMapHeight; v++)
		{
			for (U32 u = 0; u < mLightMapWidth; u++)
			{
				pInfo = mLightMapObj->GetLightAt(u,v);
				if(pInfo->isValid)
				{
					Point3F	lightpos = 1000.f * pLight->mDirection * -1;
					lightpos = lightpos + pInfo->pos;

					RayInfo info;
					if(pObject->getContainer()->castShadow(lightpos,pInfo->pos,ShadowCasterObjectType /*StaticTSObjectType*/, &info, pObject->getLayerID()))
					{
						pBuffer[u + (v * mLightMapWidth)] = ColorF(0.1,0.1,0.1);
						mHaveLightmap = true;
					}
					else
					{
						pBuffer[u + (v * mLightMapWidth)] = ColorF(1.0,1.0,1.0);
					}
				}
			}
		}
	}

	pObject->enableCollision();
}

/// reroutes TerrainProxy::preLight for point light and TSStatic support.
bool StaticProxy::preLight(LightInfo * light)
{
	SG_CHECK_LIGHT(light);
	if(!bool(mObj))
		return(false);

	if((light->mType != LightInfo::Vector) &&
		(light->mType != LightInfo::SGStaticPoint) &&
		(light->mType != LightInfo::SGStaticSpot))
		return(false);

	if((light->mType == LightInfo::SGStaticPoint) || (light->mType == LightInfo::SGStaticSpot))
	{
		for(int i=0;i<sgLights.size();i++)
			if(sgLights[i] == light)
				return true;

		sgLights.push_back(static_cast<sgLightInfo*>(light));
	}

	return(true);
}

/// reroutes TerrainProxy::postLight for point light and TSStatic support.
void StaticProxy::postLight(bool lastLight)
{
   TSStatic * pObject = getObject();
   if((!pObject) || (!lastLight))
      return;

   // set the lightmap...
   //把sgBakedLightmap调制到mLightmap中

   SAFE_DELETE(pObject->mLightMap);
   if(mHaveLightmap)
   {
	   pObject->mLightMap = new GBitmap(mLightMapWidth,mLightMapHeight, 0, GFXFormatR8G8B8);

	   ColorF color;
	   U8 * lPtr = pObject->mLightMap->getAddress(0,0);
	   AssertFatal(pObject->mLightMap->bytesPerPixel == 3, "Expecting 24 bit bitmap but found 32 bit");
	   for(U32 i = 0; i < (mLightMapWidth * mLightMapHeight); i++)
	   {
		   color.red = mLightmap[i].red;
		   color.green = mLightmap[i].green;
		   color.blue = mLightmap[i].blue;
		   color.clamp();
		   lPtr[i*3+0] = (U8)(color.red   * 255);
		   lPtr[i*3+1] = (U8)(color.green * 255);
		   lPtr[i*3+2] = (U8)(color.blue  * 255);
	   }
   }
 
   pObject->LightingCompleted();
}

/// reroutes TerrainProxy::light for point light and TSStatic support.
void StaticProxy::light(LightInfo * light)
{
	//AssertFatal(((light->mType == LightInfo::Vector) ||
	//	(light->mType == LightInfo::SGStaticPoint) ||
	//	(light->mType == LightInfo::SGStaticSpot)), "wrong light type");

	//if((light->mType == LightInfo::SGStaticPoint) || (light->mType == LightInfo::SGStaticSpot))
	//{
	//	//根据每个sgLights光源来烘培sgBakedLightmap
	//	BakedLightmapping(light,mLightmap);
	//}

	if(light->mType == LightInfo::Vector)
	{
		//用太阳光填充mLightmap
		lightVector(light,mLightmap);
	}
}

//--------------------------------------------------------------------------
U32 StaticProxy::getResourceCRC()
{
	TSStatic * pObject = getObject();
    if(!pObject)
      return(0);

    return(pObject->getCRC());
}

//--------------------------------------------------------------------------
bool StaticProxy::setPersistInfo(PersistInfo::PersistChunk * info)
{
   if(!Parent::setPersistInfo(info))
      return(false);

   StaticChunk * chunk = dynamic_cast<StaticChunk*>(info);
   AssertFatal(chunk, "StaticProxy::setPersistInfo: invalid info chunk!");

   TSStatic * pObject = getObject();
   if(!pObject)
   {
      return(false);
   }

   mHaveLightmap = true;
   SAFE_DELETE(pObject->mLightMap);

   pObject->mLightMap = new GBitmap( *chunk->mLightmap);

   pObject->LightingCompleted();
   return(true);
}

bool StaticProxy::getPersistInfo(PersistInfo::PersistChunk * info)
{
	if(!mHaveLightmap)
		return true;

   if(!Parent::getPersistInfo(info))
      return(false);

   StaticChunk * chunk = dynamic_cast<StaticChunk*>(info);
   AssertFatal(chunk, "StaticProxy::getPersistInfo: invalid info chunk!");

   TSStatic * pObject = getObject();
   if(!pObject || !pObject->mLightMap)
      return(false);

   SAFE_DELETE(chunk->mLightmap);

   chunk->mLightmap = new GBitmap(*pObject->mLightMap);

   return(true);
}

void StaticProxy::processTGELightProcessEvent(U32 curr, U32 max, LightInfo* currlight)
{ 
   Con::printf("      Lighting static, object %d of %d...", (curr+1), max); 
   light(currlight);
}

void StaticProxy::processSGObjectProcessEvent(LightInfo* currLight)
{
   if (currLight->mType != LightInfo::Vector)
      light(currLight);
}
