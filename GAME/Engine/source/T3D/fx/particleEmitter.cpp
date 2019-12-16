//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <WinSock2.h>
#include <d3dx9math.h>
#include "particleEmitter.h"
#include "sceneGraph/sceneGraph.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mRandom.h"
#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "renderInstance/renderInstMgr.h"
#include "renderInstance/RenderTranslucentMgr.h"
#include "platform/platformTimer.h"
#ifndef SXZPARTSYS
#include "util/processMgr.h"
#include "util/aniThread.h"
#include "T3D/gameProcess.h"
#endif
#include <d3dx9.h>
#include <memory>

#define MAX_TIME 10000.0f
#define TIME_PER_FRAME 0.03125f

GFXPrimitiveBufferHandle ParticleEmitter::primBuff;
GFXPrimitiveBufferHandle RibbonEmitter::primBuff;
#ifdef SXZPARTSYS
Vector<U16> ParticleEmitter::pbForMax;
Vector<U16> RibbonEmitter::pbForMax;
#endif

Point3F ParticleEmitter::mWindVelocity( 0.0, 0.0, 0.0 );
IMPLEMENT_CO_DATABLOCK_V1(ParticleEmitterData);
IMPLEMENT_CO_DATABLOCK_V1(RibbonEmitterData);
// Enum tables used for fields blendStyle, srcBlendFactor, dstBlendFactor.
// Note that the enums for srcBlendFactor and dstBlendFactor are consistent
// with the blending enums used in PowerEngine Game Builder.

static EnumTable::Enums blendStyleLookup[] =
{
    { ParticleEmitterData::BlendNormal,         "NORMAL"                },
    { ParticleEmitterData::BlendAdditive,       "ADDITIVE"              },
    { ParticleEmitterData::BlendSubtractive,    "SUBTRACTIVE"           },
    { ParticleEmitterData::BlendPremultAlpha,   "PREMULTALPHA"          },
    { ParticleEmitterData::BlendUser,           "USER"                  },
};
EnumTable blendStyleTable(sizeof(blendStyleLookup) / sizeof(EnumTable::Enums), &blendStyleLookup[0]);

static EnumTable::Enums srcBlendFactorLookup[] =
{
    { GFXBlendZero,                 "ZERO"                  },
    { GFXBlendOne,                  "ONE"                   },
    { GFXBlendDestColor,            "DST_COLOR"             },
    { GFXBlendInvDestColor,         "ONE_MINUS_DST_COLOR"   },
    { GFXBlendSrcAlpha,             "SRC_ALPHA"             },
    { GFXBlendInvSrcAlpha,          "ONE_MINUS_SRC_ALPHA"   },
    { GFXBlendDestAlpha,            "DST_ALPHA"             },
    { GFXBlendInvDestAlpha,         "ONE_MINUS_DST_ALPHA"   },
    { GFXBlendSrcAlphaSat,          "SRC_ALPHA_SATURATE"    },
};
EnumTable srcBlendFactorTable(sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums), &srcBlendFactorLookup[0]);

static EnumTable::Enums dstBlendFactorLookup[] =
{
    { GFXBlendZero,                 "ZERO"                  },
    { GFXBlendOne,                  "ONE"                   },
    { GFXBlendSrcColor,             "SRC_COLOR"             },
    { GFXBlendInvSrcColor,          "ONE_MINUS_SRC_COLOR"   },
    { GFXBlendSrcAlpha,             "SRC_ALPHA"             },
    { GFXBlendInvSrcAlpha,          "ONE_MINUS_SRC_ALPHA"   },
    { GFXBlendDestAlpha,            "DST_ALPHA"             },
    { GFXBlendInvDestAlpha,         "ONE_MINUS_DST_ALPHA"   },
};
EnumTable dstBlendFactorTable(sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums), &dstBlendFactorLookup[0]);

MatrixF ParentMX(EmitterInterface *pEmitter)
{
//	bool isInGame = true;
//
//#if (defined(NTJ_EDITOR) || defined(SXZPARTSYS))
//	isInGame = false;
//#endif
//
//	if(isInGame)
//		return pEmitter->getRenderTransform();
//	else
//	{
//		return MatrixF(true);
//	}

	return pEmitter->getRenderTransform();
}

ParticleEmitterData::ParticleEmitterData()
{
	m_bSort = true;
	dataBlockIds = -1;
    particleString   = NULL;
	// These members added for support of user defined blend factors
	// and optional particle sorting.
	blendStyle = BlendUndefined;
	srcBlendFactor = GFXBlendUndefined;
	dstBlendFactor = GFXBlendUndefined;
}

ParticleEmitterData::~ParticleEmitterData()
{
}

IMPLEMENT_CONSOLETYPE(ParticleEmitterData)
IMPLEMENT_GETDATATYPE(ParticleEmitterData)
IMPLEMENT_SETDATATYPE(ParticleEmitterData)

void ParticleEmitterData::initPersistFields()
{
	Parent::initPersistFields();

	addField("Sort",     TypeBool,    Offset(m_bSort,   ParticleEmitterData));
    addField("particles",            TypeString, Offset(particleString,     ParticleEmitterData));
	// These fields added for support of user defined blend factors and optional particle sorting.
	addField("blendStyle",         TypeEnum,     Offset(blendStyle,         ParticleEmitterData), 1, &blendStyleTable);
	addField("srcBlendFactor",     TypeEnum,     Offset(srcBlendFactor,     ParticleEmitterData), 1, &srcBlendFactorTable);
	addField("dstBlendFactor",     TypeEnum,     Offset(dstBlendFactor,     ParticleEmitterData), 1, &dstBlendFactorTable);
}

void ParticleEmitterData::packData(BitStream* stream)
{
	Parent::packData(stream);
	stream->writeFlag(m_bSort);
	stream->write(dataBlockIds);
	stream->writeInt(srcBlendFactor, 4);
	stream->writeInt(dstBlendFactor, 4);
	if (stream->writeFlag(particleString != 0))
		stream->writeString(particleString);
}

void ParticleEmitterData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
	m_bSort = stream->readFlag();
	stream->read(&dataBlockIds);
	srcBlendFactor = stream->readInt(4);
	dstBlendFactor = stream->readInt(4);
	particleString = (stream->readFlag()) ? stream->readSTString() : 0;
}

bool ParticleEmitterData::onAdd()
{
	if( Parent::onAdd() == false )
		return false;

#ifndef SXZPARTSYS
	if( particleString != NULL )
	{
		//   particleString is once again a list of particle datablocks so it
		//   must be parsed to extract the particle references.

		// First we parse particleString into a list of particle name tokens 
		Vector<char*> dataBlocks(__FILE__, __LINE__);
		S32 length = dStrlen(particleString) + 1;
		char* tokCopy = new char[length];
		dStrcpy(tokCopy, length, particleString);

		char * context = NULL;
		char* currTok = dStrtok(tokCopy, " \t", &context);
		while (currTok != NULL) 
		{
			dataBlocks.push_back(currTok);
			currTok = dStrtok(NULL, " \t", &context);
		}
		if (dataBlocks.size() == 0) 
		{
			Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) invalid particles string.  No datablocks found", getName());
			delete [] tokCopy;
			return false;
		}    

		for (U32 i = 0; i < dataBlocks.size(); i++) 
		{
			ParticleData* pData = NULL;
			if (Sim::findObject(dataBlocks[i], pData) == false) 
			{
				Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find particle datablock: %s", getName(), dataBlocks[i]);
			}
			else 
			{
				m_pParticleSystem = pData;
				dataBlockIds = pData->getId();
			}
		}

		// cleanup
		delete [] tokCopy;

		// check that we actually found some particle datablocks
		if (m_pParticleSystem == NULL) 
		{
			Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find any particle datablocks", getName());
			return false;
		}
	}
#endif

	// load the particle datablocks...
	//
	return true;
}

bool ParticleEmitterData::preload(bool server, char errorBuffer[256])
{
	if( Parent::preload(server, errorBuffer) == false )
		return false;

	if(dataBlockIds != -1)
	{
		ParticleData* pData = NULL;
		if (Sim::findObject(dataBlockIds, pData) == false) 
		{
			Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find particle datablock: %s", getName(), dataBlockIds);
		}
		else 
		{
			m_pParticleSystem = pData;
		}
	}

	// if blend-style is set to User, check for defined blend-factors
	if (blendStyle == BlendUser && (srcBlendFactor == GFXBlendUndefined || dstBlendFactor == GFXBlendUndefined))
	{
		blendStyle = BlendUndefined;
		Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) incomplete blend factor specification.", getName());
	}

	// silently switch Undefined blend-style to User if blend factors are both defined
	if (blendStyle == BlendUndefined && srcBlendFactor != GFXBlendUndefined && dstBlendFactor != GFXBlendUndefined)
	{
		blendStyle = BlendUser;
	}

	// if blend-style is undefined check legacy useInvAlpha settings
	if (blendStyle == BlendUndefined && m_pParticleSystem)
	{
		blendStyle = BlendNormal;
	}

	// set pre-defined blend-factors 
	switch (blendStyle)
	{
	case BlendNormal:
		srcBlendFactor = GFXBlendSrcAlpha;
		dstBlendFactor = GFXBlendInvSrcAlpha;
		break;
	case BlendSubtractive:
		srcBlendFactor = GFXBlendZero;
		dstBlendFactor = GFXBlendInvSrcColor;
		break;
	case BlendPremultAlpha:
		srcBlendFactor = GFXBlendOne;
		dstBlendFactor = GFXBlendInvSrcAlpha;
		break;
	case BlendUser:
		break;
	case BlendAdditive:
		srcBlendFactor = GFXBlendSrcAlpha;
		dstBlendFactor = GFXBlendOne;
		break;
	case BlendUndefined:
	default:
		blendStyle = BlendAdditive;
		srcBlendFactor = GFXBlendSrcAlpha;
		dstBlendFactor = GFXBlendOne;
		break;
	}

	return true;
}

void ParticleEmitterData::SetSortState(bool data)
{
	m_bSort = data;
}

bool ParticleEmitterData::GetSortState()
{
	return m_bSort;
}

bool ParticleEmitterData::reload()
{
	return true;
}

EmitterInterface *ParticleEmitterData::createEmitterInstance()
{
	return new ParticleEmitter;
}

RibbonEmitterData::RibbonEmitterData()
{
	m_pRibbon = NULL;
	RibbonString = NULL;
    dataBlockIds = -1;
}

IMPLEMENT_CONSOLETYPE(RibbonEmitterData)
IMPLEMENT_GETDATATYPE(RibbonEmitterData)
IMPLEMENT_SETDATATYPE(RibbonEmitterData)

void RibbonEmitterData::initPersistFields()
{
	Parent::initPersistFields();

#ifndef SXZPARTSYS
	addField("RibbonString",            TypeString, Offset(RibbonString,     RibbonEmitterData));
#endif
}

void RibbonEmitterData::packData(BitStream* stream)
{
	Parent::packData(stream);
	stream->write(dataBlockIds);
	if (stream->writeFlag(RibbonString != 0))
		stream->writeString(RibbonString);
}

void RibbonEmitterData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
	stream->read(&dataBlockIds);
	RibbonString = (stream->readFlag()) ? stream->readSTString() : 0;
}

bool RibbonEmitterData::onAdd()
{
	if( Parent::onAdd() == false )
		return false;

#ifndef SXZPARTSYS
	if( RibbonString != NULL )
	{
		Vector<char*> dataBlocks(__FILE__, __LINE__);
		S32 length = dStrlen(RibbonString) + 1;
		char* tokCopy = new char[length];
		dStrcpy(tokCopy, length, RibbonString);

		char * context = NULL;
		char* currTok = dStrtok(tokCopy, " \t", &context);
		while (currTok != NULL) 
		{
			dataBlocks.push_back(currTok);
			currTok = dStrtok(NULL, " \t", &context);
		}
		if (dataBlocks.size() == 0) 
		{
			Con::warnf(ConsoleLogEntry::General, "RibbonEmitterData(%s) invalid Ribbon string.  No datablocks found", getName());
			delete [] tokCopy;
			return false;
		}    

		for (U32 i = 0; i < dataBlocks.size(); i++) 
		{
			Ribbon* pData = NULL;
			if (Sim::findObject(dataBlocks[i], pData) == false) 
			{
				Con::warnf(ConsoleLogEntry::General, "RibbonEmitterData(%s) unable to find Ribbon datablock: %s", getName(), dataBlocks[i]);
			}
			else 
			{
				m_pRibbon = pData;
				dataBlockIds = pData->getId();
			}
		}

		// cleanup
		delete [] tokCopy;

		// check that we actually found some particle datablocks
		if (m_pRibbon == NULL) 
		{
			Con::warnf(ConsoleLogEntry::General, "RibbonEmitterData(%s) unable to find any Ribbon datablocks", getName());
			return false;
		}
	}
#endif

	// load the particle datablocks...
	//
	return true;
}

bool RibbonEmitterData::preload(bool server, char errorBuffer[256])
{
	if( Parent::preload(server, errorBuffer) == false )
		return false;

	if(dataBlockIds != -1)
	{
		Ribbon* pData = NULL;
		if (Sim::findObject(dataBlockIds, pData) == false) 
		{
			Con::warnf(ConsoleLogEntry::General, "RibbonEmitterData(%s) unable to find Ribbon datablock: %s", getName(), dataBlockIds);
		}
		else 
		{
			m_pRibbon = pData;
		}
	}

	return true;
}

EmitterInterface *RibbonEmitterData::createEmitterInstance()
{
	return new RibbonEmitter;
}

ParticleEmitter::ParticleEmitter()
{
	m_vCurEula = m_vEulaRand = Point3F(0.0f, 0.0f, 0.0f);
	m_vEulaStep = Point3F(0.0f, 0.0f, 0.0f);
	m_fEulaTime = 0.0f;
	m_fCurVelocity = 0.0f;
	m_fVelocityRand = 0.0f;
	m_fVelocityStep = 0.0f;
	m_fVelocityTime = 0.0f;
	m_fCurRepulseVelocity = 0.0f;
	m_fRepulseVelocityRand = 0.0f;
	m_fRepulseVelocityStep = 0.0f;
	m_fRepulseVelocityTime = 0.0f;
	m_fCurEmitTime = 0.0f;
	m_fEmitTimeInterval = 0.0f;
	m_fCurEmitTimeInterval = 0.0f;
	m_EmitterPos = Point3F(0.0f, 0.0f, 0.0f);
	m_fElapsedTime = 0.0f;
	m_vPosDeviation = Point3F(0.0f, 0.0f, 0.0f);
	m_vPosDeviationStep = Point3F(0.0f, 0.0f, 0.0f);
	m_fPosDeviationTime = 0.0f;

	m_iCurParticleNum = -1;
	m_bPause = true;
	m_pParticleList = NULL;
	m_pParticleFreeList = NULL;
	m_iRenderParticleNum = 0;
	m_bEmit = true;
	m_iMaxParticleNum = 0;

	m_vCurScale = m_vScaleStep = Point3F(1.0f, 1.0f, 1.0f);
    m_fScaleTime = 0.0f;

	mFillVB = false;

#ifdef SXZPARTSYS
	mCameraMatrix.identity();
#endif

#ifndef NTJ_SERVER
	static bool initilize = false;
	if (!initilize)
	{
        init();
        initilize = true;
	}
#endif
}

ParticleEmitter::~ParticleEmitter()
{
	Destory();
}

