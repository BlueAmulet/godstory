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
//	���߶���.
//
//	����B-Spline��Hermite Spline
//		B-Spline�Ǳ���������(Bezier Curve)��һ������, һ�������ı��������߽������ĸ����Ƶ�, ����ڸ���ϸ�������ʱ��̫�ʺ�, ��B-Spline������������Ƶ�.
//		B-Spline�Լ����������ߵ��ص���: ���߱���һ���������Ƶ�, ���Ƶ������ϸ����Ҫ.
//		Hermite���ߵ��ص���: �ھ���3����3�����Ͽ��Ƶ�ʱ�γ�����, ���߾���n�����Ƶ��ǰ n-1 ����, ���һ�����Ƶ����ǰ�����ߵ�����.
//
//	���ʹ�ø���
//		��ʼ��: ʹ��Initialize()���г�ʼ��. ע��: ��ǰ���Ƶ�����û��ʹ�ö�̬����, ���ͨ���ǹ̶���, ���Ƶ�������ҪԤ��ȷ��.
//		���ӿ��Ƶ�����: spline->mpControlPoints[n].position = VectorF(x, y, z). ���Ƶ�������ڳ�ʼ����ʱ����Ҫȷ��, ���ǿ��Ƶ�����ݿ����Ժ����.
//		��������: �� �趨/���� ���Ƶ�֮��, �����������, ��ʹ��BuildSpline()
//		
//	Paul Bourke�����ṩ���㷨��ʵ��
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
		// �޸�����������Ҫ�޸���غ���
		CtrlPointBits	= 4,
		CtrlPointMax	= BIT(CtrlPointBits),
		CtrlPointMin	= 5,
		SubdivisionMax	= 5,
		SubdivisionMin	= 1,
	};

	enum SplineType
	{
		Spline			= 0,
		B_Spline		= 1,						// B-Spline, B����, �ṩ����4�����Ƶ�ı��������߷ָ��㷨
		Hermite_Spline	= 2,						// ��������,��Ҫ�ṩ�������������ϵ�����
	};

	SplineType			mSplineType;				// ��������
	F32					mShortestDistance;			// ��̾���

	bool				mSplineBuilt;				// �����Ƿ��ѱ�����
	F32					mLengthPerNode;				// ÿ�����ĳ���
	SimTime				mRefreshTime;				// ˢ�¿��Ƶ��ʱ����
	SimTime				mLastTime;					// �ϴ�ˢ�¿��Ƶ��ʱ��


	// ...::: ���Ƶ����� :::...

	S32					mControlPointCount;			// ���Ƶ������
	ControlPoint*		mControlPoints;				// ���ߵĿ��Ƶ�
	S32					mCtrlBegin[CtrlPointMax];	// ���ߵĿ��Ƶ㣬��ʼ��ID
	S32					mCtrlEnd[CtrlPointMax];		// ���ߵĿ��Ƶ㣬������ID

	Point3F				mSurPostion;				// Դ��
	Point3F				mDesPostion;				// Ŀ���
	S32					mCurrentControlPoint;		// ��ǰ���Ƶ��index
	VectorF				mDefaultNormal;				// Ĭ�ϵķ���ʸ��

	// ...::: ���ߵ����� :::...

	// ���ߵ�ָ���������γɵ���������ĵ�. ���ߵ������ = ���Ƶ����� * ϸ��. ���ϸ��Խ��, ���ֳ���������Խ�⻬.

	CurveData*			mCurvePoints;				// ���ߵ�
	S32					mSubdivisionCount;			// ÿ�������Ƶ�֮���ϸ��, ��ֵԽ��, ����Խ�⻬
	S32					mCurvePointCount;			// ���������ϵ������
	S32					mCurrentCurvePoint;			// ��ǰ���ߵ��index


public:
	SplineBase();
	~SplineBase();

	DECLARE_CONOBJECT(SplineBase);
	static void			initPersistFields();

	S32					Initialize					(S32 controlCntIn, S32 curveSubD, SplineType curveType);
	void				RandCtrlPoints				();			// ����趨���Ƶ�
	void				BuildSpline					();			// ��������

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
	bool					Project						(const Point3F& pt, Point3F* dest);	//// ����������ϵ�е�����ת���ɴ�������ϵ�е�����
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

	//״̬��
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mClearSB;
public:
	static void init();
	static void shutdown();
	//
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();
};

