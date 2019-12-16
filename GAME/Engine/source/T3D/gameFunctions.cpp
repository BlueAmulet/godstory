//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/gameFunctions.h"
#include "T3D/gameConnection.h"
#include "T3D/camera.h"

#include "atlas/core/atlasFile.h"

#include "console/consoleTypes.h"

#include "gfx/debugDraw.h"

#include "gui/3d/guiTSControl.h"

#include "interior/interiorInstance.h"

#include "materials/materialPropertyMap.h"

#include "sceneGraph/detailManager.h"
#include "sceneGraph/sceneRoot.h"
#include "sceneGraph/decalManager.h"
#include "sceneGraph/pathManager.h"

#include "terrain/sky.h"
#include "terrain/terrRender.h"

#include "ts/tsShapeInstance.h"

#include "util/journal/process.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "UI/dGuiMouseGamePlay.h"
#endif
#ifdef NTJ_EDITOR
#include "UI/dWorldEditor.h"
#endif
#include "ts/tsLogicMarkNode.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* g_gameSetSB = NULL;
GFXStateBlock* g_gameClearSB = NULL;

extern void ShowInit();

extern ResourceInstance *constructAtlasChunkFile(Stream &stream);
#ifdef POWER_ATLAS
extern ResourceInstance *constructAtlasFileResource(Stream &stream);
#endif
extern ResourceInstance *constructTerrainFile(Stream &stream);
extern ResourceInstance *constructTSShape(Stream &);
extern ResourceInstance *constructTSShapeMD2(Stream &);

// Register the initialization and shutdown routines
static void Init3dSubsystems();
static void Shutdown3dSubsystems();

static ProcessRegisterInit sgInit(Init3dSubsystems);
static ProcessRegisterShutdown sgShutdown(Shutdown3dSubsystems);

//------------------------------------------------------------------------------
/// Camera and FOV info
namespace {

   const  U32 MaxZoomSpeed             = 2000;     ///< max number of ms to reach target FOV

   static F32 sConsoleCameraFov        = 90.f;     ///< updated to camera FOV each frame
   static F32 sDefaultFov              = 90.f;     ///< normal FOV
   static F32 sCameraFov               = 90.f;     ///< current camera FOV
   static F32 sTargetFov               = 90.f;     ///< the desired FOV
   static F32 sLastCameraUpdateTime    = 0;        ///< last time camera was updated
   static S32 sZoomSpeed               = 500;      ///< ms per 90deg fov change

} // namespace {}

// query
static SimpleQueryList sgServerQueryList;
static U32 sgServerQueryIndex = 0;

//SERVER FUNCTIONS ONLY
ConsoleFunctionGroupBegin( Containers, "Spatial query functions. <b>Server side only!</b>");

ConsoleFunction(containerFindFirst, const char*, 6, 6, "(bitset type, Point3F point, float x, float y, float z)"
                "Find objects matching the bitmask type within a box centered at point, with extents x, y, z.\n\n"
                "Returns the first object found; thereafter, you can get more results using containerFindNext().")
{
   //find out what we're looking for
   U32 typeMask = U32(dAtoi(argv[1]));

   //find the center of the container volume
   Point3F origin(0.0f, 0.0f, 0.0f);
   dSscanf(argv[2], "%g %g %g", &origin.x, &origin.y, &origin.z);

   //find the box dimensions
   Point3F size(0.0f, 0.0f, 0.0f);
   size.x = mFabs(dAtof(argv[3]));
   size.y = mFabs(dAtof(argv[4]));
   size.z = mFabs(dAtof(argv[5]));

   //build the container volume
   Box3F queryBox;
   queryBox.min = origin;
   queryBox.max = origin;
   queryBox.min -= size;
   queryBox.max += size;

   //initialize the list, and do the query
   sgServerQueryList.mList.clear();
   gServerContainer.findObjects(queryBox, typeMask, SimpleQueryList::insertionCallback, &sgServerQueryList);

   //return the first element
   sgServerQueryIndex = 0;
   char *buff = Con::getReturnBuffer(100);
   if (sgServerQueryList.mList.size())
      dSprintf(buff, 100, "%d", sgServerQueryList.mList[sgServerQueryIndex++]->getId());
   else
      buff[0] = '\0';

   return buff;
}

