//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gfx/D3D9/gfxD3D9Device.h"
#include "particle.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mRandom.h"
#include "math/mathIO.h"

IMPLEMENT_CO_DATABLOCK_V1(ParticleData);

ParticleData::ParticleData()
{
	m_fLifetime = 0.0f;
	m_fLifetimeRand = 0.0f;
	m_vecSizeXRand = 0;
	m_vecSizeXPoint = 0;
	m_vecSizeXTimePoint = 0;
	m_vecSizeYRand = 0;
	m_vecSizeYPoint = 0;
	m_vecSizeYTimePoint = 0;
	m_vecEulaRand = 0;
	m_vecEula = 0;
	m_vecEulaTimePoint = 0;
	m_vecVelocityRand = 0;
	m_vecVelocityPoint = 0;
	m_vecVelocityTimePoint = 0;
	m_vecRepulseVelocityRand = 0;
	m_vecRepulseVelocityPoint = 0;
	m_vecRepulseVelocityTimePoint = 0;
	m_vecColorPointRand = 0;
	m_vecColorPoint = 0;
	m_vecColorTimePoint = 0;                        
	m_vecAlphaPoint = 0;                       
	m_vecAlphaTimePoint = 0;                   
	m_vecAnglePoint = 0;
	m_vecAngleTimePoint = 0;
	m_vecAnglePointRand = 0;
	m_fGravity = 0;
	m_fGravityRand = 0;
	m_vecGravityTimePoint = 0;
	m_vecEmitTimeIntervalTimePoint = 0;
	m_vecEmitTimeIntervalRand = 0;
    m_vecEmitTimeInterval = 0;
    m_vecPosDeviation = 0;
	m_vecPosDeviationTimePoint = 0;
	m_vecEmitStateTimePoint = 0;
	m_vecEmitState = 0;
	m_iPerEmitNum = 0;
	m_bBillboard = false;
	m_vecRotTimePoint = 0;                  
	m_vecRotValue = 0;     
	m_vecRotValueRand = 0;     
	m_bTextureSplinter = false;
	m_iTextureSplinterNum = 0;
	m_iTextureSplinterStageNumLength = 0;
	m_iTextureSplinterStageNumWidth = 0;
	m_bTexSplinterBroadRandom = false;
	m_fTexRemainTime = 0.0f;
	m_fFlickerInterval = 0.0f;
	m_fFlickerTime = 0.0f;
	m_fFlickerIntervalRand = 0.0f;
	m_fFlickerTimeRand = 0.0f;
	m_iParticleRot = PARTICLEROT_CW;
	m_bRepeat = false;
	m_fTextureCoordUV1 = NULL;
	m_fTextureCoordUV2 = NULL;
	m_fTextureCoordUV3 = NULL;
	m_fTextureCoordUV4 = NULL;
	m_dwMaxParticleNum = 0;
	m_TextureHandle = NULL;
	m_TextureName = NULL;
    m_iTextureStageState = 1;
	m_fEmitLife = 0.0f;
	m_fDisplayVelocity = 1.0f;
	m_vecAnimationTimePoint = 0;
    m_vecAnimationPosition = 0;
	m_fGlow = 1.0f;
	m_bFollow = false;
	m_bHalfCut = false;
	m_vecScalePoint = 0;
	m_vecScaleTimePoint = 0;
	m_bHaveTail = false;
	m_bEmitByRadius = false;
	m_fEmitRadius = 0.0f;
	m_fEmitRadiusOffset = 0.0f;
	m_bModelEmit = false;
	m_vecModelVertex = 0;
	m_vecModelNormal = 0;
	m_vSelfIllumination = Point3F(0.0f, 0.0f, 0.0f);
	m_fPowValue = 1.0f;
	m_bSizeRandomBothXAndY = true;

	m_bElectricity = false;
	m_fGlowStrength = 144.0f;
	m_fHeight = 0.44f;
	m_fGlowFallOff = 0.024f;
	m_fSpeed = 1.86f;
	m_fSampleDist = 0.0076f;
	m_fAmbientGlow = 0.5f;
	m_fAmbientGlowHeightScale = 1.68f;
	m_fVertNoise = 0.78f;
	m_pTexture = NULL;
}

ParticleData::~ParticleData()
{
	Destory();
}

void ParticleData::Destory()
{
	m_vecSizeXPoint.clear();
	m_vecSizeXTimePoint.clear();
	m_vecSizeXRand.clear();
	m_vecSizeYRand.clear();
	m_vecSizeYPoint.clear();
	m_vecSizeYTimePoint.clear();
	m_vecEula.clear();
	m_vecEulaRand.clear();
	m_vecEulaTimePoint.clear();
	m_vecVelocityPoint.clear();
	m_vecVelocityTimePoint.clear();
	m_vecVelocityRand.clear();
	m_vecRepulseVelocityRand.clear();
	m_vecRepulseVelocityPoint.clear();
	m_vecRepulseVelocityTimePoint.clear();
	m_vecColorPointRand.clear();
	m_vecColorPoint.clear();
	m_vecColorTimePoint.clear();
	m_vecAlphaPoint.clear();
	m_vecAlphaTimePoint.clear();

	m_vecAnglePoint.clear();
	m_vecAngleTimePoint.clear();
	m_vecAnglePointRand.clear();
	m_fGravity.clear();
	m_fGravityRand.clear();
	m_vecGravityTimePoint.clear();
	m_vecEmitTimeIntervalTimePoint.clear();
	m_vecEmitTimeIntervalRand.clear();
	m_vecEmitTimeInterval.clear();
	m_vecPosDeviation.clear();
	m_vecPosDeviationTimePoint.clear();
	m_vecEmitStateTimePoint.clear();
    m_vecEmitState.clear();
	m_vecAnimationTimePoint.clear();
	m_vecAnimationPosition.clear();

	m_vecRotTimePoint.clear();
	m_vecRotValue.clear();
	m_vecRotValueRand.clear();

    m_vecModelVertex.clear();

	m_vecModelNormal.clear();

	m_vecScalePoint.clear();
	m_vecScaleTimePoint.clear();

	DELS(m_fTextureCoordUV1);
	DELS(m_fTextureCoordUV2);
	DELS(m_fTextureCoordUV3);
	DELS(m_fTextureCoordUV4);

	m_TextureHandle = NULL;
	SAFE_RELEASE(m_pTexture);
}