#ifndef NTJ_SERVER
void      ParticleEmitter::init()
{
	const U32 indexListSize = 2400; // 6 indices per particle
	const U32 MaxParticleNum = 400;
	U16 *indices = new U16[ indexListSize ];

	for( U32 i=0; i<MaxParticleNum; i++ )
	{
		// this index ordering should be optimal (hopefully) for the vertex cache
		U16 *idx = &indices[i*6];
		volatile U32 offset = i * 4;  // set to volatile to fix VC6 Release mode compiler bug
		idx[0] = 0 + offset;
		idx[1] = 1 + offset;
		idx[2] = 3 + offset;
		idx[3] = 0 + offset;
		idx[4] = 2 + offset;
		idx[5] = 3 + offset;
	}

#ifndef SXZPARTSYS
	U16 *ibIndices;
	GFXBufferType bufferType = GFXBufferTypeStatic;

#ifdef POWER_OS_XENON
	// Because of the way the volatile buffers work on Xenon this is the only
	// way to do this.
	bufferType = GFXBufferTypeVolatile;
#endif
	primBuff.set( GFX, indexListSize, 0, bufferType );
	primBuff.lock( &ibIndices );
	dMemcpy( ibIndices, indices, indexListSize * sizeof(U16) );
	primBuff.unlock();
#else
    pbForMax.setSize( indexListSize );
    dMemcpy( pbForMax.address(), indices, indexListSize * sizeof(U16) );
#endif

	delete [] indices;
}

void      ParticleEmitter::shutdown()
{
	primBuff = NULL;
}

#endif

bool ParticleEmitter::onNewDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<ParticleEmitterData*>(dptr);
	if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
		return false;

	scriptOnNewDataBlock();
	return true;
}

void ParticleEmitter::emitParticles(const Point3F& start,
									 const Point3F& end,
									 const Point3F& axis,
									 const Point3F& velocity,
									 const U32      numMilliseconds)
{
	if(m_iCurParticleNum > 0)
		updateBBox();

	if( mSceneManager == NULL )
	{
		gClientSceneGraph->addObjectToScene(this);
		gClientContainer.addObject(this);
		gClientProcessList.addObject(this);
	}
}

void ParticleEmitter::emitParticles(const Point3F& point,
									 const bool     useLastPosition,
									 const Point3F& axis,
									 const Point3F& velocity,
									 const U32      numMilliseconds)
{
	if(m_iCurParticleNum > 0)
		updateBBox();

	if( mSceneManager == NULL )
	{
		gClientSceneGraph->addObjectToScene(this);
		gClientContainer.addObject(this);
		gClientProcessList.addObject(this);
	}
}

void ParticleEmitter::emitParticles(const Point3F& rCenter,
									 const Point3F& rNormal,
									 const F32      radius,
									 const Point3F& velocity,
									 S32 count)
{
	// Set world bounding box
	mObjBox.min = rCenter - Point3F(radius, radius, radius);
	mObjBox.max = rCenter + Point3F(radius, radius, radius);
	resetWorldBox();

	// Make sure we're part of the world
	if( m_iCurParticleNum > 0 && mSceneManager == NULL )
	{
		gClientSceneGraph->addObjectToScene(this);
		gClientContainer.addObject(this);
		gClientProcessList.addObject(this);
	}
}

void ParticleEmitter::updateBBox()
{
	Point3F min1(1e10,   1e10,  1e10);
	Point3F max1(-1e10, -1e10, -1e10);

	for (Particle* part = m_pParticleList; part != NULL; part = part->m_pNext)
	{
		min1.setMin( part->m_vWorldPos );
		max1.setMax( part->m_vWorldPos );
	}

	mObjBox = Box3F(min1, max1);
	mWorldToObj.mul(mObjBox);
	mObjBox.min.convolveInverse(mObjScale);
	mObjBox.max.convolveInverse(mObjScale);
	resetWorldBox();
}

bool ParticleEmitter::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	removeFromProcessList();

	F32 radius = 5.0;
	mObjBox.min = Point3F(-radius, -radius, -radius);
	mObjBox.max = Point3F(radius, radius, radius);
	resetWorldBox();

	m_fLastDT = 0.0f;

	return true;
}

void ParticleEmitter::onRemove()
{
	removeFromScene();
	Parent::onRemove();
}

void ParticleEmitter::processTick(const Move *move)
{

}

void ParticleEmitter::SetTime(F32 time)
{
	F32 emitlife = mDataBlock->m_pParticleSystem->GetEmitLife();
	if (time > emitlife)
		return;
	else if (time > m_fCurEmitTime)
	{
		advanceTime(time - m_fCurEmitTime);
	}
	else if (time <= m_fCurEmitTime)
	{
		if (mDataBlock->m_pParticleSystem->GetRepeat())
		{
            advanceTime(emitlife - m_fCurEmitTime + time);
		}
		else
		{
			Reset();
            advanceTime(time);
		}
	}
}