ConsoleFunction( containerFindNext, const char*, 1, 1, "Get more results from a previous call to containerFindFirst().")
{
   //return the next element
   char *buff = Con::getReturnBuffer(100);
   if (sgServerQueryIndex < sgServerQueryList.mList.size())
      dSprintf(buff, 100, "%d", sgServerQueryList.mList[sgServerQueryIndex++]->getId());
   else
      buff[0] = '\0';

   return buff;
}

ConsoleFunctionGroupEnd( Containers );

//------------------------------------------------------------------------------

bool GameGetCameraTransform(MatrixF *mat, Point3F *velocity)
{
   // Return the position and velocity of the control object
   GameConnection* connection = GameConnection::getConnectionToServer();
   return connection && connection->getControlCameraTransform(0, mat) &&
      connection->getControlCameraVelocity(velocity);
}

//------------------------------------------------------------------------------
ConsoleFunctionGroupBegin( CameraFunctions, "Functions controlling the global camera properties defined in main.cc.");

ConsoleFunction(setDefaultFov, void, 2,2, "(defaultFov) - Set the default FOV for a camera.")
{
   argc;
   sDefaultFov = mClampF(dAtof(argv[1]), MinCameraFov, MaxCameraFov);
   if(sCameraFov == sTargetFov)
      sTargetFov = sDefaultFov;
}

ConsoleFunction(setZoomSpeed, void, 2,2, "(speed) - Set the zoom speed of the camera, in ms per 90deg FOV change.")
{
   argc;
   sZoomSpeed = mClamp(dAtoi(argv[1]), 0, MaxZoomSpeed);
}

ConsoleFunction(setFov, void, 2, 2, "(fov) - Set the FOV of the camera.")
{
   argc;
   sTargetFov = mClampF(dAtof(argv[1]), MinCameraFov, MaxCameraFov);
}

ConsoleFunctionGroupEnd( CameraFunctions );

F32 GameGetCameraFov()
{
   return(sCameraFov);
}

void GameSetCameraFov(F32 fov)
{
   sTargetFov = sCameraFov = fov;
}

void GameSetCameraTargetFov(F32 fov)
{
   sTargetFov = fov;
}

void GameUpdateCameraFov()
{
   F32 time = F32(Platform::getVirtualMilliseconds());

   // need to update fov?
   if(sTargetFov != sCameraFov)
   {
      F32 delta = time - sLastCameraUpdateTime;

      // snap zoom?
      if((sZoomSpeed == 0) || (delta <= 0.f))
         sCameraFov = sTargetFov;
      else
      {
         // gZoomSpeed is time in ms to zoom 90deg
         F32 step = 90.f * (delta / F32(sZoomSpeed));

         if(sCameraFov > sTargetFov)
         {
            sCameraFov -= step;
            if(sCameraFov < sTargetFov)
               sCameraFov = sTargetFov;
         }
         else
         {
            sCameraFov += step;
            if(sCameraFov > sTargetFov)
               sCameraFov = sTargetFov;
         }
      }
   }

   // the game connection controls the vertical and the horizontal
   GameConnection * connection = GameConnection::getConnectionToServer();
   if(connection)
   {
      // check if fov is valid on control object
      if(connection->isValidControlCameraFov(sCameraFov))
         connection->setControlCameraFov(sCameraFov);
      else
      {
         // will set to the closest fov (fails only on invalid control object)
         if(connection->setControlCameraFov(sCameraFov))
         {
            F32 setFov = sCameraFov;
            connection->getControlCameraFov(&setFov);
            sTargetFov = sCameraFov = setFov;
         }
      }
   }

   // update the console variable
   sConsoleCameraFov = sCameraFov;
   sLastCameraUpdateTime = time;
}
//--------------------------------------------------------------------------

#ifdef POWER_DEBUG
// ConsoleFunction(dumpTSShapes, void, 1, 1, "dumpTSShapes();")
// {
//    argc, argv;

//    FindMatch match("*.dts", 4096);
//    ResourceManager->findMatches(&match);

//    for (U32 i = 0; i < match.numMatches(); i++)
//    {
//       U32 j;
//       Resource<TSShape> shape = ResourceManager->load(match.matchList[i]);
//       if (bool(shape) == false)
//          Con::errorf(" aaa Couldn't load: %s", match.matchList[i]);