void ParticleData::packData(BitStream* stream)
{
	S32 temp_num;
	Parent::packData(stream);
	if (stream->writeFlag(m_TextureName && m_TextureName[0]))
		stream->writeString(m_TextureName);
	stream->writeBits(32, &m_fLifetime);
	stream->writeBits(32, &m_fLifetimeRand);

	temp_num = m_vecSizeXTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecSizeXPoint[i]);
        stream->writeBits(32, &m_vecSizeXRand[i]);
		stream->writeBits(32, &m_vecSizeXTimePoint[i]);
	}

	temp_num = m_vecSizeYTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecSizeYPoint[i]);
		stream->writeBits(32, &m_vecSizeYRand[i]);
		stream->writeBits(32, &m_vecSizeYTimePoint[i]);
	}

    temp_num = m_vecEulaTimePoint.size();
	stream->writeInt(temp_num, 16);
	for (S32 i = 0; i < temp_num; i++)
	{
	    stream->writeBits(96, &m_vecEula[i]);
		stream->writeBits(96, &m_vecEulaRand[i]);
		stream->writeBits(32,&m_vecEulaTimePoint[i]);
	}
    temp_num = m_vecVelocityTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecVelocityPoint[i]);
        stream->writeBits(32, &m_vecVelocityRand[i]);
		stream->writeBits(32,&m_vecVelocityTimePoint[i]);
	}
	temp_num = m_vecRepulseVelocityTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecRepulseVelocityPoint[i]);
		stream->writeBits(32, &m_vecRepulseVelocityRand[i]);
		stream->writeBits(32,&m_vecRepulseVelocityTimePoint[i]);
	}
    temp_num = m_vecColorTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(128, &m_vecColorPoint[i]);
		stream->writeBits(128, &m_vecColorPointRand[i]);
		stream->writeBits(32, &m_vecColorTimePoint[i]);
	}
	temp_num = m_vecAlphaTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecAlphaPoint[i]);
		stream->writeBits(32, &m_vecAlphaTimePoint[i]);
	}
    temp_num = m_vecAngleTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecAnglePoint[i]);
		stream->writeBits(32, &m_vecAnglePointRand[i]);
		stream->writeBits(32, &m_vecAngleTimePoint[i]);
	}
    temp_num = m_vecGravityTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_fGravity[i]);
		stream->writeBits(32, &m_fGravityRand[i]);
		stream->writeBits(32, &m_vecGravityTimePoint[i]);
	}
	temp_num = m_vecEmitTimeIntervalTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecEmitTimeInterval[i]);
		stream->writeBits(32, &m_vecEmitTimeIntervalRand[i]);
		stream->writeBits(32, &m_vecEmitTimeIntervalTimePoint[i]);
	}
	temp_num = m_vecPosDeviationTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(96, &m_vecPosDeviation[i]);
		stream->writeBits(32, &m_vecPosDeviationTimePoint[i]);
	}
	temp_num = m_vecEmitStateTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecEmitState[i]);
		stream->writeBits(32, &m_vecEmitStateTimePoint[i]);
	}
	stream->writeInt(m_iPerEmitNum, 8);
	stream->writeFlag(m_bBillboard);

	temp_num = m_vecRotTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32,&m_vecRotTimePoint[i]);
		stream->writeBits(96,&m_vecRotValue[i]);
		stream->writeBits(96,&m_vecRotValueRand[i]);
	}

	stream->writeFlag(m_bTextureSplinter);
	stream->writeFlag(m_bTexSplinterBroadRandom);

	stream->writeInt(m_iTextureSplinterStageNumLength, 8);
	stream->writeInt(m_iTextureSplinterStageNumWidth, 8);
    stream->writeInt(m_dwMaxParticleNum, 32);
	stream->writeBits(32, &m_fEmitLife);
    stream->writeBits(32, &m_fGlow);
	stream->writeBits(96, &m_vSelfIllumination);
    stream->writeBits(32, &m_fPowValue);

    stream->writeBits(32, &m_fTexRemainTime);
    stream->writeBits(32, &m_fFlickerInterval);
    stream->writeBits(32, &m_fFlickerIntervalRand);
    stream->writeBits(32, &m_fFlickerTime);
    stream->writeBits(32, &m_fFlickerTimeRand);
    stream->writeInt(m_iParticleRot, 8);
    stream->writeFlag(m_bRepeat);
    stream->writeFlag(m_bFollow);
    stream->writeInt(m_iTextureStageState, 8);
	stream->writeBits(32, &m_fDisplayVelocity);
    stream->writeFlag(m_bHalfCut);
    
    stream->writeFlag(m_bHaveTail);
    stream->writeFlag(m_bEmitByRadius);
    stream->writeBits(32, &m_fEmitRadius);
    stream->writeBits(32, &m_fEmitRadiusOffset);

	temp_num = m_vecAnimationTimePoint.size();
	stream->writeInt(temp_num, 16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32,&m_vecAnimationTimePoint[i]);
	}
	temp_num = m_vecAnimationPosition.size();
	stream->writeInt(temp_num, 16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(96,&m_vecAnimationPosition[i]);
	}

	stream->writeFlag(m_bModelEmit);
	temp_num = m_vecModelVertex.size();
	stream->writeInt(temp_num, 16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(96,&m_vecModelVertex[i]);
        stream->writeBits(96,&m_vecModelNormal[i]);
	}

	temp_num = m_vecScalePoint.size();
	stream->writeInt(temp_num, 16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(96,&m_vecScalePoint[i]);
		stream->writeBits(32,&m_vecScaleTimePoint[i]);
	}
	stream->writeFlag(m_bSizeRandomBothXAndY);

	stream->writeFlag(m_bElectricity);
	stream->writeBits(32,&m_fGlowStrength);
	stream->writeBits(32,&m_fHeight);
	stream->writeBits(32,&m_fGlowFallOff);
	stream->writeBits(32,&m_fSpeed);
	stream->writeBits(32,&m_fSampleDist);
	stream->writeBits(32,&m_fAmbientGlow);
	stream->writeBits(32,&m_fAmbientGlowHeightScale);
	stream->writeBits(32,&m_fVertNoise);
}