void ParticleEmitter::doAdvanceTime(F32 dt)
{
	static Point3F dummyPoint(0,0,0);

	F32 displayv = mDataBlock->m_pParticleSystem->GetDisplayVelocity();

	if((displayv != 1.0f) && (displayv > 0.0f))
		dt *= displayv; 

	m_fElapsedTime += dt;
	if( m_fElapsedTime < TIME_PER_FRAME ) 
		return;

	while(m_fElapsedTime >= TIME_PER_FRAME)
	{
        m_fElapsedTime -= TIME_PER_FRAME;

		Parent::advanceTime(TIME_PER_FRAME);       

		if(!m_bPause)
		{
			if (m_fCurEmitTime > mDataBlock->m_pParticleSystem->GetEmitLife())
			{
				if (mDataBlock->m_pParticleSystem->GetRepeat())
				{
					//m_fCurEmitTime = 0.0f;
					//m_fEulaTime = 0.0f;
					//m_fVelocityTime = 0.0f;                     
					//m_fPosDeviationTime = 0.0f;
					//m_fScaleTime = 0.0f;
					//m_bEmit = true;
					InitSys();
				}
				else
				{
					m_bPause = true;
				}
			}
		}
#ifdef SXZPARTSYS
        //m_matParent = GetParentMatrix();
#endif

        m_fCurEmitTime += TIME_PER_FRAME;

		//粒子系统的更新以及相关变量的获取
		bool UseBillboard = mDataBlock->m_pParticleSystem->GetBillboardState();
		bool bTexSplinter = mDataBlock->m_pParticleSystem->GetTextureSplinterState();
		bool bTexSplinterBroadRandom = mDataBlock->m_pParticleSystem->GetTexSplinterBroadRandom();
		F32 texremaintime = mDataBlock->m_pParticleSystem->GetTexRemainTime();
		S32 splinternum = mDataBlock->m_pParticleSystem->GetTextureSplinterNum();

		D3DXMATRIX matReflect;
		D3DXVECTOR3 point(0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 normal( 0.0f,1.0f, 0.0f );
		D3DXPLANE   plane;
		D3DXPlaneFromPointNormal( &plane, &point, &normal);
		D3DXMatrixReflect( &matReflect, &plane );

		Point3F emitterpos;
		if(mDataBlock->m_pParticleSystem->GetAnimationPosition(m_fCurEmitTime, &emitterpos))
		{
			m_EmitterPos = emitterpos;
		}

		if(m_fEulaTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pParticleSystem->GetEulaTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pParticleSystem->GetEulaTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fEulaTime = point2;
							Point3F temp_vec1, temp_vec2;
							if(mDataBlock->m_pParticleSystem->GetRandomEula(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetRandomEula(i2 + 1, &temp_vec2))
							{
								m_vEulaStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_vCurEula = temp_vec1;
								mDataBlock->m_pParticleSystem->GetEulaRand(i2, &m_vEulaRand);
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							m_fEulaTime = point1;
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomEula(i2, &temp_vec1))
							{
								m_vEulaStep = (temp_vec1 - m_vCurEula)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime <= point1)
						{
							m_fEulaTime = point1;
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomEula(i2, &temp_vec1))
							{
								m_vEulaStep = (temp_vec1 - m_vCurEula)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							m_vEulaStep = Point3F(0.0f, 0.0f, 0.0f);
							m_fEulaTime = MAX_TIME;
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomEula(i2, &temp_vec1))
							{
								m_vCurEula = temp_vec1;
								mDataBlock->m_pParticleSystem->GetEulaRand(i2, &m_vEulaRand);
							}
						}
						break;
					}
				}
				else
				{
					m_vEulaStep = Point3F(0.0f, 0.0f, 0.0f);
					m_fEulaTime = MAX_TIME;
					break;
				}
			}
		}
		if((m_vEulaStep.x != 0.0f) || (m_vEulaStep.y != 0.0f) || (m_vEulaStep.z != 0.0f))
			m_vCurEula += m_vEulaStep * TIME_PER_FRAME;

		if(m_fVelocityTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pParticleSystem->GetVelocityTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pParticleSystem->GetVelocityTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fVelocityTime = point2;
							F32 temp_vec1, temp_vec2;
							if(mDataBlock->m_pParticleSystem->GetRandomVelocity(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetRandomVelocity(i2 + 1, &temp_vec2))
							{
								m_fVelocityStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_fCurVelocity = temp_vec1;
								mDataBlock->m_pParticleSystem->GetVelocityRand(i2, &m_fVelocityRand);
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							m_fVelocityTime = point1;
							F32 temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomVelocity(i2, &temp_vec1))
							{
								m_fVelocityStep = (temp_vec1 - m_fCurVelocity)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime <= point1)
						{
							m_fVelocityTime = point1;
							F32 temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomVelocity(i2, &temp_vec1))
							{
								m_fVelocityStep = (temp_vec1 - m_fCurVelocity)/(point1 - m_fCurEmitTime);
							}
						}
						else
						{
							m_fVelocityStep = 0.0f;
							m_fVelocityTime = MAX_TIME;
							F32 temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomVelocity(i2, &temp_vec1))
							{
								m_fCurVelocity = temp_vec1;
								mDataBlock->m_pParticleSystem->GetVelocityRand(i2, &m_fVelocityRand);
							}
						}
						break;
					}
				}
				else
				{
					m_fVelocityStep = 0.0f;
					m_fVelocityTime = MAX_TIME;
					break;
				}
			}
		}
		if(m_fVelocityStep != 0.0f)
			m_fCurVelocity += m_fVelocityStep * TIME_PER_FRAME;

		if(m_fRepulseVelocityTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pParticleSystem->GetRepulseVelocityTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pParticleSystem->GetRepulseVelocityTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fRepulseVelocityTime = point2;
							F32 temp_vec1, temp_vec2;
							if(mDataBlock->m_pParticleSystem->GetRandomRepulseVelocity(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetRandomRepulseVelocity(i2 + 1, &temp_vec2))
							{
								m_fRepulseVelocityStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_fCurRepulseVelocity = temp_vec1;
								mDataBlock->m_pParticleSystem->GetRepulseVelocityRand(i2, &m_fRepulseVelocityRand);
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							m_fRepulseVelocityTime = point1;
							F32 temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomRepulseVelocity(i2, &temp_vec1))
							{
								m_fRepulseVelocityStep = (temp_vec1 - m_fCurRepulseVelocity)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime <= point1)
						{
							m_fRepulseVelocityTime = point1;
							F32 temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomRepulseVelocity(i2, &temp_vec1))
							{
								m_fRepulseVelocityStep = (temp_vec1 - m_fCurRepulseVelocity)/(point1 - m_fCurEmitTime);
							}
						}
						else
						{
							m_fRepulseVelocityStep = 0.0f;
							m_fRepulseVelocityTime = MAX_TIME;
							F32 temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomRepulseVelocity(i2, &temp_vec1))
							{
								m_fCurRepulseVelocity = temp_vec1;
								mDataBlock->m_pParticleSystem->GetRepulseVelocityRand(i2, &m_fRepulseVelocityRand);
							}
						}
						break;
					}
				}
				else
				{
					m_fRepulseVelocityStep = 0.0f;
					m_fRepulseVelocityTime = MAX_TIME;
					break;
				}
			}
		}
		if(m_fRepulseVelocityStep != 0.0f)
			m_fCurRepulseVelocity += m_fRepulseVelocityStep * TIME_PER_FRAME;

		if(m_fScaleTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pParticleSystem->GetScaleTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pParticleSystem->GetScaleTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fScaleTime = point2;
							Point3F temp_vec1, temp_vec2;
							if(mDataBlock->m_pParticleSystem->GetScalePoint(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetScalePoint(i2 + 1, &temp_vec2))
							{
								m_vScaleStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_vCurScale = temp_vec1;
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetScalePoint(i2, &temp_vec1))
							{
								m_vScaleStep = (temp_vec1 - m_vCurScale)/(point1 - m_fCurEmitTime);
							}
							m_fScaleTime = point1;
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime > point1)
						{
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetScalePoint(i2, &temp_vec1))
							{
								m_fScaleTime = MAX_TIME;
								m_vScaleStep = Point3F(0.0f, 0.0f, 0.0f);
								m_vCurScale = temp_vec1;
							}
						}
						else
						{
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetScalePoint(i2, &temp_vec1))
							{
								m_vScaleStep = (temp_vec1 - m_vCurScale)/(point1 - m_fCurEmitTime);
							}
							m_fScaleTime = point1;
						}
						break;
					}
				}
				else
				{
					m_vScaleStep = Point3F(0.0f, 0.0f, 0.0f);
					m_fScaleTime = MAX_TIME;
					break;
				}
			}
		}
		if((m_vScaleStep.x != 0.0f) || (m_vScaleStep.y != 0.0f) || (m_vScaleStep.z != 0.0f))
		{
			m_vCurScale += m_vScaleStep * TIME_PER_FRAME;
		}

		if(m_fPosDeviationTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pParticleSystem->GetPosDeviationTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pParticleSystem->GetPosDeviationTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fPosDeviationTime = point2;
							Point3F temp_vec1, temp_vec2;
							if(mDataBlock->m_pParticleSystem->GetRandomPosDeviation(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetRandomPosDeviation(i2 + 1, &temp_vec2))
							{
								m_vPosDeviationStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_vPosDeviation = temp_vec1;
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							m_fPosDeviationTime = point1;
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomPosDeviation(i2, &temp_vec1))
							{
								m_vPosDeviationStep = (temp_vec1 - m_vPosDeviation)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime <= point1)
						{
							m_fPosDeviationTime = point1;
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomPosDeviation(i2, &temp_vec1))
							{
								m_vPosDeviationStep = (temp_vec1 - m_vPosDeviation)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							m_vPosDeviationStep = Point3F(0.0f, 0.0f, 0.0f);
							m_fPosDeviationTime = MAX_TIME;
							Point3F temp_vec1;
							if(mDataBlock->m_pParticleSystem->GetRandomPosDeviation(i2, &temp_vec1))
							{
								m_vPosDeviation = temp_vec1;
							}
						}
						break;
					}
				}
				else
				{
					m_vPosDeviationStep = Point3F(0.0f, 0.0f, 0.0f);
					m_fPosDeviationTime = MAX_TIME;
					break;
				}
			}
		}
		if((m_vPosDeviationStep.x != 0.0f) || (m_vPosDeviationStep.y != 0.0f) || (m_vPosDeviationStep.z != 0.0f))
			m_vPosDeviation += m_vPosDeviationStep * TIME_PER_FRAME;

		//粒子更新
		Particle* pParticle = m_pParticleList;
		Particle* temp_pp = NULL;

		while(pParticle)
		{
			pParticle->m_fAge += TIME_PER_FRAME;
			temp_pp = pParticle->m_pNext;
			if(pParticle->m_fAge > pParticle->m_fLife)
			{
				if((pParticle->m_pPrev) && (pParticle->m_pNext))
				{
					pParticle->m_pPrev->m_pNext = pParticle->m_pNext;
					pParticle->m_pNext->m_pPrev = pParticle->m_pPrev;
				}
				else if (pParticle->m_pPrev)
				{
					pParticle->m_pPrev->m_pNext = NULL;
				}
				else if (pParticle->m_pNext)
				{
					pParticle->m_pNext->m_pPrev = NULL;
					m_pParticleList = pParticle->m_pNext;
				}
				else
				{
					m_pParticleList = NULL;
				}
				pParticle->m_pNext = m_pParticleFreeList;
				pParticle->m_pPrev = NULL;
				m_pParticleFreeList = pParticle;
				m_iCurParticleNum -= 1;
			}
			else 
			{
				if(pParticle->m_fSizeTimeX < pParticle->m_fAge)
				{
					for(S32 i2 = 0; ; i2++)
					{
						F32 point1,point2;
						if (mDataBlock->m_pParticleSystem->GetSizeXTimePoint(i2, &point1))
						{
							if (mDataBlock->m_pParticleSystem->GetSizeXTimePoint(i2 + 1, &point2))
							{
								if((pParticle->m_fAge > point1) && (pParticle->m_fAge <= point2))
								{
									pParticle->m_fSizeTimeX = point2;
									F32 temp_vec1=0,temp_vec2=0,temp_vec3=0,temp_vec4=0;
									if(mDataBlock->m_pParticleSystem->GetSizePointXStep(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetSizePointXStep(i2 + 1, &temp_vec2))
									{
										pParticle->m_fSizeStepX = (temp_vec2 - temp_vec1)/(point2 - point1);
										temp_vec3 = pParticle->m_fCurSizeX;
										pParticle->m_fCurSizeX = temp_vec1;
									}
									if (!mDataBlock->m_pParticleSystem->GetSizeRandomBothXAndY())
									{
										if(i2 == 0)
										{
											if(mDataBlock->m_pParticleSystem->GetSizePointX(i2, &temp_vec3) && mDataBlock->m_pParticleSystem->GetSizePointY(i2, &temp_vec4))
											{
												pParticle->m_fCurSizeY = temp_vec1/temp_vec3*temp_vec4;
												pParticle->m_fSizeStepY = (temp_vec2/temp_vec1*pParticle->m_fCurSizeY - pParticle->m_fCurSizeY)/(point2 - point1);
											}
										}
										else
										{
											pParticle->m_fCurSizeY = pParticle->m_fCurSizeX/temp_vec3*pParticle->m_fCurSizeY;
											pParticle->m_fSizeStepY = (temp_vec2/temp_vec1*pParticle->m_fCurSizeY - pParticle->m_fCurSizeY)/(point2 - point1);
										}
									}
									break;
								}
								else if (pParticle->m_fAge <= point1)
								{
									pParticle->m_fSizeTimeX = point1;
									F32 temp_vec1;
									if(mDataBlock->m_pParticleSystem->GetSizePointXStep(i2, &temp_vec1))
									{
										pParticle->m_fSizeStepX = (temp_vec1 - pParticle->m_fCurSizeX)/(point1 - pParticle->m_fAge);
									}
									if (!mDataBlock->m_pParticleSystem->GetSizeRandomBothXAndY())
									{
										pParticle->m_fSizeStepY = (temp_vec1/pParticle->m_fCurSizeX*pParticle->m_fCurSizeY - pParticle->m_fCurSizeY)/(point1 - pParticle->m_fAge);
									}
									break;
								}
								else
								{
									continue;
								}
							}
							else
							{
								if(pParticle->m_fAge <= point1)
								{
									pParticle->m_fSizeTimeX = point1;
									F32 temp_vec,temp_vec1,temp_vec2;
									if(mDataBlock->m_pParticleSystem->GetSizePointXStep(i2, &temp_vec))
										pParticle->m_fSizeStepX = (temp_vec - pParticle->m_fCurSizeX)/(point1 - pParticle->m_fAge);
									if (!mDataBlock->m_pParticleSystem->GetSizeRandomBothXAndY())
									{
										if(mDataBlock->m_pParticleSystem->GetSizePointY(i2, &temp_vec1))
										{
											mDataBlock->m_pParticleSystem->GetSizePointX(i2, &temp_vec2);
											pParticle->m_fSizeStepY = (temp_vec/temp_vec2*temp_vec1 - pParticle->m_fCurSizeY)/(point1 - pParticle->m_fAge);
										}
									}
								}
								else
								{
									pParticle->m_fSizeStepX = 0.0f;
									pParticle->m_fSizeTimeX = MAX_TIME;
									F32 temp_vec,temp_vec1;
									if(mDataBlock->m_pParticleSystem->GetSizePointXStep(i2, &temp_vec))
										pParticle->m_fCurSizeX = temp_vec;
									if (!mDataBlock->m_pParticleSystem->GetSizeRandomBothXAndY())
									{
										pParticle->m_fSizeStepY = 0.0f;
										pParticle->m_fSizeTimeY = MAX_TIME;
										if(mDataBlock->m_pParticleSystem->GetSizePointY(i2, &temp_vec))
										{
											mDataBlock->m_pParticleSystem->GetSizePointX(i2, &temp_vec1);
											pParticle->m_fCurSizeY = pParticle->m_fCurSizeX/temp_vec1*temp_vec;
										}
									}
								}
								break;
							}
						}
						else
						{
							pParticle->m_fSizeStepX = 0.0f;
							pParticle->m_fSizeTimeX = MAX_TIME;
							if (!mDataBlock->m_pParticleSystem->GetSizeRandomBothXAndY())
							{
								pParticle->m_fSizeStepY = 0.0f;
								pParticle->m_fSizeTimeY = MAX_TIME;
							}
							break;
						}
					}
				}
				if(pParticle->m_fSizeStepX != 0.0f)
				{
					pParticle->m_fCurSizeX += pParticle->m_fSizeStepX * TIME_PER_FRAME;
					if (pParticle->m_fCurSizeX < 0.0f)
					{
						pParticle->m_fCurSizeX = 0.0f;
					}
				}

				if(mDataBlock->m_pParticleSystem->GetSizeRandomBothXAndY())
				{
					if(pParticle->m_fSizeTimeY < pParticle->m_fAge)
					{
						for(S32 i2 = 0; ; i2++)
						{
							F32 point1,point2;
							if (mDataBlock->m_pParticleSystem->GetSizeYTimePoint(i2, &point1))
							{
								if (mDataBlock->m_pParticleSystem->GetSizeYTimePoint(i2 + 1, &point2))
								{
									if((pParticle->m_fAge > point1) && (pParticle->m_fAge <= point2))
									{
										pParticle->m_fSizeTimeY = point2;
										F32 temp_vec1, temp_vec2;
										if(mDataBlock->m_pParticleSystem->GetSizePointYStep(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetSizePointYStep(i2 + 1, &temp_vec2))
										{
											pParticle->m_fSizeStepY = (temp_vec2 - temp_vec1)/(point2 - point1);
											pParticle->m_fCurSizeY = temp_vec1;
										}
										break;
									}
									else if (pParticle->m_fAge <= point1)
									{
										pParticle->m_fSizeTimeY = point1;
										F32 temp_vec1;
										if(mDataBlock->m_pParticleSystem->GetSizePointYStep(i2, &temp_vec1))
										{
											pParticle->m_fSizeStepY = (temp_vec1 - pParticle->m_fCurSizeY)/(point1 - pParticle->m_fAge);
										}
										break;
									}
									else
									{
										continue;
									}
								}
								else
								{
									if(pParticle->m_fAge <= point1)
									{
										pParticle->m_fSizeTimeY = point1;
										F32 temp_vec;
										if(mDataBlock->m_pParticleSystem->GetSizePointYStep(i2, &temp_vec))
											pParticle->m_fSizeStepY = (temp_vec - pParticle->m_fCurSizeY)/(point1 - pParticle->m_fAge);
									}
									else
									{
										pParticle->m_fSizeStepY = 0.0f;
										pParticle->m_fSizeTimeY = MAX_TIME;
										F32 temp_vec;
										if(mDataBlock->m_pParticleSystem->GetSizePointYStep(i2, &temp_vec))
											pParticle->m_fCurSizeY = temp_vec;
									}
									break;
								}
							}
							else
							{
								pParticle->m_fSizeStepY = 0.0f;
								pParticle->m_fSizeTimeY = MAX_TIME;
								break;
							}
						}
					}
				}
				if(pParticle->m_fSizeStepY != 0.0f)
				{
					pParticle->m_fCurSizeY += pParticle->m_fSizeStepY * TIME_PER_FRAME;
					if (pParticle->m_fCurSizeY < 0.0f)
					{
						pParticle->m_fCurSizeY = 0.0f;
					}
				}

				Point3F offset;
				D3DXVECTOR3 temp_vec;
				F32 temp_velocity = (pParticle->m_fVelocity - m_fCurRepulseVelocity) * TIME_PER_FRAME;
				if(!mDataBlock->m_pParticleSystem->GetModelEmit())
				{
					D3DXMATRIX temp_mat;
					D3DXMatrixRotationYawPitchRoll(&temp_mat, pParticle->m_vEula.y, pParticle->m_vEula.x, pParticle->m_vEula.z);
					temp_vec = D3DXVECTOR3(0.0f, temp_velocity, 0.0f);
					D3DXVec3TransformNormal(&temp_vec, &temp_vec, &temp_mat);
					memcpy(&offset, &temp_vec, 12);
				}
				else
				{
					offset = pParticle->m_vEula * temp_velocity;
				}

				pParticle->m_CurPos += offset;

#ifndef SXZPARTSYS
				MatrixF tempcamView = gClientSceneGraph->getLastWorld2ViewMX();
#else
				MatrixF tempcamView = getCameraMatrix();
				tempcamView.transpose();  // inverse - this gets the particles facing camera
#endif			

				if (UseBillboard)
				{
					bool tail = mDataBlock->m_pParticleSystem->GetHaveTail();
					if(tail || (!tail && m_bRotation))
					{
						if(!tail)
						{
							EulerF oo = m_vRotationMX.toEuler();
							D3DXMATRIX temp_mat;
							D3DXMatrixRotationYawPitchRoll(&temp_mat, oo.z, oo.x, oo.y);
							temp_vec = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
							D3DXVec3TransformNormal(&temp_vec, &temp_vec, &temp_mat);
							memcpy(&offset, &temp_vec, 12);
						}

						tempcamView.mulV(offset, (Point3F *)&temp_vec);
						D3DXVec3TransformNormal(&temp_vec, &temp_vec, &matReflect);
						if ((temp_vec.x >= 0.0f))
						{
							if(temp_vec.z >= 0.0f)
								pParticle->Rotate.m_fAngle = mAtan(temp_vec.x, temp_vec.z) - 0.5f * D3DX_PI; 
							else
								pParticle->Rotate.m_fAngle = mAtan(-temp_vec.z, temp_vec.x);
						}
						else 
						{
							if(temp_vec.z >= 0.0f)
								pParticle->Rotate.m_fAngle = mAtan(temp_vec.z, -temp_vec.x) + 1.0f * D3DX_PI; 
							else
								pParticle->Rotate.m_fAngle = mAtan(-temp_vec.x, -temp_vec.z) + 0.5f * D3DX_PI;
						}
						//pParticle->Rotate.m_fAngle += D3DX_PI * 0.5f;
					}
				}

				if(pParticle->m_fColorTime < pParticle->m_fAge)
				{
					for(S32 i2 = 0; ; i2++)
					{
						F32 point1, point2;
						if (mDataBlock->m_pParticleSystem->GetColorTimePoint(i2, &point1))
						{
							if (mDataBlock->m_pParticleSystem->GetColorTimePoint(i2 + 1, &point2))
							{
								if((pParticle->m_fAge > point1) && (pParticle->m_fAge <= point2))
								{
									pParticle->m_fColorTime = point2;
									ColorF temp_vec1, temp_vec2;
									if(mDataBlock->m_pParticleSystem->GetColorPointStep(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetColorPointStep(i2 + 1, &temp_vec2))
									{
										pParticle->m_ColorStep = (temp_vec2 - temp_vec1)/(point2 - point1);
										F32 tempalpha = pParticle->m_Color.alpha;
										pParticle->m_Color = temp_vec1;
										pParticle->m_Color.alpha = tempalpha;  
									}
									break;
								}
								else if (pParticle->m_fAge <= point1)
								{
									pParticle->m_fColorTime = point1;
									ColorF temp_vec1;
									if(mDataBlock->m_pParticleSystem->GetColorPointStep(i2, &temp_vec1))
									{
										pParticle->m_ColorStep = (temp_vec1 - pParticle->m_Color)/(point1 - pParticle->m_fAge);
									}
									break;
								}
								else
								{
									continue;
								}
							}
							else
							{
								if(pParticle->m_fAge > point1)
								{
									pParticle->m_ColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
									pParticle->m_fColorTime = MAX_TIME;
									ColorF temp_vec1;
									if(mDataBlock->m_pParticleSystem->GetColorPointStep(i2, &temp_vec1))
									{
										F32 tempalpha = pParticle->m_Color.alpha;
										pParticle->m_Color = temp_vec1;
										pParticle->m_Color.alpha = tempalpha;  
									}
								}
								else
								{
									pParticle->m_fColorTime = point1;
									ColorF temp_vec1;
									if(mDataBlock->m_pParticleSystem->GetColorPointStep(i2, &temp_vec1))
									{
										pParticle->m_ColorStep = (temp_vec1 - pParticle->m_Color)/(point1 - pParticle->m_fAge);
									}
								}
								break;
							}
						}
						else
						{
							pParticle->m_ColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
							pParticle->m_fColorTime = MAX_TIME;
							break;
						}
					}
				}
				if((pParticle->m_ColorStep.red != 0.0f) || (pParticle->m_ColorStep.green != 0.0f)|| (pParticle->m_ColorStep.blue != 0.0f))
				{
					F32 tempalpha = pParticle->m_Color.alpha;
					pParticle->m_Color += pParticle->m_ColorStep * TIME_PER_FRAME;
					pParticle->m_Color.alpha = tempalpha;
					pParticle->m_Color.clamp();
				}

				if(pParticle->m_fAlphaTime < pParticle->m_fAge)
				{
					for(S32 i2 = 0; ; i2++)
					{
						F32 point1, point2;
						if (mDataBlock->m_pParticleSystem->GetAlphaTimePoint(i2, &point1))
						{
							if (mDataBlock->m_pParticleSystem->GetAlphaTimePoint(i2 + 1, &point2))
							{
								if((pParticle->m_fAge > point1) && (pParticle->m_fAge <= point2))
								{
									pParticle->m_fAlphaTime = point2;
									F32 temp_vec1, temp_vec2;
									if(mDataBlock->m_pParticleSystem->GetAlphaPoint(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetAlphaPoint(i2 + 1, &temp_vec2))
									{
										pParticle->m_fAlphaStep = (temp_vec2 - temp_vec1)/(point2 - point1);
										pParticle->m_Color.alpha = temp_vec1;
									}
									break;
								}
								else if (pParticle->m_fAge <= point1)
								{
									F32 temp_vec1;
									if(mDataBlock->m_pParticleSystem->GetAlphaPoint(i2, &temp_vec1))
									{
										pParticle->m_fAlphaStep = (temp_vec1 - pParticle->m_Color.alpha)/(point1 - pParticle->m_fAge);
									}
									pParticle->m_fAlphaTime = point1;
									break;
								}
								else
								{
									continue;
								}
							}
							else
							{
								if(pParticle->m_fAge > point1)
								{
									F32 temp_vec1;
									if(mDataBlock->m_pParticleSystem->GetAlphaPoint(i2, &temp_vec1))
									{
										pParticle->m_fAlphaTime = MAX_TIME;
										pParticle->m_fAlphaStep = 0.0f;
										pParticle->m_Color.alpha = temp_vec1;
									}
								}
								else
								{
									F32 temp_vec1;
									if(mDataBlock->m_pParticleSystem->GetAlphaPoint(i2, &temp_vec1))
									{
										pParticle->m_fAlphaStep = (temp_vec1 - pParticle->m_Color.alpha)/(point1 - pParticle->m_fAge);
									}
									pParticle->m_fAlphaTime = point1;
								}
								break;
							}
						}
						else
						{
							pParticle->m_fAlphaStep = 0.0f;
							pParticle->m_fAlphaTime = MAX_TIME;
							break;
						}
					}
				}
				if(pParticle->m_fAlphaStep != 0.0f)
				{
					pParticle->m_Color.alpha += pParticle->m_fAlphaStep * TIME_PER_FRAME;
					if (pParticle->m_Color.alpha > 1.0f)
					{
						pParticle->m_Color.alpha = 1.0f;
					}
					else if (pParticle->m_Color.alpha < 0.0f)
					{
						pParticle->m_Color.alpha = 0.0f;
					}
				}

				if(UseBillboard)
				{
					if(!mDataBlock->m_pParticleSystem->GetHaveTail() && !m_bRotation)
					{
						if(pParticle->m_fRotTime < pParticle->m_fAge)
						{
							for(S32 i2 = 0; ; i2++)
							{
								F32 point1, point2;
								if (mDataBlock->m_pParticleSystem->GetAngleTimePoint(i2, &point1))
								{
									if (mDataBlock->m_pParticleSystem->GetAngleTimePoint(i2 + 1, &point2))
									{
										if((pParticle->m_fAge > point1) && (pParticle->m_fAge <= point2))
										{
											pParticle->m_fRotTime = point2;
											F32 temp_vec;
											if(mDataBlock->m_pParticleSystem->GetAnglePointStep(i2, &temp_vec))
												pParticle->RotateStep.m_fAngleStep = temp_vec;
											break;
										}
										else if (pParticle->m_fAge <= point1)
										{
											pParticle->m_fRotTime = point1;
											break;
										}
										else
										{
											continue;
										}
									}
									else
									{
										if(pParticle->m_fAge > point1)
										{
											F32 temp_vec;
											if(mDataBlock->m_pParticleSystem->GetAnglePointStep(i2, &temp_vec))
											{
												pParticle->RotateStep.m_fAngleStep = temp_vec;
											}
											pParticle->m_fRotTime = MAX_TIME;
										}
										break;
									}
								}
								else
								{
									pParticle->m_fRotTime = MAX_TIME;
									break;
								}
							}
						}
						if(!pParticle->m_bRotCCW)
						{
							if(pParticle->RotateStep.m_fAngleStep != 0.0f)
								pParticle->Rotate.m_fAngle += TIME_PER_FRAME * pParticle->RotateStep.m_fAngleStep;
						}
						else
						{
							if(pParticle->RotateStep.m_fAngleStep != 0.0f)
								pParticle->Rotate.m_fAngle -= TIME_PER_FRAME * pParticle->RotateStep.m_fAngleStep;
						}
					}
					else
					{

					}
				}
				else
				{
					if(pParticle->m_fRotTime < pParticle->m_fAge)
					{
						for(S32 i2 = 0; ; i2++)
						{
							F32 point1, point2; 
							if (mDataBlock->m_pParticleSystem->GetRotDeviationTimePoint(i2, &point1))
							{
								if (mDataBlock->m_pParticleSystem->GetRotDeviationTimePoint(i2 + 1, &point2))
								{
									if((pParticle->m_fAge > point1) && (pParticle->m_fAge <= point2))
									{
										pParticle->m_fRotTime = point2;
										Point3F temp_vec;
										if(mDataBlock->m_pParticleSystem->GetRotDeviationStep(i2, &temp_vec))
										{
											pParticle->RotateStep.m_fRotStep[0] = temp_vec.x;
											pParticle->RotateStep.m_fRotStep[1] = temp_vec.y;
											pParticle->RotateStep.m_fRotStep[2] = temp_vec.z;
										}
										break;
									}
									else if(pParticle->m_fAge <= point1)
									{
										pParticle->m_fRotTime = point1;
										break;
									}
									else
									{
										continue;
									}
								}
								else
								{
									if(pParticle->m_fAge > point1)
									{
										Point3F temp_vec;
										if(mDataBlock->m_pParticleSystem->GetRotDeviationStep(i2, &temp_vec))
										{
											pParticle->RotateStep.m_fRotStep[0] = temp_vec.x;
											pParticle->RotateStep.m_fRotStep[1] = temp_vec.y;
											pParticle->RotateStep.m_fRotStep[2] = temp_vec.z;
										}
										pParticle->m_fRotTime = MAX_TIME;
									}
									break;
								}
							}
							else
							{
								pParticle->m_fRotTime = MAX_TIME;
								break;
							}
						}
					}
					if((pParticle->RotateStep.m_fRotStep[0] != 0.0f) || (pParticle->RotateStep.m_fRotStep[1] != 0.0f) || (pParticle->RotateStep.m_fRotStep[2] != 0.0f))
					{
						pParticle->Rotate.m_fRot[0] += TIME_PER_FRAME * pParticle->RotateStep.m_fRotStep[0];
						pParticle->Rotate.m_fRot[1] += TIME_PER_FRAME * pParticle->RotateStep.m_fRotStep[1];
						pParticle->Rotate.m_fRot[2] += TIME_PER_FRAME * pParticle->RotateStep.m_fRotStep[2];
					}
				}

				if(bTexSplinter)
				{
					pParticle->m_fCurTexRemainTime += TIME_PER_FRAME;
					if(pParticle->m_fCurTexRemainTime > texremaintime)
					{
						if (bTexSplinterBroadRandom)
						{
							pParticle->m_iTextureSplinterCode = mRandGen.randI(0, splinternum - 1);
						}
						else
						{
							if(pParticle->m_iTextureSplinterCode < splinternum - 1)
							{
								pParticle->m_iTextureSplinterCode += 1;
							}
							else
							{
								pParticle->m_iTextureSplinterCode = 0;
							}
						}
						pParticle->m_fCurTexRemainTime = 0.0f;
					}
				}
				else
				{
				}

				if(pParticle->m_fGravityTime < pParticle->m_fAge)
				{
					for(S32 i2 = 0; ; i2++)
					{
						F32 point1, point2;
						if (mDataBlock->m_pParticleSystem->GetGravityTimePoint(i2, &point1))
						{
							if (mDataBlock->m_pParticleSystem->GetGravityTimePoint(i2 + 1, &point2))
							{
								if((pParticle->m_fAge > point1) && (pParticle->m_fAge <= point2))
								{
									F32 temp_vec1, temp_vec2;
									if(mDataBlock->m_pParticleSystem->GetGravityStep(i2, &temp_vec1) && mDataBlock->m_pParticleSystem->GetGravityStep(i2 + 1, &temp_vec2))
									{
										pParticle->m_fGravityStep = (temp_vec2 - temp_vec1)/(point2 - point1);
										pParticle->m_fGravity = temp_vec1;
									}
									pParticle->m_fGravityTime = point2;
									break;
								}
								else if(pParticle->m_fAge <= point1)
								{
									pParticle->m_fGravityTime = point1;
									F32 temp_vec;
									if(mDataBlock->m_pParticleSystem->GetGravityStep(i2, &temp_vec))
										pParticle->m_fGravityStep = (temp_vec - pParticle->m_fGravity)/(point1 - pParticle->m_fAge);
									break;
								}
								else
								{
									continue;
								}
							}
							else
							{
								if(pParticle->m_fAge <= point1)
								{
									pParticle->m_fGravityTime = point1;
									F32 temp_vec;
									if(mDataBlock->m_pParticleSystem->GetGravityStep(i2, &temp_vec))
										pParticle->m_fGravityStep = (temp_vec - pParticle->m_fGravity)/(point1 - pParticle->m_fAge);
								}
								else
								{
									pParticle->m_fGravityTime = MAX_TIME;
									pParticle->m_fGravityStep = 0.0f;
									F32 temp_vec;
									if(mDataBlock->m_pParticleSystem->GetGravityStep(i2, &temp_vec))
										pParticle->m_fGravity = temp_vec;
								}
								break;
							}
						}
						else
						{
							pParticle->m_fGravityTime = MAX_TIME;
							break;
						}
					}
				}
				if(pParticle->m_fGravityStep != 0.0f)
					pParticle->m_fGravity += pParticle->m_fGravityStep * TIME_PER_FRAME;
				pParticle->m_CurPos.z += (-TIME_PER_FRAME) * pParticle->m_fGravity * pParticle->m_fAge;

				if(pParticle->m_fFlickerTime > 0.0f)
				{
					pParticle->m_fCurFlickerTime += TIME_PER_FRAME;
					if(pParticle->m_fCurFlickerTime > pParticle->m_fFlickerTime) 
					{
						if (pParticle->m_bDraw)
						{
							pParticle->m_fFlickerTime = mDataBlock->m_pParticleSystem->GetRandomFlickerInterval();
						}
						else
						{
							pParticle->m_fFlickerTime = mDataBlock->m_pParticleSystem->GetRandomFlickerTime();
						}
						pParticle->m_bDraw = !pParticle->m_bDraw;
						pParticle->m_fCurFlickerTime = 0.0f;
					}
				}
			}
			pParticle = temp_pp;
		}

		Particle* pParticle1;
		if(!m_bPause)
		{
			m_fCurEmitTimeInterval += TIME_PER_FRAME;
			if(m_fCurEmitTimeInterval > m_fEmitTimeInterval)
			{
				for(S32 i2 = 0; ; i2++)
				{
					F32 point1, point2;
					if (mDataBlock->m_pParticleSystem->GetEmitStateTimePoint(i2, &point1))
					{
						if (mDataBlock->m_pParticleSystem->GetEmitStateTimePoint(i2 + 1, &point2))
						{
							if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
							{
								bool temp_vec;
								if(mDataBlock->m_pParticleSystem->GetEmitState(i2, &temp_vec))
									m_bEmit = temp_vec;
								break;
							}
							else if (m_fCurEmitTime <= point1)
							{
								break;
							}
							else
							{
								continue;
							}
						}
						else
						{
							if(m_fCurEmitTime > point1)
							{
								bool temp_vec;
								if(mDataBlock->m_pParticleSystem->GetEmitState(i2, &temp_vec))
									m_bEmit = temp_vec;
							}
							break;
						}
					}
					else
					{
						break;
					}
				}

				for(S32 i2 = 0; ; i2++)
				{
					F32 point1, point2;
					if(mDataBlock->m_pParticleSystem->GetEmitTimeIntervalTimePoint(i2, &point1))
					{
						if(mDataBlock->m_pParticleSystem->GetEmitTimeIntervalTimePoint(i2 + 1, &point2))
						{
							if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
							{
								F32 temp_vec;
								if(mDataBlock->m_pParticleSystem->GetRandomEmitTimeInterval(i2, &temp_vec))
									m_fEmitTimeInterval = temp_vec;
								break;
							}
							else if (m_fCurEmitTime <= point1)
							{
								break;
							}
							else
							{
								continue;
							}
						}
						else
						{
							if(m_fCurEmitTime > point1) 
							{
								F32 temp_vec;
								if(mDataBlock->m_pParticleSystem->GetRandomEmitTimeInterval(i2, &temp_vec))
									m_fEmitTimeInterval = temp_vec;
							}
							break;
						}
					}
					else
					{
						break;
					}
				}

				if(m_bEmit)
				{
					S32 iPerEmitNum = mDataBlock->m_pParticleSystem->GetPerEmitNum();
					S32 MaxNum = mDataBlock->m_pParticleSystem->GetMaxParticleNum(); 
					for(S32 i2 = 0; i2 < iPerEmitNum; i2++)
					{
						if(m_iCurParticleNum < MaxNum)
						{
							if ( m_pParticleFreeList )
							{
								pParticle1 = m_pParticleFreeList;
							}
							else
							{
								pParticle1 = new Particle[16];
								m_vecParticle.push_back(pParticle1);
								for (S32 i = 0; i < 16; i++)
								{
									pParticle1[i].m_pPrev = (i == 0) ? NULL : &pParticle1[i - 1];
									pParticle1[i].m_pNext = (i == 15) ? NULL : &pParticle1[i + 1];	 
								}
							}
							m_pParticleFreeList = pParticle1->m_pNext;
							pParticle1->m_pNext = m_pParticleList;
							pParticle1->m_pPrev = NULL;
							if(m_pParticleList)
								m_pParticleList->m_pPrev = pParticle1;
							m_pParticleList = pParticle1;
							m_iCurParticleNum += 1;
							initializeParticle(pParticle1);
						}
						else
						{
							break;
						}
					}
				}
				m_fCurEmitTimeInterval = 0.0f;
			}
		}

		if((m_bPause) && (m_iCurParticleNum == 0))
		{
			Destory();
			m_iCurParticleNum = -1;
		}
	}

	if(mDataBlock->m_bSort)
		Sort();
}

void ParticleEmitter::renderObject( SceneState *state, RenderInst *ri )
{
	GFX->setCullMode( GFXCullNone );
	GFX->setZWriteEnable( false );
	GFX->setZEnable( true );
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
	S32 temp_s32 = (ri->texWrapFlags - 1) % 4;
	if(temp_s32 == 0)
	{
		GFX->setSrcBlend(GFXBlendSrcAlpha);
		GFX->setDestBlend(GFXBlendInvSrcAlpha);
	}
	else if (temp_s32 == 1)
	{
		GFX->setSrcBlend(GFXBlendZero);
		GFX->setDestBlend(GFXBlendInvSrcColor);
	}
	else if (temp_s32 == 2)
	{
		GFX->setSrcBlend(GFXBlendOne);
		GFX->setDestBlend(GFXBlendInvSrcAlpha);
	}
	else if (temp_s32 == 3)
	{
		GFX->setSrcBlend(GFXBlendSrcAlpha);
		GFX->setDestBlend(GFXBlendOne);
	}
	else
	{

	}

	bool reEnableLighting = GFX->getRenderState( GFXRSLighting );

	GFX->setLightingEnable( false );
	GFX->setAlphaBlendEnable( true );

	MatrixF mat;
	mat = (*ri->objXform) * (*ri->worldXform);

	if(!mDataBlock->m_pParticleSystem->GetElectricityState())
	{
		static bool bfind = false;
		static ShaderData *sgParticleRender = NULL; 
		if(!bfind)
		{
			if ( !Sim::findObject( "ParticleRender", sgParticleRender ) )
				Con::warnf("ParticleRender - failed to locate ParticleRender shader ParticleRenderBlendData!");
			bfind = true;
		}
		GFX->setShader(sgParticleRender->getShader());

		mat.transpose();
		GFX->setVertexShaderConstF(0, (float*)&mat, 4);
		ColorF temp_point4f = ri->maxBlendColor;
		GFX->setPixelShaderConstF(0, (float*)&temp_point4f, 1);
		Point4F temp_p4 = Point4F(0.0f, 0.0f ,0.0f, 0.0f);
		temp_p4.x = ri->visibility;
		GFX->setPixelShaderConstF(1, (float*)&temp_p4, 1);
		temp_p4.x = m_fEmitterAlpha;
		GFX->setPixelShaderConstF(2, (float*)&temp_p4, 1);
		GFX->setTexture( 0, ri->miscTex );
	}
	else
	{
		static bool bfind = false;
		static ShaderData *sgParticleRender = NULL; 
		if(!bfind)
		{
			if ( !Sim::findObject( "ParticleElectricity", sgParticleRender ) )
				Con::warnf("ParticleRender - failed to locate ParticleRender shader ParticleRenderBlendData!");
			bfind = true;
		}
		GFX->setShader(sgParticleRender->getShader());

		GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
		GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
		GFX->setTextureStageAddressModeW(0, GFXAddressWrap);

		mat.transpose();
		GFX->setVertexShaderConstF(0, (float*)&mat, 4);
		Point4F temp_p4 = Point4F(0.0f, 0.0f ,0.0f, 0.0f);
		temp_p4.x = m_fEmitterAlpha;
		GFX->setPixelShaderConstF(1, (float*)&temp_p4, 1);

		static float p_time = 0.0f;
		p_time += float(TimeManager::getDeltaTime()) * 0.001f;
		temp_p4 = Point4F(p_time, 0.0f ,0.0f, 0.0f);
		GFX->setPixelShaderConstF(0, (float*)&temp_p4, 1);

		Point4F cons = Point4F(mDataBlock->m_pParticleSystem->GetGlowStrength(), 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(2, cons, 1);
		cons = Point4F(mDataBlock->m_pParticleSystem->GetHeight(), 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(3, cons, 1);
		cons = Point4F(mDataBlock->m_pParticleSystem->GetGlowFallOff(), 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(4, cons, 1);
		cons = Point4F(mDataBlock->m_pParticleSystem->GetSpeed(), 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(5, cons, 1);
		cons = Point4F(mDataBlock->m_pParticleSystem->GetSampleDist(), 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(6, cons, 1);
		cons = Point4F(mDataBlock->m_pParticleSystem->GetAmbientGlow(), 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(7, cons, 1);
		cons = Point4F(mDataBlock->m_pParticleSystem->GetAmbientGlowHeightScale(), 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(8, cons, 1);
		cons = Point4F(mDataBlock->m_pParticleSystem->GetVertNoise(), 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(9, cons, 1);

		GFX->SetVolumeTexture(0, mDataBlock->m_pParticleSystem->GetVolumeTexture());
	}

	GFX->setPrimitiveBuffer( *ri->primBuff );
	GFX->setVertexBuffer( *ri->vertBuff );

	GFX->drawIndexedPrimitive( GFXTriangleList, 0, ri->primBuffIndex * 4, 0, ri->primBuffIndex * 2 );

	GFX->disableShaders();

	//GFX->popWorldMatrix();
	GFX->setLightingEnable( reEnableLighting );
	GFX->setZEnable( true );

	if(mDataBlock->m_pParticleSystem->GetElectricityState())
	{
		GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMipFilter(0, GFXTextureFilterNone);
		GFX->SetVolumeTexture(0, NULL);
		GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
		GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
		GFX->setTextureStageAddressModeW(0, GFXAddressClamp);
	}
}

bool ParticleEmitter::prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState)
{
	if( isLastState(state, stateKey) )
		return false;

	if(gClientSceneGraph->isReflectPass())
		return false;

	PROFILE_START(ParticleEmitter_prepRenderImage);

	setLastState(state, stateKey);

	// This should be sufficient for most objects that don't manage zones, and
	//  don't need to return a specialized RenderImage...
	if( state->isObjectRendered(this) )
	{
		prepBatchRender( state->getCameraPosition() );
	}

	PROFILE_END(ParticleEmitter_prepRenderImage);

	return false;
}

void ParticleEmitter::advanceTime(F32 dt)
{
	ParticleParam *pParticleParamData = new ParticleParam;
	pParticleParamData->dt = dt;
	pParticleParamData->pEmitter = this;

#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(SXZPARTSYS))

	if(g_MultThreadWorkMgr->isEnable())
	{
		//Ray: 加入多线程处理列表，本帧不做渲染
		stThreadParam *pThreadData = new stThreadParam;
		pThreadData->pParam = pParticleParamData;
		pThreadData->pFunc = doParticleRoutine;
		pThreadData->pObj = this;

		g_MultThreadWorkMgr->addInProcessList(pThreadData);
	}
	else
		doParticleRoutine(pParticleParamData);
#else
	doParticleRoutine(pParticleParamData);
#endif
}

void ParticleEmitter::doParticleRoutine(void *pParam)
{
	ParticleParam *pParticleParamData = (ParticleParam *)pParam;
	pParticleParamData->pEmitter->doAdvanceTime(pParticleParamData->dt);
	if(pParticleParamData->pEmitter->mFillVB)
	{
		pParticleParamData->pEmitter->copyToVB();
		pParticleParamData->pEmitter->mFillVB = false;
	}
	delete pParticleParamData;
}

void ParticleEmitter::prepBatchRender( const Point3F& camPos )
{
	if(m_iCurParticleNum <= 0)
		return;

#ifndef SXZPARTSYS
		mLastCamView = gClientSceneGraph->getLastWorld2ViewMX();
#else
		mLastCamView = getCameraMatrix();
#endif

#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(SXZPARTSYS))
    if(g_MultThreadWorkMgr->isEnable())
    {
		mFillVB = true;
	    if(!mpVertBuff->getPointer() || mpVertBuff->getPointer()->isNull()) //Ray: 没有做填充的VB不做渲染
		    return;
    }
    else
	{
        copyToVB();
	}
#else
	copyToVB();
#endif


#ifndef SXZPARTSYS
	RenderInst *ri = gRenderInstManager.allocInst();
	ri->vertBuff = mpVertBuff;
	ri->primBuff = &primBuff;
	ri->matInst = NULL;
	ri->translucent = true;
	ri->type = RenderInstManager::RIT_Translucent;
	ri->calcSortPoint( this, camPos );
	ri->particles = RenderTranslucentMgr::RENDERTRANSLUCENT_TYPE_PARTICLE;
	ri->texWrapFlags = mDataBlock->m_pParticleSystem->GetTextureStageState();
	ri->obj = this;

	ri->worldXform = gRenderInstManager.allocXform();
	ri->objXform = gRenderInstManager.allocXform();

	*ri->worldXform = gClientSceneGraph->getLastWorld2ViewMX();
	*ri->objXform = GFX->getProjectionMatrix();  // The reason for storing off the projection matrix here is that the render translucent manager relied on having a valid projection matrix. This is not the case if the particles are rendered after a processedFFMaterial is used.

	ri->primBuffIndex = m_iRenderParticleNum;
	//   For particles, transFlags now contains both src and dest blend
	//   settings packed together. SrcBlend is in the upper 4 bits and
	//   DestBlend is in the lower 4.
	ri->transFlags = ((mDataBlock->srcBlendFactor << 4) & 0xf0) | (mDataBlock->dstBlendFactor & 0x0f);
	// use first particle's texture unless there is an emitter texture to override it
	
	ri->miscTex = &*(mDataBlock->m_pParticleSystem->GetTexture());
	Point3F temp_color = mDataBlock->m_pParticleSystem->GetSelfIllumination();
	ri->maxBlendColor = ColorF(temp_color.x, temp_color.y, temp_color.z, mDataBlock->m_pParticleSystem->GetGlow());
    ri->visibility = mDataBlock->m_pParticleSystem->GetPowValue();

	gRenderInstManager.addInst( ri );
#endif
}

void ParticleEmitter::copyToVB()
{
	PROFILE_START(ParticleEmitter_copyToVB);
	
	Vector<GFXVertexPCT> tempBuff(2048);
	tempBuff.reserve( m_iCurParticleNum*4 + 64); // make sure tempBuff is big enough
	GFXVertexPCT *buffPtr = tempBuff.address(); // use direct pointer (faster)

	S32 offset = 0;

	MatrixF temp_mat;
	temp_mat.identity();
	temp_mat.scale(m_vCurScale);
	Point3F basePoints[4], basePoints2[4];
	if(!mDataBlock->m_pParticleSystem->GetHalfCut())
	{
		basePoints2[0] = Point3F(-1.0, 0.0,  1.0);
		basePoints2[1] = Point3F( 1.0, 0.0,  1.0);
		basePoints2[2] = Point3F(-1.0, 0.0, -1.0);
		basePoints2[3] = Point3F( 1.0, 0.0, -1.0);
	}
	else
	{
		basePoints2[0] = Point3F( 0.0, 0.0,  1.0);
		basePoints2[1] = Point3F( 1.0, 0.0,  1.0);
		basePoints2[2] = Point3F( 0.0, 0.0, -1.0);
		basePoints2[3] = Point3F( 1.0, 0.0, -1.0);
	}
	temp_mat.mulV(basePoints2[0], &basePoints[0]);
	temp_mat.mulV(basePoints2[1], &basePoints[1]);
	temp_mat.mulV(basePoints2[2], &basePoints[2]);
	temp_mat.mulV(basePoints2[3], &basePoints[3]);

	MatrixF &camView = mLastCamView;
	camView.transpose();  // inverse - this gets the particles facing camera

	Particle* particle = m_pParticleList;
	m_iRenderParticleNum = 0;
	for (S32 i = 0; i < m_iCurParticleNum; i++)
	{
		if(!particle->m_bDraw)
		{
			particle = particle->m_pNext;
			continue;
		}
		SetParticleRenderData(buffPtr, particle, offset, basePoints, camView);
		offset += 4;
		particle = particle->m_pNext;
		m_iRenderParticleNum += 1;
	}

	PROFILE_START(ParticleEmitter_copyToVB_LockCopy);

    S32 maxnum = mDataBlock->m_pParticleSystem->GetMaxParticleNum();

    // create new VB if emitter size grows
    if(( !(*mpVertBuff2) ) || (m_iMaxParticleNum < maxnum))
    {
        if(m_iMaxParticleNum > maxnum)
            Reset();
#ifndef SXZPARTSYS
        mpVertBuff2->set( GFX, maxnum * 4, GFXBufferTypeDynamic );
#endif
    }
    m_iMaxParticleNum = maxnum;
#ifndef SXZPARTSYS
    // lock and copy tempBuff to video RAM
    GFXVertexPCT *verts = mpVertBuff2->lock();
    dMemcpy( verts, tempBuff.address(), m_iRenderParticleNum * 4 * sizeof(GFXVertexPCT) );
    mpVertBuff2->unlock();
#else
    mVBForMax.setSize( m_iRenderParticleNum * 4 );
    dMemcpy( mVBForMax.address(), tempBuff.address(), m_iRenderParticleNum * 4 * sizeof(GFXVertexPCT) );
#endif

	switchBuff();

	PROFILE_END(ParticleEmitter_copyToVB_LockCopy);
	PROFILE_END(ParticleEmitter_copyToVB);
}

void ParticleEmitter::InitSys()
{
	m_vCurEula = Point3F(0.0f, 0.0f, 0.0f);
	m_vEulaRand = Point3F(0.0f, 0.0f, 0.0f);
	m_vEulaStep = Point3F(0.0f, 0.0f, 0.0f);
	m_fEulaTime = 0.0f;
	m_fCurVelocity = 0.0f;
	m_fVelocityRand = 0.0f;
	m_fVelocityStep = 0.0f;
	m_fVelocityTime = 0.0f;
	m_fCurRepulseVelocity = 0.0f;
	m_fRepulseVelocityRand = 0.0f;
	m_fRepulseVelocityStep = 0.0f;
	m_fRepulseVelocityTime = 0.0f;
	m_fCurEmitTime = 0.0f;                  
	m_fEmitTimeInterval = 0.0f;
	m_fCurEmitTimeInterval = 0.0f;
	m_EmitterPos = Point3F(0.0f, 0.0f, 0.0f);                    
	m_vPosDeviation = Point3F(0.0f, 0.0f, 0.0f);
	m_vPosDeviationStep = Point3F(0.0f, 0.0f, 0.0f);
	m_fPosDeviationTime = 0.0f;
	m_bEmit = true;
	m_fElapsedTime = 0.0f;
	m_vCurScale = m_vScaleStep = Point3F(1.0f, 1.0f, 1.0f);
	m_fScaleTime = 0.0f;
}

void ParticleEmitter::SetPause(bool pause)
{
	m_bPause = pause;
	if ((!m_bPause) && (m_iCurParticleNum == -1))
	{
        InitSys();
		m_iCurParticleNum = 0;

		m_iMaxParticleNum = mDataBlock->m_pParticleSystem->GetMaxParticleNum();
		AssertFatal( m_iMaxParticleNum!=0, "Error, SetPause()" );
		Particle* temp_ptr = new Particle[m_iMaxParticleNum];
		m_pParticleFreeList = temp_ptr;
		m_vecParticle.push_back(temp_ptr);
		for (S32 i = 0; i < m_iMaxParticleNum; i++)
		{
			temp_ptr[i].m_pPrev = (i == 0) ? NULL : &temp_ptr[i - 1];
			temp_ptr[i].m_pNext = (i == m_iMaxParticleNum - 1) ? NULL : &temp_ptr[i + 1];	 
		}
	}
}

bool ParticleEmitter::GetPause()
{
	return m_bPause;
}

void ParticleEmitter::SetParticleSystem(ParticleEmitterData* system)
{
	if(system)
	{
		mDataBlock = system;
	}
}

ParticleEmitterData* ParticleEmitter::GetParticleSystem()
{
	return mDataBlock;
}

void ParticleEmitter::Destory()
{
	for (size_t i = 0; i < m_vecParticle.size(); i++)
	{
		DELS(m_vecParticle[i]);
	}
	m_vecParticle.clear();
	m_pParticleFreeList = NULL;
	m_pParticleList = NULL;
}

void ParticleEmitter::SetParticleRenderData(GFXVertexPCT* tempverx, Particle* particle, S32 offset, Point3F *basePts, MatrixF &camView)
{
	tempverx[offset].color = tempverx[offset + 1].color = tempverx[offset + 2].color = tempverx[offset + 3].color = particle->m_Color;

	if(mDataBlock->m_pParticleSystem->GetTextureSplinterState())
	{
		mDataBlock->m_pParticleSystem->GetTextureSplinterCoord(particle->m_iTextureSplinterCode, &tempverx[offset].texCoord, &tempverx[offset + 1].texCoord,
			&tempverx[offset + 2].texCoord, &tempverx[offset + 3].texCoord);
	}
	else
	{
		if(!mDataBlock->m_pParticleSystem->GetElectricityState())
		{
			tempverx[offset].texCoord = Point2F(0.0f, 0.0f);
			tempverx[offset + 1].texCoord = Point2F(1.0f, 0.0f);
			tempverx[offset + 2].texCoord = Point2F(0.0f, 1.0f);
			tempverx[offset + 3].texCoord = Point2F(1.0f, 1.0f);
		}
		else
		{
			tempverx[offset].texCoord = Point2F(-1.0f, -1.0f);
			tempverx[offset + 1].texCoord = Point2F(1.0f, -1.0f);
			tempverx[offset + 2].texCoord = Point2F(-1.0f, 1.0f);
			tempverx[offset + 3].texCoord = Point2F(1.0f, 1.0f);
		}
	}

	const S32 num = 4;
	if(!mDataBlock->m_pParticleSystem->GetBillboardState())
	{
        Point3F vec[4], vec1[4];

		if(!mDataBlock->m_pParticleSystem->GetHalfCut())
		{
			vec1[0].x = -particle->m_fCurSizeX;
			vec1[0].y = 0.0f;
			vec1[0].z = particle->m_fCurSizeY;

			vec1[1].x = particle->m_fCurSizeX;
			vec1[1].y = 0.0f;
			vec1[1].z = particle->m_fCurSizeY;

			vec1[2].x = -particle->m_fCurSizeX;
			vec1[2].y = 0.0f;
			vec1[2].z = -particle->m_fCurSizeY;

			vec1[3].x = particle->m_fCurSizeX;
			vec1[3].y = 0.0f;
			vec1[3].z = -particle->m_fCurSizeY;
		}
		else
		{
			vec1[0].x = 0.0f;
			vec1[0].y = 0.0f;
			vec1[0].z = particle->m_fCurSizeY;

			vec1[1].x = particle->m_fCurSizeX;
			vec1[1].y = 0.0f;
			vec1[1].z = particle->m_fCurSizeY;

			vec1[2].x = 0.0f;
			vec1[2].y = 0.0f;
			vec1[2].z = -particle->m_fCurSizeY;

			vec1[3].x = particle->m_fCurSizeX;
			vec1[3].y = 0.0f;
			vec1[3].z = -particle->m_fCurSizeY;
		}
		MatrixF temp_mat;
		temp_mat.identity();
		temp_mat.scale(m_vCurScale);
		temp_mat.mulV(vec1[0], &vec[0]);
		temp_mat.mulV(vec1[1], &vec[1]);
		temp_mat.mulV(vec1[2], &vec[2]);
		temp_mat.mulV(vec1[3], &vec[3]);

		MatrixF mat(true), matTrans1(true);

		if(mDataBlock->m_pParticleSystem->GetRotNum() > 0)
		{
			mat.set(EulerF(particle->Rotate.m_fRot[0],particle->Rotate.m_fRot[1],particle->Rotate.m_fRot[2]));
			MatrixF temp_mat = particle->m_matParent;
            temp_mat.setPosition(Point3F(0,0,0));
            mat = temp_mat * mat;
			mat.setPosition(Point3F(particle->m_vWorldPos.x, particle->m_vWorldPos.y, particle->m_vWorldPos.z));
		}
		else
		{
			mat.setPosition(Point3F(particle->m_vWorldPos.x, particle->m_vWorldPos.y, particle->m_vWorldPos.z));
		}

		mat.mulP(vec[0], &tempverx[offset].point);
		mat.mulP(vec[1], &tempverx[offset + 1].point);
		mat.mulP(vec[2], &tempverx[offset + 2].point);
		mat.mulP(vec[3], &tempverx[offset + 3].point);
	}
	else
	{
		if(mDataBlock->m_pParticleSystem->GetAngleNum() > 0)
		{
		    camView.setPosition(particle->m_vWorldPos);

			float s, c, x, z;
			mSinCos( particle->Rotate.m_fAngle, s, c );

			x = basePts->x * particle->m_fCurSizeX;  
			z = basePts->z * particle->m_fCurSizeY;
			tempverx[offset].point.x = x * c + z * s;
			tempverx[offset].point.y = 0.0f;
			tempverx[offset].point.z = -x * s + z * c;
			basePts++;

			x = basePts->x * particle->m_fCurSizeX;  
			z = basePts->z * particle->m_fCurSizeY;
			tempverx[offset+1].point.x = x * c + z * s;
			tempverx[offset+1].point.y = 0.0f;
			tempverx[offset+1].point.z = -x * s + z * c;
			basePts++;

			x = basePts->x * particle->m_fCurSizeX;  
			z = basePts->z * particle->m_fCurSizeY;
			tempverx[offset+2].point.x = x * c + z * s;
			tempverx[offset+2].point.y = 0.0f;
			tempverx[offset+2].point.z = -x * s + z * c;
			basePts++;

			x = basePts->x * particle->m_fCurSizeX;  
			z = basePts->z * particle->m_fCurSizeY;
			tempverx[offset+3].point.x = x * c + z * s;
			tempverx[offset+3].point.y = 0.0f;
			tempverx[offset+3].point.z = -x * s + z * c;
			basePts++;

			camView.mulP( tempverx[offset].point );      
			camView.mulP( tempverx[offset + 1].point ); 
			camView.mulP( tempverx[offset + 2].point );      
			camView.mulP( tempverx[offset + 3].point );             
		}
		else
		{
			camView.setPosition(particle->m_vWorldPos);

			tempverx[offset].point.x = basePts->x * particle->m_fCurSizeX;  
			tempverx[offset].point.y = 0.0f;                                
			tempverx[offset].point.z = basePts->z * particle->m_fCurSizeY;  
			camView.mulP( tempverx[offset].point );                        			                      
			                
			++basePts;
			tempverx[offset + 1].point.x = basePts->x * particle->m_fCurSizeX;  
			tempverx[offset + 1].point.y = 0.0f;                                
			tempverx[offset + 1].point.z = basePts->z * particle->m_fCurSizeY;  
			camView.mulP( tempverx[offset + 1].point );                        				                 
			 
			++basePts;
			tempverx[offset + 2].point.x = basePts->x * particle->m_fCurSizeX;  
			tempverx[offset + 2].point.y = 0.0f;                                
			tempverx[offset + 2].point.z = basePts->z * particle->m_fCurSizeY;  
			camView.mulP( tempverx[offset + 2].point );                        				                             
			
			++basePts;
			tempverx[offset + 3].point.x = basePts->x * particle->m_fCurSizeX;  
			tempverx[offset + 3].point.y = 0.0f;
			tempverx[offset + 3].point.z = basePts->z * particle->m_fCurSizeY;  
			camView.mulP( tempverx[offset + 3].point );
		}
	}
}

void ParticleEmitter::Sort()
{
	Point3F TempViewPos;
#ifndef SXZPARTSYS
	MatrixF Matrix = gClientSceneGraph->getLastWorld2ViewMX();
#else
    MatrixF Matrix = getCameraMatrix();
#endif
	Point3F pos;
	Matrix.getColumn(3,&pos);
	Matrix.transpose();

	MatrixF temp_mat;

	Particle*   temp = m_pParticleList;
	if(!mDataBlock->m_pParticleSystem->GetFollow())
	{
		while(temp)
		{
			temp_mat.identity();
			temp_mat.scale(m_vCurScale);
			temp_mat.setPosition(temp->m_vEmitterPos);
			temp_mat = temp->m_matParent * temp_mat;
			temp_mat.mulP(temp->m_CurPos ,&temp->m_vWorldPos);
			TempViewPos = temp->m_vWorldPos - pos;
			temp->m_fViewZ = TempViewPos.lenSquared();
			temp = temp->m_pNext;
		}
	}
	else
	{
		while(temp)
		{
			temp_mat.identity();
			temp_mat.scale(m_vCurScale);
			temp_mat.setPosition(temp->m_vEmitterPos);
			temp_mat = ParentMX(this) * temp_mat;
			temp->m_matParent = ParentMX(this);
			temp_mat.mulP(temp->m_CurPos ,&temp->m_vWorldPos);
			TempViewPos = temp->m_vWorldPos - pos;
			temp->m_fViewZ = TempViewPos.lenSquared();
			temp = temp->m_pNext;
		}
	}
	temp = m_pParticleList;
	if(temp)
	{
		for(S32 i = 0; i < m_iCurParticleNum - 1; i++)
		{
			temp = m_pParticleList;
			for(S32 j = 0; j < m_iCurParticleNum - i - 1; j++)
			{
				if(temp->m_pNext)
				{
					if (temp->m_fViewZ < temp->m_pNext->m_fViewZ)
					{
						if(temp->m_pNext->m_pNext)
						{
							Particle* pTempNext = temp->m_pNext;
							Particle* pNextNext = temp->m_pNext->m_pNext;
							pNextNext->m_pPrev = temp;
							if(temp->m_pPrev)
							{
								pTempNext->m_pPrev = temp->m_pPrev;
								temp->m_pPrev->m_pNext = pTempNext;
							}
							else
							{
								pTempNext->m_pPrev = NULL;
								m_pParticleList = pTempNext;
							}
							pTempNext->m_pNext = temp;
							temp->m_pNext = pNextNext;
							temp->m_pPrev = pTempNext;
						}
						else
						{
							Particle* pTempNext = temp->m_pNext;
							pTempNext->m_pNext = temp;
							if(temp->m_pPrev)
							{
								pTempNext->m_pPrev = temp->m_pPrev;
								temp->m_pPrev->m_pNext = pTempNext;
							}
							else
							{
								pTempNext->m_pPrev = NULL;
								m_pParticleList = pTempNext;
							}
							temp->m_pNext = NULL;
							temp->m_pPrev = pTempNext;
						}
					}
					else
					{
						temp = temp->m_pNext;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
}

void ParticleEmitter::deleteWhenEmpty()
{
      deleteObject();
}

ConsoleMethod(ParticleEmitterData, reload, void, 2, 2, "(void)"
			  "Reloads this emitter")
{
#ifndef  NTJ_SERVER
    	object->reload();
#endif
}

ColorF ParticleEmitter::getCollectiveColor()
{
	ColorF color = ColorF(0.0f, 0.0f, 0.0f);

	for( Particle* part = m_pParticleList; part != NULL; part = part->m_pNext )
	{
		color += part->m_Color;
	}

	if(m_iCurParticleNum > 0)
	{
		color /= F32(m_iCurParticleNum);
	}

	//if(color.red == 0.0f && color.green == 0.0f && color.blue == 0.0f)
	//	color = color;

	return color;
}

void ParticleEmitter::setSizes( F32 *sizeList )
{
}

void ParticleEmitter::setColors( ColorF *colorList )
{

}

void ParticleEmitter::initializeParticle(Particle* temp)
{
	ParticleData* temp_data = mDataBlock->m_pParticleSystem;
	temp->m_fAge = 0.0f;
	temp->m_fLife = temp_data->GetLifetime();
	temp->m_fCurSizeX = 0.0f;
	temp->m_fSizeStepX = 0.0f;
	temp->m_fSizeTimeX = 0.0f;
	temp->m_fCurSizeY = 0.0f;
	temp->m_fSizeStepY = 0.0f;
	temp->m_fSizeTimeY = 0.0f;
	temp->m_fVelocity = m_fCurVelocity + mRandGen.randF(-m_fVelocityRand, m_fVelocityRand);
	if(!temp_data->GetEmitByRadius())
	{
		if(!temp_data->GetModelEmit())
		{
			temp->m_CurPos = mRandGen.randP3(-m_vPosDeviation, m_vPosDeviation);
			temp->m_vEula = m_vCurEula + mRandGen.randP3(-m_vEulaRand, m_vEulaRand);
		}
		else
		{
			Point3F temp_eula = mRandGen.randP3(-m_vEulaRand, m_vEulaRand);
			temp_data->GetModelVertexAndNormal(&temp->m_CurPos, &temp->m_vEula);
            temp->m_vEula += temp_eula;
            temp->m_vEula.normalize();
		}
	}
	else
	{
		Point3F randpoint1 = Point3F(0.0f, 0.0f, 0.0f);
		Point3F randpoint2 = Point3F(6.28f, 6.28f, 6.28f);
		temp->m_vEula = mRandGen.randP3(randpoint1, randpoint2);
		D3DXMATRIX temp_mat;
		F32 offset = temp_data->GetEmitRadiusOffset();
		D3DXMatrixRotationYawPitchRoll(&temp_mat, temp->m_vEula.y, temp->m_vEula.x, temp->m_vEula.z);
		D3DXVECTOR3 temp_vec = D3DXVECTOR3(0.0f, temp_data->GetEmitRadius() + mRandGen.randF(-offset, offset), 0.0f);
		D3DXVec3TransformNormal(&temp_vec, &temp_vec, &temp_mat);
		Point3F offset1;
		memcpy(&offset1, &temp_vec, 12);
		temp->m_CurPos = offset1;
	}
	temp->m_Color = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
	temp->m_ColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
	temp->m_fColorTime = 0.0f;
	temp->m_fAlphaStep = 0.0f;
	temp->m_fAlphaTime = 0.0f;
	temp->m_fCurFlickerTime = 0.0f;
	temp->m_fFlickerTime = temp_data->GetRandomFlickerTime();
	temp->m_bDraw = true;
	
    temp->m_vEmitterPos = m_EmitterPos;
	temp->m_matParent = ParentMX(this);

	if(temp_data->GetBillboardState())
	{
		temp->Rotate.m_fAngle = 0.0f;
		temp->RotateStep.m_fAngleStep = 0.0f;
	}
	else
	{
		temp->Rotate.m_fRot[0] = temp->Rotate.m_fRot[1] = temp->Rotate.m_fRot[2] = 0.0f;
		temp->RotateStep.m_fRotStep[0] = temp->RotateStep.m_fRotStep[1] = temp->RotateStep.m_fRotStep[2] = 0.0f;
	}
	temp->m_fRotTime = 0.0f;

	temp->m_fGravity = 0.0f;
	temp->m_fGravityStep = 0.0f;
	temp->m_fGravityTime = 0.0f;

	if(temp_data->GetTextureSplinterState())
	{
		if(!temp_data->GetTexSplinterBroadRandom())
			temp->m_iTextureSplinterCode = 0;
		else
		{
			S32 temp_i = temp_data->GetTextureSplinterNum();
			temp->m_iTextureSplinterCode = mRandGen.randI(0, temp_i - 1);
		}
	}
	else
	{
		temp->m_iTextureSplinterCode = 0;
	}
	temp->m_fCurTexRemainTime = 0.0f;

	S32 particlerot = temp_data->GetParticleRot();
	if (particlerot == PARTICLEROT_CW)
	{
		temp->m_bRotCCW = false;
	}
	else if (particlerot == PARTICLEROT_CCW)
	{
		temp->m_bRotCCW = true;
	}
	else
	{
		S32 temp_i = mRandGen.randI(0, 1);
		if (temp_i == 0)
		{
			temp->m_bRotCCW = false;
		}
		else
		{
			temp->m_bRotCCW = true;
		}
	}
}

void ParticleEmitter::Reset()
{
    mDataBlock->m_pParticleSystem->mRandGen.setSeed( 2009 );
    mRandGen.setSeed( 2009 );
	Destory();
	m_iCurParticleNum = -1;
	SetPause(false);
}

#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(SXZPARTSYS))
void ParticleEmitter::deleteObject()
{
#ifndef NTJ_SERVER
	if(	g_MultThreadWorkMgr && g_MultThreadWorkMgr->isEnable())
	{
		removeObjectFromScene();
		g_MultThreadWorkMgr->addInDeleteingObjectInsList(this);
	}
	else
		Parent::deleteObject();
#else
	Parent::deleteObject();
#endif
}
#endif

RibbonEmitter::RibbonEmitter()
{
	mDataBlock = NULL;
	m_vCurSize = 0.0f;
	m_vSizeRand = 0.0f;
	m_vSizeStep = 0.0f;
	m_fSizeTime = 0.0f;
	m_vCurEula = Point3F(0.0f, 0.0f, 0.0f);
	m_vEulaRand = Point3F(0.0f, 0.0f, 0.0f);
	m_vEulaStep = Point3F(0.0f, 0.0f, 0.0f);
	m_fEulaTime = 0.0f;
    m_vCurColor = ColorF(0.0f, 0.0f, 0.0f, 1.0f);
    m_fColorRand = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
    m_vColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
    m_fColorTime = 0.0f;
	m_fCurAlpha = 1.0f;
	m_fAlphaStep = 0.0f;
	m_fAlphaTime = 0.0f;
    m_EmitterPos = Point3F(0.0f, 0.0f, 0.0f);
    m_bPause = true;
	m_pVertexList = NULL;
    m_iCurRibbonNum = 0;
	m_iCurRibbonNum = -1;
	m_pFollowPosList = NULL;
	m_iMaxRibbonNum = 0;
	mFillVB = false;

#ifdef SXZPARTSYS
    mCameraMatrix.identity();
#endif

#ifndef NTJ_SERVER
	static bool initilize = false;
	if (!initilize)
	{
		init();
		initilize = true;
	}
#endif
}

RibbonEmitter::~RibbonEmitter()
{
	Destory();
}

void RibbonEmitter::Destory()
{
    DELS(m_pVertexList);
	DELS(m_pFollowPosList);
}

void RibbonEmitter::InitSys()
{
	m_vCurSize = 0.0f;
	m_vSizeRand = 0.0f;
	m_vSizeStep = 0.0f;
	m_fSizeTime = 0.0f; 
	m_vCurEula = Point3F(0.0f, 0.0f, 0.0f);
	m_vEulaRand = Point3F(0.0f, 0.0f, 0.0f);
	m_vEulaStep = Point3F(0.0f, 0.0f, 0.0f);
	m_fEulaTime = 0.0f;
	m_vCurColor = ColorF(0.0f, 0.0f, 0.0f, 1.0f);
	m_fColorRand = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
	m_vColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
	m_fColorTime = 0.0f;
	m_fCurAlpha = 1.0f;
	m_fAlphaStep = 0.0f;
	m_fAlphaTime = 0.0f;
	m_fCurEmitTime = 0.0f;
	m_EmitterPos = Point3F(0.0f, 0.0f, 0.0f);
    m_fElapsedTime = 0.0f; 
}

void RibbonEmitter::SetPause(bool pause)
{
	m_bPause = pause;
	if ((!m_bPause) && (m_iCurRibbonNum == -1))
	{
		InitSys();
		m_iCurRibbonNum = 0;
		m_iMaxRibbonNum = mDataBlock->m_pRibbon->GetRibbonNum();
		AssertFatal( m_iMaxRibbonNum>0, "Error, SetPause()" );
		S32 num = 2 * m_iMaxRibbonNum;
		m_pVertexList = new GFXVertexPCT[num];
		float InvBlurNum = 1.0f/float(m_iMaxRibbonNum - 1);
		for (WORD i = 0; i < m_iMaxRibbonNum; i++)
		{
			m_pVertexList[i * 2].texCoord = Point2F(InvBlurNum * i ,0.0f);
			m_pVertexList[i * 2 + 1].texCoord = Point2F(m_pVertexList[i * 2].texCoord.x ,1.0f);
		}
		if(mDataBlock->m_pRibbon->GetFollow())
			m_pFollowPosList = new Point3F[num];
	}
}

bool RibbonEmitter::GetPause()
{
	return m_bPause;
}

void RibbonEmitter::SetRibbonSystem(RibbonEmitterData* system)
{
    mDataBlock = system;
}

RibbonEmitterData* RibbonEmitter::GetRibbonSystem()
{
	return mDataBlock;
}

void RibbonEmitter::Reset()
{
    mDataBlock->m_pRibbon->mRandGen.setSeed( 2009 );
    mRandGen.setSeed( 2009 );
	Destory();
	m_iCurRibbonNum = -1;
	SetPause(false);
}

bool RibbonEmitter::onNewDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<RibbonEmitterData*>(dptr);
	if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
		return false;

	scriptOnNewDataBlock();
	return true;
}

void RibbonEmitter::deleteWhenEmpty()
{
	deleteObject();
}

void RibbonEmitter::emitRibbon(const Point3F& start,
									const Point3F& end,
									const Point3F& axis,
									const Point3F& velocity,
									const U32      numMilliseconds)
{
	if(m_iCurRibbonNum > 0)
		updateBBox();

	if( mSceneManager == NULL )
	{
		gClientSceneGraph->addObjectToScene(this);
		gClientContainer.addObject(this);
		gClientProcessList.addObject(this);
	}
}

void RibbonEmitter::emitRibbon(const Point3F& point,
									const bool     useLastPosition,
									const Point3F& axis,
									const Point3F& velocity,
									const U32      numMilliseconds)
{
	if(m_iCurRibbonNum > 0)
		updateBBox();

	if( mSceneManager == NULL )
	{
		gClientSceneGraph->addObjectToScene(this);
		gClientContainer.addObject(this);
		gClientProcessList.addObject(this);
	}
}

void RibbonEmitter::emitRibbon(const Point3F& rCenter,
									const Point3F& rNormal,
									const F32      radius,
									const Point3F& velocity,
									S32 count)
{
	// Set world bounding box
	mObjBox.min = rCenter - Point3F(radius, radius, radius);
	mObjBox.max = rCenter + Point3F(radius, radius, radius);
	resetWorldBox();

	// Make sure we're part of the world
	if( m_iCurRibbonNum > 0 && mSceneManager == NULL )
	{
		gClientSceneGraph->addObjectToScene(this);
		gClientContainer.addObject(this);
		gClientProcessList.addObject(this);
	}
}

void RibbonEmitter::updateBBox()
{
	Point3F min1(1e10,   1e10,  1e10);
	Point3F max1(-1e10, -1e10, -1e10);

	for (S32 i = 0; i < m_iCurRibbonNum; i++)
	{
		min1.setMin( m_pVertexList[i * 2].point );
		max1.setMax( m_pVertexList[i * 2 + 1].point );
	}

	mObjBox = Box3F(min1, max1);
	mWorldToObj.mul(mObjBox);
	mObjBox.min.convolveInverse(mObjScale);
	mObjBox.max.convolveInverse(mObjScale);
	resetWorldBox();
}

void RibbonEmitter::SetData()
{
	if (mDataBlock->m_pRibbon->GetFollow())
	{
		S32 temp;

		Point3F temp_eula = m_vCurEula + mRandGen.randP3(-m_vEulaRand, m_vEulaRand);
		D3DXMATRIX temp_mat2;
		D3DXMatrixRotationYawPitchRoll(&temp_mat2, temp_eula.y, temp_eula.x, temp_eula.z);
        MatrixF temp_mat3;
		temp_mat3.identity();
        temp_mat3.scale(mDataBlock->m_pRibbon->GetScale());

		D3DXVECTOR3 temp_vec = D3DXVECTOR3(0.0f, m_vCurSize + mRandGen.randF(-m_vSizeRand, m_vSizeRand), 0.0f);
		D3DXVec3TransformNormal(&temp_vec, &temp_vec, &temp_mat2);

		Point3F offset;
		memcpy(&offset, &temp_vec, 12);
		temp_mat3.mulV(offset);
		m_pFollowPosList[(m_iCurRibbonNum - 1) * 2] = m_EmitterPos + offset;
		m_pFollowPosList[(m_iCurRibbonNum - 1) * 2 + 1] = m_EmitterPos - offset;

		for (S32 i = 0; i < m_iCurRibbonNum; i++)
		{
			temp = i * 2;
			ParentMX(this).mulP(m_pFollowPosList[temp] ,&m_pVertexList[temp].point);
			ParentMX(this).mulP(m_pFollowPosList[temp + 1] ,&m_pVertexList[temp + 1].point);
			if (i == m_iCurRibbonNum - 1)
			{
				ColorF temp2;
                m_vCurColor.alpha = 1.0f;
                m_fColorRand.alpha = 0.0f;
				temp2 = (m_vCurColor + mRandGen.randC(-m_fColorRand, m_fColorRand));
				temp2.clamp();
				temp2.alpha = m_fCurAlpha;
				m_pVertexList[temp].color =  temp2;
				m_pVertexList[temp + 1].color =  temp2;
			}
		}
	}
    else
    {
		Point3F temp_eula = m_vCurEula + mRandGen.randP3(-m_vEulaRand, m_vEulaRand);
		D3DXMATRIX temp_mat2;
		D3DXMatrixRotationYawPitchRoll(&temp_mat2, temp_eula.y, temp_eula.x, temp_eula.z);
		MatrixF temp_mat3;
		temp_mat3.identity();
		temp_mat3.scale(mDataBlock->m_pRibbon->GetScale());
		D3DXVECTOR3 temp_vec = D3DXVECTOR3(0.0f, m_vCurSize + mRandGen.randF(-m_vSizeRand, m_vSizeRand), 0.0f);
		D3DXVec3TransformNormal(&temp_vec, &temp_vec, &temp_mat2);

		Point3F offset;
		memcpy(&offset, &temp_vec, 12);
		temp_mat3.mulV(offset);
		Point3F tempp1 = m_EmitterPos + offset;
        Point3F tempp2 = m_EmitterPos - offset;
		ParentMX(this).mulP(tempp1 ,&m_pVertexList[(m_iCurRibbonNum - 1) * 2].point);
        ParentMX(this).mulP(tempp2 ,&m_pVertexList[(m_iCurRibbonNum - 1) * 2 + 1].point);
		ColorF temp2;
        m_vCurColor.alpha = 1.0f;
        m_fColorRand.alpha = 0.0f;
		temp2 = (m_vCurColor + mRandGen.randC(-m_fColorRand, m_fColorRand));
		temp2.clamp();
		temp2.alpha = m_fCurAlpha;
		m_pVertexList[(m_iCurRibbonNum - 1) * 2].color = temp2;
		m_pVertexList[(m_iCurRibbonNum - 1) * 2 + 1].color = temp2;

	}
}

bool RibbonEmitter::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	removeFromProcessList();

	F32 radius = 5.0;
	mObjBox.min = Point3F(-radius, -radius, -radius);
	mObjBox.max = Point3F(radius, radius, radius);
	resetWorldBox();

	return true;
}

void RibbonEmitter::onRemove()
{
	removeFromScene();
	Parent::onRemove();
}

void RibbonEmitter::SetTime(F32 time)
{
    F32 emitlife = mDataBlock->m_pRibbon->GetEmitLife();
    if (time > emitlife)
        return;
    else if (time > m_fCurEmitTime)
    {
        advanceTime(time - m_fCurEmitTime);
    }
    else if (time <= m_fCurEmitTime)
    {
        if (mDataBlock->m_pRibbon->GetRepeat())
        {
            advanceTime(emitlife - m_fCurEmitTime + time);
        }
        else
        {
            Reset();
            advanceTime(time);
        }
    }
}

void RibbonEmitter::advanceTime(F32 dt)
{
	RibbonParam *pParticleParamData = new RibbonParam;
	pParticleParamData->dt = dt;
	pParticleParamData->pEmitter = this;

#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(SXZPARTSYS))
	if(g_MultThreadWorkMgr->isEnable())
	{
		stThreadParam *pThreadData = new stThreadParam;
		pThreadData->pParam = pParticleParamData;
		pThreadData->pFunc = doRibbonRoutine;
		pThreadData->pObj = this;

		g_MultThreadWorkMgr->addInProcessList(pThreadData);
	}
	else
		doRibbonRoutine(pParticleParamData);
#else
	doRibbonRoutine(pParticleParamData);
#endif
}

void RibbonEmitter::doRibbonRoutine(void *pParam)
{
	RibbonParam *pRibbonParamData = (RibbonParam *)pParam;
	pRibbonParamData->pEmitter->doAdvanceTime(pRibbonParamData->dt);
	if(pRibbonParamData->pEmitter->mFillVB)
	{
		pRibbonParamData->pEmitter->copyToVB();
		pRibbonParamData->pEmitter->mFillVB = false;
	}
	delete pRibbonParamData;
}

void RibbonEmitter::doAdvanceTime(F32 dt)
{
	F32 displayv = mDataBlock->m_pRibbon->GetDisplayVelocity();

	if((displayv != 1.0f) && (displayv > 0.0f))
		dt *= displayv; 

	m_fElapsedTime += dt;
	if( m_fElapsedTime < TIME_PER_FRAME ) 
		return;

	while(m_fElapsedTime >= TIME_PER_FRAME)
	{
		m_fElapsedTime -= TIME_PER_FRAME;

		Parent::advanceTime(TIME_PER_FRAME);

		if(!m_bPause)
		{
			if (m_fCurEmitTime > mDataBlock->m_pRibbon->GetEmitLife())
			{
				if (mDataBlock->m_pRibbon->GetRepeat())
				{
					InitSys();
				}
				else
				{
					m_bPause = true;
				}
			}
		}

        m_fCurEmitTime += TIME_PER_FRAME;

		Point3F emitterpos;
		if(mDataBlock->m_pRibbon->GetAnimationPosition(m_fCurEmitTime, &emitterpos))
		{
			m_EmitterPos = emitterpos;
		}

		if(m_fSizeTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pRibbon->GetSizeTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pRibbon->GetSizeTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fSizeTime = point2;
							F32 temp_vec1, temp_vec2;
							if(mDataBlock->m_pRibbon->GetSizePointStep(i2, &temp_vec1) && mDataBlock->m_pRibbon->GetSizePointStep(i2 + 1, &temp_vec2))
							{
								m_vSizeStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_vCurSize = temp_vec1;
								mDataBlock->m_pRibbon->GetSizeRand(i2, &m_vSizeRand);
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							m_fSizeTime = point1;
							F32 temp_vec1;
							if(mDataBlock->m_pRibbon->GetSizePointStep(i2, &temp_vec1))
							{
								m_vSizeStep = (temp_vec1 - m_vCurSize)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime <= point1)
						{
							m_fSizeTime = point1;
							F32 temp_vec1;
							if(mDataBlock->m_pRibbon->GetSizePointStep(i2, &temp_vec1))
							{
								m_vSizeStep = (temp_vec1 - m_vCurSize)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							m_vSizeStep = 0.0f;
							m_fSizeTime = MAX_TIME;
							F32 temp_vec1;
							if(mDataBlock->m_pRibbon->GetSizePointStep(i2, &temp_vec1))
							{
								m_vCurSize = temp_vec1;
								mDataBlock->m_pRibbon->GetSizeRand(i2, &m_vSizeRand);
							}
						}
						break;
					}
				}
				else
				{
					m_vSizeStep = 0.0f;
					m_fSizeTime = MAX_TIME;
					break;
				}
			}
		}
		if(m_vSizeStep != 0.0f)
		{
			m_vCurSize += m_vSizeStep * TIME_PER_FRAME;
			if (m_vCurSize < 0.0f)
			{
				m_vCurSize = 0.0f;
			}
		}

		if(m_fEulaTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pRibbon->GetEulaTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pRibbon->GetEulaTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fEulaTime = point2;
							Point3F temp_vec1, temp_vec2;
							if(mDataBlock->m_pRibbon->GetRandomEula(i2, &temp_vec1) && mDataBlock->m_pRibbon->GetRandomEula(i2 + 1, &temp_vec2))
							{
								m_vEulaStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_vCurEula = temp_vec1;
								mDataBlock->m_pRibbon->GetEulaRand(i2, &m_vEulaRand);
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							m_fEulaTime = point1;
							Point3F temp_vec1;
							if(mDataBlock->m_pRibbon->GetRandomEula(i2, &temp_vec1))
							{
								m_vEulaStep = (temp_vec1 - m_vCurEula)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime <= point1)
						{
							m_fEulaTime = point1;
							Point3F temp_vec1;
							if(mDataBlock->m_pRibbon->GetRandomEula(i2, &temp_vec1))
							{
								m_vEulaStep = (temp_vec1 - m_vCurEula)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							m_vEulaStep = Point3F(0.0f, 0.0f, 0.0f);
							m_fEulaTime = MAX_TIME;
							Point3F temp_vec1;
							if(mDataBlock->m_pRibbon->GetRandomEula(i2, &temp_vec1))
							{
								m_vCurEula = temp_vec1;
								mDataBlock->m_pRibbon->GetEulaRand(i2, &m_vEulaRand);
							}
						}
						break;
					}
				}
				else
				{
					m_vEulaStep = Point3F(0.0f, 0.0f, 0.0f);
					m_fEulaTime = MAX_TIME;
					break;
				}
			}
		}
		if((m_vEulaStep.x != 0.0f) || (m_vEulaStep.y != 0.0f) || (m_vEulaStep.z != 0.0f))
			m_vCurEula += m_vEulaStep * TIME_PER_FRAME;

		if(m_fColorTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pRibbon->GetColorTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pRibbon->GetColorTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fColorTime = point2;
							ColorF temp_vec1, temp_vec2;
							if(mDataBlock->m_pRibbon->GetColorPointStep(i2, &temp_vec1) && mDataBlock->m_pRibbon->GetColorPointStep(i2 + 1, &temp_vec2))
							{
								m_vColorStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_vCurColor = temp_vec1;
								mDataBlock->m_pRibbon->GetColorRand(i2, &m_fColorRand);
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							m_fColorTime = point1;
							ColorF temp_vec1;
							if(mDataBlock->m_pRibbon->GetColorPointStep(i2, &temp_vec1))
							{
								m_vColorStep = (temp_vec1 - m_vCurColor)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime <= point1)
						{
							m_fColorTime = point1;
							ColorF temp_vec1;
							if(mDataBlock->m_pRibbon->GetColorPointStep(i2, &temp_vec1))
							{
								m_vColorStep = (temp_vec1 - m_vCurColor)/(point1 - m_fCurEmitTime);
							}
							break;
						}
						else
						{
							m_vColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
							m_fColorTime = MAX_TIME;
							ColorF temp_vec1;
							if(mDataBlock->m_pRibbon->GetColorPointStep(i2, &temp_vec1))
							{
								m_vCurColor = temp_vec1;
								mDataBlock->m_pRibbon->GetColorRand(i2, &m_fColorRand);
							}
						}
						break;
					}
				}
				else
				{
					m_vColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
					m_fColorTime = MAX_TIME;
					break;
				}
			}
		}
		if((m_vColorStep.red != 0.0f) || (m_vColorStep.green != 0.0f) || (m_vColorStep.blue != 0.0f))
		{
			m_vCurColor += m_vColorStep * TIME_PER_FRAME;
			m_vCurColor.clamp();
		}

		if(m_fAlphaTime < m_fCurEmitTime)
		{
			for(S32 i2 = 0; ; i2++)
			{
				F32 point1, point2;
				if (mDataBlock->m_pRibbon->GetAlphaTimePoint(i2, &point1))
				{
					if (mDataBlock->m_pRibbon->GetAlphaTimePoint(i2 + 1, &point2))
					{
						if((m_fCurEmitTime > point1) && (m_fCurEmitTime <= point2))
						{
							m_fAlphaTime = point2;
							F32 temp_vec1, temp_vec2;
							if(mDataBlock->m_pRibbon->GetAlphaPoint(i2, &temp_vec1) && mDataBlock->m_pRibbon->GetAlphaPoint(i2 + 1, &temp_vec2))
							{
								m_fAlphaStep = (temp_vec2 - temp_vec1)/(point2 - point1);
								m_fCurAlpha = temp_vec1;
							}
							break;
						}
						else if (m_fCurEmitTime <= point1)
						{
							F32 temp_vec1;
							if(mDataBlock->m_pRibbon->GetAlphaPoint(i2, &temp_vec1))
							{
								m_fAlphaStep = (temp_vec1 - m_fCurAlpha)/(point1 - m_fCurEmitTime);
							}
							m_fAlphaTime = point1;
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						if(m_fCurEmitTime > point1)
						{
							F32 temp_vec1;
							if(mDataBlock->m_pRibbon->GetAlphaPoint(i2, &temp_vec1))
							{
								m_fAlphaTime = MAX_TIME;
								m_fAlphaStep = 0.0f;
								m_fCurAlpha = temp_vec1;
							}
						}
						else
						{
							F32 temp_vec1;
							if(mDataBlock->m_pRibbon->GetAlphaPoint(i2, &temp_vec1))
							{
								m_fAlphaStep = (temp_vec1 - m_fCurAlpha)/(point1 - m_fCurEmitTime);
							}
							m_fAlphaTime = point1;
						}
						break;
					}
				}
				else
				{
					m_fAlphaStep = 0.0f;
					m_fAlphaTime = MAX_TIME;
					break;
				}
			}
		}
		if(m_fAlphaStep != 0.0f)
		{
			m_fCurAlpha += m_fAlphaStep * TIME_PER_FRAME;
			if (m_fCurAlpha > 1.0f)
			{
				m_fCurAlpha = 1.0f;
			}
			else if (m_fCurAlpha < 0.0f)
			{
				m_fCurAlpha = 0.0f;
			}
		}

		if(!m_bPause)
		{
			S32 maxnum = mDataBlock->m_pRibbon->GetRibbonNum();

			if(m_iMaxRibbonNum != maxnum)
			{
				Reset();
#ifndef SXZPARTSYS
				mpVertBuff2->set( GFX, m_iMaxRibbonNum * 2, GFXBufferTypeDynamic );
#endif
			}

            DWORD temp_dw;

            if(!mDataBlock->m_pRibbon->GetFollow())
            {
                if(m_iCurRibbonNum >= m_iMaxRibbonNum)
                {
                    for (WORD i = 0; i < m_iCurRibbonNum - 1; i++)
                    {
                        temp_dw = i * 2;
                        m_pVertexList[temp_dw].point = m_pVertexList[temp_dw + 2].point;
                        m_pVertexList[temp_dw + 1].point = m_pVertexList[temp_dw + 3].point;
                        m_pVertexList[temp_dw].color = m_pVertexList[temp_dw + 2].color;
                        m_pVertexList[temp_dw + 1].color = m_pVertexList[temp_dw + 3].color;
                    }
                }
            }
            else
            {
                if(m_iCurRibbonNum >= m_iMaxRibbonNum)
                {
                    for (WORD i = 0; i < m_iCurRibbonNum - 1; i++)
                    {
                        temp_dw = i * 2;
                        m_pFollowPosList[temp_dw] = m_pFollowPosList[temp_dw + 2];
                        m_pFollowPosList[temp_dw + 1] = m_pFollowPosList[temp_dw + 3];
                    }
                }
            }

            if((m_iCurRibbonNum < m_iMaxRibbonNum))
            {
                m_iCurRibbonNum += 1;
            }

		}

		if (m_iCurRibbonNum > 0)
		{
			SetData();
		}

		if ((m_bPause) && (m_iCurRibbonNum > 0))
		{
			m_iCurRibbonNum -= 1;
		}

		if((m_bPause) && (m_iCurRibbonNum == 0))
		{
			Destory();
			m_iCurRibbonNum = -1;
		}
	}
}

void RibbonEmitter::renderObject( SceneState *state, RenderInst *ri )
{
	GFX->setCullMode( GFXCullNone );
	GFX->setZWriteEnable( false );
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
	S32 temp_s32 = (ri->texWrapFlags - 1) % 4;
	if(temp_s32 == 0)
	{
		GFX->setSrcBlend(GFXBlendSrcAlpha);
		GFX->setDestBlend(GFXBlendInvSrcAlpha);
	}
	else if (temp_s32 == 1)
	{
		GFX->setSrcBlend(GFXBlendZero);
		GFX->setDestBlend(GFXBlendInvSrcColor);
	}
	else if (temp_s32 == 2)
	{
		GFX->setSrcBlend(GFXBlendOne);
		GFX->setDestBlend(GFXBlendInvSrcAlpha);
	}
	else if (temp_s32 == 3)
	{
		GFX->setSrcBlend(GFXBlendSrcAlpha);
		GFX->setDestBlend(GFXBlendOne);
	}
	else
	{

	}

	bool reEnableLighting = GFX->getRenderState( GFXRSLighting );

	GFX->setLightingEnable( false );
	GFX->setAlphaBlendEnable( true );

	MatrixF mat;
	mat = (*ri->objXform) * (*ri->worldXform);

	static bool bfind = false;
	static ShaderData *sgParticleRender = NULL; 
	if(!bfind)
	{
		if ( !Sim::findObject( "ParticleRender", sgParticleRender ) )
			Con::warnf("ParticleRender - failed to locate ParticleRender shader ParticleRenderBlendData!");
		bfind = true;
	}
	GFX->setShader(sgParticleRender->getShader());

	mat.transpose();
	GFX->setVertexShaderConstF(0, (float*)&mat, 4);
	ColorF temp_point4f = ri->maxBlendColor;
	GFX->setPixelShaderConstF(0, (float*)&temp_point4f, 1);
	Point4F temp_p4 = Point4F(0.0f, 0.0f ,0.0f, 0.0f);
	temp_p4.x = ri->visibility;
	GFX->setPixelShaderConstF(1, (float*)&temp_p4, 1);

	GFX->setTexture( 0, ri->miscTex );
	GFX->setPrimitiveBuffer( *ri->primBuff );
	GFX->setVertexBuffer( *ri->vertBuff );

	GFX->drawPrimitive( GFXTriangleStrip, 0, ri->primBuffIndex * 2 - 2);

	GFX->disableShaders();

	//GFX->popWorldMatrix();
	GFX->setLightingEnable( reEnableLighting );
}

bool RibbonEmitter::prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState)
{
	if( isLastState(state, stateKey) )
		return false;

	if(gClientSceneGraph->isReflectPass())
		return false;

	PROFILE_START(RibbonEmitter_prepRenderImage);

	setLastState(state, stateKey);

	// This should be sufficient for most objects that don't manage zones, and
	//  don't need to return a specialized RenderImage...
	if( state->isObjectRendered(this) )
	{
		prepBatchRender( state->getCameraPosition() );
	}

	PROFILE_END(RibbonEmitter_prepRenderImage);

	return false;
}

void RibbonEmitter::copyToVB()
{
#ifndef SXZPARTSYS
	if( !(*mpVertBuff2) )
	{
		mpVertBuff2->set( GFX, m_iMaxRibbonNum * 2, GFXBufferTypeDynamic );
	} 

	// lock and copy tempBuff to video RAM
	GFXVertexPCT *verts = mpVertBuff2->lock();
	dMemcpy( verts, m_pVertexList, m_iCurRibbonNum * 2 * sizeof(GFXVertexPCT) );
	mpVertBuff2->unlock();    

#else
    if ( m_iCurRibbonNum > 1 )
    {
        mVBForMax.setSize( m_iCurRibbonNum * 2 );
        dMemcpy( mVBForMax.address(), m_pVertexList, m_iCurRibbonNum * 2 * sizeof(GFXVertexPCT) );
    }
#endif

	switchBuff();
}

void RibbonEmitter::prepBatchRender( const Point3F& camPos )
{
	if(m_iCurRibbonNum <= 0)
		return;

#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(SXZPARTSYS))
    if(g_MultThreadWorkMgr->isEnable())
    {
		mFillVB = true;
	    if(!mpVertBuff->getPointer() || mpVertBuff->getPointer()->isNull()) //Ray: 没有做填充的VB不做渲染
		    return;
    }
    else
	{
        copyToVB();
	}
#else
	copyToVB();
#endif

#ifndef SXZPARTSYS
	RenderInst *ri = gRenderInstManager.allocInst();
	ri->vertBuff = mpVertBuff;
	ri->primBuff = &primBuff;

	ri->translucent = true;
	ri->type = RenderInstManager::RIT_Translucent;
	ri->calcSortPoint( this, camPos );
	ri->particles = RenderTranslucentMgr::RENDERTRANSLUCENT_TYPE_RIBBON;
    ri->texWrapFlags = mDataBlock->m_pRibbon->GetTextureStageState() + 4;
	ri->worldXform = gRenderInstManager.allocXform();
	ri->objXform = gRenderInstManager.allocXform();

	*ri->worldXform = gClientSceneGraph->getLastWorld2ViewMX();
	*ri->objXform = GFX->getProjectionMatrix();
	ri->obj = this;

	ri->primBuffIndex = m_iCurRibbonNum;

	ri->miscTex = &*(mDataBlock->m_pRibbon->GetTexture());
	Point3F temp_color = mDataBlock->m_pRibbon->GetSelfIllumination();
	ri->maxBlendColor = ColorF(temp_color.x, temp_color.y, temp_color.z, mDataBlock->m_pRibbon->GetGlow());
    ri->visibility = mDataBlock->m_pRibbon->GetPowValue();

	gRenderInstManager.addInst( ri );
#endif
}

#ifndef NTJ_SERVER
void      RibbonEmitter::init()
{
	const U32 indexListSize = 2400; // 6 indices per ribbon segment
	const U32 MaxRibbonNum = 400;
	U16 *indices = new U16[ indexListSize ];

	for( U32 i=0; i<MaxRibbonNum; i++ )
	{
        indices[i*6] = 0+i*2;
        indices[i*6+1] = 1+i*2;
        indices[i*6+2] = 2+i*2;
        indices[i*6+3] = 2+i*2;
        indices[i*6+4] = 1+i*2;
        indices[i*6+5] = 3+i*2;
	}

#ifndef SXZPARTSYS
    U16 *ibIndices;
    GFXBufferType bufferType = GFXBufferTypeStatic;

#ifdef POWER_OS_XENON
    // Because of the way the volatile buffers work on Xenon this is the only
    // way to do this.
    bufferType = GFXBufferTypeVolatile;
#endif
    primBuff.set( GFX, indexListSize, 0, bufferType );
    primBuff.lock( &ibIndices );
    dMemcpy( ibIndices, indices, indexListSize * sizeof(U16) );
    primBuff.unlock();
#else
    pbForMax.setSize( indexListSize );
    dMemcpy( pbForMax.address(), indices, indexListSize * sizeof(U16) );
#endif

	delete [] indices;
}

void      RibbonEmitter::shutdown()
{
	primBuff = NULL;
}
#endif

#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(SXZPARTSYS))
void RibbonEmitter::deleteObject()
{
#ifndef NTJ_SERVER
	if(	g_MultThreadWorkMgr && g_MultThreadWorkMgr->isEnable())
	{
		removeObjectFromScene();
		g_MultThreadWorkMgr->addInDeleteingObjectInsList(this);
	}
	else
		Parent::deleteObject();
#else
	Parent::deleteObject();
#endif
}
#endif
