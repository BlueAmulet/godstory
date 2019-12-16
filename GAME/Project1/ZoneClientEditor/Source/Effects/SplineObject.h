//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "sceneGraph/RenderableSceneObject.h"
#include "math/mPoint.h"
#include "gfx/gfxTextureHandle.h"


#define Calc_Spline(t,cntrl0,cntrl1,cntrl2) ( (1-t)*(1-t)*cntrl0 + 2*(1-t)*t*cntrl1 + t*t*cntrl2 )
#define Calc_B_Spline(u, u_2, u_3, cntrl0, cntrl1, cntrl2, cntrl3) (((-1*u_3 + 3*u_2 - 3*u + 1) * (cntrl0) + ( 3*u_3 - 6*u_2 + 0*u + 4) * (cntrl1) + (-3*u_3 + 3*u_2 + 3*u + 1) * (cntrl2) + ( 1*u_3 + 0*u_2 + 0*u + 0) * (cntrl3)) / 6) 
#define Calc_Hermite_Spline(u, u_2, u_3, cntrl0, cntrl1, cntrl2, cntrl3) (((-1*u_3 + 2*u_2 - 1*u + 0) * (cntrl0) + ( 3*u_3 - 5*u_2 + 0*u + 2) * (cntrl1) + (-3*u_3 + 4*u_2 + 1*u + 0) * (cntrl2) + ( 1*u_3 - 1*u_2 + 0*u + 0) * (cntrl3)) / 2) 

// ========================================================================================================================================
//  SplineBase
// ========================================================================================================================================
//	曲线对象.
//
//	关于B-Spline和Hermite Spline
//		B-Spline是贝塞尔曲线(Bezier Curve)的一个延伸, 一个独立的贝塞尔曲线仅具有四个控制点, 因此在更高细分需求的时候不太适合, 而B-Spline则可以延续控制点.
//		B-Spline以及贝塞尔曲线的特点是: 曲线本身不一定经过控制点, 控制点仅用于细分需要.
//		Hermite曲线的特点是: 在具有3个或3个以上控制点时形成曲线, 曲线经过n个控制点的前 n-1 个点, 最后一个控制点控制前面曲线的趋势.
//
//	如何使用该类
//		初始化: 使用Initialize()进行初始化. 注意: 当前控制点数据没有使用动态数组, 因此通常是固定的, 控制点数量需要预先确定.
//		增加控制点数据: spline->mpControlPoints[n].position = VectorF(x, y, z). 控制点的数量在初始化的时候需要确定, 但是控制点的数据可以以后更换.
//		构建曲线: 在 设定/重设 控制点之后, 如需更新曲线, 则使用BuildSpline()
//		
//	Paul Bourke教授提供的算法及实现
//
class SplineBase : public SimObject
{
	typedef SimObject Parent;

public:
	struct ControlPoint
	{
		Point3F			position;
	};

	struct CurveData
	{
		Point3F			position;
		F32				distance;
	};

	enum
	{
		// 修改以下两项需要修改相关函数
		CtrlPointBits	= 4,
		CtrlPointMax	= BIT(CtrlPointBits),
		CtrlPointMin	= 5,
		SubdivisionMax	= 5,
		SubdivisionMin	= 1,
	};

	enum SplineType
	{
		Spline			= 0,
		B_Spline		= 1,						// B-Spline, B曲线, 提供超过4个控制点的贝塞尔曲线分割算法
		Hermite_Spline	= 2,						// 厄米曲线,需要提供三个或三个以上的曲线
	};

	SplineType			mSplineType;				// 曲线类型
	F32					mShortestDistance;			// 最短距离

	bool				mSplineBuilt;				// 曲线是否已被构建
	F32					mLengthPerNode;				// 每个结点的长度
	SimTime				mRefreshTime;				// 刷新控制点的时间间隔
	SimTime				mLastTime;					// 上次刷新控制点的时间