void ParticleData::unpackData(BitStream* stream)
{
	S32 temp_num;
	Parent::unpackData(stream);
	m_TextureName = (stream->readFlag()) ? stream->readSTString() : 0;
	stream->readBits(32, &m_fLifetime);
    stream->readBits(32, &m_fLifetimeRand);

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecSizeXPoint[i]);
        stream->readBits(32, &m_vecSizeXRand[i]);
		stream->readBits(32, &m_vecSizeXTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecSizeYPoint[i]);
		stream->readBits(32, &m_vecSizeYRand[i]);
		stream->readBits(32, &m_vecSizeYTimePoint[i]);
	}

	temp_num = stream->readInt(16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(96, &m_vecEula[i]);
		stream->readBits(96, &m_vecEulaRand[i]);
		stream->readBits(32, &m_vecEulaTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecVelocityPoint[i]);
		stream->readBits(32, &m_vecVelocityRand[i]);
		stream->readBits(32,&m_vecVelocityTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecRepulseVelocityPoint[i]);
		stream->readBits(32, &m_vecRepulseVelocityRand[i]);
		stream->readBits(32,&m_vecRepulseVelocityTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(128, &m_vecColorPoint[i]);
		stream->readBits(128, &m_vecColorPointRand[i]);
		stream->readBits(32, &m_vecColorTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecAlphaPoint[i]);
		stream->readBits(32, &m_vecAlphaTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		 stream->readBits(32, &m_vecAnglePoint[i]);
		 stream->readBits(32, &m_vecAnglePointRand[i]);
		 stream->readBits(32, &m_vecAngleTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_fGravity[i]);
		stream->readBits(32, &m_fGravityRand[i]);
		stream->readBits(32, &m_vecGravityTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecEmitTimeInterval[i]);
		stream->readBits(32, &m_vecEmitTimeIntervalRand[i]);
		stream->readBits(32, &m_vecEmitTimeIntervalTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(96, &m_vecPosDeviation[i]);
		stream->readBits(32, &m_vecPosDeviationTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecEmitState[i]);
		stream->readBits(32, &m_vecEmitStateTimePoint[i]);
	}

	m_iPerEmitNum = stream->readInt(8);
	m_bBillboard = stream->readFlag();

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecRotTimePoint[i]);
		stream->readBits(96, &m_vecRotValue[i]);
		stream->readBits(96, &m_vecRotValueRand[i]);
	}
	m_bTextureSplinter = stream->readFlag();
	m_bTexSplinterBroadRandom = stream->readFlag();

	m_iTextureSplinterStageNumLength = stream->readInt(8);
	m_iTextureSplinterStageNumWidth = stream->readInt(8);
	m_dwMaxParticleNum = stream->readInt(32);
    stream->readBits(32, &m_fEmitLife);
	stream->readBits(32, &m_fGlow);
    stream->readBits(96, &m_vSelfIllumination);
    stream->readBits(32, &m_fPowValue);

	stream->readBits(32, &m_fTexRemainTime);
	stream->readBits(32, &m_fFlickerInterval);
    stream->readBits(32, &m_fFlickerIntervalRand);
	stream->readBits(32, &m_fFlickerTime);
    stream->readBits(32, &m_fFlickerTimeRand);
	m_iParticleRot = stream->readInt(8);
	m_bRepeat = stream->readFlag();
	m_bFollow = stream->readFlag();
	m_iTextureStageState = stream->readInt(8);
    stream->readBits(32, &m_fDisplayVelocity);
	m_bHalfCut = stream->readFlag();
    m_bHaveTail = stream->readFlag();
	m_bEmitByRadius = stream->readFlag();
    stream->readBits(32, &m_fEmitRadius);
    stream->readBits(32, &m_fEmitRadiusOffset);

	temp_num = stream->readInt(16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecAnimationTimePoint[i]);
	}
	temp_num = stream->readInt(16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(96, &m_vecAnimationPosition[i]);
	}

    m_bModelEmit = stream->readFlag();
	temp_num = stream->readInt(16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(96, &m_vecModelVertex[i]);
        stream->readBits(96, &m_vecModelNormal[i]);
	}

	temp_num = stream->readInt(16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(96,&m_vecScalePoint[i]);
		stream->readBits(32,&m_vecScaleTimePoint[i]);
	}
    m_bSizeRandomBothXAndY = stream->readFlag();

	m_bElectricity = stream->readFlag();
	stream->readBits(32,&m_fGlowStrength);
	stream->readBits(32,&m_fHeight);
	stream->readBits(32,&m_fGlowFallOff);
	stream->readBits(32,&m_fSpeed);
	stream->readBits(32,&m_fSampleDist);
	stream->readBits(32,&m_fAmbientGlow);
	stream->readBits(32,&m_fAmbientGlowHeightScale);
	stream->readBits(32,&m_fVertNoise);
