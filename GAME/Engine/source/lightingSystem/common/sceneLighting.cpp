//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/gameConnection.h"
#include "console/consoleTypes.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/shadowVolumeBSP.h"
#include "T3D/shapeBase.h"
#include "gui/core/guiCanvas.h"
#include "ts/tsShape.h"
#include "ts/tsShapeInstance.h"
#include "T3D/staticShape.h"
#include "T3D/tsStatic.h"
#include "collision/concretePolyList.h"
#include "collision/depthSortList.h"
#include "lightingSystem/common/sceneLighting.h"
#include "sceneGraph/lightingInterfaces.h"
#include "terrain/terrData.h"
#include "lightingSystem/synapseGaming/sgSceneObjectLightingPlugin.h"

namespace
{
	bool              gTerminateLighting = false;
	F32               gLightingProgress = 0.f;
	char *            gCompleteCallback = 0;
	U32               gConnectionMissionCRC = 0xffffffff;
}

SceneLighting *gLighting = 0;
F32 gParellelVectorThresh = 0.01f;
F32 gPlaneNormThresh = 0.999f;
F32 gPlaneDistThresh = 0.001f;


void SceneLighting::sgNewEvent(U32 light, S32 object, U32 event)
{
	Sim::postEvent(this, new sgSceneLightingProcessEvent(light, object, event), Sim::getTargetTime() + 1);
   // Paint canvas here?
}

//-----------------------------------------------
/*
* Called once per scenelighting - entry point for event system
*/
void SceneLighting::sgLightingStartEvent()
{   
	Con::printf("");
	Con::printf("Starting scene lighting...");

	sgTimeTemp2 = Platform::getRealMilliseconds();

	// clear interior light maps
	for(ObjectProxy **proxyItr = mSceneObjects.begin(); proxyItr != mSceneObjects.end(); proxyItr++)
	{
      ObjectProxy* objprox;
      objprox = *proxyItr;
		// is there an object?
		if(!objprox->getObject())
		{
			AssertFatal(0, "SceneLighting:: missing sceneobject on light start");
			Con::errorf(ConsoleLogEntry::General, "   SceneLighting:: missing sceneobject on light start");
			continue;
		}

      objprox->processLightingStart();
	}

	
	sgNewEvent(0, 0, sgSceneLightingProcessEvent::sgTGEPassSetupEventType);
	//sgNewEvent(0, 0, sgSceneLightingProcessEvent::sgSGPassSetupEventType);
}

/*
* Called once per scenelighting - exit from event system
*/
void SceneLighting::sgLightingCompleteEvent()
{
   Vector<TerrainBlock *> terrBlocks;

	// initialize the objects for lighting
	for(ObjectProxy ** proxyItr = mSceneObjects.begin(); proxyItr != mSceneObjects.end(); proxyItr++)
   {
      ObjectProxy* objprox = *proxyItr;
      TerrainBlock *terr = dynamic_cast<TerrainBlock *>(objprox->getObject());
      if (terr)
         terrBlocks.push_back(terr);
   }

   for (S32 i = 0; i < terrBlocks.size(); i++)
      terrBlocks[i]->postLight(terrBlocks);

	// save out the lighting?
	if(Con::getBoolVariable("$pref::sceneLighting::cacheLighting", true))
	{
		if(!savePersistInfo(mFileName))
			Con::errorf(ConsoleLogEntry::General, "SceneLighting::light: unable to persist lighting!");
		else
			Con::printf("Successfully saved mission lighting file: '%s'", mFileName);
	}

	Con::printf("Scene lighting complete (%3.3f seconds)", (Platform::getRealMilliseconds()-sgTimeTemp2)/1000.f);
	Con::printf("//-----------------------------------------------");
	Con::printf("");

	SimpleQueryList objects;
	gClientContainer.findObjects(StaticTSObjectType|ShapeBaseObjectType, &SimpleQueryList::insertionCallback, &objects);

	for(SceneObject ** itr = objects.mList.begin(); itr != objects.mList.end(); itr++)
	{
		SceneObject* obj = *itr;
		if (obj)
		{
			sgSceneObjectLightingPlugin* lightPlugin = dynamic_cast<sgSceneObjectLightingPlugin*>(obj->getLightingPlugin());
			AssertFatal(lightPlugin, "No lighting plug-in available!");
			if (lightPlugin)
			{
				lightPlugin->mLightingInfo.mInit = false;
			}
		}
	}

	completed(true);
	deleteObject();
}

//-----------------------------------------------
/*
* Called once per scenelighting - used for prepping the
* event system for TGE style scenelighting
*/
void SceneLighting::sgTGEPassSetupEvent()
{
	Con::printf("  Starting TGE based scene lighting...");

	
	sgNewEvent(0, 0, sgSceneLightingProcessEvent::sgTGELightStartEventType);
}

