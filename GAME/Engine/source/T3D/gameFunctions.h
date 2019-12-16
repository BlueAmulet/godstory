//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GAMEFUNCTIONS_H_
#define _GAMEFUNCTIONS_H_

#include "platform/platform.h"
#include "math/mPoint.h"
#include "math/mMatrix.h"
#include "math/mRect.h"

struct CameraQuery;
class SceneObject;
class SimDataBlock;


/// Actually renders the world.  This is the function that will render the
/// scene ONLY - new guis, no damage flashes.
void GameRenderWorld();

//预处理渲染相关的事务
void updateMouseGameplayUI(RectI &);

/// Renders overlays such as damage flashes, white outs, and water masks.  
/// These are usually a color applied over the entire screen.
void GameRenderFilters(const CameraQuery& camq);

/// Does the same thing as GameGetCameraTransform, but fills in other data 
/// including information about the far and near clipping planes.
bool GameProcessCameraQuery(CameraQuery *query);

/// Gets the position, rotation, and velocity of the camera.
bool GameGetCameraTransform(MatrixF *mat, Point3F *velocity);

/// Gets the camera field of view angle.
F32 GameGetCameraFov();

/// Sets the field of view angle of the camera.
void GameSetCameraFov(F32 fov);

/// Sets where the camera fov will be change to.  This is for 
/// non-instantaneous zooms/retractions.
void GameSetCameraTargetFov(F32 fov);

/// Update the camera fov to be closer to the target fov.
void GameUpdateCameraFov();

void onWindowZombify();

//extern GFXStateBlock* g_gameSetSB;
//extern GFXStateBlock* g_gameClearSB;

void GameInitsb();
void GameShutdownsb();
//
//设备丢失时调用
//
void GameReleaseStateBlock();

//
//设备重置时调用
//
void GameResetStateBlock();


bool addToMissionGroup(SceneObject *pObj);
void addToUserDataBlockGroup(SimDataBlock *pBlock);
void clearUserDataBlockGroup();

void OnGFXInited();

#endif
