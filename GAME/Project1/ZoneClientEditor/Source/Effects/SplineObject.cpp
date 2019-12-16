//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Effects/SplineObject.h"
#include "console/consoleTypes.h"
#include "core/stringTable.h"
#include "math/mMatrix.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "gfx/primBuilder.h"

#ifdef NTJ_CLIENT
#include "UI/dGuiMouseGamePlay.h"
#endif
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* SplineObject::mSetSB = NULL;
GFXStateBlock* SplineObject::mClearSB = NULL;

static Point3F ControlPoints[SplineBase::CtrlPointMax] =
{
	//Point3F(0.2f, 0.1f, 0),
	//Point3F(-0.2f, 0.0f, 0),
	//Point3F(-0.1f, 0.3f, 0.0f),
	//Point3F(-0.2f, 0.1f, 0),
	//Point3F(0.0f, 0.1f, 0),
	//Point3F(0.1f, -0.1f, 0),
	//Point3F(0.2f, -0.2f, 0),
	//Point3F(-0.2f, -0.1f, 0),
	//Point3F(0.0f, 0.0f, 0),
	//Point3F(-0.1f, 0.2f, 0.0f),
	//Point3F(0.1f, 0.1f, 0.0f),
	//Point3F(0.0f, 0.2f, 0),
	//Point3F(0.2f, -0.1f, 0),
	//Point3F(0.0f, 0.1f, 0),
	//Point3F(-0.1f, -0.2f, 0),
	//Point3F(-0.1f, -0.1f, 0),
	Point3F(0.2f, 0.1f, -0.2f),
	Point3F(-0.2f, 0.0f, -0.1f),
	Point3F(-0.1f, 0.3f, 0.0f),
	Point3F(-0.2f, 0.1f, -0.1f),
	Point3F(0.0f, 0.1f, -0.2f),
	Point3F(0.1f, -0.1f, 0.1f),
	Point3F(0.2f, -0.2f, -0.2f),
	Point3F(-0.2f, -0.1f, -0.1f),
	Point3F(0.0f, 0.0f, -0.2f),
	Point3F(-0.1f, 0.2f, 0.0f),
	Point3F(0.1f, 0.1f, 0.0f),
	Point3F(0.0f, 0.2f, 0.1f),
	Point3F(0.2f, -0.1f, -0.2f),
	Point3F(0.0f, 0.1f, 0.2f),
	Point3F(-0.1f, -0.2f, 0.1f),
	Point3F(-0.1f, -0.1f, -0.1f),
};

// ========================================================================================================================================
//  SplineBase
// ========================================================================================================================================
IMPLEMENT_CONOBJECT(SplineBase);


SplineBase::SplineBase()
{
	mControlPointCount = 0;
	mCurrentControlPoint = 0;
	mDefaultNormal = VectorF(0.0, 1.0, 0.0);
	mSubdivisionCount = 0;
	mCurvePointCount = 0;
	mCurrentCurvePoint = 0;
	mSplineType = B_Spline;
	mShortestDistance = 0;
	mControlPoints = NULL;
	mCurvePoints = NULL;
	mSplineBuilt = false;
	mLengthPerNode = 0.5f;
	mRefreshTime = 300;
	mLastTime = 0;
}


SplineBase::~SplineBase()
{
	SAFE_DELETE_ARRAY(mControlPoints);
	SAFE_DELETE_ARRAY(mCurvePoints);
}


