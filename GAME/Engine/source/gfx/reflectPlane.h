//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _REFLECTPLANE_H_
#define _REFLECTPLANE_H_

#include "math/mMath.h"
#include "gfx/gfxTextureHandle.h"

//**************************************************************************
// Reflection plane
//**************************************************************************
class IReflactPlane
{
protected:
	PlaneF         mPlane;
	GFXTexHandle   mReflectTex;

public:
	IReflactPlane(){mReflectTex = NULL;}
	void setPlane( PlaneF &plane ){ mPlane = plane; }
	const PlaneF & getPlane(){ return mPlane; }

	MatrixF getCameraReflection( MatrixF &camTrans );
	MatrixF getFrustumClipProj( MatrixF &modelview );
	GFXTexHandle & getTex(){ return mReflectTex; }
	virtual GFXTexHandle & getDepth() = 0;

	virtual void setupTex( U32 size ) {mReflectTex = NULL;};
	virtual void clearTextures() {mReflectTex = NULL;};
};

class ReflectPlane : public IReflactPlane
{
   GFXTexHandle   mDepthTex;

public:
   ReflectPlane();
   GFXTexHandle & getDepth() { return mDepthTex; }
   void setupTex( U32 size );
   void clearTextures();
};

class ReflectPlane_SingleZ : public IReflactPlane
{
	static GFXTexHandle   mDepthTex;
	static U32            mZSize;
public:
	GFXTexHandle & getDepth() { return mDepthTex; }
	void setupTex( U32 size );

	static void freeResource()
	{
		mDepthTex = NULL;
		mZSize = 0;
	}
};
#endif  // _REFLECTPLANE_H_
