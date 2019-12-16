//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "T3D/gameBase.h"
#include "console/consoleTypes.h"
#include "console/consoleInternal.h"
#include "core/bitStream.h"
#include "sim/netConnection.h"
#include "T3D/gameConnection.h"
#include "math/mathIO.h"
#include "T3D/moveManager.h"
#include "T3D/gameProcess.h"

#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/GameObjects/MountObjectData.h"
#include "Gameplay/GameObjects/NpcObjectData.h"
#include "Gameplay/GameObjects/PetObjectData.h"
#include "Gameplay/GameObjects/PlayerData.h"
#include "T3D/trigger.h"
#include "effects/EffectPacket.h"

#ifdef POWER_DEBUG_NET_MOVES
#include "T3D/aiConnection.h"
#endif

//----------------------------------------------------------------------------
// Ghost update relative priority values

static F32 sUpFov       = 1.0;
static F32 sUpDistance  = 0.4f;
static F32 sUpVelocity  = 0.4f;
static F32 sUpSkips     = 0.2f;
static F32 sUpOwnership = 0.2f;
static F32 sUpInterest  = 0.2f;

//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(GameBaseData);

GameBaseData::GameBaseData()
{
   category = "";
   packed = false;
   dataBlockId = 0;

   mNSLinkMask = LinkSuperClassName | LinkClassName;
}

bool GameBaseData::onAdd()
{
   if (!Parent::onAdd())
      return false;

   return true;
}

void GameBaseData::initPersistFields()
{
   Parent::initPersistFields();
   addField("category",   TypeCaseString,          Offset(category,   GameBaseData));
}

bool GameBaseData::preload(bool server, char errorBuffer[256])
{
   if (!Parent::preload(server, errorBuffer))
      return false;
   packed = false;
   return true;
}

void GameBaseData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   packed = true;
}


//----------------------------------------------------------------------------
bool UNPACK_DB_ID(BitStream * stream, U32 & id)
{
   if (stream->readFlag())
   {
      id = stream->readRangedU32(DataBlockObjectIdFirst,DataBlockObjectIdLast);
      return true;
   }
   return false;
}

bool PACK_DB_ID(BitStream * stream, U32 id)
{
   if (stream->writeFlag(id))
   {
      stream->writeRangedU32(id,DataBlockObjectIdFirst,DataBlockObjectIdLast);
      return true;
   }
   return false;
}

bool PRELOAD_DB(U32 & id, SimDataBlock ** data, bool server, const char * clientMissing, const char * serverMissing)
{
   if (server)
   {
      if (*data)
         id = (*data)->getId();
      else if (server && serverMissing)
      {
         Con::errorf(ConsoleLogEntry::General,serverMissing);
         return false;
      }
   }
   else
   {
      if (id && !Sim::findObject(id,*data) && clientMissing)
      {
         Con::errorf(ConsoleLogEntry::General,clientMissing);
         return false;
      }
   }
   return true;
}
//----------------------------------------------------------------------------

bool GameBase::gShowBoundingBox=false;
GameBaseData GameBase::msDataBlock;

//----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(GameBase);

DECLARE_SERIALIBLE( GameBase );

GameBase::GameBase()
{
   mNetFlags.set(Ghostable);
   mTypeMask |= GameBaseObjectType;

   mProcessTag = 0;
   mDataBlock = 0;
   mDataBlockId = 0;
   mProcessTick = true;
   mNameTag = "";
   mControllingClient = 0;
   
#ifdef POWER_DEBUG_NET_MOVES
   mLastMoveId = 0;
   mTicksSinceLastMove = 0;
   mIsAiControlled = false;
#endif

}

GameBase::~GameBase()
{
   plUnlink();
}


//----------------------------------------------------------------------------

bool GameBase::onAdd()
{
   if (!Parent::onAdd() || !mDataBlock)
      return false;

   if (isClientObject()) 
   {
	  if(isScopeLocal())
	  {
		  //Ray: 如果是客户端预载的物件需要在这里设置DATABLOCK
		  if (!onNewDataBlock(mDataBlock))
			  return false;
	  }

	  // Client datablock are initialized by the initial update
	  gClientProcessList.addObject(this);
   }
   else 
   {
      // Datablock must be initialized on the server
      if (!onNewDataBlock(mDataBlock))
         return false;
	  gServerProcessList.addObject(this);
   }
   return true;
}

void GameBase::onRemove()
{
   plUnlink();
   clearEffectItem();
   Parent::onRemove();
}