S32 SplineBase::Initialize(S32 controlCntIn, S32 curveSubD, SplineType curveType)
{
	mControlPointCount = controlCntIn ;
	mCurrentControlPoint = 0;
	mDefaultNormal = VectorF(0.0, 1.0, 0.0);
	mSubdivisionCount = curveSubD;
	mCurvePointCount = 0;
	mCurrentCurvePoint = 0;
	mSplineType = curveType;
	mShortestDistance = 0;
	SAFE_DELETE_ARRAY(mControlPoints);
	SAFE_DELETE_ARRAY(mCurvePoints);
	mControlPoints = new ControlPoint[mControlPointCount*2];
	mCurvePoints = new CurveData[mSubdivisionCount * mControlPointCount*2];
	mSplineBuilt = false;
	for (S32 i=0; i< 16; i++)
	{
		mCtrlBegin[i] = 0;
		mCtrlEnd[i] = 0;
	}
	// 需要两次随机
	RandCtrlPoints();
	RandCtrlPoints();
	mRefreshTime = 1500;//0x0FFFFFFF;
	mLastTime = Platform::getVirtualMilliseconds();
	return true;
}

void SplineBase::RandCtrlPoints()
{
	U32 rd = gRandGen.randI() * Platform::getVirtualMilliseconds();
	for (S32 i=0; i< CtrlPointMax; i++)
	{
		mCtrlBegin[i] = mCtrlEnd[i];
		mCtrlEnd[i] = (rd>>i) & 0x0000000F;
	}
}

void SplineBase::BuildSpline() 
{
	if(/*!mSplineBuilt || */mControlPointCount <= 0)
		return;
	F32 u, u_2, u_3, tempScale, tempPos;
	S32 i, j;
	S32 index;
	VectorF distVec;
	VectorF origin(0.0, 0.0, 0.0);

	SimTime runTime = Platform::getVirtualMilliseconds() - mLastTime;
	if(runTime > mRefreshTime)
	{
		if(mRefreshTime == 0)
			mRefreshTime = 1;
		runTime = runTime % mRefreshTime;
		mLastTime = Platform::getVirtualMilliseconds() - runTime;
		RandCtrlPoints();
	}

	mCurvePoints[0].position = mSurPostion;
	mControlPoints[0].position = mSurPostion;
	mControlPoints[mControlPointCount-2].position = mDesPostion;
	mControlPoints[mControlPointCount-1].position = mDesPostion;
	mControlPoints[mControlPointCount].position = mDesPostion;
	distVec = (mDesPostion - mSurPostion)/(mControlPointCount-2);
	tempScale = distVec.len() * 2.5f;
	tempPos = (F32)runTime / mRefreshTime;

	for (i = 1; i < mControlPointCount-2; i++)
	{
		mControlPoints[i].position = mSurPostion + distVec*i + (ControlPoints[mCtrlBegin[i]] * (1.0f - tempPos) + ControlPoints[mCtrlEnd[i]] * tempPos) * tempScale;
	}

	index = 1;
	if(mSplineType == Spline)
	{
		for (i = 1; i < mControlPointCount - 2; i+=2)
		{
			for(j = 0; j < mSubdivisionCount; j++)
			{
				u = (F32)j/ (mSubdivisionCount -1);
				mCurvePoints[index].position.x = Calc_Spline(u,mControlPoints[i].position.x, mControlPoints[i+1].position.x, mControlPoints[i+2].position.x);
				mCurvePoints[index].position.y = Calc_Spline(u,mControlPoints[i].position.y, mControlPoints[i+1].position.y, mControlPoints[i+2].position.y);
				mCurvePoints[index].position.z = Calc_Spline(u,mControlPoints[i].position.z, mControlPoints[i+1].position.z, mControlPoints[i+2].position.z);

				if (j == 0 && i == 0) 
				{
					mCurvePoints[index].distance = 0;
					mShortestDistance = 65536.0;
				}
				else 
				{
					distVec = mCurvePoints[index].position - mCurvePoints[index - 1].position;
					mCurvePoints[index].distance = 
						mCurvePoints[index - 1].distance + (F32)mSqrt(distVec.x * distVec.x + distVec.y * distVec.y + distVec.z * distVec.z);
					if (mCurvePoints[index].distance < mShortestDistance)
						mShortestDistance = mCurvePoints[index].distance;
				}
				index++;
			}
		}
	}
	else	
	{
		for (i = 0; i < mControlPointCount - 3; i++) 
		{
			for(j = 0; j < mSubdivisionCount; j++)
			{
				u = (F32)j / (mSubdivisionCount -1);
				u_2 = u * u;
				u_3 = u_2 * u;
				if (mSplineType == B_Spline) {
					mCurvePoints[index].position.x = Calc_B_Spline(u, u_2, u_3, mControlPoints[i].position.x, mControlPoints[i+1].position.x, mControlPoints[i+2].position.x, mControlPoints[i+3].position.x);
					mCurvePoints[index].position.y = Calc_B_Spline(u, u_2, u_3, mControlPoints[i].position.y, mControlPoints[i+1].position.y, mControlPoints[i+2].position.y, mControlPoints[i+3].position.y);
					mCurvePoints[index].position.z = Calc_B_Spline(u, u_2, u_3, mControlPoints[i].position.z, mControlPoints[i+1].position.z, mControlPoints[i+2].position.z, mControlPoints[i+3].position.z);
				}
				else 
				{
					mCurvePoints[index].position.x = Calc_Hermite_Spline(u, u_2, u_3, mControlPoints[i].position.x, mControlPoints[i+1].position.x, mControlPoints[i+2].position.x, mControlPoints[i+3].position.x);
					mCurvePoints[index].position.y = Calc_Hermite_Spline(u, u_2, u_3, mControlPoints[i].position.y, mControlPoints[i+1].position.y, mControlPoints[i+2].position.y, mControlPoints[i+3].position.y);
					mCurvePoints[index].position.z = Calc_Hermite_Spline(u, u_2, u_3, mControlPoints[i].position.z, mControlPoints[i+1].position.z, mControlPoints[i+2].position.z, mControlPoints[i+3].position.z);
				}

				if (j == 0 && i == 0) {
					mCurvePoints[index].distance = 0;
					mShortestDistance = 65536.0;
				}
				else 
				{
					distVec = mCurvePoints[index].position - mCurvePoints[index - 1].position;
					mCurvePoints[index].distance = 
						mCurvePoints[index - 1].distance + (F32)mSqrt(distVec.x * distVec.x + distVec.y * distVec.y + distVec.z * distVec.z);
					if (mCurvePoints[index].distance < mShortestDistance)
						mShortestDistance = mCurvePoints[index].distance;
				}
				index++;
			}
		}
	}
	mCurvePoints[index].position = mDesPostion;
	mCurvePoints[index+1].position = mDesPostion;
	mCurvePointCount = index + 2;
	//	mSplineBuilt = false;
}