#ifndef NTJ_SERVER
	reload();
#endif
}

bool ParticleData::onAdd()
{
	if (Parent::onAdd() == false)
		return false;
	if (!m_TextureHandle) 
	{
		Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) with no texture", getName());
	}
	if (m_fLifetime < 0.0f) 
	{
		Con::errorf(ConsoleLogEntry::General, "ParticleData(%s) Lifetime less than 0", getName());
        return false;
	}

	if( m_fDisplayVelocity < 0 )
	{
		Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) period < 1 ms", getName());
		m_fDisplayVelocity = 1.0f;
	}

#ifndef NTJ_SERVER
    reload();
#endif

	return true;
}

bool ParticleData::preload(bool server, char errorBuffer[256])
{
	if (Parent::preload(server, errorBuffer) == false)
		return false;

	//bool error = false;
	//if(!server)
	//{
	//	// Here we attempt to load the particle's texture if specified. An undefined
	//	// texture is *not* an error since the emitter may provide one.
	//	if (m_TextureName.length() > 0)
	//	{
	//		m_TextureHandle = GFXTexHandle(m_TextureName.c_str(), &GFXDefaultStaticDiffuseProfile);
	//		if (!m_TextureHandle)
	//		{
	//			dSprintf(errorBuffer, 256, "Missing particle texture: %s", m_TextureName.c_str());
	//			error = true;
	//		}
	//	}
	//}
	return true;
}