/*
* Called once per light - used for calling preLight on all objects
* Only TGE lights call prelight and continue on to the process event
*/
void SceneLighting::sgTGELightStartEvent(U32 light)
{
	// catch bad light index and jump to complete event
	if(light >= mLights.size())
	{
		sgNewEvent(light, 0, sgSceneLightingProcessEvent::sgTGELightCompleteEventType);
		return;
	}

	// can we use the light?
	if(mLights[light]->mType != LightInfo::Vector)
	{
		sgNewEvent((light+1), 0, sgSceneLightingProcessEvent::sgTGELightStartEventType);
		return;
	}

	// process pre-lighting
	Con::printf("    Lighting with light #%d (TGE vector light)...", (light+1));
	LightInfo *lightobj = mLights[light];
	mLitObjects.clear();

	for(ObjectProxy **proxyItr = mSceneObjects.begin(); proxyItr != mSceneObjects.end(); proxyItr++)
	{
      ObjectProxy* objprox = *proxyItr;

		// is there an object?
		if(!objprox->getObject())
		{
			AssertFatal(0, "SceneLighting:: missing sceneobject on light start");
			Con::errorf(ConsoleLogEntry::General, "      SceneLighting:: missing sceneobject on light start");
			continue;
		}

      if (objprox->tgePreLight(lightobj))
         mLitObjects.push_back(objprox);
	}

	// kick off lighting
	
	sgNewEvent(light, 0, sgSceneLightingProcessEvent::sgTGELightProcessEventType);
}

/*
* Called once for each TGE light and object - used for calling light on an object
*/
void SceneLighting::sgTGELightProcessEvent(U32 light, S32 object)
{
	// catch bad light or object index
	if((light >= mLights.size()) || (object >= mLitObjects.size()))
	{
		sgNewEvent(light, 0, sgSceneLightingProcessEvent::sgTGELightCompleteEventType);
		return;
	}

   //process object and light
   S32 time = Platform::getRealMilliseconds();
	// light object
   LightInfo* li = mLights[light];
   mLitObjects[object]->processTGELightProcessEvent(object, mLitObjects.size(), li);
	   
	sgTGESetProgress(light, object);
	Con::printf("      Object lighting complete (%3.3f seconds)", (Platform::getRealMilliseconds()-time)/1000.f);
   

	// kick off next object event
	
	sgNewEvent(light, (object+1), sgSceneLightingProcessEvent::sgTGELightProcessEventType);
}

/*
* Called once per TGE light - used for calling postLight on all objects
*/
void SceneLighting::sgTGELightCompleteEvent(U32 light)
{
	// catch bad light index and move to the next pass event
	if(light >= mLights.size())
	{
		sgTGESetProgress(mLights.size(), mLitObjects.size());
		Con::printf("  TGE based scene lighting complete (%3.3f seconds)", (Platform::getRealMilliseconds()-sgTimeTemp2)/1000.f);
		
		sgNewEvent(0, 0, sgSceneLightingProcessEvent::sgSGPassSetupEventType);
		//sgNewEvent(0, 0, sgSceneLightingProcessEvent::sgLightingCompleteEventType);
		return;
	}

	// process post-lighting
	// don't do this, SG lighting events will copy terrain light map...
	/*bool islast = (light == (mLights.size() - 1));
	for(U32 o=0; o<mLitObjects.size(); o++)
	{
		if(dynamic_cast<TerrainProxy *>(mLitObjects[o]))
			mLitObjects[o]->postLight(islast);
	}*/

	// kick off next light event
	
	sgNewEvent((light+1), 0, sgSceneLightingProcessEvent::sgTGELightStartEventType);
}

void SceneLighting::sgTGESetProgress(U32 light, S32 object)
{
	// TGE is light based...
	F32 val = (F32)(light * mLitObjects.size()) + object;
	F32 total = (F32)(mLights.size() * mLitObjects.size());

	if(total == 0.0f)
		return;

	val = getMin(val, total);

	// two passes...
	total *= 2.0f;

	gLightingProgress = val / total;
}

//-----------------------------------------------
/*
* Called once per scenelighting - used for prepping the
* event system for SG style scenelighting
*/
void SceneLighting::sgSGPassSetupEvent()
{
	mLitObjects.clear();
	for(ObjectProxy **proxyItr = mSceneObjects.begin(); proxyItr != mSceneObjects.end(); proxyItr++)
	{
		// is there an object?
		if(!(*proxyItr)->getObject())
		{
			AssertFatal(0, "SceneLighting:: missing sceneobject on light start");
			Con::errorf(ConsoleLogEntry::General, "   SceneLighting:: missing sceneobject on light start");
			continue;
		}

		// add all lights
		mLitObjects.push_back(*proxyItr);
	}
	
	sgNewEvent(0, 0, sgSceneLightingProcessEvent::sgSGObjectStartEventType);
}