void SplineBase::SetDestinationPoint(VectorF pos)
{
	mSurPostion = pos;
}

void SplineBase::SetSourcePoint(VectorF pos)
{
	mDesPostion = pos;
}

void SplineBase::SetControlPoint(S32 index, VectorF newPos)
{
	mControlPoints[index].position = newPos;
}

void SplineBase::SetCurvePointCount(S32 controlCntIn, S32 curveSubD)
{
	mControlPointCount = mClamp(controlCntIn, SplineBase::CtrlPointMin, SplineBase::CtrlPointMax);
	mSubdivisionCount = mClamp(curveSubD, SplineBase::SubdivisionMin, SplineBase::SubdivisionMax);
}

VectorF SplineBase::GetCurvePoint(S32 index)
{
	return mCurvePoints[index].position;
}

void SplineBase::initPersistFields()
{
	Parent::initPersistFields();
	addField("CurvePointCount", TypeS32, Offset(mCurvePointCount, SplineBase));
	addField("Subdivision", TypeS32, Offset(mSubdivisionCount, SplineBase));
	addField("SplineType", TypeS32, Offset(mSplineType, SplineBase));
}
/*
ConsoleMethod( SplineBase, Initialize, void, 5, 5, "spline.init();" )
{
	S32 ctrl_pt_count	= dAtoi(argv[2]);
	S32 subdivision		= dAtoi(argv[3]);
	SplineBase::SplineType type;

	switch(dAtoi(argv[4]))
	{
	case 0:
		type = SplineBase::B_Spline;
		break;

	case 1:
		type = SplineBase::Hermite_Spline;
		break;

	default:
		type = SplineBase::B_Spline;
		break;
	};

	SplineBase* spline = static_cast<SplineBase*>(object);
	spline->Initialize(ctrl_pt_count, subdivision, type);
}

ConsoleMethod( SplineBase, SetControlPoint, void, 6, 6, "spline.SetControlPoint(S32 index, F32 x, F32 y, F32 z);" ) {
	Con::printf("CONTROLPOINT: %i - (%f,%f,%f)", dAtoi(argv[2]), dAtof(argv[3]),dAtof(argv[4]),dAtof(argv[5]));
	SplineBase *spline = static_cast<SplineBase *>( object );
	spline->SetControlPoint(dAtoi(argv[2]), VectorF(dAtof(argv[3]),dAtof(argv[4]),dAtof(argv[5]) ));
}

ConsoleMethod( SplineBase, GetCurvePoint, const char *, 3, 3, "spline.GetCurvePoint(S32 index);" ) {

	SplineBase *spline = static_cast<SplineBase *>( object );
	VectorF movePoint = spline->GetCurvePoint(dAtoi(argv[2]));
	char *returnBuffer = Con::getReturnBuffer( 256 );
	dSprintf( returnBuffer, 256, "%f %f %f", movePoint.x, movePoint.y, movePoint.z );
	return returnBuffer;
}

ConsoleMethod( SplineBase, BuildSpline, void, 2, 2, "spline.BuildSpline();" )
{
	SplineBase *spline = static_cast<SplineBase *>( object );
	spline->BuildSpline();
}
*/

