//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/fx/lightning.h"

#include "sceneGraph/sceneState.h"
#include "console/consoleTypes.h"
#include "math/mathIO.h"
#include "core/bitStream.h"
#include "T3D/gameConnection.h"
#include "T3D/shapeBase.h"
#include "math/mRandom.h"
#include "math/mathUtils.h"
//#include "platform/platformAudio.h"
#include "terrain/terrData.h"
#include "sceneGraph/sceneGraph.h"
#pragma message(ENGINE(原Player类改成GameObject类))
#include "Gameplay/GameObjects/PlayerObject.h"
#include "T3D/camera.h"
#include "sfx/sfxSystem.h"
#include "gfx/primBuilder.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* Lightning::mSetSB = NULL;
GFXStateBlock* Lightning::mClearSB = NULL;

IMPLEMENT_CO_DATABLOCK_V1(LightningData);
IMPLEMENT_CO_NETOBJECT_V1(Lightning);

MRandomLCG sgLightningRand;

ConsoleMethod( Lightning, warningFlashes, void, 2, 2, "")
{
   if (object->isServerObject()) object->warningFlashes();
}

ConsoleMethod( Lightning, strikeRandomPoint, void, 2, 2, "")
{
   if (object->isServerObject()) object->strikeRandomPoint();
}

ConsoleMethod( Lightning, strikeObject, void, 3, 3, "(ShapeBase id)")
{
   S32 id = dAtoi(argv[2]);
   ShapeBase* pSB;

   if (object->isServerObject() && Sim::findObject(id, pSB))
      object->strikeObject(pSB);
}

S32 QSORT_CALLBACK cmpSounds(const void* p1, const void* p2)
{
   U32 i1 = *((const S32*)p1);
   U32 i2 = *((const S32*)p2);

   if (i1 < i2) {
      return 1;
   } else if (i1 > i2) {
      return -1;
   } else {
      return 0;
   }
}

//--------------------------------------------------------------------------
//--------------------------------------
//
class LightningStrikeEvent : public NetEvent
{
   typedef NetEvent Parent;

  public:
   enum EventType {
      WarningFlash   = 0,
      Strike         = 1,
      TargetedStrike = 2,

      TypeMin        = WarningFlash,
      TypeMax        = TargetedStrike
   };
   enum Constants {
      PositionalBits = 10
   };

   Point2F                   mStart;
   SimObjectPtr<SceneObject> mTarget;

   Lightning*                mLightning;

   // Set by unpack...
  public:
   S32                       mClientId;

  public:
   LightningStrikeEvent();
   ~LightningStrikeEvent();

   void pack(NetConnection*, BitStream*);
   void write(NetConnection*, BitStream*){}
   void unpack(NetConnection*, BitStream*);
   void process(NetConnection*);

   DECLARE_CONOBJECT(LightningStrikeEvent);
};
IMPLEMENT_CO_CLIENTEVENT_V1(LightningStrikeEvent);

LightningStrikeEvent::LightningStrikeEvent()
{
   mLightning = NULL;
   mTarget = NULL;
}

LightningStrikeEvent::~LightningStrikeEvent()
{

}

void LightningStrikeEvent::pack(NetConnection* con, BitStream* stream)
{
   if(!mLightning)
   {
      stream->writeFlag(false);
      return;
   }
   S32 id = con->getGhostIndex(mLightning);
   if(id == -1)
   {
      stream->writeFlag(false);
      return;
   }
   stream->writeFlag(true);
   stream->writeRangedU32(U32(id), 0, NetConnection::MaxGhostCount);
   stream->writeFloat(mStart.x, PositionalBits);
   stream->writeFloat(mStart.y, PositionalBits);

   if( mTarget )
   {
      S32 ghostIndex = con->getGhostIndex(mTarget);
      if (ghostIndex == -1)
         stream->writeFlag(false);
      else
      {
         stream->writeFlag(true);
         stream->writeRangedU32(U32(ghostIndex), 0, NetConnection::MaxGhostCount);
      }
   }
   else
      stream->writeFlag( false );
}

