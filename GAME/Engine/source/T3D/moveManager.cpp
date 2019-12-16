//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "core/bitStream.h"
#include "console/consoleTypes.h"
#include "math/mConstants.h"
#include "T3D/moveManager.h"
#pragma message(ENGINE(原Player类改成GameObject类))
#include "Gameplay/GameObjects/PlayerObject.h"

U32 Move::LastChecksum = 0;
bool Move::mUseCompressPoint = false;
const Move *Move::mCompressPoint = NULL;

bool MoveManager::mDeviceIsKeyboardMouse = false;
F32 MoveManager::mForwardAction = 0;
F32 MoveManager::mBackwardAction = 0;
F32 MoveManager::mUpAction = 0;
F32 MoveManager::mDownAction = 0;
F32 MoveManager::mLeftAction = 0;
F32 MoveManager::mRightAction = 0;

#ifdef NTJ_EDITOR
bool MoveManager::mFreeLook = false;
#else
bool MoveManager::mFreeLook = true;
#endif
F32 MoveManager::mPitch = 0;
F32 MoveManager::mYaw = 0;
F32 MoveManager::mRoll = 0;

F32 MoveManager::mPitchUpSpeed = 0;
F32 MoveManager::mPitchDownSpeed = 0;
F32 MoveManager::mYawLeftSpeed = 0;
F32 MoveManager::mYawRightSpeed = 0;
F32 MoveManager::mRollLeftSpeed = 0;
F32 MoveManager::mRollRightSpeed = 0;

F32 MoveManager::mXAxis_L = 0;
F32 MoveManager::mYAxis_L = 0;
F32 MoveManager::mXAxis_R = 0;
F32 MoveManager::mYAxis_R = 0;

U32 MoveManager::mTriggerCount[MaxTriggerKeys] = { 0, };
U32 MoveManager::mPrevTriggerCount[MaxTriggerKeys] = { 0, };

/*
const Move NullMove =
{
   16,16,16,
   0,0,0,
   0,0,0,   // x,y,z
   0,0,0,   // Yaw, pitch, roll,
   0,0,

   false,
   false,false,false,false,false,false
};
*/
#pragma message(ENGINE(初步修改了玩家移动及其校验))
const Move NullMove;

void MoveManager::init()
{
   Con::addVariable("mvForwardAction", TypeF32, &mForwardAction);
   Con::addVariable("mvBackwardAction", TypeF32, &mBackwardAction);
   Con::addVariable("mvUpAction", TypeF32, &mUpAction);
   Con::addVariable("mvDownAction", TypeF32, &mDownAction);
   Con::addVariable("mvLeftAction", TypeF32, &mLeftAction);
   Con::addVariable("mvRightAction", TypeF32, &mRightAction);

   Con::addVariable("mvFreeLook", TypeBool, &mFreeLook);
   Con::addVariable("mvDeviceIsKeyboardMouse", TypeBool, &mDeviceIsKeyboardMouse);
   Con::addVariable("mvPitch", TypeF32, &mPitch);
   Con::addVariable("mvYaw", TypeF32, &mYaw);
   Con::addVariable("mvRoll", TypeF32, &mRoll);
   Con::addVariable("mvPitchUpSpeed", TypeF32, &mPitchUpSpeed);
   Con::addVariable("mvPitchDownSpeed", TypeF32, &mPitchDownSpeed);
   Con::addVariable("mvYawLeftSpeed", TypeF32, &mYawLeftSpeed);
   Con::addVariable("mvYawRightSpeed", TypeF32, &mYawRightSpeed);
   Con::addVariable("mvRollLeftSpeed", TypeF32, &mRollLeftSpeed);
   Con::addVariable("mvRollRightSpeed", TypeF32, &mRollRightSpeed);

   // Dual-analog
   Con::addVariable( "mvXAxis_L", TypeF32, &mXAxis_L );
   Con::addVariable( "mvYAxis_L", TypeF32, &mYAxis_L );

   Con::addVariable( "mvXAxis_R", TypeF32, &mXAxis_R );
   Con::addVariable( "mvYAxis_R", TypeF32, &mYAxis_R );

   for(U32 i = 0; i < MaxTriggerKeys; i++)
   {
      char varName[256];
      dSprintf(varName, sizeof(varName), "mvTriggerCount%d", i);
      Con::addVariable(varName, TypeS32, &mTriggerCount[i]);
   }
}

static inline F32 clampFloatWrap(F32 val)
{
   return val - F32(S32(val));
}

static inline S32 clampRangeClamp(F32 val)
{
   if(val < -1)
      return 0;
   if(val > 1)
      return 32;
   return (S32)((val + 1) * 16);
}