void  ParticleData::initPersistFields()
{
	Parent::initPersistFields();

#ifndef SXZPARTSYS
	addField("TextureName",                TypeString,             Offset(m_TextureName,                     ParticleData));
	addField("Lifetime",                   TypeF32,                Offset(m_fLifetime,                       ParticleData));
    addField("LifetimeRand",               TypeF32,                Offset(m_fLifetimeRand,                   ParticleData));
	addField("vecSizeXPoint",              TypeF32Vector,          Offset(m_vecSizeXPoint,                   ParticleData));
    addField("vecSizeXRand",               TypeF32Vector,          Offset(m_vecSizeXRand,                    ParticleData));
	addField("vecSizeXTimePoint",          TypeF32Vector,          Offset(m_vecSizeXTimePoint,               ParticleData));
	addField("vecSizeYPoint",              TypeF32Vector,          Offset(m_vecSizeYPoint,                   ParticleData));
	addField("vecSizeYRand",               TypeF32Vector,          Offset(m_vecSizeYRand,                    ParticleData));
	addField("vecSizeYTimePoint",          TypeF32Vector,          Offset(m_vecSizeYTimePoint,               ParticleData));
	addField("vecEula",                    TypePoint3FVector,      Offset(m_vecEula,                         ParticleData));
	addField("vecEulaRand",                TypePoint3FVector,      Offset(m_vecEulaRand,                     ParticleData));
	addField("vecEulaTimePoint",           TypeF32Vector,          Offset(m_vecEulaTimePoint,                ParticleData));
    
	addField("VelocityPoint",              TypeF32Vector,          Offset(m_vecVelocityPoint,                ParticleData));
    addField("VelocityRand",               TypeF32Vector,          Offset(m_vecVelocityRand,                 ParticleData));
	addField("VelocityTimePoint",          TypeF32Vector,          Offset(m_vecVelocityTimePoint,            ParticleData));

	addField("RepulseVelocityPoint",       TypeF32Vector,          Offset(m_vecRepulseVelocityPoint,         ParticleData));
	addField("RepulseVelocityRand",        TypeF32Vector,          Offset(m_vecRepulseVelocityRand,          ParticleData));
	addField("RepulseVelocityTimePoint",   TypeF32Vector,          Offset(m_vecRepulseVelocityTimePoint,     ParticleData));

	addField("vecColorPoint",              TypeColorFVector,       Offset(m_vecColorPoint,                   ParticleData));
	addField("vecColorPointRand",          TypeColorFVector,       Offset(m_vecColorPointRand,               ParticleData));
	addField("vecColorTimePoint",          TypeF32Vector,          Offset(m_vecColorTimePoint,               ParticleData));

	addField("vecAlphaPoint",              TypeF32Vector,          Offset(m_vecAlphaPoint,                   ParticleData));
	addField("vecAlphaTimePoint",          TypeF32Vector,          Offset(m_vecAlphaTimePoint,               ParticleData));

	addField("vecAnglePoint",              TypeF32Vector,          Offset(m_vecAnglePoint,                   ParticleData));
    addField("vecAnglePointRand",          TypeF32Vector,          Offset(m_vecAnglePointRand,               ParticleData));
	addField("vecAngleTimePoint",          TypeF32Vector,          Offset(m_vecAngleTimePoint,               ParticleData));

	addField("Gravity",                    TypeF32Vector,          Offset(m_fGravity,                        ParticleData));
    addField("GravityRand",                TypeF32Vector,          Offset(m_fGravityRand,                    ParticleData));
    addField("GravityTimePoint",           TypeF32Vector,          Offset(m_vecGravityTimePoint,             ParticleData));

	addField("vecEmitTimeInterval",         TypeF32Vector,          Offset(m_vecEmitTimeInterval,             ParticleData));
	addField("vecEmitTimeIntervalRand",     TypeF32Vector,          Offset(m_vecEmitTimeIntervalRand,         ParticleData));
	addField("vecEmitTimeIntervalTimePoint",TypeF32Vector,          Offset(m_vecEmitTimeIntervalTimePoint,    ParticleData));

	addField("PosDeviation",               TypePoint3FVector,      Offset(m_vecPosDeviation,                 ParticleData));
	addField("vecPosDeviationTimePoint",   TypeF32Vector,          Offset(m_vecPosDeviationTimePoint,        ParticleData));
    addField("vecEmitState",               TypeBoolVector,         Offset(m_vecEmitState,                    ParticleData));
    addField("vecEmitStateTimePoint",      TypeF32Vector,          Offset(m_vecEmitStateTimePoint,           ParticleData));

	addField("PerEmitNum",                 TypeS32,                Offset(m_iPerEmitNum,                     ParticleData));
	addField("Billboard",                  TypeBool,               Offset(m_bBillboard,                      ParticleData));

	addField("vecRotTimePoint",            TypeF32Vector,          Offset(m_vecRotTimePoint,                 ParticleData));
	addField("vecRotValue",                TypePoint3FVector,      Offset(m_vecRotValue,                     ParticleData));
	addField("vecRotValueRand",            TypePoint3FVector,      Offset(m_vecRotValueRand,                 ParticleData));
	
	addField("TextureSplinter",            TypeBool,               Offset(m_bTextureSplinter,                ParticleData));
	addField("TexSplinterBroadRandom",     TypeBool,               Offset(m_bTexSplinterBroadRandom,         ParticleData));
	
	addField("TextureSplinterStageNumLength",              TypeS32,      Offset(m_iTextureSplinterStageNumLength,  ParticleData));
	addField("TextureSplinterStageNumWidth",               TypeS32,      Offset(m_iTextureSplinterStageNumWidth,   ParticleData));
    addField("TextureStageState",          TypeS32,                Offset(m_iTextureStageState,              ParticleData));
    addField("MaxParticleNum",             TypeS32,                Offset(m_dwMaxParticleNum,                ParticleData));
    addField("EmitLife",                   TypeF32,                Offset(m_fEmitLife,                       ParticleData));
    addField("Glow",                       TypeF32,                Offset(m_fGlow,                           ParticleData));
    addField("SelfIllumination",           TypePoint3F,            Offset(m_vSelfIllumination,               ParticleData));
    addField("PowValue",                   TypeF32,                Offset(m_fPowValue,                       ParticleData));

	addField("TexRemainTime",              TypeF32,                Offset(m_fTexRemainTime,                  ParticleData));
	addField("FlickerInterval",            TypeF32,                Offset(m_fFlickerInterval,                ParticleData));
    addField("FlickerIntervalRand",        TypeF32,                Offset(m_fFlickerIntervalRand,            ParticleData));
	addField("FlickerTime",                TypeF32,                Offset(m_fFlickerTime,                    ParticleData));
    addField("FlickerTimeRand",            TypeF32,                Offset(m_fFlickerTimeRand,                ParticleData));
	addField("ParticleRot",                TypeS32,                Offset(m_iParticleRot,                    ParticleData));
	addField("Repeat",                     TypeBool,               Offset(m_bRepeat,                         ParticleData));
    addField("Follow",                     TypeBool,               Offset(m_bFollow,                         ParticleData));
	addField("DisplayVelocity",            TypeF32,                Offset(m_fDisplayVelocity,                ParticleData));
    addField("HalfCut",                    TypeBool,               Offset(m_bHalfCut,                        ParticleData));
    addField("HaveTail",                   TypeBool,               Offset(m_bHaveTail,                       ParticleData));
    addField("EmitByRadius",               TypeBool,               Offset(m_bEmitByRadius,                   ParticleData));
    addField("EmitRadius",                 TypeF32,                Offset(m_fEmitRadius,                     ParticleData));
    addField("EmitRadiusOffset",           TypeF32,                Offset(m_fEmitRadiusOffset,               ParticleData));

	addField("AnimationTimePoint",         TypeF32Vector,          Offset(m_vecAnimationTimePoint,           ParticleData));
	addField("AnimationPosition",          TypePoint3FVector,      Offset(m_vecAnimationPosition,            ParticleData));

    addField("ModelEmit",                  TypeBool,               Offset(m_bModelEmit,                      ParticleData));
    addField("ModelVertex",                TypePoint3FVector,      Offset(m_vecModelVertex,                  ParticleData));
    addField("ModelNormal",                TypePoint3FVector,      Offset(m_vecModelNormal,                  ParticleData));

	addField("ScalePoint",                 TypePoint3FVector,      Offset(m_vecScalePoint,                   ParticleData));
	addField("ScaleTimePoint",             TypeF32Vector,          Offset(m_vecScaleTimePoint,               ParticleData));
    addField("SizeRandomBothXAndY",        TypeBool,               Offset(m_bSizeRandomBothXAndY,            ParticleData));

    addField("Electricity",                TypeBool,               Offset(m_bElectricity,                    ParticleData));
    addField("GlowStrength",               TypeF32,                Offset(m_fGlowStrength,                   ParticleData));
    addField("Height",                     TypeF32,                Offset(m_fHeight,                         ParticleData));
    addField("GlowFallOff",                TypeF32,                Offset(m_fGlowFallOff,                    ParticleData));
    addField("Speed",                      TypeF32,                Offset(m_fSpeed,                          ParticleData));
    addField("SampleDist",                 TypeF32,                Offset(m_fSampleDist,                     ParticleData));
    addField("AmbientGlow",                TypeF32,                Offset(m_fAmbientGlow,                    ParticleData));
    addField("AmbientGlowHeightScale",     TypeF32,                Offset(m_fAmbientGlowHeightScale,         ParticleData));
    addField("VertNoise",                  TypeF32,                Offset(m_fVertNoise,                      ParticleData));

#endif
}