void LightningStrikeEvent::unpack(NetConnection* con, BitStream* stream)
{
   if(!stream->readFlag())
      return;
   S32 mClientId = stream->readRangedU32(0, NetConnection::MaxGhostCount);
   mLightning = NULL;
   NetObject* pObject = con->resolveGhost(mClientId);
   if (pObject)
      mLightning = dynamic_cast<Lightning*>(pObject);

   mStart.x = stream->readFloat(PositionalBits);
   mStart.y = stream->readFloat(PositionalBits);

   if( stream->readFlag() )
   {
      // target id
      S32 mTargetID    = stream->readRangedU32(0, NetConnection::MaxGhostCount);

      NetObject* pObject = con->resolveGhost(mTargetID);
      if( pObject != NULL )
      {
         mTarget = dynamic_cast<SceneObject*>(pObject);
      }
      if( bool(mTarget) == false )
      {
         Con::errorf(ConsoleLogEntry::General, "LightningStrikeEvent::unpack: could not resolve target ghost properly");
      }

   }

}

void LightningStrikeEvent::process(NetConnection*)
{
   if (mLightning)
      mLightning->processEvent(this);
}


//--------------------------------------------------------------------------
//--------------------------------------
//
LightningData::LightningData()
{
   strikeSound = NULL;
   strikeSoundID = -1;

   dMemset( strikeTextureNames, 0, sizeof( strikeTextureNames ) );
   dMemset( strikeTextures, 0, sizeof( strikeTextures ) );

   U32 i;
   for (i = 0; i < MaxThunders; i++) {
      thunderSounds[i]   = NULL;
      thunderSoundIds[i] = -1;
   }
}

LightningData::~LightningData()
{

}


//--------------------------------------------------------------------------
void LightningData::initPersistFields()
{
   Parent::initPersistFields();

   addField("strikeSound",     TypeSFXProfilePtr, Offset(strikeSound,        LightningData));
   addField("thunderSounds",   TypeSFXProfilePtr, Offset(thunderSounds,      LightningData), MaxThunders);
   addField("strikeTextures",  TypeString,          Offset(strikeTextureNames, LightningData), MaxTextures);
}


//--------------------------------------------------------------------------
bool LightningData::onAdd()
{
   if(!Parent::onAdd())
      return false;

   for (U32 i = 0; i < MaxThunders; i++) {
      if (!thunderSounds[i] && thunderSoundIds[i] != -1) {
         if (Sim::findObject(thunderSoundIds[i], thunderSounds[i]) == false)
            Con::errorf(ConsoleLogEntry::General, "LightningData::onAdd: Invalid packet, bad datablockId(sound: %d", thunderSounds[i]);
      }
   }

   if( !strikeSound && strikeSoundID != -1 )
   {
      if( Sim::findObject( strikeSoundID, strikeSound ) == false)
         Con::errorf(ConsoleLogEntry::General, "LightningData::onAdd: Invalid packet, bad datablockId(sound: %d", strikeSound);
   }

   return true;
}


bool LightningData::preload(bool server, char errorBuffer[256])
{
   if (Parent::preload(server, errorBuffer) == false)
      return false;

   dQsort(thunderSounds, MaxThunders, sizeof(SFXProfile*), cmpSounds);
   for (numThunders = 0; numThunders < MaxThunders && thunderSounds[numThunders] != NULL; numThunders++) {
      //
   }

   if (server == false) 
   {
      for (U32 i = 0; i < MaxTextures; i++) 
      {
         if (strikeTextureNames[i][0])
            strikeTextures[i] = GFXTexHandle(strikeTextureNames[i], &GFXDefaultStaticDiffuseProfile);
      }
   }


   return true;
}


//--------------------------------------------------------------------------
void LightningData::packData(BitStream* stream)
{
   Parent::packData(stream);

   U32 i;
   for (i = 0; i < MaxThunders; i++) {
      if (stream->writeFlag(thunderSounds[i] != NULL)) {
         stream->writeRangedU32(thunderSounds[i]->getId(), DataBlockObjectIdFirst,
                                                           DataBlockObjectIdLast);
      }
   }
   for (i = 0; i < MaxTextures; i++) {
      stream->writeString(strikeTextureNames[i]);
   }

   if( stream->writeFlag( strikeSound != NULL) )
   {
      stream->writeRangedU32( strikeSound->getId(), DataBlockObjectIdFirst,
                                                    DataBlockObjectIdLast);
   }
}

void LightningData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   U32 i;
   for (i = 0; i < MaxThunders; i++) {
      if (stream->readFlag())
         thunderSoundIds[i] = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
      else
         thunderSoundIds[i] = -1;
   }
   for (i = 0; i < MaxTextures; i++) {
      strikeTextureNames[i] = stream->readSTString();
   }

   if (stream->readFlag())
      strikeSoundID = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   else
      strikeSoundID = -1;
}