	// ...::: 控制点数据 :::...

	S32					mControlPointCount;			// 控制点的数量
	ControlPoint*		mControlPoints;				// 曲线的控制点
	S32					mCtrlBegin[CtrlPointMax];	// 曲线的控制点，起始点ID
	S32					mCtrlEnd[CtrlPointMax];		// 曲线的控制点，结束点ID

	Point3F				mSurPostion;				// 源点
	Point3F				mDesPostion;				// 目标点
	S32					mCurrentControlPoint;		// 当前控制点的index
	VectorF				mDefaultNormal;				// 默认的法线矢量

	// ...::: 曲线点数据 :::...

	// 曲线点指的是最终形成的曲线上面的点. 曲线点的数量 = 控制点数量 * 细分. 因此细分越大, 表现出来的曲线越光滑.

	CurveData*			mCurvePoints;				// 曲线点
	S32					mSubdivisionCount;			// 每两个控制点之间的细分, 数值越大, 曲线越光滑
	S32					mCurvePointCount;			// 最终曲线上点的数量
	S32					mCurrentCurvePoint;			// 当前曲线点的index


public:
	SplineBase();
	~SplineBase();

	DECLARE_CONOBJECT(SplineBase);
	static void			initPersistFields();

	S32					Initialize					(S32 controlCntIn, S32 curveSubD, SplineType curveType);
	void				RandCtrlPoints				();			// 随机设定控制点
	void				BuildSpline					();			// 构建曲线

	void				SetControlPoint				(S32 index, Point3F newPos);
	VectorF				GetCurvePoint				(S32 index);
	void				SetSourcePoint				(Point3F Pos);
	void				SetDestinationPoint			(Point3F pos);
	VectorF				GetSourcePoint				() { return mSurPostion;}
	VectorF				GetDestinationPoint			() { return mDesPostion;}

	void				SetCurvePointCount			(S32 controlCntIn, S32 curveSubD);
	S32					GetCurvePointCount			() {return mCurvePointCount;}
};


// ========================================================================================================================================
//  SplineObject
// ========================================================================================================================================

class GameObject;
class GFXStateBlock;

class SplineObject : public RenderableSceneObject
{
	typedef RenderableSceneObject Parent;
public:
	enum Constants
	{
		CreatePoint_Min		= 5,
		CreatePoint_Max		= 15,
	};

	enum State
	{
		State_NotCreated,
		State_Created,
		State_Started,
		State_Destroyed,
	};

	SplineObject			();
	~SplineObject			();
	DECLARE_CONOBJECT		(SplineObject);

	bool					Create						(GameObject* src, GameObject* dest, S32 lifeTime, StringTableEntry fname = NULL);
	void					Destroy						();

	State					GetState					() const { return mState; }
	void					SetState					(State state) { mState = state; }

	bool					AdvanceTime					(SimTime delta);


	void					InitializeSplinePoints		();
	void					CalculateSplinePoints		();
	bool					Project						(const Point3F& pt, Point3F* dest);	//// 将对象坐标系中的坐标转换成窗口坐标系中的坐标
	void					SetVisible					(bool val);

protected:

	bool					onAdd						();
	void					onRemove					();
	bool					prepRenderImage				( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
	void					renderObject				(  SceneState *state, RenderInst *ri );
	void					resetObjBox					();

	SimObjectPtr<GameObject>mSource;
	SimObjectPtr<GameObject>mTarget;

private:	
	bool					mVisible;
	F32						mStartTime;
	F32						mLastUpdateTime;
	State					mState;
	SplineBase				mSplineBase;
	S32						mCurIndex;

	F32						mLifeTime;
	F32						mEscapeTime;

	GFXTexHandle			mSplineTexture;				// Texture of the motion trail, would be used in every triangle strip.
	StringTableEntry		mSplineTextureFile;			// File name of the motion trail texture.

	//状态块
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mClearSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