//       U32 numMeshes = 0, numSkins = 0;
//       for (j = 0; j < shape->meshes.size(); j++)
//          if (shape->meshes[j])
//             numMeshes++;
//       for (j = 0; j < shape->skins.size(); j++)
//          if (shape->skins[j])
//             numSkins++;

//      Con::printf(" aaa Shape: %s (%d meshes, %d skins)", match.matchList[i], numMeshes, numSkins);
//       Con::printf(" aaa   Meshes");
//       for (j = 0; j < shape->meshes.size(); j++)
//       {
//          if (shape->meshes[j])
//             Con::printf(" aaa     %d -> nf: %d, nmf: %d, nvpf: %d (%d, %d, %d, %d, %d)",
//                         shape->meshes[j]->meshType & TSMesh::TypeMask,
//                         shape->meshes[j]->numFrames,
//                         shape->meshes[j]->numMatFrames,
//                         shape->meshes[j]->vertsPerFrame,
//                         shape->meshes[j]->verts.size(),
//                         shape->meshes[j]->norms.size(),
//                         shape->meshes[j]->tverts.size(),
//                         shape->meshes[j]->primitives.size(),
//                         shape->meshes[j]->indices.size());
//       }
//       Con::printf(" aaa   Skins");
//       for (j = 0; j < shape->skins.size(); j++)
//       {
//          if (shape->skins[j])
//             Con::printf(" aaa     %d -> nf: %d, nmf: %d, nvpf: %d (%d, %d, %d, %d, %d)",
//                         shape->skins[j]->meshType & TSMesh::TypeMask,
//                         shape->skins[j]->numFrames,
//                         shape->skins[j]->numMatFrames,
//                         shape->skins[j]->vertsPerFrame,
//                         shape->skins[j]->verts.size(),
//                         shape->skins[j]->norms.size(),
//                         shape->skins[j]->tverts.size(),
//                         shape->skins[j]->primitives.size(),
//                         shape->skins[j]->indices.size());
//       }
//    }
// }
#endif

ConsoleFunction( getControlObjectAltitude, const char*, 1, 1, "Get distance from bottom of controlled object to terrain.")
{
   GameConnection* connection = GameConnection::getConnectionToServer();
   if (connection) {
      ShapeBase* pSB = dynamic_cast<ShapeBase*>(connection->getControlObject());
      if (pSB != NULL && pSB->isClientObject())
      {
         Point3F pos(0.f, 0.f, 0.f);

         // if this object is mounted, then get the bottom position of the mount's bbox
         if(pSB->getObjectMount())
         {
            static Point3F BoxPnts[] = {
               Point3F(0.0f,0.0f,0.0f),
                  Point3F(0.0f,0.0f,1.0f),
                  Point3F(0.0f,1.0f,0.0f),
                  Point3F(0.0f,1.0f,1.0f),
                  Point3F(1.0f,0.0f,0.0f),
                  Point3F(1.0f,0.0f,1.0f),
                  Point3F(1.0f,1.0f,0.0f),
                  Point3F(1.0f,1.0f,1.0f)
            };

            ShapeBase * mount = pSB->getObjectMount();
            Box3F box = mount->getObjBox();
            MatrixF mat = mount->getTransform();
            VectorF scale = mount->getScale();

            Point3F projPnts[8];
            F32 minZ = 1e30f;

            for(U32 i = 0; i < 8; i++)
            {
               Point3F pnt(BoxPnts[i].x ? box.max.x : box.min.x,
                  BoxPnts[i].y ? box.max.y : box.min.y,
                  BoxPnts[i].z ? box.max.z : box.min.z);

               pnt.convolve(scale);
               mat.mulP(pnt, &projPnts[i]);

               if(projPnts[i].z < minZ)
                  pos = projPnts[i];
            }
         }
         else
            pSB->getTransform().getColumn(3, &pos);

         TerrainBlock* pBlock = gClientSceneGraph->getCurrentTerrain();
         if (pBlock != NULL) {
            Point3F terrPos = pos;
            pBlock->getWorldTransform().mulP(terrPos);
            terrPos.convolveInverse(pBlock->getScale());

            F32 height;
            if (pBlock->getHeight(Point2F(terrPos.x, terrPos.y), &height) == true) {
               terrPos.z = height;
               terrPos.convolve(pBlock->getScale());
               pBlock->getTransform().mulP(terrPos);

               pos.z -= terrPos.z;
            }
         }

         char* retBuf = Con::getReturnBuffer(128);
         dSprintf(retBuf, 128, "%g", mFloor(getMax(pos.z, 0.f)));
         return retBuf;
      }
   }

   return "0";
}

