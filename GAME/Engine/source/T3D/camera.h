//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CAMERA_H_
#define _CAMERA_H_

#ifndef _SHAPEBASE_H_
#include "T3D/shapeBase.h"
#endif

//----------------------------------------------------------------------------
struct CameraData: public ShapeBaseData {
   typedef ShapeBaseData Parent;

   //
   DECLARE_CONOBJECT(CameraData);
   static void initPersistFields();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};


//----------------------------------------------------------------------------
/// Implements a basic camera object.
class Camera: public ShapeBase
{
   typedef ShapeBase Parent;

   enum MaskBits {
      MoveMask          = Parent::NextFreeMask,
      UpdateMask        = MoveMask << 1,
      NextFreeMask      = Parent::NextFreeMask << 1
   };

   struct StateDelta {
      Point3F pos;
      Point3F rot;
      VectorF posVec;
      VectorF rotVec;
   };
   Point3F mRot;
   StateDelta delta;

   Point3F mOffset;

   static F32 mMovementSpeed;

   void setPosition(const Point3F& pos,const Point3F& viewRot);
   void setRenderPosition(const Point3F& pos,const Point3F& viewRot);

   SimObjectPtr<GameBase> mOrbitObject;
   F32 mMinOrbitDist;
   F32 mMaxOrbitDist;
   F32 mCurOrbitDist;
   Point3F mPosition;
   bool mObservingClientObject;

   bool mLocked;

   enum
   {
      StationaryMode  = 0,

      FreeRotateMode  = 1,
      FlyMode         = 2,
      OrbitObjectMode = 3,
      OrbitPointMode  = 4,

      CameraFirstMode = 0,
      CameraLastMode  = 4
   };

   int mode;
   void setPosition(const Point3F& pos,const Point3F& viewRot, MatrixF *mat);
   void setTransform(const MatrixF& mat);
   void setRenderTransform(const MatrixF& mat);
   F32 getCameraFov();
   F32 getDefaultCameraFov();
   bool isValidCameraFov(F32 fov);
   void setCameraFov(F32 fov);

public:
   DECLARE_CONOBJECT(Camera);

   Camera();
   ~Camera();
   static void initPersistFields();
   static void consoleInit();

   void onEditorEnable();
   void onEditorDisable();

   bool onAdd();
   void onRemove();
   void renderImage(SceneState* state);
   void processTick(const Move* move);
   void interpolateTick(F32 delta);
   void getCameraTransform(F32* pos,MatrixF* mat);

   void writePacketData(GameConnection *conn, BitStream *stream);
   void readPacketData(GameConnection *conn, BitStream *stream);
   U64 packUpdate(NetConnection *conn, U64 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn, BitStream *stream);
   Point3F getPosition();
   Point3F getRotation() { return mRot; };
   Point3F getOffset() { return mOffset; };
   void setOffset(Point3F offset) { mOffset = offset; };
   void setFlyMode();
   void setOrbitMode(GameBase *obj, Point3F &pos, Point3F &rot, Point3F& offset,
                     F32 minDist, F32 maxDist, F32 curDist, bool ownClientObject, bool locked = false);
   void validateEyePoint(F32 pos, MatrixF *mat);
   void onDeleteNotify(SimObject *obj);

   GameBase * getOrbitObject()      { return(mOrbitObject); }
   bool isObservingClientObject()   { return(mObservingClientObject); }
};


#endif
