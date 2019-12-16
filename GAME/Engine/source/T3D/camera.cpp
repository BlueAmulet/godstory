//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "app/game.h"
#include "math/mMath.h"
#include "core/bitStream.h"
#include "T3D/camera.h"
#include "T3D/gameConnection.h"
#include "math/mathIO.h"
#include "gui/missionEditor/editor.h"
#include "console/consoleTypes.h"

#define MaxPitch 1.5706f
#define CameraRadius 0.05f;


//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(CameraData);

void CameraData::initPersistFields()
{
   Parent::initPersistFields();
}

void CameraData::packData(BitStream* stream)
{
   Parent::packData(stream);
}

void CameraData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
}


//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(Camera);
F32 Camera::mMovementSpeed = 40.0f;

Camera::Camera()
{
   mNetFlags.clear(Ghostable);
   mTypeMask |= CameraObjectType;
   delta.pos = Point3F(0.0f, 0.0f, 100.0f);
   delta.rot = Point3F(0.0f, 0.0f, 0.0f);
   delta.posVec = delta.rotVec = VectorF(0.0f, 0.0f, 0.0f);
   mObjToWorld.setColumn(3, delta.pos);
   mRot = delta.rot;

   mOffset.set(0.0f, 0.0f, 0.0f);

   mMinOrbitDist = 0.0f;
   mMaxOrbitDist = 0.0f;
   mCurOrbitDist = 0.0f;
   mOrbitObject = NULL;
   mPosition.set(0.0f, 0.0f, 0.0f);
   mObservingClientObject = false;
   mode = 2;

   mLocked = false;
}

Camera::~Camera()
{
}


//----------------------------------------------------------------------------

bool Camera::onAdd()
{
   if(!Parent::onAdd())
      return false;

   mObjBox.max = mObjScale;
   mObjBox.min = mObjScale;
   mObjBox.min.neg();
   resetWorldBox();

   if(isClientObject())
      gClientContainer.addObject(this);
   else
      gServerContainer.addObject(this);

 //  addToScene();
   return true;
}

void Camera::onEditorEnable()
{
   mNetFlags.set(Ghostable);
}

void Camera::onEditorDisable()
{
   mNetFlags.clear(Ghostable);
}

void Camera::onRemove()
{
//   removeFromScene();
   if (getContainer())
      getContainer()->removeObject(this);

   Parent::onRemove();
}


//----------------------------------------------------------------------------
// check if the object needs to be observed through its own camera...
void Camera::getCameraTransform(F32* pos, MatrixF* mat)
{
   // The camera doesn't support a third person mode,
   // so we want to override the default ShapeBase behavior.
   ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
   if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
      obj->getCameraTransform(pos, mat);
   else
      getRenderEyeTransform(mat);
}

F32 Camera::getCameraFov()
{
   ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
   if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
      return(obj->getCameraFov());
   else
      return(Parent::getCameraFov());
}

F32 Camera::getDefaultCameraFov()
{
   ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
   if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
      return(obj->getDefaultCameraFov());
   else
      return(Parent::getDefaultCameraFov());
}

bool Camera::isValidCameraFov(F32 fov)
{
   ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
   if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
      return(obj->isValidCameraFov(fov));
   else
      return(Parent::isValidCameraFov(fov));
}

void Camera::setCameraFov(F32 fov)
{
   ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
   if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
      obj->setCameraFov(fov);
   else
      Parent::setCameraFov(fov);
}