ConsoleFunction( getControlObjectSpeed, const char*, 1, 1, "Get speed (but not velocity) of controlled object.")
{
   GameConnection* connection = GameConnection::getConnectionToServer();
   if (connection)
   {
      ShapeBase* pSB = dynamic_cast<ShapeBase*>(connection->getControlObject());
      if (pSB != NULL && pSB->isClientObject()) {
         Point3F vel = pSB->getVelocity();
         F32 speed = vel.len();

         // We're going to force the formating to be what we want...
         F32 intPart = mFloor(speed);
         speed -= intPart;
         speed *= 10;
         speed  = mFloor(speed);

         char* retBuf = Con::getReturnBuffer(128);
         dSprintf(retBuf, 128, "%g.%g", intPart, speed);
         return retBuf;
      }
   }

   return "0";
}

bool GameProcessCameraQuery(CameraQuery *query)
{
   GameConnection* connection = GameConnection::getConnectionToServer();

   if (connection && connection->getControlCameraTransform(0.032f, &query->cameraMatrix))
   {
      query->object = dynamic_cast<ShapeBase*>(connection->getControlObject());
      query->nearPlane = Con::getFloatVariable("$pref::Video::nearPlane", 0.1f);

      Sky* pSky = gClientSceneGraph->getCurrentSky();

      if (pSky)
	  {
         query->farPlane = pSky->getVisibleDistance();
		 query->staticDist = pSky->getStaticObjDistance();
	  }
      else
	  {
         query->farPlane = 1000.0f;
		 query->staticDist = 1000.0f;
	  }

      F32 cameraFov;
#ifdef NTJ_EDITOR
	  cameraFov = dWorldEditor::GetCameraFov();
#endif
#ifdef NTJ_CLIENT
	  #pragma message(ENGINE(更新视角))
	  cameraFov = dGuiMouseGamePlay::GetCameraFov();
#endif
#ifdef NTJ_SERVER
      if(!connection->getControlCameraFov(&cameraFov))
         return false;
#endif

      query->fov = mDegToRad(cameraFov);

	  gClientSceneGraph->setLastView2WorldMX(query->cameraMatrix);

      return true;
   }
   return false;
}

void GameRenderFilters(const CameraQuery& camq)
{
   // Stubbed out currently - check version history for old code.
}

void onWindowZombify()
{
	if(gClientSceneGraph)
		gClientSceneGraph->onZombify();
}

void updateMouseGameplayUI(RectI &updateRegion)
{
#ifdef NTJ_CLIENT
	//g_UIMouseGamePlay->setupCamera(g_UIMouseGamePlay->getPosition(),updateRegion);
	//gClientSceneGraph->updateScene();
#endif
}

void GameRenderWorld()
{
   PROFILE_START(GameRenderWorld);
   FrameAllocator::setWaterMark(0);
#ifdef STATEBLOCK
	AssertFatal(g_gameSetSB, "GameRenderWorld -- g_gameSetSB cannot be NULL.");
	g_gameSetSB->apply();
#else
   GFX->setZEnable( true );
   GFX->setZFunc( GFXCmpLessEqual );

   // Wait cull none? Investigate. -patw
   GFX->setCullMode( GFXCullNone );
#endif

#ifdef NTJ_CLIENT
   //gClientSceneGraph->doRenderScene();
   gClientSceneGraph->renderScene();
#else
   gClientSceneGraph->renderScene();
#endif

#ifdef STATEBLOCK
	AssertFatal(g_gameClearSB, "GameRenderWorld -- g_gameClearSB cannot be NULL.");
	g_gameClearSB->apply();
#else
   GFX->setZEnable( false );
   GFX->setTextureStageMagFilter( 0, GFXTextureFilterPoint );
   GFX->setTextureStageMinFilter( 0, GFXTextureFilterPoint );
   GFX->setTextureStageMipFilter( 0, GFXTextureFilterNone );

#endif

   AssertFatal(FrameAllocator::getWaterMark() == 0,
      "Error, someone didn't reset the water mark on the frame allocator!");
   FrameAllocator::setWaterMark(0);
   PROFILE_END(GameRenderWorld);
}