/*
* Called once per object - used for calling preLight on all SG lights
*/
void SceneLighting::sgSGObjectStartEvent(S32 object)
{
	// catch bad light index and jump to complete event
	if(object >= mLitObjects.size())
	{
		sgNewEvent(0, object, sgSceneLightingProcessEvent::sgSGObjectCompleteEventType);
		return;
	}

   ObjectProxy *obj = mLitObjects[object];
   bool bHandled = obj->processStartObjectLightingEvent(object, mLitObjects.size());
   if (!bHandled)
   {
      Con::printf("    Lighting object %d of %d... %s: %s", (object+1), mLitObjects.size(), obj->getObject()->getClassName(), obj->getObject()->getName());
   }
	
	for(U32 i=0; i<mLights.size(); i++)
	{
		// can we use the light?
		LightInfo *lightobj = mLights[i];
		//if((lightobj->mType == LightInfo::SGStaticPoint) || (lightobj->mType == LightInfo::SGStaticSpot))
			obj->preLight(lightobj);
	}

	sgTimeTemp = Platform::getRealMilliseconds();

	// kick off lighting
	
	
	// this is slow with multiple objects...
	//sgNewEvent(0, object, sgSceneLightingProcessEvent::sgSGObjectProcessEventType);
	// jump right to the method...
	sgSGObjectProcessEvent(0, object);
}

/*
* Called once per object and SG light - used for calling light on an object
*/
void SceneLighting::sgSGObjectProcessEvent(U32 light, S32 object)
{
	// catch bad light or object index
	if((light >= mLights.size()) || (object >= mLitObjects.size()))
	{
		// this is slow with multiple objects...
		//sgNewEvent(0, object, sgSceneLightingProcessEvent::sgSGObjectCompleteEventType);
		// jump right to the method...
		sgSGObjectCompleteEvent(object);
		return;
	}

	// avoid the event overhead...
	// 80 lights == 0.6 seconds an interior without ANY lighting (events only)...
	U32 time = Platform::getRealMilliseconds();
   ObjectProxy* objprox = mLitObjects[object];
	while((light < mLights.size()) && ((Platform::getRealMilliseconds() - time) < 500))
	{
		// can we use the light?
		LightInfo *lightobj = mLights[light];

      objprox->processSGObjectProcessEvent(lightobj);
      
		sgSGSetProgress(light, object);

		light++;
	}

	light--;

	// kick off next light event
	
	sgNewEvent((light+1), object, sgSceneLightingProcessEvent::sgSGObjectProcessEventType);
}

/*
* Called once per object - used for calling postLight on all SG lights
*/
void SceneLighting::sgSGObjectCompleteEvent(S32 object)
{
	// catch bad light index and move to the next pass event
	if(object >= mLitObjects.size())
	{
		sgSGSetProgress(mLights.size(), mLitObjects.size());
		
		sgNewEvent(0, 0, sgSceneLightingProcessEvent::sgLightingCompleteEventType);
		return;
	}

	// process post-lighting
	Con::printf("    Object lighting complete (%3.3f seconds)", (Platform::getRealMilliseconds()-sgTimeTemp)/1000.f);

	// in case Atlas turned off rendering...
	GFX->setAllowRender(true);

	// only the last light does something
	mLitObjects[object]->postLight(true);

/*
	InteriorInstance *interiorinst = dynamic_cast<InteriorInstance *>(mLitObjects[object]->getObject());
	if(interiorinst)
	{
		Interior *detail = interiorinst->getDetailLevel(0);
		for(U32 i=0; i<detail->mNormalLMapIndices.size(); i++)
		{
			GFXTexHandle normHandle = gInteriorLMManager.duplicateBaseLightmap(detail->getLMHandle(),
				interiorinst->getLMHandle(), detail->getNormalLMapIndex(i));
			GBitmap *normLightmap = normHandle->getBitmap();
			
			FileStream output;
			output.open(avar("lightmaps/lm_%d_%d.png", object, i), FileStream::Write);
			normLightmap->writePNG(output);
		}
	}
*/
	/*ObjectProxy *obj = mLitObjects[object];
	for(U32 i=0; i<mLights.size(); i++)
	{
	// can we use the light?
	LightInfo *lightobj = mLights[i];
	if((lightobj->mType == LightInfo::SGStaticPoint) || (lightobj->mType == LightInfo::SGStaticSpot))
	obj->postLight((i == (mLights.size() - 1)));
	}*/

	// kick off next light event
	

	// this is slow with multiple objects...
	//sgNewEvent(0, (object+1), sgSceneLightingProcessEvent::sgSGObjectStartEventType);
	// jump right to the method...
	sgSGObjectStartEvent((object+1));
}

void SceneLighting::sgSGSetProgress(U32 light, S32 object)
{
	// SG is object based...
	F32 val = (F32)((object * mLights.size()) + light);
	F32 total = (F32)(mLights.size() * mLitObjects.size());

	if(total == 0.0f)
		return;

	val = getMin(val, total);

	// two passes...
	total *= 2.0f;

	gLightingProgress = (val / total) + 0.5f;
}

//-----------------------------------------------

void SceneLighting::processEvent(U32 light, S32 object)
{
	sgNewEvent(light, object, sgSceneLightingProcessEvent::sgLightingStartEventType);
}


//-----------------------------------------------


SceneLighting::SceneLighting(AvailableSLInterfaces* lightingInterfaces)
{
   mLightingInterfaces = lightingInterfaces;
	mStartTime = 0;
	mFileName[0] = 0;

	static bool initialized = false;
	if(!initialized)
	{
		Con::addVariable("SceneLighting::terminateLighting", TypeBool, &gTerminateLighting);
		Con::addVariable("SceneLighting::lightingProgress", TypeF32, &gLightingProgress);
		initialized = true;
      mLightingInterfaces->initInterfaces();
	}
}