//----------------------------------------------------------------------------
void Camera::processTick(const Move* move)
{
   Parent::processTick(move);

   Point3F vec,pos;
   if (move) 
   {
      // If using editor then force camera into fly mode
      if(gEditingMission && mode != FlyMode)
         setFlyMode();

      // Update orientation
      delta.rotVec = mRot;
      mObjToWorld.getColumn(3,&delta.posVec);

      bool ignoreInput = (mode == OrbitObjectMode || mode == OrbitPointMode) && mLocked;

      if (!ignoreInput)
      {
         mRot.x += move->pitch;
         if(mRot.x > MaxPitch)
            mRot.x = MaxPitch;
         else if(mRot.x < -MaxPitch)
            mRot.x = -MaxPitch;

         mRot.z += move->yaw;
      }

      if(mode == OrbitObjectMode || mode == OrbitPointMode)
      {
         if(mode == OrbitObjectMode && bool(mOrbitObject)) 
         {
            // If this is a shapebase, use its render eye transform
            // to avoid jittering.
            GameBase *castObj = mOrbitObject;
            ShapeBase* shape = dynamic_cast<ShapeBase*>(castObj);
            if( shape != NULL ) 
            {
               MatrixF ret;
               shape->getRenderEyeTransform( &ret );
               mPosition = ret.getPosition();
            } 
            else 
            {
               // Hopefully this is a static object that doesn't move,
               // because the worldbox doesn't get updated between ticks.
               mOrbitObject->getWorldBox().getCenter(&mPosition);
            }
         }

         setPosition(mPosition + mOffset, mRot);
         validateEyePoint(1.0f, &mObjToWorld);
         pos = mPosition + mOffset;
      }
      else
      {
         // Update pos
         bool faster = move->trigger[0] || move->trigger[1];
         F32 scale = mMovementSpeed * (faster + 1);

         mObjToWorld.getColumn(3,&pos);
         mObjToWorld.getColumn(0,&vec);
         pos += vec * move->x * TickSec * scale;
         mObjToWorld.getColumn(1,&vec);
         pos += vec * move->y * TickSec * scale;
         mObjToWorld.getColumn(2,&vec);
         pos += vec * move->z * TickSec * scale;

		 if(isClientObject())
			setPosition(pos,mRot);
		 else
			setPosition(move->mPos, Point3F(0,0,move->mRotationZ));
      }

      // If on the client, calc delta for backstepping
      if (isClientObject()) 
      {
         delta.pos = pos;
         delta.rot = mRot;
         delta.posVec = delta.posVec - delta.pos;
         delta.rotVec = delta.rotVec - delta.rot;

		 Move* pm = (Move*)move;
		 pm->mPos = pos;
		 pm->mRotationZ = mRot.z;
      }
      setMaskBits(MoveMask);
   }

   if(getControllingClient() && mContainer)
      updateContainer();
}

void Camera::onDeleteNotify(SimObject *obj)
{
   Parent::onDeleteNotify(obj);
   if (obj == (SimObject*)mOrbitObject)
   {
      mOrbitObject = NULL;

      if(mode == OrbitObjectMode)
         mode = OrbitPointMode;
   }
}

void Camera::interpolateTick(F32 dt)
{
   Parent::interpolateTick(dt);
   Point3F rot = delta.rot + delta.rotVec * dt;

   if(mode == OrbitObjectMode || mode == OrbitPointMode)
   {
      if(mode == OrbitObjectMode && bool(mOrbitObject))
      {
         // If this is a shapebase, use its render eye transform
         // to avoid jittering.
         GameBase *castObj = mOrbitObject;
         ShapeBase* shape = dynamic_cast<ShapeBase*>(castObj);
         if( shape != NULL ) 
         {
            MatrixF ret;
            shape->getRenderEyeTransform( &ret );
            mPosition = ret.getPosition();
         } 
         else 
         {
            // Hopefully this is a static object that doesn't move,
            // because the worldbox doesn't get updated between ticks.
            mOrbitObject->getWorldBox().getCenter(&mPosition);
         }
      }
      setRenderPosition(mPosition + mOffset, rot);
      validateEyePoint(1.0f, &mRenderObjToWorld);
   }
   else 
   {
      Point3F pos = delta.pos + delta.posVec * dt;
      setRenderPosition(pos,rot);
   }
}

void Camera::setPosition(const Point3F& pos, const Point3F& rot)
{
   MatrixF xRot, zRot;
   xRot.set(EulerF(rot.x, 0.0f, 0.0f));
   zRot.set(EulerF(0.0f, 0.0f, rot.z));
   
   MatrixF temp;
   temp.mul(zRot, xRot);
   temp.setColumn(3, pos);
   Parent::setTransform(temp);
   mRot = rot;
}

void Camera::setRenderPosition(const Point3F& pos,const Point3F& rot)
{
   MatrixF xRot, zRot;
   xRot.set(EulerF(rot.x, 0, 0));
   zRot.set(EulerF(0, 0, rot.z));
   MatrixF temp;
   temp.mul(zRot, xRot);
   temp.setColumn(3, pos);
   Parent::setRenderTransform(temp);
}

//----------------------------------------------------------------------------