static void Process3D()
{
   Material::updateTime();
}

static void RegisterGameFunctions()
{
   Con::addVariable("$pref::TS::autoDetail", TypeF32, &DetailManager::smDetailScale);
   Con::addVariable("$pref::visibleDistanceMod", TypeF32, &SceneGraph::smVisibleDistanceMod);

   // updated every frame
   Con::addVariable("cameraFov", TypeF32, &sConsoleCameraFov);

   // Stuff game types into the console
   Con::setIntVariable("$TypeMasks::StaticObjectType",         StaticObjectType);
   Con::setIntVariable("$TypeMasks::EnvironmentObjectType",    EnvironmentObjectType);
   Con::setIntVariable("$TypeMasks::AtlasObjectType",          AtlasObjectType);
   Con::setIntVariable("$TypeMasks::TerrainObjectType",        TerrainObjectType);
   Con::setIntVariable("$TypeMasks::InteriorObjectType",       InteriorObjectType);
   Con::setIntVariable("$TypeMasks::WaterObjectType",          WaterObjectType);
   Con::setIntVariable("$TypeMasks::TriggerObjectType",        TriggerObjectType);
   Con::setIntVariable("$TypeMasks::MarkerObjectType",         MarkerObjectType);
   Con::setIntVariable("$TypeMasks::GameBaseObjectType",       GameBaseObjectType);
   Con::setIntVariable("$TypeMasks::ShapeBaseObjectType",      ShapeBaseObjectType);
   Con::setIntVariable("$TypeMasks::CameraObjectType",         CameraObjectType);
   Con::setIntVariable("$TypeMasks::StaticShapeObjectType",    StaticShapeObjectType);
   Con::setIntVariable("$TypeMasks::GameObjectType",           GameObjectType);
   Con::setIntVariable("$TypeMasks::ItemObjectType",           ItemObjectType);
   Con::setIntVariable("$TypeMasks::VehicleObjectType",        VehicleObjectType);
   Con::setIntVariable("$TypeMasks::VehicleBlockerObjectType", VehicleBlockerObjectType);
   Con::setIntVariable("$TypeMasks::ProjectileObjectType",     ProjectileObjectType);
   Con::setIntVariable("$TypeMasks::ExplosionObjectType",      ExplosionObjectType);
   Con::setIntVariable("$TypeMasks::CorpseObjectType",         CorpseObjectType);
   Con::setIntVariable("$TypeMasks::DebrisObjectType",         DebrisObjectType);
   Con::setIntVariable("$TypeMasks::PhysicalZoneObjectType",   PhysicalZoneObjectType);
   Con::setIntVariable("$TypeMasks::StaticTSObjectType",       StaticTSObjectType);
   Con::setIntVariable("$TypeMasks::StaticRenderedObjectType", StaticRenderedObjectType);
   Con::setIntVariable("$TypeMasks::DamagableItemObjectType",  DamagableItemObjectType);   
}

static void Init3dSubsystems()
{
   // Set up the various 3d resource types.
   ResourceManager->registerExtension(".dif", constructInteriorDIF);
   ResourceManager->registerExtension(".ter", constructTerrainFile);
   ResourceManager->registerExtension(".mod", constructTSShape);
   ResourceManager->registerExtension(".md2", constructTSShapeMD2);
#ifdef POWER_ATLAS
   ResourceManager->registerExtension(".atlas", constructAtlasFileResource);
#endif

   // Set up the global material map object.
   MaterialPropertyMap *map = new MaterialPropertyMap;
   map->registerObject("GlobalMaterialPropertyMap");
   Sim::getRootGroup()->addObject(map);

   // Client scenegraph & root zone.
   gClientSceneGraph = new SceneGraph(true);
   gClientSceneRoot  = new SceneRoot;
   gClientSceneGraph->addObjectToScene(gClientSceneRoot);

   // Server scenegraph & root zone.
   gServerSceneGraph = new SceneGraph(false);
   gServerSceneRoot  = new SceneRoot;
   gServerSceneGraph->addObjectToScene(gServerSceneRoot);

   // Decal manager (just for clients).
   gDecalManager = new DecalManager;
   gClientContainer.addObject(gDecalManager);
   gClientSceneGraph->addObjectToScene(gDecalManager);
   #pragma message(ENGINE(新加Decal投影模式))
   gProjectDecalManager = new ProjectDecalManager;
   gClientContainer.addObject(gProjectDecalManager);
   gClientSceneGraph->addObjectToScene(gProjectDecalManager);
#pragma message(ENGINE(新加逻辑标记点))
	gClientLogicMarkNodeManager = new LogicMarkNodeManager;
	gSeverLogicMarkNodeManager = new LogicMarkNodeManager;
	//gClientContainer.addObject(gClientLogicMarkNodeManager);
	//gClientSceneGraph->addObjectToScene(gClientLogicMarkNodeManager);
   // Misc other subsystems.
   TerrainRender::init();
   InteriorInstance::init();
   TSShapeInstance::init();
   DebugDrawer::init();
   DetailManager::init();
   PathManager::init();
#ifdef NTJ_EDITOR
   ShowInit();
#endif
   MoveManager::init();

   Process::notify(Process3D, PROCESS_TIME_ORDER);

   GameConnection::smFovUpdate.notify(GameSetCameraFov);

   RegisterGameFunctions();
}