// ========================================================================================================================================
//  SplineObject
// ========================================================================================================================================
IMPLEMENT_CONOBJECT(SplineObject);

SplineObject::SplineObject()
{
#ifdef NTJ_CLIENT
	mNetFlags.set(Ghostable | ScopeAlways | IsGhost);
#else
	mNetFlags.clear(Ghostable | ScopeAlways | IsGhost);
#endif
	mTypeMask |= StaticObjectType | StaticRenderedObjectType | ShadowCasterObjectType;
	mSplineTexture = NULL;
	mSplineTextureFile = StringTable->insert("");
	mCurIndex = 0;
	mEscapeTime = 0.0f;
}

SplineObject::~SplineObject()
{
	mSplineTexture = NULL;
	mSplineTextureFile = StringTable->insert("");
	mCurIndex = 0;
}

bool SplineObject::Create(GameObject* src, GameObject* dest, S32 lifeTime, StringTableEntry fname)
{
	if(!src || !dest)
		return false;

	mSource = src;
	mTarget = dest;
	mLifeTime = lifeTime;

	// Get the current time.
	S32 current_time = Platform::getVirtualMilliseconds();

	// Tell it if this is the first time to update the object.
	mLastUpdateTime = current_time;
	mStartTime = current_time;

	if(!fname || !fname[0])
	{
		mSplineTextureFile = StringTable->insert("gameres/data/shapes/effect/effect_line.png");	
		mSplineTexture = GFXTexHandle(mSplineTextureFile, &GFXDefaultRenderTargetZBufferProfile, true);
	}
	else
	{
		mSplineTextureFile = fname;	
		mSplineTexture = GFXTexHandle(mSplineTextureFile, &GFXDefaultRenderTargetZBufferProfile, true);
	}


	SetState(State_Created);

	InitializeSplinePoints();

	return true;
}

void SplineObject::Destroy()
{
}

bool SplineObject::AdvanceTime(SimTime delta)
{
	if(!mTarget || !mSource)
		return false;

	switch(GetState())
	{
	case State_NotCreated:
		break;

	case State_Created:
		{
			SetState(State_Started);
		}
		break;

	case State_Started:
		{
			resetObjBox();
			//计算贝塞尔曲线
			CalculateSplinePoints();

			//mEscapeTime += delta;
			//if (mLifeTime > 0 && mEscapeTime > mLifeTime)
			//	SetState(State_Destroyed);
		}
		break;

	case State_Destroyed:
		break;
	}
	return true;
}