void Camera::writePacketData(GameConnection *connection, BitStream *bstream)
{
   // Update client regardless of status flags.
   Parent::writePacketData(connection, bstream);

   Point3F pos;
   mObjToWorld.getColumn(3,&pos);
   bstream->setCompressionPoint(pos);
   mathWrite(*bstream, pos);
   bstream->write(mRot.x);
   bstream->write(mRot.z);

   U32 writeMode = mode;
   Point3F writePos = mPosition;
   S32 gIndex = -1;
   if(mode == OrbitObjectMode)
   {
      gIndex = bool(mOrbitObject) ? connection->getGhostIndex(mOrbitObject): -1;
      if(gIndex == -1)
      {
         writeMode = OrbitPointMode;
         mOrbitObject->getWorldBox().getCenter(&writePos);
      }
   }
   bstream->writeRangedU32(writeMode, CameraFirstMode, CameraLastMode);

   if (writeMode == OrbitObjectMode || writeMode == OrbitPointMode)
   {
      bstream->write(mMinOrbitDist);
      bstream->write(mMaxOrbitDist);
      bstream->write(mCurOrbitDist);
      if(writeMode == OrbitObjectMode)
      {
         bstream->writeFlag(mObservingClientObject);
         bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);
      }
      if (writeMode == OrbitPointMode)
         bstream->writeCompressedPoint(writePos);
   }
}

void Camera::readPacketData(GameConnection *connection, BitStream *bstream)
{
   Parent::readPacketData(connection, bstream);
   Point3F pos,rot;
   mathRead(*bstream, &pos);
   bstream->setCompressionPoint(pos);
   bstream->read(&rot.x);
   bstream->read(&rot.z);

   GameBase* obj = 0;
   mode = bstream->readRangedU32(CameraFirstMode, CameraLastMode);
   mObservingClientObject = false;
   if (mode == OrbitObjectMode || mode == OrbitPointMode)
   {
      bstream->read(&mMinOrbitDist);
      bstream->read(&mMaxOrbitDist);
      bstream->read(&mCurOrbitDist);

      if(mode == OrbitObjectMode)
      {
         mObservingClientObject = bstream->readFlag();
         S32 gIndex = bstream->readInt(NetConnection::GhostIdBitSize);
         obj = static_cast<GameBase*>(connection->resolveGhost(gIndex));
      }
      if (mode == OrbitPointMode)
         bstream->readCompressedPoint(&mPosition);
   }

   if (obj != (GameBase*)mOrbitObject)
   {
      if (mOrbitObject)
      {
         clearProcessAfter();
         clearNotify(mOrbitObject);
      }

      mOrbitObject = obj;
      if (mOrbitObject)
      {
         processAfter(mOrbitObject);
         deleteNotify(mOrbitObject);
      }
   }

   setPosition(pos,rot);
   delta.pos = pos;
   delta.rot = rot;
   delta.rotVec.set(0.0f, 0.0f, 0.0f);
   delta.posVec.set(0.0f, 0.0f, 0.0f);
}

U64 Camera::packUpdate(NetConnection *con, U64 mask, BitStream *bstream)
{
   Parent::packUpdate(con, mask, bstream);

   if (bstream->writeFlag(mask & UpdateMask))
   {
      bstream->writeFlag(mLocked);
      mathWrite(*bstream, mOffset);
   }

   // The rest of the data is part of the control object packet update.
   // If we're controlled by this client, we don't need to send it.
   if(bstream->writeFlag(getControllingClient() == con && !(mask & InitialUpdateMask)))
      return 0;

   if (bstream->writeFlag(mask & MoveMask))
   {
      Point3F pos;
      mObjToWorld.getColumn(3,&pos);
      bstream->write(pos.x);
      bstream->write(pos.y);
      bstream->write(pos.z);
      bstream->write(mRot.x);
      bstream->write(mRot.z);
   }

   return 0;
}

void Camera::unpackUpdate(NetConnection *con, BitStream *bstream)
{
   Parent::unpackUpdate(con,bstream);

   if (bstream->readFlag())
   {
      mLocked = bstream->readFlag();
      mathRead(*bstream, &mOffset);
   }

   // controlled by the client?
   if(bstream->readFlag())
      return;

   if (bstream->readFlag())
   {
      Point3F pos,rot;
      bstream->read(&pos.x);
      bstream->read(&pos.y);
      bstream->read(&pos.z);
      bstream->read(&rot.x);
      bstream->read(&rot.z);
      setPosition(pos,rot);

      // New delta for client side interpolation
      delta.pos = pos;
      delta.rot = rot;
      delta.posVec = delta.rotVec = VectorF(0.0f, 0.0f, 0.0f);
   }
}


