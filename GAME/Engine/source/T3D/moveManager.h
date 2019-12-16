//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _MOVEMANAGER_H_
#define _MOVEMANAGER_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#include "math/mPoint.h"

enum MoveConstants {
   MaxTriggerKeys = 6,
   MaxMoveQueueSize = 45,
};

class BitStream;

struct Move
{
   enum { ChecksumBits = 16, ChecksumMask = ((1<<ChecksumBits)-1), ChecksumMismatch = U32(-1) };

   F32 x, y, z;          // float -1 to 1
   F32 yaw, pitch, roll; // 0-2PI
   U32 id;               // sync'd between server & client - debugging tool.
   U32 sendCount;
   U32 checksum;

   bool deviceIsKeyboardMouse;
   bool freeLook;
   bool trigger[MaxTriggerKeys];

   #pragma message(ENGINE(初步修改了玩家移动及其校验))
   //玩家移动的数据包
   S32		mJumpDelay;
   bool		isMounted;
   Point3F	mPos;
   VectorF	mVelocity;
   U32		mJumpSurfaceLastContact;
   F32		mRotationZ;
   bool		jumping;
   bool		fall;
   bool		blink;
   U32		flyPathId;
   U32		flyPos;
   bool		flyAtEnd;

   //玩家数据验证
   F32		mCheck_1;
   static bool mUseCompressPoint;
   static const Move *mCompressPoint;
   static U32 LastChecksum;

   Move()
   {
	   memset(this,0,sizeof(Move));
   }

   void pack(BitStream *stream, const Move * move = NULL);
   void unpack(BitStream *stream, const Move * move = NULL);
};

extern const Move NullMove;

class MoveManager
{
public:
   static bool mDeviceIsKeyboardMouse;
   static F32 mForwardAction;
   static F32 mBackwardAction;
   static F32 mUpAction;
   static F32 mDownAction;
   static F32 mLeftAction;
   static F32 mRightAction;

   static bool mFreeLook;
   static F32 mPitch;
   static F32 mYaw;
   static F32 mRoll;

   static F32 mPitchUpSpeed;
   static F32 mPitchDownSpeed;
   static F32 mYawLeftSpeed;
   static F32 mYawRightSpeed;
   static F32 mRollLeftSpeed;
   static F32 mRollRightSpeed;
   static F32 mXAxis_L;
   static F32 mYAxis_L;
   static F32 mXAxis_R;
   static F32 mYAxis_R;

   static U32 mTriggerCount[MaxTriggerKeys];
   static U32 mPrevTriggerCount[MaxTriggerKeys];

   static void init();
};

#endif