static void Shutdown3dSubsystems()
{
   GameConnection::smFovUpdate.remove(GameSetCameraFov);

   Process::remove(Process3D);

   PathManager::destroy();
   DetailManager::shutdown();
   TSShapeInstance::destroy();
   InteriorInstance::destroy();

   gClientSceneGraph->removeObjectFromScene(gDecalManager);
   gClientContainer.removeObject(gDecalManager);
   gClientSceneGraph->removeObjectFromScene(gProjectDecalManager);
   gClientContainer.removeObject(gProjectDecalManager);
   gClientSceneGraph->removeObjectFromScene(gClientSceneRoot);
   gServerSceneGraph->removeObjectFromScene(gServerSceneRoot);

   SAFE_DELETE(gClientSceneRoot);
   SAFE_DELETE(gServerSceneRoot);
   SAFE_DELETE(gClientSceneGraph);
   SAFE_DELETE(gServerSceneGraph);
   SAFE_DELETE(gDecalManager);
   SAFE_DELETE(gProjectDecalManager);
	SAFE_DELETE(gClientLogicMarkNodeManager);
	SAFE_DELETE(gSeverLogicMarkNodeManager);
   TerrainRender::shutdown();
}

void GameResetStateBlock()
{
	//g_gameSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZFunc, GFXCmpLessEqual);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(g_gameSetSB);

	//g_gameClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterPoint);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterPoint);
	GFX->setSamplerState(0, GFXSAMPMipFilter, GFXTextureFilterNone);
	GFX->endStateBlock(g_gameClearSB);
}


void GameReleaseStateBlock()
{
	if (g_gameSetSB)
	{
		g_gameSetSB->release();
	}

	if (g_gameClearSB)
	{
		g_gameClearSB->release();
	}
}

void GameInitsb()
{
	if (g_gameSetSB == NULL)
	{
		g_gameSetSB = new GFXD3D9StateBlock;
		g_gameSetSB->registerResourceWithDevice(GFX);
		g_gameSetSB->mZombify = &GameReleaseStateBlock;
		g_gameSetSB->mResurrect = &GameResetStateBlock;

		g_gameClearSB = new GFXD3D9StateBlock;

		GameResetStateBlock();
	}
}

void GameShutdownsb()
{
	SAFE_DELETE(g_gameSetSB);
	SAFE_DELETE(g_gameClearSB);
}

bool addToMissionGroup(SceneObject *pObj)
{
	SimGroup* grp = (SimGroup*)Sim::findObject("MissionGroup");
	if(grp)
	{
		grp->addObject(pObj);
		return true;
	}

	return false;
}

void addToUserDataBlockGroup(SimDataBlock *pBlock)
{
	pBlock->setAdded(true);
	Sim::getUserDataBlockGroup()->addObject(pBlock);
}

void clearUserDataBlockGroup()
{
	SimGroup *grp = Sim::getUserDataBlockGroup();
	for(S32 i = grp->size() - 1; i >= 0; i--)
	{
		SimDataBlock *obj = (SimDataBlock*)(*grp)[i];
		obj->setAdded(false);
	}

	Sim::getUserDataBlockGroup()->clear(false);
}

void OnGFXInited()
{
	TSShapeInstance::boneAnimationInit();
}