//----------------------------------------------------------------------------

void Camera::initPersistFields()
{
   Parent::initPersistFields();
}

void Camera::consoleInit()
{
   Con::addVariable("Camera::movementSpeed", TypeF32, &mMovementSpeed);
}

Point3F Camera::getPosition()
{
   static Point3F position;
   mObjToWorld.getColumn(3, &position);
   return position;
}

ConsoleMethod( Camera, getPosition, const char *, 2, 2, "()"
              "Get the position of the camera.\n\n"
              "@returns A string of form \"x y z\".")
{
   static char buffer[256];

   Point3F pos = object->getPosition();
   dSprintf(buffer, sizeof(buffer),"%g %g %g",pos.x,pos.y,pos.z);
   return buffer;
}

ConsoleMethod( Camera, getRotation, const char *, 2, 2, "()"
              "Get the euler rotation of the camera.\n\n"
              "@returns A string of form \"x y z\".")
{
   static char buffer[256];

   Point3F rot = object->getRotation();
   dSprintf(buffer, sizeof(buffer),"%g %g %g",rot.x,rot.y,rot.z);
   return buffer;
}

ConsoleMethod( Camera, getOffset, const char *, 2, 2, "()"
              "Get the offset for the camera.\n\n"
              "@returns A string of form \"x y z\".")
{
   static char buffer[256];

   Point3F offset = object->getOffset();
   dSprintf(buffer, sizeof(buffer),"%g %g %g",offset.x,offset.y,offset.z);
   return buffer;
}

ConsoleMethod( Camera, setOffset, void, 2, 2, "(Point3F offset)"
              "Set the offset for the camera.")
{
   Point3F offset(0.0f, 0.0f, 0.0f);

   dSscanf(argv[3],"%g %g %g",
      &offset.x,&offset.y,&offset.z);

   object->setOffset(offset);
}

ConsoleMethod( Camera, setOrbitMode, void, 7, 10, "(GameBase orbitObject, transform mat, float minDistance,"
              " float maxDistance, float curDistance, [bool ownClientObject = false], [Point3F offset], [bool locked = false])"
              "Set the camera to orbit around some given object.\n\n"
              "@param   orbitObject  Object we want to orbit.\n"
              "@param   mat          A set of fields: posX posY rotX rotY rotZ\n"
              "@param   minDistance  Minimum distance to keep from object.\n"
              "@param   maxDistance  Maximum distance to keep from object.\n"
              "@param   curDistance  Distance to set initially from object.\n"
              "@param   ownClientObj Are we observing an object owned by us?\n"
              "@param   offset       An offset to add to our position\n"
              "@param   locked       Camera doesn't receive inputs from player")
{
   Point3F pos;
   Point3F rot;
   Point3F offset(0.0f, 0.0f, 0.0f);
   F32 minDis, maxDis, curDis;
   bool locked = false;

   GameBase *orbitObject = NULL;

   // See if we have an orbit object
   if ((dStricmp(argv[2], "NULL") == 0) || (dStricmp(argv[2], "0") == 0))
      orbitObject = NULL;
   else
   {
      if(Sim::findObject(argv[2],orbitObject) == false)
      {
         Con::warnf("Cannot orbit non-existing object.");
         object->setFlyMode();
         return;
      }
   }

   dSscanf(argv[3],"%g %g %g %g %g %g",
      &pos.x,&pos.y,&pos.z,&rot.x,&rot.y,&rot.z);
   minDis = dAtof(argv[4]);
   maxDis = dAtof(argv[5]);
   curDis = dAtof(argv[6]);

   if (argc == 9)
      dSscanf(argv[8],"%g %g %g",
         &offset.x,&offset.y,&offset.z);

   if (argc == 10)
      locked = dAtob(argv[9]);

   object->setOrbitMode(orbitObject, pos, rot, offset, minDis, maxDis, curDis, (argc == 8) ? dAtob(argv[7]) : false, locked);
}

ConsoleMethod( Camera, setFlyMode, void, 2, 2, "()"
              "Set the camera to be able to fly freely.")
{
   object->setFlyMode();
}

//----------------------------------------------------------------------------