void GameBase::addEffectItem(U32 id)
{
	m_EffectItem.push_back(id);
}

void GameBase::deleteEffectItem(U32 id)
{
	for(int i=m_EffectItem.size()-1;i>=0;i--)
	{
		if(id == m_EffectItem[i])
		{
			m_EffectItem.erase(i);
			break;
		}
	}
}

void GameBase::clearEffectItem()
{
	for(int i=m_EffectItem.size()-1;i>=0;i--)
	{
		g_EffectPacketContainer.removeEffectPacket(m_EffectItem[i],false);
	}
	m_EffectItem.clear();
}

bool GameBase::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dptr;
   mDataBlockId = 0;

   if (!mDataBlock)
      return false;

   mDataBlockId = mDataBlock->dataBlockId;

   setMaskBits(DataBlockMask);
   return true;
}

bool GameBase::onChangeDataBlock(GameBaseData* dptr)
{
	if (!mDataBlock)
		AssertRelease(false, "GameBase::onChangeDataBlock");

	mDataBlock = dptr;
	mDataBlockId = dptr->dataBlockId;

	setMaskBits(DataBlockMask);
	return true;
}

void GameBase::inspectPostApply()
{
   Parent::inspectPostApply();
   setMaskBits(ExtendedInfoMask);
}

//----------------------------------------------------------------------------

void GameBase::processTick(const Move * move)
{
#ifdef POWER_DEBUG_NET_MOVES
   if (!move)
      mTicksSinceLastMove++;

   const char * srv = isClientObject() ? "client" : "server";
   const char * who = "";
   if (isClientObject())
   {
      if (this == (GameBase*)GameConnection::getConnectionToServer()->getControlObject())
         who = " player";
      else
         who = " ghost";
      if (mIsAiControlled)
         who = " ai";
   }
   if (isServerObject())
   {
      if (dynamic_cast<AIConnection*>(getControllingClient()))
      {
         who = " ai";
         mIsAiControlled = true;
      }
      else if (getControllingClient())
      {
         who = " player";
         mIsAiControlled = false;
      }
      else
      {
         who = "";
         mIsAiControlled = false;
      }
   }
   U32 moveid = mLastMoveId+mTicksSinceLastMove;
   if (move)
      moveid = move->id;

   if (move)
   {
      mLastMoveId = move->id;
      mTicksSinceLastMove=0;
   }
#endif
}

void GameBase::interpolateTick(F32 backDelta)
{
}

void GameBase::advanceTime(F32)
{
}

void GameBase::preprocessMove(Move *move)
{
}

//----------------------------------------------------------------------------

F32 GameBase::getUpdatePriority(CameraScopeQuery *camInfo, U32 updateMask, S32 updateSkips)
{
   updateMask;

   // Calculate a priority used to decide if this object
   // will be updated on the client.  All the weights
   // are calculated 0 -> 1  Then weighted together at the
   // end to produce a priority.
   Point3F pos;
   getWorldBox().getCenter(&pos);
   pos -= camInfo->pos;
   F32 dist = pos.len();
   if (dist == 0.0f) dist = 0.001f;
   pos *= 1.0f / dist;

   // Weight based on linear distance, the basic stuff.
   F32 wDistance = (dist < camInfo->visibleDistance)?
      1.0f - (dist / camInfo->visibleDistance): 0.0f;

   // Weight by field of view, objects directly in front
   // will be weighted 1, objects behind will be 0
   F32 dot = mDot(pos,camInfo->orientation);
   bool inFov = dot > camInfo->cosFov;
   F32 wFov = inFov? 1.0f: 0;

   // Weight by linear velocity parallel to the viewing plane
   // (if it's the field of view, 0 if it's not).
   F32 wVelocity = 0.0f;
   if (inFov)
   {
      Point3F vec;
      mCross(camInfo->orientation,getVelocity(),&vec);
      wVelocity = (vec.len() * camInfo->fov) /
         (camInfo->fov * camInfo->visibleDistance);
      if (wVelocity > 1.0f)
         wVelocity = 1.0f;
   }

   // Weight by interest.
   F32 wInterest;
   if (getType() & GameObjectType)
      wInterest = 0.75f;
   else if (getType() & ProjectileObjectType)
   {
      // Projectiles are more interesting if they
      // are heading for us.
      wInterest = 0.30f;
      F32 dot = -mDot(pos,getVelocity());
      if (dot > 0.0f)
         wInterest += 0.20 * dot;
   }
   else
   {
      if (getType() & ItemObjectType)
         wInterest = 0.25f;
      else
         // Everything else is less interesting.
         wInterest = 0.0f;
   }

   // Weight by updateSkips
   F32 wSkips = updateSkips * 0.5;

   // Calculate final priority, should total to about 1.0f
   //
   return
      wFov       * sUpFov +
      wDistance  * sUpDistance +
      wVelocity  * sUpVelocity +
      wSkips     * sUpSkips +
      wInterest  * sUpInterest;
}