SceneLighting::~SceneLighting()
{
	gLighting = 0;
	gLightingProgress = 0.f;

	ObjectProxy ** proxyItr;
	for(proxyItr = mSceneObjects.begin(); proxyItr != mSceneObjects.end(); proxyItr++)
		delete *proxyItr;
}

void SceneLighting::getMLName(const char* misName, const U32 missionCRC, const U32 buffSize, char* filenameBuffer)
{
   dSprintf(filenameBuffer, buffSize, "%s_%x.ml", misName, missionCRC);
}

bool SceneLighting::light(BitSet32 flags)
{
	if(!gClientSceneGraph) 
		return(false);

#ifdef NTJ_CLIENT
	//只对当前地形打光
	if(!gClientSceneGraph->getCurrentTerrain())
		return false;
#endif

	mStartTime = Platform::getRealMilliseconds();

	// register static lights
	LightManager * lManager = gClientSceneGraph->getLightManager();
	lManager->registerGlobalLights(true);

   // Notify each system factory that we are beginning to light
   for(SceneLightingInterface** sitr = mLightingInterfaces->mAvailableSystemInterfaces.begin(); sitr != mLightingInterfaces->mAvailableSystemInterfaces.end(); sitr++)
   {
      SceneLightingInterface* si = (*sitr);
      si->processLightingBegin();
   }

	// grab all the lights
	mLights.clear();
	lManager->getAllUnsortedLights(mLights);

	if(!mLights.size())
		return(false);

	// get all the objects and create proxy's for them
   SimpleQueryList objects;	
   gClientContainer.findObjects(mLightingInterfaces->mAvailableObjectTypes, &SimpleQueryList::insertionCallback, &objects);

	for(SceneObject ** itr = objects.mList.begin(); itr != objects.mList.end(); itr++)
	{
		ObjectProxy * proxy = NULL;
      SceneObject* obj = *itr;

      // Create the right chunk for the system 
      for(SceneLightingInterface** sitr = mLightingInterfaces->mAvailableSystemInterfaces.begin(); sitr != mLightingInterfaces->mAvailableSystemInterfaces.end() && proxy == NULL; sitr++)
      {
         SceneLightingInterface* si = (*sitr);
         proxy = si->createObjectProxy(obj, &mSceneObjects);
      }
      
      if (proxy)
      {
		   if(!proxy->calcValidation())
		   {
			   Con::errorf(ConsoleLogEntry::General, "Failed to calculate validation info for object.  Skipped.");
			   delete proxy;
			   continue;
		   }

		   if(!proxy->loadResources())
		   {
			   Con::errorf(ConsoleLogEntry::General, "Failed to load resources for object.  Skipped.");
			   delete proxy;
			   continue;
		   }

		   mSceneObjects.push_back(proxy);
      }
	}

	if(!mSceneObjects.size())
		return(false);

	// grab the missions crc
	U32 missionCRC = calcMissionCRC();

	// remove the '.mis' extension from the mission name
	char misName[256];
	dSprintf(misName, sizeof(misName), "%s", Con::getVariable("$Client::MissionFile"));
	char * dot = dStrstr((const char*)misName, ".mis");
	if(dot)
		*dot = '\0';

	// get the mission name
   //getMLName(misName, missionCRC, 1023, mFileName);
	dSprintf(mFileName,1023,"%s.ml",misName);  //Ray: 固定光照文件名称防止客户端重复生成
	if(!ResourceManager->isValidWriteFileName(mFileName))
	{
		Con::warnf("Invalid filename '%s'.  Failed to light mission.", mFileName);
		return(false);
	}

	// check for some persisted data, check if being forced..
	if(!flags.test(ForceAlways|ForceWritable))
	{
		if(loadPersistInfo(mFileName))
		{
			Con::printf(" Successfully loaded mission lighting file: '%s'", mFileName);

			Con::evaluatef( "SetLoadingValue( 1 );" );
			// touch this file...
			if(!dFileTouch(mFileName))
				Con::warnf("  Failed to touch file '%s'.  File may be read only.", mFileName);

			return(false);
		}

		// texture manager must have lighting complete now
		if(flags.test(LoadOnly))
		{
			Con::errorf(ConsoleLogEntry::General, "Failed to load mission lighting!");
			return(false);
		}
	}

	// don't light if file is read-only?
	if(!flags.test(ForceAlways))
	{
		FileStream fileStream;
		if(!ResourceManager->openFileForWrite(fileStream, mFileName))
		{
			Con::errorf(ConsoleLogEntry::General, "SceneLighting::Light: Failed to light mission.  File '%s' cannot be written to.", mFileName);
			return(false);
		}
	}

	// initialize the objects for lighting
	for(ObjectProxy ** proxyItr = mSceneObjects.begin(); proxyItr != mSceneObjects.end(); proxyItr++)
		(*proxyItr)->init();

	// get things started
	Sim::postEvent(this, new sgSceneLightingProcessEvent(0, -1,
		sgSceneLightingProcessEvent::sgLightingStartEventType), Sim::getTargetTime() + 1);
	return(true);
}