bool ParticleData::reload()
{
	DELS(m_fTextureCoordUV1);
	DELS(m_fTextureCoordUV2);
	DELS(m_fTextureCoordUV3);
	DELS(m_fTextureCoordUV4);

	if ((m_iTextureSplinterStageNumLength > 0) && (m_iTextureSplinterStageNumWidth > 0))
	{
		m_iTextureSplinterNum = m_iTextureSplinterStageNumWidth* m_iTextureSplinterStageNumLength;
		m_fTextureCoordUV1 = new Point2F[m_iTextureSplinterNum];
		m_fTextureCoordUV2 = new Point2F[m_iTextureSplinterNum];
		m_fTextureCoordUV3 = new Point2F[m_iTextureSplinterNum];
		m_fTextureCoordUV4 = new Point2F[m_iTextureSplinterNum];

		S32 temp_line, temp_col;
		F32 temp_f1 = 1.0f / F32(m_iTextureSplinterStageNumLength);
		F32 temp_f2 = 1.0f / F32(m_iTextureSplinterStageNumWidth);

		for (S32 i = 0; i < m_iTextureSplinterNum; i++)
		{
			temp_line = i / m_iTextureSplinterStageNumLength;
			temp_col = i % m_iTextureSplinterStageNumLength;

			m_fTextureCoordUV1[i].x = temp_col * temp_f1;
			m_fTextureCoordUV1[i].y = temp_line * temp_f2;
			m_fTextureCoordUV2[i].x = (temp_col + 1) * temp_f1;
			m_fTextureCoordUV2[i].y = m_fTextureCoordUV1[i].y;
			m_fTextureCoordUV3[i].x = m_fTextureCoordUV1[i].x;
			m_fTextureCoordUV3[i].y = (temp_line + 1) * temp_f2;
			m_fTextureCoordUV4[i].x = m_fTextureCoordUV2[i].x;
			m_fTextureCoordUV4[i].y = m_fTextureCoordUV3[i].y;
		}
	}

#ifndef SXZPARTSYS
	if(!m_bElectricity)
	{
		m_TextureHandle.free();
		m_TextureHandle = GFXTexHandle(m_TextureName, &GFXDefaultStaticDiffuseProfile);
		if (!m_TextureHandle)
		{
			return false;
		}
	}
	else
	{
		SAFE_RELEASE(m_pTexture);
		GFX->CreateVolumeTexture(m_TextureName, &m_pTexture);
		if (!m_pTexture)
		{
			return false;
		}
	}
#endif

	return true;
}

ConsoleMethod(ParticleData, reload, void, 2, 2, "(void)"
			  "Reloads this particle")
{
#ifndef NTJ_SERVER
	object->reload();
#endif
}

bool ParticleData::GetAnimationPosition(F32 time, Point3F* pos)
{
	for(S32 i2 = 0; i2 < m_vecAnimationTimePoint.size() - 1; i2++)
	{
		if ((m_vecAnimationTimePoint[i2] <= time) && (m_vecAnimationTimePoint[i2 + 1] > time))
		{
			*pos = m_vecAnimationPosition[i2] + (time-m_vecAnimationTimePoint[i2])/(m_vecAnimationTimePoint[i2 + 1] - m_vecAnimationTimePoint[i2]) * (m_vecAnimationPosition[i2 + 1] - m_vecAnimationPosition[i2]);
			return true;
		}
		else if (m_vecAnimationTimePoint[i2] > time)
		{
			return false;
		}
		else 
		{
			continue;
		}
	}
	return false;
}

IMPLEMENT_CO_DATABLOCK_V1(Ribbon);