//----------------------------------------------------------------------------
bool GameBase::setDataBlock(GameBaseData* dptr)
{
   if (isGhost() || isProperlyAdded()) {
      if (mDataBlock != dptr)
         return onNewDataBlock(dptr);
   }
   else
      mDataBlock = dptr;
   return true;
}


//--------------------------------------------------------------------------
void GameBase::scriptOnAdd()
{
   // Script onAdd() must be called by the leaf class after
   // everything is ready.
   if (!isGhost())
      Con::executef(mDataBlock, "onAdd",scriptThis());
}

void GameBase::scriptOnNewDataBlock()
{
   // Script onNewDataBlock() must be called by the leaf class
   // after everything is loaded.
   if (!isGhost())
      Con::executef(mDataBlock, "onNewDataBlock",scriptThis());
}

void GameBase::scriptOnRemove()
{
   // Script onRemove() must be called by leaf class while
   // the object state is still valid.
   if (!isGhost() && mDataBlock)
      Con::executef(mDataBlock, "onRemove",scriptThis());
}

//----------------------------------------------------------------------------
void GameBase::processAfter(GameBase* obj)
{
   mAfterObject = obj;
   if ((const GameBase*)obj->mAfterObject == this)
      obj->mAfterObject = 0;
   if (isGhost())
      gClientProcessList.markDirty();
   else
      gServerProcessList.markDirty();
}

void GameBase::clearProcessAfter()
{
   mAfterObject = 0;
}

//----------------------------------------------------------------------------

void GameBase::setControllingClient(GameConnection* client)
{
   if (isClientObject())
   {
      if (mControllingClient)
         Con::executef(this, "setControl", "0");
      if (client)
         Con::executef(this, "setControl", "1");
   }

   mControllingClient = client;
   setMaskBits(ControlMask);
}

U32 GameBase::getPacketDataChecksum(GameConnection * connection)
{
   // just write the packet data into a buffer
   // then we can CRC the buffer.  This should always let us
   // know when there is a checksum problem.

   static U8 buffer[Net::MaxPacketBuffSize] = { 0, };
   BitStream stream(buffer, sizeof(buffer));

   writePacketData(connection, &stream);
   U32 byteCount = stream.getPosition();
   U32 ret = calculateCRC(buffer, byteCount, 0xFFFFFFFF);
   dMemset(buffer, 0, byteCount);
   return ret;
}

void GameBase::writePacketData(GameConnection*, BitStream*)
{
}

void GameBase::readPacketData(GameConnection*, BitStream*)
{
}

U64 GameBase::packUpdate(NetConnection *, U64 mask, BitStream *stream)
{
   // Check the mask for the ScaleMask; if it's true, pass that in.
   if (stream->writeFlag( mask & ScaleMask ) ) 
   {
       mathWrite( *stream, Parent::getScale() );
   }

   if (stream->writeFlag((mask & DataBlockMask) && mDataBlock != NULL)) 
   {
	  if (stream->writeFlag(mNetFlags.test(NetOrdered)))
		   stream->writeInt(mOrderGUID,16);

	  //Ray: 因为datablock在客户端本地载入，所以这里需要发送名称
      if(stream->writeFlag(mDataBlock->getName() && mDataBlock->getName()[0]))
		  stream->writeString(mDataBlock->getName());
	  else
		  stream->writeInt(mDataBlockId, DataBlockBits);
   }

#ifdef POWER_DEBUG_NET_MOVES
   stream->write(mLastMoveId);
   stream->writeFlag(mIsAiControlled);
#endif

   return 0;
}

