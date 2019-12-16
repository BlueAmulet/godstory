//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CAMERAFXMGR_H_
#define _CAMERAFXMGR_H_

#ifndef _LLIST_H_
#include "core/llist.h"
#endif
#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif
#ifndef _MMATRIX_H_
#include "math/mMatrix.h"
#endif
#ifndef _GAMEBASE_H_
#include "T3D/gameBase.h"
#endif

class GFXD3D9Effect;

//**************************************************************************
// Abstract camera effect template
//**************************************************************************
class CameraFX : public GameBaseData
{
public:
	typedef GameBaseData Parent;
	CameraFX();
	F32          mDuration;
	VectorF      mFreq;      // these are vectors to represent these values in 3D
	VectorF      mStartAmp;
	F32          mFalloff;

	DECLARE_CONOBJECT(CameraFX);
	bool onAdd();
	bool preload(bool server, char errorBuffer[256]);
	static void  initPersistFields();
	virtual void packData(BitStream* stream);
	virtual void unpackData(BitStream* stream);
};

DECLARE_CONSOLETYPE(CameraFX)

//--------------------------------------------------------------------------
// Camera shake effect
//--------------------------------------------------------------------------
class CameraShake 
{
   F32           mElapsedTime;
   VectorF       mAmp;
   VectorF       mTimeOffset;
   MatrixF       mCamFXTrans;
   CameraFX*     m_pCameraFX;
   bool          m_bByEditor;
public:
   CameraShake();
   ~CameraShake();
   void          newinit(CameraFX* cf, bool ByEditor = false);
   void          fadeAmplitude();
   MatrixF &     getTrans(){ return mCamFXTrans; }
   bool          isExpired(){ return mElapsedTime >= m_pCameraFX->mDuration; }
   virtual void  update( F32 dt );
};


//--------------------------------------------------------------------------
// Camera Slerp effect
//--------------------------------------------------------------------------
class CameraSlerp 
{
	F32           mDuration;
	F32           mElapsedTime;
	VectorF       mOffset;
	VectorF       mCurOffset;
public:
	CameraSlerp();
	~CameraSlerp();
	void clear();
	void setSlerp(F32 dur, Point3F& offset);
	void update(F32 dt);
	inline Point3F& getOffset() { return mCurOffset;}
};

//**************************************************************************
// CameraFXManager
//**************************************************************************
class CameraFXManager 
{
   typedef CameraShake * CameraShakePtr;

   LList< CameraShakePtr > mFXList;
   MatrixF              mCamFXTrans;
public:
   CameraSlerp          mCamSlerp;

public:
   void        addFX( char* datablockname );
   void        addFX( S32 id );
   void        addFX( F32 Duration, VectorF Freq, VectorF StartAmp, F32 Falloff); 
   void        clear();
   MatrixF &   getTrans(){ return mCamFXTrans; }
   void        update( F32 dt );

   CameraFXManager();
   ~CameraFXManager();

};

extern CameraFXManager gCamFXMgr;

class CameraShakeEX
{
public:
	CameraShakeEX();
	~CameraShakeEX();

    void       Render();

    void       SetConstants(F32 speed, F32 Shake, F32 Sharpness, F32 ShakeTime);

	static     void      shutdown();
protected:
private:
    F32                                                        m_fSpeed;
    F32                                                        m_fShake;
	F32                                                        m_fSharpness;
	GFXVertexBufferHandle<GFXVertexP4WT>                       mVertBuff;
	GFXVertexP4WT                                              mVertex[4];
	static  GFXD3D9Effect*                                     m_pEffect;
	F32                                                        m_fShakeTime;
};

extern CameraShakeEX gCameraShakeEX;

#endif
