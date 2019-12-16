//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PARTICLEEMITTERDUMMY_H_
#define _PARTICLEEMITTERDUMMY_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase.h"
#endif

class EmitterDataInterface;
class EmitterInterface;
//*****************************************************************************
// ParticleEmitterNodeData
//*****************************************************************************
class ParticleEmitterNodeData : public GameBaseData
{
   typedef GameBaseData Parent;

protected:
   bool onAdd();

public:
   StringTableEntry emitters;	//发射器Data名列表
   Vector<StringTableEntry> mEmitterDatablockNames;

public:
   ParticleEmitterNodeData();
   ~ParticleEmitterNodeData();

   void packData(BitStream*);
   void unpackData(BitStream*);
   bool preload(bool server, char errorBuffer[256]);

   DECLARE_CONOBJECT(ParticleEmitterNodeData);
   static void initPersistFields();
};


//*****************************************************************************
// ParticleEmitterNode
//*****************************************************************************
class ParticleEmitterNode : public GameBase
{
	typedef GameBase Parent;

	enum EMITTER_NODE_TYPE
	{
		EMITTER_NODE_TYPE_PARTICLE = 1,
		EMITTER_NODE_TYPE_RIBBON,
	};

  private:
   ParticleEmitterNodeData* mDataBlock;

  protected:
   bool onAdd();
   void onRemove();
   
   Vector<EmitterInterface*>        mEmitter;

  public:
   ParticleEmitterNode();
   ~ParticleEmitterNode();
   
     
   // Time/Move Management
  public:
   void advanceTime(F32 dt);
   void setPause(bool pause);
   void setTransform(const MatrixF & mat);
   void setRotationMX(const MatrixF & mat);

   void setTime(F32 time);
   void setTimeScale(F32 scale);
   void render(const Point3F& camPos);

   DECLARE_CONOBJECT(ParticleEmitterNode);
   static void initPersistFields();
   bool onNewDataBlock(GameBaseData *dptr);

   U64  packUpdate  (NetConnection *conn, U64 mask, BitStream* stream);
   void unpackUpdate(NetConnection *conn,           BitStream* stream);
};

#endif // _H_PARTICLEEMISSIONDUMMY