void GameBase::unpackUpdate(NetConnection *con, BitStream *stream)
{
   if (stream->readFlag()) {
      VectorF scale;
      mathRead( *stream, &scale );
      setScale( scale );
   }
   if (stream->readFlag())
   {
      GameBaseData* dptr = 0;
	  if (stream->readFlag())
		  mOrderGUID = stream->readInt(16);

	  //Ray: 因为datablock在客户端本地载入，所以这里需要发送名称
	  if(stream->readFlag())
	  {
		  StringTableEntry pDatablockName = stream->readSTString();
		  if (!Sim::findObject(pDatablockName,dptr) || !setDataBlock(dptr))
			  con->setLastError("Invalid packet GameBase::unpackUpdate() name:%s", pDatablockName);
	  }
	  else
	  // <Edit> [3/11/2009 joy] datablock需要从多个地方获取
	  {
		  mDataBlockId = stream->readInt(DataBlockBits);
		  if (!dptr)
			  dptr = g_NpcRepository.GetNpcData(mDataBlockId);							//查找NPC
		  if (!dptr)
			  dptr = g_PlayerRepository.GetPlayerData(mDataBlockId);					//查找玩家
		  if (!dptr)
			  dptr = gCollectionRepository.GetCollectionObjectData(mDataBlockId);		//查找采集对象
		  if (!dptr)
			  dptr = g_TriggerDataManager->getTriggerData(mDataBlockId);				//触发器管理
		  if (!dptr)
			  dptr = g_PetRepository.GetPetData(mDataBlockId);							//查找宠物
		  if (!dptr)
			  dptr = g_MountRepository.GetMountData(mDataBlockId);						//查找骑宠
		  if (!dptr)
			  dptr = &msDataBlock;

		  // 更换dataBlock
		  if (mDataBlock && dptr && !onChangeDataBlock(dptr))
			  AssertRelease(false, "GameBase::unpackUpdate - onChangeDataBlock");
		  if (!dptr || !setDataBlock(dptr))
			  con->setLastError("Invalid packet GameBase::unpackUpdate()");
	  }
   }

#ifdef POWER_DEBUG_NET_MOVES
   stream->read(&mLastMoveId);
   mTicksSinceLastMove = 0;
   mIsAiControlled = stream->readFlag();
#endif
}

//addGroup("Misc");	
//addField("nameTag",   TypeCaseString,      Offset(mNameTag,   GameBase));
//addField("dataBlock", TypeGameBaseDataPtr, Offset(mDataBlock, GameBase));
//addField("dataBlockId", TypeS32,			  Offset(mDataBlockId, GameBase));
//endGroup("Misc");	

void GameBase::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	stream << mDataBlockId;
	if( mDataBlock )
	{
		stream.writeString( mDataBlock->getName() );
	}
	else
	{
		stream.writeString( "" );
	}
	
	stream.writeString( mNameTag );
}

void GameBase::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	stream >> mDataBlockId;

	char name[1024];
	stream.readString( name, 1024 );
	if( dStrcmp( name, "" ) )
		mDataBlock = (GameBaseData*)Sim::findObject( name );

	char buf[1024];
	stream.readString( buf, 1024 );
	mNameTag = StringTable->insert( buf );
}

//----------------------------------------------------------------------------
ConsoleMethod( GameBase, getDataBlock, S32, 2, 2, "()"
              "Return the datablock this GameBase is using.")
{
   return object->getDataBlock()? object->getDataBlock()->getId(): 0;
}

//----------------------------------------------------------------------------
ConsoleMethod(GameBase, setDataBlock, bool, 3, 3, "(DataBlock db)"
              "Assign this GameBase to use the specified datablock.")
{
   GameBaseData* data;
   if (Sim::findObject(argv[2],data)) {
      return object->setDataBlock(data);
   }
   Con::errorf("Could not find data block \"%s\"",argv[2]);
   return false;
}

//----------------------------------------------------------------------------
// 获取游戏对象的数据ID
ConsoleMethod(GameBase,GetDataID, S32, 2, 2, "%obj.GetDataID()")
{
	return object->getDataID();
}

//----------------------------------------------------------------------------
// 安全删除游戏对象
ConsoleMethod(GameBase,safeDeleteObject, void, 2, 2, "%obj.safeDeleteObject()")
{
	object->safeDeleteObject();
}

//----------------------------------------------------------------------------
IMPLEMENT_CONSOLETYPE(GameBaseData)
IMPLEMENT_GETDATATYPE(GameBaseData)
IMPLEMENT_SETDATATYPE(GameBaseData)

void GameBase::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Misc");	
   addField("nameTag",   TypeCaseString,      Offset(mNameTag,   GameBase));
   addField("dataBlock", TypeGameBaseDataPtr, Offset(mDataBlock, GameBase));
   addField("dataBlockId", TypeS32,			  Offset(mDataBlockId, GameBase));
   endGroup("Misc");	
}

void GameBase::consoleInit()
{
#ifdef POWER_DEBUG
   Con::addVariable("GameBase::boundingBox", TypeBool, &gShowBoundingBox);
#endif
}