//--------------------------------------------------------------------------
//--------------------------------------
//
Lightning::Lightning()
{
   mNetFlags.set(Ghostable|ScopeAlways);
   mTypeMask |= StaticObjectType|EnvironmentObjectType;

   mLastThink = 0;

   mStrikeListHead  = NULL;
   mThunderListHead = NULL;

   strikesPerMinute = 12;
   strikeWidth = 2.5;
   chanceToHitTarget = 0.5f;
   strikeRadius = 20.0f;
   boltStartRadius = 20.0f;
   color.set( 1.0f, 1.0f, 1.0f, 1.0f );
   fadeColor.set( 0.1f, 0.1f, 1.0f, 1.0f );
   useFog = true;

   setScale( VectorF( 512.0f, 512.0f, 300.0f ) );
}

Lightning::~Lightning()
{
   //
}

//--------------------------------------------------------------------------
void Lightning::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Strikes");	// MM: Added Group Header.
   addField("strikesPerMinute",TypeS32,             Offset(strikesPerMinute,   Lightning));
   addField("strikeWidth",     TypeF32,             Offset(strikeWidth,        Lightning));
   addField("strikeRadius",    TypeF32,             Offset(strikeRadius,       Lightning));
   endGroup("Strikes");	// MM: Added Group Footer.

   addGroup("Colors");	// MM: Added Group Header.
   addField("color",           TypeColorF,          Offset(color,              Lightning));
   addField("fadeColor",       TypeColorF,          Offset(fadeColor,          Lightning));
   endGroup("Colors");	// MM: Added Group Footer.

   addGroup("Bolts");	// MM: Added Group Header.
   addField("chanceToHitTarget", TypeF32,           Offset(chanceToHitTarget,  Lightning));
   addField("boltStartRadius", TypeF32,             Offset(boltStartRadius,    Lightning));
   addField("useFog",          TypeBool,            Offset(useFog,             Lightning));
   endGroup("Bolts");	// MM: Added Group Footer.
}

//--------------------------------------------------------------------------
bool Lightning::onAdd()
{
   if(!Parent::onAdd())
      return false;

   mObjBox.min.set( -0.5f, -0.5f, -0.5f );
   mObjBox.max.set(  0.5f,  0.5f,  0.5f );

   resetWorldBox();
   addToScene();

   return true;
}


void Lightning::onRemove()
{
   removeFromScene();

   Parent::onRemove();
}


bool Lightning::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<LightningData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;

   scriptOnNewDataBlock();
   return true;
}


//--------------------------------------------------------------------------
bool Lightning::prepRenderImage(SceneState* state, const U32 stateKey,
                                const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if (state->isObjectRendered(this))
   {
      RenderInst *ri = gRenderInstManager.allocInst();
      ri->obj = this;
      ri->state = state;
      // The Lightning isn't technically foliage but our debug
      // effect seems to render best as a Foliage type (translucent,
      // renders itself, no sorting)
      ri->type = RenderInstManager::RIT_Foliage;
      gRenderInstManager.addInst( ri );
   }

   return false;
}