bool SplineObject::Project(const Point3F& pt, Point3F* dest)
{
	/*
	GLdouble winx,winy,winz;
	GLint result = gluProject(pt.x,pt.y,pt.z,
		g_UIMouseGamePlay->m_pfModelviewMatrix,g_UIMouseGamePlay->m_pfProjectionMatrix,g_UIMouseGamePlay->m_pnViewport,
		&winx,&winy,&winz);

	if(result == GL_FALSE || winz <0 || winz >1)
		return false;

	dest->Set(winx,winy,winz);*/
	return true;
}

bool SplineObject::onAdd()
{
	if(!Parent::onAdd())
		return false;

	resetObjBox();

	addToScene();
	return true;
}

void SplineObject::onRemove()
{
	removeFromScene();

	Parent::onRemove();
}

bool SplineObject::prepRenderImage (SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState)
{
#ifdef NTJ_CLIENT
	if(!mVisible)
		return false;

	if(!AdvanceTime(32) || mSplineBase.mCurvePointCount < 2)
		return false;
	if (isLastState(state, stateKey))
		return false;
	setLastState(state, stateKey);
	RenderInst *ri = gRenderInstManager.allocInst();
	ri->obj = this;
	ri->state = state;
	ri->type = RenderInstManager::RIT_Decal;
	//ri->calcSortPoint(this, state->getCameraPosition());
	gRenderInstManager.addInst(ri);


	//GFX->setFillMode((GFXFillMode)GFXFillSolid);
	/*
	if(mSplineBase.mCurvePointCount)
	{
		GFX->setBaseRenderState();
		GFX->setCullMode( GFXCullCCW );
		GFX->setLightingEnable( false );
		GFX->setTextureStageColorOp( 0, GFXTOPDisable );
		GFX->setTextureStageColorOp( 1, GFXTOPDisable );
		//GFX->setTexture( 0, mSplineTexture );
		//GFX->setTexture( 1, NULL );

		ColorI color(255,255,255);
		PrimBuild::begin( GFXLineStrip, mSplineBase.mCurvePointCount );
		for(U32 k = 0; k < mSplineBase.mCurvePointCount; k++)
		{
			PrimBuild::color( color);
			PrimBuild::vertex3fv( mSplineBase.mCurvePoints[k].position);
		}
		PrimBuild::end();
	}
	*/
#endif
	return true;
}

void SplineObject::renderObject( SceneState *state, RenderInst *ri )
{
	static VectorF vec, texturePos,camVec;
	static F32 tp,tpOff;

	camVec.set(0,1,0);
	g_UIMouseGamePlay->mLastCameraQuery.cameraMatrix.mulV(camVec);
	tpOff = mSplineBase.mRefreshTime > 100 ? ((Platform::getVirtualMilliseconds()-mSplineBase.mLastTime)/(F32)mSplineBase.mRefreshTime) : gRandGen.randF();

	GFX->setBaseRenderState();
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "SplineObject::renderObject -- mSetSB cannot be NULL.");
	mSetSB->apply();
	GFX->setTexture( 0, mSplineTexture );
	GFX->setupGenericShaders( GFXDevice::GSModColorTexture );
#else
	GFX->setCullMode( GFXCullNone );
	GFX->setLightingEnable( false );
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcColor );
	GFX->setDestBlend( GFXBlendInvSrcColor);
	GFX->setTextureStageColorOp( 0, GFXTOPModulate2X );
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
	GFX->setTexture( 0, mSplineTexture );
	GFX->setupGenericShaders( GFXDevice::GSModColorTexture );
	GFX->setZWriteEnable(false);
	GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