void Move::pack(BitStream *stream, const Move * basemove)
{
   bool alwaysWriteAll = basemove!=NULL;
   if (!basemove)
      basemove = &NullMove;
   mCompressPoint = basemove;

   S32 i;
   bool triggerDifferent = false;
   for (i=0; i < MaxTriggerKeys; i++)
      if (trigger[i] != basemove->trigger[i])
         triggerDifferent = true;
   bool somethingDifferent = (yaw!=basemove->yaw)           ||
                             (pitch!=basemove->pitch)       ||
                             (roll!=basemove->roll)         ||
                             (deviceIsKeyboardMouse!=basemove->deviceIsKeyboardMouse) ||
                             (freeLook!=basemove->freeLook) ||
							 (mPos.x != basemove->mPos.x)	||
							 (mPos.y != basemove->mPos.y)	||
							 (mPos.z != basemove->mPos.z)	||
							 flyPathId						||
                             triggerDifferent;
   
   if (alwaysWriteAll || stream->writeFlag(somethingDifferent))
   {
      stream->writeFlag(freeLook);
      stream->writeFlag(deviceIsKeyboardMouse);

      if (stream->writeFlag(triggerDifferent))
         for(i = 0; i < MaxTriggerKeys; i++)
      stream->writeFlag(trigger[i]);

      //更新移动数据包
	  if(stream->writeFlag(flyPathId))
	  {
		  stream->writeInt(flyPathId, Base::Bit32);
		  stream->writeInt(flyPos, 24);
		  stream->writeFlag(flyAtEnd);
		  return;
	  }
      if (stream->writeFlag((bool)mJumpDelay))
         stream->writeInt(mJumpDelay,PlayerData::JumpDelayBits);
	  stream->writeSignedInt(x,2);
	  stream->writeSignedInt(y,2);
	  stream->writeSignedInt(z,2);

	  bool useZero = true;
	  Point3F savePoint = stream->getCompressionPoint();

	  if(mUseCompressPoint)
	  {
		  if(mCompressPoint && mCompressPoint != &NullMove && !mCompressPoint->flyPathId)
			  useZero = false;
	  }
	  	  
	  stream->writeFlag((bool)useZero);
      stream->clearCompressionPoint();
	  stream->writeCompressedPoint(mVelocity, 0.001f);

	  if(!useZero)
		  stream->setCompressionPoint(mCompressPoint->mPos);
	  stream->writeCompressedPoint(mPos, 0.001f);

	  stream->setCompressionPoint(savePoint);

      stream->writeFlag(isMounted);
      stream->writeInt(mJumpSurfaceLastContact > 15 ? 15 : mJumpSurfaceLastContact, 4);

	  stream->writeFloat(mRotationZ/M_2PI_F, 10);
	  if(stream->writeFlag(jumping))
		  stream->writeFlag(fall);
	  stream->writeFlag(blink);

      //验证数据
      //stream->write(mCheck_1);
   }
}

void Move::unpack(BitStream *stream, const Move * basemove)
{
   bool alwaysReadAll = basemove!=NULL;
   if (!basemove)
      basemove=&NullMove;
   mCompressPoint = basemove;

   if (alwaysReadAll || stream->readFlag())
   {
      freeLook = stream->readFlag();
      deviceIsKeyboardMouse = stream->readFlag();

      bool triggersDiffer = stream->readFlag();
      for (S32 i = 0; i< MaxTriggerKeys; i++)
         trigger[i] = triggersDiffer ? stream->readFlag() : basemove->trigger[i];

      //更新移动数据包
	  if(stream->readFlag())
	  {
		  flyPathId = stream->readInt(Base::Bit32);
		  flyPos = stream->readInt(24);
		  flyAtEnd = stream->readFlag();
		  return;
	  }
	  else
	  {
		  flyPathId = 0;
		  flyPos = 0;
		  flyAtEnd = false;
	  }
      if (stream->readFlag())
         mJumpDelay = stream->readInt(PlayerData::JumpDelayBits);
      else
         mJumpDelay = 0;
	  x = stream->readSignedInt(2);
	  y = stream->readSignedInt(2);
	  z = stream->readSignedInt(2);

	  //mUseCompressPoint = stream->readFlag();
	  bool useZero = stream->readFlag();
	  Point3F savePoint = stream->getCompressionPoint();
      stream->clearCompressionPoint();
	  stream->readCompressedPoint(&mVelocity, 0.001f);

	  if(!useZero)
		  stream->setCompressionPoint(mCompressPoint->mPos);
	  stream->readCompressedPoint(&mPos, 0.001f);
      stream->setCompressionPoint(savePoint);

	  isMounted = stream->readFlag();
	  mJumpSurfaceLastContact = stream->readInt(4);

	  mRotationZ = stream->readFloat(10) * M_2PI_F;
	  jumping = stream->readFlag();
	  if(jumping)
		  fall = stream->readFlag();
	  blink = stream->readFlag();

      mCheck_1 = mVelocity.len();
   }
   else
      *this = *basemove;
}

ConsoleFunction(moveleft, void, 2, 2, "moveleft(%val);")
{
	MoveManager::mLeftAction = atof(argv[1]);
}

ConsoleFunction(moveright, void, 2, 2, "moveright(%val);")
{
	MoveManager::mRightAction = atof(argv[1]);
}

ConsoleFunction(moveforward, void, 2, 2, "moveforward(%val);")
{
	MoveManager::mForwardAction = atof(argv[1]);
}

ConsoleFunction(movebackward, void, 2, 2, "movebackward(%val);")
{
	MoveManager::mBackwardAction = atof(argv[1]);
}
