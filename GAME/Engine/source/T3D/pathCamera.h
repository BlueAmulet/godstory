//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PATHCAMERA_H_
#define _PATHCAMERA_H_

#ifndef _SHAPEBASE_H_
#include "T3D/shapeBase.h"
#endif

#ifndef _CAMERASPLINE_H_
#include "T3D/cameraSpline.h"
#endif


//----------------------------------------------------------------------------
struct PathCameraData: public ShapeBaseData {
   typedef ShapeBaseData Parent;

   //
   DECLARE_CONOBJECT(PathCameraData);
   static void consoleInit();
   static void initPersistFields();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};


//----------------------------------------------------------------------------
class PathCamera: public ShapeBase
{
public:
   enum State {
      Forward,
      Backward,
      Stop,
      StateBits = 3
   };

private:
   typedef ShapeBase Parent;

   enum MaskBits {
      WindowMask     = Parent::NextFreeMask,
      PositionMask   = Parent::NextFreeMask + 1,
      TargetMask     = Parent::NextFreeMask + 2,
      StateMask      = Parent::NextFreeMask + 3,
      NextFreeMask   = Parent::NextFreeMask << 1
   };

   struct StateDelta {
      F32 time;
      F32 timeVec;
   };
   StateDelta delta;

   enum Constants {
      NodeWindow = 128    // Maximum number of active nodes
   };

   //
   PathCameraData* mDataBlock;
   CameraSpline mSpline;
   S32 mNodeBase;
   S32 mNodeCount;
   F32 mPosition;
   int mState;
   F32 mTarget;
   bool mTargetSet;

   void interpolateMat(F32 pos,MatrixF* mat);
   void advancePosition(S32 ms);

public:
   DECLARE_CONOBJECT(PathCamera);

   PathCamera();
   ~PathCamera();
   static void initPersistFields();
   static void consoleInit();

   void onEditorEnable();
   void onEditorDisable();

   bool onAdd();
   void onRemove();
   bool onNewDataBlock(GameBaseData* dptr);
   void onNode(S32 node);

   void processTick(const Move*);
   void interpolateTick(F32 dt);
   void getCameraTransform(F32* pos,MatrixF* mat);
   void renderImage(SceneState* state);

   U64  packUpdate(NetConnection *, U64 mask, BitStream *stream);
   void unpackUpdate(NetConnection *, BitStream *stream);

   void reset(F32 speed = 1);
   void pushFront(CameraSpline::Knot *knot);
   void pushBack(CameraSpline::Knot *knot);
   void popFront();

   void setPosition(F32 pos);
   void setTarget(F32 pos);
   void setState(State s);
};


#endif