#endif




	PrimBuild::begin( GFXTriangleStrip, mSplineBase.mCurvePointCount*2 );
	for(U32 k = 0; k < mSplineBase.mCurvePointCount-1; k++)
	{
		vec = mSplineBase.mCurvePoints[k+1].position - mSplineBase.mCurvePoints[k].position;
		vec = mCross(vec, camVec);
		if(vec.isZero())
			continue;
		vec.normalize();
		vec *= 0.5f;

		texturePos = mSplineBase.mCurvePoints[k].position - mSplineBase.mCurvePoints[0].position;
		//tp = fmod(texturePos.len()/10.0f + tpOff, 1.0f);
		tp = texturePos.len()/10.0f + tpOff;
		PrimBuild::texCoord2f(tp, 0.0f);
		PrimBuild::vertex3fv( mSplineBase.mCurvePoints[k].position + vec);
		PrimBuild::texCoord2f(tp, 1.0f);
		PrimBuild::vertex3fv( mSplineBase.mCurvePoints[k].position - vec);
	}
	texturePos = mSplineBase.mCurvePoints[mSplineBase.mCurvePointCount-1].position - mSplineBase.mCurvePoints[0].position;
	tp = texturePos.len()/10.0f + tpOff;
	PrimBuild::texCoord2f(tp, 0.0f);
	PrimBuild::vertex3fv( mSplineBase.mCurvePoints[mSplineBase.mCurvePointCount-1].position + vec);
	PrimBuild::texCoord2f(tp, 1.0f);
	PrimBuild::vertex3fv( mSplineBase.mCurvePoints[mSplineBase.mCurvePointCount-1].position - vec);
	PrimBuild::end();


#ifdef STATEBLOCK
	AssertFatal(mClearSB, "SplineObject::renderObject -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
	GFX->setAlphaBlendEnable( false );
	GFX->setZWriteEnable(true);
	GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
#endif

}

void SplineObject::resetObjBox()
{
	// Calc mWorldBox
	mWorldBox = mSource->getWorldBox();
	mWorldBox.intersect(mTarget->getWorldBox());

	// Create mWorldSphere from mWorldBox
	mWorldBox.getCenter(&mWorldSphere.center);
	mWorldSphere.radius = (mWorldBox.max - mWorldSphere.center).len();

	mObjToWorld.setPosition(mWorldSphere.center);
	mObjBox = mWorldBox;
	mObjBox.min -= mWorldSphere.center;
	mObjBox.max -= mWorldSphere.center;

	setTransform(mObjToWorld);
}

void SplineObject::SetVisible(bool val)
{
	mVisible = val;
}


void SplineObject::InitializeSplinePoints()
{
	mSplineBase.Initialize(CreatePoint_Max,CreatePoint_Max*2,SplineBase::B_Spline);
}

void SplineObject::CalculateSplinePoints()
{
	if (!mSource.isNull() && !mTarget.isNull())
	{
		//1、先将开始点和结束点分为5个点
		VectorF	SurPostion;
		VectorF	DesPostion;

		SurPostion = mSource->getWorldSphere().center;
		DesPostion = mTarget->getWorldSphere().center;

		VectorF	vecSpline = DesPostion -SurPostion;
		F32 distance =  vecSpline.len();
		mSplineBase.SetCurvePointCount((S32)(distance/mSplineBase.mLengthPerNode), 2);
		VectorF normal(0.0f, 0.0f, 1.0f);
		VectorF vecX;
		mCross(vecSpline, normal, &vecX);
		vecX.normalize();

		mSplineBase.SetSourcePoint(DesPostion);
		mSplineBase.SetDestinationPoint(SurPostion);
		mSplineBase.BuildSpline();
	}
	else
	{
		SetState(State_Destroyed);
	}
}



void SplineObject::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcColor);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate2X );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->endStateBlock(mSetSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->endStateBlock(mClearSB);
}


void SplineObject::releaseStateBlock()
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

void SplineObject::init()
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

void SplineObject::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mClearSB);
}