void SceneLighting::completed(bool success)
{
	// process the cached lighting files
	processCache();

   // Notify each system factory that we are have lit!
   for(SceneLightingInterface** sitr = mLightingInterfaces->mAvailableSystemInterfaces.begin(); sitr != mLightingInterfaces->mAvailableSystemInterfaces.end(); sitr++)
   {
      SceneLightingInterface* si = (*sitr);
      si->processLightingCompleted(success);
   }

	if(gCompleteCallback && gCompleteCallback[0])
		Con::executef(gCompleteCallback);

   dFree(gCompleteCallback);
   gCompleteCallback = NULL;
}

//------------------------------------------------------------------------------
// Static access method: there can be only one SceneLighting object
bool SceneLighting::lightScene(const char * callback, BitSet32 flags)
{   
	if(gLighting)
	{
      Con::errorf(ConsoleLogEntry::General, "Lighting is already in progress!");
      return false;
	}	

	// register the object
	if(!registerObject())
	{
		AssertFatal(0, "SceneLighting:: Unable to register SceneLighting object!");
		Con::errorf(ConsoleLogEntry::General, "SceneLighting:: Unable to register SceneLighting object!");
		delete this;
		return(false);
	}

	// could have interior resources but no instances (hey, got this far didnt we...)
	GameConnection * con = dynamic_cast<GameConnection*>(NetConnection::getConnectionToServer());
	if(!con)
	{
		Con::errorf(ConsoleLogEntry::General, "SceneLighting:: no GameConnection");
		return(false);
	}
	con->addObject(this);

	// set the globals
	gLighting = this;
	gTerminateLighting = false;
	gLightingProgress = 0.f;
   if (gCompleteCallback)
      dFree(gCompleteCallback);   
	gCompleteCallback = dStrdup(callback);
	gConnectionMissionCRC = con->getMissionCRC();


	if(!light(flags))
	{
		completed(true);
		deleteObject();
		return(false);
	}
	return(true);
}

bool SceneLighting::isLighting()
{
	return(bool(gLighting));
}

/// adds TSStatic objects as shadow casters.
void SceneLighting::addStatic(ShadowVolumeBSP *shadowVolume,
							  SceneObject *sceneobject, LightInfo *light, S32 level)
{
   if (!sceneobject)
      return;

	if(light->mType != LightInfo::Vector)
		return;

	ConcretePolyList polylist;
	const Box3F box;
	const SphereF sphere;
	sceneobject->buildPolyList(&polylist, box, sphere);

	// retrieve the poly list (uses the collision mesh)...
	//sobj->sgAdvancedStaticOptionsData.sgBuildPolyList(sobj, &polylist);

	S32 i, count, vertind[3];
	ConcretePolyList::Poly *poly;

	count = polylist.mPolyList.size();

	// add the polys to the shadow volume...
	for(i=0; i<count; i++)
	{
		poly = (ConcretePolyList::Poly *)&polylist.mPolyList[i];
		AssertFatal((poly->vertexCount == 3), "Hmmm... vert count is greater than 3.");

		vertind[0] = polylist.mIndexList[poly->vertexStart];
		vertind[1] = polylist.mIndexList[poly->vertexStart + 1];
		vertind[2] = polylist.mIndexList[poly->vertexStart + 2];

		if(mDot(PlaneF(polylist.mVertexList[vertind[0]], polylist.mVertexList[vertind[1]],
			polylist.mVertexList[vertind[2]]), light->mDirection) < gParellelVectorThresh)
		{
			ShadowVolumeBSP::SVPoly *svpoly = shadowVolume->createPoly();
			svpoly->mWindingCount = 3;

			svpoly->mWinding[0].set(polylist.mVertexList[vertind[0]]);
			svpoly->mWinding[1].set(polylist.mVertexList[vertind[1]]);
			svpoly->mWinding[2].set(polylist.mVertexList[vertind[2]]);
			svpoly->mPlane = PlaneF(svpoly->mWinding[0], svpoly->mWinding[1], svpoly->mWinding[2]);
			svpoly->mPlane.neg();

			shadowVolume->buildPolyVolume(svpoly, light);
			shadowVolume->insertPoly(svpoly);
		}
	}
}

//------------------------------------------------------------------------------
bool SceneLighting::verifyMissionInfo(PersistInfo::PersistChunk * chunk)
{
	PersistInfo::MissionChunk * info = dynamic_cast<PersistInfo::MissionChunk*>(chunk);
	if(!info)
		return(false);

	PersistInfo::MissionChunk curInfo;
	if(!getMissionInfo(&curInfo))
		return(false);

	return(curInfo.mChunkCRC == info->mChunkCRC);
}

bool SceneLighting::getMissionInfo(PersistInfo::PersistChunk * chunk)
{
	PersistInfo::MissionChunk * info = dynamic_cast<PersistInfo::MissionChunk*>(chunk);
	if(!info)
		return(false);

	info->mChunkCRC = gConnectionMissionCRC ^ PersistInfo::smFileVersion;
	return(true);
}