Ribbon::Ribbon()
{
    m_TextureHandle = NULL;
	m_TextureName = NULL;
	m_vecSize = 0;
	m_vecSizeRand = 0;
	m_vecSizeTimePoint = 0;
	m_vecEula = 0;
	m_vecEulaRand = 0;
	m_vecEulaTimePoint = 0;
	m_vecColor = 0;
	m_vecColorRand = 0;
	m_vecColorTimePoint = 0;
	m_vecAlphaPoint = 0;                       
	m_vecAlphaTimePoint = 0;   
	m_fGlow = 1.0f;
    m_bRepeat = true;
	m_fDisplayVelocity = 1.0f;
	m_vecAnimationTimePoint = 0;
	m_vecAnimationPosition = 0;
    m_bFollow = false;
    m_iRibbonNum = 0;
	m_iTextureStageState = 1;
	m_fEmitLife = 0.0f;
	m_vScale = Point3F(1.0f, 1.0f, 1.0f);
	m_vSelfIllumination = Point3F(0.0f, 0.0f, 0.0f);
	m_fPowValue = 1.0f;
}

Ribbon::~Ribbon()
{
	m_vecSize.clear();
	m_vecSizeRand.clear();
	m_vecSizeTimePoint.clear();
	m_vecEula.clear();
	m_vecEulaRand.clear();
	m_vecEulaTimePoint.clear();
	m_vecColor.clear();
	m_vecColorRand.clear();
	m_vecColorTimePoint.clear();
	m_vecAlphaPoint.clear();                      
	m_vecAlphaTimePoint.clear();
	m_vecAnimationTimePoint.clear();
	m_vecAnimationPosition.clear();
	m_TextureHandle = NULL;
}

void Ribbon::packData(BitStream* stream)
{
	S32 temp_num;
	Parent::packData(stream);
	if (stream->writeFlag(m_TextureName && m_TextureName[0]))
		stream->writeString(m_TextureName);

	temp_num = m_vecSizeTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecSize[i]);
		stream->writeBits(32, &m_vecSizeRand[i]);
		stream->writeBits(32, &m_vecSizeTimePoint[i]);
	}

	temp_num = m_vecEulaTimePoint.size();
	stream->writeInt(temp_num, 16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(96, &m_vecEula[i]);
		stream->writeBits(96, &m_vecEulaRand[i]);
		stream->writeBits(32,&m_vecEulaTimePoint[i]);
	}
	
	temp_num = m_vecColorTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(128, &m_vecColor[i]);
		stream->writeBits(128, &m_vecColorRand[i]);
		stream->writeBits(32, &m_vecColorTimePoint[i]);
	}
	
	temp_num = m_vecAlphaTimePoint.size();
	stream->writeInt(temp_num, 8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32, &m_vecAlphaPoint[i]);
		stream->writeBits(32, &m_vecAlphaTimePoint[i]);
	}

	stream->writeBits(32, &m_fGlow);
	stream->writeBits(96, &m_vSelfIllumination);
	stream->writeBits(32, &m_fPowValue);
	stream->writeFlag(m_bRepeat);
	stream->writeFlag(m_bFollow);
	stream->writeInt(m_iTextureStageState, 8);
	stream->writeBits(32, &m_fDisplayVelocity);
    stream->writeInt(m_iRibbonNum, 32);
	stream->writeBits(32, &m_fEmitLife);
	stream->writeBits(96, m_vScale);

	temp_num = m_vecAnimationTimePoint.size();
	stream->writeInt(temp_num, 16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(32,&m_vecAnimationTimePoint[i]);
	}
	temp_num = m_vecAnimationPosition.size();
	stream->writeInt(temp_num, 16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->writeBits(96,&m_vecAnimationPosition[i]);
	}
}

void Ribbon::unpackData(BitStream* stream)
{
	S32 temp_num;
	Parent::unpackData(stream);
	m_TextureName = (stream->readFlag()) ? stream->readSTString() : 0;
	
	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecSize[i]);
		stream->readBits(32, &m_vecSizeRand[i]);
		stream->readBits(32, &m_vecSizeTimePoint[i]);
	}

	temp_num = stream->readInt(16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(96, &m_vecEula[i]);
		stream->readBits(96, &m_vecEulaRand[i]);
		stream->readBits(32, &m_vecEulaTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(128, &m_vecColor[i]);
		stream->readBits(128, &m_vecColorRand[i]);
		stream->readBits(32, &m_vecColorTimePoint[i]);
	}

	temp_num = stream->readInt(8);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecAlphaPoint[i]);
		stream->readBits(32, &m_vecAlphaTimePoint[i]);
	}

	stream->readBits(32, &m_fGlow);
	stream->readBits(96, &m_vSelfIllumination);
	stream->writeBits(32, &m_fPowValue);
	m_bRepeat = stream->readFlag();
	m_bFollow = stream->readFlag();
	m_iTextureStageState = stream->readInt(8);
	stream->readBits(32, &m_fDisplayVelocity);
	m_iRibbonNum = stream->readInt(32);
	stream->readBits(32, &m_fEmitLife);
	stream->readBits(96, &m_vScale);

	temp_num = stream->readInt(16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(32, &m_vecAnimationTimePoint[i]);
	}
	temp_num = stream->readInt(16);
	for (S32 i = 0; i < temp_num; i++)
	{
		stream->readBits(96, &m_vecAnimationPosition[i]);
	}

