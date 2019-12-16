//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/D3D9/gfxD3D9Effect.h"
#include "platform/platform.h"
#include "T3D/fx/cameraFXMgr.h"
#include "math/mRandom.h"
#include "math/mMatrix.h"
#include <assert.h>
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "lightingSystem/synapseGaming/sgFormatManager.h"
#include "platform/platformTimer.h"
#include "gfx/primBuilder.h"

// global cam fx
CameraFXManager gCamFXMgr;
CameraShakeEX gCameraShakeEX;
IMPLEMENT_CO_DATABLOCK_V1(CameraFX);

//**************************************************************************
// Camera effect
//**************************************************************************
CameraFX::CameraFX()
{
	mFreq.zero();
	mDuration = 1.0;
	mStartAmp.zero();
	mFalloff = 10.0;
}

IMPLEMENT_CONSOLETYPE(CameraFX)
IMPLEMENT_SETDATATYPE(CameraFX)
IMPLEMENT_GETDATATYPE(CameraFX)

bool CameraFX::onAdd()
{
	if (Parent::onAdd() == false)
		return false;

	if (mDuration < 0.0f)
	{
		Con::warnf(ConsoleLogEntry::General, "CameraFX(%s) mDuration < 0.0", getName());
		mDuration = 0.0f;
	}
	if (mFalloff < 0.0f)
	{
		Con::warnf(ConsoleLogEntry::General, "CameraFX(%s) mFalloff < 0.0", getName());
		mFalloff = 0.0f;
	}

	return true;
}

bool CameraFX::preload(bool server, char errorBuffer[256])
{
	if (Parent::preload(server, errorBuffer) == false)
		return false;

	return true;
}

void CameraFX::initPersistFields()
{
	Parent::initPersistFields();

	addField("Duration",  TypeF32,                Offset(mDuration,     CameraFX));
	addField("Freq",      TypePoint3F,            Offset(mFreq,         CameraFX));
	addField("StartAmp",  TypePoint3F,            Offset(mStartAmp,     CameraFX));
	addField("Falloff",   TypeF32,                Offset(mFalloff,      CameraFX));
}

void CameraFX::packData(BitStream* stream)
{
	Parent::packData(stream);
	stream->writeBits(32, &mDuration);
	stream->writeBits(96, mFreq);
	stream->writeBits(96, mStartAmp);
	stream->writeBits(32, &mFalloff);
}

void CameraFX::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
    stream->readBits(32, &mDuration);
    stream->readBits(96, mFreq);
	stream->readBits(96, mStartAmp);
	stream->readBits(32, &mFalloff);
}

//**************************************************************************
// Camera shake effect
//**************************************************************************
CameraShake::CameraShake()
{
   mAmp.zero();
   mTimeOffset.zero();
   mCamFXTrans.identity();
   mElapsedTime = 0.0;
   m_pCameraFX = NULL;
}

CameraShake::~CameraShake()
{
	if (m_bByEditor)
	{
		delete m_pCameraFX;
	}
}

//--------------------------------------------------------------------------
// Update
//--------------------------------------------------------------------------
void CameraShake::update( F32 dt )
{
	if(m_pCameraFX)
	{
		mElapsedTime += dt;

		if(!isExpired())
		{
			fadeAmplitude();

			VectorF camOffset;
			camOffset.x = mAmp.x * sin( M_2PI * (mTimeOffset.x + mElapsedTime) * m_pCameraFX->mFreq.x );
			camOffset.y = mAmp.y * sin( M_2PI * (mTimeOffset.y + mElapsedTime) * m_pCameraFX->mFreq.y );
			camOffset.z = mAmp.z * sin( M_2PI * (mTimeOffset.z + mElapsedTime) * m_pCameraFX->mFreq.z );

			VectorF rotAngles;
			rotAngles.x = camOffset.x * 10.0 * M_PI/180.0;
			rotAngles.y = camOffset.y * 10.0 * M_PI/180.0;
			rotAngles.z = camOffset.z * 10.0 * M_PI/180.0;
			MatrixF rotMatrix( EulerF( rotAngles.x, rotAngles.y, rotAngles.z ) );

			mCamFXTrans = rotMatrix;
			mCamFXTrans.setPosition( camOffset );
		}
	}
}