//------------------------------------------------------------------------------
//Ray: 因为生成光照图速度较慢，编辑器不再自动生成光照图，由设计者指定场景摆放完成之后一次生成
bool SceneLighting::loadPersistInfo(const char * fileName)
{
	SimTime tBegin = Platform::getRealMilliseconds();

	// open the file
	Stream * stream = 0;
	stream = ResourceManager->openStream(fileName);
	if(!stream)
		return(false);

	PersistInfo persistInfo;
	bool success = persistInfo.read(*stream);
	readDynamicShadowData(stream);
	ResourceManager->closeStream(stream);
	if(!success)
		return(false);

#ifndef NTJ_EDITOR
	// verify the mission chunk
	//if(!verifyMissionInfo(persistInfo.mChunks[0]))
	//	return(false);
#endif

   // Create the right chunk for the system    
   for(SceneLightingInterface** sitr = mLightingInterfaces->mAvailableSystemInterfaces.begin(); sitr != mLightingInterfaces->mAvailableSystemInterfaces.end(); sitr++)
   {
      SceneLightingInterface* si = (*sitr);
      if (!si->postProcessLoad(&persistInfo, &mSceneObjects))
      {
         return false;
      }
   }

#ifndef NTJ_EDITOR
	//if(mSceneObjects.size() != (persistInfo.mChunks.size() - 1))
	//	return(false);
#endif

	Vector<PersistInfo::PersistChunk*> chunks;

	// ensure that the scene objects are in the same order as the chunks
	//  - different instances will depend on this
	U32 i;
	for(i = 0; i < mSceneObjects.size(); i++)
	{
		// 0th chunk is the mission chunk
		U32 chunkIdx = i+1;

#ifndef NTJ_EDITOR
		//Ray: 客户端需要检查，因为和编辑器存放出来的顺序不一样了。
		chunkIdx=1;
		for(chunkIdx=1;chunkIdx<persistInfo.mChunks.size();chunkIdx++)
		{
			if(mSceneObjects[i]->isValidChunk(persistInfo.mChunks[chunkIdx]))
				break;
		}

		if(chunkIdx == persistInfo.mChunks.size())		//没找到
			return(true);
#endif

		chunks.push_back(persistInfo.mChunks[chunkIdx]);
	}

	// get the objects to load in the persisted chunks
	for(i = 0; i < mSceneObjects.size(); i++)
		if(!mSceneObjects[i]->setPersistInfo(chunks[i]))
			return(false);

	SimTime tEnd = Platform::getRealMilliseconds();
	Con::printf("load %s elapsed time : %d\n", fileName, tEnd-tBegin);
	return(true);
}

void shadowCallback(SceneObject* obj, void *key)
{
	Vector<SceneObject*> * pList = (Vector<SceneObject*> *)key;
	if(obj->haveDynamicShadow())
		pList->push_back(obj);
}

bool SceneLighting::saveDynamicShadowData(Stream &file)
{
	Vector<SceneObject*> mList;
	gClientContainer.findObjects(StaticTSObjectType, shadowCallback, &mList);
	file.write(tagTinySerializerFileHeader::CurrentVersion);
	file.write((U32)mList.size());
	for(int i=0;i<mList.size();i++)
	{
		file.write((U32)mList[i]->getCRC());
		mList[i]->saveDynamicShadowData(&file);
	}

	return true;
}

bool SceneLighting::readDynamicShadowData(Stream *stream)
{
	Vector<SceneObject*> mList;
	gClientContainer.findObjects(StaticTSObjectType, shadowCallback, &mList);

	int version;
	U32 size,staticHash;
	stream->read(&version);
	stream->read(&size);

	U32 MaxCount=0,LargeCount=0;

	stShadowParam shadowParam;

	for(int i=0;i<size;i++)
	{
		stream->read(&staticHash);
		S32 datasize;
		stream->read(4,&datasize);
		shadowParam.sgShadowPoints.setSize(datasize);
		for (S32 j = 0; j < datasize; j++)
		{
			stream->read(sizeof(Point3F),&shadowParam.sgShadowPoints[j]);
		}

		stream->read(4,&datasize);
		shadowParam.sgShadowPolys.setSize(datasize);
		for (S32 j = 0; j < datasize;j++)
		{
			stream->read(sizeof(PlaneF),&shadowParam.sgShadowPolys[j].plane);
			//stream->read(4,&sgShadowPolys[j].material);
			//stream->read(4,&sgShadowPolys[j].polyFlags);
			//stream->read(4,&sgShadowPolys[j].surfaceKey);
			stream->read(4,&shadowParam.sgShadowPolys[j].vertexCount);
			stream->read(4,&shadowParam.sgShadowPolys[j].vertexStart);

			if(shadowParam.sgShadowPolys[j].vertexCount > MaxCount)
				MaxCount = shadowParam.sgShadowPolys[j].vertexCount;

			if(shadowParam.sgShadowPolys[j].vertexCount>2000)
			{
				LargeCount++;
			}
		}

		stream->read(sizeof(Point3F),&shadowParam.min);
		stream->read(sizeof(Point3F),&shadowParam.max);

#ifdef DEBUG
		//S32 error = 0;
		//for(S32 k=0; k<mList.size()-1; k++)
		//{
		//	SceneObject* obj1 = mList[k];
		//	for(S32 kj=k+1;kj<mList.size(); kj++)
		//	{
		//		SceneObject* obj2 = mList[kj];
		//		if (obj1->getCRC()==obj2->getCRC())
		//		{
		//			Point3F pos1 = obj1->getPosition();
		//			Point3F pos2 = obj2->getPosition();
		//			error++;
		//		}
		//	}
		//}
#endif

		bool flag = false;
		S32 k;
		for( k=0; k<mList.size(); k++)
		{
			SceneObject* obj = mList[k];
			if (obj->getCRC()==staticHash)
			{
				obj->setDynamicShadowData(&shadowParam,NULL);
				flag = true;
				break;
			}
		}

		if(flag)
			mList.erase(k);

#ifdef DEBUG
		//if(!flag)
		//{
		//	int a = 0;
		//}
#endif
	}

	return true;
}

