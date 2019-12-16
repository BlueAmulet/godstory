//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GAMECONNECTION_H_
#define _GAMECONNECTION_H_

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif
#ifndef _GAMEBASE_H_
#include "T3D/gameBase.h"
#endif
#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif
#ifndef _MOVEMANAGER_H_
#include "T3D/moveManager.h"
#endif
#ifndef _BITVECTOR_H_
#include "core/bitVector.h"
#endif
#ifndef _MOVELIST_H_
#include "T3D/moveList.h"
#endif

enum GameConnectionConstants
{
   MaxClients = 126,
   DataBlockQueueCount = 16
};

#include <hash_set>

class SFXProfile;
class MatrixF;
class MatrixF;
class Point3F;
class MoveManager;
struct Move;
struct AuthInfo;

#define GameString POWER_APP_NAME

const F32 MinCameraFov              = 1.f;      ///< min camera FOV
const F32 MaxCameraFov              = 179.f;    ///< max camera FOV

class GameConnection : public NetConnection
{
private:
   typedef NetConnection Parent;

   bool m_NeedInit;		//Ray: 是否需要检测初始化发送
   PacketNotify *m_InitNote;    //Ray: 在这个被确认的包之后发送初始化确保客户端存在对象

   SimObjectPtr<GameBase> mControlObject;
   SimObjectPtr<GameBase> mCameraObject;
   U32 mDataBlockSequence;
   char mDisconnectReason[256];

   U32  mMissionCRC;             // crc of the current mission file from the server
	U32 mLastTime;

	U32 mLayerId;
private:
   U32 mLastControlRequestTime;
   S32 mDataBlockModifiedKey;
   S32 mMaxDataBlockModifiedKey;
   U32 mControlStateSkipCount;

   /// @name Client side first/third person
   /// @{

   ///
   bool  mFirstPerson;     ///< Are we currently first person or not.
   bool  mUpdateFirstPerson; ///< Set to notify client or server of first person change.
   bool  mUpdateCameraFov; ///< Set to notify server of camera FOV change.
   F32   mCameraFov;       ///< Current camera fov (in degrees).
   F32   mCameraPos;       ///< Current camera pos (0-1).
   F32   mCameraSpeed;     ///< Camera in/out speed.
   /// @}

public:

   /// @name Protocol Versions
   ///
   /// Protocol versions are used to indicated changes in network traffic.
   /// These could be changes in how any object transmits or processes
   /// network information. You can specify backwards compatibility by
   /// specifying a MinRequireProtocolVersion.  If the client
   /// protocol is >= this min value, the connection is accepted.
   ///
   /// PowerEngine (V12) SDK 1.0 uses protocol  =  1
   ///
   /// PowerEngine SDK 1.1 uses protocol = 2
   /// PowerEngine SDK 1.4 uses protocol = 12
   /// @{
   static const U32 CurrentProtocolVersion;
   static const U32 MinRequiredProtocolVersion;
   /// @}
   static stdext::hash_set<GameConnection*>	gameConnectionSet;
   /// Configuration
   enum Constants {
      BlockTypeMove = NetConnectionBlockTypeCount,
      GameConnectionBlockTypeCount,
      MaxConnectArgs = 16,
      DataBlocksDone = NumConnectionMessages,
      DataBlocksDownloadDone,
	  ControlStateSkipAmount = 16,
   };

   /// Set connection arguments; these are passed to the server when we connect.
   void setConnectArgs(U32 argc, const char **argv);

   /// Set the server password to use when we join.
   void setJoinPassword(const char *password);

   /// @name Event Handling
   /// @{

   virtual void onTimedOut();
   virtual void onConnectTimedOut();
   virtual void onDisconnect(const char *reason);
   virtual void onConnectionRejected(const char *reason);
   virtual void onConnectionEstablished(bool isInitiator);
   virtual void handleStartupError(const char *errorString);
   /// @}

   /// @name Packet I/O
   /// @{

   virtual void writeConnectRequest(BitStream *stream);
   virtual bool readConnectRequest(BitStream *stream, const char **errorString);
   virtual void writeConnectAccept(BitStream *stream);
   virtual bool readConnectAccept(BitStream *stream, const char **errorString);
   /// @}

   bool canRemoteCreate();
   U32 GetLayerId();
   void SetLayerId( U32 nLayerId );

private:
   /// @name Connection State
   /// This data is set with setConnectArgs() and setJoinPassword(), and
   /// sent across the wire when we connect.
   /// @{

   U32      mConnectArgc;
   char *mConnectArgv[MaxConnectArgs];
   char *mJoinPassword;
   /// @}

protected:
   struct GamePacketNotify : public NetConnection::PacketNotify
   {
      S32 cameraFov;
      GamePacketNotify();
   };
   PacketNotify *allocNotify();

   bool mControlForceMismatch;

   Vector<SimDataBlock *> mDataBlockLoadList;

public:
   MoveList    mMoveList;