void Camera::renderImage(SceneState*)
{
   if(gEditingMission)
   {
/*
      glPushMatrix();
      dglMultMatrix(&mObjToWorld);
      glScalef(mObjScale.x,mObjScale.y,mObjScale.z);
      wireCube(Point3F(1, 1, 1),Point3F(0,0,0));
      glPopMatrix();
*/
   }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//    NEW Observer Code
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Camera::setFlyMode()
{
   mode = FlyMode;

   if (bool(mOrbitObject))
   {
      clearProcessAfter();
      clearNotify(mOrbitObject);
   }
   mOrbitObject = NULL;
}

void Camera::setOrbitMode(GameBase *obj, Point3F &pos, Point3F &rot, Point3F& offset, F32 minDist, F32 maxDist, F32 curDist, bool ownClientObject, bool locked)
{
   mObservingClientObject = ownClientObject;

   rot;
   if (bool(mOrbitObject))
   {
      clearProcessAfter();
      clearNotify(mOrbitObject);
   }

   mOrbitObject = obj;
   if(bool(mOrbitObject))
   {
      processAfter(mOrbitObject);
      deleteNotify(mOrbitObject);
      mOrbitObject->getWorldBox().getCenter(&mPosition);
      mode = OrbitObjectMode;
   }
   else
   {
      mode = OrbitPointMode;
      mPosition = pos;
   }

   setPosition(mPosition, rot);

   

   mMinOrbitDist = minDist;
   mMaxOrbitDist = maxDist;
   mCurOrbitDist = curDist;

   if (locked != mLocked || mOffset != offset)
   {
      mLocked = locked;
      mOffset = offset;
      setMaskBits(UpdateMask);
   }
}


void Camera::validateEyePoint(F32 pos, MatrixF *mat)
{
   if (pos != 0) 
   {
      // Use the eye transform to orient the camera
      Point3F dir;
      mat->getColumn(1, &dir);
      pos *= mMaxOrbitDist - mMinOrbitDist;
      
      // Use the camera node's pos.
      Point3F startPos = getRenderPosition();
      Point3F endPos;

      // Make sure we don't extend the camera into anything solid
      if(mOrbitObject)
         mOrbitObject->disableCollision();
      disableCollision();
      RayInfo collision;
      U32 mask = AtlasObjectType |
                 TerrainObjectType |
                 InteriorObjectType |
                 WaterObjectType |
                 StaticShapeObjectType |
                 GameObjectType |
                 ItemObjectType |
                 VehicleObjectType;

      Container* pContainer = isServerObject() ? &gServerContainer : &gClientContainer;
      if (!pContainer->castRay(startPos, startPos - dir * 2.5 * pos, mask, &collision, this->getLayerID()))
         endPos = startPos - dir * pos;
      else
      {
         float dot = mDot(dir, collision.normal);
         if (dot > 0.01f)
         {
            float colDist = mDot(startPos - collision.point, dir) - (1 / dot) * CameraRadius;
            if (colDist > pos)
               colDist = pos;
            if (colDist < 0.0f)
               colDist = 0.0f;
            endPos = startPos - dir * colDist;
         }
         else
            endPos = startPos - dir * pos;
      }
      mat->setColumn(3, endPos);
      enableCollision();
      if(mOrbitObject)
         mOrbitObject->enableCollision();
   }
}

void Camera::setPosition(const Point3F& pos, const Point3F& rot, MatrixF *mat)
{
   MatrixF xRot, zRot;
   xRot.set(EulerF(rot.x, 0.0f, 0.0f));
   zRot.set(EulerF(0.0f, 0.0f, rot.z));
   mat->mul(zRot, xRot);
   mat->setColumn(3,pos);
   mRot = rot;
}

void Camera::setTransform(const MatrixF& mat)
{
   // This method should never be called on the client.

   // This currently converts all rotation in the mat into
   // rotations around the z and x axis.
   Point3F pos,vec;
   mat.getColumn(1, &vec);
   mat.getColumn(3, &pos);
   Point3F rot(-mAtan(vec.z, mSqrt(vec.x * vec.x + vec.y * vec.y)), 0.0f, -mAtan(-vec.x, vec.y));
   setPosition(pos,rot);
}

void Camera::setRenderTransform(const MatrixF& mat)
{
   // This method should never be called on the client.

   // This currently converts all rotation in the mat into
   // rotations around the z and x axis.
   Point3F pos,vec;
   mat.getColumn(1,&vec);
   mat.getColumn(3,&pos);
   Point3F rot(-mAtan(vec.z, mSqrt(vec.x*vec.x + vec.y*vec.y)),0,-mAtan(-vec.x,vec.y));
   setRenderPosition(pos,rot);
}