void Lightning::renderObject(SceneState* state, RenderInst *)
{
   state->setupObjectProjection(this);

#ifdef STATEBLOCK
	AssertFatal(mSetSB, "Lightning::renderObject -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
   // Blend ops
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendOne);
	GFX->setCullMode(GFXCullNone);

   GFX->setZWriteEnable(false);

   // Texture parameters
   GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
#endif


   Strike* walk = mStrikeListHead;
   while (walk != NULL)
   {
      GFX->setTexture(0, mDataBlock->strikeTextures[0]);

      for( U32 i=0; i<3; i++ )
      {
         if( walk->bolt[i].isFading )
         {
            F32 alpha = 1.0f - walk->bolt[i].percentFade;
            if( alpha < 0.0f ) alpha = 0.0f;
            PrimBuild::color4f( fadeColor.red, fadeColor.green, fadeColor.blue, alpha );
         }
         else
         {
            PrimBuild::color4f( color.red, color.green, color.blue, color.alpha );
         }
         walk->bolt[i].render( state->getCameraPosition() );
      }

      walk = walk->next;
   }
#ifdef STATEBLOCK
	AssertFatal(mClearSB, "Lightning::renderObject -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
	GFX->setZWriteEnable(true);
	GFX->setAlphaTestEnable(false);
	GFX->setAlphaBlendEnable(false);
#endif

}

void Lightning::scheduleThunder(Strike* newStrike)
{
   AssertFatal(isClientObject(), "Lightning::scheduleThunder: server objects should not enter this version of the function");

   // If no thunder sounds, don't schedule anything!
   if (mDataBlock->numThunders == 0)
      return;

   GameConnection* connection = GameConnection::getConnectionToServer();
   if (connection) {
      MatrixF cameraMatrix;

      if (connection->getControlCameraTransform(0, &cameraMatrix)) {
         Point3F worldPos;
         cameraMatrix.getColumn(3, &worldPos);

         worldPos.x -= newStrike->xVal;
         worldPos.y -= newStrike->yVal;
         worldPos.z  = 0.0f;

         F32 dist = worldPos.len();
         F32 t    = dist / 330.0f;

         // Ok, we need to schedule a random strike sound t secs in the future...
         //
         if (t <= 0.03f) {
            // If it's really close, just play it...
            U32 thunder = sgLightningRand.randI(0, mDataBlock->numThunders - 1);
//            SFX->playOnce(mDataBlock->thunderSounds[thunder]);
         } else {
            Thunder* pThunder = new Thunder;
            pThunder->tRemaining = t;
            pThunder->next       = mThunderListHead;
            mThunderListHead     = pThunder;
         }
      }
   }
}


//--------------------------------------------------------------------------
void Lightning::processTick(const Move* move)
{
   Parent::processTick(move);

   if (isServerObject()) {
      S32 msBetweenStrikes = (S32)(60.0 / strikesPerMinute * 1000.0);

      mLastThink += TickMs;
      if( mLastThink > msBetweenStrikes )
      {
         strikeRandomPoint();
         mLastThink -= msBetweenStrikes;
      }
   }
}

void Lightning::interpolateTick(F32 dt)
{
   Parent::interpolateTick(dt);
}

void Lightning::advanceTime(F32 dt)
{
   Parent::advanceTime(dt);

   Strike** pWalker = &mStrikeListHead;
   while (*pWalker != NULL) {
      Strike* pStrike = *pWalker;

      for( U32 i=0; i<3; i++ )
      {
         pStrike->bolt[i].update( dt );
      }

      pStrike->currentAge += dt;
      if (pStrike->currentAge > pStrike->deathAge) {
         *pWalker = pStrike->next;
         delete pStrike;
      } else {
         pWalker = &((*pWalker)->next);
      }
   }

   Thunder** pThunderWalker = &mThunderListHead;
   while (*pThunderWalker != NULL) {
      Thunder* pThunder = *pThunderWalker;

      pThunder->tRemaining -= dt;
      if (pThunder->tRemaining <= 0.0f) {
         *pThunderWalker = pThunder->next;
         delete pThunder;

         // Play the sound...
         U32 thunder = sgLightningRand.randI(0, mDataBlock->numThunders - 1);
//         SFX->playOnce(mDataBlock->thunderSounds[thunder]);
      } else {
         pThunderWalker = &((*pThunderWalker)->next);
      }
   }
}


//--------------------------------------------------------------------------
void Lightning::processEvent(LightningStrikeEvent* pEvent)
{
      AssertFatal(pEvent->mStart.x >= 0.0f && pEvent->mStart.x <= 1.0f, "Out of bounds coord!");

      Strike* pStrike = new Strike;

      Point3F strikePoint;
      strikePoint.zero();

      if( pEvent->mTarget )
      {
         Point3F objectCenter;
         pEvent->mTarget->getObjBox().getCenter( &objectCenter );
         objectCenter.convolve( pEvent->mTarget->getScale() );
         pEvent->mTarget->getTransform().mulP( objectCenter );

         strikePoint = objectCenter;
      }
      else
      {
         strikePoint.x = pEvent->mStart.x;
         strikePoint.y = pEvent->mStart.y;
         strikePoint *= mObjScale;
         strikePoint += getPosition();
         strikePoint += Point3F( -mObjScale.x * 0.5f, -mObjScale.y * 0.5f, 0.0f );

         RayInfo rayInfo;
         Point3F start = strikePoint;
         start.z = mObjScale.z * 0.5f + getPosition().z;
         strikePoint.z += -mObjScale.z * 0.5f;
         bool rayHit = gClientContainer.castRay( start, strikePoint,
                                      (STATIC_COLLISION_MASK | WaterObjectType),
                                      &rayInfo);
         if( rayHit )
         {
            strikePoint.z = rayInfo.point.z;
         }
         else
         {
            strikePoint.z = pStrike->bolt[0].findHeight( strikePoint, mSceneManager );
         }
      }

      pStrike->xVal       = strikePoint.x;
      pStrike->yVal       = strikePoint.y;

      pStrike->deathAge   = 1.6f;
      pStrike->currentAge = 0.0f;
      pStrike->next       = mStrikeListHead;

      for( U32 i=0; i<3; i++ )
      {
         F32 randStart = boltStartRadius;
         F32 height = mObjScale.z * 0.5f + getPosition().z;
         pStrike->bolt[i].startPoint.set( pStrike->xVal + gRandGen.randF( -randStart, randStart ), pStrike->yVal + gRandGen.randF( -randStart, randStart ), height );
         pStrike->bolt[i].endPoint = strikePoint;
         pStrike->bolt[i].width = strikeWidth;
         pStrike->bolt[i].numMajorNodes = 10;
         pStrike->bolt[i].maxMajorAngle = 30.0f;
         pStrike->bolt[i].numMinorNodes = 4;
         pStrike->bolt[i].maxMinorAngle = 15.0f;
         pStrike->bolt[i].generate();
         pStrike->bolt[i].startSplits();
         pStrike->bolt[i].lifetime = 1.0f;
         pStrike->bolt[i].fadeTime = 0.2f;
         pStrike->bolt[i].renderTime = gRandGen.randF(0.0f, 0.25f);
      }

      mStrikeListHead     = pStrike;

      scheduleThunder(pStrike);

      MatrixF trans(true);
      trans.setPosition( strikePoint );

      if (mDataBlock->strikeSound)
      {
//         SFX->playOnce(mDataBlock->strikeSound, &trans );
      }

}

void Lightning::warningFlashes()
{
   AssertFatal(isServerObject(), "Error, client objects may not initiate lightning!");


   SimGroup* pClientGroup = Sim::getClientGroup();
   for (SimGroup::iterator itr = pClientGroup->begin(); itr != pClientGroup->end(); itr++) {
      NetConnection* nc = static_cast<NetConnection*>(*itr);
      if (nc != NULL)
      {
         LightningStrikeEvent* pEvent = new LightningStrikeEvent;
         pEvent->mLightning = this;

         nc->postNetEvent(pEvent);
      }
   }
}

void Lightning::strikeRandomPoint()
{
   AssertFatal(isServerObject(), "Error, client objects may not initiate lightning!");


   Point3F strikePoint( gRandGen.randF( 0.0f, 1.0f ), gRandGen.randF( 0.0f, 1.0f ), 0.0f );

   // check if an object is within target range

   strikePoint *= mObjScale;
   strikePoint += getPosition();
   strikePoint += Point3F( -mObjScale.x * 0.5f, -mObjScale.y * 0.5f, 0.0f );

   Box3F queryBox;
   F32 boxWidth = strikeRadius * 2.0f;

   queryBox.min.set( -boxWidth * 0.5f, -boxWidth * 0.5f, -mObjScale.z * 0.5f );
   queryBox.max.set(  boxWidth * 0.5f,  boxWidth * 0.5f,  mObjScale.z * 0.5f );
   queryBox.min += strikePoint;
   queryBox.max += strikePoint;

   SimpleQueryList sql;
   getContainer()->findObjects(queryBox, DAMAGEABLE_MASK,
                               SimpleQueryList::insertionCallback, &sql);

   SceneObject *highestObj = NULL;
   F32 highestPnt = 0.0f;

   for( U32 i = 0; i < sql.mList.size(); i++ )
   {
      Point3F objectCenter;
      sql.mList[i]->getObjBox().getCenter(&objectCenter);
      objectCenter.convolve(sql.mList[i]->getScale());
      sql.mList[i]->getTransform().mulP(objectCenter);

      // check if object can be struck

      RayInfo rayInfo;
      Point3F start = objectCenter;
      start.z = mObjScale.z * 0.5f + getPosition().z;
      Point3F end = objectCenter;
      end.z = -mObjScale.z * 0.5f + getPosition().z;
      bool rayHit = gServerContainer.castRay( start, end,
                                   (0xFFFFFFFF),
                                   &rayInfo);

      if( rayHit && rayInfo.object == sql.mList[i] )
      {
         if( !highestObj )
         {
            highestObj = sql.mList[i];
            highestPnt = objectCenter.z;
            continue;
         }

         if( objectCenter.z > highestPnt )
         {
            highestObj = sql.mList[i];
            highestPnt = objectCenter.z;
         }
      }


   }

   // hah haaaaa, we have a target!
   SceneObject *targetObj = NULL;
   if( highestObj )
   {
      F32 chance = gRandGen.randF();
      if( chance <= chanceToHitTarget )
      {
         Point3F objectCenter;
         highestObj->getObjBox().getCenter(&objectCenter);
         objectCenter.convolve(highestObj->getScale());
         highestObj->getTransform().mulP(objectCenter);

         bool playerInWarmup = false;
         Player *playerObj = dynamic_cast< Player * >(highestObj);
         if( playerObj )
         {
            if( !playerObj->getControllingClient() )
            {
               playerInWarmup = true;
            }
         }

         if( !playerInWarmup )
         {
            applyDamage( objectCenter, VectorF( 0.0f, 0.0f, 1.0f ), highestObj );
            targetObj = highestObj;
         }
      }
   }

   SimGroup* pClientGroup = Sim::getClientGroup();
   for (SimGroup::iterator itr = pClientGroup->begin(); itr != pClientGroup->end(); itr++)
   {
      NetConnection* nc = static_cast<NetConnection*>(*itr);

      LightningStrikeEvent* pEvent = new LightningStrikeEvent;
      pEvent->mLightning = this;

      pEvent->mStart.x = strikePoint.x;
      pEvent->mStart.y = strikePoint.y;
      pEvent->mTarget = targetObj;

      nc->postNetEvent(pEvent);
   }


}

//--------------------------------------------------------------------------
void Lightning::strikeObject(ShapeBase*)
{
   AssertFatal(isServerObject(), "Error, client objects may not initiate lightning!");

   AssertFatal(false, "Lightning::strikeObject is not implemented.");
}


//--------------------------------------------------------------------------
U64 Lightning::packUpdate(NetConnection* con, U64 mask, BitStream* stream)
{
   U64 retMask = Parent::packUpdate(con, mask, stream);

   // Only write data if this is the initial packet or we've been inspected.
   if (stream->writeFlag(mask & (InitialUpdateMask | ExtendedInfoMask)))
   {
      // Initial update
      mathWrite(*stream, getPosition());
      mathWrite(*stream, mObjScale);

      stream->write(strikeWidth);
      stream->write(chanceToHitTarget);
      stream->write(strikeRadius);
      stream->write(boltStartRadius);
      stream->write(color.red);
      stream->write(color.green);
      stream->write(color.blue);
      stream->write(fadeColor.red);
      stream->write(fadeColor.green);
      stream->write(fadeColor.blue);
      stream->write(useFog);
      stream->write(strikesPerMinute);
   }

   return retMask;
}

//--------------------------------------------------------------------------
void Lightning::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   if (stream->readFlag())
   {
      // Initial update
      Point3F pos;
      mathRead(*stream, &pos);
      setPosition( pos );

      mathRead(*stream, &mObjScale);

      stream->read(&strikeWidth);
      stream->read(&chanceToHitTarget);
      stream->read(&strikeRadius);
      stream->read(&boltStartRadius);
      stream->read(&color.red);
      stream->read(&color.green);
      stream->read(&color.blue);
      stream->read(&fadeColor.red);
      stream->read(&fadeColor.green);
      stream->read(&fadeColor.blue);
      stream->read(&useFog);
      stream->read(&strikesPerMinute);
   }
}

