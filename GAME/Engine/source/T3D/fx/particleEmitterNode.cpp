//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "particleEmitterNode.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "T3D/fx/particleEmitter.h"
#include "math/mathIO.h"

IMPLEMENT_CO_DATABLOCK_V1(ParticleEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(ParticleEmitterNode);

//-----------------------------------------------------------------------------
// ParticleEmitterNodeData
//-----------------------------------------------------------------------------
ParticleEmitterNodeData::ParticleEmitterNodeData()
{
	emitters = NULL;
}

ParticleEmitterNodeData::~ParticleEmitterNodeData()
{

}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleEmitterNodeData::initPersistFields()
{
   Parent::initPersistFields();

   addField("emitters", TypeString, Offset(emitters, ParticleEmitterNodeData));
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleEmitterNodeData::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   if(emitters)
   {
	   char buff[1024];
	   dStrcpy(buff,1024,emitters);

	   StringTableEntry item;
	   char *begin = buff;
	   char *end = buff;
	   while(1)
	   {
		   if(*end==' ')
		   {
			   *end = 0;
			   item = StringTable->insert(begin);
			   mEmitterDatablockNames.push_back(item);

			   end++;
			   begin = end;
		   }
		   else if(*end==0)
		   {
			   item = StringTable->insert(begin);
			   mEmitterDatablockNames.push_back(item);
			   break;
		   }

		   end++;
	   }
   }

   return true;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void ParticleEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->writeInt((U32)mEmitterDatablockNames.size(),32);
	for(int i=0;i<mEmitterDatablockNames.size();i++)
	{
		stream->writeString(mEmitterDatablockNames[i]);
	}
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void ParticleEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	mEmitterDatablockNames.clear();

	char buff[256];
	StringTableEntry item;

	U32 size = stream->readInt(32);
	for(int i=0;i<size;i++)
	{
		stream->readString(buff,256);
		item = StringTable->insert(buff);
		mEmitterDatablockNames.push_back(item);
	}
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool ParticleEmitterNodeData::preload(bool server, char errorBuffer[256])
{
   if( Parent::preload(server, errorBuffer) == false )
      return false;

   return true;
}


//-----------------------------------------------------------------------------
// ParticleEmitterNode
//-----------------------------------------------------------------------------
ParticleEmitterNode::ParticleEmitterNode()
{
   // Todo: ScopeAlways?
   mNetFlags.set(Ghostable);
   mTypeMask |= EnvironmentObjectType;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
ParticleEmitterNode::~ParticleEmitterNode()
{
   //
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleEmitterNode::initPersistFields()
{
   Parent::initPersistFields();
   removeField("dataBlockId");
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleEmitterNode::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	mEmitter.clear();

	EmitterDataInterface *datablockptr=NULL;
	for(int i=0;i<mDataBlock->mEmitterDatablockNames.size();i++)
	{
		if( Sim::findObject(mDataBlock->mEmitterDatablockNames[i], datablockptr) )
		{
			EmitterInterface* pEmitter = datablockptr->createEmitterInstance();
			pEmitter->onNewDataBlock(datablockptr);
			pEmitter->SetPause(false);
			pEmitter->useClosestPoint(IsUseClosestPoint());
			if( pEmitter->registerObject() == false )
			{
				Con::warnf(ConsoleLogEntry::General, "Could not register base emitter for particle of class: %s", mDataBlock->getName());
				delete pEmitter;
				return false;
			}
			mEmitter.push_back(pEmitter);
		}
	}

	mObjBox.min.set(-0.5, -0.5, -0.5);
	mObjBox.max.set( 0.5,  0.5,  0.5);
	resetWorldBox();
	addToScene();

	return true;
}

//-----------------------------------------------------------------------------
// onRemove
//-----------------------------------------------------------------------------
void ParticleEmitterNode::onRemove()
{
	removeFromScene();

	//Ray: Çå³ý¶ÔÏó
	for(int i=0;i<mEmitter.size();i++)
	{
		mEmitter[i]->deleteObject();
	}

	mEmitter.clear();

	Parent::onRemove();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool ParticleEmitterNode::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<ParticleEmitterNodeData*>(dptr);
   if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
      return false;

   // Todo: Uncomment if this is a "leaf" class
   scriptOnNewDataBlock();
   return true;
}

//-----------------------------------------------------------------------------
// advanceTime
//-----------------------------------------------------------------------------
void ParticleEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	Point3F emitPoint, emitVelocity;
	Point3F emitAxis(0, 0, 1);
	getTransform().mulV(emitAxis);
	getTransform().getColumn(3, &emitPoint);
	emitVelocity = emitAxis;

	for(int i=0;i<mEmitter.size();i++)
	{
		mEmitter[i]->setTransform(getTransform());
		mEmitter[i]->doEmitter(emitPoint, emitPoint, emitAxis,	emitVelocity, (U32)(dt  * 1000.0f));
		mFinalVisibility = getFadeVal() * getVisibility();
		mEmitter[i]->SetEmitterAlpha(getFinalVisibility());
	}
}

//-----------------------------------------------------------------------------
// setPause
//-----------------------------------------------------------------------------
void ParticleEmitterNode::setPause(bool pause)
{
	for(int i=0;i<mEmitter.size();i++)
	{
		mEmitter[i]->SetPause(pause);
	}
}

//-----------------------------------------------------------------------------
// setTransform
//-----------------------------------------------------------------------------
void ParticleEmitterNode::setTransform(const MatrixF & mat)
{
	Parent::setTransform(mat);
	for(int i=0;i<mEmitter.size();i++)
	{
		mEmitter[i]->setTransform(mat);
	}
}

void ParticleEmitterNode::setRotationMX(const MatrixF & mat)
{
	for(int i=0;i<mEmitter.size();i++)
	{
		mEmitter[i]->SetRotationMX(mat);
	}
}

//-----------------------------------------------------------------------------
// setTime
//-----------------------------------------------------------------------------
void ParticleEmitterNode::setTime(F32 time)
{
    for(int i=0;i<mEmitter.size();i++)
        mEmitter[i]->SetTime(time);
}

//-----------------------------------------------------------------------------
// setTimeScale
//-----------------------------------------------------------------------------
void ParticleEmitterNode::setTimeScale(F32 scale)
{
    for(int i=0;i<mEmitter.size();i++)
        mEmitter[i]->SetTimeScale(scale);
}

//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
void ParticleEmitterNode::render(const Point3F& camPos)
{
    for(int i=0;i<mEmitter.size();i++)
        mEmitter[i]->Render(camPos);
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U64 ParticleEmitterNode::packUpdate(NetConnection* con, U64 mask, BitStream* stream)
{
   U64 retMask = Parent::packUpdate(con, mask, stream);

   mathWrite(*stream, getTransform());
   mathWrite(*stream, getScale());

   return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void ParticleEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   MatrixF temp;
   Point3F tempScale;
   mathRead(*stream, &temp);
   mathRead(*stream, &tempScale);

   setScale(tempScale);
   setTransform(temp);
}