//--------------------------------------------------------------------------
// Fade out the amplitude over time
//--------------------------------------------------------------------------
void CameraShake::fadeAmplitude()
{
   F32 percentDone = (mElapsedTime / m_pCameraFX->mDuration);
   if( percentDone > 1.0 ) percentDone = 1.0;

   F32 time = 1 + percentDone * m_pCameraFX->mFalloff;
   time = 1 / (time * time);

   mAmp = m_pCameraFX->mStartAmp * time;
}

//--------------------------------------------------------------------------
// Initialize
//--------------------------------------------------------------------------
void CameraShake::newinit(CameraFX* cf, bool ByEditor)
{
   mTimeOffset.x = 0.0;
   mTimeOffset.y = gRandGen.randF();
   mTimeOffset.z = gRandGen.randF();
   m_pCameraFX = cf;
   m_bByEditor = ByEditor;
}

//**************************************************************************
// Camera Slerp effect
//**************************************************************************
CameraSlerp::CameraSlerp()
{
	clear();
}

CameraSlerp::~CameraSlerp()
{
}

void CameraSlerp::clear()
{
	mDuration = 0;
	mElapsedTime = 0;
	mOffset.zero();
	mCurOffset.zero();
}

void CameraSlerp::setSlerp(F32 dur, Point3F& offset)
{
	mDuration = getMax(mDuration-mElapsedTime, dur);
	mElapsedTime = 0;
	if(mDuration > 0)
	{
		mCurOffset = mCurOffset + offset;
		mOffset = mCurOffset;
	}
}

void CameraSlerp::update(F32 dt)
{
	if(mElapsedTime >= mDuration)
		return;
	mElapsedTime += dt;
	if(mElapsedTime >= mDuration)
	{
		clear();
		return;
	}
	mCurOffset = mOffset * ((mDuration - mElapsedTime)/mDuration);
}