//--------------------------------------------------------------------------
void Lightning::applyDamage( const Point3F& hitPosition,
                             const Point3F& hitNormal,
                             SceneObject*   hitObject)
{
   if (!isClientObject() && hitObject != NULL)
   {
      char *posArg = Con::getArgBuffer(64);
      char *normalArg = Con::getArgBuffer(64);

      dSprintf(posArg, 64, "%f %f %f", hitPosition.x, hitPosition.y, hitPosition.z);
      dSprintf(normalArg, 64, "%f %f %f", hitNormal.x, hitNormal.y, hitNormal.z);

      Con::executef(mDataBlock, "applyDamage",
         Con::getIntArg(getId()),
         Con::getIntArg(hitObject->getId()),
         posArg,
         normalArg);
   }
}


void Lightning::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->endStateBlock(mSetSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->endStateBlock(mClearSB);
}


void Lightning::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void Lightning::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void Lightning::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mClearSB);
}
//**************************************************************************
// Lightning Bolt
//**************************************************************************
LightningBolt::LightningBolt()
{
   width = 0.1f;
   startPoint.zero();
   endPoint.zero();
   chanceOfSplit = 0.0f;
   isFading = false;
   elapsedTime = 0.0f;
   lifetime = 1.0f;
   startRender = false;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
LightningBolt::~LightningBolt()
{
   splitList.free();
}

//--------------------------------------------------------------------------
// Generate nodes
//--------------------------------------------------------------------------
void LightningBolt::NodeManager::generateNodes()
{
   F32 overallDist = VectorF( endPoint - startPoint ).magnitudeSafe();
   F32 minDistBetweenNodes = overallDist / (numNodes-1);
   F32 maxDistBetweenNodes = minDistBetweenNodes / mCos( maxAngle * M_PI_F / 180.0f );

   VectorF mainLineDir = endPoint - startPoint;
   mainLineDir.normalizeSafe();

   for( U32 i=0; i<numNodes; i++ )
   {
      Node node;

      if( i == 0 )
      {
         node.point = startPoint;
         node.dirToMainLine = mainLineDir;
         nodeList[i] = node;
         continue;
      }
      if( i == numNodes - 1 )
      {
         node.point = endPoint;
         nodeList[i] = node;
         break;
      }

      Node lastNode = nodeList[i-1];

      F32 segmentLength = gRandGen.randF( minDistBetweenNodes, maxDistBetweenNodes );
      VectorF segmentDir = MathUtils::randomDir( lastNode.dirToMainLine, 0, maxAngle );
      node.point = lastNode.point + segmentDir * segmentLength;

      node.dirToMainLine = endPoint - node.point;
      node.dirToMainLine.normalizeSafe();
      nodeList[i] = node;
   }
}


//--------------------------------------------------------------------------
// Render bolt
//--------------------------------------------------------------------------
void LightningBolt::render( const Point3F &camPos )
{
   if (!startRender)
      return;

   if (!isFading)
      generateMinorNodes();

   U32 maxVerts = 0;
   for (U32 i = 0; i < mMinorNodes.size(); i++)
      maxVerts += mMinorNodes[i].numNodes * 2;

   PrimBuild::begin(GFXTriangleStrip, maxVerts);

   for (U32 i = 0; i < mMinorNodes.size(); i++)
   {
      if (i+1 == mMinorNodes.size())
         renderSegment(mMinorNodes[i], camPos, true);
      else
         renderSegment(mMinorNodes[i], camPos, false);
   }

	PrimBuild::end();

   LightningBolt *curBolt = NULL;
   for( curBolt = splitList.next( curBolt ); curBolt; curBolt = splitList.next( curBolt ) )
   {
      if( isFading )
      {
         curBolt->isFading = true;
      }
      curBolt->render( camPos );
   }


}

//--------------------------------------------------------------------------
// Render segment
//--------------------------------------------------------------------------
void LightningBolt::renderSegment( NodeManager &segment, const Point3F &camPos, bool renderLastPoint )
{

   for (U32 i = 0; i < segment.numNodes; i++)
   {
      Point3F  curPoint = segment.nodeList[i].point;

      Point3F  nextPoint;
      Point3F  segDir;

      if( i == (segment.numNodes-1) )
      {
         if( renderLastPoint )
         {
            segDir = curPoint - segment.nodeList[i-1].point;
         }
         else
         {
            continue;
         }
      }
      else
      {
         nextPoint = segment.nodeList[i+1].point;
         segDir = nextPoint - curPoint;
      }
      segDir.normalizeSafe();


      Point3F dirFromCam = curPoint - camPos;
      Point3F crossVec;
      mCross(dirFromCam, segDir, &crossVec);
      crossVec.normalize();
      crossVec *= width * 0.5f;

      F32 u = i % 2;

      PrimBuild::texCoord2f( u, 1.0 );
      PrimBuild::vertex3fv( curPoint - crossVec );

      PrimBuild::texCoord2f( u, 0.0 );
      PrimBuild::vertex3fv( curPoint + crossVec );
   }

}

//----------------------------------------------------------------------------
// Find height
//----------------------------------------------------------------------------
F32 LightningBolt::findHeight( Point3F &point, SceneGraph *sceneManager )
{
   TerrainBlock* pTerrain = sceneManager->getCurrentTerrain();
   if( !pTerrain )
      return 0.0f;

   Point3F terrPt = point;
   pTerrain->getWorldTransform().mulP(terrPt);
   F32 h;
   if (pTerrain->getHeight(Point2F(terrPt.x, terrPt.y), &h))
   {
      return h;
   }


   return 0.0f;
}


//----------------------------------------------------------------------------
// Generate lightning bolt
//----------------------------------------------------------------------------
void LightningBolt::generate()
{
   mMajorNodes.startPoint   = startPoint;
   mMajorNodes.endPoint     = endPoint;
   mMajorNodes.numNodes     = numMajorNodes;
   mMajorNodes.maxAngle     = maxMajorAngle;

   mMajorNodes.generateNodes();

   generateMinorNodes();

}

//----------------------------------------------------------------------------
// Generate Minor Nodes
//----------------------------------------------------------------------------
void LightningBolt::generateMinorNodes()
{
   mMinorNodes.clear();

   for( int i=0; i<mMajorNodes.numNodes - 1; i++ )
   {
      NodeManager segment;
      segment.startPoint = mMajorNodes.nodeList[i].point;
      segment.endPoint = mMajorNodes.nodeList[i+1].point;
      segment.numNodes = numMinorNodes;
      segment.maxAngle = maxMinorAngle;
      segment.generateNodes();

      mMinorNodes.increment(1);
      mMinorNodes[i] = segment;
   }
}

//----------------------------------------------------------------------------
// Recursive algo to create bolts that split off from main bolt
//----------------------------------------------------------------------------
void LightningBolt::createSplit( const Point3F &startPoint, const Point3F &endPoint, U32 depth, F32 width )
{
   if( depth == 0 )
      return;
	  
   F32 chanceToEnd = gRandGen.randF();
   if( chanceToEnd > 0.70f )
      return;

   if( width < 0.75f )
      width = 0.75f;

   VectorF diff = endPoint - startPoint;
   F32 length = diff.len();
   diff.normalizeSafe();

   LightningBolt newBolt;
   newBolt.startPoint = startPoint;
   newBolt.endPoint = endPoint;
   newBolt.width = width;
   newBolt.numMajorNodes = 3;
   newBolt.maxMajorAngle = 30.0f;
   newBolt.numMinorNodes = 3;
   newBolt.maxMinorAngle = 10.0f;
   newBolt.startRender = true;
   newBolt.generate();

   splitList.link( newBolt );

   VectorF newDir1 = MathUtils::randomDir( diff, 10.0f, 45.0f );
   Point3F newEndPoint1 = endPoint + newDir1 * gRandGen.randF( 0.5f, 1.5f ) * length;

   VectorF newDir2 = MathUtils::randomDir( diff, 10.0f, 45.0f );
   Point3F newEndPoint2 = endPoint + newDir2 * gRandGen.randF( 0.5f, 1.5f ) * length;

   createSplit( endPoint, newEndPoint1, depth - 1, width * 0.30f );
   createSplit( endPoint, newEndPoint2, depth - 1, width * 0.30f );

}

//----------------------------------------------------------------------------
// Start split - kick off the recursive 'createSplit' procedure
//----------------------------------------------------------------------------
void LightningBolt::startSplits()
{

   for( U32 i=0; i<mMajorNodes.numNodes-1; i++ )
   {
      if( gRandGen.randF() > 0.3f )
	     continue;

      Node node = mMajorNodes.nodeList[i];
      Node node2 = mMajorNodes.nodeList[i+1];

      VectorF segDir = node2.point - node.point;
      F32 length = segDir.len();
      segDir.normalizeSafe();

      VectorF newDir = MathUtils::randomDir( segDir, 20.0f, 40.0f );
      Point3F newEndPoint = node.point + newDir * gRandGen.randF( 0.5f, 1.5f ) * length;


      createSplit( node.point, newEndPoint, 4, width * 0.30f );
   }


}

//----------------------------------------------------------------------------
// Update
//----------------------------------------------------------------------------
void LightningBolt::update( F32 dt )
{
   elapsedTime += dt;

   F32 percentDone = elapsedTime / lifetime;

   if( elapsedTime > fadeTime )
   {
      isFading = true;
      percentFade = percentDone + (fadeTime/lifetime);
   }

   if( elapsedTime > renderTime && !startRender )
   {
      startRender = true;
      isFading = false;
      elapsedTime = 0.0f;
   }
}