#ifndef NTJ_SERVER
	reload();
#endif
}

bool Ribbon::onAdd()
{
	if (Parent::onAdd() == false)
		return false;
	if (!m_TextureHandle) 
	{
		Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) with no texture", getName());
	}

	if( m_fDisplayVelocity < 0 )
	{
		Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) period < 1 ms", getName());
		m_fDisplayVelocity = 1.0f;
	}

#ifndef NTJ_SERVER
	reload();
#endif

	return true;
}

bool Ribbon::preload(bool server, char errorBuffer[256])
{
	if (Parent::preload(server, errorBuffer) == false)
		return false;

	//bool error = false;
	//if(!server)
	//{
	//	// Here we attempt to load the particle's texture if specified. An undefined
	//	// texture is *not* an error since the emitter may provide one.
	//	if (m_TextureName.length() > 0)
	//	{
	//		m_TextureHandle = GFXTexHandle(m_TextureName.c_str(), &GFXDefaultStaticDiffuseProfile);
	//		if (!m_TextureHandle)
	//		{
	//			dSprintf(errorBuffer, 256, "Missing particle texture: %s", m_TextureName.c_str());
	//			error = true;
	//		}
	//	}
	//}
	return true;
}

bool Ribbon::GetAnimationPosition(F32 time, Point3F* pos)
{
	for(S32 i2 = 0; i2 < m_vecAnimationTimePoint.size() - 1; i2++)
	{
		if ((m_vecAnimationTimePoint[i2] <= time) && (m_vecAnimationTimePoint[i2 + 1] > time))
		{
			*pos = m_vecAnimationPosition[i2] + (time-m_vecAnimationTimePoint[i2])/(m_vecAnimationTimePoint[i2 + 1] - m_vecAnimationTimePoint[i2]) * (m_vecAnimationPosition[i2 + 1] - m_vecAnimationPosition[i2]);
			return true;
		}
		else if (m_vecAnimationTimePoint[i2] > time)
		{
			return false;
		}
		else 
		{
			continue;
		}
	}
	return false;
}

void  Ribbon::initPersistFields()
{
	Parent::initPersistFields();

#ifndef SXZPARTSYS
	addField("TextureName",                TypeString,             Offset(m_TextureName,                     Ribbon));
	addField("vecSize",                    TypeF32Vector,          Offset(m_vecSize,                         Ribbon));
	addField("vecSizeRand",                TypeF32Vector,          Offset(m_vecSizeRand,                     Ribbon));
	addField("vecSizeTimePoint",           TypeF32Vector,          Offset(m_vecSizeTimePoint,                Ribbon));
	addField("vecEula",                    TypePoint3FVector,      Offset(m_vecEula,                         Ribbon));
	addField("vecEulaRand",                TypePoint3FVector,      Offset(m_vecEulaRand,                     Ribbon));
	addField("vecEulaTimePoint",           TypeF32Vector,          Offset(m_vecEulaTimePoint,                Ribbon));
	addField("vecColor",                   TypeColorFVector,       Offset(m_vecColor,                        Ribbon));
	addField("vecColorRand",               TypeColorFVector,       Offset(m_vecColorRand,                    Ribbon));
	addField("vecColorTimePoint",          TypeF32Vector,          Offset(m_vecColorTimePoint,               Ribbon));
	addField("vecAlphaPoint",              TypeF32Vector,          Offset(m_vecAlphaPoint,                   Ribbon));
	addField("vecAlphaTimePoint",          TypeF32Vector,          Offset(m_vecAlphaTimePoint,               Ribbon));

	addField("TextureStageState",          TypeS32,                Offset(m_iTextureStageState,              Ribbon));
	addField("Glow",                       TypeF32,                Offset(m_fGlow,                           Ribbon));
	addField("SelfIllumination",           TypePoint3F,            Offset(m_vSelfIllumination,               Ribbon));
	addField("PowValue",                   TypeF32,                Offset(m_fPowValue,                       Ribbon));
	addField("Repeat",                     TypeBool,               Offset(m_bRepeat,                         Ribbon));
	addField("Follow",                     TypeBool,               Offset(m_bFollow,                         Ribbon));
	addField("DisplayVelocity",            TypeF32,                Offset(m_fDisplayVelocity,                Ribbon));
    addField("RibbonNum",                  TypeS32,                Offset(m_iRibbonNum,                      Ribbon));
    addField("EmitLife",                   TypeF32,                Offset(m_fEmitLife,                       Ribbon));
	addField("Scale",                      TypePoint3F,            Offset(m_vScale,                          Ribbon));

	addField("AnimationTimePoint",         TypeF32Vector,          Offset(m_vecAnimationTimePoint,           Ribbon));
	addField("AnimationPosition",          TypePoint3FVector,      Offset(m_vecAnimationPosition,            Ribbon));
#endif
}

bool Ribbon::reload()
{
#ifndef SXZPARTSYS
	m_TextureHandle.free();
	m_TextureHandle = GFXTexHandle(m_TextureName, &GFXDefaultStaticDiffuseProfile);
	if (!m_TextureHandle)
	{
		return false;
	}
#endif

	return true;
}

ConsoleMethod(Ribbon, reload, void, 2, 2, "(void)"
			  "Reloads this Ribbon")
{
#ifndef NTJ_SERVER
	object->reload();
#endif
}