   #pragma message(ENGINE(初步修改了玩家移动及其校验))
   // 跟踪抛弃的包
   int			m_nIngorePack;
   SimTime		m_TrackTime;
   bool			m_bNoMove;
   void			SetNoMove() { m_bNoMove = true; m_TrackTime = Sim::getCurrentTime() + 2000; }
protected:
   bool        mAIControlled;
   AuthInfo *  mAuthInfo;

   static S32  mLagThresholdMS;
   S32         mLastPacketTime;
   bool        mLagging;

   /// @name Flashing
   ////
   /// Note, these variables are not networked, they are for the local connection only.
   /// @{
   F32 mDamageFlash;
   F32 mWhiteOut;

   F32   mBlackOut;
   S32   mBlackOutTimeMS;
   S32   mBlackOutStartTimeMS;
   bool  mFadeToBlack;

   /// @}

   /// @name Packet I/O
   /// @{

   void readPacket      (BitStream *bstream);
   void writePacket     (BitStream *bstream, PacketNotify *note);
   void packetReceived  (PacketNotify *note);
   void packetDropped   (PacketNotify *note);
   void connectionError (const char *errorString);

   void writeDemoStartBlock   (ResizeBitStream *stream);
   bool readDemoStartBlock    (BitStream *stream);
   void handleRecordedBlock   (U32 type, U32 size, void *data);
   /// @}
   void ghostWriteExtra(NetObject *,BitStream *);
   void ghostReadExtra(NetObject *,BitStream *, bool newGhost);
   void ghostPreRead(NetObject *, bool newGhost);
   
public:

   DECLARE_CONOBJECT(GameConnection);
   void handleConnectionMessage(U32 message, U32 sequence, U32 ghostCount);
   void preloadDataBlock(SimDataBlock *block);
   void fileDownloadSegmentComplete();
   void preloadNextDataBlock(bool hadNew);
   static void consoleInit();

   void forceMismatch(){mControlForceMismatch = true;}
   void setDisconnectReason(const char *reason);
   GameConnection();
   ~GameConnection();

   void setResCrc(Vector<U32>& terrCRC,U32 misCRC,StringTableEntry misFile);

   U32 getDataBlockSequence() { return mDataBlockSequence; }
   void setDataBlockSequence(U32 seq) { mDataBlockSequence = seq; }

   bool onAdd();
   void onRemove();

   static GameConnection *getConnectionToServer() 
   { 
      return dynamic_cast<GameConnection*>((NetConnection *) mServerConnection); 
   }
   
   static GameConnection *getLocalClientConnection() 
   { 
      return dynamic_cast<GameConnection*>((NetConnection *) mLocalClientConnection); 
   }

   /// @name Control object
   /// @{

   ///
   void setControlObject(GameBase *);
   GameBase* getControlObject() {  return  mControlObject; }
   
   void setCameraObject(GameBase *);
   GameBase* getCameraObject();
   
   bool getControlCameraTransform(F32 dt,MatrixF* mat);
   bool getControlCameraVelocity(Point3F *vel);

   bool getControlCameraFov(F32 *fov);
   bool setControlCameraFov(F32 fov);
   bool isValidControlCameraFov(F32 fov);
   
   void setFirstPerson(bool firstPerson);
   
   /// @}

   void detectLag();

   /// @name Datablock management
   /// @{

   S32  getDataBlockModifiedKey     ()  { return mDataBlockModifiedKey; }
   void setDataBlockModifiedKey     (S32 key)  { mDataBlockModifiedKey = key; }
   S32  getMaxDataBlockModifiedKey  ()  { return mMaxDataBlockModifiedKey; }
   void setMaxDataBlockModifiedKey  (S32 key)  { mMaxDataBlockModifiedKey = key; }
   /// @}

   /// @name Fade control
   /// @{

   F32 getDamageFlash() { return mDamageFlash; }
   F32 getWhiteOut() { return mWhiteOut; }

   void setBlackOut(bool fadeToBlack, S32 timeMS);
   F32  getBlackOut();
   /// @}

   /// @name Authentication
   ///
   /// This is remnant code from Tribes 2.
   /// @{

   void            setAuthInfo(const AuthInfo *info);
   const AuthInfo *getAuthInfo();
   /// @}

   /// @name Sound
   /// @{

   void play2D(const SFXProfile *profile);
   void play3D(const SFXProfile *profile, const MatrixF *transform);
   /// @}

   /// @name Misc.
   /// @{

   bool isFirstPerson()  { return mCameraPos == 0; }
   bool isAIControlled() { return mAIControlled; }

   void doneScopingScene();
   void demoPlaybackComplete();

   void setMissionCRC(U32 crc)           { mMissionCRC = crc; }
   U32  getMissionCRC()           { return(mMissionCRC); }
   /// @}

   static Signal<F32> smFovUpdate;
   static Signal<> smPlayingDemo;
};

#endif