bool SceneLighting::savePersistInfo(const char * fileName)
{
	// open the file
	FileStream file;
	if(!ResourceManager->openFileForWrite(file, fileName))
		return(false);

	PersistInfo persistInfo;

	// add in the mission chunk
	persistInfo.mChunks.push_back(new PersistInfo::MissionChunk);

	// get the mission info, will return false when there are 0 lights
	if(!getMissionInfo(persistInfo.mChunks[0]))
		return(false);

	// get all the persist chunks
   bool bChunkFound;
	for(U32 i = 0; i < mSceneObjects.size(); i++)
	{
      bChunkFound = false;
      // Create the right chunk for the system 
      for(SceneLightingInterface** sitr = mLightingInterfaces->mAvailableSystemInterfaces.begin(); sitr != mLightingInterfaces->mAvailableSystemInterfaces.end() && !bChunkFound; sitr++)
      {
         SceneLightingInterface* si = (*sitr);
         PersistInfo::PersistChunk* chunk;
         if (si->createPersistChunkFromProxy(mSceneObjects[i], &chunk))
         {
            if (chunk)
            {
               persistInfo.mChunks.push_back(chunk);
               bChunkFound = true;
            }            
         }
      }   

      // Make sure the chunk worked.
      if (!mSceneObjects[i]->getPersistInfo(persistInfo.mChunks.last()))
         return false;
	}

	if(!persistInfo.write(file))
		return(false);

	saveDynamicShadowData(file);

	file.close();

	// open/close the stream to get the fileSize calculated on the resource object
	ResourceManager->closeStream(ResourceManager->openStream(fileName));
	return(true);
}

struct CacheEntry {
	ResourceObject *  mFileObject;
	const char *      mFileName;

	CacheEntry() {
		mFileObject = 0;
		mFileName = 0;
	};
};

// object list sort methods: want list in reverse
static int QSORT_CALLBACK minSizeSort(const void * p1, const void * p2)
{
	const CacheEntry * entry1 = (const CacheEntry *)p1;
	const CacheEntry * entry2 = (const CacheEntry *)p2;

	return(entry2->mFileObject->fileSize - entry1->mFileObject->fileSize);
}

static int QSORT_CALLBACK maxSizeSort(const void * p1, const void * p2)
{
	const CacheEntry * entry1 = (const CacheEntry *)p1;
	const CacheEntry * entry2 = (const CacheEntry *)p2;

	return(entry1->mFileObject->fileSize - entry2->mFileObject->fileSize);
}

static int QSORT_CALLBACK lastCreatedSort(const void * p1, const void * p2)
{
	const CacheEntry * entry1 = (const CacheEntry *)p1;
	const CacheEntry * entry2 = (const CacheEntry *)p2;

	FileTime create[2];
	FileTime modify;

	bool ret[2];

	ret[0] = Platform::getFileTimes(entry1->mFileName, &create[0], &modify);
	ret[1] = Platform::getFileTimes(entry2->mFileName, &create[1], &modify);

	// check return values
	if(!ret[0] && !ret[1])
		return(0);
	if(!ret[0])
		return(1);
	if(!ret[1])
		return(-1);

	return(Platform::compareFileTimes(create[1], create[0]));
}

static int QSORT_CALLBACK lastModifiedSort(const void * p1, const void * p2)
{
	const CacheEntry * entry1 = (const CacheEntry *)p1;
	const CacheEntry * entry2 = (const CacheEntry *)p2;

	FileTime create;
	FileTime modify[2];

	bool ret[2];

	ret[0] = Platform::getFileTimes(entry1->mFileName, &create, &modify[0]);
	ret[1] = Platform::getFileTimes(entry2->mFileName, &create, &modify[1]);

	// check return values
	if(!ret[0] && !ret[1])
		return(0);
	if(!ret[0])
		return(1);
	if(!ret[1])
		return(-1);

	return(Platform::compareFileTimes(modify[1], modify[0]));
}