//**************************************************************************
// CameraFXManager
//**************************************************************************
CameraFXManager::CameraFXManager()
{
   mCamFXTrans.identity();
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
CameraFXManager::~CameraFXManager()
{
   clear();
}

//--------------------------------------------------------------------------
// Add new effect to currently running list
//--------------------------------------------------------------------------
void CameraFXManager::addFX( char* datablockname )
{
	CameraShake *newFX = new CameraShake;
	CameraFX* tempCameraFX;
	if( Sim::findObject(datablockname, tempCameraFX) == false )
		Con::errorf(ConsoleLogEntry::General, "CameraFXManager::addFX: Invalid packet, bad datablockname(tempCameraFX): %d", datablockname);
	newFX->newinit(tempCameraFX);
	mFXList.link( newFX );
}

void CameraFXManager::addFX( S32 id )
{
	CameraShake *newFX = new CameraShake;
	CameraFX* tempCameraFX;
	if( Sim::findObject(id, tempCameraFX) == false )
		Con::errorf(ConsoleLogEntry::General, "CameraFXManager::addFX: Invalid packet, bad datablockId(tempCameraFX): %d", id);
	newFX->newinit(tempCameraFX);
	mFXList.link( newFX );
}

void CameraFXManager::addFX( F32 Duration, VectorF Freq, VectorF StartAmp, F32 Falloff)
{
	CameraFX* tempCameraFX = new CameraFX;
	tempCameraFX->mDuration = Duration;
	tempCameraFX->mFreq = Freq;
	tempCameraFX->mStartAmp = StartAmp;
	tempCameraFX->mFalloff = Falloff;
	CameraShake *newFX = new CameraShake;
	newFX->newinit(tempCameraFX, true);
	mFXList.link( newFX );
}

//--------------------------------------------------------------------------
// Clear all currently running camera effects
//--------------------------------------------------------------------------
void CameraFXManager::clear()
{
   mFXList.free();
}

//--------------------------------------------------------------------------
// Update camera effects
//--------------------------------------------------------------------------
void CameraFXManager::update( F32 dt )
{
   CameraShakePtr *cur = NULL;
   mCamFXTrans.identity();

   for( cur = mFXList.next( cur ); cur; cur = mFXList.next( cur ) )
   {
      CameraShake * curFX = *cur;
      curFX->update( dt );
      MatrixF fxTrans = curFX->getTrans();

      mCamFXTrans.mul( fxTrans );

      if( curFX->isExpired() )
      {
         CameraShakePtr *prev = mFXList.prev( cur );
         mFXList.free( cur );
         cur = prev;
      }
   }

   mCamSlerp.update(dt);
}

GFXD3D9Effect* CameraShakeEX::m_pEffect = NULL;

CameraShakeEX::CameraShakeEX()
{
    m_fSpeed = 21.0f;
	m_fShake = 0.25f;
	m_fSharpness = 2.2f;
	mVertBuff = NULL;
	m_fShakeTime = 0.0f;
}

CameraShakeEX::~CameraShakeEX()
{
	mVertBuff = NULL;
	SAFE_DELETE(m_pEffect);
}

void CameraShakeEX::Render()
{ 
	if(m_fShakeTime > 0.0f) 
	{
		static bool init = false;
		Point2I cursize = gClientSceneGraph->getDisplayTargetResolution();
		if (!init)
		{
			m_pEffect = new GFXD3D9Effect;
			if(!m_pEffect->init("gameres/shaders/post_cameraShake.fx", NULL))
				assert(0);

			init = true;
		}

		GFX->copyBBToSfxBuff();

		GFX->setCullMode(GFXCullNone);
		GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMipFilter(0, GFXTextureFilterNone);
		GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
		GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
		GFX->setLightingEnable(false);
		GFX->setAlphaBlendEnable(false);
		GFX->setZEnable(false);
		GFX->setZWriteEnable(false);

		m_pEffect->setFloat( "Speed", m_fSpeed );
		static float time = 0.0f;
		float etime = (float(TimeManager::getDeltaTime()) * 0.001f);
		time += etime;
        m_fShakeTime -= etime;

		m_pEffect->setFloat( "Time", time );
		m_pEffect->setFloat( "Shake", m_fShake );
		m_pEffect->setFloat( "Sharpness", m_fSharpness );
		m_pEffect->setTexture( "g_txScene", GFX->getSfxBackBuffer() );

		RectI rect(-1, 1, 1, -1);
		mVertex[0].point = Point4F(rect.point.x, rect.point.y, 0.0f, 1.0f);
		mVertex[0].texCoord = Point2F(0.0f, 0.0f);
		mVertex[1].point = Point4F(rect.extent.x, rect.point.y, 0.0f, 1.0f);
		mVertex[1].texCoord = Point2F(1.0f, 0.0f);
		mVertex[2].point = Point4F(rect.point.x, rect.extent.y, 0.0f, 1.0f);
		mVertex[2].texCoord = Point2F(0.0f, 1.0f);
		mVertex[3].point = Point4F(rect.extent.x, rect.extent.y, 0.0f, 1.0f);
		mVertex[3].texCoord = Point2F(1.0f, 1.0f);

		m_pEffect->setTechnique( "CamShake" );

		mVertBuff.set(GFX, 4, GFXBufferTypeVolatile);
		GFXVertexP4WT *verts = mVertBuff.lock();
		dMemcpy( verts, &mVertex[0], 4 * sizeof(GFXVertexP4WT) );
		mVertBuff.unlock(); 
		GFX->setVertexBuffer( mVertBuff );
		//GFX->clear(GFXClearTarget, ColorI( 0, 0, 0 ), 1.f, 0 );

		m_pEffect->begin();
		m_pEffect->beginPass( 0 );
		m_pEffect->commit();
		GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
		m_pEffect->endPass();
		m_pEffect->end();

		GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
		GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
		/*PrimBuild::color(ColorF(0.0f, 0.0f, 0.0f));
		PrimBuild::begin(GFXTriangleStrip, 4);
		PrimBuild::texCoord2f(0.0f, 0.0f);
		PrimBuild::vertex2f(rect.point.x, rect.point.y);
		PrimBuild::texCoord2f(1.0f, 0.0f);
		PrimBuild::vertex2f(rect.extent.x, rect.point.y);
		PrimBuild::texCoord2f(0.0f, 1.0f);
		PrimBuild::vertex2f(rect.point.x, rect.extent.y);
		PrimBuild::texCoord2f(1.0f, 1.0f);
		PrimBuild::vertex2f(rect.extent.x, rect.extent.y);
		PrimBuild::end(false);*/
	}
}

void CameraShakeEX::SetConstants(F32 speed, F32 Shake, F32 Sharpness, F32 ShakeTime)
{
    m_fSpeed = speed;
	m_fShake = Shake;
	m_fSharpness = Sharpness;
	m_fShakeTime = ShakeTime;
}

void      CameraShakeEX::shutdown()
{
}