void SceneLighting::processCache()
{
	return;
	// get size in kb
	S32 quota = Con::getIntVariable("$pref::sceneLighting::cacheSize", -1);

	Vector<CacheEntry> files;

	ResourceObject * match = 0;
	const char * name;

	S32 curCacheSize = 0;
	match = ResourceManager->findMatch("*.ml", &name, 0);
	while(match)
	{
		if(match->flags & ResourceObject::File)
		{
			// dont allow the current file to be removed...
			if(!dStrstr(name, mFileName))
			{
				CacheEntry entry;
				entry.mFileObject = match;

				// get out of vfs...
				char fileName[1024];
				dSprintf(fileName, sizeof(fileName), "%s/%s", match->path, match->name);

				entry.mFileName = StringTable->insert(fileName);
				files.push_back(entry);
			}
			else
				curCacheSize += match->fileSize;
		}

		match = ResourceManager->findMatch("*.ml", &name, match);
	}

	// remove old files
	for(S32 i = files.size() - 1; i >= 0; i--)
	{
		char buf[1024];
		dSprintf(buf, sizeof(buf), "%s/%s", files[i].mFileObject->path, files[i].mFileObject->name);

		Stream * stream = ResourceManager->openStream(buf);
		if(!stream)
			continue;

		// read in the version
		U32 version;
		bool ok = (stream->read(&version) && (version == PersistInfo::smFileVersion));
		ResourceManager->closeStream(stream);

		// ok?
		if(ok)
			continue;

		// delete the file
		ResourceManager->freeResource(files[i].mFileObject);

		// no sneaky names
		if(!dStrstr(files[i].mFileName, ".."))
		{
			Con::warnf("Removing old lighting file '%s'.", files[i].mFileName);
			dFileDelete(files[i].mFileName);
		}

		files.pop_back();
	}

	// no size restriction?
	if(quota == -1 || !files.size())
		return;

	for(U32 i = 0; i < files.size(); i++)
		curCacheSize += files[i].mFileObject->fileSize;

	// need to remove?
	if(quota > (curCacheSize >> 10))
		return;

	// sort the entries by the correct method
	const char * purgeMethod = Con::getVariable("$pref::sceneLighting::purgeMethod");
	if(!purgeMethod)
		purgeMethod = "";

	// determine the method (default to least recently used)
	if(!dStricmp(purgeMethod, "minSize"))
		dQsort(files.address(), files.size(), sizeof(CacheEntry), minSizeSort);
	else if(!dStricmp(purgeMethod, "maxSize"))
		dQsort(files.address(), files.size(), sizeof(CacheEntry), maxSizeSort);
	else if(!dStricmp(purgeMethod, "lastCreated"))
		dQsort(files.address(), files.size(), sizeof(CacheEntry), lastCreatedSort);
	else
		dQsort(files.address(), files.size(), sizeof(CacheEntry), lastModifiedSort);

	// go through and remove the best candidate first (sorted reverse)
	while(((curCacheSize >> 10) > quota) && files.size())
	{
		curCacheSize -= files.last().mFileObject->fileSize;
		ResourceManager->freeResource(files.last().mFileObject);

		// no sneaky names
		if(!dStrstr(files.last().mFileName, ".."))
		{
			Con::warnf("Removing lighting file '%s'.", files.last().mFileName);
			dFileDelete(files.last().mFileName);
		}

		files.pop_back();
	}
}

static S32 QSORT_CALLBACK compareS32(const void * a, const void * b)
{
	return(*((S32 *)a) - *((S32 *)b));
}

U32 SceneLighting::calcMissionCRC()
{
	// all the objects + mission chunk
	Vector<U32> crc;

	// grab the object crcs
	for(U32 i = 0; i < mSceneObjects.size(); i++)
      crc.push_back( mSceneObjects[i]->mChunkCRC );

	// grab the missions crc
	PersistInfo::MissionChunk curInfo;
	getMissionInfo(&curInfo);
	crc.push_back(curInfo.mChunkCRC);

	// sort them (order may not have been preserved)
	dQsort(crc.address(), crc.size(), sizeof(U32), compareS32);

#ifdef POWER_BIG_ENDIAN
   // calculateCRC operates on 8-bit chunks of memory. The memory is a vector
   // of U32's, and so the result will be different on big/little endian hardware.
   // To fix this, swap endians on the CRC's in the vector. This must be done
   // _after_ the qsort.
   for( int i = 0; i < crc.size(); i++ )
      crc[i] = endianSwap( crc[i] );
#endif

	return(calculateCRC(crc.address(), sizeof(U32) * crc.size(), 0xffffffff));
}

bool SceneLighting::ObjectProxy::calcValidation()
{
	mChunkCRC = getResourceCRC();
	if(!mChunkCRC)
		return(false);

	return(true);
}

bool SceneLighting::ObjectProxy::isValidChunk(PersistInfo::PersistChunk * chunk)
{
	return(chunk->mChunkCRC == mChunkCRC);
}

bool SceneLighting::ObjectProxy::getPersistInfo(PersistInfo::PersistChunk * chunk)
{
	chunk->mChunkCRC = mChunkCRC;
	return(true);
}

bool SceneLighting::ObjectProxy::setPersistInfo(PersistInfo::PersistChunk * chunk)
{
	mChunkCRC = chunk->mChunkCRC;
	return(true);
}